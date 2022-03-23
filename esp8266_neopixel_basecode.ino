/**
 * Smart LED Lamp - basecode
 * 
 * created by techniccontroller 21.03.2022
 * 
 * components:
 * - ESP8266 (ESP-01)
 * - Neopixelstrip
 *  
 * 
 * with code parts from:
 * - Adafruit NeoPixel strandtest.ino, https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/strandtest/strandtest.ino
 * - Esp8266 filemanager / Webserver https://fipsok.de/
 */

#include "secrets.h"
#include <Adafruit_NeoPixel.h>          // NeoPixel library used to run the NeoPixel LEDs
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <WiFiManager.h>                //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include "udplogger.h"

#define NEOPIXELPIN 3           // pin to which the NeoPixels are attached
#define NUMPIXELS 16            // number of pixels attached
#define PERIOD_HEARTBEAT 1000   // period of sending a heartbeat message

// credentials for Access Point
#define AP_SSID "SmartLEDAP"
#define AP_PASS "123123123"

ESP8266WebServer server(80);  // serve webserver on port 80

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);
long timeOfLastHeartbeat = millis();

// Variables for Logger
IPAddress logMulticastIP = IPAddress(230, 120, 10, 2);
int logMulticastPort = 8123;
UDPLogger logger;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.printf("\nSketchname: %s\nBuild: %s\n", (__FILE__), (__TIMESTAMP__));
  Serial.println();

  // setup LED functions
  setupLED();

  delay(250);
  
  // Turn on all LEDs blue
  setColorAll(pixels.Color(0, 0, 50));
  delay(1000);

  /** Use WiFiMaanger for handling initial Wifi setup **/

  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  // Uncomment and run it once, if you want to erase all the stored information
  //wifiManager.resetSettings();

  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name (AP_SSID)
  // and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect(AP_SSID);


  /** (alternative) Use directly STA/AP Mode of ESP8266   **/

  /*
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  
  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  //wifi_station_set_hostname("esplamp");

  int timeoutcounter = 0;
  while (WiFi.status() != WL_CONNECTED && timeoutcounter < 30) {
    delay(1000);
    Serial.print(".");
    timeoutcounter++;
  }

  // start request of program
  if (WiFi.status() == WL_CONNECTED) {      //Check WiFi connection status
    Serial.println("");

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP()); 
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
  
  } else {
    // no wifi found -> open access point
    // ip addresses for Access Point
    IPAddress IPAdress_AccessPoint(192,168,10,2);
    IPAddress Gateway_AccessPoint(192,168,10,0);
    IPAddress Subnetmask_AccessPoint(255,255,255,0);
    
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(IPAdress_AccessPoint, Gateway_AccessPoint, Subnetmask_AccessPoint);
    WiFi.softAP(AP_SSID, AP_PASS);

    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
  }*/


  // if you get here you have connected to the WiFi
  Serial.println("Connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); 

  // Turn off all LEDs
  setColorAll(pixels.Color(0, 0, 0));

  // init File manager
  spiffs(); // SPIFFS
  server.begin();

  // setup OTA
  setupOTA();

  // add handler for processing command to /cmd endpoint
  server.on("/cmd", handleCommand); 

  // setup UDPLogger
  logger = UDPLogger(WiFi.localIP(), logMulticastIP, logMulticastPort);
  logger.setName(WiFi.localIP().toString());
  logger.logString("Start program\n");
}

void loop() {
  // handle OTA
  handleOTA();
  
  // handle Webserver
  server.handleClient();

  if(millis() - timeOfLastHeartbeat > 1000){
    logger.logString("Heartbeat\n");
    timeOfLastHeartbeat = millis();
  }

  delay(10);
}

/**
 * @brief Handler for handling commands sent to "/cmd" url
 * 
 */
void handleCommand() {
  // receive command and handle accordingly
  for (uint8_t i = 0; i < server.args(); i++) {
    Serial.print(server.argName(i));
    Serial.print(F(": "));
    Serial.println(server.arg(i));
  }
  
  if (server.argName(0) == "led") // the parameter which was sent to this server
  {
    String colorstr = server.arg(0) + "-";
    String redstr = split(colorstr, '-', 0);
    String greenstr= split(colorstr, '-', 1);
    String bluestr = split(colorstr, '-', 2);
    logger.logString(colorstr);
    logger.logString("r: " + String(redstr.toInt()));
    logger.logString("g: " + String(greenstr.toInt()));
    logger.logString("b: " + String(bluestr.toInt()));
    setColorAll(pixels.Color(redstr.toInt(), greenstr.toInt(), bluestr.toInt()));
  }
  server.send(204, "text/plain", "No Content"); // this page doesn't send back content --> 204
}

/**
 * @brief Splits a string at given character and return specified element
 * 
 * @param s string to split
 * @param parser separating character
 * @param index index of the element to return
 * @return String 
 */
String split(String s, char parser, int index) {
  String rs="";
  int parserIndex = index;
  int parserCnt=0;
  int rFromIndex=0, rToIndex=-1;
  while (index >= parserCnt) {
    rFromIndex = rToIndex+1;
    rToIndex = s.indexOf(parser,rFromIndex);
    if (index == parserCnt) {
      if (rToIndex == 0 || rToIndex == -1) return "";
      return s.substring(rFromIndex,rToIndex);
    } else parserCnt++;
  }
  return rs;
}

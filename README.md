# Basecode for a Project with ESP8266 and NeoPixel LEDs

Website: https://www.techniccontroller.de/smart-led-lamp-my-basic-setup-with-esp8266


![IMG_20211127_231618_edit](https://user-images.githubusercontent.com/36072504/159768935-70f78c46-c1f2-4afe-9fb2-c67baedb6788.jpg)


## Used Libraries

- WiFiManager: https://github.com/tzapu/WiFiManager
- Adafruit_NeoPixel: https://github.com/adafruit/Adafruit_NeoPixel
- ArduinoOTA: https://github.com/jandrassy/ArduinoOTA


## How to see Logging messages

I wrote an small python script [multicastUDP_receiver.py](https://github.com/techniccontroller/esp8266_neopixel_basecode/blob/master/multicastUDP_receiver.py) to display the logging messages, which are sent from my own UDPLogger object.
The logging messages are UDP multicast messages, so the python script will listen for those message on the specific address. 

Make sure to adjust this line according to your setup:

```python
# ip address of network interface
MCAST_IF_IP = '192.168.0.4'
```

Run the script with following command:

```
python .\multicastUDP_receiver.py
```

## References

- Adafruit NeoPixel strandtest.ino, https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/strandtest/strandtest.ino
- Esp8266 filemanager / Webserver https://fipsok.de/

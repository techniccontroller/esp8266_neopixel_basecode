/**
 * @file ledfunctions.ino
 * @author techniccontroller (mail[at]techniccontroller.com)
 * @brief file contains all functions relevant for LEDs
 * @version 0.1
 * @date 2022-03-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/**
 * @brief Input a value 0 to 255 to get a color value. The colors are a transition r - g - b - back to r.
 * 
 * @param WheelPos Value between 0 and 255
 * @return uint32_t color of colorwheel
 */
uint32_t Wheel(byte WheelPos)
{
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85)
    {
        return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if (WheelPos < 170)
    {
        WheelPos -= 85;
        return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


/**
 * @brief Set all pixel to given color and update neopixel
 * 
 * @param color 
 */
void setColorAll(uint32_t color)
{
    for (int i = 0; i < NUMPIXELS; i++)
    {
        pixels.setPixelColor(i, color); // set color of pixel i
    }
    pixels.show(); // This sends the updated pixel color to the hardware.
}

/**
 * @brief Interpolates two colors and returns an color of the result
 * 
 * @param color1 startcolor for interpolation
 * @param color2 endcolor for interpolation
 * @param factor which color is wanted on the path from start to end color
 * @return uint32_t interpolated color
 */
uint32_t interpolateColor(uint32_t color1, uint32_t color2, float factor)
{
    uint8_t resultRed = color1 >> 16 & 0xff;
    uint8_t resultGreen = color1 >> 8 & 0xff;
    uint8_t resultBlue = color1 & 0xff;
    resultRed = (int16_t)(resultRed + (int16_t)(factor * ((int16_t)(color2 >> 16 & 0xff) - (int16_t)resultRed)));
    resultGreen = (int16_t)(resultGreen + (int16_t)(factor * ((int16_t)(color2 >> 8 & 0xff) - (int16_t)resultGreen)));
    resultBlue = (int16_t)(resultBlue + (int16_t)(factor * ((int16_t)(color2 & 0xff) - (int16_t)resultBlue)));
    return pixels.Color(resultRed, resultGreen, resultBlue);
}

/**
 * @brief Setup LED functions
 * 
 */
void setupLED()
{
    pixels.begin();            // This initializes the NeoPixel library.
    pixels.setBrightness(255); // Set brightness of LEDs to 100%
}


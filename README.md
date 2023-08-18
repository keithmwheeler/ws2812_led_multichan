# WS2812 LED multiple strip library for use with PSoC® 6

This is a driver library to interface WS2812 LED strips using a PSOC 6. It allows use of multiple independent strip channels. Each channel uses an SPI port for interface to the LED strip.

## Quick Start

To use the library:

1. Include the header file *ws2812.h*.

2. Declare an instance of a ws2812 channel

        ws2812_obj_t ws2812_obj

3. Call *ws2812_init* to initialize the library. The user specifies the the number of LEDs (pixels) and the SPI pins (MOSI, SCLK) and the channel to use. Note that while this driver uses an SPI peripheral only the MOSI pin is required. The SCLK is provided as an optional debug output; the underlying call to the setup the SPI sets MISO to NC (No Connect). You may set the SCLK to NC. The underlying setup code automatically handles getting the SPI clock set for correct speed for the WS2812 LEDs. This is required per channel.

        ws2812_rtn_t 	ws2812_init(ws2812_obj_t *obj, uint16_t numPixels, cyhal_gpio_t mosi, cyhal_gpio_t sclk);

4. Call *ws2812_setRGB* to set the color of one LED or call *ws2812_setMultiRGB* to set a range of LEDs to the same color for a specific strip channel.

        ws2812_rtn_t 	ws2812_setRGB(ws2812_obj_t *obj, uint16_t led, uint8_t red, uint8_t green, uint8_t blue);

        ws2812_rtn_t 	ws2812_setMultiRGB(ws2812_obj_t *obj, uint16_t startLED, uint16_t endLED, uint8_t red, uint8_t green, uint8_t blue);

5. Call *ws2812_update* to send a new set of colors to the desired specific LED strip.

        ws2812_rtn_t ws2812_update(ws2812_obj_t *obj);

6. Note the macro MAX_PIXELS_PER_CHANNEL in ws2812.h. Adjust this as needed, it's used to set the per channel buffer size so having it larger than your strip size results in wasting memory.

**Example Code**

The folowing code will initialize the library for two strips of 10 LEDs and will then set the strips to various colors.

    #include "ws2812.h"

    ws2812_obj_t ws2812_channel1;
    ws2812_obj_t ws2812_channel2;

    ws2812_init(&ws2812_channel1, 10, CYBSP_SPI_MOSI, CYBSP_SPI_CLK);  //Initialize channel 1 with clock output
    ws2812_init(&ws2812_channel2, 10, P10_0, NC);  //Initialize channel 2 without clock output

    while(1)
    {
        ws2812_setMultiRGB(&ws2812_channel1, 0, 9, 255, 0, 0); // Set all channel 1 LEDs to RED at max brighness
        ws2812_update(&ws2812_channel1);
        Cy_SysLib_Delay(100);
        ws2812_setMultiRGB(&ws2812_channel1, 0, 9, 0, 255, 0); // Set all channel 1 LEDs to GREEN at max brighness
        ws2812_update(&ws2812_channel1);
        Cy_SysLib_Delay(100);
        ws2812_setMultiRGB(&ws2812_channel1, 0, 9, 0, 0, 255); // Set all channel 1 LEDs to BLUE at max brighness
        ws2812_update(&ws2812_channel1);
        Cy_SysLib_Delay(100);
        ws2812_setMultiRGB(&ws2812_channel2, 0, 9, 255, 0, 255); // Set all channel 2 LEDs to PURPLE at max brighness
        ws2812_update(&ws2812_channel2);
        Cy_SysLib_Delay(100);
        ws2812_setMultiRGB(&ws2812_channel2, 0, 9, 0, 0, 0); // Set all channel 2 LEDs off
        ws2812_update(&ws2812_channel2);
        Cy_SysLib_Delay(100);
    }

/*
 * ws2812.c
 */

#include "cyhal.h"
#include "ws2812.h"

// static uint8_t WS_frameBuffer[(MAX_PIXELS_PER_CHANNEL*WS_BYTES_PER_PIXEL)+WS_ZOFFSET];
// static uint16_t numberOfPixels;

// static cyhal_spi_t 	 ws2182SpiHandle;

/* This function takes an 8-bit value representing a color
 * and turns it into a WS2812 bit code... where 1=110 and 0=011
 * 1 input byte turns into three output bytes of a uint32_t
 * */
static uint32_t WS_convert3Code(uint8_t input)
{
    uint32_t rval=0;
    for(int i=0;i<8;i++)
    {
        if(input%2)
        {
            rval |= WS_ONE3;
        }
        else
        {
            rval |= WS_ZERO3;
        }
        rval = rval >> 3;

        input = input >> 1;
    }
    return rval;
}

/* Initialize an LED driver channel */
ws2812_rtn_t ws2812_init(ws2812_obj_t *obj, uint16_t numPixels, cyhal_gpio_t mosi, cyhal_gpio_t sclk)
{
	cy_rslt_t result;
	ws2812_rtn_t ws_result;

	if(numPixels > MAX_PIXELS_PER_CHANNEL)
	{
		return (ws2182_too_many_pixels);
	}

	/* Save number of pixels provided by the user to a global */
	obj->numberOfPixels = numPixels;

	/* Initialize the SPI block that will be used to drive data to the LEDs */
	result = cyhal_spi_init(&obj->ws2182SpiHandle, mosi, NC, sclk, NC, NULL, 8, CYHAL_SPI_MODE_11_MSB, false);
	if(result != CY_RSLT_SUCCESS)
	{
		return (ws2812_error);
	}

	result = cyhal_spi_set_frequency(&obj->ws2182SpiHandle, 2200000);
	if(result != CY_RSLT_SUCCESS)
	{
		return (ws2812_error);
	}

	/* Initialize the array to all off RGB values and turn off the LEDs */
    obj->WS_frameBuffer[0] = 0x00;
    ws2812_setMultiRGB(obj, 0, obj->numberOfPixels-1, 0, 0, 0);
    ws_result = ws2812_update(obj);

	return(ws_result);
}

/* Set the RGB values for a single LED */
/* This does NOT send new values to the LEDs - you must call ws2812_update to send the values */
ws2812_rtn_t ws2812_setRGB(ws2812_obj_t *obj, uint16_t led, uint8_t red, uint8_t green, uint8_t blue)
{
	if( led > (obj->numberOfPixels-1) )
	{
    	return (ws2812_invalid_LED_id);
	}

	typedef union {
    uint8_t bytes[4];
    uint32_t word;
    } WS_colorUnion;

    WS_colorUnion color;
    color.word = WS_convert3Code(green);
    obj->WS_frameBuffer[(led*WS_BYTES_PER_PIXEL)+0+WS_ZOFFSET] = color.bytes[2];
    obj->WS_frameBuffer[(led*WS_BYTES_PER_PIXEL)+1+WS_ZOFFSET] = color.bytes[1];
    obj->WS_frameBuffer[(led*WS_BYTES_PER_PIXEL)+2+WS_ZOFFSET] = color.bytes[0];

    color.word = WS_convert3Code(red);
    obj->WS_frameBuffer[(led*WS_BYTES_PER_PIXEL)+3+WS_ZOFFSET] = color.bytes[2];
    obj->WS_frameBuffer[(led*WS_BYTES_PER_PIXEL)+4+WS_ZOFFSET] = color.bytes[1];
    obj->WS_frameBuffer[(led*WS_BYTES_PER_PIXEL)+5+WS_ZOFFSET] = color.bytes[0];

    color.word = WS_convert3Code(blue);
    obj->WS_frameBuffer[(led*WS_BYTES_PER_PIXEL)+6+WS_ZOFFSET] = color.bytes[2];
    obj->WS_frameBuffer[(led*WS_BYTES_PER_PIXEL)+7+WS_ZOFFSET] = color.bytes[1];
    obj->WS_frameBuffer[(led*WS_BYTES_PER_PIXEL)+8+WS_ZOFFSET] = color.bytes[0];

    return (ws2812_success);
}

/* Set the RGB values for a range of LEDs */
/* This does NOT send new values to the LEDs - you must call ws2812_update to send the values */
ws2812_rtn_t ws2812_setMultiRGB(ws2812_obj_t *obj, uint16_t startLED, uint16_t endLED, uint8_t red, uint8_t green, uint8_t blue)
{
	ws2812_rtn_t ws_result;

	if( (startLED > endLED) || (endLED > (obj->numberOfPixels-1) ) )
	{
    	return (ws2812_invalid_LED_id);
	}

    /* Set RGB values for the first LED and then copy to the rest of them */
	ws_result = ws2812_setRGB(obj, startLED, red, green, blue);
    for(int i=1; i<=endLED-startLED; i++)
    {
        memcpy(&obj->WS_frameBuffer[(startLED*WS_BYTES_PER_PIXEL)+(i*WS_BYTES_PER_PIXEL)+WS_ZOFFSET],
        &obj->WS_frameBuffer[(startLED*WS_BYTES_PER_PIXEL)+WS_ZOFFSET],WS_BYTES_PER_PIXEL);
    }

    return (ws_result);
}

/* Send the latest frame buffer to the LEDs */
ws2812_rtn_t ws2812_update(ws2812_obj_t *obj)
{
	cy_rslt_t result;

	result = cyhal_spi_transfer(&obj->ws2182SpiHandle, obj->WS_frameBuffer, (obj->numberOfPixels*WS_BYTES_PER_PIXEL)+WS_ZOFFSET, NULL, 0 , 0x00);
	if(result != CY_RSLT_SUCCESS)
	{
		return (ws2812_error);
	}

	return(ws2812_success);
}

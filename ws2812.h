/*
 * ws2812.h
 */

#ifndef WS2812_H_
#define WS2812_H_

/* Max number of pixels per channel supported by the library */
#define MAX_PIXELS_PER_CHANNEL (288)

#define WS_ZOFFSET (1)
#define WS_ONE3  (0b110<<24)
#define WS_ZERO3 (0b100<<24)
#define WS_SPI_BIT_PER_BIT (3)
#define WS_COLOR_PER_PIXEL (3)
#define WS_BYTES_PER_PIXEL (WS_SPI_BIT_PER_BIT * WS_COLOR_PER_PIXEL)

typedef struct {
    uint16_t		numberOfPixels;
	cyhal_gpio_t 	mosi;
	uint8_t			WS_frameBuffer[(MAX_PIXELS_PER_CHANNEL*WS_BYTES_PER_PIXEL)+WS_ZOFFSET];
	cyhal_spi_t 	ws2182SpiHandle;
} ws2812_obj_t;

typedef enum {
	ws2812_success,
	ws2812_error,
	ws2812_invalid_LED_id,
	ws2182_too_many_pixels,
}ws2812_rtn_t;

ws2812_rtn_t 	ws2812_init(ws2812_obj_t *obj, uint16_t numPixels, cyhal_gpio_t mosi, cyhal_gpio_t sclk);
ws2812_rtn_t 	ws2812_setRGB(ws2812_obj_t *obj, uint16_t led, uint8_t red, uint8_t green, uint8_t blue);
ws2812_rtn_t 	ws2812_setMultiRGB(ws2812_obj_t *obj, uint16_t startLED, uint16_t endLED, uint8_t red, uint8_t green, uint8_t blue);
ws2812_rtn_t	ws2812_update(ws2812_obj_t *obj);

#endif /* WS2812_H_ */

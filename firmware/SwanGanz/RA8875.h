/**************************************************************************/
/*!
    @file     Adafruit_RA8875.h
    @author   Limor Friend/Ladyada, K.Townsend/KTOWN for Adafruit Industries

     This is the library for the Adafruit RA8875 Driver board for TFT displays
     ---------------> http://www.adafruit.com/products/1590
     The RA8875 is a TFT driver for up to 800x480 dotclock'd displays
     It is tested to work with displays in the Adafruit shop. Other displays
     may need timing adjustments and are not guanteed to work.

     Adafruit invests time and resources providing this open
     source code, please support Adafruit and open-source hardware
     by purchasing products from Adafruit!

     Written by Limor Fried/Ladyada for Adafruit Industries.
     BSD license, check license.txt for more information.
     All text above must be included in any redistribution.
*/
/**************************************************************************/

#ifndef _ADAFRUIT_RA8875_H
#define _ADAFRUIT_RA8875_H 

#define TFT_CS_LOW()     (GPIOB->DOUTCLR31_0 = (1<<6))   // PB6 low
#define TFT_CS_HIGH()    (GPIOB->DOUTSET31_0 = (1<<6))   // PB6 high
#define TFT_RST_LOW()    (GPIOB->DOUTCLR31_0 = (1))


/**************************************************************************/
/*!
 @enum RA8875sizes The Supported Screen Sizes
 */
/**************************************************************************/
enum RA8875sizes {
  RA8875_480x80,  /*!<  480x80 Pixel Display */
  RA8875_480x128, /*!< 480x128 Pixel Display */
  RA8875_480x272, /*!< 480x272 Pixel Display */
  RA8875_800x480  /*!< 800x480 Pixel Display */
};

int RA8875_begin(enum RA8875sizes s);
void RA8875_softReset(void);
void RA8875_displayOn(int on);
void RA8875_sleep(int sleep);

/* Text functions */
void RA8875_textMode(void);
void RA8875_textSetCursor(uint16_t x, uint16_t y);
void RA8875_textColor(uint16_t foreColor, uint16_t bgColor);
void RA8875_textTransparent(uint16_t foreColor);
void RA8875_textEnlarge(uint8_t scale);
void RA8875_textWrite(const char* buffer, uint16_t len);
void RA8875_cursorBlink(uint8_t rate);

/* Graphics functions */
void RA8875_graphicsMode(void);
void RA8875_setXY(uint16_t x, uint16_t y);
void RA8875_pushPixels(uint32_t num, uint16_t p);
void RA8875_fillScreenWithCurrentColor(void);

/* Adafruit_GFX functions */
void RA8875_drawPixel(int16_t x, int16_t y, uint16_t color);
void RA8875_drawPixels(uint16_t* p, uint32_t count, int16_t x, int16_t y);
void RA8875_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void RA8875_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

/* HW accelerated wrapper functions (override Adafruit_GFX prototypes) */
void RA8875_fillScreen(uint16_t color);
void RA8875_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void RA8875_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void RA8875_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void RA8875_drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color);
void RA8875_fillCircle(int16_t x, int16_t y, int16_t r, uint16_t color);
void RA8875_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2,
                int16_t y2, uint16_t color);
void RA8875_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2,
                int16_t y2, uint16_t color);
void RA8875_drawEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis,
                int16_t shortAxis, uint16_t color);
void RA8875_fillEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis,
                int16_t shortAxis, uint16_t color);
void RA8875_drawCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis,
                int16_t shortAxis, uint8_t curvePart, uint16_t color);
void RA8875_fillCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis,
                int16_t shortAxis, uint8_t curvePart, uint16_t color);
void RA8875_drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r,
                    uint16_t color);
void RA8875_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r,
                    uint16_t color);

/* Scroll */
void RA8875_setScrollWindow(int16_t x, int16_t y, int16_t w, int16_t h,
                    uint8_t mode);
void RA8875_scrollX(int16_t dist);
void RA8875_scrollY(int16_t dist);

/* Backlight */
void RA8875_setGPIOX(int on);
void RA8875_PWM1config(int on, uint8_t clock);
void RA8875_PWM2config(int on, uint8_t clock);
void RA8875_PWM1out(uint8_t p);
void RA8875_PWM2out(uint8_t p);

/* Touch screen */
void RA8875_touchEnable(int on);
int RA8875_touched(void);
int RA8875_touchRead(uint16_t* x, uint16_t* y);


// Colors (RGB565)
#define RA8875_BLACK 0x0000   ///< Black Color
#define RA8875_BLUE 0x001F    ///< Blue Color
#define RA8875_RED 0xF800     ///< Red Color
#define RA8875_GREEN 0x07E0   ///< Green Color
#define RA8875_CYAN 0x07FF    ///< Cyan Color
#define RA8875_MAGENTA 0xF81F ///< Magenta Color
#define RA8875_YELLOW 0xFFE0  ///< Yellow Color
#define RA8875_WHITE 0xFFFF   ///< White Color


#endif
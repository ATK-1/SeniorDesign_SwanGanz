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
void softReset(void);
void displayOn(int on);
void sleep(int sleep);

/* Text functions */
void textMode(void);
void textSetCursor(uint16_t x, uint16_t y);
void textColor(uint16_t foreColor, uint16_t bgColor);
void textTransparent(uint16_t foreColor);
void textEnlarge(uint8_t scale);
void textWrite(const char* buffer, uint16_t len);
void cursorBlink(uint8_t rate);

/* Graphics functions */
void graphicsMode(void);
void setXY(uint16_t x, uint16_t y);
void pushPixels(uint32_t num, uint16_t p);
void fillScreenWithCurrentColor(void);

/* Adafruit_GFX functions */
void drawPixel(int16_t x, int16_t y, uint16_t color);
void drawPixels(uint16_t* p, uint32_t count, int16_t x, int16_t y);
void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

/* HW accelerated wrapper functions (override Adafruit_GFX prototypes) */
void fillScreen(uint16_t color);
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color);
void fillCircle(int16_t x, int16_t y, int16_t r, uint16_t color);
void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2,
                int16_t y2, uint16_t color);
void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2,
                int16_t y2, uint16_t color);
void drawEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis,
                int16_t shortAxis, uint16_t color);
void fillEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis,
                int16_t shortAxis, uint16_t color);
void drawCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis,
                int16_t shortAxis, uint8_t curvePart, uint16_t color);
void fillCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis,
                int16_t shortAxis, uint8_t curvePart, uint16_t color);
void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r,
                    uint16_t color);
void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r,
                    uint16_t color);

/* Scroll */
void setScrollWindow(int16_t x, int16_t y, int16_t w, int16_t h,
                    uint8_t mode);
void scrollX(int16_t dist);
void scrollY(int16_t dist);

/* Backlight */
void GPIOX(int on);
void PWM1config(int on, uint8_t clock);
void PWM2config(int on, uint8_t clock);
void PWM1out(uint8_t p);
void PWM2out(uint8_t p);

/* Touch screen */
void touchEnable(int on);
int touched(void);
int touchRead(uint16_t* x, uint16_t* y);


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
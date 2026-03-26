/*!
 * @file     Adafruit_RA8875.cpp
 *
 * @mainpage Adafruit RA8875 TFT Driver
 *
 * @author   Limor Friend/Ladyada, K.Townsend/KTOWN for Adafruit Industries
 *
 * @section intro_sec Introduction
 *
 * This is the library for the Adafruit RA8875 Driver board for TFT displays
 * ---------------> http://www.adafruit.com/products/1590
 * The RA8875 is a TFT driver for up to 800x480 dotclock'd displays
 * It is tested to work with displays in the Adafruit shop. Other displays
 * may need timing adjustments and are not guanteed to work.
 *
 * Adafruit invests time and resources providing this open
 * source code, please support Adafruit and open-source hardware
 * by purchasing products from Adafruit!
 *
 * @section author Author
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 *
 * @section license License
 *
 * BSD license, check license.txt for more information.
 * All text above must be included in any redistribution.
 *
 * @section  HISTORY
 *
 * v1.0 - First release
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <ti/devices/msp/msp.h>
#include "../inc/SPI.h"
#include "../inc/Clock.h"
#include "file.h"
#include "RA8875.h"

// Command/Data pins for SPI
#define RA8875_DATAWRITE 0x00 ///< See datasheet
#define RA8875_DATAREAD 0x40  ///< See datasheet
#define RA8875_CMDWRITE 0x80  ///< See datasheet
#define RA8875_CMDREAD 0xC0   ///< See datasheet

// Registers & bits
#define RA8875_PWRR 0x01           ///< See datasheet
#define RA8875_PWRR_DISPON 0x80    ///< See datasheet
#define RA8875_PWRR_DISPOFF 0x00   ///< See datasheet
#define RA8875_PWRR_SLEEP 0x02     ///< See datasheet
#define RA8875_PWRR_NORMAL 0x00    ///< See datasheet
#define RA8875_PWRR_SOFTRESET 0x01 ///< See datasheet

#define RA8875_MRWC 0x02 ///< See datasheet

#define RA8875_GPIOX 0xC7 ///< See datasheet

#define RA8875_PLLC1 0x88         ///< See datasheet
#define RA8875_PLLC1_PLLDIV2 0x80 ///< See datasheet
#define RA8875_PLLC1_PLLDIV1 0x00 ///< See datasheet

#define RA8875_PLLC2 0x89        ///< See datasheet
#define RA8875_PLLC2_DIV1 0x00   ///< See datasheet
#define RA8875_PLLC2_DIV2 0x01   ///< See datasheet
#define RA8875_PLLC2_DIV4 0x02   ///< See datasheet
#define RA8875_PLLC2_DIV8 0x03   ///< See datasheet
#define RA8875_PLLC2_DIV16 0x04  ///< See datasheet
#define RA8875_PLLC2_DIV32 0x05  ///< See datasheet
#define RA8875_PLLC2_DIV64 0x06  ///< See datasheet
#define RA8875_PLLC2_DIV128 0x07 ///< See datasheet

#define RA8875_SYSR 0x10       ///< See datasheet
#define RA8875_SYSR_8BPP 0x00  ///< See datasheet
#define RA8875_SYSR_16BPP 0x0C ///< See datasheet
#define RA8875_SYSR_MCU8 0x00  ///< See datasheet
#define RA8875_SYSR_MCU16 0x03 ///< See datasheet

#define RA8875_PCSR 0x04       ///< See datasheet
#define RA8875_PCSR_PDATR 0x00 ///< See datasheet
#define RA8875_PCSR_PDATL 0x80 ///< See datasheet
#define RA8875_PCSR_CLK 0x00   ///< See datasheet
#define RA8875_PCSR_2CLK 0x01  ///< See datasheet
#define RA8875_PCSR_4CLK 0x02  ///< See datasheet
#define RA8875_PCSR_8CLK 0x03  ///< See datasheet

#define RA8875_HDWR 0x14 ///< See datasheet

#define RA8875_HNDFTR 0x15         ///< See datasheet
#define RA8875_HNDFTR_DE_HIGH 0x00 ///< See datasheet
#define RA8875_HNDFTR_DE_LOW 0x80  ///< See datasheet

#define RA8875_HNDR 0x16      ///< See datasheet
#define RA8875_HSTR 0x17      ///< See datasheet
#define RA8875_HPWR 0x18      ///< See datasheet
#define RA8875_HPWR_LOW 0x00  ///< See datasheet
#define RA8875_HPWR_HIGH 0x80 ///< See datasheet

#define RA8875_VDHR0 0x19     ///< See datasheet
#define RA8875_VDHR1 0x1A     ///< See datasheet
#define RA8875_VNDR0 0x1B     ///< See datasheet
#define RA8875_VNDR1 0x1C     ///< See datasheet
#define RA8875_VSTR0 0x1D     ///< See datasheet
#define RA8875_VSTR1 0x1E     ///< See datasheet
#define RA8875_VPWR 0x1F      ///< See datasheet
#define RA8875_VPWR_LOW 0x00  ///< See datasheet
#define RA8875_VPWR_HIGH 0x80 ///< See datasheet

#define RA8875_HSAW0 0x30 ///< See datasheet
#define RA8875_HSAW1 0x31 ///< See datasheet
#define RA8875_VSAW0 0x32 ///< See datasheet
#define RA8875_VSAW1 0x33 ///< See datasheet

#define RA8875_HEAW0 0x34 ///< See datasheet
#define RA8875_HEAW1 0x35 ///< See datasheet
#define RA8875_VEAW0 0x36 ///< See datasheet
#define RA8875_VEAW1 0x37 ///< See datasheet

#define RA8875_MCLR 0x8E            ///< See datasheet
#define RA8875_MCLR_START 0x80      ///< See datasheet
#define RA8875_MCLR_STOP 0x00       ///< See datasheet
#define RA8875_MCLR_READSTATUS 0x80 ///< See datasheet
#define RA8875_MCLR_FULL 0x00       ///< See datasheet
#define RA8875_MCLR_ACTIVE 0x40     ///< See datasheet

#define RA8875_DCR 0x90                   ///< See datasheet
#define RA8875_DCR_LINESQUTRI_START 0x80  ///< See datasheet
#define RA8875_DCR_LINESQUTRI_STOP 0x00   ///< See datasheet
#define RA8875_DCR_LINESQUTRI_STATUS 0x80 ///< See datasheet
#define RA8875_DCR_CIRCLE_START 0x40      ///< See datasheet
#define RA8875_DCR_CIRCLE_STATUS 0x40     ///< See datasheet
#define RA8875_DCR_CIRCLE_STOP 0x00       ///< See datasheet
#define RA8875_DCR_FILL 0x20              ///< See datasheet
#define RA8875_DCR_NOFILL 0x00            ///< See datasheet
#define RA8875_DCR_DRAWLINE 0x00          ///< See datasheet
#define RA8875_DCR_DRAWTRIANGLE 0x01      ///< See datasheet
#define RA8875_DCR_DRAWSQUARE 0x10        ///< See datasheet

#define RA8875_ELLIPSE 0xA0        ///< See datasheet
#define RA8875_ELLIPSE_STATUS 0x80 ///< See datasheet

#define RA8875_MWCR0 0x40         ///< See datasheet
#define RA8875_MWCR0_GFXMODE 0x00 ///< See datasheet
#define RA8875_MWCR0_TXTMODE 0x80 ///< See datasheet
#define RA8875_MWCR0_CURSOR 0x40  ///< See datasheet
#define RA8875_MWCR0_BLINK 0x20   ///< See datasheet

#define RA8875_MWCR0_DIRMASK 0x0C ///< Bitmask for Write Direction
#define RA8875_MWCR0_LRTD 0x00    ///< Left->Right then Top->Down
#define RA8875_MWCR0_RLTD 0x04    ///< Right->Left then Top->Down
#define RA8875_MWCR0_TDLR 0x08    ///< Top->Down then Left->Right
#define RA8875_MWCR0_DTLR 0x0C    ///< Down->Top then Left->Right

#define RA8875_BTCR 0x44  ///< See datasheet
#define RA8875_CURH0 0x46 ///< See datasheet
#define RA8875_CURH1 0x47 ///< See datasheet
#define RA8875_CURV0 0x48 ///< See datasheet
#define RA8875_CURV1 0x49 ///< See datasheet

#define RA8875_P1CR 0x8A         ///< See datasheet
#define RA8875_P1CR_ENABLE 0x80  ///< See datasheet
#define RA8875_P1CR_DISABLE 0x00 ///< See datasheet
#define RA8875_P1CR_CLKOUT 0x10  ///< See datasheet
#define RA8875_P1CR_PWMOUT 0x00  ///< See datasheet

#define RA8875_P1DCR 0x8B ///< See datasheet

#define RA8875_P2CR 0x8C         ///< See datasheet
#define RA8875_P2CR_ENABLE 0x80  ///< See datasheet
#define RA8875_P2CR_DISABLE 0x00 ///< See datasheet
#define RA8875_P2CR_CLKOUT 0x10  ///< See datasheet
#define RA8875_P2CR_PWMOUT 0x00  ///< See datasheet

#define RA8875_P2DCR 0x8D ///< See datasheet

#define RA8875_PWM_CLK_DIV1 0x00     ///< See datasheet
#define RA8875_PWM_CLK_DIV2 0x01     ///< See datasheet
#define RA8875_PWM_CLK_DIV4 0x02     ///< See datasheet
#define RA8875_PWM_CLK_DIV8 0x03     ///< See datasheet
#define RA8875_PWM_CLK_DIV16 0x04    ///< See datasheet
#define RA8875_PWM_CLK_DIV32 0x05    ///< See datasheet
#define RA8875_PWM_CLK_DIV64 0x06    ///< See datasheet
#define RA8875_PWM_CLK_DIV128 0x07   ///< See datasheet
#define RA8875_PWM_CLK_DIV256 0x08   ///< See datasheet
#define RA8875_PWM_CLK_DIV512 0x09   ///< See datasheet
#define RA8875_PWM_CLK_DIV1024 0x0A  ///< See datasheet
#define RA8875_PWM_CLK_DIV2048 0x0B  ///< See datasheet
#define RA8875_PWM_CLK_DIV4096 0x0C  ///< See datasheet
#define RA8875_PWM_CLK_DIV8192 0x0D  ///< See datasheet
#define RA8875_PWM_CLK_DIV16384 0x0E ///< See datasheet
#define RA8875_PWM_CLK_DIV32768 0x0F ///< See datasheet

#define RA8875_TPCR0 0x70               ///< See datasheet
#define RA8875_TPCR0_ENABLE 0x80        ///< See datasheet
#define RA8875_TPCR0_DISABLE 0x00       ///< See datasheet
#define RA8875_TPCR0_WAIT_512CLK 0x00   ///< See datasheet
#define RA8875_TPCR0_WAIT_1024CLK 0x10  ///< See datasheet
#define RA8875_TPCR0_WAIT_2048CLK 0x20  ///< See datasheet
#define RA8875_TPCR0_WAIT_4096CLK 0x30  ///< See datasheet
#define RA8875_TPCR0_WAIT_8192CLK 0x40  ///< See datasheet
#define RA8875_TPCR0_WAIT_16384CLK 0x50 ///< See datasheet
#define RA8875_TPCR0_WAIT_32768CLK 0x60 ///< See datasheet
#define RA8875_TPCR0_WAIT_65536CLK 0x70 ///< See datasheet
#define RA8875_TPCR0_WAKEENABLE 0x08    ///< See datasheet
#define RA8875_TPCR0_WAKEDISABLE 0x00   ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV1 0x00   ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV2 0x01   ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV4 0x02   ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV8 0x03   ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV16 0x04  ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV32 0x05  ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV64 0x06  ///< See datasheet
#define RA8875_TPCR0_ADCCLK_DIV128 0x07 ///< See datasheet

#define RA8875_TPCR1 0x71            ///< See datasheet
#define RA8875_TPCR1_AUTO 0x00       ///< See datasheet
#define RA8875_TPCR1_MANUAL 0x40     ///< See datasheet
#define RA8875_TPCR1_VREFINT 0x00    ///< See datasheet
#define RA8875_TPCR1_VREFEXT 0x20    ///< See datasheet
#define RA8875_TPCR1_DEBOUNCE 0x04   ///< See datasheet
#define RA8875_TPCR1_NODEBOUNCE 0x00 ///< See datasheet
#define RA8875_TPCR1_IDLE 0x00       ///< See datasheet
#define RA8875_TPCR1_WAIT 0x01       ///< See datasheet
#define RA8875_TPCR1_LATCHX 0x02     ///< See datasheet
#define RA8875_TPCR1_LATCHY 0x03     ///< See datasheet

#define RA8875_TPXH 0x72  ///< See datasheet
#define RA8875_TPYH 0x73  ///< See datasheet
#define RA8875_TPXYL 0x74 ///< See datasheet

#define RA8875_INTC1 0xF0     ///< See datasheet
#define RA8875_INTC1_KEY 0x10 ///< See datasheet
#define RA8875_INTC1_DMA 0x08 ///< See datasheet
#define RA8875_INTC1_TP 0x04  ///< See datasheet
#define RA8875_INTC1_BTE 0x02 ///< See datasheet

#define RA8875_INTC2 0xF1     ///< See datasheet
#define RA8875_INTC2_KEY 0x10 ///< See datasheet
#define RA8875_INTC2_DMA 0x08 ///< See datasheet
#define RA8875_INTC2_TP 0x04  ///< See datasheet
#define RA8875_INTC2_BTE 0x02 ///< See datasheet

#define RA8875_SCROLL_BOTH 0x00   ///< See datasheet
#define RA8875_SCROLL_LAYER1 0x40 ///< See datasheet
#define RA8875_SCROLL_LAYER2 0x80 ///< See datasheet
#define RA8875_SCROLL_BUFFER 0xC0 ///< See datasheet

#define TRUE 1
#define FALSE 0

static uint16_t _width;
static uint16_t _height;
static uint8_t _textScale;
static uint8_t _rotation;
static uint8_t _voffset;
static enum RA8875sizes _size;


/************************* Low Level ***********************************/
/**************************************************************************/
/*!
    Write data to the current register

    @param d Data to write
*/
/**************************************************************************/
static void writeData(uint8_t d) {
  TFT_CS_LOW();
  
  SPI_OutCommand(RA8875_DATAWRITE);
  SPI_OutData(d);
  
  TFT_CS_HIGH();
}

/**************************************************************************/
/*!
    Read the data from the current register

    @return The Value
*/
/**************************************************************************/
static uint16_t readData(void) {
  TFT_CS_LOW();

  SPI_OutCommand(RA8875_DATAREAD);
  uint16_t x = xchg_spi(0x00);
  //uint16_t x = SPI_OutRead16(RA8875_DATAREAD << 8);

  TFT_CS_HIGH();
  return x;
}

/**************************************************************************/
/*!
    Write a command to the current register

    @param d The data to write as a command
 */
/**************************************************************************/
static void writeCommand(uint8_t d) {
  TFT_CS_LOW();

  SPI_OutCommand(RA8875_CMDWRITE);
  SPI_OutData(d);
  //SPI_Out16((RA8875_CMDWRITE << 8) | d);

  TFT_CS_HIGH();
}

/**************************************************************************/
/*!
    Read the status from the current register

    @return The value
 */
/**************************************************************************/
static uint16_t readStatus(void) {
  TFT_CS_LOW();

  SPI_OutCommand(RA8875_CMDREAD);
  uint8_t x = xchg_spi(0x0);

  TFT_CS_HIGH();
  return x;
}

/**************************************************************************/
/*!
    Write data to the specified register

    @param reg Register to write to
    @param val Value to write
*/
/**************************************************************************/
static void writeReg(uint8_t reg, uint8_t val) {
  writeCommand(reg);
  writeData(val);
}

/**************************************************************************/
/*!
    Set the register to read from

    @param reg Register to read

    @return The value
*/
/**************************************************************************/
static uint8_t readReg(uint8_t reg) {
  writeCommand(reg);
  return readData();
}

/************************* Initialization *********************************/

/**************************************************************************/
/*!
      Performs a SW-based reset of the RA8875
*/
/**************************************************************************/
void softReset(void) {
  writeCommand(RA8875_PWRR);
  writeData(RA8875_PWRR_SOFTRESET);
  writeData(RA8875_PWRR_NORMAL);
  Clock_Delay1ms(1);
}

/**************************************************************************/
/*!
      Initialise the PLL
*/
/**************************************************************************/
static void PLLinit(void) {
  if (_size == RA8875_480x80 || _size == RA8875_480x128 ||
      _size == RA8875_480x272) {
    writeReg(RA8875_PLLC1, RA8875_PLLC1_PLLDIV1 + 10);
    Clock_Delay1ms(1);
    writeReg(RA8875_PLLC2, RA8875_PLLC2_DIV4);
    Clock_Delay1ms(1);
  } else /* (_size == RA8875_800x480) */ {
    writeReg(RA8875_PLLC1, RA8875_PLLC1_PLLDIV1 + 11);
    Clock_Delay1ms(1);
    writeReg(RA8875_PLLC2, RA8875_PLLC2_DIV4);
    Clock_Delay1ms(1);
  }
}

/**************************************************************************/
/*!
      Initialises the driver IC (clock setup, etc.)
*/
/**************************************************************************/
static void initialize(void) {
  PLLinit();
  writeReg(RA8875_SYSR, RA8875_SYSR_16BPP | RA8875_SYSR_MCU8);

  /* Timing values */
  uint8_t pixclk;
  uint8_t hsync_start;
  uint8_t hsync_pw;
  uint8_t hsync_finetune;
  uint8_t hsync_nondisp;
  uint8_t vsync_pw;
  uint16_t vsync_nondisp;
  uint16_t vsync_start;

  /* Set the correct values for the display being used */
  if (_size == RA8875_480x80) {
    pixclk = RA8875_PCSR_PDATL | RA8875_PCSR_4CLK;
    hsync_nondisp = 10;
    hsync_start = 8;
    hsync_pw = 48;
    hsync_finetune = 0;
    vsync_nondisp = 3;
    vsync_start = 8;
    vsync_pw = 10;
    _voffset = 192; // This uses the bottom 80 pixels of a 272 pixel controller
  } else if (_size == RA8875_480x128 || _size == RA8875_480x272) {
    pixclk = RA8875_PCSR_PDATL | RA8875_PCSR_4CLK;
    hsync_nondisp = 10;
    hsync_start = 8;
    hsync_pw = 48;
    hsync_finetune = 0;
    vsync_nondisp = 3;
    vsync_start = 8;
    vsync_pw = 10;
    _voffset = 0;
  } else {// (_size == RA8875_800x480)
    pixclk = RA8875_PCSR_PDATL | RA8875_PCSR_2CLK;
    hsync_nondisp = 26;
    hsync_start = 32;
    hsync_pw = 96;
    hsync_finetune = 0;
    vsync_nondisp = 32;
    vsync_start = 23;
    vsync_pw = 2;
    _voffset = 0;
  }

  writeReg(RA8875_PCSR, pixclk);
  Clock_Delay1ms(1);

  /* Horizontal settings registers */
  writeReg(RA8875_HDWR, (_width / 8) - 1); // H width: (HDWR + 1) * 8 = 480
  writeReg(RA8875_HNDFTR, RA8875_HNDFTR_DE_HIGH + hsync_finetune);
  writeReg(RA8875_HNDR, (hsync_nondisp - hsync_finetune - 2) /
                            8); // H non-display: HNDR * 8 + HNDFTR + 2 = 10
  writeReg(RA8875_HSTR, hsync_start / 8 - 1); // Hsync start: (HSTR + 1)*8
  writeReg(
      RA8875_HPWR,
      RA8875_HPWR_LOW + (hsync_pw / 8 - 1)); // HSync pulse width = (HPWR+1) * 8

  /* Vertical settings registers */
  writeReg(RA8875_VDHR0, (uint16_t)(_height - 1 + _voffset) & 0xFF);
  writeReg(RA8875_VDHR1, (uint16_t)(_height - 1 + _voffset) >> 8);
  writeReg(RA8875_VNDR0, vsync_nondisp - 1); // V non-display period = VNDR + 1
  writeReg(RA8875_VNDR1, vsync_nondisp >> 8);
  writeReg(RA8875_VSTR0, vsync_start - 1); // Vsync start position = VSTR + 1
  writeReg(RA8875_VSTR1, vsync_start >> 8);
  writeReg(RA8875_VPWR,
           RA8875_VPWR_LOW + vsync_pw - 1); // Vsync pulse width = VPWR + 1

  /* Set active window X */
  writeReg(RA8875_HSAW0, 0); // horizontal start point
  writeReg(RA8875_HSAW1, 0);
  writeReg(RA8875_HEAW0, (uint16_t)(_width - 1) & 0xFF); // horizontal end point
  writeReg(RA8875_HEAW1, (uint16_t)(_width - 1) >> 8);

  /* Set active window Y */
  writeReg(RA8875_VSAW0, 0 + _voffset); // vertical start point
  writeReg(RA8875_VSAW1, 0 + _voffset);
  writeReg(RA8875_VEAW0,
           (uint16_t)(_height - 1 + _voffset) & 0xFF); // vertical end point
  writeReg(RA8875_VEAW1, (uint16_t)(_height - 1 + _voffset) >> 8);

  /* ToDo: Setup touch panel? */

  /* Clear the entire window */
  writeReg(RA8875_MCLR, RA8875_MCLR_START | RA8875_MCLR_FULL);
  Clock_Delay1ms(500);
}

/**************************************************************************/
/*!
      Initialises the LCD driver and any HW required by the display

      @param s The display size, which can be either:
                  'RA8875_480x80'  (3.8" displays) or
                  'RA8875_480x128' (3.9" displays) or
                  'RA8875_480x272' (4.3" displays) or
                  'RA8875_800x480' (5" and 7" displays)

      @return True if we reached the end
*/
/**************************************************************************/
int RA8875_begin(enum RA8875sizes s) {
  _size = s;

  if (_size == RA8875_480x80) {
    _width = 480;
    _height = 80;
  } else if (_size == RA8875_480x128) {
    _width = 480;
    _height = 128;
  } else if (_size == RA8875_480x272) {
    _width = 480;
    _height = 272;
  } else if (_size == RA8875_800x480) {
    _width = 800;
    _height = 480;
  } else {
    return FALSE;
  }
  _rotation = 0;
  SPI_Init();

  uint16_t x = readReg(0);
  //    Serial.print("x = 0x"); Serial.println(x,HEX);
  if (x != 0x75) { //0x75
    int y;
    while(1) {
      y = readReg(0);
    }
    //Serial.println(x);
    return FALSE;
  }

  initialize();

  return TRUE;
}

/**************************************************************************/
/*!
      Returns the display width in pixels

      @return  The 1-based display width in pixels
*/
/**************************************************************************/
uint16_t width(void) {
  return _width;
}

/**************************************************************************/
/*!
      Returns the display height in pixels

      @return  The 1-based display height in pixels
*/
/**************************************************************************/
uint16_t height(void) {
  return _height;
}

/**************************************************************************/
/*!
 Returns the current rotation (0-3)

 @return  The Rotation Setting
 */
/**************************************************************************/
int8_t getRotation(void) {
  return _rotation;
}

/**************************************************************************/
/*!
 Sets the current rotation (0-3)

 @param rotation The Rotation Setting
 */
/**************************************************************************/
void setRotation(int8_t rotation) {
  switch (rotation) {
    case 2:
      _rotation = rotation;
      break;
    default:
      _rotation = 0;
      break;
  }
}

/************************* Graphics ***********************************/

/**************************************************************************/
/*!
      Sets the display in graphics mode (as opposed to text mode)
*/
/**************************************************************************/
void graphicsMode(void) {
  writeCommand(RA8875_MWCR0);
  uint8_t temp = readData();
  temp &= ~RA8875_MWCR0_TXTMODE; // bit #7
  writeData(temp);
}

/**************************************************************************/
/*!
      Waits for screen to finish by polling the status!

      @param regname The register name to check
      @param waitflag The value to wait for the status register to match

      @return True if the expected status has been reached
*/
/**************************************************************************/
static int waitPoll(uint8_t regname, uint8_t waitflag) {
  /* Wait for the command to finish */
  while (1) {
    uint8_t temp = readReg(regname);
    if (!(temp & waitflag))
      return TRUE;
  }
  return FALSE; // MEMEFIX: yeah i know, unreached! - add timeout?
}

/**************************************************************************/
/*!
      Sets the current X/Y position on the display before drawing

      @param x The 0-based x location
      @param y The 0-base y location
*/
/**************************************************************************/
void setXY(uint16_t x, uint16_t y) {
  writeReg(RA8875_CURH0, x);
  writeReg(RA8875_CURH1, x >> 8);
  writeReg(RA8875_CURV0, y);
  writeReg(RA8875_CURV1, y >> 8);
}

/**************************************************************************/
/*!
      HW accelerated function to push a chunk of raw pixel data

      @param num The number of pixels to push
      @param p   The pixel color to use
*/
/**************************************************************************/
void pushPixels(uint32_t num, uint16_t p) {
  TFT_CS_LOW();
  SPI_OutCommand(RA8875_DATAWRITE);
  while (num--) {
    SPI_OutData(p >> 8);
    SPI_OutData(p & 0x00FF);
  }
  TFT_CS_HIGH();
}

/**************************************************************************/
/*!
    Fill the screen with the current color
*/
/**************************************************************************/
void fillScreenWithCurrentColor(void) {
  writeCommand(RA8875_DCR);
  writeData(RA8875_DCR_LINESQUTRI_STOP | RA8875_DCR_DRAWSQUARE);
  writeData(RA8875_DCR_LINESQUTRI_START | RA8875_DCR_FILL |
            RA8875_DCR_DRAWSQUARE);
}

/**************************************************************************/
/*!
    Apply current rotation in the X direction

    @return the X value with current rotation applied
 */
/**************************************************************************/
static int16_t applyRotationX(int16_t x) {
  switch (_rotation) {
    case 2:
      x = _width - 1 - x;
      break;
  }

  return x;
}

/**************************************************************************/
/*!
    Apply current rotation in the Y direction

    @return the Y value with current rotation applied
 */
/**************************************************************************/
static int16_t applyRotationY(int16_t y) {
  switch (_rotation) {
    case 2:
      y = _height - 1 - y;
      break;
  }

  return y + _voffset;
}

/**************************************************************************/
/*!
      Draws a single pixel at the specified location

      @param x     The 0-based x location
      @param y     The 0-base y location
      @param color The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void drawPixel(int16_t x, int16_t y, uint16_t color) {
  x = applyRotationX(x);
  y = applyRotationY(y);

  writeReg(RA8875_CURH0, x);
  writeReg(RA8875_CURH1, x >> 8);
  writeReg(RA8875_CURV0, y);
  writeReg(RA8875_CURV1, y >> 8);
  writeCommand(RA8875_MRWC);
  TFT_CS_LOW();
  SPI_OutCommand(RA8875_DATAWRITE);
  SPI_OutData(color >> 8);
  SPI_OutData(color);
  TFT_CS_HIGH();
}

/**************************************************************************/
/*!
 Draws a series of pixels at the specified location without the overhead

 @param p     An array of RGB565 color pixels
 @param num   The number of the pixels to draw
 @param x     The 0-based x location
 @param y     The 0-base y location
 */
/**************************************************************************/
void drawPixels(uint16_t* p, uint32_t num, int16_t x,
                                 int16_t y) {
  x = applyRotationX(x);
  y = applyRotationY(y);

  writeReg(RA8875_CURH0, x);
  writeReg(RA8875_CURH1, x >> 8);
  writeReg(RA8875_CURV0, y);
  writeReg(RA8875_CURV1, y >> 8);

  uint8_t dir = RA8875_MWCR0_LRTD;
  if (_rotation == 2) {
    dir = RA8875_MWCR0_RLTD;
  }
  writeReg(RA8875_MWCR0, (readReg(RA8875_MWCR0) & ~RA8875_MWCR0_DIRMASK) | dir);

  writeCommand(RA8875_MRWC);
  TFT_CS_LOW();
  SPI_OutCommand(RA8875_DATAWRITE);
  while (num--) {
    //SPI_transfer16(*p++); //TODO 16 bit transfer?
    SPI_OutData(*p & 0x00FF);
    SPI_OutData((*p & 0xFF00) >> 8);
  }
  TFT_CS_HIGH();
}

/**************************************************************************/
/*!
      Draws a HW accelerated line on the display

      @param x0    The 0-based starting x location
      @param y0    The 0-base starting y location
      @param x1    The 0-based ending x location
      @param y1    The 0-base ending y location
      @param color The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                               uint16_t color) {
  x0 = applyRotationX(x0);
  y0 = applyRotationY(y0);
  x1 = applyRotationX(x1);
  y1 = applyRotationY(y1);

  /* Set X */
  writeCommand(0x91);
  writeData(x0);
  writeCommand(0x92);
  writeData(x0 >> 8);

  /* Set Y */
  writeCommand(0x93);
  writeData(y0);
  writeCommand(0x94);
  writeData(y0 >> 8);

  /* Set X1 */
  writeCommand(0x95);
  writeData(x1);
  writeCommand(0x96);
  writeData((x1) >> 8);

  /* Set Y1 */
  writeCommand(0x97);
  writeData(y1);
  writeCommand(0x98);
  writeData((y1) >> 8);

  /* Set Color */
  writeCommand(0x63);
  writeData((color & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((color & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((color & 0x001f));

  /* Draw! */
  writeCommand(RA8875_DCR);
  writeData(0x80);

  /* Wait for the command to finish */
  waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

/**************************************************************************/
/*!
    Draw a vertical line

    @param x The X position
    @param y The Y position
    @param h Height
    @param color The color
*/
/**************************************************************************/
void drawFastVLine(int16_t x, int16_t y, int16_t h,
                                    uint16_t color) {
  drawLine(x, y, x, y + h, color);
}

/**************************************************************************/
/*!
     Draw a horizontal line

     @param x The X position
     @param y The Y position
     @param w Width
     @param color The color
*/
/**************************************************************************/
void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  drawLine(x, y, x + w, y, color);
}

/**************************************************************************/
/*!
      Helper function for higher level circle drawing code
*/
/**************************************************************************/
static void circleHelper(int16_t x, int16_t y, int16_t r, uint16_t color, int filled) {
  x = applyRotationX(x);
  y = applyRotationY(y);

  /* Set X */
  writeCommand(0x99);
  writeData(x);
  writeCommand(0x9a);
  writeData(x >> 8);

  /* Set Y */
  writeCommand(0x9b);
  writeData(y);
  writeCommand(0x9c);
  writeData(y >> 8);

  /* Set Radius */
  writeCommand(0x9d);
  writeData(r);

  /* Set Color */
  writeCommand(0x63);
  writeData((color & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((color & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((color & 0x001f));

  /* Draw! */
  writeCommand(RA8875_DCR);
  if (filled) {
    writeData(RA8875_DCR_CIRCLE_START | RA8875_DCR_FILL);
  } else {
    writeData(RA8875_DCR_CIRCLE_START | RA8875_DCR_NOFILL);
  }

  /* Wait for the command to finish */
  waitPoll(RA8875_DCR, RA8875_DCR_CIRCLE_STATUS);
}

/**************************************************************************/
/*!
      Helper function for higher level rectangle drawing code
*/
/**************************************************************************/
static void rectHelper(int16_t x, int16_t y, int16_t w, int16_t h,
                                 uint16_t color, bool filled) {
  x = applyRotationX(x);
  y = applyRotationY(y);
  w = applyRotationX(w);
  h = applyRotationY(h);

  /* Set X */
  writeCommand(0x91);
  writeData(x);
  writeCommand(0x92);
  writeData(x >> 8);

  /* Set Y */
  writeCommand(0x93);
  writeData(y);
  writeCommand(0x94);
  writeData(y >> 8);

  /* Set X1 */
  writeCommand(0x95);
  writeData(w);
  writeCommand(0x96);
  writeData((w) >> 8);

  /* Set Y1 */
  writeCommand(0x97);
  writeData(h);
  writeCommand(0x98);
  writeData((h) >> 8);

  /* Set Color */
  writeCommand(0x63);
  writeData((color & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((color & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((color & 0x001f));

  /* Draw! */
  writeCommand(RA8875_DCR);
  if (filled) {
    writeData(0xB0);
  } else {
    writeData(0x90);
  }

  /* Wait for the command to finish */
  waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

/**************************************************************************/
/*!
      Helper function for higher level triangle drawing code
*/
/**************************************************************************/
static void triangleHelper(int16_t x0, int16_t y0, int16_t x1,
                                     int16_t y1, int16_t x2, int16_t y2,
                                     uint16_t color, bool filled) {
  x0 = applyRotationX(x0);
  y0 = applyRotationY(y0);
  x1 = applyRotationX(x1);
  y1 = applyRotationY(y1);
  x2 = applyRotationX(x2);
  y2 = applyRotationY(y2);

  /* Set Point 0 */
  writeCommand(0x91);
  writeData(x0);
  writeCommand(0x92);
  writeData(x0 >> 8);
  writeCommand(0x93);
  writeData(y0);
  writeCommand(0x94);
  writeData(y0 >> 8);

  /* Set Point 1 */
  writeCommand(0x95);
  writeData(x1);
  writeCommand(0x96);
  writeData(x1 >> 8);
  writeCommand(0x97);
  writeData(y1);
  writeCommand(0x98);
  writeData(y1 >> 8);

  /* Set Point 2 */
  writeCommand(0xA9);
  writeData(x2);
  writeCommand(0xAA);
  writeData(x2 >> 8);
  writeCommand(0xAB);
  writeData(y2);
  writeCommand(0xAC);
  writeData(y2 >> 8);

  /* Set Color */
  writeCommand(0x63);
  writeData((color & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((color & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((color & 0x001f));

  /* Draw! */
  writeCommand(RA8875_DCR);
  if (filled) {
    writeData(0xA1);
  } else {
    writeData(0x81);
  }

  /* Wait for the command to finish */
  waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

/**************************************************************************/
/*!
      Helper function for higher level ellipse drawing code
*/
/**************************************************************************/
static void ellipseHelper(int16_t xCenter, int16_t yCenter,
                                    int16_t longAxis, int16_t shortAxis,
                                    uint16_t color, bool filled) {
  xCenter = applyRotationX(xCenter);
  yCenter = applyRotationY(yCenter);

  /* Set Center Point */
  writeCommand(0xA5);
  writeData(xCenter);
  writeCommand(0xA6);
  writeData(xCenter >> 8);
  writeCommand(0xA7);
  writeData(yCenter);
  writeCommand(0xA8);
  writeData(yCenter >> 8);

  /* Set Long and Short Axis */
  writeCommand(0xA1);
  writeData(longAxis);
  writeCommand(0xA2);
  writeData(longAxis >> 8);
  writeCommand(0xA3);
  writeData(shortAxis);
  writeCommand(0xA4);
  writeData(shortAxis >> 8);

  /* Set Color */
  writeCommand(0x63);
  writeData((color & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((color & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((color & 0x001f));

  /* Draw! */
  writeCommand(0xA0);
  if (filled) {
    writeData(0xC0);
  } else {
    writeData(0x80);
  }

  /* Wait for the command to finish */
  waitPoll(RA8875_ELLIPSE, RA8875_ELLIPSE_STATUS);
}

/**************************************************************************/
/*!
      Helper function for higher level curve drawing code
*/
/**************************************************************************/
static void curveHelper(int16_t xCenter, int16_t yCenter,
                                  int16_t longAxis, int16_t shortAxis,
                                  uint8_t curvePart, uint16_t color,
                                  bool filled) {
  xCenter = applyRotationX(xCenter);
  yCenter = applyRotationY(yCenter);
  curvePart = (curvePart + _rotation) % 4;

  /* Set Center Point */
  writeCommand(0xA5);
  writeData(xCenter);
  writeCommand(0xA6);
  writeData(xCenter >> 8);
  writeCommand(0xA7);
  writeData(yCenter);
  writeCommand(0xA8);
  writeData(yCenter >> 8);

  /* Set Long and Short Axis */
  writeCommand(0xA1);
  writeData(longAxis);
  writeCommand(0xA2);
  writeData(longAxis >> 8);
  writeCommand(0xA3);
  writeData(shortAxis);
  writeCommand(0xA4);
  writeData(shortAxis >> 8);

  /* Set Color */
  writeCommand(0x63);
  writeData((color & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((color & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((color & 0x001f));

  /* Draw! */
  writeCommand(0xA0);
  if (filled) {
    writeData(0xD0 | (curvePart & 0x03));
  } else {
    writeData(0x90 | (curvePart & 0x03));
  }

  /* Wait for the command to finish */
  waitPoll(RA8875_ELLIPSE, RA8875_ELLIPSE_STATUS);
}

static void swap(int16_t* x, int16_t* y) {
  int16_t temp = *x;
  *x = *y;
  *y = temp;
}

/**************************************************************************/
/*!
      Helper function for higher level rounded rectangle drawing code
 */
/**************************************************************************/
static void roundRectHelper(int16_t x, int16_t y, int16_t w,
                                      int16_t h, int16_t r, uint16_t color,
                                      int filled) {
  x = applyRotationX(x);
  y = applyRotationY(y);
  w = applyRotationX(w);
  h = applyRotationY(h);
  if (x > w)
    swap(&x, &w);
  if (y > h)
    swap(&y, &h);

  /* Set X */
  writeCommand(0x91);
  writeData(x);
  writeCommand(0x92);
  writeData(x >> 8);

  /* Set Y */
  writeCommand(0x93);
  writeData(y);
  writeCommand(0x94);
  writeData(y >> 8);

  /* Set X1 */
  writeCommand(0x95);
  writeData(w);
  writeCommand(0x96);
  writeData((w) >> 8);

  /* Set Y1 */
  writeCommand(0x97);
  writeData(h);
  writeCommand(0x98);
  writeData((h) >> 8);

  writeCommand(0xA1);
  writeData(r);
  writeCommand(0xA2);
  writeData((r) >> 8);

  writeCommand(0xA3);
  writeData(r);
  writeCommand(0xA4);
  writeData((r) >> 8);

  /* Set Color */
  writeCommand(0x63);
  writeData((color & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((color & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((color & 0x001f));

  /* Draw! */
  writeCommand(RA8875_ELLIPSE);
  if (filled) {
    writeData(0xE0);
  } else {
    writeData(0xA0);
  }

  /* Wait for the command to finish */
  waitPoll(RA8875_ELLIPSE, RA8875_DCR_LINESQUTRI_STATUS);
}

/**************************************************************************/
/*!
      Draws a HW accelerated rectangle on the display

      @param x     The 0-based x location of the top-right corner
      @param y     The 0-based y location of the top-right corner
      @param w     The rectangle width
      @param h     The rectangle height
      @param color The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  rectHelper(x, y, x + w - 1, y + h - 1, color, FALSE);
}

/**************************************************************************/
/*!
      Draws a HW accelerated filled rectangle on the display

      @param x     The 0-based x location of the top-right corner
      @param y     The 0-based y location of the top-right corner
      @param w     The rectangle width
      @param h     The rectangle height
      @param color The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  rectHelper(x, y, x + w - 1, y + h - 1, color, TRUE);
}

/**************************************************************************/
/*!
      Fills the screen with the spefied RGB565 color

      @param color The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void fillScreen(uint16_t color) {
  rectHelper(0, 0, _width - 1, _height - 1, color, TRUE);
}

/**************************************************************************/
/*!
      Draws a HW accelerated circle on the display

      @param x     The 0-based x location of the center of the circle
      @param y     The 0-based y location of the center of the circle
      @param r     The circle's radius
      @param color The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color) {
  circleHelper(x, y, r, color, FALSE);
}

/**************************************************************************/
/*!
      Draws a HW accelerated filled circle on the display

      @param x     The 0-based x location of the center of the circle
      @param y     The 0-based y location of the center of the circle
      @param r     The circle's radius
      @param color The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void fillCircle(int16_t x, int16_t y, int16_t r, uint16_t color) {
  circleHelper(x, y, r, color, TRUE);
}

/**************************************************************************/
/*!
      Draws a HW accelerated triangle on the display

      @param x0    The 0-based x location of point 0 on the triangle
      @param y0    The 0-based y location of point 0 on the triangle
      @param x1    The 0-based x location of point 1 on the triangle
      @param y1    The 0-based y location of point 1 on the triangle
      @param x2    The 0-based x location of point 2 on the triangle
      @param y2    The 0-based y location of point 2 on the triangle
      @param color The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void drawTriangle(int16_t x0, int16_t y0, int16_t x1,
                                   int16_t y1, int16_t x2, int16_t y2,
                                   uint16_t color) {
  triangleHelper(x0, y0, x1, y1, x2, y2, color, FALSE);
}

/**************************************************************************/
/*!
      Draws a HW accelerated filled triangle on the display

      @param x0    The 0-based x location of point 0 on the triangle
      @param y0    The 0-based y location of point 0 on the triangle
      @param x1    The 0-based x location of point 1 on the triangle
      @param y1    The 0-based y location of point 1 on the triangle
      @param x2    The 0-based x location of point 2 on the triangle
      @param y2    The 0-based y location of point 2 on the triangle
      @param color The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void fillTriangle(int16_t x0, int16_t y0, int16_t x1,
                                   int16_t y1, int16_t x2, int16_t y2,
                                   uint16_t color) {
  triangleHelper(x0, y0, x1, y1, x2, y2, color, TRUE);
}

/**************************************************************************/
/*!
      Draws a HW accelerated ellipse on the display

      @param xCenter   The 0-based x location of the ellipse's center
      @param yCenter   The 0-based y location of the ellipse's center
      @param longAxis  The size in pixels of the ellipse's long axis
      @param shortAxis The size in pixels of the ellipse's short axis
      @param color     The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void drawEllipse(int16_t xCenter, int16_t yCenter,
                                  int16_t longAxis, int16_t shortAxis,
                                  uint16_t color) {
  ellipseHelper(xCenter, yCenter, longAxis, shortAxis, color, FALSE);
}

/**************************************************************************/
/*!
      Draws a HW accelerated filled ellipse on the display

      @param xCenter   The 0-based x location of the ellipse's center
      @param yCenter   The 0-based y location of the ellipse's center
      @param longAxis  The size in pixels of the ellipse's long axis
      @param shortAxis The size in pixels of the ellipse's short axis
      @param color     The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void fillEllipse(int16_t xCenter, int16_t yCenter,
                                  int16_t longAxis, int16_t shortAxis,
                                  uint16_t color) {
  ellipseHelper(xCenter, yCenter, longAxis, shortAxis, color, TRUE);
}

/**************************************************************************/
/*!
      Draws a HW accelerated curve on the display

      @param xCenter   The 0-based x location of the ellipse's center
      @param yCenter   The 0-based y location of the ellipse's center
      @param longAxis  The size in pixels of the ellipse's long axis
      @param shortAxis The size in pixels of the ellipse's short axis
      @param curvePart The corner to draw, where in clock-wise motion:
                            0 = 180-270°
                            1 = 270-0°
                            2 = 0-90°
                            3 = 90-180°
      @param color     The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void drawCurve(int16_t xCenter, int16_t yCenter,
                                int16_t longAxis, int16_t shortAxis,
                                uint8_t curvePart, uint16_t color) {
  curveHelper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, FALSE);
}

/**************************************************************************/
/*!
      Draws a HW accelerated filled curve on the display

      @param xCenter   The 0-based x location of the ellipse's center
      @param yCenter   The 0-based y location of the ellipse's center
      @param longAxis  The size in pixels of the ellipse's long axis
      @param shortAxis The size in pixels of the ellipse's short axis
      @param curvePart The corner to draw, where in clock-wise motion:
                            0 = 180-270°
                            1 = 270-0°
                            2 = 0-90°
                            3 = 90-180°
      @param color     The RGB565 color to use when drawing the pixel
*/
/**************************************************************************/
void fillCurve(int16_t xCenter, int16_t yCenter,
                                int16_t longAxis, int16_t shortAxis,
                                uint8_t curvePart, uint16_t color) {
  curveHelper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, TRUE);
}

/**************************************************************************/
/*!
      Draws a HW accelerated rounded rectangle on the display

      @param x   The 0-based x location of the rectangle's upper left corner
      @param y   The 0-based y location of the rectangle's upper left corner
      @param w   The size in pixels of the rectangle's width
      @param h   The size in pixels of the rectangle's height
      @param r   The radius of the curves in the corners of the rectangle
      @param color  The RGB565 color to use when drawing the pixel
 */
/**************************************************************************/
void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
                                    int16_t r, uint16_t color) {
  roundRectHelper(x, y, x + w, y + h, r, color, FALSE);
}

/**************************************************************************/
/*!
      Draws a HW accelerated filled rounded rectangle on the display

      @param x   The 0-based x location of the rectangle's upper left corner
      @param y   The 0-based y location of the rectangle's upper left corner
      @param w   The size in pixels of the rectangle's width
      @param h   The size in pixels of the rectangle's height
      @param r   The radius of the curves in the corners of the rectangle
      @param color  The RGB565 color to use when drawing the pixel
 */
/**************************************************************************/
void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
                                    int16_t r, uint16_t color) {
  roundRectHelper(x, y, x + w, y + h, r, color, TRUE);
}

/**************************************************************************/
/*!
      Set the scroll window

      @param x  X position of the scroll window
      @param y  Y position of the scroll window
      @param w  Width of the Scroll Window
      @param h  Height of the Scroll window
      @param mode Layer to Scroll

 */
/**************************************************************************/
void setScrollWindow(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t mode) {
  // Horizontal Start point of Scroll Window
  writeCommand(0x38);
  writeData(x);
  writeCommand(0x39);
  writeData(x >> 8);

  // Vertical Start Point of Scroll Window
  writeCommand(0x3a);
  writeData(y);
  writeCommand(0x3b);
  writeData(y >> 8);

  // Horizontal End Point of Scroll Window
  writeCommand(0x3c);
  writeData(x + w);
  writeCommand(0x3d);
  writeData((x + w) >> 8);

  // Vertical End Point of Scroll Window
  writeCommand(0x3e);
  writeData(y + h);
  writeCommand(0x3f);
  writeData((y + h) >> 8);

  // Scroll function setting
  writeCommand(0x52);
  writeData(mode);
}

/**************************************************************************/
/*!
    Scroll in the X direction

    @param dist The distance to scroll

 */
/**************************************************************************/
void scrollX(int16_t dist) {
  writeCommand(0x24);
  writeData(dist);
  writeCommand(0x25);
  writeData(dist >> 8);
}

/**************************************************************************/
/*!
     Scroll in the Y direction

     @param dist The distance to scroll

 */
/**************************************************************************/
void scrollY(int16_t dist) {
  writeCommand(0x26);
  writeData(dist);
  writeCommand(0x27);
  writeData(dist >> 8);
}

/************************* Text Mode ***********************************/

/**************************************************************************/
/*!
      Sets the display in text mode (as opposed to graphics mode)
*/
/**************************************************************************/
void textMode(void) {
  /* Set text mode */
  writeCommand(RA8875_MWCR0);
  uint8_t temp = readData();
  temp |= RA8875_MWCR0_TXTMODE; // Set bit 7
  writeData(temp);

  /* Select the internal (ROM) font */
  writeCommand(0x21);
  temp = readData();
  temp &= ~((1 << 7) | (1 << 5)); // Clear bits 7 and 5
  writeData(temp);
}

/**************************************************************************/
/*!
      Sets the display in text mode (as opposed to graphics mode)

      @param x The x position of the cursor (in pixels, 0..1023)
      @param y The y position of the cursor (in pixels, 0..511)
*/
/**************************************************************************/
void textSetCursor(uint16_t x, uint16_t y) {
  x = applyRotationX(x);
  y = applyRotationY(y);

  /* Set cursor location */
  writeCommand(0x2A);
  writeData(x & 0xFF);
  writeCommand(0x2B);
  writeData(x >> 8);
  writeCommand(0x2C);
  writeData(y & 0xFF);
  writeCommand(0x2D);
  writeData(y >> 8);
}

/**************************************************************************/
/*!
      Sets the fore and background color when rendering text

      @param foreColor The RGB565 color to use when rendering the text
      @param bgColor   The RGB565 colot to use for the background
*/
/**************************************************************************/
void textColor(uint16_t foreColor, uint16_t bgColor) {
  /* Set Fore Color */
  writeCommand(0x63);
  writeData((foreColor & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((foreColor & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((foreColor & 0x001f));

  /* Set Background Color */
  writeCommand(0x60);
  writeData((bgColor & 0xf800) >> 11);
  writeCommand(0x61);
  writeData((bgColor & 0x07e0) >> 5);
  writeCommand(0x62);
  writeData((bgColor & 0x001f));

  /* Clear transparency flag */
  writeCommand(0x22);
  uint8_t temp = readData();
  temp &= ~(1 << 6); // Clear bit 6
  writeData(temp);
}

/**************************************************************************/
/*!
      Sets the fore color when rendering text with a transparent bg

      @param foreColor The RGB565 color to use when rendering the text
*/
/**************************************************************************/
void textTransparent(uint16_t foreColor) {
  /* Set Fore Color */
  writeCommand(0x63);
  writeData((foreColor & 0xf800) >> 11);
  writeCommand(0x64);
  writeData((foreColor & 0x07e0) >> 5);
  writeCommand(0x65);
  writeData((foreColor & 0x001f));

  /* Set transparency flag */
  writeCommand(0x22);
  uint8_t temp = readData();
  temp |= (1 << 6); // Set bit 6
  writeData(temp);
}

/**************************************************************************/
/*!
      Sets the text enlarge settings, using one of the following values:

      0 = 1x zoom
      1 = 2x zoom
      2 = 3x zoom
      3 = 4x zoom

      @param scale   The zoom factor (0..3 for 1-4x zoom)
*/
/**************************************************************************/
void textEnlarge(uint8_t scale) {
  if (scale > 3)
    scale = 3; // highest setting is 3

  /* Set font size flags */
  writeCommand(0x22);
  uint8_t temp = readData();
  temp &= ~(0xF); // Clears bits 0..3
  temp |= scale << 2;
  temp |= scale;

  writeData(temp);

  _textScale = scale;
}


/**************************************************************************/
/*!
     Enable Cursor Visibility and Blink
     Here we set bits 6 and 5 in 40h
     As well as the set the blink rate in 44h
     The rate is 0 through max 255
     the lower the number the faster it blinks (00h is 1 frame time,
     FFh is 256 Frames time.
     Blink Time (sec) = BTCR[44h]x(1/Frame_rate)

     @param rate The frame rate to blink
 */
/**************************************************************************/

void cursorBlink(uint8_t rate) {
  writeCommand(RA8875_MWCR0);
  uint8_t temp = readData();
  temp |= RA8875_MWCR0_CURSOR;
  writeData(temp);

  writeCommand(RA8875_MWCR0);
  temp = readData();
  temp |= RA8875_MWCR0_BLINK;
  writeData(temp);

  if (rate > 255)
    rate = 255;
  writeCommand(RA8875_BTCR);
  writeData(rate);
}

/**************************************************************************/
/*!
      Renders some text on the screen when in text mode

      @param buffer    The buffer containing the characters to render
      @param len       The size of the buffer in bytes
*/
/**************************************************************************/
void textWrite(const char* buffer, uint16_t len) {
  if (len == 0) {
    return;
  }
  writeCommand(RA8875_MRWC);
  for (uint16_t i = 0; i < len; i++) {
    writeData(buffer[i]);
    //if (_textScale > 1)
    if (_textScale > 0) { 
      Clock_Delay1ms(1);
    }
  }
}

/************************* Mid Level ***********************************/

/**************************************************************************/
/*!
    Set the Extra General Purpose IO Register

    @param on Whether to turn Extra General Purpose IO on or not

 */
/**************************************************************************/
void GPIOX(int isOn) {
  if (isOn)
    writeReg(RA8875_GPIOX, 1);
  else
    writeReg(RA8875_GPIOX, 0);
}

/**************************************************************************/
/*!
    Set the duty cycle of the PWM 1 Clock

    @param p The duty Cycle (0-255)
*/
/**************************************************************************/
void PWM1out(uint8_t p) {
  writeReg(RA8875_P1DCR, p);
}

/**************************************************************************/
/*!
     Set the duty cycle of the PWM 2 Clock

     @param p The duty Cycle (0-255)
*/
/**************************************************************************/
void PWM2out(uint8_t p) {
  writeReg(RA8875_P2DCR, p);
}

/**************************************************************************/
/*!
    Configure the PWM 1 Clock

    @param on Whether to enable the clock
    @param clock The Clock Divider
*/
/**************************************************************************/
void PWM1config(int isOn, uint8_t clock) {
  if (isOn) {
    writeReg(RA8875_P1CR, RA8875_P1CR_ENABLE | (clock & 0xF));
  } else {
    writeReg(RA8875_P1CR, RA8875_P1CR_DISABLE | (clock & 0xF));
  }
}

/**************************************************************************/
/*!
     Configure the PWM 2 Clock

     @param on Whether to enable the clock
     @param clock The Clock Divider
*/
/**************************************************************************/
void PWM2config(int isOn, uint8_t clock) {
  if (isOn) {
    writeReg(RA8875_P2CR, RA8875_P2CR_ENABLE | (clock & 0xF));
  } else {
    writeReg(RA8875_P2CR, RA8875_P2CR_DISABLE | (clock & 0xF));
  }
}

/**************************************************************************/
/*!
      Enables or disables the on-chip touch screen controller

      @param on Whether to turn touch sensing on or not
*/
/**************************************************************************/
void touchEnable(int isOn) {
  uint8_t adcClk = (uint8_t)RA8875_TPCR0_ADCCLK_DIV4;

  if (_size == RA8875_800x480) // match up touch size with LCD size
    adcClk = (uint8_t)RA8875_TPCR0_ADCCLK_DIV16;

  if (isOn) {
    /* Enable Touch Panel (Reg 0x70) */
    writeReg(RA8875_TPCR0, RA8875_TPCR0_ENABLE | RA8875_TPCR0_WAIT_4096CLK |
                               RA8875_TPCR0_WAKEENABLE | adcClk); // 10mhz max!
    /* Set Auto Mode      (Reg 0x71) */
    writeReg(RA8875_TPCR1, RA8875_TPCR1_AUTO |
                               // RA8875_TPCR1_VREFEXT |
                               RA8875_TPCR1_DEBOUNCE);
    /* Enable TP INT */
    writeReg(RA8875_INTC1, readReg(RA8875_INTC1) | RA8875_INTC1_TP);
  } else {
    /* Disable TP INT */
    writeReg(RA8875_INTC1, readReg(RA8875_INTC1) & ~RA8875_INTC1_TP);
    /* Disable Touch Panel (Reg 0x70) */
    writeReg(RA8875_TPCR0, RA8875_TPCR0_DISABLE);
  }
}

/**************************************************************************/
/*!
      Checks if a touch event has occured

      @return  True is a touch event has occured (reading it via
               touchRead() will clear the interrupt in memory)
*/
/**************************************************************************/
int touched(void) {
  if (readReg(RA8875_INTC2) & RA8875_INTC2_TP)
    return TRUE;
  return FALSE;
}

/**************************************************************************/
/*!
      Reads the last touch event

      @param x  Pointer to the uint16_t field to assign the raw X value
      @param y  Pointer to the uint16_t field to assign the raw Y value

      @return True if successful

      @note Calling this function will clear the touch panel interrupt on
            the RA8875, resetting the flag used by the 'touched' function
*/
/**************************************************************************/
int touchRead(uint16_t* x, uint16_t* y) {
  uint16_t tx, ty;
  uint8_t temp;

  tx = readReg(RA8875_TPXH);
  ty = readReg(RA8875_TPYH);
  temp = readReg(RA8875_TPXYL);
  tx <<= 2;
  ty <<= 2;
  tx |= temp & 0x03;        // get the bottom x bits
  ty |= (temp >> 2) & 0x03; // get the bottom y bits

  *x = tx;
  *y = ty;

  /* Clear TP INT Status */
  writeReg(RA8875_INTC2, RA8875_INTC2_TP);

  return TRUE;
}

/**************************************************************************/
/*!
      Turns the display on or off

      @param on Whether to turn the display on or not
*/
/**************************************************************************/
void displayOn(int isOn) {
  if (isOn)
    writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON);
  else
    writeReg(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPOFF);
}

/**************************************************************************/
/*!
    Puts the display in sleep mode, or disables sleep mode if enabled

    @param sleep Whether to sleep or not
*/
/**************************************************************************/
void sleep(int isSleep) {
  if (isSleep)
    writeReg(RA8875_PWRR, RA8875_PWRR_DISPOFF | RA8875_PWRR_SLEEP);
  else
    writeReg(RA8875_PWRR, RA8875_PWRR_DISPOFF);
}
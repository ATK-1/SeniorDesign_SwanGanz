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

static int16_t _width = 800;   
static int16_t _height = 480;



/************************* Low Level ***********************************/

/**************************************************************************/
/*!
    Write data to the specified register

    @param reg Register to write to
    @param val Value to write
*/
/**************************************************************************/
void writeReg(uint8_t reg, uint8_t val) {
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
uint8_t readReg(uint8_t reg) {
  writeCommand(reg);
  return readData();
}

/**************************************************************************/
/*!
    Write data to the current register

    @param d Data to write
*/
/**************************************************************************/
void writeData(uint8_t d) {
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
uint8_t readData(void) {
  TFT_CS_LOW();

  SPI_OutCommand(RA8875_DATAREAD);
  uint8_t x = xchg_spi(0x0);

  TFT_CS_HIGH();
  return x;
}

/**************************************************************************/
/*!
    Write a command to the current register

    @param d The data to write as a command
 */
/**************************************************************************/
void writeCommand(uint8_t d) {
  TFT_CS_LOW();

  SPI_OutCommand(RA8875_CMDWRITE);
  SPI_OutData(d);

  TFT_CS_HIGH();
}

/**************************************************************************/
/*!
    Read the status from the current register

    @return The value
 */
/**************************************************************************/
uint8_t readStatus(void) {
  TFT_CS_LOW();

  SPI_OutCommand(RA8875_CMDREAD);
  uint8_t x = xchg_spi(0x0);

  TFT_CS_HIGH();
  return x;
}
/**
 * @file      ADC.h
 * @brief     Initialize 12-bit ADC0 and/or ADC1
 * @details   ADC input, software trigger, 12-bit conversion<br>
 * The ADC allows two possible references: 2.5V or 3.3V.<br>
 * The internal 2.5V reference is lower noise, but limits the range to 0 to 2.5V.<br>
 * The other possibility is to use the analog voltage supply at 3.3V,
 * making the ADC range 0 to 3.3V. In this driver, the 3.3V range is selected.
 * There are several configurations (each with initialization and a
 * read ADC with software trigger, busy-wait function)<br>

 * @version   ECE319K v1.0
 * @author    Daniel Valvano and Jonathan Valvano
 * @copyright Copyright 2023 by Jonathan W. Valvano, valvano@mail.utexas.edu,
 * @warning   AS-IS
 * @note      For more information see  http://users.ece.utexas.edu/~valvano/
 * @date      August 13, 2023
 */
//#ifndef __ADC_H__
//#define __ADC_H__
#include <ti/devices/msp/msp.h>
#include <stdint.h>


/**
 * Initialize 12-bit ADC0 and ADC1 in software-triggered mode to take
 * measurements when the associated function is called.
 * One channel is to be measured
 * @param adc12 is ADC0 or ADC1
 * @param channel is the 0 to 7
 * @param reference is ADCVREF_INT, ADCVREF_EXT, ADCVREF_VDDA
 * @return none
 * @brief  Initialize 12-bit ADC0
 */
void ADC_Init();

void ADC_In(uint32_t* sampleBuffer);
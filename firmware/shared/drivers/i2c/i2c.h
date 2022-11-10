/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup i2c
 * @{
 */

/**
 * @file        i2c.h
 *
 * @brief       Provides the API for I2C
 *
 * @author      Matthew Krause
 *
 * @date        2022-11-09
 */

/*****************************************************************************/
#ifndef __I2C_H
#define __I2C_H

#include "platform.h"

typedef enum
{
    I2C_CODEC,
    NUM_I2C
} I2C_Type_t;

void I2C_Init(void);
void I2C_Transmit(I2C_Type_t type,
                  uint8_t address,
                  uint8_t *data,
                  uint32_t size);
void I2C_Receive(I2C_Type_t type,
                 uint8_t *data,
                 uint32_t size);
void I2C_Flush(I2C_Type_t type);

#endif //__I2C_H
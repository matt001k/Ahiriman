/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup spi
 * @{
 */

/**
 * @file        spi.h
 *
 * @brief       Provides the API for GPIO
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-05
 */

/*****************************************************************************/
#ifndef __SPI_H
#define __SPI_H

#include "gpio.h"
#include "platform.h"

typedef enum
{
    SPI_FLASH,
    NUM_SPI,
} SPI_Type_t;

typedef void (*SPI_TxCallback_t)(void);
typedef void (*SPI_RxCallback_t)(void);

void SPI_Init(void);
void SPI_Deinit(void);
void SPI_Transmit(SPI_Type_t type, uint8_t *data, uint32_t size);
void SPI_Receive(SPI_Type_t type, uint8_t *data, uint32_t size);
void SPI_TransmitReceive(SPI_Type_t type, uint8_t *txData,
                         uint8_t *rxData, uint32_t size);
void SPI_AssertCs(GPIO_Pin_t pin);
void SPI_DeassertCs(GPIO_Pin_t pin);
void SPI_RegisterTxCallback(SPI_Type_t type, SPI_TxCallback_t cb);
void SPI_RegisterRxCallback(SPI_Type_t type, SPI_RxCallback_t cb);


#endif

/**@} spi */

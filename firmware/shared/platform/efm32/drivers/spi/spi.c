/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup spi
 * @{
 */

/**
 * @file        spi.c
 *
 * @brief       Provides the API for GPIO
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-05
 */

/*****************************************************************************/
#include "spi.h"
#include "clock.h"
#include "dma.h"
#include "atomic.h"

#include <stddef.h>

#define SPI_FLAG_CLEAR (0U)
#define SPI_FLAG_SET (1U)
#define SPI_BUF_SIZE (256U)
#define SPI_BAUD_CALC(baud, var)                        \
do                                                      \
{                                                       \
    var = Clock_GetFrequency(0U) / (2UL * baud) - 1U;   \
    var = var << 8;                                     \
} while (0U)

#define SPI_FLASH_BAUD (4000000U)

typedef uint32_t spi_Location_t;

typedef struct
{
    USART_TypeDef *spi;
    uint32_t baud;
    spi_Location_t clk;
    spi_Location_t miso;
    spi_Location_t mosi;
    spi_Location_t cs;
    SPI_TxCallback_t txCb;
    DMA_Channel_t txChannel;
    uint8_t txReady;
    DMA_Callback_t txDMACb;
    SPI_RxCallback_t rxCb;
    DMA_Channel_t rxChannel;
    uint8_t rxReady;
    DMA_Callback_t rxDMACb;
} spi_Defs_t;

static void sFlashTxCb(void *arg);
static void sFlashRxCb(void *arg);

static spi_Defs_t sCfg[NUM_SPI] =
{
    [SPI_FLASH] =
    {
        USART2,
        SPI_FLASH_BAUD,
        USART_ROUTELOC0_CLKLOC_LOC1,
        USART_ROUTELOC0_RXLOC_LOC1,
        USART_ROUTELOC0_TXLOC_LOC1,
        USART_ROUTELOC0_CSLOC_LOC1,
        NULL,
        SPI_FLASH_TX_DMA,
        SPI_FLAG_SET,
        sFlashTxCb,
        NULL,
        SPI_FLASH_RX_DMA,
        SPI_FLAG_SET,
        sFlashRxCb,
    }
};

void SPI_Init(void)
{
    uint32_t sClockSelect = CMU_HFPERCLKEN0_USART0;
    uint32_t sClkDiv = 0UL;

    for (uint8_t sIdx = 0U; sIdx < NUM_SPI; sIdx++)
    {
        if(sCfg[sIdx].spi == USART0)
        {
            sClockSelect = CMU_HFPERCLKEN0_USART0;
        }
        else if(sCfg[sIdx].spi == USART1)
        {
            sClockSelect = CMU_HFPERCLKEN0_USART1;
        }
        else if(sCfg[sIdx].spi == USART2)
        {
            sClockSelect = CMU_HFPERCLKEN0_USART2;
        }
        else
        {
            sClockSelect = CMU_HFPERCLKEN0_USART3;
        }
        CMU->HFPERCLKEN0 |= sClockSelect;

        sCfg[sIdx].spi->CTRL = USART_CTRL_SMSDELAY | USART_CTRL_SSSEARLY |
                               USART_CTRL_CLKPHA_SAMPLETRAILING |
                               USART_CTRL_CLKPOL_IDLEHIGH | USART_CTRL_SYNC |
                               USART_CTRL_MSBF;

        sCfg[sIdx].spi->FRAME = USART_FRAME_DATABITS_EIGHT |
                                 USART_FRAME_PARITY_NONE |
                                 USART_FRAME_STOPBITS_DEFAULT;

        SPI_BAUD_CALC(sCfg[sIdx].baud, sClkDiv);
        sCfg[sIdx].spi->CLKDIV = sClkDiv;

        sCfg[sIdx].spi->TIMING = _USART_TIMING_RESETVALUE;

        sCfg[sIdx].spi->ROUTELOC0 = sCfg[sIdx].mosi | sCfg[sIdx].miso |
                                    sCfg[sIdx].clk;
        sCfg[sIdx].spi->ROUTEPEN = USART_ROUTEPEN_RXPEN |
                                   UART_ROUTEPEN_TXPEN |
                                   UART_ROUTEPEN_CLKPEN;
        sCfg[sIdx].spi->CMD = USART_CMD_MASTEREN;
        while (!(sCfg[sIdx].spi->STATUS & USART_STATUS_MASTER)) {};

        sCfg[sIdx].spi->CMD = USART_CMD_RXEN | USART_CMD_TXEN;
        while (!(sCfg[sIdx].spi->STATUS &
               (USART_STATUS_TXENS | USART_STATUS_RXENS))) {};
        
        DMA_RegisterCb(sCfg[sIdx].txChannel, sCfg[sIdx].txDMACb, NULL);
        DMA_RegisterCb(sCfg[sIdx].rxChannel, sCfg[sIdx].rxDMACb, NULL);
    }
}

void SPI_Deinit(void)
{
    uint32_t sClockSelect = CMU_HFPERCLKEN0_USART0;

    for (uint8_t sIdx = 0U; sIdx < NUM_SPI; sIdx--)
    {
        ENTER_CRITICAL();
        sCfg[sIdx].spi->CTRL = 0U;
        sCfg[sIdx].spi->FRAME = 0U;
        sCfg[sIdx].spi->TRIGCTRL = 0U;
        sCfg[sIdx].spi->CLKDIV = 0U;
        sCfg[sIdx].spi->IEN = 0U;
        sCfg[sIdx].spi->IFC = 0U;
        sCfg[sIdx].spi->TIMING = 0U;
        sCfg[sIdx].spi->ROUTEPEN = 0U;
        sCfg[sIdx].spi->ROUTELOC0 = 0U;
        sCfg[sIdx].spi->ROUTELOC1 = 0U;

        /* Disable clock for the peripheral */
        if(sCfg[sIdx].spi == USART0)
        {
            sClockSelect = CMU_HFPERCLKEN0_USART0;
        }
        else if(sCfg[sIdx].spi == USART1)
        {
            sClockSelect = CMU_HFPERCLKEN0_USART1;
        }
        else if(sCfg[sIdx].spi == USART2)
        {
            sClockSelect = CMU_HFPERCLKEN0_USART2;
        }
        else
        {
            sClockSelect = CMU_HFPERCLKEN0_USART3;
        }
        CMU->HFPERCLKEN0 &= ~(sClockSelect);
        EXIT_CRITICAL();
    }
}

void SPI_Transmit(SPI_Type_t type, uint8_t *data, uint32_t size)
{
    uint8_t ready = SPI_FLAG_CLEAR;
    if (type < NUM_SPI && data && size)
    {
        while (ready == SPI_FLAG_CLEAR)
        {
            ENTER_CRITICAL();
            if (sCfg[type].txReady && sCfg[type].rxReady)
            {
                ready = SPI_FLAG_SET;
            }
            EXIT_CRITICAL();
        };
        sCfg[type].spi->CMD = USART_CMD_CLEARTX;
        sCfg[type].txReady = SPI_FLAG_CLEAR;
        size = size < SPI_BUF_SIZE ? size : SPI_BUF_SIZE;
        DMA_StartTransfer(SPI_FLASH_TX_DMA, data, size);
    }
}

void SPI_Receive(SPI_Type_t type, uint8_t *data, uint32_t size);

void SPI_TransmitReceive(SPI_Type_t type, uint8_t *txData,
                         uint8_t *rxData, uint32_t size)
{
    uint8_t ready = SPI_FLAG_CLEAR;
    if (type < NUM_SPI && txData && rxData && size)
    {
        while (ready == SPI_FLAG_CLEAR)
        {
            ENTER_CRITICAL();
            if (sCfg[type].txReady && sCfg[type].rxReady)
            {
                ready = SPI_FLAG_SET;
            }
            EXIT_CRITICAL();
        };
        sCfg[type].txReady = SPI_FLAG_CLEAR;
        sCfg[type].rxReady = SPI_FLAG_CLEAR;
        sCfg[type].spi->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;
        DMA_StartTransfer(sCfg[type].rxChannel, rxData, size);
        DMA_StartTransfer(sCfg[type].txChannel, txData, size);
    }
}

void SPI_RegisterTxCallback(SPI_Type_t type, SPI_TxCallback_t cb)
{
    if (type < NUM_SPI && cb)
    {
        sCfg[type].txCb = cb;
    }
}

void SPI_RegisterRxCallback(SPI_Type_t type, SPI_RxCallback_t cb)
{
    if (type < NUM_SPI && cb)
    {
        sCfg[type].rxCb = cb;
    }
}

void SPI_AssertCs(GPIO_Pin_t pin)
{
    GPIO_Clear(pin);
}

void SPI_DeassertCs(GPIO_Pin_t pin)
{
    GPIO_Set(pin);
}

static void sFlashTxCb(void *arg)
{
    if (sCfg[SPI_FLASH].txCb)
    {
        sCfg[SPI_FLASH].txCb();
    }
    sCfg[SPI_FLASH].txReady = SPI_FLAG_SET;
}
static void sFlashRxCb(void *arg)
{
    if (sCfg[SPI_FLASH].rxCb)
    {
        sCfg[SPI_FLASH].rxCb();
    }
    sCfg[SPI_FLASH].rxReady = SPI_FLAG_SET;
}

/**@} spi */

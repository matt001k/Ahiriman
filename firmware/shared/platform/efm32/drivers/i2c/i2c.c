
/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup i2c
 * @{
 */

/**
 * @file        i2c.c
 *
 * @brief       Provides the API for I2C
 *
 * @author      Matthew Krause
 *
 * @date        2022-11-09
 */

/*****************************************************************************/
#include "i2c.h"
#include "atomic.h"
#include "clock.h"

#define I2C_TX_BUFFER_FULL(t) \
(!(i2c[t].i2c->IF & I2C_IF_TXBL))
#define I2C_TRANSFER_ONGOING(t) \
(!(i2c[t].i2c->IF & (I2C_IF_NACK | I2C_IF_ACK | \
                        I2C_IF_BUSERR | I2C_IF_ARBLOST)))
#define I2C_ERRORS(t) \
(i2c[t].i2c->IF & (I2C_IF_NACK | I2C_IF_BUSERR | I2C_IF_ARBLOST))
#define I2C_BAUD_CALC(baud, var, N)                                         \
do                                                                          \
{                                                                           \
    var = (((Clock_GetFrequency(0U) / baud) - 8U) / i2c_NSum_LUT[N]) - 1U;  \
} while (0U)

#define I2C_CODEC_BAUD (100000U)

typedef uint32_t i2c_Location_t;

typedef enum
{
    CLHR_DEFAULT,
    CLHR_ASYMMETRIC,
    CLHR_FAST,
} i2c_CLHR_t;

typedef struct
{
    I2C_TypeDef *i2c;
    uint32_t baud;
    i2c_Location_t scl;
    i2c_Location_t sda;
} i2c_t;

static const uint8_t i2c_NSum_LUT[] =
{
    [CLHR_DEFAULT] = 4U + 4U,
    [CLHR_ASYMMETRIC] = 6U + 3U,
    [CLHR_FAST] = 11U + 6U,
};

static i2c_t i2c[NUM_I2C] =
{
    [I2C_CODEC] =
    {
        I2C0,
        I2C_CODEC_BAUD,
        I2C_ROUTELOC0_SCLLOC_LOC1,
        I2C_ROUTELOC0_SDALOC_LOC1,
    },
};

void I2C_Init(void)
{
    uint32_t iClockSelect = CMU_HFPERCLKEN0_I2C0;
    for (uint8_t iIdx = 0U; iIdx < NUM_I2C; iIdx++)
    {
        if (i2c[iIdx].i2c == I2C0)
        {
            iClockSelect = CMU_HFPERCLKEN0_I2C0;
        }
        else
        {
            iClockSelect = CMU_HFPERCLKEN0_I2C1;
        }
        CMU->HFPERCLKEN0 |= iClockSelect;
        i2c[iIdx].i2c->ROUTEPEN = I2C_ROUTEPEN_SCLPEN | I2C_ROUTEPEN_SDAPEN;
        i2c[iIdx].i2c->ROUTELOC0 = i2c[iIdx].scl | i2c[iIdx].sda;
        I2C_BAUD_CALC(i2c[iIdx].baud, i2c[iIdx].i2c->CLKDIV, CLHR_DEFAULT);
        i2c[iIdx].i2c->CTRL = I2C_CTRL_CLHR_DEFAULT | I2C_CTRL_AUTOSN |
                               I2C_CTRL_AUTOSE | I2C_CTRL_EN;
        i2c[iIdx].i2c->CMD = I2C_CMD_ABORT;
    }
}

void I2C_Transmit(I2C_Type_t type,
                  uint8_t address,
                  uint8_t *data,
                  uint32_t size)
{

    if (!(i2c[type].i2c->IF & (I2C_IF_BUSERR | I2C_IF_ARBLOST)))
    {
        #define I2C_IF_MASK                        0x001FFFFFUL
        i2c[type].i2c->CMD = I2C_CMD_CLEARPC | I2C_CMD_CLEARTX;
        while (i2c[type].i2c->STATUS & I2C_STATUS_RXDATAV)
        {
            i2c[type].i2c->RXDATA;
        }
        i2c[type].i2c->IFC = I2C_IF_MASK;
        while(I2C_TX_BUFFER_FULL(type)) {};
        i2c[type].i2c->TXDATA = address << 1U;
        i2c[type].i2c->CMD = I2C_CMD_START;
        while (I2C_TRANSFER_ONGOING(type)) {};
        if (I2C_ERRORS(type))
        {
            i2c[type].i2c->CMD = I2C_CMD_STOP;
        }
        else
        {
            while (size--)
            {
                while(I2C_TX_BUFFER_FULL(type)) {};
                i2c[type].i2c->TXDATA = *data++;
                while (I2C_TRANSFER_ONGOING(type)) {};
                if (I2C_ERRORS(type))
                {
                    break;
                }
            }
            i2c[type].i2c->CMD = I2C_CMD_STOP;
        }
    }
    if (I2C_ERRORS(type))
    {
        i2c[type].i2c->IFC = I2C_IFC_BUSERR | I2C_IFC_ARBLOST | I2C_IFC_NACK;
    }
}

void I2C_Receive(I2C_Type_t type,
                 uint8_t *data,
                 uint32_t size);

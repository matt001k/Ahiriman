/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup flash
 * @{
 */

/**
 * @file        ext_flash.c
 *
 * @brief       Provides the API for Flash Storage
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-05
 */

/*****************************************************************************/
#include "ext_flash.h"
#include "GD25Q80C.h"
#include "delay.h"
#include "spi.h"

#define FLASH_BUF_SIZE (GD25Q80C_PAGE_SIZE + GD25Q80C_CMD_SIZE + \
                        GD25Q80C_ADDRESS_SIZE + GD25Q80C_DUMMY_SIZE)

typedef struct
{
    Flash_State_t tx : 1U;
    Flash_State_t rx : 1U;
} flash_Done_t;


static void spi_TxDone(void);
static void spi_RxDone(void);
static GD25Q80C_Status_t spi_Transfer(GD25Q80C_Data_t data);

static uint8_t rxBuf[FLASH_BUF_SIZE] = {0U};
static uint8_t txBuf[FLASH_BUF_SIZE] = {0U};
static GD25Q80C_Inst_t gInst = {0U};
static flash_Done_t fDone = {FLASH_TRANSFER_STATE_DONE};

void Ext_FlashInit(void)
{
    GD25Q80C_Config_t cfg =
    {
        spi_Transfer,
        Delay_Us,
    };

    GD25Q80C_Init(&gInst, cfg);

    SPI_RegisterTxCallback(SPI_FLASH, spi_TxDone);
    SPI_RegisterRxCallback(SPI_FLASH, spi_RxDone);
}

void Ext_FlashWrite(uint16_t sector, uint16_t offset, 
                    uint8_t *data, uint16_t size)
{
    uint32_t length = 0U;
    fDone.tx = FLASH_TRANSFER_STATE_IN_PROGRESS;

    while (size)
    {
        length = GD25Q80C_PAGE_SIZE - (offset % GD25Q80C_PAGE_SIZE);
        if (length > size)
        {
            length = size;
        }
        GD25Q80C_Program(&gInst, sector * GD25Q80C_SECTOR_SIZE + offset, data, length);
        offset += length;
        if (offset >= GD25Q80C_SECTOR_SIZE)
        {
            offset = 0U;
            sector++;
        }
        size -= length;
        data += length;
    }
}

void Ext_FlashStartRead(uint16_t sector, uint16_t offset, uint16_t size)
{
    fDone.rx = FLASH_TRANSFER_STATE_IN_PROGRESS;
    GD25Q80C_Read(&gInst, sector * GD25Q80C_SECTOR_SIZE + offset, rxBuf, size);
}

void Ext_FlashGetRead(uint8_t *data, uint16_t size)
{
    memcpy(data, rxBuf + GD25Q80C_CMD_READ_LENGTH, size);
}

Flash_State_t Ext_FlashGetState(void)
{
    Flash_State_t state = FLASH_TRANSFER_STATE_IN_PROGRESS;

    if (fDone.rx == FLASH_TRANSFER_STATE_DONE &&
        fDone.tx == FLASH_TRANSFER_STATE_DONE)
    {
        state = FLASH_TRANSFER_STATE_DONE;
    }

    return state;
}

void Ext_FlashEraseSector(uint16_t sector)
{
    GD25Q80C_EraseSector(&gInst, sector);
}

static void spi_TxDone(void)
{
    fDone.tx = FLASH_TRANSFER_STATE_DONE;
}

static void spi_RxDone(void)
{
    fDone.rx = FLASH_TRANSFER_STATE_DONE;
    GD25Q80C_TransferComplete(&gInst);
    SPI_DeassertCs(FLASH_CS);
}

static GD25Q80C_Status_t spi_Transfer(GD25Q80C_Data_t data)
{
    GD25Q80C_Status_t status = GD25Q80C_OK;
    uint8_t *rxPtr = rxBuf;

    memcpy(txBuf, data.cmd, data.cSize);
    if (data.tx)
    {
        memcpy(txBuf + data.cSize, data.tx, data.dSize);
    }
    else if (data.dSize)
    {
        memset(txBuf + data.cSize, 0xFF, data.dSize);
    }

    if (data.cmd[0U] == GD25Q80C_CMD_READ_STATUS)
    {
        rxPtr = data.rx;
    }

    SPI_AssertCs(FLASH_CS);
    SPI_TransmitReceive(SPI_FLASH, txBuf, rxPtr, data.cSize + data.dSize);
    
    return status;
}

/**@} flash */
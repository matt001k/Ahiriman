/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup command
 * @{
 */

/**************************************************************************//**
 * @file        command.c
 *
 * @brief       Provides an interface for the bootloader to send commands to
 *              and receive commands from the device loading the new
 *              application
 * 
 * @author      Matthew Krause
 *
 * @date        2022-09-25
 *****************************************************************************/
#include "command.h"
#include "dict.h"
#include "serial.h"

#define COMMAND_SIZE BL_SIZEOF(Dict_Item_t)

BL_STATIC BL_BOOL cReady = BL_FALSE;
BL_STATIC BL_BOOL cCb_Set = BL_FALSE;

BL_STATIC BL_CONST Dict_Item_t tList[TRANSMIT_NUM_COMMAND] =
{
    [TRANSMIT_READY] = BL_READY,
    [TRANSMIT_ERROR] = BL_ERROR,
};

BL_STATIC BL_CONST Dict_Item_t rList[RECEIVE_NUM_COMMAND] =
{
    [RECEIVE_READY] = BL_READY,
    [RECEIVE_ERROR] = BL_ERROR,
    [RECEIVE_WRITE] = BL_WRITE,
    [RECEIVE_DATA] = BL_DATA,
    [RECEIVE_VALIDATE] = BL_VALIDATE,
    [RECEIVE_ERASE] = BL_ERASE,
    [RECEIVE_LOCK] = BL_LOCK,
    [RECEIVE_UNLOCK] = BL_UNLOCK,
    [RECEIVE_RELEASE] = BL_RELEASE_PORT,
};

BL_STATIC void command_Cb(BL_UINT32_T length);

BL_Err_t Command_Init(void)
{
    BL_Err_t err = BL_OK;

    if (cCb_Set == BL_FALSE)
    {
        cCb_Set = BL_TRUE;
        err = Serial_RegisterCb(command_Cb);
    }

    return err; 
}

BL_Err_t Command_Deinit(void)
{
    BL_Err_t err = BL_OK;

    if (cCb_Set == BL_TRUE)
    {
        cCb_Set = BL_FALSE;
        err = Serial_DeregisterCb();
    }

    return err;
}

BL_Err_t Command_Send(Command_Transmit_e cmd)
{
    BL_Err_t err = BL_EINVAL;
    uint8_t buf[COMMAND_SIZE] = {0U};
    BL_INT8_T bIdx = COMMAND_SIZE - 1U;
    BL_UINT32_T tTemp = 0U;

    if (cmd < TRANSMIT_NUM_COMMAND)
    {
        tTemp = tList[cmd];
        buf[bIdx--] = (BL_UINT8_T) (tTemp);
        for (; bIdx >= 0; --bIdx)
        {
            buf[bIdx] = (BL_UINT8_T) (tTemp >>= 8U);
        }
        err = Serial_Transmit(buf, COMMAND_SIZE);
    }

    return err;
}

BL_Err_t Command_Receive(Command_Receive_e *cmd)
{
    BL_Err_t err = BL_ENODATA;
    BL_UINT8_T buf[COMMAND_SIZE] = {0U};
    Dict_Item_t item = 0U;

    if (cReady == BL_TRUE)
    {
        cReady = BL_FALSE;
        Serial_Receive(buf, COMMAND_SIZE);
        item = (Dict_Item_t) (buf[0] << 24U |
                              buf[1] << 16U |
                              buf[2] << 8U |
                              buf[3]);
        err = BL_ENOMSG;

        for (uint8_t cIdx = 0U; cIdx < RECEIVE_NUM_COMMAND; cIdx++)
        {
            if (rList[cIdx] == item)
            {
                *cmd = (Command_Receive_e) cIdx;
                err = BL_OK;
                break;
            }
        }
    }

    return err;
}

BL_STATIC void command_Cb(BL_UINT32_T length)
{
    BL_STATIC BL_UINT8_T cnt = 0U;

    cnt += length;

    if (cnt >= COMMAND_SIZE)
    {
        cReady = BL_TRUE;
        cnt = 0U;
    }
}

/**@} command */

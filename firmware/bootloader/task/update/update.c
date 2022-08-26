/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup update
 * @{
 */

/**************************************************************************//**
 * @file        update.c
 *
 * @brief       Task to update the application
 * 
 * @author      Matthew Krause
 *
 * @date        2022-09-30
 *****************************************************************************/
#include "update.h"
#include "data.h"
#include "command.h"
#include "schedule.h"
#include "serial.h"
#include "helper.h"

#define UPDATE_TASK_PERIOD_MS (10U)
#define ACK_READY() Command_Send(TRANSMIT_READY)

typedef enum
{
    COMMAND = 0U,
    DATA,
} update_State_e;

typedef enum
{
    D_INIT = 0u,
    D_LENGTH,
    D_DATA,
} data_State_e;

BL_STATIC Schedule_Node_t uNode = {0U};

BL_STATIC void update_Run(void);
BL_STATIC update_State_e command_Handler(Command_Receive_e command);
BL_STATIC update_State_e data_Handler(void);

BL_Err_t Update_Init(void)
{
    BL_Err_t err = BL_OK;

    err = Schedule_Add(&uNode, UPDATE_TASK_PERIOD_MS, update_Run);

    return err;
}

BL_STATIC void update_Run(void)
{
    BL_STATIC update_State_e state = COMMAND;
    Command_Receive_e cmd = RECEIVE_READY;

    switch (state)
    {
    case COMMAND:
        Command_Init();
        if (Command_Receive(&cmd) == BL_OK)
        {
            state = command_Handler(cmd);
        }
        break;
    case DATA:
        data_Handler();
        break;
    default:
        break;
    }
}

BL_STATIC update_State_e command_Handler(Command_Receive_e cmd)
{
    update_State_e state = COMMAND;

    switch (cmd)
    {
    case RECEIVE_WRITE:
        Command_Deinit();
        state = DATA;
        break;
    case RECEIVE_VALIDATE:
        ACK_READY();
        break;
    case RECEIVE_ERASE:
        ACK_READY();
        break;
    case RECEIVE_LOCK:
        break;
    case RECEIVE_UNLOCK:
        break;
    case RECEIVE_RELEASE:
        Serial_Unlock();
        break;

    default:
        break;
    }

    return state;
}

BL_STATIC update_State_e data_Handler(void)
{
    BL_STATIC data_State_e dState = D_INIT;
    BL_STATIC DataLength_t length = 0U;
    BL_STATIC BL_UINT8_T buf[BL_BUFFER_SIZE] = {0U};
    update_State_e uState = DATA;

    switch (dState)
    {
    case D_INIT:
        Data_LengthCbInit();
        ACK_READY();
        dState = D_LENGTH;
        break;
    case D_LENGTH:
        if (Data_GetLength(&length) == BL_OK)
        {
            Data_SetLength(length);
            Data_LengthCbDeinit();
            Data_DataCbInit();
            ACK_READY();
            dState = D_DATA;
        }
        break;
    case D_DATA:
        if (Data_ReceiveData(buf) == BL_OK)
        {
            Data_LengthCbDeinit();
            ACK_READY();
            dState = D_INIT;
            length = 0U;
            MEMSET(buf, 0U, BL_BUFFER_SIZE);
            uState = COMMAND;
        }
    default:
        break;
    }

    return uState;
}


/**@} update */

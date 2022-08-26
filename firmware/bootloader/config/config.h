/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup config
 * @{
 */

/**
 * @file        config.h
 *
 * @brief       Configuration file necessary for bootloader
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-14
 */

/*****************************************************************************/
#ifndef __CONFIG_H
#define __CONFIG_H

#include "platform.h"
#include "gpio.h"
#include "time.h"
#include "uart.h"
#include "watchdog.h"

/**************************************************************************//**
 * @brief Definitions of various types within the bootloader in accordance to
 *        system requirements
 * 
 * @details All of these must be defined in order to use the bootloader
 *****************************************************************************/
#define BL_UINT8_T uint8_t
#define BL_UINT16_T uint16_t
#define BL_UINT32_T uint32_t
#define BL_UINT64_T uint64_t

#define BL_INT8_T int8_t
#define BL_INT16_T int16_t
#define BL_INT32_T int32_t
#define BL_INT64_T int64_t

#define BL_BOOL bool
#define BL_TRUE true
#define BL_FALSE false

#define BL_STATIC static
#define BL_INLINE inline
#define BL_CONST const

#define BL_SIZEOF sizeof

#define BL_NULL NULL

typedef enum
{
    BL_OK       = 0U,
    BL_ERR      = 1U,
    BL_ENOENT   = 2U,
    BL_EIO      = 5U,
    EL_ENXIO    = 6U,
    BL_ENOMEM   = 12U,
    BL_EACCES   = 13U,
    BL_EBUSY    = 16U,
    BL_ENODEV   = 19U,
    BL_EINVAL   = 22U,
    BL_ENOSYS   = 38U,
    BL_ENOMSG   = 41U,
    BL_ENODATA  = 61U,
} BL_Err_t;

#define BL_BUFFER_SIZE (1024U)

/**************************************************************************//**
 * @brief Abstractions for Necessary Functions
 * 
 * @details This ensures that the necessary function abstractions will be
 *          compatible with the API being ported.
 *****************************************************************************/
BL_STATIC BL_INLINE void UART_TransmitAbstract(BL_UINT8_T *data,
                                               BL_UINT32_T length);
BL_STATIC BL_INLINE void UART_RegisterCbAbstract(void (*cb)(BL_UINT8_T *data,
                                                 BL_UINT32_T length));
BL_STATIC BL_INLINE void UART_DeregisterCbAbstract(void);

BL_STATIC BL_INLINE void UART_TransmitAbstract(BL_UINT8_T *data,
                                               BL_UINT32_T length)
{
    UART_Transmit(UART_DEBUG, data, length);
}

BL_STATIC BL_INLINE void UART_RegisterCbAbstract(void (*cb)(BL_UINT8_T *data,
                                                 BL_UINT32_T length))
{
    UART_RegisterRxCallback(UART_DEBUG, cb);
}

BL_STATIC BL_INLINE void UART_DeregisterCbAbstract(void)
{
    UART_DeregisterRxCallback(UART_DEBUG);
}

BL_STATIC BL_INLINE void LED1_Toggle(void);
BL_STATIC BL_INLINE void LED2_Toggle(void);
BL_STATIC BL_INLINE void LED3_Toggle(void);

BL_STATIC BL_INLINE void LED1_Toggle(void)
{
    GPIO_Toggle(LED1);
}

BL_STATIC BL_INLINE void LED2_Toggle(void)
{
    GPIO_Toggle(LED2);
}

BL_STATIC BL_INLINE void LED3_Toggle(void)
{
    GPIO_Toggle(LED3);
}

BL_STATIC BL_INLINE void WDT_InitAbstract(void);

BL_STATIC BL_INLINE void WDT_InitAbstract(void)
{
    Watchdog_Init();
    Watchdog_Enable();
}

/**************************************************************************//**
 * @brief Configuration Entries for Serial Peripherals
 * 
 * @details This configuration includes all of the serial peripherals that are
 *          planned on being utilized within the bootloader. Serial peripherals
 *          include anything that may speak serially, such as SPI, UART, I2C,
 *          CAN, LIN, etc. The correct format of an entry is as follows:
 * 
 *          SERIAL_ENTRY(name, index, init, transmit, register, deregister)
 *
 *          @param name name of the serial module, this is text not a string
 *          @param index index of the entry, this starts at zero
 *          @param init initialization function pointer in the format of:
 *
 *                  void init(void)
 * 
 *          @param transmit transmit function pointer to transmit data in the
 *                          format of:
 * 
 *                  void transmit(BL_UINT8_T *data, BL_UINT32_T length)
 * 
 *          @param register register function pointer to register a receive
 *                          interrupt in the format of:
 * 
 *                  void register(void (*cb)(BL_UINT8_T *data,
 *                                BL_UINT32_T length))
 *
 *          @param deregister deregister function pointer to deregister a
 *                            receive interrupt in the format of:
 * 
 *                  void deregister(void)
 * 
 *          The serial abstraction will lock onto the first peripheral to take
 *          ahold of the bus. Once that has been established another peripheral
 *          cannot take control unless a command is sent to the device to
 *          unlock the peripheral.
 *****************************************************************************/
#define SERIAL_CFG                              \
    SERIAL_ENTRY(UART0,                         \
                 0U,                            \
                 UART_Init,                     \
                 UART_TransmitAbstract,         \
                 UART_RegisterCbAbstract,       \
                 UART_DeregisterCbAbstract)     \

/**************************************************************************//**
 * @brief Configuration Entry for Systick Peripheral
 * 
 * @details This configuration is used for the systick timer which will be used
 *          for handling certain events through the bootloader process. Only
 *          one systick timer may be entered, if multiple timer's are
 *          entered, the system will throw an assertion. The correct format
 *          of an entry looks as follows:
 * 
 *          SYSTICK_ENTRY(init, ms)
 * 
 *          @param init initialization function pointer in the format of:
 *
 *                 void init(void)
 *
 *          @param ms function pointer which obtains the current runtime of the
 *                    system in milliseconds in the format of:
 * 
 *                 BL_UINT32_T ms(void)
 * 
 *****************************************************************************/
#define SYSTICK_CFG                             \
    SYSTICK_ENTRY(Time_Init, Time_GetRuntimeMs)

/**************************************************************************//**
 * @brief Configuration Entry for LED Peripheral
 * 
 * @details This configuration is used for toggling LEDs while the bootloader
 *          is running. There can be up to 255 LEDs used, although it is not
 *          recommended or necessary. LED periods must be no less than 100ms,
 *          they should also be aligned to the nearest 100ms as that is how
 *          often the task for the LED is running, the led's will not
 *          run properly if this is not configured correctly. The correct
 *          format of an entry looks as follows:
 * 
 *          LED_ENTRY(toggle, period)
 * 
 *          @param toggle toggle function pointer in the format of:
 *
 *                 void toggle(void)
 *
 *          @param period period of the led toggling
 *****************************************************************************/
#define LED_CFG                   \
    LED_ENTRY(LED1_Toggle, 2000U) \
    LED_ENTRY(LED2_Toggle, 500U)  \
    LED_ENTRY(LED3_Toggle, 200U)  \

/**************************************************************************//**
 * @brief Configuration Entry for Watchdog Timer
 * 
 * @details This configuration is used for ensuring that the bootloader does
 *          not get stuck in an infinite loop. The WDT will reset the device.
 *          Only one watchdog timer can be utilized, if more than one are
 *          defined, an assertion will be thrown. The correct format of an
 *          entry looks as follows:
 * 
 *          WDT_ENTRY(init, kick)
 * 
 *          @param init initialization function for the WDT, this will include
 *                      API used to start the watchdog as well, the format of
 *                      this function is as follows:
 * 
 *                      void init(void)
 * 
 *          @param kick API to kick the watchdog and keep the bootloader alive
 *                      the format of this function is as follows:
 * 
 *                      void kick(void)
 * 
 *****************************************************************************/
#define WDT_CFG                                \
    WDT_ENTRY(WDT_InitAbstract, Watchdog_Kick) \

#define NVM_CFG \
    NVM_ENTRY(INT,
              Flash_Init,
              Flash_Write,
              Flash_Erase,
              Flash_Read,
              0x800U)
//    NVM_ENTRY(DEV1,
//              )

#endif // __CONFIG_H

/**@} config */
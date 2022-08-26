#include "clock.h"
#include "cpu.h"
#include "delay.h"
#include "dma.h"
#include "flash.h"
#include "gpio.h"
#include "spi.h"
#include "timer.h"
#include "uart.h"
#include "watchdog.h"
#include "systick.h"
#include "serial.h"
#include "led.h"
#include "run.h"
#include "blink.h"
#include "nvm.h"
#include "update.h"
#include "jump.h"
#include "hold.h"
#include "timeout.h"

#include "validator.h"
#include "buffer.h"


#include <stdio.h>

int main(void)
{
    CPU_Init();
    GPIO_Init();
    Flash_Init();
    Timer_Init();
    Delay_Init();
    //UART_Init();
    SPI_Init();
    DMA_Init();
    Watchdog_Init();
    Watchdog_Enable();
    Systick_Init();
    Serial_Init();
    NVM_Init();
    LED_Init();
    Jump_Init();
    Hold_Init();

    Blink_Init();
    Update_Init();
    Timeout_Init();

    GPIO_Set(FLASH_WP);

    BL_Err_t err = BL_OK;
    BL_BOOL_T hold = BL_FALSE;
    Hold_Get(&hold);
    err = Validator_Run(Buffer_Get(), BL_BUFFER_SIZE);

    if (hold == BL_FALSE && err == BL_OK)
    {
        Jump_ToApp();
    }

    while(1)
    {
        Run();
        Watchdog_Kick();
    }
}

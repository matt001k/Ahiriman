#include "platform.h"
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
#include "bootloader.h"

int main(void)
{
    Watchdog_Deinit();
    CPU_Init();
    Clock_Init(12000000);
    GPIO_Init();
    Timer_Init();
    Delay_Init();
    UART_Init();
    SPI_Init();
    DMA_Init();
    Watchdog_Init();
    Watchdog_Enable();

    GPIO_Set(FLASH_WP);

    Bootloader_Get();


    while(1)
    {
        GPIO_Toggle(LED1);
        //for(uint32_t i = 0; i < 1000000; i++);
        Delay_Ms(100U);
        Watchdog_Kick();
    }
}

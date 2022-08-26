add_library(PLATFORM STATIC
    efm32/startup_efm32tg11b.S
    efm32/drivers/atomic/atomic.c
    efm32/drivers/clock/clock.c
    efm32/drivers/delay/delay.c
    efm32/drivers/dma/dma.c
    efm32/drivers/cpu/cpu.c
    efm32/drivers/flash/flash.c
    efm32/drivers/gpio/gpio.c
    efm32/drivers/spi/spi.c
    efm32/drivers/timer/timer.c
    efm32/drivers/uart/uart.c
    efm32/drivers/watchdog/watchdog.c)

target_include_directories(PLATFORM PUBLIC
    efm32/toolchain/include
    efm32)

target_link_libraries(PLATFORM PUBLIC DRIVERS_LIB SHARED_LIB)
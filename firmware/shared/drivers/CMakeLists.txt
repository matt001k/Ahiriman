add_library(DRIVERS_LIB INTERFACE)

target_include_directories(DRIVERS_LIB INTERFACE
    atomic
    clock
    cpu
    delay
    dma
    flash
    gpio
    spi
    timer
    uart
    watchdog)

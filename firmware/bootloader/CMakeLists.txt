add_library(BOOTLOADER STATIC
    abstraction/led/led.c
    abstraction/serial/serial.c
    abstraction/systick/systick.c
    abstraction/wdt/wdt.c
    interface/command/command.c
    interface/data/data.c
    lib/schedule/schedule.c
    main/run/run.c
    task/blink/blink.c
    task/update/update.c)

target_include_directories(BOOTLOADER PUBLIC
    config
    abstraction/led
    abstraction/serial
    abstraction/systick
    abstraction/wdt
    interface/command
    interface/data
    lib/dict
    lib/helper
    lib/schedule
    main/run
    task/blink
    task/update)

target_link_libraries(BOOTLOADER PUBLIC
    PLATFORM
    DRIVERS_LIB
    INTERFACE_LIB)

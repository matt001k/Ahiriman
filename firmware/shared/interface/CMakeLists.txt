add_library(INTERFACE_LIB STATIC
    ext_flash/ext_flash.c
    time/time.c)

target_include_directories(INTERFACE_LIB PUBLIC
    ext_flash
    time)

target_link_libraries(INTERFACE_LIB PUBLIC
    PLATFORM
    CHIP_LIB)
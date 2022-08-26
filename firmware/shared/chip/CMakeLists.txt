add_library(CHIP_LIB STATIC
    GD25Q80C/GD25Q80C.c)

target_include_directories(CHIP_LIB PUBLIC
    GD25Q80C)

target_link_libraries(CHIP_LIB PUBLIC
    PLATFORM_LIB)
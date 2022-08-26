###############################################################################
# Chip specific variables
###############################################################################
set(LINKER_FILE 
    ${CMAKE_SOURCE_DIR}/shared/platform/efm32/toolchain/efm32tg11b.ld)

include(${CMAKE_SOURCE_DIR}/cmake/arm.cmake)
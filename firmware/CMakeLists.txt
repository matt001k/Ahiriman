cmake_minimum_required(VERSION 3.15)
###############################################################################
# CMake Project Setup
###############################################################################
set(CMAKE_SYSTEM_NAME Generic)
project(ahriman)
message("CMAKE_SYSTEM_NAME: ${CMAKE_HOST_SYSTEM_NAME}")

#set(CMAKE_VERBOSE_MAKEFILE ON)

###############################################################################
# CMake Build Variables
###############################################################################
set(COMPILER_SET_UP_FILE shared/platform/efm32/efm32.cmake)
set(PROJECT_TOOLS cmake/tools.cmake)
set(USED_LANGUAGES C ASM)
set(PROJECT_LIBRARIES shared/platform shared/lib shared/drivers
                      shared/chip shared/interface
                      bootloader)
set(PROJECT_EXECUTABLE ${PROJECT_NAME}.elf CACHE INTERNAL "")

###############################################################################
# Determine compiler used and set languages
###############################################################################
include(${COMPILER_SET_UP_FILE})
enable_language(${USED_LANGUAGES})

###############################################################################
# Add Executable and link corresponding libraries
###############################################################################
add_executable(${PROJECT_EXECUTABLE} main.c)

###############################################################################
# Add required libraries
###############################################################################
foreach(LIBRARY ${PROJECT_LIBRARIES})
    add_subdirectory(${CMAKE_SOURCE_DIR}/${LIBRARY}
        ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY})
endforeach()
target_link_libraries(${PROJECT_EXECUTABLE}
    PLATFORM
    DRIVERS_LIB
    SHARED_LIB
    CHIP_LIB
    INTERFACE_LIB
    BOOTLOADER)

###############################################################################
# Run any extra tools that may be required post compilation
###############################################################################
include(${PROJECT_TOOLS})

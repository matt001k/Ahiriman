##################################################################
# Print executable size
##################################################################
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(SIZE_TOOL_NAME ${TOOLCHAIN_LOCATION}/arm-none-eabi-size)
else()
    set(SIZE_TOOL_NAME ${TOOLCHAIN_LOCATION}/arm-none-eabi-size.exe)
endif()
add_custom_command(TARGET ${PROJECT_EXECUTABLE}
        POST_BUILD
        COMMAND ${SIZE_TOOL_NAME} ${PROJECT_EXECUTABLE})

##################################################################
# Create binary/hex file
##################################################################
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(OBJCOPY_TOOL_NAME ${TOOLCHAIN_LOCATION}/arm-none-eabi-objcopy)
else()
    set(OBJCOPY_TOOL_NAME ${TOOLCHAIN_LOCATION}/arm-none-eabi-objcopy.exe)
endif()
add_custom_command(TARGET ${PROJECT_EXECUTABLE}
        POST_BUILD
        COMMAND ${OBJCOPY_TOOL_NAME} -O ihex ${PROJECT_EXECUTABLE} ${PROJECT_NAME}.hex
        COMMAND ${OBJCOPY_TOOL_NAME} -O binary ${PROJECT_EXECUTABLE} ${PROJECT_NAME}.bin)
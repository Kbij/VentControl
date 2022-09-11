cmake_minimum_required(VERSION 3.13)
set(FREERTOS_KERNEL_PATH "/opt/FreeRTOS-Kernel")
#set(FREERTOS_SRC_DIRECTORY "${PICO_SDK_PATH}/lib/tinyusb/lib/FreeRTOS-Kernel") 

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

include(FreeRTOS_Kernel_import.cmake)

project(FreeRtosTest C CXX ASM)
 
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_BOARD "pico_w")

pico_sdk_init()
 
add_executable(${PROJECT_NAME} main.c)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(${PROJECT_NAME})
 
target_link_libraries(${PROJECT_NAME}
        pico_cyw43_arch_lwip_sys_freertos
        pico_stdlib
        FreeRTOS-Kernel-Heap4 # FreeRTOS kernel and dynamic heap
        )

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)


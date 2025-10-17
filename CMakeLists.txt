cmake_minimum_required(VERSION 3.20)
project(LCD_menu C ASM)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/arm-gcc.cmake)
endif()

set(MCU_TYPE STM32F103xB)

file(GLOB_RECURSE SOURCES
    "src/*.c"
    "cmsis/*.c"
    "cmsis/startup_stm32f103xb.s"
    "cmsis/system_stm32f1xx.c"
)

include_directories(
    cmsis
    inc
)

add_executable(${PROJECT_NAME}.elf ${SOURCES})

# Определяем макрос устройства
target_compile_definitions(${PROJECT_NAME}.elf PRIVATE STM32F103xB)

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=cortex-m3
    -mthumb
    -Wall
    -O0
    -g
    -ffreestanding
)

target_link_options(${PROJECT_NAME}.elf PRIVATE
    -T${CMAKE_SOURCE_DIR}/ld/bootloader.ld
    -nostdlib
    -nostartfiles
    -lgcc
)

# Генерация .bin
add_custom_target(${PROJECT_NAME}.bin
    COMMAND arm-none-eabi-objcopy -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    DEPENDS ${PROJECT_NAME}.elf
)

# Прошивка
add_custom_target(flash
    COMMAND st-flash write ${PROJECT_NAME}.bin 0x08000000
    DEPENDS ${PROJECT_NAME}.bin
)

# Размер
add_custom_target(size
    COMMAND arm-none-eabi-size ${PROJECT_NAME}.elf
    DEPENDS ${PROJECT_NAME}.elf
)

# Очистка
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning all build files"
)
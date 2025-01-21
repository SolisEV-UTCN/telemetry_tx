cmake_minimum_required(VERSION 3.18)
project(telemetry_tx LANGUAGES ASM C)

# Compiler and linker options
set(LINKER_SCRIPT "../../dev/linkers/STM32F103RBTX_FLASH.ld")
set(MCPU "-mcpu=cortex-m3")
set(MFLOAT_ABI "-mfloat-abi=soft")
set(RUNTIME_LIBRARY "--specs=nano.specs")
set(RUNTIME_LIBRARY_SYSCALLS "--specs=nosys.specs")

# Configure compiler and linker flags
set(CMAKE_C_FLAGS "${MCPU} -std=gnu11 ${MFLOAT_ABI} ${RUNTIME_LIBRARY} -mthumb -Wall -Werror")
set(CMAKE_EXE_LINKER_FLAGS "-T${LINKER_SCRIPT} ${RUNTIME_LIBRARY_SYSCALLS} -Wl,-Map=test.map -Wl,--gc-sections -static -Wl,--start-group -lc -lm -Wl,--end-group")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp")

# Project configurations
set(CMAKE_EXECUTABLE_SUFFIX ".elf")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CONAN_OPTION_PLATFORM})

include("dev/cmake/stm32f1xx_hal.cmake")

set(PROJECT_INCLUDES
    ${HAL_INCLUDES}
    inc
)
set(PROJECT_SOURCES
    src/startup/startup_stm32f103rbtx.s
    src/startup/syscalls.c
    src/startup/sysmem.c
    src/startup/system_stm32f1xx.c
    src/main.c
    src/stm32f1xx_hal_msp.c
    src/stm32f1xx_it.c
)

# Main executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
include_directories(${PROJECT_INCLUDES})
add_compile_definitions(
    USE_HAL_DRIVER
    STM32F103xB
)

# Link to HAL interface
target_link_libraries(${PROJECT_NAME} PUBLIC ${STM32F1_HAL})

include("dev/cmake/flash.cmake")
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>)

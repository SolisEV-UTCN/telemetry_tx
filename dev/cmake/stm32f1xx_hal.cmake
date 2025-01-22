set(STM32F1_HAL "stm32f1_hal")

# Prefixes
set(CMSIS_CORE_PATH "libs/cmsis")
set(CMSIS_DEVICE_PATH "libs/cmsis-device-f1")
set(HAL_PATH "libs/stm32f1xx-hal-driver")

set(HAL_INCLUDES
    ${CMSIS_CORE_PATH}/Include
    ${CMSIS_DEVICE_PATH}/Include
    ${HAL_PATH}/Inc
    inc/hal
)

# Add HAL library
add_library(${STM32F1_HAL} STATIC
    ${HAL_PATH}/Src/stm32f1xx_hal_can.c
    ${HAL_PATH}/Src/stm32f1xx_hal_cortex.c
    ${HAL_PATH}/Src/stm32f1xx_hal_crc.c
    ${HAL_PATH}/Src/stm32f1xx_hal_dma.c
    ${HAL_PATH}/Src/stm32f1xx_hal_exti.c
    ${HAL_PATH}/Src/stm32f1xx_hal_flash.c
    ${HAL_PATH}/Src/stm32f1xx_hal_gpio.c
    ${HAL_PATH}/Src/stm32f1xx_hal_pwr.c
    ${HAL_PATH}/Src/stm32f1xx_hal_rcc.c
    ${HAL_PATH}/Src/stm32f1xx_hal_uart.c
    ${HAL_PATH}/Src/stm32f1xx_hal_usart.c
    ${HAL_PATH}/Src/stm32f1xx_hal.c
)
target_include_directories(${STM32F1_HAL} PUBLIC ${HAL_INCLUDES})

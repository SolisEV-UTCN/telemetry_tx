#include "main.h"
#include "stm32f1xx_it.h"
#include "stm32f1xx_hal_crc.h"

#define DEBUG_FRAME 0
#define UART_MSG_LEN (16)
#define DMA_UART_MSG_CNT (20)
#define DMA_BUFFER_SIZE (UART_MSG_LEN * DMA_UART_MSG_CNT)
#define CAN_MSG_LEN (8)
#define CRC_LEN (((CAN_MSG_LEN) * (sizeof(uint8_t))) / (sizeof(uint32_t)))

union CanData{
  uint8_t byte[CAN_MSG_LEN];
  uint32_t word[CRC_LEN];
};

static union CanData can_data;
static CAN_RxHeaderTypeDef header;
static uint32_t can_crc = { 0 };
static uint8_t dma_buffer[DMA_BUFFER_SIZE] = { 0 };
static uint8_t dma_offset = 0;
static uint8_t *const p_buffer = (uint8_t *)&dma_buffer;
#if DEBUG_FRAME == 1
static uint32_t frame_count = 0;
static char frames[10] = { 0 };
#endif

extern CAN_HandleTypeDef hcan;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart1;
extern CRC_HandleTypeDef hcrc;

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  while (1) {}
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  while (1) {}
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  while (1) {}
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  while (1) {}
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  while (1) {}
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void) {}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void) {}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void) {}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel4 global interrupt.
  */
void DMA1_Channel4_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
}

/**
  * @brief This function handles USB low priority or CAN RX0 interrupts.
  */
void USB_LP_CAN_RX0_IRQHandler(void)
{
  HAL_CAN_IRQHandler(&hcan);
  HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &header, can_data.byte);

  if (HAL_CRC_GetState(&hcrc) == HAL_CRC_STATE_READY) {
    can_crc = HAL_CRC_Calculate(&hcrc, can_data.word, CRC_LEN);
  }

  // Fill DMA buffer
#if DEBUG_FRAME == 1
  sprintf(frames, "%09u", ++frame_count);
  dma_buffer[UART_MSG_LEN * dma_offset +  0] = 'F';
  dma_buffer[UART_MSG_LEN * dma_offset +  1] = ':';
  dma_buffer[UART_MSG_LEN * dma_offset +  2] = frames[0];
  dma_buffer[UART_MSG_LEN * dma_offset +  3] = frames[1];
  dma_buffer[UART_MSG_LEN * dma_offset +  4] = frames[2];
  dma_buffer[UART_MSG_LEN * dma_offset +  5] = frames[3];
  dma_buffer[UART_MSG_LEN * dma_offset +  6] = frames[4];
  dma_buffer[UART_MSG_LEN * dma_offset +  7] = frames[5];
  dma_buffer[UART_MSG_LEN * dma_offset +  8] = frames[6];
  dma_buffer[UART_MSG_LEN * dma_offset +  9] = frames[7];
  dma_buffer[UART_MSG_LEN * dma_offset + 10] = frames[8];
  dma_buffer[UART_MSG_LEN * dma_offset + 11] = frames[9];
  dma_buffer[UART_MSG_LEN * dma_offset + 12] = 'O';
  dma_buffer[UART_MSG_LEN * dma_offset + 13] = ':';
  dma_buffer[UART_MSG_LEN * dma_offset + 14] = (dma_offset / 10) + 48;
  dma_buffer[UART_MSG_LEN * dma_offset + 15] = (dma_offset % 10) + 48;
#else
  dma_buffer[UART_MSG_LEN * dma_offset +  0] = 0xFE; // Start padding
  dma_buffer[UART_MSG_LEN * dma_offset +  1] = (header.StdId & 0x000000FF) >> 0;
  dma_buffer[UART_MSG_LEN * dma_offset +  2] = (header.StdId & 0x0000FF00) >> 8;
  dma_buffer[UART_MSG_LEN * dma_offset +  3] = can_data.byte[0];
  dma_buffer[UART_MSG_LEN * dma_offset +  4] = can_data.byte[1];
  dma_buffer[UART_MSG_LEN * dma_offset +  5] = can_data.byte[2];
  dma_buffer[UART_MSG_LEN * dma_offset +  6] = can_data.byte[3];
  dma_buffer[UART_MSG_LEN * dma_offset +  7] = can_data.byte[4];
  dma_buffer[UART_MSG_LEN * dma_offset +  8] = can_data.byte[5];
  dma_buffer[UART_MSG_LEN * dma_offset +  9] = can_data.byte[6];
  dma_buffer[UART_MSG_LEN * dma_offset + 10] = can_data.byte[7];
  dma_buffer[UART_MSG_LEN * dma_offset + 11] = (can_crc & 0x000000FF) >> 0;  // CRC-32 CAN data
  dma_buffer[UART_MSG_LEN * dma_offset + 12] = (can_crc & 0x0000FF00) >> 8;  // CRC-32 CAN data
  dma_buffer[UART_MSG_LEN * dma_offset + 13] = (can_crc & 0x00FF0000) >> 16; // CRC-32 CAN data
  dma_buffer[UART_MSG_LEN * dma_offset + 14] = (can_crc & 0xFF000000) >> 24; // CRC-32 CAN data
  dma_buffer[UART_MSG_LEN * dma_offset + 15] = 0x7F; // End padding
#endif
  // Send USART via DMA
  if (++dma_offset == DMA_UART_MSG_CNT) {
    dma_offset = 0;
    HAL_UART_Transmit_DMA(&huart1, p_buffer, DMA_BUFFER_SIZE);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
  }
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart1);
}

#include "port.h"
#include <main.h>
#include <gpio.h>
#include <dma.h>

void peripheral_init(void) {
  MX_GPIO_Init();
  MX_DMA_Init();

  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 4, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
}

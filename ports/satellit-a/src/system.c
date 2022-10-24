#include <dma.h>
#include <gpio.h>
#include <main.h>

#include "port.h"

extern void SystemClock_Config(void);
uint32_t get_tick_ms(void) { return HAL_GetTick(); }

void peripheral_init(void) {
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();

  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 4, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
}

void peripheral_reset(void) {
  HAL_DeInit();

  /* Reset the RCC clock configuration to the default reset state ------------*/
  /* Set HSION bit */
  RCC->CR |= (uint32_t)0x00000001;

  /* Reset CFGR register */
  RCC->CFGR = 0x00000000;

  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t)0xFEF6FFFF;

  /* Reset PLLCFGR register */
  RCC->PLLCFGR = 0x24003010;

  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* Disable all interrupts */
  RCC->CIR = 0x00000000;

  /* Reset NVIC */
  int cnt = sizeof(NVIC->ICPR) >> 2;  // 8 for stm32f4xx
  for (int i = 0; i < cnt; i++) {
    NVIC->ICER[i] = ~0;
    NVIC->ICPR[i] = ~0;
  }
  cnt = sizeof(NVIC->IP);  // 240 for stm32f4xx
  for (int i = 0; i < cnt; i++) {
    NVIC->IP[i] = 0;
  }
}

int erase_flash(unsigned int offset, unsigned int len) { return -1; }

int write_flash(unsigned int offset, char *buff, unsigned int len) {
  return -1;
}

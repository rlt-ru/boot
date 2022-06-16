#include "port.h"

void peripheral_reset(void)
{
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
  int cnt = sizeof(NVIC->ICPR) >> 2;//8 for stm32f4xx
  for(int i = 0; i < cnt; i++){
    NVIC->ICER[i] = ~0;
    NVIC->ICPR[i] = ~0;
  }
  cnt = sizeof(NVIC->IP);//240 for stm32f4xx
  for(int i = 0; i < cnt; i++){
    NVIC->IP[i] = 0;
  }
}

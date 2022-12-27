#include <gpio.h>
#include <main.h>

#include "port.h"
#include "icache.h"

extern void SystemClock_Config(void);
void PeriphCommonClock_Config(void);

__weak void PeriphCommonClock_Config(void) {
  __NOP();
}

uint32_t get_tick_ms(void) { return HAL_GetTick(); }

void peripheral_init(void) {
  uwTickPrio = TICK_INT_PRIORITY;
  HAL_Init();
  SystemClock_Config();
  PeriphCommonClock_Config();

  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_EnableVddIO2();
  HAL_PWREx_DisableUCPDDeadBattery();
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  HAL_ICACHE_Invalidate();
  MX_ICACHE_Init();
  MX_GPIO_Init();
}

void peripheral_reset(void) {
  HAL_DeInit();
  SysTick->CTRL &= ~(SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk);

  SystemInit();

  /* Reset NVIC */
  int cnt = sizeof(NVIC->ICPR) >> 2;
  for (int i = 0; i < cnt; i++) {
    NVIC->ICER[i] = ~0;
    NVIC->ICPR[i] = ~0;
  }
  cnt = sizeof(NVIC->IPR);
  for (int i = 0; i < cnt; i++) {
    NVIC->IPR[i] = 0;
  }
}

static uint32_t GetPage(uint32_t Addr) {
  uint32_t page = 0;

  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE)) {
    /* Bank 1 */
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  } else {
    /* Bank 2 */
    page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
  }

  return page;
}

int erase_flash(unsigned int offset, unsigned int len) {
  uint32_t PageError;
  HAL_StatusTypeDef flash_ret;
  FLASH_EraseInitTypeDef EraseInitStruct;

  HAL_ICACHE_Disable();

  flash_ret = HAL_FLASH_Unlock();
  if (flash_ret != HAL_OK) {
    return -1;
  }

  unsigned int page = GetPage(APP_BASE_ADDRESS + offset);

  /* Configure flash erase of sector */
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.Banks = FLASH_BANK_1;
  EraseInitStruct.Page = page;
  EraseInitStruct.NbPages = (len / FLASH_PAGE_SIZE) + 1;
  flash_ret = HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);

  HAL_FLASH_Lock();

  HAL_ICACHE_Enable();

  return flash_ret != HAL_OK;
}

int write_flash(unsigned int offset, char *buff, unsigned int len) {
  HAL_StatusTypeDef flash_ret;

  HAL_ICACHE_Disable();

  flash_ret = HAL_FLASH_Unlock();
  if (flash_ret != HAL_OK) {
    return -1;
  }

  unsigned int addr = APP_BASE_ADDRESS + offset;
  while (addr < (APP_BASE_ADDRESS + offset + len)) {
    flash_ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, (uint32_t)(addr),
                                  (uint32_t)buff);
    if (flash_ret != HAL_OK) {
      break;
    }

    addr += 16;
    buff += 16;
  }

  flash_ret = HAL_FLASH_Lock();

  HAL_ICACHE_Enable();

  return 0;
}

void led_on(LED led) {
  switch (led) {
    case LED_GREEN:
      HAL_GPIO_WritePin(LED0G_GPIO_Port, LED0G_Pin, GPIO_PIN_RESET);
      break;
    case LED_RED:
      HAL_GPIO_WritePin(LED0R_GPIO_Port, LED0R_Pin, GPIO_PIN_RESET);
      break;
    default:
      break;
  }
}
void led_off(LED led) {
  switch (led) {
    case LED_GREEN:
      HAL_GPIO_WritePin(LED0G_GPIO_Port, LED0G_Pin, GPIO_PIN_SET);
      break;
    case LED_RED:
      HAL_GPIO_WritePin(LED0R_GPIO_Port, LED0R_Pin, GPIO_PIN_SET);
      break;
    default:
      break;
  }
}

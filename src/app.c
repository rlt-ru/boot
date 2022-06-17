#include "app.h"
#include <port.h>

typedef void (*app)(void);

bool app_valid_start(void) {
  return ((*(__IO uint32_t*)APP_BASE_ADDRESS) & 0x2FFE0000) == 0x20000000 ||
         ((*(__IO uint32_t*)APP_BASE_ADDRESS) & 0x1FFE0000) == 0x10000000;
}

void app_launch(void) {
  uint32_t appAddress;
  app application;

  appAddress = *(__IO uint32_t*)(APP_BASE_ADDRESS + 4);
  application = (app)appAddress;

  __disable_irq();
  {
    peripheral_reset();

    SCB->VTOR = APP_BASE_ADDRESS;
    __DSB();
    __ISB();

    /* Initialize user application's Process and Stack Pointer */
    __set_PSP(*(__IO uint32_t*)APP_BASE_ADDRESS);
    __set_MSP(*(__IO uint32_t*)APP_BASE_ADDRESS);
  }
  __enable_irq();

  /* Jump to application */
  application();
}

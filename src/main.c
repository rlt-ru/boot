#include <dma.h>
#include <ff.h>
#include <gpio.h>
#include <main.h>
#include <port.h>

FATFS fatfs;

extern void SystemClock_Config(void);
static void app_launch(void);
typedef void (*app)(void);

void main(void) {
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();

  if (button_pressed()) {
    if (f_mount(&fatfs, "", 1) != FR_OK) {
    }

    while (1) {
      HAL_Delay(100);
    }
  }

  // start application
  app_launch();
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

  // end application
  HAL_NVIC_SystemReset();
}

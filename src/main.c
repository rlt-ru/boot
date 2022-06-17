#include <main.h>
#include <ff.h>
#include <port.h>
#include "app.h"

FATFS fatfs;

extern void SystemClock_Config(void);

void main(void) {
  HAL_Init();
  SystemClock_Config();

  peripheral_init();

  do {
    if (!button_pressed() && app_valid_start()) break;

    if (f_mount(&fatfs, "", 1) != FR_OK) break;
    while (1) {
      HAL_Delay(100);
    }
  } while (0);

  // start application
  app_launch();
  // end application
  HAL_NVIC_SystemReset();
}

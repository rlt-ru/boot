#include <ff.h>
#include <main.h>
#include <port.h>

#include "app.h"

FATFS fatfs;

extern void SystemClock_Config(void);

typedef struct {
  union {
    uint32_t value;
    struct {
      uint32_t button_pressed : 1;
      uint32_t app_valid : 1;
      uint32_t fatfs_ready : 1;
      uint32_t updated : 1;
    } b;
  } flags;
} boot_state_t;

boot_state_t boot_state;

void main(void) {
  HAL_Init();
  SystemClock_Config();

  peripheral_init();

  boot_state.flags.value = 0;

  do {
    boot_state.flags.b.button_pressed = button_pressed() ? 1 : 0;
    boot_state.flags.b.app_valid = app_valid_start() ? 1 : 0;
    boot_state.flags.b.fatfs_ready = (FR_OK == f_mount(&fatfs, "", 1)) ? 1 : 0;

    if (!boot_state.flags.b.button_pressed) {
      if (boot_state.flags.b.fatfs_ready) {
        boot_state.flags.b.updated = app_update() ? 0 : 1;
      }

      if (boot_state.flags.b.app_valid) {
        break;
      }
    }

    while (1) {
    }
  } while (0);

  // start application
  app_launch();
  // end application
  HAL_NVIC_SystemReset();
}

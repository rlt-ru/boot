#include <ff.h>
#include <main.h>
#include <port.h>

#include "app.h"

FATFS fatfs;

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
  peripheral_init();

  led_on(LED_RED);

  boot_state.flags.value = 0;

  do {
    led_strob_set(4);
    int cnt = 0;
    for(int i = 0; i < 100; i++) {
      led_toggle(LED_RED);
      cnt += button_pressed() ? 1 : 0;
      HAL_Delay(20);
    }
    boot_state.flags.b.button_pressed = cnt > 10 ? 1 : 0;
    boot_state.flags.b.app_valid = app_valid_start() ? 1 : 0;
    boot_state.flags.b.fatfs_ready = (FR_OK == f_mount(&fatfs, "", 1)) ? 1 : 0;

    if (!boot_state.flags.b.button_pressed) {
      if (boot_state.flags.b.fatfs_ready) {
        boot_state.flags.b.updated = app_update() ? 0 : 1;
        led_off(LED_GREEN);
        led_on(LED_RED);
        if(boot_state.flags.b.updated) {
          boot_state.flags.b.app_valid = app_valid_start() ? 1 : 0;
        }
      }

      if (boot_state.flags.b.app_valid) {
        break;
      }
    }

    led_off(LED_RED);
    led_strob_set(0);

    while (1) {
      led_toggle(LED_GREEN);
      HAL_Delay(1000);
    }
  } while (0);

  // start application
  app_launch();
  // end application
  HAL_NVIC_SystemReset();
}

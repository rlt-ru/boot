#include "app.h"
#include <stdio.h>
#include <string.h>
#include "ff.h"
#include "crc_posix.h"

typedef void (*app)(void);

uint32_t led_strob = 1;
void led_strob_set(uint32_t strob) {
  led_strob = strob;
}
void led_toggle(LED led) {
  static LED _l[LED_COUNT] = {0};
  static uint32_t strob = 0;

  if(strob) {
    strob--;
  }

  if(!strob) {
    if(_l[led]) {
      _l[led] = 0;
      led_off(led);
    } else {
      _l[led] = 1;
      led_on(led);
    }
    strob = led_strob;
  }
}

bool app_valid_start(void) {
  uint32_t addr = *(__IO uint32_t *)APP_BASE_ADDRESS;
  return (addr & 0x20000000) == 0x20000000 ||
         (addr & 0x10000000) == 0x10000000;
}

void app_launch(void) {
  uint32_t appAddress;
  app application;

  appAddress = *(__IO uint32_t *)(APP_BASE_ADDRESS + 4);
  application = (app)appAddress;

  __disable_irq();
  {
//    peripheral_reset();

    SCB->VTOR = APP_BASE_ADDRESS;
    __DSB();
    __ISB();

    /* Initialize user application's Process and Stack Pointer */
    __set_PSP(*(__IO uint32_t *)APP_BASE_ADDRESS);
    __set_MSP(*(__IO uint32_t *)APP_BASE_ADDRESS);
  }
  __enable_irq();

  /* Jump to application */
  application();
}

int app_read_line(FIL *f, char *buff, unsigned int size) {
  unsigned int readed = 0;
  unsigned int idx = 0;
  char ch;
  while (idx < (size - 1)) {
    readed = 0;
    f_read(f, &ch, 1, &readed);
    if (readed != 1) {
      break;
    }
    if (ch == '\r') {
      continue;
    }
    if (ch == '\n') {
      break;
    }
    *buff = ch;

    idx++;
    buff++;
  }
  *buff = 0;
  return idx;
}

int app_check_file(FIL *f) {
  f_lseek(f, 0);

  char params[256];
  int p = app_read_line(f, params, sizeof(params));

  unsigned long crc;
  if(sscanf(params, "CRC:%lu", &crc) != 1) {
    return -1;
  }

  led_strob = f_size(f) / 100;

  unsigned int readed = 0;
  unsigned long crc_acc = 0;
  size_t cnt = 0;
  char ch;
  while (1) {
    led_toggle(LED_GREEN);
    readed = 0;
    if(f_read(f, &ch, 1, &readed) != FR_OK) {
      return -1;
    }

    if (readed == 0) {
      break;
    }
    crc_acc = streamcrc_acc(crc_acc, ch);
    cnt++;
  }
  crc_acc = streamcrc_end(crc_acc, cnt);

  if(crc != crc_acc) {
    return -1;
  }

  f_lseek(f, 0);
  app_read_line(f, params, sizeof(params));
  return 0;
}

int app_flash_file(FIL *f) {
  static char buff[512];
  unsigned int offset = 0;

  led_strob = 20;

  int result = -1;
  while (1) {
    unsigned int readed = 0;
    if (f_read(f, buff, sizeof(buff), &readed) != FR_OK) {
      // err
      result = -1;
      break;
    }

    if (readed == 0) {
      // end
      result = 0;
      break;
    }

    led_toggle(LED_GREEN);

    // flash
    if (write_flash(offset, buff, readed)) {
      // err
      result = -1;
      break;
    }

    if (readed < sizeof(buff)) {
      // end
      result = 0;
      break;
    }

    offset += readed;
  }
  return result;
}

int app_update(void) {
  FRESULT fr;
  DIR dj;
  FILINFO fno;
  uint8_t file_name1[FF_MAX_LFN + 1];
  uint8_t file_name2[FF_MAX_LFN - 5 + 1];
  fr = f_findfirst(&dj, &fno, "firmware", "*.fpk");
  if (fr == FR_OK && fno.fname[0]) {
    memset(file_name1, 0, sizeof(file_name1));
    memset(file_name2, 0, sizeof(file_name2));

    uint8_t suffix[] = ".last";
    sprintf((char *) file_name1, "firmware/%s", fno.fname);
    sprintf((char *) file_name2, "%s%s", file_name1, suffix);

    FIL f;
    FRESULT result = f_open(&f, file_name1, FA_READ);
    if (result != FR_OK) {
      return -1;
    }

    led_off(LED_RED);
    led_on(LED_GREEN);

    do {
      uint32_t file_size = f_size(&f);

      if (app_check_file(&f)) {
        break;
      }

      if (erase_flash(0, file_size)) {
        break;
      }
      if (app_flash_file(&f)) {
        break;
      }

      f_close(&f);
      f_unlink((const char *)file_name2);
      f_rename(file_name1, file_name2);

      return 0;
    } while (0);

    f_close(&f);
  }
  return -1;
}

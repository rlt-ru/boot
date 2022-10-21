#include "app.h"
#include <stdio.h>
#include <ff.h>
#include <port.h>

#define APP_FIRMWARE_FILE_NAME "FIRMWARE/firmware.fpk"
#define APP_FIRMWARE_FILE_READY "FIRMWARE/firmware.fpk.last"

typedef void (*app)(void);

bool app_valid_start(void) {
  return ((*(__IO uint32_t *)APP_BASE_ADDRESS) & 0x2FFE0000) == 0x20000000 ||
         ((*(__IO uint32_t *)APP_BASE_ADDRESS) & 0x1FFE0000) == 0x10000000;
}

void app_launch(void) {
  uint32_t appAddress;
  app application;

  appAddress = *(__IO uint32_t *)(APP_BASE_ADDRESS + 4);
  application = (app)appAddress;

  __disable_irq();
  {
    peripheral_reset();

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

  uint16_t crc;
  if(scanf("%hu", &crc) != 1) {
    return -1;
  }

  

  return 0;
}

FRESULT app_flash_file(FIL *f) {
  FRESULT result;
  static char buff[512];
  unsigned int offset = 0;
  while (1) {
    unsigned int readed = 0;
    result = f_read(f, buff, sizeof(buff), &readed);

    if (result != FR_OK) {
      // err
      break;
    }

    // flash
    if (write_flash(offset, buff, readed)) {
      // err
      break;
    }

    offset += readed;

    if (readed < sizeof(buff)) {
      // end
      break;
    }
  }
  return result;
}

int app_update(void) {
  FIL f;
  FRESULT result = f_open(&f, APP_FIRMWARE_FILE_NAME, FA_READ);
  if (result != FR_OK) {
    return -1;
  }

  do {
    uint32_t file_size = f_size(&f);

    if (app_check_file(&f)) {
      break;
    }

    if (erase_flash(0, file_size)) {
      break;
    }
    if (app_flash_file(&f) != FR_OK) {
      break;
    }

    f_close(&f);
    // f_rename(APP_FIRMWARE_FILE_NAME, APP_FIRMWARE_FILE_READY);

    return 0;
  } while (0);

  f_close(&f);
  return -1;
}

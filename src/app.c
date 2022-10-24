#include "app.h"
#include <stdio.h>
#include "ff.h"
#include <port.h>
#include "crc_posix.h"

#define APP_FIRMWARE_FILE_NAME "FIRMWARE/firmware.fpk"
#define APP_FIRMWARE_FILE_READY "FIRMWARE/firmware.lst"

typedef void (*app)(void);

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
    peripheral_reset();

    SCB->VTOR = APP_BASE_ADDRESS;
    __DSB();
    __ISB();

    /* Initialize user application's Process and Stack Pointer */
    __set_PSP(*(__IO uint32_t *)APP_BASE_ADDRESS);
    __set_MSP(*(__IO uint32_t *)APP_BASE_ADDRESS);
  }

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

  unsigned int readed = 0;
  unsigned long crc_acc = 0;
  size_t cnt = 0;
  char ch;
  while (1) {
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
    if (app_flash_file(&f)) {
      break;
    }

    f_close(&f);
    f_rename(APP_FIRMWARE_FILE_NAME, APP_FIRMWARE_FILE_READY);

    return 0;
  } while (0);

  f_close(&f);
  return -1;
}

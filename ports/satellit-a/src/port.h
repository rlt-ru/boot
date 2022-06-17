#ifndef SDIO_H
#define SDIO_H

#include <main.h>
#include <stdint.h>
#include <stdbool.h>

#define APP_BASE_ADDRESS 0x08020000
#define ETH_HOSTNAME "bootloader"

uint32_t get_tick_ms(void);
void peripheral_init(void);
void peripheral_reset(void);

bool button_pressed(void);

#endif // SDIO_H

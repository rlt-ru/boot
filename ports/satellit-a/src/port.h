#ifndef SDIO_H
#define SDIO_H

#include <main.h>
#include <stdint.h>
#include <stdbool.h>

#define SDIO_DMA_IRQn DMA2_Stream3_IRQn

#define APP_BASE_ADDRESS 0x08010000

void peripheral_reset(void);
bool button_pressed(void);

#endif // SDIO_H

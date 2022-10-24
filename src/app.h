#ifndef APP_H
#define APP_H

#include <stdint.h>
#include <stdbool.h>
#include "port.h"

bool app_valid_start(void);
void app_launch(void);
int app_update(void);
void led_strob_set(uint32_t strob);
void led_toggle(LED led);

#endif //APP_H

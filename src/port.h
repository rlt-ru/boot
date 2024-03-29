#ifndef PORT_SAT_A10
#define PORT_SAT_A10

#include <main.h>
#include <stdint.h>
#include <stdbool.h>

#define APP_BASE_ADDRESS 0x08020000

typedef enum {
  LED_GREEN,
  LED_RED,
  LED_COUNT
}LED;

uint32_t get_tick_ms(void);
void peripheral_init(void);
void peripheral_reset(void);
void led_on(LED led);
void led_off(LED led);
int erase_flash(unsigned int offset, unsigned int len);
int write_flash(unsigned int offset, char *buff, unsigned int len);
bool button_pressed(void);

#endif // PORT_SAT_A10

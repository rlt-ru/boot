#include "port.h"

bool button_pressed(void){
  return HAL_GPIO_ReadPin(BOOT_GPIO_Port, BOOT_Pin) != GPIO_PIN_RESET;
}

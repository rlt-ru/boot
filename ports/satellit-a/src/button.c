#include "port.h"

bool button_pressed(void){
  return HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin) != GPIO_PIN_RESET;
}

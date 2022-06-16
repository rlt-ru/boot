#include <main.h>
#include <gpio.h>
#include <dma.h>
#include <ff.h>

FATFS fatfs;

extern void SystemClock_Config(void);

void main(void){
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();

  if(f_mount(&fatfs, "", 1) != FR_OK){
    HAL_Delay(100);
  }

  while(1){
    HAL_Delay(100);
  }
}

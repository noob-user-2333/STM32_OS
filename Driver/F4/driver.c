#include "driver.h"






extern void FLASH_INIT();

extern void TIMER_Init();
extern void DMA_init();
extern void MPU_Init();
extern void UART1_init();
void driver_init()
{
	RCC->APB2ENR |= 0x01UL;

	DMA_init();
	

	FLASH_INIT();

//	SPI1_init();
	UART1_init();

	MPU_Init();
	TIMER_Init();
}


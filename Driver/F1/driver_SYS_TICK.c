#include "driver.h"



void sys_tick_enable(unsigned int ms_per_ticks)
{
	SysTick->CTRL = 0x00UL;
	SysTick->LOAD = (CPU_FREQ / 8 / 1000) * ms_per_ticks;
	SysTick->CTRL = 0x07UL;
	SCB->SHP[11] = 0xFF; 

}



void sys_tick_disable()
{
	SysTick->CTRL = 0;
}

void sys_tick_info_get(unsigned int *enable_flag,unsigned int *ms_per_ticks)
{
	*enable_flag = (SysTick->CTRL & 0x01);
	*ms_per_ticks = SysTick->LOAD * 8 * 1000 / CPU_FREQ;
}
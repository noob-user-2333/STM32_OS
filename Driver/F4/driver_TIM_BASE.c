#include "driver.h"





EXE_STATE timer_timing(const TIMER *timer_ptr,unsigned int ms_per_ticks,unsigned int is_circle)
{
	if(timer_ptr->reg_base->CR1 & 1)
		return EXE_DEVICE_BUSYING;

		
		timer_ptr->reg_base->PSC = (CPU_FREQ / 1000 / 10) - 1;
		timer_ptr->reg_base->CNT = ms_per_ticks * 10;
		timer_ptr->reg_base->ARR = ms_per_ticks * 10;
		
		
		timer_ptr->reg_base->DIER = 1;
		
		
		
		if(is_circle)
		{

			timer_ptr->reg_base->CR1 = 0x84;
		}
		else
		{
			timer_ptr->reg_base->CR1 = 0x0C;
		}
		timer_ptr->reg_base->CR1 |= 1;
	
	
	return EXE_SUCCESS;
}





EXE_STATE timer_disable(TIMER *timer_ptr)
{
	timer_ptr->reg_base->CR1 = 0;
////	timer_ptr->ms_per_ticks =  0;
////	timer_ptr->timer_work_type = NO_WORK;
	return EXE_SUCCESS;
}


EXE_STATE timer_state_get(TIMER *timer_ptr,TIMER_TYPE *type,TIMER_WORK *work,unsigned int *ms_per_ticks)
{
	if(timer_ptr == NULL)
		return EXE_PARA_ERROR;
	if(work)
	{
		if(timer_ptr->reg_base->CR1 & 0x01)
		{
			switch(timer_ptr->reg_base->CR1)
			{
				case 0x0D:
				{
					*work = TIMER_TIMING;
					break;
				}
				case 0x85:
				{
					*work = TIMER_TIMING_CIRCLE;
				}
				default:
				{
					return EXE_UNKNOW_ERROR;
				}
			}
		}
		else
		{
			*work = TIMER_NO_WORK;
		}
	}
	
	if(type)
	{
		*type = timer_ptr->timer_type;
	}
	if(ms_per_ticks)
	{
		*ms_per_ticks = timer_ptr->reg_base->ARR / 10;
	}
	
	return EXE_SUCCESS;
}


const TIMER timer[2] ={	[0].name = "TIM6",[1].name = "TIM7",
												[0].reg_base = TIM6,[1].reg_base = TIM7};


void TIMER_Init()
{
//

//	timer[0].timer_type = BASIC_TIMER;
//	timer[1].timer_type = BASIC_TIMER;

//	timer[0].timer_work_type = NO_WORK;
//	timer[1].timer_work_type = NO_WORK;
//	
//	
//	timer[0].ms_per_ticks = 0;
//	timer[1].ms_per_ticks = 0;
//	
//	timer[0].event_func = NULL;
//	timer[1].event_func = NULL;

//	timer[0].input_para = 0;
//	timer[1].input_para = 0;
	
	RCC->APB1ENR |= 0x30UL;
	NVIC->ISER[1] |= (0x03 << 22);
}


RAM_FUNC void TIMER_Handle(const TIMER *timer_ptr)
{
	printf_("test is succeed");
}

void TIM7_IRQHandler()
{
	TIM7->SR = 0;
	TIMER_Handle(timer + 1);
}
void TIM6_DAC_IRQHandler()
{
	TIM6->SR = 0;
	TIMER_Handle(timer);
}




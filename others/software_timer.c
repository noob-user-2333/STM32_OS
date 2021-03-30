#include "software_timer.h"
#define SYSTEM_CLOCK      72000000
#define TIM_DIV						71
#define TIM_CYCLES				1000





TIMER* os_timer_create(TIMER *timer_ptr,char *name_ptr,void (*timer_timeout_function)(unsigned long para),
										unsigned long para,unsigned long initial_ticks)
{
	if(initial_ticks && timer_ptr)
	{
//		TIMER *timer_ptr = os_unused_memory_ptr;
//		os_unused_memory_ptr += sizeof(TIMER);
//		
		timer_ptr->name =									name_ptr;
		timer_ptr->remaining_ticks = 			initial_ticks;
		timer_ptr->timer_timeout_function=timer_timeout_function;
		timer_ptr->type_id =							SOFTWARE_TIM_ID;
		timer_ptr->input_para =						para;
		
		if(os_timer_current_ptr)
		{
			TIMER *timer_next;
			if(os_timer_current_ptr->remaining_ticks > initial_ticks)
			{
				os_timer_current_ptr->remaining_ticks -= initial_ticks;
				
				timer_ptr->timer_next = 		os_timer_current_ptr;
				timer_ptr->timer_previous =	os_timer_current_ptr->timer_previous;
				
				os_timer_current_ptr->timer_previous->timer_next = timer_ptr;
				os_timer_current_ptr->timer_previous	=						 timer_ptr;
				
				os_timer_current_ptr = timer_ptr;
				
			}
			else
			{
				timer_next = os_timer_current_ptr;
				do{
					timer_ptr->remaining_ticks -= timer_next->remaining_ticks;
					timer_next = timer_next->timer_next;
				}while(timer_ptr->remaining_ticks >= timer_next->remaining_ticks && timer_next != os_timer_current_ptr);
			
				timer_ptr->timer_next = 			timer_next;
				timer_ptr->timer_previous = 	timer_next->timer_previous;
				
				timer_next->timer_previous->timer_next = timer_ptr;
				timer_next->timer_previous	=						 timer_ptr;
			}
		}
		else
		{
			timer_ptr->timer_next = 			timer_ptr;
			timer_ptr->timer_previous =		timer_ptr;
			os_timer_current_ptr =				timer_ptr;
			TIM3->CR1 |= 1UL;
		}
		return timer_ptr;
	}
	return NULL;
}
TIMER* os_timer_delete(TIMER *timer_ptr)
{
	if(timer_ptr)
	{
		timer_ptr->timer_next->remaining_ticks += timer_ptr->remaining_ticks;
		timer_ptr->timer_next->timer_previous =	timer_ptr->timer_previous;
		timer_ptr->timer_previous->timer_next = timer_ptr->timer_next;
		
		if(timer_ptr == os_timer_current_ptr)
		{
			os_timer_current_ptr = os_timer_current_ptr->timer_next;
			if(os_timer_current_ptr == timer_ptr)
			{
				os_timer_current_ptr = NULL;
			}
		}
		kmem_free(timer_ptr->kmem_own);
		
	}
	return NULL;
}
























void software_tim_init()
{
	RCC->APB1ENR |= 0x03UL;

	NVIC->ISER[0] |= 1 << 29;

	TIM3->CR1 = 0x9C;
	TIM3->PSC = TIM_DIV;
	TIM3->ARR = TIM_CYCLES;
	TIM3->CNT = TIM_CYCLES;
	TIM3->SR = 0;
	TIM3->DIER = 1;

}

void TIM3_IRQHandler()
{
	TIM3->SR = 0;
	if(os_timer_current_ptr)
	{
		os_timer_current_ptr->remaining_ticks --;
		while(os_timer_current_ptr && os_timer_current_ptr->remaining_ticks == 0)
		{
			os_timer_current_ptr->timer_timeout_function(os_timer_current_ptr->input_para);
			os_timer_delete(os_timer_current_ptr);
		}
		if(os_timer_current_ptr)
		{
			TIM3->CR1 |= 0x01UL;
		}
	}
}



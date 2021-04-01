#pragma once
#include "memory_block.h"




#include "os_port.h"

typedef struct SOFTWARE_TIMER_STRUCT{
		unsigned long		type_id;
		const char			*name;

		unsigned long   remaining_ticks;
		
		void						(*timer_timeout_function)(unsigned long para);
		unsigned long		input_para;
	 
		struct SOFTWARE_TIMER_STRUCT
										*timer_next,
										*timer_previous;
		
		struct KERNEL_MEMORY_CACHE
										*kmem_own;
}TIMER;

extern TIMER* os_timer_current_ptr;



void software_tim_init();
TIMER* os_timer_create(char *name_ptr,void (*timer_timeout_function)(unsigned long para),
										unsigned long para,unsigned long initial_ticks);
unsigned int os_timer_delete(TIMER *timer_ptr);

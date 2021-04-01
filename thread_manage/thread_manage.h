#pragma once

#include "memory_block.h"
#include "software_timer.h"
#include "event_group_flag.h"
#define STACK_SIZE 256
#define THREAD_YILED()   SCB->ICSR = 1 << 28;



typedef struct OS_THREAD_STRUCT{
		unsigned int			type_id;
		const char				*name;
		OS_STATE					state;
		
		void							*top_of_stack;
		unsigned int 			stack[STACK_SIZE];
		

	
		unsigned int			(*func)(unsigned int para);
		unsigned int			input_para;
	
		unsigned int			time_slice_new;
		unsigned int			time_slice_current;
	
		struct OS_THREAD_STRUCT
											*os_thread_create_next,
											*os_thread_create_previous;
		
		struct OS_THREAD_STRUCT
											*os_thread_ready_next,
											*os_thread_ready_previous;
		struct OS_THREAD_STRUCT
											*os_thread_suspend_next,
											*os_thread_suspend_previous;
		struct OS_EVENT_FLAGS_GROUP_STRUCT
											*event_flags_group_own;
		struct SOFTWARE_TIMER_STRUCT
											*timer_own;
		
		KERNEL_MEMORY_CACHE	
											*kmem_list;
}OS_THREAD;

unsigned int thread_create(const char *name,
						unsigned int(*func)(unsigned int),unsigned int para,
						unsigned int	time_slice);


void os_kernel_enter();
void os_state_switch();
void os_thread_remove_from_list(OS_THREAD **thread_list,OS_THREAD *thread_ptr);
void os_thread_add_to_list(OS_THREAD **thread_list,OS_THREAD *thread_ptr);

unsigned int os_thread_create(OS_THREAD *thread_ptr,const char *name,
															 unsigned int(*func)(unsigned int),unsigned int para, 
															unsigned int	time_slice);

unsigned int os_thread_delete(OS_THREAD *thread_ptr);															 
										
unsigned int os_thread_sleeping(OS_THREAD *thread_ptr,unsigned int sleep_ticks);
unsigned int os_thread_resume(OS_THREAD *thread_ptr);

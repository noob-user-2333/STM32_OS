#pragma once


#include "queue.h"
#include "software_timer.h"
#include "thread_manage.h"




typedef struct SYSTEM_CALL_MESSAGE{
		unsigned int							type_id;
		OS_THREAD									*action_thread_ptr;
		unsigned int							system_call_func_id;
		unsigned int							input_para[4];
		unsigned int      				reserved;
}SYSTEM_CALL_MESSAGE_STRUCT;


//extern unsigned int(*system_call_array[512])(unsigned int,unsigned int,unsigned int,unsigned int);

unsigned int thread_create(const char *name,
													unsigned int(*func)(unsigned int),unsigned int para, 
													unsigned int	time_slice);

unsigned int thread_delete(OS_THREAD *thread_ptr);															 
										
unsigned int thread_sleeping(OS_THREAD *thread_ptr,unsigned int sleep_ticks);
unsigned int thread_wake_up(OS_THREAD *thread_ptr);









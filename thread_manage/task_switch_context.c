#include "thread_manage.h"

extern OS_THREAD *os_thread_current_ptr;
extern OS_THREAD *os_thread_execute_ptr;

void task_switch_context()
{
	
	if(os_thread_execute_ptr)
	{
		if(os_thread_current_ptr->state != THREAD_READY)
		{
			os_thread_remove_from_list(&os_thread_current_ptr,os_thread_current_ptr);	
		}
		OS_THREAD *thread_ptr = os_thread_execute_ptr;
		os_thread_remove_from_list(&os_thread_execute_ptr,thread_ptr);
		os_thread_add_to_list(&os_thread_current_ptr,thread_ptr);
		os_thread_current_ptr = thread_ptr;
	}
	else
	{
		if(os_thread_current_ptr->state != THREAD_READY)
		{
			os_thread_remove_from_list(&os_thread_current_ptr,os_thread_current_ptr);	
		}
		else
		{
			os_thread_current_ptr = os_thread_current_ptr->os_thread_ready_next;
		}
	}
	
	
}



void SysTick_Handler(void)
{
	if(os_thread_current_ptr)
	{
		os_thread_current_ptr->time_slice_current--;
	
		if(os_thread_current_ptr ->time_slice_current == 0)
		{
			os_thread_current_ptr ->time_slice_current = os_thread_current_ptr ->time_slice_new;
			THREAD_YILED();
		}
	}
}

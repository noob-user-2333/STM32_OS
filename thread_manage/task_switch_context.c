#include "thread_manage.h"

extern OS_THREAD *os_thread_current_ptr;
extern OS_THREAD *os_thread_execute_ptr;
extern OS_THREAD *os_thread_kernel_thread_ptr;

void task_switch_context()
{
	//没有任务可运行则运行内核进程
	if(os_thread_current_ptr == NULL && os_thread_execute_ptr == NULL)
	{
			os_thread_kernel_thread_ptr->os_thread_ready_next = os_thread_kernel_thread_ptr;
			os_thread_kernel_thread_ptr->os_thread_ready_previous = os_thread_kernel_thread_ptr;
			os_thread_execute_ptr = os_thread_kernel_thread_ptr;
	}
	if(os_thread_execute_ptr)
	{
		OS_THREAD *thread_ptr = os_thread_execute_ptr;
		os_thread_remove_from_list(&os_thread_execute_ptr,thread_ptr);
		os_thread_add_to_list(&os_thread_current_ptr,thread_ptr);
		os_thread_current_ptr = thread_ptr;
	}
	else
	{
		
		if(os_thread_current_ptr->state != READY)
		{
			os_thread_remove_from_list(&os_thread_current_ptr,os_thread_current_ptr);	
		}
		else
		{
			
			os_thread_current_ptr = os_thread_current_ptr->os_thread_ready_next;
		}
	}
	
	
}
void task_switch_to_kernel_thread()
{
	if(os_thread_current_ptr != os_thread_kernel_thread_ptr)
	{
		os_thread_kernel_thread_ptr->os_thread_ready_next = 		os_thread_current_ptr;
		os_thread_kernel_thread_ptr->os_thread_ready_previous = os_thread_current_ptr->os_thread_ready_previous;

		os_thread_current_ptr = os_thread_kernel_thread_ptr;
		os_thread_current_ptr->time_slice_current = os_thread_current_ptr->time_slice_new;
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

#include "thread_manage.h"

extern OS_THREAD *os_thread_execute_ptr;
extern OS_THREAD *os_thread_current_ptr;
extern OS_THREAD *os_thread_created_ptr;

void os_thread_remove_from_list(OS_THREAD **thread_list,OS_THREAD *thread_ptr)
{
	if(thread_list && thread_ptr)
	{
		if(thread_list == &os_thread_created_ptr)
		{
			thread_ptr->os_thread_create_next->os_thread_create_previous = thread_ptr->os_thread_create_previous;
			thread_ptr->os_thread_create_previous->os_thread_create_next = thread_ptr->os_thread_create_next;
		}
		else
		{
			thread_ptr->os_thread_ready_next->os_thread_ready_previous = thread_ptr->os_thread_ready_previous;
			thread_ptr->os_thread_ready_previous->os_thread_ready_next = thread_ptr->os_thread_ready_next;
		}
		if(*thread_list == thread_ptr )
		{
			if(thread_list == &os_thread_current_ptr || thread_list == &os_thread_execute_ptr)
			{
				*thread_list = thread_ptr->os_thread_ready_next;
			}
			else
			{
				*thread_list = thread_ptr->os_thread_create_next;
			}
			if(*thread_list == thread_ptr)
			{
				*thread_list = NULL;
			}
		}
	}
}

void os_thread_add_to_list(OS_THREAD **thread_list,OS_THREAD *thread_ptr)
{
	if(thread_list && thread_list)
	{
		if(*thread_list == NULL)
		{
			*thread_list = thread_ptr;
			if(thread_list == &os_thread_created_ptr)
			{
				thread_ptr->os_thread_create_next = thread_ptr->os_thread_create_previous = thread_ptr;
			}
			else
			{
				thread_ptr->os_thread_ready_next = thread_ptr->os_thread_ready_previous = thread_ptr;
			}
		}
		else
		{
			if(thread_list == &os_thread_created_ptr)
			{
				thread_ptr->os_thread_create_next = 			*thread_list;
				thread_ptr->os_thread_create_previous =  (*thread_list)->os_thread_create_previous;
				
				(*thread_list)->os_thread_create_previous->os_thread_create_next = thread_ptr;
				(*thread_list)->os_thread_create_previous											 = thread_ptr;
			}
			else
			{
				thread_ptr->os_thread_ready_next = 			*thread_list;
				thread_ptr->os_thread_ready_previous =  (*thread_list)->os_thread_ready_previous;
				
				(*thread_list)->os_thread_ready_previous->os_thread_ready_next = thread_ptr;
				(*thread_list)->os_thread_ready_previous											 = thread_ptr;
			}
		}
	}
}
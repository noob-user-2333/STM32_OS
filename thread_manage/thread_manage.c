#include "thread_manage.h"
#include "queue.h"
extern OS_THREAD *os_thread_execute_ptr;
extern OS_THREAD *os_thread_current_ptr;
extern OS_THREAD *os_thread_created_ptr;
extern OS_THREAD os_thread_init;
extern unsigned int     os_thread_created_count;
extern unsigned char		os_queue_buff[OS_QUEUE_CAPACITY * OS_QUEUE_MESSAGE_SIZE];




unsigned int init_func(unsigned int para)
{
	while(1)
	{
		OS_DISABLE
		memory_manage_sort();
		OS_ENABLE
	}
	return OS_THREAD_ERROR_RETURN_ID;
}





void os_thread_initialize(void)
{
	os_thread_created_ptr = NULL;
	os_thread_current_ptr = NULL;
	os_thread_execute_ptr = NULL;
	os_thread_created_count = 0;
	
	os_thread_create(&os_thread_init,"thread_init",init_func,0,2);
	os_queue_create(&os_queue,"os_queue",OS_QUEUE_MESSAGE_SIZE,os_queue_buff,OS_QUEUE_CAPACITY);
}

unsigned int os_thread_sleeping(OS_THREAD *thread_ptr,unsigned int sleep_ticks)
{
	if(thread_ptr && sleep_ticks && thread_ptr->timer_own == NULL)
	{
		KERNEL_MEMORY_CACHE *kmem_ptr = kmem_malloc(sizeof(TIMER));
		TIMER *timer_ptr = kmem_ptr->start;
		timer_ptr->kmem_own = kmem_ptr;
		

		thread_ptr->timer_own  = timer_ptr;
		thread_ptr->state =			 THREAD_SLEEPING;
		
		os_timer_create(timer_ptr,(char*)thread_ptr->name,(void (*)(unsigned long))os_thread_wake_up,(unsigned long)thread_ptr,sleep_ticks);
			
		
		if(thread_ptr == os_thread_current_ptr)
		{
			THREAD_YILED();
		}
		else
		{
			os_thread_remove_from_list(&os_thread_current_ptr,thread_ptr);
			os_thread_remove_from_list(&os_thread_execute_ptr,thread_ptr);
		}
		return 0;
	}
	return 0xFFFFFFFF;
}
unsigned int os_thread_wake_up(OS_THREAD *thread_ptr)					
{
	if(thread_ptr && thread_ptr->state == THREAD_SLEEPING)
	{
		os_thread_add_to_list(&os_thread_execute_ptr,thread_ptr);
		thread_ptr->state = THREAD_READY;
		thread_ptr->timer_own = NULL;
	
		
		return 0;
	}
	return 0xFFFFFFFF;
}



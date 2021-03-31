#include "thread_manage.h"
#include "software_timer.h"
extern OS_THREAD *os_thread_current_ptr;
extern OS_THREAD *os_thread_execute_ptr;
extern OS_THREAD *os_thread_created_ptr;
extern OS_THREAD os_thread_init;
extern unsigned int     os_thread_created_count;


#define portINITIAL_XPSR			( 0x01000000 ) //第24位强制规定必须置1

unsigned int os_thread_delete(OS_THREAD *thread_ptr)
{
	//kmem_list == NULL说明其为系统进程，不可删除
	if(thread_ptr && thread_ptr->kmem_list)
	{
		if(os_thread_current_ptr == os_thread_current_ptr->os_thread_ready_next && os_thread_execute_ptr == NULL)
			return 0xFFFFFFF0;
		
		if(thread_ptr->state == READY)
		{
			thread_ptr->os_thread_ready_next->os_thread_ready_previous = thread_ptr->os_thread_ready_previous;
			thread_ptr->os_thread_ready_previous->os_thread_ready_next = thread_ptr->os_thread_ready_next;
			if(thread_ptr == os_thread_current_ptr)
			{
				os_thread_current_ptr = os_thread_current_ptr->os_thread_ready_next;
				if(thread_ptr == os_thread_current_ptr)
				{
					os_thread_current_ptr = NULL;
				}
			}
			if(thread_ptr == os_thread_execute_ptr)
			{
				os_thread_execute_ptr = os_thread_execute_ptr->os_thread_ready_next;
				if(thread_ptr == os_thread_execute_ptr)
				{
					os_thread_execute_ptr = NULL;
				}
			}
		}
		thread_ptr->os_thread_create_next->os_thread_create_previous = thread_ptr->os_thread_create_previous;
		thread_ptr->os_thread_create_previous->os_thread_create_next = thread_ptr->os_thread_create_next;
		os_timer_delete(thread_ptr->timer_own);
		
		for(KERNEL_MEMORY_CACHE* kmem_ptr = thread_ptr->kmem_list->work_previous;thread_ptr->kmem_list != kmem_ptr;kmem_ptr = kmem_ptr->work_previous)
		{
			kmem_free(kmem_ptr);
		}
		
		kmem_free(thread_ptr->kmem_list);
		os_thread_created_count--;
		return 0;
	}
	return 0xFFFFFFFF;
}
void thread_return_handle(unsigned int para)
{
	if(para == OS_THREAD_ERROR_RETURN_ID)
	{
		OS_RESET;
		while(1)
		{}
	}
	else
	{
		os_thread_delete(os_thread_current_ptr);
	}
	THREAD_YILED();
	while(1)
	{}
}
unsigned int* os_build_stack(OS_THREAD*thread_ptr)
{
	//向下做8字节对齐
	unsigned int* stack_ptr = thread_ptr->stack + STACK_SIZE - 1;
		stack_ptr = (unsigned int*)((unsigned int)stack_ptr & (~(0x07UL)));
	
		stack_ptr--;
		*stack_ptr = portINITIAL_XPSR;
		stack_ptr--;
		*stack_ptr = (unsigned int)thread_ptr->func;
		stack_ptr--;
		*stack_ptr = (unsigned int)thread_return_handle;
	  stack_ptr -= 5; //R12,R3,R2,R1全为0
		*stack_ptr = (unsigned int)thread_ptr->input_para;
		//异常发生时手动加载到cpu寄存器的内容
		stack_ptr -= 8;
		return stack_ptr;
}


unsigned int os_thread_create(OS_THREAD *thread_ptr,const char *name,
															 unsigned int(*func)(unsigned int),unsigned int para, 
															unsigned int	time_slice)
{
		memset(thread_ptr,0,sizeof(OS_THREAD));
		
		thread_ptr->type_id =				THREAD_ID;
		thread_ptr->name =					name;
		thread_ptr->func =					func;	
		thread_ptr->input_para = 		para;
		thread_ptr->state =					NEW;
		thread_ptr->time_slice_new =time_slice;
		thread_ptr->time_slice_current=time_slice;
	
		thread_ptr->top_of_stack =  os_build_stack(thread_ptr);
	
		if(os_thread_created_ptr)
		{
				thread_ptr->os_thread_create_previous = os_thread_created_ptr->os_thread_create_previous; 
				thread_ptr->os_thread_create_next	=			os_thread_created_ptr;
			
				os_thread_created_ptr->os_thread_create_previous->os_thread_create_next = thread_ptr;
				os_thread_created_ptr->os_thread_create_previous		 										= thread_ptr;
		}
		else
		{
			os_thread_created_ptr = thread_ptr;
			thread_ptr->os_thread_create_next = 			thread_ptr;
			thread_ptr->os_thread_create_previous =		thread_ptr;
		}
	
		if(os_thread_current_ptr)
		{
			thread_ptr->os_thread_ready_previous = os_thread_current_ptr->os_thread_ready_previous; 
			thread_ptr->os_thread_ready_next	=			os_thread_current_ptr;
			
			os_thread_current_ptr->os_thread_ready_previous->os_thread_ready_next = thread_ptr;
			os_thread_current_ptr->os_thread_ready_previous		 										= thread_ptr;
		}
		else
		{
			os_thread_current_ptr = thread_ptr;
			thread_ptr->os_thread_ready_next = 			thread_ptr;
			thread_ptr->os_thread_ready_previous =		thread_ptr;
		}
		os_thread_created_count++;
		thread_ptr->state = READY;

		return OS_SUCCESS;
}






#define CPU_FREQ 			72000000
#define SYS_CYCLES		((CPU_FREQ/20)-1)
/*
function:将SVC与penSVC中断优先级设定为最低
					将sysTick设定为次低
					中断优先级全为抢断优先级
					
*/
void os_initialize_low_level()
{
	//设定中断优先级全为抢断
		SCB->AIRCR = 0x300UL;
	//设定SVC与pendSV、systick中断优先级
		SCB->SHP[7] = 0xff;
		SCB->SHP[10]=0xee;
		SCB->SHP[11]=0xff;
	//设定systick
		
		SysTick->LOAD = SYS_CYCLES;
		SysTick->CTRL = 0x07UL;
	
}
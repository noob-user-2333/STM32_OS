#include "system_call.h"

extern OS_THREAD *os_thread_current_ptr;
extern OS_THREAD *os_thread_execute_ptr;
extern OS_THREAD  os_thread_init;
extern OS_STATE  os_state;
void  switch_to_init_thread();
unsigned int thread_create(const char *name,
						unsigned int(*func)(unsigned int),unsigned int para,
						unsigned int	time_slice);
const unsigned int(*system_call_array[512])(unsigned int,unsigned int,unsigned int,unsigned int)/*__attribute__((section(".ARM.__AT_0x0800F000")))*/ = {
																			thread_create,os_thread_delete,os_thread_sleeping,os_thread_resume,0,0,0,0,0,0,0,0,0,0,0,0	};

																			
																			
																			
																			
																			
																			
																			
																			
																			
unsigned int system_call_use(unsigned int sys_call_id,unsigned int para0,unsigned int para1,unsigned int para2,unsigned int para3)
{
	if(sys_call_id < 512)
	{
		SYSTEM_CALL_MESSAGE_STRUCT message;
		message.type_id	=							SYSTEM_CALL_MESSAGE_ID;
		message.system_call_func_id = sys_call_id;
		message.action_thread_ptr   = os_thread_current_ptr;
		message.input_para[0] =				para0; 
		message.input_para[1] = 			para1;
		message.input_para[2] = 			para2;
		message.input_para[3] = 			para3;
		message.reserved	=						0;

		os_queue_send(&os_queue,&message);
		os_switch_to_kernel_thread();
		for(unsigned int i=0;i<=100000 && os_state == WAIT_FOR_SYSTEM_CALL;i++)
		{
			if(i == 100000)
			{
				os_state = READY;
				return 0xFFFFFF00;
			}
		}
		return 0;
	}
	return 0xFFFFFFFF;
}
																			






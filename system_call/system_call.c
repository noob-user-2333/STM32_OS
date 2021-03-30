#include "system_call.h"

extern OS_THREAD *os_thread_current_ptr;
extern OS_THREAD *os_thread_execute_ptr;
extern OS_THREAD  os_thread_init;
void  switch_to_init_thread();
//unsigned int(*system_call_array[512])(unsigned int,unsigned int,unsigned int,unsigned int)__attribute__((section(".ARM.__AT_0x0800F000"))) = {
//																			thread_create,thread_delete,thread_sleeping,thread_wake_up	};

																			
																			
unsigned int thread_create(const char *name,
													unsigned int(*func)(unsigned int),unsigned int para, 
													unsigned int	time_slice)
{
	SYSTEM_CALL_MESSAGE_STRUCT message;
	message.type_id 	=								SYSTEM_CALL_MESSAGE_ID;
	message.action_thread_ptr = 			os_thread_current_ptr;
	message.system_call_func_id =	    SYS_CAL_THREAD_CREATE;
	message.input_para[0]	=						(unsigned int)name;				
	message.input_para[1] =						(unsigned int)func;
	message.input_para[2] =						para;
	message.input_para[3] =						time_slice;
	message.reserved =								0;
	os_queue_send(&os_queue,&message);
	switch_to_init_thread();
	return 0;
}

unsigned int thread_delete(OS_THREAD *thread_ptr)
{
	SYSTEM_CALL_MESSAGE_STRUCT message;
	message.type_id 	=								SYSTEM_CALL_MESSAGE_ID;
	message.action_thread_ptr = 			os_thread_current_ptr;
	message.system_call_func_id =	    SYS_CAL_THREAD_DELETE;
	message.input_para[0]	=						(unsigned int)thread_ptr;				
	message.input_para[1] =						(unsigned int)0;
	message.input_para[2] =						0;
	message.input_para[3] =						0;
	message.reserved =								0;
	os_queue_send(&os_queue,&message);
	switch_to_init_thread();
	return 0;
}	
										
unsigned int thread_sleeping(OS_THREAD *thread_ptr,unsigned int sleep_ticks)
{
	SYSTEM_CALL_MESSAGE_STRUCT message;
	message.type_id 	=								SYSTEM_CALL_MESSAGE_ID;
	message.action_thread_ptr = 			os_thread_current_ptr;
	message.system_call_func_id =	     SYS_CAL_THREAD_SLEEPING;
	message.input_para[0]	=						(unsigned int)thread_ptr;				
	message.input_para[1] =						(unsigned int)sleep_ticks;
	message.input_para[2] =						0;
	message.input_para[3] =						0;
	message.reserved =								0;
	os_queue_send(&os_queue,&message);
	switch_to_init_thread();
	return 0;
}
unsigned int thread_wake_up(OS_THREAD *thread_ptr)
{
	SYSTEM_CALL_MESSAGE_STRUCT message;
	message.type_id 	=								SYSTEM_CALL_MESSAGE_ID;
	message.action_thread_ptr = 			os_thread_current_ptr;
	message.system_call_func_id =	    SYS_CAL_THREAD_WAKE_UP;
	message.input_para[0]	=						(unsigned int)thread_ptr;				
	message.input_para[1] =						0;
	message.input_para[2] =						0;
	message.input_para[3] =						0;
	message.reserved =								0;
	os_queue_send(&os_queue,&message);
	switch_to_init_thread();
	return 0;
}


void  switch_to_init_thread()
{
	os_thread_execute_ptr = &os_thread_init;
	THREAD_YILED();
}


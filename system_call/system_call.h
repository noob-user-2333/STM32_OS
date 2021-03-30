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


extern unsigned int(*system_call_array[512])(unsigned int,unsigned int,unsigned int,unsigned int);

unsigned int system_call_use(unsigned int sys_call_id,unsigned int para0,unsigned int para1,unsigned int para2,unsigned int para3);








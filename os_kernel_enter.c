#include "memory_block.h"
#include "software_timer.h"
#include "thread_manage.h"

extern OS_STATE os_state;
extern void os_initialize_low_level();
extern void os_thread_initialize(void);
extern void os_thread_schedule();
extern void os_define_application();


void os_kernel_enter()
{
	os_state = NEW;
	memory_manage_initialize();


	software_tim_init();

	os_thread_initialize();
	
	
	os_define_application();
	os_initialize_low_level();
	
	os_thread_schedule();
}

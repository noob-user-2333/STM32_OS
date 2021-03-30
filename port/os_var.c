#include "memory_block.h"
#include "software_timer.h"
#include "thread_manage.h"
#include "queue.h"



/*
//that is something about memory manage
*/
MEMORY_BLOCK	*free_info_block;
MEMORY_BLOCK *buddy_system_list[BUDDY_SYSTEM_LIST_LENGTH];
MEMORY_BLOCK *slob_current;
MEMORY_BLOCK *slob_partial;

void					*os_unused_memory_front_ptr;
void					*os_unused_memory_back_ptr;


/*
//that is something about thread manage
*/

OS_THREAD *os_thread_current_ptr;
OS_THREAD *os_thread_created_ptr;
OS_THREAD *os_thread_execute_ptr;
OS_THREAD  os_thread_init;
unsigned int os_thread_created_count;






/*
//that is something about others
*/

TIMER* os_timer_current_ptr;
QUEUE *os_queue_create_ptr;
QUEUE  os_queue;
unsigned char os_queue_buff[OS_QUEUE_CAPACITY * OS_QUEUE_MESSAGE_SIZE];
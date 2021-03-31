#pragma once 
#include "main.h"
#include <string.h>

#define SOFTWARE_TIM_ID 					0xFEFEFEF0
#define MEMORY_PAGE_ID 						0xFEFEFE00
#define MEMORY_BLOCK_ID						0xFEFEFE10
#define THREAD_ID 								0xFE00FEFE
#define QUEUE_ID									0xFEFEFE20
#define SYSTEM_CALL_MESSAGE_ID		0xFEFE0000



#define SRAM_START										0x20000000UL
#define SRAM_END											0x20005000UL
#define KMEM_MASK 										0x80000000UL
#define BUDDY_SYSTEM_LIST_LENGTH			8

#define SYSTEM_CALL_ARRAY_SIZE				0x1000UL
#define FLASH_START										0x08000000UL
#define FLASH_SIZE										0x10000UL
#define FLASH_END											(FLASH_START+FLASH_SIZE-SYSTEM_CALL_ARRAY_SIZE)
#define SYSTEM_CALL_ARRAY_LOCAL				(FLASH_END)



#define OS_THREAD_ERROR_RETURN_ID			0xFFFF00FE

#define OS_QUEUE_MESSAGE_SIZE					32
#define OS_QUEUE_CAPACITY							32





#define OS_MAX_INTERRUPT  0xEE

#define OS_RESET		 {__set_FAULTMASK (1);SCB->AIRCR = 0x05FA0004;}
#define OS_DISABLE   __set_BASEPRI(OS_MAX_INTERRUPT);
#define OS_ENABLE     __set_BASEPRI(0);

#define OS_FAILED  1
#define OS_SUCCESS 0


#define SYS_CAL_FUNC_TYPE_PTR			(unsigned int (*)(unsigned int,unsigned int,unsigned int,unsigned int))

#define SYS_CAL_THREAD_CREATE 				0
#define SYS_CAL_THREAD_DELETE					1
#define SYS_CAL_THREAD_SLEEPING 			2
#define SYS_CAL_THREAD_WAKE_UP 				3


typedef enum OS_STATE_ENUM{
		NEW,
		READY,
		SLEEPING,
		BUSY_IN_READ,
		BUSY_IN_WRITE,
		BUSYING,
		BLOCKED,
		DELETED
}OS_STATE;


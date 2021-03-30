#pragma once 
#include "main.h"
#include <string.h>

#define SOFTWARE_TIM_ID 	0xFEFEFEF0
#define MEMORY_PAGE_ID 		0xFEFEFE00
#define MEMORY_BLOCK_ID		0xFEFEFE10
#define THREAD_ID 				0xFE00FEFE
#define QUEUE_ID					0xFEFEFE20

#define OS_THREAD_ERROR_RETURN_ID			0xFFFF00FE

#define OS_QUEUE_MESSAGE_SIZE					0x20UL
#define OS_QUEUE_CAPACITY							0x20UL





#define OS_MAX_INTERRUPT  0xEE

#define OS_RESET		 {__set_FAULTMASK (1);SCB->AIRCR = 0x05FA0004;}
#define OS_DISABLE   __set_BASEPRI(OS_MAX_INTERRUPT);
#define OS_ENABLE     __set_BASEPRI(0);

#define OS_FAILED  1
#define OS_SUCCESS 0







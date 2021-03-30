#pragma once
#include "os_port.h"


typedef struct KERNEL_MEMORY_CACHE{
		unsigned int 	size;
		
		void					*start;
		void					*end;

		struct KERNEL_MEMORY_CACHE
									*work_previous,
									*work_next;
		
		struct KERNEL_MEMORY_CACHE
									*location_previous,
									*location_next;
		
		struct MEMORY_BLOCK_STRUCT
									*owner;
}KERNEL_MEMORY_CACHE;



typedef struct MEMORY_BLOCK_STRUCT{
		unsigned int			size;
		
	
		void							*start;
		void							*end;

		struct MEMORY_BLOCK_STRUCT
											*work_previous,
											*work_next;
		
		unsigned int			available_storage;
		unsigned int			refered_number;
		struct KERNEL_MEMORY_CACHE
											*slob;
}MEMORY_BLOCK;

extern void					*os_unused_memory_front_ptr;
extern void					*os_unused_memory_back_ptr;

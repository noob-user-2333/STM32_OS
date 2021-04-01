#include "memory_block.h"



extern MEMORY_BLOCK *slob_current;
extern MEMORY_BLOCK *slob_partial;

extern MEMORY_BLOCK	*free_info_block;
extern unsigned int __RAM_segment_used_end__;



static void memory_block_slob_recyc(MEMORY_BLOCK *block_ptr);



void memory_manage_initialize()
{
	os_unused_memory_front_ptr = (void*)&__RAM_segment_used_end__;
	os_unused_memory_back_ptr =  (void*)SRAM_END;
	
	memset(buddy_system_list,0,sizeof(MEMORY_BLOCK*) * BUDDY_SYSTEM_LIST_LENGTH);
	free_info_block = NULL;
	slob_current = memory_block_create(0x1000);
	slob_partial = NULL;
}

void memory_manage_sort()
{
	MEMORY_BLOCK *block_ptr;
	memory_block_slob_recyc(slob_current);
	if(slob_partial)
	{
		block_ptr = slob_partial;
		do{
			memory_block_slob_recyc(block_ptr);

			block_ptr = block_ptr->work_next;
		}while(block_ptr != slob_partial);

	}
	for(unsigned int local = 0;local < BUDDY_SYSTEM_LIST_LENGTH;)
	{
		if(buddy_system_list[local] && buddy_system_list[local]->start == os_unused_memory_back_ptr)
		{
			block_ptr = buddy_system_list[local];
			if(buddy_system_list[local] == buddy_system_list[local]->work_next)
			{
				buddy_system_list[local] = NULL;
			}
			else
			{
				buddy_system_list[local]->work_previous->work_next = buddy_system_list[local]->work_next;
				buddy_system_list[local]->work_next->work_previous = buddy_system_list[local]->work_previous;
				buddy_system_list[local] = buddy_system_list[local]->work_next;
			}
			os_unused_memory_back_ptr = block_ptr->end;
			info_block_insert(block_ptr);
			local = 0;
		}
		else
		{
			local++;
		}
	}
	

}


static void memory_block_slob_recyc(MEMORY_BLOCK *block_ptr)
{
	if(block_ptr && block_ptr->slob)
	{
		KERNEL_MEMORY_CACHE *kmem_ptr;
		for(kmem_ptr = block_ptr->slob->location_previous;kmem_ptr != block_ptr->slob && (kmem_ptr->size & KMEM_MASK) == 0;
				kmem_ptr = kmem_ptr->location_previous)
		{}
		block_ptr->slob->location_previous = kmem_ptr;
		kmem_ptr->location_next =						 block_ptr->slob; 
		block_ptr->available_storage = block_ptr->end - kmem_ptr->end;
	}
}

#include "memory_block.h"
extern MEMORY_BLOCK	*free_info_block;
extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;
extern void					*os_unused_memory_front_ptr;
extern void					*os_unused_memory_back_ptr;

MEMORY_BLOCK* info_block_get()
{
	MEMORY_BLOCK *info_ptr = NULL;
	if(free_info_block)
	{
		info_ptr = free_info_block;
		
		free_info_block->work_previous->work_next = free_info_block->work_next;
		free_info_block->work_next->work_previous = free_info_block->work_previous;
		free_info_block = free_info_block->work_next;
	}
	else
	{
		if(os_unused_memory_front_ptr + sizeof(MEMORY_BLOCK) < os_unused_memory_back_ptr)
		{
			info_ptr = os_unused_memory_front_ptr;
			os_unused_memory_front_ptr += sizeof(MEMORY_BLOCK);
		}
	}
	return info_ptr;
}

unsigned int info_block_insert(MEMORY_BLOCK *info_ptr)
{
	info_ptr->work_next->work_previous = info_ptr->work_previous;
	info_ptr->work_previous->work_next = info_ptr->work_next;
	if(free_info_block)
	{
		info_ptr->work_next = 		free_info_block;
		info_ptr->work_previous = free_info_block->work_previous;
		free_info_block->work_previous->work_next = info_ptr;
		free_info_block->work_previous =						info_ptr;
	}
	else
	{
		free_info_block = 				info_ptr;
		info_ptr->work_next = 		info_ptr;
		info_ptr->work_previous = info_ptr;
	}
	return 0;
}


MEMORY_BLOCK* memory_block_create(unsigned int size)
{
	MEMORY_BLOCK *block_ptr = NULL;
	if(size >= 0x1000)
	{
		block_ptr = info_block_get();
		if( (unsigned)os_unused_memory_back_ptr - (unsigned)os_unused_memory_front_ptr >= size)	
		{
			block_ptr->size = 							size;
			block_ptr->available_storage =	size;
			block_ptr->refered_number =			0;
			
			block_ptr->end =								os_unused_memory_back_ptr;
			os_unused_memory_back_ptr -=		size;
			block_ptr->start =							os_unused_memory_back_ptr;
			block_ptr->slob =								NULL;
			
			block_ptr->work_next =					block_ptr;
			block_ptr->work_previous =			block_ptr;
		}
		else
		{
			info_block_insert(block_ptr);
			block_ptr = NULL;
		}
	}
	return block_ptr;
}

MEMORY_BLOCK* memory_block_get(unsigned int size)
{
	MEMORY_BLOCK *block_ptr = NULL;
	if(size >= 0x1000 )
	{
		unsigned int highest_bit;
		NORMALIZE_SIZE(size,highest_bit);
		if(highest_bit - 12 < BUDDY_SYSTEM_LIST_LENGTH)
		{
			for(unsigned int start_position = highest_bit - 12;start_position < BUDDY_SYSTEM_LIST_LENGTH ;start_position++)
			{
				if(buddy_system_list[start_position])
				{
					for(unsigned int position = start_position;;position--)
					{
						MEMORY_BLOCK *block1 = buddy_system_list[position];
						
						
							buddy_system_list[position] = block1->work_next;
							if(buddy_system_list[position] == block1)
							{
								buddy_system_list[position] = NULL;
							}
							block1->work_next->work_previous = block1->work_previous;
							block1->work_previous->work_next = block1->work_next;
							block1->refered_number =					 0;
						
						if(position != highest_bit - 12)
						{
//							block1->work_next->work_previous = block1->work_previous;
//							block1->work_previous->work_next = block1->work_next;
//							buddy_system_list[position] = block1->work_next;
//							if(buddy_system_list[position] == block1)
//							{
//								buddy_system_list[position] = NULL;
//							}
							block_ptr = info_block_get();
							block_ptr->work_next = 			block_ptr;
							block_ptr->work_previous = 	block_ptr;
							block_ptr->start = 					block1->start;
							block_ptr->slob =						NULL;
							block_ptr->refered_number = 0;
							block_ptr->size =						block1->size >> 1;
							block_ptr->available_storage = block_ptr->size;
							block_ptr->end =						block_ptr->start + block_ptr->size;
							
							block1->size >>=						1;
							block1->available_storage = block1->size;
							block1->start =							block1->end - block1->size;
							
							memory_block_insert(block1);
							memory_block_insert(block_ptr);
							
							block_ptr = NULL;
							continue;
						}
						return block1;
					}
				}
			}
			if(block_ptr == NULL)
			{
				block_ptr = memory_block_create(size);
			}
		}
	}
	return block_ptr;
}




unsigned int memory_block_insert(MEMORY_BLOCK *block_ptr)
{
	if(block_ptr &&  block_ptr->refered_number == 0)
	{
		MEMORY_BLOCK *block_next;
		unsigned int highest_bit;
		NORMALIZE_SIZE(block_ptr->size,highest_bit);
		highest_bit -= 12;
		block_ptr->slob = NULL;
		if(buddy_system_list[highest_bit] == NULL)
		{
			buddy_system_list[highest_bit] = block_ptr;
			block_ptr->work_next = block_ptr;
			block_ptr->work_previous = block_ptr;
			return 0;
		}
		block_next = buddy_system_list[highest_bit];
		do{
			if(block_next->start > block_ptr->start)
			{
				break;
			}
			block_next = block_next->work_next;
		}while(block_next != buddy_system_list[highest_bit]);
		if(block_next == buddy_system_list[highest_bit] && block_next->start > block_ptr->start)
		{
			buddy_system_list[highest_bit] = block_ptr;
		}
		block_ptr->work_next = block_next;
		block_ptr->work_previous = block_next->work_previous;
		
		block_next->work_previous->work_next = block_ptr;
		block_next->work_previous =						 block_ptr;
		
		return 0;
	}
	return 0xFFFFFFFF;
}







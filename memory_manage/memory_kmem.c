#include "memory_block.h"
#define KMEM_MIN_SIZE 0x40UL
#define KMEM_MASK 	0x80000000UL
#define INTERVAL		0x10UL

extern MEMORY_BLOCK *slob_current;
extern MEMORY_BLOCK *slob_partial;



static KERNEL_MEMORY_CACHE *kmem_get_from_block(MEMORY_BLOCK *block_ptr,unsigned int size)
{
	KERNEL_MEMORY_CACHE *kmem_ptr = NULL;
	if(block_ptr->slob)
	{
		kmem_ptr = block_ptr->slob;
		do{
			if(kmem_ptr->size >= size  && (kmem_ptr->size & KMEM_MASK) == 0)
			{
				kmem_ptr->work_next =					kmem_ptr;
				kmem_ptr->work_previous = 		kmem_ptr;
				kmem_ptr->size |= KMEM_MASK;
				kmem_ptr->owner->refered_number++;
				return kmem_ptr;
			}
			kmem_ptr = kmem_ptr->location_next;
		}while(kmem_ptr != block_ptr->slob);
	}
	if(block_ptr->available_storage >= size + sizeof(KERNEL_MEMORY_CACHE) + INTERVAL)
	{
		kmem_ptr = block_ptr->end - block_ptr->available_storage + INTERVAL;
		kmem_ptr->owner = block_ptr;
		kmem_ptr->size = size;
		kmem_ptr->start = (void*)kmem_ptr + sizeof(KERNEL_MEMORY_CACHE);
		kmem_ptr->end =  kmem_ptr->start + size;
		
		block_ptr->refered_number ++;
		block_ptr->available_storage -= size + sizeof(KERNEL_MEMORY_CACHE) + INTERVAL;
		if(block_ptr->slob)
		{
			kmem_ptr->location_next =  	block_ptr->slob;
			kmem_ptr->location_previous=block_ptr->slob->location_previous;
			
			block_ptr->slob->location_previous->location_next = kmem_ptr;
			block_ptr->slob->location_previous =								kmem_ptr;
		}
		else
		{
			block_ptr->slob = kmem_ptr;
			kmem_ptr->location_next = 		kmem_ptr;
			kmem_ptr->location_previous = kmem_ptr;
		}
		kmem_ptr->work_next =					kmem_ptr;
		kmem_ptr->work_previous = 		kmem_ptr;
		kmem_ptr->size |= KMEM_MASK;
		return kmem_ptr;
	}
	if(block_ptr->available_storage <= KMEM_MIN_SIZE)
	{
		block_ptr->work_next->work_previous = block_ptr->work_previous;
		block_ptr->work_previous->work_next = block_ptr->work_next;
		if(block_ptr == slob_current)
		{
			if(slob_partial)
			{
				slob_current = slob_partial;
				slob_partial = slob_partial->work_next;
				if(slob_partial == slob_current)
				{
					slob_partial = NULL;
				}
			}
			else
			{
				slob_current = memory_block_get(0x1000);
			}
		}
		else if(block_ptr == slob_partial)
		{
			slob_partial = slob_partial->work_next;
			if(slob_partial == slob_partial->work_next)
			{
				slob_partial = NULL;
			}
		}
	}
	return NULL;
}









KERNEL_MEMORY_CACHE *kmem_malloc(unsigned int size)
{
	if(size == 0)
		return NULL;
	KERNEL_MEMORY_CACHE *kmem_ptr = NULL;
	//防止接近0x1000的内存无法分配
	if(size < 0x1000 && size + sizeof(KERNEL_MEMORY_CACHE) >= 0x1000)
	{
		size = 0x1000;
	}
	if(size >= 0x1000)
	{
		kmem_ptr = (void*)info_block_get();
		kmem_ptr->owner = memory_block_get(size);
		kmem_ptr->owner->refered_number ++;
		kmem_ptr->owner->available_storage = 0;
		kmem_ptr->owner->slob =	kmem_ptr;
		
		kmem_ptr->end = 		kmem_ptr->owner->end;
		kmem_ptr->start =		kmem_ptr->owner->start;
		kmem_ptr->size= 		kmem_ptr->owner->size;
		kmem_ptr->size |=		KMEM_MASK;
		kmem_ptr->location_next = kmem_ptr->location_previous = kmem_ptr;
		kmem_ptr->work_next = kmem_ptr->work_previous = kmem_ptr;
		memset(kmem_ptr->start,0,kmem_ptr->size);
	}
	else
	{
		MEMORY_BLOCK *block_ptr = slob_partial;
		kmem_ptr = kmem_get_from_block(slob_current,size);
		if(kmem_ptr)
			return kmem_ptr;
		if(kmem_ptr == NULL && slob_partial)
		{
			do{
				kmem_ptr = kmem_get_from_block(block_ptr,size);
				if(kmem_ptr)
				{
					return kmem_ptr;
				}
				block_ptr = block_ptr->work_next;
			}while(block_ptr != slob_partial);
		}
		//重新从伙伴系统获取内存块进行分配
		block_ptr = memory_block_get(0x1000);
		kmem_ptr = kmem_get_from_block(block_ptr,size);
		if(block_ptr)
		{
			if(slob_partial)
			{
				block_ptr->work_next = 			slob_partial;
				block_ptr->work_previous =	slob_partial->work_previous;
			
				slob_partial->work_previous->work_next = block_ptr;
				slob_partial->work_previous	=						 block_ptr;
			}
			else
			{
				slob_partial = 							block_ptr;
				block_ptr->work_next = 			block_ptr;
				block_ptr->work_previous = 	block_ptr;
			}
		}
	}
	return kmem_ptr;
}
unsigned int kmem_free(KERNEL_MEMORY_CACHE *kmem_ptr)
{
	if(kmem_ptr && (kmem_ptr->size & KMEM_MASK))
	{
		kmem_ptr->owner->refered_number--;
		kmem_ptr->size &= (~KMEM_MASK);
		if(kmem_ptr->size >= 0x1000)
		{
			kmem_ptr->owner->available_storage = kmem_ptr->owner->size;
			memory_block_insert(kmem_ptr->owner);
			info_block_insert((MEMORY_BLOCK*)kmem_ptr);
		}
		else
		{
			kmem_ptr->work_next->work_previous = kmem_ptr->work_previous;
			kmem_ptr->work_previous->work_next = kmem_ptr->work_next;

			if(kmem_ptr->owner->refered_number == 0)
			{
				kmem_ptr->owner->slob = NULL;
				kmem_ptr->owner->available_storage = kmem_ptr->owner->size;
				kmem_ptr->owner->work_next->work_previous = kmem_ptr->owner->work_previous;
				kmem_ptr->owner->work_previous->work_next = kmem_ptr->owner->work_next;
				
				if(kmem_ptr->owner != slob_current)
				{
					if(kmem_ptr->owner == slob_partial)
					{
						slob_partial = slob_partial->work_next;
						if(slob_partial == slob_partial->work_next)
						{
							slob_partial = NULL;
						}
					}
					memory_block_insert(kmem_ptr->owner);
				}
			}

		}
		return 0;
	}
	return 0xFFFFFFF;
}
#pragma once
#include "memory_manage.h"


extern MEMORY_BLOCK	*free_info_block;
extern MEMORY_BLOCK *buddy_system_list[BUDDY_SYSTEM_LIST_LENGTH];

void memory_manage_initialize(void);
void memory_manage_sort(void);





KERNEL_MEMORY_CACHE *kmem_malloc(unsigned int size);
unsigned int			 kmem_free(KERNEL_MEMORY_CACHE *kmem_ptr);
MEMORY_BLOCK* memory_block_create(unsigned int size);
unsigned int memory_block_insert(MEMORY_BLOCK *block_ptr);
MEMORY_BLOCK* memory_block_get(unsigned int size);

unsigned int info_block_insert(MEMORY_BLOCK *info_ptr);
MEMORY_BLOCK* info_block_get();
#define  NORMALIZE_SIZE(size,highest_bit)			{size &= (~0x80000000UL);\
																							for(highest_bit = 31;(size & 0x80000000) == 0;highest_bit--,size <<= 1){}\
																							if(size << 1) {size = 1 << (highest_bit+1);}\
																							else {size = 1 << (highest_bit);} }


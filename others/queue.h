#pragma once
#include "memory_block.h"

typedef struct QUEUE_STRUCT{
	unsigned int	 type_id;
	const char	 	*name;	
	
	unsigned int	message_size;
	unsigned int	capacity;

	unsigned int  enqueued_number;
	unsigned int	available_storage;
	
	void					*queue_start;
	void  				*queue_end;
	
	void					*read_ptr;
	void					*write_ptr;
	
	
	struct QUEUE_STRUCT
								*queue_create_next,
								*queue_create_previous;
	
	struct KERNEL_MEMORY_CACHE
								*kmem_own;
}QUEUE;

extern QUEUE *os_queue_create_ptr;
extern QUEUE  os_queue;

unsigned int os_queue_create(QUEUE* queue_ptr,char *name,unsigned int message_size,void *queue_start,unsigned int queue_size);
unsigned int os_queue_delete(QUEUE* queue_ptr);
unsigned int os_queue_cleanup(QUEUE *queue_ptr);

unsigned int os_queue_receive(QUEUE *queue_ptr,void	*dest_ptr);
unsigned int os_queue_send(QUEUE	*queue_ptr,void *src_ptr);

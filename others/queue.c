#include "queue.h"
QUEUE* queue_create(char *name,unsigned int message_size,unsigned int queue_size)
{
	KERNEL_MEMORY_CACHE *kmem_ptr = kmem_malloc(sizeof(QUEUE) + message_size * queue_size);
	if(kmem_ptr)
	{
		QUEUE *queue_ptr = kmem_ptr->start;
		os_queue_create(queue_ptr,name,message_size,kmem_ptr->start + sizeof(QUEUE),queue_size);
		queue_ptr->kmem_own = kmem_ptr;
		return queue_ptr;
	}
	return NULL;
}

unsigned int os_queue_receive(QUEUE *queue_ptr,void	*dest_ptr)
{
	if(queue_ptr && dest_ptr)
	{
		if(queue_ptr->enqueued_number && queue_ptr->state == READY)
		{
			queue_ptr->state = BUSYING;
			memcpy(dest_ptr,queue_ptr->read_ptr,queue_ptr->message_size);
			queue_ptr->enqueued_number--;
			queue_ptr->available_storage++;
			queue_ptr->read_ptr += queue_ptr->message_size;
			if(queue_ptr->enqueued_number)
			{
				if(queue_ptr->read_ptr == queue_ptr->queue_end)
				{
					queue_ptr->read_ptr = queue_ptr->queue_start;
				}
			}
			else
			{
				queue_ptr->read_ptr = NULL;
			}
			queue_ptr->state = READY;
			return 0;
		}
		return 0xFFFFFF00;
	}
	return 0xFFFFFFFF;
}

unsigned int os_queue_send(QUEUE	*queue_ptr,void *src_ptr)
{
	if(queue_ptr && src_ptr)
	{
		if(queue_ptr->available_storage && queue_ptr->state == READY)
		{
			queue_ptr->state =	BUSYING;
			memcpy(queue_ptr->write_ptr,src_ptr,queue_ptr->message_size);
			if(queue_ptr->enqueued_number == 0)
			{
				queue_ptr->read_ptr = queue_ptr->write_ptr;
			}
			queue_ptr->enqueued_number++;
			queue_ptr->available_storage--;
			queue_ptr->write_ptr += queue_ptr->message_size;
			if(queue_ptr->write_ptr == queue_ptr->queue_end)
			{
				queue_ptr->write_ptr = queue_ptr->queue_start;
			}
			queue_ptr->state = READY;
			return 0;
		}
		return 0xFFFFFF00;
	}
	return 0xFFFFFFFF;
}



unsigned int os_queue_create(QUEUE* queue_ptr,char *name,unsigned int message_size,void *queue_start,unsigned int queue_size)
{
	if(queue_ptr && message_size && queue_start && queue_size)
	{
		queue_ptr->type_id = 				QUEUE_ID;
		queue_ptr->name =		 			name;
		queue_ptr->state =					NEW;
		queue_ptr->capacity = 				queue_size;
		queue_ptr->available_storage = 		queue_size;
		queue_ptr->enqueued_number =		0;
		queue_ptr->message_size =			message_size;
		queue_ptr->queue_start =			queue_start;
		queue_ptr->queue_end =				queue_start + message_size * queue_size;
		queue_ptr->read_ptr =				NULL;
		queue_ptr->write_ptr =				queue_ptr->queue_start;
		memset(queue_ptr->queue_start,0,message_size * queue_size);
		if(os_queue_create_ptr)
		{
			queue_ptr->queue_create_next = 			os_queue_create_ptr;
			queue_ptr->queue_create_previous =	os_queue_create_ptr->queue_create_previous;
			
			os_queue_create_ptr->queue_create_previous->queue_create_next = queue_ptr;
			os_queue_create_ptr->queue_create_previous =	queue_ptr;
		}
		else
		{
			queue_ptr->queue_create_next = 		queue_ptr;
			queue_ptr->queue_create_previous =	queue_ptr;
			os_queue_create_ptr =				queue_ptr;
		}
		queue_ptr->state =						READY;
		return 0;
	}
	return 0xFFFFFFFF;
}
unsigned int os_queue_delete(QUEUE* queue_ptr)
{
	if(queue_ptr)
	{
		if(os_queue_create_ptr == queue_ptr)
		{
			if(queue_ptr == queue_ptr->queue_create_next)
			{
				os_queue_create_ptr = NULL;
				kmem_free(queue_ptr->kmem_own);
				return 0;
			}
			os_queue_create_ptr = queue_ptr->queue_create_next;		
		}
		queue_ptr->queue_create_previous->queue_create_next = queue_ptr->queue_create_next;
		queue_ptr->queue_create_next->queue_create_previous = queue_ptr->queue_create_previous;
		queue_ptr->state =									  DELETED;
		kmem_free(queue_ptr->kmem_own);
		return 0;
	}
	return 0xFFFFFFFF;
}

unsigned int queue_cleanup(QUEUE *queue_ptr)
{
	if(queue_ptr && queue_ptr->state == READY)
	{

		queue_ptr->state =			   BUSYING;
		queue_ptr->available_storage = queue_ptr->capacity;
		queue_ptr->enqueued_number =   0;
		queue_ptr->write_ptr =		   queue_ptr->queue_start;
		queue_ptr->read_ptr =    	   NULL;
		queue_ptr->state =			   READY;
		return 0;
	}
	return 0xFFFFFFFF;
}

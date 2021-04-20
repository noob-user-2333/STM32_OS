#include "driver.h"
#include  <string.h>
#define FLASH_KEY_FPEC 0x00A5UL
#define FLASH_BUF_SIZE 1024

#define FLASH_WAIT() {	for(unsigned int retry = 0;retry < 0x1000 && FLASH->SR & 0x01;retry ++)\
		{\
			if(FLASH->SR & 0x01){\
			peripheral_ptr->peripheral_status = DRIVER_IO_ERROR;\
			return;}\
		}}

#define FLASH_ERASURE(address) 			{FLASH->CR |= 0x02;\
		FLASH->AR = (unsigned int)address;\
		FLASH->CR |= 0x40;\
		FLASH_WAIT();\
		FLASH->CR &= (~(0x42));\
		FLASH->CR &= (~(0x42));\
		FLASH->SR = 0x20;}


		
void	*os_unused_flash_ptr = (void*)FLASH_BEGIN;

		
DRIVER_PERIPHERAL flash={0};
unsigned char     flash_buf[FLASH_BUF_SIZE]={0};



void FLASH_driver(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr)
{
	if(peripheral_ptr->peripheral_id != DEVICE_FLASH_ID)
		return;
	if(peripheral_ptr->peripheral_data_reg_ptr < (void*)FLASH_BEGIN || peripheral_ptr->peripheral_data_reg_ptr >= (void*)(FLASH_BEGIN + FLASH_SIZE))
	{
		peripheral_ptr->peripheral_status = DRIVER_IO_ERROR;
		return;
	}
	
	switch (peripheral_ptr->peripheral_request)
  {

    case DRIVER_READ:
    {
				memcpy(peripheral_ptr->peripheral_buffer,peripheral_ptr->peripheral_data_reg_ptr,peripheral_ptr->peripheral_operate_bytes);
				peripheral_ptr->peripheral_status = DRIVER_SUCCESS;
        break;
    }

    case DRIVER_WRITE:
    {
				unsigned int offset;
				void 					*address_end;
				void 					*src =	 peripheral_ptr->peripheral_buffer;
				unsigned int size;
				FLASH->KEYR = FLASH_KEY1;
				FLASH->KEYR = FLASH_KEY2;
				if(FLASH->CR & 0x80)
				{
					peripheral_ptr->peripheral_status = DRIVER_IO_ERROR;
				}
			
				
				if(peripheral_ptr->peripheral_operate_bytes	& 0x01)
					peripheral_ptr->peripheral_operate_bytes++;
				size =  peripheral_ptr->peripheral_operate_bytes;
				address_end = peripheral_ptr->peripheral_data_reg_ptr + size;
				
				
				offset = ((unsigned int)peripheral_ptr->peripheral_data_reg_ptr << 22) >> 22;
				for(unsigned int address = (((unsigned int)peripheral_ptr->peripheral_data_reg_ptr  >> 10) << 10); address < FLASH_END && address <(unsigned int)address_end; offset = 0,address += 1024)
				{
				
					memcpy(peripheral_ptr->peripheral_memory_buffer,(void*)address,FLASH_BUF_SIZE);
				
				
					for(;offset <FLASH_BUF_SIZE && size;offset++,src++,size--)
					{
						((unsigned char*)(peripheral_ptr->peripheral_memory_buffer))[offset] = *(unsigned char*)src;
					}
					FLASH_ERASURE(address);
					FLASH->CR |= 0x01;
					for(unsigned int times=0;times<512;times++)
					{
						*(((unsigned short*)address)+times) = ((unsigned short*)peripheral_ptr->peripheral_memory_buffer)[times];
					}
					FLASH->CR &= (~0x01);
				}
				FLASH->CR = 0x80;
        peripheral_ptr->peripheral_status = DRIVER_SUCCESS;
        break;
    }
    case DRIVER_INIT:
    {

       peripheral_ptr->peripheral_status = DRIVER_SUCCESS;
       break;
    }

    case DRIVER_UNINIT:
    {

        /* There is nothing to do in this case for the RAM driver.  For actual
           devices some shutdown processing may be necessary.  */

        /* Successful driver request.  */
        peripheral_ptr->peripheral_status = DRIVER_SUCCESS;
        break;
    }
    default:
    {

        /* Invalid driver request.  */
        peripheral_ptr->peripheral_status = DRIVER_IO_ERROR;
        break;
    }
    }

}

//unsigned int BLOCK_FLASH_init(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,unsigned int mode)
//{
//	if(peripheral_ptr)
//	{
//		peripheral_ptr->state = UNINITIALIZE;
//		if(peripheral_ptr->dev_id == DEVICE_FLASH_ID)
//		{

//			peripheral_ptr->mode = 0;
//		
//			peripheral_ptr->buf_offset =0;
//			peripheral_ptr->buf_size = FLASH_BUF_SIZE;
//			peripheral_ptr->buf_start = flash_buf;
//			peripheral_ptr->buf_end =		flash_buf + FLASH_BUF_SIZE;
//		
//		
//			FLASH->KEYR = FLASH_KEY1;
//			FLASH->KEYR = FLASH_KEY2;
//		
//		
//			if(FLASH->CR & 0x80)
//				return 0xFFFF0000;
//			
//			peripheral_ptr->state = READY;
//			return 0;
//		}
//		return 0xFFFFFF00;
//	}
//	return 0xFFFFFFFF;
//}


//unsigned int BLOCK_FLASH_write(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void* src,void* dest,unsigned int size)
//{
//	if(peripheral_ptr && src && dest >= (void*)FLASH_BEGIN && size)
//	{
//	
//		if(peripheral_ptr->state == READY && peripheral_ptr->dev_id == DEVICE_FLASH_ID)
//		{
//			unsigned int	 offset;
//			unsigned short temp;
//			void 					 *address_end = dest;
//			if(size & 0x01)
//				size++;
//		
//			address_end+= size;

//			peripheral_ptr->state = BUSYING;
//			peripheral_ptr->buf_offset = 1024;
//			if(peripheral_ptr->buf_offset > peripheral_ptr->buf_size)
//			{
//				return 0xFF000000;
//			}
//			offset = ((unsigned int)dest << 22) >> 22;
//			for(unsigned int address = (((unsigned int)dest >> 10) << 10); address < FLASH_END && address <(unsigned int)address_end; offset = 0,address += 1024)
//			{
//				
//				memcpy(peripheral_ptr->buf_start,(void*)address,1024);
//				
//				
//				for(;offset <1024 && size;offset++,src++,size--)
//				{
//					((unsigned char*)(peripheral_ptr->buf_start))[offset] = *(unsigned char*)src;
//				}
//				FLASH_ERASURE(address);
//				FLASH->CR |= 0x01;
//				for(unsigned int times=0;times<512;times++)
//				{
//					*(((unsigned short*)address)+times) = *(((unsigned short*)peripheral_ptr->buf_start)+times);
//				}
//				FLASH->CR &= (~0x01);
//			}
//			peripheral_ptr->buf_offset = 0;
//			peripheral_ptr->state = READY;
//		
//			return 0;
//		}
//		return 0xFFFFFF00;
//	}
//	return 0xFFFFFFFF;
//}

//unsigned int BLOCK_FLASH_read(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void* buff,void* readAdd,unsigned int size)
//{
//	if(peripheral_ptr && buff && readAdd)
//	{
//		if(peripheral_ptr->state == READY && peripheral_ptr->dev_id == DEVICE_FLASH_ID)
//		{
//			if(readAdd < (void*)FLASH_BASE || readAdd >= (void*)FLASH_END)
//			{
//				return 0xFFFF0000;
//			}
//			peripheral_ptr->state = BUSYING;
//			memcpy(buff,readAdd,size);
//			peripheral_ptr->state = READY;
//			return 0;
//		}
//		return 0xFFFFFF00;
//	}
//	return 0xFFFFFFFF;
//}


void FLASH_INIT()
{
	memset(&flash,0,sizeof(flash));
	flash.name = "INNER_FLASH";
//	flash.type_id = DRIVER_TYPE_ID;
	flash.peripheral_id =						 DEVICE_FLASH_ID;
	flash.peripheral_memory_buffer = flash_buf;
	flash.peripheral_memory_size =	 FLASH_BUF_SIZE;
	flash.peripheral_request =			 DRIVER_UNINIT;
	flash.peripheral_status =				 DRIVER_IO_ERROR;
	flash.peripheral_driver =				 FLASH_driver;
	flash.mode = 										 0;
	flash.speed = 									 0;
//	flash.dev_id = DEVICE_FLASH_ID;
	flash.reg_base = FLASH;
	
	flash.peripheral_request =			 DRIVER_INIT;
	flash.peripheral_driver(&flash);
//	flash.peripheral_init = BLOCK_FLASH_init;
//	flash.peripheral_read = BLOCK_FLASH_read;
//	flash.peripheral_write =BLOCK_FLASH_write;
//	flash.peripheral_init(&flash,0);
}
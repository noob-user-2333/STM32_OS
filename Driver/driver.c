#include "driver.h"
//extern void SPI1_init();
extern void FLASH_INIT();
//extern void I2C1_init();
//extern void DMA1_init();
//extern void SD_Init();

void driver_init()
{
	RCC->APB2ENR |= 0x01UL;
//	DMA1_init();


	FLASH_INIT();
//	I2C1_init();
//	SPI1_init();


//	SD_Init();

}


DRIVER_STATE driver_read(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *memory_buffer,void *src,unsigned int size)
{
//	if(peripheral_ptr == NULL)
//	{
//		return dma1.dma_driver(NULL,src,memory_buffer,size);
//	}
//	
	peripheral_ptr->peripheral_buffer = 			 memory_buffer;
	peripheral_ptr->peripheral_operate_bytes = size;
	if(peripheral_ptr->peripheral_id == DEVICE_FLASH_ID)
	{
		peripheral_ptr->peripheral_data_reg_ptr = src;
	}
		
	peripheral_ptr->peripheral_request = DRIVER_READ;
	peripheral_ptr->peripheral_status =	 DRIVER_IO_ERROR;
	
	(peripheral_ptr->peripheral_driver)(peripheral_ptr);

	if(peripheral_ptr->peripheral_status == DRIVER_IO_ERROR)
	{
		peripheral_ptr->peripheral_request = DRIVER_UNINIT;
		peripheral_ptr->peripheral_status =	 DRIVER_IO_ERROR;
		
		(peripheral_ptr->peripheral_driver)(peripheral_ptr);
	}
	
	return peripheral_ptr->peripheral_status;
}
DRIVER_STATE driver_write(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *memory_src_buffer,void *dest,unsigned int size)
{
//	if(peripheral_ptr == NULL)
//	{
//		return dma1.dma_driver(NULL,memory_src_buffer,dest,size);
//	}
	
	peripheral_ptr->peripheral_buffer = 			 memory_src_buffer;
	peripheral_ptr->peripheral_operate_bytes = size;
	if(peripheral_ptr->peripheral_id == DEVICE_FLASH_ID)
	{
		peripheral_ptr->peripheral_data_reg_ptr = dest;
	}
		
	peripheral_ptr->peripheral_request = DRIVER_WRITE;
	peripheral_ptr->peripheral_status =	 DRIVER_IO_ERROR;
	
	(peripheral_ptr->peripheral_driver)(peripheral_ptr);

	if(peripheral_ptr->peripheral_status != DRIVER_SUCCESS)
	{
		peripheral_ptr->peripheral_request = DRIVER_UNINIT;
		peripheral_ptr->peripheral_status =	 DRIVER_IO_ERROR;
		
		(peripheral_ptr->peripheral_driver)(peripheral_ptr);
	}
	
	
	return peripheral_ptr->peripheral_status;
}


DRIVER_STATE driver_read_asyn(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *memory_buffer,void *src,unsigned int size)
{
	if(peripheral_ptr == NULL)
	{
		return DRIVER_IO_ERROR;
	}
	
	peripheral_ptr->peripheral_buffer = 			 memory_buffer;
	peripheral_ptr->peripheral_operate_bytes = size;
	if(peripheral_ptr->peripheral_id == DEVICE_FLASH_ID)
	{
		peripheral_ptr->peripheral_data_reg_ptr = src;
	}
		
	peripheral_ptr->peripheral_request = DRIVER_READ_ASYN;
	peripheral_ptr->peripheral_status =	 DRIVER_IO_ERROR;
	
	(peripheral_ptr->peripheral_driver)(peripheral_ptr);

	if(peripheral_ptr->peripheral_status == DRIVER_IO_ERROR)
	{
		peripheral_ptr->peripheral_request = DRIVER_UNINIT;
		peripheral_ptr->peripheral_status =	 DRIVER_IO_ERROR;
		
		(peripheral_ptr->peripheral_driver)(peripheral_ptr);
	}
	
	return peripheral_ptr->peripheral_status;
}

DRIVER_STATE driver_write_asyn(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *memory_src_buffer,void *dest,unsigned int size)
{
	if(peripheral_ptr == NULL)
	{
		return DRIVER_IO_ERROR;
	}
	peripheral_ptr->peripheral_buffer = 			 memory_src_buffer;
	peripheral_ptr->peripheral_operate_bytes = size;
	if(peripheral_ptr->peripheral_id == DEVICE_FLASH_ID)
	{
		peripheral_ptr->peripheral_data_reg_ptr = dest;
	}
		
	peripheral_ptr->peripheral_request = DRIVER_WRITE_ASYN;
	peripheral_ptr->peripheral_status =	 DRIVER_IO_ERROR;
	
	(peripheral_ptr->peripheral_driver)(peripheral_ptr);

	if(peripheral_ptr->peripheral_status != DRIVER_SUCCESS)
	{
		peripheral_ptr->peripheral_request = DRIVER_UNINIT;
		peripheral_ptr->peripheral_status =	 DRIVER_IO_ERROR;
		
		(peripheral_ptr->peripheral_driver)(peripheral_ptr);
	}
	
	
	return peripheral_ptr->peripheral_status;
}
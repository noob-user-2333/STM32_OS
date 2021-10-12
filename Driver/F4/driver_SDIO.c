#include "driver.h"


EXE_STATE SDIO_driver(const struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *buff,unsigned int size,REQUEST request);

const DRIVER_PERIPHERAL sdio = {
																};

																
EXE_STATE SDIO_driver(const struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *buff,unsigned int size,REQUEST request)
{
	SDIO_TypeDef *sdio_reg = peripheral_ptr->reg_base;
	if(peripheral_ptr->peripheral_id != DEVICE_SDIO_ID)
		return EXE_PARA_ERROR;
	if(peripheral_ptr->peripheral_data_reg_ptr && (buff && size || request == INIT))
	{
		switch(request)
		{
			case READ_ASYN:
			case READ:
			{
			
				break;
			}
			case WRITE_ASYN:
			case WRITE:
			{
			
				break;
			}
			case INIT:
			{	
				sdio_reg->CLKCR = 0x2000 + 0x200 + 0x100 + 0xFF;
				sdio_reg->CMD = 0x1000 + 0x400;
				
				
				
				
				
				
//				
//				sdio_reg->CLKCR = 0x2000 + 0x200 + 0x100 + 0x01;
			}
		}
		return EXE_SUCCESS;
	}
	return EXE_UNKNOW_ERROR;
}
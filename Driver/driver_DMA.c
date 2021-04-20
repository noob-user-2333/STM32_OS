#include "driver.h"


DRIVER_DMA_CONTROL dma1;

//typedef enum DMA_TRANS_STATE{
//	TRANSFER_ERROR,
//	TRANSFER_COMPLETE
//}DMA_TRANS_STATE;






//unsigned int DMA_write(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void* src,void* dest,unsigned int size)
//{
//	DRIVER_DMA_CHANNEL *dma_ptr;
//	if(peripheral_ptr && peripheral_ptr->dma_channel_tx && peripheral_ptr->dma_channel_tx->state == READY)
//	{
//			dma_ptr = peripheral_ptr->dma_channel_tx;
//			dma_ptr->state = BUSY_IN_WRITE;
//			dma_ptr->owner = peripheral_ptr;
//			dma_ptr->reg_base->CNDTR = size;
//			dma_ptr->reg_base->CMAR = (unsigned int)src;

//				if(peripheral_ptr->state == BUSYING)
//				{
//						dma_ptr->state = READY;
//						dma_ptr->owner = NULL;
//						return 0xFF000000;
//				}
//			
//				dma_ptr->reg_base->CPAR = (unsigned int)dest;
//				dma_ptr->reg_base->CCR = 0x9A;
//				
////				
////			}
//			if(dma_ptr->reg_base->CMAR && dma_ptr->reg_base->CPAR)
//			{
//				dma_ptr->reg_base->CCR |= 1;
//				return 0;
//			}
//			return 0xFFFF0000;
//	}
//	else if(peripheral_ptr == NULL)
//	{
//		//寻找空闲通道
//		for(unsigned channel = 0;channel < DMA_CHANNEL_NUMBER;channel++)
//		{
//			if(dma1.dma_channel[channel].state == READY)
//			{
//				dma_ptr = &dma1.dma_channel[channel];
//				dma_ptr->state = BUSYING;
//				dma_ptr->owner = NULL;
//				dma_ptr->reg_base->CNDTR = size;
//				dma_ptr->reg_base->CMAR = (unsigned int)src;
//				dma_ptr->reg_base->CPAR = (unsigned int)dest;
//				dma_ptr->reg_base->CCR = 0x40DA;
//				if(dma_ptr->reg_base->CMAR && dma_ptr->reg_base->CPAR)
//				{
//					dma_ptr->reg_base->CCR |= 1;
//					return 0;
//				}
//				return 0xFFFF0000;
//			}
//		}
//	}
//	return 0xFFFFFFFF;
//}



////src最高位位为1表示内存见传输数据
//unsigned int DMA_read(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void* buf,void* src,unsigned int size)
//{
//	DRIVER_DMA_CHANNEL *dma_ptr;
//	if(peripheral_ptr)
//	{
//		if(peripheral_ptr && peripheral_ptr->dma_channel_rx && peripheral_ptr->dma_channel_rx->state == READY)
//		{	
//				dma_ptr = peripheral_ptr->dma_channel_rx;
//				dma_ptr->state = BUSY_IN_READ;
//				dma_ptr->owner = peripheral_ptr;
//				dma_ptr->reg_base->CNDTR = size;
//				dma_ptr->reg_base->CMAR = (unsigned int)buf;

//					if(peripheral_ptr->state == BUSYING)
//					{
//						dma_ptr->state = READY;
//						dma_ptr->owner = NULL;
//						return 0xFF000000;
//					}
//					dma_ptr->reg_base->CPAR = (unsigned int)src;
//					dma_ptr->reg_base->CCR = 0x8A;
//					
//				
//				//dma_ptr->state = READY;
//				if(dma_ptr->reg_base->CMAR && dma_ptr->reg_base->CPAR)
//				{
//					dma_ptr->reg_base->CCR |= 1;
//					return 0;
//				}
//				return 0xFFFF0000;
//		}
//	}
//	else
//	{
//		//寻找空闲通道
//		for(unsigned channel = 0;channel < DMA_CHANNEL_NUMBER;channel++)
//		{
//			if(dma1.dma_channel[channel].state == READY)
//			{
//				dma_ptr = &dma1.dma_channel[channel];
//				dma_ptr->state = BUSYING;
//				dma_ptr->owner = NULL;
//				dma_ptr->reg_base->CNDTR = size;
//				dma_ptr->reg_base->CMAR = (unsigned int)buf;
//				dma_ptr->reg_base->CPAR = (unsigned int)src;
//				dma_ptr->reg_base->CCR = 0x40CA;
//				if(dma_ptr->reg_base->CMAR && dma_ptr->reg_base->CPAR)
//				{
//					dma_ptr->reg_base->CCR |= 1;
//					return 0;
//				}
//				return 0xFFFF0000;
//			}
//		}
//	
//	}
//	return 0xFFFFFFFF;
//}







unsigned int DMA_driver(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *src,void *dest,unsigned int size)
{
	DRIVER_DMA_CHANNEL *channel_ptr;
	if(peripheral_ptr == NULL)
	{
		for(unsigned channel = 0;channel < DMA_CHANNEL_NUMBER;channel++)
		{
			if(dma1.dma_channel[channel].channel_status != DRIVER_IO_BUSYING)
			{
				channel_ptr = &dma1.dma_channel[channel];
				channel_ptr->channel_request = DRIVER_READ_ASYN;
				channel_ptr->channel_status = DRIVER_IO_ERROR;
				channel_ptr->used_peripheral = NULL;
				channel_ptr->reg_base->CNDTR = size;
				channel_ptr->reg_base->CMAR = (unsigned int)dest;
				channel_ptr->reg_base->CPAR = (unsigned int)src;
				channel_ptr->reg_base->CCR = 0x40CA;
				if(channel_ptr->reg_base->CMAR && channel_ptr->reg_base->CPAR)
				{
					channel_ptr->channel_status = DRIVER_IO_BUSYING;
					channel_ptr->reg_base->CCR |= 1;
					return DRIVER_SUCCESS;
				}
				break;
			}
		}
		if(channel_ptr->translate_error_notify)
		{
			(channel_ptr->translate_error_notify)(channel_ptr);
		}
		return DRIVER_IO_ERROR;
	}
	
	
	
	
	
	
	switch(peripheral_ptr->peripheral_request)
	{
	  case DRIVER_READ_ASYN:
    {
				channel_ptr = peripheral_ptr->dma_channel_rx;
			
				channel_ptr->channel_request = DRIVER_READ_ASYN;
				channel_ptr->channel_status = DRIVER_IO_ERROR;
				peripheral_ptr->peripheral_status = DRIVER_IO_ERROR;
			
				channel_ptr->reg_base->CMAR = (unsigned int)(peripheral_ptr->peripheral_buffer);
				channel_ptr->reg_base->CPAR = (unsigned int)(peripheral_ptr->peripheral_data_reg_ptr);
				channel_ptr->reg_base->CNDTR = peripheral_ptr->peripheral_operate_bytes;
				channel_ptr->reg_base->CCR = 0x8A;
			
				if(channel_ptr->reg_base->CMAR && channel_ptr->reg_base->CPAR)
				{
					channel_ptr->used_peripheral = peripheral_ptr;
					peripheral_ptr->peripheral_status = DRIVER_IO_BUSYING;
					channel_ptr->channel_status = DRIVER_IO_BUSYING;
					channel_ptr->reg_base->CCR |= 1;
				}
				break;
    }

    case DRIVER_WRITE_ASYN:
    {
				channel_ptr = peripheral_ptr->dma_channel_tx;
			
				channel_ptr->channel_request = DRIVER_WRITE_ASYN;
				channel_ptr->channel_status = DRIVER_IO_ERROR;
				peripheral_ptr->peripheral_status = DRIVER_IO_ERROR;
			
				channel_ptr->reg_base->CMAR = (unsigned int)(peripheral_ptr->peripheral_buffer);
				channel_ptr->reg_base->CPAR = (unsigned int)(peripheral_ptr->peripheral_data_reg_ptr);
				channel_ptr->reg_base->CNDTR = peripheral_ptr->peripheral_operate_bytes;
				channel_ptr->reg_base->CCR = 0x9A;
			
        if(channel_ptr->reg_base->CMAR && channel_ptr->reg_base->CPAR)
				{
					channel_ptr->used_peripheral = peripheral_ptr;
					peripheral_ptr->peripheral_status = DRIVER_IO_BUSYING;
					channel_ptr->channel_status = DRIVER_IO_BUSYING;
					channel_ptr->reg_base->CCR |= 1;
				}
        break;
    }
    case DRIVER_INIT:
    {

       channel_ptr->channel_status = DRIVER_SUCCESS;
       break;
    }

    case DRIVER_UNINIT:
    {

        /* There is nothing to do in this case for the RAM driver.  For actual
           devices some shutdown processing may be necessary.  */

        /* Successful driver request.  */
        channel_ptr->channel_status = DRIVER_SUCCESS;
        break;
    }
    default:
    {
        /* Invalid driver request.  */
        channel_ptr->channel_status = DRIVER_IO_ERROR;
        break;
    }
   }
		if(channel_ptr->channel_status == DRIVER_IO_ERROR)
		{
			channel_ptr->reg_base->CCR = 0;
			if(channel_ptr->translate_error_notify)
			{
				channel_ptr->translate_error_notify(channel_ptr);
			}
			if(channel_ptr->used_peripheral && channel_ptr->used_peripheral->peripheral_error_notify)
			{
				(channel_ptr->used_peripheral->peripheral_error_notify)(peripheral_ptr);
			}
			channel_ptr->used_peripheral = NULL;
		}
	 
	 
	 return channel_ptr->channel_status;
}

void DMA1_init()
{
	memset(&dma1,0,sizeof(DRIVER_DMA_CONTROL));
	dma1.name =			"DMA1";
	dma1.reg_base = DMA1;
	
	
	RCC->AHBENR |= 0x01UL;
	NVIC->ISER[0] = (0x7f << 11);
	for(unsigned int i=0;i < DMA_CHANNEL_NUMBER;i++)
	{
		dma1.dma_channel[i].dma_owner = &dma1;
		dma1.dma_channel[i].reg_base = (DMA_Channel_TypeDef*)(((unsigned char*)DMA1_Channel1) + i * 20);
		dma1.dma_channel[i].channel_status = DRIVER_IO_ERROR;
		dma1.dma_channel[i].channel_request = DRIVER_UNINIT;
		dma1.dma_channel[i].used_peripheral = NULL;
		dma1.dma_channel[i].translate_complete_notify = NULL;
		dma1.dma_channel[i].translate_error_notify = NULL;
	}
	dma1.dma_driver = DMA_driver;
}


void DMA1_IRQHandler(struct DRIVER_DMA_CHANNEL_STRUCT *channel_ptr)
{
	DMA_TypeDef 					*dma_reg = 		channel_ptr->dma_owner->reg_base;
	DMA_Channel_TypeDef   *channel_reg =channel_ptr->reg_base;
	unsigned int channel_number;
	unsigned int flag_shift;
	for(channel_number=0;channel_number < DMA_CHANNEL_NUMBER;channel_number++)
	{
		if(channel_ptr == (channel_ptr->dma_owner->dma_channel + channel_number))
			break;
	}
	flag_shift = channel_number << 2;
	//清除通道使能位
	channel_reg->CCR = 0UL;
	//确定传输状态并清除标志位
	dma_reg->IFCR = (0x04 << flag_shift);
	if(dma_reg->ISR & (0x02 << flag_shift) )//传输成功
	{
		DMA1->IFCR = (0x01 << flag_shift);
		DMA1->IFCR = (0x02 << flag_shift);
		channel_ptr->channel_status = DRIVER_SUCCESS;
		if(channel_ptr->used_peripheral)
		{
			channel_ptr->used_peripheral->peripheral_status = DRIVER_SUCCESS;
			if(channel_ptr->used_peripheral->peripheral_complete_notify)
			{
				(channel_ptr->used_peripheral->peripheral_complete_notify)(channel_ptr->used_peripheral);
			}
		}
		if(channel_ptr->translate_complete_notify)
		{
			channel_ptr->translate_complete_notify(channel_ptr);
		}
	}
	else//传输失败
	{
		DMA1->IFCR = (0x01 << flag_shift);
		DMA1->IFCR = (0x08 << flag_shift);
		channel_ptr->channel_status = DRIVER_IO_ERROR;
		if(channel_ptr->used_peripheral)
		{
			channel_ptr->used_peripheral->peripheral_status = DRIVER_IO_ERROR;
			if(channel_ptr->used_peripheral->peripheral_error_notify)
			{
				(channel_ptr->used_peripheral->peripheral_error_notify)(channel_ptr->used_peripheral);
			}
		}
		if(channel_ptr->translate_error_notify)
		{
			channel_ptr->translate_error_notify(channel_ptr);
		}
	}
	channel_ptr->used_peripheral = NULL;
}
//	if(state == TRANSFER_COMPLETE)
//	{
//		dma1.dma_channel[index_in_dma_control_struct].reg_base->CCR = 0;
//		if(dma1.dma_channel[index_in_dma_control_struct ].owner)
//		{
//			if((dma1.dma_channel + index_in_dma_control_struct) == dma1.dma_channel[index_in_dma_control_struct].owner->dma_channel_rx)
//			{
//				dma1.dma_channel[index_in_dma_control_struct].owner->peripheral_complete(dma1.dma_channel[index_in_dma_control_struct].owner,READ_COMPLETE);
//			}
//			else
//			{
//				dma1.dma_channel[index_in_dma_control_struct].owner->peripheral_complete(dma1.dma_channel[index_in_dma_control_struct].owner,WRITE_COMPLETE);
//			}
//		}			
//		dma1.dma_channel[index_in_dma_control_struct].state = READY;
//		dma1.dma_channel[index_in_dma_control_struct].owner = NULL;
//	}
//	else
//	{
//		dma1.dma_channel[index_in_dma_control_struct].reg_base->CCR = 0;
//		dma1.dma_channel[index_in_dma_control_struct].state = IS_ERROR;
//		if(dma1.dma_channel[index_in_dma_control_struct].owner)
//		{
//			dma1.dma_channel[index_in_dma_control_struct].owner->peripheral_error_handle(dma1.dma_channel[index_in_dma_control_struct].owner,0);
//		}
//	}
//}





void DMA1_Channel1_IRQHandler()
{
//	DMA1->IFCR = (0x04 << 0);
//	if(DMA1->ISR & (0x02 << 0) )
//	{
//		DMA1->IFCR = (0x01 << 0);
//		DMA1->IFCR = (0x02 << 0);
//		dma1.dma_channel[0].state = DRIVER_SUCCESS;
//	}
//	else
//	{
//		DMA1->IFCR = (0x01 << 0);
//		DMA1->IFCR = (0x08 << 0);
//		dma1.dma_channel[0].state = DRIVER_IO_ERROR;
//	}
	DMA1_IRQHandler(&dma1.dma_channel[0]);
}


void DMA1_Channel2_IRQHandler()
{
//	DMA1->IFCR = (0x04 << 4);
//	if(DMA1->ISR & (0x02 << 4) )
//	{
//		DMA1->IFCR = (0x01 << 4);
//		DMA1->IFCR = (0x02 << 4);
//		dma1.dma_channel[0].state = DRIVER_SUCCESS;
//	}
//	else
//	{
//		DMA1->IFCR = (0x01 << 4);
//		DMA1->IFCR = (0x08 << 4);
//		
//	}
	DMA1_IRQHandler(&dma1.dma_channel[1]);
}
void DMA1_Channel3_IRQHandler()
{
//	DMA1->IFCR = (0x04 << 8);
//	if(DMA1->ISR & (0x02 << 8) )
//	{
//		DMA1->IFCR = (0x01 << 8);
//		DMA1->IFCR = (0x02 << 8);
//		dma1.dma_channel[0].state = DRIVER_SUCCESS;
//	}
//	else
//	{
//		DMA1->IFCR = (0x01 << 8);
//		DMA1->IFCR = (0x08 << 8);
//		
//	}
	DMA1_IRQHandler(&dma1.dma_channel[2]);
}
void DMA1_Channel4_IRQHandler()
{
//	DMA1->IFCR = (0x04 << 12);
//	if(DMA1->ISR & (0x02 << 12) )
//	{
//		DMA1->IFCR = (0x01 << 12);
//		DMA1->IFCR = (0x02 << 12);
//		dma1.dma_channel[0].state = DRIVER_SUCCESS;
//	}
//	else
//	{
//		DMA1->IFCR = (0x01 << 12);
//		DMA1->IFCR = (0x08 << 12);
//		
//	}
	DMA1_IRQHandler(&dma1.dma_channel[3]);
}
void DMA1_Channel5_IRQHandler()
{
//	DMA1->IFCR = (0x04 << 16);
//	if(DMA1->ISR & (0x02 << 16) )
//	{
//		DMA1->IFCR = (0x01 << 16);
//		DMA1->IFCR = (0x02 << 16);
//		dma1.dma_channel[0].state = DRIVER_SUCCESS;
//	}
//	else
//	{
//		DMA1->IFCR = (0x01 << 16);
//		DMA1->IFCR = (0x08 << 16);
//		
//	}
	DMA1_IRQHandler(&dma1.dma_channel[4]);
}
void DMA1_Channel6_IRQHandler()
{
//	DMA1->IFCR = (0x04 << 20);
//	if(DMA1->ISR & (0x02 << 20) )
//	{
//		DMA1->IFCR = (0x01 << 20);
//		DMA1->IFCR = (0x02 << 20);
//		dma1.dma_channel[0].state = DRIVER_SUCCESS;
//	}
//	else
//	{
//		DMA1->IFCR = (0x01 << 20);
//		DMA1->IFCR = (0x08 << 20);
//		
//	}
	DMA1_IRQHandler(&dma1.dma_channel[5]);
}
void DMA1_Channel7_IRQHandler()
{
//	DMA1->IFCR = (0x04 << 24);
//	if(DMA1->ISR & (0x02 << 24) )
//	{
//		DMA1->IFCR = (0x01 << 24);
//		DMA1->IFCR = (0x02 << 24);
//		dma1.dma_channel[0].state = DRIVER_SUCCESS;
//	}
//	else
//	{
//		DMA1->IFCR = (0x01 << 24);
//		DMA1->IFCR = (0x08 << 24);
//		
//	}
	DMA1_IRQHandler(&dma1.dma_channel[6]);
}

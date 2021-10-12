#include "driver.h"
DRIVER_DMA_CONTROL dma1;








EXE_STATE DMA_peripheral_driver(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *src,unsigned int size,REQUEST request)
{
	DRIVER_DMA_CHANNEL *channel_ptr;
	if(size == 0 || src == NULL || peripheral_ptr == NULL || peripheral_ptr->peripheral_data_reg_ptr == NULL)
		return EXE_PARA_ERROR;

	switch(request)
	{
	  case READ_ASYN:
    {
				channel_ptr = peripheral_ptr->dma_channel_rx;
			
				if(channel_ptr->reg_base->CCR & 0x01)
				{
					return EXE_DEV_BUSYING;
				}
			
				channel_ptr->reg_base->CMAR = (unsigned int)(src);
				channel_ptr->reg_base->CPAR = (unsigned int)(peripheral_ptr->peripheral_data_reg_ptr);
				channel_ptr->reg_base->CNDTR = size;
				channel_ptr->reg_base->CCR = 0x8A;
			
				if(channel_ptr->reg_base->CMAR && channel_ptr->reg_base->CPAR)
				{
					channel_ptr->reg_base->CCR |= 1;
					return EXE_SUCCESS;
				}
				break;
    }
		
    case WRITE_ASYN:
    {
				channel_ptr = peripheral_ptr->dma_channel_tx;
				
				if(channel_ptr->reg_base->CCR & 0x01)
				{
					return EXE_DEV_BUSYING;
				}
			

			
				channel_ptr->reg_base->CMAR = (unsigned int)(src);
				channel_ptr->reg_base->CPAR = (unsigned int)(peripheral_ptr->peripheral_data_reg_ptr);
				channel_ptr->reg_base->CNDTR = size;
				channel_ptr->reg_base->CCR = 0x9A;
			
        if(channel_ptr->reg_base->CMAR && channel_ptr->reg_base->CPAR)
				{
					channel_ptr->reg_base->CCR |= 1;
					return EXE_SUCCESS;
				}
        break;
    }
    case INIT:
    {

       return EXE_SUCCESS;
    }

    case UNINIT:
    {

        /* There is nothing to do in this case for the RAM driver.  For actual
           devices some shutdown processing may be necessary.  */

        /* Successful driver request.  */
        return EXE_SUCCESS;
    }
    default:
    {
        /* Invalid driver request.  */
        return EXE_PARA_ERROR;
    }
	}
	 
	 
	 return EXE_UNKNOWN_ERROR;
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
		dma1.dma_channel[i].translate_complete_notify = NULL;
		dma1.dma_channel[i].translate_error_notify = NULL;
	}
	dma1.dma_driver_periprheral = DMA_peripheral_driver;
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
		
		if(channel_ptr->translate_complete_notify)
		{
			channel_ptr->translate_complete_notify();
		}
	}
	else//传输失败
	{
		DMA1->IFCR = (0x01 << flag_shift);
		DMA1->IFCR = (0x08 << flag_shift);
		if(channel_ptr->translate_error_notify)
		{
			channel_ptr->translate_error_notify();
		}
	}
}





void DMA1_Channel1_IRQHandler()
{

	DMA1_IRQHandler(&dma1.dma_channel[0]);
}


void DMA1_Channel2_IRQHandler()
{

	DMA1_IRQHandler(&dma1.dma_channel[1]);
}
void DMA1_Channel3_IRQHandler()
{

	DMA1_IRQHandler(&dma1.dma_channel[2]);
}
void DMA1_Channel4_IRQHandler()
{

	DMA1_IRQHandler(&dma1.dma_channel[3]);
}
void DMA1_Channel5_IRQHandler()
{

	DMA1_IRQHandler(&dma1.dma_channel[4]);
}
void DMA1_Channel6_IRQHandler()
{

	DMA1_IRQHandler(&dma1.dma_channel[5]);
}
void DMA1_Channel7_IRQHandler()
{

	DMA1_IRQHandler(&dma1.dma_channel[6]);
}

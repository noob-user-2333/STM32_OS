#include "driver.h"



EXE_STATE DMA_peripheral_driver(const struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void* buff,unsigned int size,REQUEST request);
DMA_Stream_TypeDef* DMA_memory_driver(void *src,void *dest,unsigned int size,REQUEST request);
static void (*dma_complate_notify[2][DMA_STREAM_NUMBER])(void);
static void (*dma_error_notify[2][DMA_STREAM_NUMBER])(void);





const DMA_CONTROL dma[2]={ [0].dma_peripheral_driver = DMA_peripheral_driver,[0].dma_memory_driver = NULL,
													[0].name = "DMA1",
													[0].reg_base = DMA1,[0].dma_stream_number = DMA_STREAM_NUMBER,
													[0].dma_stream_reg_base[0] = DMA1_Stream0,
													[0].dma_stream_reg_base[1] = DMA1_Stream1,
													[0].dma_stream_reg_base[2] = DMA1_Stream2,
													[0].dma_stream_reg_base[3] = DMA1_Stream3,
													[0].dma_stream_reg_base[4] = DMA1_Stream4,
													[0].dma_stream_reg_base[5] = DMA1_Stream5,
													[0].dma_stream_reg_base[6] = DMA1_Stream6,
													[0].dma_stream_reg_base[7] = DMA1_Stream7,
													[0].translate_complete_notify = dma_complate_notify[0],
											    [0].translate_error_notify = dma_error_notify[0],
	
													[1].dma_peripheral_driver = DMA_peripheral_driver,[1].dma_memory_driver = DMA_memory_driver,
													[1].name = "DMA2",
													[1].reg_base = DMA2,[1].dma_stream_number = DMA_STREAM_NUMBER,
													[1].dma_stream_reg_base[0] = DMA2_Stream0,
													[1].dma_stream_reg_base[1] = DMA2_Stream1,
													[1].dma_stream_reg_base[2] = DMA2_Stream2,
													[1].dma_stream_reg_base[3] = DMA2_Stream3,
													[1].dma_stream_reg_base[4] = DMA2_Stream4,
													[1].dma_stream_reg_base[5] = DMA2_Stream5,
													[1].dma_stream_reg_base[6] = DMA2_Stream6,
													[1].dma_stream_reg_base[7] = DMA2_Stream7,
													[1].translate_complete_notify = dma_complate_notify[1],
													[1].translate_error_notify = dma_error_notify[1]
};



DMA_Stream_TypeDef* DMA_memory_driver(void *src,void *dest,unsigned int size,REQUEST request)
{
		DMA_Stream_TypeDef *stream_ptr;
	
		unsigned int align = 0;
		if(size == 0)
			return NULL;
		
		//align
		switch((~((unsigned int)src ^ (unsigned int)dest))%4)
		{
			case 1:
			{
				align = 1;
				size >>= 1;
				break;
			}
			case 3:
			{
				align = 2;
				size >>= 2;
			}
		}
				
		for(unsigned int stream_number = 0;stream_number < dma[1].dma_stream_number;stream_number++)
		{
			stream_ptr = dma[1].dma_stream_reg_base[stream_number];
			if(((stream_ptr->CR) & 0x01) == 0)	
				break;
		}
		if((stream_ptr->CR) & 0x01)
			return NULL;
				//memory_copy is this status
		if(request == WRITE )
		{
			stream_ptr->CR = 0x696 + 0x30000 + align * 0x800 + align * 0x2000;		
		}
		else//memory_set is DRIVER_READ
		{
			stream_ptr->CR = 0x496 + 0x30000 + align * 0x800 + align * 0x2000;
		}
				
				

		stream_ptr->NDTR = size;
		stream_ptr->PAR = (unsigned int)src;
		stream_ptr->M0AR = (unsigned int)dest;
				
				
		if(stream_ptr->M0AR && stream_ptr->PAR)
		{

			stream_ptr->CR |= 1;
			return stream_ptr;
		}
		return NULL;
}



EXE_STATE DMA_peripheral_driver(const struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void* buff,unsigned int size,REQUEST request)
{
	DMA_Stream_TypeDef *stream_ptr;
	if(peripheral_ptr && buff && size)
	{
		switch(request)
		{
			case READ_ASYN:
			{
					stream_ptr = peripheral_ptr->dma_stream_rx;
			
					if(stream_ptr->CR & 0x01)
					{
						return EXE_DEVICE_BUSYING;
					}
			

			
					stream_ptr->M0AR = (unsigned int)buff;
					stream_ptr->PAR = (unsigned int)(peripheral_ptr->peripheral_data_reg_ptr);
					stream_ptr->NDTR = size;
					stream_ptr->CR = 0x416 + (peripheral_ptr->dma_channel << 25);
			
					if(stream_ptr->M0AR && stream_ptr->PAR)
					{
						stream_ptr->CR |= 1;
					}
					else
					{
						return EXE_UNKNOW_ERROR;
					}
					break;
			}

			case WRITE_ASYN:
			{
					stream_ptr = peripheral_ptr->dma_stream_tx;
				
					if(stream_ptr->CR & 0x01)
					{
						return EXE_DEVICE_BUSYING;
					}
			
			
					stream_ptr->M0AR = (unsigned int)(buff);
					stream_ptr->PAR = (unsigned int)(peripheral_ptr->peripheral_data_reg_ptr);
					stream_ptr->NDTR = size;
					stream_ptr->CR = 0x456 + (peripheral_ptr->dma_channel << 25);
			
					if(stream_ptr->M0AR && stream_ptr->PAR)
					{
						stream_ptr->CR |= 1;
					}
					else
					{
						return EXE_UNKNOW_ERROR;
					}
					break;
			}
			default:
			{
					/* Invalid driver request.  */
					return EXE_PARA_ERROR;
			}
		}	 
		return EXE_SUCCESS;
	}
	else
		return EXE_PARA_ERROR;
}


int DMA_stream_state_get(DMA_Stream_TypeDef *stream_ptr,unsigned int *busying,unsigned long *complete_func,unsigned long *error_func)
{
	if(stream_ptr == NULL)
		return -1;
	unsigned int stream_number;
	const DMA_CONTROL *dma_ptr;
	for(dma_ptr = dma;dma_ptr <= dma + DMA_NUMBER - 1;dma_ptr++)
	{
		for(stream_number = 0;stream_number < DMA_STREAM_NUMBER;stream_number++)
		{
			if(stream_ptr == dma_ptr->dma_stream_reg_base[stream_number])
				break;
		}
	}
	if(stream_number == DMA_STREAM_NUMBER)
		return -1;
	if(busying)
		*busying = (stream_ptr->CR & 0x01);
	if(complete_func)
		*complete_func = (unsigned long)dma_ptr->translate_complete_notify[stream_number];
	if(error_func)
		*error_func = (unsigned long)dma_ptr->translate_error_notify[stream_number];
	
	return 0;
	
}

















int DMA_stream_func_set(DMA_Stream_TypeDef *stream_ptr,void (*complete_func)(),void (*error_func)())
{
	if(stream_ptr == NULL)
		return -1;
	unsigned int stream_number;
	const DMA_CONTROL *dma_ptr;
	for(dma_ptr = dma;dma_ptr <= dma + DMA_NUMBER - 1;dma_ptr++)
	{
		for(stream_number = 0;stream_number < DMA_STREAM_NUMBER;stream_number++)
		{
			if(stream_ptr == dma_ptr->dma_stream_reg_base[stream_number])
				break;
		}
	}
	if(stream_number == DMA_STREAM_NUMBER)
		return -1;

	if(complete_func)
		dma_ptr->translate_complete_notify[stream_number] = complete_func;
	if(error_func)
		dma_ptr->translate_error_notify[stream_number] = error_func;
	return 0;
}
int DMA_stream_func_clear(DMA_Stream_TypeDef *stream_ptr,unsigned int clear_complete_func,unsigned int clear_error_func)
{
	if(stream_ptr == NULL)
		return -1;
	unsigned int stream_number;
	const DMA_CONTROL *dma_ptr;
	for(dma_ptr = dma;dma_ptr <= dma + DMA_NUMBER - 1;dma_ptr++)
	{
		for(stream_number = 0;stream_number < DMA_STREAM_NUMBER;stream_number++)
		{
			if(stream_ptr == dma_ptr->dma_stream_reg_base[stream_number])
				break;
		}
	}
	if(stream_number == DMA_STREAM_NUMBER)
		return -1;
	if(clear_complete_func)
		dma_ptr->translate_complete_notify[stream_number] = NULL;
	if(clear_error_func)
		dma_ptr->translate_error_notify[stream_number] = NULL;
	
	return 0;
}

void DMA_init()
{
	RCC->AHB1ENR |= 0x600001UL;

	
	NVIC->ISER[0] |= (0x7f << 11);
	NVIC->ISER[1] |= (0x01 << 15);
	NVIC->ISER[1] |= (0x1f << 24);
	NVIC->ISER[2] |= (0x07 << 4);
	
	for(unsigned int i=0;i<DMA_STREAM_NUMBER;i++)
	{
		dma[0].translate_complete_notify[i] = NULL;
		dma[0].translate_error_notify[i] = NULL;
		
		dma[1].translate_complete_notify[i] = NULL;
		dma[1].translate_error_notify[i] = NULL;
	}

}




RAM_FUNC void DMA_IRQHandler(const DMA_CONTROL *dma_ptr,DMA_Stream_TypeDef *stream_ptr)
{
	DMA_TypeDef 					*dma_reg = 		dma_ptr->reg_base;
	unsigned int stream_number;
	unsigned int shift_bits;
	unsigned int	*flag_reg;
	unsigned int	*flag_clear_reg;
	
	for(stream_number=0;stream_number < DMA_STREAM_NUMBER;stream_number++)
	{
		if(stream_ptr == dma_ptr->dma_stream_reg_base[stream_number])
			break;
	}
	//清除通道使能位
	stream_ptr->CR = 0UL;
	//确定传输状态并清除标志位
	if(stream_number < 4)
	{
		flag_reg = (unsigned int*)&(dma_reg->LISR);
		flag_clear_reg = (unsigned int*)&(dma_reg->LIFCR);
	}	
	else
	{
		flag_reg = (unsigned int*)&(dma_reg->HISR);
		flag_clear_reg = (unsigned int*)&(dma_reg->HIFCR);
		stream_number -= 4;
	}
	
	switch(stream_number)
	{
		case 0:
		{
			shift_bits = 0;
			break;
		}
		case 1:
		{
			shift_bits = 6;
			break;
		}
		case 2:
		{
			shift_bits = 16;
			break;
		}
		case 3:
		{
			shift_bits = 22;
			break;
		}
	}
	
	
	*flag_clear_reg = (0x10 << shift_bits);
	if(*flag_reg & (0x20 << shift_bits)) //传输成功
	{
		*flag_clear_reg = (0x20 << shift_bits);
		if(dma_ptr->translate_complete_notify[stream_number])
			dma_ptr->translate_complete_notify[stream_number]();
//		
//		stream_ptr->stream_status = DRIVER_SUCCESS;
//		if(stream_ptr->used_peripheral)
//		{
//			stream_ptr->used_peripheral->peripheral_status = DRIVER_SUCCESS;
//			if(stream_ptr->used_peripheral->peripheral_complete_notify)
//			{
//				(stream_ptr->used_peripheral->peripheral_complete_notify)(stream_ptr->used_peripheral);
//			}
//		}
//		if(stream_ptr->translate_complete_notify)
//		{
//			stream_ptr->translate_complete_notify(stream_ptr);
//		}
	}
	else//传输失败
	{
		*flag_clear_reg = (0x08 << shift_bits);
		if(dma_ptr->translate_error_notify[stream_number])
			dma_ptr->translate_error_notify[stream_number]();
//		stream_ptr->stream_status = DRIVER_IO_ERROR;
//		if(stream_ptr->used_peripheral)
//		{
//			stream_ptr->used_peripheral->peripheral_status = DRIVER_IO_ERROR;
//			if(stream_ptr->used_peripheral->peripheral_error_notify)
//			{
//				(stream_ptr->used_peripheral->peripheral_error_notify)(stream_ptr->used_peripheral);
//			}
//		}
//		if(stream_ptr->translate_error_notify)
//		{
//			stream_ptr->translate_error_notify(stream_ptr);
//		}
	}
//	stream_ptr->used_peripheral = NULL;
}



RAM_FUNC void DMA1_Stream0_IRQHandler()
{
	DMA_IRQHandler(&dma[0],DMA1_Stream0);
}

RAM_FUNC void DMA1_Stream1_IRQHandler()
{

	DMA_IRQHandler(&dma[0],DMA1_Stream1);
}


RAM_FUNC void DMA1_Stream2_IRQHandler()
{

	DMA_IRQHandler(&dma[0],DMA1_Stream2);
}
RAM_FUNC void DMA1_Stream3_IRQHandler()
{

	DMA_IRQHandler(&dma[0],DMA1_Stream3);
}
RAM_FUNC void DMA1_Stream4_IRQHandler()
{

	DMA_IRQHandler(&dma[0],DMA1_Stream4);
}
RAM_FUNC void DMA1_Stream5_IRQHandler()
{

	DMA_IRQHandler(&dma[0],DMA1_Stream5);
}
RAM_FUNC void DMA1_Stream6_IRQHandler()
{

	DMA_IRQHandler(&dma[0],DMA1_Stream6);
}
RAM_FUNC void DMA1_Stream7_IRQHandler()
{

	DMA_IRQHandler(&dma[0],DMA1_Stream7);
}


RAM_FUNC void DMA2_Stream0_IRQHandler()
{
	DMA_IRQHandler(&dma[1],DMA2_Stream0);
}

RAM_FUNC void DMA2_Stream1_IRQHandler()
{

	DMA_IRQHandler(&dma[1],DMA2_Stream1);
}

RAM_FUNC void DMA2_Stream2_IRQHandler()
{

	DMA_IRQHandler(&dma[1],DMA2_Stream2);
}
RAM_FUNC void DMA2_Stream3_IRQHandler()
{

	DMA_IRQHandler(&dma[1],DMA2_Stream3);
}
RAM_FUNC void DMA2_Stream4_IRQHandler()
{

	DMA_IRQHandler(&dma[1],DMA2_Stream4);
}
RAM_FUNC void DMA2_Stream5_IRQHandler()
{

	DMA_IRQHandler(&dma[1],DMA2_Stream5);
}
RAM_FUNC void DMA2_Stream6_IRQHandler()
{

	DMA_IRQHandler(&dma[1],DMA2_Stream6);
}
RAM_FUNC void DMA2_Stream7_IRQHandler()
{

	DMA_IRQHandler(&dma[1],DMA2_Stream7);
}

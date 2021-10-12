#include "driver.h"


static unsigned char *spi_buff;
static unsigned int	 spi_offset = 0;


EXE_STATE SPI_driver(const struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *buff,unsigned int size,REQUEST request)
{
	SPI_TypeDef *spi_reg = peripheral_ptr->reg_base;
	if(peripheral_ptr->peripheral_id != DEVICE_UART_ID)
		return EXE_PARA_ERROR;
	if(peripheral_ptr->peripheral_data_reg_ptr && (buff && size || request == INIT))
	{
		switch(request)
		{
			case INIT:
			{
				spi_reg->SR = 2;
				spi_reg->CR2 = 0x00;
				spi_reg->CR1 =  0x300 + (0x07  << 3) + 0x04;
				break;
			}
			case READ_ASYN:
			{
			
				
			}
			case READ:
			{
				spi_reg->CR1 |= 0x40;
				for(unsigned int size_ = 0;size_ < size;size_++)
				{
					DELAY(( ( (spi_reg->SR) & 0x02) == 0 ),0x10000,return EXE_DEVICE_ERROR,spi_reg->DR = 0xFF);
					DELAY( ((spi_reg->SR & 0x01) == 0),0x10000,return EXE_DEVICE_ERROR,((unsigned char*)buff)[size_] = spi_reg->DR )
				}
				break;
			}
			case WRITE:
			{
				spi_reg->CR1 |= 0x40;
				for(unsigned int size_ = 0;size_ < size;size_++)
				{					
					DELAY(( ( (spi_reg->SR) & 0x02) == 0 ),0x10000,return EXE_DEVICE_ERROR,UNUSED(size));
					spi_reg->DR = ((unsigned char*)buff)[size_];
				}
				break;
			}
			
			case WRITE_ASYN:
			{
				spi_reg->CR2 = 0x03;
				if(dma[1].dma_peripheral_driver(peripheral_ptr,buff,size,request) != EXE_SUCCESS)
				{
					return EXE_DEVICE_ERROR;
				}
				break;
			}
			default:
			{
				return EXE_PARA_ERROR;
			}
		}
		return EXE_SUCCESS;
	}
	return EXE_PARA_ERROR;
}


const DRIVER_PERIPHERAL spi1 = {.name = "SPI1",.dma_channel = 3,
																.dma_stream_rx = DMA2_Stream2 ,.dma_stream_tx = DMA2_Stream3,
																.peripheral_data_reg_ptr = (void*)&(SPI1->DR),.reg_base = SPI1,
																.peripheral_id = DEVICE_SPI_ID,.peripheral_driver = SPI_driver};


void SPI1_init()
{
	unsigned int temp;
/*
		PA4--->SPI1_NSS
		PA5--->SPI1_SCK		AF_PP
		PA6--->SPI1_MISO	AF_PP
		PA7--->SPI1_MOSI	AF_PP
*/


	
	RCC->AHB1ENR |= 1;
	RCC->APB2ENR |= 0x1000UL;
	
	temp = 	GPIOA->MODER;
	temp &= (~(0xFC00UL));
	temp |= 0xA800UL;
	GPIOA->MODER = temp;
	GPIOA->AFR[0] |= 0x55500000;
	GPIOA->OSPEEDR|= 0xFC00;
	spi1.peripheral_driver(&spi1,0,0,INIT);
	
	NVIC->ISER[1] |= 0x08;
}

//void SPI1_IRQHandler()
//{
//	if(spi1.peripheral_operate_bytes)
//	{
//		*(unsigned char*)(spi1.peripheral_buffer) = SPI1->DR;
//		spi1.peripheral_buffer++;
//		spi1.peripheral_operate_bytes--;
//	}

//}
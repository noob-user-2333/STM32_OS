#include "driver.h"
/*
	WARMING!!!!!!!!!!
			DO NOT OPERATE NSS PIN!!!!!!!!!
			AN UNKNOWN ERROR WILL OCCURRENCED!!!!!!!!!!!!!!
*/
DRIVER_PERIPHERAL spi1;


///*
//mode:
//0bit: 0第一个时钟沿数据采样，1第二个时钟沿数据采样
//1bit: 1空闲时SCK高电平，0空闲时SCK低电平
//2bit: 1主设备	0从设备
//3~5bit:波特率控制，最低为010
//				001:18MHz
//				010:9MHz
//				011:4.5MHz
//				100:2.25MHz
//				101:1.125MHz
//				110:562.5KHz
//				111:281.25KHz
//*/



EXE_STATE SPI_driver(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *buff,unsigned int size,REQUEST request)
{
	SPI_TypeDef* spi_reg = peripheral_ptr->reg_base;
	unsigned int temp;
	if(peripheral_ptr->peripheral_id != DEVICE_SPI_ID )
		return EXE_PARA_ERROR;
//	if(spi_reg->SR & 0x80)
//		return EXE_DEV_BUSYING;
	
	switch (request)
  {
		case READ_ASYN:
		{
			spi_reg->CR2 = 0x03;
			spi_reg->CR1 |= 0x40;
			if(dma1.dma_driver_periprheral(peripheral_ptr,buff,size,WRITE_ASYN)!= EXE_SUCCESS)
				return EXE_DEV_ERROR;
			return dma1.dma_driver_periprheral(peripheral_ptr,buff,size,request);
			
			break;
		}
    case READ:
    {
				spi_reg->CR1 |= 0x40;
			//发送读取数据设备地址与寄存器地址			
				for(;size;size--,buff++)	
				{
					
					DELAY(( ( (spi_reg->SR) & 0x02) == 0 ),0x1000,return EXE_DEV_ERROR,spi_reg->DR = 0xFF);
					DELAY(( (spi_reg->SR) & 0x01) ==  0,0x1000,return EXE_DEV_ERROR,*((unsigned char*)buff) = spi_reg->DR);
					
				}

				return EXE_SUCCESS;
        break;
    }

    case WRITE:
    {
				spi_reg->CR1 |= 0x40;
				for(unsigned int size_ = 0;size_ < size;size_++)
				{
					
					DELAY(( ( (spi_reg->SR) & 0x02) == 0 ),0x200,return EXE_DEV_ERROR,spi_reg->DR = ((unsigned char*)buff)[size_]);
				}

				return EXE_SUCCESS;
        break;
    }
		

		case WRITE_ASYN:
		{
				spi_reg->CR1 |= 0x40;
				spi_reg->CR2 = 0x03;
				return dma1.dma_driver_periprheral(peripheral_ptr,buff,size,request);
				break;
		}
    case INIT:
    {
			 
			 peripheral_ptr->peripheral_data_reg_ptr = (void*)&(spi_reg->DR);
			
			spi_reg->SR = 0x02;
			spi_reg->CR2 = 0x40;
			spi_reg->CR1 =  0x200 + 0x100 + 0x04 + (0x07 << 3);

			
      return EXE_SUCCESS;
       break;
    }

    case UNINIT:
    {

        /* There is nothing to do in this case for the RAM driver.  For actual
           devices some shutdown processing may be necessary.  */

        /* Successful driver request.  */
        return EXE_SUCCESS;
        break;
    }
    default:
    {

        /* Invalid driver request.  */
        return EXE_PARA_ERROR;
        break;
    }

	}
	return EXE_UNKNOWN_ERROR;
}

EXE_STATE SPI_init(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,SPEED speed,unsigned int mode)
{
	SPI_TypeDef* spi_reg = peripheral_ptr->reg_base;
	unsigned int temp;
	if(peripheral_ptr->peripheral_id != DEVICE_SPI_ID )
		return EXE_PARA_ERROR;
	if(spi_reg->SR & 0x80)
		return EXE_DEV_BUSYING;
	spi_reg->CR1 &= (~0x40UL);
	temp = spi_reg->CR1;
	temp &= (~0x38UL);
	temp |= speed << 3;
	spi_reg->CR1 = temp;
	

	return EXE_SUCCESS;
}


void SPI1_init()
{
/*
		PA5--->SPI1_SCK
		PA6--->SPI1_MISO
		PA7--->SPI1_MOSI
*/
	memset(&spi1,0,sizeof(DRIVER_PERIPHERAL));
	spi1.peripheral_id =  DEVICE_SPI_ID;
	spi1.name =		 "SPI1";
	spi1.reg_base =(void*)SPI1_BASE;
	spi1.mode = 	 0;

	spi1.peripheral_driver = SPI_driver;
	spi1.peripheral_init = SPI_init;
	
	

	
	spi1.dma_channel_rx = &dma1.dma_channel[1];
	spi1.dma_channel_tx = &dma1.dma_channel[2];
	
	RCC->APB2ENR |= 0x1004;
	
	GPIOA->CRL |= ((0xA0B) << 20);
	GPIOA->LCKR |= 0xE0UL;
	
//	NVIC->ISER[1] |= 0x08;
	
	spi1.peripheral_driver(&spi1,0,0,INIT);
	
	
}

//void SPI1_IRQHandler()
//{
//	if(spi1.peripheral_operate_bytes)
//	{
//		*(unsigned char*)(spi1.peripheral_buffer ++) = SPI1->DR;
//		spi1.peripheral_operate_bytes --;
//	}
//	
//}

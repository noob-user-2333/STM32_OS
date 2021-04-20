#include "driver.h"

extern DRIVER_DMA_CONTROL dma1;
DRIVER_PERIPHERAL i2c1;


//unsigned int I2C_init(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,unsigned int mode)
//{
//	if(peripheral_ptr && peripheral_ptr->dev_id == DEVICE_I2C_ID )
//	{
//		if(peripheral_ptr->state == UNINITIALIZE || peripheral_ptr->state == READY || peripheral_ptr->state == IS_ERROR)
//		{
//			I2C_TypeDef* i2c_reg = peripheral_ptr->reg_base;
//			peripheral_ptr->state = BUSYING;
//			i2c_reg->SR1 = i2c_reg->SR2 = 0x00;
//			i2c_reg->TRISE = 0x25;
//			i2c_reg->CCR = 0xB4;
//			i2c_reg->CR1 = 0x01;
//			i2c_reg->CR2 = 0x24;
//			i2c_reg->OAR1 = 0x4000;
//			
//			peripheral_ptr->mode = mode;
//			peripheral_ptr->state = READY;
//			return 0;
//		}
//		return 0xFFFFFF00;
//	}
//	return 0xFFFFFFFF;
//}

//unsigned int I2C_write(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void* src,void* dest,unsigned int size)
//{
//	if(peripheral_ptr && peripheral_ptr->dev_id == DEVICE_I2C_ID && peripheral_ptr->state == READY)
//	{
//		I2C_TypeDef *reg_base = peripheral_ptr->reg_base;
//		
//		if(dma1.dma_write(peripheral_ptr,src,(void*)&(reg_base->DR),size))
//		{
//		
//		}
//		else
//		{
//			peripheral_ptr->state = BUSY_IN_WRITE;
//		}
//		return 0;
//	}
//	return 0xFFFFFFFF;
//}




//unsigned int I2C_read(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void* buff,void* address,unsigned int size)
//{
//	if(peripheral_ptr && peripheral_ptr->dev_id == DEVICE_I2C_ID && peripheral_ptr->state == READY)
//	{
//		unsigned int temp;
//		I2C_TypeDef* i2c_reg = peripheral_ptr->reg_base;
//		if(i2c_reg->SR2 & 0x02)
//		{
//			peripheral_ptr->state = IS_ERROR;
//			return 0xFF00FF00;
//		}
//		peripheral_ptr->state = START_UP;
//		

//		//发送读取数据设备地址与寄存器地址
//		i2c_reg->CR1 |= (0x400UL);
//		i2c_reg->CR1 |= 0x100;
//		DELAY(((i2c_reg->SR1 & 0x01) == 0),0x10000,i2c_reg->SR1 &= (~0x4UL));
//		i2c_reg->CR1 &= (~0x100UL);
//		i2c_reg->DR = 0xD0;
//		
//		DELAY(((i2c_reg->SR1 & 0x02) == 0),0x10000,temp =i2c_reg->SR2);
//		i2c_reg->DR = 0x00;
//		
//		DELAY(((i2c_reg->SR1 & 0x04) == 0),0x10000,i2c_reg->SR1 &= (~0x4UL));

//		

//		i2c_reg->CR1 |= 0x100;
//		DELAY(((i2c_reg->SR1 & 0x01) == 0),0x10000,UNUSED(address));

//		i2c_reg->DR = 0xD1;
//		DELAY(((i2c_reg->SR1 & 0x02) == 0),0x10000,temp =i2c_reg->SR2);
//		DELAY(((i2c_reg->SR1 & 0x04) == 0),0x10000,i2c_reg->CR2 |= 0x1800);
//		
//		
//		//接收数据
//		if(dma1.dma_read(peripheral_ptr,buff,(void*)&(i2c_reg->DR),size))
//		{
//		
//		}
//		else
//		{
//			peripheral_ptr->state = BUSY_IN_READ;
//		}
//		
//		return 0;
//	}
//	return 0xFFFFFFFF;
//}

//unsigned int I2C_complete(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,unsigned int para)
//{
//	I2C_TypeDef *i2c_reg = peripheral_ptr->reg_base;
//	if(peripheral_ptr->device_using)
//	{
//		peripheral_ptr->device_using->dev_complete(peripheral_ptr->device_using,0);
//	}
//	if(para)
//	{
//		i2c_reg->CR1 &= (~0x400UL);
//		unsigned int temp = i2c_reg->DR;
//	}
//	i2c_reg->CR1 |= 0x200;
//	peripheral_ptr->state = READY;
//	peripheral_ptr->peripheral_init(peripheral_ptr,peripheral_ptr->mode);
//	return 0;
//}
//unsigned int I2C_error_handle(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,unsigned int para)
//{

//	I2C_TypeDef *i2c_reg = peripheral_ptr->reg_base;
//	if(peripheral_ptr->device_using)
//	{
//		peripheral_ptr->device_using->dev_complete(peripheral_ptr->device_using,0);
//	}
//	i2c_reg->CR1 |= 0x200;
//	peripheral_ptr->state = IS_ERROR;
//	return 0;
//}

void I2C_driver(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr)
{
	I2C_TypeDef* i2c_reg = peripheral_ptr->reg_base;
	unsigned int temp;
	if(peripheral_ptr->peripheral_id != DEVICE_I2C_ID || peripheral_ptr->peripheral_status == DRIVER_IO_BUSYING)
		return;
	peripheral_ptr->peripheral_status = DRIVER_IO_ERROR;
	switch (peripheral_ptr->peripheral_request)
  {

    case DRIVER_READ:
    {
				peripheral_ptr->peripheral_status = DRIVER_IO_BUSYING;
			//发送读取数据设备地址与寄存器地址			
			i2c_reg->CR1 |= (0x400UL);
			i2c_reg->CR1 |= 0x100;
			DELAY(((i2c_reg->SR1 & 0x01) == 0),0x10000,peripheral_ptr->peripheral_status = DRIVER_IO_ERROR,i2c_reg->SR1 &= (~0x4UL));
			if(peripheral_ptr->peripheral_status == DRIVER_IO_ERROR)
				break;
			
			
			i2c_reg->CR1 &= (~0x100UL);
			i2c_reg->DR = ((unsigned char*)(peripheral_ptr->peripheral_buffer))[0];
		
			DELAY(((i2c_reg->SR1 & 0x02) == 0),0x10000,peripheral_ptr->peripheral_status = DRIVER_IO_ERROR;,temp =i2c_reg->SR2);
			if(peripheral_ptr->peripheral_status == DRIVER_IO_ERROR)
				break;
			
			
			i2c_reg->DR = ((unsigned char*)(peripheral_ptr->peripheral_buffer))[1];
		
			DELAY(((i2c_reg->SR1 & 0x04) == 0),0x10000,peripheral_ptr->peripheral_status = DRIVER_IO_ERROR,i2c_reg->SR1 &= (~0x4UL));
			if(peripheral_ptr->peripheral_status == DRIVER_IO_ERROR)
				break;
		

			i2c_reg->CR1 |= 0x100;
			DELAY(((i2c_reg->SR1 & 0x01) == 0),0x10000,peripheral_ptr->peripheral_status = DRIVER_IO_ERROR,UNUSED(NULL));
			if(peripheral_ptr->peripheral_status == DRIVER_IO_ERROR)
				break;
			
			i2c_reg->DR = ((unsigned char*)(peripheral_ptr->peripheral_buffer))[0];
			DELAY(((i2c_reg->SR1 & 0x02) == 0),0x10000,peripheral_ptr->peripheral_status = DRIVER_IO_ERROR,temp =i2c_reg->SR2);
			if(peripheral_ptr->peripheral_status == DRIVER_IO_ERROR)
				break;
			DELAY(((i2c_reg->SR1 & 0x04) == 0),0x10000,peripheral_ptr->peripheral_status = DRIVER_IO_ERROR,i2c_reg->SR1 &= (~0x4UL));
			if(peripheral_ptr->peripheral_status == DRIVER_IO_ERROR)
				break;
			
			for(unsigned int size = 0 ; size < peripheral_ptr->peripheral_operate_bytes; size ++)
			{
				DELAY(((i2c_reg->SR1 & 0x04) == 0),0x10000,peripheral_ptr->peripheral_status = DRIVER_IO_ERROR,((unsigned char*)(peripheral_ptr->peripheral_buffer))[size] = i2c_reg->DR);
				if(peripheral_ptr->peripheral_status == DRIVER_IO_ERROR)
					break;
			}
			i2c_reg->CR1 |= 0x200;
			
				peripheral_ptr->peripheral_status = DRIVER_SUCCESS;
        break;
    }

    case DRIVER_WRITE:
    {
				peripheral_ptr->peripheral_status = DRIVER_IO_BUSYING;
				if(peripheral_ptr->peripheral_operate_bytes <= 2)
					break;
				peripheral_ptr->peripheral_status = DRIVER_IO_BUSYING;
				//发送开始信号
				i2c_reg->CR1 |= (0x400UL);
				i2c_reg->CR1 |= 0x100;
				DELAY(((i2c_reg->SR1 & 0x01) == 0),0x10000,peripheral_ptr->peripheral_status = DRIVER_IO_ERROR,i2c_reg->SR1 &= (~0x4UL));
				if(peripheral_ptr->peripheral_status == DRIVER_IO_ERROR)
					break;
				i2c_reg->CR1 &= (~0x100UL);
				//发送设备地址
				i2c_reg->DR = ((unsigned char*)(peripheral_ptr->peripheral_buffer))[0];
				DELAY(((i2c_reg->SR1 & 0x02) == 0),0x10000,peripheral_ptr->peripheral_status = DRIVER_IO_ERROR;,temp =i2c_reg->SR2);
				if(peripheral_ptr->peripheral_status == DRIVER_IO_ERROR)
					break;
				
				//发送数据
				for(unsigned int size = 1;size < peripheral_ptr->peripheral_operate_bytes;size++)
				{
					i2c_reg->DR = ((unsigned char*)(peripheral_ptr->peripheral_buffer))[size];
		
					DELAY(((i2c_reg->SR1 & 0x04) == 0),0x10000,peripheral_ptr->peripheral_status = DRIVER_IO_ERROR,i2c_reg->SR1 &= (~0x4UL));
					if(peripheral_ptr->peripheral_status == DRIVER_IO_ERROR)
						break;
				}				
				i2c_reg->CR1 |= 0x200;
				if(peripheral_ptr->peripheral_status != DRIVER_IO_ERROR)
					peripheral_ptr->peripheral_status = DRIVER_SUCCESS;
        break;
    }
		case DRIVER_READ_ASYN:
		{
			peripheral_ptr->peripheral_status = DRIVER_IO_BUSYING;
			//发送读取数据设备地址与寄存器地址			
			i2c_reg->CR1 |= (0x400UL);
			i2c_reg->CR1 |= 0x100;
			DELAY(((i2c_reg->SR1 & 0x01) == 0),0x10000,peripheral_ptr->peripheral_status = DRIVER_IO_ERROR,i2c_reg->SR1 &= (~0x4UL));
			if(peripheral_ptr->peripheral_status == DRIVER_IO_ERROR)
				break;
			
			
			i2c_reg->CR1 &= (~0x100UL);
			i2c_reg->DR = ((unsigned char*)(peripheral_ptr->peripheral_buffer))[0];
		
			DELAY(((i2c_reg->SR1 & 0x02) == 0),0x10000,peripheral_ptr->peripheral_status = DRIVER_IO_ERROR;,temp =i2c_reg->SR2);
			if(peripheral_ptr->peripheral_status == DRIVER_IO_ERROR)
				break;
			
			
			i2c_reg->DR = ((unsigned char*)(peripheral_ptr->peripheral_buffer))[1];
		
			DELAY(((i2c_reg->SR1 & 0x04) == 0),0x10000,peripheral_ptr->peripheral_status = DRIVER_IO_ERROR,i2c_reg->SR1 &= (~0x4UL));
			if(peripheral_ptr->peripheral_status == DRIVER_IO_ERROR)
				break;
		

			i2c_reg->CR1 |= 0x100;
			DELAY(((i2c_reg->SR1 & 0x01) == 0),0x10000,peripheral_ptr->peripheral_status = DRIVER_IO_ERROR,UNUSED(NULL));
			if(peripheral_ptr->peripheral_status == DRIVER_IO_ERROR)
				break;
			
			i2c_reg->DR = ((unsigned char*)(peripheral_ptr->peripheral_buffer))[0] + 1;
			DELAY(((i2c_reg->SR1 & 0x02) == 0),0x10000,peripheral_ptr->peripheral_status = DRIVER_IO_ERROR,temp =i2c_reg->SR2);
			if(peripheral_ptr->peripheral_status == DRIVER_IO_ERROR)
				break;
			i2c_reg->CR2 |= 0x1800;		
			dma1.dma_driver(peripheral_ptr,0,0,0);
			
				break;
		}
		case DRIVER_WRITE_ASYN:
		{
		
				peripheral_ptr->peripheral_status = DRIVER_IO_BUSYING;
				if(peripheral_ptr->peripheral_operate_bytes <= 2)
					break;
				peripheral_ptr->peripheral_status = DRIVER_IO_BUSYING;
				//发送起始信号
				i2c_reg->CR1 |= (0x400UL);
				i2c_reg->CR1 |= 0x100;
				DELAY(((i2c_reg->SR1 & 0x01) == 0),0x10000,peripheral_ptr->peripheral_status = DRIVER_IO_ERROR,i2c_reg->SR1 &= (~0x4UL));
				if(peripheral_ptr->peripheral_status == DRIVER_IO_ERROR)
					break;
				//发送设备地址
				i2c_reg->CR1 &= (~0x100UL);
				i2c_reg->DR = ((unsigned char*)(peripheral_ptr->peripheral_buffer))[0];
				DELAY(((i2c_reg->SR1 & 0x02) == 0),0x10000,peripheral_ptr->peripheral_status = DRIVER_IO_ERROR,temp =i2c_reg->SR2);
				if(peripheral_ptr->peripheral_status == DRIVER_IO_ERROR)
					break;	
				
				
				
				//启动DMA传输
				i2c_reg->CR2 |= 0x1800;
				peripheral_ptr->peripheral_buffer++;
				peripheral_ptr->peripheral_operate_bytes--;
				dma1.dma_driver(peripheral_ptr,0,0,0);
				break;
		}
    case DRIVER_INIT:
    {
			 i2c_reg->SR1  = 	0x00;
			 i2c_reg->SR2 =	 	0x00;
			 i2c_reg->TRISE = 0x25;
			 i2c_reg->CCR = 	0xB4;
			 i2c_reg->CR1 =	 	0x401;
			 i2c_reg->CR2 = 	0x24;
			 i2c_reg->OAR1 = 	0x4000;
			
			 peripheral_ptr->peripheral_data_reg_ptr = (void*)&(i2c_reg->DR);
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
		if(peripheral_ptr->peripheral_status == DRIVER_IO_ERROR && peripheral_ptr->peripheral_error_notify)
		{
			(peripheral_ptr->peripheral_error_notify)(peripheral_ptr);
		}
		if(peripheral_ptr->peripheral_status == DRIVER_SUCCESS && peripheral_ptr->peripheral_complete_notify)
		{
			(peripheral_ptr->peripheral_complete_notify)(peripheral_ptr);
		}
}


void I2C1_init()
{
/*	
		PB7 ----> I2C_SDA
		PB6 ----> I2C_SCL
*/
	memset(&i2c1,0,sizeof(DRIVER_PERIPHERAL));
	i2c1.peripheral_id = DEVICE_I2C_ID;
	i2c1.name =		 "I2C1";
	i2c1.peripheral_request =	 DRIVER_INIT;
	i2c1.peripheral_status =   DRIVER_IO_ERROR;
	i2c1.reg_base =(void*)I2C1_BASE;
	i2c1.mode = 	 0;

//	i2c1.peripheral_init = I2C_init;
//	i2c1.peripheral_read = I2C_read;
//	i2c1.peripheral_write =I2C_write;
//	i2c1.peripheral_complete = I2C_complete;
//	i2c1.peripheral_error_handle = I2C_error_handle;
	
	i2c1.dma_channel_rx = &dma1.dma_channel[6];
	i2c1.dma_channel_tx = &dma1.dma_channel[5];
	i2c1.peripheral_driver = I2C_driver;
	
	I2C_driver(&i2c1);
	
	RCC->APB1ENR |= (1<<21);
	RCC->APB2ENR |= 0x08;
	
	GPIOB->CRL |= (0xFF<<24);

}




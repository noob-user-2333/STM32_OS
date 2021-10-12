#include "driver.h"
#include <stdarg.h>
#define UART_BUFF_SIZE   256
static unsigned char rece_buff[UART_BUFF_SIZE];	
static unsigned int  rece_offset = 0;
static unsigned char print_current_buf[UART_BUFF_SIZE]; 
static unsigned char buf[20];
static const unsigned char digitals[16]="0123456789ABCDEF";
static unsigned int	 print_size = 0;
static unsigned int  print_buf_offset = 0;
DRIVER_PERIPHERAL uart1;


static void printint(int xx, int base,int sign)
{
	
	unsigned int  i;
	unsigned int  x;

	if(sign && (sign = xx < 0))
		x = -xx;
	else
		x = xx;

	i = 19;
	do{
		buf[i--] = digitals[x%base];
	}while( (x/=base) != 0 );
	
	if(base == 16)
	{
		buf[i--] = 'x';
		buf[i--] = '0';
	}
	
	if(sign)
		buf[i--] = '-';
		
	memcpy(print_current_buf + print_buf_offset,&buf[i+1],19 - i);
	print_buf_offset += 19 - i;
}


void printf_(char *fmt,...)
{
	unsigned int len = strlen(fmt);
	unsigned int var_number = 0;
	va_list			 ap;
	va_start(ap,fmt);
	for(unsigned int i = 0; i < len ; i ++)
	{
		if(fmt[i] == '%')
		{
			switch(fmt[i+1])
			{
				case 'd':
				{
					i += 2;
					printint(va_arg(ap,unsigned int),10,1);
					break;
				}
				case 'x':
				case 'p':
				{
					i += 2;
					printint(va_arg(ap,unsigned int),16,1);
					break;
				}
				case 's':
				{
					i += 2;
					char *str = va_arg(ap,char*);
					memcpy(print_current_buf + print_buf_offset,str,strlen(str));
				}
			}
		}
		print_current_buf[print_buf_offset] = fmt[i];
		print_buf_offset ++;
	}
	uart1.peripheral_driver(&uart1,print_current_buf,print_buf_offset,WRITE_ASYN);
	print_buf_offset = 0;
}

EXE_STATE UART_driver(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *buff,unsigned int size,REQUEST request)
{
	USART_TypeDef* uart_reg = peripheral_ptr->reg_base;
	unsigned int temp;
	if(peripheral_ptr->peripheral_id != DEVICE_UART_ID)
		return EXE_PARA_ERROR;
	if((uart_reg->SR & 0x80) == 0 || (uart_reg->SR & 0x20))
		return EXE_DEV_BUSYING;
	switch (request)
  {
		case READ_ASYN:
		{

		}
    case READ:
    {
				
				//发送读取数据设备地址与寄存器地址			
				if(size > rece_offset)
				{
					size = rece_offset;
				}
				memcpy(buff,rece_buff,size);
				rece_offset = 0;
				return EXE_SUCCESS;
    }

    case WRITE:
    {
				for(unsigned int size_ = 0;size_ < size;size_++)
				{
					DELAY(((uart_reg->SR & 0x80) == 0),0x1000,return EXE_DEV_ERROR,uart_reg->DR = ((unsigned char*)buff)[size_]);
					
				}
				return EXE_SUCCESS;
    }
	
		case WRITE_ASYN:
		{
				//启动DMA传输
				return dma1.dma_driver_periprheral(peripheral_ptr,buff,size,request);
		}
    case INIT:
    {
			 uart_reg->BRR =  0x00000271;
			 uart_reg->CR1 =  0x203C;
			 uart_reg->CR2 =  0;

			 uart_reg->CR3 = 0xC0;
			 peripheral_ptr->peripheral_data_reg_ptr = (void*)&(uart_reg->DR);
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

}




void UART1_init()
{
/*	
		PA9  ----> UART_TX
		PA10 ----> UART_RX
*/
	unsigned int temp;
	memset(&uart1,0,sizeof(DRIVER_PERIPHERAL));
	uart1.name = 								"UART1";
	uart1.peripheral_id  =			DEVICE_UART_ID;

	uart1.peripheral_driver =					UART_driver;
	uart1.reg_base =									USART1;
	
	
	uart1.dma_channel_rx =						&(dma1.dma_channel[4]);
	uart1.dma_channel_tx =						&(dma1.dma_channel[3]);
	
	RCC->APB2ENR |= 0x4005;		
	temp = GPIOA->CRH;
	temp &= ~(0xf0UL); 
	temp |= (0x4B0);
	GPIOA->CRH = temp;
	
	
	NVIC->ISER[1] |= (0x1 << 5);
	uart1.peripheral_driver(&uart1,0,0,INIT);
}


void USART1_IRQHandler()
{
	if(USART1->SR & 0x10)//总线空闲中断
	{
		
		unsigned int temp = USART1->SR;
		temp = USART1->DR;

	}
	else//接收中断
	{
		if(rece_offset >= UART_BUFF_SIZE)
			rece_offset = UART_BUFF_SIZE - 1;
		rece_buff[rece_offset] = USART1->DR;
		rece_offset ++;
	}
}






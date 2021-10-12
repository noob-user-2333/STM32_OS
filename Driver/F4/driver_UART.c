#include "driver.h"
#include <stdarg.h>
#define UART_BUFF_SIZE   512

static unsigned char uart_buff[UART_BUFF_SIZE];
CCM_RAM static unsigned int buff_offset = 0;
CCM_RAM static unsigned int	 print_size = 0;
CCM_RAM static unsigned int  print_buf_offset = 0;
CCM_RAM static unsigned char*print_current_buf;


static unsigned char print_buf0[UART_BUFF_SIZE]; 
//static unsigned char print_buf1[UART_BUFF_SIZE]; 
CCM_RAM static unsigned char buf[20];
static const unsigned char digitals[16]="0123456789ABCDEF";

RAM_FUNC static void printint(int xx, int base,int sign)
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


RAM_FUNC void printf_(char *fmt,...)
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
					print_buf_offset += strlen(str);
				}
			}
		}
		print_current_buf[print_buf_offset] = fmt[i];
		print_buf_offset ++;
	}
	uart1.peripheral_driver(&uart1,print_current_buf,print_buf_offset,WRITE_ASYN);
	print_buf_offset = 0;
}

EXE_STATE UART_driver(const struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *buff,unsigned int size,REQUEST request)
{
	USART_TypeDef* uart_reg = peripheral_ptr->reg_base;
	unsigned int temp;
	if(peripheral_ptr->peripheral_id != DEVICE_UART_ID)
		return EXE_PARA_ERROR;
	if(peripheral_ptr->peripheral_data_reg_ptr && (buff && size || request == INIT))
	{
	switch (request)
  {
			case READ_ASYN:
			{

			}
			case READ:
			{
					//发送读取数据设备地址与寄存器地址			
					if(size > buff_offset)
					{
						size = buff_offset;
					}
					memcpy(buff,uart_buff,size);
					buff_offset = 0;
					break;
			}
			case WRITE:
			{
					for(unsigned int size_ = 0;size_ < size ;size_++)
					{
						DELAY(((uart_reg->SR & 0x80) == 0),0x1000,return EXE_DEVICE_ERROR,
											uart_reg->DR = ((unsigned char*)buff)[size_]);			
					}
					break;
			}		
			case WRITE_ASYN:
			{
				//启动DMA传输
				uart_reg->CR3 = 0x80;
				if(dma[1].dma_peripheral_driver(peripheral_ptr,buff,size,request) != EXE_SUCCESS)
						return EXE_DEVICE_ERROR;
				break;
			}
			case INIT:
			{
					uart_reg->BRR =  0x000002D9;
					uart_reg->CR1 =  0x203C;
					uart_reg->CR2 =  0;
					uart_reg->CR3 =  0x0;
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
		return EXE_PARA_ERROR;
}
//	uart1.name = 								"UART1";
//	uart1.peripheral_id  =			DEVICE_UART_ID;
//	uart1.peripheral_request =  DRIVER_INIT;
//	uart1.peripheral_status =		DRIVER_IO_ERROR;
//	uart1.peripheral_memory_buffer = 	buff;
//	uart1.peripheral_memory_size =		UART_BUFF_SIZE;
//	uart1.peripheral_memory_offset =	0;
//	uart1.peripheral_driver =					UART_driver;
//	uart1.reg_base =									USART1;

//	uart1.dma_control =								dma + 1;
//	uart1.dma_channel =								4;
//	uart1.dma_stream_rx =							&dma[1].dma_stream[5];
//	uart1.dma_stream_tx =             &dma[1].dma_stream[7];



const DRIVER_PERIPHERAL uart1 = {.name = "UART1",.dma_channel = 4,.reg_base = USART1,.peripheral_id = DEVICE_UART_ID,
																.dma_stream_rx = DMA2_Stream5,.dma_stream_tx = DMA2_Stream7,.peripheral_data_reg_ptr = (void*)&(USART1->DR),
																	.peripheral_driver = UART_driver};


void UART1_init()
{
/*	
		PA9  ----> UART_TX 	复用推挽输出
		PA10 ----> UART_RX	浮空输入
*/
	unsigned int temp;

	
	RCC->APB2ENR |= 0x4010;
	RCC->AHB1ENR |= 1;
	temp = GPIOA->MODER;
	temp &= ~(0x3C0000UL); 
	temp |= (0x280000UL);
	GPIOA->MODER = temp;
	
	GPIOA->AFR[1] |= 0x770;
	GPIOA->OSPEEDR |=0xf << 18;

	NVIC->ISER[1] |= (0x1 << 5);
	uart1.peripheral_driver(&uart1,0,0,INIT);
	
	print_current_buf = print_buf0;
}










RAM_FUNC void USART1_IRQHandler()
{
	if(USART1->SR & 0x10)//总线空闲中断
	{
		unsigned int temp = USART1->DR;
	}
	else//接收中断
	{
		if(buff_offset >= UART_BUFF_SIZE)
			buff_offset = UART_BUFF_SIZE - 1;
		((unsigned char*)uart_buff)[buff_offset] = USART1->DR;
		USART1->DR = ((unsigned char*)uart_buff)[buff_offset];
		buff_offset ++;
//	
//		//准备启动DMA
//		uart1.peripheral_request =      DRIVER_READ_ASYN;
//		uart1.peripheral_status = 			DRIVER_IO_ERROR;
//		uart1.peripheral_buffer = 			uart1.peripheral_memory_buffer + uart1.peripheral_memory_offset;
//		uart1.peripheral_operate_bytes =UART_BUFF_SIZE - uart1.peripheral_memory_offset;
//		if(dma1.dma_driver(&uart1,0,0,0) != DRIVER_IO_ERROR) //启动成功
//		{
//			USART1->CR1 &= (~(0x20UL));	//关闭接收缓冲区非空中断
//		}
	}
}




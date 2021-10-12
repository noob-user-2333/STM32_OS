#include "driver.h"
#include  <string.h>
#define FLASH_KEY_FPEC 0x00A5UL
#define FLASH_BUF_SIZE 1024




extern unsigned int Image$$ER_IROM1$$RO$$Limit;

//		
//const unsigned int secotrs_start_address[FLASH_SECTORS_NUMBER] = {FLASH_BEGIN,FLASH_BEGIN + 0x4000,FLASH_BEGIN + 0x8000,FLASH_BEGIN + 0xC000,
//																																	FLASH_BEGIN + 0x10000,FLASH_BEGIN + 0x20000 ,FLASH_BEGIN + 0x40000,FLASH_BEGIN+0x60000};
//const unsigned int sectors_size[FLASH_SECTORS_NUMBER] = {0x4000,0x4000,0x4000,0x4000,0x10000,0x20000,0x20000,0x20000};
////DRIVER_PERIPHERAL flash={0};
//unsigned char     flash_buf[FLASH_BUF_SIZE]={0};



unsigned int flash_write(void *src,void *dest,unsigned int size,unsigned int is_asyn)
{
		src = (void*)((unsigned int)src & (~(0x03UL)));
		dest = (void*)((unsigned int)dest & (~(0x03UL)));
	
		if(dest <= flash.first_unused_rom || dest + size >= flash.stop_address || flash.reg_base->SR & 0x80000000UL)
					return -1;
		FLASH->KEYR = FLASH_KEY1;
		FLASH->KEYR = FLASH_KEY2;
		if(FLASH->CR & 0x80000000UL)
		{
			return -1;
		}
	

			if(is_asyn)
			{
				if(size & 0x03)
				{
					size = size & (~(0x03UL));
					size += 4;
				}
					FLASH->CR = 0x1000201 + 0x2000000 + 0x200;
					FLASH->CR |= 0x200;
//					memory_copy((void*)src,(void*)dest,size);
			}
			else
			{
				unsigned int offset;

				if(size % 4)
					size += 4;
				size >>= 2; 
		
				FLASH->CR = 0x201 + 0x2000000;
				for(offset = 0;offset < size ;offset ++)
				{
						((unsigned int*)dest)[offset] = ((unsigned int*)src)[offset];
				}
				FLASH->CR = 0x80000000UL;
			}
			
		return 0;
}
unsigned int flash_erasure(void *base_address)
{
	if(base_address <= flash.first_unused_rom || base_address >= flash.stop_address || flash.reg_base->SR & 0x80000000UL)
				return -1;
	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2;
	if(FLASH->CR & 0x80000000UL)
	{
		return -1;
	}
	unsigned int sectors_number;
	for(sectors_number = 0;sectors_number < FLASH_SECTORS_NUMBER && flash.sectors_start_address[sectors_number] < (unsigned int)base_address;sectors_number++)
	{}
	flash.reg_base->CR = (sectors_number << 3) + 2;
	flash.reg_base->CR |= 0x10000UL + 0x3000000UL;
	return 0;
}

void FLASH_INIT()
{
	/*
		When Vcc = 2.7 ~ 3.6V,Wait states = 5WS as 6 CPU cycles in 168MHz
	*/
	NVIC->ISER[0] |= 0x10;
	FLASH->ACR = 0x705;
}

const INNER_FLASH flash = {.name = "INNER_FLASH", .first_unused_rom = &Image$$ER_IROM1$$RO$$Limit ,.reg_base = FLASH,
														.flash_write = flash_write,.flash_erasure =  flash_erasure,
														.sectors_number = FLASH_SECTORS_NUMBER , .start_address =  (void*)FLASH_BEGIN,
														.stop_address =   (void*)FLASH_BEGIN + FLASH_SIZE,.size =	FLASH_SIZE,
														.sectors_start_address = {FLASH_BEGIN,FLASH_BEGIN + 0x4000,FLASH_BEGIN + 0x8000,FLASH_BEGIN + 0xC000,
																																	FLASH_BEGIN + 0x10000,FLASH_BEGIN + 0x20000 ,FLASH_BEGIN + 0x40000,FLASH_BEGIN+0x60000},
														.sectors_size = {0x4000,0x4000,0x4000,0x4000,0x10000,0x20000,0x20000,0x20000}};


RAM_FUNC void FLASH_IRQHandler()
{
	if(FLASH->SR & 0x01)
	{
		FLASH->SR = 0x01;
//		flash.state = DRIVER_SUCCESS;
//		if(flash.flash_complete_notify)
//		{
//			flash.flash_complete_notify(&flash);
//		}
	}
	else
	{
		FLASH->SR = 0x03;
//		flash.state = DRIVER_IO_ERROR;
//		if(flash.flash_error_notify)
//		{
//			flash.flash_error_notify(&flash);
//		}
	}
	FLASH->CR = 0x80000000UL;
}
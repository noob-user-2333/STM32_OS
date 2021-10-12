#pragma `once

#include "main.h"
#include <string.h>

#define CPU_FREQ              72000000

#define DEVICE_FLASH_ID 			0xFEFEFEF0
#define DEVICE_UART_ID				0xFEFEFE30
#define DEVICE_SPI_ID					0xFEFEFE20
#define DEVICE_I2C_ID					0xFEFEFE10
#define DEVICE_DMA_ID					0xFEFEFE00


#define DMA_CHANNEL_NUMBER    7 
#define HARDWARE_TIM_NUMBER   3

#define FLASH_BEGIN 0x08008000UL
#define FLASH_SIZE	0x8000UL
#define FLASH_END		(FLASH_BEGIN + FLASH_SIZE)


#define DELAY(require,delay_times,nosatify,satify) {for(unsigned int times = 0;times < (delay_times) && (require);times++){}\
																		if(require) {nosatify;}else (satify);}


struct DRIVER_PERIPHERAL_STRUCT;

																		
typedef enum EXE_STATE{
	EXE_SUCCESS,
	EXE_PARA_ERROR,
	EXE_DEV_BUSYING,
	EXE_DEV_ERROR,
	EXE_UNKNOWN_ERROR
}EXE_STATE;


typedef enum DRIVER_REQUEST_ENUM{
	UNINIT = 0,
	INIT,
	READ,
	WRITE,
	READ_ASYN,
	WRITE_ASYN
}REQUEST;

typedef enum DRIVER_SPEED{
	CLK_DIV_4 = 1, 
	CLK_DIV_8 = 2,
	CLK_DIV_16 = 3,
	CLK_DIV_32 = 4,
	CLK_DIV_64 = 5,
	CLK_DIV_128 = 6,
	CLK_DIV_256 = 7
}SPEED;


typedef struct DRIVER_DMA_CHANNEL_STRUCT{
	DMA_Channel_TypeDef
								*reg_base;
	struct DRIVER_DMA_CONTROL
								*dma_owner;

	
	void	(*translate_complete_notify)();
	void 	(*translate_error_notify)();
}DRIVER_DMA_CHANNEL;

typedef struct DRIVER_DMA_CONTROL{
	const char				 *name;
	void							 *reg_base;
	
	DRIVER_DMA_CHANNEL *memory_operate_channel;
	

	EXE_STATE (*dma_driver_periprheral)(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *src,unsigned int size,REQUEST request);
	DRIVER_DMA_CHANNEL dma_channel[DMA_CHANNEL_NUMBER];

}DRIVER_DMA_CONTROL;



typedef struct DRIVER_PERIPHERAL_STRUCT{
		unsigned int  peripheral_id;
		const char		*name;
	
		void						*peripheral_data_reg_ptr;
		
	
		unsigned int 	mode;
		unsigned int  speed;
		void					*reg_base;
		
		
		
	
	
		DRIVER_DMA_CHANNEL 							
									*dma_channel_rx,
		 							*dma_channel_tx;


		
		
		EXE_STATE (*peripheral_driver)(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *buff,unsigned int size,REQUEST request);
		EXE_STATE (*peripheral_init)(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,SPEED speed,unsigned int mode);

	
}DRIVER_PERIPHERAL;

typedef struct DRIVER_STRUCT{
	
		unsigned int mode;
		DRIVER_PERIPHERAL* peripheral_ptr;
		
		void  (*translate_complete_notify)();
		void  (*translate_error_notify)();

}DRIVER_STRUCT;






extern DRIVER_DMA_CONTROL dma1;
extern DRIVER_PERIPHERAL spi1;
extern DRIVER_PERIPHERAL uart1;


void driver_init();
EXE_STATE driver_open(DRIVER_STRUCT *device_ptr,DRIVER_PERIPHERAL *peripheral_ptr,unsigned int mode);
EXE_STATE driver_func_set(DRIVER_STRUCT *device_ptr,void (*translate_complete)(void),void (*translate_error)(void));
EXE_STATE driver_func_clear(DRIVER_STRUCT *device_ptr,unsigned int translate_complete_func_clear,unsigned int translate_error_func_clear);
EXE_STATE driver_read(DRIVER_STRUCT *device_ptr,void *memory_buffer,unsigned int size);
EXE_STATE driver_write(DRIVER_STRUCT *device_ptr,void *memory_src_buffer,unsigned int size);
EXE_STATE driver_read_asyn(DRIVER_STRUCT *device_ptr,void *memory_buffer,unsigned int size);
EXE_STATE driver_write_asyn(DRIVER_STRUCT *device_ptr,void *memory_src_buffer,unsigned int size);

EXE_STATE driver_close(DRIVER_STRUCT *device_ptr);
void sys_tick_enable(unsigned int ms_per_ticks);
void sys_tick_disable();
void sys_tick_info_get(unsigned int *enable_flag,unsigned int *ms_per_ticks);
void printf_(char *fmt,...);
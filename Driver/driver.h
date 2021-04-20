#pragma `once

#include "main.h"
#include <string.h>

#define DEVICE_SD_ID					0xFEFEF000


#define DEVICE_FLASH_ID 			0xFEFEFEF0
#define DEVICE_SPI_ID					0xFEFEFE20
#define DEVICE_I2C_ID					0xFEFEFE10
#define DEVICE_DMA_ID					0xFEFEFE00
#define DRIVER_TYPE_ID				0x0000FF01


#define DMA_CHANNEL_NUMBER    7 
#define HARDWARE_TIM_NUMBER   3

#define FLASH_BEGIN 0x08008000UL
#define FLASH_SIZE	0x8000UL
#define FLASH_END		(FLASH_BEGIN + FLASH_SIZE)


#define DELAY(require,delay_times,nosatify,satify) {for(unsigned int times = 0;times < (delay_times) && (require);times++){}\
																		if(require) {nosatify;}else (satify);}


struct DRIVER_PERIPHERAL_STRUCT;

typedef enum DRIVER_STATE_ENUM{
	DRIVER_IO_ERROR = 0,
	DRIVER_IO_BUSYING,
	DRIVER_SUCCESS
}DRIVER_STATE;



typedef enum DRIVER_REQUEST_ENUM{
	DRIVER_UNINIT = 0,
	DRIVER_INIT,
	DRIVER_READ,
	DRIVER_WRITE,
	DRIVER_READ_ASYN,
	DRIVER_WRITE_ASYN
}DRIVER_REQUEST;
//typedef enum DRIVE_COMPLETE_STATE{
//	READ_COMPLETE,
//	WRITE_COMPLETE
//}DRIVER_COMPLETE_STATE;

//typedef enum{
//	READ_ERROR,
//	WRITE_ERROR,
//	OVERTIME_ERROR
//}DRIVER_ERROR_STATE;

//typedef enum{
//	TIM_BASE,
//	TIM_GENERAL,
//	TIM_ADVANCED
//}DRIVER_TIM_TYPE;


typedef struct DRIVER_DMA_CHANNEL_STRUCT{
	
	
	DRIVER_REQUEST channel_request;
	DRIVER_STATE 	 channel_status;
	
	
	DMA_Channel_TypeDef
								*reg_base;
	struct DRIVER_DMA_CONTROL
								*dma_owner;
	struct DRIVER_PERIPHERAL_STRUCT 
								*used_peripheral;
	
	void	(*translate_complete_notify)(struct DRIVER_DMA_CHANNEL_STRUCT* dma_ptr);
	void 	(*translate_error_notify)(struct DRIVER_DMA_CHANNEL_STRUCT* dma_ptr);
}DRIVER_DMA_CHANNEL;

typedef struct DRIVER_DMA_CONTROL{
	const char				 *name;
	void							 *reg_base;
	
	
	
//	unsigned int (*dma_read)(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr
//													,void* buf,void* src,unsigned int size);
//	unsigned int (*dma_write)(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr
//													,void* src,void* dest,unsigned int size);
	unsigned int (*dma_driver)(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *src,void *dest,unsigned int size);
	DRIVER_DMA_CHANNEL dma_channel[DMA_CHANNEL_NUMBER];
}DRIVER_DMA_CONTROL;



typedef struct DRIVER_PERIPHERAL_STRUCT{
		unsigned int  peripheral_id;
		const char		*name;
	
	
		DRIVER_REQUEST  peripheral_request;
		DRIVER_STATE		peripheral_status;
		unsigned int		peripheral_operate_bytes;
	  void						*peripheral_buffer;
		void					  *peripheral_memory_buffer;
		unsigned int	  peripheral_memory_size;
	
	
	
	
	
	
		void						*peripheral_data_reg_ptr;
		
	
		unsigned int 	mode;
		unsigned int  speed;
		void					*reg_base;
		
		
		
	
	
		DRIVER_DMA_CHANNEL 							
									*dma_channel_rx,
		 							*dma_channel_tx;

//		struct DRIVER_DEVICE_STRUCT
//									*device_using;
//		unsigned int (*peripheral_init)(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,unsigned int mode);
//		unsigned int (*peripheral_read)(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void* buf,void* src,unsigned int size);
//		unsigned int (*peripheral_write)(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void* src,void* dest,unsigned int size);
////		unsigned int (*peripheral_write_read)(struct DRIadVER_PERIPHERAL_STRUCT *peripheral_ptr,void* src,void* buff,unsigned int size);
		
		
		
		void (*peripheral_driver)(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr);
//		
		
		void (*peripheral_complete_notify)(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr);
		void (*peripheral_error_notify)(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr);
//	
//		void					*buf_start;
//		void					*buf_end;
//		unsigned int	buf_offset;
//		unsigned int  buf_size;
	
}DRIVER_PERIPHERAL;




//typedef struct DRIVER_DEVICE_STRUCT{
//	unsigned int  type_id;

//	const char    *name;
//	DRIVER_STATE	state;
//	unsigned int	mode;
//	unsigned int  dev_id;
//	
//	struct DRIVER_PERIPHERAL_STRUCT
//								*peripheral_used;
//	
//	unsigned int (*dev_init)(struct DRIVER_DEVICE_STRUCT *device_ptr,unsigned int mode);
//	unsigned int (*dev_read)(struct DRIVER_DEVICE_STRUCT *device_ptr,void* buf,void* src,unsigned int size);
//	unsigned int (*dev_write)(struct DRIVER_DEVICE_STRUCT *device_ptr,void* src,void* dest,unsigned int size);
//	
//	
//	unsigned int (*dev_complete)(struct DRIVER_DEVICE_STRUCT *device_ptr,unsigned int para);
//	unsigned int (*dev_error_handle)(struct DRIVER_DEVICE_STRUCT *device_ptr,unsigned int para);

//	
//	void					*buf_start;
//	void					*buf_end;
//	unsigned int	buf_offset;
//	unsigned int  buf_size;

//}DRIVER_DEVICE;






//extern DRIVER_DMA_CONTROL dma1;
//extern DRIVER_PERIPHERAL spi1;
extern DRIVER_PERIPHERAL flash;
//extern DRIVER_PERIPHERAL i2c1;
//extern DRIVER_DEVICE		 SD;

	
void driver_init();
DRIVER_STATE driver_read(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *memory_buffer,void *src,unsigned int size);
DRIVER_STATE driver_write(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *memory_src_buffer,void *dest,unsigned int size);
DRIVER_STATE driver_read_asyn(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *memory_buffer,void *src,unsigned int size);
DRIVER_STATE driver_write_asyn(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *memory_src_buffer,void *dest,unsigned int size);

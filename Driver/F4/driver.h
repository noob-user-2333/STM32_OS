#pragma `once
#include "defs_base.h"
#include "main.h"

#include <string.h>
#define CCM_RAM   __attribute__((section("CCM_RAM")))
#define RAM_FUNC   __attribute__((section("RAM_FUNC")))
#define CPU_FREQ	168000000

#define DEVICE_SD_ID					0xFEFEF000


#define DEVICE_FLASH_ID 			0xFEFEFEF0
#define DEVICE_UART_ID				0xFEFEFE30
#define DEVICE_SPI_ID					0xFEFEFE20
#define DEVICE_I2C_ID					0xFEFEFE10
#define DEVICE_DMA_ID					0xFEFEFE00
#define DEVICE_SDIO_ID				0xFEFEFE40


#define DMA_STREAM_NUMBER    8
#define DMA_NUMBER						2

#define MPU_REGION_NUMBER    8


#define FLASH_BEGIN 0x08000000UL
#define FLASH_SIZE	0x80000UL
#define FLASH_SECTORS_NUMBER 8


#define DELAY(require,delay_times,nosatify,satify) {for(unsigned int times = 0;times < (delay_times) && (require);times++){}\
																		if(require) {nosatify;}else (satify);}

extern unsigned int unused_memory_ram1;
extern unsigned int unused_memory_ram2;														
																		
struct DRIVER_PERIPHERAL_STRUCT;

typedef enum MPU_PERISSIONS_ENUM{
	NO_ACCESS,
	READ_ONLY,
	READ_AND_WRITE
}PERISSIONS;																		
									

typedef enum MPU_SIZE_ENUM{
	REGION_SIZE_32BIT = 0x04,
	REGION_SIZE_64BIT = 0x05,
	REGION_SIZE_128BIT = 0x06,
	REGION_SIZE_256BIT = 0x07,
	REGION_SIZE_512BIT = 0x08,
	REGION_SIZE_1KB = 0x09,
	REGION_SIZE_2KB = 0x0A,
	REGION_SIZE_4KB = 0x0B,
	REGION_SIZE_8KB = 0x0C,
	REGION_SIZE_16KB = 0x0D,
	REGION_SIZE_32KB = 0x0E,
	REGION_SIZE_64KB = 0x0F,
	REGION_SIZE_128KB = 0x10,
	REGION_SIZE_256KB = 0x11,
	REGION_SIZE_512KB = 0x12,
	REGION_SIZE_1MB = 0x13,
	REGION_SIZE_1GB = 0x1D,
	REGION_SIZE_4GB = 0x1F
}MPU_REGION_SIZE;

typedef enum REQUEST{
		INIT,
		READ,
		WRITE,
		READ_ASYN,
		WRITE_ASYN,
		
}REQUEST; 





typedef struct DRIVER_DMA_CONTROL{
	unsigned int 				dma_id;
	const char				 *name;
	DMA_TypeDef				 *reg_base;
	
	unsigned int				dma_stream_number;
	DMA_Stream_TypeDef 
										*dma_stream_reg_base[DMA_STREAM_NUMBER];
	
	void 							(**translate_complete_notify)(void);
	void							(**translate_error_notify)(void);
	

	EXE_STATE (*dma_peripheral_driver)(const struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void* buff,unsigned int size,REQUEST request);
	DMA_Stream_TypeDef* (*dma_memory_driver)(void *src,void *dest,unsigned int size,REQUEST request);
}DMA_CONTROL;



typedef struct DRIVER_PERIPHERAL_STRUCT{
		unsigned int  peripheral_id;
		const char		*name;
		void					*peripheral_data_reg_ptr;

		void					*reg_base;

	
		unsigned int	dma_channel;
	
		DMA_Stream_TypeDef 							
									*dma_stream_rx,
		 							*dma_stream_tx;

		
	
	
	
		EXE_STATE (*peripheral_driver)(const struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *buff,unsigned int size,REQUEST request);

}DRIVER_PERIPHERAL;


typedef struct INNER_FLASH{
	unsigned int  flash_id;
	char   *name;

	FLASH_TypeDef *reg_base;
							
	
	void*				 first_unused_rom;
	void*        start_address;
	void*        stop_address;
	unsigned int size;

	unsigned int sectors_number;
	unsigned int sectors_start_address[FLASH_SECTORS_NUMBER];
	unsigned int sectors_size[FLASH_SECTORS_NUMBER];

	
	unsigned int (*flash_write)(void *src,void *dest,unsigned int size,unsigned int is_asyn);
	unsigned int (*flash_erasure)(void *base_address);
}INNER_FLASH;

typedef struct DRIVER_MPU_REGION_STRUCT{

	unsigned int      region_valid;
	unsigned int 			region_base_address;
	unsigned int			region_end_address;
	MPU_REGION_SIZE		region_size;
	PERISSIONS		privileged_perissions;
	PERISSIONS		unprivileged_perissions;
}DRIVER_MPU_REGION;
typedef struct DRIVER_MPU_STRUCT{
	unsigned int 			mpu_enable;
	DRIVER_MPU_REGION*current_regions;
	DRIVER_MPU_REGION*next_regions;
}DRIVER_MPU;




typedef enum{
	ADVANCED_TIMER,
	GENERAL_BUT_MORE_ADVANCED_TIMER,
	GENERAL_TIMER,
	BASIC_TIMER
}TIMER_TYPE;


typedef enum{
	TIMER_NO_WORK,
	TIMER_TIMING,
	TIMER_TIMING_CIRCLE
}TIMER_WORK;




typedef struct DRIVER_TIM{
	const char   *name;
	

	TIMER_TYPE	 timer_type;
	
	TIM_TypeDef	 *reg_base;

	
	
	
}TIMER;




extern const TIMER timer[2];
extern DRIVER_MPU				  mpu;



extern const INNER_FLASH			 flash;
extern const DMA_CONTROL dma[2];
extern const DRIVER_PERIPHERAL spi1;
extern const DRIVER_PERIPHERAL uart1;



EXE_STATE MPU_add(unsigned int region_base_address,
											MPU_REGION_SIZE		region_size,
											PERISSIONS		privileged_perissions,
											PERISSIONS		unprivileged_perissions);
EXE_STATE MPU_delete(unsigned int region_base_address);
void				 MPU_exchange();

void sys_tick_enable(unsigned int ms_per_ticks);
void sys_tick_disable();
void sys_tick_info_get(unsigned int *enable_flag,unsigned int *ms_per_ticks);

int DMA_stream_state_get(DMA_Stream_TypeDef *stream_ptr,unsigned int *busying,unsigned long *complete_func,unsigned long *error_func);

void driver_init();
//EXE_STATE driver_read(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *memory_buffer,unsigned int size);
//EXE_STATE driver_write(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *memory_src_buffer,unsigned int size);
//EXE_STATE driver_read_asyn(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *memory_buffer,unsigned int size);
//EXE_STATE driver_write_asyn(struct DRIVER_PERIPHERAL_STRUCT *peripheral_ptr,void *memory_src_buffer,unsigned int size);

EXE_STATE timer_timing(const TIMER *timer_ptr,unsigned int ms_per_ticks,unsigned int is_circle);
EXE_STATE timer_disable(TIMER *timer_ptr);
EXE_STATE timer_state_get(TIMER *timer_ptr,TIMER_TYPE *type,TIMER_WORK *work,unsigned int *ms_per_ticks);
//unsigned int flash_write(INNER_FLASH *flash_ptr,void *src,void *dest,unsigned int size);
//unsigned int flash_write_asyn(INNER_FLASH *flash_ptr,void *src,void *dest,unsigned int size);
//unsigned int flash_erasure(INNER_FLASH *flash_ptr,void* erasure_address);

//void memory_copy(void *src,void *dest,int size);
//void memory_set(void *dest,unsigned char num,int size);
void printf_(char *fmt,...);
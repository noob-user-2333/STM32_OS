#include "driver.h"
extern void SPI1_init();

extern void DMA1_init();

extern void UART1_init();
void driver_init()
{
	RCC->APB2ENR |= 0x01UL;
	DMA1_init();

	SPI1_init();
	UART1_init();



}


EXE_STATE driver_open(DRIVER_STRUCT *device_ptr,DRIVER_PERIPHERAL *peripheral_ptr,unsigned int mode)
{
	if(device_ptr == NULL || peripheral_ptr == NULL)
		return EXE_PARA_ERROR;
	
	device_ptr->peripheral_ptr = peripheral_ptr;
	device_ptr->mode = mode;
	
	device_ptr->translate_complete_notify = NULL;
	device_ptr->translate_error_notify = NULL;
	return EXE_SUCCESS;
}
EXE_STATE driver_close(DRIVER_STRUCT *device_ptr)
{
	if(device_ptr == NULL || device_ptr->peripheral_ptr == NULL)
		return EXE_PARA_ERROR;
	EXE_STATE state;
	state = device_ptr->peripheral_ptr->peripheral_driver(device_ptr->peripheral_ptr,0,0,UNINIT);
	device_ptr->translate_complete_notify = NULL;
	device_ptr->translate_error_notify = NULL;
	return state; 
}




EXE_STATE driver_func_set(DRIVER_STRUCT *device_ptr,void (*translate_complete)(void),void (*translate_error)(void))
{
	if(device_ptr == NULL || device_ptr->peripheral_ptr == NULL)
		return EXE_PARA_ERROR;
	if(translate_complete)
		device_ptr->translate_complete_notify = translate_complete;
	if(translate_error)
		device_ptr->translate_error_notify = translate_error;
	
	return EXE_SUCCESS;
}
EXE_STATE driver_func_clear(DRIVER_STRUCT *device_ptr,unsigned int translate_complete_func_clear,unsigned int translate_error_func_clear)
{
	if(device_ptr == NULL || device_ptr->peripheral_ptr == NULL)
		return EXE_PARA_ERROR;
	if(translate_complete_func_clear)
		device_ptr->translate_complete_notify = NULL;
	if(translate_error_func_clear)
		device_ptr->translate_error_notify = NULL;
	
	return EXE_SUCCESS;
}















EXE_STATE driver_read(DRIVER_STRUCT *device_ptr,void *memory_buffer,unsigned int size)
{
	if(device_ptr == NULL || device_ptr->peripheral_ptr == NULL)
		return EXE_PARA_ERROR;
	EXE_STATE state;
	state = device_ptr->peripheral_ptr->peripheral_driver(device_ptr->peripheral_ptr,memory_buffer,size,READ);
	if(state == EXE_SUCCESS && device_ptr->translate_complete_notify)
		device_ptr->translate_complete_notify();
	else
	{
		if(device_ptr->translate_error_notify)
			device_ptr->translate_error_notify();
	}
	return state;
}
EXE_STATE driver_write(DRIVER_STRUCT *device_ptr,void *memory_src_buffer,unsigned int size)
{
	if(device_ptr == NULL || device_ptr->peripheral_ptr == NULL)
		return EXE_PARA_ERROR;
	EXE_STATE state;
	state = device_ptr->peripheral_ptr->peripheral_driver(device_ptr->peripheral_ptr,memory_src_buffer,size,WRITE);
	if(state == EXE_SUCCESS && device_ptr->translate_complete_notify)
		device_ptr->translate_complete_notify();
	else
	{
		if(device_ptr->translate_error_notify)
			device_ptr->translate_error_notify();
	}
	return state;
}












EXE_STATE driver_read_asyn(DRIVER_STRUCT *device_ptr,void *memory_buffer,unsigned int size)
{
if(device_ptr == NULL || device_ptr->peripheral_ptr == NULL)
		return EXE_PARA_ERROR;
	EXE_STATE state;

	device_ptr->peripheral_ptr->dma_channel_rx->translate_complete_notify = device_ptr->translate_complete_notify;
	device_ptr->peripheral_ptr->dma_channel_rx->translate_error_notify = device_ptr->translate_error_notify;

	state = device_ptr->peripheral_ptr->peripheral_driver(device_ptr->peripheral_ptr,memory_buffer,size,READ_ASYN);
	if(state != EXE_SUCCESS && device_ptr->translate_error_notify)
		device_ptr->translate_error_notify();
	
	return state;
}
EXE_STATE driver_write_asyn(DRIVER_STRUCT *device_ptr,void *memory_src_buffer,unsigned int size)
{
if(device_ptr == NULL || device_ptr->peripheral_ptr == NULL)
		return EXE_PARA_ERROR;
	EXE_STATE state;

	device_ptr->peripheral_ptr->dma_channel_tx->translate_complete_notify = device_ptr->translate_complete_notify;
	device_ptr->peripheral_ptr->dma_channel_tx->translate_error_notify = device_ptr->translate_error_notify;

	state = device_ptr->peripheral_ptr->peripheral_driver(device_ptr->peripheral_ptr,memory_src_buffer,size,WRITE_ASYN);
	if(state != EXE_SUCCESS && device_ptr->translate_error_notify)
		device_ptr->translate_error_notify();
	
	return state;
}















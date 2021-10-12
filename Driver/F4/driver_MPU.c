#include "driver.h"
CCM_RAM DRIVER_MPU				mpu;
CCM_RAM DRIVER_MPU_REGION mpu_regions0[MPU_REGION_NUMBER];
CCM_RAM DRIVER_MPU_REGION mpu_regions1[MPU_REGION_NUMBER];
CCM_RAM unsigned int			regions_base_address[MPU_REGION_NUMBER];
CCM_RAM unsigned int			regions_end_address[MPU_REGION_NUMBER];


EXE_STATE MPU_add(unsigned int region_base_address,
											MPU_REGION_SIZE		region_size,
											PERISSIONS		privileged_perissions,
											PERISSIONS		unprivileged_perissions)
{
	memset(regions_base_address,0,sizeof(unsigned int) * MPU_REGION_NUMBER);
	memset(regions_end_address,0,sizeof(unsigned int) * MPU_REGION_NUMBER);
	if(privileged_perissions < unprivileged_perissions)
		return EXE_PARA_ERROR;

	for(unsigned int number = 0;number < MPU_REGION_NUMBER;number++)
	{
		if(mpu.next_regions[number].region_valid == 0)
		{
			//check the address and size
			regions_base_address[number] = region_base_address;
			regions_end_address[number] = region_base_address + (1 << (((unsigned int)region_size) + 1)) - 1;
		
			for(unsigned int check = 0;check <= number;check++)
			{
				for(unsigned int test_number = 0;test_number <= number;test_number++)
				{
					if(check != test_number)
					{
						if(mpu.next_regions[check].region_base_address >= mpu.next_regions[test_number].region_base_address
							&& mpu.next_regions[check].region_base_address <= mpu.next_regions[test_number].region_end_address)
							return EXE_PARA_ERROR;
					}
				}
			}
			mpu.next_regions[number].region_valid = 1;
			mpu.next_regions[number].region_base_address = regions_base_address[number];
			mpu.next_regions[number].region_end_address =  regions_end_address[number];
			mpu.next_regions[number].region_size =				 region_size;
			mpu.next_regions[number].unprivileged_perissions=unprivileged_perissions;
			mpu.next_regions[number].privileged_perissions=privileged_perissions;
			return EXE_SUCCESS;
		}
		regions_base_address[number] = mpu.next_regions[number].region_base_address;
		regions_end_address[number] = mpu.next_regions[number].region_end_address;
	}
	return EXE_UNKNOW_ERROR;
}

EXE_STATE MPU_delete(unsigned int region_base_address)
{
	unsigned int find = 0;
	for(unsigned int number = 0;number < MPU_REGION_NUMBER && mpu.next_regions[number].region_valid;number++)
	{	
		if(mpu.next_regions[number].region_base_address == region_base_address)
		{
			find = 1;
		}
		if(find)
		{
			if(number == MPU_REGION_NUMBER - 1 )
				mpu.next_regions[number].region_valid = 0;
			else
				mpu.next_regions[number] = mpu.next_regions[number + 1];
		}
		
	}
	if(find)
		return EXE_SUCCESS;
	return EXE_UNKNOW_ERROR;
}


void MPU_exchange()
{
	DRIVER_MPU_REGION *mpu_regions = mpu.next_regions;
	MPU->CTRL = 0x00;

	for(unsigned int number = 0;number < MPU_REGION_NUMBER;number++)
	{
		MPU->RNR = number;
		MPU->RASR = 0;
		if(mpu_regions[number].region_valid == 0)
			continue;
		
		MPU->RBAR = (mpu_regions[number].region_base_address & (~(0x1fUL)));
		MPU->RASR = 0x10000000UL + 0xF0000UL + (mpu_regions[number].region_size << 1);
		if(mpu_regions[number].privileged_perissions >= mpu_regions[number].unprivileged_perissions)
		{
			switch(mpu_regions[number].privileged_perissions)
			{
				
				case READ_AND_WRITE:
				{
					switch(mpu_regions[number].unprivileged_perissions)
					{
						case READ_AND_WRITE:
						{
							MPU->RASR |= 0x03 << 24;
							break;
						}
						case READ_ONLY:
						{
							MPU->RASR |= 0x02 << 24;
							break;
						}
						case NO_ACCESS:
						{
							MPU->RASR |= 0x01 << 24;
						}
					}
					break;
				}
				case READ_ONLY:
				{
					if(mpu_regions[number].unprivileged_perissions == READ_ONLY)
					{
						MPU->RASR |= 0x06 << 24;
					}
					else
					{
						MPU->RASR |= 0x05 << 24;
					}
				}
				case NO_ACCESS:
				{
					
				}
			}
			MPU->RASR |= 0x01;
		}
	}
	MPU->CTRL = 0x05;
	mpu.mpu_enable = 1;
	mpu.next_regions = mpu.current_regions;
	mpu.current_regions = mpu_regions;
	for(unsigned int number = 0;number < MPU_REGION_NUMBER;number++)
	{
		mpu.next_regions[number].region_valid = 0;
	}
}



void MPU_Init()
{
	memset(&mpu,0,sizeof(DRIVER_MPU));
	memset(mpu_regions0,0,sizeof(DRIVER_MPU_REGION) * MPU_REGION_NUMBER);
	memset(mpu_regions1,0,sizeof(DRIVER_MPU_REGION) * MPU_REGION_NUMBER);
	SCB->SHCSR |= 0x10000UL;
	mpu.mpu_enable = 0;
	mpu.current_regions = mpu_regions0;
	mpu.next_regions =		mpu_regions1;
	
	
}






void MemManage_Handler()
{
	
	
	while(1)
	{

	}
}























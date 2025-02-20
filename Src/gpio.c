/*
 * gpio.c
 *
 *  Created on: Jan 12, 2025
 *      Author: wataoxp
 */

#include "gpio.h"

GPIO_Code GPIO_OutputInit(GPIO_TypeDef *GPIOx,GPIO_InitTypedef *InitStruct)
{
	if((InitStruct->PinPos == Pin13) || (InitStruct->PinPos == Pin14))
	{
		if(GPIOx == GPIOA)
		{
			return init_Failed;
		}
	}
	WRITE_REG(GPIOx->BRR,1 << InitStruct->PinPos);
	GPIO_SetPinSpeed(GPIOx, InitStruct->PinPos, InitStruct->Speed);
	GPIO_SetOutputPinType(GPIOx, (1 << InitStruct->PinPos), InitStruct->OutputType);

	if(InitStruct->Mode == LL_GPIO_MODE_ALTERNATE)
	{
		if(InitStruct->PinPos < Pin8)
		{
			GPIO_SetAlternate0_7(GPIOx, InitStruct->PinPos, InitStruct->Alternate);
		}
		else
		{
			GPIO_SetAlternate8_15(GPIOx, InitStruct->PinPos, InitStruct->Alternate);
		}
	}
	GPIO_SetPinPull(GPIOx, InitStruct->PinPos, InitStruct->Pull);
	GPIO_SetPinMode(GPIOx, InitStruct->PinPos, InitStruct->Mode);

	return init_Success;
}
GPIO_Code GPIO_InputInit(GPIO_TypeDef *GPIOx,GPIO_Port port,uint32_t PinPos,uint32_t Mode,uint32_t Pull)
{
	if((PinPos == Pin13) || (PinPos == Pin14))
	{
		if(GPIOx == GPIOA)
		{
			return init_Failed;
		}
	}
	if(LL_IOP_GRP1_IsEnabledClock(1 << port) == 0)
	{
		LL_IOP_GRP1_EnableClock(1 << port);
	}
	GPIO_SetPinPull(GPIOx, PinPos, Pull);
	GPIO_SetPinMode(GPIOx, PinPos, Mode);

	return init_Success;
}

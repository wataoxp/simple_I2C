/*
 * isr.c
 *
 *  Created on: Feb 4, 2025
 *      Author: wataoxp
 */

#include "isr.h"
#include "i2c.h"

/* SysTick ISR */
static uint32_t Tick;

void OverRunHandler(void)
{
	while(1);
}

void SysTick_Handler(void)
{
	Tick++;
}
void SetTick(uint32_t ntick)
{
	Tick = ntick;
}
uint32_t GetTick(void)
{
	return Tick;
}
/* I2C ISR */
void I2C1_IRQHandler(void)
{
	if(I2C_Recive_IT(I2C1) != 0)
	{
		OverRunHandler();
	}
}
void I2C2_IRQHandler(void)
{
	if(I2C_Transmit_IT(I2C2) != 0)
	{
		OverRunHandler();
	}
}

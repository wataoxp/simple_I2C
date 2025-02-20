/*
 * i2c.c
 *
 *  Created on: Nov 5, 2024
 *      Author: wataoxp
 */

#include "i2c.h"
#include <string.h>

/* 割り込み送信用のバッファ */
static uint8_t SendBuffer[128];
static uint8_t SendSize;
/* 割り込み受信用のバッファ */
static uint8_t Buf[UINT8_MAX];
static uint8_t BufSize = 0;
static I2C_IsrTypedef obj = {
		.BufAdd = Buf,
		.BufSize = 0,
};

void I2C_Config_Master(I2C_TypeDef *I2Cx)
{
	uint32_t Timing = 0x00C12166;	//400KHz

	LL_I2C_Disable(I2Cx);
	LL_I2C_ConfigFilters(I2Cx, LL_I2C_ANALOGFILTER_ENABLE,0);
	LL_I2C_SetTiming(I2Cx, Timing);

	LL_I2C_Enable(I2Cx);
	LL_I2C_EnableAutoEndMode(I2Cx);
	LL_I2C_EnableClockStretching(I2Cx);
}
void I2C_Config_Slave(I2C_TypeDef *I2Cx,uint8_t SlaveAddr)
{
	uint32_t Timing = 0x00C12166;	//400KHz

	LL_I2C_Disable(I2Cx);
	LL_I2C_ConfigFilters(I2Cx, LL_I2C_ANALOGFILTER_ENABLE,0);
	LL_I2C_SetTiming(I2Cx, Timing);

	/* I2C Interrupt */
	LL_I2C_EnableIT_ADDR(I2Cx);
	LL_I2C_EnableIT_STOP(I2Cx);
	LL_I2C_EnableIT_RX(I2Cx);
	LL_I2C_EnableIT_TX(I2Cx);

	/* Slave Mode */
	LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
	LL_I2C_DisableGeneralCall(I2Cx);
	LL_I2C_SetOwnAddress1(I2Cx, (SlaveAddr << I2C_OA1_7BIT_Pos), LL_I2C_OWNADDRESS1_7BIT);
	LL_I2C_EnableOwnAddress1(I2Cx);
	LL_I2C_DisableOwnAddress2(I2Cx);

	LL_I2C_Enable(I2Cx);
}
void PushI2C_Mem_Write(I2C_TypeDef *I2Cx,uint8_t address,uint8_t data,uint16_t Reg,uint8_t RegSize)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	CR2SetUP(I2Cx, address, LL_I2C_REQUEST_WRITE, RegSize+1, LL_I2C_MODE_AUTOEND);

	if(RegSize == I2C_MEMADD_SIZE_8BIT)
	{
		LL_I2C_TransmitData8(I2Cx, (Reg & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}
	else
	{
		LL_I2C_TransmitData8(I2Cx, ((Reg >> 8) & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
		LL_I2C_TransmitData8(I2Cx, (Reg & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}
	LL_I2C_TransmitData8(I2Cx, data);

	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
	LL_I2C_ClearFlag_STOP(I2Cx);
}
void SeqI2C_Mem_Write(I2C_TypeDef *I2Cx,uint8_t address,uint8_t *data,uint16_t Reg,uint8_t RegSize,uint8_t length)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	//LL_I2C_HandleTransfer(I2Cx, (uint8_t)address, LL_I2C_ADDRSLAVE_7BIT, size+1, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

	CR2SetUP(I2Cx, address, LL_I2C_REQUEST_WRITE, RegSize+length, LL_I2C_MODE_AUTOEND);

	if(RegSize == I2C_MEMADD_SIZE_8BIT)
	{
		LL_I2C_TransmitData8(I2Cx, (Reg & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}
	else
	{
		LL_I2C_TransmitData8(I2Cx, ((Reg >> 8) & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
		LL_I2C_TransmitData8(I2Cx, (Reg & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}

	while(length)
	{
		LL_I2C_TransmitData8(I2Cx, *data++);
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
		length--;
	}
	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
	LL_I2C_ClearFlag_STOP(I2Cx);
}
void I2C_Master_Transmit(I2C_TypeDef *I2Cx,uint8_t address,uint8_t *data,uint8_t length)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	CR2SetUP(I2Cx, address, LL_I2C_REQUEST_WRITE, length, LL_I2C_MODE_AUTOEND);

	while(length)
	{
		LL_I2C_TransmitData8(I2Cx, *data++);
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
		length--;
	}
	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
	LL_I2C_ClearFlag_STOP(I2Cx);
}
void I2C_Mem_Read(I2C_TypeDef *I2Cx,uint8_t address,uint8_t *buffer,uint16_t Reg,uint8_t RegSize,uint8_t length)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);
	CR2SetUP(I2Cx, address, LL_I2C_REQUEST_WRITE , RegSize, LL_I2C_MODE_SOFTEND);

	if(RegSize == I2C_MEMADD_SIZE_8BIT)
	{
		LL_I2C_TransmitData8(I2Cx, (Reg & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}
	else
	{
		LL_I2C_TransmitData8(I2Cx, ((Reg >> 8) & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
		LL_I2C_TransmitData8(I2Cx, (Reg & 0xFF));
		while(LL_I2C_IsActiveFlag_TXE(I2Cx) == 0);
	}

	while(LL_I2C_IsActiveFlag_TC(I2Cx) == 0); 	//転送が完了したかチェック

	CR2SetUP(I2Cx, address, LL_I2C_REQUEST_READ, length, LL_I2C_MODE_AUTOEND);

	while(length)
	{
		while(LL_I2C_IsActiveFlag_RXNE(I2Cx) == 0);
		*buffer++ = LL_I2C_ReceiveData8(I2Cx);
		length--;
	}
	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
	LL_I2C_ClearFlag_STOP(I2Cx);
}
void I2C_ISR_Mem_Write(I2C_TypeDef *I2Cx,uint8_t address,uint8_t *data,uint16_t Reg,uint8_t RegSize,uint8_t length)
{
	uint8_t i = 0;
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	CR2SetUP(I2Cx, address, LL_I2C_REQUEST_WRITE, RegSize+length, LL_I2C_MODE_AUTOEND);

	if(RegSize == I2C_MEMADD_SIZE_8BIT)
	{
		Buf[i++] = (uint8_t)Reg & 0xFF;
	}
	else
	{
		Buf[i++] = (uint8_t)(Reg >> 8);
		Buf[i++] = (uint8_t)Reg & 0xFF;
	}
	memcpy(Buf+i,data,length);
}
void I2C_ISR_Master_Transmit(I2C_TypeDef *I2Cx,uint8_t address,uint8_t *data,uint8_t length)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2Cx) != 0);

	CR2SetUP(I2Cx, address, LL_I2C_REQUEST_WRITE, length, LL_I2C_MODE_AUTOEND);

	memcpy(SendBuffer,data,length);
}
uint8_t I2C_Recive_IT(I2C_TypeDef *I2Cx)
{
	if(BufSize >= sizeof(Buf))
	{
		return 1;
	}

	if(LL_I2C_IsActiveFlag_ADDR(I2Cx))
	{
		LL_I2C_ClearFlag_ADDR(I2Cx);
	}
	else if(LL_I2C_IsActiveFlag_RXNE(I2Cx))
	{
		Buf[BufSize++] = LL_I2C_ReceiveData8(I2Cx);
	}
	else if(LL_I2C_IsActiveFlag_STOP(I2Cx))
	{
		LL_I2C_ClearFlag_STOP(I2Cx);
	}

	return 0;
}
uint8_t I2C_Transmit_IT(I2C_TypeDef *I2Cx)
{
	if(SendSize >= sizeof(SendBuffer))
	{
		return 1;
	}

	if(LL_I2C_IsActiveFlag_TXIS(I2Cx))
	{
		LL_I2C_TransmitData8(I2Cx, SendBuffer[SendSize++]);
	}
	else if(LL_I2C_IsActiveFlag_STOP(I2Cx))
	{
		LL_I2C_ClearFlag_STOP(I2Cx);
	}

	return 0;
}
I2C_IsrTypedef* I2C_GetBufferAddress(void)
{
	obj.BufSize = BufSize;
	BufSize = 0;

	return &obj;
}

/*
 * gpio.h
 *
 *  Created on: Jan 12, 2025
 *      Author: wataoxp
 */

#ifndef INC_GPIO_H_
#define INC_GPIO_H_

#include "periph.h"

#define GPIO_BASE  (0x00000400UL)
#define GPIO_SET_PORT(x) ((GPIO_TypeDef *)(IOPORT_BASE + GPIO_BASE * x))

typedef enum{
	Pin0,
	Pin1,
	Pin2,
	Pin3,
	Pin4,
	Pin5,
	Pin6,
	Pin7,
	Pin8,
	Pin9,
	Pin10,
	Pin11,
	Pin12,
	Pin13,
	Pin14,
	Pin15,
}GPIO_Pin;

typedef enum{
	PORTA,
	PORTB,
	PORTC,
	PORTD,
	PORTF = 5,
}GPIO_Port;

typedef enum{
	init_Success,
	init_Failed,
}GPIO_Code;

typedef struct{
	uint32_t PinPos;
	uint32_t Mode;
	uint32_t Speed;
	uint32_t OutputType;
	uint32_t Pull;
	uint32_t Alternate;
}GPIO_InitTypedef;

static inline void GPIO_SetPinSpeed(GPIO_TypeDef *GPIOx,uint32_t pinpos,uint32_t speed)
{
	MODIFY_REG(GPIOx->OSPEEDR,GPIO_OSPEEDR_OSPEED0 << (pinpos * 2),(speed << (pinpos * 2)));
}
static inline void GPIO_SetPinPull(GPIO_TypeDef *GPIOx,uint32_t pinpos,uint32_t pull)
{
	MODIFY_REG(GPIOx->PUPDR,GPIO_PUPDR_PUPD0 << (pinpos * 2),(pull << (pinpos * 2)));
}
static inline void GPIO_SetPinMode(GPIO_TypeDef *GPIOx,uint32_t pinpos,uint32_t mode)
{
	MODIFY_REG(GPIOx->MODER,GPIO_MODER_MODE0 << (pinpos * 2),(mode << (pinpos * 2)));
}
static inline void GPIO_SetOutputPinType(GPIO_TypeDef *GPIOx,uint32_t pinmask,uint32_t outputType)
{
	MODIFY_REG(GPIOx->OTYPER,pinmask,(pinmask * outputType));
}
static inline void GPIO_SetAlternate0_7(GPIO_TypeDef* GPIOx,uint32_t pinpos,uint32_t alternate)
{
	MODIFY_REG(GPIOx->AFR[0],GPIO_AFRL_AFSEL0 << (pinpos * 4),alternate << (pinpos * 4));
}
static inline void GPIO_SetAlternate8_15(GPIO_TypeDef* GPIOx,uint32_t pinpos,uint32_t alternate)
{
	pinpos -= 8;
	MODIFY_REG(GPIOx->AFR[1],GPIO_AFRH_AFSEL8 << (pinpos * 4),alternate << (pinpos * 4));
}
/*
 * SetAF8~15について
 * AFRレジスタは16個のピンを8・8で設定するレジスタになっている。
 * このためPin8(8U)以上の値を渡すとうまくセットできない。
 * Pin8であるならAFR[1]の0~3ビットにセットしたいので、8を引く。
 * これによってPin8~Pin15(7U)と適切な値をセットできる。
 *
 * じゃあ最初からPin8~Pin15を別枠で用意すればいいじゃないか、という訳にはならない
 * AFR以外のレジスタは単にPin8の8Uという値を必要とするので、あくまでAFR[1]だけの話になる。
 */
GPIO_Code GPIO_OutputInit(GPIO_TypeDef *GPIOx,GPIO_InitTypedef *InitStruct);
GPIO_Code GPIO_InputInit(GPIO_TypeDef *GPIOx,GPIO_Port port,uint32_t PinPos,uint32_t Mode,uint32_t Pull);


#endif /* INC_GPIO_H_ */

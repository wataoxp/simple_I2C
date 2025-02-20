#include "main.h"


#include <string.h>

static void RCC_Config(void);
static void GPIO_OutputConfig(GPIO_Port port,uint32_t Pos);
static void I2C1_Config(void);
static void I2C2_Config(void);

int main(void)
{
	uint8_t sendbuf[] = {0,1,2,3,4,5,6,7,8,9,};
	uint8_t recvbuf[10];
	int8_t size;
	I2C_IsrTypedef *pStatus;

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

	RCC_Config();
	SysTick_Init(MILL_SECOND);
	GPIO_OutputConfig(PORTA, Pin0);

	I2C1_Config();
	I2C2_Config();

	I2C_TransmitMode(I2C2, TX_ISR);
	I2C_ISR_Master_Transmit(I2C2, (0x01 << 1), sendbuf, sizeof(sendbuf));
	//I2C_Master_Transmit(I2C2, (0x01 << 1), sendbuf, sizeof(sendbuf));

	Delay(500);
	pStatus = I2C_GetBufferAddress();

	memset(recvbuf,0xff,sizeof(recvbuf));

	for(size = (pStatus->BufSize - 1);size >= 0;size--)
	{
		recvbuf[size] = pStatus->BufAdd[size];
	}
	memset(pStatus->BufAdd,0,pStatus->BufSize);		//SlaveBufferClear

	while(1)
	{
		WRITE_REG(GPIOA->BSRR,1<<Pin0);
		Delay(100);
		WRITE_REG(GPIOA->BRR,1<<Pin0);
		Delay(100);
	}
}
static void RCC_Config(void)
{
	RCC_InitTypedef rcc;
	rcc.Latency = LL_FLASH_LATENCY_2;
	rcc.PLLSrc = LL_RCC_PLLSOURCE_HSI;
	rcc.PLLM = LL_RCC_PLLM_DIV_1;
	rcc.PLLN = 8;
	rcc.PLLR = LL_RCC_PLLR_DIV_2;
	rcc.AHBdiv = LL_RCC_SYSCLK_DIV_1;
	rcc.SysClkSrc = LL_RCC_SYS_CLKSOURCE_PLL;
	rcc.APBdiv = LL_RCC_APB1_DIV_1;
	rcc.clock = 64000000;

	RCC_InitG0(&rcc);
}
static void GPIO_OutputConfig(GPIO_Port port,uint32_t Pos)
{
	GPIO_TypeDef *GPIOx = GPIO_SET_PORT(port);
	GPIO_InitTypedef init;
	init.PinPos = Pos;
	init.Pull = LL_GPIO_PULL_NO;
	init.Mode = LL_GPIO_MODE_OUTPUT;
	init.Speed = LL_GPIO_SPEED_FREQ_LOW;
	init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;

	if(LL_IOP_GRP1_IsEnabledClock(1 << port) == 0)
	{
		LL_IOP_GRP1_EnableClock(1 << port);
	}
	GPIO_OutputInit(GPIOx, &init);
}
static void I2C1_Config(void)
{
	GPIO_InitTypedef init = {
		.Mode = LL_GPIO_MODE_ALTERNATE,
		.Speed = LL_GPIO_SPEED_FREQ_LOW,
		.OutputType = LL_GPIO_OUTPUT_OPENDRAIN,
		.Pull = LL_GPIO_PULL_NO,
		.Alternate = LL_GPIO_AF_6,
	};
	if(LL_IOP_GRP1_IsEnabledClock(1 << PORTB) == 0)
	{
		LL_IOP_GRP1_EnableClock(1 << PORTB);
	}
	init.PinPos = Pin8;		//SCL
	GPIO_OutputInit(GPIOB, &init);
	init.PinPos = Pin9;		//SDA
	GPIO_OutputInit(GPIOB, &init);

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

	/* I2C1 ISR */
	__NVIC_SetPriority(I2C1_IRQn, 0);
	__NVIC_EnableIRQ(I2C1_IRQn);

	I2C_Config_Slave(I2C1, 0x01);
}
static void I2C2_Config(void)
{
	GPIO_InitTypedef init = {
		.Mode = LL_GPIO_MODE_ALTERNATE,
		.Speed = LL_GPIO_SPEED_FREQ_LOW,
		.OutputType = LL_GPIO_OUTPUT_OPENDRAIN,
		.Pull = LL_GPIO_PULL_NO,
		.Alternate = LL_GPIO_AF_6,
	};
	if(LL_IOP_GRP1_IsEnabledClock(1 << PORTA) == 0)
	{
		LL_IOP_GRP1_EnableClock(1 << PORTA);
	}
	init.PinPos = Pin11;	//SCL
	GPIO_OutputInit(GPIOA, &init);

	init.PinPos = Pin12;	//SDA
	GPIO_OutputInit(GPIOA, &init);

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);

	/* I2C2 ISR */
	__NVIC_SetPriority(I2C2_IRQn, 1);
	__NVIC_DisableIRQ(I2C2_IRQn);

	I2C_Config_Master(I2C2);
}

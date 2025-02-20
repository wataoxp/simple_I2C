/*
 * periph.h
 *
 *  Created on: Feb 17, 2025
 *      Author: wataoxp
 */

#ifndef SRC_PERIPH_H_
#define SRC_PERIPH_H_

#define STM32G0xx

#ifdef STM32G0xx
#include "stm32g0xx.h"

/* System Headers */
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_exti.h"
#include "stm32g0xx_ll_cortex.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_pwr.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_gpio.h"

/* Peripheral Headers */
#include "stm32g0xx_ll_i2c.h"

#endif

#endif /* SRC_PERIPH_H_ */

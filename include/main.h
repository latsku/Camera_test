/*
 * main.h
 *
 *  Created on: 2014-08-04
 *      Author: Lari Lehtomäki
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "stm32f4xx.h"
#include "system_stm32f4xx.h"

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_dcmi.h"
#include "stm32f4xx_usart.h"
#include "dcmi_ov2640.h"
#include "misc.h"

#ifdef __cplusplus
 extern "C" {
#endif



void Delay(uint32_t nTime);
void TimingDelay_Decrement(void);


#ifdef __cplusplus
}
#endif

#endif /* MAIN_H_ */

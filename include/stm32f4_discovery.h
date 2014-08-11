/*
 * stm32f4_discovery.h
 *
 *  Created on: 2014-08-04
 *      Author: Lari Lehtomäki
 */

#ifndef STM32F4_DISCOVERY_H_
#define STM32F4_DISCOVERY_H_

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

#define USART2_TX_PORT	GPIOA
#define USART2_RX_PORT	GPIOA
#define USART2_TX_PIN	GPIO_Pin_2
#define USART2_RX_PIN	GPIO_Pin_3
#define USART2_TX_PIN_SOURCE	GPIO_PinSource2
#define USART2_RX_PIN_SOURCE	GPIO_PinSource3

// GPIOD
#define GREEN_LED 		GPIO_Pin_12
#define ORANGE_LED		GPIO_Pin_13
#define RED_LED			GPIO_Pin_14
#define BLUE_LED		GPIO_Pin_15

// GPIOA
#define PUSHBUTTON		GPIO_Pin_0



void InitLeds(void) {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	// Configure pin in output push/pull mode
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GREEN_LED;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ORANGE_LED;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = RED_LED;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = BLUE_LED;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOD, GREEN_LED);
	GPIO_ResetBits(GPIOD, ORANGE_LED);
	GPIO_ResetBits(GPIOD, RED_LED);
	GPIO_ResetBits(GPIOD, BLUE_LED);
}

void BlinkGreen(int count) {

	while (count > 0)
	{
		GPIO_ToggleBits(GPIOD, GREEN_LED);
		Delay(1500);
		GPIO_ToggleBits(GPIOD, GREEN_LED);
		count--;
	}

}

void BlinkOrange(int count) {

	while (count > 0)
	{
		GPIO_ToggleBits(GPIOD, ORANGE_LED);
		Delay(1500);
		GPIO_ToggleBits(GPIOD, ORANGE_LED);
		count--;
	}

}

void BlinkBlue(int count) {

	while (count > 0)
	{
		GPIO_ToggleBits(GPIOD, BLUE_LED);
		Delay(1500);
		GPIO_ToggleBits(GPIOD, BLUE_LED);
		count--;
	}

}

void BlinkRed(int count) {

	while (count > 0)
	{
		GPIO_ToggleBits(GPIOD, RED_LED);
		Delay(1500);
		GPIO_ToggleBits(GPIOD, RED_LED);
		count--;
	}

}


#endif /* STM32F4_DISCOVERY_H_ */

/*
 * main.cpp
 *
 *  Created on: 30 Jul 2014
 *      Author: Lari Lehtomäki
 */

#include "main.h"
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"

unsigned int Targetbuffer[160*120];
static __IO uint32_t TimingDelay;

void print_CameraData();
void Serial_print(USART_TypeDef *USARTx, char *s);
void Serial_print(USART_TypeDef *USARTx, int value, int base);
void Serial_println(USART_TypeDef *USARTx, char *s);

#ifdef __cplusplus
 extern "C" {
#endif

void Serial_print_c(USART_TypeDef *USARTx, int value, int base) {
	Serial_print(USARTx, value, base);
}

void Serial_println_c(USART_TypeDef *USARTx, char *s) {
	Serial_println(USARTx, s);
}

int main() {
	SysTick_Config(SystemCoreClock / 1000);

	InitLeds();
	GPIO_SetBits(GPIOD,   GREEN_LED);
	Delay(100);
	GPIO_ResetBits(GPIOD, GREEN_LED);
	GPIO_SetBits(GPIOD,   ORANGE_LED);
	Delay(100);
	GPIO_ResetBits(GPIOD, ORANGE_LED);
	GPIO_SetBits(GPIOD,   RED_LED);
	Delay(100);
	GPIO_ResetBits(GPIOD, RED_LED);
	GPIO_SetBits(GPIOD,   BLUE_LED);
	Delay(100);
	GPIO_ResetBits(GPIOD, BLUE_LED);


	/**
	 * USART
	 */

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_PinAFConfig(USART2_TX_PORT, USART2_TX_PIN_SOURCE, GPIO_AF_USART2);
	GPIO_PinAFConfig(USART2_RX_PORT, USART2_RX_PIN_SOURCE, GPIO_AF_USART2);

	/* Configure USART Tx as alternate function  */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Pin = USART2_TX_PIN;
	GPIO_Init(USART2_TX_PORT, &GPIO_InitStructure);

	/* Configure USART Rx as alternate function  */
	GPIO_InitStructure.GPIO_Pin = USART2_RX_PIN;
	GPIO_Init(USART2_RX_PORT, &GPIO_InitStructure);

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	USART2->CR3 |= USART_CR3_DMAT;

	/* Enable USART */
	USART_Cmd(USART2, ENABLE);

	Serial_print(USART2, "Hello World!  --  Compiled on: ");
	Serial_print(USART2, __DATE__);
	Serial_print(USART2, " - ");
	Serial_println(USART2, __TIME__);



	/**
	 * Interrupts
	 */

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DCMI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	DCMI->IER |= DCMI_IER_FRAME_IE;

	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	Serial_print(USART2, "Interrupts done. \r\n");

	/** Camera Reset Pin */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_10);
	GPIO_ResetBits(GPIOA, GPIO_Pin_9);

	/**
	 * DCMI
	 */
	OV2640_HW_Init();
	Serial_print(USART2, "HW_Init done. \r\n");
	/* Print camera Id */
	OV2640_IDTypeDef camera_id;
	OV2640_ReadID(&camera_id);
	Serial_print(USART2, "Camera ID: ");
	Serial_print(USART2, camera_id.Manufacturer_ID1, 16);
	Serial_print(USART2, " - ");
	Serial_print(USART2, camera_id.Manufacturer_ID2, 16);
	Serial_print(USART2, " - ");
	Serial_print(USART2, camera_id.PIDH, 16);
	Serial_print(USART2, " - ");
	Serial_print(USART2, camera_id.PIDL, 16);
	Serial_println(USART2, "");

	OV2640_QQVGAConfig();
	Serial_print(USART2, "QQVGAConfig done. \r\n");
	// OV2640_BandWConfig(0x18); // BW


	OV2640_Init(BMP_QQVGA);
	Serial_print(USART2, "Init done. \r\n");

	// Memset
	int i;
	for (i=0; i<160*120; i++) {
		Targetbuffer[i] = 0xdeadbeef;
	}

	DMA_Cmd(DMA2_Stream1, ENABLE);
	Serial_print(USART2, "DMA Enable done. \r\n");

	DCMI_Cmd(ENABLE);
	Serial_print(USART2, "DCMI Enable done. \r\n");
	DCMI_CaptureCmd(ENABLE);
	Serial_print(USART2, "DCMI Capture start. \r\n");

	Delay(1000);
	Serial_print(USART2, "Print: \r\n.\r\n.\r\n");
	print_CameraData();
	Serial_print(USART2, "done. \r\n");

}

#ifdef __cplusplus
}
#endif


void print_CameraData() {
	int i;
	for (i = 0; i < 10; i++) {
		Serial_print(USART2, (Targetbuffer[i] >> 24) & 0xff, 16);
		Serial_print(USART2, (Targetbuffer[i] >> 16) & 0xff, 16);
		Serial_print(USART2, (Targetbuffer[i] >> 8)  & 0xff, 16);
		Serial_print(USART2, (Targetbuffer[i])       & 0xff, 16);
		Serial_print(USART2, "\r\n");
	}
}


void Serial_print(USART_TypeDef *USARTx, char *s) {

	while (*s) {
		while( !(USARTx->SR & USART_SR_TC) );
		USART_SendData(USARTx, *s++);
	}
}

void Serial_print(USART_TypeDef *USARTx, int value, int base)
{
    char buffer[8 * sizeof(value) + 1];
    // Null byte to the end.
    char *str = &buffer[sizeof(buffer) - 1];
    *str = '\0';

    // If negative, print minus and make it positive.
    if ( value < 0 ) {
        Serial_print(USARTx, "-");
        value = -value;
    }

    // Loop the number from least significant to most significant.
    do {
        int m = value;
        value /= base;
        char c = m - base * value;

        if ( c < 10 ) {
            // Numbers are continously in ASCII from 0 to 9 at 48 to 57
            *--str = c + '0';
        } else {
            // Capital letters are also continously in ASCII starting with A in 65
            *--str = c + 'A' - 10;
        }
    } while(value);

    Serial_print(USARTx, str);
}

void Serial_println(USART_TypeDef *USARTx, char *s) {
	Serial_print(USARTx, s);
	Serial_print(USARTx, "\r\n");
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds
  * @retval None
  */
void Delay(uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);

}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}

#ifdef __cplusplus
 extern "C" {
#endif

void assert_failed(uint8_t* file, uint32_t line) {
	Serial_print(USART2, (char*)file);
	Serial_print(USART2, ":");
	Serial_print(USART2, (int)line, 10);
	Serial_println(USART2, " Assert failed");
	while (1) {
		BlinkBlue(1);
	}
}



void DCMI_IRQHandler(void) {
	GPIO_ToggleBits(GPIOD, GREEN_LED);
	while (1)
	{
	}
}

void DMA2_Stream1_IRQHandler(void) {
	GPIO_ToggleBits(GPIOD, ORANGE_LED);
	while (1)
	{
	}
}

#ifdef __cplusplus
}
#endif

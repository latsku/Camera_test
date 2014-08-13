/*
 * main.cpp
 *
 *  Created on: 30 Jul 2014
 *      Author: Lari Lehtomäki
 */

#include "main.h"
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"

#define VERTICAL_RESOLUTION 120
#define HORIZONTAL_RESOLUTION 160

uint16_t Targetbuffer[160*120];
static __IO uint32_t TimingDelay;


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


void print_CameraDataHex() {
	int line, column;
	for (line = 0; line < VERTICAL_RESOLUTION; ) {
		for (column = 0; column < HORIZONTAL_RESOLUTION;) {
//			Serial_print(USART2, (Targetbuffer[line*HORIZONTAL_RESOLUTION+column] >> 8)  & 0xff, 16);
			Serial_print(USART2, (Targetbuffer[line*HORIZONTAL_RESOLUTION+column])       , 16);
			column = column + 8;
			Serial_print(USART2, " ");
		}
		Serial_print(USART2, "\r\n");
		line = line + 4;
	}
}

void print_CameraData() {
	int line, column;
	for (line = 0; line < VERTICAL_RESOLUTION; ) {
		for (column = 0; column < HORIZONTAL_RESOLUTION;) {

			if ( Targetbuffer[line*HORIZONTAL_RESOLUTION+column] > 0x00 && Targetbuffer[line*HORIZONTAL_RESOLUTION+column] < 0x4000 ) {
				Serial_print(USART2, " ");
			}
			if ( Targetbuffer[line*HORIZONTAL_RESOLUTION+column] >= 0x4000 && Targetbuffer[line*HORIZONTAL_RESOLUTION+column] < 0x8000 ) {
				Serial_print(USART2, ".");
			}
			if ( Targetbuffer[line*HORIZONTAL_RESOLUTION+column] >= 0x8000 && Targetbuffer[line*HORIZONTAL_RESOLUTION+column] < 0xb000 ) {
				Serial_print(USART2, "o");
			}
			if ( Targetbuffer[line*HORIZONTAL_RESOLUTION+column] >= 0xb000 && Targetbuffer[line*HORIZONTAL_RESOLUTION+column] < 0xffff ) {
				Serial_print(USART2, "#");
			}

			column = column + 2;
		}
		Serial_print(USART2, "\r\n");
		line = line + 4;
	}
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

void Serial_print_c(USART_TypeDef *USARTx, int value, int base) {
	Serial_print(USARTx, value, base);
}

void Serial_println_c(USART_TypeDef *USARTx, char *s) {
	Serial_println(USARTx, s);
}

void assert_failed(uint8_t* file, uint32_t line) {
	Serial_print(USART2, (char*)file);
	Serial_print(USART2, ":");
	Serial_print(USART2, (int)line, 10);
	Serial_println(USART2, " Assert failed");
	while (1) {
		BlinkBlue(1);
	}
}


void ms_delay(int ms)
{
   while (ms-- > 0) {
      volatile int x=5971;
      while (x-- > 0)
         __asm("nop");
   }
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
	DCMI_ClearITPendingBit(DCMI_IT_FRAME | DCMI_IT_OVF | DCMI_IT_ERR);
	DCMI_ITConfig(DCMI_IT_FRAME | DCMI_IT_OVF | DCMI_IT_ERR, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1 | DMA_IT_TEIF1);
	DMA_ITConfig(DMA2_Stream1, DMA_IT_TC | DMA_IT_FE | DMA_IT_TE | DMA_IT_DME, ENABLE);
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
		Targetbuffer[i] = 0xdead;
	}

	DMA_Cmd(DMA2_Stream1, ENABLE);
	Serial_print(USART2, "DMA Enable done. \r\n");

	DCMI_Cmd(ENABLE);
	Serial_print(USART2, "DCMI Enable done. \r\n");
	DCMI_CaptureCmd(ENABLE);
	Serial_print(USART2, "DCMI Capture start. \r\n");

	Delay(1000);
	Serial_print(USART2, "Print: \r\n.\r\n.\r\n");
	Serial_print(USART2, "done. \r\n");

	while (1) {
		while (DCMI_GetFlagStatus(DCMI_FLAG_FRAMERI) == RESET)
			;

		print_CameraData();
		Serial_print(USART2, 0x1b);
		Serial_print(USART2, "[2J");
		Serial_print(USART2, 0x1b);
		Serial_print(USART2, "[H");

	}
}


void DCMI_IRQHandler(void) {
	GPIO_SetBits(GPIOD, GREEN_LED);
//	Serial_print(USART2, DCMI->RISR, 16);

	/* DCMI overrun */
	if ( DCMI_GetITStatus(DCMI_IT_OVF) != RESET) { // Overflow interrupt mask
		while (1){
			GPIO_SetBits(GPIOD, GREEN_LED);
			ms_delay(500);
		}
		DCMI_ClearITPendingBit(DCMI_IT_OVF);
	}

	if ( DCMI_GetITStatus(DCMI_IT_FRAME) != RESET) { // Frame capture complete interrupt mask
		DCMI_ClearITPendingBit(DCMI_IT_FRAME);
	}

	if ( DCMI_GetITStatus(DCMI_IT_ERR) != RESET) { // Synchronization error interrupt mask
		while (1)
			;
		DCMI_ClearITPendingBit(DCMI_IT_ERR);
	}

	if ( DCMI_GetITStatus(DCMI_IT_LINE) != RESET) { // Line interrupt mask
		DCMI_ClearITPendingBit(DCMI_IT_LINE);
	}

	if ( DCMI_GetITStatus(DCMI_IT_VSYNC) != RESET) { // Line interrupt mask
		DCMI_ClearITPendingBit(DCMI_IT_VSYNC);
	}

}

void DMA2_Stream1_IRQHandler(void) {
	GPIO_SetBits(GPIOD, ORANGE_LED);

	if ( DMA_GetITStatus(DMA2_Stream1, DMA_IT_DMEIF1) != RESET) { // direct mode error interrupt
		Serial_print(USART2, "direct mode error interrupt\r\n");
		while (1) {
			GPIO_ToggleBits(GPIOD, ORANGE_LED);
			ms_delay(200);
		}
		// DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_DMEIF1);
	}

	if ( DMA_GetITStatus(DMA2_Stream1, DMA_IT_FEIF1) != RESET) { // FIFO error interrupt
		Serial_print(USART2, "FIFO error interrupt\r\n");
		while (1) {
			GPIO_ToggleBits(GPIOD, ORANGE_LED);
			ms_delay(800);
		}
		// DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_FEIF1);
	}

	if ( DMA_GetITStatus(DMA2_Stream1, DMA_IT_HTIF1) != RESET) { // half transfer complete interrupt

		DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_HTIF1);
	}

	if ( DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1) != RESET) { // transfer complete interrupt

		DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);
	}

	if ( DMA_GetITStatus(DMA2_Stream1, DMA_IT_TEIF1) != RESET) { // transfer error interrupt
		Serial_print(USART2, "transfer error interrupt\r\n");
		while (1) {
			GPIO_ToggleBits(GPIOD, ORANGE_LED);
			ms_delay(1500);
		}
		// DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TEIF1);
	}

}


#ifdef __cplusplus
}
#endif

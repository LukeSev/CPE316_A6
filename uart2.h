/*
 * uart2.h
 *
 *  Created on: May 9, 2023
 *      Author: lucasloughner
 */

#ifndef SRC_UART2_H_
#define SRC_UART2_H_
#endif /* SRC_UART2_H_ */


char number_codes[10] = {'0', '1', '2', '3', '4',
						 '5', '6', '7', '8', '9'};

void UART_init(void)
{
	/* BEGIN GPIO -----------------------------------------------------------*/

	// Configure Tx (PA2) and Rx (PA3)

	// set AF to 7 for both Tx and Rx
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFRL2);
	GPIOA->AFR[0] |= (0x7 << GPIO_AFRL_AFSEL2_Pos);

	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFRL3);
	GPIOA->AFR[0] |= (0x7 << GPIO_AFRL_AFSEL3_Pos);

	// set OTYPE to PP (0) for both Tx and Rx
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT2);
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT3);

	// set OSPEED to low (00) for both Tx and Rx
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED2);
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED3);

	// set PUPD to no resistors (00) for both Tx and Rx
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD2);
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD3);

	// set MODER to AF
	GPIOA->MODER &= ~(GPIO_MODER_MODE2); 	// clear MODER
	GPIOA->MODER |= GPIO_MODER_MODE2_1;		// set to 10 (AF)

	GPIOA->MODER &= ~(GPIO_MODER_MODE3);	// clear MODER
	GPIOA->MODER |= GPIO_MODER_MODE3_1;		// set to 10 (AF)

	/* END GPIO -------------------------------------------------------------*/

	// Program the M bits in USART_CR1
	USART2->CR1 &= ~(USART_CR1_M1);
	USART2->CR1 &= ~(USART_CR1_M0);

	// set the baud rate
	USART2->BRR = USARTDIV;

	// set stop bits to 1
	USART2->CR2 &= ~(USART_CR2_STOP);	// set to 00 (1 stop bit)]

	// enable the USART
	USART2->CR1 |= (USART_CR1_UE);

	// send an idle frame as first transmission
	USART2->CR1 |= (USART_CR1_TE);

	// enable reception interrupt
	USART2->CR1 |= (USART_CR1_RXNEIE);
	USART2->RQR |= (USART_RQR_RXFRQ);	// clear flag

	// enable the receiver
	USART2->CR1 |= (USART_CR1_RE);

}

void UART_print(char string[])
{
	int i = 0;
	while (string[i] != '\0')
	{
		while(!(USART2->ISR & USART_ISR_TC)); // wait for TC to be equal to 1
		USART2->TDR = string[i]; 			   // write to UART_TDR
		i++;
	}
}

void UART_ESC_Code(char string[])
{
	USART2->TDR = ESC;	// send ESC key
	int i = 0;
	while (string[i] != '\0')
	{
		while(!(USART2->ISR & USART_ISR_TC)); 	// wait for TC to be equal to 1
		USART2->TDR = string[i]; 			   	// write to UART_TDR
		i++;
	}
}

void UART_print_mV(uint16_t mv)
{
	// Takes a voltage in mV, prints to screen in V
	// Iteratively print one digit at a time, going to 2 decimal places
	uint8_t digit;
	char output[2] = {'', '\0'};

	// Start with ones digit (how many volts)
	digit = (uint8_t)(mv / 1000);
	output[0] = number_codes[digit];
	UART_print(output);
	mv -= (digit*1000);

	// Print decimal point
	output[0] = '.';
	UART_print(output);

	// Tenth's digit
	digit = (uint8_t)(mv / 100);
	output[0] = number_codes[digit];
	UART_print(output);
	mv -= (digit*100);

	// Hundredth's digit
	digit = (uint8_t)(mv / 10);
	output[0] = number_codes[digit];
	UART_print(output);
	mv -= (digit*10);
}

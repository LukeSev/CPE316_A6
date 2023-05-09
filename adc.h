/*
 * adc.h
 *
 *  Created on: May 9, 2023
 *      Author: lucasloughner
 */

#ifndef SRC_ADC_H_
#define SRC_ADC_H_
#endif /* SRC_ADC_H_ */

#define SMP_0 0			// 	2.5 	ADC cycles
#define SMP_1 4			// 	47.5 	ADC cycles
#define SMP_2 7			// 	640.5 	ADC cycles
#define CAL 1			//	Calibration, placeholder for now

void ADC_init()
{
	// Configure GPIOA --> ADC_in5 = PA4
	// Set to Analog mode (b'11)
	GPIOA->MODER |= (GPIO_MODER_MODE4);

	// Configure ADC
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
	ADC123_COMMON->CCR = (1 << ADC_CR_DEEPPWD);

	// Power it up
	ADC1->CR &= ~(ADC_CR_DEEPPWD);
	ADC1->CR |= (ADC_CR_ADVREGEN);
	for(uint32t i = 4000; i > 0; i--);		// Busy wait

	// Calibrate ADC
	ADC1->CR &= ~(ADC_CR_ADEN | ADC_CR_ADCALDIF);
	ADC1->CR |= ADC_CR_ADCAL;
	while(ADC1->CR & ADC_CR_ADCAL);

	// Configure for single-ended mode on channel 5
	ADC1->DIFSEL &= ~(ADC_DIFSEL_DIFSEL_5);

	// Enable ADC
	ADC1->ISR |= (ADC_ISR_ADRDY);			// Clear ready bit
	ADC1->CR |= ADC_CR_ADEN;				// Enable ADC
	while(!(ADC1->ISR & ADC_ISR_ADRDY));	// Wait for ready bit to become 1
	ADC1->ISR |= (ADC_ISR_ADRDY);			// Clear ready bit

	// Configure sequence
	ADC1->SQR1 = (5 << ADC_SQR1_SQ1_Pos);	// Channel 5

	// 12-bit resolution, software trigger
	ADC1->CFGR = 0;

	// Sampling time set to 2.5 ADC cycles
	ADC1->SMPR1 = SMP_0;

	ADC1->IER |= (ADC_IER_EOC);
	ADC1->ISR |= (ADC_ISR_EOC);
	NVIC->ISER = (1 << (ADC1_2_IRQn & 0x1F));

}

/*
 * pwm.c
 *
 *  Created on: Oct 9, 2021
 *      Author: Кирилл
 */
#include "pwm.h"

void pwm_gpio_init() { //настройка порта для шим
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
		GPIOC->MODER |= GPIO_MODER_MODER8_1; //AF
}

void pwm_init() {
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
		    TIM3->CR1 |= TIM_CR1_ARPE;
		    TIM3->ARR = 8096;
		    TIM3->PSC = 6;//делитель
		    TIM3->CCMR2 |= TIM_CCMR2_OC3M;
		    TIM3->CCMR2 |= TIM_CCMR2_OC3PE;
		    TIM3->CCER |= TIM_CCER_CC3P;
		    TIM3->CCER |= TIM_CCER_CC3E; //включение канала
		    TIM3->CR1 |= TIM_CR1_CEN; //включение
}

void pwm_set_val(uint8_t num_ch, uint16_t val) {

			TIM3->CCR3 = val;

}


/*
 * pwm.h
 *
 *  Created on: Oct 9, 2021
 *      Author: Кирилл
 */

#ifndef INC_PWM_H_
#define INC_PWM_H_

#include <stm32f0xx.h>

void pwm_gpio_init();

void pwm_init();

void pwm_set_val(uint8_t num_ch, uint16_t val);




#endif /* PWM_H_ */


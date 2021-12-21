/*
 * global.h
 *
 *  Created on: 2 нояб. 2021 г.
 *      Author: Kirill
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_


#include <stdint.h>
#include "stm32f0xx.h"
#include "gpio.h"
#include "spi_dma.h"
#include "tft.h"
#include "command.h"
#include "library.h"

extern uint8_t status_dma_tx;
extern uint16_t MAX_Y;
extern uint16_t MAX_X;
extern uint16_t Y_SIZE;
extern uint16_t X_SIZE;

#define FONT_Y 8
#define FONT_X 8

#define RESET_ACTIVE() GPIOA->ODR&=~GPIO_ODR_2
#define RESET_IDLE() GPIOA->ODR|=GPIO_ODR_2

#define DC_COMMAND() GPIOA->ODR&=~GPIO_ODR_3
#define DC_DATA() GPIOA->ODR|=GPIO_ODR_3

#define SET_CS() GPIOA->ODR|=GPIO_ODR_4
#define RESET_CS() GPIOA->ODR&=~GPIO_ODR_4

#define LED_on() GPIOA->ODR|=GPIO_ODR_1
#define LED_off() GPIOA->ODR&=~GPIO_ODR_1


#define BLACK 0x0000
#define GREEN 0x001F
#define RED 0xB800
#define BLUE 0x07E0
//#define GREEN 0x07E0
//#define RED 0xF800
//#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

//(blue & 0x1F) | ((green & 0x3F) << 5) | ((red & 0x1F) << 11)


#define Start_DMA_Send_Data() DMA1_Channel3->CCR |= DMA_CCR_EN
#define Stop_DMA_Send_Data() DMA1_Channel3->CCR &= ~DMA_CCR_EN


#endif /* GLOBAL_H_ */

/* Includes */
#include "stm32f0xx.h"
#include "ds18b20.h"
#include "pwm.h"
//for display
#include "global.h"
#include "gpio.h"
#include "tft.h"
#include "library.h"
#include "command.h"
#include "spi_dma.h"

uint8_t status_dma_tx=1;
uint8_t data_for_spi[8] = {0,0,0,0,0,0,0,0};
uint8_t data_for_spi_ready = 0;
uint8_t size_data_spi = 0;
uint16_t MAX_X=160;
uint16_t MAX_Y=128;
uint16_t Y_SIZE=0;
uint16_t X_SIZE=0;

void clock48Mhz_init() {
	RCC->CR &= ~RCC_CR_PLLON;
	while(RCC->CR & RCC_CR_PLLRDY);

	RCC->CFGR |= RCC_CFGR_PLLMUL12;

	RCC->CR |= RCC_CR_PLLON;
	while((RCC->CR & RCC_CR_PLLRDY) != RCC_CR_PLLRDY);

	RCC->CFGR |= RCC_CFGR_SW_1;	//as PLL

	SystemCoreClockUpdate();
}

#define PI

float temp_ust = 40;
float real_temp = 0;
uint32_t temperature = 0;
uint16_t pwm_val = 0;
uint16_t ticks = 100;
uint16_t Kp =1.5;// задаем коэффициенты
float Ki = 1.1;	//small.... 1.1
int32_t resP;
int16_t resI;

uint16_t P_set(){      //реализация P составляющей PI-регулятора
	resP = Kp * (temp_ust - real_temp);
	if (resP < 0) {
		resP = 0;
	}
}

uint16_t I_set(uint8_t reset) {    //реализация I составляющей PI-регулятора
	static float resI_f = 0;

	resI_f = Ki * (temp_ust - real_temp);

	if (reset == 1) {
		resI_f = 0;
	}

	resI = resI_f;

	if (resI_f < 0) {
		resI = 0;
	}

	return resI;
}

int main(void)
{
	clock48Mhz_init();
	TFT_init();
	ds18b20_Init(GPIOC, 13, 12);

#ifdef PI//
	pwm_init();
	pwm_set_val(1, pwm_val);
#endif

  /* TODO - Add your application code here */
	while (1)
	{
		ds18b20_StartMeas();
		for (int i = 0; i < 1000; i++) {
			Delay_tick(3400);
		}
		temperature = ds18b20_GetTemperature();
		real_temp = (temperature & 0xFFF) * 0.0625;
		real_temp *= (temperature & 0xF000) ? -1 : 1;
		float_to_string(real_temp);     // вывод значения температуры в строку на дисплей
#ifdef PI
		pwm_val = P_set() + I_set(0);	//управляющий сигнал PI-регулятора
		pwm_set_val(1, pwm_val);//регулировка скважности ШИМ через изменение управляющего сигнала PI-регулятора
#endif
		Delay_tick(100);
	}
}


#include "spi_dma.h"

uint8_t spi_buffer_tx[BUFFER_SPI_SIZE];
uint8_t spi_buffer_rx[BUFFER_SPI_SIZE];

void DMA1_Channel2_3_IRQHandler(){
	if (DMA1->ISR & DMA_ISR_TCIF3) {
		DMA1->IFCR |= DMA_IFCR_CTCIF3;
		status_dma_tx = 1;
		SET_CS();
	}
	if (DMA1->ISR & DMA_ISR_TCIF2) {
		DMA1->IFCR |= DMA_IFCR_CTCIF2;
		status_dma_tx = 1;
	}
}

void dma_tx_init(){
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	DMA1_Channel3->CMAR = (uint32_t)(&spi_buffer_tx[0]);
	DMA1_Channel3->CPAR = (uint32_t)(&(SPI1->DR));
	DMA1_Channel3->CCR |= DMA_CCR_MINC;
	DMA1_Channel3->CCR |= DMA_CCR_DIR;
	DMA1_Channel3->CCR |= DMA_CCR_TCIE;
	DMA1_Channel3->CCR |= DMA_CCR_PL_1;

	NVIC_SetPriority(DMA1_Channel2_3_IRQn, 8);
	NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
}


void spi1_master_init(uint8_t am_bits_send) {
	GPIOA_init_spi_TFT();

	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	SPI1->CR1 &= ~SPI_CR1_SPE;

	//SPI1->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_2;
	SPI1->CR1 &= ~SPI_CR1_BR;
	//SPI1->CR1 |= SPI_CR1_BR_1;

	SPI1->CR1 |= SPI_CR1_MSTR;	//master
	SPI1->CR1 |= SPI_CR1_SSI;
	SPI1->CR1 |= SPI_CR1_SSM;

	uint16_t DataSize = SPI1->CR2 & ~SPI_CR2_DS;
	DataSize |= (am_bits_send - 1) << 8;
	SPI1->CR2 = DataSize;
	SPI1->CR2 |= SPI_CR2_FRXTH;

	dma_tx_init();
	SPI1->CR2 |= SPI_CR2_TXDMAEN;

	SPI1->CR1 |= SPI_CR1_SPE;
}

void spi1_setDataSize(uint8_t am_bits_send) {
	SPI1->CR1 &= ~SPI_CR1_SPE;
	uint16_t DataSize = SPI1->CR2 & ~SPI_CR2_DS;
	DataSize |= (am_bits_send - 1) << 8;
	SPI1->CR2 = DataSize;
	SPI1->CR1 |= SPI_CR1_SPE;
}

void spi1_SendDataDMA_1Byte(uint8_t* data, uint16_t count_byte) {
	while((SPI1->SR & SPI_SR_TXE)!=SPI_SR_TXE);
	status_dma_tx = 0;
	spi1_setDataSize(8);

	for (int i = 0; i < count_byte; i++) {
		spi_buffer_tx[i] = data[i];
	}

	DMA1_Channel3->CCR &= ~DMA_CCR_EN;
 	DMA1_Channel3->CNDTR = count_byte;
	DMA1_Channel3->CCR |= DMA_CCR_MINC;	//increment memory
	DMA1_Channel3->CCR &= ~DMA_CCR_MSIZE;
	DMA1_Channel3->CCR &= ~DMA_CCR_PSIZE;	//size 8bits of data and peripheral

	RESET_CS();
	Start_DMA_Send_Data();
}

void spi1_SendDataDMA_2byte(uint16_t *data, uint16_t count_word) {
	while((SPI1->SR & SPI_SR_TXE)!=SPI_SR_TXE);
	status_dma_tx = 0;

	DMA1_Channel3->CCR &= ~DMA_CCR_EN;
	DMA1_Channel3->CCR |= DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0;//16 bits
	DMA1_Channel3->CCR |= DMA_CCR_MINC;
	DMA1_Channel3->CMAR = (uint32_t)(&spi_buffer_tx[0]);
	DMA1_Channel3->CNDTR = count_word;

	spi1_setDataSize(16);

	count_word = count_word << 1;
	for (int i = 0; i < count_word; i += 2, data++) {
		spi_buffer_tx[i] =  0x00ff & (*data >> 8);
		spi_buffer_tx[i+1] = 0x00ff & (*data);
	}

	RESET_CS();
	Start_DMA_Send_Data();
}


void spi1_SendDataDMA_2byteNTimes(uint16_t data, uint32_t count_word) {
	while((SPI1->SR & SPI_SR_TXE)!=SPI_SR_TXE);
	status_dma_tx = 0;
	count_word = count_word<<1;
	//if not mult by 2, CS will up in the middle of sending data


	DMA1_Channel3->CCR &= ~DMA_CCR_EN;
	DMA1_Channel3->CCR |= DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0;
	DMA1_Channel3->CCR &= ~DMA_CCR_MINC;
	DMA1_Channel3->CMAR = (uint32_t)(&spi_buffer_tx[0]);
	DMA1_Channel3->CNDTR = count_word;

	spi1_setDataSize(16);
	spi_buffer_tx[0] =  0x00ff & (data >> 8);
	spi_buffer_tx[1] = 0x00ff & (data);

	RESET_CS();
	DMA1_Channel3->CCR |= DMA_CCR_EN;
}

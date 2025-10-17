/**
 * @file SPI.c
 * @brief Драйвер SPI для STM32F103
 * 
 * Используется для связи с microSD-картой по протоколу SPI.
 * 
 * Пины:
 * - PA5 (SCK) — Serial Clock
 * - PA6 (MISO) — Master In, Slave Out
 * - PA7 (MOSI) — Master Out, Slave In
 * - PA4 (CS) — программный Chip Select
 */

#include "SPI.h"
#include "stm32f1xx.h"

// -----------------------------------------------------------------------------
// Глобальные переменные
// -----------------------------------------------------------------------------
spi_device_t devices[4];
spi_device_t* SPI_devices = devices;

// -----------------------------------------------------------------------------
// Внутренние функции
// -----------------------------------------------------------------------------

/**
 * @brief Инициализация SPI1 в режиме мастера
 */
static void spi1_init_master(void) {
    // Включаем тактирование
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // Сбрасываем конфигурацию
    SPI1->CR1 = 0;

    // Настройка SPI:
    // - Master mode
    // - Software slave management (SSI = 1)
    // - CPOL = 0, CPHA = 0 (Mode 0)
    // - Baud rate: PCLK2/256 (на старте)
    // - SPI Enable
    // - SPI TX DMA

    SPI1->CR1 = SPI_CR1_MSTR |
                SPI_CR1_SSM  |
                SPI_CR1_SSI  |
                SPI_BaudRatePrescaler_256 | 
                SPI_CR1_SPE;	
}

// -----------------------------------------------------------------------------
// Публичные функции
// -----------------------------------------------------------------------------

/**
 * @brief Инициализация SPI1
 * 
 * Настройка пинов:
 * - PA4: CS (Output Push-Pull, 50MHz) SD cart
 * - PC0: RST (Output Push-Pull, 50MHz) RST LCD
 * - PC1: CS  (Output Push-Pull, 50MHz) CS LCD
 * - PC2: RS  (Output Push-Pull, 50MHz) RS LCD
 * - PA5: SCK (Alternate Function Push-Pull, 50MHz)
 * - PA6: MISO (Input Floating)
 * - PA7: MOSI (Alternate Function Push-Pull, 50MHz)
 */
void spi_init(void) {
    // Тактирование порта A и альтернативных функций
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;

    // Сбрасываем настройки пинов
    GPIOA->CRL &= ~(GPIO_CRL_CNF4  | GPIO_CRL_MODE4);
    GPIOA->CRL &= ~(GPIO_CRL_CNF5  | GPIO_CRL_MODE5);
    GPIOA->CRL &= ~(GPIO_CRL_CNF6  | GPIO_CRL_MODE6);
    GPIOA->CRL &= ~(GPIO_CRL_CNF7  | GPIO_CRL_MODE7);

    GPIOC->CRL &= ~(GPIO_CRL_CNF0  | GPIO_CRL_MODE0);
    GPIOC->CRL &= ~(GPIO_CRL_CNF1  | GPIO_CRL_MODE1);
    GPIOC->CRL &= ~(GPIO_CRL_CNF2  | GPIO_CRL_MODE2);

    // PA4 - CS (Output Push-Pull, 50MHz)
    GPIOA->CRL |= GPIO_CRL_MODE4;  // SD cart
    GPIOC->CRL |= GPIO_CRL_MODE0;  // RST LCD
    GPIOC->CRL |= GPIO_CRL_MODE1;  // CS LCD
    GPIOC->CRL |= GPIO_CRL_MODE2;  // RS LCD

    // PA5 - SCK (Alternate Function Push-Pull, 50MHz)
    GPIOA->CRL |= GPIO_CRL_MODE5 | GPIO_CRL_CNF5_1;

    // PA6 - MISO (Input Floating)
    GPIOA->CRL |= GPIO_CRL_CNF6_0;


    // PA7 - MOSI (Alternate Function Push-Pull, 50MHz)
    GPIOA->CRL |= GPIO_CRL_MODE7 | GPIO_CRL_CNF7_1;

    // CS высокий (неактивен)
    GPIOA->BSRR = GPIO_BSRR_BS4;
    GPIOC->BSRR = GPIO_BSRR_BS0;
    GPIOC->BSRR = GPIO_BSRR_BS1;
    GPIOC->BSRR = GPIO_BSRR_BS2;

    // Инициализация SPI
    spi1_init_master();
    Create_SPI_devices(SPI_devices);
}

/**
 * @brief Изменение скорости SPI
 * @param prescaler Делитель частоты (например, SPI_BaudRatePrescaler_4)
 */
void SPI_SetSpeed(uint16_t prescaler) {
    // Отключаем SPI
    SPI1->CR1 &= ~SPI_CR1_SPE;
    // Сбрасываем старый делитель
    SPI1->CR1 &= ~SPI_CR1_BR;
    // Устанавливаем новый
    SPI1->CR1 |= prescaler;
    // Включаем SPI
    SPI1->CR1 |= SPI_CR1_SPE;
}

/**
 * @brief Активация CS (PA4 = 0)
 */
void CS_Activate_0(void) {
    GPIOA->BSRR = GPIO_BSRR_BR4;  // PA4 = 0
}

/**
 * @brief Деактивация CS (PA4 = 1)
 */
void CS_Deactivate_0(void) {
    GPIOA->BSRR = GPIO_BSRR_BS4;  // PA4 = 1
}

/**
 * @brief Активация RST (PC0 = 0)
 */
void CS_Activate_1(void) {
    GPIOC->BSRR = GPIO_BSRR_BR0;  // PC0 = 0
}

/**
 * @brief Деактивация RST (PC0 = 1)
 */
void CS_Deactivate_1(void) {
    GPIOC->BSRR = GPIO_BSRR_BS0;  // PC0 = 1
}

/**
 * @brief Активация CS (PC1 = 0)
 */
void CS_Activate_2(void) {
    GPIOC->BSRR = GPIO_BSRR_BR1;  // PC1 = 0
}

/**
 * @brief Деактивация CS (PC1 = 1)
 */
void CS_Deactivate_2(void) {
    GPIOC->BSRR = GPIO_BSRR_BS1;  // PC1 = 1
}

/**
 * @brief Активация RS (PC2 = 0)
 */
void CS_Activate_3(void) {
    GPIOC->BSRR = GPIO_BSRR_BR2;  // PC2 = 0
}

/**
 * @brief Деактивация RS (PC2 = 1)
 */
void CS_Deactivate_3(void) {
    GPIOC->BSRR = GPIO_BSRR_BS2;  // PC2 = 1
}


/**
 * @brief Настройка SPI-устройств
 * @param SPI_devices Указатель на массив структур устройств
 */
void Create_SPI_devices(spi_device_t* SPI_devices) {
    SPI_devices[0].activate   = CS_Activate_0;
    SPI_devices[0].deactivate = CS_Deactivate_0;
    SPI_devices[1].activate   = CS_Activate_1;
    SPI_devices[1].deactivate = CS_Deactivate_1;
    SPI_devices[2].activate   = CS_Activate_2;
    SPI_devices[2].deactivate = CS_Deactivate_2;
    SPI_devices[3].activate   = CS_Activate_3;
    SPI_devices[3].deactivate = CS_Deactivate_3;
}

/**
 * @brief Обмен данными с устройством (передача/приём)
 * @param tx_data Буфер передаваемых данных
 * @param rx_data Буфер принимаемых данных
 * @param key_number Номер устройства в массиве
 */
void SPI1_TransmitReceive(uint16_t *tx_data, uint16_t *rx_data, uint8_t key_number) {
    SPI_devices[key_number].activate();
    for (uint8_t i = 0; i < 2; i++) {
        while (!(SPI1->SR & SPI_SR_TXE));
        SPI1->DR = tx_data[i];
        while (!(SPI1->SR & SPI_SR_RXNE));
        rx_data[i] = SPI1->DR;
    }
    SPI_devices[key_number].deactivate();
}


/**
 * @brief Обмен байтами по SPI
 * @param data Байт для отправки
 * @return Принятый байт
 */
uint8_t SPI_transfer(uint8_t data) {
    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = data;
    while (!(SPI1->SR & SPI_SR_RXNE));
    return (uint8_t)SPI1->DR;
}


/**
 * @brief Обмен байтами по SPI для SD карты с управляемым CS
 * @param data Байт для отправки
 */
void SPI_send(char data) {
	while(!(SPI1->SR & SPI_SR_TXE)) {};
	SPI_devices[0].activate();
    Delay_us(2);
    SPI1->DR = data;
	while(!(SPI1->SR & SPI_SR_TXE)) {};
	while((SPI1->SR & SPI_SR_BSY)) {};
	SPI_devices[0].deactivate();
}


/**
 * @brief Обмен байтами по SPI для LCD по 16 бит с управляемым CS
 * @param data Байт для отправки
 */
void SPI_send_16bit(uint16_t data) {
	uint8_t buff = 0;
	while(!(SPI1->SR & SPI_SR_TXE)) {};
	LCD_CS.activate();
    
	buff = data >> 8;
	SPI1->DR = buff;
	while(!(SPI1->SR & SPI_SR_TXE)) {};
		

	buff = (uint8_t) (0x00FF & data);
	SPI1->DR = buff;
	while(!(SPI1->SR & SPI_SR_TXE)) {};
		
	while((SPI1->SR & SPI_SR_BSY)) {};
	LCD_CS.deactivate();
}




#ifndef SPI_H

	#define SPI_H
	
	#include "stm32f1xx.h"
	#include <string.h>
	#include "TIMER.h"

	// -----------------------------------------------------------------------------
	// Дефайны для удобной смены скорости работы SPI
	// -----------------------------------------------------------------------------
	
	#define SPI_BaudRatePrescaler_256 	( SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0 )
	#define SPI_BaudRatePrescaler_128 	( SPI_CR1_BR_2 )
	#define SPI_BaudRatePrescaler_64 	( SPI_CR1_BR_2 | SPI_CR1_BR_1 )
	#define SPI_BaudRatePrescaler_16 	( SPI_CR1_BR_1 | SPI_CR1_BR_0 )
	#define SPI_BaudRatePrescaler_4 	( SPI_CR1_BR_0 )
	#define SPI_BaudRatePrescaler_0 	( ~SPI_BaudRatePrescaler_256 )

	#define SD_cart_CS		SPI_devices[0]
	#define LCD_RST			SPI_devices[1]
	#define LCD_CS			SPI_devices[2]
	#define LCD_RS			SPI_devices[3]


	// -----------------------------------------------------------------------------
	// Типы данных
	// -----------------------------------------------------------------------------
	typedef struct {
		void (*activate)(void);
		void (*deactivate)(void);
	} spi_device_t;


	// -----------------------------------------------------------------------------
	// Глобальные переменные
	// -----------------------------------------------------------------------------
	extern spi_device_t* SPI_devices;

	// -----------------------------------------------------------------------------
	// Публичные функции
	// -----------------------------------------------------------------------------

	/**
	 * @brief Инициализация SPI1
	 */
	void spi_init(void);

	/**
	 * @brief Изменение скорости SPI
	 * @param prescaler Делитель частоты (например, SPI_BaudRatePrescaler_4)
	 */
	void SPI_SetSpeed(uint16_t prescaler);

	/**
	 * @brief Активация CS (PA4 = 0)
	 */
	void CS_Activate_0(void);

	/**
	 * @brief Деактивация CS (PA4 = 1)
	 */
	void CS_Deactivate_0(void);
	
	/**
	 * @brief Активация RST (PC0 = 0)
	 */
	void CS_Activate_1(void);

	/**
	 * @brief Деактивация RST (PC0 = 1)
	 */
	void CS_Deactivate_1(void);

	/**
	 * @brief Активация CS (PC1 = 0)
	 */
	void CS_Activate_2(void);

	/**
	 * @brief Деактивация CS (PC1 = 1)
	 */
	void CS_Deactivate_2(void);

	/**
	 * @brief Активация RS (PC2 = 0)
	 */
	void CS_Activate_3(void);

	/**
	 * @brief Деактивация RS (PC2 = 1)
	 */
	void CS_Deactivate_3(void);

	
	/**
	 * @brief Настройка SPI-устройств
	 * @param SPI_devices Указатель на массив структур устройств
	 */
	void Create_SPI_devices(spi_device_t* SPI_devices);

	/**
	 * @brief Обмен данными с устройством (передача/приём)
	 * @param tx_data Буфер передаваемых данных
	 * @param rx_data Буфер принимаемых данных
	 * @param key_number Номер устройства в массиве
	 */
	void SPI1_TransmitReceive(uint16_t *tx_data, uint16_t *rx_data, uint8_t key_number);

	/**
	 * @brief Передача одного байта и приём ответа
	 * @param data Байт для передачи
	 * @return Принятый байт
	 */
	uint8_t SPI1_write(uint8_t data);

	/**
	 * @brief Обмен байтами по SPI
	 * @param data Байт для отправки
	 * @return Принятый байт
	 */
	uint8_t SPI_transfer(uint8_t data);


	/**
	 * @brief Обмен байтами по SPI для SD карты с управляемым CS
	 * @param data Байт для отправки
	*/
	void SPI_send(char data);

	/**
	 * @brief Обмен байтами по SPI для LCD по 16 бит с управляемым CS
	 * @param data Байт для отправки
	 */
	void SPI_send_16bit(uint16_t data);


#endif /* SPI_H */



		
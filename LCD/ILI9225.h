#ifndef SRC_ILI9225_H_
	
	#define SRC_ILI9225_H_
	
	#include "stm32f1xx.h"
	#include "SPI.h"
	#include "ILI9225_registers.h"
	#include "ILI9225_colours.h"
	#include <limits.h>
	#include "TIMER.h"
	#include "fonts.h"


	// ILI9225 screen size
	#define LCD_WIDTH  				176
	#define LCD_HEIGHT 				220
	extern uint8_t  ILI9225_orientation;
	extern uint16_t  ILI9225_maxX;
	extern uint16_t  ILI9225_maxY;

	/**
	 * @brief Сброс дисплея или его активация
	 */
	void ILI9225_reset(void);

	/**
	 * @brief Отправка на дисплей команды должен быть при отпущенной ноге RS
	 * @param address регистр к которому обращаешься
	 */
	void ILI9225_writeIndex(uint16_t address);


	/**
	 * @brief Отправка на дисплей команды должен быть при отпущенной ноге RS
	 * @param address регистр к которому обращаешься
	 * @param data параметр который ты хочешь положить в указанный регистр
	 */
	void ILI9225_write(uint16_t address, uint16_t data);


	/**
	 * @brief Изменение ориентации дисплея
	 * @param orientation одно из четырех возможных положений см документацию вложенную
	 */
	void ILI9225_setOrientation(uint8_t orientation);

	/**
	 * @brief Инициализация дисплея
	 * Записью в настроечные регистры конфигурации дисплея
	 */
	void ILI9225_init(void);

	/**
	 * @brief Функция swap смены значения переменных
	 */
	void ILI9225_swap(uint16_t *a, uint16_t *b);

	/**
	 * @brief Функция по смене системы координат
	 */
	void ILI9225_orientCoordinates(uint16_t *x1, uint16_t *y1);

	/**
	 * @brief Функция выбора части дисплея для изменения
	 * @param x0 координата левого верхнего угла
	 * @param y0 координата левого верхнего угла
	 * @param x1 координата правого нижнего угла
	 * @param y1 координата правого нижнего угла
	 */
	void ILI9225_setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

	/**
	 * @brief Функция закращивания пикселя по координатам
	 * @param x1 координата x пикселя
	 * @param y1 координата y пикселя
	 * @param color цвет в который нужно закрасить пиксель
	 */
	void ILI9225_drawPixel(uint16_t x1, uint16_t y1, uint16_t color);

	/**
	 * @brief Очистка экрана путем залития его цветом COLOR_BLACK
	 */
	void ILI9225_clear(void);

	/**
	 * @brief Отправка на дисплей массива данных 
	 * @param data данные которые нужно отправить
	 * @param len_b длинна массива
	 */
	void ILI9225_Draw_File(uint8_t const *data, uint16_t *len_b);
	
#endif /* SRC_ILI9225_H_ */

#include "ILI9225.h"


uint8_t  ILI9225_orientation = 0;
uint16_t ILI9225_maxX = LCD_WIDTH;
uint16_t ILI9225_maxY = LCD_HEIGHT;

/**
 * @brief Сброс дисплея или его активация
 */
void ILI9225_reset(void) {
	LCD_RST.deactivate();
	Delay_ms(10);
	LCD_RST.activate();
	Delay_ms(50);
	LCD_RST.deactivate();
	Delay_ms(50);
}

/**
 * @brief Отправка на дисплей команды должен быть при отпущенной ноге RS
 * @param address регистр к которому обращаешься
 */
void ILI9225_writeIndex(uint16_t address) {
	LCD_CS.activate();
	LCD_RS.activate();
	SPI_send_16bit(address);
	LCD_RS.deactivate();
}

/**
 * @brief Отправка на дисплей команды должен быть при отпущенной ноге RS
 * @param address регистр к которому обращаешься
 * @param data параметр который ты хочешь положить в указанный регистр
 */
void ILI9225_write(uint16_t address, uint16_t data) {
	ILI9225_writeIndex(address);
	SPI_send_16bit(data);
	LCD_CS.deactivate();
}


/**
 * @brief Изменение ориентации дисплея
 * @param orientation одно из четырех возможных положений см документацию вложенную
 */
void ILI9225_setOrientation(uint8_t orientation) {
    ILI9225_orientation = orientation % 4;

    switch (ILI9225_orientation) {
        case 0: // Портрет, нормальный
            ILI9225_maxX = LCD_WIDTH;   // 176
            ILI9225_maxY = LCD_HEIGHT;  // 220
            ILI9225_write(ENTRY_MODE, 0x1038); // AM=0, ID1=1, ID0=1 → горизонтальный инкремент, вертикальный инкремент
            ILI9225_write(DRIVER_OUTPUT_CTRL, 0x001C); // SM=0, GS=0 → нормальное сканирование
            break;

        case 1: // Альбом, поворот на 90°
            ILI9225_maxX = LCD_HEIGHT;  // 220
            ILI9225_maxY = LCD_WIDTH;   // 176
            ILI9225_write(ENTRY_MODE, 0x1030); // AM=1, ID1=1, ID0=0 → вертикальный инкремент, горизонтальный декремент
            ILI9225_write(DRIVER_OUTPUT_CTRL, 0x001C); // SM=0, GS=0
            break;

        case 2: // Портрет, 180°
            ILI9225_maxX = LCD_WIDTH;
            ILI9225_maxY = LCD_HEIGHT;
            ILI9225_write(ENTRY_MODE, 0x1028); // AM=0, ID1=0, ID0=1 → горизонтальный декремент, вертикальный декремент
            ILI9225_write(DRIVER_OUTPUT_CTRL, 0x001C);
            break;

        case 3: // Альбом, 270°
            ILI9225_maxX = LCD_HEIGHT;
            ILI9225_maxY = LCD_WIDTH;
            ILI9225_write(ENTRY_MODE, 0x1020); // AM=1, ID1=0, ID0=0 → вертикальный декремент, горизонтальный инкремент
            ILI9225_write(DRIVER_OUTPUT_CTRL, 0x001C);
            break;
    }
}

/**
 * @brief Инициализация дисплея
 * Записью в настроечные регистры конфигурации дисплея
 */
void ILI9225_init(void) {
	ILI9225_reset();

	/* Start Initial Sequence */
	/* Set SS bit and direction output from S528 to S1 */
	ILI9225_write(POWER_CTRL1, 0x0000); // Set SAP,DSTB,STB
	ILI9225_write(POWER_CTRL2, 0x0000); // Set APON,PON,AON,VCI1EN,VC
	ILI9225_write(POWER_CTRL3, 0x0000); // Set BT,DC1,DC2,DC3
	ILI9225_write(POWER_CTRL4, 0x0000); // Set GVDD
	ILI9225_write(POWER_CTRL5, 0x0000); // Set VCOMH/VCOML voltage
	Delay_ms(40);

   // Power-on sequence
	ILI9225_write(POWER_CTRL2, 0x0018); // Set APON,PON,AON,VCI1EN,VC
	ILI9225_write(POWER_CTRL3, 0x6121); // Set BT,DC1,DC2,DC3
	ILI9225_write(POWER_CTRL4, 0x00DF); // Set GVDD   /*007F 0088 */
	ILI9225_write(POWER_CTRL5, 0x495F); // Set VCOMH/VCOML voltage
	ILI9225_write(POWER_CTRL1, 0x0F00); // Set SAP,DSTB,STB
	Delay_ms(10);
	ILI9225_write(POWER_CTRL2, 0x103B); // Set APON,PON,AON,VCI1EN,VC
	Delay_ms(50);

	ILI9225_write(DRIVER_OUTPUT_CTRL, 0x001C); // set the display line number and display direction
	ILI9225_write(LCD_AC_DRIVING_CTRL, 0x0100); // set 1 line inversion
	ILI9225_write(ENTRY_MODE, 0x1038); // set GRAM write direction and BGR=1. //0x1030
	ILI9225_write(DISP_CTRL1, 0x0000); // Display off
	ILI9225_write(BLANK_PERIOD_CTRL1, 0x0808); // set the back porch and front porch
	ILI9225_write(FRAME_CYCLE_CTRL, 0x1100); // set the clocks number per line
	ILI9225_write(INTERFACE_CTRL, 0x0001); // CPU interface
	ILI9225_write(OSC_CTRL, 0x0D01); // Set Osc  /*0e01*/
	ILI9225_write(VCI_RECYCLING, 0x0020); // Set VCI recycling
	ILI9225_write(RAM_ADDR_SET1, 0x0000); // RAM Address
	ILI9225_write(RAM_ADDR_SET2, 0x0000); // RAM Address

/* Set GRAM area */
	ILI9225_write(GATE_SCAN_CTRL, 0x0000);
	ILI9225_write(VERTICAL_SCROLL_CTRL1, 0x00DB);
	ILI9225_write(VERTICAL_SCROLL_CTRL2, 0x0000);
	ILI9225_write(VERTICAL_SCROLL_CTRL3, 0x0000);
	ILI9225_write(PARTIAL_DRIVING_POS1, 0x00DB);
	ILI9225_write(PARTIAL_DRIVING_POS2, 0x0000);
	ILI9225_write(HORIZONTAL_WINDOW_ADDR1, 0x00AF);
	ILI9225_write(HORIZONTAL_WINDOW_ADDR2, 0x0000);
	ILI9225_write(VERTICAL_WINDOW_ADDR1, 0x00DB);
	ILI9225_write(VERTICAL_WINDOW_ADDR2, 0x0000);

/* Set GAMMA curve */
	ILI9225_write(GAMMA_CTRL1, 0x0000);
	ILI9225_write(GAMMA_CTRL2, 0x0808);
	ILI9225_write(GAMMA_CTRL3, 0x080A);
	ILI9225_write(GAMMA_CTRL4, 0x000A);
	ILI9225_write(GAMMA_CTRL5, 0x0A08);
	ILI9225_write(GAMMA_CTRL6, 0x0808);
	ILI9225_write(GAMMA_CTRL7, 0x0000);
	ILI9225_write(GAMMA_CTRL8, 0x0A00);
	ILI9225_write(GAMMA_CTRL9, 0x0710);
	ILI9225_write(GAMMA_CTRL10, 0x0710);

	ILI9225_write(DISP_CTRL1, 0x0012);
	Delay_ms(50);
	ILI9225_write(DISP_CTRL1, 0x0017);


	ILI9225_setOrientation(0);

	ILI9225_clear();
	LCD_CS.deactivate();
}

/**
 * @brief Функция swap смены значения переменных
 */
void ILI9225_swap(uint16_t *a, uint16_t *b) {
    uint16_t w = *a;
    *a = *b;
    *b = w;
}

/**
 * @brief Функция по смене системы координат
 */
void ILI9225_orientCoordinates(uint16_t *x1, uint16_t *y1) {

    switch (ILI9225_orientation) {
    case 0:  // ok
        break;
    case 1: // ok
        *y1 = ILI9225_maxY - *y1 - 1;
        ILI9225_swap(x1, y1);
        break;
    case 2: // ok
        *x1 = ILI9225_maxX - *x1 - 1;
        *y1 = ILI9225_maxY - *y1 - 1;
        break;
    case 3: // ok
        *x1 = ILI9225_maxX - *x1 - 1;
        ILI9225_swap(x1, y1);
        break;
    }
}

/**
 * @brief Функция выбора части дисплея для изменения
 * @param x0 координата левого верхнего угла
 * @param y0 координата левого верхнего угла
 * @param x1 координата правого нижнего угла
 * @param y1 координата правого нижнего угла
 */
void ILI9225_setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	uint16_t xb = x0;
	uint16_t xe = x1;
	uint16_t yb = y0;
	uint16_t ye = y1;

	ILI9225_orientCoordinates(&xb, &yb);
	ILI9225_orientCoordinates(&xe, &ye);

    if (x1 < x0) ILI9225_swap(&xb, &xe);
    if (y1 < y0) ILI9225_swap(&yb, &ye);

    ILI9225_write(HORIZONTAL_WINDOW_ADDR1, xe);
    ILI9225_write(HORIZONTAL_WINDOW_ADDR2, xb);

    ILI9225_write(VERTICAL_WINDOW_ADDR1, ye);
    ILI9225_write(VERTICAL_WINDOW_ADDR2, yb);

    ILI9225_write(RAM_ADDR_SET1, xb);
    ILI9225_write(RAM_ADDR_SET2, yb);
}

/**
 * @brief Функция закращивания пикселя по координатам
 * @param x1 координата x пикселя
 * @param y1 координата y пикселя
 * @param color цвет в который нужно закрасить пиксель
 */
void ILI9225_drawPixel(uint16_t x1, uint16_t y1, uint16_t color) {
    if ((x1 >= ILI9225_maxX) || (y1 >= ILI9225_maxY)) return;

	uint16_t x = x1;
	uint16_t y = y1;

    ILI9225_orientCoordinates(&x, &y);
    ILI9225_write(RAM_ADDR_SET1,x);
    ILI9225_write(RAM_ADDR_SET2,y);
    ILI9225_write(GRAM_DATA_REG,color);
}

/**
 * @brief Очистка экрана путем залития его цветом COLOR_BLACK
 */
void ILI9225_clear(void) {
	uint16_t size = LCD_WIDTH*LCD_HEIGHT;
	int i = 0;
		
    ILI9225_setWindow(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);

    ILI9225_writeIndex(GRAM_DATA_REG);

    for(i = 0; i< size; i++)
    {
    	SPI_send_16bit(COLOR_BLACK);
    }
}

/**
 * @brief Отправка на дисплей массива данных 
 * @param data данные которые нужно отправить
 * @param len_b длинна массива
 */
void ILI9225_Draw_File(uint8_t const *data, uint16_t *len_b) {
    for(int i = 0; i < *len_b; i+=3) {
		SPI_send_16bit(RGB888_RGB565(data[i+2]<<16 | data[i + 1] << 8 | data[i]));
    }
}	
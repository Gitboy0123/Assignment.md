// st7735.c - Minimal single file ST7735 driver for STM32 SPI

#include "main.h"           // For GPIO pins and hspi1
#include "stm32l4xx_hal.h"  // HAL header

// Define your LCD resolution here:
#define ST7735_WIDTH  128
#define ST7735_HEIGHT 160

// LCD commands
#define ST7735_SWRESET  0x01
#define ST7735_SLPOUT   0x11
#define ST7735_DISPON   0x29
#define ST7735_CASET    0x2A
#define ST7735_RASET    0x2B
#define ST7735_RAMWR    0x2C
#define ST7735_COLMOD   0x3A
#define ST7735_MADCTL   0x36

extern SPI_HandleTypeDef hspi1;

// Control pins (from your main.h)
#define CS_GPIO_Port   GPIOB
#define CS_Pin         GPIO_PIN_6
#define DC_GPIO_Port   GPIOB
#define DC_Pin         GPIO_PIN_2
#define RST_GPIO_Port  GPIOB
#define RST_Pin        GPIO_PIN_1

static void ST7735_Select(void) {
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
}

static void ST7735_Unselect(void) {
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}

static void ST7735_WriteCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET); // Command mode
    ST7735_Select();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    ST7735_Unselect();
}

static void ST7735_WriteData(uint8_t *buff, size_t buff_size) {
    HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET); // Data mode
    ST7735_Select();
    HAL_SPI_Transmit(&hspi1, buff, buff_size, HAL_MAX_DELAY);
    ST7735_Unselect();
}

void ST7735_Reset(void) {
    HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(20);
    HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
    HAL_Delay(150);
}

void ST7735_Init(void) {
    ST7735_Reset();

    ST7735_WriteCommand(ST7735_SWRESET); // Software reset
    HAL_Delay(150);

    ST7735_WriteCommand(ST7735_SLPOUT);  // Sleep out
    HAL_Delay(500);

    uint8_t color_mode = 0x05; // 16-bit color mode (65k colors)
    ST7735_WriteCommand(ST7735_COLMOD);
    ST7735_WriteData(&color_mode, 1);
    HAL_Delay(10);

    ST7735_WriteCommand(ST7735_MADCTL);
    uint8_t madctl = 0x00; // Default orientation, RGB order - adjust if needed
    ST7735_WriteData(&madctl, 1);

    ST7735_WriteCommand(ST7735_DISPON); // Display on
    HAL_Delay(100);

    // Clear screen to black after init
    ST7735_FillScreen(0x0000);
}

void ST7735_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    uint8_t data[4];

    ST7735_WriteCommand(ST7735_CASET); // Column address set
    data[0] = (x0 >> 8) & 0xFF;
    data[1] = x0 & 0xFF;
    data[2] = (x1 >> 8) & 0xFF;
    data[3] = x1 & 0xFF;
    ST7735_WriteData(data, 4);

    ST7735_WriteCommand(ST7735_RASET); // Row address set
    data[0] = (y0 >> 8) & 0xFF;
    data[1] = y0 & 0xFF;
    data[2] = (y1 >> 8) & 0xFF;
    data[3] = y1 & 0xFF;
    ST7735_WriteData(data, 4);

    ST7735_WriteCommand(ST7735_RAMWR); // Write to RAM
}

void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT) return;

    ST7735_SetAddressWindow(x, y, x, y);

    uint8_t data[2] = { color >> 8, color & 0xFF };
    ST7735_WriteData(data, 2);
}

void ST7735_FillScreen(uint16_t color) {
    ST7735_SetAddressWindow(0, 0, ST7735_WIDTH - 1, ST7735_HEIGHT - 1);
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;

    // Buffer to send multiple pixels at once (64 pixels)
    uint8_t buffer[128];
    for (int i = 0; i < sizeof(buffer); i += 2) {
        buffer[i] = hi;
        buffer[i + 1] = lo;
    }

    for (int i = 0; i < (ST7735_WIDTH * ST7735_HEIGHT) / 64; i++) {
        ST7735_WriteData(buffer, sizeof(buffer));
    }
}

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body for Calculator
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include "font.h"
#include "st7735.h"   // ST7735 LCD driver
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

/* Function prototypes -------------------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
void draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void draw_char(uint8_t x, uint8_t y, char c);
void draw_string(uint8_t x, uint8_t y, const char *str);

/* USER CODE BEGIN 0 */
volatile uint8_t keypad_value = 0xFF;

#define MAX_EXPR_LEN 32
char expression[MAX_EXPR_LEN + 1] = {0};
uint8_t expr_len = 0;

char keypad_decode(uint8_t val) {
    const char keymap[16] = {
        '1', '2', '3', 'A',
        '4', '5', '6', 'B',
        '7', '8', '9', 'C',
        '*', '0', '#', 'D'
    };
    return (val < 16) ? keymap[val] : '?';
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_13) { // DATA AVAILABLE (from PC13)
        uint8_t val = 0;
        val |= HAL_GPIO_ReadPin(DATA0_GPIO_Port, DATA0_Pin) << 0; // DATA0
        val |= HAL_GPIO_ReadPin(DATA1_GPIO_Port, DATA1_Pin) << 1; // DATA1
        val |= HAL_GPIO_ReadPin(DATA2_GPIO_Port, DATA2_Pin) << 2; // DATA2
        val |= HAL_GPIO_ReadPin(DATA3_GPIO_Port, DATA3_Pin) << 3; // DATA3
        keypad_value = val;
    }
}

char get_keypad_key(void) {
    if (keypad_value != 0xFF) {
        char key = keypad_decode(keypad_value);
        keypad_value = 0xFF;
        return key;
    }
    return 0;
}

void clear_expression(void) {
    memset(expression, 0, sizeof(expression));
    expr_len = 0;
    draw_string(10, 30, "                                ");
}

void display_expression(void) {
    draw_string(10, 30, "                                ");
    draw_string(10, 30, expression);
}

double evaluate_expression(const char *expr, int *error) {
    *error = 0;
    double operand1 = 0, operand2 = 0;
    char op = 0;
    int i = 0;

    while (expr[i] && ((expr[i] >= '0' && expr[i] <= '9') || expr[i] == '.')) i++;
    if (i == 0) {
        *error = 1;
        return 0;
    }

    char temp1[16] = {0};
    strncpy(temp1, expr, i);
    operand1 = atof(temp1);

    if (expr[i] == '\0') return operand1;

    op = expr[i++];
    int start = i;
    while (expr[i] && ((expr[i] >= '0' && expr[i] <= '9') || expr[i] == '.')) i++;
    if (i == start) {
        *error = 1;
        return 0;
    }

    char temp2[16] = {0};
    strncpy(temp2, &expr[start], i - start);
    operand2 = atof(temp2);

    switch (op) {
        case '+': return operand1 + operand2;
        case '-': return operand1 - operand2;
        case '*': return operand1 * operand2;
        case '/':
            if (operand2 == 0) {
                *error = 1;
                return 0;
            }
            return operand1 / operand2;
        default:
            *error = 1;
            return 0;
    }
}

void draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
    ST7735_DrawPixel(x, y, color);
}

void draw_char(uint8_t x, uint8_t y, char c) {
    const uint8_t *bitmap = NULL;
    if (c >= '0' && c <= '9') bitmap = font8x8_calc[c - '0'];
    else if (c == '+') bitmap = font8x8_calc[10];
    else if (c == '-') bitmap = font8x8_calc[11];
    else if (c == '*') bitmap = font8x8_calc[12];
    else if (c == '/') bitmap = font8x8_calc[13];
    else if (c == '=') bitmap = font8x8_calc[14];
    else if (c == 'C') bitmap = font8x8_calc[15];
    else return;

    for (uint8_t row = 0; row < 8; row++) {
        uint8_t bits = bitmap[row];
        for (uint8_t col = 0; col < 8; col++) {
            uint16_t color = (bits & (1 << (7 - col))) ? 0xFFFF : 0x0000;
            draw_pixel(x + col, y + row, color);
        }
    }
}

void draw_string(uint8_t x, uint8_t y, const char *str) {
    while (*str) {
        draw_char(x, y, *str++);
        x += 8;
    }
}

/* USER CODE END 0 */

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_SPI1_Init();
    ST7735_Init();

    draw_string(10, 10, "READY");
    clear_expression();

    HAL_GPIO_WritePin(R_LED_GPIO_Port, R_LED_Pin, GPIO_PIN_SET);   // Red ON (idle)
    HAL_GPIO_WritePin(G_LED_GPIO_Port, G_LED_Pin, GPIO_PIN_RESET); // Green OFF

    while (1) {
        char key = get_keypad_key();
        if (key) {
            if (key == 'D') { // Clear expression
                clear_expression();
                draw_string(10, 10, "CLEARED  ");
                HAL_GPIO_WritePin(R_LED_GPIO_Port, R_LED_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(G_LED_GPIO_Port, G_LED_Pin, GPIO_PIN_RESET);
            }
            else if (key == 'A') { // Evaluate expression
                int err;
                double result = evaluate_expression(expression, &err);
                if (!err) {
                    int int_part = (int)result;
                    int frac_part = (int)((result - int_part) * 100 + 0.5);
                    if (frac_part < 0) frac_part = -frac_part;

                    char result_str[64];
                    snprintf(result_str, sizeof(result_str), "%s=%d.%02d", expression, int_part, frac_part);
                    draw_string(10, 30, "                    ");
                    draw_string(10, 30, result_str);

                    HAL_GPIO_WritePin(G_LED_GPIO_Port, G_LED_Pin, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(R_LED_GPIO_Port, R_LED_Pin, GPIO_PIN_RESET);
                } else {
                    draw_string(10, 30, " ERROR   ");
                    HAL_GPIO_WritePin(R_LED_GPIO_Port, R_LED_Pin, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(G_LED_GPIO_Port, G_LED_Pin, GPIO_PIN_RESET);
                }
                clear_expression();
            }
            else {
                char actual = 0;
                switch (key) {
                    case 'B': actual = '+'; break;
                    case 'C': actual = '-'; break;
                    case '*': actual = '*'; break;
                    case '#': actual = '/'; break;
                    default: actual = key; break;
                }

                if ((actual >= '0' && actual <= '9') || actual == '+' || actual == '-' || actual == '*' || actual == '/') {
                    if (expr_len < MAX_EXPR_LEN) {
                        expression[expr_len++] = actual;
                        expression[expr_len] = '\0';
                    }
                    display_expression();

                    // Blink red LED once to show feedback
                    HAL_GPIO_TogglePin(R_LED_GPIO_Port, R_LED_Pin);
                    HAL_Delay(100);
                    HAL_GPIO_TogglePin(R_LED_GPIO_Port, R_LED_Pin);
                }
            }
            HAL_Delay(300); // Debounce delay
        }
    }
}

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 10;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;

    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
}

static void MX_SPI1_Init(void) {
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 7;
    hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
    HAL_SPI_Init(&hspi1);
}

static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Initialize LED pins low
    HAL_GPIO_WritePin(R_LED_GPIO_Port, R_LED_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(G_LED_GPIO_Port, G_LED_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);

    // Keypad data pins input
    GPIO_InitStruct.Pin = DATA0_Pin | DATA1_Pin | DATA2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DATA0_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = DATA3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DATA3_GPIO_Port, &GPIO_InitStruct);

    // LEDs output pins
    GPIO_InitStruct.Pin = R_LED_Pin | G_LED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(R_LED_GPIO_Port, &GPIO_InitStruct);

    // LCD control pins output
    GPIO_InitStruct.Pin = RST_Pin | DC_Pin | CS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(RST_GPIO_Port, &GPIO_InitStruct);

    // EXTI line for keypad DATA_AVAILABLE (PC13)
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Enable and set EXTI interrupt priority
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void Error_Handler(void) {
    __disable_irq();
    while (1) {}
}

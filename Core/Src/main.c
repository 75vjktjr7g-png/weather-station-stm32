/* USER CODE BEGIN Header */
/**
  ****************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ****************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include "gpio.h"
#include "usart.h"
#include "i2c.h"
#include <stdio.h>
#include "bme680_port.h"

/* USER CODE BEGIN PV */
extern I2C_HandleTypeDef hi2c1;
/* USER CODE END PV */

/* retarget printf */
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

void SystemClock_Config(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_I2C1_Init();
    struct bme68x_dev bme;
    struct bme68x_conf conf;
    struct bme68x_data data;
    uint8_t n_fields;

    char txBuffer[100];

    if (bme680_init_sensor(&bme) != BME68X_OK)
    {
        printf("BME680 init FAILED\r\n");
        while (1);
    }

    printf("BME680 init OK\r\n");

    conf.os_hum = BME68X_OS_2X;
    conf.os_temp = BME68X_OS_4X;
    conf.os_pres = BME68X_OS_4X;
    conf.filter = BME68X_FILTER_OFF;

    bme68x_set_conf(&conf, &bme);

    printf("\r\n=== UART OK ===\r\n");

    /* -------- I2C TEST -------- */
    printf("Scanning I2C...\r\n");

    for (uint8_t addr = 1; addr < 127; addr++)
    {
        if (HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 2, 10) == HAL_OK)
        {
            printf("Found device at 0x%02X\r\n", addr);
        }
    }

    printf("Scan done\r\n");
    while (1)
    {
        /* Set sensor to forced mode */
        bme68x_set_op_mode(BME68X_FORCED_MODE, &bme);

        /* Wait for measurement to complete */
        HAL_Delay(150);

        if (bme68x_get_data(BME68X_FORCED_MODE, &data, &n_fields, &bme) == BME68X_OK)
        {
            if (n_fields > 0)
            {
                int temp = (int)(data.temperature);
                int hum  = (int)(data.humidity);
                int pres = (int)(data.pressure / 100);

                snprintf(txBuffer, sizeof(txBuffer),
                         "T=%d C  H=%d %%  P=%d hPa\r\n",
                         temp, hum, pres);

                HAL_UART_Transmit(&huart2,
                                  (uint8_t*)txBuffer,
                                  strlen(txBuffer),
                                  HAL_MAX_DELAY);
            }
            else
            {
                char msg[] = "No data\r\n";
                HAL_UART_Transmit(&huart2,
                                  (uint8_t*)msg,
                                  strlen(msg),
                                  HAL_MAX_DELAY);
            }
        }

        HAL_Delay(2000);
    }

}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = 0;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK |
                                  RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 |
                                  RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2 | RCC_PERIPHCLK_I2C1;
    PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
    PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;

    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
}

void Error_Handler(void)
{
    __disable_irq();
    while (1) {}
}

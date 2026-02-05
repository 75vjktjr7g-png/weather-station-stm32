#include "bme680_port.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c1;

/* I2C read */
int8_t bme680_i2c_read(uint8_t reg, uint8_t *data, uint32_t len, void *intf_ptr)
{
    return (HAL_I2C_Mem_Read(&hi2c1,
                             BME68X_I2C_ADDR_HIGH << 1,
                             reg,
                             I2C_MEMADD_SIZE_8BIT,
                             data,
                             len,
                             HAL_MAX_DELAY) == HAL_OK) ? 0 : -1;
}

/* I2C write */
int8_t bme680_i2c_write(uint8_t reg, const uint8_t *data, uint32_t len, void *intf_ptr)
{
    return (HAL_I2C_Mem_Write(&hi2c1,
                              BME68X_I2C_ADDR_HIGH << 1,
                              reg,
                              I2C_MEMADD_SIZE_8BIT,
                              (uint8_t *)data,
                              len,
                              HAL_MAX_DELAY) == HAL_OK) ? 0 : -1;
}

/* Delay */
void bme680_delay_us(uint32_t period, void *intf_ptr)
{
    HAL_Delay(period / 1000);
}

/* Init */
int8_t bme680_init_sensor(struct bme68x_dev *dev)
{
    dev->intf = BME68X_I2C_INTF;
    dev->read = bme680_i2c_read;
    dev->write = bme680_i2c_write;
    dev->delay_us = bme680_delay_us;
    dev->amb_temp = 25;
    dev->intf_ptr = NULL;

    return bme68x_init(dev);
}

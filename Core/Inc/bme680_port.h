#ifndef BME680_PORT_H
#define BME680_PORT_H

#include "bme68x.h"
#include "i2c.h"

int8_t bme680_init_sensor(struct bme68x_dev *dev);

#endif

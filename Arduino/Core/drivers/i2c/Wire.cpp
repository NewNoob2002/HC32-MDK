#include <Wire.h>

uint8_t rxBuffer[WIRE_BUFF_SIZE];
uint8_t txBuffer[WIRE_BUFF_SIZE];

i2c_peripheral_config_t I2C1_config = {
    .register_base = CM_I2C1,
    .clock_id = PWC_FCG1_I2C1,
    .scl_pin_function = Func_I2c1_Scl,
    .sda_pin_function = Func_I2c1_Sda,
    .clock_frequency = 100000,
};

i2c_peripheral_config_t *I2Cx[1] = {
    &I2C1_config,
};

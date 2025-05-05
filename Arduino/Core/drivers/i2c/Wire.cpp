#include "delay.h"
#include "variant.h"
#include <Wire.h>
#include <debug.h>
#include <hc32_ll.h>

#define WireDEBUG
#ifdef WireDEBUG
#define Wire_DEBUG(fmt, args...) LOG_DEBUG(fmt, ##args)
#else
#define Wire_DEBUG(fmt, args...)
#endif
uint8_t rxBuffer[WIRE_BUFF_SIZE];
uint8_t txBuffer[WIRE_BUFF_SIZE];

i2c_peripheral_config_t I2C1_config = {
    .register_base = CM_I2C1,
    .clock_id = FCG1_PERIPH_I2C1,
    .scl_pin_function = Func_I2c1_Scl,
    .sda_pin_function = Func_I2c1_Sda,
};

i2c_peripheral_config_t *I2Cx[1] = {
    &I2C1_config,
};

TwoWire Wire(&I2C1_config, PA9, PA8, 100000);

TwoWire::TwoWire(struct i2c_peripheral_config_t *config, gpio_pin_t scl_pin,
                 gpio_pin_t sda_pin, uint32_t clockFre) {
  this->_config = config;
  this->_scl_pin = scl_pin;
  this->_sda_pin = sda_pin;

  this->_clock_frequency = clockFre;
}

int32_t TwoWire::begin() {
  // this->_rxBuffer = new RingBuffer<uint8_t>(rxBuffer, WIRE_BUFF_SIZE);
  // this->_txBuffer = new RingBuffer<uint8_t>(txBuffer, WIRE_BUFF_SIZE);

  // if (this->_rxBuffer == nullptr || this->_txBuffer == nullptr) {
  //     LOG_ERROR("Failed to allocate memory for rxBuffer or txBuffer");
  // }
  Wire_DEBUG("set function");
  GPIO_SetFunction(this->_scl_pin, this->_config->scl_pin_function);
  GPIO_SetFunction(this->_sda_pin, this->_config->sda_pin_function);

  Wire_DEBUG("enable clock");
  FCG_Fcg1PeriphClockCmd(this->_config->clock_id, ENABLE);

  int32_t i32Ret;
  stc_i2c_init_t stcI2cInit;
  float32_t fErr;

  (void)I2C_DeInit(this->_config->register_base);
  Wire_DEBUG("enter init");
  (void)I2C_StructInit(&stcI2cInit);
  stcI2cInit.u32ClockDiv = I2C_CLK_DIV8;
  stcI2cInit.u32Baudrate = this->_clock_frequency;
  stcI2cInit.u32SclTime = 3UL;
  i32Ret = I2C_Init(this->_config->register_base, &stcI2cInit, &fErr);
  Wire_DEBUG("exit init");
  if (i32Ret != LL_OK) {
    LOG_ERROR("Failed to initialize I2C, error: %d, can't init in Clock Div: "
              "%d and %dKhz Baudrate",
              i32Ret, 1 << stcI2cInit.u32ClockDiv,
              stcI2cInit.u32Baudrate / 1000);
    return i32Ret;
  }
  Wire_DEBUG("I2C initialized in %dKhz success", stcI2cInit.u32Baudrate / 1000);
  I2C_BusWaitCmd(this->_config->register_base, ENABLE);
	return i32Ret;
}

void TwoWire::end() { I2C_DeInit(this->_config->register_base); }

void TwoWire::setClock(uint32_t clockFreq) {
  this->_clock_frequency = clockFreq;

  int32_t i32Ret;
  stc_i2c_init_t stcI2cInit;
  float32_t fErr;

  (void)I2C_DeInit(this->_config->register_base);
}

bool TwoWire::beginTransmission(uint8_t address) {
  uint32_t i32Ret = LL_ERR;
  I2C_Cmd(this->_config->register_base, ENABLE);

  I2C_SWResetCmd(this->_config->register_base, ENABLE);
  I2C_SWResetCmd(this->_config->register_base, DISABLE);

  if (I2C_Start(this->_config->register_base, 1000) == LL_OK) {
#if (I2C_ADDR_MD == I2C_ADDR_MD_10BIT)
    i32Ret = I2C_Trans10BitAddr(this->_config->register_base, address,
                                I2C_DIR_TX, 1000);
#else
    i32Ret =
        I2C_TransAddr(this->_config->register_base, address, I2C_DIR_TX, 1000);
#endif
		Wire_DEBUG("generate satrt");
  }
  return i32Ret == LL_OK;
}

int8_t TwoWire::endTransmission(bool stopBit) {
  int8_t ret = 1;
  if (stopBit) {
    // Stop by software
    ret = I2C_Stop(this->_config->register_base, 1000);
    // Disable I2C
    I2C_Cmd(this->_config->register_base, DISABLE);
		Wire_DEBUG("Stop by software, ret = %d", ret);
  } else {
    //		I2C_Cmd(this->_config->register_base, ENABLE);
    ret = I2C_Restart(this->_config->register_base, 1000);
		Wire_DEBUG("generate restart, ret = %d", ret);
  }

  return ret;
}

size_t TwoWire::write(uint8_t data) {
  if (I2C_TransData(this->_config->register_base, &data, 1, 1000) == LL_OK) {
    return 1;
  }
  return 0;
}

size_t TwoWire::write(const uint8_t *data, size_t quantity) {
  if (I2C_TransData(this->_config->register_base, data, quantity, 1000) ==
      LL_OK) {
    return quantity;
  }
  return 0;
}

size_t TwoWire::requestFrom(uint8_t device_addr, uint8_t *buffer,
                            size_t quantity, bool stopBit) {
	uint8_t length = quantity;
	int32_t ret;
	if (1UL == quantity) {
      I2C_AckConfig(this->_config->register_base, I2C_NACK);
  }
  if (I2C_TransAddr(this->_config->register_base, device_addr, I2C_DIR_RX,
                    1000) == LL_OK) {
      ret = I2C_MasterReceiveDataAndStop(this->_config->register_base, buffer, quantity, 1000);
  }
	I2C_AckConfig(this->_config->register_base, I2C_ACK);
	
	if(ret != LL_OK)
  {
    (void)I2C_Stop(this->_config->register_base, 1000);
		length = 0;
  }
	I2C_Cmd(this->_config->register_base, DISABLE);
  return length;
}

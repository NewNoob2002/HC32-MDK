#pragma once

#include <hc32_ll.h>
#include <Print.h>
#include <Stream.h>
#include <gpio.h>
#include <RingBuffer.h>

#ifndef WIRE_BUFF_SIZE
#define WIRE_BUFF_SIZE 128
#endif

/**
 * @brief USART peripheral configuration
 */
struct i2c_peripheral_config_t
{
    /**
     * @brief USART peripheral register base address
     */
    CM_I2C_TypeDef *register_base;

    /**
     * @brief USART peripheral clock id
     * @note in FCG1
     */
    uint32_t clock_id;

    /**
     * @brief pin function for usart tx pin
     */
    uint16_t scl_pin_function;

    /**
     * @brief pin function for usart rx pin
     */
    uint16_t sda_pin_function;

    /**
     * @brief clock frequency
     */
    uint32_t clock_frequency;
};

class TwoWire: public Stream {
public:
	TwoWire(struct i2c_peripheral_config_t *config, gpio_pin_t scl_pin, gpio_pin_t sda_pin);

	void begin();
	void begin(uint8_t address);

	void end();
	void setClock(uint32_t clockFreq);

	void beginTransmission(uint8_t address);
	uint8_t endTransmission(bool stopBit = true);

	uint8_t requestFrom(uint8_t address, size_t quantity, bool stopBit = true);

	size_t write(uint8_t data);
	size_t write(const uint8_t * data, size_t quantity);

	virtual int available(void);
	virtual int read(void);
	virtual int peek(void);
	virtual void flush(void);

	using Print::write;

private:
    RingBuffer<uint8_t> *_rxBuffer;
    RingBuffer<uint8_t> *_txBuffer;
};

extern TwoWire Wire;
extern i2c_peripheral_config_t I2C1_config;
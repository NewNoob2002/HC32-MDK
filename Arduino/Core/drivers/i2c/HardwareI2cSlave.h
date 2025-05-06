#ifndef _HardWAREI2C_H
#define _HardWAREI2C_H
#include <hc32_ll.h>
#include <RingBuffer.h>
#include <Stream.h>

/* Define I2C unit used for the example */
#define I2C_UNIT    (CM_I2C2)
#define I2C_FCG_USE (FCG1_PERIPH_I2C2)

/* Define port and pin for SDA and SCL */
#if 1
#define I2C_SCL_PORT      (GPIO_PORT_A)
#define I2C_SCL_PIN       (GPIO_PIN_03)
#define I2C_SDA_PORT      (GPIO_PORT_A)
#define I2C_SDA_PIN       (GPIO_PIN_02)
#else
#define I2C_SCL_PORT      (GPIO_PORT_A)
#define I2C_SCL_PIN       (GPIO_PIN_09)
#define I2C_SDA_PORT      (GPIO_PORT_A)
#define I2C_SDA_PIN       (GPIO_PIN_08)
#endif
#define I2C_GPIO_SCL_FUNC (GPIO_FUNC_51)
#define I2C_GPIO_SDA_FUNC (GPIO_FUNC_50)

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
/**
 * @brief I2c communication mode enum
 */
typedef enum {
    MD_TX = 0U,
    MD_RX = 1U,
} stc_i2c_com_mode_t;

/**
 * @brief I2c communication status enum
 */
typedef enum {
    I2C_COM_BUSY = 0U,
    I2C_COM_IDLE = 1U,
} stc_i2c_com_status_t;

/**
 * @brief I2c communication structure
 */
typedef struct {
    stc_i2c_com_mode_t enMode;             /*!< I2C communication mode*/
    __IO stc_i2c_com_status_t enComStatus; /*!< I2C communication status*/
} stc_i2c_communication_t;

static stc_i2c_communication_t stcI2cCom;
int32_t Slave_Initialize(void);

extern RingBuffer<uint8_t> *SlaveRxBuffer;
extern RingBuffer<uint8_t> *SlaveTxBuffer;

uint8_t Slave_Read(void);

class HardwareI2cSlave : public Stream
{
public:
    HardwareI2cSlave();
    ~HardwareI2cSlave();
    bool begin(void);
    void updateBuffers(RingBuffer<uint8_t> *rxBuffer, RingBuffer<uint8_t> *txBuffer);
    virtual int available(void);
    int availableForWrite(void);
    virtual int peek(void);
    virtual int read(void);
    virtual void flush(void);

    virtual size_t write(uint8_t n);
    inline size_t write(unsigned long n)
    {
        return write((uint8_t)n);
    }
    inline size_t write(long n)
    {
        return write((uint8_t)n);
    }
    inline size_t write(unsigned int n)
    {
        return write((uint8_t)n);
    }
    inline size_t write(int n)
    {
        return write((uint8_t)n);
    }
    using Print::write; // pull in write(str) and write(buf, size) from Print

    bool isTransmitting(void);
private:
    RingBuffer<uint8_t> *__SlaveRxBuffer;
    RingBuffer<uint8_t> *__SlaveTxBuffer;

    bool initialized;
};

extern HardwareI2cSlave I2C_Slave;

// 声明更新I2C_Slave对象内部指针的函数
void UpdateI2CSlave();

#endif

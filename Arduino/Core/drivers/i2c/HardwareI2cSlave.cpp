#include <irqn.h>
#include <debug.h>
#include "delay.h"
#include "HardwareI2cSlave.h"

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/* unlock/lock peripheral */
#define EXAMPLE_PERIPH_WE (LL_PERIPH_GPIO | LL_PERIPH_EFM | LL_PERIPH_FCG | \
                           LL_PERIPH_PWC_CLK_RMU | LL_PERIPH_SRAM)
#define EXAMPLE_PERIPH_WP (LL_PERIPH_EFM | LL_PERIPH_FCG | LL_PERIPH_SRAM)

/* Define slave device address for example */
#define DEVICE_ADDR (0x11U)
/* I2C address mode */
#define I2C_ADDR_MD_7BIT  (0U)
#define I2C_ADDR_MD_10BIT (1U)
/* Config I2C address mode: I2C_ADDR_MD_7BIT or I2C_ADDR_MD_10BIT */
#define I2C_ADDR_MD (I2C_ADDR_MD_7BIT)

/* Note: The polarity of EEI interrupt should be higher than other I2C interrupt */
#define I2C_EEI_IRQN_DEF (INT001_IRQn)
#define I2C_RXI_IRQN_DEF (INT002_IRQn)
#define I2C_TXI_IRQN_DEF (INT003_IRQn)
#define I2C_TEI_IRQN_DEF (INT004_IRQn)

#define I2C_INT_EEI_DEF  (INT_SRC_I2C2_EEI)
#define I2C_INT_RXI_DEF  (INT_SRC_I2C2_RXI)
#define I2C_INT_TXI_DEF  (INT_SRC_I2C2_TXI)
#define I2C_INT_TEI_DEF  (INT_SRC_I2C2_TEI)

#define TIMEOUT          (0x40000UL)

/* Define Write and read data length for the example */
#define TEST_DATA_LEN (512U)
/* Define i2c baudrate */
#define I2C_BAUDRATE (400000UL)

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
RingBuffer<uint8_t> *SlaveRxBuffer = nullptr;
RingBuffer<uint8_t> *SlaveTxBuffer = nullptr;

static uint8_t u8TxBuf[TEST_DATA_LEN];
static uint8_t u8RxBuf[TEST_DATA_LEN];

/**
 * @brief   I2C EEI(communication error or event) interrupt callback function
 * @param   None
 * @retval  None
 */
static void I2C_EEI_Callback(void)
{
    /* If address interrupt occurred */
    if (SET == I2C_GetStatus(I2C_UNIT, I2C_FLAG_MATCH_ADDR0)) {
        I2C_ClearStatus(I2C_UNIT, I2C_CLR_SLADDR0FCLR | I2C_CLR_NACKFCLR | I2C_CLR_STOPFCLR);

        if ((SET == I2C_GetStatus(I2C_UNIT, I2C_FLAG_TRA))) {
            /* Enable tx end interrupt function*/
            I2C_IntCmd(I2C_UNIT, I2C_INT_TX_CPLT, ENABLE);
            /* Write the first data to DTR immediately */
            uint8_t data;
            if (SlaveTxBuffer->pop(data)) {
                I2C_WriteData(I2C_UNIT, data);
            }
            else {
                LOG_ERROR("SlaveTxBuffer is empty");
            }

            /* Enable stop and NACK interrupt */
            I2C_IntCmd(I2C_UNIT, I2C_INT_STOP | I2C_INT_NACK, ENABLE);
        } else if ((RESET == I2C_GetStatus(I2C_UNIT, I2C_FLAG_TRA))) {
            I2C_IntCmd(I2C_UNIT, I2C_INT_RX_FULL, ENABLE);
            // stcI2cCom.enMode = MD_RX;
            /* Enable stop and NACK interrupt */
            I2C_IntCmd(I2C_UNIT, I2C_INT_STOP | I2C_INT_NACK, ENABLE);
        } else {
        }
    } else if (SET == I2C_GetStatus(I2C_UNIT, I2C_FLAG_NACKF)) {
        /* If NACK interrupt occurred */
        /* clear NACK flag*/
        I2C_ClearStatus(I2C_UNIT, I2C_CLR_NACKFCLR);
        /* Stop tx or rx process*/
        if (SET == I2C_GetStatus(I2C_UNIT, I2C_FLAG_TRA)) {
            /* Config tx end interrupt function disable*/
            I2C_IntCmd(I2C_UNIT, I2C_INT_TX_CPLT, DISABLE);
            I2C_ClearStatus(I2C_UNIT, I2C_CLR_TENDFCLR);

            /* Read DRR register to release */
            (void)I2C_ReadData(I2C_UNIT);
        } else {
            /* Config rx buffer full interrupt function disable */
            I2C_IntCmd(I2C_UNIT, I2C_INT_RX_FULL, DISABLE);
        }
    } else if (SET == I2C_GetStatus(I2C_UNIT, I2C_FLAG_STOP)) {
        /* If stop interrupt occurred */
        /* Disable all interrupt enable flag except SLADDR0IE*/
        I2C_IntCmd(I2C_UNIT, I2C_INT_TX_CPLT | I2C_INT_RX_FULL | I2C_INT_STOP | I2C_INT_NACK, DISABLE);
        /* Clear STOPF flag */
        I2C_ClearStatus(I2C_UNIT, I2C_CLR_STOPFCLR);
        I2C_Cmd(I2C_UNIT, DISABLE);
        /* Communication finished */
        I2C_Cmd(I2C_UNIT, ENABLE);
        /* Config slave address match and receive full interrupt function*/
        I2C_IntCmd(I2C_UNIT, I2C_INT_MATCH_ADDR0, ENABLE);
    } else {
    }
}

/**
 * @brief   I2C TEI(transfer end) interrupt callback function
 * @param   None
 * @retval  None
 */
static void I2C_TEI_Callback(void)
{
    if ((SET == I2C_GetStatus(I2C_UNIT, I2C_FLAG_TX_CPLT)) &&
        (RESET == I2C_GetStatus(I2C_UNIT, I2C_FLAG_NACKF))) {
        uint8_t data;
        if (SlaveTxBuffer->pop(data)) {
            I2C_WriteData(I2C_UNIT, data);
        }
        else {
            LOG_ERROR("SlaveTxBuffer TEI is empty");
        }
    }
}

/**
 * @brief   I2C RXI(receive buffer full) interrupt callback function
 * @param   None
 * @retval  None
 */
static void I2C_RXI_Callback(void)
{
    if (SET == I2C_GetStatus(I2C_UNIT, I2C_FLAG_RX_FULL)) {
        uint8_t data    = I2C_ReadData(I2C_UNIT);
        bool didOverrun = false;
        SlaveRxBuffer->push(data, true, didOverrun);
    }
}

inline void i2c_irq_register(stc_irq_signin_config_t &irq, const char *name, uint32_t priority = DDL_IRQ_PRIO_05)
{
    // get auto-assigned irqn and set in irq struct
    IRQn_Type irqn;
    irqn_aa_get(irqn, name);
    irq.enIRQn = irqn;

    // create irq registration struct
    stc_irq_signin_config_t irqConf = {
        .enIntSrc    = irq.enIntSrc,
        .enIRQn      = irq.enIRQn,
        .pfnCallback = irq.pfnCallback,
    };

    // register and enable irq
    INTC_IrqSignIn(&irqConf);
    NVIC_SetPriority(irqConf.enIRQn, priority);
    NVIC_ClearPendingIRQ(irqConf.enIRQn);
    NVIC_EnableIRQ(irqConf.enIRQn);
}

inline void i2c_irq_resign(stc_irq_signin_config_t &irq, const char *name)
{
    // disable interrupt and clear pending
    NVIC_DisableIRQ(irq.enIRQn);
    NVIC_ClearPendingIRQ(irq.enIRQn);
    INTC_IrqSignOut(irq.enIRQn);

    // resign auto-assigned irqn
    irqn_aa_resign(irq.enIRQn, name);
}

/**
 * @brief   Initialize the I2C peripheral for slave
 * @param   None
 * @retval int32_t:
 *            - LL_OK:              Success
 *            - LL_ERR_INVD_PARAM:  Invalid parameter
 */
int32_t Slave_Initialize(void)
{
    /* Enable I2C Peripheral*/
    FCG_Fcg1PeriphClockCmd(I2C_FCG_USE, ENABLE);
    if (SlaveRxBuffer == nullptr) {
        SlaveRxBuffer = new RingBuffer<uint8_t>(u8RxBuf, TEST_DATA_LEN);
    }
    if (SlaveTxBuffer == nullptr) {
        SlaveTxBuffer = new RingBuffer<uint8_t>(u8TxBuf, TEST_DATA_LEN);
    }

    /* Initialize I2C port*/
    GPIO_SetFunc(I2C_SCL_PORT, I2C_SCL_PIN, I2C_GPIO_SCL_FUNC);
    GPIO_SetFunc(I2C_SDA_PORT, I2C_SDA_PIN, I2C_GPIO_SDA_FUNC);

    int32_t i32Ret;
    stc_i2c_init_t stcI2cInit;
    stc_irq_signin_config_t stcIrqRegCfg;
    float32_t fErr;

    (void)I2C_DeInit(I2C_UNIT);

    stcI2cCom.enComStatus = I2C_COM_IDLE;

    (void)I2C_StructInit(&stcI2cInit);
    stcI2cInit.u32ClockDiv = I2C_CLK_DIV2;
    stcI2cInit.u32Baudrate = I2C_BAUDRATE;
    stcI2cInit.u32SclTime  = 5U;
    i32Ret                 = I2C_Init(I2C_UNIT, &stcI2cInit, &fErr);

    if (LL_OK == i32Ret) {
        /* Set slave address*/
#if (I2C_ADDR_MD == I2C_ADDR_MD_10BIT)
        I2C_SlaveAddrConfig(I2C_UNIT, I2C_ADDR0, I2C_ADDR_10BIT, DEVICE_ADDR);
#else
        I2C_SlaveAddrConfig(I2C_UNIT, I2C_ADDR0, I2C_ADDR_7BIT, DEVICE_ADDR);
#endif
        stcIrqRegCfg.enIntSrc    = I2C_INT_EEI_DEF;
        stcIrqRegCfg.pfnCallback = &I2C_EEI_Callback;
        i2c_irq_register(stcIrqRegCfg, "I2C_EEI_IRQN_DEF");
        // stcIrqRegCfg.enIRQn = I2C_EEI_IRQN_DEF;
        // stcIrqRegCfg.enIntSrc = I2C_INT_EEI_DEF;
        // stcIrqRegCfg.pfnCallback = &I2C_EEI_Callback;
        // (void)INTC_IrqSignIn(&stcIrqRegCfg);
        // NVIC_ClearPendingIRQ(stcIrqRegCfg.enIRQn);
        // NVIC_SetPriority(stcIrqRegCfg.enIRQn, DDL_IRQ_PRIO_DEFAULT);
        // NVIC_EnableIRQ(stcIrqRegCfg.enIRQn);

        stcIrqRegCfg.enIntSrc    = I2C_INT_RXI_DEF;
        stcIrqRegCfg.pfnCallback = &I2C_RXI_Callback;
        i2c_irq_register(stcIrqRegCfg, "I2C_RXI_IRQN_DEF");
        // stcIrqRegCfg.enIRQn = I2C_RXI_IRQN_DEF;
        // stcIrqRegCfg.enIntSrc = I2C_INT_RXI_DEF;
        // stcIrqRegCfg.pfnCallback = &I2C_RXI_Callback;
        // (void)INTC_IrqSignIn(&stcIrqRegCfg);
        // NVIC_ClearPendingIRQ(stcIrqRegCfg.enIRQn);
        // NVIC_SetPriority(stcIrqRegCfg.enIRQn, DDL_IRQ_PRIO_DEFAULT);
        // NVIC_EnableIRQ(stcIrqRegCfg.enIRQn);

        stcIrqRegCfg.enIntSrc    = I2C_INT_TEI_DEF;
        stcIrqRegCfg.pfnCallback = &I2C_TEI_Callback;
        i2c_irq_register(stcIrqRegCfg, "I2C_TEI_IRQN_DEF");
        // stcIrqRegCfg.enIRQn = I2C_TEI_IRQN_DEF;
        // stcIrqRegCfg.enIntSrc = I2C_INT_TEI_DEF;
        // stcIrqRegCfg.pfnCallback = &I2C_TEI_Callback;
        // (void)INTC_IrqSignIn(&stcIrqRegCfg);
        // NVIC_ClearPendingIRQ(stcIrqRegCfg.enIRQn);
        // NVIC_SetPriority(stcIrqRegCfg.enIRQn, DDL_IRQ_PRIO_DEFAULT);
        // NVIC_EnableIRQ(stcIrqRegCfg.enIRQn);

        I2C_Cmd(I2C_UNIT, ENABLE);
        /* Config slave address match and receive full interrupt function*/
        I2C_IntCmd(I2C_UNIT, I2C_INT_MATCH_ADDR0 | I2C_INT_RX_FULL, ENABLE);
    }
    return i32Ret;
}

HardwareI2cSlave ::HardwareI2cSlave()
{
    this->initialized     = false;
    this->__SlaveRxBuffer = nullptr;
    this->__SlaveTxBuffer = nullptr;
}

HardwareI2cSlave ::~HardwareI2cSlave()
{
    this->__SlaveRxBuffer = nullptr;
    this->__SlaveTxBuffer = nullptr;
}

bool HardwareI2cSlave ::begin(void)
{
    if (Slave_Initialize() == LL_OK) {
        updateBuffers(SlaveRxBuffer, SlaveTxBuffer);
        this->initialized = true;
        return true;
    }
    return false;
}

int HardwareI2cSlave ::available(void)
{
    if (this->initialized == false)
        return 0;
    return this->__SlaveRxBuffer->count();
}

int HardwareI2cSlave ::availableForWrite(void)
{
    return this->__SlaveTxBuffer->capacity() - this->__SlaveTxBuffer->count();
}

int HardwareI2cSlave ::peek(void)
{
    if (this->initialized == false)
        return 0;
    return this->__SlaveRxBuffer->peek();
}

int HardwareI2cSlave ::read(void)
{
    if (this->initialized == false)
        return 0;

    if (this->__SlaveRxBuffer == nullptr) {
        LOG_ERROR("SlaveRxBuffer is not initialized");
        return 0;
    }
    uint8_t data;
    if (this->__SlaveRxBuffer->pop(data)) {
        return data;
    }
    return -1;
}

void HardwareI2cSlave ::flush(void)
{
    if (this->initialized == false)
        return;
    this->__SlaveTxBuffer->clear();
}

size_t HardwareI2cSlave ::write(uint8_t n)
{
    if (this->initialized == false)
        return 0;
    while (this->__SlaveTxBuffer->isFull()) {
        delay_ms(1);
    }
    if (this->__SlaveTxBuffer->push(n)) {
        return 1;
    }
    return 0;
}

// 添加updateBuffers的实现
void HardwareI2cSlave::updateBuffers(RingBuffer<uint8_t> *rxBuffer, RingBuffer<uint8_t> *txBuffer)
{
    if (rxBuffer != nullptr) {
        this->__SlaveRxBuffer = rxBuffer;
    }
    if (txBuffer != nullptr) {
        this->__SlaveTxBuffer = txBuffer;
    }
}

bool HardwareI2cSlave::isTransmitting(void)
{
    return stcI2cCom.enMode == MD_TX;
}

HardwareI2cSlave I2C_Slave;

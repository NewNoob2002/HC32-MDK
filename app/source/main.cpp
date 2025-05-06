/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "rtthread.h"
#include <Arduino.h>
#include <Bq40z50.h>
#include <Mp2762A.h>
#include <Led.h>
#include <HardwareI2cSlave.h>
#include <Task.h>
#include <Wire.h>
#include <global.h>
#include <hc32_ll.h>
#include <settings.h>

// #include "MillisTaskManager.h"
// MillisTaskManager task;
/**
 * @addtogroup HC32F460_DDL_Examples
 * @{
 */
uint8_t chargeStatus      = notCharge;
uint16_t batteryLevelPercent = 50; // SOC measured from fuel gauge, in %. Used in
                                // multiple places (display, serial debug, log)
float batteryVoltage                = 0.0;
float batteryChargingPercentPerHour = 0.0;
float batteryTempC                  = 0.0;

uint8_t POWER_OFF_FLAG = 0;
present_device present_devices;
online_device online_devices;

// system device
unsigned short reg_value        = 0;
static uint32_t lastPowerOnTime = 0;
SystemParameter DisplayPannelParameter;
/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
static struct rt_thread keyCheck_thread;
static rt_uint8_t keyCheck_thread_stack[1024];
static rt_uint8_t keyCheck_thread_priority = 5;

static struct rt_thread led_thread;
static rt_uint8_t led_thread_stack[1024];
static rt_uint8_t led_thread_priority = 5;

static struct rt_thread message_thread;
static rt_uint8_t message_thread_stack[2048];
static rt_uint8_t message_thread_priority = 7;

static struct rt_thread BatteryCheck_thread;
static rt_uint8_t BatteryCheck_thread_stack[1024];
static rt_uint8_t BatteryCheck_thread_priority = 6;
/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/* unlock/lock peripheral */
#define EXAMPLE_PERIPH_WE                                                     \
    (LL_PERIPH_GPIO | LL_PERIPH_EFM | LL_PERIPH_FCG | LL_PERIPH_PWC_CLK_RMU | \
     LL_PERIPH_SRAM)
#define EXAMPLE_PERIPH_WP (LL_PERIPH_EFM | LL_PERIPH_FCG | LL_PERIPH_SRAM)

void Charging_State_Indication()
{
    if (online_devices.bq40z50 == true) {
        static char on_off = 0;

        unsigned char value1 = 0;
        unsigned char value2 = 0;

        value1 = GPIO_ReadInputPins(CHARGER_CTRL_PORT, CHARGER_CTRL_PIN1);
        value2 = GPIO_ReadInputPins(CHARGER_CTRL_PORT, CHARGER_CTRL_PIN2);

        checkBatteryLevels();
        AdcPolling();

        /// if (BAT_Parameter.EQ < 100)
        if (batteryLevelPercent < 99) {
            if (value1) {
                on_off = ~on_off;
                chargeLedBlink(100);
                //				if (value2)
                //					Charge_Current_Select(3000);
                //				else
                //					Charge_Current_Select(500);
                //				Charge_Enable_Switch(1);
            }
        } else if (batteryLevelPercent < 101) {
            //			Charge_Led_Switch(1);
            //			DDL_DelayMS(100);
            //			Charge_Enable_Switch(1);
        }
        WatchdogFeed();
    }
}
/**
 * @brief  Main function of SPI tx/rx dma project
 * @param  None
 * @retval int32_t return value, if needed
 */
int main(void)
{
    /* Peripheral registers write unprotected */
    LL_PERIPH_WE(EXAMPLE_PERIPH_WE);
    /* Configure BSP */
    PowerControlInit();
    keyInit();
    ledInit();
    // start Init device
    unsigned short reg_value = *((unsigned short *)0x400540C0UL);
    if (reg_value & 0x0100U) {
        LOG_DEBUG("SoftReset");
        Power_Control_Pin_Switch(1);
        //        Power_Control_Pin_Switch(1);
    } else if (reg_value & 0x0002U) {
        LOG_DEBUG("EWDT or Hardware reset");
    } else if (reg_value & 0x2000U) {
        LOG_ERROR("XTAL error");
    }
    //	Charger_Control_GPIO_Init();
    //	USB_Switch_GPIO_Init();
#if 1
    delay_ms(50);
    if (Wire.begin() == LL_OK)
        online_devices.i2c = true;
    bq40z50 = new BQ40Z50();
    //	mp2762a = new MP2762A();
    if (bq40z50->begin(Wire)) {
        LOG_INFO("Find bq40z50 fuel gauge-- 0x0B");
        online_devices.bq40z50 = true;
    }
    rt_thread_idle_sethook(idleFeedWatchdogTask);
    /// Poweroff charge
    //    USB_Switch_GPIO_Control(1);
    while (1) {
        if (reg_value & 0x0100U)
            break;
        if (PowerKeyTrigger >= 6)
            break;
        Charging_State_Indication();
        delay_ms(500);
    }
    //	USB_Switch_GPIO_Control(0);
    //
    //	Charge_Enable_Switch(0);
    chargeLedSwitch(0);
    PowerKeyTrigger = 0;

    powerLedSwitch(1);
    /// Start receiver system power
    Power_Control_Pin_Switch(1);
    functionKeyLedSwitch(1);
    delay_ms(1000);
#endif
    memset(&DisplayPannelParameter, 0, sizeof(SystemParameter));
    memcpy(DisplayPannelParameter.hw_version, HW_VERSION, strlen(HW_VERSION));
    memcpy(DisplayPannelParameter.sw_version, SW_VERSION, strlen(SW_VERSION));
    //	mp2762a->registerReset();
    //	mp2762a->setFastChargeVoltageMv(6600);
    //	mp2762a->setPrechargeCurrentMa(500);

    //  mp2762a->setFastChargeCurrentMa(1600);
    //  mp2762a->getChargeStatus();

    if (I2C_Slave.begin() != true) {
        LOG_ERROR("I2C Slave init failed--Reset");
        powerLedSwitch(0);
        delay_ms(1000);
        powerLedSwitch(1);
        NVIC_SystemReset();

    } else {
        online_devices.i2c_slave = true;
        LOG_INFO("I2C Slave init success");
    }

    /* Peripheral registers write protected */
    LL_PERIPH_WP(EXAMPLE_PERIPH_WP);
    rt_thread_init(&message_thread,
                   "message_task",
                   btReadTask,
                   RT_NULL,
                   &message_thread_stack,
                   sizeof(message_thread_stack),
                   message_thread_priority,
                   1000);
    rt_thread_init(&keyCheck_thread, "keyCheck", KeyMonitor, RT_NULL,
                   &keyCheck_thread_stack, sizeof(keyCheck_thread_stack),
                   keyCheck_thread_priority, 100);
    rt_thread_init(&led_thread, "led", ledStatusUpdateTask, RT_NULL,
                   &led_thread_stack, sizeof(led_thread_stack),
                   led_thread_priority, 100);
    rt_thread_init(&BatteryCheck_thread, "BatteryCheck", BatteryCheckTask, RT_NULL,
                   &BatteryCheck_thread_stack, sizeof(BatteryCheck_thread_stack),
                   BatteryCheck_thread_priority, 100);
    rt_thread_startup(&led_thread);
    rt_thread_startup(&message_thread);
    rt_thread_startup(&keyCheck_thread);
    rt_thread_startup(&BatteryCheck_thread);

    LOG_DEBUG("Start Task Now Time: %d", millis());
}

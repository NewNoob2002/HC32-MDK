#include "debug.h"
#include "AdcExpand.h"
#include "led.h"
#include "Charger.h"
#include "settings.h"

#include "powerControl.h"
// local variable
static unsigned int voltage = 0;
/* ADC reference voltage. The voltage of pin VREFH. */
#define ADC_VREF (3.3F)

//
/* Calculate the voltage(mV). */
#define ADC_CAL_VOL(adcVal) (uint16_t)((((adcVal) * ADC_VREF) / (4095)) * 1000.F)

unsigned int PowerKeyTrigger = 0;
unsigned int PowerOffCount = 0;
unsigned int PowerOffFlag = 0;

// 采样时间数组
uint8_t au8SampTime[1] = {100};

bool adc_config_average(adc_device_t *adc_device, en_adc_avcnt_t avgCount)
{
    if(ADC_ConfigAvg(adc_device->adc.register_base, avgCount) == 0)
    {
        LOG_INFO("ADC average config %d", avgCount);
        return true;
    }
    LOG_ERROR("ADC average config failed");
    return false;
}

bool adc_add_average_channel(adc_device_t *adc_device, uint32_t channel)
{
    if(ADC_AddAvgChannel(adc_device->adc.register_base, channel) == 0)
    {
        LOG_INFO("ADC add average channel %d", channel);
        return true;
    }
    LOG_ERROR("ADC add average channel failed");
    return false;
}

void adcInit()
{
    adc_device_init(&ADC1_device);
    adc_enable_channel(&ADC1_device, ADC_PIN_CH0, au8SampTime[0]);
    adc_config_average(&ADC1_device, AdcAvcnt_8);
    adc_add_average_channel(&ADC1_device, ADC_PIN_CH0);
    online_devices.adc = true;
}

void AdcPolling()
{
    if (!online_devices.adc)
    {
        LOG_ERROR("ADC is not initialized");
        return;
    }
    uint16_t u16AdcValue = 0;
    uint32_t u32Timeout = 0ul;

    bool bAdcConvertDone = false;
    adc_start_conversion(&ADC1_device);
    do
    {
        if (adc_is_conversion_completed(&ADC1_device))
        {
            bAdcConvertDone = true;
            break;
        }
    } while (u32Timeout++ < ADC_TIMEOUT_VAL);

    if (bAdcConvertDone)
    {
        u16AdcValue = adc_conversion_read_result(&ADC1_device, ADC_PIN_CH0);
        if (u16AdcValue > 2304)
            PowerKeyTrigger++;
        else
            PowerKeyTrigger = 0;
        voltage = ADC_CAL_VOL(u16AdcValue);
#if 0
        LOG_INFO("ADC value: %d", u16AdcValue);
        LOG_INFO("Voltage: %d mV", voltage);
        LOG_INFO("PowerKeyTrigger: %d", PowerKeyTrigger);
#endif
    }
    else
    {
        LOG_ERROR("ADC timeout\n");
    }
}

void Power_Control_Pin_Init()
{
    pinMode(MCU_ON_OFF_PIN, OUTPUT);
}

void Power_Control_Pin_Switch(uint8_t off_on)
{
    digitalWrite(MCU_ON_OFF_PIN, off_on);
}

void WatchdogFeedPinInit()
{
    pinMode(WATCHDOG_FEED_PIN, OUTPUT);
    digitalWrite(WATCHDOG_FEED_PIN, LOW);
    online_devices.watchDog = true;
}

void WatchdogFeed()
{
    if (!online_devices.watchDog)
    {
        LOG_ERROR("Watchdog is not initialized\n");
        return;
    }
    static unsigned char value = 0;

    value = ~value;
    if (value)
    {
        digitalWrite(WATCHDOG_FEED_PIN, LOW);
    }
    else
    {
        digitalWrite(WATCHDOG_FEED_PIN, HIGH);
    }
}

void PowerControlInit()
{
    adcInit();
    // Power_Control_Pin_Init();
    // WatchdogFeedPinInit();
}

void PowerControlTask()
{
    if (PowerKeyTrigger >= 5)
    {
        functionKeyLedOn();
        DisplayPannelParameter.poweroff_flag = 1;
    }
    if ((PowerKeyTrigger >= 6) || (PowerOffFlag))
    {
        PowerKeyTrigger = 0;
        powerLedOff();
        functionKeyLedOff();
        USB_Switch_GPIO_Control(1);

        if (8 == PowerOffFlag)
            delay(2000);

        Power_Control_Pin_Switch(0);
        /// USB_Switch_GPIO_Control(1);
        while (1)
        {
            delay(300);
            ChargeingStateCheck();
            if (PowerKeyTrigger >= 6)
            {
                NVIC_SystemReset();
            }
        }
    }
    AdcPolling();
    // ChargerControlMonitor();
    // FunctionKeyLedRecordIndicate();
}

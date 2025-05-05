#include <rtthread.h>
#include <Arduino.h>
#include "settings.h"
#include "global.h"

#include <Led.h>
ledState powerLed       = {false, 0, 0};
ledState functionKeyLed = {false, 0, 0};
ledState chargerLed     = {false, 0, 0};
ledState gnssLed        = {false, 0, 0};
ledState dataLed        = {false, 0, 0};

// Timer0 timer0(&TIMER02A_config, ledStatusUpdate);

void ledInit()
{
    pinMode(CHG_LED_PIN, OUTPUT);
    pinMode(POWER_LED_PIN, OUTPUT);
    pinMode(DATA_LED_PIN, OUTPUT);
    pinMode(GNSS_STATE_LED_PIN, OUTPUT);
    pinMode(FunctionKey_LED_PIN, OUTPUT);
    //	  timer0.start(1000000, 1);
    //    timer0.resume();
}

// 充电灯
void chargeLedSwitch(uint8_t newState)
{
    if (CHG_LED_PIN != PIN_UNDEFINED)
        digitalWrite(CHG_LED_PIN, newState);
}

void chargeLedBlink(int rate)
{
    if (CHG_LED_PIN != PIN_UNDEFINED) {
        chargerLed.currentRate = rate;
    }
}

void powerLedSwitch(uint8_t newState)
{
    if (POWER_LED_PIN != PIN_UNDEFINED)
        digitalWrite(POWER_LED_PIN, newState);
}

void powerLedBlink(int rate)
{
    if (POWER_LED_PIN != PIN_UNDEFINED)
        powerLed.currentRate = rate;
}

void dataLedSwitch(uint8_t newState)
{
    if (DATA_LED_PIN != PIN_UNDEFINED)
        digitalWrite(DATA_LED_PIN, newState);
}

void dataLedBlink(int rate)
{
    if (DATA_LED_PIN != PIN_UNDEFINED)
        dataLed.currentRate = rate;
}

void gnssLedSwitch(uint8_t newState)
{
    if (GNSS_STATE_LED_PIN != PIN_UNDEFINED)
        digitalWrite(GNSS_STATE_LED_PIN, newState);
}

void gnssLedBlink(int rate)
{
    if (GNSS_STATE_LED_PIN != PIN_UNDEFINED)
        gnssLed.currentRate = rate;
}

void functionKeyLedSwitch(uint8_t newState)
{
    if (FunctionKey_LED_PIN != PIN_UNDEFINED)
        digitalWrite(FunctionKey_LED_PIN, newState);
}

void functionKeyLedBlink(int rate)
{
    if (FunctionKey_LED_PIN != PIN_UNDEFINED)
        functionKeyLed.currentRate = rate;
}

void ChargerLedUpdate()
{
    if (CHG_LED_PIN != PIN_UNDEFINED) {
        // if (online_devices.mp2762 == false)
        //     return;

        if (chargerLed.currentRate > 0) {
            const uint32_t now = millis();
            if (now - chargerLed.lastToggleTime >= (uint32_t)chargerLed.currentRate) {
                chargerLed.isOn = !chargerLed.isOn;
                digitalWrite(CHG_LED_PIN, chargerLed.isOn ? LOW : HIGH);
                chargerLed.lastToggleTime = now;
            }
        }
        // 充电状态）
        static const struct
        {
            uint8_t status;
            uint16_t blinkInterval;
        } chargePatterns[] = {
            {normalCharge, 1000}, // 普通充电：1秒间隔
            {fastCharge, 300},    // 快速充电：0.3秒间隔
        };

        // if (isCharging())
        // {
        //     // 充满电判断（带0.5%滞回区间防抖）
        //     const bool isFullyCharged = (batteryLevelPercent >= 99.80f);

        //     if (!isFullyCharged)
        //     {
        //         // 动态匹配当前充电模式
        //         for (const auto &pattern : chargePatterns)
        //         {
        //             if (chargeStatus == pattern.status)
        //             {
        //                 chargeLedBlink(pattern.blinkInterval);
        //                 break;
        //             }
        //         }
        //     }
        //     else
        //     {
        //         // 进入饱和阶段：常亮指示
        //         // poweroffCharger = false;
        //         chargeLedBlink(0);
        //         chargeLedOn();
        //     }
        // }
        // else
        // {
        //     // 非充电状态统一关闭
        //     chargeLedBlink(0);
        //     chargeLedOff();
        // }
    }
}

void PowerLedUpdate()
{
    if (POWER_LED_PIN != PIN_UNDEFINED) {
        if (online_devices.bq40z50 == false)
            return;
        if (powerLed.currentRate > 0) {
            const uint32_t now = millis();
            if (now - powerLed.lastToggleTime >= (uint32_t)powerLed.currentRate) {
                powerLed.isOn = !powerLed.isOn;
                digitalWrite(POWER_LED_PIN, powerLed.isOn ? LOW : HIGH);
                powerLed.lastToggleTime = now;
            }
        }
				if(batteryLevelPercent >=50)
				{
					powerLedBlink(0);
					powerLedSwitch(0);
				}
    }
}

void DataLedUpdate()
{
    if (DATA_LED_PIN != PIN_UNDEFINED) {
        if (dataLed.currentRate > 0) {
            const uint32_t now = millis();
            if (now - dataLed.lastToggleTime >= (uint32_t)dataLed.currentRate) {
                dataLed.isOn = !dataLed.isOn;
                digitalWrite(DATA_LED_PIN, dataLed.isOn ? LOW : HIGH);
                dataLed.lastToggleTime = now;
            }
        }
    }
}

void GnssLedUpdate()
{
    if (GNSS_STATE_LED_PIN != PIN_UNDEFINED) {
        if (gnssLed.currentRate > 0) {
            const uint32_t now = millis();
            if (now - gnssLed.lastToggleTime >= (uint32_t)gnssLed.currentRate) {
                gnssLed.isOn = !gnssLed.isOn;
                digitalWrite(GNSS_STATE_LED_PIN, gnssLed.isOn ? LOW : HIGH);
                gnssLed.lastToggleTime = now;
            }
        }
    }
}

void FunctionKeyLedUpdate()
{
    if (FunctionKey_LED_PIN != PIN_UNDEFINED) {
        if (functionKeyLed.currentRate > 0) {
            const uint32_t now = millis();
            if (now - functionKeyLed.lastToggleTime >= (uint32_t)functionKeyLed.currentRate) {
                functionKeyLed.isOn = !functionKeyLed.isOn;
                digitalWrite(FunctionKey_LED_PIN, functionKeyLed.isOn ? LOW : HIGH);
                functionKeyLed.lastToggleTime = now;
            }
        }
    }
}

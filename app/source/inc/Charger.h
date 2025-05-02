#ifndef CHARGE_H
#define CHARGE_H

#include <SoftI2C.h>
#include <HardwareI2C.h>

#define UseHardwareI2C true
const uint8_t mp2762deviceAddress = 0x5C;

#define MP2762A_SETCHARGE_CURRENT 0x02
#define MP2762A_PRECHARGE_CURRENT 0x03
#define MP2762A_PRECHARGE_THRESHOLD 0x07
#define MP2762A_CONFIG_0 0x08
#define MP2762A_CONFIG_1 0x09
#define MP2762A_STATUS 0x13
#define MP2762A_FAULT_REGISTER 0x14
#define MP2762A_BATTERY_VOLTAGE 0x16
#define MP2762A_SYSTEM_VOLTAGE 0x18
#define MP2762A_CHARGE_CURRENT_L 0x1A
#define MP2762A_CHARGE_CURRENT_H 0x1B
#define MP2762A_INPUT_VOLTAGE_L 0x1C
#define MP2762A_INPUT_VOLTAGE_H 0x1D
#define MP2762A_INPUT_CURRENT_L 0x1E
#define MP2762A_INPUT_CURRENT_H 0x1F
#define MP2762A_PRECHARGE_THRESHOLD_OPTION 0x30

#define CHARGER_ENABLE_PIN           PA4

#define CHARGER_CTRL_PIN1            PB7           ///1.65V
#define CHARGER_CTRL_PIN2            PB6           ///3.23V

#define CHARGER_SWITCH_PIN           PB10

#define USB_SWITCH_PIN               PB8

#define CHARGER_POWER_DET_PIN        PA1

class Charger
{
private:
    SoftI2C *softi2c;
    HardwareI2C *hardwarei2c;
    bool debug = false;
    int maxTry = 3;
    uint32_t timeout = 400ul;

public:
    Charger(SoftI2C *softi2c);
    Charger(HardwareI2C *hardwarei2c);
    bool isPresent();
    bool writeRegister8(uint8_t reg, uint8_t value);
    bool readRegister8(uint8_t reg, uint8_t *data);
    uint16_t readRegister16(uint8_t reg);
    // Set the fast charge voltage, in mV
    void mp2762setFastChargeVoltageMv(uint16_t mVoltLevel);
    void mp2762setFastChargeCurrentMa(uint16_t currentLevelMa);
    // Set the current limit during precharge phase, in mA
    void mp2762setPrechargeCurrentMa(uint16_t currentLevelMa);
    float mp2762getBatteryVoltageMv();
    float mp2762getSystemVoltageMv();
    uint8_t mp2762getChargeStatus();
    float mp2762getChargeCurrentMa();
    float mp2762getInputVolatgeMv();
    float mp2762getInputCurrentMa();

    void mp2762resetSafetyTimer();
    void mp2762registerReset();
};

bool isCharging();

void ChargerControlInit();
void USB_Switch_GPIO_Control(unsigned char sw);
		
void Charge_Enable_Switch(unsigned char on_off);

void Charge_Current_Select(unsigned short select);

void ChargeingStateCheck();
#endif

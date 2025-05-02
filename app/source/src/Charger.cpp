#include <settings.h>
#include "global.h"

#include "Charger.h"
Charger::Charger(SoftI2C *softi2c)
{
    this->softi2c = softi2c;
    this->hardwarei2c = nullptr;
}

Charger::Charger(HardwareI2C *hardwarei2c)
{
    this->hardwarei2c = hardwarei2c;
    this->softi2c = nullptr;
}

bool Charger::isPresent()
{
    if (present_devices.mp2762)
    {
        if (hardwarei2c->isDeviceReady(mp2762deviceAddress))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}

bool Charger::writeRegister8(uint8_t reg, uint8_t value)
{
    int i = 0;
    for (i = 0; i < maxTry; i++)
    {
        if (hardwarei2c->writeRegister8(mp2762deviceAddress, reg, value))
            break;
        delay(1);
    }
    if (i == maxTry)
        return false;

    if (debug)
    {
        LOG_INFO("writeRegister8: 0x%x = 0b%x", reg, value);
    }
    return true;
}

bool Charger::readRegister8(uint8_t reg, uint8_t *data)
{
    int i = 0;
    for (i = 0; i < maxTry; i++)
    {
        if (hardwarei2c->readRegister8(mp2762deviceAddress, reg, data))
            break;
        delay(1);
    }
    if (i == maxTry)
        return false;

    if (debug)
    {
        Serial.print("readRegister8: 0x");
        Serial.print(reg, HEX);
        Serial.print(" = 0b");
        Serial.println(*data, BIN);
    }
    return true;
}

uint16_t Charger::readRegister16(uint8_t reg)
{
    int i = 0;
    uint8_t data[2];
    for (i = 0; i < maxTry; i++)
    {
        if (hardwarei2c->readRegister16(mp2762deviceAddress, reg, data))
            break;
        delay(1);
    }
    if (i == maxTry)
        return 0;

    if (debug)
    {
        Serial.print("readRegister16: 0x");
        Serial.print(reg, HEX);
        Serial.print(" = 0b");
        Serial.println((data[1] << 8) | data[0], BIN);
    }
    return (data[1] << 8) | data[0];
}

// Given a bit field, and a startingBitValue
// Example: Battery voltage is bit 12.5mV per bit
float convertBitsToDoubler(uint16_t bitField, float startingBitValue)
{
    float totalMv = 0;
    for (int x = 0; x < 16; x++)
    {
        if (bitField & 0x0001)
            totalMv += startingBitValue;

        bitField >>= 1;

        startingBitValue *= 2;
    }
    return (totalMv);
}

// Set the Precharge threshold
// 5.8V, 6.0, 6.2, 6.4, 6.6, 6.8, 7.4, 7.2 (oddly out of order)
void Charger::mp2762setFastChargeVoltageMv(uint16_t mVoltLevel)
{
    // Default to 6.8V (requires option '2')
    uint8_t option = 1;         // This is option 2 confusingly
    uint8_t newVbattPre = 0b01; // Default to 6.8V

    if (mVoltLevel <= 5800)
    {
        option = 0;
        newVbattPre = 0b00; // 5.8V
    }
    else if (mVoltLevel <= 6000)
    {
        option = 0;
        newVbattPre = 0b01; // 6.0V
    }
    else if (mVoltLevel <= 6200)
    {
        option = 0;
        newVbattPre = 0b10; // 6.2V
    }
    else if (mVoltLevel <= 6400)
    {
        option = 0;
        newVbattPre = 0b11; // 6.4V
    }
    else if (mVoltLevel <= 6600)
    {
        option = 1;
        newVbattPre = 0b00; // 6.6V
    }
    else if (mVoltLevel <= 6800)
    {
        option = 1;
        newVbattPre = 0b01; // 6.8V
    }
    else if (mVoltLevel <= 7200)
    {
        option = 1;
        newVbattPre = 0b11; // 7.2V
    }
    else if (mVoltLevel <= 7400)
    {
        option = 1;
        newVbattPre = 0b10; // 7.4V
    }

    // Set the Precharge bits
    uint8_t status = 0;
    readRegister8(MP2762A_PRECHARGE_THRESHOLD, &status);
    status &= ~(0b11 << 4); // Clear bits 4 and 5
    newVbattPre <<= 4;      // Shift to correct position
    status |= newVbattPre;  // Set bits accordingly
    writeRegister8(MP2762A_PRECHARGE_THRESHOLD, status);

    // Set the option bit
    readRegister8(MP2762A_PRECHARGE_THRESHOLD_OPTION, &status);
    status &= ~(1 << 3); // Clear bit 3
    option <<= 3;        // Shift to correct position
    status |= option;    // Set bit accordingly
    writeRegister8(MP2762A_PRECHARGE_THRESHOLD_OPTION, status);
}

void Charger::mp2762setFastChargeCurrentMa(uint16_t currentLevelMa)
{
    // defualt to 1A
    //  uint8_t oldBit = 0x00;
    //  mp2762ReadRegister8(MP2762A_SETCHARGE_CURRENT, &oldBit);
    uint8_t newIFast = 0b00100001;
    writeRegister8(MP2762A_SETCHARGE_CURRENT, newIFast);
}

void Charger::mp2762setPrechargeCurrentMa(uint16_t currentLevelMa)
{
    uint8_t newIPre = 0b0011; // Default to 180mA

    if (currentLevelMa <= 180)
        newIPre = 0b0011; // 180mA
    else
    {
        uint8_t steps = (currentLevelMa - 240) / 60; //(480 - 240)/ 60 = 4
        newIPre = 0b0101 + steps;
    }

    // Set the Precharge current bits
    uint8_t status = 0;
    readRegister8(MP2762A_PRECHARGE_CURRENT, &status);
    status &= ~(0b1111 << 4); // Clear bits 7, 6, 5, 4
    newIPre <<= 4;            // Shift to correct position
    status |= newIPre;        // Set bits accordingly
    writeRegister8(MP2762A_PRECHARGE_CURRENT, status);
}

float Charger::mp2762getBatteryVoltageMv()
{
    uint16_t voltage = readRegister16(MP2762A_BATTERY_VOLTAGE);
    float batteryVoltage = convertBitsToDoubler(voltage >>= 6, 12.5); // Battery voltage is bit 15:6 so we need a 6 bit shift
    return (batteryVoltage);
}

float Charger::mp2762getSystemVoltageMv()
{
    uint16_t voltage = readRegister16(MP2762A_SYSTEM_VOLTAGE);
    float batteryVoltage = convertBitsToDoubler(voltage >>= 6, 12.5); // Battery voltage is bit 15:6 so we need a 6 bit shift
    return (batteryVoltage);
}

uint8_t Charger::mp2762getChargeStatus()
{
    uint8_t status = 0;
    readRegister8(MP2762A_STATUS, &status);
    status >>= 2;
    status &= 0b11;
    if (status == 0)
        chargeStatus = notCharge;
    else if (status == 1)
        chargeStatus = normalCharge;
    else if (status == 2)
        chargeStatus = fastCharge;
    return (status);
}

float Charger::mp2762getChargeCurrentMa()
{
    uint8_t CurrentL = 0, CurrentH = 0;
    readRegister8(MP2762A_CHARGE_CURRENT_L, &CurrentL);
    readRegister8(MP2762A_CHARGE_CURRENT_H, &CurrentH);

    uint16_t Current = (CurrentH << 8) | CurrentL;
    Current = (Current >> 6) & 0x03FF;

    float charge_current_mA = Current * 12.5;
    return charge_current_mA;
}

float Charger::mp2762getInputVolatgeMv()
{
    uint8_t VoltageL = 0, VoltageH = 0;
    readRegister8(MP2762A_INPUT_VOLTAGE_L, &VoltageL);
    readRegister8(MP2762A_INPUT_VOLTAGE_H, &VoltageH);

    uint16_t Voltage = (VoltageL << 8) | VoltageH;
    Voltage = (Voltage >> 6) & 0x03FF;

    float Input_voltage_mV = Voltage * 25;
    return Input_voltage_mV;
}

float Charger::mp2762getInputCurrentMa()
{
    uint8_t CurrentL = 0, CurrentH = 0;
    readRegister8(MP2762A_INPUT_CURRENT_L, &CurrentL);
    readRegister8(MP2762A_INPUT_CURRENT_L, &CurrentH);

    uint16_t Current = (CurrentH << 8) | CurrentL;
    Current = (Current >> 6) & 0x03FF;

    float charge_current_mA = Current * 6.25;
    return charge_current_mA;
}

void Charger::mp2762resetSafetyTimer()
{
    uint8_t status = 0;
    readRegister8(MP2762A_CONFIG_0, &status);

    status &= ~(1 << 4); // Clear the CHG_EN bit
    writeRegister8(MP2762A_CONFIG_0, status);

    status |= (1 << 4); // Set the CHG_EN bit
    writeRegister8(MP2762A_CONFIG_0, status);
}

void Charger::mp2762registerReset()
{
    uint8_t status = 0;
    readRegister8(MP2762A_CONFIG_0, &status);
    status |= 1 << 7; // Set REG_RST
    writeRegister8(MP2762A_CONFIG_0, status);
}

bool isCharging()
{
    return chargeStatus == fastCharge || chargeStatus == normalCharge;
}

void ChargerControlInit()
{
    pinMode(CHARGER_CTRL_PIN1, INPUT); // CHARGER_CON

    pinMode(CHARGER_CTRL_PIN2, INPUT); // RCSO

    pinMode(CHARGER_POWER_DET_PIN, INPUT); // CHARGER_ADC

    pinMode(CHARGER_ENABLE_PIN, OUTPUT); // CHARGER_ON
    digitalWrite(CHARGER_ENABLE_PIN, HIGH);

    pinMode(CHARGER_SWITCH_PIN, OUTPUT); // CHARGER_SEL
    digitalWrite(CHARGER_SWITCH_PIN, HIGH);

    pinMode(USB_SWITCH_PIN, OUTPUT); // USB_CHNL_SEL
    digitalWrite(USB_SWITCH_PIN, LOW);
}

void USB_Switch_GPIO_Control(unsigned char sw)
{
    digitalWrite(USB_SWITCH_PIN, sw);
}

void Charge_Enable_Switch(unsigned char on_off)
{
    digitalWrite(CHARGER_ENABLE_PIN, on_off);
}

void Charge_Current_Select(unsigned short select)
{
    if (500 == select)
        digitalWrite(CHARGER_SWITCH_PIN, LOW);
    else if (3000 == select)
        digitalWrite(CHARGER_SWITCH_PIN, HIGH);
    else
        digitalWrite(CHARGER_SWITCH_PIN, LOW);
}

void ChargeingStateCheck()
{
    static char on_off = 0;

    unsigned char value1 = 0;
    unsigned char value2 = 0;

    value1 = digitalRead(CHARGER_CTRL_PIN1);
    value2 = digitalRead(CHARGER_CTRL_PIN2);

    Serial.printf("value1: %d\n", value1);
    Serial.printf("value2: %d\n", value2);

    checkBatteryLevels();
    AdcPolling();

    /// if (BAT_Parameter.EQ < 100)
    if (batteryLevelPercent < 99)
    {
        if (value1)
        {
            on_off = ~on_off;
            digitalWrite(CHG_LED_PIN, on_off);
            delay(100);
            if (value2)
                Charge_Current_Select(3000);
            else
                Charge_Current_Select(500);
            Charge_Enable_Switch(1);
        }
    }
    else if (batteryLevelPercent < 101)
    {
        digitalWrite(CHG_LED_PIN, 1);
        delay(100);
        Charge_Enable_Switch(1);
    }
    WatchdogFeed();
}

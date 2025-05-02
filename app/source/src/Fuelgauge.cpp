#include <settings.h>
#include "global.h"

#include "Fuelgauge.h"

Fuelgauge::Fuelgauge(SoftI2C *softi2c)
{
    this->softi2c = softi2c;
}

Fuelgauge::Fuelgauge(HardwareI2C *hardwarei2c)
{
    this->hardwarei2c = hardwarei2c;
}

bool Fuelgauge::isPresent()
{
    if (present_devices.bq40z50)
    {
        if (hardwarei2c->isDeviceReady(bq40z50deviceAddress))
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

uint8_t Fuelgauge::readRegister8(uint8_t reg)
{
    uint8_t data;
    uint8_t i;
    for (i = 0; i < maxTry; i++)
    {
        if (hardwarei2c->readRegister8(bq40z50deviceAddress, reg, &data))
            break;
        delay(1);
    }
    if (i == maxTry)
        return 0;
    if (debug)
    {
        Serial.print("fuelgauge readRegister8: 0x");
        Serial.print(reg, HEX);
        Serial.print(" = 0b");
        Serial.println(data, BIN);
    }
    return data;
}

uint16_t Fuelgauge::readRegister16(uint8_t reg)
{
    uint8_t data[2];
    uint8_t i;
    for (i = 0; i < maxTry; i++)
    {
        if (hardwarei2c->readRegister16(bq40z50deviceAddress, reg, data))
            break;
        delay(1);
    }
    if (i == maxTry)
        return 0;
    uint16_t value = (data[1] << 8) | data[0];
    if (debug)
    {
        LOG_INFO("fuelgauge readRegister16: 0x%x = 0b%x", reg, value);
    }
    return value;
}

float Fuelgauge::bq40z50getTemperatureC()
{
    uint16_t temperature = readRegister16(BQ40Z50_TEMPERATURE); // In 0.1 K
    float tempC = temperature / 10.0;
    tempC -= 273.15;
    return (tempC);
}

uint16_t Fuelgauge::bq40z50getVoltageMv()
{
    return readRegister16(BQ40Z50_VOLTAGE);
}

int16_t Fuelgauge::bq40z50getCurrentMa()
{
    uint16_t current = readRegister16(BQ40Z50_CURRENT);
    return (current);
}

int16_t Fuelgauge::bq40z50getAverageCurrentMa()
{
    uint16_t current = readRegister16(BQ40Z50_AVERAGE_CURRENT);
    return (current);
}

uint8_t Fuelgauge::bq40z50getMaxError()
{
    return readRegister8(BQ40Z50_MAX_ERROR);
}

uint8_t Fuelgauge::bq40z50getRelativeStateOfCharge()
{
    return readRegister8(BQ40Z50_RELATIVE_STATE_OF_CHARGE);
}

float Fuelgauge::bq40z50getRelativeStateOfCharge(bool calculate)
{
    uint16_t nowBatteryCapacity = bq40z50getRemainingCapacityMah();
    uint16_t fullBatteryCapacity = bq40z50getFullChargeCapacityMah();
    return ((float)nowBatteryCapacity / (float)fullBatteryCapacity) * 100.0;
}

uint8_t Fuelgauge::bq40z50getAbsoluteStateOfCharge()
{
    return readRegister8(BQ40Z50_ABSOLUTE_STATE_OF_CHARGE);
}

uint16_t Fuelgauge::bq40z50getRemainingCapacityMah()
{
    return readRegister16(BQ40Z50_REMAINING_CAPACITY);
}

uint16_t Fuelgauge::bq40z50getFullChargeCapacityMah()
{
    return readRegister16(BQ40Z50_FULL_CHARGE_CAPACITY);
}

uint16_t Fuelgauge::bq40z50getRunTimeToEmptyMin()
{
    return readRegister16(BQ40Z50_RUNTIME_TO_EMPTY);
}

uint16_t Fuelgauge::bq40z50getAverageTimeToEmptyMin()
{
    return readRegister16(BQ40Z50_AVERAGE_TIME_TO_EMPTY);
}

uint16_t Fuelgauge::bq40z50getAverageTimeToFullMin()
{
    return readRegister16(BQ40Z50_AVERAGE_TIME_TO_FULL);
}

uint16_t Fuelgauge::bq40z50getChargingCurrentMa()
{
    return readRegister16(BQ40Z50_CHARGING_CURRENT);
}

uint16_t Fuelgauge::bq40z50getChargingVoltageMv()
{
    return readRegister16(BQ40Z50_CHARGING_VOLTAGE);
}

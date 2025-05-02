#ifndef FUELGAUGE_H
#define FUELGAUGE_H

#include <SoftI2C.h>
#include <HardwareI2C.h>
const uint8_t bq40z50deviceAddress = 0x0B;

#define BQ40Z50_TEMPERATURE 0x08
#define BQ40Z50_VOLTAGE 0x09
#define BQ40Z50_CURRENT 0x0A
#define BQ40Z50_AVERAGE_CURRENT 0x0B
#define BQ40Z50_MAX_ERROR 0x0C
#define BQ40Z50_RELATIVE_STATE_OF_CHARGE 0x0D
#define BQ40Z50_ABSOLUTE_STATE_OF_CHARGE 0x0E
#define BQ40Z50_REMAINING_CAPACITY 0x0F
#define BQ40Z50_FULL_CHARGE_CAPACITY 0x10
#define BQ40Z50_RUNTIME_TO_EMPTY 0x11
#define BQ40Z50_AVERAGE_TIME_TO_EMPTY 0x12
#define BQ40Z50_AVERAGE_TIME_TO_FULL 0x13
#define BQ40Z50_CHARGING_CURRENT 0x14
#define BQ40Z50_CHARGING_VOLTAGE 0x15
#define BQ40Z50_CYCLE_COUNT 0x17
#define BQ40Z50_CELL_VOLTAGE_1 0x3F
#define BQ40Z50_CELL_VOLTAGE_2 0x3E
#define BQ40Z50_CELL_VOLTAGE_3 0x3D
#define BQ40Z50_CELL_VOLTAGE_4 0x3C

class Fuelgauge
{
private:
    bool debug = false;
    SoftI2C *softi2c;
    HardwareI2C *hardwarei2c;
    uint8_t maxTry = 3;
    uint8_t timeout = 200ul;

public:
    Fuelgauge(SoftI2C *softi2c);
    Fuelgauge(HardwareI2C *hardwarei2c);
    bool isPresent();
    uint8_t readRegister8(uint8_t reg);
    uint16_t readRegister16(uint8_t reg);
    // void writeRegister(uint8_t address, uint8_t *data, uint16_t length);
    float bq40z50getTemperatureC();
    uint16_t bq40z50getVoltageMv();
    int16_t bq40z50getCurrentMa();
    int16_t bq40z50getAverageCurrentMa();
    uint8_t bq40z50getMaxError();
    uint8_t bq40z50getRelativeStateOfCharge();
    float bq40z50getRelativeStateOfCharge(bool calculate);
    uint8_t bq40z50getAbsoluteStateOfCharge();
    uint16_t bq40z50getRemainingCapacityMah();
    uint16_t bq40z50getFullChargeCapacityMah();
    uint16_t bq40z50getRunTimeToEmptyMin();
    uint16_t bq40z50getAverageTimeToEmptyMin();
    uint16_t bq40z50getAverageTimeToFullMin();
    uint16_t bq40z50getChargingCurrentMa();
    uint16_t bq40z50getChargingVoltageMv();
    // uint16_t bq40z50getCycleCount();
    // uint16_t bq40z50getCellVoltage1Mv();
    // uint16_t bq40z50getCellVoltage2Mv();
    // uint16_t bq40z50getCellVoltage3Mv();
    // uint16_t bq40z50getCellVoltage4Mv();
};

#endif

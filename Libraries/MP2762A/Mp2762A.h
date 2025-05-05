#ifndef _MP2762A_LIBRARY_H
#define _MP2762A_LIBRARY_H

#include <Arduino.h>
#include <Wire.h>

#define MP2762A_SETCHARGE_CURRENT 0x02
#define MP2762A_PRECHARGE_CURRENT 0x03
#define MP2762A_PRECHARGE_THRESHOLD 0x07
#define MP2762A_CONFIG_0 0x08
#define MP2762A_CONFIG_1 0x09
#define MP2762A_STATUS 0x13
#define MP2762A_FAULT_REGISTER 0x14
#define MP2762A_BATTERY_VOLTAGE 0x16
#define MP2762A_CHARGE_CURRENT 0x1A
#define MP2762A_INPUT_VOLTAGE 0x1C
#define MP2762A_INPUT_CURRENT 0x1E
#define MP2762A_PRECHARGE_THRESHOLD_OPTION 0x30

float convertBitsToDoubler(uint16_t bitField, float startingBitValue);

class MP2762A {
private:
  const uint8_t mp2762aDeviceAddress = 0x5C;
  TwoWire *_i2cPort;

  uint8_t readRegister8(uint8_t addr);
  uint16_t readRegister16(uint8_t addr);
  uint8_t writeRegister8(uint8_t address, uint8_t value);

public:
  bool begin(TwoWire &wirePort = Wire);
  bool isConnected();

  // Reset all registers to defaults
  void registerReset();
  // The reset timer can be reset by sequentially writing 0 then 1 to REG08H,
  // bit[4].
  void resetSafetyTimer();
  // Returns: 0b00 - Not charging, 01 - trickle or precharge, 10 - fast charge,
  // 11 - charge termination
  uint8_t getChargeStatus();
  uint16_t getBatteryVoltageMv();
  // Set the current limit during precharge phase, in mA
  void setPrechargeCurrentMa(uint16_t currentLevelMa);
  // Set the Precharge threshold
  // 5.8V, 6.0, 6.2, 6.4, 6.6, 6.8, 7.4, 7.2 (oddly out of order)
  void setFastChargeVoltageMv(uint16_t mVoltLevel);

	void setFastChargeCurrentMa(uint16_t currentLevelMa);
};

extern MP2762A *mp2762a;
#endif
#include <debug.h>

#include <Mp2762A.h>

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

MP2762A *mp2762a = nullptr;
// Initializes the sensor with basic settings
// Returns false if sensor is not detected
bool MP2762A::begin(TwoWire &wirePort) {
  _i2cPort = &wirePort;

  if (isConnected() == false)
    return (false);

  return (true);
}

bool MP2762A::isConnected() {
  _i2cPort->beginTransmission(mp2762aDeviceAddress);
  if (_i2cPort->endTransmission() == 0)
    return (true); // All good
  return (false);  // not NACK
}

// Reads from a given location
// Stores the result at the provided outputPointer
uint8_t MP2762A::readRegister8(uint8_t addr) {
  _i2cPort->beginTransmission(mp2762aDeviceAddress);
  _i2cPort->write(addr);
  if (_i2cPort->endTransmission(false) !=
      0) // Send a restart command. Do not release bus.
  {
    // Sensor did not ACK
    LOG_ERROR("Sensor did not ack");
  }
  uint8_t reg_value;
  if (_i2cPort->requestFrom((uint8_t)mp2762aDeviceAddress, &reg_value, 1) > 0) {
    LOG_INFO("read 0x%02x, register :%d", addr, reg_value);
    return reg_value;
  }

  LOG_ERROR("can't read register 8");
  return (0);
}

// Reads two consecutive bytes from a given location
uint16_t MP2762A ::readRegister16(uint8_t addr) {
  _i2cPort->beginTransmission(mp2762aDeviceAddress);
  _i2cPort->write(addr);
  if (_i2cPort->endTransmission(false) != 0) {
    LOG_ERROR("Sensor did not ack!");
    return (0); // Sensor did not ACK
  }
  uint8_t buff[2];
  if (_i2cPort->requestFrom(mp2762aDeviceAddress, buff, 2) > 0) {
    LOG_INFO("read 0x%02x, register :%d", addr, buff[1] << 8 | buff[0]);
    return (uint16_t)buff[1] << 8 | buff[0];
  }
  LOG_ERROR("can't read register 16");
  return (0); // Sensor did not respond
}

uint8_t MP2762A ::writeRegister8(uint8_t address, uint8_t value) {
  _i2cPort->beginTransmission(mp2762aDeviceAddress);
	uint8_t buffer[2] = {address, value};
	_i2cPort->write(buffer, 2);
//  _i2cPort->write(address);
//  _i2cPort->write(value);
  if (_i2cPort->endTransmission() != 0) {
    LOG_ERROR("MP2762 error: Device did not ack write");
    return (false); // Sensor did not ACK
  }
  LOG_INFO("write 0x%02x to register 0x%02x", value, address);
  return (true);
}

void MP2762A ::registerReset() {
  uint8_t status = readRegister8(MP2762A_CONFIG_0);
  status |= 1 << 7; // Set REG_RST
  writeRegister8(MP2762A_CONFIG_0, status);
}

void MP2762A ::resetSafetyTimer() {
  uint8_t status = readRegister8(MP2762A_CONFIG_0);

  status &= ~(1 << 4); // Clear the CHG_EN bit
  writeRegister8(MP2762A_CONFIG_0, status);

  status |= (1 << 4); // Set the CHG_EN bit
  writeRegister8(MP2762A_CONFIG_0, status);
}

// Returns: 0b00 - Not charging, 01 - trickle or precharge, 10 - fast charge, 11
// - charge termination
uint8_t MP2762A ::getChargeStatus() {
  uint8_t status = readRegister8(MP2762A_STATUS);
  status >>= 2;
  status &= 0b11;
  return (status);
}

uint16_t MP2762A :: getBatteryVoltageMv()
{
    uint16_t voltage = readRegister16(MP2762A_BATTERY_VOLTAGE);
    voltage =
        (uint16_t)convertBitsToDoubler(voltage >>= 6, 12.5); // Battery voltage is bit 15:6 so we need a 6 bit shift
    return (voltage);
}

// Set the current limit during precharge phase, in mA
void MP2762A :: setPrechargeCurrentMa(uint16_t currentLevelMa)
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
    uint8_t status = readRegister8(MP2762A_PRECHARGE_CURRENT);
    status &= ~(0b1111 << 4); // Clear bits 7, 6, 5, 4
    newIPre <<= 4;            // Shift to correct position
    status |= newIPre;        // Set bits accordingly
    writeRegister8(MP2762A_PRECHARGE_CURRENT, status);
}

// Set the Precharge threshold
// 5.8V, 6.0, 6.2, 6.4, 6.6, 6.8, 7.4, 7.2 (oddly out of order)
void MP2762A :: setFastChargeVoltageMv(uint16_t mVoltLevel)
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
    uint8_t status = readRegister8(MP2762A_PRECHARGE_THRESHOLD);
    status &= ~(0b11 << 4); // Clear bits 4 and 5
    newVbattPre <<= 4;      // Shift to correct position
    status |= newVbattPre;  // Set bits accordingly
    writeRegister8(MP2762A_PRECHARGE_THRESHOLD, status);

    // Set the option bit
    status = readRegister8(MP2762A_PRECHARGE_THRESHOLD_OPTION);
    status &= ~(1 << 3); // Clear bit 3
    option <<= 3;        // Shift to correct position
    status |= option;    // Set bit accordingly
    writeRegister8(MP2762A_PRECHARGE_THRESHOLD_OPTION, status);
}

void MP2762A :: setFastChargeCurrentMa(uint16_t currentLevelMa)
{
    // defualt to 1A
    //  uint8_t oldBit = 0x00;
    //  mp2762ReadRegister8(MP2762A_SETCHARGE_CURRENT, &oldBit);
    uint8_t newIFast = 0b00100001;
    writeRegister8(MP2762A_SETCHARGE_CURRENT, newIFast);
}
#include <debug.h>

#include <Bq40z50.h>

BQ40Z50 *bq40z50 = nullptr;
//Initializes the sensor with basic settings
//Returns false if sensor is not detected
bool BQ40Z50::begin(TwoWire &wirePort)
{
  _i2cPort = &wirePort;

  if (isConnected() == false)
    return (false);

  return (true);
}

bool BQ40Z50::isConnected()
{
  _i2cPort->beginTransmission(bq40z50DeviceAddress);
  if (_i2cPort->endTransmission() == 0)
    return(true); //All good
  return(false); //not NACK
}

//Reads from a given location
//Stores the result at the provided outputPointer
uint8_t BQ40Z50::readRegister8(uint8_t addr)
{
  _i2cPort->beginTransmission(bq40z50DeviceAddress);
  _i2cPort->write(addr);
  if (_i2cPort->endTransmission(false) != 0) //Send a restart command. Do not release bus.
  {
    //Sensor did not ACK
    LOG_ERROR("Sensor did not ack");
  }
	uint8_t reg_value;
  if(_i2cPort->requestFrom((uint8_t)bq40z50DeviceAddress, &reg_value, 1) > 0)
	{
		LOG_INFO("read 0x%02x, register :%d", addr, reg_value);
		return reg_value;
	}

  LOG_ERROR("can't read register 8");
  return (0);
}

//Reads two consecutive bytes from a given location
uint16_t BQ40Z50::readRegister16(uint8_t addr)
{
  _i2cPort->beginTransmission(bq40z50DeviceAddress);
  _i2cPort->write(addr);
  if (_i2cPort->endTransmission(false) != 0)
  {
    LOG_ERROR("Sensor did not ack!");
    return (0); //Sensor did not ACK
  }
	uint8_t buff[2];
  if(_i2cPort->requestFrom(bq40z50DeviceAddress, buff, 2) > 0)
	{
		LOG_INFO("read 0x%02x, register :%d", addr, buff[1] << 8 | buff[0]);
		return (uint16_t) buff[1] << 8 | buff[0];
	}
	LOG_ERROR("can't read register 16");
  return (0); //Sensor did not respond
}

//Get/Set Helper Functions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

float BQ40Z50::getTemperatureC()
{
  uint16_t temperature = readRegister16(BQ40Z50_TEMPERATURE); //In 0.1 K
  float tempC = temperature / 10.0;
  tempC -= 273.15;
  return(tempC);
}

float BQ40Z50::getTemperatureF()
{
  return((getTemperatureC() * 9.0/5) + 32.0);
}

uint16_t BQ40Z50::getVoltageMv()
{
  return(readRegister16(BQ40Z50_VOLTAGE)); //In mV
}

int16_t BQ40Z50::getCurrentMa()
{
  return(readRegister16(BQ40Z50_CURRENT)); //In mA
}

int16_t BQ40Z50::getAverageCurrentMa()
{
  return(readRegister16(BQ40Z50_AVERAGE_CURRENT)); //In mA
}

uint8_t BQ40Z50::getMaxError()
{
  uint8_t maxError = readRegister8(BQ40Z50_MAX_ERROR); //In %
  return(maxError);
}

uint8_t BQ40Z50::getRelativeStateOfCharge()
{
  uint8_t relStateOfCharge = readRegister8(BQ40Z50_RELATIVE_STATE_OF_CHARGE); //In %
  return(relStateOfCharge);
}

uint8_t BQ40Z50::getAbsoluteStateOfCharge()
{
  uint8_t absStateOfCharge = readRegister8(BQ40Z50_ABSOLUTE_STATE_OF_CHARGE); //In %
  return(absStateOfCharge);
}

uint16_t BQ40Z50::getRemainingCapacityMah()
{
  return(readRegister16(BQ40Z50_REMAINING_CAPACITY)); //In mAh when CAPM = 0
}

uint16_t BQ40Z50::getFullChargeCapacityMah()
{
  return(readRegister16(BQ40Z50_FULL_CHARGE_CAPACITY)); //In mAh when CAPM = 0
}

uint16_t BQ40Z50::getRunTimeToEmptyMin()
{
  return(readRegister16(BQ40Z50_RUNTIME_TO_EMPTY)); //In minutes
}

uint16_t BQ40Z50::getAverageTimeToEmptyMin()
{
  return(readRegister16(BQ40Z50_AVERAGE_TIME_TO_EMPTY)); //In minutes
}

uint16_t BQ40Z50::getAverageTimeToFullMin()
{
  return(readRegister16(BQ40Z50_AVERAGE_TIME_TO_FULL)); //In minutes
}

uint16_t BQ40Z50::getChargingCurrentMa()
{
  return(readRegister16(BQ40Z50_CHARGING_CURRENT)); //In mA
}

uint16_t BQ40Z50::getChargingVoltageMv()
{
  return(readRegister16(BQ40Z50_CHARGING_VOLTAGE)); //In mV
}

uint16_t BQ40Z50::getCycleCount()
{
  return(readRegister16(BQ40Z50_CYCLE_COUNT));
}

uint16_t BQ40Z50::getCellVoltage1Mv()
{
  return(readRegister16(BQ40Z50_CELL_VOLTAGE_1));
}

uint16_t BQ40Z50::getCellVoltage2Mv()
{
  return(readRegister16(BQ40Z50_CELL_VOLTAGE_2));
}

uint16_t BQ40Z50::getCellVoltage3Mv()
{
  return(readRegister16(BQ40Z50_CELL_VOLTAGE_3));
}

uint16_t BQ40Z50::getCellVoltage4Mv()
{
  return(readRegister16(BQ40Z50_CELL_VOLTAGE_4));
}
#include <Arduino.h>
#include <HardwareI2cSlave.h>
#include <Bq40z50.h>
#include <Mp2762A.h>
#include <adc.h>
#include <global.h>
#include <settings.h>
#include <SparkFun_Extensible_Message_Parser.h>

#include <Task.h>
// debug set
bool DebugTask = true;
/// @brief Bluetooth parser
SEMP_PARSE_ROUTINE const btParserTable[] = {
    sempBluetoothPreamble,
};
const int btParserCount = sizeof(btParserTable) / sizeof(btParserTable[0]);

const char *const btParserNames[] = {
    "BluetoothAPP",
};
const int btParserNameCount = sizeof(btParserNames) / sizeof(btParserNames[0]);

SEMP_PARSE_STATE *Btparse  = nullptr;
bool btReadTaskRunning     = false;
bool btReadTaskStopRequest = false;
uint8_t bluetoothRxBuffer[1024];

SEMP_Bluetooth_HEADER *messageHeader = nullptr;
uint8_t *messageTxBuffer             = nullptr;

static int messageLength = 0;
uint8_t messageType      = 0x00;
uint16_t messageId       = 0x00;

void ledStatusUpdateTask(void *e)
{
    static uint32_t lastCheckTime = 0;
	  if (DebugTask == true)
        LOG_INFO("Task ledStatusUpdateTask started");
    while (1) {
        if (millis() - lastCheckTime > 10) {
            ChargerLedUpdate();
            PowerLedUpdate();
            DataLedUpdate();
            GnssLedUpdate();
            FunctionKeyLedUpdate();
						lastCheckTime = millis();
        }
        rt_thread_mdelay(10);
    }
}

void KeyMonitor(void *e)
{
	if (DebugTask == true)
      LOG_INFO("Task KeyMonitor started");
	while(1)
	{
		functionKey.update(!digitalRead(FunctionKey_PIN));
		rt_thread_mdelay(15);
	}
	if (DebugTask == true)
      LOG_INFO("Task KeyMonitor end");
}

void checkBatteryLevels()
{
    if (online_devices.bq40z50 == false)
        return;

    batteryVoltage = (bq40z50->getVoltageMv() / 1000.0);

		batteryLevelPercent = bq40z50->getRelativeStateOfCharge();
		batteryTempC = bq40z50->getTemperatureC();
		batteryChargingPercentPerHour = bq40z50->getBatteryChargingPercentPerHour();

    if (batteryLevelPercent == 0.0) {
        batteryLevelPercent = 50.0;
    } else if (batteryLevelPercent <= 99.80 && batteryVoltage >= 8.38) {
        batteryLevelPercent = 100.0;
    }
    return;
}


//void handleChargerTask()
//{
//chager
		//static uint32_t lastCheckChargerOnlineTime   = 0;
//    if (online_devices.mp2762) {
//        if (millis() - lastCheckChargerOnlineTime > 30 * 1000) {
//            if (mp2762.isPresent()) {
//                online_devices.mp2762 = true;
//                TaskDebug("mp2762 is still online-", millis());
//            } else {
//                online_devices.mp2762 = false;
//                TaskDebug("mp2762 is offline first time-", millis());
//            }
//            lastCheckChargerOnlineTime = millis();
//        }
//        mp2762.mp2762getChargeStatus();
//    } else {
//        I2cEnd();
//        I2cBegin();
//        if (mp2762.isPresent()) {
//            online_devices.mp2762 = true;
//            TaskDebug("mp2762 is offline but online again-", millis());
//        } else {
//            online_devices.mp2762 = false;
//            TaskDebug("mp2762 is offline second time-", millis());
//        }
//        lastCheckChargerOnlineTime = millis();
//    }
//}

void handleFuelgaugeTask()
{
	//battery fuelgauge
	static uint32_t lastCheckFuelGaugeOnlineTime = 0;
		if(bq40z50 == nullptr)
			return;
    if (online_devices.bq40z50) {
        if (millis() - lastCheckFuelGaugeOnlineTime > 60 * 1000) {
            if (bq40z50->isConnected()) {
                online_devices.bq40z50 = true;
                LOG_DEBUG("bq40z50 is still online-", millis());
            } else {
                online_devices.bq40z50 = false;
                LOG_DEBUG("bq40z50 is offline first time-", millis());
            }
            lastCheckFuelGaugeOnlineTime = millis();
        }
        checkBatteryLevels();
        if (1) {
            Serial.print("batteryLevelPercent:");
            Serial.println(batteryLevelPercent);
            Serial.print("batteryVoltage:");
            Serial.println(batteryVoltage);
            Serial.print("batteryChargingPercentPerHour:");
            Serial.println(batteryChargingPercentPerHour);
            Serial.print("batteryTempC:");
            Serial.println(batteryTempC);
        }
    } else {
        if (bq40z50->isConnected()) {
            online_devices.bq40z50 = true;
            LOG_DEBUG("bq40z50 is offline but online again-", millis());
        } else {
            online_devices.bq40z50 = false;
            LOG_DEBUG("bq40z50 is offline second time-", millis());
        }
        lastCheckFuelGaugeOnlineTime = millis();
    }
}

void BatteryCheckTask(void *e)
{
	if (DebugTask == true)
      LOG_INFO("Task BatteryCheckTask started");
	// static uint32_t lastBatteryChargerUpdate = 0;
  static uint32_t lastBatteryFuelGaugeUpdate = 0;
	while(1)
	{
    // 统一处理所有定时任务
    // if (millis() - lastBatteryChargerUpdate >= 1000)
    // {
    //     lastBatteryChargerUpdate = millis();
    //     handleChargerTask();
    // }

    if (millis() - lastBatteryFuelGaugeUpdate >= 3000) {
        lastBatteryFuelGaugeUpdate = millis();
        handleFuelgaugeTask();
    }
		rt_thread_mdelay(500);
	}
	if (DebugTask == true)
      LOG_INFO("Task BatteryCheckTask end");
}
void btDataProcess(SEMP_PARSE_STATE *parse, uint16_t type)
{
    messageHeader = (SEMP_Bluetooth_HEADER *)parse->buffer;
    messageId     = messageHeader->messageId;
    messageType   = messageHeader->messageType;

    LOG_DEBUG("this is the 0x%02x message", messageId);
    switch (messageId) {
        case 0x01: // 查询型号版本
        {
        }
    }
}
void btReadTask(void *e)
{
    int rxBytes = 0;
    Btparse     = sempBeginParser(btParserTable,
                                  btParserCount,
                                  btParserNames,
                                  btParserNameCount,
                                  0,
                                  1024 * 3,
                                  btDataProcess,
                                  "BluetoothDebug");
    if (!Btparse)
        LOG_ERROR("Failed to initialize the Bt parser");
    btReadTaskRunning = true;
    if (DebugTask == true)
        LOG_INFO("Task btReadTask started");
    while (btReadTaskStopRequest == false) {
        if (I2C_Slave.available() > 0) {
            rxBytes = I2C_Slave.readBytes(bluetoothRxBuffer, sizeof(bluetoothRxBuffer));
            for (int x = 0; x < rxBytes; x++) {
                sempParseNextByte(Btparse, bluetoothRxBuffer[x]);
            }
        }
        rt_thread_mdelay(10);
    }
    if (DebugTask == true)
        LOG_INFO("Task btReadTask end");
    btReadTaskRunning = false;
}
#include <Arduino.h>
#include <HardwareI2cSlave.h>
#include <global.h>
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

void i2c_slave_task(void *e)
{
    uint8_t rxBuff[512];
    if (DebugTask == true)
        LOG_INFO("Task i2c_slave_task started");
    while (1) {
        if (I2C_Slave.available()) {
            int len = I2C_Slave.readBytes(rxBuff, 512);
            Serial.write(rxBuff, len);
        }
        rt_thread_mdelay(1000);
    }
    if (DebugTask == true)
        LOG_INFO("Task i2c_slave_task end");
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
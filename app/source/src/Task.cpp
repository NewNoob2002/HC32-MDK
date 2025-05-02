#include <Arduino.h>
#include <HardwareI2cSlave.h>
#include <global.h>
#include <SparkFun_Extensible_Message_Parser.h>

#include <Task.h>

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
uint8_t *messageTxBuffer = nullptr;

static int messageLength = 0;
uint8_t messageType = 0x00;
uint16_t messageId = 0x00;

void led_task(void *e)
{
    while (1) {
        GPIO_TogglePins(GPIO_PORT_B, GPIO_PIN_14);
        rt_thread_mdelay(100);
    }
}

void i2c_slave_task(void *e)
{
    uint8_t rxBuff[512];
    while (1) {
        if (I2C_Slave.available()) {
            int len = I2C_Slave.readBytes(rxBuff, 512);
            Serial.write(rxBuff, len);
        }
        rt_thread_mdelay(1000);
    }
}

void btDataProcess(SEMP_PARSE_STATE *parse, uint16_t type)
{
    messageHeader = (SEMP_Bluetooth_HEADER *)parse->buffer;
    messageId     = messageHeader->messageId;
    messageType   = messageHeader->messageType;

    LOG_DEBUG("this is the 0x%02x message\n", messageId);
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

    btReadTaskRunning = false;
}
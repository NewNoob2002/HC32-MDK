#include "Led.h"
#include "Wire.h"
#include "rtthread.h"
#include "settings.h"
#include <Arduino.h>
#include <HardwareI2cSlave.h>
#include <Bq40z50.h>
#include <Mp2762A.h>
#include <adc.h>
#include <Message.h>
#include <string.h>
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
uint8_t messageRxBuffer[1024];

SEMP_Bluetooth_HEADER *messageHeader = nullptr;
const uint8_t messageHeaderLength    = sizeof(SEMP_Bluetooth_HEADER);
static uint8_t messageLength         = 0;
uint8_t *messageTxBuffer             = nullptr;

uint8_t messageType = 0x00;
uint16_t messageId  = 0x00;

void idleFeedWatchdogTask(void)
{
    LOG_INFO("idleFeedWatchdogTask started");
    static uint32_t lastFeedTime = 0;
    while (1) {
        if (millis() - lastFeedTime > 10000) {
            lastFeedTime = millis();
            if (online_devices.watchDog) {
                WatchdogFeed();
                LOG_DEBUG("Watchdog to be feed 10s one time");
            }
        }
    }
    LOG_INFO("idleFeedWatchdogTask end");
}

void ledStatusUpdateTask(void *e)
{
    static uint32_t lastCheckTime = 0;
    if (DebugTask == true)
        LOG_INFO("Task ledStatusUpdateTask started");
    while (1) {
        if (millis() - lastCheckTime > 100) {
            ChargerLedUpdate();
            PowerLedUpdate();
            DataLedUpdate();
            GnssLedUpdate();
            FunctionKeyLedUpdate();
            lastCheckTime = millis();
        }
        rt_thread_mdelay(100);
    }
}

void KeyMonitor(void *e)
{
    if (DebugTask == true)
        LOG_INFO("Task KeyMonitor started");
    while (1) {
        functionKey.update(!digitalRead(FunctionKey_PIN));
        rt_thread_mdelay(15);
    }
    if (DebugTask == true)
        LOG_INFO("Task KeyMonitor end");
}

// void handleChargerTask()
//{
// chager
// static uint32_t lastCheckChargerOnlineTime   = 0;
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
    // battery fuelgauge
    static uint32_t lastCheckFuelGaugeOnlineTime = 0;
    if (bq40z50 == nullptr)
        return;
    if (online_devices.bq40z50) {
        if (millis() - lastCheckFuelGaugeOnlineTime > 60 * 1000) {
            if (bq40z50->isConnected()) {
                online_devices.bq40z50 = true;
                LOG_DEBUG("bq40z50 is still online- %d", millis());
            } else {
                online_devices.bq40z50 = false;
                LOG_DEBUG("bq40z50 is offline first time- %d", millis());
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
            LOG_DEBUG("bq40z50 is offline but online again- %d", millis());
        } else {
            Wire.end();
            rt_thread_mdelay(200);
            Wire.begin();
            online_devices.bq40z50 = false;
            LOG_DEBUG("bq40z50 is offline second time- %d", millis());
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
    while (1) {
        // 统一处理所有定时任务
        // if (millis() - lastBatteryChargerUpdate >= 1000)
        // {
        //     lastBatteryChargerUpdate = millis();
        //     handleChargerTask();
        // }

        if (millis() - lastBatteryFuelGaugeUpdate >= 2000) {
            lastBatteryFuelGaugeUpdate = millis();
            handleFuelgaugeTask();
        }
        rt_thread_mdelay(250);
    }
    if (DebugTask == true)
        LOG_INFO("Task BatteryCheckTask end");
}
/*消息处理任务和相关函数↓*/
uint32_t calculate_crc(uint8_t *msg, uint16_t len)
{
    uint32_t crc = 0xFFFFFFFF;
    uint16_t n;

    if (NULL == msg)
        return 1;
    for (n = 0; n < len; n++)
        crc = semp_crc32Table[(crc ^ msg[n]) & 0xff] ^ (crc >> 8);

    return crc ^ 0xFFFFFFFF;
}
// fill message header
void fill_message_header(uint8_t *msg, uint16_t msg_id, uint8_t msg_type, uint16_t len)
{
    if (len == 0)
        return;
    msg[0]                            = MSG_BluetoothSyncA;
    msg[1]                            = MSG_BluetoothSyncB;
    msg[2]                            = MSG_BluetoothSyncC;
    msg[3]                            = messageHeaderLength;
    msg[NM_PROTOCOL_MSG_ID_INDEX_L]   = msg_id & 0xFF;
    msg[NM_PROTOCOL_MSG_ID_INDEX_H]   = (msg_id >> 8) & 0xFF;
    msg[NM_PROTOCOL_MSG_LEN_INDEX_L]  = len & 0x00FF;
    msg[NM_PROTOCOL_MSG_LEN_INDEX_H]  = (len >> 8) & 0x00FF;
    msg[NM_PROTOCOL_MSG_SENDER_INDEX] = MSG_SENDER_PANNEL;

    if (msg_type == MSG_QUERY_TYPE)
        msg[NM_PROTOCOL_MSG_TYPE_INDEX] = MSG_QUERY_RES_TYPE;
    else if (msg_type == MSG_SET_TYPE)
        msg[NM_PROTOCOL_MSG_TYPE_INDEX] = MSG_SET_RES_TYPE;
    else
        msg[NM_PROTOCOL_MSG_TYPE_INDEX] = MSG_ERR_RES_TYPE;
}

bool allocateBuffer(size_t payloadSize, uint16_t id)
{
    messageLength = payloadSize;
    if (payloadSize > 0) {
        const size_t totalLength = messageHeaderLength + payloadSize + CRC_LEN;
        messageTxBuffer          = new uint8_t[totalLength];
        if (messageTxBuffer == nullptr) {
            return false;
        }
        memset(messageTxBuffer, 0, totalLength);
        return true;
    }
    return false;
}
void btDataProcess(SEMP_PARSE_STATE *parse, uint16_t type)
{
    messageHeader = (SEMP_Bluetooth_HEADER *)parse->buffer;
    messageId     = messageHeader->messageId;
    messageType   = messageHeader->messageType;

    LOG_DEBUG("this is the 0x%02x message", messageId);
    switch (messageId) {
        case NM_PANNEL_INFO_ID: {
            if (allocateBuffer(NM_PROTOCOL_PINFO_MSG_LEN, messageId) == false) {
                LOG_ERROR("allocateBuffer", "Failed to allocate buffer for message 0x%02X", messageId);
                return;
            }
            uint8_t *payload = messageTxBuffer + messageHeaderLength;
            memcpy(payload + NM_PROTOCOL_PHV_OFFSET, HW_VERSION, strlen(HW_VERSION));
            memcpy(payload + NM_PROTOCOL_PFV_OFFSET, SW_VERSION, strlen(SW_VERSION));
            // battery Level
            memcpy(payload + NM_PROTOCOL_PBL_OFFSET, &batteryLevelPercent, 2);
            // battery Temp
            memcpy(payload + NM_PROTOCOL_PBT_OFFSET, &batteryTempC, 2);
            // battery Voltage
            memcpy(payload + NM_PROTOCOL_PBV_OFFSET, &batteryVoltage, 2);

            payload[NM_PROTOCOL_PSS_OFFSET] = DisplayPannelParameter.sound_status;
            break;
        }
        case NM_PANNEL_CTRL_ID: {
            if (allocateBuffer(NM_PROTOCOL_PCTRL_MSG_LEN, messageId) == false) {
                LOG_ERROR("allocateBuffer", "Failed to allocate buffer for message 0x%02X", messageId);
                return;
            }

            uint8_t *payload                = messageTxBuffer + messageHeaderLength;
            payload[NM_PROTOCOL_PRE_OFFSET] = DisplayPannelParameter.reset_flag;
            payload[NM_PROTOCOL_PPC_OFFSET] = DisplayPannelParameter.poweroff_flag;

            if (DisplayPannelParameter.poweroff_flag)
                POWER_OFF_FLAG = 1;
            else
                POWER_OFF_FLAG = 0;
            payload[NM_PROTOCOL_PRC_OFFSET]  = DisplayPannelParameter.record_flag;
            payload[NM_PROTOCOL_PRO_OFFSET]  = DisplayPannelParameter.record_op;
            DisplayPannelParameter.record_op = 0;

            payload[NM_PROTOCOL_PEP_OFFSET] = DisplayPannelParameter.usb_power_flag;
            break;
        }
        case NM_PANNEL_HOST_ID: {
            if (allocateBuffer(NM_PROTOCOL_HOST_MSG_LEN, messageId) == false) {
                LOG_ERROR("allocateBuffer", "Failed to allocate buffer for message 0x%02X", messageId);
                return;
            }

            uint8_t *rxData = parse->buffer + messageHeaderLength;

            uint8_t state = rxData[NM_PROTOCOL_HR1_OFFSET];
            if (!DisplayPannelParameter.record_op) {
                if (state) {
                    DisplayPannelParameter.record_flag = 1;
                    functionKeyLedSwitch(1);
                } else {
                    DisplayPannelParameter.record_flag = 0;
                    functionKeyLedSwitch(0);
                }
            }

            state = rxData[NM_PROTOCOL_HNS_OFFSET];

            if (state)
                functionKeyLedSwitch(1);
            else {
                if (!DisplayPannelParameter.record_flag)
                    functionKeyLedSwitch(0);
                else
                    functionKeyLedSwitch(1);
            }

            uint8_t *payload = messageTxBuffer + messageHeaderLength;
            payload[0]       = 0x01;
            payload[1]       = 0x01;
            break;
        }
        case NM_PANNEL_RST_ID: {
            if (allocateBuffer(NM_PROTOCOL_RST_RESP_MSG_LEN, messageId) == false) {
                LOG_ERROR("allocateBuffer", "Failed to allocate buffer for message 0x%02X", messageId);
                return;
            }
            
        }
        case 0x0D:{
            if (allocateBuffer(NM_PROTOCOL_RST_RESP_MSG_LEN, messageId) == false) {
                LOG_ERROR("allocateBuffer", "Failed to allocate buffer for message 0x%02X", messageId);
                return;
            }
            uint8_t *payload = messageTxBuffer + messageHeaderLength;
            payload[0]       = 0x01;
            payload[1]       = 0x01;
        }
    }
    if (messageTxBuffer != nullptr) {
        fill_message_header(messageTxBuffer, messageId, messageType, messageLength);
        uint32_t crc                                             = calculate_crc(messageTxBuffer, messageHeaderLength + messageLength);
        messageTxBuffer[messageHeaderLength + messageLength]     = crc & 0xFF;
        messageTxBuffer[messageHeaderLength + messageLength + 1] = (crc >> 8) & 0xFF;
        messageTxBuffer[messageHeaderLength + messageLength + 2] = (crc >> 16) & 0xFF;
        messageTxBuffer[messageHeaderLength + messageLength + 3] = (crc >> 24) & 0xFF;

        if (I2C_Slave.write(messageTxBuffer, messageHeaderLength + messageLength + CRC_LEN) > 0) {
            LOG_DEBUG("write %d bytes of 0x%02X message to I2C_Slave success", messageHeaderLength + messageLength + CRC_LEN, messageId);
        } else {
            LOG_ERROR("write message to I2C_Slave failed");
        }
        // 释放原始指针
        delete[] messageTxBuffer;
        messageTxBuffer = nullptr;
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
            rxBytes = I2C_Slave.readBytes(messageRxBuffer, sizeof(messageRxBuffer));
            for (int x = 0; x < rxBytes; x++) {
                sempParseNextByte(Btparse, messageRxBuffer[x]);
            }
        }
        rt_thread_mdelay(1);
    }
    if (DebugTask == true)
        LOG_INFO("Task btReadTask end");
    btReadTaskRunning = false;
}
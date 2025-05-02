#include "Arduino.h"
#include "SparkFun_Extensible_Message_Parser.h"

#include "support.h"
#include "Bluetooth.h"
#include "GlobalDef.h"
#include "States.h"
#include "Radio.h"
#include "log.h"
#include "Power.h"
#include "Message.h"

comconfig ComToUSB = {
    .com_num = 0x01,
    .com_enable = 0x00,
    .crc_type = 0x00,
    .com_baud = settings.gnssToUSBSerialBaud,
    .dataType = 0x01,
    .includedMessageNum = 0,
};
comconfig COM2;
uint8_t BASE_ID[8] = "1234";
// <0：Interval = fabs(v);   >0：Interval = 1/v。
uint8_t GPGGA_Interval = 0;
uint8_t SATSINFO_Interval = 0;

const uint8_t messageHeaderLength = sizeof(SEMP_Bluetooth_HEADER);
SEMP_Bluetooth_HEADER *messageHeader = nullptr;
uint8_t *messageTxBuffer = nullptr;

static int messageLength = 0;
uint8_t messageType = 0x00;
// query for device information
void fill_message_header(uint8_t *msg, uint16_t msg_id, uint8_t msg_type, uint16_t len)
{
    msg[0] = MSG_BluetoothSyncA;
    msg[1] = MSG_BluetoothSyncB;
    msg[2] = MSG_BluetoothSyncC;
    msg[3] = messageHeaderLength;
    msg[MSG_ID_INDEX] = msg_id & 0xFF;
    msg[MSG_ID_INDEX + 1] = (msg_id >> 8) & 0xFF;
    msg[MSG_LEN_INDEX_L] = len & 0xFF;
    msg[MSG_LEN_INDEX_H] = (len >> 8) & 0xFF;
    msg[MSG_SENDER_INDEX] = MSG_SENDER_DEV;

    if (msg_type == MSG_QUERY_TYPE)
        msg[MSG_TYPE_INDEX] = MSG_QUERY_RES_TYPE;
    else if (msg_type == MSG_SET_TYPE)
        msg[MSG_TYPE_INDEX] = MSG_SET_RES_TYPE;
    else
        msg[MSG_TYPE_INDEX] = 0x05;
}

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

bool allocateBuffer(size_t payloadSize, uint16_t id)
{
    messageLength = payloadSize;
    const size_t totalLength = messageHeaderLength + messageLength + CRC_LEN;
    messageTxBuffer = new uint8_t[totalLength];
    if (!messageTxBuffer)
    {
        log_e("aloocateBtBuffer", "Failed to allocate %zu bytes for message 0x%02X", totalLength, id);
        return false;
    }
    memset(messageTxBuffer, 0, totalLength);
    return true;
}

void btDataProcess(SEMP_PARSE_STATE *parse, uint16_t type)
{
    uint16_t messageId = 0x00;
#if 0
    systemPrintf("%s %s, 0x%04x (%d) bytes\r\n", parse->parserName, btParserNames[type], parse->length,
                 parse->length);
    for (int i = 0; i < parse->length; i++)
    {
        systemPrint(parse->buffer[i], HEX);
        systemPrint(" ");
    }
    systemPrintln();
#endif // DEBUG
    if (type == BluetoothRTCMType)
    {
        // systemPrintf("%s %s %d, 0x%04x (%d) bytes\r\n", parse->parserName, btParserNames[type],
        //     sempRtcmGetMessageNumber(parse), parse->length, parse->length);
        gnss->pushRawData(parse->buffer, parse->length);
        return;
    }
    else if (type == BluetoothAPPType)
    {
        messageHeader = (SEMP_Bluetooth_HEADER *)parse->buffer;
        messageId = messageHeader->messageId;
        messageType = messageHeader->messageType;

        switch (messageId)
        {
        case 0x01: // 查询型号版本
        {
            if (!allocateBuffer(136, messageId))
                return;

            uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength;    // 指向载荷起始位置
            memcpy(bluetoothTxBuffer + 0, productDisplayNames[productVariant], 8); // Device Type
            memcpy(bluetoothTxBuffer + 8, settings.deviceSN, 16);                  // Device Serial Number
            memcpy(bluetoothTxBuffer + 24, "V1.1.0", 8);                           // Hardware Version
            memcpy(bluetoothTxBuffer + 32, "V2.1.0", 16);                          // Firmware Version
            memcpy(bluetoothTxBuffer + 56, "UM980", 8);                            // Board Version
            memcpy(bluetoothTxBuffer + 64, gnssUniqueId, 16);                      // Board SN
            memcpy(bluetoothTxBuffer + 80, gnssFirmwareVersion, 16);               // Board Firmware Version
            memcpy(bluetoothTxBuffer + 96, "5002D", 8);                            // Radio Type
            memcpy(bluetoothTxBuffer + 104, settings.radioFirmwareVersion, 16);    // Radio Firmware Version
            break;
        }
        case 0x02: // 查询设备状态
        {
            if (!allocateBuffer(43, messageId))
                return;

            uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置

            bluetoothTxBuffer[0] = 0x03;  // support Work Mode
            bluetoothTxBuffer[1] = 0x01;  // Position or Head
            bluetoothTxBuffer[2] = 0x81;  // Gnss Board status
            bluetoothTxBuffer[3] = 0x83;  // IMU
            bluetoothTxBuffer[4] = 0x81;  // Radio
            bluetoothTxBuffer[5] = 0x00;  // 4G/5G
            bluetoothTxBuffer[9] = 0x81;  // Battery
            bluetoothTxBuffer[10] = 0x81; // WIfi
            bluetoothTxBuffer[11] = 0x81; // Bluetooth
            bluetoothTxBuffer[12] = 0x00; // display
            bluetoothTxBuffer[13] = 0x01; // PPP
            break;
        }
        case 0x03: // 查询电池、存储信息
        {
            if (!allocateBuffer(16, messageId))
                return;

            uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置

            bluetoothTxBuffer[0] = 0x01;                      // 0x00 - external; 0x01 - battery
            bluetoothTxBuffer[2] = (char)batteryLevelPercent; // battery percent

            bluetoothTxBuffer[12] = (char)batteryTempC; // battery temperature
            break;
        }
        case 0x04: // 定位GPGGA
        {
            // int8_t interval = messageHeader->MsgInterval;
            // if (interval > 0)
            //     BTnema[GPGGA] = 1 / interval;
            // else
            //     BTnema[GPGGA] = fabs(interval);

            if (!allocateBuffer(2, messageId))
                return;
            uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
            bluetoothTxBuffer[0] = 0x01;                                        //
            bluetoothTxBuffer[1] = 0x01;                                        //

            break;
        }
        case 0x05: // 可见卫星SATSINFOA
        {
            // int8_t interval = messageHeader->MsgInterval;
            // if (interval > 0)
            //     BTnema[SATSINFOA] = 1 / interval;
            // else
            //     BTnema[SATSINFOA] = fabs(interval);

            if (!allocateBuffer(2, messageId))
                return;
            uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
            bluetoothTxBuffer[0] = 0x01;                                        //
            bluetoothTxBuffer[1] = 0x01;                                        //
            break;
        }
        case 0x06: // 卫星追踪
        {
            if (messageType == MSG_QUERY_TYPE)
            {
                if (!allocateBuffer(12, messageId))
                    return;

                uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
                bluetoothTxBuffer[0] = 0x01;                                        //
                bluetoothTxBuffer[1] = settings.minElev;
                bluetoothTxBuffer[4] = settings.um980Constellations[3]; // GPS
                bluetoothTxBuffer[5] = settings.um980Constellations[2]; // GLONASS
                bluetoothTxBuffer[6] = settings.um980Constellations[0]; // BDS
                bluetoothTxBuffer[7] = settings.um980Constellations[1]; // GALILEO
            }
            else if (messageType == MSG_SET_TYPE)
            {
                // uint8_t *rxData = parse->buffer + messageHeaderLength;

                if (!allocateBuffer(2, messageId))
                    return;

                uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
                bluetoothTxBuffer[0] = 0x01;                                        //
                bluetoothTxBuffer[1] = 0x01;
            }
            break;
        }
        case 0x07: // 工作模式查询
        {
            if (messageType == MSG_QUERY_TYPE)
            {
                if (!allocateBuffer(48, messageId))
                    return;

                uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向数据体起始位置
                if (inBaseMode() == true)
                    bluetoothTxBuffer[0] = 0x01; // base mode
                else
                    bluetoothTxBuffer[0] = 0x00; // work mode

                bluetoothTxBuffer[1] = settings.fixedBase;
                if (settings.BaseEnable == 0x01)
                    bluetoothTxBuffer[2] = 0x01;
                else
                    bluetoothTxBuffer[2] = 0x00;
                memcpy(bluetoothTxBuffer + 4, &BASE_ID, 8);
                if (settings.fixedBase == 0x01)
                {
                    memcpy(bluetoothTxBuffer + 12, &settings.fixedLong, 8);
                    // 经度方向
                    if (strstr(settings.LongType, "E") != NULL)
                        bluetoothTxBuffer[20] = 'E';
                    else
                        bluetoothTxBuffer[20] = 'W';
                    memcpy(bluetoothTxBuffer + 24, &settings.fixedLat, 8);
                    // 纬度方向待定
                    if (strstr(settings.LatType, "N") != NULL)
                        bluetoothTxBuffer[32] = 'N';
                    else
                        bluetoothTxBuffer[32] = 'S';
                    memcpy(bluetoothTxBuffer + 36, &settings.fixedAltitude, 8);
                }
            }
            else if (messageType == MSG_SET_TYPE)
            {
                uint8_t *rxData = parse->buffer + messageHeaderLength;
                settings.fixedBase = rxData[1];
                settings.BaseEnable = rxData[2];
                if (rxData[0] == 0x01 && settings.BaseEnable == 0x01)
                {
                    changeState(STATE_BASE_NOT_STARTED);
                }
                else
                {
                    changeState(STATE_ROVER_NOT_STARTED);
                }

                memcpy(&BASE_ID, rxData + 4, 8);
                if (settings.fixedBase == 0x01)
                {
                    memcpy(&settings.fixedLong, rxData + 12, 8);
                    // 经度方向
                    if (strstr((char *)rxData + 20, "E") != NULL)
                        strcpy(settings.LongType, "E");
                    else
                        strcpy(settings.LongType, "W");
                    memcpy(&settings.fixedLat, rxData + 24, 8);
                    // 纬度方向待定
                    if (strstr((char *)rxData + 32, "N") != NULL)
                        strcpy(settings.LatType, "N");
                    else
                        strcpy(settings.LatType, "S");
                    memcpy(&settings.fixedAltitude, rxData + 36, 8);
                }
                if (!allocateBuffer(2, messageId))
                    return;

                uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
                bluetoothTxBuffer[0] = 0x01;                                        //
                bluetoothTxBuffer[1] = 0x01;
            }
            break;
        }
        case 0x0D: // com口设置
        {
            if (messageType == MSG_QUERY_TYPE)
            {
                static uint8_t msgCount = 0;
                for (int i = 0; i < 9; i++)
                {
                    if (ComToUSB.messageList[i].enable == 1)
                        msgCount++;
                }
                if (!allocateBuffer(28 + msgCount * 4, messageId))
                    return;
                uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
                bluetoothTxBuffer[0] = ComToUSB.com_num;                            // com number
                bluetoothTxBuffer[1] = ComToUSB.com_enable;                         // 0x00-off; 0x01-on
                bluetoothTxBuffer[2] = ComToUSB.crc_type;                           // 0x00-无校验; 0x01-奇校验; 0x02-偶校验
                memcpy(bluetoothTxBuffer + 4, &ComToUSB.com_baud, 4);               // 波特率
                bluetoothTxBuffer[8] = 0x01;                                        // 0x01-导航定位数据; 0x02-原始观测数据；0x03-RTCM23；
                // 0x04-RTCM30; 0x05-RTCM32; 0x06-CMR; 0x10-自定义; 0xFF-固定输出不可配置
                bluetoothTxBuffer[27] = msgCount;

                if (ComToUSB.messageList[GPGGA].enable == 1)
                {
                    memcpy(bluetoothTxBuffer + 28, &ComToUSB.messageList[GPGGA].id, 2);
                    bluetoothTxBuffer[30] = ComToUSB.messageList[GPGGA].enable;   // 0x00-off; 0x01-on
                    bluetoothTxBuffer[31] = ComToUSB.messageList[GPGGA].interval; //<0-Interval = fabs(v);   >0-Interval = 1/v
                }
                if (ComToUSB.messageList[GPRMC].enable == 1)
                {
                    memcpy(bluetoothTxBuffer + 32, &ComToUSB.messageList[GPRMC].id, 2);
                    bluetoothTxBuffer[34] = ComToUSB.messageList[GPRMC].enable;   // 0x00-off; 0x01-on
                    bluetoothTxBuffer[35] = ComToUSB.messageList[GPRMC].interval; //<0-Interval = fabs(v);   >0-Interval = 1/v
                }
                if (ComToUSB.messageList[GPGSV].enable == 1)
                {
                    memcpy(bluetoothTxBuffer + 36, &ComToUSB.messageList[GPGSV].id, 2);
                    bluetoothTxBuffer[38] = ComToUSB.messageList[GPGSV].enable;   // 0x00-off; 0x01-on
                    bluetoothTxBuffer[39] = ComToUSB.messageList[GPGSV].interval; //<0-Interval = fabs(v);   >0-Interval = 1/v
                }
                if (ComToUSB.messageList[GPGST].enable == 1)
                {
                    memcpy(bluetoothTxBuffer + 40, &ComToUSB.messageList[GPGST].id, 2);
                    bluetoothTxBuffer[42] = ComToUSB.messageList[GPGST].enable;   // 0x00-off; 0x01-on
                    bluetoothTxBuffer[43] = ComToUSB.messageList[GPGST].interval; //<0-Interval = fabs(v);   >0-Interval = 1/v
                }
                if (ComToUSB.messageList[GPVTG].enable == 1)
                {
                    memcpy(bluetoothTxBuffer + 44, &ComToUSB.messageList[GPVTG].id, 2);
                    bluetoothTxBuffer[46] = ComToUSB.messageList[GPVTG].enable;   // 0x00-off; 0x01-on
                    bluetoothTxBuffer[47] = ComToUSB.messageList[GPVTG].interval; //<0-Interval = fabs(v);   >0-Interval = 1/v
                }
                if (ComToUSB.messageList[HEADING].enable == 1)
                {
                    memcpy(bluetoothTxBuffer + 48, &ComToUSB.messageList[HEADING].id, 2);
                    bluetoothTxBuffer[50] = ComToUSB.messageList[HEADING].enable;   // 0x00-off; 0x01-on
                    bluetoothTxBuffer[51] = ComToUSB.messageList[HEADING].interval; //<0-Interval = fabs(v);   >0-Interval = 1/v
                }
            }
            else if (messageType == MSG_SET_TYPE)
            {
                // uint8_t *rxData = parse->buffer + messageHeaderLength;
                for (int i = messageHeaderLength; i < parse->length; i++)
                {
                    systemPrint(parse->buffer[i], HEX);
                    systemPrint(" ");
                }
                systemPrintln();
                // memcpy(&settings.gnssToUSBSerialBaud, rxData + 4, 4);
                // memcpy(&messageList[GPGGA].interval, rxData + 28, 2);

                // if (!allocateBuffer(2))
                //     return;

                // uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
                // bluetoothTxBuffer[0] = 0x01;                                        //
                // bluetoothTxBuffer[1] = 0x01;
            }
            break;
        }
        case 0x14: // 电台配置
        {
            if (messageType == MSG_QUERY_TYPE)
            {
                if (!allocateBuffer(28, messageId))
                    return;

                uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
                bluetoothTxBuffer[0] = settings.radioNumber;                        // radio number
                bluetoothTxBuffer[1] = settings.enableLora;                         // rado enable
                if (inBaseMode() == true)                                           // work mode
                    bluetoothTxBuffer[2] = 0x00;
                else
                    bluetoothTxBuffer[2] = 0x01;

                bluetoothTxBuffer[3] = 0x00; // channel

                memcpy(bluetoothTxBuffer + 4, &settings.radioRxFreqMHz, 4);
                memcpy(bluetoothTxBuffer + 8, &settings.radioTxFreqMHz, 4);
                bluetoothTxBuffer[12] = settings.radioPower;
                bluetoothTxBuffer[13] = settings.radioProtocol;
                bluetoothTxBuffer[14] = settings.radioAirBaud;

                bluetoothTxBuffer[17] = 0x05; // Data type：0x03-RTCM23数据，0x04-RTCM30数据，0x05-RTCM32数据，0x06-CMR数据。
            }
            else if (messageType == MSG_SET_TYPE)
            {
                uint8_t *rxData = parse->buffer + messageHeaderLength;
                settings.radioNumber = rxData[0];
                settings.enableLora = rxData[1];
                memcpy(&settings.radioRxFreqMHz, rxData + 4, 4);
                memcpy(&settings.radioTxFreqMHz, rxData + 8, 4);
                settings.radioPower = rxData[12];
                settings.radioProtocol = rxData[13];
                settings.radioAirBaud = rxData[14];
                if (rxData[2] == 0x00)
                {
                    systemState = STATE_BASE_NOT_STARTED;
                    loraState = LORA_NOT_STARTED;
                }

                else if (rxData[2] == 0x01)
                {
                    systemState = STATE_ROVER_NO_FIX;
                    loraState = LORA_NOT_STARTED;
                }
                if (!allocateBuffer(2, messageId))
                    return;

                uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
                bluetoothTxBuffer[0] = 0x01;                                        //
                bluetoothTxBuffer[1] = 0x01;
            }
            break;
        }
        case 0x15: // 静态存储配置
        {
            if (messageType == MSG_QUERY_TYPE)
            {
                if (!allocateBuffer(50, messageId))
                    return;

                uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
                bluetoothTxBuffer[0] = 0x01;
                bluetoothTxBuffer[1] = logConfig.logState;
                bluetoothTxBuffer[2] = logConfig.logMode;
                bluetoothTxBuffer[3] = logConfig.logEnable;
                bluetoothTxBuffer[4] = logConfig.logFileType; // 0x00-XYZ; 0x01-Rinex2.10; 0x02-Rinex3.02; 0x03-Rinex3.04;
                bluetoothTxBuffer[5] = logConfig.logLoop;     // 循环存储 00-不循环 01-循环
                memcpy(bluetoothTxBuffer + 8, logConfig.logFileName, 16);
                bluetoothTxBuffer[26] = logConfig.logRecordSpace;
                bluetoothTxBuffer[28] = logConfig.sampleInterval;  //<0-Interval = fabs(v);   >0-Interval = 1/v
                bluetoothTxBuffer[30] = logConfig.logIntervalMode; // 默认0
                bluetoothTxBuffer[31] = logConfig.logFileInterval; // 观测时间
                memcpy(bluetoothTxBuffer + 38, logConfig.logPointName, 12);
            }
            else if (messageType == MSG_SET_TYPE)
            {

                uint8_t *rxData = parse->buffer + messageHeaderLength;
                logConfig.logRecordSpace = rxData[27] << 8 | rxData[26];
                // getfreeSdSpace
                if (logConfig.logRecordSpace < sdFreeSpace_MB)
                {
                    logConfig.logState = rxData[3];
                    logConfig.logMode = rxData[2];
                    logConfig.logEnable = rxData[3];
                    logConfig.logFileType = rxData[4];
                    logConfig.logLoop = rxData[5];
                    memcpy(logConfig.logFileName, rxData + 8, 16);

                    logConfig.sampleInterval = rxData[28];
                    logConfig.logIntervalMode = 0;
                    if (rxData[31] == 0x00) // 0x00-15分钟; 0x01-1小时; 0x02-2小时; 0x04-4小时;0x18-24小时
                    {
                        logConfig.logFileInterval = 0;
                        logConfig.logFileIntervalMinutes = 15;
                    }
                    else if (rxData[31] == 0x01)
                    {
                        logConfig.logFileInterval = 1;
                        logConfig.logFileIntervalMinutes = 60;
                    }
                    else if (rxData[31] == 0x02)
                    {
                        logConfig.logFileInterval = 0x02;
                        logConfig.logFileIntervalMinutes = 120;
                    }
                    else if (rxData[31] == 0x04)
                    {
                        logConfig.logFileInterval = 0x04;
                        logConfig.logFileIntervalMinutes = 240;
                    }
                    else if (rxData[31] == 0x18)
                    {
                        logConfig.logFileInterval = 0x18;
                        logConfig.logFileIntervalMinutes = 1440;
                    }
                    memcpy(logConfig.logPointName, rxData + 38, 12);
                    // record settings to nvs----wait do
                    if (!allocateBuffer(2, messageId))
                        return;

                    uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
                    bluetoothTxBuffer[0] = 0x01;                                        //
                    bluetoothTxBuffer[1] = 0x01;
                }
                else
                {
                    log_e("setLogConfig", "Not enough space on SD card");
                    if (!allocateBuffer(2, messageId))
                        return;

                    uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
                    bluetoothTxBuffer[0] = 0x02;                                        //
                    bluetoothTxBuffer[1] = 0x02;
                }
            }
            break;
        }
        // case 0x1E: // 主机控制
        // {
        //     break;
        // }
        case 0x21: // GPGST *****
        {
            // int8_t interval = messageHeader->MsgInterval;
            // if (interval > 0)
            //     BTnema[GPGST] = 1 / interval;
            // else
            //     BTnema[GPGST] = fabs(interval);

            if (!allocateBuffer(2, messageId))
                return;
            uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
            bluetoothTxBuffer[0] = 0x01;                                        //
            bluetoothTxBuffer[1] = 0x01;
            break;
        }
        case 0x22: // GPRMC *****
        {
            // int8_t interval = messageHeader->MsgInterval;
            // if (interval > 0)
            //     BTnema[GPRMC] = 1 / interval;
            // else
            //     BTnema[GPRMC] = fabs(interval);

            if (!allocateBuffer(2, messageId))
                return;
            uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
            bluetoothTxBuffer[0] = 0x01;                                        //
            bluetoothTxBuffer[1] = 0x01;
            break;
        }
        case 0x23: // GPGSA*****
        {
            // int8_t interval = messageHeader->MsgInterval;
            // if (interval > 0)
            //     BTnema[GPGSA] = 1 / interval;
            // else
            //     BTnema[GPGSA] = fabs(interval);

            if (!allocateBuffer(2, messageId))
                return;
            uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
            bluetoothTxBuffer[0] = 0x01;                                        //
            bluetoothTxBuffer[1] = 0x01;
            break;
        }
        // case 0x24: // IMU测量设置天线高 ***** 0x02
        // {
        //     break;
        // }
        // case 0x25: // IMU数据，获取倾斜测量模块NAVI惯性定位数据，GPFMI *****
        // {
        //     break;
        // }
        // case 0x26: // 对中杆校准开关控制
        // {
        //     break;
        // }
        case 0x27: // 手簿网络设置
        {
            if (messageType == MSG_QUERY_TYPE)
            {
                if (!allocateBuffer(4, messageId))
                    return;
                uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
                bluetoothTxBuffer[0] = 0x01;                                        //
                bluetoothTxBuffer[1] = 0x01;
            }
            else if (messageType == MSG_SET_TYPE)
            {
                if (!allocateBuffer(2, messageId))
                    return;
                uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
                bluetoothTxBuffer[0] = 0x01;                                        //
                bluetoothTxBuffer[1] = 0x01;
            }
            break;
        }
        case 0x28: // WIFI控制 *****
        {
            if (messageType == MSG_SET_TYPE)
            {
                uint8_t *rxData = parse->buffer + messageHeaderLength;
                char *wifiDomain = nullptr;
                memcpy(wifiDomain, rxData + 1, 4);
                if (rxData[0] == 0x01)
                    log_i("open wifi in %d", wifiDomain);
            }
            break;
        }
        case 0x29: // BASEINFO *****
        {
            // int8_t interval = messageHeader->MsgInterval;
            // if (interval > 0)
            //     BTnema[BASEINFO] = 1 / interval;
            // else
            //     BTnema[BASEINFO] = fabs(interval);

            if (!allocateBuffer(2, messageId))
                return;
            uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
            bluetoothTxBuffer[0] = 0x01;                                        //
            bluetoothTxBuffer[1] = 0x01;
            break;
        }
        // case 0x30: // 接收机注册 *****
        // {
        //     break;
        // }
        case 0x31: // 接收机关机控制
        {
            if (messageType == MSG_QUERY_TYPE)
            {
                if (!allocateBuffer(4, messageId))
                    return;
                uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
                bluetoothTxBuffer[0] = 0x01;                                        //
                bluetoothTxBuffer[1] = 0x01;
            }
            else if (messageType == MSG_SET_TYPE)
            {
                uint8_t *rxData = parse->buffer + messageHeaderLength;
                if (rxData[0] == 0x01)
                {
                    performShutDown();
                }
                if (!allocateBuffer(2, messageId))
                    return;
                uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
                bluetoothTxBuffer[0] = 0x01;                                        //
                bluetoothTxBuffer[1] = 0x01;
            }
            break;
        }
        case 0x32: // 数据记录设置天线高
        {
            if (messageType == MSG_QUERY_TYPE)
            {
                if (!allocateBuffer(12, messageId))
                    return;
                uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
                memcpy(bluetoothTxBuffer, &settings.antennaHeight_m, 4);
                bluetoothTxBuffer[4] = settings.antennaMeasureType;
            }
            else if (messageType == MSG_SET_TYPE)
            {
                uint8_t *rxData = parse->buffer + messageHeaderLength;
                memcpy(&settings.antennaHeight_m, rxData, 4);
                settings.antennaMeasureType = rxData[4];
                if (!allocateBuffer(2, messageId))
                    return;
                uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
                bluetoothTxBuffer[0] = 0x01;                                        //
                bluetoothTxBuffer[1] = 0x01;
            }
            break;
        }
        case 0x33: // PPP控制 *****
        {
            if (messageType == MSG_QUERY_TYPE)
            {
                if (!allocateBuffer(4, messageId))
                    return;
                uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
                bluetoothTxBuffer[0] = uint8_t(settings.enableGalileoHas);          //
            }
            else if (messageType == MSG_SET_TYPE)
            {
                uint8_t *rxData = parse->buffer + messageHeaderLength;
                if (rxData[0] == 0x01)
                {
                    if (gnss->setHighAccuracyService(true))
                        settings.enableGalileoHas = true;
                }
                else if (rxData[0] == 0x00)
                {

                    if (gnss->setHighAccuracyService(false))
                        settings.enableGalileoHas = false;
                }

                if (!allocateBuffer(2, messageId))
                    return;
                uint8_t *bluetoothTxBuffer = messageTxBuffer + messageHeaderLength; // 指向载荷起始位置
                bluetoothTxBuffer[0] = 0x01;                                        //
                bluetoothTxBuffer[1] = 0x01;
            }
            break;
        }
            // case 0x34: // 外置电台控制 *****
            // {
            //     break;
            // }

        default:
        {
            messageTxBuffer = nullptr;
            // for (int i = 0; i < parse->length; i++)
            // {
            //     systemPrint(parse->buffer[i], HEX);
            //     systemPrint(" ");
            // }
            systemPrintf("Unknown Msgid:0x%02x, type :0x%02x\r\n", messageId, messageType);
            break;
        }
        }
        // case ⬆
        if (messageTxBuffer != nullptr)
        {
            fill_message_header(messageTxBuffer, messageHeader->messageId,
                                messageHeader->messageType, messageLength);

            // 计算CRC
            uint32_t crc = calculate_crc(messageTxBuffer, messageHeaderLength + messageLength);

            // 写入CRC字段
            uint8_t *crc_ptr = messageTxBuffer + messageHeaderLength + messageLength;
            crc_ptr[0] = crc & 0xFF;
            crc_ptr[1] = (crc >> 8) & 0xFF;
            crc_ptr[2] = (crc >> 16) & 0xFF;
            crc_ptr[3] = (crc >> 24) & 0xFF;

            // 发送完整数据
            int sendBytes = bluetoothWrite(messageTxBuffer, messageHeaderLength + messageLength + CRC_LEN);
#if 0
            if (sendBytes <= 0)
            {
                log_e("bluetooth", "bluetoothWrite error");
            }
            else
            {
                log_i("bluetooth", "Msgid:0x%02x msgType:0x%02x send %d bytes", messageId, messageType, sendBytes);
            }
#endif
        }
        messageHeader = nullptr;
        // 释放原始指针
        if (messageTxBuffer != nullptr)
        {
            delete[] messageTxBuffer;
            messageTxBuffer = nullptr;
        }
    }
}
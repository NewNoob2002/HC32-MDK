#pragma once

#define BluetoothAPPType 0
#define BluetoothRTCMType 1

#define MSG_SENDER_APP 0x00
#define MSG_SENDER_DEV 0x01
#define MSG_SENDER_PANNEL 0x02

#define MSG_QUERY_TYPE 0x00
#define MSG_QUERY_RES_TYPE 0x01
#define MSG_SET_TYPE 0x02
#define MSG_SET_RES_TYPE 0x03
#define MSG_ERR_REP_TYPE 0x05

#define MSG_BluetoothSyncA 0xAA
#define MSG_BluetoothSyncB 0x44
#define MSG_BluetoothSyncC 0x18

#define MSG_ID_INDEX 4 // MSG_ID位置

#define MSG_UPLOAD_COUNT_L 6
#define MSG_UPLOAD_COUNT_H 7

#define MSG_LEN_INDEX_L 12
#define MSG_LEN_INDEX_H 13

#define MSG_UPLOAD_ID_L 14
#define MSG_UPLOAD_ID_H 15

#define MSG_SENDER_INDEX 16
#define MSG_TYPE_INDEX 17
#define MSG_INTERVAL_INDEX 19
#define CRC_LEN 4 // CRC长度
// typedef
enum msg_type
{
    GPGGA = 0,
    GPRMC,
    GPGSV,
    GPGST,
    GPVTG,
    HEADING,
    GPINS,
    GPFPD,
    INSPVAXA
};
typedef struct _message
{
    uint16_t id;
    uint8_t enable;
    uint8_t interval;
} message;

typedef struct _comconfig
{
    uint8_t com_num;
    uint8_t com_enable;
    uint8_t crc_type;
    uint32_t com_baud;
    uint8_t dataType;
    uint8_t includedMessageNum;

    message messageList[9] = {
    {.id = 218, .enable = 0, .interval = 0}, // GPGGA	218
    {.id = 225, .enable = 0, .interval = 0}, // GPRMC	225
    {.id = 223, .enable = 0, .interval = 0}, // GPGSV	223
    {.id = 222, .enable = 0, .interval = 0}, // GPGST	222
    {.id = 226, .enable = 0, .interval = 0}, // GPVTG	226
    {.id = 971, .enable = 0, .interval = 0}, // HEADING	971
    {.id = 972, .enable = 0, .interval = 0}, // GPINS	972
    {.id = 973, .enable = 0, .interval = 0}, // GPFPD	973
    {.id = 974, .enable = 0, .interval = 0}, // INSPVAXA	974
};
}comconfig;

void fill_message_header(uint8_t *msg, uint16_t msg_id, uint8_t msg_type, uint16_t len);
uint32_t calculate_crc(uint8_t *msg, uint16_t len);
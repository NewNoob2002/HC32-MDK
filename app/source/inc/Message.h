#pragma once
#include <stdint.h>

#define MSG_SENDER_APP 0x00
#define MSG_SENDER_DEV 0x01
#define MSG_SENDER_PANNEL 0x03

#define MSG_QUERY_TYPE 0x00
#define MSG_QUERY_RES_TYPE 0x01
#define MSG_SET_TYPE 0x02
#define MSG_SET_RES_TYPE 0x03
#define MSG_ERR_RES_TYPE 0x05

#define MSG_BluetoothSyncA 0xAA
#define MSG_BluetoothSyncB 0x44
#define MSG_BluetoothSyncC 0x18

#define NM_PROTOCOL_MSG_ID_INDEX_L   4
#define NM_PROTOCOL_MSG_ID_INDEX_H   5
#define NM_PROTOCOL_MSG_LEN_INDEX_L       12
#define NM_PROTOCOL_MSG_LEN_INDEX_H       13
#define NM_PROTOCOL_MSG_SENDER_INDEX      16
#define NM_PROTOCOL_MSG_TYPE_INDEX        17
#define NM_PROTOCOL_MSG_INTERVAL_INDEX    19

#define NM_MSG_QUERY_TYPE            0x00
#define NM_MSG_QUERY_RES_TYPE        0x01
#define NM_MSG_SET_TYPE              0x02
#define NM_MSG_SET_RES_TYPE          0x03
#define NM_MSG_ERR_REP_TYPE          0x05

#define NM_MSG_CRC_ERR_REP_ID        0x01
#define NM_MSG_MSG_ID_ERR_REP_ID     0x02
#define NM_MSG_MODULE_ID_ERR_REP_ID  0x03

#define CRC_LEN 4 // CRC长度
//具体协议
#define NM_PANNEL_INFO_ID                 0x01
#define NM_PROTOCOL_PINFO_MSG_LEN         36///32

#define NM_PROTOCOL_PHV_OFFSET            (0)
#define NM_PROTOCOL_PFV_OFFSET            (8)
#define NM_PROTOCOL_PBR_OFFSET            (16)
#define NM_PROTOCOL_PBL_OFFSET            (18)
#define NM_PROTOCOL_PBT_OFFSET            (20)
#define NM_PROTOCOL_PBV_OFFSET            (22)
#define NM_PROTOCOL_PSS_OFFSET            (26)


#define NM_PANNEL_CTRL_ID                 0x02
#define NM_PROTOCOL_PCTRL_MSG_LEN         8
#define NM_PROTOCOL_PRE_OFFSET            (0)
#define NM_PROTOCOL_PPC_OFFSET            (1)
#define NM_PROTOCOL_PRC_OFFSET            (2)
#define NM_PROTOCOL_PRO_OFFSET            (3)
#define NM_PROTOCOL_PEP_OFFSET            (4)

#define NM_PANNEL_HOST_ID                 0x03
#define NM_PROTOCOL_HOST_MSG_LEN          2
#define NM_PROTOCOL_HR1_OFFSET            (0)
#define NM_PROTOCOL_HR2_OFFSET            (1)
#define NM_PROTOCOL_HNS_OFFSET            (4)

#define NM_PANNEL_RST_ID                       0x04
#define NM_PROTOCOL_RST_RESP_MSG_LEN           2
#define NM_PROTOCOL_RRST_OFFSET                (0)

#define NM_PANNEL_POWER_ID                     0x05
#define NM_PROTOCOL_POWER_RESP_MSG_LEN         2

void fill_message_header(uint8_t *msg, uint16_t msg_id, uint8_t msg_type, uint16_t len);
uint32_t calculate_crc(uint8_t *msg, uint16_t len);
#ifndef __YC_BT_H__
#define __YC_BT_H__
#include <stdint.h>
#include "le.h"
#define ADV_MAX_LEN                 (32)  //BLE广播数据最大长度
/**
 * @brief ble adv type enum
 *
 */
typedef enum
{
    ADV_IND = 0,
    ADV_DIRECT_IND,
    ADV_NONCONN_IND,
    ADV_SCAN_IND = 6,
    ADV_EXT_IND,
    AUX_ADV_IND = 7,
    AUX_SYNC_IND = 7,
    AUX_CHAIN_IND = 7,
    AUX_SYNC_SUBEVENT_IND = 7,
    ADV_DECISION_IND = 9,
}BLE_AdvTypeEnum;
void BT_BleSetMac(uint8_t * bleAddr);
void BT_BleSetName(uint8_t* bleName, uint8_t len);
void BT_BleSetAdvParams(BLE_AdvTypeEnum type, uint8_t *advData, uint8_t len);
void BT_BleSetScanData(uint8_t* scanData, int len);
void BT_BleSetAdvInterval(uint32_t interval);
void BT_SetVisibility(uint8_t discoverable);
void BT_SetScanEnable(uint8_t discoverable, uint32_t scan_interval, uint16_t scan_window);
void BT_SetBleConnAddr(uint8_t *addr);
void BT_SetATTService(att_record* att_list,uint8_t att_size);
void BT_BleConnectParmUpdateReq(uint8_t *data,uint8_t len);
void BT_BleUpdateMtu(uint16_t mtu);
void BT_BleSetMtu(uint16_t mtu);

_Bool BT_BleSlaveSendData(uint16_t handle, uint8_t * bleData, uint16_t len);
_Bool BT_BleSendReadRSP(uint16_t handle, uint8_t * bleData, uint16_t len);
_Bool BT_BleSendReadByTypeRSP(uint16_t handle, uint8_t * bleData, uint16_t len);
_Bool BT_BleMasterSendData(uint16_t handle, uint8_t *bleData, uint16_t len);
#endif
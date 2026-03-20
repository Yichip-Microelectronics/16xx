#ifndef __YC_USER_H__
#define __YC_USER_H__
#include <stdint.h>
#include "yc_bt.h"
extern uint8_t gen_ble_adv_normal[ADV_MAX_LEN];
extern uint8_t gen_ble_scan_rsp[ADV_MAX_LEN];
extern uint16_t yc_rx_handle,yc_tx_handle;
void U_BleSetDeviceName(char *name_in);
void U_BleGenAdvScanData(uint8_t *adv_data,uint8_t *scan_rsp,char *name,uint8_t name_len,uint8_t *mac);
void U_BleParseWriteData(uint16_t len,uint8_t *dataPtr);
void U_BleParseReadData(uint16_t len,uint8_t *dataPtr);
void U_BleParseReadByTypeData(uint16_t len, uint8_t *dataPtr);
void U_BleParsePrepareWriteData(uint16_t len,uint8_t *dataPtr);
void U_BleParseExecuteWriteData(uint16_t len,uint8_t *dataPtr);
void U_BleParseScanAdvData(uint8_t *p);
void U_BleParseNotifyData(uint16_t len,uint8_t *dataPtr);
#endif // !__YC_USER_H__
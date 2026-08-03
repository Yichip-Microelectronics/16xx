#ifndef __YC_BT_H__
#define __YC_BT_H__
#include <stdint.h>
#include "le.h"
#define ADV_MAX_LEN                 (32)  //BLE广播数据，包含一个字节的长度。


#define RSSI_WINDOW_SIZE        (16)   //计算真实rssi的窗口大小,取5-16
#define NON_RESOLV_ADDR_TYPE    (0x00)
#define RESOLV_ADDR_TYPE        (0x40)
#define RESERVED_ADDR_TYPE      (0x80)
#define STATIC_ADDR_TYPE        (0xc0)


#define NVRAM_FLASH_ADDR    (0x3F000)     //last 4KB of 2M-bitflash

#define NVRAM_SIZE                  (34)
#define NVRAM_NUM                   (5)
#define NVRAM_LEN                   (NVRAM_SIZE*NVRAM_NUM)
typedef struct
{
    uint8_t num;
    uint8_t type;
    union
    {
        uint8_t addr[6];
        uint8_t irk[16];
        uint8_t ediv_rand[16];
    };
    uint8_t key[16];
}NvramItem;

extern NvramItem NvramData[NVRAM_NUM];
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

typedef enum
{
    ADDR_PUBLIC = 0,
    ADDR_RANDOM = 1,
}BLE_AdvAddrType;

typedef enum
{
    PAIR_NONE          = 0x00,
    PAIR_JUST_WORK     = 0x01,
    PAIR_PASSKEY       = 0x02,
    PAIR_PASSKEY_INPUT = 0x03,
    PAIR_SC_JUST_WORK  = 0x81,
    PAIR_SC_PASSKEY    = 0x83,
    PAIR_SC_NUMERIC    = 0x82
}BLE_PairModeEnum;


uint16_t BT_GetLibVersion(void);
void BT_BleSetMac(uint8_t * bleAddr);
void BT_BleSetName(uint8_t* bleName, uint8_t len);
void BT_BleSetAdvParams(BLE_AdvTypeEnum type,BLE_AdvAddrType own_addr_type, BLE_AdvAddrType peer_addr_type, uint8_t *advData, uint8_t len);
void BT_BleSetScanData(uint8_t* scanData, int len);
void BT_BleSetAdvInterval(uint32_t interval);
void BT_SetVisibility(uint8_t discoverable);
void BT_SetScanEnable(uint8_t discoverable, uint32_t scan_interval, uint16_t scan_window);
void BT_SetBleConnAddr(uint8_t *addr);
void BT_BleConnectParmUpdateReq(uint8_t *data,uint8_t len);
void BT_BleUpdateMtu(void);
void BT_BleSetMtu(uint16_t mtu);
void BT_BleSetPairMode(BLE_PairModeEnum mode);
_Bool BT_BleSlaveSendData(dev_handle_t dhandle,uint16_t handle, uint8_t * bleData, uint16_t len);
_Bool BT_BleMasterSendData(dev_handle_t dhandle,uint16_t handle, uint8_t *bleData, uint16_t len);
void BT_SetTXPower(char db);
void BT_SetLLParam(uint8_t *p);
uint8_t BT_NvramFindFromList(void);
void BT_NvramLoad(void);
void BT_BleNvramSave(void);
uint8_t BT_NvramUpdateToFlash(void);
void BT_BleDisconnectIND(uint8_t reason);
int8_t BT_GetRssi(void);
#endif
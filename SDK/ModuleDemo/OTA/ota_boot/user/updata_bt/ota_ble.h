#ifndef _OTA_BLE_H
#define _OTA_BLE_H

#include "yc_ipc.h"
#include "yc_qspi.h"
#include "ota_config.h"
#include "board_config.h"
#include "flash.h"
#include "le.h"
#define	OTA_UART_RX_BUFFER_SIZE                 	0x1200
#define	OTA_BUCK_SIZE                           	0x1004
#define	OTA_COMMAND                             	0XBA
#define	OTA_PORTOCOL_VERSION_REQUEST            	0x10
#define	OTA_BUCK_SIZE_REQUEST                   	0x11
#define	OTA_WORK_MODE_REQUEST                   	0x12
#define	OTA_SWITCH_WORK_MODE_REQUEST            	0x13
#define	OTA_FLASH_CHECKSUM_REQUEST              	0x14
#define	OTA_START_REQUEST                       	0x15
#define	OTA_DATA_WRITE_CMD                      	0x16
#define	OTA_DATA_WRITE_REQUEST                  	0x17
#define	OTA_END_REQUEST                         	0x18
#define	OTA_ENTER_OTA_REQ                       	0x80
#define	OTA_EXIT_OTA_REQ                        	0x81
#define	OTA_UPDATE_FLASH_NORMAL                 	0x00
#define	OTA_UPDATE_FLASH_PCM                    	0x02
#define	OTA_UPDATE_FLASH_OTA                    	0x01
typedef enum
{
    PORROOL_VERSION_REQUEST     =   OTA_PORTOCOL_VERSION_REQUEST,
    BUCK_SIZE_REQUEST           =   OTA_BUCK_SIZE_REQUEST,
    START_REQUEST               =   OTA_START_REQUEST,
    DATA_WRITE_CMD              =   OTA_DATA_WRITE_CMD,
    DATA_WRITE_REQUEST          =   OTA_DATA_WRITE_REQUEST,
    END_REQUEST                 =   OTA_END_REQUEST,
}BLE_OTA_CMD;


#define RESULTTRUE                  0x00
#define RESULTERROR                 0x01
#define EVTRESPOND                  0xAB
#define RECDATAPACKCMD              0xBA

#define NORMALOTA                   0x00

#define BLE_OTA_DATA_LEN            128

#define PACKET_MAXLEN               0x85  //MAX = 150
#define BUCK_LEN                    0x1000
#pragma pack(1)
typedef struct
{
    uint8_t Evt;
    uint8_t ResultCode;
    uint8_t RcvFlag;
    uint8_t Payload[32];
}BleDataPackStr;


typedef struct
{
    uint8_t Evt;
    uint8_t ResultCode;
    uint8_t RcvFlag;
    uint8_t updateFlashMode;
    uint32_t length;
    uint32_t checkSum;
    uint8_t Payload[32];
}BleEndDataPack;
#pragma pack()

uint8_t cmd_para_check(BLE_OTA_CMD check_cmd);
void Ble_Cmd_Check(dev_handle_t dhandle,uint8_t* bledata);


#endif


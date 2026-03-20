#include "yc_bt.h"
#include <stdint.h>
#include "le.h"
#include "BLE_L2capATT.h"
#include "BLE_L2capSIgn.h"
#include "config.h"
#include "yc_user.h"
#include "stdlib.h"
#include "BLE_LL_Scan.h"
/*********************************************
 * FunctionName : BT_BleSetMac
 * Description  : 设置 BLE MAC 地址
 * Parameters   : bleAddr :6个字节MAC地址
 * Returns      : none
**********************************************/
void BT_BleSetMac(uint8_t * bleAddr)
{
    for(int i=0;i<6;i++){
        adv_param.bdaddr[i] = bleAddr[i];
        conn.own_addr[i] = bleAddr[i];
        mconn.own_addr[i] = bleAddr[i];
    }
}

/*********************************************
 * FunctionName : BT_BleSetName
 * Description  : 设置 BLE 名称
 * Parameters   : bleName : 蓝牙4.0名称，不能包含'\0', len :名称长度小于16字节
 * Returns      : none
**********************************************/
void BT_BleSetName(uint8_t* bleName, uint8_t len)
{
    if(len > 16) len = 16; //名称长度不能超过16字节
    adv_param.name_len = len;
    for(int i=0;i<len;i++){
        adv_param.name[i] = bleName[i];
    }
}

/*********************************************
 * FunctionName : BT_BleSetAdvParams
 * Description  : 设置Ble广播数据
 * Parameters   : type: 广播类型
 *                advData: 广播数据或者回连地址
 *                len: 必须<=0x20
 * Returns      : none
**********************************************/
void BT_BleSetAdvParams(BLE_AdvTypeEnum type, uint8_t *advData, uint8_t len)
{
    uint8_t adv_len;
    adv_len = (len > ADV_MAX_LEN ? ADV_MAX_LEN : len);
    adv_param.adv_type = (uint8_t)type;
    for(uint8_t i=0; i<adv_len; i++)
    {
        adv_param.adv_data[i] = advData[i];
    }
}

/*********************************************
 * FunctionName : BT_BleSetScanData
 * Description  : 设置 BLE 扫描包数据
 * Parameters   : scanData : 扫描数据指针, len :数据长度小于等于32字节
 * Returns      : none
**********************************************/
void BT_BleSetScanData(uint8_t* scanData, int len)
{
    uint8_t scan_len;
    scan_len = (len > ADV_MAX_LEN ? ADV_MAX_LEN : len);
    for(uint8_t i=0; i<scan_len; i++)
    {
        adv_param.scan_rsp_data[i] = scanData[i];
    }
}


/*********************************************
 * FunctionName : BT_BleSetAdvInterval
 * Description  : 设置ble广播间隔
 * Parameters   : 设置间隔,单位us。
 * Returns      : none
**********************************************/
void BT_BleSetAdvInterval(uint32_t interval)
{
    adv_param.interval.adv_interval = 300;
    adv_param.evt_interval.adv_event_interval = interval;
}

/*********************************************
 * FunctionName : BT_SetVisibility
 * Description  : BLE 可发现(开广播)
 * Parameters   : discoverable : ENABLE or DISABLE
 * Returns      : none
**********************************************/
void BT_SetVisibility(uint8_t discoverable)
{
    if(discoverable)
    {
        adv_param.adv_channel_map = 7; //三个信道都广播
        init_le_adv();
    }
    else
    {
        adv_param.adv_channel_map = 0; //不广播
    }
}


/*********************************************
 * FunctionName : BT_SetScanEnable
 * Description  : BLE 开扫描，用于ble做主接收从机端广播数据
 * Parameters   : discoverable : ENABLE or DISABLE
 *                scan_interval : 扫描间隔,单位us。
 *                scan_window : 扫描窗口,单位us。
 * Returns      : none
**********************************************/
void BT_SetScanEnable(uint8_t discoverable,uint32_t scan_interval,uint16_t scan_window)
{
    if(discoverable)
    {
        adv_param.evt_interval.scan_interval = scan_interval;
        adv_param.interval.scan_window = scan_window;
        init_le_scan();
        le_start_scan();
    }
}

/*********************************************
 * FunctionName : BT_SetBleConnAddr
 * Description  : BLE 连接到指定地址
 * Parameters   : addr : 目标地址
 * Returns      : none
**********************************************/
void BT_SetBleConnAddr(uint8_t *addr)
{
    memcpy(adv_param.peer_addr, addr, 6);
}


/*********************************************
 * FunctionName : BT_BleConnectParmUpdateReq(must be connected ble before call this function)
 * Description  : BLE 更新连接参数
 * Parameters   : data : ble updete data
 *                       byte0-byte1:min connect interval[6~3200],minConnectionInterval=(byte0+(byte1<<8))*1.25ms
 *                       byte2-byte3:max connect interval[6~3200],maxConnectionInterval=(byte2+(byte3<<8))*1.25ms
 *                       byte4-byte5:slaveLatency[0 ~ 499]
 *                       byte6-byte7:Connection Supervision Timeout[10 ~ 3200],SupervisionTimeout=(byte6+(byte7<<8))*10ms
 *                       SupervisionTimeout >(1+slaveLatency)*(maxConnectionInterval)
 *                len :数据长度,must be 8 bytes
 * Returns      : none
**********************************************/
void BT_BleConnectParmUpdateReq(uint8_t *data,uint8_t len)
{
    if (len != 8)
    {
        return;
    }
    le_l2cap_tx_updata_req(data);
}

/*********************************************
 * FunctionName : BT_BleUpdateMtu
 * Description  : BLE 更新MTU 数据,仅在连接成功后执行操作。
 * Parameters   : mtu : mtu range <23 - 517>
 * Returns      : none
**********************************************/
void BT_BleUpdateMtu(uint16_t mtu)
{
    if((mtu < 23) || (mtu > 517))
    {
        return;
    }
    ct->mtu.local_mtu = mtu;
    le_send_att_ex_mtu(mtu);
}

/*********************************************
 * FunctionName : BT_BleSetMtu
 * Description  : BLE 设置本地MTU，仅在未连接的时候进行设置
 * Parameters   : mtu : mtu range <23 - 517>
 * Returns      : none
**********************************************/
void BT_BleSetMtu(uint16_t mtu)
{
    if((mtu < 23) || (mtu > 517))
    {
        return;
    }
    conn.mtu.local_mtu = mtu;
    mconn.mtu.local_mtu = mtu;
}

void BT_SetATTService(att_record* att_list,uint8_t att_size)
{
	int i, j, len;
    conn.att_list = (att_record*)att_list;
    conn.att_size = att_size;

	for(i = len = 0;i < conn.att_size;i++)
		len += conn.att_list[i].len;
	if((conn.att_value = malloc(len))) {
		for(i = j = 0;i < conn.att_size;i++) {
            switch (conn.att_list[i].uuid)
            {
            case User_UUID_RX:
                yc_rx_handle = i+1;
                break;
            case User_UUID_TX:
                yc_tx_handle = i+1;
                break;
            default:
                break;
            }
			memcpy(conn.att_value + j, conn.att_list[i].dat, conn.att_list[i].len);
			j += conn.att_list[i].len;
		}
	}
}


/*********************************************
 * FunctionName : BT_BleSlaveSendData
 * Description  : BLE 发送数据
 * Parameters   :   handle  :ble handle
 *                  bleData : ble data
 *                  len     :data len(len<253)
 * Returns      : true or false
**********************************************/
_Bool BT_BleSlaveSendData(uint16_t handle, uint8_t * bleData, uint16_t len)
{
    if((handle == 0)||(len > 253))
    {
        return false;
    }
    le_send_att_notify(handle,len, bleData);
    return true;
}

/*********************************************
 * FunctionName : BT_BleMasterSendData
 * Description  : BLE  master 发送数据
 * Parameters   :   handle  :ble handle
 *                  bleData : ble data
 *                  len     :data len(len<253)
 * Returns      : true or false
**********************************************/
_Bool BT_BleMasterSendData(uint16_t handle, uint8_t * bleData, uint16_t len)
{
    if((handle == 0)||(len > 253))
    {
        return false;
    }
    le_send_att_write_command(handle,len, bleData);
    return true;
}


/*********************************************
 * FunctionName : BT_BleSendReadRSP
 * Description  : BLE send read response
 * Parameters   :   handle  :ble handle
 *                  bleData : ble data
 *                  len     :data len(len<253)
 * Returns      : true or false
**********************************************/
_Bool BT_BleSendReadRSP(uint16_t handle, uint8_t * bleData, uint16_t len)
{
    if((handle == 0)||(len > 253))
    {
        return false;
    }
    le_send_att_read_rsp(handle,len, bleData);
    return true;
}

/*********************************************
 * FunctionName : BT_BleSendReadByTypeRSP
 * Description  : BLE send read response
 * Parameters   :   handle  :ble handle
 *                  bleData : ble data
 *                  len     :data len(len<253)
 * Returns      : true or false
**********************************************/
_Bool BT_BleSendReadByTypeRSP(uint16_t handle, uint8_t * bleData, uint16_t len)
{
    if((handle == 0)||(len > 253))
    {
        return false;
    }
    yc_bt_log("BT_BleSendReadByTypeRSP handle = 0x%04x, len = 0x%04x\n", handle, len);
    le_send_att_read_bytype_rsp(handle,len, bleData);
    return true;
}
#include "yc_bt.h"
#include <stdint.h>
#include "le.h"
#include "BLE_L2capSIgn.h"
#include "BLE_L2capATTS.h"
#include "BLE_L2capATTC.h"
#include "yc_user.h"
#include "stdlib.h"
#include "BLE_LL_Scan.h"
#include "BLE_LL.h"
#include "yc_flash.h"

LPM_RETENTION NvramItem NvramData[NVRAM_NUM] = {0};
/*********************************************
 * FunctionName : BT_GetLibVersion
 * Description  : 获取蓝牙库版本
 * Parameters   : none
 * Returns      : version
**********************************************/
uint16_t BT_GetLibVersion(void)
{
    return bt_lib_version;
}
/*********************************************
 * FunctionName : BT_BleSetMac
 * Description  : 设置 BLE MAC 地址
 * Parameters   : bleAddr :6个字节MAC地址
 * Returns      : none
**********************************************/
void BT_BleSetMac(uint8_t * bleAddr)
{
    for(int i=0;i<6;i++){
        dev->bdaddr[i] = bleAddr[i];
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
    dev->name_len = len;
    for(int i=0;i<len;i++){
        dev->name[i] = bleName[i];
    }
}

/*********************************************
 * FunctionName : BT_BleSetAdvParams
 * Description  : 设置Ble广播数据
 * Parameters   : type: 广播类型
 *                own_addr_type,peer_addr_type 00:public address, 01: random address
 *                advData: 广播数据或者回连地址
 *                len: 必须<=0x20
 * Returns      : none
**********************************************/
void BT_BleSetAdvParams(BLE_AdvTypeEnum type,BLE_AdvAddrType own_addr_type, BLE_AdvAddrType peer_addr_type, uint8_t *advData, uint8_t len)
{
    uint8_t adv_len;
    adv_len = (len > ADV_MAX_LEN ? ADV_MAX_LEN : len);
    dev->adv_type = (uint8_t)type;
    dev->own_addr_type = own_addr_type;
    dev->peer_addr_type = peer_addr_type;
    for(uint8_t i=0; i<adv_len; i++)
    {
        dev->adv_data[i] = advData[i];
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
        dev->scan_rsp_data[i] = scanData[i];
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
    dev->interval.adv_interval = 300;
    dev->evt_interval.adv_event_interval = interval;
}

/*********************************************
 * FunctionName : BT_SetVisibility
 * Description  : BLE 可发现(开广播)
 * Parameters   : discoverable : ENABLE or DISABLE
 * Returns      : none
**********************************************/
void BT_SetVisibility(uint8_t discoverable)
{
    if(discoverable && (dev->state == STATE_ADV))
    {
        dev->advch = 7; //三个信道都广播
    }
    else if(discoverable)
    {
        dev->advch = 7; //三个信道都广播
        le_adv();
    }
    else
    {
        dev->advch = 0; //不广播
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
        dev->evt_interval.scan_interval = scan_interval;
        dev->interval.scan_window = scan_window;
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
    memcpy(dev->conn_mac, addr, 6);
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
    le_l2cap_tx_updata_req(dev->dhandle, data);
}

/*********************************************
 * FunctionName : BT_BleUpdateMtu
 * Description  : BLE 更新MTU 数据,仅在连接成功后执行操作。
 * Parameters   : none
 * Returns      : none
**********************************************/
void BT_BleUpdateMtu(void)
{
    le_send_att_mtu_req(dev->dhandle);
}
/*********************************************
 * FunctionName : BT_BleDisconnectIND
 * Description  : BLE 断开连接指令
 * Parameters   : reason : 断开连接原因，0x13:Remote User Terminated Connection
 * Returns      : none
**********************************************/
void BT_BleDisconnectIND(uint8_t reason)
{
    le_send_ll(dev->dhandle, LL_TERMINATE_IND, &reason);
}
/*********************************************
 * FunctionName : BT_BleSetMtu
 * Description  : BLE set local MTU
 * Parameters   : mtu : mtu range <23 - 517>
 * Returns      : none
**********************************************/
void BT_BleSetMtu(uint16_t mtu)
{
    if((mtu < 23) || (mtu > 517))
    {
        return;
    }
    g_conn.att_para.mtu_set = mtu;
}

/*********************************************
 * FunctionName : BT_BleSlaveSendData
 * Description  : BLE slave send data
 * Parameters   :   handle  :ble handle
 *                  bleData : ble data
 *                  len     :data len(len<253)
 * Returns      : true or false
**********************************************/
_Bool BT_BleSlaveSendData(dev_handle_t dhandle,uint16_t handle, uint8_t * bleData, uint16_t len)
{
    if((handle == 0)||(len > 253))
    {
        return false;
    }
    struct ble_gatt_attr *chrc_decl = find_characteristic_decl_by_handle(handle);
    if (chrc_decl == NULL) return false;
    if (chrc_decl->value & BLE_GATT_CHRC_NOTIFY)
    {
        uint16_t ccc_value = get_cccd_value(handle);
        if (ccc_value & BLE_GATT_CCC_NOTIFY)
        {
            yc_bt_log_buf("send notify data",bleData,len);
            atts_send_notify(dhandle,handle,bleData,len);
            return true;
        }
    }
    if (chrc_decl->value & BLE_GATT_CHRC_INDICATE)
    {
        uint16_t ccc_value = get_cccd_value(handle);
        if (ccc_value & BLE_GATT_CCC_INDICATE)
        {
            yc_bt_log_buf("send indicate data",bleData,len);
            atts_send_indicate(dhandle,handle,bleData,len);
            return true;
        }
    }
    return false;
}

/*********************************************
 * FunctionName : BT_BleMasterSendData
 * Description  : The current version does not support.
 * Parameters   :   handle  :ble handle
 *                  bleData : ble data
 *                  len     :data len(len<253)
 * Returns      : true or false
**********************************************/
_Bool BT_BleMasterSendData(dev_handle_t dhandle,uint16_t handle, uint8_t * bleData, uint16_t len)
{
    if((handle == 0)||(len > 253))
    {
        return false;
    }
    le_send_att_write_cmd(dhandle,handle,bleData,len);
    return true;
}



/*********************************************
 * FunctionName : BT_BleSetPairMode
 * Description  : BLE set pair mode
 * Parameters   : mode : value range enum BLE_PairModeEnum
 *                  PAIR_NONE,PAIR_JUST_WORK,PAIR_PASSKEY,PAIR_PASSKEY_INPUT,PAIR_SC_JUST_WORK,PAIR_SC_PASSKEY,PAIR_SC_NUMERIC
 * Returns      : none
**********************************************/
void BT_BleSetPairMode(BLE_PairModeEnum mode)
{
    switch (mode)
    {
    case PAIR_NONE:
        break;

    case PAIR_JUST_WORK:
        dev->smp.io_cap = IOCAP_NOINPUTNOOUTPUT;
        dev->smp.oob_flag = 0;
        dev->smp.auth_req = BONDING_FLAG;
        break;

    case PAIR_PASSKEY:
        dev->smp.io_cap = IOCAP_DISPLAYONLY;
        dev->smp.oob_flag = 0;
        dev->smp.auth_req = BONDING_FLAG;
        break;

    case PAIR_PASSKEY_INPUT:
        dev->smp.io_cap = IOCAP_KEYBOARDONLY;
        dev->smp.oob_flag = 0;
        dev->smp.auth_req = BONDING_FLAG;
        break;
    case PAIR_SC_JUST_WORK:
        dev->smp.io_cap = IOCAP_NOINPUTNOOUTPUT;
        dev->smp.oob_flag = 0;
        dev->smp.auth_req = BONDING_FLAG | AUTH_REQ_SC_FLAG;
        break;

    case PAIR_SC_PASSKEY:
        dev->smp.io_cap = IOCAP_DISPLAYONLY;
        dev->smp.oob_flag = 0;
        dev->smp.auth_req = BONDING_FLAG | AUTH_REQ_SC_FLAG;
        break;

    case PAIR_SC_NUMERIC:
        dev->smp.io_cap = IOCAP_KEYBOARDDISPLAY;
        dev->smp.oob_flag = 0;
        dev->smp.auth_req = BONDING_FLAG | AUTH_REQ_SC_FLAG;
        break;

    default:
        break;
    }
}


/*********************************************
 * FunctionName : BT_NvramLoad
 * Description  : 从Flash读NVRAM配置
 * Parameters   : none
 * Returns      : none
**********************************************/
void BT_NvramLoad(void)
{
    if(FLASH_Read(NVRAM_FLASH_ADDR, NVRAM_LEN, (uint8_t *)NvramData) == SUCCESS)
    {
        yc_log("nvram load success");
    }else
    {
        yc_log("nvram load fail");
    }
}


/*********************************************
 * FunctionName : BT_SetTXPower
 * Description  : 设置TX功率
 * Parameters   : db : TX功率
 *                -3=-3db,3=3db,5=5db,7=7db,10=10db,其它等于0db
 * Returns      : none
**********************************************/
void BT_SetTXPower(char db)
{
    rf_txpower(db);
}

/*********************************************
 * FunctionName : BT_SetLLParam
 * Description  : 设置LL参数,仅在未连接状态下配置
 * Parameters   : p : LL参数指针，8字节特征，5字节版本
 * Returns      : none
**********************************************/
void BT_SetLLParam(uint8_t *p)
{
    memcpy(dev->features, p, 8);
    memcpy(dev->version, p+8, 5);
}


/*********************************************
 * FunctionName : BT_NvramFindFromList
 * Description  : 从列表中查找引引
 * Parameters   : none
 * Returns      : index, if not found, return NVRAM_NUM
**********************************************/
__WEAK uint8_t BT_NvramFindFromList(void)
{
    uint8_t i = 0;
    if(ct->peer_addr_type == RANDOM_ADDR_TYPE)
    {
        switch (ct->peer_addr[5]&0xc0)
        {
        case NON_RESOLV_ADDR_TYPE:
            break;
        case RESOLV_ADDR_TYPE:
            {
                uint8_t p_rand[3] = {0};
                uint8_t out[16] = {0};
                memcpy(p_rand, ct->peer_addr+3, 3);
                for (i = 0; i < NVRAM_NUM; i++)
                {
                    aes_encrypt(AES_BIG_ENDIAN, NvramData[i].irk, p_rand, 3);
                    aes_result(out);
                    if(memcmp(ct->peer_addr, out, 3) == 0)
                        return i;
                }
            }
            break;
        case RESERVED_ADDR_TYPE:
            break;
        case STATIC_ADDR_TYPE:
            for(i = 0;i<NVRAM_NUM;i++)
            {
                if(memcmp(NvramData[i].addr, ct->peer_addr, 6) == 0)
                    return i;
            }
            break;
        default:
            yc_bt_log("BT_NvramFindFromList fail\n");
            break;
        }
    }
    else
    {
        for(i = 0;i<NVRAM_NUM;i++)
        {
            if(memcmp(NvramData[i].addr, ct->peer_addr, 6) == 0)
                return i;
        }
    }
    return i;
}

/*********************************************
 * FunctionName : BT_NvramUpdateNvramData
 * Description  : 更新nvram数据
 * Parameters   : index : nvram_num
 * Returns      : none
**********************************************/
__WEAK void BT_NvramUpdateNvramData(uint8_t index)
{
    if(index == NVRAM_NUM)
    {
        index --;
        for (uint8_t i = 0; i < NVRAM_NUM; i++)
        {
            if(NvramData[i].num == index)
            {
                NvramData[i].num = 0;
                index = i;
            }
            else
                NvramData[i].num ++;
        }
    }
    if(ct->peer_addr_type == RANDOM_ADDR_TYPE)
    {
        switch (ct->peer_addr[5]&0xc0)
        {
        case NON_RESOLV_ADDR_TYPE:
            break;
        case RESOLV_ADDR_TYPE:
            NvramData[index].type = RANDOM_ADDR_TYPE|RESOLV_ADDR_TYPE;
            memcpy(NvramData[index].irk, ct->remote_irk, 16);
            memcpy(NvramData[index].key, ct->ltk, 16);
            break;
        case RESERVED_ADDR_TYPE:
            break;
        case STATIC_ADDR_TYPE:
            NvramData[index].type = RANDOM_ADDR_TYPE|STATIC_ADDR_TYPE;
            memcpy(NvramData[index].addr, ct->peer_addr, 6);
            memcpy(NvramData[index].key, ct->ltk, 16);
            break;
        default:
            yc_bt_log("BT_NvramFindFromList fail\n");
            break;
        }
    }
    else if(ct->peer_addr_type == PUBLIC_ADDR_TYPE)
    {
        NvramData[index].type = PUBLIC_ADDR_TYPE;
        memcpy(NvramData[index].addr, ct->peer_addr, 6);
        memcpy(NvramData[index].key, ct->ltk, 16);
    }
}



/*********************************************
 * FunctionName : BT_BleNvramSave
 * Description  : 更新nvram数据
 * Parameters   : none
 * Returns      : none
**********************************************/
__WEAK void BT_BleNvramSave(void)
{
    //set1 NvramData init
    if(NvramData[0].num == NvramData[1].num)
    {
        for(int i = 0;i<NVRAM_NUM;i++)
        {
            NvramData[i].num = i;
        }
    }
    //set2 查看Nvram是否已经存储该连接数据
    uint8_t index = BT_NvramFindFromList();
    //set3 更新Nvram数据
    BT_NvramUpdateNvramData(index);
    yc_bt_log_buf("nvram data", (uint8_t *)NvramData, sizeof(NvramData));
    BT_NvramUpdateToFlash();
}

/*********************************************
 * FunctionName : BT_NvramUpdateToFlash
 * Description  : 更新Flash中Nvram数据
 * Parameters   : none
 * Returns      : SUCCESS or ERROR
**********************************************/
__WEAK uint8_t BT_NvramUpdateToFlash(void)
{
    ct->lpm_mult = 0xc8;
    while ((ct->lpm_mult & 0x40))
    {
        delay_ms(1);
    }
    ct->lpm_mult = 0x00;
    yc_bt_log("nvram update to flash");
    FLASH_SectorErase(NVRAM_FLASH_ADDR);
    return FLASH_Write(NVRAM_FLASH_ADDR, NVRAM_LEN, (uint8_t *)NvramData);
}


/*********************************************
 * FunctionName : BT_GetRealRssi
 * Description  : 获取Rssi值,仅在开ble 开rx的状态下使用。
 * Parameters   : none
 * Returns      : RSSI值
**********************************************/
int8_t BT_GetRssi(void)
{
    return get_real_rssi(RSSI_WINDOW_SIZE);
}
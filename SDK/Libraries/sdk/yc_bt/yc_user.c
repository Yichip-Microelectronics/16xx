
#include "yc_user.h"
#include "yc_bt.h"
#include "string.h"
#include "le.h"
#include "yc_log.h"
#include "ble_ll_scan.h"
uint8_t gen_ble_adv_normal[ADV_MAX_LEN];
uint8_t gen_ble_scan_rsp[ADV_MAX_LEN];
uint16_t  yc_rx_handle,yc_tx_handle,yc_name_handle;
void U_BleSetDeviceName(char *name_in)
{
    int name_index,i;
    char ble_name_index[16];

    for (i = 0,name_index = 0; i < strlen(name_in); name_index++, i++) {
        if (name_in[i] == 0)
            break;
        ble_name_index[name_index] = name_in[i];
    }

    ble_name_index[name_index++] = '\0';

    // yc_log_buf("ble_name_gen", (uint8_t *)ble_name_index, (name_index-1));
    BT_BleSetName((uint8_t *)ble_name_index, name_index);//蓝牙名称配置

}


void U_BleGenAdvScanData(uint8_t *adv_data,uint8_t *scan_rsp,
                            char *name,uint8_t name_len,uint8_t *mac)
{
    uint8_t adv_index,scan_index;
    adv_index = 1;
    adv_data[adv_index++] = 0x02;
    adv_data[adv_index++] = 0x01;
    adv_data[adv_index++] = 0x06;
    //manufacturer info
    adv_data[adv_index++] = 0x09;
    adv_data[adv_index++] = 0xff;//manufacturer info flag
    adv_data[adv_index++] = 0xff;
    adv_data[adv_index++] = 0xff;
    for(int i=0; i<6; i++)      //mac
    {
        adv_data[adv_index++] = *(mac + i);
    }
    adv_data[0] = adv_index - 1;
    // if(adv_index > ADV_MAX_LEN)
    //     yc_log_buf("error adv data overflow",&adv_index,1);
    // yc_log_buf("adv data",adv_data,adv_index);
    scan_index = 1;
    scan_rsp[scan_index++] = name_len;            //name flag

    scan_rsp[scan_index++] = 0x09;
    for(int i=0; i<name_len; i++)      //name
    {
        scan_rsp[scan_index++] = *(name+i);
    }
    scan_rsp[0] = scan_index - 1;

    // if(scan_index > ADV_MAX_LEN)
    //     yc_log_buf("error scan rsp overflow",&scan_index,1);
    // yc_log_buf("scan rsp",scan_rsp,scan_index);
}


void U_BleParseWriteData(uint16_t len,uint8_t *dataPtr)
{
    //yc_log_buf("Ble_Write_RX_Handle", dataPtr, 2);
    //yc_log_buf("Ble_Write_RX_Data", dataPtr+2, len-2);

    uint16_t ble_handle = dataPtr[0] | (dataPtr[1]<<8);
    uint16_t ble_data_len = len-2;
    yc_bt_log("ble_handle = 0x%04x, ble_data_len = 0x%04x\n", ble_handle, ble_data_len);
    if(ble_handle == yc_rx_handle){
        BT_BleSlaveSendData(yc_tx_handle, dataPtr+2, ble_data_len);
    }
    return;
}
void U_BleParseReadData(uint16_t len,uint8_t *dataPtr)
{
    uint16_t ble_handle = dataPtr[0] | (dataPtr[1]<<8);

    switch (ct->att_list[ble_handle-1].uuid)
    {
    case UUID_DEVICE_NAME:
        BT_BleSendReadRSP(ble_handle,adv_param.name,adv_param.name_len);
        break;
    default:
         BT_BleSendReadRSP(ble_handle,le_find_att_value_ptr(ble_handle),le_find_att_valuelen(ble_handle));
        break;
    }
    return;
}

void U_BleParseReadByTypeData(uint16_t len,uint8_t *dataPtr)
{
    uint16_t ble_uuid = dataPtr[4] | (dataPtr[5]<<8);
    uint16_t ble_start_handle = dataPtr[0] | (dataPtr[1]<<8);
    uint16_t ble_end_handle = dataPtr[2] | (dataPtr[3]<<8);
    uint16_t ble_handle = le_search_uuid(ble_uuid,ble_start_handle,ble_end_handle);
    switch (ble_uuid)
    {
    case UUID_DEVICE_NAME:
        yc_bt_log_buf("UUID_DEVICE_NAME",adv_param.name,adv_param.name_len);
        BT_BleSendReadByTypeRSP(ble_handle,adv_param.name,adv_param.name_len);
        break;
    default:
         BT_BleSendReadByTypeRSP(ble_handle,le_find_att_value_ptr(ble_handle),le_find_att_valuelen(ble_handle));
        break;
    }
    return;
}

void U_BleParsePrepareWriteData(uint16_t len,uint8_t *dataPtr)
{
    uint16_t ble_handle = dataPtr[0] | (dataPtr[1]<<8);
    uint16_t data_offset = dataPtr[2] | (dataPtr[3]<<8);
    uint16_t ble_data_len = len-4;
    yc_bt_log("ble_handle = 0x%04x, ble_data_len = 0x%04x, data_offset = 0x%04x\n", ble_handle, ble_data_len, data_offset);
    if(ble_handle == yc_rx_handle){
        BT_BleSlaveSendData(yc_tx_handle, dataPtr+4, ble_data_len);
    }
    return;
}

void U_BleParseExecuteWriteData(uint16_t len,uint8_t *dataPtr)
{
    uint16_t FlagStatus = dataPtr[0];
    yc_bt_log("FlagStatus = 0x%02x\n", FlagStatus);
    return;
}

void U_BleParseScanAdvData(uint8_t *p)
{
    if(memcmp(adv_param.peer_addr, p + ADV_ADDR, 6) == 0)
    {
        le_start_conn(adv_param.peer_addr);
    }
    else
        le_start_scan();
}



void U_BleParseNotifyData(uint16_t len,uint8_t *dataPtr)
{
    yc_log_buf("Ble_Notify_RX_Handle", dataPtr, 2);
    yc_log_buf("Ble_Notify_RX_Data", dataPtr+2, len-2);
    uint16_t ble_notify_uuid = 0xf3ff;
    uint16_t ble_data_len = len-2;
    uint16_t ble_handle = 0;
    ble_find_ATTHandleByUUID((uint8_t *)ct->att_ptr, ble_notify_uuid, &ble_handle);
    if(ble_handle != 0)
        yc_bt_log("ble_notify_uuid = 0x%04xerr", ble_notify_uuid);
    yc_bt_log("ble_handle = 0x%04x, ble_data_len = 0x%04x\n", ble_handle, ble_data_len);
    BT_BleMasterSendData(ble_handle, dataPtr+2, ble_data_len);
    return;
}
#include "yc_ipc.h"
#include "le.h"
#include "yc_bt_evt.h"
#include "yc_user.h"
#include "yc_bt.h"
#include "yc_types.h"
#include "yc_log.h"
#include "BLE_L2capMain.h"
#include "yc_lpm.h"

/*********************************************
 * FunctionName : IPC_HandleRxPacket
 * Description  : BLE 处理接收数据包
 * Parameters   : NA
 * Returns      : NA
**********************************************/
void IPC_HandleRxPacket(void)
{
    int cmd;
    while((cmd = get_queue(dev->cmds, sizeof(dev->cmds), &dev->cwptr, &dev->crptr))) {
            IPC_ParseEvtCallback(cmd);
    }
}
/*********************************************
 * FunctionName : IPC_ParseEvtCallback
 * Description  : 解析事件回调函数，根据事件类型进行处理
 * Parameters   : dataPtr :接收数据包指针
 * Returns      : NA
**********************************************/
void IPC_ParseEvtCallback(uint32_t dataPtr)
{
    uint8_t *p;
    p = (uint8_t *) (dataPtr >> 8);
    leconn *ct = GET_CT(dev->dhandle);
    yc_bt_log("[%s] handle %d",__func__, dev->dhandle);
    switch(dataPtr & 0xff)
    {
    case HCI_EVENT_LE_L2CAP_RX_PARSE:
        le_parse_l2cap(dev->dhandle, p);
        free(p);
        break;
    case HCI_EVENT_LE_CONN_REP:
    {
        extern const uint8_t ble_conn_param[8];
        yc_bt_log("HCI_EVENT_LE_CONN_REP");
        BT_BleConnectParmUpdateReq((uint8_t*)ble_conn_param,sizeof(ble_conn_param));
        BT_BleUpdateMtu();
        break;
    }
    case HCI_EVENT_LE_DIS_REP:
        yc_bt_log("HCI_EVENT_LE_DIS_REP");
        le_context_init(DISABLE);
        BT_SetVisibility(ENABLE);
        break;
    case HCI_EVENT_MASTER_SCAN_ADV:
        yc_bt_log_buf("HCI_EVENT_MASTER_SCAN_ADV",p,p[1]+2);
        U_BleParseScanAdvData(p);
        free(p);
        break;
    case HCI_EVENT_MASTER_SCAN_RSP:
        yc_bt_log_buf("HCI_EVENT_MASTER_SCAN_RSP",p,p[1]+2);
        free(p);
        break;
    case HCI_EVENT_MASTER_SERVICE_DISCOVER_DONE:
        yc_bt_log("HCI_EVENT_MASTER_SERVICE_DISCOVER_SUCCESS");
        //lxy p>>8,修改为p
        ct->att_ptr = (struct ble_gatt_attr *)(p);
        //TODO: 发起配对
        // le_master_start_pairing(dev->dhandle);
        U_BleServiceParse(dev->dhandle);
        break;
    case HCI_EVENT_ENC_DONE:
        yc_bt_log("HCI_EVENT_ENC_DONE");

        BT_BleNvramSave();
        break;
    case HCI_EVENT_GET_PASSKEY:
        /*在蓝牙 Passkey 配对方式中会产生 Passkey， 在需要输入 Passkey 时，模块会
        发送 HCI_EVENT_GET_PASSKEY 事件来提醒 MCU 使用 HCI_CMD_PASSKEY_ENTRY 来
        下发 Passkey，事件格式如下：
        */
        // yc_bt_log("HCI_EVENT_LE_INPUT_GKEY_WAIT");
        if(U_smp_passkey_wait_input(dev->dhandle)==0)
        {
            add_timeout_timer(100000, (void*)U_smp_passkey_wait_input_timeout, dev->dhandle);
        }
        break;
    case HCI_EVENT_GKEY:
        //在 Numeric Comparison 配对方式中会产生密钥，用于和配对另一方比较，确认是否一致
        yc_bt_log("HCI_EVENT_GKEY");
        yc_bt_log("Numeric Comparison::%06d",ct->passkey_ctx->passkey);
        break;
    case HCI_EVENT_LE_TK:
        //在 BLE PASSKEY 配对方式中， 需要对 TK 值进行校验， 在手机上输入对应的 TK值进行配对。
        yc_bt_log("HCI_EVENT_LE_TK");
        yc_bt_log("Passkey::%06d",ct->passkey_ctx->passkey);
        break;
    case HCI_EVENT_LE_ENCRYPTION_START:
        yc_bt_log("HCI_EVENT_LE_ENCRYPTION_START");
        break;
    case HCI_EVENT_LE_HANDLE_VALUE_CONFIRMATION:
        //发送indicate包的应答
        yc_bt_log("HCI_EVENT_LE_HANDLE_VALUE_CONFIRMATION");
        break;
    default:
        yc_bt_log("Unknown event: %02x", dataPtr & 0xff);
        break;
    }
}
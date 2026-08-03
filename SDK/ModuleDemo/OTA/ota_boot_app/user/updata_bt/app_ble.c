#include "app_ble.h"
#include "ota_ble.h"
#define BLE_LOCAL_MTU 240

#define BLE_ADV_INTERVAL        (30000) //30ms
#define BLE_SCAN_INTERVAL       (20000)
#define BLE_SCAN_WINDOW         (3000)
uint16_t LPM_RETENTION yc_ota_rx_handle = 0;
uint16_t LPM_RETENTION yc_ota_tx_handle = 0;
static LPM_RETENTION uint16_t OTA_ccc_value = 0;
static LPM_RETENTION uint16_t OTA_ccc_value1 = 0;
static const struct ble_uuid_128 OTAService_uuid =
{
    .uuid.type = BLE_UUID_TYPE_128,
    .val = {0x55,0xe4,0x05,0xd2,0xaf,0x9f,0xa9,0x8f,0xe5,0x4a,0x7d,0xfe,0x43,0x53,0x53,0x49},
};
static const struct ble_uuid_128 OTATransmicTX_uuid =
{
    .uuid.type = BLE_UUID_TYPE_128,
    .val = {0x18,0x03,0xA6,0x28,0x5E,0xD8,0xEC,0x91,0x1C,0x48,0xA3,0xAC,0x43,0x53,0x53,0x49},
};
static const struct ble_uuid_128 OTATransmicRX_uuid =
{
    .uuid.type = BLE_UUID_TYPE_128,
    .val = {0xfe,0x69,0xca,0x9a,0x56,0x19,0xf6,0xab,0x02,0x4d,0xaa,0x6d,0x43,0x53,0x53,0x49},
};
static const struct ble_uuid_128 OTATX_uuid =
{
    .uuid.type = BLE_UUID_TYPE_128,
    .val = {0x16,0x96,0x24,0x47,0xc6,0x23,0x61,0xba,0xd9,0x4b,0x4d,0x1e,0x43,0x53,0x53,0x49},
};
static const struct ble_uuid_128 OTARX_uuid =
{
    .uuid.type = BLE_UUID_TYPE_128,
    .val = {0xb3,0x9b,0x72,0x34,0xbe,0xec,0xd4,0xa8,0xf4,0x43,0x41,0x88,0x43,0x53,0x53,0x49},
};
uint8_t transmit_tx[10]={0};
uint8_t transmit_rx[20]={0};
uint8_t ota_data[20]={0};
int U_ATT_OTA_ServiceCbWrite(dev_handle_t dhandle, uint16_t att_handle, const struct ble_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset);
BLE_GATT_SERVICE_DEFINE_EX(OTA_Service, U_ATT_OTA_ServiceCbWrite,NULL,
                           // 服务声明
                           BLE_GATT_PRIMARY_SERVICE(BLE_GATT_PTR(&OTAService_uuid), 16),

                           // 特征1
                           BLE_GATT_CHARACTERISTIC(BLE_GATT_PTR(&OTATransmicTX_uuid),
                                   BLE_GATT_CHRC_NOTIFY,
                                   BLE_GATT_PERM_WRITE,
                                   BLE_GATT_PTR(transmit_tx),                     // 数据指针
                                   10),                   // 数据长度
                           // 特征1描述
                           BLE_GATT_CCC(BLE_GATT_CHRC_READ | BLE_GATT_CHRC_WRITE,
                                        BLE_GATT_PTR(&OTA_ccc_value),
                                        2),
                           // 特征2
                           BLE_GATT_CHARACTERISTIC(BLE_GATT_PTR(&OTATransmicRX_uuid),
                                   BLE_GATT_CHRC_WRITE | BLE_GATT_CHRC_WRITE_WITHOUT_RESP,
                                   BLE_GATT_PERM_WRITE,
                                   BLE_GATT_PTR(transmit_rx),                     // 数据指针
                                   20),                   // 数据长度
                            // 特征3
                           BLE_GATT_CHARACTERISTIC(BLE_GATT_PTR(&OTATX_uuid),
                                   BLE_GATT_CHRC_READ | BLE_GATT_CHRC_WRITE_WITHOUT_RESP | BLE_GATT_CHRC_NOTIFY| BLE_GATT_CHRC_INDICATE ,
                                   BLE_GATT_PERM_READ | BLE_GATT_PERM_WRITE | BLE_GATT_PERM_PREPARE_WRITE,
                                   BLE_GATT_PTR(ota_data),                     // 数据指针
                                   20),
                           // 特征3描述
                           BLE_GATT_CCC(BLE_GATT_CHRC_READ | BLE_GATT_CHRC_WRITE,
                                        BLE_GATT_PTR(&OTA_ccc_value1),
                                        2),
                            // 特征4
                           BLE_GATT_CHARACTERISTIC(BLE_GATT_PTR(&OTARX_uuid),
                                   BLE_GATT_CHRC_WRITE_WITHOUT_RESP,
                                   BLE_GATT_PERM_WRITE | BLE_GATT_PERM_PREPARE_WRITE,
                                   BLE_GATT_PTR(ota_data),                     // 数据指针
                                   20),
                          );
/**************************蓝牙Public地址***************************/
//ble 从机地址，前四位有效，后两位为flash ID
static uint8_t ble_addr_public[6] = {0x65,0x22,0x72,0x30,0x00,0x00};
/**************************蓝牙LL 参数****************************/
static const uint8_t ll_param[13] = {0x21,0x09,0x00,0x00,0x00,0x00,0x00,0x00,0x0b, 0x0e, 0x05, 0x5a, 0x02};
/**************************蓝牙设备名***************************/
//最大不能超过16个字节
static char ble_name[15] = "yc1161";
/**************************蓝牙连接参数***************************/
const uint8_t ble_conn_param[8]  =
{
    0x18, 0x00,         //minimum connection interval. unit: 1.25ms
    0x24, 0x00,         //maximum connection interval, in normal case can be set same as min_intv. unit: 1.25ms
    0x00, 0x00,         //slave latency, number of connection events that slave would like to skip.
    0x2c, 0x01          //timeout before connection is dropped. unit: 10ms
};

struct ble_gatt_service_static *gatt_svc[] =
{
    (struct ble_gatt_service_static *) &Generic_Access,
    (struct ble_gatt_service_static *) &Generic_Attribute,
    (struct ble_gatt_service_static *) &OTA_Service,
    NULL,
};




void Ble_ParamInit(void)
{
   uint16_t version = BT_GetLibVersion();
    yc_log("bt_lib_version:%04x",version);
    init_le();
    //设置BLE地址，public or random.
    U_BleSetMac((uint8_t*)ble_addr_public);
    yc_bt_log_buf("BLE_ADDR_Public:", ble_addr_public, sizeof(ble_addr_public));
    //设置BLE名称
    U_BleSetDeviceName((char *)ble_name,(uint8_t*)ble_addr_public);

    yc_bt_log("ble_device_name:%s", ble_name);
    //生成BLE广播数据和扫描响应包
    U_BleGenAdvScanData(gen_ble_adv_normal,gen_ble_scan_rsp,dev->name,dev->name_len,(uint8_t*)ble_addr_public);
    yc_bt_log_buf("BLE_adv_normal :", gen_ble_adv_normal, sizeof(gen_ble_adv_normal));
    yc_bt_log_buf("BLE_scan_rsp   :", gen_ble_scan_rsp, sizeof(gen_ble_scan_rsp));
    //使能广播
    BT_BleSetAdvParams(ADV_IND,ADDR_PUBLIC,ADDR_PUBLIC, (uint8_t *)gen_ble_adv_normal, ADV_MAX_LEN);
    //使能扫描响应
    BT_BleSetScanData((uint8_t *)gen_ble_scan_rsp, ADV_MAX_LEN);
    //设置广播间隔
    BT_BleSetAdvInterval(BLE_ADV_INTERVAL);
    yc_bt_log("BLE_ADV_Interval:%d", BLE_ADV_INTERVAL);
    //设置本地MTU
    BT_BleSetMtu(BLE_LOCAL_MTU);
    //从Flash读NVRAM配置
    BT_NvramLoad();
    //设置LL参数
    BT_SetLLParam((uint8_t*)ll_param);
    //设置BLE配对模式
    BT_BleSetPairMode(PAIR_JUST_WORK);
    yc_bt_log("BLE_Pair_Mode:%s", "PAIR_JUST_WORK");
    //设置发射功率
    BT_SetTXPower(0);
    //设置BLE可发现
    BT_SetVisibility(ENABLE);
    //查询蓝牙状态
    //yc_bt_log("Bt state = 0x%x", BT_GetStatus());
    yc_ota_tx_handle=atts_get_handle_by_uuid(0,(uint8_t*)OTATX_uuid.val,16);
    yc_ota_rx_handle=atts_get_handle_by_uuid(0,(uint8_t*)OTARX_uuid.val,16);
}
int U_ATT_OTA_ServiceCbWrite(dev_handle_t dhandle, uint16_t att_handle, const struct ble_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)
{
    if (get_attr_uuid16((struct ble_gatt_attr *)attr) == BLE_UUID_GATT_CCC_VAL)
    {
        return atts_write_value((struct ble_gatt_attr *)attr, (uint8_t *)buf, len);
    }
    if (att_handle==yc_ota_rx_handle)
    {
        Ble_Cmd_Check(dhandle,buf);
    }
    return true;
}

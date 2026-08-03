#ifndef __BLE_L2CAPATT_LIST_H__
#define __BLE_L2CAPATT_LIST_H__
#include "BLE_L2capATTS.h"
#include "yc_user.h"

/*设备名称数据 */
/*在自定义服务*/
/*指针模式 UUID 必须 ble_uuid_16*，ble_uuid_128*这么定义*/
static const struct ble_uuid_128 serviefffe_uuid =
{
    .uuid.type = BLE_UUID_TYPE_128,
    .val = {
        0xFE, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA
    },
};

static const struct ble_uuid_16 serviefffc_uuid =
{
    .uuid.type = BLE_UUID_TYPE_16,
    .val = 0xfffc,
};
static const struct ble_uuid_16 serviefffa_uuid =
{
    .uuid.type = BLE_UUID_TYPE_16,
    .val = 0xfffa,
};
static LPM_RETENTION uint16_t Use_ccc_value2 = 0;
/* 服务定义 */
BLE_GATT_SERVICE_DEFINE_EX(Generic_Access,NULL,U_ATT_GenericAccess_CbRead,
                        // 服务声明
                        BLE_GATT_PRIMARY_SERVICE(BLE_GATT_VAL(UUID_SERVICE_GENERIC_ACCESS), 2),
                        // 特征1
                        BLE_GATT_CHARACTERISTIC(BLE_GATT_PTR(BLE_UUID_GAP_DEVICE_NAME),
                                BLE_GATT_CHRC_READ | BLE_GATT_CHRC_WRITE,         // 属性：读写
                                BLE_GATT_PERM_READ | BLE_GATT_PERM_WRITE,         // 权限：读写
                                BLE_GATT_PTR(NULL),                               // 数据指针
                                0),                                               // 数据长度
                        // 特征2
                        BLE_GATT_CHARACTERISTIC(BLE_GATT_PTR(BLE_UUID_GAP_APPEARANCE),
                                BLE_GATT_CHRC_READ,           // 属性：只读
                                BLE_GATT_PERM_READ,         // 权限：可读
                                BLE_GATT_VAL(0x00c8),              // 数据值
                                2),      // 数据长度
                       );

BLE_GATT_SERVICE_DEFINE_EX(Generic_Attribute,NULL,NULL,
                        // 服务声明
                        BLE_GATT_PRIMARY_SERVICE(BLE_GATT_VAL(UUID_SERVICE_GENERIC_ATT), 2),

                        // 特征1
                        BLE_GATT_CHARACTERISTIC(BLE_GATT_PTR(BLE_UUID_GATT_SC),
                                BLE_GATT_CHRC_INDICATE,
                                BLE_GATT_PERM_READ | BLE_GATT_PERM_WRITE,
                                BLE_GATT_PTR(NULL),              // 数据指针
                                0),      // 数据长度
                        // 特征1描述
                        BLE_GATT_CCC(BLE_GATT_PERM_READ,
                                     BLE_GATT_VAL(0),
                                     2)     // 数据长度

                       );

BLE_GATT_SERVICE_DEFINE_EX(User_Service, U_ATT_User_ServiceCbWrite,NULL,
                           // 服务声明
                           BLE_GATT_PRIMARY_SERVICE(BLE_GATT_PTR(&serviefffe_uuid), 16),

                           // 特征1
                           BLE_GATT_CHARACTERISTIC(BLE_GATT_PTR(&serviefffc_uuid),
                                   BLE_GATT_CHRC_WRITE_WITHOUT_RESP,
                                   BLE_GATT_PERM_WRITE,
                                   BLE_GATT_PTR(NULL),                     // 数据指针
                                   0),                   // 数据长度
                           // 特征2
                           BLE_GATT_CHARACTERISTIC(BLE_GATT_PTR(&serviefffa_uuid),
                                   BLE_GATT_CHRC_READ | BLE_GATT_CHRC_WRITE | BLE_GATT_CHRC_NOTIFY | BLE_GATT_CHRC_INDICATE,
                                   BLE_GATT_PERM_READ | BLE_GATT_PERM_WRITE | BLE_GATT_PERM_PREPARE_WRITE,
                                   BLE_GATT_VAL(0x0001),                     // 数据指针
                                   2),                   // 数据长度
                           // 特征2描述
                           BLE_GATT_CCC(BLE_GATT_CHRC_READ | BLE_GATT_CHRC_WRITE,
                                        BLE_GATT_PTR(&Use_ccc_value2),
                                        2)
                          );
#endif

#ifndef __BLE_L2CAP_ATTS_H__
#define __BLE_L2CAP_ATTS_H__
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "le.h"
#include "BLE_L2capATT_UUID.h"

#ifdef __cplusplus
extern "C" {
#endif

/** GATT attribute permission bit field values */
enum ble_gatt_perm
{
    BLE_GATT_PERM_NONE = 0,
    BLE_GATT_PERM_READ = 1 << 0,
    BLE_GATT_PERM_WRITE = 1 << 1,
    BLE_GATT_PERM_READ_ENCRYPT = 1 << 2,
    BLE_GATT_PERM_WRITE_ENCRYPT = 1 << 3,
    BLE_GATT_PERM_READ_AUTHEN = 1 << 4,
    BLE_GATT_PERM_WRITE_AUTHEN = 1 << 5,
    BLE_GATT_PERM_PREPARE_WRITE = 1 << 6,
    BLE_GATT_PERM_READ_LESC = 1 << 7,
    BLE_GATT_PERM_WRITE_LESC = 1 << 8,
};

#define BLE_GATT_CHRC_BROADCAST          0x01
#define BLE_GATT_CHRC_READ               0x02
#define BLE_GATT_CHRC_WRITE_WITHOUT_RESP 0x04
#define BLE_GATT_CHRC_WRITE              0x08
#define BLE_GATT_CHRC_NOTIFY             0x10
#define BLE_GATT_CHRC_INDICATE           0x20
#define BLE_GATT_CHRC_AUTH               0x40
#define BLE_GATT_CHRC_EXT_PROP           0x80

/* ========== CCCD (Client Characteristic Configuration) Values ========== */
#define BLE_GATT_CCC_NOTIFY              0x0001  // 通知已订阅
#define BLE_GATT_CCC_INDICATE            0x0002  // 指示已订阅

/* ========== CCCD 组合宏 ========== */
#define BLE_GATT_CCC_NOTIFY_INDICATE     (BLE_GATT_CCC_NOTIFY | BLE_GATT_CCC_INDICATE)  // 0x0003

struct ble_gatt_attr
{
    uint16_t perm;                    // 权限标志
    uint16_t attr_len : 14;           // 属性长度（低14位）
    uint8_t uuid_flag : 1;            // 1: uuid内嵌，0: uuid_ptr指针
    uint8_t value_flag : 1;           // 1: value内嵌，0: value_ptr指针
    union
    {
        const struct ble_uuid *p_uuid;   // 指向外部UUID的指针（uuid_flag=0时使用）
        uint32_t uuid;                  // 内嵌UUID数据（uuid_flag=1时使用，最多4字节）
    };                               // 结构体成员名仍为uuid
    union
    {
        uint8_t *p_value;               // 指向外部值的指针（value_flag=0时使用）
        uint32_t value;                 // 内嵌小数据（value_flag=1时使用，最多4字节）
    };                              // 结构体成员名仍为value
};

typedef int (*att_callback_t)(dev_handle_t dhandle, uint16_t att_handle,
                              const struct ble_gatt_attr *attr,
                              void *buf, uint16_t len,
                              uint16_t offset);

struct ble_gatt_service_static
{
    const struct ble_gatt_attr *attrs;
    uint32_t attr_count;
    att_callback_t att_wfunc;
    att_callback_t att_rfunc;
};
/* ==========  ========== */
#define BLE_GATT_PTR(ptr)    ptr,0
#define BLE_GATT_VAL(val)    val,1

/* ========== UUID ========== */
#define BLE_GATT_UUID_INIT(_uuid, _flag) \
    BLE_GATT_UUID_INIT_##_flag(_uuid)

#define BLE_GATT_UUID_INIT_0(_uuid) \
    .uuid_flag = 0, \
    .p_uuid = (const struct ble_uuid *)(_uuid)

#define BLE_GATT_UUID_INIT_1(_uuid) \
    .uuid_flag = 1, \
    .uuid = (uint32_t)(_uuid)

/* ========== Value ========== */
#define BLE_GATT_VALUE_INIT(_value, _flag) \
    BLE_GATT_VALUE_INIT_##_flag(_value)

#define BLE_GATT_VALUE_INIT_0(_value) \
    .value_flag = 0, \
    .p_value = (uint8_t *)(_value)

#define BLE_GATT_VALUE_INIT_1(_value) \
    .value_flag = 1, \
    .value = (uint32_t)(_value)

/* ========== 属性 ========== */
#define BLE_GATT_ATTRIBUTE_EX(_perm,_len,_uuid, _uflag, _value, _vflag) \
    { \
        .perm = (_perm), \
        .attr_len = (_len), \
        BLE_GATT_UUID_INIT(_uuid, _uflag), \
        BLE_GATT_VALUE_INIT(_value, _vflag) \
    }
#define BLE_GATT_ATTRIBUTE_(...)  BLE_GATT_ATTRIBUTE_EX(__VA_ARGS__)
/* ========== Service  ========== */
#define BLE_GATT_PRIMARY_SERVICE(_service,_len) \
            BLE_GATT_ATTRIBUTE_(BLE_GATT_PERM_READ, \
                          _len, \
                         BLE_GATT_VAL(BLE_UUID_GATT_PRIMARY_VAL), \
                         _service)
/* ========== Characteristic Macros ========== */
#define BLE_GATT_CHARACTERISTIC(_uuid, _props, _perm, _user_data, _pAttrlen) \
            BLE_GATT_ATTRIBUTE_(_perm, \
                         1, \
                         BLE_GATT_VAL(BLE_UUID_GATT_CHRC_VAL), \
                         BLE_GATT_VAL(_props)), \
            BLE_GATT_ATTRIBUTE_(_perm, \
                          _pAttrlen, \
                         _uuid, \
                         _user_data)

/* ========== CCCD Macros ========== */
#define BLE_GATT_CCC(_perm, _service, _pAttrlen) \
            BLE_GATT_ATTRIBUTE_(_perm, \
                          _pAttrlen, \
                         BLE_GATT_VAL(BLE_UUID_GATT_CCC_VAL), \
                         _service)

/* ========== Helper Macros ========== */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* ========== Service Definition Macros ========== */
#define BLE_GATT_SERVICE_DEFINE_EX(_name, _att_wfunc,_att_rfunc, ...) \
    static const struct ble_gatt_attr _name##_attrs[] = { __VA_ARGS__ }; \
    static const struct ble_gatt_service_static _name = { \
        .attrs = _name##_attrs, \
        .attr_count = ARRAY_SIZE(_name##_attrs), \
        .att_wfunc = (_att_wfunc), \
        .att_rfunc = (_att_rfunc), \
    }

#define BLE_GATT_SERVICE_DEFINE(_name, ...) \
    BLE_GATT_SERVICE_DEFINE_EX(_name, NULL,NULL, __VA_ARGS__)

/* Error codes for Error response PDU
*
* Defined by The Bluetooth Core Specification, Version 5.4, Vol 3, Part F, Section 3.4.1.1
*/
/** The ATT operation was successful */
#define BLE_ATT_ERR_SUCCESS          0x00
/** The attribute handle given was not valid on the server */
#define BLE_ATT_ERR_INVALID_HANDLE       0x01
/** The attribute cannot be read */
#define BLE_ATT_ERR_READ_NOT_PERMITTED       0x02
/** The attribute cannot be written */
#define BLE_ATT_ERR_WRITE_NOT_PERMITTED      0x03
/** The attribute PDU was invalid */
#define BLE_ATT_ERR_INVALID_PDU          0x04
/** The attribute requires authentication before it can be read or written */
#define BLE_ATT_ERR_AUTHENTICATION       0x05
/** The ATT Server does not support the request received from the client */
#define BLE_ATT_ERR_NOT_SUPPORTED        0x06
/** Offset specified was past the end of the attribute */
#define BLE_ATT_ERR_INVALID_OFFSET       0x07
/** The attribute requires authorization before it can be read or written */
#define BLE_ATT_ERR_AUTHORIZATION        0x08
/** Too many prepare writes have been queued */
#define BLE_ATT_ERR_PREPARE_QUEUE_FULL       0x09
/** No attribute found within the given attribute handle range */
#define BLE_ATT_ERR_ATTRIBUTE_NOT_FOUND      0x0a
/** The attribute cannot be read using the ATT_READ_BLOB_REQ PDU */
#define BLE_ATT_ERR_ATTRIBUTE_NOT_LONG       0x0b
/** The Encryption Key Size used for encrypting this link is too short */
#define BLE_ATT_ERR_ENCRYPTION_KEY_SIZE      0x0c
/** The attribute value length is invalid for the operation */
#define BLE_ATT_ERR_INVALID_ATTRIBUTE_LEN    0x0d
/**
 * @brief The attribute request that was requested has encountered an error that was unlikely
 *
 * The attribute request could therefore not be completed as requested
 */
#define BLE_ATT_ERR_UNLIKELY         0x0e
/** The attribute requires encryption before it can be read or written */
#define BLE_ATT_ERR_INSUFFICIENT_ENCRYPTION  0x0f
/**
 * @brief The attribute type is not a supported grouping attribute
 *
 * The attribute type is not a supported grouping attribute as defined by a higher layer
 * specification.
 */
#define BLE_ATT_ERR_UNSUPPORTED_GROUP_TYPE   0x10
/** Insufficient Resources to complete the request */
#define BLE_ATT_ERR_INSUFFICIENT_RESOURCES   0x11
/** The server requests the client to rediscover the database */
#define BLE_ATT_ERR_DB_OUT_OF_SYNC       0x12
/** The attribute parameter value was not allowed */
#define BLE_ATT_ERR_VALUE_NOT_ALLOWED        0x13

/* Common Profile Error Codes
 *
 * Defined by the Supplement to the Bluetooth Core Specification (CSS), v11, Part B, Section 1.2.
 */
/** Write Request Rejected */
#define BLE_ATT_ERR_WRITE_REQ_REJECTED       0xfc
/** Client Characteristic Configuration Descriptor Improperly Configured */
#define BLE_ATT_ERR_CCC_IMPROPER_CONF        0xfd
/** Procedure Already in Progress */
#define BLE_ATT_ERR_PROCEDURE_IN_PROGRESS    0xfe
/** Out of Range */
#define BLE_ATT_ERR_OUT_OF_RANGE         0xff

/* Version 5.2, Vol 3, Part F, 3.2.9 defines maximum attribute length to 512 */
#define BLE_ATT_MAX_ATTRIBUTE_LEN        512

/* Handle 0x0000 is reserved for future use */
#define BLE_ATT_FIRST_ATTRIBUTE_HANDLE           0x0001
/* 0xffff is defined as the maximum, and thus last, valid attribute handle */
#define BLE_ATT_LAST_ATTRIBUTE_HANDLE            0xffff

struct ble_gatt_attr *find_characteristic_decl_by_handle(uint16_t handle);
bool atts_write_value(struct ble_gatt_attr *attr, uint8_t *value, uint16_t len);
uint16_t get_cccd_value(uint16_t char_handle);
uint16_t get_attr_uuid16(struct ble_gatt_attr *attr);
uint16_t atts_read_value(struct ble_gatt_attr *attr, uint8_t *buf, uint16_t buf_len);
uint16_t atts_get_handle_by_uuid(dev_handle_t dhandle,uint8_t *uuid, uint8_t uuidlen);
__LEATTFN_WEAK_ atts_exchange_mtu_req(dev_handle_t dhandle,uint8_t *rxp);
__LEATTFN_WEAK_ atts_find_info_req(dev_handle_t dhandle,uint8_t *rxp);
__LEATTFN_WEAK_ atts_find_type_value_req(dev_handle_t dhandle,uint8_t *rxp);
__LEATTFN_WEAK_ atts_read_by_type_req(dev_handle_t dhandle,uint8_t *rxp);
__LEATTFN_WEAK_ atts_read_req(dev_handle_t dhandle,uint8_t *rxp);
__LEATTFN_WEAK_ atts_read_blob_req(dev_handle_t dhandle,uint8_t *rxp);
__LEATTFN_WEAK_ atts_read_by_group_type_req(dev_handle_t dhandle,uint8_t *rxp);
__LEATTFN_WEAK_ atts_write_req(dev_handle_t dhandle,uint8_t *rxp);
__LEATTFN_WEAK_ atts_prepare_write_req(dev_handle_t dhandle,uint8_t *rxp);
__LEATTFN_WEAK_ atts_execute_write_req(dev_handle_t dhandle,uint8_t *rxp);
__LEATTFN_WEAK_ atts_write_cmd(dev_handle_t dhandle,uint8_t *rxp);
__LEATTFN_WEAK_ atts_send_notify(dev_handle_t dhandle, uint16_t handle, uint8_t *attr_val, uint16_t attr_len);
__LEATTFN_WEAK_ atts_send_indicate(dev_handle_t dhandle, uint16_t handle, uint8_t *attr_val, uint16_t attr_len);
__LEATTFN_WEAK_  atts_indicate_response(dev_handle_t dhandle,uint8_t *rxp);
#ifdef __cplusplus
}
#endif

#endif /* __BLE_GATT_H__ */
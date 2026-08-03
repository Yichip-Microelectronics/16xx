#ifndef __BLE_L2CAPATT_UUID_H__
#define __BLE_L2CAPATT_UUID_H__

/** @file
 *  @brief Bluetooth UUID handling
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Bluetooth UUID types */
enum
{
    /** UUID type 16-bit. */
    BLE_UUID_TYPE_16,
    /** UUID type 32-bit. */
    BLE_UUID_TYPE_32,
    /** UUID type 128-bit. */
    BLE_UUID_TYPE_128,
};

/** Size in octets of a 16-bit UUID */
#define BLE_UUID_SIZE_16                  2

/** Size in octets of a 32-bit UUID */
#define BLE_UUID_SIZE_32                  4

/** Size in octets of a 128-bit UUID */
#define BLE_UUID_SIZE_128                 16

/** @brief This is a 'tentative' type and should be used as a pointer only */
struct ble_uuid
{
    uint8_t type;
};

struct ble_uuid_16
{
    /** UUID generic type. */
    struct ble_uuid uuid;
    /** UUID value, 16-bit in host endianness. */
    uint16_t val;
};

struct ble_uuid_32
{
    /** UUID generic type. */
    struct ble_uuid uuid;
    /** UUID value, 32-bit in host endianness. */
    uint32_t val;
};

struct ble_uuid_128
{
    /** UUID generic type. */
    struct ble_uuid uuid;
    /** UUID value, 128-bit in little-endian format. */
    uint8_t val[BLE_UUID_SIZE_128];
};

/** @brief Helper type that can store any UUID size. */
struct ble_uuid_any
{
    /** Union of all supported UUID sizes. */
    union
    {
        /** Generic UUID view. */
        struct ble_uuid uuid;
        /** 16-bit UUID view. */
        struct ble_uuid_16 u16;
        /** 32-bit UUID view. */
        struct ble_uuid_32 u32;
        /** 128-bit UUID view. */
        struct ble_uuid_128 u128;
    };
};

/** @brief Initialize a 16-bit UUID.
 *
 *  @param value 16-bit UUID value in host endianness.
 */
#define BLE_UUID_INIT_16(value)      \
{                   \
    .uuid = { BLE_UUID_TYPE_16 },    \
    .val = (value),         \
}

/** @brief Initialize a 32-bit UUID.
 *
 *  @param value 32-bit UUID value in host endianness.
 */
#define BLE_UUID_INIT_32(value)      \
{                   \
    .uuid = { BLE_UUID_TYPE_32 },    \
    .val = (value),         \
}

/** @brief Initialize a 128-bit UUID.
 *
 *  @param value 128-bit UUID array values in little-endian format.
 *               Can be combined with @ref BLE_UUID_128_ENCODE to initialize a
 *               UUID from the readable form of UUIDs.
 */
#define BLE_UUID_INIT_128(value...)  \
{                   \
    .uuid = { BLE_UUID_TYPE_128 },   \
    .val = { value },       \
}

/** @brief Helper to declare a 16-bit UUID inline.
 *
 *  @param value 16-bit UUID value in host endianness.
 *
 *  @return Pointer to a generic UUID.
 */
#define BLE_UUID_DECLARE_16(value) \
    ((const struct ble_uuid *) ((const struct ble_uuid_16[]) {BLE_UUID_INIT_16(value)}))

/** @brief Helper to declare a 32-bit UUID inline.
 *
 *  @param value 32-bit UUID value in host endianness.
 *
 *  @return Pointer to a generic UUID.
 */
#define BLE_UUID_DECLARE_32(value) \
    ((const struct ble_uuid *) ((const struct ble_uuid_32[]) {BLE_UUID_INIT_32(value)}))

/** @brief Helper to declare a 128-bit UUID inline.
 *
 *  @param value 128-bit UUID array values in little-endian format.
 *               Can be combined with @ref BLE_UUID_128_ENCODE to declare a
 *               UUID from the readable form of UUIDs.
 *
 *  @return Pointer to a generic UUID.
 */
#define BLE_UUID_DECLARE_128(value...) \
    ((const struct ble_uuid *) ((const struct ble_uuid_128[]) {BLE_UUID_INIT_128(value)}))

/** Helper macro to access the 16-bit UUID from a generic UUID. */
#define BLE_UUID_16(__u) CONTAINER_OF(__u, struct ble_uuid_16, uuid)

/** Helper macro to access the 32-bit UUID from a generic UUID. */
#define BLE_UUID_32(__u) CONTAINER_OF(__u, struct ble_uuid_32, uuid)

/** Helper macro to access the 128-bit UUID from a generic UUID. */
#define BLE_UUID_128(__u) CONTAINER_OF(__u, struct ble_uuid_128, uuid)

/** @brief Encode 128 bit UUID into array values in little-endian format.
 *
 *  Helper macro to initialize a 128-bit UUID array value from the readable form
 *  of UUIDs, or encode 128-bit UUID values into advertising data
 *  Can be combined with BLE_UUID_DECLARE_128 to declare a 128-bit UUID.
 *
 *  Example of how to declare the UUID `6E400001-B5A3-F393-E0A9-E50E24DCCA9E`
 *
 *  @code
 *  BLE_UUID_DECLARE_128(
 *       BLE_UUID_128_ENCODE(0x6E400001, 0xB5A3, 0xF393, 0xE0A9, 0xE50E24DCCA9E))
 *  @endcode
 *
 *  Example of how to encode the UUID `6E400001-B5A3-F393-E0A9-E50E24DCCA9E`
 *  into advertising data.
 *
 *  @code
 *  BLE_DATA_BYTES(BLE_DATA_UUID128_ALL,
 *       BLE_UUID_128_ENCODE(0x6E400001, 0xB5A3, 0xF393, 0xE0A9, 0xE50E24DCCA9E))
 *  @endcode
 *
 *  Just replace the hyphen by the comma and add `0x` prefixes.
 *
 *  @param w32 First part of the UUID (32 bits)
 *  @param w1  Second part of the UUID (16 bits)
 *  @param w2  Third part of the UUID (16 bits)
 *  @param w3  Fourth part of the UUID (16 bits)
 *  @param w48 Fifth part of the UUID (48 bits)
 *
 *  @return The comma separated values for UUID 128 initializer that
 *          may be used directly as an argument for
 *          @ref BLE_UUID_INIT_128 or @ref BLE_UUID_DECLARE_128
 */
#define BLE_UUID_128_ENCODE(w32, w1, w2, w3, w48) \
    BLE_BYTES_LIST_LE48(w48),\
    BLE_BYTES_LIST_LE16(w3), \
    BLE_BYTES_LIST_LE16(w2), \
    BLE_BYTES_LIST_LE16(w1), \
    BLE_BYTES_LIST_LE32(w32)

/** @brief Encode 16-bit UUID into array values in little-endian format.
 *
 *  Helper macro to encode 16-bit UUID values into advertising data.
 *
 *  Example of how to encode the UUID `0x180a` into advertising data.
 *
 *  @code
 *  BLE_DATA_BYTES(BLE_DATA_UUID16_ALL, BLE_UUID_16_ENCODE(0x180a))
 *  @endcode
 *
 * @param w16 UUID value (16-bits)
 *
 * @return The comma separated values for UUID 16 value that
 *         may be used directly as an argument for @ref BLE_DATA_BYTES.
 */
#define BLE_UUID_16_ENCODE(w16) BLE_BYTES_LIST_LE16(w16)

/** @brief Encode 32-bit UUID into array values in little-endian format.
 *
 *  Helper macro to encode 32-bit UUID values into advertising data.
 *
 *  Example of how to encode the UUID `0x180a01af` into advertising data.
 *
 *  @code
 *  BLE_DATA_BYTES(BLE_DATA_UUID32_ALL, BLE_UUID_32_ENCODE(0x180a01af))
 *  @endcode
 *
 * @param w32 UUID value (32-bits)
 *
 * @return The comma separated values for UUID 32 value that
 *         may be used directly as an argument for @ref BLE_DATA_BYTES.
 */
#define BLE_UUID_32_ENCODE(w32) BLE_BYTES_LIST_LE32(w32)

/**
 *  @brief Recommended length of user string buffer for Bluetooth UUID.
 *
 *  @details The recommended length guarantee the output of UUID
 *  conversion will not lose valuable information about the UUID being
 *  processed. If the length of the UUID is known the string can be shorter.
 */
#define BLE_UUID_STR_LEN 37

/**
 *  @brief Generic Access UUID value
 */
#define BLE_UUID_GAP_VAL 0x1800
/**
 *  @brief Generic Access
 */
#define BLE_UUID_GAP \
    BLE_UUID_DECLARE_16(BLE_UUID_GAP_VAL)
/**
 *  @brief Generic attribute UUID value
 */
#define BLE_UUID_GATT_VAL 0x1801
/**
 *  @brief Generic Attribute
 */
#define BLE_UUID_GATT \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_VAL)
/**
 *  @brief Immediate Alert Service UUID value
 */
#define BLE_UUID_IAS_VAL 0x1802
/**
 *  @brief Immediate Alert Service
 */
#define BLE_UUID_IAS \
    BLE_UUID_DECLARE_16(BLE_UUID_IAS_VAL)
/**
 *  @brief Link Loss Service UUID value
 */
#define BLE_UUID_LLS_VAL 0x1803
/**
 *  @brief Link Loss Service
 */
#define BLE_UUID_LLS \
    BLE_UUID_DECLARE_16(BLE_UUID_LLS_VAL)
/**
 *  @brief Tx Power Service UUID value
 */
#define BLE_UUID_TPS_VAL 0x1804
/**
 *  @brief Tx Power Service
 */
#define BLE_UUID_TPS \
    BLE_UUID_DECLARE_16(BLE_UUID_TPS_VAL)
/**
 *  @brief Current Time Service UUID value
 */
#define BLE_UUID_CTS_VAL 0x1805
/**
 *  @brief Current Time Service
 */
#define BLE_UUID_CTS \
    BLE_UUID_DECLARE_16(BLE_UUID_CTS_VAL)
/**
 *  @brief Reference Time Update Service UUID value
 */
#define BLE_UUID_RTUS_VAL 0x1806
/**
 *  @brief Reference Time Update Service
 */
#define BLE_UUID_RTUS \
    BLE_UUID_DECLARE_16(BLE_UUID_RTUS_VAL)
/**
 *  @brief Next DST Change Service UUID value
 */
#define BLE_UUID_NDSTS_VAL 0x1807
/**
 *  @brief Next DST Change Service
 */
#define BLE_UUID_NDSTS \
    BLE_UUID_DECLARE_16(BLE_UUID_NDSTS_VAL)
/**
 *  @brief Glucose Service UUID value
 */
#define BLE_UUID_GS_VAL 0x1808
/**
 *  @brief Glucose Service
 */
#define BLE_UUID_GS \
    BLE_UUID_DECLARE_16(BLE_UUID_GS_VAL)
/**
 *  @brief Health Thermometer Service UUID value
 */
#define BLE_UUID_HTS_VAL 0x1809
/**
 *  @brief Health Thermometer Service
 */
#define BLE_UUID_HTS \
    BLE_UUID_DECLARE_16(BLE_UUID_HTS_VAL)
/**
 *  @brief Device Information Service UUID value
 */
#define BLE_UUID_DIS_VAL 0x180a
/**
 *  @brief Device Information Service
 */
#define BLE_UUID_DIS \
    BLE_UUID_DECLARE_16(BLE_UUID_DIS_VAL)
/**
 *  @brief Network Availability Service UUID value
 */
#define BLE_UUID_NAS_VAL 0x180b
/**
 *  @brief Network Availability Service
 */
#define BLE_UUID_NAS \
    BLE_UUID_DECLARE_16(BLE_UUID_NAS_VAL)
/**
 *  @brief Watchdog Service UUID value
 */
#define BLE_UUID_WDS_VAL 0x180c
/**
 *  @brief Watchdog Service
 */
#define BLE_UUID_WDS \
    BLE_UUID_DECLARE_16(BLE_UUID_WDS_VAL)
/**
 *  @brief Heart Rate Service UUID value
 */
#define BLE_UUID_HRS_VAL 0x180d
/**
 *  @brief Heart Rate Service
 */
#define BLE_UUID_HRS \
    BLE_UUID_DECLARE_16(BLE_UUID_HRS_VAL)
/**
 *  @brief Phone Alert Service UUID value
 */
#define BLE_UUID_PAS_VAL 0x180e
/**
 *  @brief Phone Alert Service
 */
#define BLE_UUID_PAS \
    BLE_UUID_DECLARE_16(BLE_UUID_PAS_VAL)
/**
 *  @brief Battery Service UUID value
 */
#define BLE_UUID_BAS_VAL 0x180f
/**
 *  @brief Battery Service
 */
#define BLE_UUID_BAS \
    BLE_UUID_DECLARE_16(BLE_UUID_BAS_VAL)
/**
 *  @brief Blood Pressure Service UUID value
 */
#define BLE_UUID_BPS_VAL 0x1810
/**
 *  @brief Blood Pressure Service
 */
#define BLE_UUID_BPS \
    BLE_UUID_DECLARE_16(BLE_UUID_BPS_VAL)
/**
 *  @brief Alert Notification Service UUID value
 */
#define BLE_UUID_ANS_VAL 0x1811
/**
 *  @brief Alert Notification Service
 */
#define BLE_UUID_ANS \
    BLE_UUID_DECLARE_16(BLE_UUID_ANS_VAL)
/**
 *  @brief HID Service UUID value
 */
#define BLE_UUID_HIDS_VAL 0x1812
/**
 *  @brief HID Service
 */
#define BLE_UUID_HIDS \
    BLE_UUID_DECLARE_16(BLE_UUID_HIDS_VAL)
/**
 *  @brief Scan Parameters Service UUID value
 */
#define BLE_UUID_SPS_VAL 0x1813
/**
 *  @brief Scan Parameters Service
 */
#define BLE_UUID_SPS \
    BLE_UUID_DECLARE_16(BLE_UUID_SPS_VAL)
/**
 *  @brief Running Speed and Cadence Service UUID value
 */
#define BLE_UUID_RSCS_VAL 0x1814
/**
 *  @brief Running Speed and Cadence Service
 */
#define BLE_UUID_RSCS \
    BLE_UUID_DECLARE_16(BLE_UUID_RSCS_VAL)
/**
 *  @brief Automation IO Service UUID value
 */
#define BLE_UUID_AIOS_VAL 0x1815
/**
 *  @brief Automation IO Service
 */
#define BLE_UUID_AIOS \
    BLE_UUID_DECLARE_16(BLE_UUID_AIOS_VAL)
/**
 *  @brief Cycling Speed and Cadence Service UUID value
 */
#define BLE_UUID_CSC_VAL 0x1816
/**
 *  @brief Cycling Speed and Cadence Service
 */
#define BLE_UUID_CSC \
    BLE_UUID_DECLARE_16(BLE_UUID_CSC_VAL)
/**
 *  @brief Cycling Power Service UUID value
 */
#define BLE_UUID_CPS_VAL 0x1818
/**
 *  @brief Cycling Power Service
 */
#define BLE_UUID_CPS \
    BLE_UUID_DECLARE_16(BLE_UUID_CPS_VAL)
/**
 *  @brief Location and Navigation Service UUID value
 */
#define BLE_UUID_LNS_VAL 0x1819
/**
 *  @brief Location and Navigation Service
 */
#define BLE_UUID_LNS \
    BLE_UUID_DECLARE_16(BLE_UUID_LNS_VAL)
/**
 *  @brief Environmental Sensing Service UUID value
 */
#define BLE_UUID_ESS_VAL 0x181a
/**
 *  @brief Environmental Sensing Service
 */
#define BLE_UUID_ESS \
    BLE_UUID_DECLARE_16(BLE_UUID_ESS_VAL)
/**
 *  @brief Body Composition Service UUID value
 */
#define BLE_UUID_BCS_VAL 0x181b
/**
 *  @brief Body Composition Service
 */
#define BLE_UUID_BCS \
    BLE_UUID_DECLARE_16(BLE_UUID_BCS_VAL)
/**
 *  @brief User Data Service UUID value
 */
#define BLE_UUID_UDS_VAL 0x181c
/**
 *  @brief User Data Service
 */
#define BLE_UUID_UDS \
    BLE_UUID_DECLARE_16(BLE_UUID_UDS_VAL)
/**
 *  @brief Weight Scale Service UUID value
 */
#define BLE_UUID_WSS_VAL 0x181d
/**
 *  @brief Weight Scale Service
 */
#define BLE_UUID_WSS \
    BLE_UUID_DECLARE_16(BLE_UUID_WSS_VAL)
/**
 *  @brief Bond Management Service UUID value
 */
#define BLE_UUID_BMS_VAL 0x181e
/**
 *  @brief Bond Management Service
 */
#define BLE_UUID_BMS \
    BLE_UUID_DECLARE_16(BLE_UUID_BMS_VAL)
/**
 *  @brief Continuous Glucose Monitoring Service UUID value
 */
#define BLE_UUID_CGMS_VAL 0x181f
/**
 *  @brief Continuous Glucose Monitoring Service
 */
#define BLE_UUID_CGMS \
    BLE_UUID_DECLARE_16(BLE_UUID_CGMS_VAL)
/**
 *  @brief IP Support Service UUID value
 */
#define BLE_UUID_IPSS_VAL 0x1820
/**
 *  @brief IP Support Service
 */
#define BLE_UUID_IPSS \
    BLE_UUID_DECLARE_16(BLE_UUID_IPSS_VAL)
/**
 *  @brief Indoor Positioning Service UUID value
 */
#define BLE_UUID_IPS_VAL 0x1821
/**
 *  @brief Indoor Positioning Service
 */
#define BLE_UUID_IPS \
    BLE_UUID_DECLARE_16(BLE_UUID_IPS_VAL)
/**
 *  @brief Pulse Oximeter Service UUID value
 */
#define BLE_UUID_POS_VAL 0x1822
/**
 *  @brief Pulse Oximeter Service
 */
#define BLE_UUID_POS \
    BLE_UUID_DECLARE_16(BLE_UUID_POS_VAL)
/**
 *  @brief HTTP Proxy Service UUID value
 */
#define BLE_UUID_HPS_VAL 0x1823
/**
 *  @brief HTTP Proxy Service
 */
#define BLE_UUID_HPS \
    BLE_UUID_DECLARE_16(BLE_UUID_HPS_VAL)
/**
 *  @brief Transport Discovery Service UUID value
 */
#define BLE_UUID_TDS_VAL 0x1824
/**
 *  @brief Transport Discovery Service
 */
#define BLE_UUID_TDS \
    BLE_UUID_DECLARE_16(BLE_UUID_TDS_VAL)
/**
 *  @brief Object Transfer Service UUID value
 */
#define BLE_UUID_OTS_VAL 0x1825
/**
 *  @brief Object Transfer Service
 */
#define BLE_UUID_OTS \
    BLE_UUID_DECLARE_16(BLE_UUID_OTS_VAL)
/**
 *  @brief Fitness Machine Service UUID value
 */
#define BLE_UUID_FMS_VAL 0x1826
/**
 *  @brief Fitness Machine Service
 */
#define BLE_UUID_FMS \
    BLE_UUID_DECLARE_16(BLE_UUID_FMS_VAL)
/**
 *  @brief Mesh Provisioning Service UUID value
 */
#define BLE_UUID_MESH_PROV_VAL 0x1827
/**
 *  @brief Mesh Provisioning Service
 */
#define BLE_UUID_MESH_PROV \
    BLE_UUID_DECLARE_16(BLE_UUID_MESH_PROV_VAL)
/**
 *  @brief Mesh Proxy Service UUID value
 */
#define BLE_UUID_MESH_PROXY_VAL 0x1828
/**
 *  @brief Mesh Proxy Service
 */
#define BLE_UUID_MESH_PROXY \
    BLE_UUID_DECLARE_16(BLE_UUID_MESH_PROXY_VAL)
/**
 *  @brief Proxy Solicitation UUID value
 */
#define BLE_UUID_MESH_PROXY_SOLICITATION_VAL 0x1859
/**
 *  @brief Reconnection Configuration Service UUID value
 */
#define BLE_UUID_RCSRV_VAL 0x1829
/**
 *  @brief Reconnection Configuration Service
 */
#define BLE_UUID_RCSRV \
    BLE_UUID_DECLARE_16(BLE_UUID_RCSRV_VAL)
/**
 *  @brief Insulin Delivery Service UUID value
 */
#define BLE_UUID_IDS_VAL 0x183a
/**
 *  @brief Insulin Delivery Service
 */
#define BLE_UUID_IDS \
    BLE_UUID_DECLARE_16(BLE_UUID_IDS_VAL)
/**
 *  @brief Binary Sensor Service UUID value
 */
#define BLE_UUID_BSS_VAL 0x183b
/**
 *  @brief Binary Sensor Service
 */
#define BLE_UUID_BSS \
    BLE_UUID_DECLARE_16(BLE_UUID_BSS_VAL)
/**
 *  @brief Emergency Configuration Service UUID value
 */
#define BLE_UUID_ECS_VAL 0x183c
/**
 *  @brief Emergency Configuration Service
 */
#define BLE_UUID_ECS \
    BLE_UUID_DECLARE_16(BLE_UUID_ECS_VAL)
/**
 *  @brief Authorization Control Service UUID value
 */
#define BLE_UUID_ACLS_VAL 0x183d
/**
 *  @brief Authorization Control Service
 */
#define BLE_UUID_ACLS \
    BLE_UUID_DECLARE_16(BLE_UUID_ACLS_VAL)
/**
 *  @brief Physical Activity Monitor Service UUID value
 */
#define BLE_UUID_PAMS_VAL 0x183e
/**
 *  @brief Physical Activity Monitor Service
 */
#define BLE_UUID_PAMS \
    BLE_UUID_DECLARE_16(BLE_UUID_PAMS_VAL)
/**
 *  @brief Elapsed Time Service UUID value
 */
#define BLE_UUID_ETS_VAL 0x183f
/**
 *  @brief Elapsed Time Service
 */
#define BLE_UUID_ETS \
    BLE_UUID_DECLARE_16(BLE_UUID_ETS_VAL)
/**
 *  @brief Audio Input Control Service UUID value
 */
#define BLE_UUID_AICS_VAL 0x1843
/**
 *  @brief Audio Input Control Service
 */
#define BLE_UUID_AICS \
    BLE_UUID_DECLARE_16(BLE_UUID_AICS_VAL)
/**
 *  @brief Volume Control Service UUID value
 */
#define BLE_UUID_VCS_VAL 0x1844
/**
 *  @brief Volume Control Service
 */
#define BLE_UUID_VCS \
    BLE_UUID_DECLARE_16(BLE_UUID_VCS_VAL)
/**
 *  @brief Volume Offset Control Service UUID value
 */
#define BLE_UUID_VOCS_VAL 0x1845
/**
 *  @brief Volume Offset Control Service
 */
#define BLE_UUID_VOCS \
    BLE_UUID_DECLARE_16(BLE_UUID_VOCS_VAL)
/**
 *  @brief Coordinated Set Identification Service UUID value
 */
#define BLE_UUID_CSIS_VAL 0x1846
/**
 *  @brief Coordinated Set Identification Service
 */
#define BLE_UUID_CSIS \
    BLE_UUID_DECLARE_16(BLE_UUID_CSIS_VAL)
/**
 *  @brief Device Time Service UUID value
 */
#define BLE_UUID_DTS_VAL 0x1847
/**
 *  @brief Device Time Service
 */
#define BLE_UUID_DTS \
    BLE_UUID_DECLARE_16(BLE_UUID_DTS_VAL)
/**
 *  @brief Media Control Service UUID value
 */
#define BLE_UUID_MCS_VAL 0x1848
/**
 *  @brief Media Control Service
 */
#define BLE_UUID_MCS \
    BLE_UUID_DECLARE_16(BLE_UUID_MCS_VAL)
/**
 *  @brief Generic Media Control Service UUID value
 */
#define BLE_UUID_GMCS_VAL 0x1849
/**
 *  @brief Generic Media Control Service
 */
#define BLE_UUID_GMCS \
    BLE_UUID_DECLARE_16(BLE_UUID_GMCS_VAL)
/**
 *  @brief Constant Tone Extension Service UUID value
 */
#define BLE_UUID_CTES_VAL 0x184a
/**
 *  @brief Constant Tone Extension Service
 */
#define BLE_UUID_CTES \
    BLE_UUID_DECLARE_16(BLE_UUID_CTES_VAL)
/**
 *  @brief Telephone Bearer Service UUID value
 */
#define BLE_UUID_TBS_VAL 0x184b
/**
 *  @brief Telephone Bearer Service
 */
#define BLE_UUID_TBS \
    BLE_UUID_DECLARE_16(BLE_UUID_TBS_VAL)
/**
 *  @brief Generic Telephone Bearer Service UUID value
 */
#define BLE_UUID_GTBS_VAL 0x184c
/**
 *  @brief Generic Telephone Bearer Service
 */
#define BLE_UUID_GTBS \
    BLE_UUID_DECLARE_16(BLE_UUID_GTBS_VAL)
/**
 *  @brief Microphone Control Service UUID value
 */
#define BLE_UUID_MICS_VAL 0x184d
/**
 *  @brief Microphone Control Service
 */
#define BLE_UUID_MICS \
    BLE_UUID_DECLARE_16(BLE_UUID_MICS_VAL)
/**
 *  @brief Audio Stream Control Service UUID value
 */
#define BLE_UUID_ASCS_VAL 0x184e
/**
 *  @brief Audio Stream Control Service
 */
#define BLE_UUID_ASCS \
    BLE_UUID_DECLARE_16(BLE_UUID_ASCS_VAL)
/**
 *  @brief Broadcast Audio Scan Service UUID value
 */
#define BLE_UUID_BASS_VAL 0x184f
/**
 *  @brief Broadcast Audio Scan Service
 */
#define BLE_UUID_BASS \
    BLE_UUID_DECLARE_16(BLE_UUID_BASS_VAL)
/**
 *  @brief Published Audio Capabilities Service UUID value
 */
#define BLE_UUID_PACS_VAL 0x1850
/**
 *  @brief Published Audio Capabilities Service
 */
#define BLE_UUID_PACS \
    BLE_UUID_DECLARE_16(BLE_UUID_PACS_VAL)
/**
 *  @brief Basic Audio Announcement Service UUID value
 */
#define BLE_UUID_BASIC_AUDIO_VAL 0x1851
/**
 *  @brief Basic Audio Announcement Service
 */
#define BLE_UUID_BASIC_AUDIO \
    BLE_UUID_DECLARE_16(BLE_UUID_BASIC_AUDIO_VAL)
/**
 *  @brief Broadcast Audio Announcement Service UUID value
 */
#define BLE_UUID_BROADCAST_AUDIO_VAL 0x1852
/**
 *  @brief Broadcast Audio Announcement Service
 */
#define BLE_UUID_BROADCAST_AUDIO \
    BLE_UUID_DECLARE_16(BLE_UUID_BROADCAST_AUDIO_VAL)
/**
 *  @brief Common Audio Service UUID value
 */
#define BLE_UUID_CAS_VAL 0x1853
/**
 *  @brief Common Audio Service
 */
#define BLE_UUID_CAS \
    BLE_UUID_DECLARE_16(BLE_UUID_CAS_VAL)
/**
 *  @brief Hearing Access Service UUID value
 */
#define BLE_UUID_HAS_VAL 0x1854
/**
 *  @brief Hearing Access Service
 */
#define BLE_UUID_HAS \
    BLE_UUID_DECLARE_16(BLE_UUID_HAS_VAL)
/**
 *  @brief Telephony and Media Audio Service UUID value
 */
#define BLE_UUID_TMAS_VAL 0x1855
/**
 *  @brief Telephony and Media Audio Service
 */
#define BLE_UUID_TMAS \
    BLE_UUID_DECLARE_16(BLE_UUID_TMAS_VAL)
/**
 *  @brief Public Broadcast Announcement Service UUID value
 */
#define BLE_UUID_PBA_VAL 0x1856
/**
 *  @brief Public Broadcast Announcement Service
 */
#define BLE_UUID_PBA \
    BLE_UUID_DECLARE_16(BLE_UUID_PBA_VAL)
/**
 *  @brief GATT Primary Service UUID value
 */
#define BLE_UUID_GATT_PRIMARY_VAL 0x2800
/**
 *  @brief GATT Primary Service
 */
#define BLE_UUID_GATT_PRIMARY \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_PRIMARY_VAL)
/**
 *  @brief GATT Secondary Service UUID value
 */
#define BLE_UUID_GATT_SECONDARY_VAL 0x2801
/**
 *  @brief GATT Secondary Service
 */
#define BLE_UUID_GATT_SECONDARY \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_SECONDARY_VAL)
/**
 *  @brief GATT Include Service UUID value
 */
#define BLE_UUID_GATT_INCLUDE_VAL 0x2802
/**
 *  @brief GATT Include Service
 */
#define BLE_UUID_GATT_INCLUDE \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_INCLUDE_VAL)
/**
 *  @brief GATT Characteristic UUID value
 */
#define BLE_UUID_GATT_CHRC_VAL 0x2803
/**
 *  @brief GATT Characteristic
 */
#define BLE_UUID_GATT_CHRC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CHRC_VAL)
/**
 *  @brief GATT Characteristic Extended Properties UUID value
 */
#define BLE_UUID_GATT_CEP_VAL 0x2900
/**
 *  @brief GATT Characteristic Extended Properties
 */
#define BLE_UUID_GATT_CEP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CEP_VAL)
/**
 *  @brief GATT Characteristic User Description UUID value
 */
#define BLE_UUID_GATT_CUD_VAL 0x2901
/**
 *  @brief GATT Characteristic User Description
 */
#define BLE_UUID_GATT_CUD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CUD_VAL)
/**
 *  @brief GATT Client Characteristic Configuration UUID value
 */
#define BLE_UUID_GATT_CCC_VAL 0x2902
/**
 *  @brief GATT Client Characteristic Configuration
 */
#define BLE_UUID_GATT_CCC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CCC_VAL)
/**
 *  @brief GATT Server Characteristic Configuration UUID value
 */
#define BLE_UUID_GATT_SCC_VAL 0x2903
/**
 *  @brief GATT Server Characteristic Configuration
 */
#define BLE_UUID_GATT_SCC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_SCC_VAL)
/**
 *  @brief GATT Characteristic Presentation Format UUID value
 */
#define BLE_UUID_GATT_CPF_VAL 0x2904
/**
 *  @brief GATT Characteristic Presentation Format
 */
#define BLE_UUID_GATT_CPF \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CPF_VAL)
/**
 *  @brief GATT Characteristic Aggregated Format UUID value
 */
#define BLE_UUID_GATT_CAF_VAL 0x2905
/**
 *  @brief GATT Characteristic Aggregated Format
 */
#define BLE_UUID_GATT_CAF \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CAF_VAL)
/**
 *  @brief Valid Range Descriptor UUID value
 */
#define BLE_UUID_VALID_RANGE_VAL 0x2906
/**
 *  @brief Valid Range Descriptor
 */
#define BLE_UUID_VALID_RANGE \
    BLE_UUID_DECLARE_16(BLE_UUID_VALID_RANGE_VAL)
/**
 *  @brief HID External Report Descriptor UUID value
 */
#define BLE_UUID_HIDS_EXT_REPORT_VAL 0x2907
/**
 *  @brief HID External Report Descriptor
 */
#define BLE_UUID_HIDS_EXT_REPORT \
    BLE_UUID_DECLARE_16(BLE_UUID_HIDS_EXT_REPORT_VAL)
/**
 *  @brief HID Report Reference Descriptor UUID value
 */
#define BLE_UUID_HIDS_REPORT_REF_VAL 0x2908
/**
 *  @brief HID Report Reference Descriptor
 */
#define BLE_UUID_HIDS_REPORT_REF \
    BLE_UUID_DECLARE_16(BLE_UUID_HIDS_REPORT_REF_VAL)
/**
 *  @brief Value Trigger Setting Descriptor UUID value
 */
#define BLE_UUID_VAL_TRIGGER_SETTING_VAL 0x290a
/**
 *  @brief Value Trigger Setting Descriptor
 */
#define BLE_UUID_VAL_TRIGGER_SETTING \
    BLE_UUID_DECLARE_16(BLE_UUID_VAL_TRIGGER_SETTING_VAL)
/**
 *  @brief Environmental Sensing Configuration Descriptor UUID value
 */
#define BLE_UUID_ES_CONFIGURATION_VAL 0x290b
/**
 *  @brief Environmental Sensing Configuration Descriptor
 */
#define BLE_UUID_ES_CONFIGURATION \
    BLE_UUID_DECLARE_16(BLE_UUID_ES_CONFIGURATION_VAL)
/**
 *  @brief Environmental Sensing Measurement Descriptor UUID value
 */
#define BLE_UUID_ES_MEASUREMENT_VAL 0x290c
/**
 *  @brief Environmental Sensing Measurement Descriptor
 */
#define BLE_UUID_ES_MEASUREMENT \
    BLE_UUID_DECLARE_16(BLE_UUID_ES_MEASUREMENT_VAL)
/**
 *  @brief Environmental Sensing Trigger Setting Descriptor UUID value
 */
#define BLE_UUID_ES_TRIGGER_SETTING_VAL 0x290d
/**
 *  @brief Environmental Sensing Trigger Setting Descriptor
 */
#define BLE_UUID_ES_TRIGGER_SETTING \
    BLE_UUID_DECLARE_16(BLE_UUID_ES_TRIGGER_SETTING_VAL)
/**
 *  @brief Time Trigger Setting Descriptor UUID value
 */
#define BLE_UUID_TM_TRIGGER_SETTING_VAL 0x290e
/**
 *  @brief Time Trigger Setting Descriptor
 */
#define BLE_UUID_TM_TRIGGER_SETTING \
    BLE_UUID_DECLARE_16(BLE_UUID_TM_TRIGGER_SETTING_VAL)
/**
 *  @brief GAP Characteristic Device Name UUID value
 */
#define BLE_UUID_GAP_DEVICE_NAME_VAL 0x2a00
/**
 *  @brief GAP Characteristic Device Name
 */
#define BLE_UUID_GAP_DEVICE_NAME \
    BLE_UUID_DECLARE_16(BLE_UUID_GAP_DEVICE_NAME_VAL)
/**
 *  @brief GAP Characteristic Appearance UUID value
 */
#define BLE_UUID_GAP_APPEARANCE_VAL 0x2a01
/**
 *  @brief GAP Characteristic Appearance
 */
#define BLE_UUID_GAP_APPEARANCE \
    BLE_UUID_DECLARE_16(BLE_UUID_GAP_APPEARANCE_VAL)
/**
 *  @brief GAP Characteristic Peripheral Privacy Flag UUID value
 */
#define BLE_UUID_GAP_PPF_VAL 0x2a02
/**
 *  @brief GAP Characteristic Peripheral Privacy Flag
 */
#define BLE_UUID_GAP_PPF \
    BLE_UUID_DECLARE_16(BLE_UUID_GAP_PPF_VAL)
/**
 *  @brief GAP Characteristic Reconnection Address UUID value
 */
#define BLE_UUID_GAP_RA_VAL 0x2a03
/**
 *  @brief GAP Characteristic Reconnection Address
 */
#define BLE_UUID_GAP_RA \
    BLE_UUID_DECLARE_16(BLE_UUID_GAP_RA_VAL)
/**
 *  @brief GAP Characteristic Peripheral Preferred Connection Parameters UUID
 *         value
 */
#define BLE_UUID_GAP_PPCP_VAL 0x2a04
/**
 *  @brief GAP Characteristic Peripheral Preferred Connection Parameters
 */
#define BLE_UUID_GAP_PPCP \
    BLE_UUID_DECLARE_16(BLE_UUID_GAP_PPCP_VAL)
/**
 *  @brief GATT Characteristic Service Changed UUID value
 */
#define BLE_UUID_GATT_SC_VAL 0x2a05
/**
 *  @brief GATT Characteristic Service Changed
 */
#define BLE_UUID_GATT_SC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_SC_VAL)
/**
 *  @brief GATT Characteristic Alert Level UUID value
 */
#define BLE_UUID_ALERT_LEVEL_VAL  0x2a06
/**
 *  @brief GATT Characteristic Alert Level
 */
#define BLE_UUID_ALERT_LEVEL \
    BLE_UUID_DECLARE_16(BLE_UUID_ALERT_LEVEL_VAL)
/**
 *  @brief TPS Characteristic Tx Power Level UUID value
 */
#define BLE_UUID_TPS_TX_POWER_LEVEL_VAL 0x2a07
/**
 *  @brief TPS Characteristic Tx Power Level
 */
#define BLE_UUID_TPS_TX_POWER_LEVEL \
    BLE_UUID_DECLARE_16(BLE_UUID_TPS_TX_POWER_LEVEL_VAL)
/**
 *  @brief GATT Characteristic Date Time UUID value
 */
#define BLE_UUID_GATT_DT_VAL 0x2a08
/**
 *  @brief GATT Characteristic Date Time
 */
#define BLE_UUID_GATT_DT \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_DT_VAL)
/**
 *  @brief GATT Characteristic Day of Week UUID value
 */
#define BLE_UUID_GATT_DW_VAL 0x2a09
/**
 *  @brief GATT Characteristic Day of Week
 */
#define BLE_UUID_GATT_DW \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_DW_VAL)
/**
 *  @brief GATT Characteristic Day Date Time UUID value
 */
#define BLE_UUID_GATT_DDT_VAL 0x2a0a
/**
 *  @brief GATT Characteristic Day Date Time
 */
#define BLE_UUID_GATT_DDT \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_DDT_VAL)
/**
 *  @brief GATT Characteristic Exact Time 256 UUID value
 */
#define BLE_UUID_GATT_ET256_VAL 0x2a0c
/**
 *  @brief GATT Characteristic Exact Time 256
 */
#define BLE_UUID_GATT_ET256 \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ET256_VAL)
/**
 *  @brief GATT Characteristic DST Offset UUID value
 */
#define BLE_UUID_GATT_DST_VAL 0x2a0d
/**
 *  @brief GATT Characteristic DST Offset
 */
#define BLE_UUID_GATT_DST \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_DST_VAL)
/**
 *  @brief GATT Characteristic Time Zone UUID value
 */
#define BLE_UUID_GATT_TZ_VAL 0x2a0e
/**
 *  @brief GATT Characteristic Time Zone
 */
#define BLE_UUID_GATT_TZ \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TZ_VAL)
/**
 *  @brief GATT Characteristic Local Time Information UUID value
 */
#define BLE_UUID_GATT_LTI_VAL 0x2a0f
/**
 *  @brief GATT Characteristic Local Time Information
 */
#define BLE_UUID_GATT_LTI \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_LTI_VAL)
/**
 *  @brief GATT Characteristic Time with DST UUID value
 */
#define BLE_UUID_GATT_TDST_VAL 0x2a11
/**
 *  @brief GATT Characteristic Time with DST
 */
#define BLE_UUID_GATT_TDST \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TDST_VAL)
/**
 *  @brief GATT Characteristic Time Accuracy UUID value
 */
#define BLE_UUID_GATT_TA_VAL 0x2a12
/**
 *  @brief GATT Characteristic Time Accuracy
 */
#define BLE_UUID_GATT_TA \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TA_VAL)
/**
 *  @brief GATT Characteristic Time Source UUID value
 */
#define BLE_UUID_GATT_TS_VAL 0x2a13
/**
 *  @brief GATT Characteristic Time Source
 */
#define BLE_UUID_GATT_TS \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TS_VAL)
/**
 *  @brief GATT Characteristic Reference Time Information UUID value
 */
#define BLE_UUID_GATT_RTI_VAL 0x2a14
/**
 *  @brief GATT Characteristic Reference Time Information
 */
#define BLE_UUID_GATT_RTI \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_RTI_VAL)
/**
 *  @brief GATT Characteristic Time Update Control Point UUID value
 */
#define BLE_UUID_GATT_TUCP_VAL 0x2a16
/**
 *  @brief GATT Characteristic Time Update Control Point
 */
#define BLE_UUID_GATT_TUCP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TUCP_VAL)
/**
 *  @brief GATT Characteristic Time Update State UUID value
 */
#define BLE_UUID_GATT_TUS_VAL 0x2a17
/**
 *  @brief GATT Characteristic Time Update State
 */
#define BLE_UUID_GATT_TUS \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TUS_VAL)
/**
 *  @brief GATT Characteristic Glucose Measurement UUID value
 */
#define BLE_UUID_GATT_GM_VAL 0x2a18
/**
 *  @brief GATT Characteristic Glucose Measurement
 */
#define BLE_UUID_GATT_GM \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_GM_VAL)
/**
 *  @brief BAS Characteristic Battery Level UUID value
 */
#define BLE_UUID_BAS_BATTERY_LEVEL_VAL 0x2a19
/**
 *  @brief BAS Characteristic Battery Level
 */
#define BLE_UUID_BAS_BATTERY_LEVEL \
    BLE_UUID_DECLARE_16(BLE_UUID_BAS_BATTERY_LEVEL_VAL)
/**
 *  @brief BAS Characteristic Battery Power State UUID value
 */
#define BLE_UUID_BAS_BATTERY_POWER_STATE_VAL 0x2a1a
/**
 *  @brief BAS Characteristic Battery Power State
 */
#define BLE_UUID_BAS_BATTERY_POWER_STATE \
    BLE_UUID_DECLARE_16(BLE_UUID_BAS_BATTERY_POWER_STATE_VAL)
/**
 *  @brief BAS Characteristic Battery Level StateUUID value
 */
#define BLE_UUID_BAS_BATTERY_LEVEL_STATE_VAL 0x2a1b
/**
 *  @brief BAS Characteristic Battery Level State
 */
#define BLE_UUID_BAS_BATTERY_LEVEL_STATE \
    BLE_UUID_DECLARE_16(BLE_UUID_BAS_BATTERY_LEVEL_STATE_VAL)
/**
 *  @brief HTS Characteristic Temperature Measurement UUID value
 */
#define BLE_UUID_HTS_MEASUREMENT_VAL 0x2a1c
/**
 *  @brief HTS Characteristic Temperature Measurement Value
 */
#define BLE_UUID_HTS_MEASUREMENT \
    BLE_UUID_DECLARE_16(BLE_UUID_HTS_MEASUREMENT_VAL)
/**
 *  @brief HTS Characteristic Temperature Type UUID value
 */
#define BLE_UUID_HTS_TEMP_TYP_VAL 0x2a1d
/**
 *  @brief HTS Characteristic Temperature Type
 */
#define BLE_UUID_HTS_TEMP_TYP \
    BLE_UUID_DECLARE_16(BLE_UUID_HTS_TEMP_TYP_VAL)
/**
 *  @brief HTS Characteristic Intermediate Temperature UUID value
 */
#define BLE_UUID_HTS_TEMP_INT_VAL 0x2a1e
/**
 *  @brief HTS Characteristic Intermediate Temperature
 */
#define BLE_UUID_HTS_TEMP_INT \
    BLE_UUID_DECLARE_16(BLE_UUID_HTS_TEMP_INT_VAL)
/**
 *  @brief HTS Characteristic Temperature Celsius UUID value
 */
#define BLE_UUID_HTS_TEMP_C_VAL 0x2a1f
/**
 *  @brief HTS Characteristic Temperature Celsius
 */
#define BLE_UUID_HTS_TEMP_C \
    BLE_UUID_DECLARE_16(BLE_UUID_HTS_TEMP_C_VAL)
/**
 *  @brief HTS Characteristic Temperature Fahrenheit UUID value
 */
#define BLE_UUID_HTS_TEMP_F_VAL 0x2a20
/**
 *  @brief HTS Characteristic Temperature Fahrenheit
 */
#define BLE_UUID_HTS_TEMP_F \
    BLE_UUID_DECLARE_16(BLE_UUID_HTS_TEMP_F_VAL)
/**
 *  @brief HTS Characteristic Measurement Interval UUID value
 */
#define BLE_UUID_HTS_INTERVAL_VAL 0x2a21
/**
 *  @brief HTS Characteristic Measurement Interval
 */
#define BLE_UUID_HTS_INTERVAL \
    BLE_UUID_DECLARE_16(BLE_UUID_HTS_INTERVAL_VAL)
/**
 *  @brief HID Characteristic Boot Keyboard Input Report UUID value
 */
#define BLE_UUID_HIDS_BOOT_KB_IN_REPORT_VAL 0x2a22
/**
 *  @brief HID Characteristic Boot Keyboard Input Report
 */
#define BLE_UUID_HIDS_BOOT_KB_IN_REPORT \
    BLE_UUID_DECLARE_16(BLE_UUID_HIDS_BOOT_KB_IN_REPORT_VAL)
/**
 *  @brief DIS Characteristic System ID UUID value
 */
#define BLE_UUID_DIS_SYSTEM_ID_VAL 0x2a23
/**
 *  @brief DIS Characteristic System ID
 */
#define BLE_UUID_DIS_SYSTEM_ID \
    BLE_UUID_DECLARE_16(BLE_UUID_DIS_SYSTEM_ID_VAL)
/**
 *  @brief DIS Characteristic Model Number String UUID value
 */
#define BLE_UUID_DIS_MODEL_NUMBER_VAL 0x2a24
/**
 *  @brief DIS Characteristic Model Number String
 */
#define BLE_UUID_DIS_MODEL_NUMBER \
    BLE_UUID_DECLARE_16(BLE_UUID_DIS_MODEL_NUMBER_VAL)
/**
 *  @brief DIS Characteristic Serial Number String UUID value
 */
#define BLE_UUID_DIS_SERIAL_NUMBER_VAL 0x2a25
/**
 *  @brief DIS Characteristic Serial Number String
 */
#define BLE_UUID_DIS_SERIAL_NUMBER \
    BLE_UUID_DECLARE_16(BLE_UUID_DIS_SERIAL_NUMBER_VAL)
/**
 *  @brief DIS Characteristic Firmware Revision String UUID value
 */
#define BLE_UUID_DIS_FIRMWARE_REVISION_VAL 0x2a26
/**
 *  @brief DIS Characteristic Firmware Revision String
 */
#define BLE_UUID_DIS_FIRMWARE_REVISION \
    BLE_UUID_DECLARE_16(BLE_UUID_DIS_FIRMWARE_REVISION_VAL)
/**
 *  @brief DIS Characteristic Hardware Revision String UUID value
 */
#define BLE_UUID_DIS_HARDWARE_REVISION_VAL 0x2a27
/**
 *  @brief DIS Characteristic Hardware Revision String
 */
#define BLE_UUID_DIS_HARDWARE_REVISION \
    BLE_UUID_DECLARE_16(BLE_UUID_DIS_HARDWARE_REVISION_VAL)
/**
 *  @brief DIS Characteristic Software Revision String UUID value
 */
#define BLE_UUID_DIS_SOFTWARE_REVISION_VAL 0x2a28
/**
 *  @brief DIS Characteristic Software Revision String
 */
#define BLE_UUID_DIS_SOFTWARE_REVISION \
    BLE_UUID_DECLARE_16(BLE_UUID_DIS_SOFTWARE_REVISION_VAL)
/**
 *  @brief DIS Characteristic Manufacturer Name String UUID Value
 */
#define BLE_UUID_DIS_MANUFACTURER_NAME_VAL 0x2a29
/**
 *  @brief DIS Characteristic Manufacturer Name String
 */
#define BLE_UUID_DIS_MANUFACTURER_NAME \
    BLE_UUID_DECLARE_16(BLE_UUID_DIS_MANUFACTURER_NAME_VAL)
/**
 *  @brief GATT Characteristic IEEE Regulatory Certification Data List UUID Value
 */
#define BLE_UUID_GATT_IEEE_RCDL_VAL 0x2a2a
/**
 *  @brief GATT Characteristic IEEE Regulatory Certification Data List
 */
#define BLE_UUID_GATT_IEEE_RCDL \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_IEEE_RCDL_VAL)
/**
 *  @brief CTS Characteristic Current Time UUID value
 */
#define BLE_UUID_CTS_CURRENT_TIME_VAL 0x2a2b
/**
 *  @brief CTS Characteristic Current Time
 */
#define BLE_UUID_CTS_CURRENT_TIME \
    BLE_UUID_DECLARE_16(BLE_UUID_CTS_CURRENT_TIME_VAL)
/**
 *  @brief Magnetic Declination Characteristic UUID value
 */
#define BLE_UUID_MAGN_DECLINATION_VAL 0x2a2c
/**
 *  @brief Magnetic Declination Characteristic
 */
#define BLE_UUID_MAGN_DECLINATION \
    BLE_UUID_DECLARE_16(BLE_UUID_MAGN_DECLINATION_VAL)
/**
 *  @brief GATT Characteristic Legacy Latitude UUID Value
 */
#define BLE_UUID_GATT_LLAT_VAL 0x2a2d
/**
 *  @brief GATT Characteristic Legacy Latitude
 */
#define BLE_UUID_GATT_LLAT \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_LLAT_VAL)
/**
 *  @brief GATT Characteristic Legacy Longitude UUID Value
 */
#define BLE_UUID_GATT_LLON_VAL 0x2a2e
/**
 *  @brief GATT Characteristic Legacy Longitude
 */
#define BLE_UUID_GATT_LLON \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_LLON_VAL)
/**
 *  @brief GATT Characteristic Position 2D UUID Value
 */
#define BLE_UUID_GATT_POS_2D_VAL 0x2a2f
/**
 *  @brief GATT Characteristic Position 2D
 */
#define BLE_UUID_GATT_POS_2D \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_POS_2D_VAL)
/**
 *  @brief GATT Characteristic Position 3D UUID Value
 */
#define BLE_UUID_GATT_POS_3D_VAL 0x2a30
/**
 *  @brief GATT Characteristic Position 3D
 */
#define BLE_UUID_GATT_POS_3D \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_POS_3D_VAL)
/**
 *  @brief GATT Characteristic Scan Refresh UUID Value
 */
#define BLE_UUID_GATT_SR_VAL 0x2a31
/**
 *  @brief GATT Characteristic Scan Refresh
 */
#define BLE_UUID_GATT_SR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_SR_VAL)
/**
 *  @brief HID Boot Keyboard Output Report Characteristic UUID value
 */
#define BLE_UUID_HIDS_BOOT_KB_OUT_REPORT_VAL 0x2a32
/**
 *  @brief HID Boot Keyboard Output Report Characteristic
 */
#define BLE_UUID_HIDS_BOOT_KB_OUT_REPORT \
    BLE_UUID_DECLARE_16(BLE_UUID_HIDS_BOOT_KB_OUT_REPORT_VAL)
/**
 *  @brief HID Boot Mouse Input Report Characteristic UUID value
 */
#define BLE_UUID_HIDS_BOOT_MOUSE_IN_REPORT_VAL 0x2a33
/**
 *  @brief HID Boot Mouse Input Report Characteristic
 */
#define BLE_UUID_HIDS_BOOT_MOUSE_IN_REPORT \
    BLE_UUID_DECLARE_16(BLE_UUID_HIDS_BOOT_MOUSE_IN_REPORT_VAL)
/**
 *  @brief GATT Characteristic Glucose Measurement Context UUID Value
 */
#define BLE_UUID_GATT_GMC_VAL 0x2a34
/**
 *  @brief GATT Characteristic Glucose Measurement Context
 */
#define BLE_UUID_GATT_GMC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_GMC_VAL)
/**
 *  @brief GATT Characteristic Blood Pressure Measurement UUID Value
 */
#define BLE_UUID_GATT_BPM_VAL 0x2a35
/**
 *  @brief GATT Characteristic Blood Pressure Measurement
 */
#define BLE_UUID_GATT_BPM \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_BPM_VAL)
/**
 *  @brief GATT Characteristic Intermediate Cuff Pressure UUID Value
 */
#define BLE_UUID_GATT_ICP_VAL 0x2a36
/**
 *  @brief GATT Characteristic Intermediate Cuff Pressure
 */
#define BLE_UUID_GATT_ICP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ICP_VAL)
/**
 *  @brief HRS Characteristic Measurement Interval UUID value
 */
#define BLE_UUID_HRS_MEASUREMENT_VAL 0x2a37
/**
 *  @brief HRS Characteristic Measurement Interval
 */
#define BLE_UUID_HRS_MEASUREMENT \
    BLE_UUID_DECLARE_16(BLE_UUID_HRS_MEASUREMENT_VAL)
/**
 *  @brief HRS Characteristic Body Sensor Location
 */
#define BLE_UUID_HRS_BODY_SENSOR_VAL 0x2a38
/**
 *  @brief HRS Characteristic Control Point
 */
#define BLE_UUID_HRS_BODY_SENSOR \
    BLE_UUID_DECLARE_16(BLE_UUID_HRS_BODY_SENSOR_VAL)
/**
 *  @brief HRS Characteristic Control Point UUID value
 */
#define BLE_UUID_HRS_CONTROL_POINT_VAL 0x2a39
/**
 *  @brief HRS Characteristic Control Point
 */
#define BLE_UUID_HRS_CONTROL_POINT \
    BLE_UUID_DECLARE_16(BLE_UUID_HRS_CONTROL_POINT_VAL)
/**
 *  @brief GATT Characteristic Removable UUID Value
 */
#define BLE_UUID_GATT_REM_VAL 0x2a3a
/**
 *  @brief GATT Characteristic Removable
 */
#define BLE_UUID_GATT_REM \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_REM_VAL)
/**
 *  @brief GATT Characteristic Service Required UUID Value
 */
#define BLE_UUID_GATT_SRVREQ_VAL 0x2a3b
/**
 *  @brief GATT Characteristic Service Required
 */
#define BLE_UUID_GATT_SRVREQ \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_SRVREQ_VAL)
/**
 *  @brief GATT Characteristic Scientific Temperature in Celsius UUID Value
 */
#define BLE_UUID_GATT_SC_TEMP_C_VAL 0x2a3c
/**
 *  @brief GATT Characteristic Scientific Temperature in Celsius
 */
#define BLE_UUID_GATT_SC_TEMP_C \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_SC_TEMP_C_VAL)
/**
 *  @brief GATT Characteristic String UUID Value
 */
#define BLE_UUID_GATT_STRING_VAL 0x2a3d
/**
 *  @brief GATT Characteristic String
 */
#define BLE_UUID_GATT_STRING \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_STRING_VAL)
/**
 *  @brief GATT Characteristic Network Availability UUID Value
 */
#define BLE_UUID_GATT_NETA_VAL 0x2a3e
/**
 *  @brief GATT Characteristic Network Availability
 */
#define BLE_UUID_GATT_NETA \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_NETA_VAL)
/**
 *  @brief GATT Characteristic Alert Status UUID Value
 */
#define BLE_UUID_GATT_ALRTS_VAL 0x2a3f
/**
 *  @brief GATT Characteristic Alert Status
 */
#define BLE_UUID_GATT_ALRTS \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ALRTS_VAL)
/**
 *  @brief GATT Characteristic Ringer Control Point UUID Value
 */
#define BLE_UUID_GATT_RCP_VAL 0x2a40
/**
 *  @brief GATT Characteristic Ringer Control Point
 */
#define BLE_UUID_GATT_RCP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_RCP_VAL)
/**
 *  @brief GATT Characteristic Ringer Setting UUID Value
 */
#define BLE_UUID_GATT_RS_VAL 0x2a41
/**
 *  @brief GATT Characteristic Ringer Setting
 */
#define BLE_UUID_GATT_RS \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_RS_VAL)
/**
 *  @brief GATT Characteristic Alert Category ID Bit Mask UUID Value
 */
#define BLE_UUID_GATT_ALRTCID_MASK_VAL 0x2a42
/**
 *  @brief GATT Characteristic Alert Category ID Bit Mask
 */
#define BLE_UUID_GATT_ALRTCID_MASK \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ALRTCID_MASK_VAL)
/**
 *  @brief GATT Characteristic Alert Category ID UUID Value
 */
#define BLE_UUID_GATT_ALRTCID_VAL 0x2a43
/**
 *  @brief GATT Characteristic Alert Category ID
 */
#define BLE_UUID_GATT_ALRTCID \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ALRTCID_VAL)
/**
 *  @brief GATT Characteristic Alert Notification Control Point Value
 */
#define BLE_UUID_GATT_ALRTNCP_VAL 0x2a44
/**
 *  @brief GATT Characteristic Alert Notification Control Point
 */
#define BLE_UUID_GATT_ALRTNCP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ALRTNCP_VAL)
/**
 *  @brief GATT Characteristic Unread Alert Status UUID Value
 */
#define BLE_UUID_GATT_UALRTS_VAL 0x2a45
/**
 *  @brief GATT Characteristic Unread Alert Status
 */
#define BLE_UUID_GATT_UALRTS \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_UALRTS_VAL)
/**
 *  @brief GATT Characteristic New Alert UUID Value
 */
#define BLE_UUID_GATT_NALRT_VAL 0x2a46
/**
 *  @brief GATT Characteristic New Alert
 */
#define BLE_UUID_GATT_NALRT \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_NALRT_VAL)
/**
 *  @brief GATT Characteristic Supported New Alert Category UUID Value
 */
#define BLE_UUID_GATT_SNALRTC_VAL 0x2a47
/**
 *  @brief GATT Characteristic Supported New Alert Category
 */
#define BLE_UUID_GATT_SNALRTC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_SNALRTC_VAL)
/**
 *  @brief GATT Characteristic Supported Unread Alert Category UUID Value
 */
#define BLE_UUID_GATT_SUALRTC_VAL 0x2a48
/**
 *  @brief GATT Characteristic Supported Unread Alert Category
 */
#define BLE_UUID_GATT_SUALRTC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_SUALRTC_VAL)
/**
 *  @brief GATT Characteristic Blood Pressure Feature UUID Value
 */
#define BLE_UUID_GATT_BPF_VAL 0x2a49
/**
 *  @brief GATT Characteristic Blood Pressure Feature
 */
#define BLE_UUID_GATT_BPF \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_BPF_VAL)
/**
 *  @brief HID Information Characteristic UUID value
 */
#define BLE_UUID_HIDS_INFO_VAL 0x2a4a
/**
 *  @brief HID Information Characteristic
 */
#define BLE_UUID_HIDS_INFO \
    BLE_UUID_DECLARE_16(BLE_UUID_HIDS_INFO_VAL)
/**
 *  @brief HID Report Map Characteristic UUID value
 */
#define BLE_UUID_HIDS_REPORT_MAP_VAL 0x2a4b
/**
 *  @brief HID Report Map Characteristic
 */
#define BLE_UUID_HIDS_REPORT_MAP \
    BLE_UUID_DECLARE_16(BLE_UUID_HIDS_REPORT_MAP_VAL)
/**
 *  @brief HID Control Point Characteristic UUID value
 */
#define BLE_UUID_HIDS_CTRL_POINT_VAL 0x2a4c
/**
 *  @brief HID Control Point Characteristic
 */
#define BLE_UUID_HIDS_CTRL_POINT \
    BLE_UUID_DECLARE_16(BLE_UUID_HIDS_CTRL_POINT_VAL)
/**
 *  @brief HID Report Characteristic UUID value
 */
#define BLE_UUID_HIDS_REPORT_VAL 0x2a4d
/**
 *  @brief HID Report Characteristic
 */
#define BLE_UUID_HIDS_REPORT \
    BLE_UUID_DECLARE_16(BLE_UUID_HIDS_REPORT_VAL)
/**
 *  @brief HID Protocol Mode Characteristic UUID value
 */
#define BLE_UUID_HIDS_PROTOCOL_MODE_VAL 0x2a4e
/**
 *  @brief HID Protocol Mode Characteristic
 */
#define BLE_UUID_HIDS_PROTOCOL_MODE \
    BLE_UUID_DECLARE_16(BLE_UUID_HIDS_PROTOCOL_MODE_VAL)
/**
 *  @brief GATT Characteristic Scan Interval Windows UUID Value
 */
#define BLE_UUID_GATT_SIW_VAL 0x2a4f
/**
 *  @brief GATT Characteristic Scan Interval Windows
 */
#define BLE_UUID_GATT_SIW \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_SIW_VAL)
/**
 *  @brief DIS Characteristic PnP ID UUID value
 */
#define BLE_UUID_DIS_PNP_ID_VAL 0x2a50
/**
 *  @brief DIS Characteristic PnP ID
 */
#define BLE_UUID_DIS_PNP_ID \
    BLE_UUID_DECLARE_16(BLE_UUID_DIS_PNP_ID_VAL)
/**
 *  @brief GATT Characteristic Glucose Feature UUID Value
 */
#define BLE_UUID_GATT_GF_VAL 0x2a51
/**
 *  @brief GATT Characteristic Glucose Feature
 */
#define BLE_UUID_GATT_GF \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_GF_VAL)
/**
 *  @brief Record Access Control Point Characteristic value
 */
#define BLE_UUID_RECORD_ACCESS_CONTROL_POINT_VAL 0x2a52
/**
 *  @brief Record Access Control Point
 */
#define BLE_UUID_RECORD_ACCESS_CONTROL_POINT \
    BLE_UUID_DECLARE_16(BLE_UUID_RECORD_ACCESS_CONTROL_POINT_VAL)
/**
 *  @brief RSC Measurement Characteristic UUID value
 */
#define BLE_UUID_RSC_MEASUREMENT_VAL 0x2a53
/**
 *  @brief RSC Measurement Characteristic
 */
#define BLE_UUID_RSC_MEASUREMENT \
    BLE_UUID_DECLARE_16(BLE_UUID_RSC_MEASUREMENT_VAL)
/**
 *  @brief RSC Feature Characteristic UUID value
 */
#define BLE_UUID_RSC_FEATURE_VAL 0x2a54
/**
 *  @brief RSC Feature Characteristic
 */
#define BLE_UUID_RSC_FEATURE \
    BLE_UUID_DECLARE_16(BLE_UUID_RSC_FEATURE_VAL)
/**
 *  @brief SC Control Point Characteristic UUID value
 */
#define BLE_UUID_SC_CONTROL_POINT_VAL 0x2a55
/**
 *  @brief SC Control Point Characteristic
 */
#define BLE_UUID_SC_CONTROL_POINT \
    BLE_UUID_DECLARE_16(BLE_UUID_SC_CONTROL_POINT_VAL)
/**
 *  @brief GATT Characteristic Digital Input UUID Value
 */
#define BLE_UUID_GATT_DI_VAL 0x2a56
/**
 *  @brief GATT Characteristic Digital Input
 */
#define BLE_UUID_GATT_DI \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_DI_VAL)
/**
 *  @brief GATT Characteristic Digital Output UUID Value
 */
#define BLE_UUID_GATT_DO_VAL 0x2a57
/**
 *  @brief GATT Characteristic Digital Output
 */
#define BLE_UUID_GATT_DO \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_DO_VAL)
/**
 *  @brief GATT Characteristic Analog Input UUID Value
 */
#define BLE_UUID_GATT_AI_VAL 0x2a58
/**
 *  @brief GATT Characteristic Analog Input
 */
#define BLE_UUID_GATT_AI \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_AI_VAL)
/**
 *  @brief GATT Characteristic Analog Output UUID Value
 */
#define BLE_UUID_GATT_AO_VAL 0x2a59
/**
 *  @brief GATT Characteristic Analog Output
 */
#define BLE_UUID_GATT_AO \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_AO_VAL)
/**
 *  @brief GATT Characteristic Aggregate UUID Value
 */
#define BLE_UUID_GATT_AGGR_VAL 0x2a5a
/**
 *  @brief GATT Characteristic Aggregate
 */
#define BLE_UUID_GATT_AGGR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_AGGR_VAL)
/**
 *  @brief CSC Measurement Characteristic UUID value
 */
#define BLE_UUID_CSC_MEASUREMENT_VAL 0x2a5b
/**
 *  @brief CSC Measurement Characteristic
 */
#define BLE_UUID_CSC_MEASUREMENT \
    BLE_UUID_DECLARE_16(BLE_UUID_CSC_MEASUREMENT_VAL)
/**
 *  @brief CSC Feature Characteristic UUID value
 */
#define BLE_UUID_CSC_FEATURE_VAL 0x2a5c
/**
 *  @brief CSC Feature Characteristic
 */
#define BLE_UUID_CSC_FEATURE \
    BLE_UUID_DECLARE_16(BLE_UUID_CSC_FEATURE_VAL)
/**
 *  @brief Sensor Location Characteristic UUID value
 */
#define BLE_UUID_SENSOR_LOCATION_VAL 0x2a5d
/**
 *  @brief Sensor Location Characteristic
 */
#define BLE_UUID_SENSOR_LOCATION \
    BLE_UUID_DECLARE_16(BLE_UUID_SENSOR_LOCATION_VAL)
/**
 *  @brief GATT Characteristic PLX Spot-Check Measurement UUID Value
 */
#define BLE_UUID_GATT_PLX_SCM_VAL 0x2a5e
/**
 *  @brief GATT Characteristic PLX Spot-Check Measurement
 */
#define BLE_UUID_GATT_PLX_SCM \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_PLX_SCM_VAL)
/**
 *  @brief GATT Characteristic PLX Continuous Measurement UUID Value
 */
#define BLE_UUID_GATT_PLX_CM_VAL 0x2a5f
/**
 *  @brief GATT Characteristic PLX Continuous Measurement
 */
#define BLE_UUID_GATT_PLX_CM \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_PLX_CM_VAL)
/**
 *  @brief GATT Characteristic PLX Features UUID Value
 */
#define BLE_UUID_GATT_PLX_F_VAL 0x2a60
/**
 *  @brief GATT Characteristic PLX Features
 */
#define BLE_UUID_GATT_PLX_F \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_PLX_F_VAL)
/**
 *  @brief GATT Characteristic Pulse Oximetry Pulastile Event UUID Value
 */
#define BLE_UUID_GATT_POPE_VAL 0x2a61
/**
 *  @brief GATT Characteristic Pulse Oximetry Pulsatile Event
 */
#define BLE_UUID_GATT_POPE \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_POPE_VAL)
/**
 *  @brief GATT Characteristic Pulse Oximetry Control Point UUID Value
 */
#define BLE_UUID_GATT_POCP_VAL 0x2a62
/**
 *  @brief GATT Characteristic Pulse Oximetry Control Point
 */
#define BLE_UUID_GATT_POCP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_POCP_VAL)
/**
 *  @brief GATT Characteristic Cycling Power Measurement UUID Value
 */
#define BLE_UUID_GATT_CPS_CPM_VAL 0x2a63
/**
 *  @brief GATT Characteristic Cycling Power Measurement
 */
#define BLE_UUID_GATT_CPS_CPM \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CPS_CPM_VAL)
/**
 *  @brief GATT Characteristic Cycling Power Vector UUID Value
 */
#define BLE_UUID_GATT_CPS_CPV_VAL 0x2a64
/**
 *  @brief GATT Characteristic Cycling Power Vector
 */
#define BLE_UUID_GATT_CPS_CPV \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CPS_CPV_VAL)
/**
 *  @brief GATT Characteristic Cycling Power Feature UUID Value
 */
#define BLE_UUID_GATT_CPS_CPF_VAL 0x2a65
/**
 *  @brief GATT Characteristic Cycling Power Feature
 */
#define BLE_UUID_GATT_CPS_CPF \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CPS_CPF_VAL)
/**
 *  @brief GATT Characteristic Cycling Power Control Point UUID Value
 */
#define BLE_UUID_GATT_CPS_CPCP_VAL 0x2a66
/**
 *  @brief GATT Characteristic Cycling Power Control Point
 */
#define BLE_UUID_GATT_CPS_CPCP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CPS_CPCP_VAL)
/**
 *  @brief GATT Characteristic Location and Speed UUID Value
 */
#define BLE_UUID_GATT_LOC_SPD_VAL 0x2a67
/**
 *  @brief GATT Characteristic Location and Speed
 */
#define BLE_UUID_GATT_LOC_SPD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_LOC_SPD_VAL)
/**
 *  @brief GATT Characteristic Navigation UUID Value
 */
#define BLE_UUID_GATT_NAV_VAL 0x2a68
/**
 *  @brief GATT Characteristic Navigation
 */
#define BLE_UUID_GATT_NAV \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_NAV_VAL)
/**
 *  @brief GATT Characteristic Position Quality UUID Value
 */
#define BLE_UUID_GATT_PQ_VAL 0x2a69
/**
 *  @brief GATT Characteristic Position Quality
 */
#define BLE_UUID_GATT_PQ \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_PQ_VAL)
/**
 *  @brief GATT Characteristic LN Feature UUID Value
 */
#define BLE_UUID_GATT_LNF_VAL 0x2a6a
/**
 *  @brief GATT Characteristic LN Feature
 */
#define BLE_UUID_GATT_LNF \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_LNF_VAL)
/**
 *  @brief GATT Characteristic LN Control Point UUID Value
 */
#define BLE_UUID_GATT_LNCP_VAL 0x2a6b
/**
 *  @brief GATT Characteristic LN Control Point
 */
#define BLE_UUID_GATT_LNCP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_LNCP_VAL)
/**
 *  @brief Elevation Characteristic UUID value
 */
#define BLE_UUID_ELEVATION_VAL 0x2a6c
/**
 *  @brief Elevation Characteristic
 */
#define BLE_UUID_ELEVATION \
    BLE_UUID_DECLARE_16(BLE_UUID_ELEVATION_VAL)
/**
 *  @brief Pressure Characteristic UUID value
 */
#define BLE_UUID_PRESSURE_VAL 0x2a6d
/**
 *  @brief Pressure Characteristic
 */
#define BLE_UUID_PRESSURE \
    BLE_UUID_DECLARE_16(BLE_UUID_PRESSURE_VAL)
/**
 *  @brief Temperature Characteristic UUID value
 */
#define BLE_UUID_TEMPERATURE_VAL 0x2a6e
/**
 *  @brief Temperature Characteristic
 */
#define BLE_UUID_TEMPERATURE \
    BLE_UUID_DECLARE_16(BLE_UUID_TEMPERATURE_VAL)
/**
 *  @brief Humidity Characteristic UUID value
 */
#define BLE_UUID_HUMIDITY_VAL 0x2a6f
/**
 *  @brief Humidity Characteristic
 */
#define BLE_UUID_HUMIDITY \
    BLE_UUID_DECLARE_16(BLE_UUID_HUMIDITY_VAL)
/**
 *  @brief True Wind Speed Characteristic UUID value
 */
#define BLE_UUID_TRUE_WIND_SPEED_VAL 0x2a70
/**
 *  @brief True Wind Speed Characteristic
 */
#define BLE_UUID_TRUE_WIND_SPEED \
    BLE_UUID_DECLARE_16(BLE_UUID_TRUE_WIND_SPEED_VAL)
/**
 *  @brief True Wind Direction Characteristic UUID value
 */
#define BLE_UUID_TRUE_WIND_DIR_VAL 0x2a71
/**
 *  @brief True Wind Direction Characteristic
 */
#define BLE_UUID_TRUE_WIND_DIR \
    BLE_UUID_DECLARE_16(BLE_UUID_TRUE_WIND_DIR_VAL)
/**
 *  @brief Apparent Wind Speed Characteristic UUID value
 */
#define BLE_UUID_APPARENT_WIND_SPEED_VAL 0x2a72
/**
 *  @brief Apparent Wind Speed Characteristic
 */
#define BLE_UUID_APPARENT_WIND_SPEED \
    BLE_UUID_DECLARE_16(BLE_UUID_APPARENT_WIND_SPEED_VAL)
/**
 *  @brief Apparent Wind Direction Characteristic UUID value
 */
#define BLE_UUID_APPARENT_WIND_DIR_VAL 0x2a73
/**
 *  @brief Apparent Wind Direction Characteristic
 */
#define BLE_UUID_APPARENT_WIND_DIR \
    BLE_UUID_DECLARE_16(BLE_UUID_APPARENT_WIND_DIR_VAL)
/**
 *  @brief Gust Factor Characteristic UUID value
 */
#define BLE_UUID_GUST_FACTOR_VAL 0x2a74
/**
 *  @brief Gust Factor Characteristic
 */
#define BLE_UUID_GUST_FACTOR \
    BLE_UUID_DECLARE_16(BLE_UUID_GUST_FACTOR_VAL)
/**
 *  @brief Pollen Concentration Characteristic UUID value
 */
#define BLE_UUID_POLLEN_CONCENTRATION_VAL 0x2a75
/**
 *  @brief Pollen Concentration Characteristic
 */
#define BLE_UUID_POLLEN_CONCENTRATION \
    BLE_UUID_DECLARE_16(BLE_UUID_POLLEN_CONCENTRATION_VAL)
/**
 *  @brief UV Index Characteristic UUID value
 */
#define BLE_UUID_UV_INDEX_VAL 0x2a76
/**
 *  @brief UV Index Characteristic
 */
#define BLE_UUID_UV_INDEX \
    BLE_UUID_DECLARE_16(BLE_UUID_UV_INDEX_VAL)
/**
 *  @brief Irradiance Characteristic UUID value
 */
#define BLE_UUID_IRRADIANCE_VAL 0x2a77
/**
 *  @brief Irradiance Characteristic
 */
#define BLE_UUID_IRRADIANCE \
    BLE_UUID_DECLARE_16(BLE_UUID_IRRADIANCE_VAL)
/**
 *  @brief Rainfall Characteristic UUID value
 */
#define BLE_UUID_RAINFALL_VAL 0x2a78
/**
 *  @brief Rainfall Characteristic
 */
#define BLE_UUID_RAINFALL \
    BLE_UUID_DECLARE_16(BLE_UUID_RAINFALL_VAL)
/**
 *  @brief Wind Chill Characteristic UUID value
 */
#define BLE_UUID_WIND_CHILL_VAL 0x2a79
/**
 *  @brief Wind Chill Characteristic
 */
#define BLE_UUID_WIND_CHILL \
    BLE_UUID_DECLARE_16(BLE_UUID_WIND_CHILL_VAL)
/**
 *  @brief Heat Index Characteristic UUID value
 */
#define BLE_UUID_HEAT_INDEX_VAL 0x2a7a
/**
 *  @brief Heat Index Characteristic
 */
#define BLE_UUID_HEAT_INDEX \
    BLE_UUID_DECLARE_16(BLE_UUID_HEAT_INDEX_VAL)
/**
 *  @brief Dew Point Characteristic UUID value
 */
#define BLE_UUID_DEW_POINT_VAL 0x2a7b
/**
 *  @brief Dew Point Characteristic
 */
#define BLE_UUID_DEW_POINT \
    BLE_UUID_DECLARE_16(BLE_UUID_DEW_POINT_VAL)
/**
 *  @brief GATT Characteristic Trend UUID Value
 */
#define BLE_UUID_GATT_TREND_VAL 0x2a7c
/**
 *  @brief GATT Characteristic Trend
 */
#define BLE_UUID_GATT_TREND \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TREND_VAL)
/**
 *  @brief Descriptor Value Changed Characteristic UUID value
 */
#define BLE_UUID_DESC_VALUE_CHANGED_VAL 0x2a7d
/**
 *  @brief Descriptor Value Changed Characteristic
 */
#define BLE_UUID_DESC_VALUE_CHANGED \
    BLE_UUID_DECLARE_16(BLE_UUID_DESC_VALUE_CHANGED_VAL)
/**
 *  @brief GATT Characteristic Aerobic Heart Rate Low Limit UUID Value
 */
#define BLE_UUID_GATT_AEHRLL_VAL 0x2a7e
/**
 *  @brief GATT Characteristic Aerobic Heart Rate Lower Limit
 */
#define BLE_UUID_GATT_AEHRLL \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_AEHRLL_VAL)
/**
 *  @brief GATT Characteristic Aerobic Threshold UUID Value
 */
#define BLE_UUID_GATT_AETHR_VAL 0x2a7f
/**
 *  @brief GATT Characteristic Aerobic Threshold
 */
#define BLE_UUID_GATT_AETHR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_AETHR_VAL)
/**
 *  @brief GATT Characteristic Age UUID Value
 */
#define BLE_UUID_GATT_AGE_VAL 0x2a80
/**
 *  @brief GATT Characteristic Age
 */
#define BLE_UUID_GATT_AGE \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_AGE_VAL)
/**
 *  @brief GATT Characteristic Anaerobic Heart Rate Lower Limit UUID Value
 */
#define BLE_UUID_GATT_ANHRLL_VAL 0x2a81
/**
 *  @brief GATT Characteristic Anaerobic Heart Rate Lower Limit
 */
#define BLE_UUID_GATT_ANHRLL \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ANHRLL_VAL)
/**
 *  @brief GATT Characteristic Anaerobic Heart Rate Upper Limit UUID Value
 */
#define BLE_UUID_GATT_ANHRUL_VAL 0x2a82
/**
 *  @brief GATT Characteristic Anaerobic Heart Rate Upper Limit
 */
#define BLE_UUID_GATT_ANHRUL \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ANHRUL_VAL)
/**
 *  @brief GATT Characteristic Anaerobic Threshold UUID Value
 */
#define BLE_UUID_GATT_ANTHR_VAL 0x2a83
/**
 *  @brief GATT Characteristic Anaerobic Threshold
 */
#define BLE_UUID_GATT_ANTHR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ANTHR_VAL)
/**
 *  @brief GATT Characteristic Aerobic Heart Rate Upper Limit UUID Value
 */
#define BLE_UUID_GATT_AEHRUL_VAL 0x2a84
/**
 *  @brief GATT Characteristic Aerobic Heart Rate Upper Limit
 */
#define BLE_UUID_GATT_AEHRUL \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_AEHRUL_VAL)
/**
 *  @brief GATT Characteristic Date of Birth UUID Value
 */
#define BLE_UUID_GATT_DATE_BIRTH_VAL 0x2a85
/**
 *  @brief GATT Characteristic Date of Birth
 */
#define BLE_UUID_GATT_DATE_BIRTH \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_DATE_BIRTH_VAL)
/**
 *  @brief GATT Characteristic Date of Threshold Assessment UUID Value
 */
#define BLE_UUID_GATT_DATE_THRASS_VAL 0x2a86
/**
 *  @brief GATT Characteristic Date of Threshold Assessment
 */
#define BLE_UUID_GATT_DATE_THRASS \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_DATE_THRASS_VAL)
/**
 *  @brief GATT Characteristic Email Address UUID Value
 */
#define BLE_UUID_GATT_EMAIL_VAL 0x2a87
/**
 *  @brief GATT Characteristic Email Address
 */
#define BLE_UUID_GATT_EMAIL \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_EMAIL_VAL)
/**
 *  @brief GATT Characteristic Fat Burn Heart Rate Lower Limit UUID Value
 */
#define BLE_UUID_GATT_FBHRLL_VAL 0x2a88
/**
 *  @brief GATT Characteristic Fat Burn Heart Rate Lower Limit
 */
#define BLE_UUID_GATT_FBHRLL \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_FBHRLL_VAL)
/**
 *  @brief GATT Characteristic Fat Burn Heart Rate Upper Limit UUID Value
 */
#define BLE_UUID_GATT_FBHRUL_VAL 0x2a89
/**
 *  @brief GATT Characteristic Fat Burn Heart Rate Upper Limit
 */
#define BLE_UUID_GATT_FBHRUL \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_FBHRUL_VAL)
/**
 *  @brief GATT Characteristic First Name UUID Value
 */
#define BLE_UUID_GATT_FIRST_NAME_VAL 0x2a8a
/**
 *  @brief GATT Characteristic First Name
 */
#define BLE_UUID_GATT_FIRST_NAME \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_FIRST_NAME_VAL)
/**
 *  @brief GATT Characteristic Five Zone Heart Rate Limits UUID Value
 */
#define BLE_UUID_GATT_5ZHRL_VAL 0x2a8b
/**
 *  @brief GATT Characteristic Five Zone Heart Rate Limits
 */
#define BLE_UUID_GATT_5ZHRL \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_5ZHRL_VAL)
/**
 *  @brief GATT Characteristic Gender UUID Value
 */
#define BLE_UUID_GATT_GENDER_VAL 0x2a8c
/**
 *  @brief GATT Characteristic Gender
 */
#define BLE_UUID_GATT_GENDER \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_GENDER_VAL)
/**
 *  @brief GATT Characteristic Heart Rate Max UUID Value
 */
#define BLE_UUID_GATT_HR_MAX_VAL 0x2a8d
/**
 *  @brief GATT Characteristic Heart Rate Max
 */
#define BLE_UUID_GATT_HR_MAX \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_HR_MAX_VAL)
/**
 *  @brief GATT Characteristic Height UUID Value
 */
#define BLE_UUID_GATT_HEIGHT_VAL 0x2a8e
/**
 *  @brief GATT Characteristic Height
 */
#define BLE_UUID_GATT_HEIGHT \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_HEIGHT_VAL)
/**
 *  @brief GATT Characteristic Hip Circumference UUID Value
 */
#define BLE_UUID_GATT_HC_VAL 0x2a8f
/**
 *  @brief GATT Characteristic Hip Circumference
 */
#define BLE_UUID_GATT_HC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_HC_VAL)
/**
 *  @brief GATT Characteristic Last Name UUID Value
 */
#define BLE_UUID_GATT_LAST_NAME_VAL 0x2a90
/**
 *  @brief GATT Characteristic Last Name
 */
#define BLE_UUID_GATT_LAST_NAME \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_LAST_NAME_VAL)
/**
 *  @brief GATT Characteristic Maximum Recommended Heart Rate> UUID Value
 */
#define BLE_UUID_GATT_MRHR_VAL 0x2a91
/**
 *  @brief GATT Characteristic Maximum Recommended Heart Rate
 */
#define BLE_UUID_GATT_MRHR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_MRHR_VAL)
/**
 *  @brief GATT Characteristic Resting Heart Rate UUID Value
 */
#define BLE_UUID_GATT_RHR_VAL 0x2a92
/**
 *  @brief GATT Characteristic Resting Heart Rate
 */
#define BLE_UUID_GATT_RHR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_RHR_VAL)
/**
 *  @brief GATT Characteristic Sport Type for Aerobic and Anaerobic Thresholds UUID Value
 */
#define BLE_UUID_GATT_AEANTHR_VAL 0x2a93
/**
 *  @brief GATT Characteristic Sport Type for Aerobic and Anaerobic Threshold
 */
#define BLE_UUID_GATT_AEANTHR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_AEANTHR_VAL)
/**
 *  @brief GATT Characteristic Three Zone Heart Rate Limits UUID Value
 */
#define BLE_UUID_GATT_3ZHRL_VAL 0x2a94
/**
 *  @brief GATT Characteristic Three Zone Heart Rate Limits
 */
#define BLE_UUID_GATT_3ZHRL \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_3ZHRL_VAL)
/**
 *  @brief GATT Characteristic Two Zone Heart Rate Limits UUID Value
 */
#define BLE_UUID_GATT_2ZHRL_VAL 0x2a95
/**
 *  @brief GATT Characteristic Two Zone Heart Rate Limits
 */
#define BLE_UUID_GATT_2ZHRL \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_2ZHRL_VAL)
/**
 *  @brief GATT Characteristic VO2 Max UUID Value
 */
#define BLE_UUID_GATT_VO2_MAX_VAL 0x2a96
/**
 *  @brief GATT Characteristic VO2 Max
 */
#define BLE_UUID_GATT_VO2_MAX \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_VO2_MAX_VAL)
/**
 *  @brief GATT Characteristic Waist Circumference UUID Value
 */
#define BLE_UUID_GATT_WC_VAL 0x2a97
/**
 *  @brief GATT Characteristic Waist Circumference
 */
#define BLE_UUID_GATT_WC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_WC_VAL)
/**
 *  @brief GATT Characteristic Weight UUID Value
 */
#define BLE_UUID_GATT_WEIGHT_VAL 0x2a98
/**
 *  @brief GATT Characteristic Weight
 */
#define BLE_UUID_GATT_WEIGHT \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_WEIGHT_VAL)
/**
 *  @brief GATT Characteristic Database Change Increment UUID Value
 */
#define BLE_UUID_GATT_DBCHINC_VAL 0x2a99
/**
 *  @brief GATT Characteristic Database Change Increment
 */
#define BLE_UUID_GATT_DBCHINC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_DBCHINC_VAL)
/**
 *  @brief GATT Characteristic User Index UUID Value
 */
#define BLE_UUID_GATT_USRIDX_VAL 0x2a9a
/**
 *  @brief GATT Characteristic User Index
 */
#define BLE_UUID_GATT_USRIDX \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_USRIDX_VAL)
/**
 *  @brief GATT Characteristic Body Composition Feature UUID Value
 */
#define BLE_UUID_GATT_BCF_VAL 0x2a9b
/**
 *  @brief GATT Characteristic Body Composition Feature
 */
#define BLE_UUID_GATT_BCF \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_BCF_VAL)
/**
 *  @brief GATT Characteristic Body Composition Measurement UUID Value
 */
#define BLE_UUID_GATT_BCM_VAL 0x2a9c
/**
 *  @brief GATT Characteristic Body Composition Measurement
 */
#define BLE_UUID_GATT_BCM \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_BCM_VAL)
/**
 *  @brief GATT Characteristic Weight Measurement UUID Value
 */
#define BLE_UUID_GATT_WM_VAL 0x2a9d
/**
 *  @brief GATT Characteristic Weight Measurement
 */
#define BLE_UUID_GATT_WM \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_WM_VAL)
/**
 *  @brief GATT Characteristic Weight Scale Feature UUID Value
 */
#define BLE_UUID_GATT_WSF_VAL 0x2a9e
/**
 *  @brief GATT Characteristic Weight Scale Feature
 */
#define BLE_UUID_GATT_WSF \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_WSF_VAL)
/**
 *  @brief GATT Characteristic User Control Point UUID Value
 */
#define BLE_UUID_GATT_USRCP_VAL 0x2a9f
/**
 *  @brief GATT Characteristic User Control Point
 */
#define BLE_UUID_GATT_USRCP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_USRCP_VAL)
/**
 *  @brief Magnetic Flux Density - 2D Characteristic UUID value
 */
#define BLE_UUID_MAGN_FLUX_DENSITY_2D_VAL 0x2aa0
/**
 *  @brief Magnetic Flux Density - 2D Characteristic
 */
#define BLE_UUID_MAGN_FLUX_DENSITY_2D \
    BLE_UUID_DECLARE_16(BLE_UUID_MAGN_FLUX_DENSITY_2D_VAL)
/**
 *  @brief Magnetic Flux Density - 3D Characteristic UUID value
 */
#define BLE_UUID_MAGN_FLUX_DENSITY_3D_VAL 0x2aa1
/**
 *  @brief Magnetic Flux Density - 3D Characteristic
 */
#define BLE_UUID_MAGN_FLUX_DENSITY_3D \
    BLE_UUID_DECLARE_16(BLE_UUID_MAGN_FLUX_DENSITY_3D_VAL)
/**
 *  @brief GATT Characteristic Language UUID Value
 */
#define BLE_UUID_GATT_LANG_VAL 0x2aa2
/**
 *  @brief GATT Characteristic Language
 */
#define BLE_UUID_GATT_LANG \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_LANG_VAL)
/**
 *  @brief Barometric Pressure Trend Characteristic UUID value
 */
#define BLE_UUID_BAR_PRESSURE_TREND_VAL 0x2aa3
/**
 *  @brief Barometric Pressure Trend Characteristic
 */
#define BLE_UUID_BAR_PRESSURE_TREND \
    BLE_UUID_DECLARE_16(BLE_UUID_BAR_PRESSURE_TREND_VAL)
/**
 *  @brief Bond Management Control Point UUID value
 */
#define BLE_UUID_BMS_CONTROL_POINT_VAL 0x2aa4
/**
 *  @brief Bond Management Control Point
 */
#define BLE_UUID_BMS_CONTROL_POINT \
    BLE_UUID_DECLARE_16(BLE_UUID_BMS_CONTROL_POINT_VAL)
/**
 *  @brief Bond Management Feature UUID value
 */
#define BLE_UUID_BMS_FEATURE_VAL 0x2aa5
/**
 *  @brief Bond Management Feature
 */
#define BLE_UUID_BMS_FEATURE \
    BLE_UUID_DECLARE_16(BLE_UUID_BMS_FEATURE_VAL)
/**
 *  @brief Central Address Resolution Characteristic UUID value
 */
#define BLE_UUID_CENTRAL_ADDR_RES_VAL 0x2aa6
/**
 *  @brief Central Address Resolution Characteristic
 */
#define BLE_UUID_CENTRAL_ADDR_RES \
    BLE_UUID_DECLARE_16(BLE_UUID_CENTRAL_ADDR_RES_VAL)
/**
 *  @brief CGM Measurement Characteristic value
 */
#define BLE_UUID_CGM_MEASUREMENT_VAL 0x2aa7
/**
 *  @brief CGM Measurement Characteristic
 */
#define BLE_UUID_CGM_MEASUREMENT \
    BLE_UUID_DECLARE_16(BLE_UUID_CGM_MEASUREMENT_VAL)
/**
 *  @brief CGM Feature Characteristic value
 */
#define BLE_UUID_CGM_FEATURE_VAL 0x2aa8
/**
 *  @brief CGM Feature Characteristic
 */
#define BLE_UUID_CGM_FEATURE \
    BLE_UUID_DECLARE_16(BLE_UUID_CGM_FEATURE_VAL)
/**
 *  @brief CGM Status Characteristic value
 */
#define BLE_UUID_CGM_STATUS_VAL 0x2aa9
/**
 *  @brief CGM Status Characteristic
 */
#define BLE_UUID_CGM_STATUS \
    BLE_UUID_DECLARE_16(BLE_UUID_CGM_STATUS_VAL)
/**
 *  @brief CGM Session Start Time Characteristic value
 */
#define BLE_UUID_CGM_SESSION_START_TIME_VAL 0x2aaa
/**
 *  @brief CGM Session Start Time
 */
#define BLE_UUID_CGM_SESSION_START_TIME \
    BLE_UUID_DECLARE_16(BLE_UUID_CGM_SESSION_START_TIME_VAL)
/**
 *  @brief CGM Session Run Time Characteristic value
 */
#define BLE_UUID_CGM_SESSION_RUN_TIME_VAL 0x2aab
/**
 *  @brief CGM Session Run Time
 */
#define BLE_UUID_CGM_SESSION_RUN_TIME \
    BLE_UUID_DECLARE_16(BLE_UUID_CGM_SESSION_RUN_TIME_VAL)
/**
 *  @brief CGM Specific Ops Control Point Characteristic value
 */
#define BLE_UUID_CGM_SPECIFIC_OPS_CONTROL_POINT_VAL 0x2aac
/**
 *  @brief CGM Specific Ops Control Point
 */
#define BLE_UUID_CGM_SPECIFIC_OPS_CONTROL_POINT \
    BLE_UUID_DECLARE_16(BLE_UUID_CGM_SPECIFIC_OPS_CONTROL_POINT_VAL)
/**
 *  @brief GATT Characteristic Indoor Positioning Configuration UUID Value
 */
#define BLE_UUID_GATT_IPC_VAL 0x2aad
/**
 *  @brief GATT Characteristic Indoor Positioning Configuration
 */
#define BLE_UUID_GATT_IPC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_IPC_VAL)
/**
 *  @brief GATT Characteristic Latitude UUID Value
 */
#define BLE_UUID_GATT_LAT_VAL 0x2aae
/**
 *  @brief GATT Characteristic Latitude
 */
#define BLE_UUID_GATT_LAT \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_LAT_VAL)
/**
 *  @brief GATT Characteristic Longitude UUID Value
 */
#define BLE_UUID_GATT_LON_VAL 0x2aaf
/**
 *  @brief GATT Characteristic Longitude
 */
#define BLE_UUID_GATT_LON \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_LON_VAL)
/**
 *  @brief GATT Characteristic Local North Coordinate UUID Value
 */
#define BLE_UUID_GATT_LNCOORD_VAL 0x2ab0
/**
 *  @brief GATT Characteristic Local North Coordinate
 */
#define BLE_UUID_GATT_LNCOORD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_LNCOORD_VAL)
/**
 *  @brief GATT Characteristic Local East Coordinate UUID Value
 */
#define BLE_UUID_GATT_LECOORD_VAL 0x2ab1
/**
 *  @brief GATT Characteristic Local East Coordinate
 */
#define BLE_UUID_GATT_LECOORD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_LECOORD_VAL)
/**
 *  @brief GATT Characteristic Floor Number UUID Value
 */
#define BLE_UUID_GATT_FN_VAL 0x2ab2
/**
 *  @brief GATT Characteristic Floor Number
 */
#define BLE_UUID_GATT_FN \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_FN_VAL)
/**
 *  @brief GATT Characteristic Altitude UUID Value
 */
#define BLE_UUID_GATT_ALT_VAL 0x2ab3
/**
 *  @brief GATT Characteristic Altitude
 */
#define BLE_UUID_GATT_ALT \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ALT_VAL)
/**
 *  @brief GATT Characteristic Uncertainty UUID Value
 */
#define BLE_UUID_GATT_UNCERTAINTY_VAL 0x2ab4
/**
 *  @brief GATT Characteristic Uncertainty
 */
#define BLE_UUID_GATT_UNCERTAINTY \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_UNCERTAINTY_VAL)
/**
 *  @brief GATT Characteristic Location Name UUID Value
 */
#define BLE_UUID_GATT_LOC_NAME_VAL 0x2ab5
/**
 *  @brief GATT Characteristic Location Name
 */
#define BLE_UUID_GATT_LOC_NAME \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_LOC_NAME_VAL)
/**
 *  @brief URI UUID value
 */
#define BLE_UUID_URI_VAL 0x2ab6
/**
 *  @brief URI
 */
#define BLE_UUID_URI \
    BLE_UUID_DECLARE_16(BLE_UUID_URI_VAL)
/**
 *  @brief HTTP Headers UUID value
 */
#define BLE_UUID_HTTP_HEADERS_VAL 0x2ab7
/**
 *  @brief HTTP Headers
 */
#define BLE_UUID_HTTP_HEADERS \
    BLE_UUID_DECLARE_16(BLE_UUID_HTTP_HEADERS_VAL)
/**
 *  @brief HTTP Status Code UUID value
 */
#define BLE_UUID_HTTP_STATUS_CODE_VAL 0x2ab8
/**
 *  @brief HTTP Status Code
 */
#define BLE_UUID_HTTP_STATUS_CODE \
    BLE_UUID_DECLARE_16(BLE_UUID_HTTP_STATUS_CODE_VAL)
/**
 *  @brief HTTP Entity Body UUID value
 */
#define BLE_UUID_HTTP_ENTITY_BODY_VAL 0x2ab9
/**
 *  @brief HTTP Entity Body
 */
#define BLE_UUID_HTTP_ENTITY_BODY \
    BLE_UUID_DECLARE_16(BLE_UUID_HTTP_ENTITY_BODY_VAL)
/**
 *  @brief HTTP Control Point UUID value
 */
#define BLE_UUID_HTTP_CONTROL_POINT_VAL 0x2aba
/**
 *  @brief HTTP Control Point
 */
#define BLE_UUID_HTTP_CONTROL_POINT \
    BLE_UUID_DECLARE_16(BLE_UUID_HTTP_CONTROL_POINT_VAL)
/**
 *  @brief HTTPS Security UUID value
 */
#define BLE_UUID_HTTPS_SECURITY_VAL 0x2abb
/**
 *  @brief HTTPS Security
 */
#define BLE_UUID_HTTPS_SECURITY \
    BLE_UUID_DECLARE_16(BLE_UUID_HTTPS_SECURITY_VAL)
/**
 *  @brief GATT Characteristic TDS Control Point UUID Value
 */
#define BLE_UUID_GATT_TDS_CP_VAL 0x2abc
/**
 *  @brief GATT Characteristic TDS Control Point
 */
#define BLE_UUID_GATT_TDS_CP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TDS_CP_VAL)
/**
 *  @brief OTS Feature Characteristic UUID value
 */
#define BLE_UUID_OTS_FEATURE_VAL 0x2abd
/**
 *  @brief OTS Feature Characteristic
 */
#define BLE_UUID_OTS_FEATURE \
    BLE_UUID_DECLARE_16(BLE_UUID_OTS_FEATURE_VAL)
/**
 *  @brief OTS Object Name Characteristic UUID value
 */
#define BLE_UUID_OTS_NAME_VAL 0x2abe
/**
 *  @brief OTS Object Name Characteristic
 */
#define BLE_UUID_OTS_NAME \
    BLE_UUID_DECLARE_16(BLE_UUID_OTS_NAME_VAL)
/**
 *  @brief OTS Object Type Characteristic UUID value
 */
#define BLE_UUID_OTS_TYPE_VAL 0x2abf
/**
 *  @brief OTS Object Type Characteristic
 */
#define BLE_UUID_OTS_TYPE \
    BLE_UUID_DECLARE_16(BLE_UUID_OTS_TYPE_VAL)
/**
 *  @brief OTS Object Size Characteristic UUID value
 */
#define BLE_UUID_OTS_SIZE_VAL 0x2ac0
/**
 *  @brief OTS Object Size Characteristic
 */
#define BLE_UUID_OTS_SIZE \
    BLE_UUID_DECLARE_16(BLE_UUID_OTS_SIZE_VAL)
/**
 *  @brief OTS Object First-Created Characteristic UUID value
 */
#define BLE_UUID_OTS_FIRST_CREATED_VAL 0x2ac1
/**
 *  @brief OTS Object First-Created Characteristic
 */
#define BLE_UUID_OTS_FIRST_CREATED \
    BLE_UUID_DECLARE_16(BLE_UUID_OTS_FIRST_CREATED_VAL)
/**
 *  @brief OTS Object Last-Modified Characteristic UUI value
 */
#define BLE_UUID_OTS_LAST_MODIFIED_VAL 0x2ac2
/**
 *  @brief OTS Object Last-Modified Characteristic
 */
#define BLE_UUID_OTS_LAST_MODIFIED \
    BLE_UUID_DECLARE_16(BLE_UUID_OTS_LAST_MODIFIED_VAL)
/**
 *  @brief OTS Object ID Characteristic UUID value
 */
#define BLE_UUID_OTS_ID_VAL 0x2ac3
/**
 *  @brief OTS Object ID Characteristic
 */
#define BLE_UUID_OTS_ID \
    BLE_UUID_DECLARE_16(BLE_UUID_OTS_ID_VAL)
/**
 *  @brief OTS Object Properties Characteristic UUID value
 */
#define BLE_UUID_OTS_PROPERTIES_VAL 0x2ac4
/**
 *  @brief OTS Object Properties Characteristic
 */
#define BLE_UUID_OTS_PROPERTIES \
    BLE_UUID_DECLARE_16(BLE_UUID_OTS_PROPERTIES_VAL)
/**
 *  @brief OTS Object Action Control Point Characteristic UUID value
 */
#define BLE_UUID_OTS_ACTION_CP_VAL 0x2ac5
/**
 *  @brief OTS Object Action Control Point Characteristic
 */
#define BLE_UUID_OTS_ACTION_CP \
    BLE_UUID_DECLARE_16(BLE_UUID_OTS_ACTION_CP_VAL)
/**
 *  @brief OTS Object List Control Point Characteristic UUID value
 */
#define BLE_UUID_OTS_LIST_CP_VAL 0x2ac6
/**
 *  @brief OTS Object List Control Point Characteristic
 */
#define BLE_UUID_OTS_LIST_CP \
    BLE_UUID_DECLARE_16(BLE_UUID_OTS_LIST_CP_VAL)
/**
 *  @brief OTS Object List Filter Characteristic UUID value
 */
#define BLE_UUID_OTS_LIST_FILTER_VAL 0x2ac7
/**
 *  @brief OTS Object List Filter Characteristic
 */
#define BLE_UUID_OTS_LIST_FILTER \
    BLE_UUID_DECLARE_16(BLE_UUID_OTS_LIST_FILTER_VAL)
/**
 *  @brief OTS Object Changed Characteristic UUID value
 */
#define BLE_UUID_OTS_CHANGED_VAL 0x2ac8
/**
 *  @brief OTS Object Changed Characteristic
 */
#define BLE_UUID_OTS_CHANGED \
    BLE_UUID_DECLARE_16(BLE_UUID_OTS_CHANGED_VAL)
/**
 *  @brief GATT Characteristic Resolvable Private Address Only UUID Value
 */
#define BLE_UUID_GATT_RPAO_VAL 0x2ac9
/**
 *  @brief GATT Characteristic Resolvable Private Address Only
 */
#define BLE_UUID_GATT_RPAO \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_RPAO_VAL)
/**
 *  @brief OTS Unspecified Object Type UUID value
 */
#define BLE_UUID_OTS_TYPE_UNSPECIFIED_VAL 0x2aca
/**
 *  @brief OTS Unspecified Object Type
 */
#define BLE_UUID_OTS_TYPE_UNSPECIFIED \
    BLE_UUID_DECLARE_16(BLE_UUID_OTS_TYPE_UNSPECIFIED_VAL)
/**
 *  @brief OTS Directory Listing UUID value
 */
#define BLE_UUID_OTS_DIRECTORY_LISTING_VAL 0x2acb
/**
 *  @brief OTS Directory Listing
 */
#define BLE_UUID_OTS_DIRECTORY_LISTING \
    BLE_UUID_DECLARE_16(BLE_UUID_OTS_DIRECTORY_LISTING_VAL)
/**
 *  @brief GATT Characteristic Fitness Machine Feature UUID Value
 */
#define BLE_UUID_GATT_FMF_VAL 0x2acc
/**
 *  @brief GATT Characteristic Fitness Machine Feature
 */
#define BLE_UUID_GATT_FMF \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_FMF_VAL)
/**
 *  @brief GATT Characteristic Treadmill Data UUID Value
 */
#define BLE_UUID_GATT_TD_VAL 0x2acd
/**
 *  @brief GATT Characteristic Treadmill Data
 */
#define BLE_UUID_GATT_TD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TD_VAL)
/**
 *  @brief GATT Characteristic Cross Trainer Data UUID Value
 */
#define BLE_UUID_GATT_CTD_VAL 0x2ace
/**
 *  @brief GATT Characteristic Cross Trainer Data
 */
#define BLE_UUID_GATT_CTD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CTD_VAL)
/**
 *  @brief GATT Characteristic Step Climber Data UUID Value
 */
#define BLE_UUID_GATT_STPCD_VAL 0x2acf
/**
 *  @brief GATT Characteristic Step Climber Data
 */
#define BLE_UUID_GATT_STPCD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_STPCD_VAL)
/**
 *  @brief GATT Characteristic Stair Climber Data UUID Value
 */
#define BLE_UUID_GATT_STRCD_VAL 0x2ad0
/**
 *  @brief GATT Characteristic Stair Climber Data
 */
#define BLE_UUID_GATT_STRCD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_STRCD_VAL)
/**
 *  @brief GATT Characteristic Rower Data UUID Value
 */
#define BLE_UUID_GATT_RD_VAL 0x2ad1
/**
 *  @brief GATT Characteristic Rower Data
 */
#define BLE_UUID_GATT_RD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_RD_VAL)
/**
 *  @brief GATT Characteristic Indoor Bike Data UUID Value
 */
#define BLE_UUID_GATT_IBD_VAL 0x2ad2
/**
 *  @brief GATT Characteristic Indoor Bike Data
 */
#define BLE_UUID_GATT_IBD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_IBD_VAL)
/**
 *  @brief GATT Characteristic Training Status UUID Value
 */
#define BLE_UUID_GATT_TRSTAT_VAL 0x2ad3
/**
 *  @brief GATT Characteristic Training Status
 */
#define BLE_UUID_GATT_TRSTAT \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TRSTAT_VAL)
/**
 *  @brief GATT Characteristic Supported Speed Range UUID Value
 */
#define BLE_UUID_GATT_SSR_VAL 0x2ad4
/**
 *  @brief GATT Characteristic Supported Speed Range
 */
#define BLE_UUID_GATT_SSR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_SSR_VAL)
/**
 *  @brief GATT Characteristic Supported Inclination Range UUID Value
 */
#define BLE_UUID_GATT_SIR_VAL 0x2ad5
/**
 *  @brief GATT Characteristic Supported Inclination Range
 */
#define BLE_UUID_GATT_SIR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_SIR_VAL)
/**
 *  @brief GATT Characteristic Supported Resistance Level Range UUID Value
 */
#define BLE_UUID_GATT_SRLR_VAL 0x2ad6
/**
 *  @brief GATT Characteristic Supported Resistance Level Range
 */
#define BLE_UUID_GATT_SRLR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_SRLR_VAL)
/**
 *  @brief GATT Characteristic Supported Heart Rate Range UUID Value
 */
#define BLE_UUID_GATT_SHRR_VAL 0x2ad7
/**
 *  @brief GATT Characteristic Supported Heart Rate Range
 */
#define BLE_UUID_GATT_SHRR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_SHRR_VAL)
/**
 *  @brief GATT Characteristic Supported Power Range UUID Value
 */
#define BLE_UUID_GATT_SPR_VAL 0x2ad8
/**
 *  @brief GATT Characteristic Supported Power Range
 */
#define BLE_UUID_GATT_SPR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_SPR_VAL)
/**
 *  @brief GATT Characteristic Fitness Machine Control Point UUID Value
 */
#define BLE_UUID_GATT_FMCP_VAL 0x2ad9
/**
 *  @brief GATT Characteristic Fitness Machine Control Point
 */
#define BLE_UUID_GATT_FMCP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_FMCP_VAL)
/**
 *  @brief GATT Characteristic Fitness Machine Status UUID Value
 */
#define BLE_UUID_GATT_FMS_VAL 0x2ada
/**
 *  @brief GATT Characteristic Fitness Machine Status
 */
#define BLE_UUID_GATT_FMS \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_FMS_VAL)
/**
 *  @brief Mesh Provisioning Data In UUID value
 */
#define BLE_UUID_MESH_PROV_DATA_IN_VAL 0x2adb
/**
 *  @brief Mesh Provisioning Data In
 */
#define BLE_UUID_MESH_PROV_DATA_IN \
    BLE_UUID_DECLARE_16(BLE_UUID_MESH_PROV_DATA_IN_VAL)
/**
 *  @brief Mesh Provisioning Data Out UUID value
 */
#define BLE_UUID_MESH_PROV_DATA_OUT_VAL 0x2adc
/**
 *  @brief Mesh Provisioning Data Out
 */
#define BLE_UUID_MESH_PROV_DATA_OUT \
    BLE_UUID_DECLARE_16(BLE_UUID_MESH_PROV_DATA_OUT_VAL)
/**
 *  @brief Mesh Proxy Data In UUID value
 */
#define BLE_UUID_MESH_PROXY_DATA_IN_VAL 0x2add
/**
 *  @brief Mesh Proxy Data In
 */
#define BLE_UUID_MESH_PROXY_DATA_IN \
    BLE_UUID_DECLARE_16(BLE_UUID_MESH_PROXY_DATA_IN_VAL)
/**
 *  @brief Mesh Proxy Data Out UUID value
 */
#define BLE_UUID_MESH_PROXY_DATA_OUT_VAL 0x2ade
/**
 *  @brief Mesh Proxy Data Out
 */
#define BLE_UUID_MESH_PROXY_DATA_OUT \
    BLE_UUID_DECLARE_16(BLE_UUID_MESH_PROXY_DATA_OUT_VAL)
/**
 *  @brief GATT Characteristic New Number Needed UUID Value
 */
#define BLE_UUID_GATT_NNN_VAL 0x2adf
/**
 *  @brief GATT Characteristic New Number Needed
 */
#define BLE_UUID_GATT_NNN \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_NNN_VAL)
/**
 *  @brief GATT Characteristic Average Current UUID Value
 */
#define BLE_UUID_GATT_AC_VAL 0x2ae0
/**
 *  @brief GATT Characteristic Average Current
 */
#define BLE_UUID_GATT_AC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_AC_VAL)
/**
 *  @brief GATT Characteristic Average Voltage UUID Value
 */
#define BLE_UUID_GATT_AV_VAL 0x2ae1
/**
 *  @brief GATT Characteristic Average Voltage
 */
#define BLE_UUID_GATT_AV \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_AV_VAL)
/**
 *  @brief GATT Characteristic Boolean UUID Value
 */
#define BLE_UUID_GATT_BOOLEAN_VAL 0x2ae2
/**
 *  @brief GATT Characteristic Boolean
 */
#define BLE_UUID_GATT_BOOLEAN \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_BOOLEAN_VAL)
/**
 *  @brief GATT Characteristic Chromatic Distance From Planckian UUID Value
 */
#define BLE_UUID_GATT_CRDFP_VAL 0x2ae3
/**
 *  @brief GATT Characteristic Chromatic Distance From Planckian
 */
#define BLE_UUID_GATT_CRDFP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CRDFP_VAL)
/**
 *  @brief GATT Characteristic Chromaticity Coordinates UUID Value
 */
#define BLE_UUID_GATT_CRCOORDS_VAL 0x2ae4
/**
 *  @brief GATT Characteristic Chromaticity Coordinates
 */
#define BLE_UUID_GATT_CRCOORDS \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CRCOORDS_VAL)
/**
 *  @brief GATT Characteristic Chromaticity In CCT And Duv Values UUID Value
 */
#define BLE_UUID_GATT_CRCCT_VAL 0x2ae5
/**
 *  @brief GATT Characteristic Chromaticity In CCT And Duv Values
 */
#define BLE_UUID_GATT_CRCCT \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CRCCT_VAL)
/**
 *  @brief GATT Characteristic Chromaticity Tolerance UUID Value
 */
#define BLE_UUID_GATT_CRT_VAL 0x2ae6
/**
 *  @brief GATT Characteristic Chromaticity Tolerance
 */
#define BLE_UUID_GATT_CRT \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CRT_VAL)
/**
 *  @brief GATT Characteristic CIE 13.3-1995 Color Rendering Index UUID Value
 */
#define BLE_UUID_GATT_CIEIDX_VAL 0x2ae7
/**
 *  @brief GATT Characteristic CIE 13.3-1995 Color Rendering Index
 */
#define BLE_UUID_GATT_CIEIDX \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CIEIDX_VAL)
/**
 *  @brief GATT Characteristic Coefficient UUID Value
 */
#define BLE_UUID_GATT_COEFFICIENT_VAL 0x2ae8
/**
 *  @brief GATT Characteristic Coefficient
 */
#define BLE_UUID_GATT_COEFFICIENT \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_COEFFICIENT_VAL)
/**
 *  @brief GATT Characteristic Correlated Color Temperature UUID Value
 */
#define BLE_UUID_GATT_CCTEMP_VAL 0x2ae9
/**
 *  @brief GATT Characteristic Correlated Color Temperature
 */
#define BLE_UUID_GATT_CCTEMP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CCTEMP_VAL)
/**
 *  @brief GATT Characteristic Count 16 UUID Value
 */
#define BLE_UUID_GATT_COUNT16_VAL 0x2aea
/**
 *  @brief GATT Characteristic Count 16
 */
#define BLE_UUID_GATT_COUNT16 \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_COUNT16_VAL)
/**
 *  @brief GATT Characteristic Count 24 UUID Value
 */
#define BLE_UUID_GATT_COUNT24_VAL 0x2aeb
/**
 *  @brief GATT Characteristic Count 24
 */
#define BLE_UUID_GATT_COUNT24 \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_COUNT24_VAL)
/**
 *  @brief GATT Characteristic Country Code UUID Value
 */
#define BLE_UUID_GATT_CNTRCODE_VAL 0x2aec
/**
 *  @brief GATT Characteristic Country Code
 */
#define BLE_UUID_GATT_CNTRCODE \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CNTRCODE_VAL)
/**
 *  @brief GATT Characteristic Date UTC UUID Value
 */
#define BLE_UUID_GATT_DATEUTC_VAL 0x2aed
/**
 *  @brief GATT Characteristic Date UTC
 */
#define BLE_UUID_GATT_DATEUTC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_DATEUTC_VAL)
/**
 *  @brief GATT Characteristic Electric Current UUID Value
 */
#define BLE_UUID_GATT_EC_VAL 0x2aee
/**
 *  @brief GATT Characteristic Electric Current
 */
#define BLE_UUID_GATT_EC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_EC_VAL)
/**
 *  @brief GATT Characteristic Electric Current Range UUID Value
 */
#define BLE_UUID_GATT_ECR_VAL 0x2aef
/**
 *  @brief GATT Characteristic Electric Current Range
 */
#define BLE_UUID_GATT_ECR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ECR_VAL)
/**
 *  @brief GATT Characteristic Electric Current Specification UUID Value
 */
#define BLE_UUID_GATT_ECSPEC_VAL 0x2af0
/**
 *  @brief GATT Characteristic Electric Current Specification
 */
#define BLE_UUID_GATT_ECSPEC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ECSPEC_VAL)
/**
 *  @brief GATT Characteristic Electric Current Statistics UUID Value
 */
#define BLE_UUID_GATT_ECSTAT_VAL 0x2af1
/**
 *  @brief GATT Characteristic Electric Current Statistics
 */
#define BLE_UUID_GATT_ECSTAT \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ECSTAT_VAL)
/**
 *  @brief GATT Characteristic Energy UUID Value
 */
#define BLE_UUID_GATT_ENERGY_VAL 0x2af2
/**
 *  @brief GATT Characteristic Energy
 */
#define BLE_UUID_GATT_ENERGY \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ENERGY_VAL)
/**
 *  @brief GATT Characteristic Energy In A Period Of Day UUID Value
 */
#define BLE_UUID_GATT_EPOD_VAL 0x2af3
/**
 *  @brief GATT Characteristic Energy In A Period Of Day
 */
#define BLE_UUID_GATT_EPOD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_EPOD_VAL)
/**
 *  @brief GATT Characteristic Event Statistics UUID Value
 */
#define BLE_UUID_GATT_EVTSTAT_VAL 0x2af4
/**
 *  @brief GATT Characteristic Event Statistics
 */
#define BLE_UUID_GATT_EVTSTAT \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_EVTSTAT_VAL)
/**
 *  @brief GATT Characteristic Fixed String 16 UUID Value
 */
#define BLE_UUID_GATT_FSTR16_VAL 0x2af5
/**
 *  @brief GATT Characteristic Fixed String 16
 */
#define BLE_UUID_GATT_FSTR16 \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_FSTR16_VAL)
/**
 *  @brief GATT Characteristic Fixed String 24 UUID Value
 */
#define BLE_UUID_GATT_FSTR24_VAL 0x2af6
/**
 *  @brief GATT Characteristic Fixed String 24
 */
#define BLE_UUID_GATT_FSTR24 \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_FSTR24_VAL)
/**
 *  @brief GATT Characteristic Fixed String 36 UUID Value
 */
#define BLE_UUID_GATT_FSTR36_VAL 0x2af7
/**
 *  @brief GATT Characteristic Fixed String 36
 */
#define BLE_UUID_GATT_FSTR36 \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_FSTR36_VAL)
/**
 *  @brief GATT Characteristic Fixed String 8 UUID Value
 */
#define BLE_UUID_GATT_FSTR8_VAL 0x2af8
/**
 *  @brief GATT Characteristic Fixed String 8
 */
#define BLE_UUID_GATT_FSTR8 \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_FSTR8_VAL)
/**
 *  @brief GATT Characteristic Generic Level UUID Value
 */
#define BLE_UUID_GATT_GENLVL_VAL 0x2af9
/**
 *  @brief GATT Characteristic Generic Level
 */
#define BLE_UUID_GATT_GENLVL \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_GENLVL_VAL)
/**
 *  @brief GATT Characteristic Global Trade Item Number UUID Value
 */
#define BLE_UUID_GATT_GTIN_VAL 0x2afa
/**
 *  @brief GATT Characteristic Global Trade Item Number
 */
#define BLE_UUID_GATT_GTIN \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_GTIN_VAL)
/**
 *  @brief GATT Characteristic Illuminance UUID Value
 */
#define BLE_UUID_GATT_ILLUM_VAL 0x2afb
/**
 *  @brief GATT Characteristic Illuminance
 */
#define BLE_UUID_GATT_ILLUM \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ILLUM_VAL)
/**
 *  @brief GATT Characteristic Luminous Efficacy UUID Value
 */
#define BLE_UUID_GATT_LUMEFF_VAL 0x2afc
/**
 *  @brief GATT Characteristic Luminous Efficacy
 */
#define BLE_UUID_GATT_LUMEFF \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_LUMEFF_VAL)
/**
 *  @brief GATT Characteristic Luminous Energy UUID Value
 */
#define BLE_UUID_GATT_LUMNRG_VAL 0x2afd
/**
 *  @brief GATT Characteristic Luminous Energy
 */
#define BLE_UUID_GATT_LUMNRG \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_LUMNRG_VAL)
/**
 *  @brief GATT Characteristic Luminous Exposure UUID Value
 */
#define BLE_UUID_GATT_LUMEXP_VAL 0x2afe
/**
 *  @brief GATT Characteristic Luminous Exposure
 */
#define BLE_UUID_GATT_LUMEXP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_LUMEXP_VAL)
/**
 *  @brief GATT Characteristic Luminous Flux UUID Value
 */
#define BLE_UUID_GATT_LUMFLX_VAL 0x2aff
/**
 *  @brief GATT Characteristic Luminous Flux
 */
#define BLE_UUID_GATT_LUMFLX \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_LUMFLX_VAL)
/**
 *  @brief GATT Characteristic Luminous Flux Range UUID Value
 */
#define BLE_UUID_GATT_LUMFLXR_VAL 0x2b00
/**
 *  @brief GATT Characteristic Luminous Flux Range
 */
#define BLE_UUID_GATT_LUMFLXR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_LUMFLXR_VAL)
/**
 *  @brief GATT Characteristic Luminous Intensity UUID Value
 */
#define BLE_UUID_GATT_LUMINT_VAL 0x2b01
/**
 *  @brief GATT Characteristic Luminous Intensity
 */
#define BLE_UUID_GATT_LUMINT \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_LUMINT_VAL)
/**
 *  @brief GATT Characteristic Mass Flow UUID Value
 */
#define BLE_UUID_GATT_MASSFLOW_VAL 0x2b02
/**
 *  @brief GATT Characteristic Mass Flow
 */
#define BLE_UUID_GATT_MASSFLOW \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_MASSFLOW_VAL)
/**
 *  @brief GATT Characteristic Perceived Lightness UUID Value
 */
#define BLE_UUID_GATT_PERLGHT_VAL 0x2b03
/**
 *  @brief GATT Characteristic Perceived Lightness
 */
#define BLE_UUID_GATT_PERLGHT \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_PERLGHT_VAL)
/**
 *  @brief GATT Characteristic Percentage 8 UUID Value
 */
#define BLE_UUID_GATT_PER8_VAL 0x2b04
/**
 *  @brief GATT Characteristic Percentage 8
 */
#define BLE_UUID_GATT_PER8 \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_PER8_VAL)
/**
 *  @brief GATT Characteristic Power UUID Value
 */
#define BLE_UUID_GATT_PWR_VAL 0x2b05
/**
 *  @brief GATT Characteristic Power
 */
#define BLE_UUID_GATT_PWR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_PWR_VAL)
/**
 *  @brief GATT Characteristic Power Specification UUID Value
 */
#define BLE_UUID_GATT_PWRSPEC_VAL 0x2b06
/**
 *  @brief GATT Characteristic Power Specification
 */
#define BLE_UUID_GATT_PWRSPEC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_PWRSPEC_VAL)
/**
 *  @brief GATT Characteristic Relative Runtime In A Current Range UUID Value
 */
#define BLE_UUID_GATT_RRICR_VAL 0x2b07
/**
 *  @brief GATT Characteristic Relative Runtime In A Current Range
 */
#define BLE_UUID_GATT_RRICR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_RRICR_VAL)
/**
 *  @brief GATT Characteristic Relative Runtime In A Generic Level Range UUID Value
 */
#define BLE_UUID_GATT_RRIGLR_VAL 0x2b08
/**
 *  @brief GATT Characteristic Relative Runtime In A Generic Level Range
 */
#define BLE_UUID_GATT_RRIGLR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_RRIGLR_VAL)
/**
 *  @brief GATT Characteristic Relative Value In A Voltage Range UUID Value
 */
#define BLE_UUID_GATT_RVIVR_VAL 0x2b09
/**
 *  @brief GATT Characteristic Relative Value In A Voltage Range
 */
#define BLE_UUID_GATT_RVIVR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_RVIVR_VAL)
/**
 *  @brief GATT Characteristic Relative Value In A Illuminance Range UUID Value
 */
#define BLE_UUID_GATT_RVIIR_VAL 0x2b0a
/**
 *  @brief GATT Characteristic Relative Value In A Illuminance Range
 */
#define BLE_UUID_GATT_RVIIR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_RVIIR_VAL)
/**
 *  @brief GATT Characteristic Relative Value In A Period Of Day UUID Value
 */
#define BLE_UUID_GATT_RVIPOD_VAL 0x2b0b
/**
 *  @brief GATT Characteristic Relative Value In A Period Of Day
 */
#define BLE_UUID_GATT_RVIPOD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_RVIPOD_VAL)
/**
 *  @brief GATT Characteristic Relative Value In A Temperature Range UUID Value
 */
#define BLE_UUID_GATT_RVITR_VAL 0x2b0c
/**
 *  @brief GATT Characteristic Relative Value In A Temperature Range
 */
#define BLE_UUID_GATT_RVITR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_RVITR_VAL)
/**
 *  @brief GATT Characteristic Temperature 8 UUID Value
 */
#define BLE_UUID_GATT_TEMP8_VAL 0x2b0d
/**
 *  @brief GATT Characteristic Temperature 8
 */
#define BLE_UUID_GATT_TEMP8 \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TEMP8_VAL)
/**
 *  @brief GATT Characteristic Temperature 8 In A Period Of Day UUID Value
 */
#define BLE_UUID_GATT_TEMP8_IPOD_VAL 0x2b0e
/**
 *  @brief GATT Characteristic Temperature 8 In A Period Of Day
 */
#define BLE_UUID_GATT_TEMP8_IPOD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TEMP8_IPOD_VAL)
/**
 *  @brief GATT Characteristic Temperature 8 Statistics UUID Value
 */
#define BLE_UUID_GATT_TEMP8_STAT_VAL 0x2b0f
/**
 *  @brief GATT Characteristic Temperature 8 Statistics
 */
#define BLE_UUID_GATT_TEMP8_STAT \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TEMP8_STAT_VAL)
/**
 *  @brief GATT Characteristic Temperature Range UUID Value
 */
#define BLE_UUID_GATT_TEMP_RNG_VAL 0x2b10
/**
 *  @brief GATT Characteristic Temperature Range
 */
#define BLE_UUID_GATT_TEMP_RNG \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TEMP_RNG_VAL)
/**
 *  @brief GATT Characteristic Temperature Statistics UUID Value
 */
#define BLE_UUID_GATT_TEMP_STAT_VAL 0x2b11
/**
 *  @brief GATT Characteristic Temperature Statistics
 */
#define BLE_UUID_GATT_TEMP_STAT \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TEMP_STAT_VAL)
/**
 *  @brief GATT Characteristic Time Decihour 8 UUID Value
 */
#define BLE_UUID_GATT_TIM_DC8_VAL 0x2b12
/**
 *  @brief GATT Characteristic Time Decihour 8
 */
#define BLE_UUID_GATT_TIM_DC8 \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TIM_DC8_VAL)
/**
 *  @brief GATT Characteristic Time Exponential 8 UUID Value
 */
#define BLE_UUID_GATT_TIM_EXP8_VAL 0x2b13
/**
 *  @brief GATT Characteristic Time Exponential 8
 */
#define BLE_UUID_GATT_TIM_EXP8 \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TIM_EXP8_VAL)
/**
 *  @brief GATT Characteristic Time Hour 24 UUID Value
 */
#define BLE_UUID_GATT_TIM_H24_VAL 0x2b14
/**
 *  @brief GATT Characteristic Time Hour 24
 */
#define BLE_UUID_GATT_TIM_H24 \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TIM_H24_VAL)
/**
 *  @brief GATT Characteristic Time Millisecond 24 UUID Value
 */
#define BLE_UUID_GATT_TIM_MS24_VAL 0x2b15
/**
 *  @brief GATT Characteristic Time Millisecond 24
 */
#define BLE_UUID_GATT_TIM_MS24 \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TIM_MS24_VAL)
/**
 *  @brief GATT Characteristic Time Second 16 UUID Value
 */
#define BLE_UUID_GATT_TIM_S16_VAL 0x2b16
/**
 *  @brief GATT Characteristic Time Second 16
 */
#define BLE_UUID_GATT_TIM_S16 \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TIM_S16_VAL)
/**
 *  @brief GATT Characteristic Time Second 8 UUID Value
 */
#define BLE_UUID_GATT_TIM_S8_VAL 0x2b17
/**
 *  @brief GATT Characteristic Time Second 8
 */
#define BLE_UUID_GATT_TIM_S8 \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TIM_S8_VAL)
/**
 *  @brief GATT Characteristic Voltage UUID Value
 */
#define BLE_UUID_GATT_V_VAL 0x2b18
/**
 *  @brief GATT Characteristic Voltage
 */
#define BLE_UUID_GATT_V \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_V_VAL)
/**
 *  @brief GATT Characteristic Voltage Specification UUID Value
 */
#define BLE_UUID_GATT_V_SPEC_VAL 0x2b19
/**
 *  @brief GATT Characteristic Voltage Specification
 */
#define BLE_UUID_GATT_V_SPEC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_V_SPEC_VAL)
/**
 *  @brief GATT Characteristic Voltage Statistics UUID Value
 */
#define BLE_UUID_GATT_V_STAT_VAL 0x2b1a
/**
 *  @brief GATT Characteristic Voltage Statistics
 */
#define BLE_UUID_GATT_V_STAT \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_V_STAT_VAL)
/**
 *  @brief GATT Characteristic Volume Flow UUID Value
 */
#define BLE_UUID_GATT_VOLF_VAL 0x2b1b
/**
 *  @brief GATT Characteristic Volume Flow
 */
#define BLE_UUID_GATT_VOLF \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_VOLF_VAL)
/**
 *  @brief GATT Characteristic Chromaticity Coordinate (not Coordinates) UUID Value
 */
#define BLE_UUID_GATT_CRCOORD_VAL 0x2b1c
/**
 *  @brief GATT Characteristic Chromaticity Coordinate (not Coordinates)
 */
#define BLE_UUID_GATT_CRCOORD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CRCOORD_VAL)
/**
 *  @brief GATT Characteristic RC Feature UUID Value
 */
#define BLE_UUID_GATT_RCF_VAL 0x2b1d
/**
 *  @brief GATT Characteristic RC Feature
 */
#define BLE_UUID_GATT_RCF \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_RCF_VAL)
/**
 *  @brief GATT Characteristic RC Settings UUID Value
 */
#define BLE_UUID_GATT_RCSET_VAL 0x2b1e
/**
 *  @brief GATT Characteristic RC Settings
 */
#define BLE_UUID_GATT_RCSET \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_RCSET_VAL)
/**
 *  @brief GATT Characteristic Reconnection Configuration Control Point UUID Value
 */
#define BLE_UUID_GATT_RCCP_VAL 0x2b1f
/**
 *  @brief GATT Characteristic Reconnection Configuration Control Point
 */
#define BLE_UUID_GATT_RCCP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_RCCP_VAL)
/**
 *  @brief GATT Characteristic IDD Status Changed UUID Value
 */
#define BLE_UUID_GATT_IDD_SC_VAL 0x2b20
/**
 *  @brief GATT Characteristic IDD Status Changed
 */
#define BLE_UUID_GATT_IDD_SC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_IDD_SC_VAL)
/**
 *  @brief GATT Characteristic IDD Status UUID Value
 */
#define BLE_UUID_GATT_IDD_S_VAL 0x2b21
/**
 *  @brief GATT Characteristic IDD Status
 */
#define BLE_UUID_GATT_IDD_S \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_IDD_S_VAL)
/**
 *  @brief GATT Characteristic IDD Annunciation Status UUID Value
 */
#define BLE_UUID_GATT_IDD_AS_VAL 0x2b22
/**
 *  @brief GATT Characteristic IDD Annunciation Status
 */
#define BLE_UUID_GATT_IDD_AS \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_IDD_AS_VAL)
/**
 *  @brief GATT Characteristic IDD Features UUID Value
 */
#define BLE_UUID_GATT_IDD_F_VAL 0x2b23
/**
 *  @brief GATT Characteristic IDD Features
 */
#define BLE_UUID_GATT_IDD_F \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_IDD_F_VAL)
/**
 *  @brief GATT Characteristic IDD Status Reader Control Point UUID Value
 */
#define BLE_UUID_GATT_IDD_SRCP_VAL 0x2b24
/**
 *  @brief GATT Characteristic IDD Status Reader Control Point
 */
#define BLE_UUID_GATT_IDD_SRCP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_IDD_SRCP_VAL)
/**
 *  @brief GATT Characteristic IDD Command Control Point UUID Value
 */
#define BLE_UUID_GATT_IDD_CCP_VAL 0x2b25
/**
 *  @brief GATT Characteristic IDD Command Control Point
 */
#define BLE_UUID_GATT_IDD_CCP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_IDD_CCP_VAL)
/**
 *  @brief GATT Characteristic IDD Command Data UUID Value
 */
#define BLE_UUID_GATT_IDD_CD_VAL 0x2b26
/**
 *  @brief GATT Characteristic IDD Command Data
 */
#define BLE_UUID_GATT_IDD_CD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_IDD_CD_VAL)
/**
 *  @brief GATT Characteristic IDD Record Access Control Point UUID Value
 */
#define BLE_UUID_GATT_IDD_RACP_VAL 0x2b27
/**
 *  @brief GATT Characteristic IDD Record Access Control Point
 */
#define BLE_UUID_GATT_IDD_RACP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_IDD_RACP_VAL)
/**
 *  @brief GATT Characteristic IDD History Data UUID Value
 */
#define BLE_UUID_GATT_IDD_HD_VAL 0x2b28
/**
 *  @brief GATT Characteristic IDD History Data
 */
#define BLE_UUID_GATT_IDD_HD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_IDD_HD_VAL)
/**
 *  @brief GATT Characteristic Client Supported Features UUID value
 */
#define BLE_UUID_GATT_CLIENT_FEATURES_VAL 0x2b29
/**
 *  @brief GATT Characteristic Client Supported Features
 */
#define BLE_UUID_GATT_CLIENT_FEATURES \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CLIENT_FEATURES_VAL)
/**
 *  @brief GATT Characteristic Database Hash UUID value
 */
#define BLE_UUID_GATT_DB_HASH_VAL 0x2b2a
/**
 *  @brief GATT Characteristic Database Hash
 */
#define BLE_UUID_GATT_DB_HASH \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_DB_HASH_VAL)
/**
 *  @brief GATT Characteristic BSS Control Point UUID Value
 */
#define BLE_UUID_GATT_BSS_CP_VAL 0x2b2b
/**
 *  @brief GATT Characteristic BSS Control Point
 */
#define BLE_UUID_GATT_BSS_CP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_BSS_CP_VAL)
/**
 *  @brief GATT Characteristic BSS Response UUID Value
 */
#define BLE_UUID_GATT_BSS_R_VAL 0x2b2c
/**
 *  @brief GATT Characteristic BSS Response
 */
#define BLE_UUID_GATT_BSS_R \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_BSS_R_VAL)
/**
 *  @brief GATT Characteristic Emergency ID UUID Value
 */
#define BLE_UUID_GATT_EMG_ID_VAL 0x2b2d
/**
 *  @brief GATT Characteristic Emergency ID
 */
#define BLE_UUID_GATT_EMG_ID \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_EMG_ID_VAL)
/**
 *  @brief GATT Characteristic Emergency Text UUID Value
 */
#define BLE_UUID_GATT_EMG_TXT_VAL 0x2b2e
/**
 *  @brief GATT Characteristic Emergency Text
 */
#define BLE_UUID_GATT_EMG_TXT \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_EMG_TXT_VAL)
/**
 *  @brief GATT Characteristic ACS Status UUID Value
 */
#define BLE_UUID_GATT_ACS_S_VAL 0x2b2f
/**
 *  @brief GATT Characteristic ACS Status
 */
#define BLE_UUID_GATT_ACS_S \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ACS_S_VAL)
/**
 *  @brief GATT Characteristic ACS Data In UUID Value
 */
#define BLE_UUID_GATT_ACS_DI_VAL 0x2b30
/**
 *  @brief GATT Characteristic ACS Data In
 */
#define BLE_UUID_GATT_ACS_DI \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ACS_DI_VAL)
/**
 *  @brief GATT Characteristic ACS Data Out Notify UUID Value
 */
#define BLE_UUID_GATT_ACS_DON_VAL 0x2b31
/**
 *  @brief GATT Characteristic ACS Data Out Notify
 */
#define BLE_UUID_GATT_ACS_DON \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ACS_DON_VAL)
/**
 *  @brief GATT Characteristic ACS Data Out Indicate UUID Value
 */
#define BLE_UUID_GATT_ACS_DOI_VAL 0x2b32
/**
 *  @brief GATT Characteristic ACS Data Out Indicate
 */
#define BLE_UUID_GATT_ACS_DOI \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ACS_DOI_VAL)
/**
 *  @brief GATT Characteristic ACS Control Point UUID Value
 */
#define BLE_UUID_GATT_ACS_CP_VAL 0x2b33
/**
 *  @brief GATT Characteristic ACS Control Point
 */
#define BLE_UUID_GATT_ACS_CP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ACS_CP_VAL)
/**
 *  @brief GATT Characteristic Enhanced Blood Pressure Measurement UUID Value
 */
#define BLE_UUID_GATT_EBPM_VAL 0x2b34
/**
 *  @brief GATT Characteristic Enhanced Blood Pressure Measurement
 */
#define BLE_UUID_GATT_EBPM \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_EBPM_VAL)
/**
 *  @brief GATT Characteristic Enhanced Intermediate Cuff Pressure UUID Value
 */
#define BLE_UUID_GATT_EICP_VAL 0x2b35
/**
 *  @brief GATT Characteristic Enhanced Intermediate Cuff Pressure
 */
#define BLE_UUID_GATT_EICP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_EICP_VAL)
/**
 *  @brief GATT Characteristic Blood Pressure Record UUID Value
 */
#define BLE_UUID_GATT_BPR_VAL 0x2b36
/**
 *  @brief GATT Characteristic Blood Pressure Record
 */
#define BLE_UUID_GATT_BPR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_BPR_VAL)
/**
 *  @brief GATT Characteristic Registered User UUID Value
 */
#define BLE_UUID_GATT_RU_VAL 0x2b37
/**
 *  @brief GATT Characteristic Registered User
 */
#define BLE_UUID_GATT_RU \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_RU_VAL)
/**
 *  @brief GATT Characteristic BR-EDR Handover Data UUID Value
 */
#define BLE_UUID_GATT_BR_EDR_HD_VAL 0x2b38
/**
 *  @brief GATT Characteristic BR-EDR Handover Data
 */
#define BLE_UUID_GATT_BR_EDR_HD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_BR_EDR_HD_VAL)
/**
 *  @brief GATT Characteristic Bluetooth SIG Data UUID Value
 */
#define BLE_UUID_GATT_BLE_SIG_D_VAL 0x2b39
/**
 *  @brief GATT Characteristic Bluetooth SIG Data
 */
#define BLE_UUID_GATT_BLE_SIG_D \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_BLE_SIG_D_VAL)
/**
 *  @brief GATT Characteristic Server Supported Features UUID value
 */
#define BLE_UUID_GATT_SERVER_FEATURES_VAL  0x2b3a
/**
 *  @brief GATT Characteristic Server Supported Features
 */
#define BLE_UUID_GATT_SERVER_FEATURES      \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_SERVER_FEATURES_VAL)
/**
 *  @brief GATT Characteristic Physical Activity Monitor Features UUID Value
 */
#define BLE_UUID_GATT_PHY_AMF_VAL 0x2b3b
/**
 *  @brief GATT Characteristic Physical Activity Monitor Features
 */
#define BLE_UUID_GATT_PHY_AMF \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_PHY_AMF_VAL)
/**
 *  @brief GATT Characteristic General Activity Instantaneous Data UUID Value
 */
#define BLE_UUID_GATT_GEN_AID_VAL 0x2b3c
/**
 *  @brief GATT Characteristic General Activity Instantaneous Data
 */
#define BLE_UUID_GATT_GEN_AID \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_GEN_AID_VAL)
/**
 *  @brief GATT Characteristic General Activity Summary Data UUID Value
 */
#define BLE_UUID_GATT_GEN_ASD_VAL 0x2b3d
/**
 *  @brief GATT Characteristic General Activity Summary Data
 */
#define BLE_UUID_GATT_GEN_ASD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_GEN_ASD_VAL)
/**
 *  @brief GATT Characteristic CardioRespiratory Activity Instantaneous Data UUID Value
 */
#define BLE_UUID_GATT_CR_AID_VAL 0x2b3e
/**
 *  @brief GATT Characteristic CardioRespiratory Activity Instantaneous Data
 */
#define BLE_UUID_GATT_CR_AID \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CR_AID_VAL)
/**
 *  @brief GATT Characteristic CardioRespiratory Activity Summary Data UUID Value
 */
#define BLE_UUID_GATT_CR_ASD_VAL 0x2b3f
/**
 *  @brief GATT Characteristic CardioRespiratory Activity Summary Data
 */
#define BLE_UUID_GATT_CR_ASD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CR_ASD_VAL)
/**
 *  @brief GATT Characteristic Step Counter Activity Summary Data UUID Value
 */
#define BLE_UUID_GATT_SC_ASD_VAL 0x2b40
/**
 *  @brief GATT Characteristic Step Counter Activity Summary Data
 */
#define BLE_UUID_GATT_SC_ASD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_SC_ASD_VAL)
/**
 *  @brief GATT Characteristic Sleep Activity Instantaneous Data UUID Value
 */
#define BLE_UUID_GATT_SLP_AID_VAL 0x2b41
/**
 *  @brief GATT Characteristic Sleep Activity Instantaneous Data
 */
#define BLE_UUID_GATT_SLP_AID \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_SLP_AID_VAL)
/**
 *  @brief GATT Characteristic Sleep Activity Summary Data UUID Value
 */
#define BLE_UUID_GATT_SLP_ASD_VAL 0x2b42
/**
 *  @brief GATT Characteristic Sleep Activity Summary Data
 */
#define BLE_UUID_GATT_SLP_ASD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_SLP_ASD_VAL)
/**
 *  @brief GATT Characteristic Physical Activity Monitor Control Point UUID Value
 */
#define BLE_UUID_GATT_PHY_AMCP_VAL 0x2b43
/**
 *  @brief GATT Characteristic Physical Activity Monitor Control Point
 */
#define BLE_UUID_GATT_PHY_AMCP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_PHY_AMCP_VAL)
/**
 *  @brief GATT Characteristic Activity Current Session UUID Value
 */
#define BLE_UUID_GATT_ACS_VAL 0x2b44
/**
 *  @brief GATT Characteristic Activity Current Session
 */
#define BLE_UUID_GATT_ACS \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ACS_VAL)
/**
 *  @brief GATT Characteristic Physical Activity Session Descriptor UUID Value
 */
#define BLE_UUID_GATT_PHY_ASDESC_VAL 0x2b45
/**
 *  @brief GATT Characteristic Physical Activity Session Descriptor
 */
#define BLE_UUID_GATT_PHY_ASDESC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_PHY_ASDESC_VAL)
/**
 *  @brief GATT Characteristic Preferred Units UUID Value
 */
#define BLE_UUID_GATT_PREF_U_VAL 0x2b46
/**
 *  @brief GATT Characteristic Preferred Units
 */
#define BLE_UUID_GATT_PREF_U \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_PREF_U_VAL)
/**
 *  @brief GATT Characteristic High Resolution Height UUID Value
 */
#define BLE_UUID_GATT_HRES_H_VAL 0x2b47
/**
 *  @brief GATT Characteristic High Resolution Height
 */
#define BLE_UUID_GATT_HRES_H \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_HRES_H_VAL)
/**
 *  @brief GATT Characteristic Middle Name UUID Value
 */
#define BLE_UUID_GATT_MID_NAME_VAL 0x2b48
/**
 *  @brief GATT Characteristic Middle Name
 */
#define BLE_UUID_GATT_MID_NAME \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_MID_NAME_VAL)
/**
 *  @brief GATT Characteristic Stride Length UUID Value
 */
#define BLE_UUID_GATT_STRDLEN_VAL 0x2b49
/**
 *  @brief GATT Characteristic Stride Length
 */
#define BLE_UUID_GATT_STRDLEN \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_STRDLEN_VAL)
/**
 *  @brief GATT Characteristic Handedness UUID Value
 */
#define BLE_UUID_GATT_HANDEDNESS_VAL 0x2b4a
/**
 *  @brief GATT Characteristic Handedness
 */
#define BLE_UUID_GATT_HANDEDNESS \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_HANDEDNESS_VAL)
/**
 *  @brief GATT Characteristic Device Wearing Position UUID Value
 */
#define BLE_UUID_GATT_DEVICE_WP_VAL 0x2b4b
/**
 *  @brief GATT Characteristic Device Wearing Position
 */
#define BLE_UUID_GATT_DEVICE_WP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_DEVICE_WP_VAL)
/**
 *  @brief GATT Characteristic Four Zone Heart Rate Limit UUID Value
 */
#define BLE_UUID_GATT_4ZHRL_VAL 0x2b4c
/**
 *  @brief GATT Characteristic Four Zone Heart Rate Limit
 */
#define BLE_UUID_GATT_4ZHRL \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_4ZHRL_VAL)
/**
 *  @brief GATT Characteristic High Intensity Exercise Threshold UUID Value
 */
#define BLE_UUID_GATT_HIET_VAL 0x2b4d
/**
 *  @brief GATT Characteristic High Intensity Exercise Threshold
 */
#define BLE_UUID_GATT_HIET \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_HIET_VAL)
/**
 *  @brief GATT Characteristic Activity Goal UUID Value
 */
#define BLE_UUID_GATT_AG_VAL 0x2b4e
/**
 *  @brief GATT Characteristic Activity Goal
 */
#define BLE_UUID_GATT_AG \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_AG_VAL)
/**
 *  @brief GATT Characteristic Sedentary Interval Notification UUID Value
 */
#define BLE_UUID_GATT_SIN_VAL 0x2b4f
/**
 *  @brief GATT Characteristic Sedentary Interval Notification
 */
#define BLE_UUID_GATT_SIN \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_SIN_VAL)
/**
 *  @brief GATT Characteristic Caloric Intake UUID Value
 */
#define BLE_UUID_GATT_CI_VAL 0x2b50
/**
 *  @brief GATT Characteristic Caloric Intake
 */
#define BLE_UUID_GATT_CI \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CI_VAL)
/**
 *  @brief GATT Characteristic TMAP Role UUID Value
 */
#define BLE_UUID_GATT_TMAPR_VAL 0x2b51
/**
 *  @brief GATT Characteristic TMAP Role
 */
#define BLE_UUID_GATT_TMAPR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TMAPR_VAL)
/**
 *  @brief Audio Input Control Service State value
 */
#define BLE_UUID_AICS_STATE_VAL 0x2b77
/**
 *  @brief Audio Input Control Service State
 */
#define BLE_UUID_AICS_STATE \
    BLE_UUID_DECLARE_16(BLE_UUID_AICS_STATE_VAL)
/**
 *  @brief Audio Input Control Service Gain Settings Properties value
 */
#define BLE_UUID_AICS_GAIN_SETTINGS_VAL 0x2b78
/**
 *  @brief Audio Input Control Service Gain Settings Properties
 */
#define BLE_UUID_AICS_GAIN_SETTINGS \
    BLE_UUID_DECLARE_16(BLE_UUID_AICS_GAIN_SETTINGS_VAL)
/**
 *  @brief Audio Input Control Service Input Type value
 */
#define BLE_UUID_AICS_INPUT_TYPE_VAL 0x2b79
/**
 *  @brief Audio Input Control Service Input Type
 */
#define BLE_UUID_AICS_INPUT_TYPE \
    BLE_UUID_DECLARE_16(BLE_UUID_AICS_INPUT_TYPE_VAL)
/**
 *  @brief Audio Input Control Service Input Status value
 */
#define BLE_UUID_AICS_INPUT_STATUS_VAL 0x2b7a
/**
 *  @brief Audio Input Control Service Input Status
 */
#define BLE_UUID_AICS_INPUT_STATUS \
    BLE_UUID_DECLARE_16(BLE_UUID_AICS_INPUT_STATUS_VAL)
/**
 *  @brief Audio Input Control Service Control Point value
 */
#define BLE_UUID_AICS_CONTROL_VAL 0x2b7b
/**
 *  @brief Audio Input Control Service Control Point
 */
#define BLE_UUID_AICS_CONTROL \
    BLE_UUID_DECLARE_16(BLE_UUID_AICS_CONTROL_VAL)
/**
 *  @brief Audio Input Control Service Input Description value
 */
#define BLE_UUID_AICS_DESCRIPTION_VAL 0x2b7c
/**
 *  @brief Audio Input Control Service Input Description
 */
#define BLE_UUID_AICS_DESCRIPTION \
    BLE_UUID_DECLARE_16(BLE_UUID_AICS_DESCRIPTION_VAL)
/**
 *  @brief Volume Control Setting value
 */
#define BLE_UUID_VCS_STATE_VAL 0x2b7d
/**
 *  @brief Volume Control Setting
 */
#define BLE_UUID_VCS_STATE \
    BLE_UUID_DECLARE_16(BLE_UUID_VCS_STATE_VAL)
/**
 *  @brief Volume Control Control point value
 */
#define BLE_UUID_VCS_CONTROL_VAL 0x2b7e
/**
 *  @brief Volume Control Control point
 */
#define BLE_UUID_VCS_CONTROL \
    BLE_UUID_DECLARE_16(BLE_UUID_VCS_CONTROL_VAL)
/**
 *  @brief Volume Control Flags value
 */
#define BLE_UUID_VCS_FLAGS_VAL 0x2b7f
/**
 *  @brief Volume Control Flags
 */
#define BLE_UUID_VCS_FLAGS \
    BLE_UUID_DECLARE_16(BLE_UUID_VCS_FLAGS_VAL)
/**
 *  @brief Volume Offset State value
 */
#define BLE_UUID_VOCS_STATE_VAL 0x2b80
/**
 *  @brief Volume Offset State
 */
#define BLE_UUID_VOCS_STATE \
    BLE_UUID_DECLARE_16(BLE_UUID_VOCS_STATE_VAL)
/**
 *  @brief Audio Location value
 */
#define BLE_UUID_VOCS_LOCATION_VAL 0x2b81
/**
 *  @brief Audio Location
 */
#define BLE_UUID_VOCS_LOCATION \
    BLE_UUID_DECLARE_16(BLE_UUID_VOCS_LOCATION_VAL)
/**
 *  @brief Volume Offset Control Point value
 */
#define BLE_UUID_VOCS_CONTROL_VAL 0x2b82
/**
 *  @brief Volume Offset Control Point
 */
#define BLE_UUID_VOCS_CONTROL \
    BLE_UUID_DECLARE_16(BLE_UUID_VOCS_CONTROL_VAL)
/**
 *  @brief Volume Offset Audio Output Description value
 */
#define BLE_UUID_VOCS_DESCRIPTION_VAL 0x2b83
/**
 *  @brief Volume Offset Audio Output Description
 */
#define BLE_UUID_VOCS_DESCRIPTION \
    BLE_UUID_DECLARE_16(BLE_UUID_VOCS_DESCRIPTION_VAL)
/**
 *  @brief Set Identity Resolving Key value
 */
#define BLE_UUID_CSIS_SIRK_VAL 0x2b84
/**
 *  @brief Set Identity Resolving Key
 */
#define BLE_UUID_CSIS_SIRK         BLE_UUID_DECLARE_16(BLE_UUID_CSIS_SIRK_VAL)
/**
 *  @brief Set size value
 */
#define BLE_UUID_CSIS_SET_SIZE_VAL 0x2b85
/**
 *  @brief Set size
 */
#define BLE_UUID_CSIS_SET_SIZE \
    BLE_UUID_DECLARE_16(BLE_UUID_CSIS_SET_SIZE_VAL)
/**
 *  @brief Set lock value
 */
#define BLE_UUID_CSIS_SET_LOCK_VAL 0x2b86
/**
 *  @brief Set lock
 */
#define BLE_UUID_CSIS_SET_LOCK \
    BLE_UUID_DECLARE_16(BLE_UUID_CSIS_SET_LOCK_VAL)
/**
 *  @brief Rank value
 */
#define BLE_UUID_CSIS_RANK_VAL 0x2b87
/**
 *  @brief Rank
 */
#define BLE_UUID_CSIS_RANK \
    BLE_UUID_DECLARE_16(BLE_UUID_CSIS_RANK_VAL)
/**
 *  @brief GATT Characteristic Encrypted Data Key Material UUID Value
 */
#define BLE_UUID_GATT_EDKM_VAL 0x2b88
/**
 *  @brief GATT Characteristic Encrypted Data Key Material
 */
#define BLE_UUID_GATT_EDKM \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_EDKM_VAL)
/**
 *  @brief GATT Characteristic Apparent Energy 32 UUID Value
 */
#define BLE_UUID_GATT_AE32_VAL 0x2b89
/**
 *  @brief GATT Characteristic Apparent Energy 32
 */
#define BLE_UUID_GATT_AE32 \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_AE32_VAL)
/**
 *  @brief GATT Characteristic Apparent Power UUID Value
 */
#define BLE_UUID_GATT_AP_VAL 0x2b8a
/**
 *  @brief GATT Characteristic Apparent Power
 */
#define BLE_UUID_GATT_AP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_AP_VAL)
/**
 *  @brief GATT Characteristic CO2 Concentration UUID Value
 */
#define BLE_UUID_GATT_CO2CONC_VAL 0x2b8c
/**
 *  @brief GATT Characteristic CO2 Concentration
 */
#define BLE_UUID_GATT_CO2CONC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CO2CONC_VAL)
/**
 *  @brief GATT Characteristic Cosine of the Angle UUID Value
 */
#define BLE_UUID_GATT_COS_VAL 0x2b8d
/**
 *  @brief GATT Characteristic Cosine of the Angle
 */
#define BLE_UUID_GATT_COS \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_COS_VAL)
/**
 *  @brief GATT Characteristic Device Time Feature UUID Value
 */
#define BLE_UUID_GATT_DEVTF_VAL 0x2b8e
/**
 *  @brief GATT Characteristic Device Time Feature
 */
#define BLE_UUID_GATT_DEVTF \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_DEVTF_VAL)
/**
 *  @brief GATT Characteristic Device Time Parameters UUID Value
 */
#define BLE_UUID_GATT_DEVTP_VAL 0x2b8f
/**
 *  @brief GATT Characteristic Device Time Parameters
 */
#define BLE_UUID_GATT_DEVTP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_DEVTP_VAL)
/**
 *  @brief GATT Characteristic Device Time UUID Value
 */
#define BLE_UUID_GATT_DEVT_VAL 0x2b90
/**
 *  @brief GATT Characteristic Device Time
 */
#define BLE_UUID_GATT_DEVT \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_DEVT_VAL)
/**
 *  @brief GATT Characteristic Device Time Control Point UUID Value
 */
#define BLE_UUID_GATT_DEVTCP_VAL 0x2b91
/**
 *  @brief GATT Characteristic Device Time Control Point
 */
#define BLE_UUID_GATT_DEVTCP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_DEVTCP_VAL)
/**
 *  @brief GATT Characteristic Time Change Log Data UUID Value
 */
#define BLE_UUID_GATT_TCLD_VAL 0x2b92
/**
 *  @brief GATT Characteristic Time Change Log Data
 */
#define BLE_UUID_GATT_TCLD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TCLD_VAL)
/**
 *  @brief Media player name value
 */
#define BLE_UUID_MCS_PLAYER_NAME_VAL 0x2b93
/**
 *  @brief Media player name
 */
#define BLE_UUID_MCS_PLAYER_NAME \
    BLE_UUID_DECLARE_16(BLE_UUID_MCS_PLAYER_NAME_VAL)
/**
 *  @brief Media Icon Object ID value
 */
#define BLE_UUID_MCS_ICON_OBJ_ID_VAL 0x2b94
/**
 *  @brief Media Icon Object ID
 */
#define BLE_UUID_MCS_ICON_OBJ_ID \
    BLE_UUID_DECLARE_16(BLE_UUID_MCS_ICON_OBJ_ID_VAL)
/**
 *  @brief Media Icon URL value
 */
#define BLE_UUID_MCS_ICON_URL_VAL 0x2b95
/**
 *  @brief Media Icon URL
 */
#define BLE_UUID_MCS_ICON_URL \
    BLE_UUID_DECLARE_16(BLE_UUID_MCS_ICON_URL_VAL)
/**
 *  @brief Track Changed value
 */
#define BLE_UUID_MCS_TRACK_CHANGED_VAL 0x2b96
/**
 *  @brief Track Changed
 */
#define BLE_UUID_MCS_TRACK_CHANGED \
    BLE_UUID_DECLARE_16(BLE_UUID_MCS_TRACK_CHANGED_VAL)
/**
 *  @brief Track Title value
 */
#define BLE_UUID_MCS_TRACK_TITLE_VAL 0x2b97
/**
 *  @brief Track Title
 */
#define BLE_UUID_MCS_TRACK_TITLE \
    BLE_UUID_DECLARE_16(BLE_UUID_MCS_TRACK_TITLE_VAL)
/**
 *  @brief Track Duration value
 */
#define BLE_UUID_MCS_TRACK_DURATION_VAL 0x2b98
/**
 *  @brief Track Duration
 */
#define BLE_UUID_MCS_TRACK_DURATION \
    BLE_UUID_DECLARE_16(BLE_UUID_MCS_TRACK_DURATION_VAL)
/**
 *  @brief Track Position value
 */
#define BLE_UUID_MCS_TRACK_POSITION_VAL 0x2b99
/**
 *  @brief Track Position
 */
#define BLE_UUID_MCS_TRACK_POSITION \
    BLE_UUID_DECLARE_16(BLE_UUID_MCS_TRACK_POSITION_VAL)
/**
 *  @brief Playback Speed value
 */
#define BLE_UUID_MCS_PLAYBACK_SPEED_VAL 0x2b9a
/**
 *  @brief Playback Speed
 */
#define BLE_UUID_MCS_PLAYBACK_SPEED \
    BLE_UUID_DECLARE_16(BLE_UUID_MCS_PLAYBACK_SPEED_VAL)
/**
 *  @brief Seeking Speed value
 */
#define BLE_UUID_MCS_SEEKING_SPEED_VAL 0x2b9b
/**
 *  @brief Seeking Speed
 */
#define BLE_UUID_MCS_SEEKING_SPEED \
    BLE_UUID_DECLARE_16(BLE_UUID_MCS_SEEKING_SPEED_VAL)
/**
 *  @brief Track Segments Object ID value
 */
#define BLE_UUID_MCS_TRACK_SEGMENTS_OBJ_ID_VAL 0x2b9c
/**
 *  @brief Track Segments Object ID
 */
#define BLE_UUID_MCS_TRACK_SEGMENTS_OBJ_ID \
    BLE_UUID_DECLARE_16(BLE_UUID_MCS_TRACK_SEGMENTS_OBJ_ID_VAL)
/**
 *  @brief Current Track Object ID value
 */
#define BLE_UUID_MCS_CURRENT_TRACK_OBJ_ID_VAL 0x2b9d
/**
 *  @brief Current Track Object ID
 */
#define BLE_UUID_MCS_CURRENT_TRACK_OBJ_ID \
    BLE_UUID_DECLARE_16(BLE_UUID_MCS_CURRENT_TRACK_OBJ_ID_VAL)
/**
 *  @brief Next Track Object ID value
 */
#define BLE_UUID_MCS_NEXT_TRACK_OBJ_ID_VAL 0x2b9e
/**
 *  @brief Next Track Object ID
 */
#define BLE_UUID_MCS_NEXT_TRACK_OBJ_ID \
    BLE_UUID_DECLARE_16(BLE_UUID_MCS_NEXT_TRACK_OBJ_ID_VAL)
/**
 *  @brief Parent Group Object ID value
 */
#define BLE_UUID_MCS_PARENT_GROUP_OBJ_ID_VAL 0x2b9f
/**
 *  @brief Parent Group Object ID
 */
#define BLE_UUID_MCS_PARENT_GROUP_OBJ_ID \
    BLE_UUID_DECLARE_16(BLE_UUID_MCS_PARENT_GROUP_OBJ_ID_VAL)
/**
 *  @brief Group Object ID value
 */
#define BLE_UUID_MCS_CURRENT_GROUP_OBJ_ID_VAL 0x2ba0
/**
 *  @brief Group Object ID
 */
#define BLE_UUID_MCS_CURRENT_GROUP_OBJ_ID \
    BLE_UUID_DECLARE_16(BLE_UUID_MCS_CURRENT_GROUP_OBJ_ID_VAL)
/**
 *  @brief Playing Order value
 */
#define BLE_UUID_MCS_PLAYING_ORDER_VAL 0x2ba1
/**
 *  @brief Playing Order
 */
#define BLE_UUID_MCS_PLAYING_ORDER \
    BLE_UUID_DECLARE_16(BLE_UUID_MCS_PLAYING_ORDER_VAL)
/**
 *  @brief Playing Orders supported value
 */
#define BLE_UUID_MCS_PLAYING_ORDERS_VAL 0x2ba2
/**
 *  @brief Playing Orders supported
 */
#define BLE_UUID_MCS_PLAYING_ORDERS \
    BLE_UUID_DECLARE_16(BLE_UUID_MCS_PLAYING_ORDERS_VAL)
/**
 *  @brief Media State value
 */
#define BLE_UUID_MCS_MEDIA_STATE_VAL 0x2ba3
/**
 *  @brief Media State
 */
#define BLE_UUID_MCS_MEDIA_STATE \
    BLE_UUID_DECLARE_16(BLE_UUID_MCS_MEDIA_STATE_VAL)
/**
 *  @brief Media Control Point value
 */
#define BLE_UUID_MCS_MEDIA_CONTROL_POINT_VAL 0x2ba4
/**
 *  @brief Media Control Point
 */
#define BLE_UUID_MCS_MEDIA_CONTROL_POINT \
    BLE_UUID_DECLARE_16(BLE_UUID_MCS_MEDIA_CONTROL_POINT_VAL)
/**
 *  @brief Media control opcodes supported value
 */
#define BLE_UUID_MCS_MEDIA_CONTROL_OPCODES_VAL 0x2ba5
/**
 *  @brief Media control opcodes supported
 */
#define BLE_UUID_MCS_MEDIA_CONTROL_OPCODES \
    BLE_UUID_DECLARE_16(BLE_UUID_MCS_MEDIA_CONTROL_OPCODES_VAL)
/**
 *  @brief Search result object ID value
 */
#define BLE_UUID_MCS_SEARCH_RESULTS_OBJ_ID_VAL 0x2ba6
/**
 *  @brief Search result object ID
 */
#define BLE_UUID_MCS_SEARCH_RESULTS_OBJ_ID \
    BLE_UUID_DECLARE_16(BLE_UUID_MCS_SEARCH_RESULTS_OBJ_ID_VAL)
/**
 *  @brief Search control point value
 */
#define BLE_UUID_MCS_SEARCH_CONTROL_POINT_VAL 0x2ba7
/**
 *  @brief Search control point
 */
#define BLE_UUID_MCS_SEARCH_CONTROL_POINT \
    BLE_UUID_DECLARE_16(BLE_UUID_MCS_SEARCH_CONTROL_POINT_VAL)
/**
 *  @brief GATT Characteristic Energy 32 UUID Value
 */
#define BLE_UUID_GATT_E32_VAL 0x2ba8
/**
 *  @brief GATT Characteristic Energy 32
 */
#define BLE_UUID_GATT_E32 \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_E32_VAL)

/**
 *  @brief Media Player Icon Object Type value
 */
#define BLE_UUID_OTS_TYPE_MPL_ICON_VAL 0x2ba9
/**
 *  @brief Media Player Icon Object Type
 */
#define BLE_UUID_OTS_TYPE_MPL_ICON \
    BLE_UUID_DECLARE_16(BLE_UUID_OTS_TYPE_MPL_ICON_VAL)
/**
 *  @brief Track Segments Object Type value
 */
#define BLE_UUID_OTS_TYPE_TRACK_SEGMENT_VAL 0x2baa
/**
 *  @brief Track Segments Object Type
 */
#define BLE_UUID_OTS_TYPE_TRACK_SEGMENT \
    BLE_UUID_DECLARE_16(BLE_UUID_OTS_TYPE_TRACK_SEGMENT_VAL)
/**
 *  @brief Track Object Type value
 */
#define BLE_UUID_OTS_TYPE_TRACK_VAL 0x2bab
/**
 *  @brief Track Object Type
 */
#define BLE_UUID_OTS_TYPE_TRACK \
    BLE_UUID_DECLARE_16(BLE_UUID_OTS_TYPE_TRACK_VAL)
/**
 *  @brief Group Object Type value
 */
#define BLE_UUID_OTS_TYPE_GROUP_VAL 0x2bac
/**
 *  @brief Group Object Type
 */
#define BLE_UUID_OTS_TYPE_GROUP \
    BLE_UUID_DECLARE_16(BLE_UUID_OTS_TYPE_GROUP_VAL)
/**
 *  @brief GATT Characteristic Constant Tone Extension Enable UUID Value
 */
#define BLE_UUID_GATT_CTEE_VAL 0x2bad
/**
 *  @brief GATT Characteristic Constant Tone Extension Enable
 */
#define BLE_UUID_GATT_CTEE \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CTEE_VAL)
/**
 *  @brief GATT Characteristic Advertising Constant Tone Extension Minimum Length UUID Value
 */
#define BLE_UUID_GATT_ACTEML_VAL 0x2bae
/**
 *  @brief GATT Characteristic Advertising Constant Tone Extension Minimum Length
 */
#define BLE_UUID_GATT_ACTEML \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ACTEML_VAL)
/**
 *  @brief GATT Characteristic Advertising Constant Tone Extension Minimum Transmit Count UUID Value
 */
#define BLE_UUID_GATT_ACTEMTC_VAL 0x2baf
/**
 *  @brief GATT Characteristic Advertising Constant Tone Extension Minimum Transmit Count
 */
#define BLE_UUID_GATT_ACTEMTC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ACTEMTC_VAL)
/**
 *  @brief GATT Characteristic Advertising Constant Tone Extension Transmit Duration UUID Value
 */
#define BLE_UUID_GATT_ACTETD_VAL 0x2bb0
/**
 *  @brief GATT Characteristic Advertising Constant Tone Extension Transmit Duration
 */
#define BLE_UUID_GATT_ACTETD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ACTETD_VAL)
/**
 *  @brief GATT Characteristic Advertising Constant Tone Extension Interval UUID Value
 */
#define BLE_UUID_GATT_ACTEI_VAL 0x2bb1
/**
 *  @brief GATT Characteristic Advertising Constant Tone Extension Interval
 */
#define BLE_UUID_GATT_ACTEI \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ACTEI_VAL)
/**
 *  @brief GATT Characteristic Advertising Constant Tone Extension PHY UUID Value
 */
#define BLE_UUID_GATT_ACTEP_VAL 0x2bb2
/**
 *  @brief GATT Characteristic Advertising Constant Tone Extension PHY
 */
#define BLE_UUID_GATT_ACTEP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ACTEP_VAL)
/**
 *  @brief Bearer Provider Name value
 */
#define BLE_UUID_TBS_PROVIDER_NAME_VAL 0x2bb3
/**
 *  @brief Bearer Provider Name
 */
#define BLE_UUID_TBS_PROVIDER_NAME \
    BLE_UUID_DECLARE_16(BLE_UUID_TBS_PROVIDER_NAME_VAL)
/**
 *  @brief Bearer UCI value
 */
#define BLE_UUID_TBS_UCI_VAL 0x2bb4
/**
 *  @brief Bearer UCI
 */
#define BLE_UUID_TBS_UCI \
    BLE_UUID_DECLARE_16(BLE_UUID_TBS_UCI_VAL)
/**
 *  @brief Bearer Technology value
 */
#define BLE_UUID_TBS_TECHNOLOGY_VAL 0x2bb5
/**
 *  @brief Bearer Technology
 */
#define BLE_UUID_TBS_TECHNOLOGY \
    BLE_UUID_DECLARE_16(BLE_UUID_TBS_TECHNOLOGY_VAL)
/**
 *  @brief Bearer URI Prefixes Supported List value
 */
#define BLE_UUID_TBS_URI_LIST_VAL 0x2bb6
/**
 *  @brief Bearer URI Prefixes Supported List
 */
#define BLE_UUID_TBS_URI_LIST \
    BLE_UUID_DECLARE_16(BLE_UUID_TBS_URI_LIST_VAL)
/**
 *  @brief Bearer Signal Strength value
 */
#define BLE_UUID_TBS_SIGNAL_STRENGTH_VAL 0x2bb7
/**
 *  @brief Bearer Signal Strength
 */
#define BLE_UUID_TBS_SIGNAL_STRENGTH \
    BLE_UUID_DECLARE_16(BLE_UUID_TBS_SIGNAL_STRENGTH_VAL)
/**
 *  @brief Bearer Signal Strength Reporting Interval value
 */
#define BLE_UUID_TBS_SIGNAL_INTERVAL_VAL 0x2bb8
/**
 *  @brief Bearer Signal Strength Reporting Interval
 */
#define BLE_UUID_TBS_SIGNAL_INTERVAL \
    BLE_UUID_DECLARE_16(BLE_UUID_TBS_SIGNAL_INTERVAL_VAL)
/**
 *  @brief Bearer List Current Calls value
 */
#define BLE_UUID_TBS_LIST_CURRENT_CALLS_VAL 0x2bb9
/**
 *  @brief Bearer List Current Calls
 */
#define BLE_UUID_TBS_LIST_CURRENT_CALLS \
    BLE_UUID_DECLARE_16(BLE_UUID_TBS_LIST_CURRENT_CALLS_VAL)
/**
 *  @brief Content Control ID value
 */
#define BLE_UUID_CCID_VAL 0x2bba
/**
 *  @brief Content Control ID
 */
#define BLE_UUID_CCID \
    BLE_UUID_DECLARE_16(BLE_UUID_CCID_VAL)
/**
 *  @brief Status flags value
 */
#define BLE_UUID_TBS_STATUS_FLAGS_VAL 0x2bbb
/**
 *  @brief Status flags
 */
#define BLE_UUID_TBS_STATUS_FLAGS \
    BLE_UUID_DECLARE_16(BLE_UUID_TBS_STATUS_FLAGS_VAL)
/**
 *  @brief Incoming Call Target Caller ID value
 */
#define BLE_UUID_TBS_INCOMING_URI_VAL 0x2bbc
/**
 *  @brief Incoming Call Target Caller ID
 */
#define BLE_UUID_TBS_INCOMING_URI \
    BLE_UUID_DECLARE_16(BLE_UUID_TBS_INCOMING_URI_VAL)
/**
 *  @brief Call State value
 */
#define BLE_UUID_TBS_CALL_STATE_VAL 0x2bbd
/**
 *  @brief Call State
 */
#define BLE_UUID_TBS_CALL_STATE \
    BLE_UUID_DECLARE_16(BLE_UUID_TBS_CALL_STATE_VAL)
/**
 *  @brief Call Control Point value
 */
#define BLE_UUID_TBS_CALL_CONTROL_POINT_VAL 0x2bbe
/**
 *  @brief Call Control Point
 */
#define BLE_UUID_TBS_CALL_CONTROL_POINT \
    BLE_UUID_DECLARE_16(BLE_UUID_TBS_CALL_CONTROL_POINT_VAL)
/**
 *  @brief Optional Opcodes value
 */
#define BLE_UUID_TBS_OPTIONAL_OPCODES_VAL 0x2bbf
/**
 *  @brief Optional Opcodes
 */
#define BLE_UUID_TBS_OPTIONAL_OPCODES \
    BLE_UUID_DECLARE_16(BLE_UUID_TBS_OPTIONAL_OPCODES_VAL)
/** BLE_UUID_TBS_TERMINATE_REASON_VAL
 *  @brief Terminate reason value
 */
#define BLE_UUID_TBS_TERMINATE_REASON_VAL 0x2bc0
/** BLE_UUID_TBS_TERMINATE_REASON
 *  @brief Terminate reason
 */
#define BLE_UUID_TBS_TERMINATE_REASON \
    BLE_UUID_DECLARE_16(BLE_UUID_TBS_TERMINATE_REASON_VAL)
/**
 *  @brief Incoming Call value
 */
#define BLE_UUID_TBS_INCOMING_CALL_VAL 0x2bc1
/**
 *  @brief Incoming Call
 */
#define BLE_UUID_TBS_INCOMING_CALL \
    BLE_UUID_DECLARE_16(BLE_UUID_TBS_INCOMING_CALL_VAL)
/**
 *  @brief Incoming Call Friendly name value
 */
#define BLE_UUID_TBS_FRIENDLY_NAME_VAL 0x2bc2
/**
 *  @brief Incoming Call Friendly name
 */
#define BLE_UUID_TBS_FRIENDLY_NAME \
    BLE_UUID_DECLARE_16(BLE_UUID_TBS_FRIENDLY_NAME_VAL)
/**
 *  @brief Microphone Control Service Mute value
 */
#define BLE_UUID_MICS_MUTE_VAL 0x2bc3
/**
 *  @brief Microphone Control Service Mute
 */
#define BLE_UUID_MICS_MUTE \
    BLE_UUID_DECLARE_16(BLE_UUID_MICS_MUTE_VAL)
/**
 *  @brief Audio Stream Endpoint Sink Characteristic value
 */
#define BLE_UUID_ASCS_ASE_SNK_VAL 0x2bc4
/**
 *  @brief Audio Stream Endpoint Sink Characteristic
 */
#define BLE_UUID_ASCS_ASE_SNK \
    BLE_UUID_DECLARE_16(BLE_UUID_ASCS_ASE_SNK_VAL)
/**
 *  @brief Audio Stream Endpoint Source Characteristic value
 */
#define BLE_UUID_ASCS_ASE_SRC_VAL 0x2bc5
/**
 *  @brief Audio Stream Endpoint Source Characteristic
 */
#define BLE_UUID_ASCS_ASE_SRC \
    BLE_UUID_DECLARE_16(BLE_UUID_ASCS_ASE_SRC_VAL)
/**
 *  @brief Audio Stream Endpoint Control Point Characteristic value
 */
#define BLE_UUID_ASCS_ASE_CP_VAL 0x2bc6
/**
 *  @brief Audio Stream Endpoint Control Point Characteristic
 */
#define BLE_UUID_ASCS_ASE_CP \
    BLE_UUID_DECLARE_16(BLE_UUID_ASCS_ASE_CP_VAL)
/**
 *  @brief Broadcast Audio Scan Service Scan State value
 */
#define BLE_UUID_BASS_CONTROL_POINT_VAL 0x2bc7
/**
 *  @brief Broadcast Audio Scan Service Scan State
 */
#define BLE_UUID_BASS_CONTROL_POINT \
    BLE_UUID_DECLARE_16(BLE_UUID_BASS_CONTROL_POINT_VAL)
/**
 *  @brief Broadcast Audio Scan Service Receive State value
 */
#define BLE_UUID_BASS_RECV_STATE_VAL 0x2bc8
/**
 *  @brief Broadcast Audio Scan Service Receive State
 */
#define BLE_UUID_BASS_RECV_STATE \
    BLE_UUID_DECLARE_16(BLE_UUID_BASS_RECV_STATE_VAL)
/**
 *  @brief Sink PAC Characteristic value
 */
#define BLE_UUID_PACS_SNK_VAL 0x2bc9
/**
 *  @brief Sink PAC Characteristic
 */
#define BLE_UUID_PACS_SNK \
    BLE_UUID_DECLARE_16(BLE_UUID_PACS_SNK_VAL)
/**
 *  @brief Sink PAC Locations Characteristic value
 */
#define BLE_UUID_PACS_SNK_LOC_VAL 0x2bca
/**
 *  @brief Sink PAC Locations Characteristic
 */
#define BLE_UUID_PACS_SNK_LOC \
    BLE_UUID_DECLARE_16(BLE_UUID_PACS_SNK_LOC_VAL)
/**
 *  @brief Source PAC Characteristic value
 */
#define BLE_UUID_PACS_SRC_VAL 0x2bcb
/**
 *  @brief Source PAC Characteristic
 */
#define BLE_UUID_PACS_SRC \
    BLE_UUID_DECLARE_16(BLE_UUID_PACS_SRC_VAL)
/**
 *  @brief Source PAC Locations Characteristic value
 */
#define BLE_UUID_PACS_SRC_LOC_VAL 0x2bcc
/**
 *  @brief Source PAC Locations Characteristic
 */
#define BLE_UUID_PACS_SRC_LOC \
    BLE_UUID_DECLARE_16(BLE_UUID_PACS_SRC_LOC_VAL)
/**
 *  @brief Available Audio Contexts Characteristic value
 */
#define BLE_UUID_PACS_AVAILABLE_CONTEXT_VAL 0x2bcd
/**
 *  @brief Available Audio Contexts Characteristic
 */
#define BLE_UUID_PACS_AVAILABLE_CONTEXT \
    BLE_UUID_DECLARE_16(BLE_UUID_PACS_AVAILABLE_CONTEXT_VAL)
/**
 *  @brief Supported Audio Context Characteristic value
 */
#define BLE_UUID_PACS_SUPPORTED_CONTEXT_VAL 0x2bce
/**
 *  @brief Supported Audio Context Characteristic
 */
#define BLE_UUID_PACS_SUPPORTED_CONTEXT \
    BLE_UUID_DECLARE_16(BLE_UUID_PACS_SUPPORTED_CONTEXT_VAL)
/**
 *  @brief GATT Characteristic Ammonia Concentration UUID Value
 */
#define BLE_UUID_GATT_NH4CONC_VAL 0x2bcf
/**
 *  @brief GATT Characteristic Ammonia Concentration
 */
#define BLE_UUID_GATT_NH4CONC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_NH4CONC_VAL)
/**
 *  @brief GATT Characteristic Carbon Monoxide Concentration UUID Value
 */
#define BLE_UUID_GATT_COCONC_VAL 0x2bd0
/**
 *  @brief GATT Characteristic Carbon Monoxide Concentration
 */
#define BLE_UUID_GATT_COCONC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_COCONC_VAL)
/**
 *  @brief GATT Characteristic Methane Concentration UUID Value
 */
#define BLE_UUID_GATT_CH4CONC_VAL 0x2bd1
/**
 *  @brief GATT Characteristic Methane Concentration
 */
#define BLE_UUID_GATT_CH4CONC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_CH4CONC_VAL)
/**
 *  @brief GATT Characteristic Nitrogen Dioxide Concentration UUID Value
 */
#define BLE_UUID_GATT_NO2CONC_VAL 0x2bd2
/**
 *  @brief GATT Characteristic Nitrogen Dioxide Concentration
 */
#define BLE_UUID_GATT_NO2CONC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_NO2CONC_VAL)
/**
 *  @brief GATT Characteristic Non-Methane Volatile Organic Compounds Concentration UUID Value
 */
#define BLE_UUID_GATT_NONCH4CONC_VAL 0x2bd3
/**
 *  @brief GATT Characteristic Non-Methane Volatile Organic Compounds Concentration
 */
#define BLE_UUID_GATT_NONCH4CONC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_NONCH4CONC_VAL)
/**
 *  @brief GATT Characteristic Ozone Concentration UUID Value
 */
#define BLE_UUID_GATT_O3CONC_VAL 0x2bd4
/**
 *  @brief GATT Characteristic Ozone Concentration
 */
#define BLE_UUID_GATT_O3CONC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_O3CONC_VAL)
/**
 *  @brief GATT Characteristic Particulate Matter - PM1 Concentration UUID Value
 */
#define BLE_UUID_GATT_PM1CONC_VAL 0x2bd5
/**
 *  @brief GATT Characteristic Particulate Matter - PM1 Concentration
 */
#define BLE_UUID_GATT_PM1CONC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_PM1CONC_VAL)
/**
 *  @brief GATT Characteristic Particulate Matter - PM2.5 Concentration UUID Value
 */
#define BLE_UUID_GATT_PM25CONC_VAL 0x2bd6
/**
 *  @brief GATT Characteristic Particulate Matter - PM2.5 Concentration
 */
#define BLE_UUID_GATT_PM25CONC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_PM25CONC_VAL)
/**
 *  @brief GATT Characteristic Particulate Matter - PM10 Concentration UUID Value
 */
#define BLE_UUID_GATT_PM10CONC_VAL 0x2bd7
/**
 *  @brief GATT Characteristic Particulate Matter - PM10 Concentration
 */
#define BLE_UUID_GATT_PM10CONC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_PM10CONC_VAL)
/**
 *  @brief GATT Characteristic Sulfur Dioxide Concentration UUID Value
 */
#define BLE_UUID_GATT_SO2CONC_VAL 0x2bd8
/**
 *  @brief GATT Characteristic Sulfur Dioxide Concentration
 */
#define BLE_UUID_GATT_SO2CONC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_SO2CONC_VAL)
/**
 *  @brief GATT Characteristic Sulfur Hexafluoride Concentration UUID Value
 */
#define BLE_UUID_GATT_SF6CONC_VAL 0x2bd9
/**
 *  @brief GATT Characteristic Sulfur Hexafluoride Concentration
 */
#define BLE_UUID_GATT_SF6CONC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_SF6CONC_VAL)
/**
 *  @brief Hearing Aid Features Characteristic value
 */
#define BLE_UUID_HAS_HEARING_AID_FEATURES_VAL 0x2bda
/**
 *  @brief Hearing Aid Features Characteristic
 */
#define BLE_UUID_HAS_HEARING_AID_FEATURES \
    BLE_UUID_DECLARE_16(BLE_UUID_HAS_HEARING_AID_FEATURES_VAL)
/**
 *  @brief Hearing Aid Preset Control Point Characteristic value
 */
#define BLE_UUID_HAS_PRESET_CONTROL_POINT_VAL 0x2bdb
/**
 *  @brief Hearing Aid Preset Control Point Characteristic
 */
#define BLE_UUID_HAS_PRESET_CONTROL_POINT \
    BLE_UUID_DECLARE_16(BLE_UUID_HAS_PRESET_CONTROL_POINT_VAL)
/**
 *  @brief Active Preset Index Characteristic value
 */
#define BLE_UUID_HAS_ACTIVE_PRESET_INDEX_VAL 0x2bdc
/**
 *  @brief Active Preset Index Characteristic
 */
#define BLE_UUID_HAS_ACTIVE_PRESET_INDEX \
    BLE_UUID_DECLARE_16(BLE_UUID_HAS_ACTIVE_PRESET_INDEX_VAL)
/**
 *  @brief GATT Characteristic Fixed String 64 UUID Value
 */
#define BLE_UUID_GATT_FSTR64_VAL 0x2bde
/**
 *  @brief GATT Characteristic Fixed String 64
 */
#define BLE_UUID_GATT_FSTR64 \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_FSTR64_VAL)
/**
 *  @brief GATT Characteristic High Temperature UUID Value
 */
#define BLE_UUID_GATT_HITEMP_VAL 0x2bdf
/**
 *  @brief GATT Characteristic High Temperature
 */
#define BLE_UUID_GATT_HITEMP \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_HITEMP_VAL)
/**
 *  @brief GATT Characteristic High Voltage UUID Value
 */
#define BLE_UUID_GATT_HV_VAL 0x2be0
/**
 *  @brief GATT Characteristic High Voltage
 */
#define BLE_UUID_GATT_HV \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_HV_VAL)
/**
 *  @brief GATT Characteristic Light Distribution UUID Value
 */
#define BLE_UUID_GATT_LD_VAL 0x2be1
/**
 *  @brief GATT Characteristic Light Distribution
 */
#define BLE_UUID_GATT_LD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_LD_VAL)
/**
 *  @brief GATT Characteristic Light Output UUID Value
 */
#define BLE_UUID_GATT_LO_VAL 0x2be2
/**
 *  @brief GATT Characteristic Light Output
 */
#define BLE_UUID_GATT_LO \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_LO_VAL)
/**
 *  @brief GATT Characteristic Light Source Type UUID Value
 */
#define BLE_UUID_GATT_LST_VAL 0x2be3
/**
 *  @brief GATT Characteristic Light Source Type
 */
#define BLE_UUID_GATT_LST \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_LST_VAL)
/**
 *  @brief GATT Characteristic Noise UUID Value
 */
#define BLE_UUID_GATT_NOISE_VAL 0x2be4
/**
 *  @brief GATT Characteristic Noise
 */
#define BLE_UUID_GATT_NOISE \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_NOISE_VAL)
/**
 *  @brief GATT Characteristic Relative Runtime in a Correlated Color Temperature Range UUID Value
 */
#define BLE_UUID_GATT_RRCCTP_VAL 0x2be5
/**
 *  @brief GATT Characteristic Relative Runtime in a Correlated Color Temperature Range
 */
#define BLE_UUID_GATT_RRCCTR \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_RRCCTR_VAL)
/**
 *  @brief GATT Characteristic Time Second 32 UUID Value
 */
#define BLE_UUID_GATT_TIM_S32_VAL 0x2be6
/**
 *  @brief GATT Characteristic Time Second 32
 */
#define BLE_UUID_GATT_TIM_S32 \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_TIM_S32_VAL)
/**
 *  @brief GATT Characteristic VOC Concentration UUID Value
 */
#define BLE_UUID_GATT_VOCCONC_VAL 0x2be7
/**
 *  @brief GATT Characteristic VOC Concentration
 */
#define BLE_UUID_GATT_VOCCONC \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_VOCCONC_VAL)
/**
 *  @brief GATT Characteristic Voltage Frequency UUID Value
 */
#define BLE_UUID_GATT_VF_VAL 0x2be8
/**
 *  @brief GATT Characteristic Voltage Frequency
 */
#define BLE_UUID_GATT_VF \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_VF_VAL)
/**
 *  @brief BAS Characteristic Battery Critical Status UUID Value
 */
#define BLE_UUID_BAS_BATTERY_CRIT_STATUS_VAL 0x2be9
/**
 *  @brief BAS Characteristic Battery Critical Status
 */
#define BLE_UUID_BAS_BATTERY_CRIT_STATUS \
    BLE_UUID_DECLARE_16(BLE_UUID_BAS_BATTERY_CRIT_STATUS_VAL)
/**
 *  @brief BAS Characteristic Battery Health Status UUID Value
 */
#define BLE_UUID_BAS_BATTERY_HEALTH_STATUS_VAL 0x2bea
/**
 *  @brief BAS Characteristic Battery Health Status
 */
#define BLE_UUID_BAS_BATTERY_HEALTH_STATUS \
    BLE_UUID_DECLARE_16(BLE_UUID_BAS_BATTERY_HEALTH_STATUS_VAL)
/**
 *  @brief BAS Characteristic Battery Health Information UUID Value
 */
#define BLE_UUID_BAS_BATTERY_HEALTH_INF_VAL 0x2beb
/**
 *  @brief BAS Characteristic Battery Health Information
 */
#define BLE_UUID_BAS_BATTERY_HEALTH_INF \
    BLE_UUID_DECLARE_16(BLE_UUID_BAS_BATTERY_HEALTH_INF_VAL)
/**
 *  @brief BAS Characteristic Battery Information UUID Value
 */
#define BLE_UUID_BAS_BATTERY_INF_VAL 0x2bec
/**
 *  @brief BAS Characteristic Battery Information
 */
#define BLE_UUID_BAS_BATTERY_INF \
    BLE_UUID_DECLARE_16(BLE_UUID_BAS_BATTERY_INF_VAL)
/**
 *  @brief BAS Characteristic Battery Level Status UUID Value
 */
#define BLE_UUID_BAS_BATTERY_LEVEL_STATUS_VAL 0x2bed
/**
 *  @brief BAS Characteristic Battery Level Status
 */
#define BLE_UUID_BAS_BATTERY_LEVEL_STATUS \
    BLE_UUID_DECLARE_16(BLE_UUID_BAS_BATTERY_LEVEL_STATUS_VAL)
/**
 *  @brief BAS Characteristic Battery Time Status UUID Value
 */
#define BLE_UUID_BAS_BATTERY_TIME_STATUS_VAL 0x2bee
/**
 *  @brief BAS Characteristic Battery Time Status
 */
#define BLE_UUID_BAS_BATTERY_TIME_STATUS \
    BLE_UUID_DECLARE_16(BLE_UUID_BAS_BATTERY_TIME_STATUS_VAL)
/**
 *  @brief GATT Characteristic Estimated Service Date UUID Value
 */
#define BLE_UUID_GATT_ESD_VAL 0x2bef
/**
 *  @brief GATT Characteristic Estimated Service Date
 */
#define BLE_UUID_GATT_ESD \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_ESD_VAL)
/**
 *  @brief BAS Characteristic Battery Energy Status UUID Value
 */
#define BLE_UUID_BAS_BATTERY_ENERGY_STATUS_VAL 0x2bf0
/**
 *  @brief BAS Characteristic Battery Energy Status
 */
#define BLE_UUID_BAS_BATTERY_ENERGY_STATUS \
    BLE_UUID_DECLARE_16(BLE_UUID_BAS_BATTERY_ENERGY_STATUS_VAL)
/**
 *  @brief ETS Characteristic Current Elapsed Time UUID Value
 */
#define BLE_UUID_ETS_CURRENT_ELAPSED_TIME_VAL 0x2bf2
/**
 *  @brief ETS Characteristic Current Elapsed Time
 */
#define BLE_UUID_ETS_CURRENT_ELAPSED_TIME \
    BLE_UUID_DECLARE_16(BLE_UUID_ETS_CURRENT_ELAPSED_TIME_VAL)
/**
 *  @brief GATT Characteristic LE GATT Security Levels UUID Value
 */
#define BLE_UUID_GATT_SL_VAL 0x2bf5
/**
 *  @brief GATT Characteristic LE GATT Security Levels
 */
#define BLE_UUID_GATT_SL \
    BLE_UUID_DECLARE_16(BLE_UUID_GATT_SL_VAL)

/**
 *  @brief GATT Characteristic UDI for Medical Devices UUID Value
 */
#define BLE_UUID_UDI_FOR_MEDICAL_DEVICES_VAL 0x2bff
/**
 *  @brief GATT Characteristic UDI for Medical Devices
 */
#define BLE_UUID_UDI_FOR_MEDICAL_DEVICES \
    BLE_UUID_DECLARE_16(BLE_UUID_UDI_FOR_MEDICAL_DEVICES_VAL)

/**
 *  @brief Gaming Service UUID value
 */
#define BLE_UUID_GMAS_VAL 0x1858
/**
 *  @brief Common Audio Service
 */
#define BLE_UUID_GMAS     BLE_UUID_DECLARE_16(BLE_UUID_GMAS_VAL)

/**
 *  @brief Gaming Audio Profile Role UUID value
 */
#define BLE_UUID_GMAP_ROLE_VAL 0x2C00
/**
 *  @brief Gaming Audio Profile Role
 */
#define BLE_UUID_GMAP_ROLE     BLE_UUID_DECLARE_16(BLE_UUID_GMAP_ROLE_VAL)

/**
 *  @brief Gaming Audio Profile Unicast Game Gateway Features UUID value
 */
#define BLE_UUID_GMAP_UGG_FEAT_VAL 0x2C01
/**
 *  @brief Gaming Audio Profile Unicast Game Gateway Features
 */
#define BLE_UUID_GMAP_UGG_FEAT     BLE_UUID_DECLARE_16(BLE_UUID_GMAP_UGG_FEAT_VAL)

/**
 *  @brief Gaming Audio Profile Unicast Game Terminal Features UUID value
 */
#define BLE_UUID_GMAP_UGT_FEAT_VAL 0x2C02
/**
 *  @brief Gaming Audio Profile Unicast Game Terminal Features
 */
#define BLE_UUID_GMAP_UGT_FEAT     BLE_UUID_DECLARE_16(BLE_UUID_GMAP_UGT_FEAT_VAL)

/**
 *  @brief Gaming Audio Profile Broadcast Game Sender Features UUID value
 */
#define BLE_UUID_GMAP_BGS_FEAT_VAL 0x2C03
/**
 *  @brief Gaming Audio Profile Broadcast Game Sender Features
 */
#define BLE_UUID_GMAP_BGS_FEAT     BLE_UUID_DECLARE_16(BLE_UUID_GMAP_BGS_FEAT_VAL)

/**
 *  @brief Gaming Audio Profile Broadcast Game Receiver Features UUID value
 */
#define BLE_UUID_GMAP_BGR_FEAT_VAL 0x2C04
/**
 *  @brief Gaming Audio Profile Broadcast Game Receiver Features
 */
#define BLE_UUID_GMAP_BGR_FEAT     BLE_UUID_DECLARE_16(BLE_UUID_GMAP_BGR_FEAT_VAL)

/*
 * Protocol UUIDs
 */
#define BLE_UUID_SDP_VAL               0x0001
#define BLE_UUID_SDP                   BLE_UUID_DECLARE_16(BLE_UUID_SDP_VAL)
#define BLE_UUID_UDP_VAL               0x0002
#define BLE_UUID_UDP                   BLE_UUID_DECLARE_16(BLE_UUID_UDP_VAL)
#define BLE_UUID_RFCOMM_VAL            0x0003
#define BLE_UUID_RFCOMM                BLE_UUID_DECLARE_16(BLE_UUID_RFCOMM_VAL)
#define BLE_UUID_TCP_VAL               0x0004
#define BLE_UUID_TCP                   BLE_UUID_DECLARE_16(BLE_UUID_TCP_VAL)
#define BLE_UUID_TCS_BIN_VAL           0x0005
#define BLE_UUID_TCS_BIN               BLE_UUID_DECLARE_16(BLE_UUID_TCS_BIN_VAL)
#define BLE_UUID_TCS_AT_VAL            0x0006
#define BLE_UUID_TCS_AT                BLE_UUID_DECLARE_16(BLE_UUID_TCS_AT_VAL)
#define BLE_UUID_ATT_VAL               0x0007
#define BLE_UUID_ATT                   BLE_UUID_DECLARE_16(BLE_UUID_ATT_VAL)
#define BLE_UUID_OBEX_VAL              0x0008
#define BLE_UUID_OBEX                  BLE_UUID_DECLARE_16(BLE_UUID_OBEX_VAL)
#define BLE_UUID_IP_VAL                0x0009
#define BLE_UUID_IP                    BLE_UUID_DECLARE_16(BLE_UUID_IP_VAL)
#define BLE_UUID_FTP_VAL               0x000a
#define BLE_UUID_FTP                   BLE_UUID_DECLARE_16(BLE_UUID_FTP_VAL)
#define BLE_UUID_HTTP_VAL              0x000c
#define BLE_UUID_HTTP                  BLE_UUID_DECLARE_16(BLE_UUID_HTTP_VAL)
#define BLE_UUID_WSP_VAL               0x000e
#define BLE_UUID_WSP                   BLE_UUID_DECLARE_16(BLE_UUID_WSP_VAL)
#define BLE_UUID_BNEP_VAL              0x000f
#define BLE_UUID_BNEP                  BLE_UUID_DECLARE_16(BLE_UUID_BNEP_VAL)
#define BLE_UUID_UPNP_VAL              0x0010
#define BLE_UUID_UPNP                  BLE_UUID_DECLARE_16(BLE_UUID_UPNP_VAL)
#define BLE_UUID_HIDP_VAL              0x0011
#define BLE_UUID_HIDP                  BLE_UUID_DECLARE_16(BLE_UUID_HIDP_VAL)
#define BLE_UUID_HCRP_CTRL_VAL         0x0012
#define BLE_UUID_HCRP_CTRL             BLE_UUID_DECLARE_16(BLE_UUID_HCRP_CTRL_VAL)
#define BLE_UUID_HCRP_DATA_VAL         0x0014
#define BLE_UUID_HCRP_DATA             BLE_UUID_DECLARE_16(BLE_UUID_HCRP_DATA_VAL)
#define BLE_UUID_HCRP_NOTE_VAL         0x0016
#define BLE_UUID_HCRP_NOTE             BLE_UUID_DECLARE_16(BLE_UUID_HCRP_NOTE_VAL)
#define BLE_UUID_AVCTP_VAL             0x0017
#define BLE_UUID_AVCTP                 BLE_UUID_DECLARE_16(BLE_UUID_AVCTP_VAL)
#define BLE_UUID_AVCTP_BROWSING_VAL    0x0018
#define BLE_UUID_AVCTP_BROWSING        BLE_UUID_DECLARE_16(BLE_UUID_AVCTP_BROWSING_VAL)
#define BLE_UUID_AVDTP_VAL             0x0019
#define BLE_UUID_AVDTP                 BLE_UUID_DECLARE_16(BLE_UUID_AVDTP_VAL)
#define BLE_UUID_CMTP_VAL              0x001b
#define BLE_UUID_CMTP                  BLE_UUID_DECLARE_16(BLE_UUID_CMTP_VAL)
#define BLE_UUID_UDI_VAL               0x001d
#define BLE_UUID_UDI                   BLE_UUID_DECLARE_16(BLE_UUID_UDI_VAL)
#define BLE_UUID_MCAP_CTRL_VAL         0x001e
#define BLE_UUID_MCAP_CTRL             BLE_UUID_DECLARE_16(BLE_UUID_MCAP_CTRL_VAL)
#define BLE_UUID_MCAP_DATA_VAL         0x001f
#define BLE_UUID_MCAP_DATA             BLE_UUID_DECLARE_16(BLE_UUID_MCAP_DATA_VAL)
#define BLE_UUID_L2CAP_VAL             0x0100
#define BLE_UUID_L2CAP                 BLE_UUID_DECLARE_16(BLE_UUID_L2CAP_VAL)


#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_UUID_H_ */

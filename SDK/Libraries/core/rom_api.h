/*
File Name    : rom_api.h
Author       : Yichip
Version      : V1.0
Date         : 2022/11/21
Description  : rom api address define.
*/

#ifndef __ROM_API_H__
#define __ROM_API_H__

#include "yc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_USE_ROM_API                   (1)
#define DELAY_USER_ROM_API                  (1)

#define ROM_API_DELAY                       (0x1ff0004+1)
#define ROM_API_DELAY_US                    (0x1ff0008+1)
#define ROM_API_DELAY_MS                    (0x1ff000c+1)

#define ROM_API_INIT_OTP                    (0x1ff0010+1)
#define ROM_API_INIT_OTP_WRITE              (0x1ff0014+1)
#define ROM_API_DEINIT_OTP                  (0x1ff0018+1)
#define ROM_API_READ_OTP                    (0x1ff001c+1)
#define ROM_API_WRITE_OTP                   (0x1ff0020+1)
#define ROM_API_PROGRAM_OTP_BIT             (0x1ff0024+1)

#define ROM_API_QSPI0_DMASTART              (0x1ff0028+1)
#define ROM_API_QSPI0_WRITE                 (0x1ff002c+1)
#define ROM_API_QSPI0_READWRITE             (0x1ff0030+1)
#define ROM_API_QSPI0_FLASHCMD              (0x1ff0034+1)
#define ROM_API_QSPI0_READ_FLASHSTATUS1     (0x1ff0038+1)
#define ROM_API_QSPI0_READ_FLASHSTATUS2     (0x1ff003c+1)
#define ROM_API_QSPI0_WRITE_FLASHSTATUS     (0x1ff0040+1)
#define ROM_API_QSPI0_FLASH_QUADDEABLE      (0x1ff0044+1)
#define ROM_API_QSPI0_FLASH_WRITE           (0x1ff0048+1)
#define ROM_API_QSPI0_FLASH_READ            (0x1ff004c+1)
#define ROM_API_QSPI0_FLASH_CHIPERASE       (0x1ff0050+1)
#define ROM_API_QSPI0_FLASH_ERASE           (0x1ff0054+1)
#define ROM_API_QSPI0_FLASH_JEDECID         (0x1ff0058+1)

#define ROM_API_WFI                         (0x1ff005c+1)
#define ROM_API_CRC16                       (0x1ff0060+1)
#define ROM_API_PARAM_CHECK                 (0x1ff0064+1)
#define ROM_API_FLASH_START                 (0x1ff0068+1)

#define LPM_RAM_FIX_STRUCT_LEN              (24)
#define LPM_RECOVER_STRUCT_LEN              (50)    //qspi_param-crc16

#define OTP_RAM_FIX_NUM                     ((int)5)
#define OTP_RAM_FIX_LEN                     ((int)3*OTP_RAM_FIX_NUM)
#define ROM_GPIO_NUM                        (38)

#define WAKEUP_FLAG_HOST                    (1<<0)
#define WAKEUP_FLAG_KEY_SCAN                (1<<1)
#define WAKEUP_FLAG_RTCOV                   (1<<2)
#define WAKEUP_FLAG_RTC1                    (1<<3)
#define WAKEUP_FLAG_RTC0                    (1<<4)
#define WAKEUP_FLAG_GPIO                    (1<<5)
#define WAKEUP_FLAG_WAKE                    (1<<6)
#define WAKEUP_FLAG_RAM_CRC_ERR             (1<<7)

typedef struct
{
    uint8_t             ram_value_backup[OTP_RAM_FIX_NUM];
    uint8_t             ram_fix_buf[OTP_RAM_FIX_LEN];
    uint16_t            reserved;
    uint16_t            crc16;
} RAM_FixStruct;

typedef struct
{
    union
    {
        uint8_t value;
        struct
        {
            uint8_t io2_3_ctrl      :1;
            uint8_t status_reg_len  :3;
            uint8_t reserved        :4;
        } bit;
    } flag;
    uint8_t             retry_num;
    uint16_t            wake_delay;
    uint32_t            qspi_ctrl;
    uint32_t            code_offset;
    uint8_t             flash_ncs;
    uint8_t             flash_sck;
    uint8_t             flash_io0;
    uint8_t             flash_io1;
    uint8_t             flash_io2;
    uint8_t             flash_io3;
    uint16_t            reserved;

} QSPI_ParamTypeDef;

typedef struct
{
    RAM_FixStruct       ram_fix;//24 byte
    QSPI_ParamTypeDef   qspi_param;//20
    uint32_t            rv_break[2];
    uint8_t             rc384_coarse;
    uint8_t             rc384_fine;
    union
    {
        uint8_t value;
        struct
        {
            uint8_t swtich      :3;
            uint8_t sleep       :3;
            uint8_t reserved    :2;
        } bit;
    } rg_lpm_ldo_dig_retmem_vsel;
    union
    {
        uint8_t value;
        struct
        {
            uint8_t run         :3;
            uint8_t sleep       :3;
            uint8_t reserved    :2;
        } bit;
    } rg_lpm_ldo_dig_lpm_vosel;
    union
    {
        uint8_t value;
        struct
        {
            uint8_t wake_patch  :4;
            uint8_t reserved    :4;
        } bit;
    } rom_flag;
    uint8_t             gpio_revover_cfg[((ROM_GPIO_NUM*3)+7)/8];//15
    uint16_t            crc16;
    uint8_t             wakeup_flag;
    uint8_t             gpio_in[(ROM_GPIO_NUM+7)/8];//5
} LPM_RecoverStruct;//80(0x50) bytes

#define LPM_RECOVER_STRUCT_ADDR             ((uint32_t)0x10000)
#define LpmRecoverStruct ((LPM_RecoverStruct*)LPM_RECOVER_STRUCT_ADDR)

void static inline prefetch(void *start_addr, void *end_addr, int permanent)
{
    int addr, val = 1;
    for(addr = (int)start_addr & ~0x1f;addr < (int)end_addr + 32;addr += 32)
        if(permanent)
            asm volatile("sw %1,0(%0)" ::"r"(addr), "r"(val));
        else
            asm volatile("lw x0,0(%0)" ::"r"(addr));
}


#ifdef __cplusplus
}
#endif

#endif // __ROM_API_H__

/*
File Name    : yc_otp.c
Author       : Yichip
Version      : V1.0
Date         : 2022/12/12
Description  : otp interface implement.
*/

#include "yc_otp.h"
#include "rom_api.h"

#define OTP_WRITE_CHGPUMP_CFG   0x07

/**
 * @method OTP_Init
 * @brief  init otp module
 * @param  NULL
 * @retval NULL
 */
void OTP_Init(void)
{
    ((void(*)())ROM_API_INIT_OTP)();
}

/**
 * @method OTP_Deinit
 * @brief  deinit otp module
 * @param  NULL
 * @retval NULL
 */
void OTP_Deinit(void)
{
    ((void(*)())ROM_API_DEINIT_OTP)();
}

/**
 * @method OTP_Read
 * @brief  read otp data
 * @param  addr:   read addr
 * @param  *data:  read data buf ptr
 * @param  len:    read data len
 * @retval SUCCESS / ERROR
 */
uint8_t OTP_Read(uint32_t addr,uint8_t *data,uint32_t len)
{
    OTP_Init();
    ((uint8_t(*)(uint32_t addr,uint8_t *data,uint32_t len))ROM_API_READ_OTP)(addr, data, len);
    OTP_Deinit();
    return SUCCESS;
}

/*
 * @method OTP_Write
 * @brief:  write otp data
 * @param:  addr:  write addr
 * @param:  *data: write data buf ptr
 * @param:  len:   write data len
 * @return: SUCCESS or ERROR
 */
uint8_t OTP_Write(uint32_t addr, uint8_t *data, uint32_t len)
{
    return ((uint8_t(*)(uint32_t addr, uint8_t *data, uint32_t len,uint8_t chgpumpConfig))ROM_API_WRITE_OTP)(addr, data, len,OTP_WRITE_CHGPUMP_CFG);
}

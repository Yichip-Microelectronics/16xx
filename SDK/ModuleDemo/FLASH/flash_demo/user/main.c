/*
 * Copyright (c) 2006-2020, YICHIP Development Team
 * @file     main.c
 * @brief    flash function tests.
 *
 * Change Logs:
 * Date           Author             Version        Notes
 * 2023-05-26     jiangheng          V1.0.0         the first version
 */

/* Includes ------------------------------------------------------------------*/
#include "yc_flash.h"
#include "stdlib.h"

#define CPU_READ_PARAM_AREA_TEST    (1)
#define FLASH_PARAM_ADDR            (0x3e000)   /*所选的地址必须满足(FLASH_PARAM_ADDR-4096-8-16)/34能够整除*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TEST_FLASH_ADDR             (0x30000)
#define TEST_FLASH_LEN              (0x10000)
#define SECTOR_SIZE                 (0x1000)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/


/**
 * @brief generate random data
 * @param  len : buff length
 * @param  *buff : buff ptr
 * @retval None
 */
void gen_random_array(uint16_t len, uint8_t *buff)
{
    for (uint16_t i = 0; i < len; i++)
    {
        buff[i] = (uint8_t)(rand() % 256);
    }
}

/**
 * @brief  flash full chip lock and unlock test
 * @param  None
 * @retval None
 */
void FLASH_LockUnlockTest(void)
{
    if (FLASH_Lock() == SUCCESS)
        yc_log("FLASH_Lock success");
    else
        yc_log("FLASH_Lock Fail");

    if (FLASH_Unlock() == SUCCESS)
        yc_log("FLASH_Unlock success");
    else
        yc_log("FLASH_Unlock Fail");
}

/**
 * @brief flash 10KB write and read test
 * @param flash_addr The flash addr to be read to or to be written to
 * @param len The databuf addr for len
 * @return None
 */
uint8_t FLASH_RWTest(uint32_t flash_addr, uint32_t len)
{
    uint8_t tbuf[SECTOR_SIZE];
    uint8_t rbuf[SECTOR_SIZE];
    uint32_t tested_len = 0;
    uint32_t packlen = 0;

    if(flash_addr%SECTOR_SIZE != 0)
    {
        yc_log("flash_addr unaligned[flash_addr=%x]",flash_addr);
        return ERROR;
    }

    gen_random_array(sizeof(tbuf), tbuf);
    while(tested_len < len)
    {
        packlen = (len-tested_len) > SECTOR_SIZE ? SECTOR_SIZE : (len-tested_len);
        yc_log("test flash_addr=%x",flash_addr);
        FLASH_SectorErase(flash_addr);
        FLASH_Read(flash_addr,packlen,rbuf);
         for(int j=0;j<packlen;j++)
        {
            if(rbuf[j] != 0xff)
            {
                yc_log("erase error break! rbuf[%d] = %d",j,rbuf[j]);
                return ERROR;
            }
        }
        FLASH_Write(flash_addr, packlen, tbuf);
        FLASH_Read(flash_addr, packlen, rbuf);
        if (0 != memcmp(tbuf, rbuf, packlen))
        {
            yc_log("FLASH_RWTest Fail[flash_addr=%x]",flash_addr);
            printv("tbuf",tbuf,sizeof(tbuf));
            printv("rbuf",rbuf,sizeof(rbuf));
            return ERROR;
        }

        flash_addr += packlen;
        tested_len += packlen;
    }
    yc_log("FLASH_RWTest success");
    return SUCCESS;
}

#if CPU_READ_PARAM_AREA_TEST
void cpu_read_param_area_test(void)
{
    uint8_t write_buf[256];
    yc_log("PARAM_FLASH_ADDR=%x",FLASH_PARAM_ADDR);
    yc_log("PARAM_MCU_ADDR=%x",PARAM_MCU_ADDR(FLASH_PARAM_ADDR));
    for(int i=0;i<sizeof(write_buf);i++)
    {
        write_buf[i]=i;
    }
    yc_log_buf("write buf",write_buf,sizeof(write_buf));
    FLASH_SectorErase(FLASH_PARAM_ADDR);
    FLASH_Write(FLASH_PARAM_ADDR,sizeof(write_buf),write_buf);
    FLASH_SetParamAddr(PARAM_MCU_ADDR(FLASH_PARAM_ADDR));
    yc_log_buf("cpu read",(uint8_t*)PARAM_MCU_ADDR(FLASH_PARAM_ADDR),sizeof(write_buf));

}
#endif

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{
    SYSCTRL_ChipInit();
    printport_init();
    yc_log("flash demo");
    yc_log("FLASH ID:%x\n",FLASH_GetJedecId());
    FLASH_LockUnlockTest();
    FLASH_RWTest(TEST_FLASH_ADDR, TEST_FLASH_LEN);
    #if CPU_READ_PARAM_AREA_TEST
    cpu_read_param_area_test();
    #endif

    while(1)
    {

    }
}

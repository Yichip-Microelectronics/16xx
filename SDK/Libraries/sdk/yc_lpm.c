/*
File Name    : yc_lpm.c
Author       : Yichip
Version      : V1.0
Date         : 2023/11/21
Description  : lpm driver interface implement.
*/
#include "yc_lpm.h"
#include "yc_flash.h"
#include "yc_systick.h"
#include "yc_rtc.h"
#if(BT_EN)
#include "yc_bt.h"
#endif

#define GPIO_LATCH_CFG_BIT_NUM  (3)
#define MCU_BIST_FIX_EN         *(volatile uint32_t*)(0x84c4)
#define MCU_BIST_RESULT(x)      *(volatile uint32_t*)(0x84d0+(x)*4)

/**
  * @brief  lpm init
  * @param  none
  * @return None
  */
void LPM_Init(void)
{
    //clear lpm gpio latch config
    memset(LpmRecoverStruct->gpio_revover_cfg,0,sizeof(LpmRecoverStruct->gpio_revover_cfg));
    //clear gpio wakeup config & clear gpio wakeup flag
    for(int i = 0; i < (GPIO_NUM/2); i++)
    {
        LPM_PUWAKEUP->gpio_wake_cfg[i]=0;
    }
    //clear rtc wakeup flag
    LPM_RTC->rtc_ov_en.bit.rtc_wk_en=0;
    //clear puwk wakeup flag
    LPM_PUWAKEUP->puchk.bit.enable=0;
    SYSCTRL_WaitLpoEdge(3);
    for(int i = 0; i < GPIO_NUM; i++)
    {
        if(CORE_GPIO->gpio_cfg[i] == 0)
        {
            CORE_GPIO->gpio_cfg[i]=GPIO_MODE_HI_Z;
        }
    }
    //wakeup cnt init
    LPM_CONTROL->digldo_cnt.bit.digldo_cnt=0x02;
    LPM_CONTROL->digldo_cnt.bit.rco_cnt=0x07;
    LPM_CONTROL->xtal_cnt.reg=0x35;
    LPM_CONTROL->exen_cnt.reg=0x08;
}

/**
 * @method LPM_SetGpioLatch
 * @brief  set gpio latch type when chip sleep
 * @param  gpiox:gpio number
 * @param  type:LATCH_NONE、LATCH_PD、LATCH_PU、LATCH_OUTPUT_LOW or LATCH_OUTPUT_HIGH
 * @retval none
 */
void LPM_SetGpioLatch(GPIO_NumEnum gpiox,LPM_LatchEnum type)
{
    int start_bit = gpiox*GPIO_LATCH_CFG_BIT_NUM;
    int second_bit_num = (GPIO_LATCH_CFG_BIT_NUM-(8-(start_bit%8)));

    LpmRecoverStruct->gpio_revover_cfg[start_bit/8] |= (uint8_t)(type<<(start_bit%8));
    if(second_bit_num>0)
    {
        LpmRecoverStruct->gpio_revover_cfg[(start_bit/8)+1] |= (type>>(GPIO_LATCH_CFG_BIT_NUM-second_bit_num));
    }
}

static bool LPM_BtprepareActionBeforeSleep(LPM_ModeEnum sleepLevel)
{
    #if(BT_EN)
    if(sleepLevel==SLEEP_MODE_LPM)
    {
        rtc_calibration_init();
        return LE_PrepareActionBeforeSleep();
    }
    else
    {
        return true;
    }
    #else
    return true;
    #endif
}

__attribute__((naked)) void sleep_entry(uint8_t  wakeupMode,uint8_t retentionCfg,uint8_t ramLdoCfg,uint8_t save_stack)
{
    if(save_stack)
    {
        //save core reg
        __ASM("addi sp,sp,-64");
        __ASM("sw   s0, 0(sp)");
        __ASM("sw   s1, 4(sp)");
        __ASM("sw   s2, 8(sp)");
        __ASM("sw   s3,12(sp)");
        __ASM("sw   s4,16(sp)");
        __ASM("sw   s5,20(sp)");
        __ASM("sw   s6,24(sp)");
        __ASM("sw   s7,28(sp)");
        __ASM("sw   s8,32(sp)");
        __ASM("sw   s9,36(sp)");
        __ASM("sw  s10,40(sp)");
        __ASM("sw  s11,44(sp)");
        __ASM("sw   ra,48(sp)");
        //save sp&wakeup_mode
        __ASM("li   t1,0x9f1c");
        //sp
        __ASM("mv   t2,sp"    );
        __ASM("srli t2,t2,2"  );
        __ASM("li t3,0xffff"  );
        __ASM("and t2,t2,t3"  );
        //wakeup_mode
        __ASM("slli a0,a0,16" );
        __ASM("or t2,t2,a0"   );//wakeup_mode into bit16
        __ASM("sw    t2,0(t1)");
    }
    //Cache parameters
    __ASM("mv   s1,a1"    );//retentionCfg
    __ASM("mv   s2,a2"    );//ramLdoCfg
    //flash powerdown
    ((void(*)(uint8_t cmd))ROM_API_QSPI0_FLASHCMD)(0xb9);
    //set flash io HI_Z
    #if BOARD_TYPE==BOARD_2025_223_YC1161AA_EVB_V_1_0
    CORE_GPIO->gpio_cfg[LpmRecoverStruct->qspi_param.flash_ncs] = GPIO_MODE_HI_Z;
    #else
    CORE_GPIO->gpio_cfg[LpmRecoverStruct->qspi_param.flash_ncs] = GPIO_MODE_PULL_UP;
    #endif
    CORE_GPIO->gpio_cfg[LpmRecoverStruct->qspi_param.flash_sck] = GPIO_MODE_HI_Z;
    CORE_GPIO->gpio_cfg[LpmRecoverStruct->qspi_param.flash_io0] = GPIO_MODE_HI_Z;
    CORE_GPIO->gpio_cfg[LpmRecoverStruct->qspi_param.flash_io1] = GPIO_MODE_HI_Z;
    CORE_GPIO->gpio_cfg[LpmRecoverStruct->qspi_param.flash_io2] = GPIO_MODE_HI_Z;
    CORE_GPIO->gpio_cfg[LpmRecoverStruct->qspi_param.flash_io3] = GPIO_MODE_HI_Z;
    //vio ldo lowpower mode en
    LPM_HW_CONTROL_NOEXP->da_lpm_ldo_0p33vdd_aon_bypb.bit.da_lpm_ldo_io_doze_en=1;
    ////save fix ram value
    for(int i=0;i<5;i++)
    {
        if(MCU_BIST_FIX_EN&(1<<i))
        {
            uint32_t addr=(MCU_BIST_RESULT(i)>>8);
            uint32_t bit=(addr&0x1f);
            addr = (addr>>5);
            addr *=4;
            switch (i)
            {
            case 0:
                addr +=0x10000;
                break;
            case 1:
                addr +=0x12000;
                break;
            case 2:
                addr +=0x14000;
                break;
            case 3:
                addr +=0x18000;
                break;
            default:
                addr +=0x20000;
                break;
            }
            addr +=(bit/8);
            LpmRecoverStruct->ram_fix.ram_value_backup[i]=(*((uint8_t*)addr));
        }
        else
        {
            LpmRecoverStruct->ram_fix.ram_value_backup[i]=0;
        }
    }
    //update crc
    LpmRecoverStruct->ram_fix.crc16 = ((uint16_t(*)(uint16_t base_crc, uint8_t* data, unsigned int len))ROM_API_CRC16)(0xffff,(uint8_t*)&LpmRecoverStruct->ram_fix,LPM_RAM_FIX_STRUCT_LEN-2);
    //ramr_isolation
    LPM_RAMR->ramr_isolation.bit.retention_memory_chip_select=1;
    LPM_RAMR->ramr_isolation.bit.isolation_gate=1;
    //switch ram power to ram ldo
    __ASM("li   t1,0x9fc2" );
    __ASM("lb   t2,0(t1)"  );
    __ASM("andi t2,t2,0xc0");
    __ASM("or   t2,t2,s1"  );
    __ASM("sb   t2,0(t1)"  );
    LPM_HW_CONTROL_NOEXP->da_lpm_dvdd_retmem_en.bit.da_lpm_dvdd_retmem_mode=0;
    // LPM_HW_CONTROL_NOEXP->rg_lpm_ldo_dig_lpm_vosel.bit.rg_lpm_ldo_dig_retmem_vsel=LpmRecoverStruct->rg_lpm_ldo_dig_retmem_vsel.bit.sleep;//ramr_isolation不能再访问ram
    __ASM("li   t1,0x9fc3" );
    __ASM("lb   t2,0(t1)"  );
    __ASM("andi t2,t2,0x8f");
    __ASM("slli s2,s2,4"   );
    __ASM("or   t2,t2,s2"  );
    __ASM("sb   t2,0(t1)"  );
    LPM_CONTROL->pad_latch.bit.pad_latch=1;
    LPM_CONTROL->lpm_wreg.reg=0x902bed;
    LPM_CONTROL->go2sleep_wr.bit.go2sleep_wr=1;
    __ASM("j .");
}

extern void wakeup_recover(void);
extern void flash_start(void);
void LPM_SleepEntry(LPM_ModeEnum sleepLevel,uint8_t  wakeupMode,uint8_t retentionCfg,uint8_t save_stack)
{
    //CPU降频
    SYSCTRL_ClkInit(SYS_CLK_SOURCE, SYS_CLK_DIV, DPLL_CLKSEL_48MHZ, DPLL_CLKSEL_48MHZ);
    //关闭vdt
    LPM_HW_CONTROL_NOEXP->da_lpm_vdt_dcdc_en.bit.da_lpm_vdt_vdcdc_en=0;
    LPM_HW_CONTROL_NOEXP->da_lpm_vdt_dcdc_en.bit.da_lpm_vdt_vddio_en=0;
    ///*lpm set default*/
    LPM_HW_CONTROL_NOEXP->da_lpm_xtal_ck2bbpll_en.reg=0x6f;
    LPM_HW_CONTROL_NOEXP->da_lpm_ldo_dig_core_vsel.bit.da_lpm_ldo_dig_core_ildsel=2;

    LPM_HW_CONTROL_NOEXP->rg_lpm_xtal_ldo_vref_sel.bit.rg_lpm_xtal_ldo_vref_sel=2;
    LPM_HW_CONTROL_NOEXP->rg_lpm_xtal_ldo_vref_sel.bit.rg_lpm_xtal_ldo_vout_sel=1;
    LPM_HW_CONTROL_NOEXP->rg_lpm_xtal_ldo_vref_sel.bit.da_lpm_xtal_isel=0x0f;

    LPM_HW_CONTROL_NOEXP->rg_lpm_ldo_dig_lpm_vosel.bit.rg_lpm_ldo_dig_lpm_vosel=LpmRecoverStruct->rg_lpm_ldo_dig_lpm_vosel.bit.sleep;
    LPM_HW_CONTROL_NOEXP->rg_lpm_ldo_dig_lpm_vosel.bit.rg_lpm_ldo_dig_retmem_vsel=LpmRecoverStruct->rg_lpm_ldo_dig_retmem_vsel.bit.swtich;
    if(retentionCfg)
    {
        LPM_HW_CONTROL_NOEXP->da_lpm_dvdd_retmem_en.bit.da_lpm_ldo_dig_retmem_en=1;
        if((retentionCfg&0x10)==0x10)
        {
            LPM_CONTROL->lpm_pc.bit.lpm_pc=(((uint32_t)wakeup_recover)>>1);
        }
        else
        {
            LPM_CONTROL->lpm_pc.bit.lpm_pc=(((uint32_t)flash_start)>>1);
        }
    }
    else
    {
        LPM_HW_CONTROL_NOEXP->da_lpm_dvdd_retmem_en.bit.da_lpm_ldo_dig_retmem_en=0;
    }

    prefetch((void*)sleep_entry,(void*)(sleep_entry+0x200),DISABLE);
    sleep_entry(wakeupMode,retentionCfg,LpmRecoverStruct->rg_lpm_ldo_dig_retmem_vsel.bit.sleep,save_stack);
}

/**
 * @method LPM_Sleep
 * @brief  chip into sleep mode
 * @param  sleepTimeMs:sleep time(0 is Do not enable timed wake-up)
 * @param  sleepLevel:SLEEP_MODE_LPM、MODE_SLEEP_LIGHT or SLEEP_MODE_DEEP
 * @param  retentionCfg:retention config:1 is retention,0 is no retention
 *                                      bit0:ram0[0x10000-0x11fff],8k
 *                                      bit1:ram1[0x12000-0x13fff],8k
 *                                      bit2:ram2[0x14000-0x17fff],16k
 *                                      bit3:ram3[0x18000-0x1ffff],32k
 *                                      bit4:ram4[0x20000-0x23fff],16k
 * @param  wakeMode:WAKEUP_FROM_MAIN or WAKEUP_FROM_MAIN
 * @param  lpmPreCallback:the callback function for prepare sleep
 * @param  lpmPostCallback:the callback function for post sleep
 * @retval true:lpm ok; false:lpm fail
 */
bool LPM_Sleep(uint32_t sleepTimeMs, LPM_ModeEnum sleepLevel,uint8_t retentionCfg,LPM_WakeupModeEnum wakeMode,lpmPreActionCb lpmPreCallback,lpmPostActionCb lpmPostCallback)
{
    LPM_Init();
    RTC_Cmd(ENABLE);

    if(false == LPM_BtprepareActionBeforeSleep(sleepLevel))
    {
        return false;
    }
    lpmPreCallback(sleepLevel);

    if(sleepTimeMs>0)
    {
        RTC_SetAlarm1(RTC_GetCount(OV_INTR)+(sleepTimeMs*(RTC_GetClk()/1000)));
        RTC_WKConfig(ALARM1,ENABLE);
    }

    LpmRecoverStruct->rg_lpm_ldo_dig_lpm_vosel.bit.sleep_mode=sleepLevel;
    LpmRecoverStruct->rg_lpm_ldo_dig_retmem_vsel.bit.wake_mode=wakeMode;
    LpmRecoverStruct->qspi_param.retention_cfg=(retentionCfg&0x1f);

    LpmRecoverStruct->crc16 = ((uint16_t(*)(uint16_t base_crc, uint8_t* data, unsigned int len))ROM_API_CRC16)(0xffff,(uint8_t*)&LpmRecoverStruct->qspi_param,LPM_RECOVER_STRUCT_LEN-2);

    LPM_SleepEntry(sleepLevel,wakeMode,LpmRecoverStruct->qspi_param.retention_cfg,1);
    lpmPostCallback(sleepLevel);
    return true;
}

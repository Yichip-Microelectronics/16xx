/*
File Name    : yc_sysctrl.c
Author       : Yichip
Version      : V1.0
Date         : 2023/11/01
Description  : sysctrl driver interface implement.
*/

#include "yc_sysctrl.h"
#include "board_config.h"
#include "yc_lpm.h"
#include "yc_timer.h"

/**
 * @method SYSCTRL_WaitLpoEdge
 * @brief  wait lpo clock edge
 * @param  edgeNum:wait lpo clock edge nummber
 * @retval none
 */
void SYSCTRL_WaitLpoEdge(int edgeNum)
{
    for (int i = 0; i < edgeNum; i++)
    {
        while (CORE_CHIP_SYS->sys_status.bit.lpo_clock);
        while (!(CORE_CHIP_SYS->sys_status.bit.lpo_clock));
    }
}

/**
 * @method delay_us
 * @brief  delay us
 * @param  us:delay us time
 * @retval none
 */
void delay_us(uint32_t us)
{
    #if DELAY_USER_ROM_API
    ((void(*)(uint32_t us))ROM_API_DELAY_US)(us);
    #else
    int delay_us_num=((CORE_CHIP_SYS->sysclk_reg.reg>>20)&0xfff);//cpu clock of us
    delay_us_num=delay_us_num*us;
    delay_us_num-=15; //sub others cycle
    delay_us_num =(delay_us_num>>2);//delay_us_num=delay_us_num/4
    if(delay_us_num > 0)
    {
        delay(delay_us_num);
    }
    else//处理低频(cpu_clk<1M)
    {
        delay_us_num=(CORE_CHIP_SYS->sysclk_reg.reg&0xfffff);//cpu clock of ms
        delay_us_num=delay_us_num*us;
        delay_us_num=delay_us_num/1000;
        delay_us_num =(delay_us_num>>2);//delay_us_num=delay_us_num/4
        if(delay_us_num > 0)
        {
            delay(delay_us_num);
        }
    }
    #endif
}

/**
 * @method delay_ms
 * @brief  delay ms
 * @param  ms:delay ms time
 * @retval none
 */
void delay_ms(uint32_t ms)
{
    #if DELAY_USER_ROM_API
    ((void(*)(int ms))ROM_API_DELAY_MS)(ms);
    #else
    int delay_ms_num=(CORE_CHIP_SYS->sysclk_reg.reg&0xfffff);//cpu clock of ms
    delay_ms_num =(delay_ms_num>>2);//delay_ms_num=delay_ms_num/4
    delay_ms_num=delay_ms_num*ms;
    if(delay_ms_num>0)
    {
        delay(delay_ms_num);
    }
    #endif
}

/**
 * @method SYSCTRL_crystalCtrl
 * @brief  open or close crystal
 * @param  state:ENABLE is open,other is close
 * @retval none
 */
_ramcode void SYSCTRL_CrystalCtrl(FunctionalState state)
{
    #if(NO_CRYSTAL ==1)
    RF_LDO_RF->ldo_ctrl0.bit.da_ldo_abb_en =1;
    RF_RCO384M->rco384m_ctrl.bit.da_rco384m_en=1;
    #else
    RF_RCO384M->rco384m_ctrl.bit.da_rco384m_en=0;
    RF_LDO_RF->ldo_ctrl0.bit.da_ldo_abb_en = 0;
    if (state == ENABLE)
    {
        if (!LPM_CONTROL->ice_mode.bit.xtal_en)
        {
            LPM_HW_CONTROL_NOEXP->da_lpm_xtal_cmom_sel.bit.da_lpm_xtal_cmom_sel=0xe;
            LPM_HW_CONTROL_NOEXP->da_lpm_xtal_cmom_sel.bit.da_lpm_xtal_cmos_sel=0x3;
            // Enable xtal.
            LPM_CONTROL->ice_mode.bit.xtal_en = 1;
            // Wait xtal stable.
            while (!(CORE_CHIP_SYS->sys_status.bit.crystal_stable));
            // Force wait a while.
            delay_ms(1);
        }
    }
    else
    {
        LPM_CONTROL->ice_mode.bit.xtal_en = 0;
    }
    #endif
}

/**
 * @method SYSCTRL_dpllInit
 * @brief  init dpll
 * @param  none
 * @retval none
 */
_ramcode void SYSCTRL_DpllInit(void)
{
    #if (BOARD_TYPE !=BOARD_2020_022_1508FPGA_V_1_2)
    if (RF_BBPLL->bbpll_ctrl0.bit.da_ldo_bbpll_en == 0)
    {
        #if NO_CRYSTAL
        CORE_GPIO->gpio_cfg[NO_CRYSTAL_GPIO] = GPIO_MODE_PWM_OUT;
        PWM_PCNT((NO_CRYSTAL_GPIO%8)) = 1;
        PWM_NCNT((NO_CRYSTAL_GPIO%8)) = 1;
        PWM_CTRL((NO_CRYSTAL_GPIO%8))->pwm0_ctrl.reg = 0xa0;
        PWM_CLKEN |= (1 << (NO_CRYSTAL_GPIO%8));
        #endif
        RF_BBPLL->bbpll_ctrl0.reg |= 0x87;
        RF_BBPLL->bbpll_ctrl1.reg =0x50;
        ((void(*)(uint32_t cnt))ROM_API_DELAY)(10);
        RF_BBPLL->bbpll_ctrl1.reg = 0xf0|
                                    1<<3|  //384_en
                                    1<<2;  //256_en

        #if (EXTERNAL_CRYSTAL == 16000000)
        RF_BBPLL->bbpll_div_int.reg=0x18;
        #elif (EXTERNAL_CRYSTAL == 32000000)
        RF_BBPLL->bbpll_div_int.reg=0x0c;
        #endif

        RF_BBPLL->bbpll_sdm.bit.da_bbpll_sdm_rstn = 1;

        RF_BBPLL->bbpll_cfg5.bit.rg_bbpll_logen_cksel = NO_CRYSTAL;

        CORE_CHIP_SYS->anaclk_en.bit.clk_dpll_384_en = 1;
        CORE_CHIP_SYS->anaclk_en.bit.clk_dpll_256_en = 1;
        #if NO_CRYSTAL
        delay_us(50);
        CORE_CHIP_SYS->sys_clksel.bit.uart_clksel=1;
        PWM_CTRL((NO_CRYSTAL_GPIO%8))->pwm0_ctrl.reg = 0;
        PWM_CLKEN &= (~(1 << (NO_CRYSTAL_GPIO%8)));
        #endif
    }
    #endif
}


/**
 * @brief Get mcu clock
 *
 * @param None
 *
 * @return mcu clock(Hz)
 */
_ramcode uint32_t SYSCTRL_GetMcuClk(void)
{
    uint8_t sys_clksel = CORE_CHIP_SYS->sys_clksel.bit.sys_clksel;
    uint8_t mcu_clksel = CORE_CHIP_SYS->mcu_clk_sel.bit.mcu_clk_sel;
    uint32_t clock_freq = 48000000;
    switch (sys_clksel)
    {
    case SYS_CLKSEL_RC:
        clock_freq = RC_CLK_SOURCE;
        break;
    case SYS_CLKSEL_DPLL:
        switch (mcu_clksel)
        {
        case DPLL_CLKSEL_24MHZ:
            clock_freq = 24000000;
            break;
        case DPLL_CLKSEL_64MHZ:
            clock_freq = 64000000;
            break;
        case DPLL_CLKSEL_96MHZ:
            clock_freq = 96000000;
            break;
        case DPLL_CLKSEL_128MHZ:
            clock_freq = 128000000;
            break;
        case DPLL_CLKSEL_192MHZ:
            clock_freq = 192000000;
            break;
        case DPLL_CLKSEL_256MHZ:
            clock_freq = 256000000;
            break;
        default:
            clock_freq = 48000000;
            break;
        }
        break;
    case SYS_CLKSEL_CRYSTAL:
        clock_freq = EXTERNAL_CRYSTAL;
        break;
    default:
        clock_freq = 32768;
        break;
    }

    return clock_freq;
}

/**
 * @brief Get system clock
 *
 * @param None
 *
 * @return system clock(Hz)
 */
_ramcode uint32_t SYSCTRL_GetSystemClk(void)
{
    uint8_t sys_clkdiv = CORE_CHIP_SYS->sys_clksel.bit.sys_clkdiv;
    uint32_t mcu_clk = 0;
    uint32_t sys_clk = 0;
    mcu_clk = SYSCTRL_GetMcuClk();
    sys_clk = mcu_clk / (sys_clkdiv + 1);

    return sys_clk;
}

/**
 * @method SYSCTRL_ClkEn
 * @brief  module clock ctrl
 * @param  type:CLK_EnEnum
 * @param  newState:ENABLE or DISABLE
 * @retval none
 */
void SYSCTRL_ClkEn(CLK_EnEnum type,FunctionalState newState)
{
    if(newState == ENABLE)
    {
        CORE_CHIP_SYS->clken.reg |= 1 << type;
    }
    else
    {
        CORE_CHIP_SYS->clken.reg &= (~(1 << type));
    }
}
/**
 * @brief clock init
 * @param sysClkSource : SYS_CLKSEL_RC,SYS_CLKSEL_DPLL,SYS_CLKSEL_CRYSTAL or SYS_CLKSEL_LPO
 * @param sysClkDiv : system clock div(sysclk=mcuclk/(sysClkDiv+1),max sysclk is 96M)
 * @param mcuDpllClkSel : It is only valid when sysClkSource == SYS_CLKSEL_DPLL
 * @param flashClkSel : It is only valid when sysClkSource == SYS_CLKSEL_DPLL
 */
_ramcode void SYSCTRL_ClkInit(SYS_ClkEnum sysClkSource, uint8_t sysClkDiv, DPLL_ClkEnum mcuDpllClkSel, DPLL_ClkEnum flashClkSel)
{

    CORE_CHIP_SYS->sys_clksel.bit.sys_clksel = sysClkSource;
    CORE_CHIP_SYS->mcu_clk_sel.bit.mcu_clk_sel = mcuDpllClkSel;
    CORE_CHIP_SYS->qspi_clk_sel.bit.qspi_clk_sel = flashClkSel;
    CORE_CHIP_SYS->sys_clksel.bit.sys_clkdiv = sysClkDiv;

    uint32_t mcu_clk=SYSCTRL_GetMcuClk();
    CORE_CHIP_SYS->sysclk_reg.reg=(((mcu_clk/1000000)<<20)|((mcu_clk/1000)&0xfffff));
    _ASSERT((mcu_clk / (sysClkDiv + 1)) <= 96000000);
}



/**
 * @method SYSCTRL_ChipInit
 * @brief  chip init
 * @param  none
 * @retval none
 */
_ramcode void SYSCTRL_ChipInit(void)
{
    LPM_CONTROL->ice_mode.bit.wd_en = 0;//disable watch dog

    // xtal enable
    SYSCTRL_CrystalCtrl(ENABLE);
    // Dpll setup.
    SYSCTRL_DpllInit();
    SYSCTRL_ClkInit(SYS_CLK_SOURCE, SYS_CLK_DIV, MCU_DPLL_CLK_SEL, FLASH_CLK_SEL);
}


/**
 * @method SYSCTRL_SystemInit
 * @brief  ststem init(call in startup file)
 * @param  none
 * @retval none
 */
_ramcode void SYSCTRL_SystemInit(void)
{
    #if (!YC_LOG_SWITCH)
    CORE_CHIP_SYS->io_rst_src_en.bit.code_crc_fail_rst_en=1;
    CORE_CHIP_SYS->io_rst_src_en.bit.cpu_access_err_fail_rst_en=1;
    #endif
    //开启vdt
    LPM_HW_CONTROL_NOEXP->da_lpm_vdt_dcdc_en.bit.da_lpm_vdt_vdcdc_en=1;
    LPM_HW_CONTROL_NOEXP->da_lpm_vdt_dcdc_en.bit.da_lpm_vdt_vddio_en=1;

    LPM_HW_CONTROL_NOEXP->da_lpm_ldo_dig_core_vsel.bit.da_lpm_ldo_dig_core_ildsel=0;

    #if (PWR_USE_BUCK == 1)
    LPM_HW_CONTROL_NOEXP->da_lpm_ldo_0p33vdd_aon_bypb.bit.da_lpm_pwr_dcdc_mode=1;
    LPM_HW_CONTROL_NOEXP->rg_lpm_buck_dt_ctrl.bit.rg_lpm_buck_fb_sel=0Xf;
    #else
    LPM_HW_CONTROL_NOEXP->da_lpm_ldo_0p33vdd_aon_bypb.bit.da_lpm_pwr_dcdc_mode=0;
    #endif
    if(LPM_HW_CONTROL_NOEXP->da_lpm_bg_core_tc.bit.da_lpm_bg_core_tc==0x10)
    {
        LPM_HW_CONTROL_NOEXP->da_lpm_bg_core_tc.bit.da_lpm_bg_core_tc=0xc;
    }
    LPM_HW_CONTROL_NOEXP->da_lpm_bg_core_tc.bit.da_lpm_ldo_0p33vdd_aon_autoset=1;
}

/**
 * @method SYSCTRL_softReset
 * @brief  reset chip
 * @param  none
 * @retval none
 */
void SYSCTRL_SoftReset(void)
{
    CORE_CHIP_SYS->sys_reset.bit.sys_reset = 1;
    while (1);
}

/**
 * @method SYSCTRL_GetSysEvent
 * @brief  get system event
 * @param  none
 * @retval system event(define SYS_EVENT_XX)
 */
uint8_t SYSCTRL_GetSysEvent(void)
{
    return CORE_CHIP_SYS->sys_event_lock.reg;
}

/**
 * @method SYSCTRL_ClearSysEvent
 * @brief  clear system event
 * @param  event:event flag(define SYS_EVENT_XX)
 * @retval none
 */
void SYSCTRL_ClearSysEvent(uint8_t event)
{
    CORE_CHIP_SYS->sys_event_lock.reg = event;
}

/**
 * @method SYSCTRL_IceCmd
 * @brief  enable or disable ice debug port
 * @param  newState:ENABLE or DISABLE
 * @retval none
 */
void SYSCTRL_IceCmd(FunctionalState newState)
{
    if(newState == ENABLE)
    {
        LPM_CONTROL->ice_mode.bit.ice_mode=1;
    }
    else
    {
        LPM_CONTROL->ice_mode.bit.ice_mode=0;
    }
}


extern char _end;//heap start
extern char _heap_end;//heap end
#ifndef __caddr_t_defined
    #define __caddr_t_defined
    typedef    char     *caddr_t;
#endif
static LPM_RETENTION char *curbrk = &_end;

caddr_t _sbrk(ptrdiff_t incr)
{
    if ((curbrk + incr < (&_end)) || ((curbrk + incr) > (&_heap_end)))
    {
        //     yc_log("_end=%x _heap_end=%x heap_size=%x curbrk=%x incr=%x\r\n",&_end,&_heap_end,&_heap_end-&_end,curbrk,incr);
        return (caddr_t) -1;
    }
    curbrk += incr;
    return (caddr_t)(curbrk - incr);
}

/* Called from wakeup assembly before bt_lpm_recover.
 * Reset heap state: orphan free list (nodes in non-retention RAM are lost)
 * and rewind heap pointer to prevent memory leak across sleep cycles. */
void heap_reset_for_wakeup(void)
{
    extern void *__malloc_free_list;
    __malloc_free_list = NULL;
    curbrk = (char *)&_end;
}

/************************ (C) COPYRIGHT Yichip Microelectronics *****END OF FILE****/


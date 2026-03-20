/*
File Name    : yc_sadc.c
Author       : Yichip
Version      : V1.0
Date         : 2024/05/30
Description  : sadc driver interface implement.
*/

#include "yc_sadc.h"
#include "yc_otp.h"

uint8_t LPM_RETENTION otp_adc_cali_para[16] = {0};

static void SADC_AnaConfig(void)
{
    RF_GPADC->gpadc_ctrl.reg=0;
    RF_GPADC->gpadc_ctrl.bit.da_gpadc_sar_ldo_trim=5;
    RF_GPADC->gpadc_ctrl.bit.da_gpadc_sar_ldo_en=1;
    RF_GPADC->gpadc_ctrl.bit.da_gpadc_sar_vrefbuf_en=1;
    RF_GPADC->gpadc_ctrl.bit.da_gpadc_sar_en=1;
    RF_GPADC->gpadc_ctrl.bit.da_gpadc_sar_digpwr_sel=1;
    RF_GPADC->gpadc_cfg0.reg=0;
    RF_GPADC->gpadc_cfg0.bit.rg_gpadc_pga0_byp=1;
    RF_GPADC->gpadc_cfg0.bit.rg_gpadc_pga0_en=1;
    RF_GPADC->gpadc_cfg0.bit.rg_gpadc_pga0_mux_cal=0;
    RF_GPADC->gpadc_cfg0.bit.rg_gpadc_sar_calib_en=0;
    RF_GPADC->gpadc_cfg0.bit.rg_gpadc_sar_cmp_isel=3;
    RF_GPADC->gpadc_cfg0.bit.rg_gpadc_sar_cmp_auto=1;
    RF_GPADC->gpadc_cfg2.reg=0;
    RF_GPADC->gpadc_cfg2.bit.rg_gpadc_sar_inbuf_isel=3;
    RF_GPADC->gpadc_cfg2.bit.rg_gpadc_sar_vcm_isel=1;
    RF_GPADC->gpadc_cfg3.bit.rg_gpadc_sar_vref_isel=1;
    RF_GPADC->gpadc_cfg3.bit.rg_gpadc_sar_vref_sel=3;//0:internal reference 1:DVDD 2:PAD 3:LDO AVDD
    RF_GPADC->gpadc_cfg1.bit.rg_gpadc_sar_conv_mode=0;
    RF_GPADC->gpadc_cfg4.bit.rg_gpadc_invref_mode=0;// 0:3V  1:1.8V
    RF_GPADC->gpadc_cfg4.bit.rg_gpadc_dcpath_div2=0;//input div2
}
static void SADC_Calibration(int8_t *buf)
{
    SADC_AnaConfig();
    RF_GPADC->gpadc_cfg3.bit.rg_gpadc_sar_vref_sel=0;
    RF_GPADC->gpadc_cfg0.bit.rg_gpadc_sar_calib_en=1;
    DADC_SAR_ADC->sadc0_clk_div.reg=0xff;
    DADC_SAR_ADC->sadc0_en.bit.sadc0_ana_clk_inv=0x1;
    DADC_SAR_ADC->sadc0_en.bit.sadc0_ana_clk_en=0x1;
    DADC_SAR_ADC->sadc0_en.bit.sadc0_clk_en=0x1;
    DADC_SAR_ADC->sadc0_en.bit.sadc0_en=0x1;
    DADC_SAR_ADC->sadc0_ctrl.bit.etsrc=0xf;
    DADC_SAR_ADC->sadc0_ctrl.bit.ctn=DISABLE;
    SADC_SetRegularChannel(1<<SADC_CHANNEL_GPIO24);
    SADC_RegularTrigger();
    delay_ms(1);
    DADC_SAR_ADC->sadc0_clk_div.reg=0;
    DADC_SAR_ADC->sadc0_en.reg=0;
    delay_us(50);
    RF_GPADC->gpadc_ctrl.reg=0;
    RF_GPADC->gpadc_cfg0.reg=0;
    for (uint8_t n = 0; n < 7; n++)
    {
        RF_GPADC->gpadc_cfg2.bit.rg_gpadc_sar_dat_wr=0;
        RF_GPADC->gpadc_cfg2.bit.rg_gpadc_sar_dat_sel = n;
        buf[n] = RF_GPADC->gpadc_status.reg;
    }
}
static void SADC_AnaInit(void)
{
    int8_t read_buf[7] = {0};
    int16_t data_sum[7] = {0};
    for (int a = 0; a < 64; a++)
    {
        SADC_Calibration(read_buf);
        for (int i = 0; i < 7; i++)
        {
            data_sum[i] = data_sum[i] + (int8_t)(read_buf[i] << 1);
        }
    }
    for (int i = 0; i < 7; i++)
    {
        data_sum[i] = ((uint8_t)((((int16_t)((data_sum[i]))) > 0 ? \
                      (int8_t)((data_sum[i]) / 64.0f + 0.5f) : \
                      (int8_t)((data_sum[i]) / 64.0f - 0.5f)))) >> 1;
    }
    SADC_AnaConfig();
    for (int n = 0; n < 7; n++)
    {
        RF_GPADC->gpadc_cfg2.bit.rg_gpadc_sar_dat_sel  = n;
        RF_GPADC->gpadc_cfg2.bit.rg_gpadc_sar_dat_wr = 1;
        RF_GPADC->gpadc_cfg1.bit.rg_gpadc_sar_datin = data_sum[n];
    }
    SADC_AnaConfig();
}

/**
 * @name   SADC_DeInit
 * @brief  sar adc deinit.
 * @param  none
 * @retval none
 */
void SADC_DeInit(void)
{
    DADC_SAR_ADC->sadc0_en.reg=DISABLE;
    DADC_SAR_ADC->sadc0_ctrl.reg=DISABLE;
}

/**
 * @name   SADC_Init
 * @brief  sar adc init.
 * @param  initStruct:point to SADC_InitStruct
 * @retval None
 */
void SADC_Init(SADC_InitStruct *initStruct)
{
    SADC_AnaInit();
    RF_GPADC->gpadc_ctrl.bit.da_gpadc_sar_ldo_trim=5;
#if (SADC_GPIO_RANGE==SADC_GPIO_RANGE_3V3)
    DADC_SAR_ADC->sadc0_clk_div.reg=SADC_CLK_24M;
#else
    DADC_SAR_ADC->sadc0_clk_div.reg=SADC_CLK_1M;
#endif
    DADC_SAR_ADC->sadc0_ctrl.bit.ctn = initStruct->continuous_mode;
    DADC_SAR_ADC->sadc0_ctrl.bit.ica = initStruct->auto_inject;
    DADC_SAR_ADC->sadc0_ctrl.bit.etsrc = initStruct->etsrc;
    DADC_SAR_ADC->sadc0_ctrl.bit.etsic = initStruct->etsic;
    DADC_SAR_ADC->sadc0_en.bit.sadc0_ana_clk_inv=0x1;
    DADC_SAR_ADC->sadc0_en.bit.sadc0_ana_clk_en=0x1;
    DADC_SAR_ADC->sadc0_en.bit.sadc0_clk_en=0x1;
    DADC_SAR_ADC->sadc0_en.bit.sadc0_en=0x1;
    if((LpmRecoverStruct->wakeup_flag&WAKEUP_FLAG_WAKE)==0)
    {
        OTP_Read(ADC_REF_OTP_ADDR,otp_adc_cali_para,sizeof(otp_adc_cali_para));
    }
    SADC_ClearIrq(SADC_ALL_EVENT);
}
/**
 * @name   SADC_InputDiv2
 * @brief  adc input voltage div 2.
 * @param  state: ENABLE:Enable input voltage div2
 *                DISABLE:Disable input voltage div2
 * @retval None
 */
void SADC_InputDiv2(FunctionalState state)
{
    if(state)
    {
        RF_GPADC->gpadc_cfg0.bit.rg_gpadc_pga0_byp=0;
        RF_GPADC->gpadc_cfg4.bit.rg_gpadc_dcpath_div2=1;
    }
    else
    {
        RF_GPADC->gpadc_cfg0.bit.rg_gpadc_pga0_byp=1;
        RF_GPADC->gpadc_cfg4.bit.rg_gpadc_dcpath_div2=0;
    }
}
/**
 * @name   SADC_SetRegularChannel
 * @brief  adc regaluar channel config.
 * @param  rlist: regular adc channel.
 * @retval None
 */
void SADC_SetRegularChannel(uint16_t rlist)
{
    if (rlist==1<<SADC_CHANNEL_VBAT)   //only vbat
    {
        RF_GPADC->gpadc_cfg3.bit.rg_gpadc_sar_vref_sel=0;
        RF_GPADC->gpadc_cfg4.bit.rg_gpadc_invref_mode=1;
        DADC_SAR_ADC->sadc0_clk_div.reg=SADC_CLK_1M;
    }
    else
    {
        #if SADC_GPIO_RANGE==SADC_GPIO_RANGE_3V3
        RF_GPADC->gpadc_cfg3.bit.rg_gpadc_sar_vref_sel=3;
        RF_GPADC->gpadc_cfg4.bit.rg_gpadc_invref_mode=0;
        DADC_SAR_ADC->sadc0_clk_div.reg=SADC_CLK_24M;
        #else
        RF_GPADC->gpadc_cfg3.bit.rg_gpadc_sar_vref_sel=0;
        RF_GPADC->gpadc_cfg4.bit.rg_gpadc_invref_mode=1;
        DADC_SAR_ADC->sadc0_clk_div.reg=SADC_CLK_1M;
        #endif
    }
    DADC_SAR_ADC->sadc0_rlist.reg = rlist;
}

/**
 * @name   SADC_SetInjectedChannel
 * @brief  adc injected channel config.
 * @param  ilist: inject adc channel.
 * @param  num: inject channel enable number.
 * @retval None
 */
void SADC_SetInjectedChannel(uint16_t ilist, uint8_t num)
{
    DADC_SAR_ADC->sadc0_trig_ch.reg = ilist;
    DADC_SAR_ADC->sadc0_ctrl.bit.trig_len = num;
}

/**
 * @name   SADC_RegularTrigsel
 * @brief  adc regaluar trigger source select.
 * @param  ets: regular trigger source.
 * @retval None
 */
void SADC_RegularTrigsel(SADC_EtsEnum ets)
{
    _ASSERT(IS_SADC_ETS(ets));
    DADC_SAR_ADC->sadc0_ctrl.bit.etsrc = ets;
}

/**
 * @name   SADC_InjectTrigsel
 * @brief  adc inject trigger source select.
 * @param  ets: inject trigger source.
 * @retval None
 */
void SADC_InjectTrigsel(SADC_EtsEnum ets)
{
    _ASSERT(IS_SADC_ETS(ets));
    DADC_SAR_ADC->sadc0_ctrl.bit.etsic = ets;
}

/**
 * @name   SADC_RegularTrigger
 * @brief  start regular sample.
 * @param  none
 * @retval none
 */
void SADC_RegularTrigger(void)
{
    DADC_SAR_ADC->sadc0_rcr_start.reg = ENABLE;
}

/**
 * @name   SADC_InjectTrigger
 * @brief  start inject sample.
 * @param  none
 * @retval none
 */
void SADC_InjectTrigger(void)
{
    DADC_SAR_ADC->sadc0_icr_start.reg = ENABLE;
}

/**
 * @name   SADC_SetIrq
 * @brief  Enable adc irq.
 * @param  irqn: SADC_IrqEnum
 * @param  state: ENABLE and DISABLE.
 * @retval None
 */
void SADC_SetIrq(SADC_IrqEnum irqn,FunctionalState state)
{
    _ASSERT(IS_SADC_IRQ(irqn));
    SADC_ClearIrq(SADC_ALL_EVENT);
    if(state)
    {
        DADC_SAR_ADC->sadc0_int_en.reg |= irqn;
    }
    else
    {
        DADC_SAR_ADC->sadc0_int_en.reg &= ~irqn;
    }
}
/**
 * @name   SADC_GetIrqStatus
 * @brief  ADC get SADC interrupt status.
 * @param  None:
 * @retval interrupt status,bit define reference SADC_IrqEnum
 */
uint8_t SADC_GetIrqStatus(void)
{
    return DADC_SAR_ADC->sadc0_int_status.reg;
}
/**
 * @name   SADC_ClearIrq
 * @brief  clear adc irq.
 * @param  irqn: SADC_IrqEnum
 * @retval None
 */
void SADC_ClearIrq(SADC_IrqEnum irqn)
{
    DADC_SAR_ADC->sadc0_int_clr.reg |= irqn;
}

/**
 * @name   SADC_SetRegualrSampleMode
 * @brief  ADC Regualr sampling mode configuration
 * @param  state:   ENABLE:Continuous sampling mode,
 *                  DISABLE:Single sampling mode.
 * @retval None
 */
void SADC_SetRegualrSampleMode(FunctionalState state)
{
    _ASSERT(IS_FUNCTIONAL_STATE(state));
    DADC_SAR_ADC->sadc0_ctrl.bit.ctn = state;
}

/**
 * @name   SADC_SetInjectSampleMode
 * @brief  ADC inject sampling mode configuration
 * @param  state:   ENABLE:Auto mode,
 *                  DISABLE:Insert mode.
 * @retval None
 */
void SADC_SetInjectSampleMode(FunctionalState state)
{
    _ASSERT(IS_FUNCTIONAL_STATE(state));
    DADC_SAR_ADC->sadc0_ctrl.bit.ica = state;
}
/**
 * @name   SADC_GetRegularConversionValue
 * @brief  ADC get regular conversion value
 * @param  none
 * @retval adc regular conversion value
 */
uint16_t SADC_GetRegularConversionValue(void)
{
    while (!(SADC_GetStatus()&SADC_EORC));
    SADC_ClearIrq(SADC_EORC);
    return DADC_SAR_ADC->sadc0_rdat.reg;
}
/**
 * @name   SADC_GetInjectedConversionValue
 * @brief  ADC get inject conversion value
 * @param  none
 * @retval adc inject conversion value
 */
uint16_t SADC_GetInjectedConversionValue(void)
{
    while (!(SADC_GetStatus()&SADC_EOIC));
    SADC_ClearIrq(SADC_EOIC);
    return DADC_SAR_ADC->sadc0_idat.reg;
}
/**
 * @name   SADC_DmaCmd
 * @brief  ADC dma enable
 * @param  state:adc dma enable
 * @retval None :
 */
void SADC_DmaCmd(FunctionalState state)
{
    if (state)
    {
        DADC_SAR_ADC->sadc0_ctrl.bit.dma_en = ENABLE;
    }
    else
    {
        DADC_SAR_ADC->sadc0_ctrl.bit.dma_en = DISABLE;
    }
}

/**
 * @name   SADC_DmaConfig
 * @brief  adc dma buffer address config
 * @param  rsaddr : regular dma buffer start address
 *         readdr : regular dma buffer end address
 *         ieaddr : injected dma buffer end address
 * @retval None :
 */
void SADC_DmaConfig(uint16_t *rsaddr, uint16_t *readdr, uint16_t *ieaddr)
{
    DADC_SAR_ADC->sadc0_ctrl.bit.dma_en = DISABLE;
    DADC_SAR_ADC->sadc0_ieaddr.bit.sadc0_ieaddr = (uint32_t)ieaddr;
    DADC_SAR_ADC->sadc0_readdr.bit.sadc0_readdr = (uint32_t)readdr;
    DADC_SAR_ADC->sadc0_rsaddr.bit.sadc0_rsaddr = (uint32_t)rsaddr;
    DADC_SAR_ADC->sadc0_ctrl.bit.dma_en = ENABLE;

}

/**
 * @name   SADC_GetStatus
 * @brief  get adc module status
 * @param  none
 * @retval adc module status,bit define reference SADC_IrqEnum
 */
uint8_t SADC_GetStatus(void)
{
    return DADC_SAR_ADC->sadc0_status.reg;
}

/**
 * @name   SADC_WdtConfig
 * @brief  adc wdt level config
 * @param  wdt       : wdt config struct
 *         lowLimit  : wdt low limit value
 *         highLimit : wdt high limit value
 * @retval None
 */
void SADC_WdtConfig(SADC_WdtStruct *wdt,uint16_t lowLimit,uint16_t highLimit)
{
    DADC_SAR_ADC->sadc0_wdht.bit.sadc_wdht = highLimit;
    DADC_SAR_ADC->sadc0_wdlt.bit.sadc_wdlt = lowLimit;
    DADC_SAR_ADC->sadc0_wd.bit.adc_wd_ch = wdt->wdt_ch;
    DADC_SAR_ADC->sadc0_wd.bit.adc_wd_sc = wdt->wdt_sc;
    DADC_SAR_ADC->sadc0_wd.bit.adc_wd_ien = wdt->wdt_ien;
    DADC_SAR_ADC->sadc0_wd.bit.adc_wd_ren = wdt->wdt_ren;
}

/**
 * @brief  calculate voltage ,acording reference value and reference voltage
 * @param  result       : conversion value
 *         vol_high_ref : high voltage reference value
 *         vol_low_ref  : low voltage reference value
 *         vol_high     : high reference voltage(mV)
 *         vol_low      : low reference voltage(mV)
 * @retval voltage (mV)
 */
static int calculate_voltage(int result, int vol_high_ref, int vol_low_ref, int vol_high, int vol_low)
{
    int rega = result - vol_low_ref;
    int regb = vol_high_ref - vol_low_ref;
    int regc = vol_high - vol_low;
    int temp = (rega * regc) / regb + vol_low;
    return temp > 0 ? temp : (0 - temp);

}

/**
 * @name   SADC_CalculateVoltage
 * @brief  calculate Voltage(mV) by adc conversion value
 * @param  ch        : adc channel
 *         adcValue  : adc conversion value
 * @retval voltage (mV)
 */
uint16_t SADC_CalculateVoltage(SADC_ChxEnum ch,uint16_t adcValue)
{
    int result = 0;
    int cali_voltage_low = 0;   /*低校准电压值*/
    int cali_voltage_high = 0;  /*高校准电压值*/
    int cali_value_low = 0;     /*低校准电压对应的校准值*/
    int cali_value_high = 0;    /*高校准电压对应的校准值*/
    if(ch == SADC_CHANNEL_VBAT)
    {
        cali_voltage_low = 2800;
        cali_voltage_high = 4800;
        cali_value_low = otp_adc_cali_para[0] | (otp_adc_cali_para[1] << 8);
        cali_value_high = otp_adc_cali_para[2] | (otp_adc_cali_para[3] << 8);
    }
    else
    {
        #if (SADC_GPIO_RANGE==SADC_GPIO_RANGE_3V3)
        cali_voltage_low = 600;
        cali_voltage_high = 3000;
        cali_value_low = otp_adc_cali_para[12] | (otp_adc_cali_para[13] << 8);
        cali_value_high = otp_adc_cali_para[14] | (otp_adc_cali_para[15] << 8);
        #else
        if(RF_GPADC->gpadc_cfg3.bit.rg_gpadc_sar_vref_sel == 3)
        {//ft用,与上面SADC_GPIO_RANGE==SADC_GPIO_RANGE_3V3下的代码相同
            cali_voltage_low = 600;
            cali_voltage_high = 3000;
            cali_value_low = otp_adc_cali_para[12] | (otp_adc_cali_para[13] << 8);
            cali_value_high = otp_adc_cali_para[14] | (otp_adc_cali_para[15] << 8);
        }
        else
        {
            if(RF_GPADC->gpadc_cfg4.bit.rg_gpadc_dcpath_div2 == 1)
            {
                cali_voltage_low = 600;
                cali_voltage_high = 3000;
                cali_value_low = otp_adc_cali_para[8] | (otp_adc_cali_para[9] << 8);
                cali_value_high = otp_adc_cali_para[10] | (otp_adc_cali_para[11] << 8);
            }
            else
            {
                cali_voltage_low = 600;
                cali_voltage_high = 1500;
                cali_value_low = otp_adc_cali_para[4] | (otp_adc_cali_para[5] << 8);
                cali_value_high = otp_adc_cali_para[6] | (otp_adc_cali_para[7] << 8);
            }
        }
        #endif
    }
    if (cali_value_low == 0 || cali_value_high == 0)
    {
        return -1;
    }
    result = calculate_voltage(adcValue, cali_value_high, cali_value_low, cali_voltage_high, cali_voltage_low);

    return result;
}
/************************ (C) COPYRIGHT Yichip Microelectronics *****END OF FILE****/

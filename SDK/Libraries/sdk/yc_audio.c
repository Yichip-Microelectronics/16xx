/*
File Name    : yc_audio.c
Author       : Yichip
Version      : V1.0
Date         : 2024/05/27
Description  : yc audio driver interface implement.
*/

#include "yc_audio.h"
#include "board_config.h"

/*sample_rate=384000000/(128*A_clkdiv*B_clkdiv_sel*(C_int_sel+D_frac_sel/(2^24)))*/
#define CASE_ADC_CLK(sample_rate, A_clkdiv, B_clkdiv_sel, C_int_sel, D_frac_sel) \
    case sample_rate: {\
        CORE_CHIP_SYS->audio_clkctrl.bit.mic_clk_div= A_clkdiv;\
        CORE_AUDIO_ADC0->adcd_mic_ctrl.bit.micclk_sel= B_clkdiv_sel;\
        CORE_AUDIO_ADC0->adcd_dsmdiv_int.bit.adcd_dsmdiv_int = C_int_sel;\
        CORE_AUDIO_ADC0->adcd_dsmdiv_int.bit.adcd_dsmdiv_frac = D_frac_sel; break; }
uint32_t adc_rptr=0;
uint32_t addr_hi=0;
const int32_t g_adc_cicfircoeffTtablel[] =
{
    322, 564, -126, -698, 412, 971, -984, -1206, 1873, 1134, -3152, -547, 4648, -919, -6141,
    3470, 7097, -7327, -6921, 12411, 4724, -18559, 460, 25322, -10158, -32422, 27278, 40358, -61509, -57063,
    173450, 331522
};

const int32_t g_adc_halfbandcoeftablel[] =
{
    -430, 0, 643, 0, -1100, 0, 1748, 0, -2640, 0, 3841, 0, -5439, 0, 7567,
    0, -10446, 0, 14484, 0, -20557, 0, 30962, 0, -54150, 0, 166388, 262144
};
void AUDIO_AddaCoefWrite(const int32_t* coef, int size, int zero_size)
{
    for (int i = 0; i < size; i++)
    {
        CORE_AUDIO_ADC0->adcd_vof_coef_wdata.reg= coef[i];
        CORE_AUDIO_ADC0->adcd_ctrl.bit.vof_wr_en = ENABLE;
        CORE_AUDIO_ADC0->adcd_vof_wr_strobe.bit.adcd_vof_wr_strobe = ENABLE;
        CORE_AUDIO_ADC0->adcd_ctrl.bit.vof_wr_en = ENABLE;
        CORE_AUDIO_ADC0->adcd_vol_ctrl.bit.adcd_vol_ctrl_wr=1;
        CORE_AUDIO_ADC0->adcd_vof_wr_strobe.bit.adcd_vof_wr_strobe = DISABLE;
    }
    CORE_AUDIO_ADC0->adcd_vof_coef_wdata.reg= 0;
    for (int i = 0; i < zero_size; i++)
    {
        CORE_AUDIO_ADC0->adcd_ctrl.bit.vof_wr_en = ENABLE;
        CORE_AUDIO_ADC0->adcd_vof_wr_strobe.bit.adcd_vof_wr_strobe = ENABLE;
        CORE_AUDIO_ADC0->adcd_ctrl.bit.vof_wr_en = DISABLE;
        CORE_AUDIO_ADC0->adcd_vof_wr_strobe.bit.adcd_vof_wr_strobe = DISABLE;
    }
}
void AUDIO_AdcFilterInit(void)
{
    CORE_AUDIO_ADC0->adcd_ctrl.bit.vof_wr_en = ENABLE;
    AUDIO_AddaCoefWrite(g_adc_cicfircoeffTtablel, 32, 0);
    AUDIO_AddaCoefWrite(g_adc_halfbandcoeftablel, 28, 196);
    CORE_AUDIO_ADC0->adcd_ctrl.bit.vof_wr_en = DISABLE;
}

void AUDIO_AdcAnalogInit()
{
    AADC_ANALOG->aadc_en.reg=0;
    AADC_ANALOG->aadc_conf1.reg=0;
    AADC_ANALOG->aadc_conf2.reg=0;
    AADC_ANALOG->aadc_vmid.reg=0;
    AADC_ANALOG->aadc_en.bit.da_micbias_en = 1 ;
    AADC_ANALOG->aadc_en.bit.da_mic_reg_en = 1 ; //no
    AADC_ANALOG->aadc_en.bit.da_sdm_en = 1 ;
    AADC_ANALOG->aadc_en.bit.da_sdm_mode = 1 ;
    AADC_ANALOG->aadc_en.bit.da_sdm_dem_en = 1 ;
    AADC_ANALOG->aadc_en.bit.da_pga_diff_en = 1 ;
    AADC_ANALOG->aadc_en.bit.da_pga_en = 1 ; // or
    AADC_ANALOG->aadc_en.bit.da_pga_gain_boost_en = 0 ;
    AADC_ANALOG->aadc_en.bit.da_adc_reg_en = 1 ;
    AADC_ANALOG->aadc_en.bit.da_codec_reg_pwr_sel = 1 ;
    AADC_ANALOG->aadc_en.bit.da_aadc_top_ibias_en = 1 ;

    AADC_ANALOG->aadc_conf1.bit.rg_mic_hvreg_vc = 0 ;
    AADC_ANALOG->aadc_conf1.bit.rg_micbias_ibias_sel = 3 ;
    AADC_ANALOG->aadc_conf1.bit.rg_micbias_vc = 0x30 ;
    AADC_ANALOG->aadc_conf1.bit.rg_micbias_mode = 3 ;
    AADC_ANALOG->aadc_conf1.bit.rg_sdm_ota1_isel = 2 ;
    AADC_ANALOG->aadc_conf1.bit.rg_sdm_ota2_isel = 1 ;
    AADC_ANALOG->aadc_conf1.bit.rg_sdm_ref_isel = 2 ;
    AADC_ANALOG->aadc_conf1.bit.rg_pga_cfb_mode = 0 ;
    AADC_ANALOG->aadc_conf1.bit.rg_pga_gc = 4 ;

    AADC_ANALOG->aadc_conf2.bit.rg_pga_opa2_ibias_sel = 1 ;
    AADC_ANALOG->aadc_conf2.bit.rg_pga_cfsel = 7 ;
    AADC_ANALOG->aadc_conf2.bit.rg_adc_reg_ibias_sel = 1 ;
    AADC_ANALOG->aadc_conf2.bit.rg_adc_reg_vol_sel = 4 ;

    AADC_ANALOG->aadc_vmid.bit.da_vmid_bg_en=ENABLE;
    AADC_ANALOG->aadc_vmid.bit.rg_vmid_bg_trim=0x3;
    AADC_ANALOG->aadc_vmid.bit.rg_vmid_pga_mode=DISABLE;
    AADC_ANALOG->aadc_vmid.bit.rg_vmid2gpio_en=DISABLE;

    AADC_ANALOG->aadc_en.reg |=0;
    AADC_ANALOG->aadc_conf1.reg |=0;
    AADC_ANALOG->aadc_conf2.reg |=0;
    AADC_ANALOG->aadc_vmid.reg |=0;
}
/**
 * @name   AUDIO_AdcInit
 * @brief  MIC AUDIO_ADC Initialize configuration
 * @param  adcInitStruct: MIC AUDIO_ADC initialize structure
 * @retval None
 */
void AUDIO_AdcInit(AUDIO_AdcInitStruct *adcInitStruct)
{
    AUDIO_AdcAnalogInit();
    AUDIO_AdcFilterInit();
    CORE_CHIP_SYS->clken.bit.mic_dsm=ENABLE;
    CORE_CHIP_SYS->clken.bit.mic=ENABLE;//ENABLE mic
    CORE_CHIP_SYS->audio_clkctrl.bit.mic_clk_sel=1;//choose dsmdiv_clk

    switch(adcInitStruct->sampleRate)
    {
        CASE_ADC_CLK(SAMPLE_RATE_8K,      MIC_CLK_DIV_3,  0, 0x7d, 0x0)
        CASE_ADC_CLK(SAMPLE_RATE_16K,     MIC_CLK_DIV_2,  MIC_CLK_DIV_SEL_NONE, 0x5d, 0xc00000)
        CASE_ADC_CLK(SAMPLE_RATE_24K,     MIC_CLK_DIV_2,  MIC_CLK_DIV_SEL_NONE, 0x3e, 0x800000)
        CASE_ADC_CLK(SAMPLE_RATE_32K,     MIC_CLK_DIV_NONE,  MIC_CLK_DIV_SEL_NONE, 0x5d, 0xc00000)
        CASE_ADC_CLK(SAMPLE_RATE_44P1K,   MIC_CLK_DIV_NONE, MIC_CLK_DIV_SEL_NONE, 0x44, 0x45a8ec)
        CASE_ADC_CLK(SAMPLE_RATE_48K,     MIC_CLK_DIV_NONE,  MIC_CLK_DIV_SEL_NONE, 0x3e, 0x800000)
    }
    CORE_AUDIO_ADC0->adcd_dsmdiv_ctrl.bit.adcd_dsmdiv_ctrl |=1<<0;
    CORE_AUDIO_ADC0->adcd_dsmdiv_ctrl.bit.adcd_dsmdiv_ctrl |=1<<3;
    CORE_AUDIO_ADC0->adcd_dsmdiv_ctrl.bit.adcd_dsmdiv_ctrl &= ~(1<<3);

    CORE_AUDIO_ADC0->adcd_dcoff.bit.dc_offset=0;
    CORE_AUDIO_ADC0->adcd_delay.bit.adc_channel_switch_delay=0;
    CORE_AUDIO_ADC0->adcd_delay.bit.voice_filter_block_en=ENABLE;

    CORE_AUDIO_ADC0->adcd_ctrl.bit.ave_time_rssi=0x3;
    CORE_AUDIO_ADC0->adcd_ctrl.bit.clr_rssi=0;
    CORE_AUDIO_ADC0->adcd_ctrl.bit.rssi_en=DISABLE;

    CORE_AUDIO_ADC0->adcd_ctrl.bit.data_type=0;//voice data
    CORE_AUDIO_ADC0->adcd_ctrl.bit.dma_en=DISABLE;
    CORE_AUDIO_ADC0->adcd_mic_hpf_alpha.bit.mic_high_pass_filter=1997;
    CORE_AUDIO_ADC0->adcd_mic_hpf_alpha.bit.mic_hpf_clr=DISABLE;
    CORE_AUDIO_ADC0->adcd_mic_hpf_alpha.bit.mic_hpf_en=ADC_HPF;

    CORE_AUDIO_ADC0->adcd_fir_ctrl.bit.adc_comp_scale = 4;
    CORE_AUDIO_ADC0->adcd_fir_ctrl.bit.adc_hbf_scale = 4;

    CORE_AUDIO_ADC0->adcd_mic_ctrl.bit.micclk_inv = 0;
    CORE_AUDIO_ADC0->adcd_mic_ctrl.bit.micdat_mode = ADC_DATA_TYPE;//signed mode
    if(adcInitStruct->mode==MIC_MODE_ANA)
    {
        CORE_AUDIO_ADC0->adcd_mic_ctrl.bit.anaclk_en = ENABLE;
        CORE_AUDIO_ADC0->adcd_mic_ctrl.bit.pdmclk_en = DISABLE;
        CORE_AUDIO_ADC0->adcd_mic_ctrl.bit.pdmdat_sel = MIC_RISING_SAMPLE;
        CORE_AUDIO_ADC0->adcd_mic_ctrl.bit.mic_src = MIC_MODE_ANA;
    }
    else
    {
        CORE_AUDIO_ADC0->adcd_mic_ctrl.bit.anaclk_en = DISABLE;
        CORE_AUDIO_ADC0->adcd_mic_ctrl.bit.pdmclk_en = ENABLE;
        CORE_AUDIO_ADC0->adcd_mic_ctrl.bit.pdmdat_sel = adcInitStruct->pdmSampleEdge;
        CORE_AUDIO_ADC0->adcd_mic_ctrl.bit.mic_src = MIC_MODE_PDM;
    }

    CORE_AUDIO_ADC0->adcd_dc_ctrl.bit.adc_dc_dly_time = 0;
    CORE_AUDIO_ADC0->adcd_dc_ctrl.bit.adc_dc_avg_time = 0;
    CORE_AUDIO_ADC0->adcd_dc_ctrl.bit.adc_dc_en = DISABLE;
    AUDIO_AdcSetVol(ADC_VOL_INIT);
}
/**
 * @name   AUDIO_AdcSetVol
 * @brief  MIC Volume Settings
 * @param  vol:Volume,range:0~32768
 * @retval None
 */
void AUDIO_AdcSetVol(uint16_t vol)
{
    CORE_AUDIO_ADC0->adcd_vof_coef_wdata.reg=ADC_VOL_STEP|vol<<8|(~ADC_FADE)<<23;
    CORE_AUDIO_ADC0->adcd_vol_ctrl.bit.adcd_vol_ctrl_wr=1;
}
/**
 * @name   AUDIO_AdcSetMute
 * @brief  MIC mute configuration
 * @param  newState: 0:disabled MIC mute
 *                   1:enable MIC mute
 * @retval None
 */
void AUDIO_AdcSetMute(FunctionalState newState)
{
    CORE_AUDIO_ADC0->adcd_vof_coef_wdata.reg|=newState<<22;
    CORE_AUDIO_ADC0->adcd_vol_ctrl.bit.adcd_vol_ctrl_wr=1;
}
/**
 * @name   AUDIO_AdcDmaBufInit
 * @brief  MIC DMA Buffer Initialization Configuration
 * @param  buf:Buffer starting address
 * @param  bufSize:Buffer size
 * @retval None
 */
void AUDIO_AdcDmaBufInit(uint8_t *buf,uint16_t bufSize)
{
    CORE_AUDIO_ADC0->adcd_saddr.reg = (uint32_t)buf;
    CORE_AUDIO_ADC0->adcd_buflen.reg =  bufSize ;
    adc_rptr = (uint32_t)buf ;
}
/**
 * @name   AUDIO_AdcCmd
 * @brief  MIC DMA enable or disable
 * @param  state: ENABLE:enable MIC DMA
 *                DISABLE:disable MIC DMA
 * @retval None
 */
void AUDIO_AdcCmd(FunctionalState state)
{
    CORE_AUDIO_ADC0->adcd_ctrl.bit.dma_en = state;
}
/**
 * @name   AUDIO_AdcGetDataCount
 * @brief  Get MIC DMA available data
 * @param  None
 * @retval None
 */
uint32_t AUDIO_AdcGetDataCount(void)
{
    uint32_t adc_wptr;

    adc_wptr = CORE_AUDIO_ADC0->adcd_wptr.reg;
    if(adc_wptr >= adc_rptr)
    {
        return (adc_wptr-adc_rptr);
    }
    else
    {
        return ((CORE_AUDIO_ADC0->adcd_buflen.reg) - (adc_rptr - adc_wptr));
    }
}
/**
 * @name   AUDIO_AdcGetData
 * @brief  Get MIC DMA data
 * @param  buf:the starting address of the buffer that stores data
 * @param  len:buffer length
 * @retval None
 */
uint32_t AUDIO_AdcGetData(uint8_t *buf,uint32_t len)
{
    uint32_t read_len = AUDIO_AdcGetDataCount();
    uint32_t saddr = CORE_AUDIO_ADC0->adcd_saddr.reg;
    uint32_t eaddr = saddr+CORE_AUDIO_ADC0->adcd_buflen.reg;

    uint8_t *p = (uint8_t *)(adc_rptr);


    read_len = len >= read_len ? read_len : len;
    if(read_len > 0)
    {
        if((adc_rptr + read_len) <= eaddr)
        {
            memcpy(buf, p, read_len);
        }
        else
        {
            memcpy(buf, p, eaddr-adc_rptr);
            p = (uint8_t *)(saddr);
            memcpy(buf+(eaddr-adc_rptr), p, read_len-(eaddr-adc_rptr));
        }
        adc_rptr +=read_len;
        if(adc_rptr>=eaddr)
        {
            adc_rptr -=CORE_AUDIO_ADC0->adcd_buflen.reg;
        }
    }
    return read_len;
}
volatile uint32_t dac_wptr = 0;
static uint32_t AUDIO_DacGetClk(AUDIO_SampleRateEnum sampling_rate)
{
    uint32_t clk;
    if (sampling_rate==SAMPLE_RATE_8K)
    {
        CORE_CHIP_SYS->audio_clkctrl.bit.gdac_clk_sel=0;
    }
    if (sampling_rate==SAMPLE_RATE_16K)
    {
        CORE_CHIP_SYS->audio_clkctrl.bit.gdac_clk_sel=1;
    }
    if (sampling_rate==SAMPLE_RATE_32K)
    {
        CORE_CHIP_SYS->audio_clkctrl.bit.gdac_clk_sel=2;
    }
    clk = 12000000 *(1 << CORE_CHIP_SYS->audio_clkctrl.bit.gdac_clk_sel);
    return clk;
}
/**
 * @name   AUDIO_DacInit
 * @brief  DAC Initialize configuration
 * @param  sampleRate: DAC sample rate
 * @retval None
 */
void AUDIO_DacInit(AUDIO_SampleRateEnum sampleRate)
{
    SYSCTRL_ClkEn(CLK_GDAC,ENABLE);
    uint32_t dac_clk = AUDIO_DacGetClk(sampleRate);
    CORE_DAC->fs_div_cnt.bit.fs_div_cnt=dac_clk/1000/sampleRate;
    CORE_DAC->ds_coef_a.bit.ds_coef_a=0x12972;
    CORE_DAC->dac_ctrl.bit.threshold_gain=0x80;
    CORE_DAC->threshold_max.bit.threshold_max=0x100;
    CORE_DAC->dac_ctrl.bit.vox_timer_window=0X40;
    CORE_DAC->threshold_max.bit.vol_test=0x0e;
    CORE_DAC->dac_ctrl.bit.dac_ctrl=0x16;
    CORE_DAC->dac_ctrl.bit.dac_ctrl=0x17;
    CORE_DAC->dac_ctrl.bit.dac_ctrl=0x27;
    CORE_DAC->ds_coef_a.bit.calibration_window=0x2;
}
/**
 * @name   AUDIO_DacSetVol
 * @brief  DAC volume settings
 * @param  vol:Volume,range:0~7
 * @retval None
 */
void AUDIO_DacSetVol(uint8_t vol)
{
    __ASSERT_DAC_VOL(vol);
    CORE_DAC->threshold_max.bit.vol_test &= 0x0f;
    CORE_DAC->threshold_max.bit.vol_test |= (vol<<4);
}
/**
 * @name   AUDIO_DacDmaBufInit
 * @brief  DAC DMA Buffer configuration
 * @param  buf: The starting address of Buffer
 * @param  len: Buffer size
 * @retval None
 */
void AUDIO_DacDmaBufInit(uint8_t *buf, uint32_t len)
{
    __ASSERT_DAC_DMA_ADDR_ALIGNMENT((uint32_t)buf);
    CORE_DAC->dma_saddr.reg = (uint32_t)buf;
    CORE_DAC->dma_eaddr.reg = (uint32_t)buf + len-4;
    dac_wptr = CORE_DAC->dma_saddr.reg;
}
/**
 * @name   AUDIO_DacPutData
 * @brief  Put DAC data
 * @param  buf: buf addr
 * @param  len: data len
 * @retval put len
 */
uint32_t AUDIO_DacPutData(uint8_t *buf, uint32_t len)
{
    uint32_t saddr = CORE_DAC->dma_saddr.reg;
    uint32_t endaddr = CORE_DAC->dma_eaddr.reg+4;
    uint32_t packet_len = endaddr - dac_wptr;
    if(len > 0)
    {
        if(len <= packet_len)
        {
            memcpy((uint8_t *)dac_wptr , buf ,len);
            dac_wptr +=len;
        }
        else
        {
            memcpy((uint8_t *)dac_wptr , buf ,packet_len);
            memcpy((uint8_t *)saddr,buf+packet_len,len - packet_len);
            dac_wptr = saddr + len - packet_len;
        }
    }
    return len;
}
/**
 * @name   AUDIO_DacPutZero
 * @brief  Put zero DAC data
 * @param  len: data len
 * @retval put len
 */
uint32_t AUDIO_DacPutZero(uint32_t len)
{
    uint32_t saddr = CORE_DAC->dma_saddr.reg;
    uint32_t endaddr = CORE_DAC->dma_eaddr.reg+4;
    uint32_t packet_len = endaddr - dac_wptr;
    if(len > 0)
    {
        if(len <= packet_len)
        {
            memset((uint8_t *)dac_wptr , 0 ,len);
            dac_wptr +=len;
        }
        else
        {
            memset((uint8_t *)dac_wptr , 0 ,packet_len);
            memset((uint8_t *)saddr,0,len - packet_len);
            dac_wptr = saddr + len - packet_len;
        }
    }
    return len;
}
/**
 * @name   AUDIO_DacSetMode
 * @brief  Set DAC mode
 * @param  mode: DAC_MODE_SINGLE or DAC_MODE_CYCLE
 * @retval None
 */
void AUDIO_DacSetMode(uint8_t mode)
{
    if(mode == DAC_MODE_SINGLE)
    {
        CORE_DAC->dac_ctrl.bit.dac_ctrl &= ~(0x1 << 5);
    }
    else
    {
        CORE_DAC->dac_ctrl.bit.dac_ctrl |= (0x1 << 5);
    }
}
/**
 * @name   AUDIO_DacSetMute
 * @brief  DAC Mute enable or disable
 * @param  state: ENABLE:Enable DAC Mute
 *                DISABLE:Disable DAC Mute
 * @retval None
 */
void AUDIO_DacSetMute(FunctionalState mute)
{
    if(mute)
    {
        CORE_DAC->dac_ctrl.bit.dac_ctrl |=(1<<4);
    }
    else
    {
        CORE_DAC->dac_ctrl.bit.dac_ctrl &=~(1<<4);
    }
}
/**
 * @name   AUDIO_DacCmd
 * @brief  DAC DMA enable or disable
 * @param  state: ENABLE:disable DAC DMA
 *                DISABLE:enable DAC DMA
 * @retval None
 */
void AUDIO_DacCmd(FunctionalState state)
{
    if(state == ENABLE)
    {
        CORE_DAC->fs_div_cnt.bit.dma_en = 1;
    }
    else
    {
        CORE_DAC->fs_div_cnt.bit.dma_en = 0;
        dac_wptr = CORE_DAC->dma_saddr.reg;
    }
}
/**
 * @name   AUDIO_DacGetAvailableBufSize
 * @brief  Get DAC DMA buffer available data size
 * @param  None
 * @retval DAC DMA buffer available data size
 */
uint32_t AUDIO_DacGetAvailableBufSize(void)
{
    uint32_t rptr;
    rptr = CORE_DAC->dma_status.bit.ram_addr;
    if(rptr > dac_wptr)
    {
        return (rptr - dac_wptr);
    }
    else
    {
        return (CORE_DAC->dma_eaddr.reg - CORE_DAC->dma_saddr.reg) - (dac_wptr - rptr);
    }
}

/**
 * @name   AUDIO_DacSetIrq
 * @brief  Set DAC interrupt
 * @param  state: ENABLE:diable DAC interrupt
 *                DISABLE:enable DAC interrupt
 * @retval None
 */
void AUDIO_DacSetIrq(FunctionalState state)
{
    if(state == ENABLE)
    {
        CORE_DAC->irq.bit.irq_en |= 1;
    }
    else
    {
        CORE_DAC->irq.bit.irq_en &= ~1;
    }
}

/**
 * @name   AUDIO_DacSetIrqLevel
 * @brief set DAC water level interrupt
 * @param level : water level(4 bytes align)
 * @retval None
 */
void AUDIO_DacSetIrqLevel(uint16_t level)
{
    _ASSERT((level%4)==0);
    CORE_DAC->water_level.reg = level;

}

/**
 * @name   AUDIO_DacClearIrq
 * @brief clear dac interrupt status
 * @param None
 * @retval None
 */
void AUDIO_DacClearIrq(void)
{
    CORE_DAC->irq.bit.irq_reg = 1;
}


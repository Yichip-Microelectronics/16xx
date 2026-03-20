
#include "yc_sadc.h"
#include "yc_gpio.h"
#include "yc_plic.h"
#include "yc_timer.h"

#define ADC_BUF_LEN             3
#define TIMER_CH                SADC_TIMER6
#define TIMER_CLKDIV            (7)
typedef struct
{
    uint16_t adc_rbuf[ADC_BUF_LEN];
    uint16_t adc_ibuf[ADC_BUF_LEN];
}ADC_BUF_CB;
ADC_BUF_CB adc_buf={0};
/**
  * @brief  ADC_Configuration
  * @param  None
  * @retval None
  */
static void ADC_Configuration(void)
{
    SADC_InitStruct SADC_InitStruct;
    SADC_InitStruct.continuous_mode=DISABLE;
    SADC_InitStruct.auto_inject=DISABLE;
    SADC_InitStruct.etsrc=SADC_SOFTWARE;
    SADC_InitStruct.etsic=SADC_SOFTWARE;
    SADC_Init(&SADC_InitStruct);
    SADC_SetRegularChannel(1<<SADC_CHANNEL_GPIO22|1<<SADC_CHANNEL_GPIO23|1<<SADC_CHANNEL_GPIO24);
}
static void ADC_GpioConfig()
{
    GPIO_Config(GPIO_22,GPIO_MODE_HI_Z);
    GPIO_Config(GPIO_23,GPIO_MODE_HI_Z);
    GPIO_Config(GPIO_24,GPIO_MODE_HI_Z);
    GPIO_AnalogEn(GPIO_22,ENABLE);
    GPIO_AnalogEn(GPIO_23,ENABLE);
    GPIO_AnalogEn(GPIO_24,ENABLE);
}

static void TIM_CoreParaConfig(TIM_PortEnum timx, uint8_t clkDiv, uint32_t period)
{
    _ASSERT(period<=0xffff);
    TIM_InitStruct TIM_InitStruct;
    TIM_InitStruct.timx = timx;
    TIM_InitStruct.period = period;
    TIM_InitStruct.clockdiv = clkDiv;
    TIM_InitStruct.autoLoadEn=ENABLE;
    /*timer core init*/
    TIM_Init(&TIM_InitStruct);
    /*timer enable*/
    TIM_Cmd(timx, ENABLE);
}

int main(void)
{
    SYSCTRL_ChipInit();
    printport_init();
    yc_log("SADC DMA demo");
    ADC_GpioConfig();
    ADC_Configuration();
    SADC_DmaConfig(adc_buf.adc_rbuf,adc_buf.adc_rbuf+ADC_BUF_LEN-1,adc_buf.adc_ibuf+ADC_BUF_LEN-1);
    SADC_SetIrq(SADC_OVER,ENABLE);
    SADC_RegularTrigsel(TIMER_CH);
    TIM_CoreParaConfig((TIM_PortEnum)TIMER_CH, TIMER_CLKDIV, TIMER_1MS(TIMER_CLKDIV) * 100);
    PLIC_SetPriority(SARADC_IRQn,2);
    PLIC_EnableIRQ(SARADC_IRQn);

    while(1);
}
void SADC_IRQHandler(void)
{
    uint8_t irq_status=SADC_GetIrqStatus();
    if(irq_status&SADC_OVER)
    {
        SADC_DmaCmd(DISABLE);
        for(uint16_t i=0;i<ADC_BUF_LEN;i++)
        {
            yc_log("adc data[%d] = %d %dmv",i,adc_buf.adc_rbuf[i],SADC_CalculateVoltage(SADC_CHANNEL_GPIO22,adc_buf.adc_rbuf[i]));
        }
        SADC_DmaCmd(ENABLE);
    }
    SADC_ClearIrq(irq_status);
}
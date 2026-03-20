#include "yc_plic.h"
#include "yc_gpio.h"

#define VECTOR_INTERRUPT        0

#if VECTOR_INTERRUPT
#include "yc_vector.h"
#endif

int main(void)
{

    SYSCTRL_ChipInit();

    printport_init();
    #if VECTOR_INTERRUPT
    yc_log("gpio_vctor_interrupt demo");
    #else
    yc_log("gpio_interrupt demo");
    #endif
    yc_log("gpio %d is High_Interrupt",GPIO_HIGH_IRQ_PIN);
    yc_log("gpio %d is Low_Interrupt",GPIO_LOW_IRQ_PIN);
    yc_log("gpio %d is Rising_Interrupt",GPIO_RISING_IRQ_PIN);
    yc_log("gpio %d is Fall_Interrupt",GPIO_FALL_IRQ_PIN);

    GPIO_Config(GPIO_HIGH_IRQ_PIN,GPIO_MODE_PULL_DOWN);
    GPIO_SetIrqMode(GPIO_HIGH_IRQ_PIN,HIGH_INTERRUPT);
    GPIO_SetIrq(GPIO_HIGH_IRQ_PIN,ENABLE);

    GPIO_Config(GPIO_LOW_IRQ_PIN,GPIO_MODE_PULL_UP);
    GPIO_SetIrqMode(GPIO_LOW_IRQ_PIN,LOW_INTERRUPT);
    GPIO_SetIrq(GPIO_LOW_IRQ_PIN,ENABLE);

    GPIO_Config(GPIO_RISING_IRQ_PIN,GPIO_MODE_PULL_DOWN);
    GPIO_SetIrqMode(GPIO_RISING_IRQ_PIN,RISING_INTERRUPT);
    GPIO_SetIrq(GPIO_RISING_IRQ_PIN,ENABLE);

    GPIO_Config(GPIO_FALL_IRQ_PIN,GPIO_MODE_PULL_UP);
    GPIO_SetIrqMode(GPIO_FALL_IRQ_PIN,FALL_INTERRUPT);
    GPIO_SetIrq(GPIO_FALL_IRQ_PIN,ENABLE);
    #if VECTOR_INTERRUPT
    VECTOR_EnableIRQ(VECTOR_GPIO_IRQn);
    #else
    PLIC_SetPriority(GPIO_IRQn,2);
    PLIC_EnableIRQ(GPIO_IRQn);
    #endif
    while(1)
    {

    }
}

#if VECTOR_INTERRUPT
_interrupt void VECTOR_GPIO_IRQ(void)
#else
void GPIO_IRQHandler(void)
#endif
{
    uint8_t irq_status = 0;
    for(int i = 0;i <= GPIOE;i++) // find which group
    {
        irq_status=GPIO_GetPortIrqStatus(i);
        if(irq_status != 0)
        {
            for(int j=0;j<8;j++) // find which io
            {
                if(BIT_GET(irq_status,j))
                {
                    yc_log("gpio %d interrupt",j + (8*i));
                    GPIO_ClearIrq(j + (8*i));
                }
            }
        }
    }
}

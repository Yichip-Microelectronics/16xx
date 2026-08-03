#include "yc_vector.h"
#include "yc_systick.h"

int main(void)
{
    SYSCTRL_ChipInit();
    printport_init();
    yc_log("sysctrl no crystal demo");
    SYSTICK_Config(SYSTICK_ONE_MS*100);  //100ms

    while(1)
    {
        GPIO_Config(GPIO_LED_PIN,GPIO_MODE_OUT_HIGH);
        GPIO_Config(GPIO_LED_PIN,GPIO_MODE_OUT_LOW);
    }
}

/**
  * @brief  Systick interrupt service function.
  * @param  None
  * @retval None
  */
_interrupt void VECTOR_SYSTICK_IRQ(void)
{
    yc_log("SYSTICK_IRQHandler");

    SYSTICK_ClearIrq(); //must call this function at last!
}

/************************ (C) COPYRIGHT Yichip Microelectronics *****END OF FILE****/

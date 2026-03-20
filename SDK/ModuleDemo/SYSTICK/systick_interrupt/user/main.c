#include "yc_vector.h"
#include "yc_systick.h"

int main(void)
{
    SYSCTRL_ChipInit();
    printport_init();
    yc_log("systick_interrupt demo");
    SYSTICK_Config(SYSTICK_ONE_MS*100);  //100ms
    while(1);
}

/**
  * @brief  Systick interrupt service function.
  * @param  None
  * @retval None
  */
_interrupt void SYSTICK_IRQHandler(void)
{
    yc_log("SYSTICK_IRQHandler");

    SYSTICK_ClearIrq(); //must call this function at last!
}

/************************ (C) COPYRIGHT Yichip Microelectronics *****END OF FILE****/

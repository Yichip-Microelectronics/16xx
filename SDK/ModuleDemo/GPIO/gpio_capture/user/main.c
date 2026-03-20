#include "yc_plic.h"
#include "yc_gpio.h"
#include "yc_log.h"
int main(void)
{
  SYSCTRL_ChipInit();
  printport_init();
  GPIO_Config(GPIO_FREQUENCY_CAPTURE_PIN,GPIO_MODE_PULL_UP);//如果配置为高阻态则GPIO的数字功能无法使用
  uint32_t frequency = 0;
  while(1)
  {
    frequency = GPIO_Capture(GPIO_FREQUENCY_CAPTURE_PIN, 1);
    if(frequency>0)
    {
      yc_log("test gpio %d ",GPIO_FREQUENCY_CAPTURE_PIN);
      yc_log("gpio_frequency = %d ",frequency);
      delay_ms(1000);
    }
  }
}
/************************ (C) COPYRIGHT Yichip Microelectronics *****END OF FILE****/

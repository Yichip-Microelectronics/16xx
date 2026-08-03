
/* Includes ------------------------------------------------------------------*/
#include "board_config.h"
#include "ucos_ii.h"
#include "yc_systick.h"
#include "yc_plic.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define  MAIN_TASK_PRIO             4
#define  MAIN_TASK_STK_SIZE         1000
#define  V_TASK_PRIO                3
#define  V_TASK_STK_SIZE            1000
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static OS_STK  StartTaskMStk[MAIN_TASK_STK_SIZE];
static OS_STK  StartTask1Stk[MAIN_TASK_STK_SIZE];
/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  main task
  * @param  None
  * @retval None
  */
static void main_task(void *pvParameters)
{
    int i = 0;
    (void)pvParameters;

    GPIO_Config(GPIO_RISING_FALL_IRQ_PIN, GPIO_MODE_PULL_UP);
    GPIO_SetIrqMode(GPIO_RISING_FALL_IRQ_PIN, FALL_INTERRUPT);
    GPIO_SetIrq(GPIO_RISING_FALL_IRQ_PIN, ENABLE);
    PLIC_SetPriority(GPIO_IRQn,2);
    PLIC_EnableIRQ(GPIO_IRQn);

    while (1)
    {
        yc_log("main_task %d", i++);
        OSTimeDly(400);
    }
}

/**
  * @brief  main task
  * @param  None
  * @retval None
  */
static void vtask1(void *pvParameters)
{
    int i = 0;
    (void)pvParameters;

    while (1)
    {
        yc_log("vtask1 %d", i++);
        OSTimeDly(500);
    }
}

int main(void)
{
    SYSCTRL_ChipInit();
    printport_init();
    yc_log("UCOS-II demo");
    yc_log("system freq:%dHZ",SYSCTRL_GetMcuClk());

    OSInit();

    OSTaskCreateExt(main_task,
                (void *)0,
                (OS_STK *)&StartTaskMStk[MAIN_TASK_STK_SIZE - 1],
                MAIN_TASK_PRIO,
                MAIN_TASK_PRIO,
                (OS_STK *)&StartTaskMStk[0],
                MAIN_TASK_STK_SIZE,
                (void *)0,
                OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(vtask1,
                (void *)0,
                (OS_STK *)&StartTask1Stk[V_TASK_STK_SIZE - 1],
                V_TASK_PRIO,
                V_TASK_PRIO,
                (OS_STK *)&StartTask1Stk[0],
                V_TASK_STK_SIZE,
                (void *)0,
                OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSStart();
    yc_log("run here is error");
    return -1;

}

extern void xPortSysTickHandler( void );
_interrupt void VECTOR_SYSTICK_IRQ(void)
{
    SystickCount++;
    xPortSysTickHandler();
    SYSTICK_ClearIrq(); //must call this function at last!
}

void GPIO_IRQHandler(void)
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

/**
  * @brief  trap_c
  * @param  None
  * @retval None
  */
void trap_c(void)
{
    _ASSERT(0);
    while(1);
}


/************************ (C) COPYRIGHT Yichip Microelectronics *****END OF FILE****/

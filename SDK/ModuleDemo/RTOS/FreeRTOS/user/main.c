/* Kernel includes. */
#include "FreeRTOS.h" /* Must come first. */
#include "queue.h"    /* RTOS queue related API prototypes. */
#include "semphr.h"   /* Semaphore related API prototypes. */
#include "task.h"     /* RTOS task related API prototypes. */
#include "timers.h"   /* Software timer related API prototypes. */

/* TODO Add any manufacture supplied header files can be included
here. */
#include "board_config.h"
#include "yc_plic.h"
#include "yc_uart.h"
#include "yc_gpio.h"
#include "yc_timer.h"
#include "yc_systick.h"
#include "register.h"
#include "yc_log.h"
/* The period of the example software timer, specified in milliseconds, and
converted to ticks using the pdMS_TO_TICKS() macro. */
#define mainSOFTWARE_TIMER_PERIOD_MS pdMS_TO_TICKS(1000)
#define mainQUEUE_LENGTH             (1)
SemaphoreHandle_t xSemaphore=0;
static void prvSetupHardware(void);
extern void idle_task(void);
static void vExampleTimerCallback(TimerHandle_t xTimer);

/* The queue used by the queue send and queue receive tasks. */
static QueueHandle_t xQueue = NULL;

static TaskHandle_t StartTask1_Handler = NULL;
static TaskHandle_t StartTask2_Handler = NULL;
void prvSetupHardware(void)
{
    SYSCTRL_ChipInit();
    printport_init();
}
void vPortSetupTimerInterrupt(void)
{
    SYSTICK_Config(SYSTICK_ONE_MS);
}

void              start_task1(void *pvParameters);
void              start_task2(void *pvParameters);


/**
 * @brief  Timer core para config[timer_period_s=1/((SYSCTRL_GetTimerModuleClk()/(2^ClockDiv))/Period)]
 * @param  TIMx : the timer number,TIM0-TIM7
 * @param  clk_div : the timer clock div(0-7)
 * @param  period : the period count
 * @retval None
 */
#define TIMER_CLKDIV    (7)
#define TIMER_AUTOLOAD  (1)


void TIM_CoreParaConfig(TIM_PortEnum TIMx, uint8_t clk_div, uint32_t period)
{
    if(period>0xffff)
    {
        yc_log("period>65535[period=%d]",period);
    }
    else
    {
        TIM_InitStruct TIM_InitStruct;
        TIM_InitStruct.timx = TIMx;
        TIM_InitStruct.period = period;
        TIM_InitStruct.clockdiv = clk_div;
        TIM_InitStruct.autoLoadEn = TIMER_AUTOLOAD;

        /*timer core init*/
        TIM_Init(&TIM_InitStruct);

        /*irq enable*/
        TIM_ClearIrqStatus(TIMx);
        TIM_ITConfig(TIMx, ENABLE);

        /*timer enable*/
        TIM_Cmd(TIMx, ENABLE);

        PLIC_SetPriority(TIMx>TIM3?TIMERGR1_IRQn:TIMERGR0_IRQn, 1);
        PLIC_EnableIRQ(TIMx>TIM3?TIMERGR1_IRQn:TIMERGR0_IRQn);
    }
}
/**
  * @brief  Timer interrupt service function.
  * @param  None
  * @retval None
  */
void TIMERGR0_IRQHandler(void)
{
    /*Timer0 interrupt service function*/
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    TIM_ClearIrqStatus(TIM0);
    xSemaphoreGiveFromISR(xSemaphore,&xHigherPriorityTaskWoken) ;
    if(xHigherPriorityTaskWoken != false)
    {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

int main(void)
{
    TimerHandle_t xExampleSoftwareTimer = NULL;

    /* Configure the system ready to run the demo.  The clock configuration
    can be done here if it was not done before main() was called. */
    prvSetupHardware();
    xQueue = xQueueCreate(/* The number of items the queue can hold. */
                 mainQUEUE_LENGTH,
                 /* The size of each item the queue holds. */
                 sizeof(uint32_t));

    if (xQueue == NULL)
    {
        for (;;)
            ;
    }
    
	xSemaphore = xSemaphoreCreateCounting(1, 0);
    xTaskCreate((TaskFunction_t)start_task1, (const char *)"start_task1",
                (uint16_t)256, (void *)NULL, (UBaseType_t)2,
                (TaskHandle_t *)&StartTask1_Handler);

    xTaskCreate((TaskFunction_t)start_task2, (const char *)"start_task2",
                (uint16_t)256, (void *)NULL, (UBaseType_t)1,
                (TaskHandle_t *)&StartTask2_Handler);

    xExampleSoftwareTimer = xTimerCreate((const char *)"Timer", mainSOFTWARE_TIMER_PERIOD_MS,
                                         pdTRUE, (void *)0, vExampleTimerCallback);

    xTimerStart(xExampleSoftwareTimer, 0);
    printf("Before StartScheduler\r\n");

    /* Start the tasks and timer running. */
    printf("xTaskCreate done\r\n");

    vTaskStartScheduler();
    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    printf("vTaskStartScheduler error");
    return -1;
}

void start_task1(void *pvParameters)
{
    int cnt = 0;
    BaseType_t ret=pdFAIL;

    printf("Enter to task_1\r\n");

    yc_log("timer_clock_freq: %d Hz",TIMER_CLK);
    TIM_CoreParaConfig(TIM0, TIMER_CLKDIV, TIMER_1MS(TIMER_CLKDIV)*50);

    while (1)
    {
        printf("task1 is running %d.....\r\n", cnt++);
        ret=xSemaphoreTake(xSemaphore, 100);
        printf("xSemaphoreTake[%d]\r\n",ret);
    }
}

void start_task2(void *pvParameters)
{
    int cnt = 0;
    printf("Enter to task_2\r\n");
    while (1)
    {
        printf("task2 is running %d.....\r\n", cnt++);
        vTaskDelay(200);
    }
}

static void vExampleTimerCallback(TimerHandle_t xTimer)
{
    /* The timer has expired.  Count the number of times this happens.  The
    timer that calls this function is an auto re-load timer, so it will
    execute periodically. */
    static int cnt = 0;
    printf("timers Callback %d\r\n", cnt++);
}

void vApplicationTickHook(void)
{
    // BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* The RTOS tick hook function is enabled by setting configUSE_TICK_HOOK to
    1 in FreeRTOSConfig.h.

    "Give" the semaphore on every 500th tick interrupt. */

    /* If xHigherPriorityTaskWoken is pdTRUE then a context switch should
    normally be performed before leaving the interrupt (because during the
    execution of the interrupt a task of equal or higher priority than the
    running task was unblocked).  The syntax required to context switch from
    an interrupt is port dependent, so check the documentation of the port you
    are using.

    In this case, the function is running in the context of the tick interrupt,
    which will automatically check for the higher priority task to run anyway,
    so no further action is required. */
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook(void)
{
    /* The malloc failed hook is enabled by setting
    configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

    Called if a call to pvPortMalloc() fails because there is insufficient
    free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    internally by FreeRTOS API functions that create tasks, queues, software
    timers, and semaphores.  The size of the FreeRTOS heap is set by the
    configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
    printf("malloc failed\n");
    for (;;)
        ;
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    /* Run time stack overflow checking is performed if
    configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected.  pxCurrentTCB can be
    inspected in the debugger if the task name passed into this function is
    corrupt. */
    printf("Stack Overflow[xTask=%x pcTaskName=%s]\n",xTask,pcTaskName);
    for (;;)
        ;
}
/*-----------------------------------------------------------*/

extern UBaseType_t uxCriticalNesting;
void               vApplicationIdleHook(void)
{
    // volatile size_t xFreeStackSpace;
    /* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
    FreeRTOSConfig.h.

    This function is called on each cycle of the idle task.  In this case it
    does nothing useful, other than report the amount of FreeRTOS heap that
    remains unallocated. */
    /* By now, the kernel has allocated everything it is going to, so
    if there is a lot of heap remaining unallocated then
    the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
    reduced accordingly. */
}


#include "yc_log.h"
#include "yc_iic.h"
#include "yc_plic.h"
#include "yc_uart.h"
#include "yc_gpio.h"

#define IIC_DEVICE_WRITE_ADDR        0xa0
#define IIC_DEVICE_READ_ADDR         0xa1
#define IIC_INT_CONFIG               ENABLE
#define EEPROM_PAGE_SIZE             64
#define TEST_SIZE                    1000


typedef struct
{
    uint8_t deviceaddrwrite;    /*!< IIC_DEVICE_WRITE_ADDR */
    uint8_t writeaddr[2];        /*!< IIC Address for writing data */
}IIC_DataHeadStruct;

typedef struct
{
    IIC_DataHeadStruct sendhead;
    uint8_t data[EEPROM_PAGE_SIZE];
}IIC_SendDataStruct;

typedef struct
{
    IIC_DataHeadStruct readhead;
    uint8_t deviceaddrread;    /*!< IIC_DEVICE_READ_ADDR */
}IIC_ReadDataStruct;

void EEPROM_GPIO_Init(void)
{
    if(IIC_PORT==CORE_IIC)
    {
        SYSCTRL_ClkEn(CLK_IIC0,ENABLE);
        GPIO_Config(IIC_SCL_PIN, GPIO_MODE_IIC0_SCL);
        GPIO_Config(IIC_SDA_PIN, GPIO_MODE_IIC0_SDA);
        yc_log("IIC0 MODE SET OK!\n");
    }
    else if(IIC_PORT==CORE_IIC1)
    {
        SYSCTRL_ClkEn(CLK_IIC1,ENABLE);
        GPIO_Config(IIC_SCL_PIN, GPIO_MODE_IIC1_SCL);
        GPIO_Config(IIC_SDA_PIN, GPIO_MODE_IIC1_SDA);
        yc_log("IIC1 MODE SET OK!\n");
    }
}

void IIC_Configuration(uint32_t iicRate)
{
    IIC_InitStruct iicConfig;
    uint32_t sysclk,onecyle;    /*!< onecyle: one cyle length */
    sysclk=SYSCTRL_GetSystemClk();
    yc_log("sysclk=%d\n",sysclk);
    onecyle = sysclk/iicRate-8;

    iicConfig.scll=onecyle/2;
    iicConfig.sclh=onecyle/2;
    iicConfig.stsu = (iicConfig.sclh)/2;
    iicConfig.sthd = (iicConfig.sclh)/2;
    iicConfig.sosu = (iicConfig.sclh)/2;
    iicConfig.dtsu = iicConfig.sclh;
    iicConfig.dthd = 0;
    IIC_Init(IIC_PORT,&iicConfig);
}

void Eeprom_Write(uint16_t wAddr,uint8_t *data,uint16_t len)
{
    IIC_SendDataStruct send;
    uint32_t writed_len=0,pack_len=0;
    send.sendhead.deviceaddrwrite = IIC_DEVICE_WRITE_ADDR;
    for(writed_len=0;writed_len<len;)
    {
        send.sendhead.writeaddr[0] = ((wAddr >> 8)&0xff);
        send.sendhead.writeaddr[1] = (wAddr &0xff);
        pack_len=(len-writed_len)>EEPROM_PAGE_SIZE?EEPROM_PAGE_SIZE:(len-writed_len);
        memcpy(&send.data,data,pack_len);
        IIC_SendData(IIC_PORT,(uint8_t *)&send,pack_len+3);
        delay_ms(5);                      /*!< EEPROM page write time 5ms */
        data+=pack_len;
        wAddr+=pack_len;
        writed_len+=pack_len;
    }
}

void Eeprom_Read(uint16_t rAddr,uint8_t *data,uint16_t len)
{
    uint8_t sendBuff[4];
    IIC_ReadDataStruct *rSend;
    rSend=(IIC_ReadDataStruct *)sendBuff;

    rSend->readhead.deviceaddrwrite=IIC_DEVICE_WRITE_ADDR;
    rSend->readhead.writeaddr[0]=((rAddr>>8)&0xff);
    rSend->readhead.writeaddr[1]=(rAddr&0xff);
    rSend->deviceaddrread=IIC_DEVICE_READ_ADDR;

    IIC_ReceiveData(IIC_PORT,sendBuff,4,data,len);
}

int main(void)
{
    uint8_t write_buff[TEST_SIZE] = {0};
    uint8_t read_buff[TEST_SIZE] = {0};
    uint32_t i;
    SYSCTRL_ChipInit();
    printport_init();
    yc_log("iic_eeprom demo");

    EEPROM_GPIO_Init();
    IIC_Configuration(100000);    /*!< IIC rate 100k */
    IIC_SetIrq(IIC_PORT,IIC_INT_CONFIG);
    if(IIC_INT_CONFIG==ENABLE)
    {
        PLIC_SetPriority(IIC_IRQ_PORT,2);    /*!< IIC interrupt enable*/
        PLIC_EnableIRQ(IIC_IRQ_PORT);
    }
    for(i=0;i<TEST_SIZE;i++)
    {
        write_buff[i]=i%256;
    }
    Eeprom_Write(0,write_buff,TEST_SIZE);
    Eeprom_Read(0,read_buff,TEST_SIZE);
    for(i=0;i<TEST_SIZE;i++)
    {
        if(write_buff[i]!=read_buff[i])
        {
            yc_log("eeprom write&read error\nwrite_buff[%d]=%d\tread_buff[%d]=%d\n ",i,write_buff[i],i,read_buff[i]);
            break;
        }
    }
    if(i == TEST_SIZE)
    {
        yc_log("eeprom write&read success\n");
    }
    while (1)
    {
    }
}


void IIC0_IRQHandler(void)
{
    yc_log("IIC0 trigger interrupt\n");
    IIC_ClearIrq(IIC_PORT);
}

void IIC1_IRQHandler(void)
{
    yc_log("IIC1 trigger interrupt\n");
    IIC_ClearIrq(IIC_PORT);
}


#include "yc_log.h"
#include "yc_hspi.h"
#include "yc_plic.h"
#include "yc_uart.h"
#include "yc_gpio.h"

//flash cmd define
#define W25X_WriteEnable                0x06
#define W25X_WriteDisable               0x04
#define W25X_ReadStatusReg              0x05
#define W25X_WriteStatusReg             0x01
#define W25X_ReadData                   0x03
#define W25X_FastReadData               0x0B
#define W25X_FastReadDual               0x3B
#define W25X_PageProgram                0x02
#define W25X_BlockErase                 0xD8
#define W25X_SectorErase                0x20
#define W25X_ChipErase                  0xC7
#define W25X_PowerDown                  0xB9
#define W25X_ReleasePowerDown           0xAB
#define W25X_DeviceID                   0xAB
#define W25X_ManufactDeviceID           0x90
#define W25X_JedecDeviceID              0x9F

#define SINGLE_BYTE_TRANSCEIVER         1   /*单字节收发*/
#define SEND_AND_RECEIVE_INDEPENDENCE   2   /*收发独立*/
#define SEND_AND_RECEIVE_TOGETHER       3   /*收发一次性完成*/
#define TRANSCEIVER_MODE                SEND_AND_RECEIVE_TOGETHER

#define HSPI_IRQ_ENABLE                 0

void W25Q16_Gpioinit(void)
{
    #if TRANSCEIVER_MODE==SEND_AND_RECEIVE_TOGETHER
    GPIO_Config(SPI_NCS_PIN, GPIO_MODE_SPID_NCS);
    #else
    GPIO_Config(SPI_NCS_PIN, GPIO_MODE_OUT_HIGH);
    #endif
    GPIO_Config(SPI_MISO_PIN, GPIO_MODE_SPID_MISO);
    GPIO_Config(SPI_MOSI_PIN, GPIO_MODE_SPID_MOSI);
    GPIO_Config(SPI_SCK_PIN, GPIO_MODE_SPID_SCK);
}

void HSPI_Configuration(void)
{
    HSPI_InitStruct hspictrl = {0};
    uint32_t spi_module_clk=HSPI_GetClk();
    yc_log("SpiModuleClk=%d",spi_module_clk);

    hspictrl.rxAdjClk = 0;
    hspictrl.rxAdjen = 1;
    hspictrl.clkDiv = 0xf;     /*spi clock=spi_module_clk/((clkdiv+1)*2)*/
    hspictrl.txRxDly = 0;
    hspictrl.ncsDly = 7;
    hspictrl.txRxSeq = HSPI_TXRX_SEQUENCE_TX_FIRST;
    hspictrl.cpha = HSPI_CPHA_FIRST_EDGE;
    hspictrl.cpol = HSPI_CPOL_LOW;
    hspictrl.rxFifoWaterLevel = 1;
    hspictrl.txFifoWaterLevel = 5;
    hspictrl.fifoSwEn = 0;
    hspictrl.slaveMode = 0;
    hspictrl.halfDuplexEn = 0;
    HSPI_Init(&hspictrl);
}


void W25Q16_SendAndReceiveData(uint8_t *txbuff, uint32_t txlen, uint8_t *rxbuff, uint32_t rxlen)
{
    #if TRANSCEIVER_MODE==SINGLE_BYTE_TRANSCEIVER
    GPIO_Config(SPI_NCS_PIN, GPIO_MODE_OUT_LOW);
    for(int i=0;i<txlen;i++)
    {
        HSPI_SendData(txbuff[i]);
    }
    for(int i=0;i<rxlen;i++)
    {
        rxbuff[i]=HSPI_ReceiveData();
    }
    GPIO_Config(SPI_NCS_PIN, GPIO_MODE_OUT_HIGH);
    #elif TRANSCEIVER_MODE==SEND_AND_RECEIVE_INDEPENDENCE
    GPIO_Config(SPI_NCS_PIN, GPIO_MODE_OUT_LOW);
    if(txlen>0)
    {
        HSPI_SendBuff(txbuff,txlen);
    }
    if(rxlen>0)
    {
        HSPI_ReceiveBuff(rxbuff,rxlen);
    }
    GPIO_Config(SPI_NCS_PIN, GPIO_MODE_OUT_HIGH);
    #else
    HSPI_SendAndReceiveData(txbuff,txlen,rxbuff,rxlen);
    #endif
}

void W25Q16_WriteEnable(void)
{
    uint8_t TxBuff[4];
    TxBuff[0] = W25X_WriteEnable;

    W25Q16_SendAndReceiveData(TxBuff, 1, 0, 0);
}

void W25Q16_WaitBusyState(void)
{
    uint8_t TxBuff[4],RxBuff[4];
    TxBuff[0] = W25X_ReadStatusReg;

    do
    {
        W25Q16_SendAndReceiveData(TxBuff, 1, RxBuff, 1);
    }while(RxBuff[0] & 0x01);
}


void W25Q16_UnlockFlash(void)
{
    uint8_t TxBuff[4];
    TxBuff[0] = W25X_WriteStatusReg;
    TxBuff[1] = 0;
    TxBuff[2] = 0;

    W25Q16_WriteEnable();
    W25Q16_SendAndReceiveData(TxBuff, 3, 0, 0);
    W25Q16_WaitBusyState();
}



void W25Q16_SectorErase(uint32_t Addr)
{
    uint8_t TxBuff[4];

    W25Q16_WriteEnable();
    TxBuff[0] = W25X_SectorErase;
    TxBuff[1] = Addr>>16;
    TxBuff[2] =Addr>>8;
    TxBuff[3] =Addr;
    W25Q16_SendAndReceiveData(TxBuff, 4, 0, 0);
    W25Q16_WaitBusyState();
}

uint32_t W25Q16_ReadDeviceID(void)
{
    uint32_t ID = 0;
    uint8_t id[3] = {0};
    uint8_t TxBuff[4] = {W25X_JedecDeviceID,0x00,0x00,0x00};
    W25Q16_SendAndReceiveData(TxBuff, 1, id, 3);
    ID |= id[0]  << 16;
    ID |= id[1]  << 8;
    ID |= id[2]  << 0;
    return ID;
}

 void W25Q16_writeData(uint32_t Addr, uint8_t *buff, int len)
{
    uint8_t TxBuff[4+256];
    uint32_t packetAddr,packetLen,writedLen;

    TxBuff[0] = W25X_PageProgram;
    packetAddr=Addr;
    writedLen=0;

    while (writedLen<len)
    {
        packetLen= (256-(packetAddr&0xff));
        packetLen= packetLen>(len-writedLen) ? (len-writedLen) : packetLen;

        TxBuff[1] = packetAddr>>16;
        TxBuff[2] =packetAddr>>8;
        TxBuff[3] =packetAddr;
        memcpy(TxBuff+4,buff+writedLen,packetLen);

        W25Q16_WriteEnable();
        W25Q16_SendAndReceiveData(TxBuff, 4 + packetLen, 0, 0);
        W25Q16_WaitBusyState();

        packetAddr +=packetLen;
        writedLen +=packetLen;
    }
}

void W25Q16_ReadData(uint32_t Addr, uint8_t *p,uint32_t Len)
{
    uint8_t TxBuff[4];
    TxBuff[0] = W25X_ReadData;
    TxBuff[1] = Addr>>16;
    TxBuff[2] =Addr>>8;
    TxBuff[3] =Addr;

    W25Q16_SendAndReceiveData(TxBuff, sizeof(TxBuff), p, Len);
}

#define TEST_ADDR       (0)
#define TEST_SIZE       (0x2000)
#define SECTOR_SIZE     (0x1000)

int main(void)
{
    uint8_t writeBuf[SECTOR_SIZE] = {0},readBuf[SECTOR_SIZE];
    uint8_t test_error_flag=0;
    SYSCTRL_ChipInit();
    printport_init();
    yc_log("spi_flash demo\r\n");

    W25Q16_Gpioinit();
    HSPI_Configuration();
    HSPI_DmaEnableCmd(ENABLE);
    HSPI_EnableCmd(ENABLE);
    yc_log("id=%x\r\n",W25Q16_ReadDeviceID());
    #if HSPI_IRQ_ENABLE
    PLIC_SetPriority(HSPI_IRQn,1);
    PLIC_EnableIRQ(HSPI_IRQn);
    HSPI_ClearIrq(DMA_DONE);
    HSPI_SetIrq(DMA_DONE,ENABLE);
    #endif
    for(int i=0;i<sizeof(writeBuf);i++)
    {
        writeBuf[i]=i;
    }
    W25Q16_UnlockFlash();
    yc_log("flash unlock end");

    for(int i=0;i<TEST_SIZE;i+=SECTOR_SIZE)
    {
        W25Q16_SectorErase(TEST_ADDR+i);
    }
    yc_log("flash erase end");

    for(int i=0;i<TEST_SIZE;i+=SECTOR_SIZE)
    {
        W25Q16_ReadData(TEST_ADDR+i, readBuf, sizeof(readBuf));
        for(int j=0;j<SECTOR_SIZE;j++)
        {
            if(readBuf[j]!=0xff)
            {
                test_error_flag = 1;
                yc_log("0x%x erase error[%02x]",TEST_ADDR+i+j,readBuf[j]);
                break;
            }
        }
        if(test_error_flag)
        {
            break;
        }
    }
    if(test_error_flag == 0)
    {
        yc_log("flash erase check success");
    }
    test_error_flag = 0;
    for(int i=0;i<TEST_SIZE;i+=SECTOR_SIZE)
    {
        W25Q16_writeData(TEST_ADDR+i,writeBuf,sizeof(writeBuf));
    }
    yc_log("flash write end");

    for(int i=0;i<TEST_SIZE;i+=SECTOR_SIZE)
    {
        W25Q16_ReadData(TEST_ADDR+i, readBuf, sizeof(readBuf));
        for(int j=0;j<SECTOR_SIZE;j++)
        {
            if(readBuf[j]!=writeBuf[j])
            {
                test_error_flag = 1;
                yc_log("0x%x check error[%02x]",TEST_ADDR+i+j,readBuf[j]);
                break;
            }
        }
        if(test_error_flag)
        {
            break;
        }
    }
    if(test_error_flag == 0)
    {
        yc_log("flash write check success");
    }
    test_error_flag=0;
    while(1);
}

#if HSPI_IRQ_ENABLE
void HSPI_IRQHandler()
{
    if(HSPI_GetIrqStatus(DMA_DONE))
    {
        yc_log("hspi dma done irq");
    }
    HSPI_ClearIrq(ALL_SPI_STATUS);
}
#endif
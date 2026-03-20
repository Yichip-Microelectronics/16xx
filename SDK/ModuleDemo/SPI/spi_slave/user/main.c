
#include "yc_log.h"
#include "yc_hspi.h"
#include "yc_plic.h"
#include "yc_uart.h"
#include "yc_gpio.h"


#define SINGLE_BYTE_TRANSCEIVER         1   /*单字节收发*/
#define SEND_AND_RECEIVE_INDEPENDENCE   2   /*收发独立*/
#define SEND_AND_RECEIVE_TOGETHER       3   /*收发一次性完成*/
#define TRANSCEIVER_MODE                SEND_AND_RECEIVE_TOGETHER
#define TEST_SIZE                       10

void W25Q16_Gpioinit(void)
{
    #if TRANSCEIVER_MODE==SEND_AND_RECEIVE_TOGETHER
    GPIO_Config(SPI_NCS_PIN, GPIO_MODE_SPID_NCS);
    #else
    GPIO_Config(SPI_NCS_PIN, GPIO_MODE_OUT_HIGH);
    #endif
    GPIO_Config(SPI_MISO_PIN, GPIO_MODE_SPID_MISO|GPIO_MODE_PULL_DOWN);
    GPIO_Config(SPI_MOSI_PIN, GPIO_MODE_SPID_MOSI);
    GPIO_Config(SPI_SCK_PIN, GPIO_MODE_SPID_SCK);
}

void HSPI_Configuration(void)
{
    HSPI_InitStruct hspictrl = {0};
    uint32_t spi_module_clk=HSPI_GetClk();
    yc_log("SpiModuleClk=%d",spi_module_clk);

    hspictrl.rxAdjClk = 0x0;
    hspictrl.rxAdjen = 0;
    hspictrl.clkDiv = 0xf;     /*spi clock=spi_module_clk/((clkdiv+1)*2)*/
    hspictrl.txRxDly = 0x0b;
    hspictrl.ncsDly = 7;
    hspictrl.txRxSeq = HSPI_TXRX_SEQUENCE_TX_FIRST;
    hspictrl.cpha = HSPI_CPHA_SECOND_EDGE;
    hspictrl.cpol = HSPI_CPOL_HIGH;
    hspictrl.rxFifoWaterLevel = 16;
    hspictrl.txFifoWaterLevel = 16;
    hspictrl.fifoSwEn = 0;
    hspictrl.slaveMode = 1;
    hspictrl.halfDuplexEn = 0;
    hspictrl.halfDuplexOe = 0;
    HSPI_Init(&hspictrl);


}



int main(void)
{

    uint8_t read_test_buf[TEST_SIZE]={0},write_test_buf[TEST_SIZE]={0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa};
    SYSCTRL_ChipInit();
    printport_init();
    SYSCTRL_ClkEn(CLK_SPI,ENABLE);
    yc_log("spi test salve");

    W25Q16_Gpioinit();
    HSPI_DmaEnableCmd(ENABLE);
    HSPI_Configuration();
    HSPI_EnableCmd(ENABLE);
    HSPI_SlaveEnableCmd(ENABLE);
    HSPI_SendAndReceiveData(write_test_buf,TEST_SIZE,read_test_buf,TEST_SIZE);
    printv("data",read_test_buf,TEST_SIZE);
    yc_log("salve send,salve done");
    while(1)
    {
    }
}

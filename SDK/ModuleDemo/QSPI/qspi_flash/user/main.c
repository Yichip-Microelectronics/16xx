
#include "yc_log.h"
#include "yc_qspi.h"
#include "yc_plic.h"
#include "yc_uart.h"
#include "yc_gpio.h"
//#define VECTOR_GQSPI
#ifdef VECTOR_GQSPI
#include "yc_vector.h"
#endif
//flash cmd define
#define QSPI_W25X_WRITE_ENABLE                  0x06
#define QSPI_W25X_WRITE_DISABLE                 0x04
#define QSPI_W25X_READ_STATUS1                  0x05
#define QSPI_W25X_READ_STATUS2                  0x35
#define QSPI_W25X_READ_STATUS3                  0x15
#define QSPI_W25X_WRITE_STATUS1                 0x01
#define QSPI_W25X_WRITE_STATUS2                 0x31
#define QSPI_W25X_WRITE_STATUS3                 0x11
#define QSPI_W25X_READ_DATA                     0x03
#define QSPI_W25X_FASTREAD_DATA                 0x0B
#define QSPI_W25X_FASTREAD_DUAL1                0x3B
#define QSPI_W25X_FASTREAD_DUAL2                0xBB
#define QSPI_W25X_FASTREAD_QUAD1                0x6B
#define QSPI_W25X_FASTREAD_QUAD2                0xEB
#define QSPI_W25X_FASTREAD_QUAD3                0xE7
#define QSPI_W25X_PAGE_PROGRAM                  0x02
#define QSPI_W25X_QUAD_PAGE_PROGRAM             0x32
#define QSPI_W25X_SECTOR_ERASE                  0x20
#define QSPI_W25X_BLOCK_ERASE32K                0x52
#define QSPI_W25X_BLOCK_ERASE64K                0xD8
#define QSPI_QSPI_FlashCMD_PageErase            0x81
#define QSPI_QSPI_W25X_CHIP_ERASE               0xC7
#define QSPI_W25X_POWER_DOWN                    0xB9
#define QSPI_W25X_RELEASE_POWERDOWN             0xAB
#define QSPI_W25X_DEVICEID                      0xAB
#define QSPI_W25X_MANUFACT_DEVICEID             0x90
#define QSPI_W25X_JEDEC_DEVICEID                0x9F
#define QSPI_W25X_VOLATILE_WRITE                0X50
#define QSPI_W25X_RESET_ENABLE                  0X66
#define QSPI_W25X_DEVICE_RESET                  0X99

// #define QSPI_IRQ_ENABLE

#define TEST_ADDR                               (0)
#define TEST_SIZE                               (0x1000)
#define W25Q16_PAGE_SIZE                        256
#define SINGLE_BYTE_TRANSCEIVER                 1           /*单字节收发*/
#define SEND_AND_RECEIVE_INDEPENDENCE           2           /*收发独立*/
#define SEND_AND_RECEIVE_TOGETHER               3           /*收发一次性完成*/

#define TRANSCEIVER_MODE                        SEND_AND_RECEIVE_TOGETHER

void W25Q16_WriteEnable(void);
void W25Q16_WaitBusyState(void);
void W25Q16_SetQuadMod(FunctionalState state);
void W25Q16_ReadStatus1(uint8_t *status);
void W25Q16_ReadStatus2(uint8_t *status);
void W25Q16_WriteStatus(uint8_t *status);

uint8_t QSPI_ModeSelInterface( CORE_QSPI1_Type* qspix,
    uint8_t *cmdbuf,    uint8_t cmdlen,
    uint8_t *txbuf,     uint32_t txlen,
    uint8_t *rxbuf,     uint32_t rxlen)
{
#if TRANSCEIVER_MODE == SEND_AND_RECEIVE_TOGETHER
    return Qspi_SendAndReceiveData(qspix, cmdbuf, cmdlen, txbuf, txlen, rxbuf, rxlen);
#elif TRANSCEIVER_MODE == SEND_AND_RECEIVE_INDEPENDENCE
    GPIO_Config(QSPI_NCS_PIN, GPIO_MODE_OUT_LOW);
    if(cmdlen > 0)
    {
        Qspi_SendAndReceiveData(qspix, cmdbuf, cmdlen, 0, 0, 0, 0);
    }
    if(txlen > 0)
    {
        Qspi_SendAndReceiveData(qspix, 0, 0, txbuf, txlen, 0, 0);
    }
    if(rxlen > 0)
    {
        Qspi_SendAndReceiveData(qspix, 0, 0, 0, 0, rxbuf, rxlen);
    }
    GPIO_Config(QSPI_NCS_PIN, GPIO_MODE_OUT_HIGH);
    return 1;
#else
    uint32_t index = 0;
    GPIO_Config(QSPI_NCS_PIN, GPIO_MODE_OUT_LOW);
    while(cmdlen > 0)
    {
        Qspi_SendAndReceiveData(qspix, cmdbuf+index, 1, 0, 0, 0, 0);
        ++index;
        --cmdlen;
    }
    index = 0;
    while(txlen > 0)
    {
        Qspi_SendAndReceiveData(qspix, 0, 0, txbuf + index, 1, 0, 0);
        ++index;
        --txlen;
    }
    index = 0;
    while(rxlen > 0)
    {
        Qspi_SendAndReceiveData(qspix, 0, 0, 0, 0, rxbuf + index, 1);
        ++index;
        --rxlen;
    }

    GPIO_Config(QSPI_NCS_PIN, GPIO_MODE_OUT_HIGH);
    return 1;
#endif
}
void W25Q16_BLOCKErase_64K(uint32_t flash_addr)
{
    uint8_t CmdBuff[4] = {0};
    CmdBuff[0] = QSPI_W25X_BLOCK_ERASE64K;
    CmdBuff[1] = (flash_addr >> 16) & 0xff;
    CmdBuff[2] = (flash_addr >> 8) & 0xff;
    CmdBuff[3] = (flash_addr >> 0) & 0xff;
    W25Q16_WriteEnable();
    QSPI_SetMode(QSPI_PORT,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE);
    QSPI_SetDummy(QSPI_PORT,0);
    QSPI_ModeSelInterface(QSPI_PORT, CmdBuff,4,0,0,0,0);
    W25Q16_WaitBusyState();
}

void W25Q16_WriteFlashData(uint32_t flash_addr, uint8_t *tbuf, uint32_t tlen)
{
    uint8_t CmdBuff[4] = {0};
    uint32_t pack_len=0,writed_len=0;
    CmdBuff[0] = QSPI_W25X_PAGE_PROGRAM;
    W25Q16_SetQuadMod(DISABLE);
    while(writed_len<tlen)
    {
        CmdBuff[1] = ((flash_addr+writed_len) >> 16) & 0xff;
        CmdBuff[2] = ((flash_addr+writed_len) >> 8) & 0xff;
        CmdBuff[3] = ((flash_addr+writed_len) >> 0) & 0xff;
        pack_len = (tlen-writed_len)>W25Q16_PAGE_SIZE ? W25Q16_PAGE_SIZE : (tlen-writed_len);
        W25Q16_WriteEnable();
        QSPI_SetMode(QSPI_PORT,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE);
        QSPI_SetDummy(QSPI_PORT,0);
        QSPI_ModeSelInterface(QSPI_PORT, CmdBuff, 4, tbuf+writed_len,pack_len, 0, 0);
        W25Q16_WaitBusyState();
        writed_len+=pack_len;
    }
}

void W25Q16_WriteFlashData_QuadMode(uint32_t flash_addr, uint8_t *tbuf, uint32_t tlen)
{
    uint8_t CmdBuff[4] = {0};
    uint32_t pack_len=0,writed_len=0;
    CmdBuff[0] = QSPI_W25X_QUAD_PAGE_PROGRAM;
    W25Q16_SetQuadMod(ENABLE);
    while(writed_len<tlen)
    {
        CmdBuff[1] = ((flash_addr+writed_len) >> 16) & 0xff;
        CmdBuff[2] = ((flash_addr+writed_len) >> 8) & 0xff;
        CmdBuff[3] = ((flash_addr+writed_len) >> 0) & 0xff;
        pack_len = (tlen-writed_len)>W25Q16_PAGE_SIZE ? W25Q16_PAGE_SIZE : (tlen-writed_len);
        W25Q16_WriteEnable();
        QSPI_SetMode(QSPI_PORT,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE,QSPI_QSPI_MODE);
        QSPI_SetDummy(QSPI_PORT,0);
        QSPI_ModeSelInterface(QSPI_PORT, CmdBuff, 4, tbuf+writed_len,pack_len, 0, 0);
        W25Q16_WaitBusyState();
        writed_len+=pack_len;
    }

}

void W25Q16_ReadFlashData_03(uint32_t flash_addr, uint8_t *rbuf, uint32_t rlen)
{
    uint8_t CmdBuff[4] = {0};
    CmdBuff[0] = QSPI_W25X_READ_DATA;
    CmdBuff[1] = (flash_addr >> 16) & 0xff;
    CmdBuff[2] = (flash_addr >> 8) & 0xff;
    CmdBuff[3] = (flash_addr >> 0) & 0xff;
    W25Q16_SetQuadMod(DISABLE);
    QSPI_SetMode(QSPI_PORT,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE);
    QSPI_SetDummy(QSPI_PORT,0);
    QSPI_ModeSelInterface(QSPI_PORT, CmdBuff,4,0,0,rbuf,rlen);
}

void W25Q16_ReadFlashData_0B(uint32_t flash_addr, uint8_t *rbuf, uint32_t rlen)
{
    uint8_t CmdBuff[4] = {0};
    CmdBuff[0] = QSPI_W25X_FASTREAD_DATA;
    CmdBuff[1] = (flash_addr >> 16) & 0xff;
    CmdBuff[2] = (flash_addr >> 8) & 0xff;
    CmdBuff[3] = (flash_addr >> 0) & 0xff;
    W25Q16_SetQuadMod(DISABLE);
    QSPI_SetMode(QSPI_PORT,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE);
    #if(TRANSCEIVER_MODE == SINGLE_BYTE_TRANSCEIVER)
    QSPI_ModeSelInterface(QSPI_PORT, CmdBuff, 5, 0, 0, rbuf, rlen);
    #else
    QSPI_SetDummy(QSPI_PORT,8);
    QSPI_ModeSelInterface(QSPI_PORT, CmdBuff,4,0,0,rbuf,rlen);
    #endif
}

void W25Q16_ReadFlashData_DualMode_3B(uint32_t flash_addr, uint8_t *rbuf, uint32_t rlen)
{
    uint8_t CmdBuff[5] = {0};
    CmdBuff[0] = QSPI_W25X_FASTREAD_DUAL1;
    CmdBuff[1] = (flash_addr >> 16) & 0xff;
    CmdBuff[2] = (flash_addr >> 8) & 0xff;
    CmdBuff[3] = (flash_addr >> 0) & 0xff;
    W25Q16_SetQuadMod(DISABLE);
    QSPI_SetMode(QSPI_PORT,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE,QSPI_DUAL_MODE);
    #if(TRANSCEIVER_MODE == SINGLE_BYTE_TRANSCEIVER)
    QSPI_ModeSelInterface(QSPI_PORT, CmdBuff, 5, 0, 0, rbuf, rlen);
    #else
    QSPI_SetDummy(QSPI_PORT,8);
    QSPI_ModeSelInterface(QSPI_PORT, CmdBuff,4,0,0,rbuf,rlen);
    #endif
}

void W25Q16_ReadFlashData_DualMode_BB(uint32_t flash_addr, uint8_t *rbuf, uint32_t rlen)
{
    uint8_t CmdBuff[5] = {0};
    CmdBuff[0] = QSPI_W25X_FASTREAD_DUAL2;
    CmdBuff[1] = (flash_addr >> 16) & 0xff;
    CmdBuff[2] = (flash_addr >> 8) & 0xff;
    CmdBuff[3] = (flash_addr >> 0) & 0xff;
    CmdBuff[4] = 1<<4;
    W25Q16_SetQuadMod(DISABLE);
    QSPI_SetMode(QSPI_PORT,QSPI_STANDARD_MODE,QSPI_DUAL_MODE,QSPI_DUAL_MODE);
    #if(TRANSCEIVER_MODE == SINGLE_BYTE_TRANSCEIVER)
    QSPI_ModeSelInterface(QSPI_PORT, CmdBuff, 1, &CmdBuff[1], 4, rbuf, rlen);
    #else
    QSPI_SetDummy(QSPI_PORT,0);
    QSPI_ModeSelInterface(QSPI_PORT, CmdBuff, 5, 0, 0, rbuf, rlen);
    #endif
}

void W25Q16_ReadFlashData_QuadMode_6B(uint32_t flash_addr, uint8_t *rbuf, uint32_t rlen)
{
    uint8_t CmdBuff[6] = {0};
    CmdBuff[0] = QSPI_W25X_FASTREAD_QUAD1;
    CmdBuff[1] = (flash_addr >> 16) & 0xff;
    CmdBuff[2] = (flash_addr >> 8) & 0xff;
    CmdBuff[3] = (flash_addr >> 0) & 0xff;
    W25Q16_SetQuadMod(ENABLE);
    QSPI_SetMode(QSPI_PORT,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE,QSPI_QSPI_MODE);
    #if(TRANSCEIVER_MODE == SINGLE_BYTE_TRANSCEIVER)
    QSPI_ModeSelInterface(QSPI_PORT, CmdBuff, 4, &CmdBuff[1], 4, rbuf, rlen);
    #else
    QSPI_SetDummy(QSPI_PORT,8);
    QSPI_ModeSelInterface(QSPI_PORT, CmdBuff, 4, 0, 0, rbuf, rlen);
    #endif
}

void W25Q16_ReadFlashData_QuadMode_EB(uint32_t flash_addr, uint8_t *rbuf, uint32_t rlen)
{
    uint8_t CmdBuff[6] = {0};
    CmdBuff[0] = QSPI_W25X_FASTREAD_QUAD2;
    CmdBuff[1] = (flash_addr >> 16) & 0xff;
    CmdBuff[2] = (flash_addr >> 8) & 0xff;
    CmdBuff[3] = (flash_addr >> 0) & 0xff;
    CmdBuff[4] = 1<<4;
    W25Q16_SetQuadMod(ENABLE);
    QSPI_SetMode(QSPI_PORT,QSPI_STANDARD_MODE,QSPI_QSPI_MODE,QSPI_QSPI_MODE);
    #if(TRANSCEIVER_MODE == SINGLE_BYTE_TRANSCEIVER)
    QSPI_ModeSelInterface(QSPI_PORT, CmdBuff, 1,&CmdBuff[1], 6, rbuf, rlen);
    #else
    QSPI_SetDummy(QSPI_PORT,4);
    QSPI_ModeSelInterface(QSPI_PORT, CmdBuff, 5, 0, 0, rbuf, rlen);
    #endif
}

void W25Q16_WriteEnable(void)
{
    uint8_t CmdBuff[4] = {0};
    CmdBuff[0] = QSPI_W25X_WRITE_ENABLE;
    QSPI_SetMode(QSPI_PORT,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE);
    QSPI_SetDummy(QSPI_PORT,0);
    QSPI_ModeSelInterface(QSPI_PORT,CmdBuff, 1, 0, 0, 0, 0);
}

void W25Q16_WaitBusyState(void)
{
    uint8_t wait[2];
    wait[0] = 1;
    while (wait[0] & 1)
    {
        W25Q16_ReadStatus1(wait);
    }
}

uint32_t W25Q16_ReadJedecID(void)
{
    uint32_t ID = 0;
    uint8_t RxBuff[4] = {0};
    uint8_t CmdBuff[4] = {0};
    CmdBuff[0] = QSPI_W25X_JEDEC_DEVICEID;
    QSPI_SetMode(QSPI_PORT,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE);
    QSPI_SetDummy(QSPI_PORT,0);
    QSPI_ModeSelInterface(QSPI_PORT, CmdBuff,1,0,0,RxBuff,3);
    ID |= RxBuff[0] <<16;
    ID |= RxBuff[1] << 8;
    ID |= RxBuff[2] << 0;
    return ID;
}

uint16_t W25Q16_ReadDeviceID(void)
{
    uint16_t ID = 0;
    uint8_t RxBuff[4] = {0};
    uint8_t CmdBuff[4] = {0};
    CmdBuff[0] = QSPI_W25X_MANUFACT_DEVICEID;
    QSPI_SetMode(QSPI_PORT,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE);
    QSPI_SetDummy(QSPI_PORT,0);
    QSPI_ModeSelInterface(QSPI_PORT, CmdBuff,4,0,0,RxBuff,2);
    ID |= RxBuff[0] << 8;
    ID |= RxBuff[1] << 0;
    return ID;
}

void W25Q16_SetQuadMod(FunctionalState state)
{
    uint8_t status[2]={0};
    W25Q16_ReadStatus1(&status[0]);
    W25Q16_ReadStatus2(&status[1]);
    if(state == ENABLE)
    {
        if((status[1] & 0x02) == 0)
        {
            status[1] |= 0x02;
            W25Q16_WriteStatus(status);
        }
    }
    else
    {
        if(status[1] & 0x02)
        {
            status[1] &= 0xfd;
            W25Q16_WriteStatus(status);
        }
    }
}

void W25Q16_ReadStatus1(uint8_t *status)
{
    uint8_t CmdBuff[4] = {0};
    CmdBuff[0] = QSPI_W25X_READ_STATUS1;
    QSPI_SetMode(QSPI_PORT,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE);
    QSPI_SetDummy( QSPI_PORT,0);
    QSPI_ModeSelInterface(QSPI_PORT, CmdBuff, 1, 0, 0, status, 1);
}

void W25Q16_ReadStatus2(uint8_t *status)
{
    uint8_t CmdBuff[4] = {0};
    CmdBuff[0] = QSPI_W25X_READ_STATUS2;
    QSPI_SetMode(QSPI_PORT,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE);
    QSPI_SetDummy( QSPI_PORT,0);
    QSPI_ModeSelInterface(QSPI_PORT, CmdBuff, 1, 0, 0, status, 1);
}


void W25Q16_WriteStatus(uint8_t *status)
{
    uint8_t CmdBuff[4] = {0};
    CmdBuff[0] = QSPI_W25X_WRITE_STATUS1;
    CmdBuff[1] = status[0];
    CmdBuff[2] = status[1];
    W25Q16_WriteEnable();
    QSPI_SetMode(QSPI_PORT,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE);
    QSPI_SetDummy( QSPI_PORT,0);
    QSPI_ModeSelInterface(QSPI_PORT, CmdBuff, 3, 0, 0, 0, 0);
    W25Q16_WaitBusyState();
}


void W25Q16_ResetEnable(void)
{
    uint8_t CmdBuff[4] = {0};
    CmdBuff[0] = QSPI_W25X_RESET_ENABLE;
    QSPI_SetMode(QSPI_PORT,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE);
    QSPI_SetDummy( QSPI_PORT,0);
    QSPI_ModeSelInterface(QSPI_PORT, CmdBuff, 1, 0, 0, 0, 0);
}

void W25Q16_Reset(void)
{
    uint8_t CmdBuff[4] = {0};
    CmdBuff[0] = QSPI_W25X_DEVICE_RESET;
    QSPI_SetMode(QSPI_PORT,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE,QSPI_STANDARD_MODE);
    QSPI_SetDummy( QSPI_PORT,0);
    QSPI_ModeSelInterface(QSPI_PORT, CmdBuff, 1, 0, 0, 0, 0);
}

void W25Q16_GPIO_Init(void)
{
    if(QSPI_PORT==CORE_QSPI1)
    {
        GPIO_Config(QSPI_NCS_PIN, GPIO_MODE_QSPI1_NCS);
        GPIO_Config(QSPI_SCK_PIN, GPIO_MODE_QSPI1_SCK);
        GPIO_Config(QSPI_IO0_PIN, GPIO_MODE_QSPI1_IO0);
        GPIO_Config(QSPI_IO1_PIN, GPIO_MODE_QSPI1_IO1);
        GPIO_Config(QSPI_IO2_PIN, GPIO_MODE_QSPI1_IO2);
        GPIO_Config(QSPI_IO3_PIN, GPIO_MODE_QSPI1_IO3);
    }
    if(QSPI_PORT==CORE_QSPI2)
    {
        GPIO_Config(QSPI_NCS_PIN, GPIO_MODE_QSPI2_NCS);
        GPIO_Config(QSPI_SCK_PIN, GPIO_MODE_QSPI2_SCK);
        GPIO_Config(QSPI_IO0_PIN, GPIO_MODE_QSPI2_IO0);
        GPIO_Config(QSPI_IO1_PIN, GPIO_MODE_QSPI2_IO1);
        GPIO_Config(QSPI_IO2_PIN, GPIO_MODE_QSPI2_IO2);
        GPIO_Config(QSPI_IO3_PIN, GPIO_MODE_QSPI2_IO3);
    }
}


int main(void)
{
    uint8_t test_wbuf[TEST_SIZE]={0};
    uint8_t test_rbuf[TEST_SIZE]={0};
    SYSCTRL_ChipInit();
    printport_init();
    yc_log("qspi flash demo");
    W25Q16_GPIO_Init();
    QSPI_ClkSel(QSPI_PORT,DPLL_CLKSEL_24MHZ);
    QSPI_ClkCmd(QSPI_PORT, ENABLE);
#ifdef QSPI_IRQ_ENABLE
    #ifdef VECTOR_GQSPI
        QSPI_IrqConfig(QSPI_PORT,ENABLE);
        VECTOR_EnableIRQ(QSPI_VECTOR_PORT);
    #else
        QSPI_IrqConfig(QSPI_PORT,ENABLE);
        PLIC_SetPriority(QSPI_IRQ_PORT,2);
        PLIC_EnableIRQ(QSPI_IRQ_PORT);
    #endif
#endif

    W25Q16_SetQuadMod(ENABLE);
    yc_log("FLASH ID:%x",W25Q16_ReadJedecID());
    W25Q16_BLOCKErase_64K(TEST_ADDR);
    yc_log("flash erase end");
    for(uint32_t i=0;i<TEST_SIZE;i++)
    {
        test_wbuf[i]=i%256;
    }
    W25Q16_WriteFlashData(TEST_ADDR,test_wbuf,TEST_SIZE);
    yc_log("flash write end!");

    W25Q16_ReadFlashData_03(TEST_ADDR,test_rbuf,TEST_SIZE);
    for(int i=0;i<TEST_SIZE;i++)
    {
        if(test_rbuf[i]!=test_wbuf[i])
        {
            yc_log("qspi flash standard_03 mod write/read error\ntest_wbuf[%d]=%d\ttest_rbuf[%d]=%d",i,test_wbuf[i],i,test_rbuf[i]);
            break;
        }
        if(i==(TEST_SIZE-1))
        {
            yc_log("qspi flash standard_03 write/read success");
        }
    }

    W25Q16_ReadFlashData_DualMode_3B(TEST_ADDR,test_rbuf,TEST_SIZE);
    for(int i=0;i<TEST_SIZE;i++)
    {
        if(test_rbuf[i]!=test_wbuf[i])
        {
            yc_log("qspi flash dual_3B mod write/read error\ntest_wbuf[%d]=%d\ttest_rbuf[%d]=%d",i,test_wbuf[i],i,test_rbuf[i]);
            break;
        }
        if(i==(TEST_SIZE-1))
        {
            yc_log("qspi flash dual_3B mod write/read success");
        }
    }
    W25Q16_ReadFlashData_QuadMode_6B(TEST_ADDR,test_rbuf,TEST_SIZE);
    for(int i=0;i<TEST_SIZE;i++)
    {
        if(test_rbuf[i]!=test_wbuf[i])
        {
            yc_log("qspi flash quad_6B mod write/read error\ntest_wbuf[%d]=%d\ttest_rbuf[%d]=%d",i,test_wbuf[i],i,test_rbuf[i]);
            break;
        }
        if(i==(TEST_SIZE-1))
        {
            yc_log("qspi flash quad_6B mod write/read success");
        }
    }
    W25Q16_BLOCKErase_64K(TEST_ADDR);
    for(uint32_t i=TEST_SIZE-1;i<TEST_SIZE;i--)
    {
        test_wbuf[i]=i%256;
    }
    W25Q16_WriteFlashData_QuadMode(TEST_ADDR,test_wbuf,TEST_SIZE);
    yc_log("flash quad mod write end!");

    W25Q16_ReadFlashData_0B(TEST_ADDR,test_rbuf,TEST_SIZE);
    for(int i=0;i<TEST_SIZE;i++)
    {
        if(test_rbuf[i]!=test_wbuf[i])
        {
            yc_log("qspi flash standard_0B write/read error\ntest_wbuf[%d]=%d\ttest_rbuf[%d]=%d",i,test_wbuf[i],i,test_rbuf[i]);
            break;
        }
        if(i==(TEST_SIZE-1))
        {
            yc_log("qspi flash standard_0B write/read success");
        }
    }


    W25Q16_ReadFlashData_DualMode_BB(TEST_ADDR,test_rbuf,TEST_SIZE);
    for(int i=0;i<TEST_SIZE;i++)
    {
        if(test_rbuf[i]!=test_wbuf[i])
        {
            yc_log("qspi flash dual_BB mod write/read error\ntest_wbuf[%d]=%d\ttest_rbuf[%d]=%d",i,test_wbuf[i],i,test_rbuf[i]);
            break;
        }
        if(i==(TEST_SIZE-1))
        {
            yc_log("qspi flash dual_BB mod write/read success");
        }
    }

    W25Q16_ReadFlashData_QuadMode_EB(TEST_ADDR,test_rbuf,TEST_SIZE);
    for(int i=0;i<TEST_SIZE;i++)
    {
        if(test_rbuf[i]!=test_wbuf[i])
        {
            yc_log("qspi flash quad_EB mod write/read error\ntest_wbuf[%d]=%d\ttest_rbuf[%d]=%d",i,test_wbuf[i],i,test_rbuf[i]);
            break;
        }
        if(i==(TEST_SIZE-1))
        {
            yc_log("qspi flash quad_EB mod write/read success");
        }
    }
    while(1);
}

#ifdef VECTOR_GQSPI
_interrupt void VECTOR_QSPI1_IRQ(void)
{
    yc_log("QSPI1 vector interrupt trigger!");
    QSPI_IrqClear(CORE_QSPI1);
}
_interrupt void VECTOR_QSPI2_IRQ(void)
{
    yc_log("QSPI2 vector interrupt trigger!");
    QSPI_IrqClear(CORE_QSPI2);
}
#else
void QSPI1_IRQHandler(void)
{
    yc_log("QSPI1 interrupt trigger!");
    QSPI_ClearIrq(CORE_QSPI1);
}

void QSPI2_IRQHandler(void)
{
    yc_log("QSPI2 interrupt trigger!");
    QSPI_ClearIrq(CORE_QSPI2);
}
#endif


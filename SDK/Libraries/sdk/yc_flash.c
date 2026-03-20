/*
File Name    : yc_flash.c
Author       : Yichip
Version      : V1.0
Date         : 2024/5/11
Description  : flash driver interface implement.
*/
#include "yc_flash.h"

#if (FLASH_USE_ROM_API == 0)
/**
 * @brief  The qspi0 dma starts
 *
 * @param src The qspi0 DMA push the TX pointer
 * @param srclen The qspi0 DMA push the TX len
 * @param dst The qspi0 DMA push the RX pointer
 * @param dstlen The qspi0 DMA push the RX len
 */
void __attribute__((noinline))  __qspi0_dmastart(uint8_t *src, int srclen, uint8_t *dst, int dstlen)
{
    CORE_QSPI->qspi_txaddr.reg =  (uint32_t)((src));

    CORE_QSPI->qspi_txlen.reg = srclen;
    CORE_QSPI->qspi_rxaddr.reg = (uint32_t)((dst));
    CORE_QSPI->qspi_rxlen.reg = dstlen;
    CORE_QSPI->qspi_start.bit.qspi_start = ENABLE;
}

void __attribute__((noinline)) __xmemcpy(uint8_t *dst, const uint8_t *src, int len)
{
    for(int i = 0; i < len; i++)
    {
        dst[i] = src[i];
    }
}
/**
 * @brief The qspi0 dma wait
 *
 * @return qspi0 dma wait status
 */
uint8_t __attribute__((noinline)) __qspi0_dmawait(void)
{
    volatile int timeout = 0;
    while(!(CORE_QSPI->qspi_done.bit.qspi_done))
    {
        timeout++;
        if(timeout > DMA_WAIT_TIMEOUT)
        {
            return ERROR;
        }
    }
    return SUCCESS;
}

/**
 * @brief The qspi0 dma wirte function
 *
 * @param tbuf The qspi0 dma write buf addr
 * @param len The qspi0 dma write buf len
 * @return qspi0 dma wait status
 */
uint8_t __attribute__((noinline)) __qspi0_write(uint8_t *tbuf, uint32_t len)
{
    volatile uint8_t ret = 0;
    volatile uint32_t ctrl = CORE_QSPI->qspi_ctrl.reg;

    CORE_QSPI->qspi_ctrl.reg = 0;
    __qspi0_dmastart(tbuf, len, 0, 0);
    ret = __qspi0_dmawait();

    CORE_QSPI->qspi_ctrl.reg = ctrl;
    return ret;
}

/**
 * @brief The qspi0 dma wirte function
 *
 * @param tbuf The qspi0 dma write buf addr
 * @param tlen The qspi0 dma write buf len
 * @param rbuf The qspi0 dma read buf addr
 * @param rlen The qspi0 dma read buf len
 * @return qspi0 dma wait status
 */
uint8_t __attribute__((noinline)) __qspi0_readwrite(uint8_t *tbuf, uint32_t tlen, uint8_t *rbuf, uint32_t rlen)
{
    volatile uint8_t ret = 0;

    volatile uint32_t ctrl = CORE_QSPI->qspi_ctrl.reg;
    CORE_QSPI->qspi_ctrl.reg = 0;
    __qspi0_dmastart(tbuf, tlen, rbuf, rlen);
    ret = __qspi0_dmawait();

    CORE_QSPI->qspi_ctrl.reg = ctrl;
    return ret;
}
/**
 * @brief The qspi0 send command to flash
 *
 * @param cmd The flash command to write to
 * @return qspi0 send command status
 */
uint8_t __attribute__((noinline)) __qspi0_flashcmd(uint8_t cmd)
{
    uint8_t tbuf[1];

    tbuf[0] = cmd;
    return __qspi0_write(tbuf, 1);
}

/**
 * @brief The read flash status reg1
 *
 * @param rbuf holds the read state
 * @return read process status
 */
uint8_t __attribute__((noinline)) __qspi0_read_flashstatus1(uint8_t *rbuf)
{
    uint8_t tbuf[1];

    tbuf[0] = W25X_READ_STATUS1;
    return __qspi0_readwrite(tbuf, 1, rbuf, 1);
}

/**
 * @brief The read flash status reg2
 *
 * @param rbuf holds the read state
 * @return read process status
 */
uint8_t __attribute__((noinline)) __qspi0_read_flashstatus2(uint8_t *rbuf)
{
    uint8_t tbuf[1];

    tbuf[0] = W25X_READ_STATUS2;
    return __qspi0_readwrite(tbuf, 1, rbuf, 1);
}

/**
 * @brief qspi0 opreta flash waitfinish
 *
 * @return qspi0 control flash status
 */
uint8_t __attribute__((noinline)) __qspi0_wait_flashfinish(void)
{
    uint32_t timeout = 0;
    uint8_t status1 = 1;
    while(status1 & 1)
    {
        __qspi0_read_flashstatus1(&status1);

        if(++timeout > QSPI_BUSYTIMEOUT)
        {
            return ERROR;
        }
    }
    return SUCCESS;
}
/**
 * @brief The write flash status reg
 * @param reg Written address
 * @return qspi0 opreta flash waitfinish
 */
uint8_t __attribute__((noinline)) __qspi0_write_flashstatus(uint8_t *tbuf, uint8_t len)
{
    if(ERROR == __qspi0_flashcmd(W25X_WRITE_ENABLE))
        return ERROR;

    if(ERROR == __qspi0_write(tbuf, len))
        return ERROR;
    return __qspi0_wait_flashfinish();
}
/**
 * @brief quad qspi0 enable
 *
 * @param on enable or disable
 * @return The write flash status reg
 */
uint8_t __attribute__((noinline)) __qspi0_flash_quadenable(FunctionalState newstate)
{
    uint8_t tbuf[3] = {0};

    if(ERROR == __qspi0_read_flashstatus1(&tbuf[1]))
        return ERROR;

    if(ERROR == __qspi0_read_flashstatus2(&tbuf[2]))
        return ERROR;

    tbuf[0]=W25X_WRITE_STATUS;
    if(newstate)
        tbuf[2] |= 0x02;
    else
        tbuf[2] &= 0xfd;

    return __qspi0_write_flashstatus(tbuf,3);
}

/**
 * @brief Qspi0 send write flash enable cmd
 *
 * @return The write flash enable status
 */
uint8_t __attribute__((noinline)) __qspi0_flash_writeenable(void)
{
    return __qspi0_flashcmd(W25X_WRITE_ENABLE);
}

/**
 * @brief qspi0 send write flash disable cmd
 *
 * @return The write flash enable status
 */
uint8_t __attribute__((noinline)) __qspi0_flash_writedisable(void)
{
    return __qspi0_flashcmd(W25X_WRITE_DISABLE);
}

/**
 * @brief qspi0 flash instruction and address combination functions
 *
 * @param buf The buff that holds the instruction
 * @param cmd Incoming instruction parameters
 * @param addr The address parameter passed in
 */
void __attribute__((noinline)) __qspi0_flash_precmd(uint8_t *buf, uint8_t cmd, uint32_t addr)
{
    buf[0] = W25X_PAGE_PROGRAM;
    buf[1] = addr >> 16;
    buf[2] = addr >> 8;
    buf[3] = addr;
}

/**
 * @brief Qspi0 flash instruction and address combination functions.
 *
 * @param flash_addr The flash address to be written to
 * @param len The databuf len to be written to
 * @param tbuf The databuf address to be written to
 * @return qspi0 write flash status
 */
uint8_t __attribute__((noinline)) __qspi0_flash_write(uint32_t flash_addr, uint32_t len, uint8_t *tbuf)
{
    uint8_t buf[FLASH_PAGE_SIZE + 4];
    uint32_t packnum = (len + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;
    uint32_t packlen = 0;
    for(uint32_t i = 0; i < packnum; i++)
    {
        packlen = len > FLASH_PAGE_SIZE ? FLASH_PAGE_SIZE : len;
        __qspi0_flash_precmd(buf, W25X_PAGE_PROGRAM, flash_addr + i * FLASH_PAGE_SIZE);

        // for(uint32_t j = 0; j < packlen; j++)
        // {
        //     buf[4 + j] =  tbuf[j + i * FLASH_PAGE_SIZE];
        // }
        __xmemcpy(buf + 4, tbuf + i * FLASH_PAGE_SIZE, packlen);
        if(ERROR == __qspi0_flash_writeenable())
            return ERROR;
        if(ERROR == __qspi0_write(buf, packlen + 4))
            return ERROR;
        if(ERROR == __qspi0_wait_flashfinish())
            return ERROR;
        len -= packlen;
    }

    return SUCCESS;
}

/**
 * @brief Qspi0 read flash.
 *
 * @param mode read flash mode
 * @param flash_addr The flash addr to be read to
 * @param rlen The databuf addr for rlen
 * @param rbuf The databuf addr for read
 * @return qspi0 read flash status
 */
uint8_t __attribute__((noinline)) __qspi0_flash_read(uint32_t ctrl, uint32_t flash_addr, uint32_t rlen, uint8_t *rbuf)
{
    uint8_t ret = 0;
    uint8_t tbuf[5] = {0};
    uint32_t tlen = 5;
    uint8_t mode;
    volatile uint32_t tmp = CORE_QSPI->qspi_ctrl.reg;

    CORE_QSPI->qspi_ctrl.reg = ctrl;

    mode = (ctrl >> 24) & 0xff;
    tbuf[0] = mode;
    tbuf[1] = flash_addr >> 16;
    tbuf[2] = flash_addr >> 8;
    tbuf[3] = flash_addr;
    tbuf[4] = 0;


    __qspi0_dmastart(tbuf, tlen, rbuf, rlen);
    ret = __qspi0_dmawait();

    CORE_QSPI->qspi_ctrl.reg = tmp;
    return ret;
}

uint8_t __attribute__((noinline)) __qspi0_flash_read_xip(uint32_t flash_addr, uint32_t rlen, uint8_t *rbuf)
{
    return __qspi0_flash_read(CORE_QSPI->qspi_ctrl.reg, flash_addr, rlen, rbuf);
}
/**
 * @brief qspi0 send the erase flash instruction.
 *
 * @return qspi0 erase flash status
 */
uint8_t __attribute__((noinline)) __qspi0_flash_chiperase()
{
    if(ERROR == __qspi0_flash_writeenable())
        return ERROR;

    if(ERROR ==  __qspi0_flashcmd(W25X_CHIP_ERASE))
        return ERROR;

    return __qspi0_wait_flashfinish();
}

/**
 * @brief qspi0 send the erase flash instruction.
 *
 * @param cmd erase cmd
 * @param flash_addr The flash addr to be erase to
 * @return qspi0 erase flash status
 */
uint8_t __attribute__((noinline)) __qspi0_flash_erase(uint8_t cmd, uint32_t flash_addr)
{
    uint8_t tbuf[4] = {0};
    uint8_t ret;
    if(ERROR == __qspi0_flash_writeenable())
        return ERROR;

    tbuf[0] = cmd;
    tbuf[1] = flash_addr >> 16;
    tbuf[2] = flash_addr >> 8;
    tbuf[3] = flash_addr;

    if(ERROR == __qspi0_write(tbuf, 4))
        return ERROR;
    ret = __qspi0_wait_flashfinish();
    return ret;
}

/**
 * @brief Qspi0 send the sectorerase flash instruction.
 *
 * @param flash_addr The flash addr to be sectorerase to
 * @return qspi0 sectorerase flash status
 */
uint8_t __attribute__((noinline)) __qspi0_flash_sectorerase(uint32_t flash_addr)
{
    return __qspi0_flash_erase(W25X_SECTOR_ERASE, flash_addr);
}

/**
 * @brief Qspi0 send the blockerase32k flash instruction.
 *
 * @param flash_addr The flash addr to be blockerase32k to
 * @return qspi0 blockerase32k flash status
 */
uint8_t __attribute__((noinline)) __qspi0_flash_blockerase32k(uint32_t flash_addr)
{
    return __qspi0_flash_erase(W25X_BLOCK_ERASE32K, flash_addr);
}

/**
 * @brief Qspi0 send the blockerase64k flash instruction.
 *
 * @param flash_addr The flash addr to be blockerase64k to
 * @return qspi0 blockerase64k flash status
 */
uint8_t __attribute__((noinline)) __qspi0_flash_blockerase64k(uint32_t flash_addr)
{
    return __qspi0_flash_erase(W25X_BLOCK_ERASE64K, flash_addr);
}

/**
 * @brief Qspi0 send the blockerasepage flash instruction.
 *
 * @param flash_addr The flash addr to be blockerasepage to
 * @return qspi0 blockerasepage flash status
 */
uint8_t __attribute__((noinline)) __qspi0_flash_pageerase(uint32_t flash_addr)
{
    return __qspi0_flash_erase(FlashCMD_PageErase, flash_addr);
}

/**
 * @brief qspi0 read flash jedecid.
 *
 * @param cmd read flash cmd
 * @param revice The databuf addr for read buf
 * @return qspi0 read flash jedecid status
 */

uint32_t __attribute__((noinline)) __qspi0_flash_jedecid(void)
{
    uint32_t ret;
     uint8_t t1buf[1];
    uint8_t r1buf[3];
    t1buf[0] = W25X_JEDEC_DEVICEID;
    __qspi0_readwrite(t1buf, 1, r1buf, 3);
    ret = (r1buf[0] << 16) | (r1buf[1] << 8) | (r1buf[2] << 0);

    return ret;
}

uint8_t __qspi0_write_flash(uint32_t flashAddr, uint32_t len, uint8_t *tbuf)
{

    uint8_t buf[FLASH_PAGE_SIZE + 4] = {0};
    uint32_t packlen = 0;
	uint32_t sys_irq_status = 0;
    while(len > 0)
    {
        if((flashAddr&0xff) == 0)
        {
            packlen = len > FLASH_PAGE_SIZE ? FLASH_PAGE_SIZE : len;
        }
        else
        {
            uint16_t remainlen = FLASH_PAGE_SIZE - (flashAddr & 0xff);
            packlen = len > remainlen ? remainlen : len;
        }

        __qspi0_flash_precmd(buf, W25X_PAGE_PROGRAM, flashAddr);

        memcpy(buf + 4, tbuf, packlen);

        sys_irq_status = __disable_irq();
        if(ERROR == __qspi0_flash_writeenable())
        {
            __enable_irq(sys_irq_status);
            return ERROR;
        }
        if(ERROR == (__qspi0_write(buf, packlen + 4)))
        {
            __enable_irq(sys_irq_status);
            return ERROR;
        }
        if(ERROR == (__qspi0_wait_flashfinish()))
        {
            __enable_irq(sys_irq_status);
            return ERROR;
        }
        __enable_irq(sys_irq_status);

        flashAddr += packlen;
        len -= packlen;
        tbuf += packlen;
    }
    return SUCCESS;
}

uint8_t __qspi0_read_flash(uint32_t ctrl, uint32_t flashAddr, uint32_t len, uint8_t *rbuf)
{
    uint8_t ret = 0;
    uint8_t tbuf[5] = {0};
    uint32_t tlen = 5;
    uint8_t mode;
    volatile uint32_t tmp = CORE_QSPI->qspi_ctrl.reg;

    CORE_QSPI->qspi_ctrl.reg = ctrl;

    mode = (ctrl >> 24) & 0xff;
    tbuf[0] = mode;
    tbuf[1] = flashAddr >> 16;
    tbuf[2] = flashAddr >> 8;
    tbuf[3] = flashAddr;
    tbuf[4] = 0;

    switch (mode)
    {
    case W25X_FASTREAD_DUAL1: // enable_cont_read_m_byte = 0
        tlen = 4;             // 1 byte cmd + 3 byte address +   8 bit dummy cycle
        break;

    case W25X_FASTREAD_DUAL2: // enable_cont_read_m_byte= 1  1 byte command + 3byte address + 1 byte continous read Mode + 8 bit dummy cycle
        break;

    case W25X_FASTREAD_QUAD1: // enable_cont_read_m_byte = 0
        tlen = 4;             // 1 byte cmd + 3 byte address +   8 bit dummy cycle
        break;

    case W25X_FASTREAD_QUAD2: // enable_cont_read_m_byte= 1  1 byte command + 3byte address + 1 byte continous read Mode + 4 bit dummy cycle
        break;

    case W25X_FASTREAD_QUAD3:
        break;
    default:
        tlen = 4;
        break;
    }

    __qspi0_dmastart(tbuf, tlen, rbuf, len);
    ret = __qspi0_dmawait();

    CORE_QSPI->qspi_ctrl.reg = tmp;
    return ret;
}
#endif

/**
 * @brief Configures the read mode for the W25X flash memory.
 *
 * This function sets the read mode for the W25X flash memory chip. The read mode
 * determines how data is read from the flash memory, with options for standard,
 * fast, dual, and quad read modes.
 *
 * @param readModeCmd The read mode command to set. This parameter can be one of the following values:
 *                    - W25X_READ_DATA
 *                    - W25X_FASTREAD_DATA
 *                    - W25X_FASTREAD_DUAL1
 *                    - W25X_FASTREAD_DUAL2
 *                    - W25X_FASTREAD_QUAD1
 *                    - W25X_FASTREAD_QUAD2
 *                    - W25X_FASTREAD_QUAD3
 *
 *
 * @retval SUCCESS or ERROR
 */
uint8_t FLASH_SetReadMode(uint8_t readModeCmd)
{
    if( (readModeCmd != W25X_READ_DATA)       && (readModeCmd != W25X_FASTREAD_DATA)    &&
        (readModeCmd != W25X_FASTREAD_DUAL1)  && (readModeCmd != W25X_FASTREAD_DUAL2)   &&
        (readModeCmd != W25X_FASTREAD_QUAD1)  && (readModeCmd != W25X_FASTREAD_QUAD2)   &&
        (readModeCmd != W25X_FASTREAD_QUAD3)  )
    {
        return 0;
    }
    uint8_t result = 1;
    uint32_t sys_irq_status = __disable_irq();
    FunctionalState is_quad_mod = readModeCmd & 0x40 ? ENABLE : DISABLE;
    uint32_t qspi_ctrl = 0;

    #if FLASH_USE_ROM_API
    result = ((uint8_t (*)(FunctionalState newstate, uint8_t reg_len, uint16_t us))ROM_API_QSPI0_FLASH_QUADDEABLE)(is_quad_mod, 2, 0);
    #else
    prefetch(__qspi0_dmastart,FLASH_SetReadMode,DISABLE);
    __qspi0_flash_quadenable(is_quad_mod);
    #endif

    if(ENABLE == is_quad_mod)
    {
        GPIO_Config(LpmRecoverStruct->qspi_param.flash_io2, GPIO_MODE_QSPI_IO2);
        GPIO_Config(LpmRecoverStruct->qspi_param.flash_io3, GPIO_MODE_QSPI_IO3);
    }
    else
    {
        GPIO_Config(LpmRecoverStruct->qspi_param.flash_io2, GPIO_MODE_OUT_HIGH);
        GPIO_Config(LpmRecoverStruct->qspi_param.flash_io3, GPIO_MODE_OUT_HIGH);
    }

    switch(readModeCmd)
    {
        case W25X_READ_DATA:
        {
            qspi_ctrl=0x03400000;
            break;
        }
        case W25X_FASTREAD_DATA:
        {
            qspi_ctrl=0x0B400800;
            break;
        }
        case W25X_FASTREAD_DUAL1:
        {
            qspi_ctrl=0x3B400801;
            break;
        }
        case W25X_FASTREAD_DUAL2:
        {
            qspi_ctrl=0xBB406011;
            // qspi_ctrl=0xBB406801;
            break;
        }
        case W25X_FASTREAD_QUAD1:
        {
            qspi_ctrl=0x6B401802;
            break;
        }
        case W25X_FASTREAD_QUAD2:
        {
            qspi_ctrl=0xEB406412;
            break;
        }
        case W25X_FASTREAD_QUAD3:
        {
            qspi_ctrl=0xE7406212;
            break;
        }
    }
    CORE_QSPI->qspi_ctrl.reg = qspi_ctrl;
    LpmRecoverStruct->qspi_param.qspi_ctrl = qspi_ctrl;

    __enable_irq(sys_irq_status);
    return result;
}

/**
 * @brief write flash.
 * @param flashAddr The flash address to be write
 * @param tlen The write len
 * @param tbuf The data buf address to be write
 * @return SUCCESS or ERROR
 */
uint8_t FLASH_Write(uint32_t flashAddr, uint32_t tlen, uint8_t *tbuf)
{
    uint32_t sys_irq_status = __disable_irq();
#if FLASH_USE_ROM_API
    uint8_t result = ((uint8_t (*)(uint32_t flash_addr, uint32_t len, uint8_t *tbuf, uint16_t us))ROM_API_QSPI0_FLASH_WRITE)(flashAddr,tlen,tbuf,0);
#else
    prefetch(__qspi0_dmastart,FLASH_Write,DISABLE);
    uint8_t result = __qspi0_write_flash(flashAddr, tlen, tbuf);
#endif
    __enable_irq(sys_irq_status);
    return result;
}

/**
 * @brief read flash.
 * @param flashAddr The flash addr to be read
 * @param rlen The read len
 * @param rbuf The data buf addr for read
 * @return SUCCESS or ERROR
 */
uint8_t FLASH_Read(uint32_t flashAddr, uint32_t rlen, uint8_t *rbuf)
{
    uint32_t sys_irq_status = __disable_irq();
    #if FLASH_USE_ROM_API
           uint8_t result = ((uint8_t (*)(uint32_t ctrl, uint32_t flash_addr, uint32_t rlen, uint8_t *rbuf))ROM_API_QSPI0_FLASH_READ)(CORE_QSPI->qspi_ctrl.reg,flashAddr,rlen,rbuf);
    #else
    prefetch(__qspi0_dmastart,FLASH_Read,DISABLE);
    uint8_t result = __qspi0_read_flash(CORE_QSPI->qspi_ctrl.reg,flashAddr, rlen, rbuf);
    #endif
    __enable_irq(sys_irq_status);
    return result;
}

/**
 * @brief lock flash.
 * @param none
 * @return SUCCESS or ERROR
 */
uint8_t FLASH_Lock(void)
{
    uint8_t status_buf[3] = {0};
    uint32_t sys_irq_status = __disable_irq();
    #if FLASH_USE_ROM_API
        ((uint8_t (*)(uint8_t *rbuf))ROM_API_QSPI0_READ_FLASHSTATUS1)((uint8_t *)&status_buf[1]);
        ((uint8_t (*)(uint8_t *rbuf))ROM_API_QSPI0_READ_FLASHSTATUS2)((uint8_t *)&status_buf[2]);
        status_buf[0]=W25X_WRITE_STATUS;
        status_buf[1] |= 0x7c;
        uint8_t result = ((uint8_t (*)(uint8_t *tbuf, uint8_t len, uint16_t us))ROM_API_QSPI0_WRITE_FLASHSTATUS)(status_buf,3,0);
    #else
    prefetch(__qspi0_dmastart,FLASH_Lock,DISABLE);
    __qspi0_read_flashstatus1((uint8_t *)&status_buf[1]);
    __qspi0_read_flashstatus2((uint8_t *)&status_buf[2]);
    status_buf[0]=W25X_WRITE_STATUS;
    status_buf[1] |= 0x7c;
    uint8_t result = __qspi0_write_flashstatus(status_buf,3);
    #endif
    __enable_irq(sys_irq_status);
    return result;
}

/**
 * @brief unlock flash.
 * @param none
 * @return SUCCESS or ERROR
 */
uint8_t FLASH_Unlock(void)
{
    uint8_t status_buf[3] = {0};
    uint32_t sys_irq_status = __disable_irq();
    #if FLASH_USE_ROM_API
    ((uint8_t (*)(uint8_t *rbuf))ROM_API_QSPI0_READ_FLASHSTATUS1)((uint8_t *)&status_buf[1]);
    ((uint8_t (*)(uint8_t *rbuf))ROM_API_QSPI0_READ_FLASHSTATUS2)((uint8_t *)&status_buf[2]);
    status_buf[0]=W25X_WRITE_STATUS;
    status_buf[1] &= (~0x7c);
    uint8_t result = ((uint8_t (*)(uint8_t *tbuf, uint8_t len, uint16_t us))ROM_API_QSPI0_WRITE_FLASHSTATUS)(status_buf,3,0);
    #else
    prefetch(__qspi0_dmastart,FLASH_Lock,DISABLE);
    __qspi0_read_flashstatus1((uint8_t *)&status_buf[1]);
    __qspi0_read_flashstatus2((uint8_t *)&status_buf[2]);
    status_buf[0]=W25X_WRITE_STATUS;
    status_buf[1] &= (~0x7c);
    uint8_t result = __qspi0_write_flashstatus(status_buf,3);
    #endif
    __enable_irq(sys_irq_status);
    return result;
}

/**
 * @brief  page erase (256B)
 * @param  flashAddr
 * @retval ERROR,SUCCESS
 */
uint8_t FLASH_PageErase(uint32_t flashAddr)
{
    uint32_t sys_irq_status = __disable_irq();
    #if FLASH_USE_ROM_API
        uint8_t result = ((uint8_t (*)(uint8_t cmd, uint32_t flash_addr, uint16_t us))ROM_API_QSPI0_FLASH_ERASE)(FlashCMD_PageErase,flashAddr,0);
    #else
    prefetch(__qspi0_dmastart,FLASH_PageErase,DISABLE);
    uint8_t result = __qspi0_flash_pageerase(flashAddr);
    #endif
    __enable_irq(sys_irq_status);
    return result;
}

/**
 * @brief  sector erase (4K)
 * @param  flashAddr
 * @retval ERROR,SUCCESS
 */
uint8_t FLASH_SectorErase(uint32_t flashAddr)
{
    uint32_t sys_irq_status = __disable_irq();
    #if FLASH_USE_ROM_API
        uint8_t result = ((uint8_t (*)(uint8_t cmd, uint32_t flash_addr, uint16_t us))ROM_API_QSPI0_FLASH_ERASE)(W25X_SECTOR_ERASE,flashAddr,0);
    #else
    prefetch(__qspi0_dmastart,FLASH_SectorErase,DISABLE);
    uint8_t result = __qspi0_flash_sectorerase(flashAddr);
    #endif
    __enable_irq(sys_irq_status);
    return result;
}

/**
 * @brief  block erase (32K)
 * @param  flashAddr
 * @retval ERROR,SUCCESS
 */
uint8_t FLASH_BlockErase_32k(uint32_t flashAddr)
{
    uint32_t sys_irq_status = __disable_irq();
    #if FLASH_USE_ROM_API
        uint8_t result = ((uint8_t (*)(uint8_t cmd, uint32_t flash_addr, uint16_t us))ROM_API_QSPI0_FLASH_ERASE)(W25X_BLOCK_ERASE32K,flashAddr,0);
    #else
    prefetch(__qspi0_dmastart,FLASH_BlockErase_32k,DISABLE);
    uint8_t result = __qspi0_flash_blockerase32k(flashAddr);
    #endif
    __enable_irq(sys_irq_status);
    return result;
}

/**
 * @brief  block erase (64K)
 * @param  flashAddr
 * @retval ERROR,SUCCESS
 */
uint8_t FLASH_BlockErase_64k(uint32_t flashAddr)
{
    uint32_t sys_irq_status = __disable_irq();
    #if FLASH_USE_ROM_API
        uint8_t result = ((uint8_t (*)(uint8_t cmd, uint32_t flash_addr, uint16_t us))ROM_API_QSPI0_FLASH_ERASE)(W25X_BLOCK_ERASE64K,flashAddr,0);
    #else
    prefetch(__qspi0_dmastart,FLASH_BlockErase_64k,DISABLE);
    uint8_t result = __qspi0_flash_blockerase64k(flashAddr);
    #endif
    __enable_irq(sys_irq_status);
    return result;
}

/**
 * @brief powerdown flash.
 * @param none
 * @return SUCCESS or ERROR
 */
uint8_t FLASH_Powerdown(void)
{
    uint32_t sys_irq_status = __disable_irq();
    #if FLASH_USE_ROM_API
        uint8_t result = ((uint8_t (*)(uint8_t cmd))ROM_API_QSPI0_FLASHCMD)(W25X_POWER_DOWN);
    #else
    prefetch(__qspi0_dmastart,FLASH_Powerdown,DISABLE);
    uint8_t result = __qspi0_flashcmd(W25X_POWER_DOWN);
    #endif
    __enable_irq(sys_irq_status);
    return result;
}

/**
 * @brief get flash jedec id.
 * @param none
 * @return flash jedec id
 */
uint32_t FLASH_GetJedecId(void)
{
    uint32_t sys_irq_status = __disable_irq();
    #if FLASH_USE_ROM_API
        uint32_t ID = ((uint32_t (*)(void))ROM_API_QSPI0_FLASH_JEDECID)();
    #else
    prefetch(__qspi0_dmastart,FLASH_GetJedecId,DISABLE);
    uint32_t ID = __qspi0_flash_jedecid();
    #endif
    __enable_irq(sys_irq_status);
    return ID;
}
/**
 * @brief set flash parameter offset.
 * @param paramOffset: flash parameter offset(MCU addr)
 * @return none
 */
void FLASH_SetParamAddr(uint32_t paramOffset)
{
    CORE_QSPI->para_offset.bit.para_offset = ((paramOffset&0xffffff)>>5);
}

/************************ (C) COPYRIGHT Yichip Microelectronics *****END OF FILE****/

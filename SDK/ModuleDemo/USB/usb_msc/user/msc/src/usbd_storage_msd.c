#include "usbd_msc_mem.h"
#include "yc_flash.h"

#define STORAGE_LUN_NBR                 1           /* 逻辑单元号,只有一个flash,设置为1 */
#define STORAGE_BLK_SIZ                 4096        /* flash扇区大小为4096byte */
#define STORAGE_BLK_NBR                 32          /* 4096*32=128k*/
#define STORAGE_BASE_ADDR               0x01080000   /*将内部flash0x1040000-0x1080000(256k)模拟为u盘*/

const int8_t STORAGE_Inquirydata[] = {  // 36

  /* LUN 0 */
  0x00,
  0x80,
  0x02,
  0x02,
  (USBD_STD_INQUIRY_LENGTH - 5),
  0x00,
  0x00,
  0x00,
  'Y', 'i', 'C', 'h', 'i', 'p', ' ', ' ', /* Manufacturer : 8 bytes */
  'm', 'i', 'c', 'r', 'o', 'S', 'D', ' ', /* Product : 16 Bytes */
  'F', 'l', 'a', 's', 'h', ' ', ' ', ' ',
  '1', '.', '0', '0',           /* Version : 4 Bytes */
};

int8_t STORAGE_Init(uint8_t lun);

int8_t STORAGE_GetCapacity(uint8_t lun,
                           uint32_t * block_num, uint32_t * block_size);

int8_t STORAGE_IsReady(uint8_t lun);

int8_t STORAGE_IsWriteProtected(uint8_t lun);

int8_t STORAGE_Read(uint8_t lun,
                    uint8_t * buf, uint32_t blk_addr, uint16_t blk_len);

int8_t STORAGE_Write(uint8_t lun,
                     uint8_t * buf, uint32_t blk_addr, uint16_t blk_len);

int8_t STORAGE_GetMaxLun(void);


USBD_STORAGE_cb_TypeDef USBD_MICRO_SDIO_fops = {
  STORAGE_Init,
  STORAGE_GetCapacity,
  STORAGE_IsReady,
  STORAGE_IsWriteProtected,
  STORAGE_Read,
  STORAGE_Write,
  STORAGE_GetMaxLun,
  (int8_t *) STORAGE_Inquirydata,
};

USBD_STORAGE_cb_TypeDef *USBD_STORAGE_fops = &USBD_MICRO_SDIO_fops;

/**
  * @brief  Initialize the storage medium
  * @param  lun : logical unit number
  * @retval Status
  */

int8_t STORAGE_Init(uint8_t lun)
{
    /*Initialize the storage media communication interface */

  return (0);

}

/**
  * @brief  return medium capacity and block size
  * @param  lun : logical unit number
  * @param  block_num :  number of physical block
  * @param  block_size : size of a physical block
  * @retval Status
  */
int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t * block_num,
                           uint32_t * block_size)
{
    *block_num=STORAGE_BLK_NBR;
    *block_size=STORAGE_BLK_SIZ;
  return (0);

}

/**
  * @brief  check whether the medium is ready
  * @param  lun : logical unit number
  * @retval Status
  */
int8_t STORAGE_IsReady(uint8_t lun)
{
    /*Check the storage media is ready*/
    return (0);
}

/**
  * @brief  check whether the medium is write-protected
  * @param  lun : logical unit number
  * @retval Status
  */
int8_t STORAGE_IsWriteProtected(uint8_t lun)
{
  return 0;
}

/**
  * @brief  Read data from the medium
  * @param  lun : logical unit number
  * @param  buf : Pointer to the buffer to save data
  * @param  blk_addr :  address of 1st block to be read
  * @param  blk_len : nmber of blocks to be read
  * @retval Status
  */
int8_t STORAGE_Read(uint8_t lun,
                    uint8_t * buf, uint32_t blk_addr, uint16_t blk_len)
{
    /* 因为传进来的参数是扇区的地址和扇区的大小，在进行读的时候要转换成字节地址和字节大小 */
    if(SUCCESS==FLASH_Read(STORAGE_BASE_ADDR+blk_addr*STORAGE_BLK_SIZ,blk_len*STORAGE_BLK_SIZ,buf))
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

/**
  * @brief  Write data to the medium
  * @param  lun : logical unit number
  * @param  buf : Pointer to the buffer to write from
  * @param  blk_addr :  address of 1st block to be written
  * @param  blk_len : nmber of blocks to be read
  * @retval Status
  */
int8_t STORAGE_Write(uint8_t lun,
                     uint8_t * buf, uint32_t blk_addr, uint16_t blk_len)
{
    /* 因为传进来的参数是扇区的地址和扇区的大小，在进行写的时候要转换成字节地址和字节大小,flash写之前要擦除 */
    if(SUCCESS==FLASH_SectorErase(STORAGE_BASE_ADDR+blk_addr*STORAGE_BLK_SIZ))
    {
        if(SUCCESS==FLASH_Write(STORAGE_BASE_ADDR+blk_addr*STORAGE_BLK_SIZ,blk_len*STORAGE_BLK_SIZ,buf))
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}

/**
  * @brief  Return number of supported logical unit
  * @param  None
  * @retval number of logical unit
  */

int8_t STORAGE_GetMaxLun(void)
{
  return (STORAGE_LUN_NBR - 1);
}

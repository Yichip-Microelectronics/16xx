#include "yc_charge.h"
#include "board_config.h"
#include "yc_plic.h"
#include "yc_otp.h"

#define OTP_CHGR_CUT_OFF_VOL_ADDR       (0x7d3)

int main(void)
{
    int8_t chgr_cut_off_vol = 0;

    SYSCTRL_ChipInit();
    printport_init();
    yc_log("CHGR interrupt demo");
    OTP_Read(OTP_CHGR_CUT_OFF_VOL_ADDR,(uint8_t*)&chgr_cut_off_vol,1);/*读取otp中的截止电压校准值*/
    if((chgr_cut_off_vol&0x80) == 0)
    {
        chgr_cut_off_vol = 8;
    }
    else
    {
        chgr_cut_off_vol = (chgr_cut_off_vol&0x3f);
    }
    yc_log("chgr_cut_off_vol=%d",chgr_cut_off_vol);

    CHARGE_VolSel(chgr_cut_off_vol,chgr_cut_off_vol-2);
    CHARGE_CurSel(CHARGE_CURRENT_100MA,2,1);
    CHARGE_SetIrqMode(CHGR_ALL_IRQ,CHGR_IRQ_MODE_1);
    CHARGE_SetIrq(CHGR_ALL_IRQ,ENABLE);
    PLIC_EnableIRQ(CHGR_IRQn);
    PLIC_SetPriority(CHGR_IRQn,2);

    while(1)
    {
        if(CHARGE_InsertDet())
        {
            yc_log("usb insert");
        }
        else
        {
            yc_log("usb not insert");
        }
        if(CHARGE_State())
        {
            yc_log("charging");
        }
        else
        {
            yc_log("not charge");
        }
        delay_ms(500);
    }
}


void CHGR_IRQHandler(void)
{
    uint8_t status = CHARGE_GetIrqStatus(CHGR_ALL_IRQ);
    uint8_t mode = CHARGE_GetIrqMode();
    yc_log("status=0x%02x,mode=0x%02x",status,mode);
    if(status&CHGR_IN_DET_AON)
    {
        if(mode&CHGR_IN_DET_AON)
        {
            yc_log("CHGR_IN_DET_AON 1");
            CHARGE_SetIrqMode(CHGR_IN_DET_AON,CHGR_IRQ_MODE_0);
        }
        else
        {
            yc_log("CHGR_IN_DET_AON 0");
            CHARGE_SetIrqMode(CHGR_IN_DET_AON,CHGR_IRQ_MODE_1);
        }
    }
    if(status&CHGR_STATE_IND)
    {
        if(mode&CHGR_STATE_IND)
        {
            yc_log("CHGR_STATE_IND 1");
            CHARGE_SetIrqMode(CHGR_STATE_IND,CHGR_IRQ_MODE_0);
        }
        else
        {
            yc_log("CHGR_STATE_IND 0");
            CHARGE_SetIrqMode(CHGR_STATE_IND,CHGR_IRQ_MODE_1);
        }
    }
    if(status&CHGR_UVLO_OK_AON)
    {
        if(mode&CHGR_UVLO_OK_AON)
        {
            yc_log("CHGR_UVLO_OK_AON 1");
            CHARGE_SetIrqMode(CHGR_UVLO_OK_AON,CHGR_IRQ_MODE_0);
        }
        else
        {
            yc_log("CHGR_UVLO_OK_AON 0");
            CHARGE_SetIrqMode(CHGR_UVLO_OK_AON,CHGR_IRQ_MODE_1);
        }
    }
    if(status&CHGR_PGOOD)
    {
        if(mode&CHGR_PGOOD)
        {
            yc_log("CHGR_PGOOD 1");
            CHARGE_SetIrqMode(CHGR_PGOOD,CHGR_IRQ_MODE_0);
        }
        else
        {
            yc_log("CHGR_PGOOD 0");
            CHARGE_SetIrqMode(CHGR_PGOOD,CHGR_IRQ_MODE_1);
        }
    }
}
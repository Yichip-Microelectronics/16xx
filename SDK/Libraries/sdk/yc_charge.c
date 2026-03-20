/*
File Name    : yc_charge.c
Author       : Yichip
Version      : V1.0
Date         : 2024/5/11
Description  : charge driver interface implement.
*/

#include "yc_charge.h"

/**
 * @brief   Configure the charge cut-off voltage and recharger voltage.
 * @param   CutOffVol :Configured with the cut-off voltage.
 *                    and the step length is 53mV.
 * @param   RechgrVol :Configured with the recharger voltage.
 *                    and the step length is 53mV
 * @retval  none
 */
void CHARGE_VolSel(int8_t CutOffVol, int8_t RechgrVol)
{
    RechgrVol = ((RechgrVol < 0) ? 0 : RechgrVol);

    LPM_HW_CONTROL_NOEXP->rg_lpm_chgr_shutdown_sw.bit.rg_lpm_chgr_cvc = CutOffVol;
    LPM_HW_CONTROL_NOEXP->rg_lpm_chgr_shutdown_sw.bit.rg_lpm_chgr_rchc = RechgrVol;
}

/**
 * @brief   Config charge current、TricCurtSel、CutOffCurtSel
 * @param   ChargCurtSel :the parameter will be enum CHARGE_CurrentEnum
 * @param   TricCurtSel :Trickle current:
                            0:10% of the ChargCurtSel(default)
                            1:20% of the ChargCurtSel
                            2:30% of the ChargCurtSel
                            3:40% of the ChargCurtSel
 * @param   CutOffCurtSel :Cutoff current:
                            0:10% of the ChargCurtSel(default)
                            1:20% of the ChargCurtSel
                            2:30% of the ChargCurtSel
                            3:40% of the ChargCurtSel
            Stop charging when the current charging current
            is less than or equal to set of the configured current.
 * @retval  none
 */
void CHARGE_CurSel(CHARGE_CurrentEnum ChargCurtSel, uint8_t TricCurtSel, uint8_t CutOffCurtSel)
{
    LPM_HW_CONTROL_NOEXP->rg_lpm_chgr_shutdown_sw.bit.rg_lpm_chgr_ichg_set = ChargCurtSel;
	LPM_HW_CONTROL_NOEXP->rg_lpm_chgr_shutdown_sw.bit.rg_lpm_chgr_tcc = TricCurtSel;
    LPM_HW_CONTROL_NOEXP->rg_lpm_chgr_shutdown_sw.bit.rg_lpm_chgr_termc = CutOffCurtSel;
}

/**
 * @brief   CHARGE CC_Mode and CV_mode ctrl
 * @param   ChargCcMod:(Constant current)
                        0:NORMAL(default)
                        1:FORCE
 * @param   ChargCvMod:(Constant voltage)
                        0:NORMAL(default)
                        1:FORCE
            If the value set to 1,
            the precharging function will cancelled
 * @retval  Success return true ,otherwise return false
 */
bool CHARGE_CcCvCtrl(uint8_t ChargCcMod, uint8_t ChargCvMod)
{
	if(ChargCcMod && ChargCvMod)
    {
        return false;
    }

	if(ChargCcMod)
	{
		LPM_HW_CONTROL_NOEXP->rg_lpm_chgr_shutdown_sw.bit.rg_lpm_chgr_force_cc = 1;
	}
	else
    {
		LPM_HW_CONTROL_NOEXP->rg_lpm_chgr_shutdown_sw.bit.rg_lpm_chgr_force_cc = 0;
	}

	if(ChargCvMod)
	{
		LPM_HW_CONTROL_NOEXP->rg_lpm_chgr_shutdown_sw.bit.rg_lpm_chgr_force_cv = 1;
	}
	else
    {
		LPM_HW_CONTROL_NOEXP->rg_lpm_chgr_shutdown_sw.bit.rg_lpm_chgr_force_cv = 0;
    }
	return true;
}

/**
 * @brief	System Power Select when usb insert
 * @param	powerSource:POWER_CHGR_IN or POWER_VBAT
 * @retval 	none
 */
void CHARGE_SystemPowerSelect(CHARGE_SysPowerEnum powerSource)
{
    if(powerSource == POWER_VBAT)
    {
        LPM_HW_CONTROL_NOEXP->rg_lpm_chgr_shutdown_sw.bit.rg_lpm_chgr_shutdown_sw = 1;
    }
    else
    {
        LPM_HW_CONTROL_NOEXP->rg_lpm_chgr_shutdown_sw.bit.rg_lpm_chgr_shutdown_sw = 0;
    }
}

/**
 * @brief	charger module enable or disable
 * @param	newState:ENABLE or DISABLE
 * @retval 	none
 */
void CHARGE_Cmd(FunctionalState newState)
{
    if (newState)
    {
        LPM_HW_CONTROL_NOEXP->rg_lpm_chgr_shutdown_sw.bit.rg_lpm_chgr_shutdown_core = 0;
    }
    else
    {
        LPM_HW_CONTROL_NOEXP->rg_lpm_chgr_shutdown_sw.bit.rg_lpm_chgr_shutdown_core = 1;
    }
}

/**
 * @brief	USB insertion detection
 * @param	none
 * @retval 	Insert is true, otherwise false
 */
bool CHARGE_InsertDet(void)
{
    return (bool)LPM_HW_CONTROL_NOEXP->ad_lpm_chgr_pgood.bit.ad_lpm_chgr_in_det_aon;
}


/**
 * @brief	Charge status detection
 * @param	none
 * @retval 	Charging is true, otherwise it is false
 */
bool CHARGE_State(void)
{
    if(((LPM_HW_CONTROL_NOEXP->ad_lpm_chgr_pgood.bit.ad_lpm_chgr_state_ichg != 0) || (LPM_HW_CONTROL_NOEXP->ad_lpm_chgr_pgood.bit.ad_lpm_chgr_state_vbat_lv != 0))
        && (LPM_HW_CONTROL_NOEXP->ad_lpm_chgr_pgood.bit.ad_lpm_chgr_uvlo_ok_aon != 0)
        && (LPM_HW_CONTROL_NOEXP->ad_lpm_chgr_pgood.bit.ad_lpm_chgr_pgood != 0)
        && (LPM_HW_CONTROL_NOEXP->ad_lpm_chgr_pgood.bit.ad_lpm_chgr_in_det_aon != 0))
    {
        return true;
    }
    else
    {
        return false;
    }

}

/**
 * @brief	set Charge irq enable or disable
 * @param	chgr_intrtype:CHARGE_IrqEnum
 * @param	newState:ENABLE or DISABLE
 * @retval 	none
 */
void CHARGE_SetIrq(CHARGE_IrqEnum chgr_intrtype, FunctionalState newState)
{
    if (newState)
    {
        CORE_CHGR->chgr_intr_en.reg |= chgr_intrtype;
    }
    else
    {
         CORE_CHGR->chgr_intr_en.reg &= ~(chgr_intrtype);
    }
}

/**
 * @brief	set Charge irq mode
 * @param	chgr_intrtype:CHARGE_IrqEnum
 * @param	mode:CHGR_IRQ_MODE_0 or CHGR_IRQ_MODE_1
 * @retval 	none
 */
void CHARGE_SetIrqMode(CHARGE_IrqEnum chgr_intrtype, CHARGE_IrqModeEnum mode)
{
     if (mode == CHGR_IRQ_MODE_0)
    {
        CORE_CHGR->chgr_intr_mod.reg |= chgr_intrtype;
    }
    else
    {
         CORE_CHGR->chgr_intr_mod.reg &= ~(chgr_intrtype);
    }
}

/**
 * @brief	get Charge irq status
 * @param	chgr_intrtype:CHARGE_IrqEnum
 * @retval 	irq status
 */
uint8_t CHARGE_GetIrqStatus(CHARGE_IrqEnum chgr_intrtype)
{
    uint8_t status = 0;
    switch (chgr_intrtype)
    {
    case CHGR_IN_DET_AON:
        status = LPM_HW_CONTROL_NOEXP->ad_lpm_chgr_pgood.bit.ad_lpm_chgr_in_det_aon;
        break;
    case CHGR_STATE_IND:
        status = LPM_HW_CONTROL_NOEXP->ad_lpm_chgr_pgood.bit.ad_lpm_chgr_state_ind<<1;
        break;
    case CHGR_UVLO_OK_AON:
        status = LPM_HW_CONTROL_NOEXP->ad_lpm_chgr_pgood.bit.ad_lpm_chgr_uvlo_ok_aon<<2;
        break;
    case CHGR_PGOOD:
        status = LPM_HW_CONTROL_NOEXP->ad_lpm_chgr_pgood.bit.ad_lpm_chgr_pgood<<3;
        break;
    case CHGR_ALL_IRQ:
        status = LPM_HW_CONTROL_NOEXP->ad_lpm_chgr_pgood.bit.ad_lpm_chgr_in_det_aon | \
                (LPM_HW_CONTROL_NOEXP->ad_lpm_chgr_pgood.bit.ad_lpm_chgr_state_ind<<1) | \
                (LPM_HW_CONTROL_NOEXP->ad_lpm_chgr_pgood.bit.ad_lpm_chgr_uvlo_ok_aon<<2) | \
                (LPM_HW_CONTROL_NOEXP->ad_lpm_chgr_pgood.bit.ad_lpm_chgr_pgood<<3);
                break;
    default:
        break;
    }
    status ^= CORE_CHGR->chgr_intr_mod.reg;
    status &= CORE_CHGR->chgr_intr_en.reg;
    return (status&0x0f);
}

/**
 * @brief	get Charge irq mode
 * @param	none
 * @retval 	irq mode(bit==1 is CHGR_IRQ_MODE_1,bit==0 is CHGR_IRQ_MODE_0)
 */
uint8_t CHARGE_GetIrqMode(void)
{
    return ((~CORE_CHGR->chgr_intr_mod.reg)&0x0f);
}

/*
File Name    : yc_charge.h
Author       : Yichip
Version      : V1.0
Date         : 2024/5/11
Description  : charge driver interface define.
*/

#ifndef _yc_CHARGE_H_
#define _yc_CHARGE_H_

#include "board_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief charge irq enum
 *
 */
typedef enum
{
    CHGR_IN_DET_AON     = 0x1,
    CHGR_STATE_IND      = 0x2,
    CHGR_UVLO_OK_AON    = 0x4,
    CHGR_PGOOD          = 0x8,
    CHGR_ALL_IRQ        = 0xf

}CHARGE_IrqEnum;

/**
 * @brief charge irq mode enum
 *
 */
typedef enum
{
    CHGR_IRQ_MODE_1,
    CHGR_IRQ_MODE_0
}CHARGE_IrqModeEnum;

/**
 * @brief systerm power source enum
 *
 */
typedef enum
{
    POWER_CHGR_IN,
    POWER_VBAT
}CHARGE_SysPowerEnum;

/**
 * @brief Charger current enum
 *
 */
typedef enum
{
    CHARGE_CURRENT_30MA,
    CHARGE_CURRENT_40MA,
    CHARGE_CURRENT_50MA,
    CHARGE_CURRENT_60MA,
    CHARGE_CURRENT_70MA,
    CHARGE_CURRENT_80MA,
    CHARGE_CURRENT_90MA,
    CHARGE_CURRENT_100MA,
    CHARGE_CURRENT_110MA,
    CHARGE_CURRENT_120MA,
    CHARGE_CURRENT_130MA,
    CHARGE_CURRENT_140MA,
    CHARGE_CURRENT_150MA,
    CHARGE_CURRENT_160MA,
    CHARGE_CURRENT_170MA,
    CHARGE_CURRENT_180MA
}CHARGE_CurrentEnum;

void CHARGE_VolSel(int8_t CutOffVol, int8_t RechgrVol);
void CHARGE_CurSel(CHARGE_CurrentEnum ChargCurtSel, uint8_t TricCurtSel, uint8_t CutOffCurtSel);
bool CHARGE_CcCvCtrl(uint8_t ChargCcMod, uint8_t ChargCvMod);
void CHARGE_SystemPowerSelect(CHARGE_SysPowerEnum powerSource);
void CHARGE_Cmd(FunctionalState newState);
bool CHARGE_InsertDet(void);
bool CHARGE_State(void);
uint8_t CHARGE_GetIrqStatus(CHARGE_IrqEnum chgr_intrtype);
void CHARGE_SetIrq(CHARGE_IrqEnum chgr_intrtype, FunctionalState newState);
void CHARGE_SetIrqMode(CHARGE_IrqEnum chgr_intrtype, CHARGE_IrqModeEnum mode);
uint8_t CHARGE_GetIrqMode(void);
#ifdef __cplusplus
}
#endif

#endif /* _yc_CHARGE_H_ */

/************************ (C) COPYRIGHT Yichip Microelectronics *****END OF FILE****/

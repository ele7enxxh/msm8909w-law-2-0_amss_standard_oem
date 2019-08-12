/*=============================================================================
						P M I C   D R I V E R   T A R G E T 	F I L E 

GENERAL DESCRIPTION     
  PMIC driver target specific functions
  
  

	Copyright 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdebug/pmic/src/target/msm8916/sysdbg_pmic_target.c#1 $

when       who          what, where, why
--------   ---          -------------------------------------------------- 
01/29/14    ntonupun    Ported to Bear
03/10/12	AJCheriyan	First cut for Aragorn. 

===========================================================================*/

#include "sysdbg_pmic.h"
#include "sysdbg_pmic_regs.h"

/*===========================================================================
**  Function : PM_PS_HOLD_CONFIG
** ==========================================================================
*/
/*!
* 
* @brief
*	Target specific initialization needed to configure PS_HOLD to do warm reset
*
* @param[in] 
*	None
*  
* @par Dependencies
*	None
* 
* @retval
*   None
* 
* @par Side Effects
*	None
*
*/
void PMPSHOLD_Config(void)
{
	
	/* For 8916 PS_HOLD rset has been configured by default to do warm reset, hence commenting out below code */
	//uint32 data;
	//PONRegType ponreg;

	/* write 0x1 to PS_HOLD_RESET_CTL register for PMIC to do warm reset after PS_HOLD pulled low */
	//data=0x1;
	//ponreg = PMIC_CONCAT(PON, PS_HOLD_RESET_CTL);
	//PM_WritePerphData(PONA, ponreg, 1, &data);


}

/*===========================================================================
**  Function : PM_TARGET_WDOG_INIT
** ==========================================================================
*/
/*!
* 
* @brief
*	Target specific initialization needed for the PMIC watchdog
*
* @param[in] 
*	Bark Time, Bite time
*  
* @par Dependencies
*	None
* 
* @retval
*   None
* 
* @par Side Effects
*	None
*
*/
void PM_target_wdog_init(uint32 bark_time, uint32 bite_time)
{
	uint32 data = 0;

	// Set the bark time
	data =  MAX(bark_time - 1, 127);
	PM_WritePerphData(PONA, PMIC_CONCAT(PON, PMIC_WD_RESET_S1_TIMER), 1, &data);

	// Now set the bite time
	data = MAX(bite_time - 1, 127);
	PM_WritePerphData(PONA, PMIC_CONCAT(PON, PMIC_WD_RESET_S2_TIMER), 1, &data);

	// Now set the PMIC action to be taken on expiration - warm reset
	data = 1;
	PM_WritePerphData(PONA, PMIC_CONCAT(PON, PMIC_WD_RESET_S2_CTL), 1, &data);

	// Pet the Watchdog once
	PM_WritePerphData(PONA, PMIC_CONCAT(PON, PMIC_WD_RESET_PET), 1, &data);

	// Now enable the watchdog
	data = 1 << 7;
	PM_WritePerphData(PONA, PMIC_CONCAT(PON, PMIC_WD_RESET_S2_CTL2), 1, &data);
}

/*===========================================================================
**  Function : PMREGULATOR_INIT
** ==========================================================================
*/
/*!
* 
* @brief
*	Regulator related setup to be done by the PMIC initialization function
*
* @param[in] 
*	None
*  
* @par Dependencies
*	None
* 
* @retval
*   None
* 
* @par Side Effects
*   This function will prevent the respective regulators from returning to their
*   default state on a reset
*
*
*/

void PMRegulator_Init(void)
{
	uint32 data;
	LDODIGRegType ldoreg;
	LDOSTEPPERDIGRegType ldosreg;
	HFBUCK2CTRLRegType hfbuckreg;
	FTS2CTRLRegType ftsctrlreg;
	
	// We want to enable Bit 0 in the registers. Bits 7:1 are reserved.
	// 0 : ENABLE_PRESET
	data = 0x1;
	
	// LDO3 = Mx rail
	// Get secure access first
	data = 0xA5;
	ldosreg = PMIC_CONCAT(LDO_STEPPER_DIG, SEC_ACCESS);
	PM_WritePerphData(LDO3A, ldosreg, 1, &data);

	data = 0x1;
	ldosreg = PMIC_CONCAT(LDO_STEPPER_DIG, PERPH_RESET_CTL1);
	PM_WritePerphData(LDO3A, ldosreg, 1, &data);

	// LDO2 = DDR rail
	// Get secure access first
    data = 0xA5;
    ldosreg = PMIC_CONCAT(LDO_STEPPER_DIG, SEC_ACCESS);
    PM_WritePerphData(LDO2A, ldosreg, 1, &data);

    data = 0x1;
    ldosreg = PMIC_CONCAT(LDO_STEPPER_DIG, PERPH_RESET_CTL1);
    PM_WritePerphData(LDO2A, ldosreg, 1, &data);

	// S1A - Cx rail
	data = 0xA5;
	ftsctrlreg = PMIC_CONCAT(FTS2_CTRL, SEC_ACCESS);
   	PM_WritePerphData(S1A_CTRL, ftsctrlreg, 1, &data);

	data = 0x1;
	ftsctrlreg = PMIC_CONCAT(FTS2_CTRL, PERPH_RESET_CTL1);
	PM_WritePerphData(S1A_CTRL, ftsctrlreg, 1, &data);
	
	// S4A, LD06 - DDR memory Rail
	data = 0xA5;
	hfbuckreg = PMIC_CONCAT(HFBUCK2_CTRL, SEC_ACCESS);
	PM_WritePerphData(S4A_CTRL, hfbuckreg, 1, &data);

	data = 0x1;
	hfbuckreg = PMIC_CONCAT(HFBUCK2_CTRL, PERPH_RESET_CTL1);
	PM_WritePerphData(S4A_CTRL, hfbuckreg, 1, &data);
	
	// LDO 6A
	data = 0xA5;
	ldoreg = PMIC_CONCAT(LDO_DIG, SEC_ACCESS);
	PM_WritePerphData(LDO6A, ldoreg, 1, &data);

	data = 0x1;
	ldoreg = PMIC_CONCAT(LDO_DIG, PERPH_RESET_CTL1);
	PM_WritePerphData(LDO6A, ldoreg, 1, &data);

	// LDO10 powers the XO
	data = 0xA5;
	ldoreg = PMIC_CONCAT(LDO_DIG, SEC_ACCESS);
	PM_WritePerphData(LDO10A, ldoreg, 1, &data);

	data = 0x1;
	ldoreg = PMIC_CONCAT(LDO_DIG, PERPH_RESET_CTL1);
	PM_WritePerphData(LDO10A, ldoreg, 1, &data);

}
	





	
		



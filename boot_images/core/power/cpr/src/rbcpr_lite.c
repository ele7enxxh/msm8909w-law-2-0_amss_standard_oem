/*===========================================================================
rbcpr_lite.c

SERVICES:

DESCRIPTION:

INITIALIZATION AND SEQUENCING REQUIREMENTS:
Description...

Copyright (c) 2014 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "HALhwio.h"

#include <stdbool.h>
#include "CoreVerify.h"
#include "pm_ldo.h"
#include "pm_smps.h"
#include "rbcpr_fuses.h"

//===========================================================================
//                           Function Definitions
//===========================================================================

#define MDM_9x45_PMIC_STEP_SIZE 12500
#define MDM_9x45_FUSE_STEP_SIZE 10000
#define MDM_9x45_TURBO_VOLTAGE_CEIL 1112500
#define MDM_9x45_LOGICAL_TURBO_FLOOR 937500
#define MDM_9x45_MEMORY_TURBO_FLOOR 1000000

void rbcpr_set_cx_mx_to_safe_turbo_voltage(void)
{
	int16 mx_turbo_fuse_value;
	int16 cx_turbo_fuse_value;
	int16 mss_turbo_fuse_value;

    unsigned mx_turbo_apply_value;
    unsigned cx_turbo_apply_value;

	// Initialize fuses
	rbcpr_fuses_init();

	// Read fuses for cx,mx, and mss
	mx_turbo_fuse_value = rbcpr_fuses_get_fuse_value("vddmx", RBCPR_FUSE_TURBO);
	cx_turbo_fuse_value = rbcpr_fuses_get_fuse_value("vddcx", RBCPR_FUSE_TURBO);
	mss_turbo_fuse_value = rbcpr_fuses_get_fuse_value("vddmss", RBCPR_FUSE_TURBO);

	//round mx to max (assuming all step sizes are the same which they currently are)
	mx_turbo_fuse_value = MAX(mx_turbo_fuse_value, MAX(cx_turbo_fuse_value, mss_turbo_fuse_value));

	// Convert fuse value to open loop value
    mx_turbo_apply_value = MDM_9x45_TURBO_VOLTAGE_CEIL + (mx_turbo_fuse_value * MDM_9x45_FUSE_STEP_SIZE);
    cx_turbo_apply_value = MDM_9x45_TURBO_VOLTAGE_CEIL + (cx_turbo_fuse_value * MDM_9x45_FUSE_STEP_SIZE);

	//apply ceil/floor to mx/cx
    mx_turbo_apply_value = MIN(MDM_9x45_TURBO_VOLTAGE_CEIL, MAX(mx_turbo_apply_value, MDM_9x45_MEMORY_TURBO_FLOOR));
    cx_turbo_apply_value = MIN(MDM_9x45_TURBO_VOLTAGE_CEIL, MAX(cx_turbo_apply_value, MDM_9x45_LOGICAL_TURBO_FLOOR));

	// Round to PMIC step size
    if (mx_turbo_apply_value % MDM_9x45_PMIC_STEP_SIZE)
	{
        mx_turbo_apply_value += (MDM_9x45_PMIC_STEP_SIZE - (mx_turbo_apply_value % MDM_9x45_PMIC_STEP_SIZE));
	}
    CORE_VERIFY(mx_turbo_apply_value % MDM_9x45_PMIC_STEP_SIZE == 0);

    if (cx_turbo_apply_value % MDM_9x45_PMIC_STEP_SIZE)
	{
        cx_turbo_apply_value += (MDM_9x45_PMIC_STEP_SIZE - (cx_turbo_apply_value % MDM_9x45_PMIC_STEP_SIZE));
	}
    CORE_VERIFY(cx_turbo_apply_value % MDM_9x45_PMIC_STEP_SIZE == 0);

	//set mx/cx
	//Transition Cx first. It might be going up or down but shouldn't go higher than Mx default of 1.1125V
    //Now transition Mx
    pm_ldo_volt_level(0 /*PMIC CHIP 0*/, PM_LDO_9, mx_turbo_apply_value);
    pm_smps_volt_level(0 /*PMIC CHIP 0*/, PM_SMPS_5, cx_turbo_apply_value);
}

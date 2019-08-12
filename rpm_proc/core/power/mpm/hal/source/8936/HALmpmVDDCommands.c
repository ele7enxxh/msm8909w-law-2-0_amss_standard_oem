/*
===========================================================================

FILE:         HALmpmVDDCommands.c

DESCRIPTION:
  This is the platform hardware abstraction layer implementation for the
  MPM VDD commands.
  This platform is for the RPM on 8936.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/mpm/hal/source/8936/HALmpmVDDCommands.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
02/21/14   sv      Used generic api to obtain vset value for regulators. (CR-549436)
===========================================================================
             Copyright (c) 2013 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
              QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include <stdlib.h>
#include "CoreVerify.h"
#include "HALhwio.h"
#include "mpm2_hwio.h"
#include "HALmpm.h"
#include "pmapp_npa.h"
#include "pm_smps.h"
#include "pm_version.h"

#include "../common/HALmpmInternal.h"

extern HAL_mpm_PlatformType mpmPlatform;

//The values for OffMicrovolts for S3/S4 has been taken from sleep sequence values in 8916_HW_SW_Document
static uint32 s3_offMicrovolts=1225000;
static uint32 s4_offMicrovolts=1850000;

/* -----------------------------------------------------------------------
**                           STATIC FUNCTIONS
** ----------------------------------------------------------------------- */

static void HAL_mpm_GetVSet
(
  HAL_mpm_VoltageRailType   rail,
  uint32                    uv,
  uint32                   *vset
)
{
  pm_railway_type_info_type pm_rail = PM_RAILWAY_INVALID;

  switch(rail)
  {
     case HAL_MPM_VOLTAGE_RAIL_VDD_DIG:
       pm_rail = PM_RAILWAY_CX;
       break;

    case HAL_MPM_VOLTAGE_RAIL_VDD_MEM:
      pm_rail = PM_RAILWAY_MX;
      break;

    default:
      abort();
  }

   CORE_VERIFY(PM_ERR_FLAG__SUCCESS == pm_railway_calculate_vset(pm_rail, uv, vset));
}

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

void HAL_mpm_CfgVoltageCommands(uint32_t offMxMicrovolts, uint32_t onMxMicrovolts, 
                                uint32_t offCxMicrovolts, uint32_t onCxMicrovolts)
{
  uint16 delay;
  uint32 vset;
  uint32 regVal =0;
  
  uint32 s3_active_volt_level;
  uint32 s4_active_volt_level;
  uint16 s3_delay=0;
  uint16 s4_delay=0;
  
  pm_device_info_type pmic_device_info;
  pm_get_pmic_info(0, &pmic_device_info);

  if((onCxMicrovolts - offCxMicrovolts) > (onMxMicrovolts - offMxMicrovolts)) 
    delay = HAL_mpm_GetDelayFromMicrovolts(offCxMicrovolts, onCxMicrovolts);
  else
    delay = HAL_mpm_GetDelayFromMicrovolts(offMxMicrovolts, onMxMicrovolts);
	
  /****** OFF COMMANDS ******/

  // VDD_OFF_CFG_1 - Set S2 (Cx) to RETENTION
  HAL_mpm_GetVSet(HAL_MPM_VOLTAGE_RAIL_VDD_DIG, offCxMicrovolts,  &vset);
  // VDD_OFF_CFG_1 - Set S2 (Cx) VOLTAGE_CTL2 VSET
  regVal   = delay        << HWIO_SHFT(MPM2_MPM_PMIC_VDD_OFF_CFG_1, DELAY);
  regVal  |= 0x1             << HWIO_SHFT(MPM2_MPM_PMIC_VDD_OFF_CFG_1, SLAVE_ID);
  regVal  |= 0x1741          << HWIO_SHFT(MPM2_MPM_PMIC_VDD_OFF_CFG_1, SLAVE_ADDR);
  regVal  |= vset            << HWIO_SHFT(MPM2_MPM_PMIC_VDD_OFF_CFG_1, SLAVE_DATA);
  HWIO_OUT(MPM2_MPM_PMIC_VDD_OFF_CFG_1, regVal);
      

  // VDD_OFF_CFG_2 - Set L3 (Mx) to RETENTION
  HAL_mpm_GetVSet(HAL_MPM_VOLTAGE_RAIL_VDD_MEM, offMxMicrovolts, &vset);
  // VDD_OFF_CFG_2 - Set L3 (Mx) VOLTAGE_CTL2 VSET
  regVal   = delay           << HWIO_SHFT(MPM2_MPM_PMIC_VDD_OFF_CFG_2, DELAY);
  regVal  |= 0x1             << HWIO_SHFT(MPM2_MPM_PMIC_VDD_OFF_CFG_2, SLAVE_ID);
  regVal  |= 0x4241          << HWIO_SHFT(MPM2_MPM_PMIC_VDD_OFF_CFG_2, SLAVE_ADDR);
  regVal  |= vset            << HWIO_SHFT(MPM2_MPM_PMIC_VDD_OFF_CFG_2, SLAVE_DATA);
  HWIO_OUT(MPM2_MPM_PMIC_VDD_OFF_CFG_2, regVal);

  // VDD_OFF_CFG_3 - Set S3 (parent of L3)
  //Calculate Vset for S3 OFF Voltage
  pm_smps_calculate_vset(0, 2, s3_offMicrovolts, &vset);
  //get S3 ACTIVE Voltage
  pm_smps_volt_level_status (0, 2, &s3_active_volt_level);
  //Calcualte delay for S3 ramp up/down
  s3_delay = HAL_mpm_GetDelayFromMicrovolts(s3_offMicrovolts, s3_active_volt_level);
	  
  regVal    = s3_delay        << HWIO_SHFT(MPM2_MPM_PMIC_VDD_OFF_CFG_3, DELAY);
  regVal   |= 1               << HWIO_SHFT(MPM2_MPM_PMIC_VDD_OFF_CFG_3, SLAVE_ID);
  regVal   |= 0x1A41          << HWIO_SHFT(MPM2_MPM_PMIC_VDD_OFF_CFG_3, SLAVE_ADDR);
  regVal   |= vset            << HWIO_SHFT(MPM2_MPM_PMIC_VDD_OFF_CFG_3, SLAVE_DATA);
  HWIO_OUT(MPM2_MPM_PMIC_VDD_OFF_CFG_3, regVal);
     
  if (!((1 == pmic_device_info.nPmicAllLayerRevision) && (pmic_device_info.nPmicMetalRevision == 0)))
  { 
    // VDD_OFF_CFG_4 - Set S4
    //Calculate Vset for S4 OFF Voltage
    pm_smps_calculate_vset(0, 3, s4_offMicrovolts, &vset);
	//get S4 ACTIVE Voltage
	pm_smps_volt_level_status (0, 3, &s4_active_volt_level);
	//Calcualte delay for S4 ramp up/down
	s4_delay = HAL_mpm_GetDelayFromMicrovolts(s4_offMicrovolts, s4_active_volt_level);
	
	regVal    = s4_delay           << HWIO_SHFT(MPM2_MPM_PMIC_VDD_OFF_CFG_4, DELAY);
    regVal   |= 1               << HWIO_SHFT(MPM2_MPM_PMIC_VDD_OFF_CFG_4, SLAVE_ID);
    regVal   |= 0x1D41          << HWIO_SHFT(MPM2_MPM_PMIC_VDD_OFF_CFG_4, SLAVE_ADDR);
    regVal   |= vset            << HWIO_SHFT(MPM2_MPM_PMIC_VDD_OFF_CFG_4, SLAVE_DATA);
	HWIO_OUT(MPM2_MPM_PMIC_VDD_OFF_CFG_4, regVal);
  }

  // VDD_OFF_CFG_5 - PBS Trigger (200us)
  regVal  = 0x6             << HWIO_SHFT(MPM2_MPM_PMIC_VDD_OFF_CFG_5, DELAY);
  regVal |= 0               << HWIO_SHFT(MPM2_MPM_PMIC_VDD_OFF_CFG_5, SLAVE_ID);
  regVal |= 0x7542          << HWIO_SHFT(MPM2_MPM_PMIC_VDD_OFF_CFG_5, SLAVE_ADDR);
  regVal |= 0x01            << HWIO_SHFT(MPM2_MPM_PMIC_VDD_OFF_CFG_5, SLAVE_DATA);
  HWIO_OUT(MPM2_MPM_PMIC_VDD_OFF_CFG_5, regVal);
  
  /****** ON COMMANDS ******/
  
  // VDD_ON_CFG_1 - PBS Trigger (200us)
  regVal  = 0x6             << HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_1, DELAY);
  regVal |= 0               << HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_1, SLAVE_ID);
  regVal |= 0x7542          << HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_1, SLAVE_ADDR);
  regVal |= 0x01            << HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_1, SLAVE_DATA);
  HWIO_OUT(MPM2_MPM_PMIC_VDD_ON_CFG_1, regVal);
  
  // VDD_ON_CFG_2 - Set S3 (parent of L3)
  //Calculate Vset for S3 ON Voltage
  pm_smps_calculate_vset(0, 2, s3_active_volt_level, &vset);
  regVal    = s3_delay           << HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_2, DELAY);
  regVal   |= 1               << HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_2, SLAVE_ID);
  regVal   |= 0x1A41          << HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_2, SLAVE_ADDR);
  regVal   |= vset            << HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_2, SLAVE_DATA);
  HWIO_OUT(MPM2_MPM_PMIC_VDD_ON_CFG_2, regVal);

  // VDD_ON_CFG_3 - Set L3 (Mx) ON
  HAL_mpm_GetVSet(HAL_MPM_VOLTAGE_RAIL_VDD_MEM, onMxMicrovolts, &vset);
  // VDD_ON_CFG_3 - Set L3 (Mx) VOLTAGE_CTL2 VSET
  regVal    = 0               << HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_3, DELAY);
  regVal   |= 0x1             << HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_3, SLAVE_ID);
  regVal   |= 0x4241          << HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_3, SLAVE_ADDR);
  regVal   |= vset            << HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_3, SLAVE_DATA);
  HWIO_OUT(MPM2_MPM_PMIC_VDD_ON_CFG_3, regVal);

  // VDD_ON_CFG_4 - Set S2 (Cx) ON
  HAL_mpm_GetVSet(HAL_MPM_VOLTAGE_RAIL_VDD_DIG, onCxMicrovolts, &vset);
  // VDD_ON_CFG_4 - Set S2 (Cx) VOLTAGE_CTL2 VSET
  regVal    = delay           << HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_4, DELAY);
  regVal   |= 0x1             << HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_4, SLAVE_ID);
  regVal   |= 0x1741          << HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_4, SLAVE_ADDR);
  regVal   |= vset            << HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_4, SLAVE_DATA);
  HWIO_OUT(MPM2_MPM_PMIC_VDD_ON_CFG_4, regVal);

  if (!((1 == pmic_device_info.nPmicAllLayerRevision) && (pmic_device_info.nPmicMetalRevision == 0)))
  {
    // VDD_ON_CFG_5- Set S4
    //Calculate Vset for S4 ON Voltage
    pm_smps_calculate_vset(0, 3, s4_active_volt_level, &vset);
	regVal    = s4_delay        << HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_5, DELAY);
    regVal   |= 1               << HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_5, SLAVE_ID);
    regVal   |= 0x1D41          << HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_5, SLAVE_ADDR);
    regVal   |= vset            << HWIO_SHFT(MPM2_MPM_PMIC_VDD_ON_CFG_5, SLAVE_DATA);
		
    HWIO_OUT(MPM2_MPM_PMIC_VDD_ON_CFG_5, regVal);
  }

  HWIO_OUTF(MPM2_MPM_SPMI_CMD_CFG, NUM_CMD_ON,  4); // 5 commands
  HWIO_OUTF(MPM2_MPM_SPMI_CMD_CFG, NUM_CMD_OFF, 4); // 5 commands
}





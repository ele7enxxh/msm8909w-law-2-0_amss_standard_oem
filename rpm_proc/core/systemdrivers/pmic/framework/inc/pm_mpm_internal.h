#ifndef __PM_MPM_IN_H__
#define __PM_MPM_IN_H__

/**
 * @file pm_mpm_internal.h
 *
 * Header file for internal declarations PMIC MPM VDD command driver.
 */
/*
  ====================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/framework/inc/pm_mpm_internal.h#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $

  ====================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "pm_mpm.h"
#include "pm_rpm_utilities.h"


/*=========================================================================
      Type Definitions
==========================================================================*/

/* Invalid MPM command to be used for rails that are not applicable on a target */
#define PM_MPM_INVALID_CMD  0xFF

/**
  * Structure to store the index of the dynamically changing
  * elements for the SPMI sequence.
  */
typedef struct
{
  uint32 sleep_level_index;    /* retention index */
  uint32 active_level_index;   /* active index  */
  uint32 sw_mode_level_index;  /* SW mode before sleep index */
} pm_mpm_cmd_index_type;

/**
  * Structure to store the rails info configured in MPM cmds.
  */
typedef struct
{
  rpm_resource_type resource_type; /* rail resource type */
  uint32            resource_index; /* resource index */
  uint32            sleep_headroom_uv; /* sleep headroom voltage */
} pm_mpm_rails_info_type;


/*----------------------------------------------------------------------------
 * Function : pm_mpm_cmd_init
 * -------------------------------------------------------------------------*/
/*!
   Get DAL device configuration. This code runs once.
     @dependencies
     pm_target_information_get_target_info
     @return
     None.
*/
void pm_mpm_cmd_init(void);

/* ============================================================================
**  Function : pm_mpm_get_delay_from_microvolts
** ============================================================================
*/
/*!
   Calculate the delay based on active and retention volttage.

   @param  pCfg - pointer to MPM config data buffer that sent to the PMIC.
   @retval None
   Note: This function is copied from MPM driver code.

*/
uint16 pm_mpm_get_delay_from_microvolts(uint32 off_microvolts, uint32 on_microvolts);

#endif /* __PM_MPM_IN_H__ */


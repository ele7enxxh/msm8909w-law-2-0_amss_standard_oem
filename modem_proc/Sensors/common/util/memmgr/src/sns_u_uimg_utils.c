/*============================================================================
  @file sns_u_uimg_utils.c

  @brief
  Contains implementation of utilities to make writing uImage aware code easier.
 
  The symbols in this file should be places in the uImage section.

  <br><br>


  Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential
  ============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/common/util/memmgr/src/sns_u_uimg_utils.c#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when       who    what, where, why 
  ---------- --- -----------------------------------------------------------
  2013-08-15  asj Initial release

  ============================================================================*/
#include "sns_common.h"
#include "sns_uimg_utils.h"
#include "sns_pm.h"

sns_pm_handle_t sns_utils_pm_client_handle;
extern bool sns_memmgr_module_restrictions[SNS_UIMG_SMR_MAX_CLIENTS];
extern OS_EVENT *sns_uimg_utils_mutex;

/*=========================================================================
  FUNCTION:  sns_utils_place_uimge_hold
  =======================================================================*/
void sns_utils_place_uimge_hold(sns_uimg_module_e module)
{
  uint8_t sns_os_err;
  SNS_ASSERT(sns_utils_pm_client_handle != NULL);
  sns_pm_vote_img_mode(sns_utils_pm_client_handle, SNS_IMG_MODE_BIG);
  sns_os_mutex_pend(sns_uimg_utils_mutex, 0, &sns_os_err);
  sns_memmgr_module_restrictions[module] = TRUE;
  sns_os_mutex_post(sns_uimg_utils_mutex);
}

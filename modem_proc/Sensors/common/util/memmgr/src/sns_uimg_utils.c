/*============================================================================
  @file sns_uimg_utils.c

  @brief
  Contains implementation of utilities to make writing uImage aware code easier.

  <br><br>


  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/common/util/memmgr/src/sns_uimg_utils.c#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when       who    what, where, why
  ---------- --- -----------------------------------------------------------
  2014-09-03 jhh Added client name on pm client
  2013-08-15 asj Initial release

  ============================================================================*/
#include "sns_common.h"
#include "sns_uimg_utils.h"
#include "sns_pm.h"

extern sns_pm_handle_t sns_utils_pm_client_handle;
bool sns_memmgr_module_restrictions[SNS_UIMG_SMR_MAX_CLIENTS];
OS_EVENT *sns_uimg_utils_mutex;

/*=========================================================================
  FUNCTION:  sns_utils_no_blocking_module
  =======================================================================*/
bool sns_utils_no_blocking_module(void)
{
  int i;
  for(i = 0; i < SNS_UIMG_SMR_MAX_CLIENTS; i++)
  {
    if(sns_memmgr_module_restrictions[i])
    {
      return(FALSE);
    }
  }
  return(TRUE);
}

/*=========================================================================
  FUNCTION:  sns_utils_remove_uimage_hold
  =======================================================================*/
void sns_utils_remove_uimage_hold(sns_uimg_module_e module)
{
  uint8_t sns_os_err;
  SNS_ASSERT(sns_utils_pm_client_handle != NULL);
  sns_os_mutex_pend(sns_uimg_utils_mutex, 0, &sns_os_err);
  sns_memmgr_module_restrictions[module] = FALSE;
  if(sns_utils_no_blocking_module())
  {
    sns_pm_vote_img_mode(sns_utils_pm_client_handle, SNS_IMG_MODE_NOCLIENT);
  }
  sns_os_mutex_post(sns_uimg_utils_mutex);
}

/*=========================================================================
  FUNCTION:  sns_utils_remove_uimage_hold
  =======================================================================*/
void sns_uimg_utils_init(void)
{
  uint8_t i, sns_os_err;
  sns_pm_err_code_e pm_err;
  pm_err = sns_pm_client_init(&sns_utils_pm_client_handle, NULL, "UTIL", SNS_PM_CLIENT_ID_UTIL);
  SNS_ASSERT(pm_err == SNS_PM_SUCCESS);
  sns_uimg_utils_mutex = sns_os_mutex_create(SNS_MEMMGR_DSPS_MUTEX, &sns_os_err);

  sns_os_mutex_pend(sns_uimg_utils_mutex, 0, &sns_os_err);
  for(i = 0; i < SNS_UIMG_SMR_MAX_CLIENTS; i++)
  {
    sns_memmgr_module_restrictions[i] = FALSE;
  }
  sns_os_mutex_post(sns_uimg_utils_mutex);
}

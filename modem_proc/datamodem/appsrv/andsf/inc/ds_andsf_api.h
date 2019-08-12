/*===========================================================================
                      DS_ANDSF_API.H

DESCRIPTION
 Definitions for ANDSF API functions

EXTERNALIZED FUNCTIONS

 Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who                            what, where, why
--------    -------                ----------------------------------------
06/09/14    Youjunf                          First version of file
===========================================================================*/
#ifndef DS_ANDSF_API_H
#define DS_ANDSF_API_H


/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "ps_sys.h"
#include "ps_sys_event.h"
#include "cm.h"


/*===========================================================================
                    EXTERNAL FUNCTION PROTOTYPES
===========================================================================*/
#ifdef __cplusplus
extern "C"
{
#endif

/*===========================================================================
FUNCTION      DS_ANDSF_INIT

DESCRIPTION   This function initializes all andsf global variables.
              The function is called during power up.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_andsf_init(void);

/*===========================================================================
FUNCTION DS_ANDSF_PS_IOCTL_POLICY_REFRESH_REQ

DESCRIPTION
  This function handles andsf policy refresh IOCTL from PS and send signal to trigger
  andsf policy fetch

PARAMETERS
  arg_val_ptr[in]- carrys required information
  ps_errno   [in]   - type of error

DEPENDENCIES
  None.

RETURN VALUE
 0 - Success
 -1- Failure

SIDE EFFECTS
  None
===========================================================================*/
//int  ds_andsf_ps_ioctl_policy_refresh_req(void *arg_val_ptr, int16 *ps_errno);


/*===========================================================================
FUNCTION DS_ANDSF_NOTIFY_AP_POLICY_REFRESH_STATUS

DESCRIPTION
  This function sends andsf policy refresh status to client

PARAMETERS
  status[in] - andsf policy refresh status

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
//void ds_andsf_notify_ap_policy_refresh_status(ps_sys_policy_refresh_status_enum_type status);


/*===========================================================================
FUNCTION DS_ANDSF_NOTIFY_AP_POLICY_FILE_INFO

DESCRIPTION
  This function sends andsf policy file info to client

PARAMETERS
  andsf_policy_file_info_ptr[in] - pointer to andsf policy file info.
                                   refer to ps_sys_policy_available_info_type

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
//void ds_andsf_notify_ap_policy_file_info(ps_sys_policy_available_info_type *andsf_policy_file_info_ptr);


/*===========================================================================
FUNCTION DS_ANDSF_NOTIFY_WLAN_INFO

DESCRIPTION
  This is the external function called by external clients.
  The clients have to notify the ANDSF module the current wlan info.


PARAMETERS
  wlan_info_ptr[in] - pointer to wlan info.
                      refer to ps_sys_wlan_status_type

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_andsf_notify_wlan_info(ps_sys_wlan_status_type   *wlan_info_ptr);


/*===========================================================================
FUNCTION DS_ANDSF_NOTIFY_WWAN_INFO

DESCRIPTION
  This is the external function called by external clients.
  The clients have to notify the ANDSF module the current wwan info.

PARAMETERS
  wwan_ss_info_ptr[in] - pointer to wwan info.
                         refer to cm_mm_msim_ss_info_s_type

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_andsf_notify_wwan_ss_info(cm_ss_event_e_type ss_event, const cm_mm_msim_ss_info_s_type *wwan_ss_info_ptr);

/*===========================================================================
FUNCTION DS_ANDSF_NV_REFRESH

DESCRIPTION
  Reset and re-initialize ANDSF module during nv refresh event

PARAMETERS
  subs_id[in] - subs id on which nv refresh happened
    
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_andsf_nv_refresh
(
  ds_sys_subscription_enum_type subs_id
);

#ifdef __cplusplus
}
#endif

#endif /* DS_ANDSF_API_H */


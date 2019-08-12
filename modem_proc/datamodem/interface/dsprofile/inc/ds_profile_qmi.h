/******************************************************************************
  @file    ds_profile_qmi.h
  @brief   DS PROFILE - QMI related code, which is not technology specific

  DESCRIPTION
      DS PROFILE - QMI related code, which is not technology specific

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ---------------------------------------------------------------------------
******************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsprofile/inc/ds_profile_qmi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/14/12   gs      First version 
===========================================================================*/
#ifndef DS_PROFILE_QMI_QNX_H
#define DS_PROFILE_QMI_QNX_H

#ifdef FEATURE_DS_PROFILE_ACCESS_QMI

#include "comdef.h"
#include "qmi.h"

/*===========================================================================
FUNCTION ds_profile_qmi_cleanup

DESCRIPTION
  cleanup routine used to clean up qmi resources

PARAMETERS 
  qmi_handle : The QMI handle to be released
  qmi_wds_client_handle : The QMI WDS service handle to be released

DEPENDENCIES

RETURN VALUE
  void
 
SIDE EFFECTS

===========================================================================*/
void ds_profile_qmi_cleanup(int *qmi_handle, int *qmi_wds_client_handle);

/*===========================================================================
FUNCTION ds_profile_sys_event_hdlr

DESCRIPTION
  System event handler for taking care of Subsystem Restart events. This is a
  callback function which will be called from QMI context.
 
PARAMETERS 
  event_id: Type of event
  event_info: Event associated info
  user_data: User data
 
DEPENDENCIES

RETURN VALUE
  void
 
SIDE EFFECTS

===========================================================================*/
void ds_profile_sys_event_hdlr(qmi_sys_event_type event_id, 
                               const qmi_sys_event_info_type *event_info, 
                               void *user_data);

/*===========================================================================
FUNCTION ds_profile_qmi_init

DESCRIPTION 
  Initialized QMI and QMI WDS service. 

PARAMETERS 
  sys_event_hdlr_func : Pointer to the system event handler which takes care of
    Subsystem Restart events.
  qmi_handle : Place holder for the QMI handle.
  qmi_client_handle : Place holder for the QMI WDS client handle

DEPENDENCIES

RETURN VALUE
  TRUE / FALSE
 
SIDE EFFECTS
===========================================================================*/
int ds_profile_qmi_init(void* sys_event_hdlr_func, int *qmi_handle, int *qmi_client_handle);


#endif /* FEATURE_DS_PROFILE_ACCESS_QMI */

#endif /* DS_PROFILE_QMI_QNX_H_ */


#ifndef SIMLOCK_TIME_NITZ_H
#define SIMLOCK_TIME_NITZ_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                S I M   L O C K  T I M E  N I T Z  H E A D E R


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        COPYRIGHT INFORMATION

Copyright (c) 2015 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/06/16   stv      Remote simlock support
04/15/15   stv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#if defined(FEATURE_SIMLOCK) && (defined(FEATURE_SIMLOCK_RSU) || defined(FEATURE_SIMLOCK_QC_REMOTE_LOCK))

#include "simlock_time.h"

/*===========================================================================
FUNCTION SIMLOCK_TIME_HANDLE_CM_SS_EVT

DESCRIPTION
  This function handles the CM SS Event posted to SIMLOCK TIME

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
 ===========================================================================*/
simlock_result_enum_type simlock_time_handle_cm_ss_evt(
  const simlock_time_cm_ss_info_type *info_ptr,
  simlock_time_source_enum_type       source
);

/*===========================================================================
FUNCTION   SIMLOCK_TIME_CM_SS_EVENT_CB

DESCRIPTION
  Handler for CM_SS_EVENT_SRV_CHANGED. It posts command to SIMLOCK task.
  This command is for NITZ time information used by SIMLOCK TIME.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_time_cm_ss_event_cb(
  cm_ss_event_e_type               ss_event,
  const cm_mm_msim_ss_info_s_type *mm_ss_info_ptr
);

#endif /* FEATURE_SIMLOCK && (FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK) */

#endif /* SIMLOCK_TIME_NITZ_H */



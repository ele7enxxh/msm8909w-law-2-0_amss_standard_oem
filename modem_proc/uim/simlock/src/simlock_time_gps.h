#ifndef SIMLOCK_TIME_GPS_H
#define SIMLOCK_TIME_GPS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                           S I M   L O C K   G P S  H E A D E R


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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_time_gps.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/06/16   stv      Remote simlock support
04/15/15   stv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#if defined(FEATURE_SIMLOCK) && (defined(FEATURE_SIMLOCK_RSU) || defined(FEATURE_SIMLOCK_QC_REMOTE_LOCK)) && defined(FEATURE_SIMLOCK_QMI_LOC)

#include "qmi_client.h"
#include "simlock_time.h"

#define SIMLOCK_TIME_GPS_MAX_VALIDITY_TIME           (3 * 24 * 60 * 60)

/*===========================================================================
FUNCTION SIMLOCK_TIME_HANDLE_QMI_LOC_READY_SIG

DESCRIPTION
  Process the service ready signal from QMI LOC
  Releases the notifier handle so as to avoid any redundant signal notifications
  Registration for error call back is done

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
void simlock_time_handle_qmi_loc_ready_sig(
  void
);

/*===========================================================================
FUNCTION SIMLOCK_TIME_HANDLE_QMI_LOC_IND

DESCRIPTION
  This function handles the command posted to SIMLOCK for QMI LOC indication
  In case of
  a. Invalid IND data - Starts failure retry timer.
  b. Success IND data - Starts max validity timer.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
void simlock_time_handle_qmi_loc_ind(
  const simlock_time_qmi_loc_ind_info_type *qmi_loc_ind_msg_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_TIME_QMI_LOC_INIT

DESCRIPTION
  Initializes the QMI-LOC client during power-up
  Register with QMI LOC service for system information indication

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
void simlock_time_qmi_loc_init(
  void
);

/*===========================================================================
FUNCTION SIMLOCK_TIME_HANDLE_SOURCE_TIME_INFO_CMD

DESCRIPTION
  This function handles SIMLOCK_TIME_HANDLE_SOURCE_TIME_CMD

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_time_handle_gps_timer_expiry_cmd(
  void
);

/*===========================================================================
FUNCTION SIMLOCK_TIME_START_GPS_TIMER

DESCRIPTION
  This function starts the timer specified.
  Before starting clears if the same timer is running already.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
void simlock_time_start_gps_timer(
  simlock_time_type timer_in_sec
);

#endif /* FEATURE_SIMLOCK && (FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK) && FEATURE_SIMLOCK_QMI_LOC */

#endif /* SIMLOCK_TIME_GPS_H */



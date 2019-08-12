#ifndef SIMLOCK_TIME_H
#define SIMLOCK_TIME_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M   L O C K  T I M E  H E A D E R


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
06/29/15   stv      Start time in temp unlock req as current time
                    Graceful handling of PMIC reset during temp unlock
05/15/15   stv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "simlock_common.h"

/* Number of seconds from UTC time  1st Jan 1970 to SIMLOCK BASE DATE i.e. 1st Jan 2014 */
#define SIMLOCK_TIME_BASE  1388534400

typedef  uint64  simlock_time_type;

/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_TIME_SOURCE_ENUM_TYPE

   DESCRIPTION:
     Enumeration for the Time sources.
-------------------------------------------------------------------------------*/
typedef enum {
  SIMLOCK_TIME_SOURCE_GPS              = 0,
  SIMLOCK_TIME_SOURCE_NITZ_PRI         = 1,
  SIMLOCK_TIME_SOURCE_NITZ_SEC         = 2,
  SIMLOCK_TIME_SOURCE_SERVER           = 3,
  SIMLOCK_TIME_TRUSTED_SOURCE_COUNT    = 4,
  /* Non trusted souces */
  SIMLOCK_TIME_SOURCE_BINARY_BLOB      = 4,
  SIMLOCK_TIME_SOURCE_LAST_POWER_CYCLE = 5,
  SIMLOCK_TIME_SOURCE_COUNT            = 6
} simlock_time_source_enum_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_TIME_QMI_LOC_IND_INFO_TYPE

   DESCRIPTION:
     The structure contains the information about :
     1. indication status  - If an error is received in the indication or
                             Time TLV is not present in the indication,
                             this is set to FALSE
     2. txn_id             - Transaction ID received in IND
     3. time               - Time is populated in this only if ind_status is TRUE.
                                     i.e. when QMI_LOC_GET_BEST_AVAILABLE_POSTION_IND is successfully
                                     received and has valid time
-------------------------------------------------------------------------------*/
typedef struct{
  boolean                              ind_status;
  boolean                              is_txn_id_present;
  boolean                              is_time_present;
  uint32                               txn_id;
  simlock_time_type                    time;
} simlock_time_qmi_loc_ind_info_type;

/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_TIME_SYS_ENUM_TYPE

   DESCRIPTION:
     Enumeration for the system modes.
-------------------------------------------------------------------------------*/
typedef enum
{
  SIMLOCK_TIME_SYS_3GPP,
  SIMLOCK_TIME_SYS_3GPP2
}simlock_time_sys_enum_type;

/* ----------------------------------------------------------------------------
    STRUCTURE:        SIMLOCK_TIME_GW_NITZ_TYPE

    DESCRIPTION:
      The structure contains the time information for 3GPP sys modes.
 -------------------------------------------------------------------------------*/
typedef struct{
  boolean           is_time_present;
  uint8             year;
  uint8             month;
  uint8             day;
  uint8             hours;
  uint8             minutes;
  uint8             seconds;
  int8              time_zone;
} simlock_time_gw_nitz_type;

/* ----------------------------------------------------------------------------
    STRUCTURE:        SIMLOCK_TIME_CM_SS_INFO_TYPE

    DESCRIPTION:
      The structure contains the time information from CM SS event
 -------------------------------------------------------------------------------*/
typedef struct{
  simlock_time_sys_enum_type         sys;
  union
  {
    simlock_time_gw_nitz_type        gw_nitz_info;
  }nitz_info;
} simlock_time_cm_ss_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_TIME_SOURCE_TYPE_MSG_REQ_TYPE

   DESCRIPTION:
     The structure contains the time source and the time information
     sent by the source to SIMLOCK TIME.
-------------------------------------------------------------------------------*/
typedef struct{
  simlock_time_source_enum_type         source;
  union{
    simlock_time_cm_ss_info_type        cm_time;
    simlock_time_qmi_loc_ind_info_type  gps_time;
  }src_data;
}simlock_time_source_time_msg_req_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_TIME_INFO_TYPE

   DESCRIPTION:   Contains time information from various sources
-----------------------------------------------------------------------------*/
typedef struct {
  simlock_time_type         last_pmic_time;
  simlock_time_type         last_sent_time;
  struct {
    boolean                 valid;
    simlock_time_type       offset;
    simlock_time_type       pmic_time_when_acquired;
  } data[SIMLOCK_TIME_SOURCE_COUNT];
} simlock_time_storage_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_TIME_INFO_TYPE
-----------------------------------------------------------------------------*/
typedef struct
{
  simlock_time_type current_time;
} simlock_time_info_type;

/*===========================================================================
DATA TYPE   SIMLOCK_TIME_EVT_CB_TYPE
Call back type registered with SIMLOCK TIME module
==========================================================================*/
typedef void simlock_time_evt_cb_type (simlock_time_info_type  time);

#if defined(FEATURE_SIMLOCK) && (defined(FEATURE_SIMLOCK_RSU) || defined(FEATURE_SIMLOCK_QC_REMOTE_LOCK))
/*===========================================================================
FUNCTION SIMLOCK_TIME_INIT

DESCRIPTION
  Initialization of SIMLOCK TIME module

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_time_init(
  void
);

/*===========================================================================
FUNCTION SIMLOCK_TIME_DEINIT

DESCRIPTION
  This function deinitalize the simlock time moduel by clearing
  simlock_time_global_time_info global. If temporary unlock is enabled then
  saves the information current time information in the SFS as a backup to keep
  the integrity of the temporary unlock.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_time_deinit(
  boolean    store_current_time
);

/*===========================================================================
FUNCTION SIMLOCK_TIME_REGISTER_TIME_EVENT

DESCRIPTION
  API to register the simlock call back function with TIME module
  NULL call back is allowed for de-registration

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type
    SIMLOCK_SUCCESS           if successful
    SIMLOCK_GENERIC_ERROR    if already a callback is registered

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_time_register_time_event(
  simlock_time_evt_cb_type *evt_cb_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_TIME_GET_CURRENT_TIME

DESCRIPTION
  This function is called from SIMLOCK at anytime to get the current time.
  Synchronous API which will fill the time in seconds from SIMLOCK BASE
  in out parameter

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type -
    SIMLOCK_SUCCESS       - If successful
                               Time is filled only if success.
    SIMLOCK_GENERIC_ERROR - If no valid time source
    SIMLOCK_INCORRECT_PARAMS

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_time_get_current_time(
  simlock_time_info_type *time_ptr
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
void simlock_time_handle_source_time_info_cmd(
  simlock_time_source_time_msg_req_type *source_time_info_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_TIME_UPDATE_SEND_TIME

DESCRIPTION
  This function updates and sends time event to SIMLOCK if its more new.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_time_update_send_time(
  simlock_time_source_enum_type source,
  simlock_time_type             source_time
);

/*===========================================================================
FUNCTION SIMLOCK_TIME_IS_GPS_TIME_REQUIRED

DESCRIPTION
  This function will check if the GPS time available in SIMLOCK is older
  by SIMLOCK_TIME_MAX_VALIDITY_TIME.
  If yes, this function will return TRUE else FALSE.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  boolean

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_time_is_gps_time_required(
  void
);

/*===========================================================================
FUNCTION SIMLOCK_TIME_PROCESS_BACKUP_TIME_UPDATE

DESCRIPTION
  This function processes the updating and backup of the current time.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_time_process_backup_time_update
(
  void
);

/*===========================================================================
FUNCTION SIMLOCK_TIME_PERIODIC_BACKUP

DESCRIPTION
  Creates/starts or stop/deletes the periodic current time backup timer.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_time_periodic_backup
(
  boolean set
);

#endif /* FEATURE_SIMLOCK && (FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK) */

#endif /* SIMLOCK_TIME_H */



#ifndef DSCAM_H
#define DSCAM_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               D S C A M 

GENERAL DESCRIPTION
  This file  contains extern definistions for DSCAM module, used
  by dsmgr.

EXTERNALIZED FUNCTIONS

  ds_cam_process_cmd()
    Processes CAM commadn received by DS task ( currently).

  FEATURE_DS_CAM        - Supports CAM module in Hybrid mode

   Copyright (c) 2004 - 2016 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary. 
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/dscam.h#1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
10/24/11    ty     Changed the dscami_process_cam_timer_expiry, dscami_process_cam_call_dormant, 
                   dscami_process_flow_ctrl_timer_expiry, dscam_timer_cb functions to not be static.
                   Done to expose functions to test classes.
06/13/11    dvk    Merged support to get and set CAM timer value at runtime.
03/09/09    sa     AU level CMI modifications.
03/01/05    kvd    Replaced ds.h withd stask.h as ds.h is deprecated now.
10/08/04    kvd    Created Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DS_CAM

#include "dstask_v.h"
#include "hdrcp_api.h"

/*===========================================================================
                            MACROS
===========================================================================*/
/*---------------------------------------------------------------------------
  The  RLP idle timeout needed if CAM module is active. We overwrite the
  existing RLP idle timeotu with this value, if we decide to swicth to EVDO
  and store it to previous value after dormancy kicks in.  This is to overwite
  default RLP idle timeotu behavior ( it coudl be 0), by what is desired by
  CAM module, for eg. to force dormancy earlier than usual.
---------------------------------------------------------------------------*/
#define DS_CAM_IDLE_PERIOD          1  /* sec */

/*---------------------------------------------------------------------------
  The duration for which we flow control (long enough for EVDO to be acquired
  ) followed by dormancy, if CAM module is active
---------------------------------------------------------------------------*/
#define DS_CAM_FLOW_CTRL_TIMER     6000 /* msec */ //changed to accomodate CSFB.

/*---------------------------------------------------------------------------
  The duration for which we flow control (long enough for LTE to be acquired)
---------------------------------------------------------------------------*/
#define DS_CAM_DO_FLOW_CTRL_TIMER   6000 /* msec */

#define DS_CAM_CONFIG_MAX_PARAMS_NUM         DS_CAM_CONFIG_PARAM_ID_MAX
#define DS_CAM_CONFIG_MAX_PARAM_NAME_SIZE    100


typedef enum
{
  DS_CAM_IDLE_STATE=0,               /* CAM module not yet active          */
  DS_CAM_CALL_ACTIVE_STATE=1,        /* CAM call active on 1X              */
  DS_CAM_CALL_DORMANT_STATE=2,       /* CAM call dormant on 1X             */
  DS_CAM_WAITING_COLOC_INFO_STATE=3, /* waiting to get PH info from CM     */
  DS_CAM_WAITING_DORMANCY_STATE=4,   /* Waiting for dormancy followed by
                                      idle timeout                         */
  DS_CAM_FLOW_CTRL_ACTIVE_STATE=5,   /* CAM flow ctrl active - long enough 
                                        for Evdo to be acquired            */
  DS_CAM_MAX_STATE
} dscam_cam_state_enum_type;

/*---------------------------------------------------------------------------
  Type that represents the current state of the SO33 call on CDMA system.
---------------------------------------------------------------------------*/

typedef enum
{
  DS_CAM_CALL_IDLE=0,                /* No  SO 33 call connected           */
  DS_CAM_CALL_ACTIVE=1,              /* SO33  call connected               */
  DS_CAM_CALL_DORMANT=2,             /* call dormant                       */
  DS_CAM_MAX_CALL_STATE
} dscam_cam_callstate_enum_type;

typedef enum
{
  DS_CAM_CALL_CONN_NONE = 0,
  DS_CAM_CALL_CONN_1X,              /* SO 33 call is connected */
  DS_CAM_CALL_CONN_HDR,             /* HDR call is connected */
  DS_CAM_CALL_CONN_MAX = 0xFF
}dscam_call_connected_enum_type;

typedef enum
{
  DS_CAM_CONFIG_PARAM_ID_MIN = 0,

  DS_CAM_DO_BACKOFF_TIMER_1 = DS_CAM_CONFIG_PARAM_ID_MIN,
  DS_CAM_DO_BACKOFF_TIMER_2,
  DS_CAM_DO_BACKOFF_TIMER_3,
  DS_CAM_DO_BACKOFF_TIMER_4,
  DS_CAM_DO_FLOW_CONTROL_TIMER,
  DS_CAM_DO_HARD_FAILURE_TIMER,
  
  DS_CAM_CONFIG_PARAM_ID_MAX
}dscam_cfg_param_name_type;

typedef enum
{
  DS_CAM_D2L_REDIR_NONE = 0,      
  DS_CAM_D2L_REDIR_HARD_FAIL = HDRIND_LMAC_ORAT_MEAS_FAILED, /* Hard failure */
  DS_CAM_D2L_REDIR_SUCCESS = 2,      /* Redirection succeeded */
  DS_CAM_MAX_D2L_REDIR_STATE
} dscam_cam_d2l_redir_enum_type;

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION DSCAMI_PROCESS_CAM_TIMER_EXPIRY

DESCRIPTION
  This module process CAM timer expired event received by the DS task
  (possibly different task if CAM module moves to a different task in future)
  If recevied in correct state, CAM module moves to CAM_CALL_WAITING_DORMANY
  or back to CAM_CALL_ACTIVE state if there is no colocated EVDO system to 
  move to.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dscami_process_cam_timer_expiry( void );

/*===========================================================================
FUNCTION DSCAMI_PROCESS_CAM_CALL_DORMANT

DESCRIPTION
  This module processes call_dormant CAM cmd received by the DS task
  (possibly tby different ask if CAM module moves to a different task later).
  If receied in correct state, CAMmodule moves to CAM_CALL_FLOW_CTRL_ACTIVE
  state.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dscami_process_cam_call_dormant( void );

/*===========================================================================
FUNCTION DSCAMI_PROCESS_FLOW_CTRL_TIMER_EXPIRY

DESCRIPTION
  This module process Dormant timer expired event.
  We start this timer for 5 sec ( effective followed by call going
  dormant to force additional flow contorl for 5 secs) to give enough
  time for 1X to release the locks.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dscami_process_flow_ctrl_timer_expiry( void );

/*===========================================================================
FUNCTION      DSCAM_TIMER_CB

DESCRIPTION   Callback for the  CAM activation & flow control timers. 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  Timer expiation cmds posted to CAM module.
===========================================================================*/
void dscam_timer_cb( unsigned long    timer_id );

/*===========================================================================

FUNCTION DSCAM_INIT

DESCRIPTION
  This function performs power-up initialization of CAM module.
  This includes initialization  and registration of ds3g timers.

  This function must be called once at data services task startup.
  This could be called from different task context if CAM module
  moves to a different task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void  dscam_init( void );

/*===========================================================================
FUNCTION DSCAM_SEND_CAM_CMD

DESCRIPTION
  This function  posts a cmd to the CAM module

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void dscam_send_cam_cmd
(
  ds_cmd_enum_type cam_cmd                    /* Cmd sent to CAM module    */
);

/*===========================================================================
FUNCTION DS_PROCESS_CAM_CMD

DESCRIPTION
  This module process CAM cmds received by the DS task (possibly by different
  task if CAM module moves to a different task in future); These cmds are for
  CAM module to kick-in and force dormancy on 1X to move to EVDO, if needed.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_process_cam_cmd( ds_cmd_enum_type cmd_name );

/*===========================================================================
FUNCTION      DSCAM_GET_CAM_TIMER_VALUE

DESCRIPTION   This function returns CAM timer value currently being used in 
              seconds. Caller of this function should convert it to 
              milliseconds or any other qualifier if required.

DEPENDENCIES  None.

RETURN VALUE  
  uint32 - CAM timer value in seconds

SIDE EFFECTS  None.
===========================================================================*/
uint32 dscam_get_cam_timer_value
( 
  void 
);

/*===========================================================================
FUNCTION      DSCAM_SET_CAM_TIMER_VALUE

DESCRIPTION   This function stores the CAM timer value in an intermediate 
              variable if CAM is not in IDLE state. This intermediate value 
              will be stored into "dscam_timer_val" when CAM transitions to 
              IDLE state. If CAM is not in IDLE state when this function is 
              called, the new timer value will be copied to "dscam_timer_val"
              immediately.              

DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  None.
===========================================================================*/
void dscam_set_cam_timer_value
( 
  uint32 new_timer_val
);

/*===========================================================================
FUNCTION      DSCAM_IS_FORCED_CALLEND

DESCRIPTION   This function returns CAM CALLSTATE for QTF testing

DEPENDENCIES  None.

RETURN VALUE  
  CAM call state

SIDE EFFECTS  None.
===========================================================================*/
boolean dscam_is_forced_callend
( 
  void 
);

/*===========================================================================
FUNCTION      DSCAM_STOP_FLOW_TIMER

DESCRIPTION   This function returns CAM CALLSTATE for QTF testing

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void dscam_stop_flow_timer
( 
  dscam_cam_d2l_redir_enum_type fail_type 
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
#endif /* FEATURE_DS_CAM */
#endif /* DSCAM_H */

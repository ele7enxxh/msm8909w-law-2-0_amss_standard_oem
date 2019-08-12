#ifndef DS3GFLOWMGR_H
#define DS3GFLOWMGR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             3G  DATA  SERVICES  FLOW  MANAGER

GENERAL DESCRIPTION
  This software unit manages the flow for DS.

 Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/ds3gflowmgr.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/16/14   vm      Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "sys.h"
#include "dstask_v.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*-------------------------------------------------------------------------
  Enum for ds3g_flowmgr timer state
-------------------------------------------------------------------------*/
typedef enum
{
  DS3GFLOWMGR_TIMER_STOPPED_EV = 0,
  DS3GFLOWMGR_TIMER_STARTED_EV = 1,
  DS3GFLOWMGR_TIMER_EXPIRED_EV = 2,
  DS3GFLOWMGR_TIMER_RUNNING_EV = 3
}ds3gflowmgr_timer_state_type;

/*-------------------------------------------------------------------------
  Enum for ds3g_flowmgr PS call event type
-------------------------------------------------------------------------*/
typedef enum
{
  DS3GFLOWMGR_PS_CALL_MIN_EV          = 0,
  DS3GFLOWMGR_PS_CALL_ORIG_EV         = DS3GFLOWMGR_PS_CALL_MIN_EV,
  DS3GFLOWMGR_PS_CALL_END_EV          = 1,
  DS3GFLOWMGR_PS_CALL_ACTIVE_EV       = 2,
  DS3GFLOWMGR_PS_CALL_DORMANT_EV      = 3,
  DS3GFLOWMGR_PS_CALL_NO_FLOW_EV      = 4,
  DS3GFLOWMGR_PS_CALL_MAX_EV          = DS3GFLOWMGR_PS_CALL_NO_FLOW_EV
}ds3gflowmgr_ps_call_event_type;

/*-------------------------------------------------------------------------
  Enum for ds3g_flowmgr PS call state type
-------------------------------------------------------------------------*/
typedef enum
{
  DS3GFLOWMGR_PS_CALL_STATE_NO_CALL   = 0,
  DS3GFLOWMGR_PS_CALL_STATE_ORIG      = 1,
  DS3GFLOWMGR_PS_CALL_STATE_ACTIVE    = 2,
  DS3GFLOWMGR_PS_CALL_STATE_DORMANT   = 3,
  DS3GFLOWMGR_PS_CALL_STATE_NO_FLOW   = 4
}ds3gflowmgr_ps_call_state_type;

/*-------------------------------------------------------------------------
  Structure for EVENT_DS3G_COEX_FLOW_CONTROL
  size of structure 12 bytes
-------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  boolean    dds_flow_ctrl;
  boolean    data_hi_prio_ind;
  boolean    coex_detected;
  boolean    non_dds_ps_call_active;
  boolean    irat_detected;
  uint8      flow_ctrl_timer_state;
  uint16     flow_ctrl_timer_val;
  uint8      dds_subs_id;
  uint8      dds_sys_mode;
  uint8      hi_prio_subs_id;
  uint8      hi_prio_sys_mode;
}ds3gflowmgr_coex_event_type;

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GFLOWMGR_INIT

DESCRIPTION   This function initializes the DS3G Flow Mgr module. This would
              be invoked from ds3g_init() which is the initialization
              routine for DSMgr.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gflowmgr_init( void );

/*===========================================================================
FUNCTION      DS3GFLOWMGR_GET_COEX_DATA_FLOW_STATUS

DESCRIPTION   This function returns the coex data flow status per 
              subscription id.

DEPENDENCIES  None.

RETURN VALUE  TRUE:  coex data flow enabled
              FALSE: coex data flow disabled

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gflowmgr_get_coex_data_flow_status
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3GFLOWMGR_GET_DATA_HI_PRIO

DESCRIPTION   This function returns the data high priority per subscription id.

DEPENDENCIES  None.

RETURN VALUE  TRUE:  data high priority set 
              FALSE: data high priority unset

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gflowmgr_get_data_hi_prio
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3GFLOWMGR_SET_DATA_HI_PRIO

DESCRIPTION   This function sets the data high priority per subscription id.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gflowmgr_set_data_hi_prio
(
  sys_modem_as_id_e_type  subs_id,
  boolean                 data_hi_prio
);

/*===========================================================================
FUNCTION      DS3GFLOWMGR_GET_SIMUL_DATA_ALLOWED

DESCRIPTION   This function returns if Simultaneous data calls are allowed 
              or not.

DEPENDENCIES  None.

RETURN VALUE  TRUE:  Simultaneous data call allowed
              FALSE: Simultaneous data call not allowed

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gflowmgr_get_simul_data_allowed( void );

/*===========================================================================
FUNCTION      DS3GFLOWMGR_SET_SIMUL_DATA_ALLOWED

DESCRIPTION   This function sets the simultaneous data call allowed flag.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gflowmgr_set_simul_data_allowed
(
  boolean simul_data_allowed
);

/*===========================================================================
FUNCTION      DS3GFLOWMGR_NOTIFY_PS_CALL_STATE_PER_SUB

DESCRIPTION   This function notifies the PS call state per subscription id.
              Clients needs to call this API as follows
              - Set ps_call_event to DS3GFLOWMGR_PS_CALL_ORIG_EV when call is
                being originated for the subs_id.
              - Set ps_call_event to DS3GFLOWMGR_PS_CALL_END_EV when all calls
                are ended for the subs_id.
              - Set ps_call_event  to DS3GFLOWMGR_PS_CALL_ACTIVE when atleast
                one call becomes active for the subs_id.
              - Set ps_call_event to DS3GFLOWMGR_PS_CALL_DORMANT_EV when all calls
                goes to dormancy for the subs_id.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gflowmgr_notify_ps_call_state_per_sub
(
  sys_modem_as_id_e_type          subs_id,
  ds3gflowmgr_ps_call_event_type  ps_call_event
);

/*===========================================================================
FUNCTION      DS3GFLOWMGR_PROCESS_TIMER_EXPIRED_CMD

DESCRIPTION   This function processes the timer expired command.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gflowmgr_process_timer_expired_cmd
(
  ds_cmd_type  *cmd_ptr
);

/*===========================================================================
FUNCTION      DS3GFLOWMGR_NOTIFY_IRAT_IND

DESCRIPTION   This function is used by DSD to notify IRAT indication on
              either subscription.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gflowmgr_notify_irat_ind( void );

#endif /* DS3GFLOWMGR_H */


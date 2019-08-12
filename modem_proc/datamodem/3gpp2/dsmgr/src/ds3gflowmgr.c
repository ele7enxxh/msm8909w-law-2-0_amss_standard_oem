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
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3gflowmgr.c#1 $ 
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

#include "cm.h"
#include "event.h"
#include "wwan_coex_mgr.h"
#include "subs_prio.h"
#include "dsmsgr.h"
#include "dsmsgrrecv_msg.h"

#include "data_msg.h"
#include "ds3gmgr.h"
#include "ds3gflowmgr.h"
#include "ds3gsubsmgr.h"
#include "ds3gcmif.h"
#include "ds3gcfgmgr.h"
#include "rex.h"
#include "policyman.h"
/*===========================================================================

                        LOCAL DATA STRUCTURES

===========================================================================*/

#define DS3GFLOWMGR_INVALID_TIMER_ID       0
#define DS3GFLOWMGR_DISABLE_FLOW_TIMER_ID  1
#define DS3GFLOWMGR_MONITOR_FLOW_TIMER_ID  2

#define DS3GFLOWMGR_NV_PATH                                       \
  "/nv/item_files/modem/data/dsmgr/dual_data_flow_control"

#define DS3GFLOWMGR_MONITOR_FLOW_NV_PATH                          \
  "/nv/item_files/modem/data/dsmgr/monitor_flow_timer"

/*---------------------------------------------------------------------------
  Default timer values
---------------------------------------------------------------------------*/
#define DS3GFLOWMGR_DISABLE_FLOW_TIMER_VAL 5  /* in minutes */
#define DS3GFLOWMGR_MONITOR_FLOW_TIMER_VAL  10  /* in seconds */

#define DS3GFLOWMGR_SINGLE_DATA_ACTIVE_SUBS 1
/*---------------------------------------------------------------------------
  DS3G Flow Mgr Timer structure
---------------------------------------------------------------------------*/
typedef struct
{
  rex_timer_type      timer;
  rex_timer_cnt_type  value;
}ds3gflowmgr_timer_info_type;

/*--------------------------------------------------------------------------
  Structure used to read the Co-ex NV item from the item file in EFS
--------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  boolean  enable_flow_control;
  uint8    timer_val;
} ds3gflowmgr_nv_type;


/*--------------------------------------------------------------------------
  Structure used to read the Monitor flow NV item
--------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  boolean   enable_monitor_flow;
  uint32    timer_val;
} ds3gflowmgr_monitor_flow_nv_type;

/*-------------------------------------------------------------------------
  Enum for DS3G Flow Mgr recompute reason
-------------------------------------------------------------------------*/
typedef enum
{
  DS3GFLOWMGR_FLOW_TIMER_EXP   = 0,
  DS3GFLOWMGR_HI_PRIO_CHG      = 1,
  DS3GFLOWMGR_COEX_CHG         = 2,
  DS3GFLOWMGR_PS_CALL_CHG      = 3,
  DS3GFLOWMGR_IRAT_IND         = 4
}ds3gflowmgr_recompute_reason_type;

/*-------------------------------------------------------------------------
  Structure for store the data activity
-------------------------------------------------------------------------*/
typedef struct
{
  sys_modem_as_id_e_type  subs_id;
  uint64                  total_uldl_wm_cnt;
}ds3gflowmgr_flow_activity_type;

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Enable / Dsiable Dual data flow control - NV controlled
  Disabled by default
---------------------------------------------------------------------------*/
static boolean ds3gflowmgr_enable_data_flow_control = FALSE;

/*---------------------------------------------------------------------------
  Data high priority per subscription
---------------------------------------------------------------------------*/
static boolean ds3gflowmgr_data_hi_prio[DS3GSUBSMGR_SUBS_ID_COUNT] = {FALSE};

/*---------------------------------------------------------------------------
  Registered for simultaneous data allowed indication
---------------------------------------------------------------------------*/
static boolean ds3gflowmgr_regd_simul_data_ind = FALSE;

/*---------------------------------------------------------------------------
  Simultaneous data allowed flag
---------------------------------------------------------------------------*/
static boolean ds3gflowmgr_simul_data_flag = FALSE;

/*---------------------------------------------------------------------------
  PS data call state per subscription
---------------------------------------------------------------------------*/
static ds3gflowmgr_ps_call_state_type
         ds3gflowmgr_ps_call_state[DS3GSUBSMGR_SUBS_ID_COUNT]
           = {DS3GFLOWMGR_PS_CALL_STATE_NO_CALL};

/*---------------------------------------------------------------------------
  Coex data data flow status per subscription
---------------------------------------------------------------------------*/
static boolean ds3gflowmgr_data_flow_status[DS3GSUBSMGR_SUBS_ID_COUNT] = {TRUE};

/*---------------------------------------------------------------------------
  Disable flow timer
---------------------------------------------------------------------------*/
static ds3gflowmgr_timer_info_type ds3gflowmgr_disable_flow_timer;

/*---------------------------------------------------------------------------
  Enable / Disable Monitor flow - NV controlled
  Disabled by default
---------------------------------------------------------------------------*/
static boolean ds3gflowmgr_monitor_flow_enabled = FALSE;

/*---------------------------------------------------------------------------
  Monitor flow timer
---------------------------------------------------------------------------*/
static ds3gflowmgr_timer_info_type ds3gflowmgr_monitor_flow_timer;

/*---------------------------------------------------------------------------
  Global variable to store data activity
---------------------------------------------------------------------------*/
static ds3gflowmgr_flow_activity_type ds3gflowmgr_flow_activity = {0};

/*===========================================================================

                    INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION       DS3GFLOWMGR_TIMER_CBACK

DESCRIPTION    Function registered when the timer expires

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
void ds3gflowmgr_timer_cback
(
  unsigned long parm
)
{
  ds_cmd_type    *cmd_ptr = NULL;
  unsigned long  *data_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(parm == DS3GFLOWMGR_INVALID_TIMER_ID)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,"Invalid parm:%d. Ignore", parm);
    return;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_MED, "ds3gflowmgr_timer_cback timer_id:%d", parm);

  /*--------------------------------------------------------------------
    Get command buffer for new command
  --------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(unsigned long));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    return;
  }
  cmd_ptr->hdr.cmd_id = DS_CMD_FLOW_MGR_TIMER_EXPIRED;
  data_ptr = (unsigned long*)cmd_ptr->cmd_payload_ptr;
  *data_ptr = parm;
  ds_put_cmd(cmd_ptr);

  return;
} /* ds3gflowmgr_timer_cback() */

/*===========================================================================
FUNCTION       DS3GFLOWMGR_GET_TIMER_INFO

DESCRIPTION    Returns the specific timer

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
ds3gflowmgr_timer_info_type* ds3gflowmgr_get_timer_info
(
  uint8  timer_id
)
{
  ds3gflowmgr_timer_info_type  *timer_info_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch( timer_id )
  {
    case DS3GFLOWMGR_DISABLE_FLOW_TIMER_ID:
      timer_info_p = &ds3gflowmgr_disable_flow_timer;
      break;

    case DS3GFLOWMGR_MONITOR_FLOW_TIMER_ID:
      timer_info_p = &ds3gflowmgr_monitor_flow_timer;
      break;

    default:
      break;
  }
  return timer_info_p;
}/* ds3gflowmgr_timer_start() */

/*===========================================================================
FUNCTION       DS3GFLOWMGR_START_TIMER

DESCRIPTION    Start the specific timer

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
void ds3gflowmgr_start_timer
(
  uint8  timer_id
)
{
  ds3gflowmgr_timer_info_type  *timer_info_p = NULL;
  uint32                        duration = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  timer_info_p = ds3gflowmgr_get_timer_info(timer_id);
  if(timer_info_p == NULL )
  {
    DATA_3GMGR_MSG1( MSG_LEGACY_LOW, "timer id :%d invalid", timer_id);
    return;
  }

  switch( timer_id )
  {
    case DS3GFLOWMGR_DISABLE_FLOW_TIMER_ID:
    /* convert minutes to milliseconds */
      duration = (uint32)timer_info_p->value*60*1000;
      break;

    case DS3GFLOWMGR_MONITOR_FLOW_TIMER_ID:
      /* convert seconds to milliseconds */
      duration = (uint32)timer_info_p->value*1000;
      break;

    default:
      break;
  }

  if(!rex_get_timer(&(timer_info_p->timer)))
  {
    (void)rex_set_timer(&(timer_info_p->timer),(rex_timer_cnt_type)duration);
    DATA_3GMGR_MSG2( MSG_LEGACY_MED,
                     "Timer started timer_id :%d duration:%d msec",
                     timer_id, duration );
  }

  return;
}/* ds3gflowmgr_start_timer() */

/*===========================================================================
FUNCTION       DS3GFLOWMGR_STOP_TIMER

DESCRIPTION    Stop the specific timer

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
void ds3gflowmgr_stop_timer
(
  uint8  timer_id
)
{
  ds3gflowmgr_timer_info_type  *timer_info_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  timer_info_p = ds3gflowmgr_get_timer_info(timer_id);
  if(timer_info_p == NULL )
  {
    DATA_3GMGR_MSG1( MSG_LEGACY_LOW, "timer id :%d invalid", timer_id);
    return;
  }

  if(rex_get_timer(&(timer_info_p->timer)))
  {
    (void) rex_clr_timer(&(timer_info_p->timer));
    DATA_3GMGR_MSG1( MSG_LEGACY_MED, "Timer stopped timer_id:%d",timer_id );
  }
  return;
}/* ds3gflowmgr_stop_timer() */

/*===========================================================================
FUNCTION       DS3GFLOWMGR_TIMER_RUNNING

DESCRIPTION    utility to find if timer is running.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
boolean ds3gflowmgr_timer_running
(
  uint8  timer_id
)
{
  boolean                       ret_val = FALSE;
  ds3gflowmgr_timer_info_type  *timer_info_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  timer_info_p = ds3gflowmgr_get_timer_info(timer_id);
  if(timer_info_p == NULL )
  {
    DATA_3GMGR_MSG1( MSG_LEGACY_LOW, "timer id :%d invalid", timer_id);
    return FALSE;
  }

  if(rex_get_timer(&(timer_info_p->timer)))
  {
    ret_val = TRUE;
  }
  else
  {
    ret_val = FALSE;
  }
  DATA_MSG2(MSG_LEGACY_LOW, "Timer:%d running:%d", timer_id, ret_val);
  return ret_val;
}/* ds3gflowmgr_timer_running() */

/*===========================================================================
FUNCTION      DS3GFLOWMGR_COEX_EVENT_GEN

DESCRIPTION   This function is used to generate coex flow control events.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gflowmgr_coex_event_gen
(
  ds3gflowmgr_recompute_reason_type  recompute_reason,
  ds3gsubsmgr_subs_id_e_type         dds_indx,
  ds3gsubsmgr_subs_id_e_type         hi_prio_indx
)
{
  ds3gflowmgr_coex_event_type  coex_event;
  sys_modem_as_id_e_type       dds_subs_id;
  sys_modem_as_id_e_type       hi_prio_subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_MSG3(MSG_LEGACY_LOW,
            "ds3gflowmgr_coex_event_gen reason:%d dds_indx:%d hi_prio_indx:%d",
            recompute_reason, dds_indx, hi_prio_indx);

  memset(&coex_event, 0, sizeof(ds3gflowmgr_coex_event_type));

  dds_subs_id = ds3gsubsmgr_subs_id_ds3g_to_cm(dds_indx);
  hi_prio_subs_id  = ds3gsubsmgr_subs_id_ds3g_to_cm(hi_prio_indx);

  if( !ds3gsubsmgr_is_subs_id_valid(dds_subs_id) ||
      !ds3gsubsmgr_is_subs_id_valid(hi_prio_subs_id))
  {
    return;
  }

  coex_event.dds_flow_ctrl = ds3gflowmgr_data_flow_status[dds_indx];
  coex_event.data_hi_prio_ind = ds3gflowmgr_data_hi_prio[hi_prio_indx];
  coex_event.coex_detected = ds3gflowmgr_simul_data_flag;
  coex_event.non_dds_ps_call_active
    = (ds3gflowmgr_ps_call_state[hi_prio_indx] == DS3GFLOWMGR_PS_CALL_ACTIVE_EV);


  if( recompute_reason == DS3GFLOWMGR_IRAT_IND )
  {
    coex_event.irat_detected = TRUE;
  }

  if( ds3gflowmgr_timer_running(DS3GFLOWMGR_DISABLE_FLOW_TIMER_ID) )
  {
    coex_event.flow_ctrl_timer_state = (uint16)DS3GFLOWMGR_TIMER_STARTED_EV;
  }
  else
  {
    if( recompute_reason == DS3GFLOWMGR_FLOW_TIMER_EXP )
    {
      coex_event.flow_ctrl_timer_state = (uint16)DS3GFLOWMGR_TIMER_EXPIRED_EV;
    }
    else
    {
      coex_event.flow_ctrl_timer_state = (uint16)DS3GFLOWMGR_TIMER_STOPPED_EV;
    }
  }

  coex_event.flow_ctrl_timer_val = (uint16)ds3gflowmgr_disable_flow_timer.value;
  coex_event.dds_subs_id = dds_subs_id;
  coex_event.dds_sys_mode = ds3g_get_current_network_mode_ex(dds_subs_id);
  coex_event.hi_prio_subs_id = hi_prio_subs_id;
  coex_event.hi_prio_sys_mode = ds3g_get_current_network_mode_ex(hi_prio_subs_id);

  /* sending event to Diag  */
  event_report_payload (EVENT_DS3G_COEX_FLOW_CONTROL,
                        sizeof(ds3gflowmgr_coex_event_type),
                        (void*)&coex_event);

  return;
} /* ds3gflowmgr_coex_event_gen() */

/*===========================================================================
FUNCTION      DS3GFLOWMGR_IS_DATA_HI_PRIO_SET_ON_NON_DDS

DESCRIPTION   This function checks if data high priority set on non-DDS sub

DEPENDENCIES  None.

RETURN VALUE  TRUE:  Data High priority set on Non-DDS Subscription
              FALSE: Data High priority not set on Non-DDS Subscription

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gflowmgr_is_data_hi_prio_set_on_non_dds
(
  ds3gsubsmgr_subs_id_e_type  *subs_indx_p
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx = DS3GSUBSMGR_SUBS_ID_MIN;
  boolean                     ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( subs_indx_p == NULL )
  {
    DATA_3GMGR_MSG0( MSG_LEGACY_LOW, "Null ptr passed");
    return ret_val;
  }

  /* Initialize subs_indx_p to satisfy KW */
  *subs_indx_p = DS3GSUBSMGR_SUBS_ID_MIN;

  for( subs_indx = DS3GSUBSMGR_SUBS_ID_MIN;
       subs_indx < DS3GSUBSMGR_SUBS_ID_MAX;
       subs_indx++ )
  {
    if( subs_indx !=
        ds3gsubsmgr_subs_id_cm_to_ds3g(ds3gsubsmgr_get_def_data_subs_id()) )
    {
      if( ds3gflowmgr_data_hi_prio[subs_indx] == TRUE )
      {
        ret_val = TRUE;
        *subs_indx_p = subs_indx;
        /* Assumption only one data hi-prio on non-DDS */
        break;
      }
    }
  }

  return ret_val;
} /* ds3gflowmgr_is_data_hi_prio_set_on_non_dds() */

/*===========================================================================
FUNCTION      DS3GFLOWMGR_FLOW_ENABLE

DESCRIPTION   This function enables the flow on the subscription

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gflowmgr_flow_enable
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds3gsubsmgr_subs_id_e_type subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                  "ds3gflowmgr_flow_enable subs_id:%d", subs_id);

  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);
  if( ds3gsubsmgr_is_ds3g_subs_index_valid(subs_indx) == FALSE)
  {
    return;
  }

  if( !ds3gflowmgr_data_flow_status[subs_indx] )
  {
    /* If flow disable timer is running, stop it */
    if( ds3gflowmgr_timer_running(DS3GFLOWMGR_DISABLE_FLOW_TIMER_ID) == TRUE )
    {
      ds3gflowmgr_stop_timer(DS3GFLOWMGR_DISABLE_FLOW_TIMER_ID);
    }

    ds3gflowmgr_data_flow_status[subs_indx] = TRUE;
    ds3g_process_flow_enable_ex(subs_id);
  }
  return;
} /* ds3gflowmgr_flow_enable() */

/*===========================================================================
FUNCTION      DS3GFLOWMGR_FLOW_DISABLE

DESCRIPTION   This function disables the flow on the subscription

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gflowmgr_flow_disable
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds3gsubsmgr_subs_id_e_type subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                  "ds3gflowmgr_flow_disable subs_id:%d", subs_id);

  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);
  if( ds3gsubsmgr_is_ds3g_subs_index_valid(subs_indx) == FALSE)
  {
    return;
  }

  if( ds3gflowmgr_data_flow_status[subs_indx] )
  {
    /* If flow disable timer is stopped, start it */
    if( ds3gflowmgr_timer_running(DS3GFLOWMGR_DISABLE_FLOW_TIMER_ID) == FALSE)
    {
      ds3gflowmgr_start_timer(DS3GFLOWMGR_DISABLE_FLOW_TIMER_ID);
    }

    ds3gflowmgr_data_flow_status[subs_indx] = FALSE;
    ds3g_process_flow_disable_ex(subs_id);
  }
  return;
} /* ds3gflowmgr_flow_disable() */

/*===========================================================================
FUNCTION      DS3GFLOWMGR_REG_SIMUL_DATA_IND

DESCRIPTION   This function registers simultaneous data indication with
              MCS.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gflowmgr_reg_simul_data_ind( void )
{
  cxm_coex_desense_ind_req_s  ind_req;
  boolean                     ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gflowmgr_enable_data_flow_control )
  {
    DATA_3GMGR_MSG0( MSG_LEGACY_MED,
                     "Data flow control disabled. Ignore registration req");
    return;
  }

  if( ds3gflowmgr_regd_simul_data_ind )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "ds3gflowmgr_reg_simul_data_ind: Already registered");
    return;
  }

  ind_req.desense_reg.is_reg = TRUE;
  ind_req.desense_reg.umid = DS_MSGRRECV_DESENSE_IND;

  /* Clear simultaneous data allowed flag */
  ds3gflowmgr_simul_data_flag = FALSE;

  ret_val = dsmsgrsnd_msg_send(MCS_CXM_DESENSE_IND_REQ,
                               (msgr_hdr_struct_type*)(&ind_req),
                               sizeof(cxm_coex_desense_ind_req_s));

  if( ret_val )
  {
    /* Set registered for simultaneous data allowed indication */
    ds3gflowmgr_regd_simul_data_ind = TRUE;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                  "ds3gflowmgr_reg_simul_data_ind success:%d",ret_val);

  return;
} /* ds3gflowmgr_reg_simul_data_ind() */

/*===========================================================================
FUNCTION      DS3GFLOWMGR_DEREG_SIMUL_DATA_IND

DESCRIPTION   This function de-registers simultaneous data indication with
              MCS.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gflowmgr_dereg_simul_data_ind( void )
{
  cxm_coex_desense_ind_req_s  ind_req;
  boolean                     ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gflowmgr_enable_data_flow_control )
  {
    DATA_3GMGR_MSG0( MSG_LEGACY_MED,
                     "Data flow control disabled. Ignore de-registration req");
    return;
  }

  if( !ds3gflowmgr_regd_simul_data_ind )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "ds3gflowmgr_dereg_simul_data_ind: Already de-registered");
    return;
  }

  ind_req.desense_reg.is_reg = FALSE;
  ind_req.desense_reg.umid = DS_MSGRRECV_DESENSE_IND;

  ret_val = dsmsgrsnd_msg_send(MCS_CXM_DESENSE_IND_REQ,
                               (msgr_hdr_struct_type*)(&ind_req),
                               sizeof(cxm_coex_desense_ind_req_s));

  if( ret_val )
  {
    /* clear registered for simultaneous data allowed indication */
    ds3gflowmgr_regd_simul_data_ind = FALSE;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                  "ds3gflowmgr_dereg_simul_data_ind success:%d", ret_val);

  /* Clear simultaneous data allowed flag */
  ds3gflowmgr_simul_data_flag = FALSE;

  return;
} /* ds3gflowmgr_dereg_simul_data_ind() */

/*===========================================================================
FUNCTION      DS3GFLOWMGR_RECOMPUTE_FLOW_STATUS

DESCRIPTION   This function runs the algorithm to compute flow status
              based on the current state of variables.

              This function will determine to either enable/disable flow
              on DDS subscription.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gflowmgr_recompute_flow_status
(
  ds3gflowmgr_recompute_reason_type  recompute_reason
)
{
  sys_modem_as_id_e_type      dds_subs_id;
  ds3gsubsmgr_subs_id_e_type  non_dds_indx = DS3GSUBSMGR_SUBS_ID_INVALID;
  boolean                     flow_disable_dds = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG0(MSG_LEGACY_MED, "ds3gflowmgr_recompute_flow_status");

  dds_subs_id = ds3gsubsmgr_get_def_data_subs_id();

  do
  {
    /*-----------------------------------------------------------------------
      Check if data high priority is set for non-DDS subscription
    -----------------------------------------------------------------------*/
    if( ds3gflowmgr_is_data_hi_prio_set_on_non_dds(&non_dds_indx) == FALSE )
    {
      /* Flow enable DDS */
      flow_disable_dds = FALSE;
      ds3gflowmgr_flow_enable( dds_subs_id );

      /* De-register to stop receiving simultaneous data indication */
      ds3gflowmgr_dereg_simul_data_ind();

      break;
    }

    /*-----------------------------------------------------------------------
      Check if PS call active on non-DDS subscription
    -----------------------------------------------------------------------*/
    if( ds3gflowmgr_ps_call_state[non_dds_indx] !=
        DS3GFLOWMGR_PS_CALL_STATE_ACTIVE)
    {
      /* Flow enable DDS */
      flow_disable_dds = FALSE;
      ds3gflowmgr_flow_enable( dds_subs_id );

      /* De-register to stop receiving simultaneous data indication */
      ds3gflowmgr_dereg_simul_data_ind();

      break;
    }


    /*-----------------------------------------------------------------------
      Check if registered to receive simultaneous data allowed indication
    -----------------------------------------------------------------------*/
    if( ds3gflowmgr_regd_simul_data_ind == FALSE )
    {
      flow_disable_dds = FALSE;

      /* register to start receiving simultaneous data indication */
      ds3gflowmgr_reg_simul_data_ind();

      break;
    }

    /*-----------------------------------------------------------------------
      Check if simultaneous data allowed
    -----------------------------------------------------------------------*/
    if( ds3gflowmgr_simul_data_flag == TRUE )
    {
      /* Flow enable DDS */
      flow_disable_dds = FALSE;
      ds3gflowmgr_flow_enable( dds_subs_id );

      break;
    }
  } while(0);

  /*-------------------------------------------------------------------------
    All rules met to disable DDS flow
  -------------------------------------------------------------------------*/
  if( flow_disable_dds )
  {
    /* De-register to stop receiving simultaneous data indication */
    ds3gflowmgr_dereg_simul_data_ind();

    /* Flow disable DDS */
    ds3gflowmgr_flow_disable( dds_subs_id );
  }

  /* QXDM Event Logging */
  ds3gflowmgr_coex_event_gen( recompute_reason,
                              ds3gsubsmgr_subs_id_cm_to_ds3g(dds_subs_id),
                              non_dds_indx);

  return;
} /* ds3gflowmgr_recompute_flow_status() */

/*===========================================================================
FUNCTION       DS3GFLOWMGR_PROCESS_FLOW_DISABLE_TIMER_EXPIRED

DESCRIPTION    Function to process flow disable timer.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
void ds3gflowmgr_process_flow_disable_timer_expired( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds3gflowmgr_enable_data_flow_control )
  {
    DATA_3GMGR_MSG0( MSG_LEGACY_MED,
                     "Data flow control disabled. Ignore flow disable timer "
                     "expired");
    return;
  }

  DATA_3GMGR_MSG0( MSG_LEGACY_MED,
                   "ds3gflowmgr_process_flow_disable_timer_expired" );

  /* Flow enable DDS*/
  ds3gflowmgr_flow_enable( ds3gsubsmgr_get_def_data_subs_id() );

  /* Run algorithm to check if flow needs to be enabled or disabled */
  ds3gflowmgr_recompute_flow_status(DS3GFLOWMGR_FLOW_TIMER_EXP);

  return;
} /* ds3gflowmgr_process_flow_disable_timer_expired() */

/*===========================================================================
FUNCTION      ds3gflowmgr_track_flow_activity

DESCRIPTION   This utitlity function caches the current WM counter values 
              and starts monitor flow timer to track flow activity

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds3gflowmgr_track_flow_activity
(
  sys_modem_as_id_e_type  subs_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gflowmgr_monitor_flow_enabled )
  {
    return;
  }

  if( !ds3gflowmgr_timer_running(DS3GFLOWMGR_MONITOR_FLOW_TIMER_ID) )
  {
    memset(&ds3gflowmgr_flow_activity, 0, sizeof(ds3gflowmgr_flow_activity_type));
    ds3gflowmgr_flow_activity.subs_id = subs_id;
    ds3gflowmgr_flow_activity.total_uldl_wm_cnt
      = ds3g_get_total_uldl_wm_cnt_ex(subs_id);

    ds3gflowmgr_start_timer(DS3GFLOWMGR_MONITOR_FLOW_TIMER_ID);
  }
  return;
} /* ds3gflowmgr_track_flow_activity() */

/*===========================================================================
FUNCTION       DS3GFLOWMGR_PROCESS_MONITOR_FLOW_TIMER_EXPIRED

DESCRIPTION    Function to process monitor flow timer.expired event

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
void ds3gflowmgr_process_monitor_flow_timer_expired( void )
{
  uint64  cur_total_uldl_wm_cnt = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG0( MSG_LEGACY_MED,
                   "ds3gflowmgr_process_flow_disable_timer_expired" );

  /* Get current WM counts */
  cur_total_uldl_wm_cnt
    = ds3g_get_total_uldl_wm_cnt_ex(ds3gflowmgr_flow_activity.subs_id);

  /*-------------------------------------------------------------------------
    Logic
     - If WM count has not changed during the timer value,
       then send PS_CALL_NO_FLOW event which will send LO_PRIO, enable DDS flow
       and bring down physlink on non-DDS sub
     - Else restart the timer to continue tracking flow
  -------------------------------------------------------------------------*/
  if( cur_total_uldl_wm_cnt == ds3gflowmgr_flow_activity.total_uldl_wm_cnt)
  {
    ds3g_process_bringdown_phys_link_ex(ds3gflowmgr_flow_activity.subs_id);

    ds3gflowmgr_notify_ps_call_state_per_sub(ds3gflowmgr_flow_activity.subs_id,
                                             DS3GFLOWMGR_PS_CALL_NO_FLOW_EV);

    /* clear data activity cache */
    memset(&ds3gflowmgr_flow_activity,
           0,
           sizeof(ds3gflowmgr_flow_activity_type));
  }
  else
  {
    ds3gflowmgr_track_flow_activity(ds3gflowmgr_flow_activity.subs_id);
  }

  return;
} /* ds3gflowmgr_process_monitor_flow_timer_expired() */

/*===========================================================================
FUNCTION       DS3GFLOWMGR_DESENSE_IND_HDLR

DESCRIPTION    This function is used to process the DS_MSGRRECV_DESENSE_IND
               message.

DEPENDENCIES   None.

RETURN VALUE   TRUE is processing was successful
               False otherwise

SIDE EFFECTS   None.
===========================================================================*/
boolean ds3gflowmgr_desense_ind_hdlr
(
  msgr_umid_type               msgrtype,
  const msgr_hdr_struct_type  *dsmsg_ptr
)
{
  cxm_coex_desense_ind_s  *msg_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (msgrtype != DS_MSGRRECV_DESENSE_IND) || (dsmsg_ptr == NULL) )
  {
    DATA_3GMGR_MSG2( MSG_LEGACY_LOW,
                     "Invalid msgrtype:%d or invalid dsmsg_ptr:%d",
                     msgrtype, dsmsg_ptr );
    return FALSE;
  }

  msg_ptr = (cxm_coex_desense_ind_s*)dsmsg_ptr;
  ds3gflowmgr_set_simul_data_allowed(!(msg_ptr->desense_ind.is_desense));

  return TRUE;
} /* ds3gflowmgr_desense_ind_hdlr() */

/*===========================================================================
FUNCTION      DS3GFLOWMGR_SUBSMGR_EVENT_CB

DESCRIPTION   This callback is called by DS3GSUBSMGR to notify events.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds3gflowmgr_subsmgr_event_cb
(
  ds3gsubsmgr_event_type        event_id,
  ds3gsubsmgr_event_info_type  *event_info_ptr,
  void                         *data_ptr
)
{
  ds3gsubsmgr_subs_id_e_type  new_subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check if callback pointer is NULL
  -------------------------------------------------------------------------*/
  if(event_info_ptr == NULL)
  {
    DATA_MSG0(MSG_LEGACY_LOW, "NULL event info pointer");
    return;
  }

  DATA_MSG1(MSG_LEGACY_HIGH, "ds3gflowmgr_subsmgr_event_cb event_id:%d",
            event_id);

  switch(event_id)
  {
    case DS3GSUBSMGR_DDS_CHANGE_EV:
      new_subs_indx = event_info_ptr->def_data_subs.curr;

      /* Set Data hi-priority to LOW if new DDS was set to high */
      if( ds3gflowmgr_data_hi_prio[new_subs_indx] )
      {
        ds3gflowmgr_set_data_hi_prio(
           ds3gsubsmgr_subs_id_ds3g_to_cm(new_subs_indx), FALSE);
      }
      break;

    default:
      break;
  }

  return;
}/* ds3gflowmgr_subsmgr_event_cb() */

/*===========================================================================
FUNCTION      ds3gflowmgr_is_dual_data_enabled

DESCRIPTION   This function is used to check if dual data feature is enabled.

DEPENDENCIES  None.

RETURN VALUE  Boolean - True, if feature is enabled
                      - False, otherwise.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gflowmgr_is_dual_data_enabled( void )
{
  boolean                   ret_val = FALSE;
#ifdef FEATURE_DUAL_SIM
  policyman_status_t        status;
  policyman_item_t const    *pItem;
  policyman_item_id_t const id = POLICYMAN_ITEM_DEVICE_CONFIGURATION;
  size_t                    active_data_num = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  status = policyman_get_items_block(&id, 1, &pItem);
  if(POLICYMAN_SUCCEEDED(status))
  {
    status = policyman_device_config_max_active_data(pItem, &active_data_num);
    if( !POLICYMAN_FAILED(status) && 
        active_data_num > DS3GFLOWMGR_SINGLE_DATA_ACTIVE_SUBS )
    {
      ret_val = TRUE;
    }
    policyman_item_release(pItem);
  } 
#endif /* FEATURE_DUAL_SIM */

  return ret_val;
}

/*===========================================================================
FUNCTION      DS3GFLOWMGR_NOTIFY_DATA_PRIORITY_TO_CM

DESCRIPTION   This utitlity function notifies CM regarding Data Priority.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds3gflowmgr_notify_data_priority_to_cm
(
  sys_modem_as_id_e_type  subs_id,
  boolean                 data_hi_prio
)
{
  sys_data_priority_e_type data_pri = SYS_DATA_PRIORITY_LOW;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds3gflowmgr_is_dual_data_enabled)
  {
    /* Don't notify is dual data is not enabled */
    return;	
  }
  
  if (TRUE == data_hi_prio)
  {
    data_pri = SYS_DATA_PRIORITY_HIGH;
  }
  
  if(!cm_msim_set_data_priority(ds3g_get_cm_client_id(),
                                subs_id,
                                data_pri))
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "Error setting CM data hi prio");
  }
  return;
} /* ds3gflowmgr_notify_data_prio_to_cm() */

/*===========================================================================
FUNCTION      DS3GFLOWMGR_NOTIFY_DATA_PRIORITY_TO_MCS

DESCRIPTION   This utitlity function notifies MCS regarding Data Priority.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds3gflowmgr_notify_data_priority_to_mcs( void )
{
  sp_data_call_state_info  sp_info;
  uint8                    as_id_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds3gflowmgr_is_dual_data_enabled)
  {
    /* Don't notify is dual data is not enabled */
    return;	
  }

  memset(&sp_info, 0, sizeof(sp_data_call_state_info));

  /* Loop through all subscription IDs and notify MCS */
  for(as_id_indx = 0; as_id_indx < MAX_AS_IDS; as_id_indx++)
  {
    if( ds3gflowmgr_ps_call_state[as_id_indx] ==
        DS3GFLOWMGR_PS_CALL_STATE_ACTIVE )
    {
      sp_info.subs_st[as_id_indx].state = SP_DATA_CALL_STATE;
    }
    else
    {
      sp_info.subs_st[as_id_indx].state = SP_NO_CALL_STATE;
    }

    sp_info.subs_st[as_id_indx].mode
      = ds3g_get_current_preferred_mode_ex((sys_modem_as_id_e_type)as_id_indx);

    sp_info.subs_st[as_id_indx].priority
      = (sys_data_priority_e_type)ds3gflowmgr_data_hi_prio[as_id_indx];

    DATA_3GMGR_MSG4(MSG_LEGACY_HIGH, "Notifying MCS. as_id_indx: %d, call_state: %d, mode: %d, priority: %d",
		    as_id_indx, 
		    sp_info.subs_st[as_id_indx].state, 
		    sp_info.subs_st[as_id_indx].mode,
		    sp_info.subs_st[as_id_indx].priority);
  }  

  if( !sp_data_call_state_update(sp_info) )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "Error setting MCS data hi prio");
  }

  return;
} /* ds3gflowmgr_notify_data_priority_to_mcs() */

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
void ds3gflowmgr_init( void )
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
  ds3gflowmgr_nv_type         flowmgr_nv_item;
  ds3gflowmgr_monitor_flow_nv_type  chk_flow_status_nv_item;
  nv_stat_enum_type           nv_read_retval;
  ds3gsubsmgr_client_type     client_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "ds3gflowmgr_init");

  /*-------------------------------------------------------------------------
    Initializing global variables
  -------------------------------------------------------------------------*/
  ds3gflowmgr_regd_simul_data_ind = FALSE;
  ds3gflowmgr_simul_data_flag     = FALSE;

  memset(&ds3gflowmgr_disable_flow_timer,
         0,
         sizeof(ds3gflowmgr_timer_info_type));
  memset(&ds3gflowmgr_monitor_flow_timer,
         0,
         sizeof(ds3gflowmgr_timer_info_type));

  for( subs_indx = DS3GSUBSMGR_SUBS_ID_MIN;
       subs_indx < DS3GSUBSMGR_SUBS_ID_MAX;
       subs_indx++ )
  {
    ds3gflowmgr_data_hi_prio[subs_indx]     = FALSE;
    ds3gflowmgr_ps_call_state[subs_indx]    = DS3GFLOWMGR_PS_CALL_STATE_NO_CALL;
    ds3gflowmgr_data_flow_status[subs_indx] = TRUE;
  }

  /*-------------------------------------------------------------------------
    Define disable flow timer and initialize to default value
  -------------------------------------------------------------------------*/
  rex_def_timer_ex(&(ds3gflowmgr_disable_flow_timer.timer),
                   ds3gflowmgr_timer_cback,
                   DS3GFLOWMGR_DISABLE_FLOW_TIMER_ID);

  /*-------------------------------------------------------------------------
    Read NV item if set
  -------------------------------------------------------------------------*/
  nv_read_retval = ds3gcfgmgr_read_efs_nv(DS3GFLOWMGR_NV_PATH,
                                          &flowmgr_nv_item,
                                          sizeof(flowmgr_nv_item));
  if( NV_DONE_S == nv_read_retval )
  {
    ds3gflowmgr_enable_data_flow_control = flowmgr_nv_item.enable_flow_control;
    ds3gflowmgr_disable_flow_timer.value = flowmgr_nv_item.timer_val;
  }
  else
  {
    ds3gflowmgr_enable_data_flow_control = FALSE;
    ds3gflowmgr_disable_flow_timer.value = DS3GFLOWMGR_DISABLE_FLOW_TIMER_VAL;
  }

  /*-------------------------------------------------------------------------
    Define monitor flow timer and initialize to default value
  -------------------------------------------------------------------------*/
  rex_def_timer_ex(&(ds3gflowmgr_monitor_flow_timer.timer),
                   ds3gflowmgr_timer_cback,
                   DS3GFLOWMGR_MONITOR_FLOW_TIMER_ID);

  /*-------------------------------------------------------------------------
    Read NV item if set
  -------------------------------------------------------------------------*/
  nv_read_retval
    = ds3gcfgmgr_read_efs_nv(DS3GFLOWMGR_MONITOR_FLOW_NV_PATH,
                             &chk_flow_status_nv_item,
                             sizeof(ds3gflowmgr_monitor_flow_nv_type));
  if( NV_DONE_S == nv_read_retval )
  {
    ds3gflowmgr_monitor_flow_enabled
      = chk_flow_status_nv_item.enable_monitor_flow;

    if( chk_flow_status_nv_item.timer_val > 0 )
    {
      ds3gflowmgr_monitor_flow_timer.value
        = chk_flow_status_nv_item.timer_val;
    }
    else
    {
      ds3gflowmgr_monitor_flow_timer.value = DS3GFLOWMGR_MONITOR_FLOW_TIMER_VAL;
    }
  }
  else
  {
    ds3gflowmgr_monitor_flow_enabled = FALSE;
    ds3gflowmgr_monitor_flow_timer.value = DS3GFLOWMGR_MONITOR_FLOW_TIMER_VAL;
  }

  /*----------------------------------------------------------------------------
    Register for all messages that DS3G Flow Mgr is interested in receiving
  ----------------------------------------------------------------------------*/
  dsmsgrrcv_reg_hdlr(DS_MSGRRECV_DESENSE_IND,
                     (dsmsgrcv_msg_hdlr_f)ds3gflowmgr_desense_ind_hdlr);

  if(ds3gsubsmgr_event_reg(DS3GSUBSMGR_DDS_CHANGE_EV,
                           ds3gflowmgr_subsmgr_event_cb,
                           NULL,
                           &client_id) == FALSE)
  {
    DATA_ERR_FATAL("DDS change ev registration failed.");
    return;
  }

  return;
} /* ds3gflowmgr_init() */

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
)
{
  ds3gsubsmgr_subs_id_e_type subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If device mode is single sim or single standby, return TRUE */
  if((ds3gsubsmgr_is_device_mode_single_sim()  == TRUE) ||
     (ds3gsubsmgr_get_standby_pref() == SYS_MODEM_DS_PREF_SINGLE_STANDBY) )
  {
    DATA_3GMGR_MSG0( MSG_LEGACY_LOW,
                     "Single sim or single standby. Return flow enabled");
    return TRUE;
  }

  /* If coex flow control is disabled, return TRUE */
  if( !ds3gflowmgr_enable_data_flow_control )
  {
    DATA_3GMGR_MSG0( MSG_LEGACY_LOW,
                     "Data flow control disabled. Return flow enabled");
    return TRUE;
  }

  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if( ds3gsubsmgr_is_ds3g_subs_index_valid(subs_indx) == FALSE)
  {
    return FALSE;
  }

  return ds3gflowmgr_data_flow_status[subs_indx];
} /* ds3gflowmgr_get_coex_data_flow_status() */

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
)
{
  ds3gsubsmgr_subs_id_e_type subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if( ds3gsubsmgr_is_ds3g_subs_index_valid(subs_indx) == FALSE)
  {
    return FALSE;
  }

  return ds3gflowmgr_data_hi_prio[subs_indx];
} /* ds3gflowmgr_get_data_hi_prio() */

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
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                  "ds3gflowmgr_set_data_hi_prio subs_id:%d data_hi_prio:%d",
                  subs_id, data_hi_prio);

  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);
  if( ds3gsubsmgr_is_ds3g_subs_index_valid(subs_indx) == FALSE)
  {
    return;
  }

  if( ds3gflowmgr_data_hi_prio[subs_indx] != data_hi_prio )
  {
    DATA_3GMGR_MSG3(MSG_LEGACY_HIGH,
                    "curr data_hi_prio:%d != new data_hi_prio:%d, subs_id:%d",
                    ds3gflowmgr_data_hi_prio[subs_indx],
                    data_hi_prio,
                    subs_id);

    ds3gflowmgr_data_hi_prio[subs_indx] = data_hi_prio;

    /* Run algorithm to check if flow needs to be enabled or disabled */
    ds3gflowmgr_recompute_flow_status(DS3GFLOWMGR_HI_PRIO_CHG);

    /* Notify CM about data high priority */
    ds3gflowmgr_notify_data_priority_to_cm(subs_id, data_hi_prio);
  }

  return;
} /* ds3gflowmgr_set_data_hi_prio()*/

/*===========================================================================
FUNCTION      DS3GFLOWMGR_GET_SIMUL_DATA_ALLOWED

DESCRIPTION   This function returns if Simultaneous data calls are allowed
              or not.

DEPENDENCIES  None.

RETURN VALUE  TRUE:  Simultaneous data call allowed
              FALSE: Simultaneous data call not allowed

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3gflowmgr_get_simul_data_allowed( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds3gflowmgr_simul_data_flag;
} /* ds3gflowmgr_get_simul_data_allowed() */

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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( !ds3gflowmgr_enable_data_flow_control )
  {
    DATA_3GMGR_MSG0( MSG_LEGACY_MED,
                     "Data flow control disabled. Ignore set simul data "
                     "allowed");
    return;
  }

  DATA_3GMGR_MSG1(MSG_LEGACY_HIGH,
                  "ds3gflowmgr_set_simul_data_allowed simul_data_allowed:%d",
                  simul_data_allowed);

  ds3gflowmgr_simul_data_flag = simul_data_allowed;

  /* Run algorithm to check if flow needs to be enabled or disabled */
  ds3gflowmgr_recompute_flow_status(DS3GFLOWMGR_COEX_CHG);

  return;
} /* ds3gflowmgr_set_simul_data_allowed() */

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
)
{
  ds3gsubsmgr_subs_id_e_type      subs_indx;
  boolean                         is_non_dds_sub = FALSE;
  ds3gflowmgr_ps_call_state_type  old_ps_call_state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                  "ds3gflowmgr_notify_ps_call_state_per_sub "
                  "subs_id:%d ps_call_event:%d",
                  subs_id, ps_call_event);

  /*-------------------------------------------------------------------------
    Validate subs_id and get ds3g subs index
  -------------------------------------------------------------------------*/
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);
  if( ds3gsubsmgr_is_ds3g_subs_index_valid(subs_indx) == FALSE )
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Validate event
  -------------------------------------------------------------------------*/
  if( (ps_call_event < DS3GFLOWMGR_PS_CALL_MIN_EV) ||
      (ps_call_event > DS3GFLOWMGR_PS_CALL_MAX_EV) )
  {
    DATA_3GMGR_MSG1( MSG_LEGACY_LOW, "invalid ps call event:%d return error",
                     ps_call_event);
    return;
  }

  is_non_dds_sub = !ds3gsubsmgr_is_def_data_subs_id(subs_id);
  old_ps_call_state = ds3gflowmgr_ps_call_state[subs_indx];

  /*-------------------------------------------------------------------------
    State transition for PS call state.

    ORIG event handling needs to be added in ACTIVE, DORMANT and NO_FLOW
    states. The reason being, for non-dds sub LTE Attach would happen
    with internet PDN. Once Internet PDN is UP, flow monitor timer will
    start and this could lead to lowering down the priority of non-dds sub
    before IMS PDN can be connected. This would lead to TRM being 
    relinquished and IMS regsitration to fail.
  -------------------------------------------------------------------------*/
  switch( ds3gflowmgr_ps_call_state[subs_indx] )
  {
    case DS3GFLOWMGR_PS_CALL_STATE_NO_CALL:
      if( ps_call_event == DS3GFLOWMGR_PS_CALL_ORIG_EV )
      {
        ds3gflowmgr_ps_call_state[subs_indx]
          = DS3GFLOWMGR_PS_CALL_STATE_ORIG;

        if( is_non_dds_sub )
        {
          ds3gflowmgr_set_data_hi_prio(subs_id, TRUE);
        }
        ds3gflowmgr_notify_data_priority_to_mcs();
      }
      /* TODO: check if this transition is possible */
      else if( ps_call_event == DS3GFLOWMGR_PS_CALL_ACTIVE_EV )
      {
        ds3gflowmgr_ps_call_state[subs_indx]
          = DS3GFLOWMGR_PS_CALL_STATE_ACTIVE;

        if( is_non_dds_sub )
        {
          ds3gflowmgr_track_flow_activity(subs_id);
          ds3gflowmgr_set_data_hi_prio(subs_id, TRUE);
        }
        ds3gflowmgr_notify_data_priority_to_mcs();
      }
      break;

    case DS3GFLOWMGR_PS_CALL_STATE_ORIG:
      if( ps_call_event == DS3GFLOWMGR_PS_CALL_END_EV )
      {
        ds3gflowmgr_ps_call_state[subs_indx]
          = DS3GFLOWMGR_PS_CALL_STATE_NO_CALL;

        if( is_non_dds_sub )
        {
          ds3gflowmgr_set_data_hi_prio(subs_id, FALSE);
        }
        ds3gflowmgr_notify_data_priority_to_mcs();
      }
      else if( ps_call_event == DS3GFLOWMGR_PS_CALL_ACTIVE_EV )
      {
        ds3gflowmgr_ps_call_state[subs_indx]
          = DS3GFLOWMGR_PS_CALL_STATE_ACTIVE;

        if( is_non_dds_sub )
        {
          ds3gflowmgr_track_flow_activity(subs_id);
          ds3gflowmgr_set_data_hi_prio(subs_id, TRUE);
        }
        ds3gflowmgr_notify_data_priority_to_mcs();
      }
      break;

    case DS3GFLOWMGR_PS_CALL_STATE_ACTIVE:
      if ( ps_call_event == DS3GFLOWMGR_PS_CALL_ORIG_EV )
      {
        ds3gflowmgr_ps_call_state[subs_indx]
                       = DS3GFLOWMGR_PS_CALL_STATE_ORIG;

        if( is_non_dds_sub )
        {
          ds3gflowmgr_stop_timer(DS3GFLOWMGR_MONITOR_FLOW_TIMER_ID);
          ds3gflowmgr_set_data_hi_prio(subs_id, TRUE);
        }
        ds3gflowmgr_notify_data_priority_to_mcs();
      }
      else if( ps_call_event == DS3GFLOWMGR_PS_CALL_END_EV )
      {
        ds3gflowmgr_ps_call_state[subs_indx]
          = DS3GFLOWMGR_PS_CALL_STATE_NO_CALL;

        if( is_non_dds_sub )
        {
          ds3gflowmgr_stop_timer(DS3GFLOWMGR_MONITOR_FLOW_TIMER_ID);
          ds3gflowmgr_set_data_hi_prio(subs_id, FALSE);
        }
        ds3gflowmgr_notify_data_priority_to_mcs();
      }
      else if( ps_call_event == DS3GFLOWMGR_PS_CALL_DORMANT_EV )
      {
        ds3gflowmgr_ps_call_state[subs_indx]
          = DS3GFLOWMGR_PS_CALL_STATE_DORMANT;

        if( is_non_dds_sub )
        {
          ds3gflowmgr_stop_timer(DS3GFLOWMGR_MONITOR_FLOW_TIMER_ID);
          ds3gflowmgr_set_data_hi_prio(subs_id, FALSE);
        }
        ds3gflowmgr_notify_data_priority_to_mcs();
      }
      else if( ps_call_event == DS3GFLOWMGR_PS_CALL_NO_FLOW_EV )
      {
        ds3gflowmgr_ps_call_state[subs_indx]
          = DS3GFLOWMGR_PS_CALL_STATE_NO_FLOW;

        if( is_non_dds_sub )
        {
          ds3gflowmgr_set_data_hi_prio(subs_id, FALSE);
        }
        ds3gflowmgr_notify_data_priority_to_mcs();
      }
      break;

    case DS3GFLOWMGR_PS_CALL_STATE_DORMANT:
      if ( ps_call_event == DS3GFLOWMGR_PS_CALL_ORIG_EV )
      {
        ds3gflowmgr_ps_call_state[subs_indx]
                = DS3GFLOWMGR_PS_CALL_STATE_ORIG;

        if( is_non_dds_sub )
        {
          ds3gflowmgr_stop_timer(DS3GFLOWMGR_MONITOR_FLOW_TIMER_ID);
          ds3gflowmgr_set_data_hi_prio(subs_id, TRUE);
        }
        ds3gflowmgr_notify_data_priority_to_mcs();
      }
      else if( ps_call_event == DS3GFLOWMGR_PS_CALL_END_EV )
      {
        ds3gflowmgr_ps_call_state[subs_indx]
          = DS3GFLOWMGR_PS_CALL_STATE_NO_CALL;

        if( is_non_dds_sub )
        {
          ds3gflowmgr_set_data_hi_prio(subs_id, FALSE);
        }
        ds3gflowmgr_notify_data_priority_to_mcs();
      }
      else if( ps_call_event == DS3GFLOWMGR_PS_CALL_ACTIVE_EV )
      {
        ds3gflowmgr_ps_call_state[subs_indx]
          = DS3GFLOWMGR_PS_CALL_STATE_ACTIVE;

        if( is_non_dds_sub )
        {
          ds3gflowmgr_track_flow_activity(subs_id);
          ds3gflowmgr_set_data_hi_prio(subs_id, TRUE);
        }
        ds3gflowmgr_notify_data_priority_to_mcs();
      }
      break;

    case DS3GFLOWMGR_PS_CALL_STATE_NO_FLOW:
      if ( ps_call_event == DS3GFLOWMGR_PS_CALL_ORIG_EV )
      {
        ds3gflowmgr_ps_call_state[subs_indx]
                   = DS3GFLOWMGR_PS_CALL_STATE_ORIG;

        if( is_non_dds_sub )
        {
          ds3gflowmgr_stop_timer(DS3GFLOWMGR_MONITOR_FLOW_TIMER_ID);
          ds3gflowmgr_set_data_hi_prio(subs_id, TRUE);
        }
        ds3gflowmgr_notify_data_priority_to_mcs();
      }
      else if( ps_call_event == DS3GFLOWMGR_PS_CALL_END_EV )
      {
        ds3gflowmgr_ps_call_state[subs_indx]
          = DS3GFLOWMGR_PS_CALL_STATE_NO_CALL;

        if( is_non_dds_sub )
        {
          ds3gflowmgr_set_data_hi_prio(subs_id, FALSE);
        }
        ds3gflowmgr_notify_data_priority_to_mcs();
      }
      else if( ps_call_event == DS3GFLOWMGR_PS_CALL_DORMANT_EV )
      {
        ds3gflowmgr_ps_call_state[subs_indx]
          = DS3GFLOWMGR_PS_CALL_STATE_DORMANT;
      }
      else if( ps_call_event == DS3GFLOWMGR_PS_CALL_ACTIVE_EV )
      {
        ds3gflowmgr_ps_call_state[subs_indx]
          = DS3GFLOWMGR_PS_CALL_STATE_ACTIVE;

        if( is_non_dds_sub )
        {
          ds3gflowmgr_track_flow_activity(subs_id);
          ds3gflowmgr_set_data_hi_prio(subs_id, TRUE);
        }
        ds3gflowmgr_notify_data_priority_to_mcs();
      }
      break;

    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid ps_call_state: %d",
                      ds3gflowmgr_ps_call_state[subs_indx]);
      break;
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                  "Old_ps_call_state:%d New_ps_call_state:%d",
                  old_ps_call_state, ds3gflowmgr_ps_call_state[subs_indx]);
  return;
} /* ds3gflowmgr_notify_ps_call_state_per_sub() */

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
)
{
  unsigned long  timer_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  timer_id = *(unsigned long*)(cmd_ptr->cmd_payload_ptr);

  switch( timer_id )
  {
    case DS3GFLOWMGR_DISABLE_FLOW_TIMER_ID:
      ds3gflowmgr_process_flow_disable_timer_expired();
      break;

    case DS3GFLOWMGR_MONITOR_FLOW_TIMER_ID:
      ds3gflowmgr_process_monitor_flow_timer_expired();
      break;

    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "invalid timer_id:%d", timer_id);
      break;
  }

  return;
} /* ds3gflowmgr_process_timer_expired_cmd() */

/*===========================================================================
FUNCTION      ds3gflowmgr_notify_irat_ind

DESCRIPTION   This function is used by DSD to notify IRAT indication on
              either subscription.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gflowmgr_notify_irat_ind( void )
{
  ds3gsubsmgr_subs_id_e_type  non_dds_indx = DS3GSUBSMGR_SUBS_ID_INVALID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gflowmgr_enable_data_flow_control )
  {
    DATA_3GMGR_MSG0( MSG_LEGACY_MED,
                     "Data flow control disabled. Ignore IRAT indication");
    return;
  }

  /* Ignore indication if hi-prio ind not set on non-DDS*/
  if( ds3gflowmgr_is_data_hi_prio_set_on_non_dds(&non_dds_indx) == FALSE )
  {
    DATA_3GMGR_MSG0( MSG_LEGACY_MED,
                     "Data hi prio not set on non-DDS. Ignore IRAT ind");
    return;
  }

  /* Flow enable DDS*/
  ds3gflowmgr_flow_enable( ds3gsubsmgr_get_def_data_subs_id() );

  /* Run algorithm to check if flow needs to be enabled or disabled */
  ds3gflowmgr_recompute_flow_status(DS3GFLOWMGR_IRAT_IND);

  return;
}/* ds3gflowmgr_notify_irat_ind() */


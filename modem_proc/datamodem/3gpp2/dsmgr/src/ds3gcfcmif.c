/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             3G  DATA  SERVICES  CFCM INTERFACE

GENERAL DESCRIPTION
  This software unit acts as interface to the MCS CFCM module.

Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3gcfcmif.c#1 $ 
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

#include "ds3gcfcmif.h"
#include "ds3geventmgr.h"
#include "ps_sys.h"
#include "ps_sys_conf.h"
#include "data_msg.h"
/*===========================================================================

                        LOCAL DATA STRUCTURES

===========================================================================*/

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Mitigatation Level state-machine
---------------------------------------------------------------------------*/
static ds3gcfcmif_ml_state_type ds3gcfcmif_ml_sm = DS3GCFCMIF_ML_STATE_MIN;


/*===========================================================================

                    INTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION      ds3gcfcmif_mit_action_convert_ds3g_to_ps

DESCRIPTION   This utility function converts DS3G Thermal Mitigation action 
              to PS_SYS.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
ps_sys_thermal_mitigation_action_enum_type ds3gcfcmif_mit_action_convert_ds3g_to_ps
(
  ds3geventmgr_th_mit_action_type ds3g_action
)
{
  ps_sys_thermal_mitigation_action_enum_type  ps_sys_action;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch( ds3g_action )
  {
    case DS3GEVENTMGR_TH_MIT_ALLOW_ALL_CALLS:
      ps_sys_action = PS_SYS_THERMAL_MITIGATION_ALL_CALLS_ALLOWED;
      break;

    case DS3GEVENTMGR_TH_MIT_ALLOW_ONLY_IMS_CALLS:
      ps_sys_action = PS_SYS_THERMAL_MITIGATION_IMS_CALLS_ONLY;
      break;

    case DS3GEVENTMGR_TH_MIT_TEAR_DOWN_BLOCK_ALL_CALLS:
      ps_sys_action = PS_SYS_THERMAL_MITIGATION_NO_CALLS_ALLOWED;
      break;

    default:
      ps_sys_action = PS_SYS_THERMAL_MITIGATION_ALL_CALLS_ALLOWED;
      break;
  }
  return ps_sys_action;
} /* ds3gcfcmif_mit_action_convert_ds3g_to_ps() */

/*===========================================================================
FUNCTION      DS3GCFCMIF_THERM_RUNAWAY_CB

DESCRIPTION   This callback function will be called by CFCM to notify thermal 
              runaway indications.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gcfcmif_therm_runaway_cb
(
  cfcm_cmd_type_s  *therm_runaway_info_p
)
{
  ds_cmd_type      *cmd_ptr = NULL;
  cfcm_cmd_type_s  *data_block_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( therm_runaway_info_p == NULL )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Null Ptr. Return");
    return;
  }

  if( therm_runaway_info_p->client_id != CFCM_CLIENT_DS )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "callback for wrong client_id:%d",
                    therm_runaway_info_p->client_id);
    return;
  }

  /*-------------------------------------------------------------------------
    Post a command to DS task to process thermal runaway indication
  -------------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(cfcm_cmd_type_s));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_ERR_FATAL("Could not allocate DS cmd");
    return;
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_CFCM_THERMAL_RUNAWAY_IND;
    data_block_ptr = (cfcm_cmd_type_s*)cmd_ptr->cmd_payload_ptr;
    memscpy(data_block_ptr, sizeof(cfcm_cmd_type_s), 
            therm_runaway_info_p, sizeof(cfcm_cmd_type_s));
    ds_put_cmd(cmd_ptr);
  }

  return;
} /* ds3gcfcmif_therm_runaway_cb() */

/*===========================================================================
FUNCTION      DS3GCFCMIF_ML_STATE_TRANSITION

DESCRIPTION   This is the state transition function for mitigation level 
              statemachine.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gcfcmif_ml_state_transition
(
  ds3gcfcmif_ml_state_type  ml_state
)
{
  ds3geventmgr_event_info_type                event_info;
  ps_sys_thermal_mitigation_action_enum_type  ps_action;
  ps_sys_subscription_enum_type               ps_subs_id_indx;
  int16                                       ps_errno;
  ds3geventmgr_filter_type                    filter_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&event_info,0,sizeof(ds3geventmgr_event_info_type));
  memset(&filter_info, 0, sizeof(ds3geventmgr_filter_type));

  /*-------------------------------------------------------------------------
    State transition ML state machine
  -------------------------------------------------------------------------*/
  switch( ds3gcfcmif_ml_sm )
  {
    case DS3GCFCMIF_ML_STATE_ML0:
    {
      switch( ml_state )
      {
        case DS3GCFCMIF_ML_STATE_ML1:
        {
          event_info.thermal_mitigation_info.action
            = DS3GEVENTMGR_TH_MIT_ALLOW_ONLY_IMS_CALLS;
        }
        break;

        case DS3GCFCMIF_ML_STATE_ML2:
        case DS3GCFCMIF_ML_STATE_ML3:
        {
          event_info.thermal_mitigation_info.action
            = DS3GEVENTMGR_TH_MIT_TEAR_DOWN_BLOCK_ALL_CALLS;
        }
        break;

        default:
        break;
      }
    }
    break;

    case DS3GCFCMIF_ML_STATE_ML1:
    {
      switch( ml_state )
      {
        case DS3GCFCMIF_ML_STATE_ML0:
        {
          event_info.thermal_mitigation_info.action
            = DS3GEVENTMGR_TH_MIT_ALLOW_ALL_CALLS;
        }
        break;

        case DS3GCFCMIF_ML_STATE_ML2:
        case DS3GCFCMIF_ML_STATE_ML3:
        {
          event_info.thermal_mitigation_info.action
            = DS3GEVENTMGR_TH_MIT_TEAR_DOWN_BLOCK_ALL_CALLS;
        }
        break;

        default:
        break;
      }
    }
    break;

    case DS3GCFCMIF_ML_STATE_ML2:
    case DS3GCFCMIF_ML_STATE_ML3:
    {
      switch( ml_state )
      {
        case DS3GCFCMIF_ML_STATE_ML0:
        {
          event_info.thermal_mitigation_info.action
            = DS3GEVENTMGR_TH_MIT_ALLOW_ALL_CALLS;
        }
        break;

        case DS3GCFCMIF_ML_STATE_ML1:
        {
          event_info.thermal_mitigation_info.action
            = DS3GEVENTMGR_TH_MIT_ALLOW_ONLY_IMS_CALLS;
        }
        break;

        /* Same action for ML2->ML3 or ML3->ML2: tear down and block all */
        default:
        {
          event_info.thermal_mitigation_info.action
            = DS3GEVENTMGR_TH_MIT_TEAR_DOWN_BLOCK_ALL_CALLS;
        }
        break;
      }
    }
    break;

    default:
    break;
  }

  /*-------------------------------------------------------------------------
    Set global statemachine value
  -------------------------------------------------------------------------*/
  ds3gcfcmif_ml_sm = ml_state;

  /*-------------------------------------------------------------------------
    Notify all registered clients via ds3geventmgr
  -------------------------------------------------------------------------*/
  DATA_3GMGR_MSG2(MSG_LEGACY_LOW, "Notifying clients event:%d action:%d",
                  DS3GEVENTMGR_THERMAL_MITIGATION_EV,
                  event_info.thermal_mitigation_info.action);

  /*-------------------------------------------------------------------------
    Notify PS_SYS framework
  -------------------------------------------------------------------------*/
  ps_action = ds3gcfcmif_mit_action_convert_ds3g_to_ps(
                event_info.thermal_mitigation_info.action);

  filter_info.tech = PS_SYS_TECH_ALL;

  for( ps_subs_id_indx = PS_SYS_PRIMARY_SUBS;
       ps_subs_id_indx < PS_SYS_SUBS_MAX;
       ps_subs_id_indx++)
  {
    filter_info.ps_subs_id = ps_subs_id_indx;

    ds3geventmgr_set_event_info(DS3GEVENTMGR_THERMAL_MITIGATION_EV,
                                (void*)&event_info,
                                &filter_info);

    ps_sys_conf_set_ex(PS_SYS_TECH_ALL,
                       PS_SYS_CONF_THERMAL_MITIGATION_INFO,
                       ps_subs_id_indx,
                       (void *)&ps_action,
                       &ps_errno);
  }

  return;
} /* ds3gcfcmif_ml_state_transition() */

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GCFCMIF_INIT

DESCRIPTION   This function initializes the DS3G CFCM IF module. This would
              be invoked during dstask init.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gcfcmif_init( void )
{
  cfcm_reg_req_type_s  register_req_msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "ds3gcfcmif_init");

  /*-------------------------------------------------------------------------
    Initializing statemachine
  -------------------------------------------------------------------------*/
  ds3gcfcmif_ml_sm = DS3GCFCMIF_ML_STATE_MIN;

  /*-------------------------------------------------------------------------
    Register with CFCM
  -------------------------------------------------------------------------*/
  register_req_msg.client_id    = CFCM_CLIENT_DS;
  register_req_msg.monitor_mask = CFCM_MONITOR_MASK_THERMAL_RUNAWAY|
                                  CFCM_MONITOR_MASK_THERMAL_RUNAWAY_EX;
  register_req_msg.req_umid     = 0x00;
  register_req_msg.req_cb       = ds3gcfcmif_therm_runaway_cb;
  cfcm_client_register(&register_req_msg);

  return;
} /* ds3gcfcmif_init() */

/*===========================================================================
FUNCTION      DS3GCFCMIF_GET_ML_SM_STATE

DESCRIPTION   This function returns the state of Thermal Runaway Mitigation 
              level state machine

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
ds3gcfcmif_ml_state_type ds3gcfcmif_get_ml_sm_state( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds3gcfcmif_ml_sm;
}/* ds3gcfcmif_get_ml_sm_state() */

/*===========================================================================
FUNCTION      DS3GCFCMIF_PROCESS_THERMAL_RUNAWAY_IND_CMD

DESCRIPTION   This function processes the thermal runaway indication command.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gcfcmif_process_thermal_runaway_ind_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  cfcm_cmd_type_s  *cfcm_cmd_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "Null ptr passed");
    return;
  }

  cfcm_cmd_p = (cfcm_cmd_type_s*)(cmd_ptr->cmd_payload_ptr);
  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,
                  "ds3gcfcmif_process_thermal_runaway_ind_cmd "
                  "cmd:%d monitors_active:%d",
                  cfcm_cmd_p->cmd, cfcm_cmd_p->monitors_active);

  /*-------------------------------------------------------------------------
    Handle CFCM commands
  -------------------------------------------------------------------------*/
  switch( cfcm_cmd_p->cmd )
  {
    /* ML0 mitigation */
    case CFCM_CMD_UP:
      if( ds3gcfcmif_ml_sm != DS3GCFCMIF_ML_STATE_ML0 )
      {
        ds3gcfcmif_ml_state_transition( DS3GCFCMIF_ML_STATE_ML0 );
      }
      break;

    /* ML3 mitigation */
    case CFCM_CMD_SHUT_DOWN:
      if( ds3gcfcmif_ml_sm != DS3GCFCMIF_ML_STATE_ML3 )
      {
        ds3gcfcmif_ml_state_transition( DS3GCFCMIF_ML_STATE_ML3 );
      }
      break;

    /* ML1 or ML2 mitigation */
    case CFCM_CMD_DOWN:
      if( cfcm_cmd_p->monitors_active & CFCM_MONITOR_MASK_THERMAL_RUNAWAY_EX )
      {
        if( ds3gcfcmif_ml_sm != DS3GCFCMIF_ML_STATE_ML2 )
        {
          ds3gcfcmif_ml_state_transition( DS3GCFCMIF_ML_STATE_ML2 );
        }
      }
      else
      {
        if( ds3gcfcmif_ml_sm != DS3GCFCMIF_ML_STATE_ML1 )
        {
          ds3gcfcmif_ml_state_transition( DS3GCFCMIF_ML_STATE_ML1 );
        }
      }
      break;

    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Unhandled CFCM cmd:%d",
                      cfcm_cmd_p->cmd );
      break;
  }

  return;
} /* ds3gcfcmif_process_thermal_runaway_ind_cmd() */


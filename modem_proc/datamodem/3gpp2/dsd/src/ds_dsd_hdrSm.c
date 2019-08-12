/*===========================================================================
                     D S _ D S D _ H D R S M . C

DESCRIPTION


EXTERNALIZED FUNCTIONS


 Copyright (c)2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_hdrSm.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/11/11    sk    Created Module

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "data_msg.h"
#include "ds_dsd_event_dp.h"
#include "ds_dsd_proc.h"
#include "ds_dsd_hdrSm.h"
#include "ds_dsd_timer.h"

/*===========================================================================

                          GLOBAL DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                        LOCAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_HDRSM_EVENT_NOTIFY_CB

DESCRIPTION   This function is the callback function called from Event
              dispatch module.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  SO and RAT mask of the RADIO are updated
===========================================================================*/
boolean ds_dsd_hdrSm_event_notify_cb
(
  dsd_event_type              notified_event,
  dsd_event_client_type       client,
  dsd_radio_type              source_radio,
  ds3gsubsmgr_subs_id_e_type  subs_id,
  void                       *aux_info_ptr
)
{
  ds_dsd_aux_info_type       *aux_info = NULL;
  ds_dsd_sm_type             *sm_ptr = NULL;
  ds_dsd_sm_state_enum_type   lte_radio_state;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*---------------------------------------------------------------------
    Validate Event Info
  ---------------------------------------------------------------------*/
  if(FALSE == ds_dsd_common_validate_event(notified_event,
                                           client,
                                           DS_DSD_CLIENT_HDR,
                                           subs_id,
                                           aux_info_ptr))
  {
    return FALSE;
  }

  /*---------------------------------------------------------------------
    Get statemachine pointer
  ---------------------------------------------------------------------*/
  sm_ptr = ds_dsd_proc_get_radio_sm_instance(subs_id, DS_DSD_RADIO_HDR);
  if(sm_ptr == NULL)
  {
    DATA_MSG0(MSG_LEGACY_LOW, "Invalid statemachine ptr. return");
    return FALSE;
  }

  /*---------------------------------------------------------------------
    Filter events applicable for State machine
  ----------------------------------------------------------------------*/
  switch(notified_event)
  {
    case DSD_ACQ_RADIO_EV:
    case DSD_OOS_RADIO_EV:
      if(source_radio == DS_DSD_RADIO_HDR)
      {
        /*-------------------------------------------------------------------
          Extract radio info and store.
        -------------------------------------------------------------------*/
        sm_ptr->radio_info_prev.srv_status = sm_ptr->radio_info.srv_status;
        sm_ptr->radio_info_prev.srv_domain = sm_ptr->radio_info.srv_domain;

        aux_info = (ds_dsd_aux_info_type *)aux_info_ptr;
        sm_ptr->radio_info.srv_status      = aux_info->attach_radio.srv_status;
        sm_ptr->radio_info.srv_domain      = aux_info->attach_radio.srv_domain;

        /* Store HDR RAT & SO mask if acquired event */
        if(notified_event == DSD_ACQ_RADIO_EV)
        {
          sm_ptr->radio_info_prev.rat_mask = sm_ptr->radio_info.rat_mask;
          sm_ptr->radio_info_prev.so_mask  = sm_ptr->radio_info.so_mask;
          sm_ptr->radio_info.rat_mask      = aux_info->hdr_session.rat_mask;
          sm_ptr->radio_info.so_mask       = aux_info->hdr_session.so_mask;
        }
      }
      /*---------------------------------------------------------------------
        Common handling
      ---------------------------------------------------------------------*/
      ds_dsd_common_state_transition(notified_event,
                                     sm_ptr,
                                     subs_id,
                                     source_radio,
                                     DS_DSD_RADIO_INVALID);
      break;

    case DSD_CALL_CONNECTED_EV:
    case DSD_CALL_ENDED_EV:
    case DSD_ACQ_HYST_TIMER_EXPIRED_EV:
    case DSD_OOS_HYST_TIMER_EXPIRED_EV:
    case DSD_HAND_DOWN_TIMER_EXPIRED_EV:
      /*---------------------------------------------------------------------
        Common handling
      ---------------------------------------------------------------------*/
      ds_dsd_common_state_transition(notified_event,
                                     sm_ptr,
                                     subs_id,
                                     source_radio,
                                     DS_DSD_RADIO_INVALID);
      break;

    case DSD_HAND_DOWN_EV:
      /*--------------------------------------------------------------------
      Common handling
      --------------------------------------------------------------------*/
      aux_info = (ds_dsd_aux_info_type *)aux_info_ptr;
      ds_dsd_common_state_transition(notified_event,
                                     sm_ptr,
                                     subs_id,
                                     source_radio,
                                     aux_info->hand_down.target_radio);
      break;

    case DSD_DCTM_THROTTLE_EV:
    case DSD_DCTM_UNTHROTTLE_EV:
      if(source_radio == DS_DSD_RADIO_HDR)
      {
        ds_dsd_common_state_transition(notified_event,
                                       sm_ptr,
                                       subs_id,
                                       source_radio,
                                       DS_DSD_RADIO_INVALID);
      }
      break;

    case DSD_NW_ATTACHED_EV:
    case DSD_NW_ATTACHING_EV:
      lte_radio_state = ds_dsd_common_get_sm_state(DS_DSD_RADIO_LTE, subs_id);
      if(source_radio == DS_DSD_RADIO_HDR)
      {
        /*------------------------------------------------------------------
        Common handling
        -------------------------------------------------------------------*/
        sm_ptr->radio_info_prev.rat_mask = sm_ptr->radio_info.rat_mask;
        sm_ptr->radio_info_prev.so_mask  = sm_ptr->radio_info.so_mask;

        aux_info = (ds_dsd_aux_info_type *)aux_info_ptr;
        sm_ptr->radio_info.rat_mask      = aux_info->hdr_session.rat_mask;
        sm_ptr->radio_info.so_mask       = aux_info->hdr_session.so_mask;

        ds_dsd_common_state_transition(notified_event,
                                       sm_ptr,
                                       subs_id,
                                       source_radio,
                                       DS_DSD_RADIO_INVALID);
      }
      else if((source_radio == DS_DSD_RADIO_LTE) &&
              ((lte_radio_state == DS_DSD_SM_STATE_ACQ_HYST) ||
               (lte_radio_state == DS_DSD_SM_STATE_IN_SVC)))
      {

        DATA_MSG2(MSG_LEGACY_MED,"Radio %d: state: %d --> DS_DSD_OOS",
                    sm_ptr->radio,sm_ptr->state);

        if(sm_ptr->state == DS_DSD_SM_STATE_OOS_HYST)
        {
          ds_dsd_timer_stop_timer(DS_HDROOSHystTimerID_T4, subs_id);
        }
        sm_ptr->state_prev = sm_ptr->state;
        sm_ptr->state = DS_DSD_SM_STATE_OOS;
      }
      break;

    case DSD_NW_DETACHED_EV:
      if(source_radio == DS_DSD_RADIO_HDR)
      {
        /*-------------------------------------------------------------------
        Common handling
        -------------------------------------------------------------------*/
        aux_info = (ds_dsd_aux_info_type *)aux_info_ptr;
        sm_ptr->radio_info.rat_mask = PS_SYS_RAT_3GPP_NULL_BEARER;
        sm_ptr->radio_info.so_mask  = 0;

        ds_dsd_common_state_transition(notified_event,
                                       sm_ptr,
                                       subs_id,
                                       source_radio,
                                       DS_DSD_RADIO_INVALID);
      }
      break;

    case DSD_VOICE_CALL_ORIG_EV:
    case DSD_VOICE_CALL_CONNECTED_EV:
    case DSD_VOICE_CALL_ENDED_EV:
    case DSD_EMERGENCY_CALL_ORIG_EV:
    case DSD_EMERGENCY_CALL_CONNECTED_EV:
    case DSD_EMERGENCY_CALL_ENDED_EV:
    case DSD_VOICE_CALL_UP_ON_OTHER_SUB_EV:
    case DSD_VOICE_CALL_DOWN_ON_OTHER_SUB_EV:
      ds_dsd_common_state_transition(notified_event,
                                     sm_ptr,
                                     subs_id,
                                     source_radio,
                                     DS_DSD_RADIO_INVALID);
      break;

    case DSD_ROAM_STATUS_CHANGED_EV:
      if(source_radio == DS_DSD_RADIO_HDR)
      {
        aux_info = (ds_dsd_aux_info_type *)aux_info_ptr;

        sm_ptr->radio_info_prev.roaming_status = 
            sm_ptr->radio_info.roaming_status;
        sm_ptr->radio_info.roaming_status = 
            aux_info->roaming_status_info.roaming_status;

        ds_dsd_common_state_transition(notified_event,
                                       sm_ptr,
                                       subs_id,
                                       source_radio,
                                       DS_DSD_RADIO_INVALID);
      }
      break;

    default:
      DATA_MSG0(MSG_LEGACY_LOW, "Unsupported events for this SM");
      break;
  }
  return TRUE;
}/* ds_dsd_hdrSm_event_notify_cb() */


/*===========================================================================

                        PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_HDRSM_TIMER_CB

DESCRIPTION   The is the function to process timer expiry events of this
              state machine.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds_dsd_hdrSm_timer_cb
(
  unsigned long               timer_id,
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
  ds_dsd_sm_type *sm_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*---------------------------------------------------------------------
    Get statemachine pointer
  ---------------------------------------------------------------------*/
  sm_ptr = ds_dsd_proc_get_radio_sm_instance(subs_id, DS_DSD_RADIO_HDR);
  if(sm_ptr == NULL)
  {
    DATA_MSG0(MSG_LEGACY_LOW, "Invalid statemachine ptr. return");
    return;
  }

  switch(timer_id)
  {
    case DS_HDRAcqHystTimerID_T3:
      ds_dsd_common_state_transition(DSD_ACQ_HYST_TIMER_EXPIRED_EV,
                                     sm_ptr,
                                     subs_id,
                                     DS_DSD_RADIO_HDR,
                                     DS_DSD_RADIO_INVALID);
      break;

    case DS_HDROOSHystTimerID_T4:
      ds_dsd_common_state_transition(DSD_OOS_HYST_TIMER_EXPIRED_EV,
                                     sm_ptr,
                                     subs_id,
                                     DS_DSD_RADIO_HDR,
                                     DS_DSD_RADIO_INVALID);
      break;

    case DS_HDRHandDownTimerID_T9:
      ds_dsd_common_state_transition(DSD_HAND_DOWN_TIMER_EXPIRED_EV,
                                     sm_ptr,
                                     subs_id,
                                     DS_DSD_RADIO_HDR,
                                     DS_DSD_RADIO_INVALID);
      break;

    case DS_HDRHandUpAvoidTimerID_T10:
      ds_dsd_common_state_transition(DSD_HAND_DOWN_AVOID_TIMER_EXPIRED_EV, 
                                     sm_ptr,
                                     subs_id,
                                     DS_DSD_RADIO_HDR,
                                     DS_DSD_RADIO_INVALID);
      break;

    default:
      DATA_MSG1(MSG_LEGACY_ERROR,"Invalid timer_id: 0x%x", timer_id);
      break;
  }

  return;
}/* ds_dsd_hdrSm_timer_cb() */

/*===========================================================================
FUNCTION      DS_DSD_HDRSM_CREATE_SM_INSTANCE

DESCRIPTION   This function creates HDR SM instance corresponding to subs id

DEPENDENCIES  None.

RETURN VALUE  If sucess, statemachine pointer.
              Else, NULL.

SIDE EFFECTS  1. Creates SM instance
              2. Variables are initialized to default value.
===========================================================================*/
ds_dsd_sm_type* ds_dsd_hdrSm_create_sm_instance
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  return ds_dsd_common_create_sm_instance(TRUE);
}/* ds_dsd_hdrSm_create_sm_instance() */

/*===========================================================================
FUNCTION      DS_DSD_HDRSM_INIT

DESCRIPTION   This function initializes HDR state machine

DEPENDENCIES  Event Dispatcher module is initialized already.

RETURN VALUE  None.

SIDE EFFECTS  Registered with event dispatcher module to get notification
===========================================================================*/
void ds_dsd_hdrSm_init(void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Register with event dispatcher */
  ds_dsd_event_dp_reg_client(DS_DSD_CLIENT_HDR,
                             ds_dsd_hdrSm_event_notify_cb,
                             DS_DSD_RADIO_HDR );
  return;
}/* ds_dsd_hdrSm_init() */


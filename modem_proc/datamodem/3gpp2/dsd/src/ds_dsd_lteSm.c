/*===========================================================================
                     D S _ D S D _ L T E S M . C

DESCRIPTION


EXTERNALIZED FUNCTIONS


 Copyright (c) 2011 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_lteSm.c#1 $
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
#include "ds_dsd_lteSm.h"
#include "ds_dsd_timer.h"

/*===========================================================================

                         GLOBAL DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                        LOCAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_LTESM_EVENT_NOTIFY_CB

DESCRIPTION   This function is the callback function called from Event
              dispatch module.


DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Stores SO and RAT mask
===========================================================================*/
boolean ds_dsd_lteSm_event_notify_cb
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
  ds_dsd_sm_state_enum_type   hdr_radio_state;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*---------------------------------------------------------------------
    Validate Event Info
  ---------------------------------------------------------------------*/
  if(FALSE == ds_dsd_common_validate_event(notified_event,
                                           client,
                                           DS_DSD_CLIENT_LTE,
                                           subs_id,
                                           aux_info_ptr))
  {
    return FALSE;
  }

  /*---------------------------------------------------------------------
    Get statemachine pointer
  ---------------------------------------------------------------------*/
  sm_ptr = ds_dsd_proc_get_radio_sm_instance(subs_id, DS_DSD_RADIO_LTE);
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
      if(source_radio == DS_DSD_RADIO_LTE)
      {
        /*-------------------------------------------------------------------
          Extract radio info and store.
        -------------------------------------------------------------------*/
        sm_ptr->radio_info_prev.srv_status = sm_ptr->radio_info.srv_status;
        sm_ptr->radio_info_prev.srv_domain = sm_ptr->radio_info.srv_domain;

        aux_info = (ds_dsd_aux_info_type *)aux_info_ptr;
        sm_ptr->radio_info.srv_status = aux_info->attach_radio.srv_status;
        sm_ptr->radio_info.srv_domain = aux_info->attach_radio.srv_domain;
      }
      /*--------------------------------------------------------------------
        Common handling
      --------------------------------------------------------------------*/
      ds_dsd_common_state_transition(notified_event,
                                     sm_ptr,
                                     subs_id,
                                     source_radio,
                                     DS_DSD_RADIO_INVALID);
      break;

    case DSD_CALL_CONNECTED_EV:
    case DSD_ACQ_HYST_TIMER_EXPIRED_EV:
    case DSD_OOS_HYST_TIMER_EXPIRED_EV:
      /*--------------------------------------------------------------------
        Common handling
      --------------------------------------------------------------------*/
      ds_dsd_common_state_transition(notified_event,
                                     sm_ptr,
                                     subs_id,
                                     source_radio,
                                     DS_DSD_RADIO_INVALID);
      break;

    case DSD_EPS_CNTXT_TX_EV:
    case DSD_NW_ATTACHED_EV:
    case DSD_NW_ATTACHING_EV:
      hdr_radio_state = ds_dsd_common_get_sm_state(DS_DSD_RADIO_HDR, subs_id);
      if(source_radio == DS_DSD_RADIO_LTE)
      {
        /*------------------------------------------------------------------
          Common handling
        -------------------------------------------------------------------*/
        aux_info = (ds_dsd_aux_info_type *)aux_info_ptr;

        sm_ptr->radio_info_prev.rat_mask = sm_ptr->radio_info.rat_mask;
        sm_ptr->radio_info.rat_mask = aux_info->emm_attach.rat_mask;

        ds_dsd_common_state_transition(notified_event,
                                       sm_ptr,
                                       subs_id,
                                       source_radio,
                                       DS_DSD_RADIO_INVALID);
      }
      else if((source_radio == DS_DSD_RADIO_HDR) &&
              (notified_event == DSD_NW_ATTACHED_EV) &&
              ((hdr_radio_state == DS_DSD_SM_STATE_ACQ_HYST) ||
               (hdr_radio_state == DS_DSD_SM_STATE_IN_SVC)))
      {

        DATA_MSG2(MSG_LEGACY_MED,"Radio %d: state: %d --> DS_DSD_OOS",
                  sm_ptr->radio,sm_ptr->state);

        if(sm_ptr->state == DS_DSD_SM_STATE_OOS_HYST)
        {
          ds_dsd_timer_stop_timer(DS_LTEOOSHystTimerID_T6, subs_id);
        }
        sm_ptr->state_prev = sm_ptr->state;
        sm_ptr->state = DS_DSD_SM_STATE_OOS;
      }
      break;

    case DSD_NW_DETACHED_EV:
      if(source_radio == DS_DSD_RADIO_LTE)
      {
        /*--------------------------------------------------------------------
        Common handling
        --------------------------------------------------------------------*/
        sm_ptr->radio_info.rat_mask = PS_SYS_RAT_3GPP_NULL_BEARER;
        sm_ptr->radio_info.so_mask  = 0;

        ds_dsd_common_state_transition(notified_event,
                                       sm_ptr,
                                       subs_id,
                                       source_radio,
                                       DS_DSD_RADIO_INVALID);
      }
      break;

    case DSD_LTE_EX_SO_CHANGED_EV:
      /*--------------------------------------------------------------------
      Only handle the event in LTE sm if it is for LTE radio.
      --------------------------------------------------------------------*/
      if(source_radio == DS_DSD_RADIO_LTE)
      {
        aux_info = (ds_dsd_aux_info_type *)aux_info_ptr;

        sm_ptr->radio_info_prev.so_mask = sm_ptr->radio_info.so_mask;

        if ((aux_info->lte_ex_so_info.lte_so_ex_bit_msk & 
             DS_DSD_LTE_EX_SO_DIV_DUPLEX_MSK) > 0)
        {
          /*--------------------------------------------------------------------
          Update the so with the LTE FDD/TDD mask if available. 
          Clear the so with the LTE FDD/TDD mask if not available
          --------------------------------------------------------------------*/
          if (aux_info->lte_ex_so_info.div_duplex == SYS_DIV_DUPLEX_FREQ)
          {
            sm_ptr->radio_info.so_mask |= DS_SYS_SO_EX_3GPP_LTE_FDD;
          }
          else
          {
            sm_ptr->radio_info.so_mask &= ~DS_SYS_SO_EX_3GPP_LTE_FDD;
          }

          if (aux_info->lte_ex_so_info.div_duplex == SYS_DIV_DUPLEX_TIME)
          {
            sm_ptr->radio_info.so_mask |= DS_SYS_SO_EX_3GPP_LTE_TDD;
          }
          else
          {
            sm_ptr->radio_info.so_mask &= ~DS_SYS_SO_EX_3GPP_LTE_TDD;
          }
        }

        if ((aux_info->lte_ex_so_info.lte_so_ex_bit_msk &
             DS_DSD_LTE_EX_SO_CA_MSK) > 0)
        {
          /*--------------------------------------------------------------------
          Update the so with the LTE CA mask if available. 
          Clear the so with the LTE CA mask if not available
          --------------------------------------------------------------------*/
          if (aux_info->lte_ex_so_info.ca_dl_available)
          {
            sm_ptr->radio_info.so_mask |= DS_SYS_SO_EX_3GPP_LTE_CA_DL;
          }
          else
          {
            sm_ptr->radio_info.so_mask &= ~DS_SYS_SO_EX_3GPP_LTE_CA_DL;
          }

          if (aux_info->lte_ex_so_info.ca_ul_available)
          {
            sm_ptr->radio_info.so_mask |= DS_SYS_SO_EX_3GPP_LTE_CA_UL;
          }
          else
          {
            sm_ptr->radio_info.so_mask &= ~DS_SYS_SO_EX_3GPP_LTE_CA_UL;
          }
        }

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
      if(source_radio == DS_DSD_RADIO_LTE)
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
}/* ds_dsd_lteSm_event_notify_cb() */

/*===========================================================================

                        PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_LTESM_TIMER_CB

DESCRIPTION   The is the function to process timer expiry events of this
              state machine.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds_dsd_lteSm_timer_cb
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
  sm_ptr = ds_dsd_proc_get_radio_sm_instance(subs_id, DS_DSD_RADIO_LTE);
  if(sm_ptr == NULL)
  {
    DATA_MSG0(MSG_LEGACY_LOW, "Invalid statemachine ptr. return");
    return;
  }

  switch(timer_id)
  {
    case DS_LTEAcqHystTimerID_T5:
      ds_dsd_common_state_transition(DSD_ACQ_HYST_TIMER_EXPIRED_EV,
                                     sm_ptr,
                                     subs_id,
                                     DS_DSD_RADIO_LTE,
                                     DS_DSD_RADIO_INVALID);
      break;

    case DS_LTEOOSHystTimerID_T6:
      ds_dsd_common_state_transition(DSD_OOS_HYST_TIMER_EXPIRED_EV,
                                     sm_ptr,
                                     subs_id,
                                     DS_DSD_RADIO_LTE,
                                     DS_DSD_RADIO_INVALID);
      break;

    default:
      DATA_MSG1(MSG_LEGACY_ERROR,"Invalid timer_id: 0x%x", timer_id);
      break;
  }

  return;
}/* ds_dsd_lteSm_timer_cb() */

/*===========================================================================
FUNCTION      DS_DSD_LTESM_CREATE_SM_INSTANCE

DESCRIPTION   This function creates LTE SM instance corresponding to subs id

DEPENDENCIES  None.

RETURN VALUE  If sucess, statemachine pointer.
              Else, NULL.

SIDE EFFECTS  1. Creates SM instance
              2. Variables are initialized to default value.
===========================================================================*/
ds_dsd_sm_type* ds_dsd_lteSm_create_sm_instance
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  return ds_dsd_common_create_sm_instance(TRUE);
}/* ds_dsd_lteSm_create_sm_instance() */

/*===========================================================================
FUNCTION      DS_DSD_LTESM_INIT

DESCRIPTION   This function initializes LTE state machine

DEPENDENCIES  Event Dispatcher module is initialized already.

RETURN VALUE  None.

SIDE EFFECTS  Registered with event dispatcher module to get notification
===========================================================================*/
void ds_dsd_lteSm_init(void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Register with event dispatcher */
  ds_dsd_event_dp_reg_client(DS_DSD_CLIENT_LTE,
                             ds_dsd_lteSm_event_notify_cb,
                             DS_DSD_RADIO_LTE );

  return;
}/* ds_dsd_lteSm_init() */


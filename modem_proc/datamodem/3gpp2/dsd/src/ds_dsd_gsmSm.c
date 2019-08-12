/*===========================================================================
                     D S _ D S D _ G S M S M . C

DESCRIPTION


EXTERNALIZED FUNCTIONS


 Copyright (c)2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_gsmSm.c#1 $
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
#include "ds_dsd_gsmSm.h"

/*===========================================================================

                          GLOBAL DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                        LOCAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_GSMSM_EVENT_NOTIFY_CB

DESCRIPTION   This function is the callback function called from Event
              dispatch module.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  SO and RAT mask of the RADIO are updated
===========================================================================*/
boolean ds_dsd_gsmSm_event_notify_cb
(
  dsd_event_type              notified_event,
  dsd_event_client_type       client,
  dsd_radio_type              source_radio,
  ds3gsubsmgr_subs_id_e_type  subs_id,
  void                       *aux_info_ptr
)
{
  ds_dsd_aux_info_type *aux_info = NULL;
  ds_dsd_sm_type       *sm_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*---------------------------------------------------------------------
    Validate Event Info
  ---------------------------------------------------------------------*/
  if(FALSE == ds_dsd_common_validate_event(notified_event,
                                           client,
                                           DS_DSD_CLIENT_GSM,
                                           subs_id,
                                           aux_info_ptr))
  {
    return FALSE;
  }

  /*---------------------------------------------------------------------
    Get statemachine pointer
  ---------------------------------------------------------------------*/
  sm_ptr = ds_dsd_proc_get_radio_sm_instance(subs_id, DS_DSD_RADIO_GSM);
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
      if(source_radio == DS_DSD_RADIO_GSM)
      {
        /*-------------------------------------------------------------------
          Extract radio info and store.
        -------------------------------------------------------------------*/
        sm_ptr->radio_info_prev.srv_status = sm_ptr->radio_info.srv_status;
        sm_ptr->radio_info_prev.srv_domain = sm_ptr->radio_info.srv_domain;
        sm_ptr->radio_info_prev.rat_mask   = sm_ptr->radio_info.rat_mask;
        sm_ptr->radio_info_prev.so_mask    = sm_ptr->radio_info.so_mask;

        aux_info = (ds_dsd_aux_info_type *)aux_info_ptr;
        sm_ptr->radio_info.srv_status   = aux_info->attach_radio.srv_status;
        sm_ptr->radio_info.srv_domain   = aux_info->attach_radio.srv_domain;
        sm_ptr->radio_info.rat_mask     = aux_info->attach_radio.rat_mask;
        sm_ptr->radio_info.so_mask      = aux_info->attach_radio.so_mask;

        /*-------------------------------------------------------------------
          If GSM/GPRS is lost, it is the responsibility of the client to 
          report correctly. 
           
          E.g. if the remote client is still PS registered but lost the 
          service, it will report still the attach radio is remote. If 
          the GSM/GPRS radio is lost due to PS de-registratio, LPM or so, 
          this needs to be reported as remote system is no more valid after 
          this point. 
        -------------------------------------------------------------------*/
        sm_ptr->is_remote = aux_info->attach_radio.is_remote;
      }

      /*------------------------------------------------------------------
        Common handling
      -------------------------------------------------------------------*/
      ds_dsd_common_state_transition(notified_event,
                                     sm_ptr,
                                     subs_id,
                                     source_radio,
                                     DS_DSD_RADIO_INVALID);
      break;

    case DSD_NETWORK_PARM_CHANGE_EV:
      if(source_radio == DS_DSD_RADIO_GSM)
      {
        /*--------------------------------------------------------------------
          Common handling
        --------------------------------------------------------------------*/
        aux_info = (ds_dsd_aux_info_type *)aux_info_ptr;
        
        sm_ptr->radio_info_prev.cell_srv_ind = sm_ptr->radio_info.cell_srv_ind;
        sm_ptr->radio_info.cell_srv_ind = aux_info->cell_srv_info.cell_srv_ind;

        ds_dsd_common_state_transition(notified_event,
                                       sm_ptr,
                                       subs_id,
                                       source_radio,
                                       DS_DSD_RADIO_INVALID);
      }
      break;

    case DSD_EMERGENCY_CALL_ORIG_EV:
    case DSD_EMERGENCY_CALL_CONNECTED_EV:
    case DSD_EMERGENCY_CALL_ENDED_EV:
      ds_dsd_common_state_transition(notified_event,
                                     sm_ptr,
                                     subs_id,
                                     source_radio,
                                     DS_DSD_RADIO_INVALID);
      break;

    case DSD_ROAM_STATUS_CHANGED_EV:
      if(source_radio == DS_DSD_RADIO_GSM)
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
}/* ds_dsd_gsmSm_event_notify_cb() */

/*===========================================================================

                        PUBLIC FUNCTION DEFINITION

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_GSMSM_CREATE_SM_INSTANCE

DESCRIPTION   This function creates GSM SM instance corresponding to subs id

DEPENDENCIES  None.

RETURN VALUE  If sucess, statemachine pointer.
              Else, NULL.

SIDE EFFECTS  1. Creates SM instance
              2. Variables are initialized to default value.
===========================================================================*/
ds_dsd_sm_type* ds_dsd_gsmSm_create_sm_instance
(
  ds3gsubsmgr_subs_id_e_type  subs_id
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  return ds_dsd_common_create_sm_instance(FALSE);
}/* ds_dsd_gsmSm_create_sm_instance() */

/*===========================================================================
FUNCTION      DS_DSD_GSMSM_INIT

DESCRIPTION   This function initializes GSM state machine

DEPENDENCIES  Event Dispatcher module is initialized already.

RETURN VALUE  None.

SIDE EFFECTS  Registered with event dispatcher module to get notification
===========================================================================*/
void ds_dsd_gsmSm_init(void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Register with event dispatcher */
  ds_dsd_event_dp_reg_client(DS_DSD_CLIENT_GSM,
                             ds_dsd_gsmSm_event_notify_cb,
                             DS_DSD_RADIO_GSM );
  return;
}/* ds_dsd_gsmSm_init() */


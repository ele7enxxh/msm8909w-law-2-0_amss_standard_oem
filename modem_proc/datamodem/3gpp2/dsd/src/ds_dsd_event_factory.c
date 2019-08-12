/*===========================================================================
                     D S _ D S D _ E V E N T _ F A C T O R Y . C

DESCRIPTION


EXTERNALIZED FUNCTIONS


 Copyright (c)2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_event_factory.c#1 $
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
#include "sys.h"
#include <stringl/stringl.h>

#include "dstask_v.h"
#include "modem_mem.h"
#include "ds_dsd_event_factory.h"
#include "ds_dsd_event_dp.h"
#include "ds_dsd_ext.h"
#include "ds_dsd_apm_ext_i.h"
/*===========================================================================

                      LOCAL DATA DEFINITIONS

===========================================================================*/

/*===========================================================================

                      LOCAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================
FUNCTION       DS_DSD_EVENT_FACTORY_SS_EVENT_CBACK_PROCESSING

DESCRIPTION    This function processess the command. This module translates 
               the client event to DSD event and passes information to the
               Event Dispatcher module.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   The global memory is freed
===========================================================================*/
boolean ds_dsd_event_factory_ss_event_cback_processing
(
  void  *data_block_ptr
)
{
  ds_dsd_event_info_type     *event_info_ptr = NULL;
  boolean                     ret_val = FALSE;
  ds_dsd_aux_info_type       *aux_info = NULL;
  dsd_radio_type              client_radio;
  ds3gsubsmgr_subs_id_e_type  subs_id;
  ds_dsd_apm_ind_pay_load     apm_pay_load;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&apm_pay_load, 0, sizeof(ds_dsd_apm_ind_pay_load));

  DATA_MSG1(MSG_LEGACY_LOW, "ds_dsd_ss_event_cback_processing 0x%x",
            data_block_ptr);
  if(data_block_ptr == NULL)
  {
    DATA_MSG0(MSG_LEGACY_ERROR ,"data_block_ptr corrupted");
    return ret_val;
  }

  /*----------------------------------------------------------------------
  Generate an Event DSD 
  ----------------------------------------------------------------------*/
  event_info_ptr = &((ds_dsd_cmd_info_type *)data_block_ptr)->event_info;
  client_radio   = ((ds_dsd_cmd_info_type *)data_block_ptr)->client_radio;
  subs_id        = event_info_ptr->subs_id;
  aux_info       = &event_info_ptr->data;
  ret_val        = TRUE;

  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_id))
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "Invalid Sub id:%d. return", subs_id);
    modem_mem_free(data_block_ptr, MODEM_MEM_CLIENT_DATA);
    return FALSE;
  }

  DATA_MSG3(MSG_LEGACY_HIGH,"New DSD event Radio:%d Event: %d Sub id:%d",
            client_radio,
            event_info_ptr->event,
            subs_id);

  switch(event_info_ptr->event)
  {
    case DS_DSD_CLIENT_RADIO_ACQUIRED_EV:
      ds_dsd_event_dp_input(DSD_ACQ_RADIO_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);

      /*-------------------------------------------------------------------
        Notify APM module for LTE loss
      --------------------------------------------------------------------*/
      if(client_radio ==  DS_DSD_RADIO_HDR ||
         client_radio ==  DS_DSD_RADIO_GSM ||
         client_radio ==  DS_DSD_RADIO_TDSCDMA ||
         client_radio ==  DS_DSD_RADIO_WCDMA)
      {
        apm_pay_load.ind_field_mask = DS_DSD_APM_SYS_CHANGED_IND;
        apm_pay_load.system 
          = ds_dsd_ext_translate_dsd_sys_to_ss(client_radio);
        apm_pay_load.subs_id = ds3gsubsmgr_subs_id_ds3g_to_cm(subs_id);
        (void)ds_dsd_apm_ind(DS_DSD_APM_SYS_CHANGED_IND,&apm_pay_load);
      }
      break;

    case DS_DSD_CLIENT_RADIO_LOST_EV:
      ds_dsd_event_dp_input(DSD_OOS_RADIO_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);

      /*-------------------------------------------------------------------
        Notify APM module for LTE loss
      --------------------------------------------------------------------*/
      if(client_radio ==  DS_DSD_RADIO_LTE)
      {
        apm_pay_load.ind_field_mask = DS_DSD_APM_SYS_CHANGED_IND;
        apm_pay_load.system = SYS_SYS_MODE_NO_SRV;
        apm_pay_load.subs_id = ds3gsubsmgr_subs_id_ds3g_to_cm(subs_id);
        (void)ds_dsd_apm_ind(DS_DSD_APM_SYS_CHANGED_IND, &apm_pay_load);
      }
      break;

    case DS_DSD_CLIENT_RADIO_EMMATTACHING_EV:
      ds_dsd_event_dp_input(DSD_NW_ATTACHING_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);
      break;

    case DS_DSD_CLIENT_RADIO_EPS_CONTEXT_TX_EV:
      ds_dsd_event_dp_input(DSD_EPS_CNTXT_TX_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);
      break;

    case DS_DSD_CLIENT_RADIO_EMMATTACHCOMPLETED_EV:
      ds_dsd_event_dp_input(DSD_NW_ATTACHED_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);
      break;

    case DS_DSD_CLIENT_RADIO_EMMDETACHCOMPLETED_EV:

      ds_dsd_event_dp_input(DSD_NW_DETACHED_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);
      break;

    case DS_DSD_CLIENT_RADIO_HDRSESSIONOPENED_EV:
      ds_dsd_event_dp_input(DSD_NW_ATTACHED_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);
      break;

    case DS_DSD_CLIENT_RADIO_HDRSESSIONCLOSED_EV:
      ds_dsd_event_dp_input(DSD_NW_DETACHED_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);
      break;

    case DS_DSD_CLIENT_RADIO_HANDDOWN_EV:
      ds_dsd_event_dp_input(DSD_HAND_DOWN_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);
      break;

    case DS_DSD_CLIENT_RADIO_CALLCONNECTED_EV:
      ds_dsd_event_dp_input(DSD_CALL_CONNECTED_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);
      break;

    case DS_DSD_CLIENT_RADIO_CALLENDED_EV:
      ds_dsd_event_dp_input(DSD_CALL_ENDED_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);
      break;

    case DS_DSD_CLIENT_RADIO_DCTMTHROTTLED_EV:
      ds_dsd_event_dp_input(DSD_DCTM_THROTTLE_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);
      break;

    case DS_DSD_CLIENT_RADIO_DCTMUNTHROTTLED_EV:
      ds_dsd_event_dp_input(DSD_DCTM_UNTHROTTLE_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);
      break;

    case DS_DSD_CLIENT_RADIO_NETWORK_PARM_CHANGED_EV:
      ds_dsd_event_dp_input(DSD_NETWORK_PARM_CHANGE_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);
      break;

    case DS_DSD_CLIENT_RADIO_VOICE_CALL_ORIG_EV:
       ds_dsd_event_dp_input(DSD_VOICE_CALL_ORIG_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);
      break;

    case DS_DSD_CLIENT_RADIO_VOICE_CALL_CONNECTED_EV:
      ds_dsd_event_dp_input(DSD_VOICE_CALL_CONNECTED_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);
      break;

    case DS_DSD_CLIENT_RADIO_VOICE_CALL_ENDED_EV:
      ds_dsd_event_dp_input(DSD_VOICE_CALL_ENDED_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);
      break;

    case DS_DSD_CLIENT_RADIO_EMERG_CALL_ORIG_EV:
      ds_dsd_event_dp_input(DSD_EMERGENCY_CALL_ORIG_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);
      break;

    case DS_DSD_CLIENT_RADIO_EMERG_CALL_STARTED_EV:
      ds_dsd_event_dp_input(DSD_EMERGENCY_CALL_CONNECTED_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);
      break;

    case DS_DSD_CLIENT_RADIO_EMERG_CALL_ENDED_EV:
      ds_dsd_event_dp_input(DSD_EMERGENCY_CALL_ENDED_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);
      break;

    case DS_DSD_CLIENT_RADIO_VOICE_CALL_UP_ON_OTHER_SUB_EV:
      ds_dsd_event_dp_input(DSD_VOICE_CALL_UP_ON_OTHER_SUB_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);
      break;

    case DS_DSD_CLIENT_RADIO_VOICE_CALL_DOWN_ON_OTHER_SUB_EV:
      ds_dsd_event_dp_input(DSD_VOICE_CALL_DOWN_ON_OTHER_SUB_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);
      break;

    case DS_DSD_CLIENT_RADIO_LTE_EX_SO_CHANGED_EV:
      ds_dsd_event_dp_input(DSD_LTE_EX_SO_CHANGED_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);
      break;

    case DS_DSD_CLIENT_RADIO_ROAM_STATUS_CHANGED_EV:
      ds_dsd_event_dp_input(DSD_ROAM_STATUS_CHANGED_EV,
                            client_radio,
                            subs_id,
                            (void *)aux_info);
      break;

    default:
      DATA_MSG1(MSG_LEGACY_MED,"Unsupported event: %d", event_info_ptr->event);
      ret_val = FALSE;
      break;
  }

  /*--------------------------------------------------------------------
    Decrement DSD's command counter
  --------------------------------------------------------------------*/
  ds_dsd_ext_decr_cmd_counter(subs_id, DS_CMD_DSD_EVENT_NOTIFY);
  return ret_val;
} /* ds_dsd_event_factory_ss_event_cback_processing() */

/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION       DS_DSD_EVENT_FACTORY_CMD_PROCESS

DESCRIPTION    This function processes the DSD input event command.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
void ds_dsd_event_factory_cmd_process
(
  ds_cmd_type  *cmd_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_MSG0(MSG_LEGACY_ERROR, "NULL data_ptr.");
    return;
  }

  switch(cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_DSD_EVENT_NOTIFY:
      ds_dsd_event_factory_ss_event_cback_processing(
         cmd_ptr->cmd_payload_ptr);
      break;

    default:
      DATA_MSG1(MSG_LEGACY_ERROR, "Command %d: not for DSD",
                cmd_ptr->hdr.cmd_id);
      break;
  }
  return;
} /* ds_dsd_event_factory_cmd_process() */

/*===========================================================================
FUNCTION       DS_DSD_EVENT_FACTORY_MH_EVENT_CBACK

DESCRIPTION    This is an entry point function to DSD engine. The context is 
               switched to DS dask for every input.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
boolean ds_dsd_event_factory_mh_event_cback
(
  dsd_radio_type           client_radio,
  ds_dsd_event_info_type  *event_info_ptr
)
{
  ds_cmd_type           *cmd_ptr = NULL;
  ds_dsd_cmd_info_type  *data_block_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MSG2(MSG_LEGACY_LOW,
            "ds_dsd_event_factory_mh client_radio: %d event_info_ptr:0x%x",
            client_radio, 
            event_info_ptr);

  /*--------------------------------------------------------------------
    Get command buffer for new command
  --------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_dsd_cmd_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    return FALSE;
  }

  data_block_ptr = (ds_dsd_cmd_info_type *)cmd_ptr->cmd_payload_ptr;
  
  /* copy the event info to data block pointer */
  memscpy(&data_block_ptr->event_info, 
          sizeof(ds_dsd_event_info_type),
          event_info_ptr,
          sizeof(ds_dsd_event_info_type));
  data_block_ptr->client_radio = client_radio;

  /*----------------------------------------------------------------------
  Switch the context early by posting the command on internal DS queue 
  ----------------------------------------------------------------------*/
  cmd_ptr->hdr.cmd_id = DS_CMD_DSD_EVENT_NOTIFY;
  ds_dsd_ext_put_cmd(DS_DSD_INTERNAL_CMD_QUEUE,
                     event_info_ptr->subs_id,
                     cmd_ptr);
  
  return TRUE;
}/* ds_dsd_event_factory_mh_event_cback() */


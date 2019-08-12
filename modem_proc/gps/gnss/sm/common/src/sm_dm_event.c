/*===========================================================================

              PDSM Event and Logging File.

DESCRIPTION
  This module contains utilities and functions related to logging of events
  and DIAG/DM packets inside PDSM 


  
  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/common/src/sm_dm_event.c#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/22/10   atien   Remove DM events related to the older XTRA-T implementation.
                   New XTRA-T events now are part of TLE.
03/11/09   ns      QWiP Sysd Transition Event
01/14/09   atien   XTRA-T events
06/12/08   lt      Support for external coarse position injection.
06/09/08   gk      On-Demand Changes to add some logs and events
01/31/07   rw      Add gpsOneXTRA events
04/02/07   gk      Modified BEGIN Event API.
12/27/06   ank     Added support for IS801 Rx and Tx events.
12/4/06    gk      Updated headers
11/10/06   gk      Initial checkin for converged GPS
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"    /* Definition for basic types and macros */
#include "target.h"    /* Target configuration file */
#include "customer.h"  /* Customer configuration file */


#include "event.h"
#include "pdapiclient.h"
#include "sm_dm_event.h"


/*===========================================================================

                     DEBUGGING RELATED DEFINITIONS 
                      (MUST DELETE THEM)

*===========================================================================*/



/*===========================================================================

                     DEFINES FOR MODULE

===========================================================================*/



/*==========================================================================
                     
                     MACRO DEFINITIONS
                     
===========================================================================*/

                     
                     
/*===========================================================================

                     DATA TYPE DEFINITIONS FOR MODULE

===========================================================================*/

                     
/*===========================================================================

                     EXTERNALISED DATA/FUNCTION
                     
===========================================================================*/

                     
                     
/*===========================================================================

                     FUNCTION IMPLEMENTATION FOR MODULE

===========================================================================*/


/*===========================================================================

FUNCTION SM_REPORT_EVENT_GPS_PD_SESSION_START

DESCRIPTION
  Report events GPS PD Session start.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gps_pd_session_start
(
  void *data_ptr, 
  uint8 u_StartSource
)
{
  sm_event_pd_session_start_payload_s_type z_Payload;
  tm_pdapi_get_pos_param_s_type *pd_info_ptr; 
  
  pd_info_ptr = (tm_pdapi_get_pos_param_s_type*)data_ptr;

  if (pd_info_ptr == NULL || pd_info_ptr->option_ptr == NULL || pd_info_ptr->qos_ptr == NULL)
  {
    return;
  }
  z_Payload.u_StartSrc_and_OpMode = u_StartSource;
  z_Payload.u_StartSrc_and_OpMode |= (uint8)(( pd_info_ptr->option_ptr->operation_mode & 0xF) << 4);
  z_Payload.u_SessType_and_Privacy = (uint8)( pd_info_ptr->option_ptr->session);
  /* Local DBM not covered.
  */
  z_Payload.u_DatadlType_and_TransType = 1;
  z_Payload.w_NumFixes = (uint16)(pd_info_ptr->option_ptr->fix_rate.num_fixes);
  z_Payload.w_FixPeriod = (uint16)(pd_info_ptr->option_ptr->fix_rate.time_between_fixes_ms);

  z_Payload.u_Prq = pd_info_ptr->qos_ptr->gps_session_timeout;
  z_Payload.w_Threshold = (uint16)(pd_info_ptr->qos_ptr->accuracy_threshold);

  event_report_payload(EVENT_GPS_PD_SESSION_START, 
                       sizeof(z_Payload), 
                       (void *) &z_Payload);
} /* sm_report_event_gps_pd_session_start */


/*===========================================================================

FUNCTION SM_REPORT_EVENT_GPS_PD_SESSION_DONE

DESCRIPTION
  Report events GPS PD Session start.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gps_pd_session_done (void)
{
  uint8 u_Payload = 0;
  
  event_report_payload(EVENT_GPS_PD_SESSION_END, sizeof(u_Payload), (void*)&u_Payload);
}

/*===========================================================================

FUNCTION SM_REPORT_EVENT_GPS_PD_COMM_START

DESCRIPTION
  Report events GPS PD Session start.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gps_pd_comm_start (uint8 u_ProtocolType)
{
  uint8 u_Payload = u_ProtocolType;
  
  event_report_payload(EVENT_GPS_PD_CONNECTION_START, sizeof(u_Payload), (void*)&u_Payload);
}
/*===========================================================================

FUNCTION SM_REPORT_EVENT_GPS_PD_COMM_FAILURE

DESCRIPTION
  Report events GPS PD Session Communication failure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gps_pd_comm_failure (uint8 u_Reason)
{
  uint8 u_Payload = u_Reason;
  
  event_report_payload(EVENT_GPS_PD_COMM_FAILURE, sizeof(u_Payload), (void*)&u_Payload);
}

/*===========================================================================

FUNCTION SM_REPORT_EVENT_GPS_PD_COMM_CONNECTED

DESCRIPTION
  Report events GPS PD Session start.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gps_pd_comm_connected (void)
{
  uint8 u_Payload = 0;
  
  event_report_payload(EVENT_GPS_PD_CONNECTION_ESTABLISHED, sizeof(u_Payload), (void*)&u_Payload);
  
}

/*===========================================================================

FUNCTION SM_REPORT_EVENT_GPS_PD_COMM_DONE

DESCRIPTION
  Report events GPS PD Session start.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gps_pd_comm_done (void)
{
  uint8 u_Payload = 0;
  
  event_report_payload(EVENT_GPS_PD_COMM_DONE, sizeof(u_Payload), (void*)&u_Payload);
  
}
/*===========================================================================

FUNCTION SM_REPORT_EVENT_GPS_PD_POSITION

DESCRIPTION
  Report events GPS PD Session start.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gps_pd_position (void)
{
  uint8 u_Payload = 0;
  
  event_report_payload(EVENT_GPS_PD_POSITION, sizeof(u_Payload), (void*)&u_Payload);
  
}

/*===========================================================================

FUNCTION SM_REPORT_EVENT_GPS_PD_EVENT_END

DESCRIPTION
  Report events GPS PD Session start.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gps_pd_event_end (uint8 u_Reason)
{
  uint8 u_Payload = u_Reason;
  
  event_report_payload(EVENT_GPS_PD_EVENT_END, sizeof(u_Payload), (void*)&u_Payload);
  
}



/*===========================================================================

FUNCTION SM_REPORT_EVENT_GPS_PA_EVENT_CALLBACK

DESCRIPTION
  Report events GPS PD Session start.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gps_pa_event_callback (pdsm_pa_e_type e_PaEvent)
{
  uint8 u_Payload;
  
  u_Payload = (uint8)e_PaEvent;
  
  event_report_payload(EVENT_GPS_PA_EVENT_CALLBACK, sizeof(u_Payload), (void*)&u_Payload);
  
}


/*
 ******************************************************************************
 * FUNCTION: SM_REPORT_EVENT_PD_CMD_ERR 
 *
 * Function description:
 *
 * This function generates a DM event when a PA Cmd error is generated
 *
 * Parameters: 
 * pointer to pdsm_dm_event_type
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_report_event_pd_cmd_err(uint8 u_CmdType, pdsm_pd_cmd_err_e_type e_CmdErr)
{
  sm_event_pd_cmd_err_callback_s_type z_Payload;
  
  z_Payload.u_CmdType = (uint8)u_CmdType;
  z_Payload.u_CmdErr  = (uint8)e_CmdErr ;
  
  event_report_payload(EVENT_GPS_PD_CMD_ERR_CALLBACK, sizeof(z_Payload), (void*)&z_Payload);

}

/*
 ******************************************************************************
 * FUNCTION: SM_REPORT_EVENT_PA_CMD_ERR 
 *
 * Function description:
 *
 * This function generates a DM event when a PA Cmd error is generated
 *
 * Parameters: 
 * pointer to pdsm_dm_event_type
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************/
void sm_report_event_pa_cmd_err(uint8 u_CmdType, pdsm_pa_cmd_err_e_type e_CmdErr)
{
  sm_event_pd_cmd_err_callback_s_type z_Payload;
  
  z_Payload.u_CmdType = (uint8)u_CmdType;
  z_Payload.u_CmdErr  = (uint8)e_CmdErr ;
  
  event_report_payload(EVENT_GPS_PA_CMD_ERR_CALLBACK, sizeof(z_Payload), (void*)&z_Payload);

}

/*===========================================================================

FUNCTION sm_report_event_gps_pd_download_start

DESCRIPTION
  Report events GPS Data Doanload Starts.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gps_pd_download_start
(
  uint8  data_type,
  uint32 sv_mask
)
{
  sm_event_gps_data_download_payload_s_type payload;

  payload.data_type = data_type;
  payload.sv_mask   = sv_mask;
  event_report_payload(EVENT_GPS_DATA_DOWNLOAD_START,
                       sizeof(payload),
                       (void *) &payload);
} /* sm_report_event_gps_pd_download_start */

/*===========================================================================

FUNCTION sm_report_event_gps_pd_download_end

DESCRIPTION
  Report events GPS Data Doanload is ended.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gps_pd_download_end
(
  sint31 status
)
{
  uint8 payload;

  payload = (uint8) (status + 1);
  event_report_payload(EVENT_GPS_DATA_DOWNLOAD_END, sizeof(payload), (void *) &payload);
} /* sm_report_event_gps_pd_download_end */

/*===========================================================================

FUNCTION sm_report_event_gps_is801_tx

DESCRIPTION
  Report events IS801 TX event.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gps_is801_tx
(
  sm_event_is801_msg_e_type  msg_type,
  uint32                     mask
)
{
  byte msg_id;

  if(msg_type == IS801_REQ)
  {
    /* MS Bit is req/rsp indicator, "1" for response, "0" for request. 
    ** The least 4 significant bits are IS-801 request type 
    */
    msg_id = (byte) (0x00 | mask);
    event_report_payload(EVENT_GPS_IS801_TX, sizeof(msg_id), &msg_id);
  }
  else if (msg_type == IS801_RESP) 
  {
    /* MS Bit is req/rsp indicator, "1" for response, "0" for request. 
    ** The least 4 significant bits are IS-801 response type 
    */
    msg_id = (byte) (0x80 | mask);
    event_report_payload(EVENT_GPS_IS801_TX, sizeof(msg_id), &msg_id);
  }
} /* sm_report_event_gps_is801_tx */

/*===========================================================================

FUNCTION sm_report_event_gps_is801_rx

DESCRIPTION
  Report events IS801 RX event.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gps_is801_rx
(
  sm_event_is801_msg_e_type  msg_type,
  uint32                     mask
)
{
  byte msg_id;

  if(msg_type == IS801_REQ)
  {
    /* MS Bit is req/rsp indicator, "1" for response, "0" for request. 
    ** The least 4 significant bits are IS-801 request type
    */
    msg_id = (byte) (0x00 | mask);
    event_report_payload(EVENT_GPS_IS801_RX, sizeof(msg_id), &msg_id);
  }
  else if (msg_type == IS801_RESP) 
  {
    /* MS Bit is req/rsp indicator, "1" for response, "0" for request. 
    ** The least 4 significant bits are IS-801 response type
    */
    msg_id = (byte) (0x80 | mask);
    event_report_payload(EVENT_GPS_IS801_RX, sizeof(msg_id), &msg_id);
  }
} /* sm_report_event_gps_is801_rx */


/*===========================================================================

FUNCTION sm_report_event_gps_fix_start

DESCRIPTION
  Report EVENT_GPS_PD_FIX_START 
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gps_fix_start
(
  uint16                                log_cnt,
  sm_event_pd_fix_start_payload_e_type  op_mode
)
{
  sm_event_gps_pd_fix_start_payload_s_type payload;

  payload.w_LogCnt        = log_cnt;
  payload.u_OperationMode = op_mode;

  event_report_payload(EVENT_GPS_PD_FIX_START, 
                       sizeof(payload), 
                       (void *) &payload);
} /* sm_report_event_gps_fix_start */

/*===========================================================================

FUNCTION sm_report_event_gps_fix_end

DESCRIPTION
  Report EVENT_GPS_PD_FIX_END

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gps_fix_end
(
  sint31 status
)
{
  uint8 payload;

  payload = (uint8) (status + 1);
  event_report_payload(EVENT_GPS_PD_FIX_END, sizeof(payload), (void*)&payload);
} /* sm_report_event_gps_fix_end */


/*===========================================================================

FUNCTION sm_report_event_gps_sa_received

DESCRIPTION
  Report EVENT_GPS_SA_RECEIVED

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gps_sa_received
(
  uint16 ref_bit_num,
  uint8  dr_size
)
{
  sm_event_gps_sa_rcv_payload_s_type sm_event_gps_sa_rcv_payload;

  sm_event_gps_sa_rcv_payload.ref_bit_num = ref_bit_num;
  sm_event_gps_sa_rcv_payload.dr_size = dr_size;
  event_report_payload(EVENT_GPS_SA_RECEIVED,
                        sizeof(sm_event_gps_sa_rcv_payload_s_type),
                        (void *) &sm_event_gps_sa_rcv_payload);
}


/*===========================================================================

FUNCTION sm_report_event_ui_req

DESCRIPTION
  This function generates a DM message when a UI request is genreated by the
  PDAPI

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_ui_req(uint32 client_id,
                                   byte priv_type)
{
  sm_report_event_type event_data;
  
  event_data.client_id = client_id;
  event_data.u_EventName = EVENT_PDSM_NOTIFY_VERIFY_REQ;
  event_data.u_EventData =  priv_type;
  event_report_payload(EVENT_GPS_PDSM_EVENT_REPORT, sizeof(sm_report_event_type), (void*)&event_data);
}

/*===========================================================================

FUNCTION sm_report_event_xtra_start_dload

DESCRIPTION
  This function generates a DM message when the gpsOneXTRA sends a start
  download request to the clients

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_xtra_start_dload
(
   sm_event_xtra_download_trigger_e_type trigger
)
{
  sm_event_xtra_start_download_s_type  sm_event_xtra_start_dload;

  sm_event_xtra_start_dload.trigger_type = trigger;
    
  event_report_payload( EVENT_GPSONEXTRA_START_DOWNLOAD, 
                        sizeof(sm_event_xtra_start_download_s_type), 
                        (void*)&sm_event_xtra_start_dload);
} /* sm_report_event_xtra_start_dload */


/*===========================================================================

FUNCTION sm_report_event_xtra_end_dload

DESCRIPTION
  This function generates a DM message when the gpsOneXTRA sends a end
  download request to the clients

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_xtra_end_dload
(
  pdsm_xtra_status_e_type reason 
)
{
  sm_event_xtra_end_download_s_type sm_event_xtra_end_dload;

  sm_event_xtra_end_dload.end_reason = reason;

  event_report_payload( EVENT_GPSONEXTRA_END_DOWNLOAD, 
                        sizeof(sm_event_xtra_end_download_s_type), 
                        (void*)&sm_event_xtra_end_dload);
    
} /* sm_report_event_xtra_end_dload */

/*===========================================================================

FUNCTION sm_report_event_gps_on_demand_mode_change

DESCRIPTION
  This function generates a DM message when tm on_demand feature changes modes

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gps_on_demand_mode_change(uint8 mode_prev, uint8 mode_next, uint32 reason_for_mode_change)
{
   sm_event_on_demand_mode_change_payload_s_type z_payload;
   z_payload.mode_prev = mode_prev;
   z_payload.mode_current = mode_next;
   z_payload.mode_change_reason = (uint8) reason_for_mode_change;
   event_report_payload(EVENT_GPS_TM_ON_DEMAND_MODE_CHANGE, sizeof(z_payload), (void*)&z_payload);
} /* end sm_report_event_gps_on_demand_mode_change()*/



/*===========================================================================

FUNCTION sm_report_event_gps_on_demand_mode_change

DESCRIPTION
  This function generates a DM message when tm on_demand feature changes modes

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gps_on_demand_mode_begin(uint8 mode)
{
   sm_event_on_demand_begin_payload_s_type z_payload;
   z_payload.mode = mode;
   
   switch(mode)
   {
     case 1:
       z_payload.gps_session_timeout = 3;
       z_payload.accuracy = 250;
       break;
     case 2:
       z_payload.gps_session_timeout = 30;
       z_payload.accuracy = 250;
       break;
     case 3:
       z_payload.gps_session_timeout = 30;
       z_payload.accuracy = 250;
       break;
     case 4:
       z_payload.gps_session_timeout = 75;
       z_payload.accuracy = 250;
       break;
     case 5:
       z_payload.gps_session_timeout = 1;
       z_payload.accuracy = 250;
       break;
     case 6:
       z_payload.gps_session_timeout = 3;
       z_payload.accuracy = 250;
       break;
     case 7:
       z_payload.gps_session_timeout = 3;
       z_payload.accuracy = 250;
       break;
     default:
       break;

   }
   event_report_payload(EVENT_GPS_TM_ON_DEMAND_BEGIN, sizeof(z_payload), (void*)&z_payload);
} /* end sm_report_event_gps_on_demand_mode_change()*/



/*===========================================================================

FUNCTION sm_report_event_gps_on_demand_mode_change

DESCRIPTION
  This function generates a DM message when tm on_demand feature changes modes

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gps_on_demand_mode_done(uint8 reason)
{
   uint8 z_payload;
   z_payload = reason;
   event_report_payload(EVENT_GPS_TM_ON_DEMAND_DONE, sizeof(z_payload), (void*)&z_payload);
} /* end sm_report_event_gps_on_demand_mode_change()*/

/*===========================================================================

FUNCTION SM_REPORT_EVENT_COARSE_POS_INJ_START

DESCRIPTION
  Report DM event EVENT_COARSE_POS_INJ_START.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_coase_pos_inj_start ()
{
  uint8 u_payload = 0;

  event_report_payload(EVENT_GPS_EXTERN_COARSE_POS_INJ_START, 
                       sizeof(u_payload), 
                       (void*)&u_payload);
}


/*===========================================================================

FUNCTION SM_REPORT_EVENT_COARSE_POS_INJ_END

DESCRIPTION
  Report DM event EVENT_COARSE_POS_INJ_END.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_coase_pos_inj_end (sm_event_extern_coarse_pos_inj_end_reason 
                                        u_reason)
{
  uint8 u_payload = 0;

  u_payload = u_reason;
  event_report_payload(EVENT_GPS_EXTERN_COARSE_POS_INJ_END, 
                       sizeof(u_payload), 
                       (void*)&u_payload);
}

/*===========================================================================

FUNCTION SM_REPORT_EVENT_QWIP_SYSD_TRANSITION

DESCRIPTION
  Report event: QWiP SYSD State transitions

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_qwip_sysd_transition (uint8 sysd_sm_type,uint8 event,
                                       uint8 current_state, uint8 next_state)
{
  sm_event_qwip_sysd_transition_s_type z_payload;

  z_payload.sysd_sm_type = sysd_sm_type;
  z_payload.event_type = event;
  z_payload.current_state = current_state;
  z_payload.next_state = next_state;


  event_report_payload(EVENT_CGPS_QWIP_SYSD_TRANSITION, 
  sizeof(sm_event_qwip_sysd_transition_s_type), (void*)&z_payload);

}

/*===========================================================================

FUNCTION SM_REPORT_EVENT_PREEMPTION_FLAG

DESCRIPTION
  Report event: Reports an event when GSM preemption happens in E911

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_preemption_flag (void)
{
  
  
  uint8 u_payload = 0;

  event_report_payload(EVENT_CGPS_PREEMPTION_FLAG, 
                       sizeof(u_payload), 
                       (void*)&u_payload);
}

/*===========================================================================

FUNCTION SM_REPORT_EVENT_GNSS_FAST_TCAL_END 

DESCRIPTION
  Report event: Fast TCal Completed

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_gnss_fast_tcal_end 
(
  int16 TCal, 
  uint8 TCalUncertainty,
  uint8 ErrorCode
)
{
  sm_event_gnss_fast_tcal_end_s_type z_payload;

  z_payload.TCal = TCal;
  z_payload.TCalUncertainty = TCalUncertainty;
  z_payload.ErrorCode = ErrorCode;


  event_report_payload(EVENT_GNSS_FAST_TCAL_END, 
                       sizeof(z_payload), 
                       (void*)&z_payload);
}

/*===========================================================================

FUNCTION SM_REPORT_EVENT_RESET_LOCATION_SERVICE_RECEIVED 

DESCRIPTION
  Report event:Reset Location Service Request Received

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_reset_location_service_received(void)
{
  uint8 u_payload = 0;

  event_report_payload(EVENT_GNSS_RESET_LOCATION_SERVICE_RECEIVED, 
                       sizeof(u_payload), 
                       (void*)&u_payload);
}

/*===========================================================================

FUNCTION SM_REPORT_EVENT_RESET_LOCATION_SERVICE_DONE 

DESCRIPTION
  Report event:Reset Location Service Request Done

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void sm_report_event_reset_location_service_done(uint32 status)
{
  uint32 u_payload = status;

  
  event_report_payload(EVENT_GNSS_RESET_LOCATION_SERVICE_DONE, 
                       sizeof(u_payload), 
                       (void*)&u_payload);
}


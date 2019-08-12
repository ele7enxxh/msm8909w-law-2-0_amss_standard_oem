
/*===========================================================================

                            TM_CM_IFace  Module

DESCRIPTION



Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary. 
Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_cm_iface.c#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
03/11/16    mj      Changes to check as_sys_mode from cell info first for CELL DCH request to CM
11/10/15    skm     Fix MT-ES failure where autp e-slp is not generated due to missing CELL ID in WCDMA. 
11/06/15    mj      Add changes to support CM CellID event processing for MultiSIM
10/19/15    mj      Add changes to provide srv_status of selected sys_mode through phone status
07/20/15    mj      Add support for MultiMode config for MultiSim devices
06/24/15    rh      Added CM CellID event processing  
05/04/15    mj     Address code review comments to make handling of operatonal/non-operational flag thread safe
04/15/15    mj     Handle updates from CM when the stack goes from operational to non-operational, without prior event update
03/18/15    sj      Update Call END handling logic for 1x 
07/16/14    mj      During LPM event set all stacks to no service irrespective of stack operational or not
06/04/14    mj      Handle MultiSIM subscription feature SRLTE
05/01/14    mj      Changes to address KW issues wrt sub id sent from CM
03/25/14    mj      Changes to fix IMSI updates during hot swap
03/21/14    ssu     Addressing compilation issues on SMART(offfline) setup.
12/23/13    mj      Add functionality to read device config from policy manager
01/15/13    mj     Remove featurization FEATURE_DUAL_SIM to handle case where SGLTE/SVLTE code was being featured out
12/12/13    ss      Use stack type while filling is serving system in SS Info mapping
11/22/13    ss      DI 3.0 onwards, CM moved to Super build. In case of single sim mode, there will be no
                          dual sim sub event. This change is to handle that.
09/27/13    mj      TSTS Support: Changes to handle the third SUB
06/14/13    ssu     GPS changes to update subs id when a dual sim device is in single sim mode.
                    ( the single/dual mode is controlled by EFS NV )  
04/16/13    mj      Update DSDS sub id
09/07/12    mj      Added new function tm_cm_map_external_RAT and modified tm_cm_iface_get_phone_state for SGLTE
01/04/12    ssu     Obtaining th CELL id information in CELL_DCH from CM APIs 
06/23/11    rh      Added cdma_mt_end_call_immed parameter to end PD call immediately
06/16/11    rh      Added CM_CALL_EVENT_OTASP_STATUS event handling for CR 280064
04/19/11    gk      Initialize the hdr_srv_status to SYS_SRV_STATUS_NO_SRV
01/20/11    rh      Fix for CR 258513
04/06/10    ns      Fix for CR 233506: add support for CM PH INFO GET.
03/25/10    ns      Fix for CR:230743: modify MOLR CM api calls for DSDS.
03/03/10    ns      DSDS Support
11/11/08    ank     When handling stop req, end PD call only if MT session is active.
04/04/08    cl      Inform TM-core when entering GSM
06/27/07    lt      Added the passing of WCDMA in-service to UMTS submodules.
04/24/07    ank     Added support for CELL_ID_DB feature.
12/20/06    ank     Changes to read NV item.
11/30/06    ank     Initial Release
===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"
#include "err.h"
#include "event.h"

#ifdef FEATURE_CGPS_CDMA_IF
#include "cai.h"
#ifdef FEATURE_CMI
#include "cai_v.h"
#endif
#endif

#include "sm_nv.h"
#include "tm_common.h"
#include "tm_data.h"
#include "pdapibuf.h"
#include "tm_cm_iface.h"
#include "policyman.h"
#ifdef FEATURE_CGPS_XTRA_T
#include "tle_api.h"
#endif /* FEATURE_CGPS_XTRA_T */

#ifdef FEATURE_CGPS_LBS_TASK
#include "lbs_v02.h"
#endif
#include "mgp_api.h"

#include "tm_rrc_iface.h"
#include "gm_api.h"
#include "tm_ruim.h"

#include "mm_umts.h"
#include "bit.h"
#ifdef FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH
LOCAL void tm_cm_cell_info_dch_report_handler
(
  const tm_cm_mm_ss_info_s_type  *ss_info_ptr, const uint8 dd_stack_id
);
#endif //FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH
boolean b_srvcc = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                        GLOBAL DATA DECLARATIONS                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
tm_cm_info_s_type tm_cm_info;

LOCAL void tm_cm_map_ss_info( const tm_cm_mm_ss_info_s_type * const cp_ss_info_ptr,
                              cgps_SSInfoType * p_cgps_ss_info, 
                              cgps_SSInfoDataType e_info_type);

LOCAL sys_sys_mode_e_type tm_cm_get_svlte_sys_mode(const sys_sys_mode_e_type  cdma_stack_sys_mode,
                           const sys_sys_mode_e_type  lte_stack_sys_mode);

LOCAL boolean tm_cm_is_cell_info_update_needed(const tm_cm_mm_ss_info_s_type *const ss_info_ptr, uint8 stack);

LOCAL void tm_cm_process_cell_info_update( const tm_cm_mm_msim_ss_stack_info_s_type *const cp_ss_info_ptr);

static void tm_core_cm_notify_srv_status(void);

#ifdef FEATURE_CGPS_CDMA_IF
/* NV buffer, used to read NV items */
static nv_item_type      tm_cm_iface_nv_read_buffer;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                          FUNCTION DECLARATIONS                          */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

LOCAL void tm_cm_call_cmd_cb(
  void                      *data_block_ptr,
  /* Pointer to client provided data block. */

  cm_call_cmd_e_type        call_cmd,
  /* Indicate which command is this error status for */

  cm_call_cmd_err_e_type    call_cmd_err
  /* Call command error code, indicating whether command
  ** is rejected and the reason */
);

LOCAL void tm_cm_ph_cmd_cb (

    void                    *data_block_ptr,
        /* Pointer to client provided data block */

    cm_ph_cmd_e_type        ph_cmd,
        /* Indicate which phone command is this error status for */

    cm_ph_cmd_err_e_type    ph_cmd_err
        /* Phone command error code, indicating whether command
        ** is rejected and the reason */
);

LOCAL void tm_cm_dbm_cmd_cb
(
  void                       *data_block_ptr,
    /* Pointer to client provided data block */
  cm_dbm_cmd_e_type           dbm_cmd,
      /* Indicate which dbm command is this error status for */
  cm_dbm_cmd_err_e_type       dbm_cmd_err
      /* Reason for rejection */
);

LOCAL void tm_cm_dbm_event_cb
(
  cm_dbm_event_e_type         dbm_event,
  const cm_dbm_info_s_type    *dbm_info_ptr
);



tm_cm_phone_state_e_type tm_cm_cdma_get_phone_state(void);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                          FUNCTION DEFINITIONS                           */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*===========================================================================

FUNCTION tm_cm_cdma_call_end

DESCRIPTION
  Asks the CM to end PD Call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void tm_cm_cdma_call_end(void)
{
  cm_end_params_s_type   ep;

  ep.info_type = CM_CALL_MODE_INFO_CDMA;
  ep.end_params.cdma_end.end_reason_included = FALSE;
  ep.end_params.cdma_end.end_reason = CAI_REL_NORMAL;

  ep.call_id = tm_cm_info.pd_call_id;

  (void)cm_mm_call_cmd_end( tm_cm_call_cmd_cb,
                   NULL,
                   tm_cm_info.cm_client,
                   1,    /* one call is being released */
                   & ep );

}

/*===========================================================================

FUNCTION tm_cm_cdma_clear_inactivity_timer

DESCRIPTION
  Clears the DTC idle timer (for auto-disconnect purposes).

DEPENDENCIES
  tm_cm_client_init must have been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void tm_cm_cdma_clear_inactivity_timer(void)
{
  if(tm_cm_info.inactivity_timer_active)
  {
    (void)os_TimerStop(tm_cm_info.inactivity_timer);
  }
  tm_cm_info.inactivity_timer_active = FALSE;
}

/*===========================================================================

FUNCTION tm_cm_set_stc_inactivity_timer

DESCRIPTION
  Sets the DTC idle timer (for auto-disconnect purposes).

DEPENDENCIES
  tm_cm_client_init must have been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void tm_cm_cdma_set_inactivity_timer(void)
{
  if(!os_TimerStart(tm_cm_info.inactivity_timer,TM_CM_AUTO_DISCONNECT_TIME,NULL))
  {
    MSG_ERROR("Failed starting DTC Inactivity timer",0,0,0);
  }
  else
  {
    tm_cm_info.inactivity_timer_active = TRUE;
  }
}

/*===========================================================================

FUNCTION tm_cm_cdma_reset_inactivity_timer

DESCRIPTION
  This function resets the idle timer if it is active.

DEPENDENCIES
  tm_cm_client_init must have been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void tm_cm_cdma_reset_inactivity_timer(void)
{
  if(tm_cm_info.inactivity_timer_active)
  {
    if(!os_TimerStart(tm_cm_info.inactivity_timer,TM_CM_AUTO_DISCONNECT_TIME,NULL))
    {
      MSG_ERROR("Failed starting DTC Inactivity timer",0,0,0);
    }
  }
}

/*===========================================================================

FUNCTION tm_cm_cdma_flush_pending_queue

DESCRIPTION
  Flushes the message pending queue.

DEPENDENCIES
  It is assumed that the the queue contains "send message" tm_cm_cmd_types.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void tm_cm_cdma_flush_pending_queue(q_type* msg_queue)
{
  tm_cm_send_msg_info_s_type* msg_info_ptr;

  MSG_LOW("Elements in queue %d",q_cnt(msg_queue),0,0);
  while(NULL != (msg_info_ptr = (tm_cm_send_msg_info_s_type*) q_get(msg_queue)))
  {
    (void)pdsm_freebuf((char *)msg_info_ptr);
  }
}

/*===========================================================================

FUNCTION tm_cm_clean_up

DESCRIPTION
  Resets all TM_CM timers, flushes all message queues and sends a disconnect
  to the client.

DEPENDENCIES
  Should only be called if the DTC is known to be down.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void tm_cm_cdma_clean_up(void)
{
  tm_cm_info.cdma_mt_session_active = FALSE;
  tm_cm_cdma_flush_pending_queue(&tm_cm_info.msg_waiting_q);
  tm_cm_cdma_clear_inactivity_timer();
  tm_core_cm_notify_event(TM_PHONE_EVENT_TC_DISCONNECTED, NULL);
}
#endif /* FEATURE_CGPS_CDMA_IF */

/*===========================================================================

FUNCTION tm_cm_handle_timer_expiry

DESCRIPTION

DEPENDENCIES
  tm_cm_client_init must have been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tm_cm_handle_timer_expiry(const os_TimerExpiryType *timer_data)
{
    if(timer_data != NULL)
    {
      MSG_HIGH("timerId 0x%x expired", timer_data->q_TimerId, 0, 0);        
      if(timer_data->q_TimerId & TM_CM_IFACE_TIMER_DISPATCH_MASK)
      {
        switch(timer_data->q_TimerId)
        {
          case TM_CM_IFACE_INACTIIVITY_TIMER_ID:
#ifdef FEATURE_CGPS_CDMA_IF
              if(tm_cm_info.inactivity_timer_active)
              {
                MSG_MED("TM CM Iface inactivity timer expired.", 0, 0, 0);
                if(tm_cm_iface_is_pd_call_connected())
                {
                  MSG_MED("TM_CM ending PD call", 0, 0, 0);
                  tm_cm_cdma_call_end();
                }
                if(tm_cm_info.cdma_mt_session_active)
                {
                  tm_cm_cdma_clean_up();
                }
              }
#endif /* FEATURE_CGPS_CDMA_IF */
          break;

          default:
            MSG_ERROR("Unknown timer exipred", 0, 0, 0);
          break;
        }
      }
      else
      {
        MSG_ERROR("Can not proess invalid timer event", 0, 0, 0);
      }
    }
    else
    {
      MSG_ERROR("Can not proess null timer msg", 0, 0, 0);  
    }
}


/*===========================================================================

FUNCTION tm_cm_handle_emergency_call_orig_event

DESCRIPTION

DEPENDENCIES
  tm_cm_client_init must have been called.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
static void tm_cm_handle_emergency_call_orig_event(void)
{


  tm_cm_phone_state_info_s_type phone_state_info;
  
  memset(&phone_state_info, 0, sizeof(tm_cm_phone_state_info_s_type));
  
  tm_cm_iface_get_phone_state(&phone_state_info);

  if(phone_state_info.e911_state != TM_CM_E911_STATE_ORIG)
  {
    tm_cm_info.e911_state = TM_CM_E911_STATE_ORIG;

    tm_core_cm_notify_event(TM_PHONE_EVENT_E911_CALL_ORIG, NULL);
      
    if (phone_state_info.srv_system == CGPS_SRV_SYS_LTE) 
    {
      /* if we are on LTE, start a TM Core session to turn on rcvr early */
      (void)tm_core_start_emergency_session();
    }
#ifdef FEATURE_CGPS_XTRA_T
    (void) tle_SendE911Events(C_TLE_EVENT_E911_CALL_ORIG);
#endif /* FEATURE_CGPS_XTRA_T */
  }
}


/*===========================================================================

FUNCTION tm_cm_handle_emergency_call_end_event

DESCRIPTION

DEPENDENCIES
  tm_cm_client_init must have been called.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
static void tm_cm_handle_emergency_call_end_event(void)
{


  tm_cm_phone_state_info_s_type phone_state_info;
  
  memset(&phone_state_info, 0, sizeof(tm_cm_phone_state_info_s_type));
  
  tm_cm_iface_get_phone_state(&phone_state_info);
  tm_cm_info.e911_state = TM_CM_E911_STATE_NONE;
  tm_core_cm_notify_event(TM_PHONE_EVENT_E911_CALL_END, NULL);
  b_srvcc = FALSE;
#ifdef FEATURE_CGPS_XTRA_T
  (void)tle_SendE911Events(C_TLE_EVENT_E911_CALL_END);
#endif /* FEATURE_CGPS_XTRA_T */

  #ifndef FEATURE_GNSS_SA
  tm_ruim_handle_post_e911(); /* for hot SIM swap */
  #endif /* ! FEATURE_GNSS_SA */
}

/*===========================================================================

FUNCTION tm_cm_handle_call_event

DESCRIPTION

DEPENDENCIES
  tm_cm_client_init must have been called.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void tm_cm_handle_call_event(
  cm_call_event_e_type   call_event,
  const tm_cm_mm_call_info_s_type *call_info_ptr)
{
#ifdef FEATURE_CGPS_CDMA_IF
  cm_ans_params_s_type ap;
#endif /* FEATURE_CGPS_CDMA_IF */


  tm_cm_phone_state_info_s_type phone_state_info = {CGPS_SRV_SYS_NONE, TM_CM_E911_STATE_NONE};

  tm_cm_iface_get_phone_state(&phone_state_info);
  if(call_info_ptr == NULL || ((call_event != CM_CALL_EVENT_EXIT_TC) && (call_info_ptr->call_id >= CM_CALL_ID_MAX)) )
  {
    MSG_ERROR("Null call_info_ptr or call_id beyond range",0,0,0);
    return;
  }

  if(call_event == CM_CALL_EVENT_ORIG)
  {
    MSG_MED("ORIG event: CallId=%d, CallState=%d, CallType=%d", 
             call_info_ptr->call_id, 
             tm_cm_info.phone_info.call[call_info_ptr->call_id].call_state, 
             call_info_ptr->call_type);

    tm_cm_info.phone_info.call[call_info_ptr->call_id].call_state = TM_CM_CALL_STATE_WAIT_CONNECT;
    tm_cm_info.phone_info.call[call_info_ptr->call_id].call_type = call_info_ptr->call_type;

    if( (call_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY) ||
        ( (tm_agps_emergency_call_simulate() == TRUE) &&
          (call_info_ptr->call_type == CM_CALL_TYPE_VOICE)
        )
      )
    {
      MSG_HIGH("Emergency call orig! CallType=%d",call_info_ptr->call_type,0,0);
      tm_cm_handle_emergency_call_orig_event();

    }
  } /* end if ORIG */
  else if(call_event  == CM_CALL_EVENT_INCOM )
  {
    MSG_MED("INCOM event: CallId=%d, CallState=%d, CallType=%d", 
             call_info_ptr->call_id, 
             tm_cm_info.phone_info.call[call_info_ptr->call_id].call_state, 
             call_info_ptr->call_type);

    tm_cm_info.phone_info.call[call_info_ptr->call_id].call_type = call_info_ptr->call_type;
#ifdef FEATURE_CGPS_CDMA_IF
    if(call_info_ptr->call_type == CM_CALL_TYPE_PD)
    {
      tm_cm_info.pd_call_id = call_info_ptr->call_id;
      tm_cm_info.phone_info.call[call_info_ptr->call_id].call_state = TM_CM_CALL_STATE_WAIT_CONNECT;

      MSG_HIGH("Answer incoming SO35/36 page",0,0,0);

      ap.info_type = CM_CALL_MODE_INFO_CDMA;
      ap.ans_params.cdma_ans.call_type =  CM_CALL_TYPE_PD;
      if(cm_mm_call_cmd_answer(
                           tm_cm_call_cmd_cb,
                           NULL,
                           tm_cm_info.cm_client,
                           tm_cm_info.pd_call_id,
                           &ap
                         ) == FALSE)
      {
        MSG_ERROR("cm_mm_call_cmd_answer failed",0,0,0);
      }
    }
    else
#endif /* FEATURE_CGPS_CDMA_IF */
    {
      tm_cm_info.phone_info.call[call_info_ptr->call_id].call_state = TM_CM_CALL_STATE_CONNECTED;
    }
  } /* end if INCOM */
  else if(call_event  == CM_CALL_EVENT_CONNECT)
  {
    tm_cm_info.phone_info.call[call_info_ptr->call_id].call_state = TM_CM_CALL_STATE_CONNECTED;
    tm_cm_info.phone_info.call[call_info_ptr->call_id].call_type  = call_info_ptr->call_type;

#ifdef FEATURE_CGPS_CDMA_IF
    if(call_info_ptr->call_type == CM_CALL_TYPE_PD)
    {
      tm_cm_info.pd_call_id = call_info_ptr->call_id;
      tm_cm_cdma_set_inactivity_timer();
    }
    else
    {
      MSG_MED("Call with SO %d up",call_info_ptr->cdma_call_srv_opt, 0, 0);
    }

    tm_cm_info.phone_info.call[call_info_ptr->call_id].srv_opt    = call_info_ptr->cdma_call_srv_opt;
    /* Fire TC_UP event to 1X UP if its registered for it */
    if(tm_cm_info.register_events_bmap & TM_CM_REGISTER_TC_UP_EVENT)  /*lint !e655 */
    {
      tm_core_cm_notify_event(TM_PHONE_EVENT_TC_UP, NULL);
    }
#endif /* FEATURE_CGPS_CDMA_IF */
    MSG_MED("CONNECT event CallId=%d, CallState=%d SrvOpt=%d", 
             call_info_ptr->call_id, 
             tm_cm_info.phone_info.call[call_info_ptr->call_id].call_state, 
             tm_cm_info.phone_info.call[call_info_ptr->call_id].srv_opt); 
  } /* End if CONNECT */
  else if(call_event == CM_CALL_EVENT_END)
  {
    MSG_MED("END event CallId=%d, CallState=%d CallType=%d", 
             call_info_ptr->call_id, 
             tm_cm_info.phone_info.call[call_info_ptr->call_id].call_state, 
             call_info_ptr->call_type);

    tm_cm_info.phone_info.call[call_info_ptr->call_id].call_state = TM_CM_CALL_STATE_IDLE;

    if(((call_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY) || 
        ((tm_agps_emergency_call_simulate() == TRUE) && (call_info_ptr->call_type == CM_CALL_TYPE_VOICE)))
       &&((tm_cm_info.e911_state == TM_CM_E911_STATE_ORIG) ||
          b_srvcc == TRUE))
    {
      MSG_MED("911 call ended without entering CB CallType=%d, srvcc=%d",call_info_ptr->call_type,b_srvcc,0);
      tm_cm_handle_emergency_call_end_event();
      b_srvcc = FALSE;
    }
#ifdef FEATURE_CGPS_CDMA_IF
#ifdef FEATURE_HDR
    /* Only consider NON HDR call events */
    if(TM_CM_ISDATA_HDR(tm_cm_info.phone_info.call[call_info_ptr->call_id].srv_opt) == FALSE)
#endif /* FEATURE_HDR */
    {
      /* At any given time, only 1 1xCP call can be active. Further, once this 
       * call goes down, no more DBMs can be sent by the server to 1xCP IS801.
       * Therefore If:-
       *      A CDMA MT session is active, AND
       *      Ended Call was on SYS MODE CDMA 
       * Cleanup and send stop indication to 1X CP.
       */
      if (tm_cm_info.cdma_mt_session_active)
      {
        if (call_info_ptr->sys_mode == SYS_SYS_MODE_CDMA)
        {
          tm_cm_cdma_clean_up();
        }
        else
        {
          MSG_MED("Ignore CM_CALL_EVENT_END for calltype %d", 
                   call_info_ptr->call_type, 0, 0);
        }
      }

      /* In case PD call ended without session being active, clear inactivity timer */
      if( (call_info_ptr->call_type == CM_CALL_TYPE_PD) &&
          (!tm_cm_info.cdma_mt_session_active) )
      {
        tm_cm_cdma_clear_inactivity_timer();
      }
    }
#endif /* FEATURE_CGPS_CDMA_IF */
  } /* End if END */
  else if (call_event == CM_CALL_EVENT_EXIT_TC)
  {
    MSG_MED("TC down",0, 0, 0);
    /* Fire EXIT_TC event to 1X UP if its registered for it */
    tm_core_cm_notify_event(TM_PHONE_EVENT_TC_EXIT, NULL);
  }
  else if(call_event == CM_CALL_EVENT_SRV_OPT)
  {
#ifdef FEATURE_CGPS_CDMA_IF
    tm_cm_info.phone_info.call[call_info_ptr->call_id].srv_opt = call_info_ptr->cdma_call_srv_opt;
#endif /* FEATURE_CGPS_CDMA_IF */
  }
  else if (call_event == CM_CALL_EVENT_OTASP_STATUS)
  {
    MSG_MED("OTASP Status Committed",0, 0, 0);
#ifdef FEATURE_CGPS_CDMA_IF
    /* Fire OTASP_STATUS_COMMITTED event to 1X UP if is registered for it */
    tm_core_cm_notify_event(TM_PHONE_EVENT_OTASP_STATUS_COMMITTED, NULL);
#endif /* FEATURE_CGPS_CDMA_IF */
  }

  else if(call_event == CM_CALL_EVENT_HO_COMPLETE)
  {
#ifdef FEATURE_LTE
    /* if HO COMPLETE is due to SRVCC handle this event in TM */
    if( (call_info_ptr->voice_ho_type == SYS_VOICE_HO_SRVCC_L_2_G) ||
        (call_info_ptr->voice_ho_type == SYS_VOICE_HO_SRVCC_L_2_W ))
    {
      MSG_MED("TM_CM_IFACE: HO_COMPLETE_EVENT",0,0,0);
      tm_core_cm_notify_event(TM_PHONE_EVENT_HO_COMPLETE, NULL);
      
      b_srvcc = TRUE;
    }
#endif /* FEATURE_LTE */
  }
  else
  {
    MSG_MED("Call event %d not handled",call_event,0,0);
  }
}

/*===========================================================================

FUNCTION tm_cm_dsds_at_in_single_standby

DESCRIPTION
  This function returns TRUE if the AT is in single standby mode. 
  False otherwise 

DEPENDENCIES

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_dsds_at_in_single_standby(void)
{
#ifndef FEATURE_GNSS_SA

  if( ( SYS_MODEM_DS_PREF_SINGLE_STANDBY  == 
       tm_cm_info.dsds_ph_info.e_StandbyPref ) ||
       /*Consider Single Standby when phone is in Single Sim mode*/
       ( cm_get_device_mode() == SYS_MODEM_DEVICE_MODE_SINGLE_SIM )
     )
  {
    return TRUE;
  }
#endif /*  FEATURE_GNSS_SA*/
  return FALSE;
}

/*===========================================================================

FUNCTION tm_cm_get_current_dd_sys_mode
  This function gets the current serving system mode of the dedicated data sub

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  sys_sys_mode_e_type: DD sub's serving system mode

SIDE EFFECTS
  None

===========================================================================*/
sys_sys_mode_e_type tm_cm_get_current_dd_sys_mode(void)
{
  sys_modem_as_id_e_type     dd_sub_id    = tm_cm_dsds_get_dd_subs();
  sys_subs_feature_t         dd_feature_mode = tm_cm_get_subs_feature(dd_sub_id);

  uint8 stack  = 0;
  uint8 cdmaStack = 0;
  uint8 lteStack = 0;
  
  boolean   cdmaStackFound = FALSE;
  boolean   lteStackFound  = FALSE;

  sys_sys_mode_e_type  external_stack_sys_mode   =   SYS_SYS_MODE_NO_SRV;
  sys_sys_mode_e_type  sys_mode                  =   SYS_SYS_MODE_NO_SRV;
  
  /* Validate the data sub id */
  if ((dd_sub_id != SYS_MODEM_AS_ID_1) &&
    (dd_sub_id != SYS_MODEM_AS_ID_2) &&
    (dd_sub_id != SYS_MODEM_AS_ID_3))
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Default Data sub is not a valid sub_id: %u ",dd_sub_id);
    return SYS_SYS_MODE_NO_SRV;
  }
   
  switch (dd_feature_mode)  
  {
  case SYS_SUBS_FEATURE_MODE_SGLTE:
    external_stack_sys_mode = tm_cm_info.external_stack_ss_info.sys_mode;      
    if (external_stack_sys_mode != SYS_SYS_MODE_NO_SRV)
    {
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "tm_cm_get_current_dd_sys_mode: In SGLTE mode and xternal stack mode %u selected",external_stack_sys_mode);
      sys_mode = external_stack_sys_mode;
    }
    else
    {
      for (stack = 0; stack < TM_CM_MAX_NO_STACKS; stack++)
      {
        /* We anticipate only one GSM per sub for SGLTE. If that changes, then we have to revisit this check */
        if (((tm_cm_info.ss_info[dd_sub_id][stack].is_operational)|| (tm_cm_info.ss_info[dd_sub_id][stack].oper_to_non_oper_transition == TRUE)) &&
            (tm_cm_info.ss_info[dd_sub_id][stack].sys_mode != SYS_SYS_MODE_GSM))
        {
          sys_mode = tm_cm_info.ss_info[dd_sub_id][stack].sys_mode;
          break;
        }
      }

      if (sys_mode == SYS_SYS_MODE_NO_SRV)
      {
        /*Not camped on any stack*/        
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_get_current_dd_sys_mode: In SGLTE mode but not camped on any stack on DD sub = %u",dd_sub_id);
        sys_mode = SYS_SYS_MODE_NO_SRV;
      }  
    }
    break;

  case SYS_SUBS_FEATURE_MODE_SVLTE: 
  case SYS_SUBS_FEATURE_MODE_SRLTE:
  case SYS_SUBS_FEATURE_MODE_DUAL_MULTIMODE:
  {
    /* SRLTE is Single Radio LTE, where the target is capable of camping on both 1x and LTE.
     However, only one RAT can be on traffic since we have only one transeiver.
     We cannot have 1x voice and LTE data simultaneously */
  
    for (stack = 0; stack < TM_CM_MAX_NO_STACKS; stack++)
    {
      if ((tm_cm_info.ss_info[dd_sub_id][stack].is_operational == TRUE)||
          (tm_cm_info.ss_info[dd_sub_id][stack].oper_to_non_oper_transition == TRUE))
      {
        if (tm_cm_info.ss_info[dd_sub_id][stack].sys_mode == SYS_SYS_MODE_LTE)
        {
          lteStack = stack;
          lteStackFound = TRUE;
        }
        else if (tm_cm_info.ss_info[dd_sub_id][stack].sys_mode == SYS_SYS_MODE_CDMA)
        {
          cdmaStack = stack;
          cdmaStackFound = TRUE;
        }
      }
    }
  
    if (cdmaStackFound && lteStackFound)
    {
      /* In SVLTE mode */
      if(tm_core_is_umts_prefered())
      {
        sys_mode = tm_cm_info.ss_info[dd_sub_id][lteStack].sys_mode;
      }
      else
      {
        sys_mode = tm_cm_info.ss_info[dd_sub_id][cdmaStack].sys_mode;
      }
    }
    else if (cdmaStackFound)
    {
      sys_mode = tm_cm_info.ss_info[dd_sub_id][cdmaStack].sys_mode;
    }
    else if (lteStackFound)
    {
      sys_mode = tm_cm_info.ss_info[dd_sub_id][lteStack].sys_mode;
    }
    else 
    {
      /* Not SVLTE nor SGLTE, we have only 1 stack */
      sys_mode = tm_cm_info.ss_info[dd_sub_id][0].sys_mode;
    }
  }
  break;

  case SYS_SUBS_FEATURE_MODE_NORMAL:
  {
    /* Not SVLTE nor SGLTE, we have only 1 stack */
    sys_mode = tm_cm_info.ss_info[dd_sub_id][0].sys_mode;
  }
  break;
     
  default:
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_cm_get_current_dd_sys_mode: Unsupported Feature mode %u",dd_feature_mode);
    sys_mode = SYS_SYS_MODE_NO_SRV;
  }
    
  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_get_current_dd_sys_mode: DD SUBs sys_mode = %u",sys_mode);
  
  return sys_mode;
}

/*===========================================================================

FUNCTION tm_cm_dsds_get_dd_subs

DESCRIPTION
  This function returns the subscription id of current DD subs.

DEPENDENCIES

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
sys_modem_as_id_e_type tm_cm_dsds_get_dd_subs(void)
{
  MSG_MED("DD ID: %d",tm_cm_info.dsds_ph_info.e_DataSubs, 0, 0);
  return tm_cm_info.dsds_ph_info.e_DataSubs;
}

/*===========================================================================
FUNCTION  tm_cm_handle_ph_sub_pref_event

DESCRIPTION
  A function to handle CM_PH_EVENT_SUBSCRIPTION_PREF_INFO

DEPENDENCIES
  tm_cm_client_init must have been called.

RETURNS
  None

SIDE EFFECTS


===========================================================================*/
/*ARGSUSED*/
void tm_cm_handle_ph_sub_pref_event(
  cm_ph_event_e_type           subs_event,
  const cm_subs_info_s_type    *subs_info_ptr
)
{
  /* Store the AS ID for this mode */
  mmgsdi_session_type_enum_type session_type;

  if ( subs_info_ptr == NULL )
  {
    MSG_ERROR("subs_info_ptr = NULL",0,0,0);
    return;
  }

  switch(subs_event)
  {
    case CM_PH_EVENT_SUBSCRIPTION_PREF_INFO:
      /* Set Subs ID for given mode*/
      if(subs_info_ptr->is_default_data == TRUE)
      {
        tm_cm_info.dsds_ph_info.e_PrevDataSubs = tm_cm_dsds_get_dd_subs();
        tm_core_set_current_dd_subs_id( subs_info_ptr->subscription_id );

    /* Verify the data sub id passed is one of the subs */
    if ((subs_info_ptr->subscription_id == SYS_MODEM_AS_ID_1) ||
        (subs_info_ptr->subscription_id == SYS_MODEM_AS_ID_2) ||
      (subs_info_ptr->subscription_id == SYS_MODEM_AS_ID_3))
    {
        tm_cm_info.dsds_ph_info.e_DataSubs = subs_info_ptr->subscription_id;
      }
    else
    {
      MSG_ERROR("CM is passing a non-sub enum value. Default Data sub not updated", 0, 0, 0);
      return;
    }
      }

      /*For DSDS in Triton and up, use session type which is set for the as_id
        Two session type can not be possible for DSDS
      */
      session_type = (subs_info_ptr->session_type_gw != MMGSDI_MAX_SESSION_TYPE_ENUM)?
                        subs_info_ptr->session_type_gw : subs_info_ptr->session_type_1x;
#ifdef FEATURE_MMGSDI_SESSION_LIB    
      tm_mmgsdi_set_subscription_id(session_type,subs_info_ptr->subscription_id );
#endif
      break;
    default:
      MSG_ERROR("Unexpected PH SUBS event %d",subs_event,0,0);

  }
}/* tm_cm_handle_ph_sub_pref_event*/


/*===========================================================================

FUNCTION tm_cm_handle_ph_event

DESCRIPTION

DEPENDENCIES
  tm_cm_client_init must have been called.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void tm_cm_handle_ph_event(
    cm_ph_event_e_type           ph_event,
        /* Indicate phone event */
    const tm_cm_ph_info_s_type      *ph_info_ptr
        /* A pointer to a phone state information struct */
    )
{

    cgps_DSPrefType           dsPrefInfo;
    sys_modem_as_id_e_type    dd_subs_id;

#ifdef FEATURE_CGPS_CELLDB
    cgps_SSInfoType ss_info;
    memset(&ss_info, 0x00, sizeof(ss_info));
#endif /* FEATURE_CGPS_CELLDB */
    if(ph_info_ptr == NULL)
    {
      MSG_ERROR("tm_cm_handle_ph_event: Null ph_info_ptr",0,0,0);
      return;
    }

    switch(ph_event)
    {
      case CM_PH_EVENT_SYS_SEL_PREF :
        {
           MSG_MED("tm_cm_handle_ph_event: SYS_SEL_PREF ph_state %d, e911_state %d srvcc %d",
                  ph_info_ptr->ph_state,tm_cm_info.e911_state,b_srvcc);

          if((ph_info_ptr->ph_state == CM_PH_STATE_EMERG_CB) && 
             (tm_cm_info.e911_state != TM_CM_E911_STATE_CB))
            {
            /* IMPORTANT First update the state & then notify TM-Core */
            tm_cm_info.e911_state = TM_CM_E911_STATE_CB;
            tm_core_cm_notify_event(TM_PHONE_EVENT_E911CB_ENTER, NULL);
#ifdef FEATURE_CGPS_XTRA_T
            (void)tle_SendE911Events(C_TLE_EVENT_E911CB_ENTER);
#endif /* FEATURE_CGPS_XTRA_T */
            }
          else if((ph_info_ptr->ph_state == CM_PH_STATE_NORMAL) && 
                  (tm_cm_info.e911_state != TM_CM_E911_STATE_NONE) &&
                  (b_srvcc == FALSE))
            {
            /* IMPORTANT First update the state & then notify TM-Core */
            tm_cm_info.e911_state = TM_CM_E911_STATE_NONE;
            tm_core_cm_notify_event(TM_PHONE_EVENT_E911CB_EXIT, NULL);
#ifdef FEATURE_CGPS_XTRA_T
            (void)tle_SendE911Events(C_TLE_EVENT_E911CB_EXIT);
#endif /* FEATURE_CGPS_XTRA_T */

              #ifndef FEATURE_GNSS_SA
              tm_ruim_handle_post_e911(); /* for hot SIM swap */
              #endif /* ! FEATURE_GNSS_SA */
            }
          }
    break;

    case CM_PH_EVENT_OPRT_MODE :
      if(ph_info_ptr->lpm_on == TRUE)
      {
        uint8 sub_id = 0;
    uint8 stack_id = 0;
        /* Fire LPM event to TM-Core, Only fire phone going into LPM mode
               * Phone coming out of lpm is not important, phone getting onto a 
               * network is important, and that is a SYS SELECT event */
        tm_core_cm_notify_event(TM_PHONE_EVENT_SYS_MODE_LPM, NULL);
        
        MSG_MED("tm_cm_handle_ph_event: :TM CM Iface, Operating mode is CM_PH_EVENT_OPRT_MODE, ph_state:%d, lpm_on:%d",
                (int32) ph_info_ptr->ph_state, (int32) ph_info_ptr->lpm_on, 0);

        tm_cm_info.prev_sys_mode = tm_cm_info.sys_mode;
        tm_cm_info.sys_mode      = SYS_SYS_MODE_NO_SRV;

      /* Set NO_SRV to all the stacks for all subs */
        for (sub_id = 0; sub_id < TM_CM_MAX_NO_SUBS; sub_id++)
        {
#ifdef FEATURE_CGPS_CELLDB
          ss_info.subs_id =  sub_id;
          ss_info.number_of_stacks = TM_CM_MAX_NO_STACKS;
#endif /*FEATURE_CGPS_CELLDB*/
      for (stack_id = 0; stack_id < TM_CM_MAX_NO_STACKS; stack_id++)
      {
            /* Check for stack being operational or not is not required. As per CM
               operational flag is set to false when the device goes LPM */
          tm_cm_info.ss_info[sub_id][stack_id].prev_sys_mode = tm_cm_info.ss_info[sub_id][stack_id].sys_mode;
              tm_cm_info.ss_info[sub_id][stack_id].sys_mode   = SYS_SYS_MODE_NO_SRV;
              tm_cm_info.ss_info[sub_id][stack_id].srv_status = SYS_SRV_STATUS_NO_SRV;
#ifdef FEATURE_CGPS_CELLDB        
        ss_info.multisim_ssInfo[stack_id].ss_data_type  = CGPS_SSINFO_SRV_SYS;
              ss_info.multisim_ssInfo[stack_id].sys_mode = SYS_SYS_MODE_NO_SRV;        
#endif    
        }
          #ifdef FEATURE_CGPS_CELLDB
          /* Phone gone into LPM mode. Inform MGP so PUNC can be propogated */
          #ifdef FEATURE_CGPS_XTRA_T
          tle_SSChange(&ss_info);
          #endif /* FEATURE_CGPS_XTRA_T */
          mgp_SSChange(&ss_info);
          #endif /* FEATURE_CGPS_CELLDB */
        }
      }
              /* Fall through intentional */
            
    case CM_PH_EVENT_DUAL_STANDBY_PREF:
    {/* DSDS Mode changed*/

      tm_cm_info.dsds_ph_info.e_PrevDataSubs = tm_cm_dsds_get_dd_subs();
      tm_cm_info.dsds_ph_info.e_StandbyPref = ph_info_ptr->standby_pref;
      // Copy phone info to Local Copy
      /* Also set the global DD subs ID here */

      tm_cm_info.dsds_ph_info.e_ActiveSubs = ph_info_ptr->active_subs;
      tm_cm_info.dsds_ph_info.e_VoiceSubs = ph_info_ptr->default_voice_subs;

        /* Verify the data sub passed is one of the subs */
        if ((ph_info_ptr->default_data_subs == SYS_MODEM_AS_ID_1) ||
            (ph_info_ptr->default_data_subs == SYS_MODEM_AS_ID_2) ||
            (ph_info_ptr->default_data_subs == SYS_MODEM_AS_ID_3))
        {
      tm_cm_info.dsds_ph_info.e_DataSubs = ph_info_ptr->default_data_subs;
        }
        else
        {
          MSG_ERROR("tm_cm_handle_ph_event: CM is passing a non-sub enum value. Default Data sub not updated", 0, 0, 0);
        return;
        }
        
      tm_cm_info.dsds_ph_info.e_PrioritySubs = ph_info_ptr->priority_subs;

      tm_core_set_current_dd_subs_id( tm_cm_dsds_get_dd_subs());
    
      MSG_3( MSG_SSID_GPSSM, MSG_LEGACY_MED,
             "tm_cm_handle_ph_event: DSDS Settings changed: Pref %d, Active %d, DD %d, DD Stack %d",
             tm_cm_info.dsds_ph_info.e_StandbyPref, 
             tm_cm_info.dsds_ph_info.e_ActiveSubs, 
             tm_cm_info.dsds_ph_info.e_DataSubs);
        
      // Send IPC to MGP and TLE
      dsPrefInfo.standby_pref = (uint8)tm_cm_info.dsds_ph_info.e_StandbyPref;
      dsPrefInfo.active_subs =  (uint8) tm_cm_info.dsds_ph_info.e_ActiveSubs ;
      dsPrefInfo.voice_subs = (uint8)tm_cm_info.dsds_ph_info.e_VoiceSubs;
      dsPrefInfo.data_subs =  (uint8) tm_cm_info.dsds_ph_info.e_DataSubs ;
      dsPrefInfo.priority_subs =  (uint8) tm_cm_info.dsds_ph_info.e_PrioritySubs;

      mgp_DSPrefUpdate(&dsPrefInfo);
      #ifdef FEATURE_CGPS_XTRA_T
      tle_DSPrefUpdate(&dsPrefInfo);
      #endif /* FEATURE_CGPS_XTRA_T */

      //Send update for sys mode change etc (if needed)to tm core...
      /*Retrieve the DD Subscription ID*/
      if (((dd_subs_id =tm_cm_dsds_get_dd_subs())!= SYS_MODEM_AS_ID_NONE) &&
            (dd_subs_id !=  tm_cm_info.dsds_ph_info.e_PrevDataSubs ))
      {

         MSG_HIGH("tm_cm_handle_ph_event: DSDS Data Subs changed from %d to %d",
                     tm_cm_info.dsds_ph_info.e_PrevDataSubs, dd_subs_id, 0);

         /* Send Notification for ending current UP Session*/
         tm_core_cm_notify_event(TM_PHONE_EVENT_DSDS_SUBS_CHANGED, NULL);

         /*  Send Notification about Srv Status*/
         tm_core_cm_notify_srv_status();
       }              
     }
     break;

   default:   
     MSG_ERROR("tm_cm_handle_ph_event: Unexpected PH event %d",ph_event,0,0);
     break;
  }
}

/*===========================================================================

FUNCTION tm_cm_dsds_is_dd_subs

DESCRIPTION
   This function checks if the subscription id matches with current 
    DD subscription
DEPENDENCIES

RETURN VALUE
  TRUE : ASID is same as dd subscription
  FALSE : ASID does not match with DD subscription.

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_dsds_is_dd_subs(
  sys_modem_as_id_e_type     asubs_id
)
{ 
  if (tm_cm_info.dsds_ph_info.e_DataSubs == asubs_id)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }  
}


/*===========================================================================

FUNCTION tm_cm_process_srv_changed

DESCRIPTION
  This function processes changes for all the subs

DEPENDENCIES
  tm_cm_client_init must have been called.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void tm_cm_process_srv_changed(
    const tm_cm_mm_msim_ss_stack_info_s_type *ss_stack_info_ptr, sys_modem_as_id_e_type sub_id, uint8 stack_id
)
{

#ifdef FEATURE_CGPS_UMTS_CP    
    tm_phone_event_payload_u_type *cm_evt_payload_ptr=NULL;
#endif /* FEATURE_CGPS_UMTS_CP */

#ifndef FEATURE_GNSS_SA
    if ( ( ss_stack_info_ptr->changed_fields & CM_SS_EVT_SRV_STATUS_MASK ) ||
         ( ss_stack_info_ptr->changed_fields & CM_SS_EVT_SYS_MODE_MASK) ||
         ( ss_stack_info_ptr->changed_fields & CM_SS_EVT_IDLE_DIGITAL_MODE_MASK )
        )
    {
      MSG_MED ("tm_cm_process_srv_changed: hdr_hybrid %d", ss_stack_info_ptr->hdr_hybrid,0,0); 
    if((ss_stack_info_ptr->hdr_hybrid) && (sys_srv_status_is_srv(ss_stack_info_ptr->srv_status)) &&
        (ss_stack_info_ptr->sys_mode == SYS_SYS_MODE_HDR))
      {
        MSG_MED ("tm_cm_process_srv_changed: hdr_hybrid in service", 0, 0, 0);
        tm_cm_info.ss_info[sub_id][stack_id].sys_mode = SYS_SYS_MODE_HDR;
      }
      else
      {
        MSG_MED ("tm_cm_process_srv_changed: hdr_hybrid not in service", 0, 0, 0);
        tm_cm_info.ss_info[sub_id][stack_id].sys_mode = ss_stack_info_ptr->sys_mode;
      }

      if ( (sys_srv_status_is_srv(ss_stack_info_ptr->srv_status) == FALSE) 
#ifdef FEATURE_HDR
            &&(sys_srv_status_is_srv(tm_cm_info.hdr_srv_status) == FALSE) 
#endif /* HDR is enabled */
         )
      {
        MSG_MED ("tm_cm_process_srv_changed: No Service", 0, 0, 0);
        tm_cm_info.ss_info[sub_id][stack_id].sys_mode =  SYS_SYS_MODE_NO_SRV;  
      }

      /*Notify TM core*/
      tm_core_cm_notify_srv_status();          
    }

#endif /* FEATURE_GNSS_SA */
#ifdef FEATURE_CGPS_UMTS_CP    
    if ( ( ss_stack_info_ptr->changed_fields & CM_SS_EVT_SRV_STATUS_MASK ) || 
         ( ss_stack_info_ptr->changed_fields & CM_SS_EVT_SYS_MODE_MASK )
       )
    {
      cm_evt_payload_ptr = 
           (tm_phone_event_payload_u_type *)pdsm_getbuf(sizeof(tm_phone_event_payload_u_type));
      if(cm_evt_payload_ptr != NULL)
      {
        cm_evt_payload_ptr->ss_info.srv_status = ss_stack_info_ptr->srv_status;
        cm_evt_payload_ptr->ss_info.sys_mode = ss_stack_info_ptr->sys_mode; 
        cm_evt_payload_ptr->ss_info.prev_sys_mode = tm_cm_info.prev_sys_mode;
        cm_evt_payload_ptr->ss_info.srv_domain = ss_stack_info_ptr->srv_domain;
        memscpy((void *)&(cm_evt_payload_ptr->ss_info.plmn), 
               sizeof(cm_evt_payload_ptr->ss_info.plmn),
               (const void *)&(ss_stack_info_ptr->sys_id_id_plmn), 
               sizeof(ss_stack_info_ptr->sys_id_id_plmn));

        /* processing of srv_status_mask and mode_mask identical, use SS_SRV_STATUS for all */
        tm_core_cm_notify_event(TM_PHONE_EVENT_SS_SRV_STATUS,cm_evt_payload_ptr); 
        (void)pdsm_freebuf((char *)cm_evt_payload_ptr);
      }
      else
      { 
        MSG_ERROR("tm_cm_process_srv_changed: No buffer available",0,0,0);
      }
    }

    if ( ss_stack_info_ptr->changed_fields & CM_SS_EVT_SRV_DOMAIN_MASK )
    {
      cm_evt_payload_ptr = 
         (tm_phone_event_payload_u_type *)pdsm_getbuf(sizeof(tm_phone_event_payload_u_type));
      if(cm_evt_payload_ptr != NULL)
      {
        cm_evt_payload_ptr->ss_info.srv_status = ss_stack_info_ptr->srv_status;
        cm_evt_payload_ptr->ss_info.srv_domain = ss_stack_info_ptr->srv_domain;
        tm_core_cm_notify_event(TM_PHONE_EVENT_SS_SRV_DOMAIN,cm_evt_payload_ptr);
        (void)pdsm_freebuf((char *)cm_evt_payload_ptr);
      }
      else
      { 
        MSG_ERROR("tm_cm_process_srv_changed: No buffer available",0,0,0);
      }
    }
#endif /* FEATURE_CGPS_UMTS_CP */

#ifdef FEATURE_MMGSDI_SESSION_LIB
  if (  (ss_stack_info_ptr->changed_fields & CM_SS_EVT_SRV_STATUS_MASK) ||
        (ss_stack_info_ptr->changed_fields & CM_SS_EVT_SYS_MODE_MASK) )
  {
    if ( cm_get_device_mode() == SYS_MODEM_DEVICE_MODE_SINGLE_SIM )
    {
      /* Device is a single SIM device .*/
      /* The system mode has changed .If it is G/W/L Inform the MMGSDI to update its IMSI table */
      if(
          (ss_stack_info_ptr->sys_mode == SYS_SYS_MODE_WCDMA) ||
          (ss_stack_info_ptr->sys_mode == SYS_SYS_MODE_LTE) ||
          (ss_stack_info_ptr->sys_mode == SYS_SYS_MODE_GSM) ||
          (ss_stack_info_ptr->sys_mode == SYS_SYS_MODE_TDS)
        )
      {

        MSG_MED( "tm_cm_process_srv_changed: Sys mode change. Update mmgsdi subs id. sys_mode : %d, asubs_id : %d, sess_type: %d", 
                  ss_stack_info_ptr->sys_mode, 
                  sub_id, 
                  MMGSDI_GW_PROV_PRI_SESSION );
        tm_mmgsdi_set_subscription_id(MMGSDI_GW_PROV_PRI_SESSION, sub_id);
      }
      else
      {
        MSG_MED( "tm_cm_process_srv_changed: Sys mode change. Update mmgsdi subs id. sys_mode : %d, asubs_id : %d, sess_type : %d", 
                  ss_stack_info_ptr->sys_mode, 
                  sub_id, 
                  MMGSDI_1X_PROV_PRI_SESSION );
        tm_mmgsdi_set_subscription_id(MMGSDI_1X_PROV_PRI_SESSION, sub_id);
      }
    }// end of if ( cm_get_device_mode() == SYS_MODEM_DEVICE_MODE_SINGLE_SIM )
  }
#endif

}

#ifdef FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH
/*===========================================================================

FUNCTION tm_cm_cell_info_dch_report_handler

DESCRIPTION
  This function is called as a result of request from TM to obtain a valid cell
  ID from CM in the CELL_DCH mode.

PARAMETERS 
  ss_info_ptr : A pointer to a serving system information structure  .
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_cm_cell_info_dch_report_handler(const tm_cm_mm_ss_info_s_type  *ss_info_ptr, uint8 dd_stack_id)
{

  MSG_MED( "Inside tm_cm_cell_info_dch_report_handler() RAT : %d LAC_ID : %d", 
            ss_info_ptr->multisim_stack_info[dd_stack_id].sys_mode, 
            ss_info_ptr->multisim_stack_info[dd_stack_id].cell_info.lac_id, 
            0 );

  // Initial status value. 
  tm_cm_info.rrc_cell_id_status = RRC_NO_CELL_ID_AVAIL;
  /* Convert the cell id info obtained from CM into the RRC cell id structure. */
  tm_cm_info.rrc_cell_info.cell_id = ss_info_ptr->multisim_stack_info[dd_stack_id].cell_info.cell_id;

  tm_cm_info.rrc_cell_info.freq = ss_info_ptr->multisim_stack_info[dd_stack_id].cell_info.uarfcn_dl;
  tm_cm_info.rrc_cell_info.psc = ss_info_ptr->multisim_stack_info[dd_stack_id].cell_info.psc;

  /* Obtain the LAC information of the system. */
  /* Pack the lac_id obtained from CM ( word ) into the byte array for TM */
  b_packw(ss_info_ptr->multisim_stack_info[dd_stack_id].cell_info.lac_id, tm_cm_info.rrc_cell_info.lac, 0, 16);

  
  /* Obtain the plmn_id information of the system. */
  /* Refer to the sys_plmn_id_s_type to find the format of the plmn_id */
  /** Public Land Mobile Network (PLMN) ID.

  A PLMN ID is defined within a GSM network to be the combination of a Mobile
  Country Code (MCC) and Mobile Network Code (MNC). A PLMN ID is stored in
  three octets, as specified in 3G TS 24.008, Section 10.5.1.3 and shown below:

  <pre>
                                        Bit
                 |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |
                 |===============================================|
         octet 1 |      MCC Digit 2      |      MCC Digit 1      |
                 |-----------------------------------------------|
         octet 2 |      MNC Digit 3      |      MCC Digit 3      |
                 |-----------------------------------------------|
         octet 3 |      MNC Digit 2      |      MNC Digit 1      |
                 |===============================================|
  </pre> 
  [ Digit range is 0..9 ]
  */
  /* Convert the MCC/MNC information received in sys_plmn_id_s_type from CM into rrc_plmn_identity_type used by TM */
  tm_cm_info.rrc_cell_info.plmn_id = mmumts_convert_nas_plmn_id_to_rrc_plmn_id(ss_info_ptr->multisim_stack_info[dd_stack_id].cell_info.plmn_id);

  /* The cell ID returned is invalid */
  if(ss_info_ptr->multisim_stack_info[dd_stack_id].cell_info.cell_id == TM_CM_CELL_DCH_CELL_ID)
  {
    MSG_HIGH( "CELLIDDCH::CELL ID is %d . Invalid ", ss_info_ptr->multisim_stack_info[dd_stack_id].cell_info.cell_id, 0, 0 );
    tm_cm_info.rrc_cell_id_status = RRC_NO_CELL_ID_AVAIL;
  }
  else
  {
    MSG_HIGH( "CELLIDDCH::CELL ID is %d . Valid", tm_cm_info.rrc_cell_info.cell_id, 0, 0 );
    tm_cm_info.rrc_cell_id_status = RRC_NEW_CELL_ID_AVAIL;
  }

  /*Pass the cell info to protocol modules*/
  MSG_MED( "CELLIDDCH::Sending Cell ID info for CELL_DCH Mode to SUPL ", 0, 0, 0 );
  /*Pass it to SUPL module*/
  #ifdef FEATURE_CGPS_WCDMA
  tm_rrc_iface_cell_info_dch_report(tm_cm_info.rrc_cell_info, tm_cm_info.rrc_cell_id_status);
  #endif /* FEATURE_CGPS_WCDMA */

  MSG_MED( "CELLIDDCH::Sending Cell ID info for CELL_DCH Mode to GM ", 0, 0, 0 );

  /*Pass it to GM module*/
  #ifdef FEATURE_CGPS_WCDMA
  gm_rrc_cell_info_dch_report(tm_cm_info.rrc_cell_info, tm_cm_info.rrc_cell_id_status);
  #endif /* FEATURE_CGPS_WCDMA */

  MSG_MED( "CELLIDDCH::Sending Cell ID info for  CELL_DCH Mode to XTRA-T ", 0, 0, 0 );

  #ifdef FEATURE_CGPS_XTRA_T
  /*Pass it to XTRA-T*/
  tle_rrc_cell_info_dch_report(tm_cm_info.rrc_cell_info, tm_cm_info.rrc_cell_id_status);
  #endif

}
#endif //FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH

/*===========================================================================

FUNCTION tm_cm_handle_ss_event

DESCRIPTION
  This function handles the SS event from Call Manager

DEPENDENCIES
  tm_cm_client_init must have been called.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void tm_cm_handle_ss_event(
  cm_ss_event_e_type       ss_event,
  /* Indicate serving system event */
  
  const tm_cm_mm_ss_info_s_type    *ss_info_ptr
  /* A pointer to a serving system information structure */
)
{
  uint8 stack = 0;

#ifdef FEATURE_CGPS_UMTS_CP    
  tm_phone_event_payload_u_type *cm_evt_payload_ptr=NULL;
#endif /* FEATURE_CGPS_UMTS_CP */
  
  cgps_SSInfoType ss_info;
  memset(&ss_info, 0x00, sizeof(ss_info));
  
  if(NULL == ss_info_ptr)
  {
  MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_cm_handle_ss_event: Null ss_info_ptr");
    return;
  }

  if (ss_info_ptr->number_of_stacks > TM_CM_MAX_NO_STACKS)
  {
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
    "tm_cm_handle_ss_event: Number of stacks from CM (%u) is greater than max stacks allowed (%u)",ss_info_ptr->number_of_stacks, TM_CM_MAX_NO_STACKS);
  return;
  }
  
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_handle_ss_event: Handling ss_event %u for asub_id = %u, with no_of_stacks = %u",
          (uint8)ss_event, ss_info_ptr->asubs_id, ss_info_ptr->number_of_stacks);

  /* Check if CM transitioned a stack from operational to non-operational */
  for (stack = 0; stack < ss_info_ptr->number_of_stacks; stack++)
  {
    if ((tm_cm_info.ss_info[ss_info_ptr->asubs_id][stack].is_operational) && 
      (!ss_info_ptr->multisim_stack_info[stack].is_operational))
  {
    tm_cm_info.ss_info[ss_info_ptr->asubs_id][stack].oper_to_non_oper_transition = TRUE;
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "tm_cm_handle_ss_event: Stack %u for sub_id %u became non-operational",
            stack,ss_info_ptr->asubs_id);
    }
  }
  
  switch (ss_event)
  {
    case CM_SS_EVENT_SRV_CHANGED:
    {
      boolean bSSChanged = FALSE, bCIDChanged = FALSE, bSIDChanged = FALSE, bBSParamsChanged = FALSE;
    
      /* Just need to map it once for both stacks (CGPS_SSINFO_SRV_SYS) */
      tm_cm_map_ss_info(ss_info_ptr, &ss_info, CGPS_SSINFO_SRV_SYS);

  for (stack = 0; stack < ss_info_ptr->number_of_stacks; stack++)
  {
      if ((ss_info_ptr->multisim_stack_info[stack].is_operational == TRUE)||
      (tm_cm_info.ss_info[ss_info_ptr->asubs_id][stack].oper_to_non_oper_transition == TRUE))
    {
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_handle_ss_event: SS event changed fields = 0x%08x%08x for stack = %d", 
                  (uint32)(ss_info_ptr->multisim_stack_info[stack].changed_fields >> 32), 
                  (uint32)(ss_info_ptr->multisim_stack_info[stack].changed_fields & 0xFFFFFFFF), stack);
  
          if ((ss_info_ptr->multisim_stack_info[stack].changed_fields & CM_SS_EVT_SRV_STATUS_MASK) ||
              (ss_info_ptr->multisim_stack_info[stack].changed_fields & CM_SS_EVT_SYS_MODE_MASK) ||
       (ss_info_ptr->multisim_stack_info[stack].changed_fields & CM_SS_EVT_IDLE_DIGITAL_MODE_MASK) ||
       (ss_info_ptr->multisim_stack_info[stack].changed_fields & CM_SS_EVT_SRV_DOMAIN_MASK)
       )
    {
      /* processing of srv_status_mask and mode_mask identical */
      MSG_4( MSG_SSID_GPSSM, MSG_LEGACY_MED,
        "tm_cm_handle_ss_event: SRV_STATUS_MASK / MODE_MASK set by CM for stack %d, srv_status=%d sys_mode=%d, srv_domain=%d",
          stack,
          ss_info_ptr->multisim_stack_info[stack].srv_status,
          ss_info_ptr->multisim_stack_info[stack].sys_mode,
          ss_info_ptr->multisim_stack_info[stack].srv_domain);

#ifdef FEATURE_CGPS_CELLDB
      MSG_4( MSG_SSID_GPSSM, MSG_LEGACY_MED,
             "tm_cm_handle_ss_event: SUB_ID and SRV SYS sent to MGP/TLE for subs_id = %d, stack = %d, srv_status=%d, sys_mode = %d",
                ss_info.subs_id, stack, ss_info.multisim_ssInfo[stack].srv_status, ss_info.multisim_ssInfo[stack].sys_mode);
  
#ifdef FEATURE_CGPS_LBS_TASK
      if ((ss_info_ptr->multisim_stack_info[stack].hdr_hybrid) && 
        (ss_info_ptr->multisim_stack_info[stack].srv_status == SYS_SRV_STATUS_SRV))
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_handle_ss_event: Updating lbs_update_roaming_status for hdr");
      lbs_update_roaming_status(ss_info_ptr->multisim_stack_info[stack].roam_status);
      }
            else if (SYS_SRV_STATUS_SRV == ss_info_ptr->multisim_stack_info[stack].srv_status)
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_handle_ss_event: Updating lbs_update_roaming_status for non-hdr");
      lbs_update_roaming_status(ss_info_ptr->multisim_stack_info[stack].roam_status);
      }
#endif /*FEATURE_CGPS_LBS_TASK*/
  
            bSSChanged = TRUE; /* serving system has changed, will notify MGP/TLE */
#endif /*FEATURE_CGPS_CELLDB*/
        
      /*Update the Mode settings for this subs ID*/
      tm_cm_info.ss_info[ss_info.subs_id][stack].is_operational = ss_info_ptr->multisim_stack_info[stack].is_operational;
      tm_cm_info.ss_info[ss_info.subs_id][stack].prev_sys_mode = tm_cm_info.ss_info[ss_info.subs_id][stack].sys_mode;
      tm_cm_info.ss_info[ss_info.subs_id][stack].sys_mode    = ss_info_ptr->multisim_stack_info[stack].sys_mode;
      tm_cm_info.ss_info[ss_info.subs_id][stack].srv_status    = ss_info_ptr->multisim_stack_info[stack].srv_status;
      tm_cm_info.ss_info[ss_info.subs_id][stack].srv_domain    = ss_info_ptr->multisim_stack_info[stack].srv_domain;
            tm_cm_info.ss_info[ss_info.subs_id][stack].changed_fields = ss_info_ptr->multisim_stack_info[stack].changed_fields;

		  /* Handle Cell Info update for if CM_SS_EVT_CELL_INFO_MASK is set */
          if ((ss_info_ptr->multisim_stack_info[stack].changed_fields & CM_SS_EVT_CELL_INFO_MASK)!=0)
          {
            
			MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_handle_ss_event: CELL_INFO_MASK sub=%d stack=%d sys_mode=%d as_sys_mode=%d",
				     (uint32)ss_info_ptr->asubs_id, stack, (uint32)ss_info_ptr->multisim_stack_info[stack].sys_mode, 
				     (uint32)ss_info_ptr->multisim_stack_info[stack].cell_info.as_sys_mode);
			
			tm_cm_info.ss_info[ss_info.subs_id][stack].as_sys_mode = ss_info_ptr->multisim_stack_info[stack].cell_info.as_sys_mode;            
  
          } /* CM_SS_CELL_INFO_MASK bitmask changed */
		  
      /* Process service changed requests for data stack in DD sub */
      if (tm_cm_dsds_is_dd_subs(ss_info_ptr->asubs_id))
      {
              /* below function should set tm_cm_info.sys_mode properly for SGLTE/SVLTE/other */
        tm_cm_process_srv_changed(&(ss_info_ptr->multisim_stack_info[stack]), ss_info_ptr->asubs_id, stack);
      }
          }  /* CM_SS_EVT_SRV_STATUS_MASK & CM_SS_EVT_SYS_MODE_MASK etc bitmask changed */


          /* Handle Cell Info update for TLE/GM/AGNSS if CM_SS_EVT_CELL_INFO_MASK is set */
          if (ss_info_ptr->multisim_stack_info[stack].changed_fields & CM_SS_EVT_CELL_INFO_MASK)
          {
            MSG_MED( "tm_cm_handle_ss_event: CELL_INFO_MASK sub=%d stack=%d sys_mode=%d", 
                     (uint32)ss_info_ptr->asubs_id, stack, (uint32)ss_info_ptr->multisim_stack_info[stack].sys_mode);

            /* 1) Handle Cell Info update for TLE (both DD & non-DD) */
            bCIDChanged = TRUE; /* Cell ID has changed, will notify TLE */

            /* 2) Handle Cell Info update for GM/AGNSS (currently DD-sub only) */
            if (tm_cm_is_cell_info_update_needed(ss_info_ptr, stack))
            {
              tm_cm_process_cell_info_update(&(ss_info_ptr->multisim_stack_info[stack])); 
    }
  
          } /* CM_SS_EVT_CELL_INFO_MASK bitmask changed */


#ifdef FEATURE_CGPS_CDMA_IF    
#ifdef FEATURE_CGPS_CDMA_CELLDB    
      
    /* Handle only SID update */
          if ((ss_info_ptr->multisim_stack_info[stack].changed_fields & CM_SS_EVT_SID_MASK) &&
       !(ss_info_ptr->multisim_stack_info[stack].changed_fields & CM_SS_EVT_BASE_STATION_PARMS_CHGD_MASK))
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_handle_ss_event: CM_SS_SID_MASK set by CM, id_type= %u, sid=%u, nid=%u",
       ss_info_ptr->multisim_stack_info[stack].sys_id.id_type,
       ss_info_ptr->multisim_stack_info[stack].sys_id_id_is95_sid,
                    ss_info_ptr->multisim_stack_info[stack].sys_id_id_is95_nid);
      if (ss_info_ptr->multisim_stack_info[stack].sys_id.id_type == SYS_SYS_ID_TYPE_IS95)
      {
              /* Usually at most one stack (per sub) will call below functions */
              bSIDChanged = TRUE;
      }
    }
      
    /* Handle either 1. SID and base station lat/long update or 2. base station lat/long update only */
    if (ss_info_ptr->multisim_stack_info[stack].changed_fields & CM_SS_EVT_BASE_STATION_PARMS_CHGD_MASK)
    {
        MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_handle_ss_event: CM_SS_BASE_STATION_PARMS_CHGD_MASK set by CM, base_id=%d srch_win=%d",
        ss_info_ptr->multisim_stack_info[stack].mode_info_cdma_info_base_id,
        ss_info_ptr->multisim_stack_info[stack].mode_info_cdma_info_srch_win_n);
        MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_handle_ss_event: base_lat=%d base_long=%d",
      ss_info_ptr->multisim_stack_info[stack].mode_info_cdma_info_base_lat,
        ss_info_ptr->multisim_stack_info[stack].mode_info_cdma_info_base_long);
      
            bBSParamsChanged = TRUE;
    }
#endif /* FEATURE_CGPS_CDMA_CELLDB */
      
          if ((ss_info_ptr->multisim_stack_info[stack].changed_fields & CM_SS_EVT_P_REV_IN_USE_MASK) &&
       (ss_info_ptr->multisim_stack_info[stack].sys_mode == SYS_SYS_MODE_CDMA)) 
    {
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_handle_ss_event: CM_SS_P_REV_IN_USE_MASK set by CM, p_rev_in_use=%u",
        ss_info_ptr->multisim_stack_info[stack].mode_info_cdma_info_p_rev_in_use);
      tm_cm_info.p_rev_in_use = ss_info_ptr->multisim_stack_info[stack].mode_info_cdma_info_p_rev_in_use;
    }
#endif /* FEATURE_CGPS_CDMA_IF */

        } /* is_operational == TRUE */
      } /* for stack loop */


      /* make sure we only call below functions once per sub (ss_info has info for both stacks) */
      /* notify MGP about ServingSystem change, notify TLE about SS/CellID change */
#ifdef FEATURE_CGPS_CELLDB
      if (bSSChanged == TRUE || bCIDChanged == TRUE)
      {
#ifdef FEATURE_CGPS_XTRA_T
        tle_SSChange(&ss_info);  /* Call TLE functions for both SS change and CID change */
#endif /* FEATURE_CGPS_XTRA_T */
        if (bSSChanged == TRUE)
        {
          mgp_SSChange(&ss_info);  /* call MGP function only for SS change */
    }
  }  
#endif /* FEATURE_CGPS_CELLDB */
      
#ifdef FEATURE_CGPS_CDMA_IF
#ifdef FEATURE_CGPS_CDMA_CELLDB
      if (bSIDChanged == TRUE)
      {
        tm_cm_map_ss_info(ss_info_ptr, &ss_info, CGPS_SSINFO_SID);
#ifdef FEATURE_CGPS_XTRA_T
        tle_SSChange(&ss_info);
#endif /* FEATURE_CGPS_XTRA_T */
        mgp_SSChange(&ss_info);
      }

      if (bBSParamsChanged == TRUE)
      {
        tm_cm_map_ss_info(ss_info_ptr, &ss_info, CGPS_SSINFO_BS_LAT_LONG);
#ifdef FEATURE_CGPS_XTRA_T
        tle_SSChange(&ss_info);
#endif /* FEATURE_CGPS_XTRA_T */
        mgp_SSChange(&ss_info);
      }
#endif /* FEATURE_CGPS_CDMA_CELLDB */
#endif /* FEATURE_CGPS_CDMA_IF */

  break;
    }
  

  case CM_SS_EVENT_DCH_CELL_ID_RSP:
#ifdef FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH
  /* Check the value of the cell ID from CM */
  for (stack = 0; stack < ss_info_ptr->number_of_stacks; stack++)
  { 
      if(((ss_info_ptr->multisim_stack_info[stack].is_operational) || 
       (tm_cm_info.ss_info[ss_info_ptr->asubs_id][stack].oper_to_non_oper_transition == TRUE))&&
             (ss_info_ptr->multisim_stack_info[stack].sys_mode == SYS_SYS_MODE_WCDMA) &&
       (ss_info_ptr->multisim_stack_info[stack].cell_info.cell_id != TM_CM_CELL_DCH_CELL_ID))
    {
        /* only call this DCH function for WCDMA stack */
      MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_handle_ss_event: CELL_DCH: Valid cell_id reported in DCH response for sub %u, stack %u. Calling tm_cm_cell_info_dch_report_handler",
        ss_info.subs_id,stack);
    tm_cm_cell_info_dch_report_handler(ss_info_ptr, stack);
    }
  }
#endif /* FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH  */
  break;


    case CM_SS_EVENT_INFO:
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"tm_cm_handle_ss_event: SS info available");
    for (stack = 0; stack < ss_info_ptr->number_of_stacks; stack++)
    {
      if ((ss_info_ptr->multisim_stack_info[stack].is_operational) ||
      (tm_cm_info.ss_info[ss_info_ptr->asubs_id][stack].oper_to_non_oper_transition == TRUE))
      {
#ifdef FEATURE_CGPS_CELLDB 
        tm_cm_map_ss_info(ss_info_ptr, &ss_info, CGPS_SSINFO_SID);   
      if (ss_info_ptr->multisim_stack_info[stack].sys_id.id_type == SYS_SYS_ID_TYPE_IS95)
      {
#ifdef FEATURE_CGPS_XTRA_T
        MSG_MED("tm_cm_handle_ss_event: Updating TLE and MGP for event CM_SS_EVENT_INFO", 0, 0, 0);
          /* Usually at most one stack will be IS95 (per sub), so won't call below functions twice */
        tle_SSChange(&ss_info);
#endif /* FEATURE_CGPS_XTRA_T */
        mgp_SSChange(&ss_info);
      }
#endif /* FEATURE_CGPS_CELLDB */
  
#ifdef FEATURE_CGPS_UMTS_CP    
          cm_evt_payload_ptr = (tm_phone_event_payload_u_type *)pdsm_getbuf(sizeof(tm_phone_event_payload_u_type));
        if (cm_evt_payload_ptr != NULL)
      {
        cm_evt_payload_ptr->ss_info.srv_status = ss_info_ptr->multisim_stack_info[stack].srv_status;
        cm_evt_payload_ptr->ss_info.sys_mode = ss_info_ptr->multisim_stack_info[stack].sys_mode; 
        cm_evt_payload_ptr->ss_info.srv_domain = ss_info_ptr->multisim_stack_info[stack].srv_domain;
        memscpy((void *)&(cm_evt_payload_ptr->ss_info.plmn), sizeof(cm_evt_payload_ptr->ss_info.plmn),
              (const void *)&(ss_info_ptr->multisim_stack_info[stack].sys_id_id_plmn), 
            sizeof(ss_info_ptr->multisim_stack_info[stack].sys_id_id_plmn));
      
          tm_core_cm_notify_event(TM_PHONE_EVENT_SS_SRV_STATUS, cm_evt_payload_ptr);
      
        (void)pdsm_freebuf((char *)cm_evt_payload_ptr);
      }
      else
      {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"tm_cm_handle_ss_event: No buffer available");
      }
#endif /* FEATURE_CGPS_UMTS_CP */
      }
    }
    break;

  
    case CM_SS_EVENT_SRV_NEW: 
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"tm_cm_handle_ss_event: New SS info available");
    for (stack = 0; stack < ss_info_ptr->number_of_stacks; stack++)
    {
      if ((ss_info_ptr->multisim_stack_info[stack].is_operational)||
      (tm_cm_info.ss_info[ss_info_ptr->asubs_id][stack].oper_to_non_oper_transition == TRUE))
      {
#ifdef FEATURE_CGPS_LBS_TASK
      if ((ss_info_ptr->multisim_stack_info[stack].hdr_hybrid) && 
            (ss_info_ptr->multisim_stack_info[stack].srv_status == SYS_SRV_STATUS_SRV))
      {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED,"tm_cm_handle_ss_event: Updating lbs_update_roaming_status for hdr");
      lbs_update_roaming_status(ss_info_ptr->multisim_stack_info[stack].roam_status);
      }
        else if (SYS_SRV_STATUS_SRV == ss_info_ptr->multisim_stack_info[stack].srv_status)
      {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED,"tm_cm_handle_ss_event: Updating lbs_update_roaming_status for non-hdr");
      lbs_update_roaming_status(ss_info_ptr->multisim_stack_info[stack].roam_status);
      }
#endif
        /* Don't care what has changed, just tell 1X UP that BS info is available. */
        tm_core_cm_notify_event(TM_PHONE_EVENT_NEW_SS_INFO, NULL);
    }
    }
  
    break;
  
   default:
      break;
   }
  
  /* After handling SS event, if the oper_to_non_oper_transition happened, then set it back to FALSE (Reset) as we just handled it */
  for (stack = 0; stack < ss_info_ptr->number_of_stacks; stack++)
  {
  if (tm_cm_info.ss_info[ss_info_ptr->asubs_id][stack].oper_to_non_oper_transition)
  {
    tm_cm_info.ss_info[ss_info_ptr->asubs_id][stack].oper_to_non_oper_transition = FALSE;   
  }
  } 
    
#ifdef FEATURE_CGPS_XTRA_T
  for (stack = 0; stack < ss_info_ptr->number_of_stacks; stack++)
  {
    if (ss_info_ptr->multisim_stack_info[stack].is_operational)
    {
      if (ss_info_ptr->multisim_stack_info[stack].changed_fields & CM_SS_EVT_MOBILITY_MGMT_MASK)
      {
        MSG_MED("tm_cm_handle_ss_event: Updating TLE and MGP for CM_SS_EVT_MOBILITY_MGMT_MASK", 0, 0, 0);
      tm_cm_map_ss_info(ss_info_ptr, &ss_info, CGPS_SSINFO_TIMING_INFO);
      tle_SSChange(&ss_info);

        break;  // if one stack has this changed bitmask, we'll send entire ss_info (with both stacks)
      }
    }
  }
#endif
}

/*===========================================================================

FUNCTION tm_cm_call_event_cb

DESCRIPTION
  This function is a callback issued by CM whenever a call event happens.
  The TM_CM uses this callback to negotiate incoming pages for PD (SO 35/36)
  traffic channels.

DEPENDENCIES
  tm_cm_client_init must have been called.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/

void tm_cm_call_event_cb
(
  cm_call_event_e_type         call_event,
  const cm_mm_call_info_s_type    * call_info_ptr
)
{
  os_IpcMsgType           *ipc_msg_ptr = NULL;
  tm_cm_event_info_s_type *event_info_ptr;
  boolean status;

  MSG_HIGH("call_event %d rcv'ed", call_event,0,0);

  if(call_info_ptr == NULL)
  {
    MSG_ERROR("Null call_info_ptr",0,0,0);
    return;
  }

  /* Filter events before forwaring to TM-Core */
  if(call_event == CM_CALL_EVENT_INCOM)
  {
    if((call_info_ptr->call_type == CM_CALL_TYPE_PD) && (call_info_ptr->mode_info.info.cdma_call.alert_ans_bypass))
    {
      MSG_MED("alert_ans_bypass is TRUE. Don't answer PD call",0,0,0);
      return;
    }
  }
  else if(call_event == CM_CALL_EVENT_ORIG && call_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
  {
    /*Send 911 indications to MGP*/    
    mgp_UpdateEmergencyEvent(GNSS_EMERGENCY_EVENT_CALL_ORIGINATION);    
  }
  else if (call_event == CM_CALL_EVENT_END && call_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
  {  
    /*Send 911 indications to MGP*/    
    mgp_UpdateEmergencyEvent(GNSS_EMERGENCY_EVENT_CALL_END);
  }
#ifdef FEATURE_CGPS_CDMA_IF
  else if (call_event == CM_CALL_EVENT_OTASP_STATUS)
  {
    if ((call_info_ptr->mode_info.info_type != CM_CALL_MODE_INFO_CDMA) ||
        (call_info_ptr->mode_info.info.cdma_call.otasp_status != CM_OTASP_STATUS_COMMITTED))
    {
      MSG_LOW("OTASP call status ignored",0,0,0);
      return;
    }
  }
#endif /* FEATURE_CGPS_CDMA_IF */

  ipc_msg_ptr = os_IpcCreate(sizeof(tm_cm_event_info_s_type), IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_TM);
  /* Queue the commands in Client command Queue.
  */
  if (ipc_msg_ptr == NULL)
  {
    MSG_ERROR( "IPC message allocation failed",0,0,0 );
  }
  else 
  {
    event_info_ptr = (tm_cm_event_info_s_type*)ipc_msg_ptr->p_Data;

    event_info_ptr->event_info.cm_event_info.call_info.call_id      = call_info_ptr->call_id;
    event_info_ptr->event_info.cm_event_info.call_info.call_type    = call_info_ptr->call_type;
    event_info_ptr->event_info.cm_event_info.call_info.cdma_call_srv_opt =
      call_info_ptr->mode_info.info.cdma_call.srv_opt;
    event_info_ptr->event_info.cm_event_info.call_info.sys_mode = call_info_ptr->sys_mode;
    
#ifdef FEATURE_LTE
    event_info_ptr->event_info.cm_event_info.call_info.voice_ho_type =
      call_info_ptr->voice_ho_type;
#endif /* FEATURE_LTE */

    event_info_ptr->event_type = TM_CM_CALL_EVENT;
    event_info_ptr->event_info.cm_event_type.call_event = call_event;
    ipc_msg_ptr->q_MsgId  = TM_CORE_CM_MSG_ID_EVENT_NTFY;
    status    = os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM);
    if (status == FALSE)
    {
      MSG_ERROR("Error in sending CM Event msg_id=%d to TM thread",ipc_msg_ptr->q_MsgId,0,0);
      status = os_IpcDelete(ipc_msg_ptr);
      ipc_msg_ptr = NULL;
    } /* End status */
    else
    {
      MSG_HIGH("sending CM Event msg_id=%d to TM thread", ipc_msg_ptr->q_MsgId,0,0);
    } /* end else status */
  }
}

/*===========================================================================

FUNCTION tm_cm_ph_event_sub_pref_cb

DESCRIPTION
  This function is a callback with information about a subscription 
  id.

DEPENDENCIES
  tm_cm_client_init must have been called.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/

void tm_cm_ph_event_sub_pref_cb
(
  cm_ph_event_e_type           subs_event,
  const cm_subs_info_s_type    *subs_info_ptr
  /* A pointer to a phone state information struct */
)
{
  os_IpcMsgType           *ipc_msg_ptr = NULL;
  tm_cm_event_info_s_type *event_info_ptr;
  boolean status;

  boolean tm_handle_ph_sub_pref_event = FALSE;

  MSG_HIGH("ph_event %d rcv'ed", subs_event,0,0);

  if(subs_info_ptr == NULL)
  {
    MSG_ERROR("Null ph_info_ptr",0,0,0);
    return;
  }

  switch ( subs_event )
  {
    case CM_PH_EVENT_SUBSCRIPTION_PREF_INFO:
      MSG_MED( "Received CM_PH_EVENT_SUBSCRIPTION_PREF_INFO",0, 0, 0 ); 
      tm_handle_ph_sub_pref_event = TRUE;
      break;

    default: 
      MSG_ERROR( "Received other cm phone sub events %d",
               subs_event, 0, 0 ); 
  }

  if(tm_handle_ph_sub_pref_event == TRUE)
  {
    ipc_msg_ptr = os_IpcCreate(sizeof(tm_cm_event_info_s_type), IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_TM);
    /* Queue the commands in Client command Queue.
    */
    if (ipc_msg_ptr == NULL)
    {
      MSG_ERROR("IPC message allocation failed",0,0,0 );
    }
    else 
    {
      event_info_ptr = (tm_cm_event_info_s_type*)ipc_msg_ptr->p_Data;

      memscpy((void *)&event_info_ptr->event_info.cm_event_info.ph_subs_info,
               sizeof(event_info_ptr->event_info.cm_event_info.ph_subs_info),
               subs_info_ptr, sizeof(*subs_info_ptr));
      event_info_ptr->event_type = TM_CM_PH_SUB_PREF_EVENT;
      event_info_ptr->event_info.cm_event_type.subs_event = subs_event;
      ipc_msg_ptr->q_MsgId  = TM_CORE_CM_MSG_ID_EVENT_NTFY;
      status    = os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM);
      if (status == FALSE)
      {
        MSG_ERROR( "Error in sending CM Event msg_id=%d to TM thread",ipc_msg_ptr->q_MsgId,0,0);
        status = os_IpcDelete(ipc_msg_ptr);
        ipc_msg_ptr = NULL;
      } /* End status */
      else
      {
        MSG_HIGH( "sending CM Event msg_id=%d to TM thread", ipc_msg_ptr->q_MsgId,0,0);
      } /* end else status */
  }
  }


}

/*===========================================================================

FUNCTION tm_cm_ph_event_cb

DESCRIPTION
  This function is a callback issued by CM whenever a phone event happens.
  The TM_CM is only interested when the mode preferences changes to 911CB.

DEPENDENCIES
  tm_cm_client_init must have been called.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/

void tm_cm_ph_event_cb
(
    cm_ph_event_e_type           ph_event,
        /* Indicate phone event */
    const cm_ph_info_s_type      *ph_info_ptr
        /* A pointer to a phone state information struct */
)
{
  os_IpcMsgType           *ipc_msg_ptr = NULL;
  tm_cm_event_info_s_type *event_info_ptr;
  boolean status;
  static boolean u_in_emergency_mode = FALSE;

  MSG_HIGH("tm_cm_ph_event_cb: ph_event %d rcv'ed", ph_event,0,0);

  if(ph_info_ptr == NULL)
  {
    MSG_ERROR("tm_cm_ph_event_cb: Null ph_info_ptr",0,0,0);
    return;
  }
  ipc_msg_ptr = os_IpcCreate(sizeof(tm_cm_event_info_s_type), IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_TM);
  /* Queue the commands in Client command Queue.
  */
  if (ipc_msg_ptr == NULL)
  {
    MSG_ERROR( "tm_cm_ph_event_cb: IPC message allocation failed",0,0,0 );
  }
  else 
  {
    event_info_ptr = (tm_cm_event_info_s_type*)ipc_msg_ptr->p_Data;

    /* We only use the some of this large structure, so just copy the info needed 
     * to prevent overflowing the IPC memory
     */
    event_info_ptr->event_info.cm_event_info.ph_info.ph_state = ph_info_ptr->ph_state;
    event_info_ptr->event_info.cm_event_info.ph_info.standby_pref = ph_info_ptr->standby_pref;
    event_info_ptr->event_info.cm_event_info.ph_info.active_subs = ph_info_ptr->active_subs;
    event_info_ptr->event_info.cm_event_info.ph_info.default_voice_subs = ph_info_ptr->default_voice_subs;
    event_info_ptr->event_info.cm_event_info.ph_info.default_data_subs = ph_info_ptr->default_data_subs;
    event_info_ptr->event_info.cm_event_info.ph_info.priority_subs = ph_info_ptr->priority_subs;
  MSG_6(MSG_SSID_GPSSM, MSG_LEGACY_MED,
         "tm_cm_ph_event_cb: Phone info from CM - Phone state = %d, Standby Pref = %d, Active Sub = %d, Default Voice Sub = %d, Default Data Sub = %d, Priority Sub = %d",
         ph_info_ptr->ph_state,ph_info_ptr->standby_pref,ph_info_ptr->active_subs,ph_info_ptr->default_voice_subs,ph_info_ptr->default_data_subs,ph_info_ptr->priority_subs);


#if (defined(FEATURE_GNSS_PERIODIC_PPM) || defined (FEATURE_CGPS_CELLDB))
    if(ph_info_ptr->oprt_mode == SYS_OPRT_MODE_LPM)
    {
      event_info_ptr->event_info.cm_event_info.ph_info.lpm_on = TRUE;
    }
    else
    {
      event_info_ptr->event_info.cm_event_info.ph_info.lpm_on = FALSE;
    }
#endif
    if( (u_in_emergency_mode == FALSE) && 
        (event_info_ptr->event_info.cm_event_info.ph_info.ph_state == CM_PH_STATE_EMERG_CB)
      )
    {      
      /*Send 911 ECBM indication to MGP*/
      mgp_UpdateEmergencyEvent(GNSS_EMERGENCY_EVENT_ECBM_ACTIVE);
      u_in_emergency_mode = TRUE;
    }
    else if( (u_in_emergency_mode == TRUE) && 
        (event_info_ptr->event_info.cm_event_info.ph_info.ph_state == CM_PH_STATE_NORMAL)
      )
    {
      /*Send 911 ECBM indication to MGP*/    
      mgp_UpdateEmergencyEvent(GNSS_EMERGENCY_EVENT_ECBM_NONE);
      u_in_emergency_mode = FALSE;
    }
    
    event_info_ptr->event_type = TM_CM_PH_EVENT;
    event_info_ptr->event_info.cm_event_type.ph_event = ph_event;
    ipc_msg_ptr->q_MsgId  = TM_CORE_CM_MSG_ID_EVENT_NTFY;
    status    = os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM);
    if (status == FALSE)
    {
      MSG_ERROR("Error in sending CM Event msg_id=%d to TM thread",ipc_msg_ptr->q_MsgId,0,0);
      status = os_IpcDelete(ipc_msg_ptr);
      ipc_msg_ptr = NULL;
    } /* End status */
    else
    {
      MSG_HIGH("sending CM Event msg_id=%d to TM thread", ipc_msg_ptr->q_MsgId,0,0);
    } /* end else status */
  }
}

/*===========================================================================

FUNCTION tm_cm_ss_event_cb

DESCRIPTION
  This function is a callback issued by CM whenever SS event has occurred.

DEPENDENCIES
  tm_cm_client_init must have been called.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/

void tm_cm_ss_event_cb
(
    cm_ss_event_e_type           ss_event,
        /* Indicate serving system event */

    const cm_mm_msim_ss_info_s_type      *ss_info_ptr
        /* A pointer to a serving system information structure */
)
{
  os_IpcMsgType           *ipc_msg_ptr = NULL;
  tm_cm_event_info_s_type *event_info_ptr;
  boolean status;
  boolean tm_handle_ss_event = FALSE;
  uint8 stack = 0;

  if(NULL == ss_info_ptr)
  {
  MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_cm_ss_event_cb: Null ss_info_ptr sent from CM ss cb");
    return;
  }

  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_ss_event_cb: Handling SS event %u for asubs_id %u...",
            ss_event, ss_info_ptr->asubs_id);
  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_ss_event_cb: Total number of stacks for asubs_id %u is %u",
            ss_info_ptr->asubs_id, ss_info_ptr->number_of_stacks);
  
  switch (ss_event)
  {
    case CM_SS_EVENT_SRV_CHANGED:
    for (stack = 0; stack < ss_info_ptr->number_of_stacks; stack++)
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_ss_event_cb: Stack: %u operational: %u. Changed fields: 0x%x",
            stack, ss_info_ptr->stack_info[stack].is_operational, ss_info_ptr->stack_info[stack].changed_fields);
    
        if((ss_info_ptr->stack_info[stack].sys_mode == SYS_SYS_MODE_HDR)&&
           (ss_info_ptr->stack_info[stack].changed_fields & CM_SS_EVT_SRV_STATUS_MASK))
        {
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_ss_event_cb: HDR SRV Status changed to %u",ss_info_ptr->stack_info[stack].srv_status);

          /*If hdr_hybrid is false, then hdr serving status need not be checked*/
          if(ss_info_ptr->stack_info[stack].srv_status != SYS_SRV_STATUS_NO_SRV)
          {
            tm_cm_info.hdr_srv_status = ss_info_ptr->stack_info[stack].srv_status;
          }
          else
          {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "tm_cm_ss_event_cb: HDR Hybrid is not set, treating the HDR serving status as NO SRV");
            tm_cm_info.hdr_srv_status = SYS_SRV_STATUS_NO_SRV;
          }
          tm_handle_ss_event = TRUE;
        }
    }

    for (stack = 0; stack < ss_info_ptr->number_of_stacks; stack++)
    {
      if (
              (ss_info_ptr->stack_info[stack].changed_fields & CM_SS_EVT_SRV_STATUS_MASK ) ||
              (ss_info_ptr->stack_info[stack].changed_fields & CM_SS_EVT_SYS_MODE_MASK) ||
              (ss_info_ptr->stack_info[stack].changed_fields & CM_SS_EVT_SRV_DOMAIN_MASK ) ||
              (ss_info_ptr->stack_info[stack].changed_fields & CM_SS_EVT_SID_MASK ) ||
              (ss_info_ptr->stack_info[stack].changed_fields & CM_SS_EVT_BASE_STATION_PARMS_CHGD_MASK) ||
              (ss_info_ptr->stack_info[stack].changed_fields & CM_SS_EVT_P_REV_IN_USE_MASK) ||
              (ss_info_ptr->stack_info[stack].changed_fields & CM_SS_EVT_IDLE_DIGITAL_MODE_MASK ) ||
              (ss_info_ptr->stack_info[stack].changed_fields & CM_SS_EVT_MOBILITY_MGMT_MASK) ||
              (ss_info_ptr->stack_info[stack].changed_fields & CM_SS_EVT_CELL_INFO_MASK)
           )
      {
        tm_handle_ss_event = TRUE;
      break;
      }
    }
      break;

    case CM_SS_EVENT_DCH_CELL_ID_RSP:
#ifdef FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH      
      /* even if cell_id is still invalid, this WCDMA CID request is done */
      tm_cm_info.ts_wcdma_cid_update_req = 0;  

      for (stack = 0; stack < ss_info_ptr->number_of_stacks; stack++)
    {
          /* Check if the mask is set. If yes then it is a response to cm_ss_cmd_get_dch_cell_info() request from the client */  
     
          if(ss_info_ptr->stack_info[stack].cell_info.cell_id != TM_CM_CELL_DCH_CELL_ID)
          {
            /* The cell ID has changed when the phone is in CELL_DCH mode . Report it to the clients. */        
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_ss_event_cb: CELL_DCH: Valid cell_id reported in DCH response. Sending IPC to tm_task");
            tm_handle_ss_event = TRUE;
          }
      }
#endif /*FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH*/
      break;
    case CM_SS_EVENT_SRV_NEW:
    case CM_SS_EVENT_INFO:
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_ss_event_cb: Event CM_SS_EVENT_SRV_NEW/CM_SS_EVENT_INFO received. Setting tm_handle_ss_event = TRUE");
      tm_handle_ss_event = TRUE;
      break;

    default:
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_ss_event_cb: Ignore SS event");
      tm_handle_ss_event = FALSE;
      break;
  }

  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_ss_event_cb: TM handle SS event %u", tm_handle_ss_event);

  if(tm_handle_ss_event == TRUE)
  {
    ipc_msg_ptr = os_IpcCreate(sizeof(tm_cm_event_info_s_type), IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_TM);
    /* Queue the commands in Client command Queue.
        */
    if (ipc_msg_ptr == NULL)
    {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_cm_ss_event_cb: IPC message allocation failed");
    }
    else 
    {
      event_info_ptr = (tm_cm_event_info_s_type*)ipc_msg_ptr->p_Data;

    event_info_ptr->event_info.cm_event_info.ss_info.asubs_id = ss_info_ptr->asubs_id;
    event_info_ptr->event_info.cm_event_info.ss_info.number_of_stacks = ss_info_ptr->number_of_stacks;
    event_info_ptr->event_info.cm_event_info.ss_info.changed_fields_subs = ss_info_ptr->changed_fields_subs;

    for (stack = 0; stack < ss_info_ptr->number_of_stacks; stack++)
    {
      event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].is_operational    = 
                                           ss_info_ptr->stack_info[stack].is_operational;
      
    /* If stack is non-operational, then this stack is considered OOS. */
    if (!ss_info_ptr->stack_info[stack].is_operational)
      {       
      event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].srv_status    = SYS_SRV_STATUS_NO_SRV;
      event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].srv_domain      = SYS_SRV_DOMAIN_NO_SRV;
      event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].sys_mode        = SYS_SYS_MODE_NO_SRV;
      /* Make sure the changed fields are set as well, so that they are handled */
        event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].changed_fields    = 
        (CM_SS_EVT_SRV_STATUS_MASK | CM_SS_EVT_SYS_MODE_MASK | CM_SS_EVT_IDLE_DIGITAL_MODE_MASK | CM_SS_EVT_SRV_DOMAIN_MASK);     
    }
    else
    {          
    event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].srv_status     = 
                                           ss_info_ptr->stack_info[stack].srv_status;
    event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].srv_domain    = 
                                           ss_info_ptr->stack_info[stack].srv_domain;
    event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].sys_mode    = 
                                           ss_info_ptr->stack_info[stack].sys_mode;
      event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].changed_fields    = 
                                           ss_info_ptr->stack_info[stack].changed_fields;
    }   
    
    /* Mode info*/
    event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].mode_info_cdma_info_base_id = 
                                           ss_info_ptr->stack_info[stack].mode_info.cdma_info.base_id;
    event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].mode_info_cdma_info_base_lat = 
                                           ss_info_ptr->stack_info[stack].mode_info.cdma_info.base_lat;
    event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].mode_info_cdma_info_base_long = 
                                           ss_info_ptr->stack_info[stack].mode_info.cdma_info.base_long;
    event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].mode_info_cdma_info_srch_win_n = 
                                           ss_info_ptr->stack_info[stack].mode_info.cdma_info.srch_win_n;
    event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].mode_info_cdma_info_p_rev_in_use = 
                                           ss_info_ptr->stack_info[stack].mode_info.cdma_info.p_rev_in_use;
    event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].mm_information = 
                                           ss_info_ptr->stack_info[stack].mode_info.gw_info.mm_information;
    /* HDR info */
    event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].hdr_hybrid = 
                                           ss_info_ptr->stack_info[stack].rat_mode_info.hdr_mode_info.hdr_hybrid;
    /* Roaming info */
    event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].roam_status = 
                                           ss_info_ptr->stack_info[stack].roam_status;
    /*Sys id info */
    event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].sys_id.id = 
                                           ss_info_ptr->stack_info[stack].sys_id.id;
    event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].sys_id.id_type= 
                                           ss_info_ptr->stack_info[stack].sys_id.id_type;
    event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].sys_id_id_is95_sid = 
                                           ss_info_ptr->stack_info[stack].sys_id.id.is95.sid;
    event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].sys_id_id_is95_nid = 
                                           ss_info_ptr->stack_info[stack].sys_id.id.is95.nid;
    memscpy((void*)&(event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].sys_id_id_plmn),
            sizeof(event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].sys_id_id_plmn),
            (const void*)&(ss_info_ptr->stack_info[stack].sys_id.id.plmn), sizeof(ss_info_ptr->stack_info[stack].sys_id.id.plmn));
    /*Cell info */
    /* Copy the cell info from CM to TM data structure */
        memscpy(&(event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].cell_info), 
             sizeof(event_info_ptr->event_info.cm_event_info.ss_info.multisim_stack_info[stack].cell_info),
             &(ss_info_ptr->stack_info[stack].cell_info),
             sizeof(ss_info_ptr->stack_info[stack].cell_info));
    
    } 
          
      event_info_ptr->event_type = TM_CM_SS_EVENT;
      event_info_ptr->event_info.cm_event_type.ss_event = ss_event;
      ipc_msg_ptr->q_MsgId  = TM_CORE_CM_MSG_ID_EVENT_NTFY;
      status    = os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM);
      if (status == FALSE)
      {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_cm_ss_event_cb: Error in sending CM Event msg_id=0x%x to TM thread",ipc_msg_ptr->q_MsgId);
        status = os_IpcDelete(ipc_msg_ptr);
        ipc_msg_ptr = NULL;
      } /* End status */
      else
      {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "tm_cm_ss_event_cb: Sending CM Event msg_id=0x%x to TM thread",ipc_msg_ptr->q_MsgId);
      } /* end else status */
    }

  }
} /* End tm_cm_ss_event_cb( ) */


#ifdef FEATURE_CGPS_UMTS_CP
/*===========================================================================

FUNCTION
  tm_cm_handle_sups_event

DESCRIPTION

RETURN VALUE
  Void

===========================================================================*/

void tm_cm_handle_sups_event (
  cm_sups_event_e_type                 sups_event,
  const tm_cm_sups_info_s_type         *sups_info_ptr
  )
{
  tm_phone_event_payload_u_type *cm_evt_payload_ptr=NULL;

  switch ( sups_event )
  {
    case CM_SUPS_EVENT_LCS_LOCATION_NOTIFICATION_IND:
      cm_evt_payload_ptr = (tm_phone_event_payload_u_type *)pdsm_getbuf(sizeof(tm_phone_event_payload_u_type));
      if(cm_evt_payload_ptr != NULL)
      {
        cm_evt_payload_ptr->sups_data.u_invoke_id  = sups_info_ptr->invoke_id;
        cm_evt_payload_ptr->sups_data.u_ss_ref_num = sups_info_ptr->ss_ref;
        cm_evt_payload_ptr->sups_data.lcs_mess.location_notification_ind = sups_info_ptr->location_notification;  
        tm_core_cm_notify_event(TM_PHONE_EVENT_LCS_MTLR_NTFY_IND,cm_evt_payload_ptr);
        (void)pdsm_freebuf((char *)cm_evt_payload_ptr);
      }
      else
      {
        MSG_ERROR("No buffer available",0,0,0);
      }
      break;

    case CM_SUPS_EVENT_LCS_LOCATION_NOTIFICATION_RES:
      tm_core_cm_notify_event(TM_PHONE_EVENT_LCS_MTLR_NTFY_RES,NULL);
      break;

    case CM_SUPS_EVENT_LCS_MOLR:
      tm_core_cm_notify_event(TM_PHONE_EVENT_LCS_MOLR,NULL);
      break;

    case CM_SUPS_EVENT_LCS_MOLR_COMPLETED:
      tm_core_cm_notify_event(TM_PHONE_EVENT_LCS_MOLR_COMPLETED,NULL);
      break;
    case CM_SUPS_EVENT_LCS_MOLR_CONF:
      cm_evt_payload_ptr = (tm_phone_event_payload_u_type *)pdsm_getbuf(sizeof(tm_phone_event_payload_u_type));
      if(cm_evt_payload_ptr != NULL)
      {
        cm_evt_payload_ptr->sups_data.u_ss_ref_num = sups_info_ptr->ss_ref;
          cm_evt_payload_ptr->sups_data.lcs_mess.lcs_molr_res = sups_info_ptr->lcs_molr_res; 
        tm_core_cm_notify_event(TM_PHONE_EVENT_LCS_MOLR_CONF, cm_evt_payload_ptr);
        (void)pdsm_freebuf((char *)cm_evt_payload_ptr);
      }
      else
      {
        MSG_ERROR("No buffer available",0,0,0);
      }
      break;

    default:
      break;
  }
}

/*===========================================================================
FUNCTION tm_cm_sups_cmd_cb

DESCRIPTION

RETURN VALUE

DEPENDENCIES
  None.
===========================================================================*/
void tm_cm_sups_cmd_cb
(
  void                        *data_block_ptr,      /* Pointer to client provided data block */
  cm_sups_cmd_e_type           sups_cmd,            /* Indicate which sups command is this error status for */
  cm_sups_cmd_err_e_type       sups_cmd_err         /* SUPS command error code, indicating whether command
                                                    ** is rejected and the reason */
)
{
  MSG_LOW("TM_CM Sups: Command Callback, Command: %d",sups_cmd,0,0);
  MSG_LOW("TM_CM Sups: Error Code: %d",sups_cmd_err,0,0);
}

/*===========================================================================

FUNCTION tm_cm_sups_event_cb

DESCRIPTION

DEPENDENCIES
  tm_cm_client_init() must have been called.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/

void tm_cm_sups_event_cb
(
  cm_sups_event_e_type       sups_event,
  const cm_sups_info_s_type  *sups_info_ptr
)
{
    os_IpcMsgType           *ipc_msg_ptr = NULL;
    tm_cm_event_info_s_type *event_info_ptr;
    boolean status;

    MSG_MED("Handling SUPS event %d...",sups_event,0,0);

    if(sups_info_ptr == NULL)
    {
      MSG_ERROR("Null sups_info_ptr",0,0,0);
      return;
    }

    ipc_msg_ptr = os_IpcCreate(sizeof(tm_cm_event_info_s_type), IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_TM);
    /* Queue the commands in Client command Queue.
    */
    if (ipc_msg_ptr == NULL)
    {
      MSG_ERROR( "IPC message allocation failed",0,0,0 );
    }
    else 
    {
      event_info_ptr = (tm_cm_event_info_s_type*)ipc_msg_ptr->p_Data;

      event_info_ptr->event_info.cm_event_info.sups_info.invoke_id              = sups_info_ptr->invoke_id;
      event_info_ptr->event_info.cm_event_info.sups_info.lcs_molr_res           = sups_info_ptr->lcs_molr_res;
      event_info_ptr->event_info.cm_event_info.sups_info.location_notification  = 
        sups_info_ptr->location_notification;
      event_info_ptr->event_info.cm_event_info.sups_info.ss_ref                 = sups_info_ptr->ss_ref;

      event_info_ptr->event_type = TM_CM_SUPS_EVENT;
      event_info_ptr->event_info.cm_event_type.sups_event = sups_event;
      ipc_msg_ptr->q_MsgId  = TM_CORE_CM_MSG_ID_EVENT_NTFY;
      status    = os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM);
      if (status == FALSE)
      {
        MSG_ERROR("Error in sending CM Event msg_id=%d to TM thread",ipc_msg_ptr->q_MsgId,0,0);
        status = os_IpcDelete(ipc_msg_ptr);
        ipc_msg_ptr = NULL;
      } /* End status */
      else
      {
        MSG_HIGH("sending CM Event msg_id=%d to TM thread", ipc_msg_ptr->q_MsgId,0,0);
      } /* end else status */
    }
}

/*===========================================================================

FUNCTION tm_cm_umts_cp_init

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_umts_cp_init(void)
{
  boolean ret_val = TRUE;

  MSG_MED("Registering with CM for UMTS_CP",0,0,0);
  if(cm_client_sups_reg(tm_cm_info.cm_client,  /* pointer to the client struct */
                             tm_cm_sups_event_cb,   /* pointer to a cb function for call events */
                             CM_CLIENT_EVENT_REG,   /* event registration */
                             CM_SUPS_EVENT_LCS_LOCATION_NOTIFICATION_IND,  /* from this call event */
                             CM_SUPS_EVENT_LCS_MOLR_CONF,                  /* to this call event */
                             NULL ) != CM_CLIENT_OK) 
  {
    MSG_ERROR("cm_client_sups_reg failed",0,0,0);
    ret_val = FALSE;
  }

  /*---------------------------------
   Register for serving system info event
  *--------------------------------*/
  if(cm_mm_client_ss_reg_msim (tm_cm_info.cm_client,       // pointer to the client struct
                         tm_cm_ss_event_cb,        // pointer to a cb function for ss events
                         CM_CLIENT_EVENT_REG,
                         CM_SS_EVENT_INFO,
                         CM_SS_EVENT_INFO,
                         NULL,
                         SYS_MODEM_AS_ID_1_MASK|SYS_MODEM_AS_ID_2_MASK|SYS_MODEM_AS_ID_3_MASK
                         ) != CM_CLIENT_OK) 
  {
    MSG_ERROR("cm_mm_client_ss_reg_msim failed",0,0,0);
    ret_val = FALSE;
  }
  return ret_val;
}

/*===========================================================================

FUNCTION tm_cm_umts_handle_start_req

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_umts_handle_start_req(const tm_cm_start_info_u_type *info_ptr)
{
  boolean ret_val = FALSE;

  if(cm_sups_cmd_lcs_molr_per_subs(tm_cm_sups_cmd_cb,
                        NULL,
                        tm_cm_info.cm_client, 
                        &info_ptr->lcs_molr_cmd_params ,
                        tm_core_get_current_dd_subs_id())
    )
  {
    ret_val = TRUE;
  }
  return ret_val;
}

/*===========================================================================

FUNCTION tm_cm_umts_handle_stop_req

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_umts_handle_stop_req(const tm_cm_stop_info_s_type *info_ptr)
{
  boolean ret_val = FALSE;

  if ( info_ptr->stop_reason == TM_CM_STOP_REASON_MOLR_COMPLETE )
  {
    if(cm_sups_cmd_lcs_molr_completed_per_subs( tm_cm_sups_cmd_cb, 
                                       NULL,
                                       tm_cm_info.cm_client, 
                                       &info_ptr->stop_params.lcs_molr_completed_cmd_params,
                                       tm_core_get_current_dd_subs_id()  )
      )
    {
      ret_val = TRUE;
    }
  }

  else if ( info_ptr->stop_reason == TM_CM_STOP_REASON_LOC_NOTIF_RET_RES ) 
  {
    if ( cm_sups_cmd_lcs_location_notification_res_per_subs( tm_cm_sups_cmd_cb, 
                                                    NULL, 
                                                    tm_cm_info.cm_client, 
                                                    &info_ptr->stop_params.lcs_loc_notif_return_result_params,
                                                     tm_core_get_current_dd_subs_id())
       )
    {
      ret_val = TRUE;
    }
  } 
  return ret_val;
}

#endif /* FEATURE_CGPS_UMTS_CP */

#ifdef FEATURE_CGPS_CDMA_IF
/*===========================================================================

FUNCTION tm_cm_cdma_map_cm_dbm_status

DESCRIPTION
  This function maps the cm_dbm_mo_status_e_type to an appropriate
  tm_cm_status_type.

DEPENDENCIES
  none

RETURN VALUE
  The appropriate TM_CM status

SIDE EFFECTS
  None

===========================================================================*/

tm_cm_sent_msg_status_e_type tm_cm_cdma_map_cm_dbm_status(
  const cm_dbm_mo_status_e_type dbm_status
  )
{
  switch(dbm_status)
  {
    case CM_DBM_MO_OK:
      return(TM_CM_STATUS_OK);

   /* Return failure if status not ok */
    default:
      return(TM_CM_STATUS_FAILURE);
  }
}

/*===========================================================================

FUNCTION tm_cm_cdma_check_msg_wait_q

DESCRIPTION
  Checks to see if there are messages on the waiting queue. If there are,
  then it sends 1 message.

DEPENDENCIES
  The queue must have been initialized.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e{506} : MSG_FATAL error */
void tm_cm_cdma_check_msg_wait_q(void)
{
  tm_cm_send_msg_info_s_type*  send_msg_ptr=NULL;
  /* Used to dequeue from the msg_waiting Q */

  if(q_cnt(&tm_cm_info.msg_waiting_q) > 0)
  {
    send_msg_ptr = (tm_cm_send_msg_info_s_type*) q_get(&tm_cm_info.msg_waiting_q);

    if(NULL == send_msg_ptr)
    {
      /* This should never happen, given that q_cnt > 0 at this point */
      MSG_FATAL("Empty pending queue dereferenced.", 0, 0, 0);
      return;
    }

    if(cm_dbm_cmd_mo_msg(
                     CAI_POSITION_DET,
                     tm_cm_dbm_cmd_cb,
                     send_msg_ptr->user_data,
                     tm_cm_info.cm_client,
                     send_msg_ptr->msg.payload,
                     send_msg_ptr->msg.length,
                     NULL,
                     TM_CM_HOLD_ORIG_TIMEOUT_SECS,
                     TM_CM_L3_ACK_WAIT_TIME,
                     CM_DBM_TRANSMIT_IN_CURR_PROTOCOL_STATE
                     ) == FALSE)
    {
      MSG_ERROR("Cannot send DBM to CM",0,0,0);
      tm_cm_info.msg_ack_pending = FALSE;
    }
    else
    {
      tm_cm_info.msg_ack_pending = TRUE;
      MSG_MED("TM_CM_Iface dequeued msg (id: 0x%x) from msg waiting q",
                send_msg_ptr->user_data, 0, 0);
      tm_cm_cdma_reset_inactivity_timer();
    }

    (void)pdsm_freebuf((char *)send_msg_ptr);
  }
  else
  {
    MSG_LOW("Q Count zero",0,0,0);
  }
}

/*===========================================================================

FUNCTION tm_cm_dbm_event_cb

DESCRIPTION
  The callback function for CM position determination events:
  Sent message status or message received.

DEPENDENCIES
  tm_cm_client_init() must have been called.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e{506} : MSG_FATAL error */
void tm_cm_handle_dbm_event
(
  cm_dbm_event_e_type         dbm_event,
  const tm_cm_dbm_info_s_type    *dbm_info_ptr
)
{
  tm_phone_event_payload_u_type *cm_evt_payload_ptr=NULL;

  if(dbm_info_ptr == NULL)
  {
    MSG_ERROR("Null dbm_info_ptr",0,0,0);
    return;
  }
  switch(dbm_event)
  {
     case CM_DBM_EVENT_MO_STATUS:
          MSG_MED("TM_CM msg stat (id: 0x%x, stat: 0x%x)",
                    dbm_info_ptr->user_data, dbm_info_ptr->mo_status, 0);
          if(tm_cm_info.msg_ack_pending == FALSE)
          {
            MSG_FATAL("Received status message for message that wasn't pending!", 0, 0, 0);
            return;
          }

          MSG_MED("TM CM Iface notifying 1X CP of message status", 0, 0, 0);

          tm_cm_info.msg_ack_pending = FALSE;

          cm_evt_payload_ptr = (tm_phone_event_payload_u_type *)pdsm_getbuf(sizeof(tm_phone_event_payload_u_type));
          if(cm_evt_payload_ptr != NULL)
          {
            cm_evt_payload_ptr->sent_msg_info.msg_status = tm_cm_cdma_map_cm_dbm_status(dbm_info_ptr->mo_status);
            cm_evt_payload_ptr->sent_msg_info.user_data = dbm_info_ptr->user_data;
            tm_core_cm_notify_event(TM_PHONE_EVENT_SENT_MSG_STATUS,cm_evt_payload_ptr);

            if(cm_evt_payload_ptr->sent_msg_info.msg_status != TM_CM_STATUS_OK)
            {
              tm_cm_cdma_flush_pending_queue(&tm_cm_info.msg_waiting_q);
            }
            else
            {
              tm_cm_cdma_check_msg_wait_q();
            }
            (void)pdsm_freebuf((char *)cm_evt_payload_ptr);
          }
          else
          {
            MSG_ERROR("No buffer available",0,0,0);
          }
          break;

     case CM_DBM_EVENT_MT_MSG:
          MSG_MED("TM_CM received message %i bytes. Passing to TM-Core",
                  dbm_info_ptr->data_payload.length, 0, 0);

          tm_cm_cdma_reset_inactivity_timer();

          cm_evt_payload_ptr = (tm_phone_event_payload_u_type *)pdsm_getbuf(sizeof(tm_phone_event_payload_u_type));
          if(cm_evt_payload_ptr != NULL)
          {
            cm_evt_payload_ptr->dbm_msg.length   = dbm_info_ptr->data_payload.length;
            // length is already checked for buffer overflow in CM callback, memcpy is safe here.
            memscpy((void *)cm_evt_payload_ptr->dbm_msg.payload, sizeof(cm_evt_payload_ptr->dbm_msg.payload),
                    (const void*)dbm_info_ptr->data_payload.payload,
                    dbm_info_ptr->data_payload.length);
            tm_core_cm_notify_event(TM_PHONE_EVENT_MT_MSG,cm_evt_payload_ptr);
            (void)pdsm_freebuf((char *)cm_evt_payload_ptr);
          }
          else
          {
            MSG_ERROR("No buffer available",0,0,0);
          }
          break;

    default:
      break;
  }
}

/*===========================================================================

FUNCTION tm_cm_dbm_event_cb

DESCRIPTION

DEPENDENCIES
  tm_cm_client_init() must have been called.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/

void tm_cm_dbm_event_cb
(
  cm_dbm_event_e_type         dbm_event,
  const cm_dbm_info_s_type    *dbm_info_ptr
)
{
  os_IpcMsgType           *ipc_msg_ptr = NULL;
  tm_cm_event_info_s_type *event_info_ptr;
  boolean status;

  MSG_MED("Handling DBM event %d...",dbm_event,0,0);

  if(dbm_info_ptr == NULL)
  {
    MSG_ERROR("Null dbm_info_ptr",0,0,0);
    return;
  }

  // If message is > max bytes, do not go to the trouble of sending IPC, return from here.
  if ((CM_DBM_EVENT_MT_MSG == dbm_event) && 
     (dbm_info_ptr->num_bytes > TM_MAX_BUFFSIZE)) {
       MSG_FATAL("Received DBM larger than 200 bytes, event: CM_DBM_EVENT_MT_MSG",0,0,0);
       return;
  }

  if (dbm_info_ptr->burst_type == CAI_POSITION_DET)
  {
    ipc_msg_ptr = os_IpcCreate(sizeof(tm_cm_event_info_s_type), IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_TM);
    /* Queue the commands in Client command Queue.
    */
    if (ipc_msg_ptr == NULL)
    {
      MSG_ERROR( "IPC message allocation failed",0,0,0 );
    }
    else 
    {
      event_info_ptr = (tm_cm_event_info_s_type*)ipc_msg_ptr->p_Data;

      event_info_ptr->event_info.cm_event_info.dbm_info.mo_status = dbm_info_ptr->mo_status;
      event_info_ptr->event_info.cm_event_info.dbm_info.user_data = dbm_info_ptr->user_data;

      if (CM_DBM_EVENT_MT_MSG == dbm_event) 
      {
        // safe up-cast from uint8 to uint16
        event_info_ptr->event_info.cm_event_info.dbm_info.data_payload.length =
          dbm_info_ptr->num_bytes;
        // no buffer overflow, TM_MAX_BUFFSIZE check above prevents it.
        memscpy((void*) event_info_ptr->event_info.cm_event_info.dbm_info.data_payload.payload,
           sizeof(event_info_ptr->event_info.cm_event_info.dbm_info.data_payload.payload),
           (const void *)dbm_info_ptr->payload, dbm_info_ptr->num_bytes);
      }

      event_info_ptr->event_type = TM_CM_DBM_EVENT;
      event_info_ptr->event_info.cm_event_type.dbm_event = dbm_event;
      ipc_msg_ptr->q_MsgId  = TM_CORE_CM_MSG_ID_EVENT_NTFY;
      status    = os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM);
      if (status == FALSE)
      {
        MSG_ERROR("Error in sending CM Event msg_id=%d to TM thread",ipc_msg_ptr->q_MsgId,0,0);
        status = os_IpcDelete(ipc_msg_ptr);
        ipc_msg_ptr = NULL;
      } /* End status */
      else
      {
        MSG_HIGH("sending CM Event msg_id=%d to TM thread", ipc_msg_ptr->q_MsgId,0,0);
      } /* end else status */
    }
  }
  else
  {
    MSG_ERROR( "Incorrect DB type",0,0,0 );
  }
}

/*===========================================================================

FUNCTION tm_cm_handle_dbm_cmd_cb

DESCRIPTION
  This function is the callback for CM DBM command errors.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

void tm_cm_handle_dbm_cmd_cb
(
  void                       *data_block_ptr,
    /* Pointer to client provided data block */
  cm_dbm_cmd_e_type           dbm_cmd,
      /* Indicate which dbm command is this error status for */
  cm_dbm_cmd_err_e_type       dbm_cmd_err
      /* Reason for rejection */
)
{
    tm_phone_event_payload_u_type cm_evt_payload;

    (void)dbm_cmd;
    (void)dbm_cmd_err;

    if(data_block_ptr == NULL)
    {
      MSG_ERROR("Null data_block_ptr",0,0,0);
      return;
    }

    MSG_MED("TM CM Iface handling dbm_cmd_err", 0, 0, 0);

    if(tm_cm_info.msg_ack_pending == TRUE)
    {
      tm_cm_info.msg_ack_pending = FALSE;

      cm_evt_payload.sent_msg_info.msg_status = TM_CM_STATUS_FAILURE;
      cm_evt_payload.sent_msg_info.user_data = data_block_ptr;
      tm_core_cm_notify_event(TM_PHONE_EVENT_SENT_MSG_STATUS,&cm_evt_payload);
      tm_cm_cdma_flush_pending_queue(&tm_cm_info.msg_waiting_q);
    }
}

/*===========================================================================

FUNCTION tm_cm_dbm_cmd_cb

DESCRIPTION
  This function is the callback for CM DBM command errors.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

void tm_cm_dbm_cmd_cb
(
  void                       *data_block_ptr,
    /* Pointer to client provided data block */
  cm_dbm_cmd_e_type           dbm_cmd,
      /* Indicate which dbm command is this error status for */
  cm_dbm_cmd_err_e_type       dbm_cmd_err
      /* Reason for rejection */
)
{
    boolean                     status;
    os_IpcMsgType               *ipc_msg_ptr = NULL;
    tm_cm_cmd_info_s_type       *cmd_info_ptr;

    MSG_HIGH("call_cmd_cb for %d rcv'ed error %d", dbm_cmd,dbm_cmd_err,0);

    if(data_block_ptr == NULL)
    {
      MSG_ERROR("Null data_block_ptr",0,0,0);
      return;
    }

    if((dbm_cmd_err != CM_DBM_CMD_ERR_NOERR) && (dbm_cmd == CM_DBM_CMD_SEND_MSG))
    {
      ipc_msg_ptr = os_IpcCreate(sizeof(tm_cm_cmd_info_s_type), IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_TM);
      /* Queue the commands in Client command Queue.
      */
      if (ipc_msg_ptr == NULL)
      {
        MSG_ERROR( "IPC message allocation failed",0,0,0 );
      }
      else 
      {
        cmd_info_ptr = (tm_cm_cmd_info_s_type*)ipc_msg_ptr->p_Data;

        cmd_info_ptr->cmd_type = TM_CM_DBM_CMD;
        cmd_info_ptr->cmd_info.cm_cmd_type.dbm_cmd = dbm_cmd;
        cmd_info_ptr->cmd_info.data_block_ptr = data_block_ptr;
        cmd_info_ptr->cmd_info.cm_cmd_err.dbm_cmd_err = dbm_cmd_err;
        ipc_msg_ptr->q_MsgId  = TM_CORE_CM_MSG_ID_CMD_NTFY;
        status    = os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM);
        if (status == FALSE)
        {
          MSG_ERROR("Error in sending CM Event msg_id=%d to TM thread",ipc_msg_ptr->q_MsgId,0,0);
          status = os_IpcDelete(ipc_msg_ptr);
          ipc_msg_ptr = NULL;
        } /* End status */
        else
        {
          MSG_HIGH("sending CM Event msg_id=%d to TM thread", ipc_msg_ptr->q_MsgId,0,0);
        } /* end else status */
      }
    }
}

/*===========================================================================

FUNCTION tm_cm_handle_call_cmd_cb

DESCRIPTION
  This is the callback for CM call commands. It reports the status of a CM
  call command. Do not confuse this with the call event cb, which reports
  call events.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void tm_cm_handle_call_cmd_cb(
  void                      *data_block_ptr,
    /* Pointer to client provided data block. */

  cm_call_cmd_e_type        call_cmd,
    /* Indicate which command is this error status for */

  cm_call_cmd_err_e_type    call_cmd_err
    /* Call command error code, indicating whether command
    ** is rejected and the reason */
  )
{
  MSG_MED("Handle call_cmd_cb for cmd %d error %d",call_cmd, call_cmd_err,0);
  if(call_cmd == CM_CALL_CMD_ANSWER)
  {
    /* We should end the call */
    MSG_MED("TM CM Iface Sending call end command ",0, 0, 0);
    tm_cm_cdma_call_end();
  }
  else if(call_cmd == CM_CALL_CMD_END)
  {
    MSG_ERROR("TM CM Iface cannot end call!", 0, 0, 0);
  }
}

/*===========================================================================

FUNCTION tm_cm_call_cmd_cb

DESCRIPTION
  This is the callback for CM call commands. It reports the status of a CM
  call command. Do not confuse this with the call event cb, which reports
  call events.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void tm_cm_call_cmd_cb(
  void                      *data_block_ptr,
    /* Pointer to client provided data block. */

  cm_call_cmd_e_type        call_cmd,
    /* Indicate which command is this error status for */

  cm_call_cmd_err_e_type    call_cmd_err
    /* Call command error code, indicating whether command
    ** is rejected and the reason */
  )
{
  boolean                     status;
  os_IpcMsgType               *ipc_msg_ptr = NULL;
  tm_cm_cmd_info_s_type       *cmd_info_ptr;

  MSG_HIGH("call_cmd_cb for %d rcv'ed error %d", call_cmd,call_cmd_err,0);

  if(call_cmd_err != CM_CALL_CMD_ERR_NOERR)
  {
    ipc_msg_ptr = os_IpcCreate(sizeof(tm_cm_cmd_info_s_type), IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_TM);
    /* Queue the commands in Client command Queue.
    */
    if (ipc_msg_ptr == NULL)
    {
      MSG_ERROR( "IPC message allocation failed",0,0,0 );
    }
    else 
    {
      cmd_info_ptr = (tm_cm_cmd_info_s_type*)ipc_msg_ptr->p_Data;

      cmd_info_ptr->cmd_type = TM_CM_CALL_CMD;
      cmd_info_ptr->cmd_info.cm_cmd_type.call_cmd = call_cmd;
      cmd_info_ptr->cmd_info.data_block_ptr = data_block_ptr;
      cmd_info_ptr->cmd_info.cm_cmd_err.call_cmd_err = call_cmd_err;
      ipc_msg_ptr->q_MsgId  = TM_CORE_CM_MSG_ID_CMD_NTFY;
      status    = os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM);
      if (status == FALSE)
      {
        MSG_ERROR("Error in sending CM Event msg_id=%d to TM thread",ipc_msg_ptr->q_MsgId,0,0);
        status = os_IpcDelete(ipc_msg_ptr);
        ipc_msg_ptr = NULL;
      } /* End status */
      else
      {
        MSG_HIGH("sending CM Event msg_id=%d to TM thread", ipc_msg_ptr->q_MsgId,0,0);
      } /* end else status */
    }
  }
}
#endif /* FEATURE_CGPS_CDMA_IF */

/*===========================================================================

FUNCTION  tm_cm_iface_event_proc

DESCRIPTION 
  Handles an incoming CM event

RETURN VALUE 
  None

DEPENDENCIES

===========================================================================*/

void tm_cm_iface_event_proc( void* p_data )
{
  tm_cm_event_info_s_type   *info_ptr;
  
  if (p_data == NULL)
  
  {
    MSG_ERROR("TM_CM_IFace: Null pointer to function ", 0,0,0); 
    return;
  }
  info_ptr = (tm_cm_event_info_s_type*)p_data;

  MSG_MED( "TM_CM_IFace: tm_cm_iface_event_proc() getting called",0,0,0);

  switch( info_ptr->event_type)
  {
  case TM_CM_CALL_EVENT:
      tm_cm_handle_call_event(info_ptr->event_info.cm_event_type.call_event, &(info_ptr->event_info.cm_event_info.call_info));
      break;

  case TM_CM_PH_EVENT:
      tm_cm_handle_ph_event(info_ptr->event_info.cm_event_type.ph_event, &(info_ptr->event_info.cm_event_info.ph_info));
      break;

  case TM_CM_PH_SUB_PREF_EVENT:
      tm_cm_handle_ph_sub_pref_event(info_ptr->event_info.cm_event_type.subs_event, &(info_ptr->event_info.cm_event_info.ph_subs_info));
      break;

    case TM_CM_SS_EVENT:
      tm_cm_handle_ss_event(info_ptr->event_info.cm_event_type.ss_event, &(info_ptr->event_info.cm_event_info.ss_info));
      break;

#ifdef FEATURE_CGPS_CDMA_IF
  case TM_CM_DBM_EVENT:
      tm_cm_handle_dbm_event(info_ptr->event_info.cm_event_type.dbm_event, &(info_ptr->event_info.cm_event_info.dbm_info));
      break;
#endif /* FEATURE_CGPS_CDMA_IF */

#ifdef FEATURE_CGPS_UMTS_CP
    case TM_CM_SUPS_EVENT:
      tm_cm_handle_sups_event(info_ptr->event_info.cm_event_type.sups_event, &(info_ptr->event_info.cm_event_info.sups_info));
      break;
#endif /* FEATURE_CGPS_UMTS_CP */

    default:
      /* There is nothing we can do with this event
      */
      MSG_ERROR(" TM_CM_IFace: Event not handled ",info_ptr->event_type,0,0);
      break;
  } /* End Switch statement */
}

/*===========================================================================

FUNCTION  tm_cm_iface_cmd_proc

DESCRIPTION 
  Handles an incoming CM cmd

RETURN VALUE 
  None

DEPENDENCIES

===========================================================================*/

void tm_cm_iface_cmd_proc( void* p_data )
{
  tm_cm_cmd_info_s_type   *info_ptr;
  
  if (p_data == NULL)
  
  {
    MSG_ERROR("TM_CM_IFace: Null pointer to function ", 0,0,0); 
    return;
  }
  info_ptr = (tm_cm_cmd_info_s_type*)p_data;

  MSG_MED( "TM_CM_IFace: tm_cm_iface_cmd_proc() getting called",0,0,0);

  switch( info_ptr->cmd_type)
  {
#ifdef FEATURE_CGPS_CDMA_IF
  case TM_CM_CALL_CMD:
      tm_cm_handle_call_cmd_cb(info_ptr->cmd_info.data_block_ptr,info_ptr->cmd_info.cm_cmd_type.call_cmd, info_ptr->cmd_info.cm_cmd_err.call_cmd_err);
      break;

  case TM_CM_DBM_CMD:
      tm_cm_handle_dbm_cmd_cb(info_ptr->cmd_info.data_block_ptr,info_ptr->cmd_info.cm_cmd_type.dbm_cmd, info_ptr->cmd_info.cm_cmd_err.dbm_cmd_err);
      break;
#endif /* FEATURE_CGPS_CDMA_IF */

    default:
      /* There is nothing we can do with this cmd
      */
      MSG_HIGH(" TM_CM_IFace: Cmd not handled ",info_ptr->cmd_type,0,0);
      break;
  } /* End Switch statement */
}

#ifdef FEATURE_CGPS_CDMA_IF
/*===========================================================================
FUNCTION tm_cm_ph_cmd_cb

DESCRIPTION

RETURN VALUE

DEPENDENCIES
  None.
===========================================================================*/
void tm_cm_ph_cmd_cb
(
  void                         *data_ptr,         /* Data block pointer    */
  cm_ph_cmd_e_type              cmd,              /* Command ID            */
  cm_ph_cmd_err_e_type          cmd_err           /* Command error code    */
)
{
  (void)data_ptr;

  /* Filter supported commands */
  switch (cmd)
  {
  case CM_PH_CMD_WAKEUP_FROM_STANDBY:
    /* Verify there is no error condition */
    if (CM_PH_CMD_ERR_NOERR != cmd_err)
    {
      MSG_ERROR("CM cannot process ph  command",0,0,0);
    }
    else
    {
      MSG_MED("CM processes ph  command",0,0,0);
    }
    break;

  default:
    /* Do nothing for unsupported commands */
    break;
  }
} /* tm_cm_ph_cmd_cb_func */

/*===========================================================================

FUNCTION tm_cm_cdma_call_orig_in_progress

DESCRIPTION


DEPENDENCIES

RETURN VALUE
  TRUE: Call is in origination
  FALSE: Either data or non-data or PD calls are connected.

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_cdma_call_orig_in_progress(void)
{
  uint8 i;

  for(i=0; i < CM_CALL_ID_MAX; i++)
  {
    if(tm_cm_info.phone_info.call[i].call_state == TM_CM_CALL_STATE_WAIT_CONNECT)
    {
      return TRUE;
    }
  }
  return FALSE;
}

/*===========================================================================

FUNCTION tm_cm_cdma_call_connected

DESCRIPTION


DEPENDENCIES

RETURN VALUE
  TRUE: One call is connected
  FALSE: No calls are connected.

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_cdma_call_connected(void)
{
  uint8 i;
  for(i=0; i < CM_CALL_ID_MAX; i++)
  {
    if(tm_cm_info.phone_info.call[i].call_state == TM_CM_CALL_STATE_CONNECTED)
    {
#ifdef FEATURE_HDR
      if(TM_CM_ISDATA_HDR(tm_cm_info.phone_info.call[i].srv_opt))
      {
        MSG_MED("HDR data call is up, continue searching for a voice call",0,0,0);
        continue;
      }
      else
#endif
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}

/*===========================================================================

FUNCTION tm_cm_cdma_all_calls_idle

DESCRIPTION
  This function checks if all calls are idle or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If all the calls are in idle state
  FALSE: If any one call is in WAIT_CONNECT or CONNECTED state.

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_cdma_all_calls_idle(void)
{
  uint8 i;

  for(i=0; i < CM_CALL_ID_MAX; i++)
  {
    if(tm_cm_info.phone_info.call[i].call_state != TM_CM_CALL_STATE_IDLE)
    {
      return FALSE;
    }
  }

  return TRUE;
}

/*===========================================================================

FUNCTION tm_cm_iface_is_pd_call_connected(void)

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If PD call connected
  FALSE: Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_iface_is_pd_call_connected(void)
{
  uint8 i;
  for(i=0; i < CM_CALL_ID_MAX; i++)
  {
    if((tm_cm_info.phone_info.call[i].call_type == CM_CALL_TYPE_PD) &&
       (tm_cm_info.phone_info.call[i].call_state == TM_CM_CALL_STATE_CONNECTED))
    {
        return TRUE;
    }
  }
  return FALSE;
}

/*===========================================================================

FUNCTION tm_cm_cdma_get_phone_state

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Any of the status_e enums.

SIDE EFFECTS
  None

===========================================================================*/
tm_cm_phone_state_e_type tm_cm_cdma_get_phone_state(void)
{
  if(tm_cm_cdma_all_calls_idle())
  {
    /* Phone is idle */
    tm_cm_info.phone_state = TM_CM_PHONE_STATE_IDLE;
  }
  else if (tm_cm_iface_is_pd_call_connected())
  {
    tm_cm_info.phone_state = TM_CM_PHONE_STATE_CONNECTED;
  }
  else if(tm_cm_cdma_call_connected())   
  {
    tm_cm_info.phone_state = TM_CM_PHONE_STATE_CONNECTED;
  }
  else
  {
    tm_cm_info.phone_state = TM_CM_PHONE_STATE_WAIT_CONNECT;
  }

  MSG_HIGH("returning phone state %d",tm_cm_info.phone_state,0,0);
  return tm_cm_info.phone_state;
}

/*===========================================================================

FUNCTION tm_cm_1x_cp_init

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_1x_cp_init(const tm_cm_pd_resp_is_within_t2m_f_type pd_resp_cb_f)
{
  uint8 i;
  boolean ret_val = TRUE;
  MSG_MED("Registering with CM for 1X_CP",0,0,0);

  tm_cm_info.cdma_mt_session_active = FALSE;
  /* Can be deferred */  
  tm_cm_info.inactivity_timer = os_TimerCreate((uint32)TM_CM_IFACE_INACTIIVITY_TIMER_ID,(uint32)THREAD_ID_SM_TM);

  (void)q_init(&tm_cm_info.msg_waiting_q);
  tm_cm_info.msg_ack_pending = FALSE;

  tm_cm_info.phone_state = TM_CM_PHONE_STATE_IDLE;
  for(i=0; i<CM_CALL_ID_MAX;i++)
  {
    tm_cm_info.phone_info.call[i].call_type = CM_CALL_TYPE_NONE;
  }

  tm_cm_info.pd_call_id = 0;
  tm_cm_info.p_rev_in_use = 0;

  /* Read the Call Related parameter */
  (void)sm_nv_read( NV_GPS1_CALL_RELATED_I, (void*) &tm_cm_iface_nv_read_buffer );
  tm_cm_info.call_related = tm_cm_iface_nv_read_buffer.gps1_call_related;
  MSG_MED("gps1_call_related = %d.", tm_cm_info.call_related, 0, 0);

  /* Configurable by bit 1(0x2) of NV_GPS1_CALL_RELATED_I(NV 1944: byte), default is 0 */
  tm_cm_info.cdma_mt_end_call_immed = FALSE;  
  if (tm_cm_iface_nv_read_buffer.gps1_call_related & NV_GPS1_CALL_RELATED_MASK_2_END_MT_CALL)
  {
    tm_cm_info.cdma_mt_end_call_immed = TRUE;
  }

  if(cm_mm_client_call_reg( tm_cm_info.cm_client,
                      tm_cm_call_event_cb,
                      CM_CLIENT_EVENT_REG,
                      CM_CALL_EVENT_INCOM,     /* from */
                      CM_CALL_EVENT_INCOM,     /* to   */
                      NULL  /* not interested in other clients' cmd errors */
                    ) != CM_CLIENT_OK) 
  {
    MSG_ERROR("cm_mm_client_call_reg failed",0,0,0);
    ret_val = FALSE;
  }

  if(cm_mm_client_call_reg( tm_cm_info.cm_client,
                      tm_cm_call_event_cb,
                      CM_CLIENT_EVENT_REG,
                      CM_CALL_EVENT_CONNECT,     /* from */
                      CM_CALL_EVENT_CONNECT,     /* to   */
                      NULL  /* not interested in other clients' cmd errors */
                    ) != CM_CLIENT_OK) 
  {
    MSG_ERROR("cm_mm_client_call_reg failed",0,0,0);
    ret_val = FALSE;
  }

  if(cm_mm_client_call_reg( tm_cm_info.cm_client,
                      tm_cm_call_event_cb,
                      CM_CLIENT_EVENT_REG,
                      CM_CALL_EVENT_SRV_OPT,     /* from */
                      CM_CALL_EVENT_SRV_OPT,     /* to   */
                      NULL  /* not interested in other clients' cmd errors */
                    ) != CM_CLIENT_OK) 
  {
    MSG_ERROR("cm_mm_client_call_reg failed",0,0,0);
    ret_val = FALSE;
  }

  if(cm_client_dbm_event_reg(
                     tm_cm_info.cm_client,
                     tm_cm_dbm_event_cb,
                     CM_CLIENT_EVENT_REG,
                     CM_DBM_EVENT_MO_STATUS,
                     CM_DBM_EVENT_MT_MSG,
                     NULL    /* not interested in other clients' cmd errors */
                   ) != CM_CLIENT_OK) 
  {
    MSG_ERROR("cm_client_dbm_event_reg failed",0,0,0);
    ret_val = FALSE;
  }


  if(cm_client_dbm_class_reg(
                     tm_cm_info.cm_client,
                     CM_CLIENT_EVENT_REG,
                     CM_DBM_CLASS_PD,
                     CM_DBM_CLASS_PD
                   ) != CM_CLIENT_OK) 
  {
    MSG_ERROR("cm_client_dbm_class_reg failed",0,0,0);
    ret_val = FALSE;
  }


  /* Register for position determination request checking function */
  if(cm_pd_reg( pd_resp_cb_f ) != CM_PD_OK)
  {
    MSG_ERROR("cm_pd_reg failed",0,0,0);
    ret_val = FALSE;
  }
  return ret_val;
}

/*===========================================================================

FUNCTION tm_cm_1x_cp_handle_start_req

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_1x_cp_handle_start_req(void)
{
  if(tm_cm_info.cdma_mt_session_active == TRUE)
  {
    MSG_ERROR("MT session already active",0,0,0);
    return FALSE;
  }
  tm_cm_info.phone_state = tm_cm_cdma_get_phone_state();

  if(tm_cm_info.phone_state == TM_CM_PHONE_STATE_CONNECTED)
  {
    /* Phone is not idle and at least 1 call is connected.
    */
    if(tm_cm_iface_is_pd_call_connected())
    {
      /* inactivity_timer is not started here as its started when CALL_CONNECT for 
         PD call is handled.
      */
      MSG_MED("Got Start Req when PD call is connected", 0, 0, 0);
    }
    else
    {
      MSG_HIGH("TC is up due to some call",0,0,0);
      tm_cm_cdma_set_inactivity_timer();
    }
    tm_core_cm_notify_event(TM_PHONE_EVENT_TC_CONNECTED, NULL);
    tm_cm_info.cdma_mt_session_active = TRUE;
  }
  else
  {
    MSG_ERROR("Error! Got Start Req in phone state %d",tm_cm_info.phone_state,0,0);
    tm_cm_cdma_clean_up();
  }
  return TRUE;
}

/*===========================================================================

FUNCTION tm_cm_1x_cp_handle_stop_req

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_1x_cp_handle_stop_req(void)
{
  if(tm_cm_info.cdma_mt_session_active == TRUE)
  {
    if(tm_cm_iface_is_pd_call_connected())
    {
      MSG_MED("TM_CM ending PD call", 0, 0, 0);
      tm_cm_cdma_call_end();
    }
    tm_cm_cdma_clean_up();
  }
  else
  {
    // 1X CP MT session is not active (GPS Lock, etc reason) but PD call is connected
    if(tm_cm_info.cdma_mt_end_call_immed == TRUE && tm_cm_iface_is_pd_call_connected())
    {
      MSG_MED("TM_CM ending PD call immediately", 0, 0, 0);
      tm_cm_cdma_call_end();
    }
  }

  return TRUE;
}

/*===========================================================================

FUNCTION tm_cm_1x_cp_send_data

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_1x_cp_send_data(byte* data_ptr, uint8 length)
{
    boolean ret_val = TRUE;

    tm_cm_send_msg_info_s_type *send_msg_info_ptr;

    if((send_msg_info_ptr = (tm_cm_send_msg_info_s_type *)pdsm_getbuf(sizeof(tm_cm_send_msg_info_s_type))) != NULL)
    {
      send_msg_info_ptr->user_data = send_msg_info_ptr;
      send_msg_info_ptr->msg.length = memscpy((void *)send_msg_info_ptr->msg.payload, sizeof(send_msg_info_ptr->msg.payload), (const void *)data_ptr,length);

      if(tm_cm_info.msg_ack_pending)
      {
        MSG_MED("TM_CM_Iface putting msg id 0x%x on pending q. Length %d",send_msg_info_ptr->user_data, send_msg_info_ptr->msg.length, 0);
        (void)q_link( send_msg_info_ptr, &(send_msg_info_ptr->link) );
        q_put(&(tm_cm_info.msg_waiting_q), &(send_msg_info_ptr->link));
      }
      else
      {
        if(cm_dbm_cmd_mo_msg(
                     CAI_POSITION_DET,
                     tm_cm_dbm_cmd_cb,
                     send_msg_info_ptr->user_data,
                     tm_cm_info.cm_client,
                     send_msg_info_ptr->msg.payload,
                     send_msg_info_ptr->msg.length,
                     NULL,
                     TM_CM_HOLD_ORIG_TIMEOUT_SECS,
                     TM_CM_L3_ACK_WAIT_TIME,
                     CM_DBM_TRANSMIT_IN_CURR_PROTOCOL_STATE
                     ) == FALSE)
        {
          MSG_ERROR("Cannot send DBM to CM",0,0,0);
          tm_cm_info.msg_ack_pending = FALSE;
          ret_val = FALSE;
        }
        else
        {
          tm_cm_info.msg_ack_pending = TRUE;
          MSG_MED("TM_CM_Iface dequeued msg (id: 0x%x) of length %d",send_msg_info_ptr->user_data, send_msg_info_ptr->msg.length, 0);
          tm_cm_cdma_reset_inactivity_timer();
        }
        (void)pdsm_freebuf((char *)send_msg_info_ptr);
      }
    }
    else
    {
      MSG_ERROR("No buffer for send_msg_info",0,0,0);
      ret_val = FALSE;
    }
    return ret_val;
}

/*===========================================================================

FUNCTION tm_cm_iface_hdr_call_connected

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_iface_hdr_call_connected(void)
{
  uint8 i;
  for(i=0; i < CM_CALL_ID_MAX; i++)
  {
    if(tm_cm_info.phone_info.call[i].call_state == TM_CM_CALL_STATE_CONNECTED)
    {
#ifdef FEATURE_HDR
        if(TM_CM_ISDATA_HDR(tm_cm_info.phone_info.call[i].srv_opt))
        {
          return TRUE;
        }
        else
#endif
        {
          return FALSE;
        }
    }
  }
  return FALSE;
}
#endif /* FEATURE_CGPS_CDMA_IF */

/*===========================================================================

FUNCTION tm_cm_map_sys_mode

DESCRIPTION
  This function maps a sys_sys_mode_e_type variable to a cgps_SrvSystemType
  variable
 
PARAMETERS
  sys_mode - The sys_sys_mode_e_type variable to be mapped

DEPENDENCIES
  None.

RETURN VALUE
  cgps_SrvSystemType

SIDE EFFECTS
  None

===========================================================================*/
cgps_SrvSystemType tm_cm_map_sys_mode(sys_sys_mode_e_type sys_mode)
{
  switch(sys_mode)
  { 
    case SYS_SYS_MODE_CDMA:
      return CGPS_SRV_SYS_CDMA;

    case SYS_SYS_MODE_HDR:
      return CGPS_SRV_SYS_HDR;

    case SYS_SYS_MODE_GW:
      return CGPS_SRV_SYS_GW;

    case SYS_SYS_MODE_GSM:
      return CGPS_SRV_SYS_GSM;

    case SYS_SYS_MODE_WCDMA:
      return CGPS_SRV_SYS_WCDMA;
      
    case SYS_SYS_MODE_LTE:
      return CGPS_SRV_SYS_LTE;
      
    case SYS_SYS_MODE_TDS:
      return CGPS_SRV_SYS_TDSCDMA;

    case SYS_SYS_MODE_NO_SRV:
      return CGPS_SRV_SYS_NONE;

    default:
      return CGPS_SRV_SYS_OTHER;
  }
}


/*===========================================================================

FUNCTION tm_cm_map_external_RAT

DESCRIPTION
  This function maps a pdsm_active_RAT_e_type variable to a cgps_SrvSystemType
  variable
 
PARAMETERS
  activeRat - The pdsm_active_RAT_e_type variable to be mapped

DEPENDENCIES
  None.

RETURN VALUE
  sys_sys_mode_e_type

SIDE EFFECTS
  None

===========================================================================*/
sys_sys_mode_e_type tm_cm_map_external_RAT(pdsm_active_RAT_e_type activeRat)
{
  switch(activeRat)
  { 
    case PDSM_ACTIVERAT_CDMA:
      return SYS_SYS_MODE_CDMA;
    
    case PDSM_ACTIVERAT_GSM:
      return SYS_SYS_MODE_GSM;

    case PDSM_ACTIVERAT_WCDMA:
      return SYS_SYS_MODE_WCDMA;
      
    case PDSM_ACTIVERAT_LTE:
      return SYS_SYS_MODE_LTE;
      
    case PDSM_ACTIVERAT_TDSCDMA:
      return SYS_SYS_MODE_TDS;

    case PDSM_ACTIVERAT_NONE:
      return SYS_SYS_MODE_NO_SRV;

    default:
      return SYS_SYS_MODE_NO_SRV;
  }
}

/*===========================================================================

FUNCTION tm_cm_iface_get_phone_state

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tm_cm_iface_get_phone_state(tm_cm_phone_state_info_s_type *phone_state_info_ptr)
{ 
  if((tm_cm_info.is_init == FALSE) || (phone_state_info_ptr == NULL))
  {
    MSG_ERROR("TM CM Iface not initialized",0,0,0);
    return;
  }

  phone_state_info_ptr->e911_state = tm_cm_info.e911_state;
  phone_state_info_ptr->srv_system = tm_cm_map_sys_mode(tm_cm_info.sys_mode);
  phone_state_info_ptr->srv_status = tm_cm_info.srv_status;
  
  MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_MED,
             "Returning e911_state = %d, sys_mode = %d, srv_system = %d, srv_status = %d",
                phone_state_info_ptr->e911_state, tm_cm_info.sys_mode, phone_state_info_ptr->srv_system, phone_state_info_ptr->srv_status);
}

/*===========================================================================

FUNCTION tm_cm_iface_start_req

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_iface_start_req(tm_prtl_type prtl_type, const tm_cm_start_info_u_type *info_ptr)
{
    boolean ret_val = FALSE;
    if((tm_cm_info.is_init == FALSE) || (prtl_type == TM_PRTL_TYPE_NONE))
    {
      MSG_ERROR("TM CM Iface not initialized or incorrect prtl_type",0,0,0);
      return FALSE;
    }

    if(prtl_type == TM_PRTL_TYPE_1X_CP)
    {
#ifdef FEATURE_CGPS_CDMA_IF
      ret_val = tm_cm_1x_cp_handle_start_req();
#endif /* FEATURE_CGPS_CDMA_IF */
    }
    else if(prtl_type == TM_PRTL_TYPE_UMTS_CP)
    {
#ifdef FEATURE_CGPS_UMTS_CP
      ret_val = tm_cm_umts_handle_start_req(info_ptr);
#endif /* FEATURE_CGPS_UMTS_CP */
    }
    else
    {
      MSG_ERROR("Incorrect prtl_type",0,0,0);
      ret_val = FALSE;
    }
    return ret_val;
}

/*===========================================================================

FUNCTION tm_cm_iface_stop_req

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_iface_stop_req(tm_prtl_type prtl_type, tm_cm_stop_info_s_type *info_ptr)
{
  boolean ret_val = FALSE; 
  if((tm_cm_info.is_init == FALSE) || (prtl_type == TM_PRTL_TYPE_NONE))
  {
    MSG_ERROR("TM CM Iface not initialized or in",0,0,0);
    return FALSE;
  }

  if(prtl_type == TM_PRTL_TYPE_1X_CP)
  {
#ifdef FEATURE_CGPS_CDMA_IF
    ret_val = tm_cm_1x_cp_handle_stop_req();
#endif /* FEATURE_CGPS_CDMA_IF */
  }
  else if(prtl_type == TM_PRTL_TYPE_UMTS_CP)
  {
#ifdef FEATURE_CGPS_UMTS_CP
    ret_val = tm_cm_umts_handle_stop_req(info_ptr);
#endif /* FEATURE_CGPS_UMTS_CP */
  }
  else
  {
    MSG_ERROR("Incorrect prtl_type",0,0,0);
    ret_val = FALSE;
  }
  return ret_val;
}

/*===========================================================================

FUNCTION tm_cm_iface_send_data

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_iface_send_data(tm_prtl_type prtl_type, byte* data_ptr, uint8 length)
{
  boolean ret_val = TRUE;

  if((tm_cm_info.is_init == FALSE) || (data_ptr == NULL) || (prtl_type == TM_PRTL_TYPE_NONE) || (length > TM_MAX_BUFFSIZE))
  {
    MSG_ERROR("TM CM Iface not initialized or Null data_ptr",0,0,0);
    return FALSE;
  }

  if(prtl_type == TM_PRTL_TYPE_1X_CP)
  {
#ifdef FEATURE_CGPS_CDMA_IF
    ret_val = tm_cm_1x_cp_send_data(data_ptr,length);
#endif /* FEATURE_CGPS_CDMA_IF */
  }
  else
  {
    MSG_ERROR("Incorrect prtl_type",0,0,0);
    ret_val = FALSE;
  }
  return ret_val;
}

#ifdef FEATURE_CGPS_CDMA_IF
/*===========================================================================

FUNCTION tm_cm_iface_handle_reg_events_req

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_iface_handle_reg_events_req(tm_cm_register_events_bmap_e_type bmap)
{
  boolean ret_val = TRUE;
  MSG_HIGH("Handling register events request 0x%x",bmap,0,0);

  if(tm_cm_info.is_init == FALSE)
  {
    MSG_ERROR("TM CM Iface not initialized",0,0,0);
    return FALSE;
  }

  /* Reg/Dereg only if TC_DOWN bit changed
  ** (new X-OR old) & TC_DOWN
  */
  if (((bmap)^(tm_cm_info.register_events_bmap)) & (TM_CM_REGISTER_TC_DOWN_EVENT))  /*lint !e655*/
  {
    if (bmap & TM_CM_REGISTER_TC_DOWN_EVENT)  /*lint !e655*/
    {
      /* Register for CM_CALL_EVENT_EXIT_TC */
      if(cm_mm_client_call_reg( tm_cm_info.cm_client,
                          tm_cm_call_event_cb,
                          CM_CLIENT_EVENT_REG,
                          CM_CALL_EVENT_EXIT_TC,     /* from */
                          CM_CALL_EVENT_EXIT_TC,     /* to   */
                          NULL  /* not interested in other clients' cmd errors */
                        ) != CM_CLIENT_OK) 
      {
        MSG_ERROR("cm_mm_client_call_reg failed",0,0,0);
        ret_val = FALSE;
      }
    }
    else
    {
      /* De-register for CM_CALL_EVENT_EXIT_TC */
      if(cm_mm_client_call_reg( tm_cm_info.cm_client,
                            tm_cm_call_event_cb,
                            CM_CLIENT_EVENT_DEREG,
                            CM_CALL_EVENT_EXIT_TC,     /* from */
                            CM_CALL_EVENT_EXIT_TC,     /* to   */
                            NULL  /* not interested in other clients' cmd errors */
                            ) != CM_CLIENT_OK) 
       {
         MSG_ERROR("cm_mm_client_call_dereg failed",0,0,0);
         ret_val = FALSE;
       }
    }
  }

  /* Reg/Dereg only if SS_EVENT bit changed
  ** (new X-OR old) & SS_EVENT
  */
  if (((bmap)^(tm_cm_info.register_events_bmap)) & (TM_CM_REGISTER_SS_EVENT))  /*lint !e655*/
  {
    if (bmap & TM_CM_REGISTER_SS_EVENT)  /*lint !e655*/
    {
       MSG_MED("Register for SS event",0,0,0);
       /* Register for SS_NEW event from CM.
       */
       if(cm_mm_client_ss_reg_msim(
             tm_cm_info.cm_client,
             tm_cm_ss_event_cb,
             CM_CLIENT_EVENT_REG,
             CM_SS_EVENT_SRV_NEW,
             CM_SS_EVENT_SRV_NEW,
             NULL,
             SYS_MODEM_AS_ID_1_MASK|SYS_MODEM_AS_ID_2_MASK|SYS_MODEM_AS_ID_3_MASK) != CM_CLIENT_OK) 
       {
         MSG_ERROR("cm_mm_client_call_reg_msim failed",0,0,0);
         ret_val = FALSE;
       }
    }
    else
    {
      /* De-register for SS_NEW event from CM.
      */
      MSG_MED("De-Register for SS event",0,0,0);
      if(cm_mm_client_ss_reg_msim(
             tm_cm_info.cm_client,
             tm_cm_ss_event_cb,
             CM_CLIENT_EVENT_DEREG,
             CM_SS_EVENT_SRV_NEW,
             CM_SS_EVENT_SRV_NEW,
             NULL,
             SYS_MODEM_AS_ID_1_MASK|SYS_MODEM_AS_ID_2_MASK|SYS_MODEM_AS_ID_3_MASK) != CM_CLIENT_OK) 
       {
         MSG_ERROR("cm_mm_client_call_dereg failed",0,0,0);
         ret_val = FALSE;
       }
    }
  }

  /* Maintain local copy of bmap */
  tm_cm_info.register_events_bmap = bmap;
  return ret_val;
}

/*===========================================================================

FUNCTION  tm_cm_iface_send_cmd

DESCRIPTION 
  Handles a request for specific command

RETURN VALUE 
  None

DEPENDENCIES

===========================================================================*/
boolean tm_cm_iface_send_cmd(const tm_cm_iface_cmd_info_s_type *cmd_info)
{
  if(tm_cm_info.is_init == FALSE)
  {                                                
    MSG_ERROR("TM CM Iface not initialized",0,0,0);
    return FALSE;
  }

  if(cmd_info == NULL )
  {
    MSG_ERROR("Null cmd_info ptr!",0,0,0);
    return FALSE;
  }

  if(cmd_info->cmd_type == TM_CM_CMD_WAKEUP_CDMA)
  {
    if(cm_ph_cmd_wakeup_from_standby2(
          tm_cm_ph_cmd_cb,
          NULL,
          tm_cm_info.cm_client,
          CM_WAKEUP_MAIN) == FALSE)
    {
      MSG_ERROR("Cannot send PH cmd to wakeup",0,0,0);
      return FALSE;
    }
  }
  return TRUE;
}
#endif /* FEATURE_CGPS_CDMA_IF */

/*===========================================================================

FUNCTION tm_cm_iface_submodule_init

DESCRIPTION
  Initialializes the TM_CM for calling protocol submodule

DEPENDENCIES
  None.

RETURN VALUE
  Boolean.

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_iface_submodule_init(tm_prtl_type prtl_type ,const tm_cm_pd_resp_is_within_t2m_f_type pd_resp_cb_f)
{
  boolean ret_val = FALSE;

  if((tm_cm_info.is_init == FALSE) || (prtl_type == TM_PRTL_TYPE_NONE))
  {
    MSG_ERROR("TM CM Iface not initialized or wrong protocol type",0,0,0);
    return FALSE;
  }

  if(prtl_type == TM_PRTL_TYPE_1X_CP)
  {
#ifdef FEATURE_CGPS_CDMA_IF
    ret_val = tm_cm_1x_cp_init(pd_resp_cb_f);
#endif /* FEATURE_CGPS_CDMA_IF */
  }
  else if(prtl_type == TM_PRTL_TYPE_UMTS_CP)
  {
#ifdef FEATURE_CGPS_UMTS_CP
    ret_val = tm_cm_umts_cp_init();
#endif /* FEATURE_CGPS_UMTS_CP */
  }
  else
  {
    MSG_ERROR("Wrong protocol type %d",prtl_type,0,0);
    ret_val = FALSE;
  }
  return ret_val;
}

/*===========================================================================

FUNCTION tm_cm_client_init

DESCRIPTION
  Initialializes the TM_CM, registers callbacks.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean.

SIDE EFFECTS
  None

===========================================================================*/

boolean tm_cm_client_init(void)
{
  uint8 sub = 0;
  uint8 stack = 0;

#ifdef __GRIFFON__ /* GRIFFON_DEBUG */

  tm_cm_info.is_init = TRUE;
  return FALSE;
#else /* GRIFFON_DEBUG */
  boolean ret_val = TRUE;
  tm_cm_info.is_init = FALSE;
  /*---------------------------------
    Initialize the CM client
   *------------------------------ */

  //Size of CM IPC used
  MSG_HIGH("Size of CM IPC used by SM_TM: %d bytes\n", sizeof(tm_cm_event_info_s_type), 0, 0);

  if(cm_client_init(CM_CLIENT_TYPE_PD, &tm_cm_info.cm_client) != CM_CLIENT_OK)
  {
    MSG_ERROR("cm_client_init failed",0,0,0);
    return FALSE;
  }

  /*---------------------------------
    Register for call events
   *------------------------------ */

  if(cm_mm_client_call_reg( tm_cm_info.cm_client,
                      tm_cm_call_event_cb,
                      CM_CLIENT_EVENT_REG,
                      CM_CALL_EVENT_ORIG,     /* from */
                      CM_CALL_EVENT_ORIG,     /* to   */
                      NULL  /* not interested in other clients' cmd errors */
                    ) != CM_CLIENT_OK) 
  {
    MSG_ERROR("cm_mm_client_call_reg failed",0,0,0);
    return FALSE;
  }


  if(cm_mm_client_call_reg( tm_cm_info.cm_client,
                      tm_cm_call_event_cb,
                      CM_CLIENT_EVENT_REG,
                      CM_CALL_EVENT_END,     /* from */
                      CM_CALL_EVENT_END,     /* to   */
                      NULL  /* not interested in other clients' cmd errors */
                    ) != CM_CLIENT_OK)
  {
    MSG_ERROR("cm_mm_client_call_reg failed",0,0,0);
    return FALSE;
  }

  if(cm_mm_client_call_reg( tm_cm_info.cm_client,
                      tm_cm_call_event_cb,
                      CM_CLIENT_EVENT_REG,
                      CM_CALL_EVENT_CONNECT,     /* from */
                      CM_CALL_EVENT_CONNECT,     /* to   */
                      NULL  /* not interested in other clients' cmd errors */
                    ) != CM_CLIENT_OK) 
  {
    MSG_ERROR("cm_mm_client_call_reg failed",0,0,0);
    ret_val = FALSE;
  }

  if(cm_mm_client_call_reg( tm_cm_info.cm_client,
                      tm_cm_call_event_cb,
                      CM_CLIENT_EVENT_REG,
                      CM_CALL_EVENT_HO_COMPLETE,     /* from */
                      CM_CALL_EVENT_HO_COMPLETE,     /* to   */
                      NULL  /* not interested in other clients' cmd errors */
                    ) != CM_CLIENT_OK) 
  {
    MSG_ERROR("cm_mm_client_call_reg failed",0,0,0);
    ret_val = FALSE;
  }

  
#ifdef FEATURE_CGPS_CDMA_IF
  // register OTASP status event for Over-The-Air MSID update (CDMA only)
  if(cm_mm_client_call_reg( tm_cm_info.cm_client,
                      tm_cm_call_event_cb,
                      CM_CLIENT_EVENT_REG,
                      CM_CALL_EVENT_OTASP_STATUS,     /* from */
                      CM_CALL_EVENT_OTASP_STATUS,     /* to   */
                      NULL  /* not interested in other clients' cmd errors */
                    ) != CM_CLIENT_OK)
  {
    MSG_ERROR("cm_mm_client_call_reg OTASP_STATUS failed",0,0,0);
    return FALSE;
  }
#endif /* FEATURE_CGPS_CDMA_IF */

  /*---------------------------------
    Register for Phone events.
   *------------------------------ */
  if(cm_client_ph_reg( tm_cm_info.cm_client,
                    tm_cm_ph_event_cb,
                    CM_CLIENT_EVENT_REG,
                    CM_PH_EVENT_SYS_SEL_PREF,   /* Mode preference was changed */
                    CM_PH_EVENT_SYS_SEL_PREF,
                    NULL    /* not interested in other clients' cmd errors */
                   ) != CM_CLIENT_OK)
  {
    MSG_ERROR("cm_client_ph_reg for Mode Pref. failed",0,0,0);
    return FALSE;
  }

  /*---------------------------------
     DSDS Phone Event.
  *----------------------------------*/
  if(cm_client_ph_reg( tm_cm_info.cm_client,
                    tm_cm_ph_event_cb,
                    CM_CLIENT_EVENT_REG,
                    CM_PH_EVENT_OPRT_MODE,   /* Mode preference was changed */
                    CM_PH_EVENT_OPRT_MODE, /* Per subscription info*/
                    NULL    /* not interested in other clients' cmd errors */
                   ) != CM_CLIENT_OK)
  {
    MSG_ERROR( "cm_client_ph_reg for Operating Mode failed",0,0,0);
    return FALSE;
  }

  if(cm_client_ph_reg( tm_cm_info.cm_client,
                    tm_cm_ph_event_cb,
                    CM_CLIENT_EVENT_REG,
                    CM_PH_EVENT_DUAL_STANDBY_PREF,   /* Mode preference was changed */
                    CM_PH_EVENT_DUAL_STANDBY_PREF, /* Per subscription info*/
                    NULL    /* not interested in other clients' cmd errors */
                   ) != CM_CLIENT_OK)
  {
    MSG_ERROR( "cm_client_ph_reg DSDS Event Reg failed",0,0,0);
    return FALSE;
  }

#ifndef FEATURE_GNSS_SA
    if(cm_client_subs_reg( tm_cm_info.cm_client,
                           tm_cm_ph_event_sub_pref_cb 
                         )!= CM_CLIENT_OK)
    {
      MSG_ERROR("cm_client_subs_reg error %d",(int)(ret_val),0,0);
    }
  /*---------------------------------
    Register for serving system events.
   *------------------------------ */

  if(cm_mm_client_ss_reg_msim(tm_cm_info.cm_client,        // pointer to the client struct
                      tm_cm_ss_event_cb,         // pointer to a cb function for ss events
                      CM_CLIENT_EVENT_REG,        // event registration
                      CM_SS_EVENT_SRV_CHANGED,    // from this call event
                      CM_SS_EVENT_SRV_CHANGED,    // to this call event
                      NULL,             // pointer to a cb function for errors
                      SYS_MODEM_AS_ID_1_MASK|SYS_MODEM_AS_ID_2_MASK|SYS_MODEM_AS_ID_3_MASK)// subscription id mask             
                     != CM_CLIENT_OK)
  {
    MSG_ERROR("cm_mm_client_ss_reg_msim failed",0,0,0);
    return FALSE;
  }
  /* Register for the Cell DCH responses from CM */
  if(cm_mm_client_ss_reg_msim(tm_cm_info.cm_client,        // pointer to the client struct
                      tm_cm_ss_event_cb,              // pointer to a cb function for ss events
                      CM_CLIENT_EVENT_REG,            // event registration
                      CM_SS_EVENT_DCH_CELL_ID_RSP,    // from this call event
                      CM_SS_EVENT_DCH_CELL_ID_RSP,    // to this call event
                      NULL,                 // pointer to a cb function for errors
                      SYS_MODEM_AS_ID_1_MASK|SYS_MODEM_AS_ID_2_MASK|SYS_MODEM_AS_ID_3_MASK)// subscription id mask             
                     != CM_CLIENT_OK)
  {
    MSG_ERROR("cm_mm_client_ss_reg_msim failed for DCH_CELL_ID_RSP event",0,0,0);
    return FALSE;
  }
#endif /*#ifndef FEATURE_GNSS_SA */

  if(cm_client_act( tm_cm_info.cm_client) != CM_CLIENT_OK)
  {
    MSG_ERROR("cm_client_act failed",0,0,0);
    return FALSE;
  }

  tm_cm_info.is_init = TRUE;

  tm_cm_info.e911_state = TM_CM_E911_STATE_NONE;
  tm_cm_info.sys_mode = SYS_SYS_MODE_NO_SRV;
  tm_cm_info.prev_sys_mode = SYS_SYS_MODE_NO_SRV;
  tm_cm_info.hdr_srv_status = SYS_SRV_STATUS_NO_SRV;
  tm_cm_info.srv_status = SYS_SRV_STATUS_NO_SRV;
  tm_cm_info.prev_srv_status = SYS_SRV_STATUS_NO_SRV;

#ifdef FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH
  /* RRC related Cell ID information. This is populated when the CM provides the cell id information.
     Set these values to be 0 to begin with. */
  memset(&tm_cm_info.rrc_cell_info, 0, sizeof(rrcmeas_plmn_cell_info_struct_type));
  memset(&tm_cm_info.rrc_cell_id_status, 0, sizeof(rrc_cell_id_status_type));
  tm_cm_info.ts_wcdma_cid_update_req = 0;
#endif //FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH

  tm_cm_info.dsds_ph_info.e_StandbyPref = SYS_MODEM_DS_PREF_NONE;
  tm_cm_info.dsds_ph_info.e_PrevDataSubs = SYS_MODEM_AS_ID_NONE;
  tm_cm_info.dsds_ph_info.e_ActiveSubs = 0;
  tm_cm_info.dsds_ph_info.e_DataSubs = SYS_MODEM_AS_ID_1;

  for (sub = 0; sub < TM_CM_MAX_NO_SUBS; sub++)
  {
    for (stack = 0; stack < TM_CM_MAX_NO_STACKS; stack++)
    {
      tm_cm_info.ss_info[sub][stack].is_operational = FALSE;
      tm_cm_info.ss_info[sub][stack].prev_sys_mode = SYS_SYS_MODE_NO_SRV;
    tm_cm_info.ss_info[sub][stack].sys_mode = SYS_SYS_MODE_NO_SRV;
    tm_cm_info.ss_info[sub][stack].srv_status = SYS_SRV_STATUS_NO_SRV;
    tm_cm_info.ss_info[sub][stack].srv_domain = SYS_SRV_DOMAIN_NO_SRV;
    tm_cm_info.ss_info[sub][stack].changed_fields = 0;
    tm_cm_info.ss_info[sub][stack].oper_to_non_oper_transition = FALSE;
    }
  } 
#ifndef FEATURE_GNSS_SA
  tm_cm_update_device_mode_info(); /** Retrieve device mode info using policy manager */
#endif /*#ifndef FEATURE_GNSS_SA*/
    
  return ret_val;
#endif /*GRIFFON_DEBUG*/
}

/*===========================================================================

FUNCTION tm_cm_map_ss_info

DESCRIPTION 
  Maps a  tm_cm_mm_ss_info_s_type data type to the coresponding cgps_SSInfoType
  data

DEPENDENCIES
  

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void tm_cm_map_ss_info( const tm_cm_mm_ss_info_s_type * const cp_ss_info_ptr,
            cgps_SSInfoType * p_cgps_ss_info, cgps_SSInfoDataType e_info_type)
{
  
  uint8 stack = 0;
  
  if((NULL == p_cgps_ss_info) ||
   (NULL == cp_ss_info_ptr))
  {
  MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "p_cgps_ss_info or cp_ss_info_ptr is NULL");
  return;
  }
 
  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Mapping SS info for SUB ID = %u, with %u stacks",
            cp_ss_info_ptr->asubs_id, cp_ss_info_ptr->number_of_stacks);
  
  memset(p_cgps_ss_info, 0x00, sizeof(*p_cgps_ss_info));
  p_cgps_ss_info->subs_id = cp_ss_info_ptr->asubs_id;
  p_cgps_ss_info->number_of_stacks = cp_ss_info_ptr->number_of_stacks;
      
  for (stack = 0; stack < cp_ss_info_ptr->number_of_stacks; stack++)
  {
    if ((cp_ss_info_ptr->multisim_stack_info[stack].is_operational)||
    (tm_cm_info.ss_info[cp_ss_info_ptr->asubs_id][stack].oper_to_non_oper_transition == TRUE))
    {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Mapping SS info for Stack %u",stack);
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Mapping changed_fields:%u, sys_mode:%u, sys_id_id_type:%u",
          cp_ss_info_ptr->multisim_stack_info[stack].changed_fields, 
          cp_ss_info_ptr->multisim_stack_info[stack].sys_mode, 
          cp_ss_info_ptr->multisim_stack_info[stack].sys_id.id_type);
      
      if(cp_ss_info_ptr->multisim_stack_info[stack].changed_fields & CM_SS_MOBILITY_MGMT_MASK)
      {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Mobility management mask is set");
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Date mm/dd/yy  %u/%u/%u",
           cp_ss_info_ptr->multisim_stack_info[stack].mm_information.univ_time_and_time_zone.month, 
           cp_ss_info_ptr->multisim_stack_info[stack].mm_information.univ_time_and_time_zone.day,
           cp_ss_info_ptr->multisim_stack_info[stack].mm_information.univ_time_and_time_zone.year);
      
      
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Time hh:mm:ss %u:%u:%u",
           cp_ss_info_ptr->multisim_stack_info[stack].mm_information.univ_time_and_time_zone.hour,
           cp_ss_info_ptr->multisim_stack_info[stack].mm_information.univ_time_and_time_zone.minute,
           cp_ss_info_ptr->multisim_stack_info[stack].mm_information.univ_time_and_time_zone.second);
      }
      else
      {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Mobility management mask is not detected");
      }
    
      p_cgps_ss_info->multisim_ssInfo[stack].ss_data_type = e_info_type;
      p_cgps_ss_info->multisim_ssInfo[stack].sys_mode = cp_ss_info_ptr->multisim_stack_info[stack].sys_mode;
    
      p_cgps_ss_info->multisim_ssInfo[stack].srv_status = cp_ss_info_ptr->multisim_stack_info[stack].srv_status;
      
      if (cp_ss_info_ptr->multisim_stack_info[stack].sys_id.id_type == SYS_SYS_ID_TYPE_IS95)
      {
        p_cgps_ss_info->multisim_ssInfo[stack].sid = cp_ss_info_ptr->multisim_stack_info[stack].sys_id.id.is95.sid;
        p_cgps_ss_info->multisim_ssInfo[stack].nid = cp_ss_info_ptr->multisim_stack_info[stack].sys_id.id.is95.nid;
      }
      else
      {
        p_cgps_ss_info->multisim_ssInfo[stack].sid = CM_SS_SID_NONE;
        p_cgps_ss_info->multisim_ssInfo[stack].nid = CM_SS_NID_NONE;
      }
      
      p_cgps_ss_info->multisim_ssInfo[stack].base_id = cp_ss_info_ptr->multisim_stack_info[stack].mode_info_cdma_info_base_id;
      p_cgps_ss_info->multisim_ssInfo[stack].base_lat = cp_ss_info_ptr->multisim_stack_info[stack].mode_info_cdma_info_base_lat;
      p_cgps_ss_info->multisim_ssInfo[stack].base_long = cp_ss_info_ptr->multisim_stack_info[stack].mode_info_cdma_info_base_long;
      p_cgps_ss_info->multisim_ssInfo[stack].srch_win_n = cp_ss_info_ptr->multisim_stack_info[stack].mode_info_cdma_info_srch_win_n;
        
      p_cgps_ss_info->multisim_ssInfo[stack].changed_fields = cp_ss_info_ptr->multisim_stack_info[stack].changed_fields;
      p_cgps_ss_info->multisim_ssInfo[stack].z_TimeZoneInfo.daylight_saving_adj_avail = 
              cp_ss_info_ptr->multisim_stack_info[stack].mm_information.daylight_saving_adj_avail;
      p_cgps_ss_info->multisim_ssInfo[stack].z_TimeZoneInfo.time_zone = 
              cp_ss_info_ptr->multisim_stack_info[stack].mm_information.time_zone;
      p_cgps_ss_info->multisim_ssInfo[stack].z_TimeZoneInfo.time_zone_avail = 
              cp_ss_info_ptr->multisim_stack_info[stack].mm_information.time_zone_avail;
      p_cgps_ss_info->multisim_ssInfo[stack].z_TimeZoneInfo.univ_time_and_time_zone.day = 
              cp_ss_info_ptr->multisim_stack_info[stack].mm_information.univ_time_and_time_zone.day;
      p_cgps_ss_info->multisim_ssInfo[stack].z_TimeZoneInfo.univ_time_and_time_zone.hour = 
              cp_ss_info_ptr->multisim_stack_info[stack].mm_information.univ_time_and_time_zone.hour;
      p_cgps_ss_info->multisim_ssInfo[stack].z_TimeZoneInfo.univ_time_and_time_zone.minute = 
              cp_ss_info_ptr->multisim_stack_info[stack].mm_information.univ_time_and_time_zone.minute;
      p_cgps_ss_info->multisim_ssInfo[stack].z_TimeZoneInfo.univ_time_and_time_zone.month = 
              cp_ss_info_ptr->multisim_stack_info[stack].mm_information.univ_time_and_time_zone.month;
      p_cgps_ss_info->multisim_ssInfo[stack].z_TimeZoneInfo.univ_time_and_time_zone.second = 
              cp_ss_info_ptr->multisim_stack_info[stack].mm_information.univ_time_and_time_zone.second;
      p_cgps_ss_info->multisim_ssInfo[stack].z_TimeZoneInfo.univ_time_and_time_zone.time_zone  = 
              cp_ss_info_ptr->multisim_stack_info[stack].mm_information.univ_time_and_time_zone.time_zone;
      p_cgps_ss_info->multisim_ssInfo[stack].z_TimeZoneInfo.univ_time_and_time_zone.year  = 
              cp_ss_info_ptr->multisim_stack_info[stack].mm_information.univ_time_and_time_zone.year;
      p_cgps_ss_info->multisim_ssInfo[stack].z_TimeZoneInfo.univ_time_and_time_zone_avail  = 
              cp_ss_info_ptr->multisim_stack_info[stack].mm_information.univ_time_and_time_zone_avail;
      
      if(cp_ss_info_ptr->multisim_stack_info[stack].mm_information.daylight_saving_adj_avail)
      {
        switch(cp_ss_info_ptr->multisim_stack_info[stack].mm_information.daylight_saving_adj)
        {
        case SYS_DAY_SAV_ADJ_NO_ADJUSTMENT:
          p_cgps_ss_info->multisim_ssInfo[stack].z_TimeZoneInfo.daylight_saving_adj = CGPS_DAY_SAV_ADJ_NO_ADJUSTMENT;
          break;
              
        case SYS_DAY_SAV_ADJ_PLUS_ONE_HOUR:
          p_cgps_ss_info->multisim_ssInfo[stack].z_TimeZoneInfo.daylight_saving_adj = CGPS_DAY_SAV_ADJ_PLUS_ONE_HOUR;
          break;
          
        case SYS_DAY_SAV_ADJ_PLUS_TWO_HOUR:
          p_cgps_ss_info->multisim_ssInfo[stack].z_TimeZoneInfo.daylight_saving_adj = CGPS_DAY_SAV_ADJ_PLUS_TWO_HOUR;
          break;
          
        default:
          p_cgps_ss_info->multisim_ssInfo[stack].z_TimeZoneInfo.daylight_saving_adj = CGPS_DAY_SAV_ADJ_NONE;
        }
      }
      else
      {
        p_cgps_ss_info->multisim_ssInfo[stack].z_TimeZoneInfo.daylight_saving_adj = CGPS_DAY_SAV_ADJ_NONE;
      }
      
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "TM CM Iface, SID:%u, NID:%u, base_id:%u",
          p_cgps_ss_info->multisim_ssInfo[stack].sid, p_cgps_ss_info->multisim_ssInfo[stack].nid, 
          p_cgps_ss_info->multisim_ssInfo[stack].base_id);
      
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "TM CM Iface, sys_mode:%u, srch_win_n:%u, ss_data_type:%u",
          (int32) p_cgps_ss_info->multisim_ssInfo[stack].sys_mode, p_cgps_ss_info->multisim_ssInfo[stack].srch_win_n, p_cgps_ss_info->multisim_ssInfo[stack].ss_data_type); 
    }
  }
}


/*===========================================================================

FUNCTION tm_cm_get_svlte_sys_mode

DESCRIPTION 
  Determine the sys_mode for SVLTE sub

DEPENDENCIES
  

RETURN VALUE
  sys_sys_mode_e_type

SIDE EFFECTS
  None

===========================================================================*/

sys_sys_mode_e_type tm_cm_get_svlte_sys_mode(const sys_sys_mode_e_type  cdma_stack_sys_mode,
                           const sys_sys_mode_e_type  lte_stack_sys_mode)
{
  sys_sys_mode_e_type  sys_mode =   SYS_SYS_MODE_NO_SRV;

  if( (lte_stack_sys_mode  != SYS_SYS_MODE_NO_SRV ) &&
      (cdma_stack_sys_mode != SYS_SYS_MODE_NO_SRV ))
  {
  /*Camped on both stacks*/        
  MSG_MED("Camped on both CDMA and LTE stack",0,0,0);

  if( (tm_core_is_agps_allowed_on_serv_sys(tm_cm_map_sys_mode(cdma_stack_sys_mode)) == TRUE) &&
      (tm_core_is_agps_allowed_on_serv_sys(tm_cm_map_sys_mode(lte_stack_sys_mode)) == FALSE))
  {
      /*CDMA stack mode is allowed but LTE stack mode is not*/
      sys_mode = cdma_stack_sys_mode; 
    }
  else if( (tm_core_is_agps_allowed_on_serv_sys(tm_cm_map_sys_mode(lte_stack_sys_mode)) == TRUE) &&
           (tm_core_is_agps_allowed_on_serv_sys(tm_cm_map_sys_mode(cdma_stack_sys_mode)) == FALSE))
  {
      /*LTE stack mode is allowed but main stack mode is not*/
      sys_mode = lte_stack_sys_mode;         
    }
    else if( (tm_core_is_agps_allowed_on_serv_sys(tm_cm_map_sys_mode(lte_stack_sys_mode)) == TRUE) &&
           (tm_core_is_agps_allowed_on_serv_sys(tm_cm_map_sys_mode(cdma_stack_sys_mode)) == TRUE))
  {
      if(tm_core_is_umts_prefered())
      {
      sys_mode = lte_stack_sys_mode;
      }
      else
      {
      sys_mode = cdma_stack_sys_mode;
      }
  }
    else
  {
      sys_mode = SYS_SYS_MODE_NO_SRV;
    }
  }
  return sys_mode;
}  


/*===========================================================================

FUNCTION tm_core_cm_notify_srv_status

DESCRIPTION 
  Sends serving status change notification to TM core if needed.
  Also sends serving status change notification to PDCOMM if needed.

DEPENDENCIES
  

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
static void tm_core_cm_notify_srv_status(void)
{
  boolean notify_tm = FALSE;  
  boolean notify_pd_comms = FALSE;
  boolean notify_tle_mgp = FALSE;
  boolean sglte_dd_sub = FALSE;
  boolean svlte_dd_sub = FALSE;
  boolean srlte_dd_sub = FALSE;
  boolean multimode_dd_sub = FALSE;
  boolean external_stack_selected = FALSE;
  boolean cdmaStackFound = FALSE;
  boolean lteStackFound = FALSE;
    
  tm_phone_event_payload_u_type *cm_evt_payload_ptr=NULL;

  
#ifdef FEATURE_CGPS_CELLDB
  cgps_SSInfoType ss_info;
#endif
  sys_sys_mode_e_type  cdma_stack_sys_mode       =   SYS_SYS_MODE_NO_SRV;
  sys_sys_mode_e_type  lte_stack_sys_mode        =   SYS_SYS_MODE_NO_SRV;
  sys_sys_mode_e_type  external_stack_sys_mode   =   SYS_SYS_MODE_NO_SRV;
  sys_sys_mode_e_type  sys_mode                  =   SYS_SYS_MODE_NO_SRV;
  sys_srv_status_e_type srv_status				 =   SYS_SRV_STATUS_NO_SRV;

  tm_core_device_mode_e_type device_mode  = tm_cm_get_device_mode();
  sys_modem_as_id_e_type     dd_sub_id    = tm_cm_dsds_get_dd_subs();
  sys_subs_feature_t         dd_feature_mode = tm_cm_get_subs_feature(dd_sub_id);
  uint8            stack      = 0;
  uint8            lteStack     = 0;
  uint8            cdmaStack    = 0;
  
  /* Validate the data sub id */
  if ((dd_sub_id != SYS_MODEM_AS_ID_1) &&
    (dd_sub_id != SYS_MODEM_AS_ID_2) &&
    (dd_sub_id != SYS_MODEM_AS_ID_3))
  {
  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Default Data sub is not a valid sub_id: %u ", dd_sub_id);
  return;
  }
  
  sglte_dd_sub = tm_cm_is_sglte_sub(dd_sub_id);
  svlte_dd_sub = tm_cm_is_svlte_sub(dd_sub_id);
  srlte_dd_sub = tm_cm_is_srlte_sub(dd_sub_id);
  multimode_dd_sub = tm_cm_is_multimode_sub(dd_sub_id);

  MSG_7( MSG_SSID_GPSSM, MSG_LEGACY_MED,
         "tm_core_cm_notify_srv_status: device_mode: %u, dd_feature_mode: %u, dd_sub_id: %u, DD sub SVLTE: %u, DD sub SGLTE: %u, DD sub SRLTE: %u, DD sub multimode: %u",
             device_mode, dd_feature_mode, dd_sub_id, svlte_dd_sub, sglte_dd_sub, srlte_dd_sub, multimode_dd_sub);
  
  
  switch (dd_feature_mode)  
  {
  case SYS_SUBS_FEATURE_MODE_SGLTE:
    external_stack_sys_mode = tm_cm_info.external_stack_ss_info.sys_mode;      
    if (external_stack_sys_mode != SYS_SYS_MODE_NO_SRV)
    {
      external_stack_selected = TRUE;
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "tm_core_cm_notify_srv_status: External stack mode %u selected",external_stack_sys_mode);
      sys_mode = external_stack_sys_mode;
	  srv_status = tm_cm_info.external_stack_ss_info.srv_status;
    }
    else
    {
    for (stack = 0; stack < TM_CM_MAX_NO_STACKS; stack++)
    {
      if ((tm_cm_info.ss_info[dd_sub_id][stack].is_operational)||
          (tm_cm_info.ss_info[dd_sub_id][stack].oper_to_non_oper_transition == TRUE))
      {
        /* We dont anticipate another GSM RAT on the same sub, if there is a new varient, then this check need to be revisited*/
        if (tm_cm_info.ss_info[dd_sub_id][stack].sys_mode != SYS_SYS_MODE_GSM)
        {
          if(tm_core_is_agps_allowed_on_serv_sys(tm_cm_map_sys_mode(tm_cm_info.ss_info[dd_sub_id][stack].sys_mode)))
          {
                sys_mode = tm_cm_info.ss_info[dd_sub_id][stack].sys_mode;
				srv_status = tm_cm_info.ss_info[dd_sub_id][stack].srv_status;
          }
          else
          {
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "tm_core_cm_notify_srv_status: Stack mode %u not allowed by NV",tm_cm_info.ss_info[dd_sub_id][stack].sys_mode);
          }
        break;
          }
      }
    }
  
    if (sys_mode == SYS_SYS_MODE_NO_SRV)
      {
      /*Not camped on any stack*/        
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_core_cm_notify_srv_status: In SGLTE mode but not camped on any stack on DD sub = %d",dd_sub_id);
        sys_mode = SYS_SYS_MODE_NO_SRV;
		srv_status = SYS_SRV_STATUS_NO_SRV;
    }  
    }
    break;

    case SYS_SUBS_FEATURE_MODE_SVLTE: 
    case SYS_SUBS_FEATURE_MODE_SRLTE:
    case SYS_SUBS_FEATURE_MODE_DUAL_MULTIMODE:

    for (stack = 0; stack < TM_CM_MAX_NO_STACKS; stack++)
    {
    if ((tm_cm_info.ss_info[dd_sub_id][stack].is_operational == TRUE)||
      (tm_cm_info.ss_info[dd_sub_id][stack].oper_to_non_oper_transition == TRUE))
    {
      if (tm_cm_info.ss_info[dd_sub_id][stack].sys_mode == SYS_SYS_MODE_LTE)
          {
            lteStack = stack;
        lteStackFound = TRUE;
      }
      else if (tm_cm_info.ss_info[dd_sub_id][stack].sys_mode == SYS_SYS_MODE_CDMA)
          {
            cdmaStack = stack;
        cdmaStackFound = TRUE;
      }
    }
      }

    cdma_stack_sys_mode = tm_cm_info.ss_info[dd_sub_id][cdmaStack].sys_mode;
    lte_stack_sys_mode  = tm_cm_info.ss_info[dd_sub_id][lteStack].sys_mode;
      
    if (cdmaStackFound && lteStackFound)
    {
      /* In SVLTE mode */
      sys_mode = tm_cm_get_svlte_sys_mode(cdma_stack_sys_mode, lte_stack_sys_mode);
	  if (sys_mode == cdma_stack_sys_mode)
	  {
	    srv_status = tm_cm_info.ss_info[dd_sub_id][cdmaStack].srv_status;
	  }
	  else if (sys_mode == lte_stack_sys_mode)
	  {
	    srv_status = tm_cm_info.ss_info[dd_sub_id][lteStack].srv_status;
	  }
    }
    else if (cdmaStackFound)
    {
      /*Only cdma stack camped*/
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_core_cm_notify_srv_status:Camped only on CDMA stack");

    /*Check if cdma stack serving system is allowed*/
    if(tm_core_is_agps_allowed_on_serv_sys(tm_cm_map_sys_mode(cdma_stack_sys_mode)))
    {
        sys_mode = cdma_stack_sys_mode;
		srv_status = tm_cm_info.ss_info[dd_sub_id][cdmaStack].srv_status;
    }
    else
    {
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_core_cm_notify_srv_status:CDMA stack mode %u not allowed by NV",cdma_stack_sys_mode);
    }
    }
    else if (lteStackFound)
    {
      /*Only LTE stack camped*/
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_core_cm_notify_srv_status:Camped only on LTE stack");
    
      /*Check if LTE stack serving system is allowed*/
      if(tm_core_is_agps_allowed_on_serv_sys(tm_cm_map_sys_mode(lte_stack_sys_mode)))
      {
        sys_mode = lte_stack_sys_mode;
		srv_status = tm_cm_info.ss_info[dd_sub_id][lteStack].srv_status;
      }
      else
      {
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_core_cm_notify_srv_status:LTE stack mode %u not allowed by NV",lte_stack_sys_mode);
      }
    }
    else 
    {
      /* Not SVLTE /SGLTE/CDMA/LTE, we have only 1 stack */
    if(tm_core_is_agps_allowed_on_serv_sys(tm_cm_map_sys_mode(tm_cm_info.ss_info[dd_sub_id][0].sys_mode)))
      {
        sys_mode = tm_cm_info.ss_info[dd_sub_id][0].sys_mode;
		srv_status = tm_cm_info.ss_info[dd_sub_id][0].srv_status;
      }
      else
      {
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_core_cm_notify_srv_status:Stack mode %u not allowed by NV",tm_cm_info.ss_info[dd_sub_id][0].sys_mode);
      }
    }
    break;

  case SYS_SUBS_FEATURE_MODE_NORMAL:
    /* Not SVLTE nor SGLTE, we have only 1 stack */
    if(tm_core_is_agps_allowed_on_serv_sys(tm_cm_map_sys_mode(tm_cm_info.ss_info[dd_sub_id][0].sys_mode)))
      {
      sys_mode = tm_cm_info.ss_info[dd_sub_id][0].sys_mode;
	  srv_status = tm_cm_info.ss_info[dd_sub_id][0].srv_status;
      }
      else
      {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_core_cm_notify_srv_status:Stack mode %u not allowed by NV",tm_cm_info.ss_info[dd_sub_id][0].sys_mode);
      }
    break;
     
  default:
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_core_cm_notify_srv_status: Unsupported Feature mode %u", dd_feature_mode);
    sys_mode = SYS_SYS_MODE_NO_SRV;
	srv_status = SYS_SRV_STATUS_NO_SRV;
  }
    
  MSG_7( MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_core_cm_notify_srv_status: DD sub id = %u, is SVLTE sub = %u, is SGLTE sub = %u, is SRLTE sub = %u, is multimode sub = %u, selected sys_mode = %d, srv_status = %d",
          (uint32)dd_sub_id, (uint32)svlte_dd_sub, (uint32)sglte_dd_sub, (uint32)srlte_dd_sub, (uint32)multimode_dd_sub, sys_mode, srv_status);

  if (tm_cm_info.srv_status != srv_status)
  { /* Update if serving status has changed. */
    tm_cm_info.prev_srv_status = tm_cm_info.srv_status;
	tm_cm_info.srv_status = srv_status;
  } 
  
  /* Notify for sys_mode changes and also for dd_sub_id changes */
  if((tm_cm_info.sys_mode != sys_mode)||
	 (dd_sub_id !=  tm_cm_info.dsds_ph_info.e_PrevDataSubs))
  {
    tm_cm_info.prev_sys_mode = tm_cm_info.sys_mode;
    tm_cm_info.sys_mode = sys_mode; 	  

    notify_tm = TRUE;

    switch (device_mode)
    {
      case TM_CORE_DEVICE_MODE_SS:
        /* Notify pd_comms only for non-sglte architecture */
        if (!external_stack_selected)
        {
        notify_pd_comms = TRUE;
        }
      break;

    case TM_CORE_DEVICE_MODE_DSDS:
    case TM_CORE_DEVICE_MODE_DSDA_VOICE:
    case TM_CORE_DEVICE_MODE_DSDA_VOICE_DATA:
    case TM_CORE_DEVICE_MODE_TSTS:  
      notify_pd_comms = TRUE;
      /* Notify tle_mgp only for non-sglte architecture */
      if (!external_stack_selected)
      {
        notify_tle_mgp = TRUE;
      }
      break;

    default:
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_core_cm_notify_srv_status: Unknown device mode %u", device_mode);
      
    }   
    }
  
      
#ifndef FEATURE_GNSS_SA
  if (notify_pd_comms)
  {
    /*Notify Pdcomm of the serving system change*/
    pd_comms_app_srv_sys_change_notification(tm_cm_info.sys_mode);
  }
#endif  
  if(notify_tm)
  {
    if ( tm_cm_info.sys_mode == SYS_SYS_MODE_CDMA )
    {
           cm_evt_payload_ptr = 
                (tm_phone_event_payload_u_type *)pdsm_getbuf(sizeof(tm_phone_event_payload_u_type));
           if(cm_evt_payload_ptr != NULL)
           {
#ifdef FEATURE_CGPS_UMTS_CP
             cm_evt_payload_ptr->ss_info.sys_mode = tm_cm_info.sys_mode; 
             cm_evt_payload_ptr->ss_info.prev_sys_mode = tm_cm_info.prev_sys_mode;
#endif
             /* processing of srv_status_mask and mode_mask identical, use SS_SRV_STATUS for all */
     MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_core_cm_notify_srv_status: E911 Sending Current and previous Serving stack information");

             tm_core_cm_notify_event(TM_PHONE_EVENT_IN_1X_COVERAGE,cm_evt_payload_ptr); 
             (void)pdsm_freebuf((char *)cm_evt_payload_ptr);
           }
           else
           { 
     MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_core_cm_notify_srv_status: No buffer available");
             tm_core_cm_notify_event(TM_PHONE_EVENT_IN_1X_COVERAGE,NULL);
           }
    }
    else if ( ( tm_cm_info.sys_mode == SYS_SYS_MODE_WCDMA ) ||
              ( tm_cm_info.sys_mode == SYS_SYS_MODE_GW ) ||
              ( tm_cm_info.sys_mode == SYS_SYS_MODE_GSM ) ||
              ( tm_cm_info.sys_mode == SYS_SYS_MODE_LTE ) 
            )
    {
         cm_evt_payload_ptr = 
                (tm_phone_event_payload_u_type *)pdsm_getbuf(sizeof(tm_phone_event_payload_u_type));
           if(cm_evt_payload_ptr != NULL)
           {
#ifdef FEATURE_CGPS_UMTS_CP
             cm_evt_payload_ptr->ss_info.sys_mode = tm_cm_info.sys_mode; 
             cm_evt_payload_ptr->ss_info.prev_sys_mode = tm_cm_info.prev_sys_mode;
#endif
             /* processing of srv_status_mask and mode_mask identical, use SS_SRV_STATUS for all */
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_core_cm_notify_srv_status: Sending Current and Previous Serving stack information");

             tm_core_cm_notify_event(TM_PHONE_EVENT_IN_UMTS_COVERAGE,cm_evt_payload_ptr); 
             (void)pdsm_freebuf((char *)cm_evt_payload_ptr);
      }
    else
      {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_core_cm_notify_srv_status: No buffer available");
              tm_core_cm_notify_event(TM_PHONE_EVENT_IN_UMTS_COVERAGE,NULL);
           }
    }

#ifdef FEATURE_SUPL1_TDS
    else if ( tm_cm_info.sys_mode == SYS_SYS_MODE_TDS )
    {
      tm_core_cm_notify_event(TM_PHONE_EVENT_IN_TDS_COVERAGE, NULL);
    }
#endif
    else if( tm_cm_info.sys_mode == SYS_SYS_MODE_NO_SRV )
    {
      tm_core_cm_notify_event(TM_PHONE_EVENT_OOS, NULL);
    }        
  }

  if (notify_tle_mgp)
  {
    if (device_mode == TM_CORE_DEVICE_MODE_SS)
    {
#ifdef FEATURE_CGPS_CELLDB
      memset(&ss_info, 0x00, sizeof(ss_info));
      ss_info.subs_id = dd_sub_id;
    if (svlte_dd_sub || sglte_dd_sub || srlte_dd_sub)
    {
      ss_info.number_of_stacks = 2;
    }
    else
    {
      ss_info.number_of_stacks = 1;
    }

    for (stack = 0; stack < ss_info.number_of_stacks; stack++)
    {
      ss_info.multisim_ssInfo[stack].ss_data_type = CGPS_SSINFO_SRV_SYS;
      ss_info.multisim_ssInfo[stack].sys_mode = tm_cm_info.ss_info[dd_sub_id][stack].sys_mode;
    ss_info.multisim_ssInfo[stack].srv_status = tm_cm_info.ss_info[dd_sub_id][stack].srv_status;
    
    MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "tm_core_cm_notify_srv_status: Serving sys_mode %u and srv_status %u passed to TLM and MGP",
      ss_info.multisim_ssInfo[stack].sys_mode, ss_info.multisim_ssInfo[stack].srv_status);
    }
      
#ifdef FEATURE_CGPS_XTRA_T
      tle_SSChange(&ss_info);
#endif /* FEATURE_CGPS_XTRA_T */
      mgp_SSChange(&ss_info);
#endif /* FEATURE_CGPS_CELLDB*/ 
    }
  }
}



/*===========================================================================

FUNCTION tm_cm_iface_is_serving_system_avail

DESCRIPTION 
  This funciton checks if the phone is camped on a particular RAT. Both stacks are checked for camped serving system.

DEPENDENCIES
  

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_iface_is_serving_system_avail(cgps_SrvSystemType system_type)
{
  if( (system_type == tm_cm_map_sys_mode(tm_cm_info.ss_info[tm_cm_info.dsds_ph_info.e_ActiveSubs][0].sys_mode)) ||
      (system_type == tm_cm_map_sys_mode(tm_cm_info.ss_info[tm_cm_info.dsds_ph_info.e_ActiveSubs][1].sys_mode))
     )
  {
    return TRUE;
  }
  return FALSE;
}



#ifdef FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH
/*===========================================================================

FUNCTION tm_cm_ss_cmd_cb

DESCRIPTION
    This is a function which serves system command callback functions from CM. 

PARAMETERS 
  data_block_ptr: Pointer to the client-provided data block.
  ss_cmd: Indicates which serving system command the ss_cmd_err error status is for.
  ss_cmd_err: Serving system command error code indicating if the 
         command is rejected and the reason for the rejection.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_cm_ss_cmd_cb (
  void                    *data_block_ptr,
  cm_ss_cmd_e_type        ss_cmd,
  cm_ss_cmd_err_e_type    ss_cmd_err
)
{
  switch (ss_cmd) 
  {
    case CM_SS_CMD_GET_DCH_CELL_ID:
      /* Check whether the command succeeded */
      if(ss_cmd_err == CM_SS_CMD_ERR_NOERR) 
      {
        /* The command for getting cell id in CELL_DCH mode succeeded  */
        MSG_LOW("Command CM_SS_CMD_GET_DCH_CELL_ID succeeded",0,0,0);
      }
      else if(ss_cmd_err == CM_SS_CMD_ERR_DUP_DCH_CMD) 
      {
        /* The command for getting cell id in CELL_DCH mode succeeded  */
        MSG_LOW("CM_SS_CMD_GET_DCH_CELL_ID duplicate DCH CELL INFO request.",0,0,0);
      }
      else
      {
        /* The command for getting cell id in CELL_DCH mode failed  */
        MSG_ERROR("Command CM_SS_CMD_GET_DCH_CELL_ID failed. Error code:: %d",ss_cmd_err,0,0);
      }
      break;

    default:
      MSG_LOW("Command %d",ss_cmd,0,0);
      break;
  }
}
/*===========================================================================

FUNCTION tm_cm_request_cell_info

DESCRIPTION
  This function is used to request CM to obtain a valid cell
  ID info in DCH mode.

PARAMETERS 
  NONE.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  The function has 2 callbacks ::
  tm_cm_ss_cmd_cb :: To indicate if the CELL_DCH command succeeded.
  tm_cm_ss_event_cb :: Which gives the cell info in CELL_DCH mode . 
===========================================================================*/
boolean tm_cm_request_cell_info(void)
{

  if (SYS_SYS_MODE_WCDMA == tm_cm_info.ss_info[tm_cm_dsds_get_dd_subs()][0].as_sys_mode)
  {
  return ( cm_ss_cmd_get_dch_cell_info ( tm_cm_ss_cmd_cb, /* client callback function */
                                         NULL, /* pointer to client callback data block */
                                         tm_cm_info.cm_client /* requesting client ID */
										  ));
}
  else if (SYS_SYS_MODE_NO_SRV == tm_cm_info.ss_info[tm_cm_dsds_get_dd_subs()][0].as_sys_mode) 
  {
	if (SYS_SYS_MODE_WCDMA == tm_cm_get_current_dd_sys_mode())
	{
	  return ( cm_ss_cmd_get_dch_cell_info ( tm_cm_ss_cmd_cb, /* client callback function */
										   NULL, /* pointer to client callback data block */
										   tm_cm_info.cm_client /* requesting client ID */
										  ));
	}
  }
  
  MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, 
  	"tm_cm_request_cell_info: Not in WCDMA mode. Cell DCH request not sent to CM, as_sys_mode=%d, sys_mode=%d",
	tm_cm_info.ss_info[tm_cm_dsds_get_dd_subs()][0].as_sys_mode, tm_cm_get_current_dd_sys_mode); 
  return FALSE;  
}

/*===========================================================================

FUNCTION tm_cm_request_wcdma_dch_cell_info

DESCRIPTION
  This function is used to request CM to obtain a valid cell
  ID in WCDMA DCH mode.

PARAMETERS 
  NONE.
  
DEPENDENCIES

RETURN VALUE
  booelan
 
SIDE EFFECTS

===========================================================================*/
boolean tm_cm_request_wcdma_dch_cell_info(void)
{
#if 0 // def FEATURE_WCDMA_GPS_CELL_ID_READING
/* should we remove this legacy code, all TM-CM interface is based on below FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH feature now */
  rrc_cmd_type *rrc_cmd_ptr;

  /* Allocate some memory for the command */
  rrc_cmd_ptr = gnss_wwan_iface_rrc_get_lsm_cmd_buf();

  if ( rrc_cmd_ptr == NULL )
  {
    MSG_ERROR("CELLIDDCH: Failed to get RRC command buffer!",0,0,0);
    return FALSE;
  }

  rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_GPS_CELL_ID_REQUEST;

  MSG_HIGH("CELLIDDCH: Sent cell id request to RRC!",0,0,0);
  /* Send the command to RRC */
  gnss_wwan_iface_rrc_put_lsm_cmd( rrc_cmd_ptr );
  return TRUE;

#elif defined ( FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH )  

  uint32 current_time_secs = time_get_secs();

  if ((uint32)(current_time_secs - tm_cm_info.ts_wcdma_cid_update_req) < TM_CM_WCDMA_DCH_UPDATE_GAP_SECS)
  {
    MSG_HIGH("CELLIDDCH: WCDMA DCH CID update already requested!",0,0,0);
    return TRUE; 
  }

  tm_cm_info.ts_wcdma_cid_update_req = current_time_secs;  

  MSG_HIGH("CELLIDDCH: Cell id request sent to CM!",0,0,0);
  /* Request the new cell id information from CM */
  if (tm_cm_request_cell_info() == FALSE) /* no buffer to request */
  {
    MSG_ERROR("CELLIDDCH: Failed to send CM cmd for DCH CID!",0,0,0);
    tm_cm_info.ts_wcdma_cid_update_req = 0;  
    return FALSE;
  }

  return TRUE;

#else
  MSG_HIGH("CELLIDDCH: No CELL_DCH cell_info request mechanism enabled.", 0, 0, 0);
  return FALSE;
#endif /* FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH */
}

#endif //FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH


/*===========================================================================

FUNCTION tm_cm_process_external_srv_changed

DESCRIPTION
  This function processes srv changes from external modem

DEPENDENCIES


RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void tm_cm_process_external_srv_changed(
    const pdsm_cell_info_s_type *external_cell_db_info_ptr
)
{
  sys_modem_as_id_e_type sglte_sub = SYS_MODEM_AS_ID_NONE;
  sys_modem_as_id_e_type dd_sub_id = tm_cm_dsds_get_dd_subs();
  
  if (tm_cm_info.external_stack_ss_info.prev_sys_mode !=
      tm_cm_info.external_stack_ss_info.sys_mode)
  {
     tm_cm_info.external_stack_ss_info.prev_sys_mode =  
    tm_cm_info.external_stack_ss_info.sys_mode;
  }  

  /* Checking two cases to verify if external modem is OOS
        1. When the serviceStatus says OOS or UNK
        2. When LAC and CID is -1. This happens when we receive an inject external cell update
            that indicate that we are camped on a non-gsm RAT. And for SGLTE that means its camped on 
            the main modem. If RAT is non-gsm then LAC and CID will be -1
    */
  if (((external_cell_db_info_ptr->serviceStatus == STATUS_OUT_OF_SERVICE) ||
       (external_cell_db_info_ptr->serviceStatus == STATUS_UNKNOWN)) ||
      ((external_cell_db_info_ptr->cellInfo.gsmCell.LAC == HEX_NEGATIVE_ONE) &&
       (external_cell_db_info_ptr->cellInfo.gsmCell.CID == HEX_NEGATIVE_ONE)))
  {
    tm_cm_info.external_stack_ss_info.sys_mode = SYS_SYS_MODE_NO_SRV; 
  } 
  else
  {
    tm_cm_info.external_stack_ss_info.sys_mode = 
    tm_cm_map_external_RAT(external_cell_db_info_ptr->activeRAT);
    
  }


  MSG_HIGH("tm_cm_process_external_srv_changed: Prev_sys_mode = %d and current sys_mode = %d and activeExternalRat = %d ",tm_cm_info.external_stack_ss_info.prev_sys_mode
            ,tm_cm_info.external_stack_ss_info.sys_mode, external_cell_db_info_ptr->activeRAT);

  sglte_sub = tm_cm_get_sglte_sub();

  if (sglte_sub == dd_sub_id)
  {
    /* SGLTE sub is the DD sub, so update tm_core */
    tm_core_cm_notify_srv_status();
  }
  else
  {
    MSG_HIGH("tm_cm_process_external_srv_changed:External update received for sglte_sub %d, and its not a DD sub. DD sub is %d", 
    (uint8)sglte_sub, dd_sub_id, 0);
  } 

}

/*===========================================================================

FUNCTION tm_cm_process_ext_emergency_call_state

DESCRIPTION
  This function processes srv changes from external modem

DEPENDENCIES


RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void tm_cm_process_ext_emergency_enter( tm_cm_lte_emergency_e_type call_state)
{

  tm_cm_phone_state_info_s_type phone_state_info;

  memset(&phone_state_info, 0, sizeof(tm_cm_phone_state_info_s_type));
  
  tm_cm_iface_get_phone_state(&phone_state_info);

  if(call_state == TM_CM_LTE_EMERGENCY_ECBM)
  {
    MSG_LOW("Ignoring ECBM indication from MSGR. Wait for ECBM from PH event",0,0,0);
  }
  if(call_state == TM_CM_LTE_EMERGENCY_START)
  {
    tm_cm_handle_emergency_call_orig_event();
    mgp_UpdateEmergencyEvent(GNSS_EMERGENCY_EVENT_CALL_ORIGINATION);    
  }
  if(call_state == TM_CM_LTE_EMERGENCY_STOP)
  {
    if (tm_cm_info.e911_state == TM_CM_E911_STATE_ORIG)
    {
      MSG_MED("Emergency VoLTE call ended without entering CB CallType=%d",0,0,0);
      tm_cm_handle_emergency_call_end_event();
      mgp_UpdateEmergencyEvent(GNSS_EMERGENCY_EVENT_CALL_END);

    }
  }
  MSG_MED("TM_CM: Got 3rd party indication for e911 state %d, e911_state=%d srv_system=%d", call_state, tm_cm_info.e911_state,tm_cm_info.sys_mode); 
}

#ifndef FEATURE_GNSS_SA
/**===========================================================================
FUNCTION tm_cm_get_device_mode_info

DESCRIPTION: 
 Get device mode. Combination number of SIMs and active subscriptions 
 determine the device configuration mode.

DEPENDENCIES:
  None

RETURN VALUE: 
  None
 
SIDE EFFECTS
  None.

===========================================================================*/
void tm_cm_update_device_mode_info(void)
{
  policyman_status_t     status;
  policyman_item_t const  *pItem     = NULL;
  policyman_item_id_t const  id      = POLICYMAN_ITEM_DEVICE_CONFIGURATION;
  size_t           num_sim;
  size_t           num_active_sub;
  uint8              sub_id    = 0;
  
  status = policyman_get_items(&id, 1, &pItem);
  if ( POLICYMAN_SUCCEEDED(status) ) 
  { 
  status = policyman_device_config_overall_feature(pItem, &tm_core_device_config.tm_cm_overall_device_feature);
  if(POLICYMAN_SUCCEEDED(status))
  {
    status = policyman_device_config_num_sims(pItem, &num_sim);
    if ( POLICYMAN_SUCCEEDED(status) ) 
    {
      tm_core_device_config.no_of_sims = num_sim;
    status = policyman_device_config_max_active(pItem, &num_active_sub);
    if ( POLICYMAN_SUCCEEDED(status) ) 
    {
      tm_core_device_config.no_of_active_subs = num_active_sub;
      if (( 3 == num_sim) && (1 == num_active_sub)) 
      {
      tm_core_device_config.tm_cm_device_info = TM_CORE_DEVICE_MODE_TSTS;
      }
      if ( (2 == num_sim) && (2 == num_active_sub)) 
      {
      tm_core_device_config.tm_cm_device_info = TM_CORE_DEVICE_MODE_DSDA_VOICE;
      }
      else if ( (2 == num_sim) && (1 == num_active_sub)) 
      {
      tm_core_device_config.tm_cm_device_info = TM_CORE_DEVICE_MODE_DSDS;
      }
      else
      {
      tm_core_device_config.tm_cm_device_info = TM_CORE_DEVICE_MODE_SS;
      }

      for (sub_id = 0; sub_id < num_sim; sub_id++)
      {
        if(POLICYMAN_SUCCEEDED(policyman_device_config_get_subs_feature(pItem,0,sub_id,&tm_core_device_config.tm_cm_subs_feature[sub_id])))
        {
          MSG_HIGH("tm_cm_update_device_mode_info: policyman subs_feature[%d] %d", sub_id, tm_core_device_config.tm_cm_subs_feature[sub_id], 0);
        }
      else
      {
        MSG_ERROR("tm_cm_update_device_mode_info: TM_CM: Failed to retrieve subs_feature from policy manager", 0, 0, 0);
      }

      if(POLICYMAN_SUCCEEDED(policyman_get_ue_mode(pItem, &(tm_core_device_config.tm_cm_subs_ue_mode[sub_id]))))
      {
          MSG_HIGH("tm_cm_update_device_mode_info: policyman tm_cm_subs_ue_mode[%d] %d", sub_id, tm_core_device_config.tm_cm_subs_ue_mode[sub_id], 0);
        }
      else
      {
        MSG_ERROR("tm_cm_update_device_mode_info: TM_CM: Failed to retrieve subs_ue_mode from policy manager", 0, 0, 0);
      } 
      } 
    }
    else
    {
      MSG_ERROR(" tm_cm_update_device_mode_info: TM_CM: Failed to retrieve max_active from policy manager", 0, 0, 0);
    }  
    }
    else
    {
    MSG_ERROR(" tm_cm_update_device_mode_info: TM_CM: Failed to retrieve num_sims from policy manager", 0, 0, 0);
    } 
  }
  else
  {
    MSG_ERROR(" tm_cm_update_device_mode_info: TM_CM: Failed to retrieve overall_feature from policy manager", 0, 0, 0);
  } 
  } 
  else
  {
  MSG_ERROR(" tm_cm_update_device_mode_info: TM_CM: Failed to get_items from policy manager", 0, 0, 0);
  } 
    
  if ( NULL != pItem )
  {
  policyman_item_release(pItem);
  }
    
}
#endif /* #ifndef FEATURE_GNSS_SA */

/**===========================================================================
  FUNCTION tm_cm_is_sglte_sub
  
  DESCRIPTION: 
   Check if the sub's feature is SGLTE
    
  DEPENDENCIES:
    None
    
  RETURN VALUE: 
    Boolean
     
    
  SIDE EFFECTS
    None.
    
===========================================================================*/
boolean tm_cm_is_sglte_sub(sys_modem_as_id_e_type sub_id)
{
  if (tm_core_device_config.tm_cm_subs_feature[sub_id] == SYS_SUBS_FEATURE_MODE_SGLTE)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  } 
}


/**===========================================================================
  FUNCTION tm_cm_is_svlte_sub
  
  DESCRIPTION: 
      Check if the sub's feature is SVLTE
      
  DEPENDENCIES:
    None
      
  RETURN VALUE: 
    Boolean
       
      
  SIDE EFFECTS
    None.
      
===========================================================================*/
boolean tm_cm_is_svlte_sub(sys_modem_as_id_e_type sub_id)
{
  if (tm_core_device_config.tm_cm_subs_feature[sub_id] == SYS_SUBS_FEATURE_MODE_SVLTE)
  {
  return TRUE;
  }
  else
  {
  return FALSE;
  } 
}

/**===========================================================================
  FUNCTION tm_cm_is_srlte_sub
  
  DESCRIPTION: 
      Check if the sub's feature is SRLTE
      
  DEPENDENCIES:
    None
      
  RETURN VALUE: 
    Boolean
       
      
  SIDE EFFECTS
    None.
      
===========================================================================*/
boolean tm_cm_is_srlte_sub(sys_modem_as_id_e_type sub_id)
{
  if (tm_core_device_config.tm_cm_subs_feature[sub_id] == SYS_SUBS_FEATURE_MODE_SRLTE)
  {
  return TRUE;
  }
  else
  {
  return FALSE;
  } 
}

/**===========================================================================
  FUNCTION tm_cm_is_multimode_sub
  
  DESCRIPTION: 
      Check if the sub's feature is multimode
      
  DEPENDENCIES:
    None
      
  RETURN VALUE: 
    Boolean. TRUE if MultiMode 
         FALSE if not MultiMode 
       
      
  SIDE EFFECTS
    None.
      
===========================================================================*/
boolean tm_cm_is_multimode_sub(sys_modem_as_id_e_type sub_id)
{
  #ifndef FEATURE_JOLOKIA_MODEM /* FR23392/21433 is not supported on this modem */
  if (tm_core_device_config.tm_cm_subs_feature[sub_id] == SYS_SUBS_FEATURE_MODE_DUAL_MULTIMODE)
  {
  return TRUE;
  }
  else
  #endif /* ! FEATURE_JOLOKIA_MODEM */
  {
  return FALSE;
  } 
}


/**===========================================================================
  FUNCTION tm_cm_get_sglte_sub
  
  DESCRIPTION: 
       Return the SVLTE sub 
      
  DEPENDENCIES:
    None
      
  RETURN VALUE: 
    sys_modem_as_id_e_type
       
      
  SIDE EFFECTS
    None.
      
===========================================================================*/

sys_modem_as_id_e_type tm_cm_get_sglte_sub(void)
{
  sys_modem_as_id_e_type sglte_sub = SYS_MODEM_AS_ID_NONE;
  uint8 i = 0;
  boolean sglte_sub_found = FALSE;

  for(i = 0; i < tm_core_device_config.no_of_sims; i++)
  {
    if (tm_cm_is_sglte_sub((sys_modem_as_id_e_type)i))
    {
      sglte_sub = (sys_modem_as_id_e_type)i;
    sglte_sub_found = TRUE;
    break;
    }
  }

  if (!sglte_sub_found)
  {
    MSG_ERROR("SGLTE sub was not found, even thought we are getting external cell updates", 0, 0, 0);
  }

  return sglte_sub;
    
}

  
/**===========================================================================
FUNCTION tm_cm_get_device_mode
  
DESCRIPTION: 
 Get device mode. Combination number of SIMs and active subscriptions 
 determine the device configuration mode.
  
DEPENDENCIES:
  None
  
RETURN VALUE: 
    TM_CORE_DEVICE_MODE_NA           = 0,  // Invalid. Could not retrieve device info from Policy manager 
    TM_CORE_DEVICE_MODE_SS          = 1,  // Single SIM mode 
    TM_CORE_DEVICE_MODE_DSDS          = 2,  // Dual SIM, Dual Standby. Only one SUB supports VOICE and DATA 
    TM_CORE_DEVICE_MODE_DSDA_VOICE      = 3, // Dual SIM, Dual Active. Both SUBs support VOICE simultaneously 
    TM_CORE_DEVICE_MODE_DSDA_VOICE_DATA  = 4,  // Dual SIM, Dual Active. Both SUBs support VOICE and DATA simultaneously 
    TM_CORE_DEVICE_MODE_TSTS           = 5,  // Triple SIM, Triple Standby. Only one SUB support VOICE and DATA 
 
  
SIDE EFFECTS
  None.
  
===========================================================================*/
tm_core_device_mode_e_type tm_cm_get_device_mode(void)
{
  return tm_core_device_config.tm_cm_device_info;
}
  
/**===========================================================================
FUNCTION tm_cm_get_subs_ue_mode

DESCRIPTION: 
 Get overall device mode. 
  
DEPENDENCIES:
  None
  
RETURN VALUE: 
  sys_ue_mode_e_type   
  
SIDE EFFECTS
  None.
  
===========================================================================*/
sys_ue_mode_e_type tm_cm_get_subs_ue_mode(sys_modem_as_id_e_type dd_sub_id)
{
  return tm_core_device_config.tm_cm_subs_ue_mode[dd_sub_id];
}

/**===========================================================================
FUNCTION tm_cm_get_subs_feature

DESCRIPTION: 
 Get overall device mode. 
  
DEPENDENCIES:
  None
  
RETURN VALUE: 
  sys_ue_mode_e_type   
  
SIDE EFFECTS
  None.
  
===========================================================================*/
sys_subs_feature_t tm_cm_get_subs_feature(sys_modem_as_id_e_type dd_sub_id)
{
  return tm_core_device_config.tm_cm_subs_feature[dd_sub_id];
}

  
/*===========================================================================

FUNCTION tm_cm_is_cell_info_update_needed

DESCRIPTION 
  Check whether or not cell info update should be sent to GM/AGNSS modules
  for a particular stack

DEPENDENCIES
  

RETURN VALUE
  boolean TRUE for YES, FALSE for No

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean tm_cm_is_cell_info_update_needed(const tm_cm_mm_ss_info_s_type *const ss_info_ptr, uint8 stack)
{
  boolean bUpdateCID = FALSE;
  boolean cdmaStackFound = FALSE;
  boolean lteStackFound = FALSE;
  sys_sys_mode_e_type  sys_mode = SYS_SYS_MODE_NO_SRV;
  sys_sys_mode_e_type  cdma_stack_sys_mode = SYS_SYS_MODE_NO_SRV;
  sys_sys_mode_e_type  lte_stack_sys_mode  = SYS_SYS_MODE_NO_SRV;
  uint8 idx          = 0;
  uint8 lteStack     = 0;
  uint8 cdmaStack    = 0;
  

  /* We will send Cell ID update to GM/AGNSS for DD sub and Preferred Stack only
     since the TM API only allow one RAT/stack */
  if (ss_info_ptr != NULL && tm_cm_dsds_is_dd_subs(ss_info_ptr->asubs_id))
  {
    sys_subs_feature_t  dd_feature_mode = tm_cm_get_subs_feature(ss_info_ptr->asubs_id);

    MSG_MED( "tm_cm_is_cell_info_update_needed: CID dd_feature=%d subsid=%d stack=%d", 
             (uint32)dd_feature_mode, (uint32)ss_info_ptr->asubs_id, stack);

    switch (dd_feature_mode)
    {
      case SYS_SUBS_FEATURE_MODE_SGLTE:  
	  {
        if (ss_info_ptr->multisim_stack_info[stack].sys_mode != SYS_SYS_MODE_GSM)
        {
          /* We dont anticipate another GSM RAT on the same sub, if there is a new variant other than SGLTE,
                      then this check need to be revisited*/
          bUpdateCID = TRUE; // only update non-GSM CID
        }        
      }
	  break;

      case SYS_SUBS_FEATURE_MODE_SVLTE: 
      case SYS_SUBS_FEATURE_MODE_SRLTE:
        case SYS_SUBS_FEATURE_MODE_DUAL_MULTIMODE:
	  {
		for (idx = 0; idx < TM_CM_MAX_NO_STACKS; idx++)
        {
          if ((tm_cm_info.ss_info[ss_info_ptr->asubs_id][idx].is_operational == TRUE)||
              (tm_cm_info.ss_info[ss_info_ptr->asubs_id][idx].oper_to_non_oper_transition == TRUE))
          { 
            if (tm_cm_info.ss_info[ss_info_ptr->asubs_id][idx].sys_mode == SYS_SYS_MODE_LTE)
            {
              lteStack = idx;
              lteStackFound = TRUE;
            }
      		else if (tm_cm_info.ss_info[ss_info_ptr->asubs_id][idx].sys_mode == SYS_SYS_MODE_CDMA)
            {
              cdmaStack = idx;
              cdmaStackFound = TRUE;
            }
          }
        }

        cdma_stack_sys_mode = tm_cm_info.ss_info[ss_info_ptr->asubs_id][cdmaStack].sys_mode;
        lte_stack_sys_mode  = tm_cm_info.ss_info[ss_info_ptr->asubs_id][lteStack].sys_mode;
      
        if (cdmaStackFound && lteStackFound)
        {
          /*Camped on CDMA and LTE*/
		  MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_is_cell_info_update_needed:Camped on CDMA and LTE stack");
          sys_mode = tm_cm_get_svlte_sys_mode(cdma_stack_sys_mode, lte_stack_sys_mode);
		  if (ss_info_ptr->multisim_stack_info[stack].cell_info.as_sys_mode == sys_mode)
		  {
		    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_is_cell_info_update_needed:Handled Cell ID update for RAT = %d", sys_mode);
		    bUpdateCID = TRUE;
		  }
		  else
		  {
		    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_is_cell_info_update_needed:Dropped Cell ID update for RAT = %d", sys_mode);
			bUpdateCID = FALSE;
		  }
	    }
        else if (cdmaStackFound)
        {
          /*Only cdma stack camped*/
          MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_is_cell_info_update_needed:Camped only on CDMA stack");
          bUpdateCID = TRUE;
        }
        else if (lteStackFound)
        {
          /*Only LTE stack camped*/
          MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_is_cell_info_update_needed:Camped only on LTE stack");
          bUpdateCID = TRUE;
	    }
        else 
        {
          /* We have only 1 stack */
          bUpdateCID = TRUE;
	    }
      }      
	  break;

      case SYS_SUBS_FEATURE_MODE_NORMAL:  /* subs configured for normal mode, only 1 stack */
	  {
        bUpdateCID = TRUE;
      } 
	  break;

      default:
	  {
	    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_cm_is_cell_info_update_needed: Unsupported Feature mode %u", dd_feature_mode);
        bUpdateCID = FALSE;
      }
	  break;
    } 

    MSG_MED( "CID Update=%d cm_mode=%d tm_mode=%d", 
             (uint32)bUpdateCID, 
             (uint32)ss_info_ptr->multisim_stack_info[stack].sys_mode,
             (uint32)tm_cm_info.sys_mode);
  }

  return bUpdateCID;
}


/*===========================================================================

FUNCTION tm_cm_convert_plmn_id_format

DESCRIPTION
  This function converts from the NAS PLMN ID format (2 BCD digits per byte)
  to the RRC PLMN_id_T format (1 BCD digit per byte).

DEPENDENCIES
  None

RETURN VALUE
  PLMN ID converted to rrc_plmn_id_type format

SIDE EFFECTS
  None

===========================================================================*/

static void tm_cm_convert_plmn_id_format( sys_plmn_id_s_type plmn_id, uint8 mcc[3], uint8 mnc[3], uint8 *num_mnc_digits)
{
   /* ----------------------------------------
   ** PLMN octet 1 = MCC digit 2 | MCC digit 1
   ** ---------------------------------------- */
   mcc[0] = plmn_id.identity[0] & 0x0F;
   mcc[1] = (plmn_id.identity[0] & 0xF0) >> 4;

   /* ----------------------------------------
   ** PLMN octet 2 = MNC digit 3 | MCC digit 3
   ** ---------------------------------------- */
   mcc[2] = plmn_id.identity[1] & 0x0F;

   if ((plmn_id.identity[1] & 0xF0) == 0xF0)
   {
      *num_mnc_digits = 2;
      mnc[2] = 0x0F; /* Set the 3rd digit to 0xF as per the spec */
   }
   else
   {
      *num_mnc_digits = 3;
      mnc[2] = (plmn_id.identity[1] & 0xF0) >> 4;
   }

   /* ----------------------------------------
   ** PLMN octet 3 = MNC digit 2 | MNC digit 1
   ** ---------------------------------------- */
   mnc[0] = plmn_id.identity[2] & 0x0F;
   mnc[1] = (plmn_id.identity[2] & 0xF0) >> 4;

} /* end tm_cm_convert_plmn_id_format() */
  
  
/*===========================================================================

FUNCTION tm_cm_process_cell_info_update

DESCRIPTION 
  Maps a CM Cell Info sys_cell_info_s_type data type to the coresponding
  cgps_CellInfoCachedStruct data

DEPENDENCIES
  

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void tm_cm_process_cell_info_update( const tm_cm_mm_msim_ss_stack_info_s_type *const cp_ss_info_ptr)
{
  cgps_CellInfoCachedStruct  cgps_cell_info;
  uint32 mcc, mnc;
  sys_cell_info_s_type  *cp_cell_info_ptr = NULL;

  if (cp_ss_info_ptr == NULL)
  {
    return;
  }

  cp_cell_info_ptr = (sys_cell_info_s_type *)&cp_ss_info_ptr->cell_info;
  memset(&cgps_cell_info, 0x00, sizeof(cgps_cell_info));

  mcc = ( ((uint32)cp_cell_info_ptr->plmn_id.identity[0] & 0x0F) << 8 ) |
          ((uint32)cp_cell_info_ptr->plmn_id.identity[0] & 0xF0) |
          ((uint32)cp_cell_info_ptr->plmn_id.identity[1] & 0x0F);
  mnc = ( ((uint32)cp_cell_info_ptr->plmn_id.identity[2] & 0x0F) << 8 ) |
          ((uint32)cp_cell_info_ptr->plmn_id.identity[2] & 0xF0) |
        ( ((uint32)cp_cell_info_ptr->plmn_id.identity[1] & 0xF0) >> 4 );
  if ((mnc & 0x0F) == 0x0F)
  {
    mnc >>= 4;  // if last one is 0x0F, then it's two-digit only, so right-shift 4 bits 
  }

  MSG_MED( "CM_SS_CELL_INFO_MASK set by CM, CID=%d, PLMN(MCC=%x MNC=%x)",
           (uint32)cp_cell_info_ptr->cell_id, mcc, mnc);

  cgps_cell_info.u_CachedCellIDValid = TRUE;
  cgps_cell_info.q_NumCellChangesCachedCellID = 1;

  switch (cp_cell_info_ptr->as_sys_mode) // serving system type
  {
  case SYS_SYS_MODE_CDMA: /* 2, CDMA mode */
  case SYS_SYS_MODE_HDR: /* 4, HDR Mode */
    cgps_cell_info.e_ActiveRAT = CELLDB_ACTIVERAT_CDMA;
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoCDMAGlobal.w_SID = cp_ss_info_ptr->sys_id_id_is95_sid;
    /* // can add NID/BSID to CDMA CellInfo
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoCDMAGlobal.w_NID = cp_ss_info_ptr->sys_id_id_is95_nid; 
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoCDMAGlobal.w_BSID = (uint16)cp_ss_info_ptr->mode_info_cdma_info_base_id; 
    */
    MSG_MED( "CDMA CID Update: SID=%d NID=%d BSID=%d", 
             cp_ss_info_ptr->sys_id_id_is95_sid,
             cp_ss_info_ptr->sys_id_id_is95_nid, 
             (uint16)cp_ss_info_ptr->mode_info_cdma_info_base_id);
    break;

#if defined(FEATURE_CGPS_UMTS_CELLDB) || defined (FEATURE_FSM_V2)
  case SYS_SYS_MODE_GSM: /* 3, GSM mode */
    cgps_cell_info.e_ActiveRAT = CELLDB_ACTIVERAT_GSM;
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.camped_on_flag = TRUE;
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.cell_identity = cp_cell_info_ptr->cell_id;
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.location_area_identification.plmn_id = cp_cell_info_ptr->plmn_id;
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.location_area_identification.location_area_code = cp_cell_info_ptr->lac_id;
    /**< Absolute Radio Frequency Channel Number (ARFCN) of the serving system
       (applicable only in GSM mode). Range is 0 to 1023. */
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.arfcn.num = cp_cell_info_ptr->arfcn;
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.arfcn.band = 0; // no band info from CM
    /**< Base Station Identity Code of the serving system (applicable only in GSM mode). 
      Range is 0 to 63 (see 3GPP TS 24.008, section 10.5.1.3). */
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.bsic = cp_cell_info_ptr->bsic;
    MSG_MED( "GSM CID Update: LAC=%d ARCFN=%d BSIC=%d", cp_cell_info_ptr->lac_id,
             cp_cell_info_ptr->arfcn, cp_cell_info_ptr->bsic);
    break;

  case SYS_SYS_MODE_WCDMA: /* 5, WCDMA mode */
    cgps_cell_info.e_ActiveRAT = CELLDB_ACTIVERAT_WCDMA;
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.cell_id = (rrc_int_cell_id_type)cp_cell_info_ptr->cell_id;
    /**< The serving Primary Scrambling Code. Applicable for WCDMA systems */
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.psc = cp_cell_info_ptr->psc; 
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.freq = cp_cell_info_ptr->uarfcn_dl;;

    /* Obtain the LAC information of the system. */
    /* Pack the lac_id obtained from CM ( word ) into the byte array for TM */
    b_packw(cp_cell_info_ptr->lac_id, cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.lac, 0, 16);

    /* Obtain the plmn_id information of the system. */
    /* Refer to the sys_plmn_id_s_type to find the format of the plmn_id */
    /** Public Land Mobile Network (PLMN) ID.

    A PLMN ID is defined within a GSM network to be the combination of a Mobile
    Country Code (MCC) and Mobile Network Code (MNC). A PLMN ID is stored in
    three octets, as specified in 3G TS 24.008, Section 10.5.1.3 and shown below:
                                        Bit
                 |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |
                 |===============================================|
         octet 1 |      MCC Digit 2      |      MCC Digit 1      |
                 |-----------------------------------------------|
         octet 2 |      MNC Digit 3      |      MCC Digit 3      |
                 |-----------------------------------------------|
         octet 3 |      MNC Digit 2      |      MNC Digit 1      |
                 |===============================================|
    [ Digit range is 0..9 ]
    */
    /* Convert the MCC/MNC information received in sys_plmn_id_s_type from CM into rrc_plmn_identity_type used by TM */
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoWCDMAGlobal.plmn_id = 
      mmumts_convert_nas_plmn_id_to_rrc_plmn_id(cp_cell_info_ptr->plmn_id);
    MSG_MED( "WCDMA CID Update: LAC=%d Freq=%d PSC=%d", cp_cell_info_ptr->lac_id,
             cp_cell_info_ptr->uarfcn_dl, cp_cell_info_ptr->psc);
    MSG_MED("WCDMA NumChange=%d", cgps_cell_info.q_NumCellChangesCachedCellID, 0, 0);
    break;

  case SYS_SYS_MODE_TDS: /* 11 TD-SCDMA mode */
    cgps_cell_info.e_ActiveRAT = CELLDB_ACTIVERAT_TDSCDMA;
    /* Global Cell ID */
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoTDSCDMAGlobal.cell_id = cp_cell_info_ptr->cell_id;
    /* Physical Cell ID (0~127 for TDSCDMA?) FIXME: need to double-check, CM is saying TDS's psc is physical cell ID */
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoTDSCDMAGlobal.cpid = (uint8)cp_cell_info_ptr->psc;
    /**< Downlink the UMTS Terrestrial Radio Access (UTRA) Absolute Radio
         Frequency Channel Number (UARFCN) of the serving system. */
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoTDSCDMAGlobal.freq = cp_cell_info_ptr->uarfcn_dl;
    tm_cm_convert_plmn_id_format(cp_cell_info_ptr->plmn_id,
         cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoTDSCDMAGlobal.plmn_id.mcc,
         cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoTDSCDMAGlobal.plmn_id.mnc,
         &cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoTDSCDMAGlobal.plmn_id.num_mnc_digits);
    /* Obtain the LAC information of the system. */
    /* Pack the lac_id obtained from CM ( word ) into the byte array for TM */
    b_packw(cp_cell_info_ptr->lac_id, cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoTDSCDMAGlobal.lac, 0, 16);
    MSG_MED( "TDSCDMA CID Update: LAC=%d Freq=%d PSC=%d", cp_cell_info_ptr->lac_id,
             cp_cell_info_ptr->uarfcn_dl, cp_cell_info_ptr->psc);
    break;
#endif /* if defined(FEATURE_CGPS_UMTS_CELLDB) || defined (FEATURE_FSM_V2) */

#ifdef FEATURE_CGPS_LTE_CELLDB
  case SYS_SYS_MODE_LTE: /* 9, LTE mode */
    cgps_cell_info.e_ActiveRAT = CELLDB_ACTIVERAT_LTE;
    tm_cm_convert_plmn_id_format(cp_cell_info_ptr->plmn_id,
         cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.mcc,
         cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.mnc,
         &cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.num_mnc_digits);
    /**< Downlink the Evolved-UTRA ARFCN (EARFCN) of the serving system. 
      Applicable for LTE systems. (DL Center frequency of cell) */
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.freq = cp_cell_info_ptr->earfcn_dl;
    /* Physical cell id of cell  CM is saving LTE physical cell ID in psc after CR 622382 */
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.cell_id = cp_cell_info_ptr->psc; 
    /* Global cell id (from SIB1) of cell */
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.cell_identity = cp_cell_info_ptr->cell_id;
    /**< TAC(Tracking Area Code) information for LTE. */
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.tac = cp_cell_info_ptr->tac;
    MSG_MED( "LTE CID Update: TAC=%d Freq=%d PSC=%d", cp_cell_info_ptr->tac,
             cp_cell_info_ptr->earfcn_dl, cp_cell_info_ptr->psc);
    break;
#endif /* FEATURE_CGPS_LTE_CELLDB */

  case SYS_SYS_MODE_GW: /* 7, GSM and WCDMA mode */  // FIXME: should we leave GW here?
  case SYS_SYS_MODE_GWL: /* 10, GSM/WCDMA/LTE mode */ 
  case SYS_SYS_MODE_NO_SRV: /* 0, NO Service */
  default:  /* All modes not covered above */
    MSG_HIGH( "Other CID Update: SS=%d CID=%d", 
              (uint32)cp_ss_info_ptr->sys_mode, cp_cell_info_ptr->cell_id, 0);
    cgps_cell_info.e_ActiveRAT = CELLDB_ACTIVERAT_NONE;
    cp_cell_info_ptr->cell_id = 0xFFFFFFFF; // mark it as invalid
    break;
  }

  /* The cell ID returned is invalid */
  if(cp_cell_info_ptr->cell_id == 0xFFFFFFFF)
  {
    cgps_cell_info.u_CachedCellIDValid = FALSE;
    cgps_cell_info.q_NumCellChangesCachedCellID = 0xFFFFFFFF; /* Set to a maximum */
  }

  /* Send Cell ID update IPC TM_CORE_MSG_ID_MC_CELLDB_UPDATE to TM task
     TM task's tm_core_cell_db_proc() function will then send to SUPL, LPP CP, GM, etc */
  tm_api_update_cell_info(cgps_cell_info.e_ActiveRAT, (const cgps_CellInfoCachedStruct *)&cgps_cell_info);
  
}

/*===========================================================================
FUNCTION tm_cm_is_phone_in_e911_state

DESCRIPTION
  This function returns TRUE if phone is in E911 state (ORIG or CB)

DEPENDENCIES

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_is_phone_in_e911_state(void)
{
  if (tm_cm_info.e911_state == TM_CM_E911_STATE_NONE)
  {
    return FALSE;
  }

  /* could be in TM_CM_E911_STATE_ORIG or TM_CM_E911_STATE_CB mode */
  return TRUE;
}

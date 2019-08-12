/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TM-Core / Protocol sub-module Interface

GENERAL DESCRIPTION
  This file implements TM-Core / protocol sub-module interface

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015, 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_prtl_iface.c#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/05/16   mc      Dynamically allocate variables for huge structures to reduce stack usage
10/01/15   rk      Define new internal API for protocol to know if A_GNSS position 
                   methods are supported. 
03/14/15   rh      Added support for MultiSIM NV  
08/02/14   gk      do not send another session begin if already sent for early rcvr session when NI is started. 
06/23/14   gk      OVerwrite the early rcvr session with a new CP session if the session starts after 911 end
11/13/13   rk      Missing initialization of pd_event_type causes sending garbage data to LBS.
12/13/11   gk      LPP related fixes
07/27/11   rh      Configurably accept 1X MT CP even if MO session is active
12/2/09    gk      intermediate position fixes
07/22/09   atien   Add tm_post_ext_refpos_rep_event
07/14/09   atien   tm_sess_req() - save client id when starting MT session
04/25/08   gk      On-Demand changes
08/10/07   gk      check if the E911 call is in orig state to accept MT-LR
05/27/07   rw      1x CP can end 1x UP during KDDI protocol
12/07/06   cl      Added checks for start session request from protocl modules
06/15/06   CL      Initial version

============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"     /* Definition for basic types and macros */
#include "customer.h"   /* Customer configuration file */
#include "msg.h"
#include "tm_data.h"
#include "tm_common.h"
#include "tm_pdapi_client.h"
#include "tm_prtl_iface.h"
#include "tm_cm_iface.h"
#include "time_svc.h"

/*===========================================================================

FUNCTION TM_PRTL_REG

DESCRIPTION
  This function is called by protocol sub-moudles to inform TM-Core sub-module 
  that it is ready to serve.

  It should provide a cb table with 5 function callbacks to TM-Core, 
  so TM-Core knows how to communicate with it.
  
DEPENDENCIES

RETURN VALUE
  TURE: If TM-Core sub-module allows registration
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
boolean tm_prtl_reg(
                     tm_prtl_type             prtl_type,
                     const tm_prtl_cb_s_type  *cb_tbl
                   )
{
  int prtlFuncTblIndex;

  if(cb_tbl == NULL)
  {
    MSG_ERROR("cb_tbl is NULL", 0, 0, 0);
    return FALSE;  
  }
  
  /* Find the correct array index for the prtl_type */
  prtlFuncTblIndex = tm_core_get_prtl_idx_by_type(prtl_type);

  if((prtlFuncTblIndex < 0) || (prtlFuncTblIndex >= TM_PRTL_NUM))
  {
    MSG_ERROR("Can not find a index for protocl type %d", prtl_type, 0, 0);
    return FALSE;
  } 
  
  /* Setup protocol callback table */  
  tm_core_info.prtl_func_cb_table[prtlFuncTblIndex].start_sess_req_fp   = cb_tbl->start_sess_req_fp;
  tm_core_info.prtl_func_cb_table[prtlFuncTblIndex].stop_sess_req_fp    = cb_tbl->stop_sess_req_fp;
  tm_core_info.prtl_func_cb_table[prtlFuncTblIndex].sess_req_data_fp    = cb_tbl->sess_req_data_fp;
  tm_core_info.prtl_func_cb_table[prtlFuncTblIndex].sess_info_fp        = cb_tbl->sess_info_fp;
  #ifdef FEATURE_CGPS_PDCOMM 
  tm_core_info.prtl_func_cb_table[prtlFuncTblIndex].data_pipe_handle_fp = cb_tbl->data_pipe_handle_fp; //gk:QST1500
  #endif
  tm_core_info.prtl_func_cb_table[prtlFuncTblIndex].timer_cb_fp         = cb_tbl->timer_cb_fp;
  tm_core_info.prtl_func_cb_table[prtlFuncTblIndex].event_cb_fp         = cb_tbl->event_cb_fp;
  tm_core_info.prtl_func_cb_table[prtlFuncTblIndex].general_query_fp    = cb_tbl->general_query_fp;
  tm_core_info.prtl_func_cb_table[prtlFuncTblIndex].gm_event_handle_fp  = cb_tbl->gm_event_handle_fp;
  return TRUE; /*lint !e506 */
}

/*===========================================================================

FUNCTION tm_sess_convert_op_mode

DESCRIPTION
  This function converts internal op mode to pdapi op mode
  
DEPENDENCIES

RETURN VALUE
  pdsm_pd_session_operation_e_type

SIDE EFFECTS

===========================================================================*/
pdsm_pd_session_operation_e_type tm_sess_convert_op_mode
(
  tm_sess_operation_mode_e_type tm_op_mode
)
{
  switch(tm_op_mode)
  {
    case TM_SESS_OPERATION_MODE_MSA:
    case TM_SESS_OPERATION_MODE_OTDOA:
      return PDSM_SESSION_OPERATION_MSASSISTED;

    case TM_SESS_OPERATION_MODE_MSB:
      return PDSM_SESSION_OPERATION_MSBASED;

    case TM_SESS_OPERATION_MODE_STANDALONE:
      return PDSM_SESSION_OPERATION_STANDALONE_ONLY;

    default:
      return PDSM_SESSION_OPERATION_MIN;
  }
}


/*===========================================================================

FUNCTION tm_sess_req

DESCRIPTION
  This function is called by protocol sub-moudles to start, stop a session.
  It is also called by protocol sub-modules to request aiding data and get 
  the status (validity) of aiding data.
  
DEPENDENCIES

RETURN VALUE
  TURE: If TM-Core sub-module accepts the request
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
boolean tm_sess_req(
                     tm_prtl_type                    prtl_type,
                     tm_sess_handle_type             sess_handle,
                     tm_sess_req_type                req_type, 
                     const tm_sess_req_param_u_type  *req_param
                   )
{
  boolean retVal                 = FALSE;
  boolean mt_allowed             = TRUE;  
  tm_cm_phone_state_info_s_type  phone_state_info;
  pdsm_pd_option_s_type          mt_pd_option;
  pdsm_pd_qos_type               mt_pd_qos;

  /* Init */
  memset(&phone_state_info, 0, sizeof(phone_state_info));
  memset(&mt_pd_option, 0, sizeof(mt_pd_option));
  memset(&mt_pd_qos, 0, sizeof(mt_pd_qos));

  if(req_param == NULL)
  {
    MSG_ERROR("NULL req_param", 0, 0, 0);
    return retVal;
  }
  
  switch(req_type)
  {
    case TM_SESS_REQ_START:
      
      tm_cm_iface_get_phone_state(&phone_state_info);    

      /* First check if an incoming MT request should be allowed */
      switch(phone_state_info.e911_state)
      { 
        case TM_CM_E911_STATE_ORIG:
        case TM_CM_E911_STATE_CB:
        {
          /* If the phone is in E911 CB state, always allow MT request */          
          mt_allowed = TRUE; /*lint !e506 */
        }
        break;

        case TM_CM_E911_STATE_NONE:
        {
          /* 
             If the phone is NOT in E911 CB state, only allow MT request when 
             1. Phone is idle
             2. GPS lock allows it
             3. Current Session is internal session
          */          
          if( (TM_CORE_CONFIG_DD(gps_lock) == PDSM_GPS_LOCK_MT) 
            ||(TM_CORE_CONFIG_DD(gps_lock) == PDSM_GPS_LOCK_ALL))
          {
            MSG_ERROR("MT GPS is locked", 0, 0, 0);
            mt_allowed = FALSE;;  
          }          
         if(mt_allowed && tm_core_session_active())  // if MT GPS is locked, don't bother to continue checking
          {
            if (tm_core_is_internal_session_on() == TRUE)
            {
              MSG_LOW("Got MT Request while there is an internal session", 0, 0, 0); // allow it
            }
            else // There is an active non-internal session
            {
              /* session handle 0x1~0xF0 for MO/MI session(will get terminated), following MT/NI will NOT get terminated:
               TM_SESS_HANDLE_1X_CP_MT   (0x0100)  TM_SESS_HANDLE_1X_UP_NI   (0x0200) 
               TM_SESS_HANDLE_UMTS_CP_NI (0x0400)  TM_SESS_HANDLE_UMTS_UP_NI (0x0800)
              */
              if (tm_core_info.config_info.accept_1xcp_mt_over_mo &&  // accept 1X CP MT even if MO session is active
                  sess_handle == TM_SESS_HANDLE_1X_CP_MT &&   // REQ_START new 1X CP MT session
                  tm_core_info.session_info.active_sess_handle != TM_SESS_HANDLE_1X_CP_MT && 
                  tm_core_info.session_info.active_sess_handle != TM_SESS_HANDLE_1X_UP_NI && 
                  tm_core_info.session_info.active_sess_handle != TM_SESS_HANDLE_UMTS_CP_NI &&
                  tm_core_info.session_info.active_sess_handle != TM_SESS_HANDLE_UMTS_UP_NI)
              {
                 MSG_HIGH("Accept 1X CP MT and terminate current session (handle=%d)", 
                          tm_core_info.session_info.active_sess_handle, 0, 0);
                 /* terminate current session, USER_TERMINATED is safe since ask_prtl=TRUE, all protocols will
                    end session if stop_reason is USER_TERMINATED */
                 tm_core_abort_recover(TRUE, TM_STOP_REASON_USER_TERMINATED);
              }
              else
              {
                /* emergnecy session started already by TM at start of emergency call, now accept 
                   the CP session and update the session handle */
                if(((prtl_type == TM_PRTL_TYPE_LPP_CP) ||
                    (prtl_type == TM_PRTL_TYPE_UMTS_UP) ||
                       (prtl_type == TM_PRTL_TYPE_1X_CP) ||
                       (prtl_type == TM_PRTL_TYPE_UMTS_CP)) &&
                      tm_core_info.session_info.active_sess_handle != TM_SESS_HANDLE_E911)
                {
                  /* Abort the existing session before starting a new E911 session */
                  MSG_HIGH("Accept LPP CP session and terminate current TM session (handle=%d)", 
                           tm_core_info.session_info.active_sess_handle, 0, 0);
                  tm_core_abort_recover(TRUE, TM_STOP_REASON_E911);
                }

                else if(((prtl_type == TM_PRTL_TYPE_LPP_CP) ||
                    (prtl_type == TM_PRTL_TYPE_1X_CP) ||
                    (prtl_type == TM_PRTL_TYPE_UMTS_UP) ||
                    (prtl_type == TM_PRTL_TYPE_UMTS_CP)) &&
                   tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_E911)
                {
                  
                  /* Overwrite the existing session with the new session */
                  MSG_HIGH("Accept CP/UP session from (prtl= %d) even if early rcvr ongoing (handle=%d)", 
                           prtl_type, tm_core_info.session_info.active_sess_handle, 0);
                }
                else
                {
                  MSG_ERROR("Rejecting MT Request(handle=%d mtflag=%d) during active session(handle=%d)", 
                            sess_handle, tm_core_info.config_info.accept_1xcp_mt_over_mo,
                            tm_core_info.session_info.active_sess_handle); 
                  mt_allowed = FALSE;
                }
              }
            }
          }

        }
        break;        

        default:
          /* 
             Only possibility to enter here is the phone is in E911 Orig state.
             In this state, E911 call is not connected, so if for any reason, a MT comes in, 
             it should not belong to the E911. We should not allow it
          */          
          mt_allowed = FALSE;
        break;
      }

      if(mt_allowed)
      {
        if(req_param->start_param.op_req != TM_OP_REQ_DEL_ASSIST)
        {
          /* Fire Begin event */
          tm_pdapi_pd_event_s_type		 *pz_pd_event_type;

          pz_pd_event_type = (tm_pdapi_pd_event_s_type *)os_MemAlloc(sizeof(tm_pdapi_pd_event_s_type), OS_MEM_SCOPE_TASK);
          if (NULL == pz_pd_event_type)
          {
            MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Cannot allocate memory for tm_pdapi_pd_event_s_type in tm_sess_req");
          }

          mt_pd_option.operation_mode = tm_sess_convert_op_mode(req_param->start_param.op_mode);
          mt_pd_option.session        = PDSM_PD_SESS_TYPE_NEW;
          mt_pd_option.fix_rate.num_fixes = 1;
          mt_pd_option.fix_rate.time_between_fixes_ms = 0;
          mt_pd_qos.gps_session_timeout = req_param->start_param.prm_prq;
          mt_pd_qos.accuracy_threshold  = req_param->start_param.accuracy_threshold;

          if (NULL != pz_pd_event_type)
          {
            memset(pz_pd_event_type, 0, sizeof(*pz_pd_event_type));
            pz_pd_event_type->e_pd_event = PDSM_PD_EVENT_BEGIN;
            pz_pd_event_type->pd_event_data.pd_info_type.client_id = pdsm_ni_client_id_get();
            pz_pd_event_type->pd_event_data.pd_get_pos_data.option_ptr = &mt_pd_option;
            pz_pd_event_type->pd_event_data.pd_get_pos_data.qos_ptr    = &mt_pd_qos;

        if (sess_handle == TM_SESS_HANDLE_ON_DEMAND)
        {
          tm_pdapi_pd_event_callback(pz_pd_event_type, TM_PDAPI_SESS_TYPE_ON_DEMAND);
        }
        else
        {
          if(tm_core_info.session_info.active_sess_handle == TM_SESS_HANDLE_NONE)
          {
            tm_pdapi_pd_event_callback(pz_pd_event_type, TM_PDAPI_SESS_TYPE_MT);
          }
        }
        (void)os_MemFree((void **)&pz_pd_event_type);
       }
        }
        else
        {
          tm_core_info.session_info.session_event_firing = FALSE;
        }
        tm_core_info.session_info.pd_param.client_id = pdsm_ni_client_id_get();
        retVal = tm_core_sess_req_start(prtl_type, sess_handle, req_param->start_param);
      }
      else
      {
        retVal = FALSE;
      }
    break;

    case TM_SESS_REQ_CONTINUE:
      retVal = tm_core_sess_req_continue(prtl_type, sess_handle, req_param->continue_param);
    break;

    case TM_SESS_REQ_POLLING_DATA:
      retVal = tm_core_sess_req_polling_data(prtl_type, sess_handle, req_param->polling_data_param);
    break;

    case TM_SESS_REQ_STOP:
      retVal = tm_core_sess_req_stop(prtl_type, sess_handle, req_param->stop_param);
    break;

    default:
      retVal = FALSE;
    break;
  }

  return retVal;
}

/*===========================================================================

FUNCTION tm_post_data

DESCRIPTION
  This function is called by protocol sub-modules to send data back to 
  TM.
  
DEPENDENCIES

RETURN VALUE
  TURE: If TM-Core sub-module accepts it
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
boolean tm_post_data(
                      tm_prtl_type                     prtl_type,
                      tm_sess_handle_type              sess_handle,
                      tm_post_data_e_type              data_type,
                      const tm_post_data_payload_type  *data_payload_ptr
                    )
{
  /* Process data which protocol sub-module provides */
  return tm_core_recv_data(prtl_type, sess_handle, data_type, data_payload_ptr);
}

/*===========================================================================

FUNCTION tm_post_pd_event

DESCRIPTION
  This function is called by protocol sub-modules to send PD API events back 
  to applications.
  
DEPENDENCIES

RETURN VALUE
  TRUE: If TM-Core sub-module accepts it
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
boolean tm_post_pd_event(
                          tm_prtl_type                prtl_type,
                          tm_sess_handle_type         sess_handle,
                          pdsm_pd_event_type          pd_event, 
                          pdsm_pd_info_s_type*        pd_info_ptr,
                          gnss_NavSlnFlagsStructType* p_PosSrcFlags
                        )
{
  /* Process data which protocol sub-module provides */
  return tm_core_recv_event(prtl_type, sess_handle, pd_event, pd_info_ptr, p_PosSrcFlags);
}

/*===========================================================================

FUNCTION tm_request_app_response

DESCRIPTION
  This function is called by protocol sub-modules to request application response
  Ex:
  1x V1/V2, accept NI request.
  1x V1/V2, NI/MO service interaction, choose MO or NI
  
DEPENDENCIES

RETURN VALUE
  TURE: If TM-Core sub-module accepts it
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
boolean tm_request_app_response(
                                 tm_prtl_type                prtl_type,
                                 pdsm_lcs_event_type         eventType,
                                 pdsm_lcs_info_s_type        *event_info                                 
                               )
{
  /* Process app request from protocol */
  (void)prtl_type;
  /* set the flag that we are waiting for the user response */
  tm_core_set_ni_resp_pending(TRUE);
  /* send the notificiation to the AP */
  tm_pdapi_lcs_event_callback(eventType, event_info);

  return TRUE; /*lint !e506 */
}

/*===========================================================================

FUNCTION tm_post_ext_meas_rep_event

DESCRIPTION
  This function is called by protocol sub-modules to send EXT_POS_STATUS_PD API 
  events back to applications.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_post_ext_meas_rep_event
(
  tm_prtl_type                 prtl_type,
  tm_sess_handle_type          sess_handle,
  pdsm_ext_status_info_s_type *ext_meas_event
)
{
  if(ext_meas_event == NULL)
  {
    MSG_ERROR("Null pointer passed by protocol %d sess_handle %d", 
               prtl_type, sess_handle, 0);
    return FALSE;
  }

  /* Send extra pdapi event */                
  ext_meas_event->client_id       = tm_core_info.session_info.pd_param.client_id;
  #ifdef FEATURE_GNSS_PERIODIC_PPM
  #error code not present
#endif

  tm_pdapi_ext_status_event_callback( 
                 ext_meas_event->ext_status_type,
                 ext_meas_event);


  return TRUE;
}

/*===========================================================================

FUNCTION tm_post_ext_meas_rep_event_deprecated

DESCRIPTION
  This function is called by protocol sub-modules to send EXT_POS_STATUS_PD API events back 
  to applications.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_post_ext_meas_rep_event_deprecated
(
  tm_prtl_type               prtl_type,
  tm_sess_handle_type        sess_handle,
  pdsm_ext_status_event_type pd_ext_status_event, 
  pdsm_srch_ppm_rpt_type     *ppm_info_ptr,
  uint16                     flags
)
{
  pdsm_ext_status_info_s_type    pd_ext_status_info;
  
  (void)prtl_type;
  (void)sess_handle;
  
  /* Send extra pdapi event for PRM */                
  pd_ext_status_info.client_id       = tm_core_info.session_info.pd_param.client_id;

  if(ppm_info_ptr == NULL)
  {
    return FALSE;
  }

  pd_ext_status_info.ext_status_type   = PDSM_EXT_STATUS_MEASUREMENT;

  pd_ext_status_info.ext_status_info.ext_meas_report_type.flags      = flags;
  pd_ext_status_info.ext_status_info.ext_meas_report_type.num_svs    = 0;
  pd_ext_status_info.ext_status_info.ext_meas_report_type.alm_svmask = 0;
  pd_ext_status_info.ext_status_info.ext_meas_report_type.eph_svmask = 0;
  memscpy((void *)&(pd_ext_status_info.ext_status_info.ext_meas_report_type.ppm_data),
          sizeof(pd_ext_status_info.ext_status_info.ext_meas_report_type.ppm_data),
         (void *)ppm_info_ptr, sizeof(*ppm_info_ptr));

  tm_pdapi_ext_status_event_callback( pd_ext_status_info.ext_status_type,
                                     &pd_ext_status_info);

  return TRUE;
}


/*===========================================================================

FUNCTION tm_post_ext_status_ref_loc_rep_event

DESCRIPTION
  This function is called by protocol sub-modules to send 
  PDSM_EXT_STATUS_EVENT_POS_REPORT to applications in order to post a
  reference position received from network.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_post_ext_status_ref_loc_rep_event
(
  tm_prtl_type               prtl_type,
  tm_sess_handle_type        sess_handle,
  pdsm_pd_data_type         *pd_info_buffer_ptr
)
{
  pdsm_ext_status_info_s_type   pd_ext_status_info;

  (void)prtl_type;
  (void)sess_handle;
  
  /* Send extended position pdapi event for REF POSITION */                
  pd_ext_status_info.client_id       = tm_core_info.session_info.pd_param.client_id;
  pd_ext_status_info.ext_status_type = PDSM_EXT_STATUS_POS_REPORT;
  (void)time_get(pd_ext_status_info.time_stamp);  

  pd_ext_status_info.ext_status_info.ext_pos_report_type.fixTimeRemaining = 
    tm_core_info.session_info.q_FixTimeRemainingMSec;

  memscpy((void *)&(pd_ext_status_info.ext_status_info.ext_pos_report_type.pd_data),
          sizeof(pd_ext_status_info.ext_status_info.ext_pos_report_type.pd_data),
         (void *)pd_info_buffer_ptr, sizeof(*pd_info_buffer_ptr));
  /* it will be reported at the next 1hz tick */
  tm_core_save_ref_position(&pd_ext_status_info);

}  /* tm_post_ext_status_ref_loc_rep_event */

/*===========================================================================

FUNCTION tm_post_ext_status_mm_event

DESCRIPTION

  This function is called by protocol sub-modules to send 
  PDSM_EXT_STATUS_EVENT_MM_IND to OEM
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_post_ext_status_generic_event
(
  tm_prtl_type               prtl_type,
  tm_sess_handle_type        sess_handle,
  pdsm_ext_status_info_s_type *p_ext_info_buffer_ptr
)
{
  (void)prtl_type;
  (void)sess_handle;
  
  /* Send extended position pdapi event for MM indication */                
  p_ext_info_buffer_ptr->ext_status_type = PDSM_EXT_STATUS_GENERIC_EVENT;
  (void)time_get(p_ext_info_buffer_ptr->time_stamp);  

  tm_pdapi_ext_status_event_callback(PDSM_EXT_STATUS_GENERIC_EVENT,
    p_ext_info_buffer_ptr);

}


/*===========================================================================

FUNCTION tm_prtl_check_mt_up_allowed

DESCRIPTION
  This function checks with TM-Core whether a mobile terminate User plane 
  session can be allowed or not based on the service set as Designated Data 
  by the user. E.g. MT V1/V2 will only be allowed if designated data is 1x.
  Similarly MT SUPL will only be allowed if designated data is GSM.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/ 
boolean tm_prtl_check_mt_up_allowed
(
  tm_prtl_type  prtl_type
)
{
#ifdef FEATURE_DUAL_SIM
  return tm_core_check_mt_up_allowed(prtl_type);
#else
  /* non DSDS phone, always ok to do MT on the service on which we received
   * MT trigger */
  return TRUE;
#endif
}


/*===========================================================================
FUNCTION  tm_core_pos_mode_allows_agnss_msb_up 

DESCRIPTION
  Checks if the correct bits are set in NV 1920 to allow AGNSS MSB position
  method type for U-Plane position protocols.

DEPENDENCIES

RETURN VALUE
  TRUE: If the AGNSS MSB bit are set correctly in the NV 1920.
  FALSE: Else, otherwise.

SIDE EFFECTS

===========================================================================*/
boolean tm_core_pos_mode_allows_agnss_msb_up( tm_positioning_protocols const e_pos_protocol)
{
  switch ( e_pos_protocol )
  {
     case TM_POS_PRTL_RRLP:
       /* Both A-GPS & A-GLO MSB bit is set to allow AGNSS MSB for RRLP */
       if ( ( TM_CONFIG_POS_MODE_BIT_IS_SET( PDSM_POS_MODE_UP_MS_BASED ) != 0) &&
            ( TM_CONFIG_POS_MODE_BIT_IS_SET( PDSM_AGLO_POS_MODE_UP_MS_BASED ) != 0 ) )
       {
         return ( TRUE );
       }
       break; 

     case TM_POS_PRTL_LPP:
       /* Both A-GPS & A-GLO MSB bit is set to allow AGNSS MSB for LPP */
       if ( ( TM_CONFIG_POS_MODE_BIT_IS_SET( PDSM_POS_MODE_4G_UP_MS_BASED ) != 0) &&
            ( TM_CONFIG_POS_MODE_BIT_IS_SET( PDSM_AGLO_POS_MODE_4G_UP_MS_BASED ) != 0 ) )
       {
         return ( TRUE );
       }
       break;

     default:
        break;
  };

  return ( FALSE );
}

/*===========================================================================
FUNCTION  tm_core_pos_mode_allows_agnss_msa_up

DESCRIPTION
  Checks if the correct bits are set in NV 1920 to allow AGNSS MSA position
  method type for U-Plane position protocols.

DEPENDENCIES

RETURN VALUE
  TRUE: If the AGNSS MSA bit are set correctly in the NV 1920.
  FALSE: Else, otherwise.

SIDE EFFECTS

===========================================================================*/
boolean tm_core_pos_mode_allows_agnss_msa_up( tm_positioning_protocols const e_pos_protocol)
{
  switch ( e_pos_protocol )
  {
     case TM_POS_PRTL_RRLP:
       /* Both A-GPS & A-GLO MSA bit is set to allow AGNSS MSA for RRLP */
       if ( ( TM_CONFIG_POS_MODE_BIT_IS_SET( PDSM_POS_MODE_UP_MS_ASSISTED ) != 0) &&
            ( TM_CONFIG_POS_MODE_BIT_IS_SET( PDSM_AGLO_POS_MODE_UP_MS_ASSISTED ) != 0 ) )
       {
         return ( TRUE );
       }
       break; 

     case TM_POS_PRTL_LPP:
       /* Both A-GPS & A-GLO MSA bit is set to allow AGNSS MSA for LPP */
       if ( ( TM_CONFIG_POS_MODE_BIT_IS_SET( PDSM_POS_MODE_4G_UP_MS_ASSISTED ) != 0) &&
            ( TM_CONFIG_POS_MODE_BIT_IS_SET( PDSM_AGLO_POS_MODE_4G_UP_MS_ASSISTED ) != 0 ) )
       {
         return ( TRUE );
       }
       break;

     default:
        break;
  };

  return ( FALSE );
}


/*===========================================================================
FUNCTION  tm_core_pos_mode_allows_agnss_msb_cp

DESCRIPTION
  Checks if the correct bits are set in NV 1920 to allow AGNSS MSB position
  method type for C-Plane position protocols.

DEPENDENCIES

RETURN VALUE
  TRUE: If the AGNSS MSA bit are set correctly in the NV 1920.
  FALSE: Else, otherwise.

SIDE EFFECTS

===========================================================================*/
boolean tm_core_pos_mode_allows_agnss_msb_cp( tm_positioning_protocols const e_pos_protocol)
{
  switch ( e_pos_protocol )
  {
     case TM_POS_PRTL_RRLP:
       /* We don't support RRLP A-GNSS on GSM CP. So return false. */
       break; 

     case TM_POS_PRTL_LPP:
       /* Both A-GPS & A-GLO MSB bit is set to allow AGNSS MSB for LPP */
       if ( ( TM_CONFIG_POS_MODE_BIT_IS_SET( PDSM_POS_MODE_4G_CP_MS_BASED ) != 0) &&
            ( TM_CONFIG_POS_MODE_BIT_IS_SET( PDSM_AGLO_POS_MODE_4G_CP_MS_BASED ) != 0 ) )
       {
         return ( TRUE );
       }
       break;

     case TM_POS_PRTL_RRC:
       /* Both A-GPS & A-GLO MSB bit is set to allow AGNSS MSB for RRC */
       if ( ( TM_CONFIG_POS_MODE_BIT_IS_SET( PDSM_POS_MODE_3G_CP_UE_BASED ) != 0) &&
            ( TM_CONFIG_POS_MODE_BIT_IS_SET( PDSM_AGLO_POS_MODE_3G_CP_MS_BASED ) != 0 ) )
       {
         return ( TRUE );
       }
       break;

     default:
        break;
  };

  return ( FALSE );
}

/*===========================================================================
FUNCTION  tm_core_pos_mode_allows_agnss_msa_cp

DESCRIPTION
  Checks if the correct bits are set in NV 1920 to allow AGNSS MSA position
  method type for C-Plane position protocols.

DEPENDENCIES

RETURN VALUE
  TRUE: If the AGNSS MSA bit are set correctly in the NV 1920.
  FALSE: Else, otherwise.

SIDE EFFECTS

===========================================================================*/
boolean tm_core_pos_mode_allows_agnss_msa_cp( tm_positioning_protocols const e_pos_protocol)
{
  switch ( e_pos_protocol )
  {
     case TM_POS_PRTL_RRLP:
       /* We don't support RRLP A-GNSS on GSM CP. So return false. */
       break; 

     case TM_POS_PRTL_LPP:
       /* Both A-GPS & A-GLO MSA bit is set to allow AGNSS MSA for LPP */
       if ( ( TM_CONFIG_POS_MODE_BIT_IS_SET( PDSM_POS_MODE_4G_CP_MS_ASSISTED ) != 0) &&
            ( TM_CONFIG_POS_MODE_BIT_IS_SET( PDSM_AGLO_POS_MODE_4G_CP_MS_ASSISTED ) != 0 ) )
       {
         return ( TRUE );
       }
       break;

     case TM_POS_PRTL_RRC:
       /* Both A-GPS & A-GLO MSA bit is set to allow AGNSS MSA for RRC */
       if ( ( TM_CONFIG_POS_MODE_BIT_IS_SET( PDSM_POS_MODE_3G_CP_UE_ASSISTED ) != 0) &&
            ( TM_CONFIG_POS_MODE_BIT_IS_SET( PDSM_AGLO_POS_MODE_3G_CP_MS_ASSISTED ) != 0 ) )
       {
         return ( TRUE );
       }
       break;

     default:
        break;
  };

  return ( FALSE );
}

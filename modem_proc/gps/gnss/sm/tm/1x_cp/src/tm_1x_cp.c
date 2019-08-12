/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM 1X UP Interface

General Description
  This file contains implementations for TM 1x CP Module
  
  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_cp/src/tm_1x_cp.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/18/15   sj      Clean up on E911 Exit
10/09/12   rh      Added some new error codes for IS801
05/27/07   rw      Abort current session for JGPS feature during 1x CP
01/31/07   ank     Added handling for generic IS801 call flow.
01/29/07   ank     Added handling for CANCEL req from PDE.
12/27/06   ank     Added support for IS801 Rx and Tx events.
12/20/06    ank     Changes to send PRM and post-fix PPM together.
12/12/06    ank     Initial Release
===========================================================================*/
#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"
#include "msg.h"
#include "event.h"
#include "pdapibuf.h"
#include "paixlate.h"
#include "tm_prtl_iface.h"
#include "sm_nv.h"
#include "sm_dm_event.h"
#include "sm_log.h"
#include "tm_1x_cp.h"
#include "tm_cm_iface.h"
#include "tm_is801_util.h"
#include "tm_1x_cp_internal.h"
#include "tm_is801b.h"
#include "tm_is801b_util.h"

/* TM-Core / 1x CP protocl interface callback table */
tm_prtl_cb_s_type            tm_1x_cp_prtl_iface_table;

/* NV buffer, used to read NV items */
static nv_item_type          tm_1x_cp_nv_read_buffer;

/* Global varialble which contains all necessary informaton for 1x CP module */
tm_1x_cp_sess_info_s_type    tm_1x_cp_session_info;
tm_1x_cp_config_s_type       tm_1x_cp_config;
is801_pddm_sess_info_s_type  last_pddm_hdr; 

/* Buffers to store prm parts */
sm_GpsMeasRptStructType     *cp_prm_buff_ptr_gps    = NULL;
sm_GpsMeasRptStructType     *cp_prm_buff_ptr_glo    = NULL;

gps_PreFixPpmSructType      *cp_prefix_ppm_buff_ptr = NULL;

/* pointers to dynamically allocated structs, to store params for creating rev
 * link response/req pddms */
is801_rev_link_resp_s_type  *is801_1_resp_parm_ptr  = NULL;
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
#error code not present
#endif

/****************************************************************************
 *                      LOCAL Function Prototypes                           *
 ****************************************************************************/
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
#error code not present
#endif

/*===========================================================================

FUNCTION tm_1x_cp_session_active

DESCRIPTION

RETURN VALUE
  boolean

DEPENDENCIES

===========================================================================*/
boolean tm_1x_cp_session_active(void)
{
  return tm_1x_cp_session_info.sess_active;
}

/********************************************************************
*
* tm_1x_cp_send_pd_comm_event
*
* Function description:
*   Send pdapi comm events through Tmcore
*
* Parameters: 
*
*   comm_event: part of pdsm_pd_event_type that are relevant to
*               comm.
*
* Return:
*  None
*  
**********************************************************************/
void tm_1x_cp_send_pd_comm_event 
(  
   pdsm_pd_event_type      comm_event
)
{
  pdsm_pd_info_s_type       z_pd_info_buffer;
  pdsm_client_id_type       client_id = pdsm_ni_client_id_get();

  if(client_id == PDSM_CLIENT_TYPE_NONE)
  {
    MSG_ERROR("No PDAPI client ID available", 0, 0, 0);
    return;
  }

  memset(&z_pd_info_buffer, 0, sizeof( pdsm_pd_info_s_type ) );

  z_pd_info_buffer.pd_info.comm_data.protocol_type = PDSM_PD_COMM_PROTOCOL_1X_CP;
  z_pd_info_buffer.client_id = client_id;

  (void) tm_post_pd_event( TM_PRTL_TYPE_1X_CP,
                           TM_SESS_HANDLE_1X_CP_MT,
                           comm_event,
                           &z_pd_info_buffer,
                           NULL);
}


/*===========================================================================
FUNCTION tm_1x_cp_update_qos

DESCRIPTION
  This function is called to update QOS parameters.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_cp_update_qos( byte prq )
{
  MSG_MED("MT session PRQ=%d",prq,0,0);

  tm_1x_cp_session_info.prm_prq = prq;

  if (prq == 0)
  {
    /* prq = 0 => T1=2s*/
    tm_1x_cp_session_info.gps_session_timeout = TM_1X_CP_PR_TIMEOUT_T1;
  }
  else if (prq == 1)
  {
    /* prq = 1 => T2=4s */
    tm_1x_cp_session_info.gps_session_timeout = TM_1X_CP_PR_TIMEOUT_T2;
  }
  else if (prq == 2)
  {
    /* prq = 2 => T3=8s */
    tm_1x_cp_session_info.gps_session_timeout = TM_1X_CP_PR_TIMEOUT_T3;
  }
  else if (prq == 3)
  {
    /* prq = 3 => T4=16s */
    tm_1x_cp_session_info.gps_session_timeout = TM_1X_CP_PR_TIMEOUT_T4;
  }
  else
  {
    /* prq = 4 & above. Cap it to 30s.
    */
    tm_1x_cp_session_info.gps_session_timeout = TM_1X_CP_PR_TIMEOUT_T5;
  }

  MSG_HIGH("Using PR resp timeout=%d sec",tm_1x_cp_session_info.gps_session_timeout,0,0);
} /* tm_1x_cp_update_qos */

/*===========================================================================

FUNCTION tm_1x_cp_terminate_session

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_1x_cp_terminate_session
(
  tm_1x_cp_term_src_e_type term_src, 
  tm_sess_stop_reason_e_type reason
)
{
  // convert PPM_TIMEOUT to Pre-Fix and Post-Fix timeout error
  if (reason == TM_STOP_REASON_PPM_TIMEOUT)
  {
    if (tm_1x_cp_session_info.doing_postfix_ppm == TRUE)
    {
      reason = TM_STOP_REASON_PPM_POST_FIX_TIMEOUT;
    }
    else
    {
      reason = TM_STOP_REASON_PPM_PRE_FIX_TIMEOUT;
    }
  }
  tm_1x_cp_session_info.doing_postfix_ppm = FALSE;

  MSG_HIGH("1X CP session termination. Reason %d",reason,0,0);

  tm_1x_cp_session_info.sess_active = FALSE;

#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif
  tm_1x_cp_session_info.num_is801_1_pddm = 0;

  if (tm_1x_cp_session_info.mgp_state.ppm_on)
  {
    (void)tm_1x_cp_end_ppm();
  }

  if (tm_1x_cp_session_info.mgp_state.prm_on)
  {
    (void)tm_1x_cp_end_prm();
  }

  /* Reset sesion_state */
  tm_1x_cp_session_info.sess_state = TM_1X_CP_SESS_STATE_NONE;

  /* Indicate TM CM Iface that session is done */
  if(term_src != TM_1X_CP_TERM_SRC_CM_IFACE)
  {
    (void)tm_cm_iface_stop_req(TM_PRTL_TYPE_1X_CP,NULL );
  }

  is801_reset_data(TM_PRTL_TYPE_1X_CP);
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif

  tm_1x_cp_session_info.tm_core_session_started = FALSE;
  tm_1x_cp_session_info.is801_req_wait_bmap     = 0;
  tm_1x_cp_session_info.is801_resp_to_send_bmap = 0;
  tm_1x_cp_session_info.cancel_req_bmap = 0;
  tm_1x_cp_session_info.fl_req_mask  = 0;
  tm_1x_cp_session_info.fl_resp_mask    = 0;

  tm_1x_cp_session_info.ppm_timeout = TM_MGP_GET_PPM_TIMEOUT_MAX;

  (void)os_TimerStop(tm_1x_cp_session_info.tm_1x_cp_sess_timer);  
  (void)os_TimerStop(tm_1x_cp_session_info.tm_1x_cp_ppm_timer);  
  (void)os_TimerStop(tm_1x_cp_session_info.tm_1x_cp_is801_timer);  
  (void)os_TimerStop(tm_1x_cp_session_info.tm_1x_cp_ppm_trigger_timer);  
  memset((void *)&tm_1x_cp_session_info.fwd_link_pddm, 0, sizeof(is801_fwd_link_pddm_info_s_type));
  memset((void *)&tm_1x_cp_session_info.rev_link_pddm, 0, sizeof(is801_rev_link_pddm_info_s_type));

  if(cp_prm_buff_ptr_gps)
  {
    (void)pdsm_freebuf((char *)cp_prm_buff_ptr_gps);
    cp_prm_buff_ptr_gps = NULL;
  }

#ifdef FEATURE_GNSS_SM_TM_1X_CP_GLONASS_ENABLE
  #error code not present
#endif

  if(cp_prefix_ppm_buff_ptr)
  {
    (void)pdsm_freebuf((char *)cp_prefix_ppm_buff_ptr);
    cp_prefix_ppm_buff_ptr = NULL;
  }

  if(is801_1_resp_parm_ptr)
  {
    (void)pdsm_freebuf((char *)is801_1_resp_parm_ptr);
    is801_1_resp_parm_ptr = NULL;
  }

#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#else
  tm_1x_cp_session_info.pde_version             = IS801_VERSION_801_1;
#endif

  tm_1x_cp_send_pd_comm_event (PDSM_PD_EVENT_COMM_DONE);
  /* Terminate session to TM-Core */
  (void) tm_1x_cp_terminate_sess_req(reason);
}


/*===========================================================================
FUNCTION tm_1x_cp_reject_is801_requests

DESCRIPTION
  This function is used to queue IS801 reject responses for PDE. The return
  value is true if any IS801 Req needs to be rejected. An MSI req however,
  is not rejected. Both IS801-1 and IS801-B rejects are handled.
  
DEPENDENCIES
  
RETURN VALUE  
TRUE:  Some response needs to be sent to PDE
FALSE: No response needs to be sent to PDE

SIDE EFFECTS

===========================================================================*/
boolean tm_1x_cp_reject_is801_requests
(
  is801_fwd_link_req_mask_type     is801_1_rej_req,
  is801B_fwd_link_req_mask_type    is801_b_v1_rej_req,
  is801B_fwd_link_req_mask_type    is801_b_v3_rej_req
)
{
  boolean                ret_val         = FALSE;
  tm_1x_cp_fl_req_type   reject_req_bmap =0;

  (void) reject_req_bmap;

  MSG_HIGH("Handle reject request bmap 801-1=0x%x 801BV1 %x 801BV3 %x",
             is801_1_rej_req, is801_b_v1_rej_req, is801_b_v3_rej_req);

  if( tm_1x_cp_session_info.pde_version == IS801_VERSION_801_1 )
  {
    ret_val = i_tm_1x_cp_reject_is801_1_requests(
                    is801_1_rej_req,
                    is801_1_resp_parm_ptr);
  }
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif
  else
  {
     MSG_ERROR("Unknown PDE version %d", tm_1x_cp_session_info.pde_version, 0, 0);
     return FALSE;
  }

  if(ret_val)
  {
    MSG_MED("reject bmap 0x%x",reject_req_bmap,0,0);
  }
  return ret_val;
} /* tm_1x_cp_reject_is801_requests */

/*===========================================================================
FUNCTION i_tm_1x_cp_cancel_req

DESCRIPTION
  This function cancels a request earlier queued by the PDE. In case of the 
  PPM and PRM reqs cancellations, a msg is passed to MGP to end these 
  measurements.

  In case of AMWF, TOM and LR reqs, the corresponding bits in the resp-to-send
  mask are cleared.
     
  In all other cancel reqs, nothing needs to be done by the state machine,
  since those reqs are not handled at all.
DEPENDENCIES
  
RETURN VALUE  

SIDE EFFECTS
1) Will clear bits from the global resp_to_send mask

===========================================================================*/
void i_tm_1x_cp_cancel_req
(
  tm_1x_cp_fl_req_type  cancel_type_1x,
  tm_1x_cp_fl_req_type *cancel_req_bmap,
  uint8                *cancel_no_out_req
)
{
#define NEGATE_FROM_RESP_TO_SEND(rl_resp_1x, fl_resp_1x)                    \
do {                                                                        \
  if(*is801_resp_to_send_bmap & rl_resp_1x)                                 \
  {                                                                         \
    *cancel_req_bmap           =   fl_resp_1x;                              \
    *is801_resp_to_send_bmap  &= ~(rl_resp_1x);                             \
    *cancel_no_out_req         = 0;                                         \
  }                                                                         \
  else                                                                      \
  {                                                                         \
    *cancel_no_out_req = 1;                                                 \
  }                                                                         \
}while(0);                                                                  \

  uint32   *is801_resp_to_send_bmap = 
              &tm_1x_cp_session_info.is801_resp_to_send_bmap;

  switch (cancel_type_1x)
  {  
    case TM_1X_CP_FL_REQ_PRM:
        MSG_HIGH("Cancel PRM req",0,0,0);
        NEGATE_FROM_RESP_TO_SEND(TM_1X_CP_RL_RESP_PRM, TM_1X_CP_FL_REQ_PRM);
        if ( *is801_resp_to_send_bmap & TM_1X_CP_RL_RESP_PRM )
        {
          if (tm_1x_cp_session_info.mgp_state.prm_on)
          {
            (void)tm_1x_cp_end_prm();
          }
        }
        break;

    case TM_1X_CP_FL_REQ_PPM:
       MSG_HIGH("Cancel PPM req",0,0,0);
       NEGATE_FROM_RESP_TO_SEND(TM_1X_CP_RL_RESP_PPM, TM_1X_CP_FL_REQ_PPM);
       if (*is801_resp_to_send_bmap & TM_1X_CP_RL_RESP_PPM )
       {
         if (tm_1x_cp_session_info.mgp_state.ppm_on)
         {
           (void)tm_1x_cp_end_ppm();
         }
       }
       break;

    case TM_1X_CP_FL_REQ_AMWF:
       MSG_HIGH("Cancel AWM req",0,0,0);
       NEGATE_FROM_RESP_TO_SEND(TM_1X_CP_RL_RESP_AMWF, TM_1X_CP_FL_REQ_AMWF);
       break;

    case TM_1X_CP_FL_REQ_TOM:
      MSG_HIGH("Cancel TOM req",0,0,0);
      NEGATE_FROM_RESP_TO_SEND(TM_1X_CP_RL_RESP_TOM, TM_1X_CP_FL_REQ_TOM);
      break;

    case TM_1X_CP_FL_REQ_LR:
      MSG_HIGH("Cancel LR req",0,0,0);
      NEGATE_FROM_RESP_TO_SEND(TM_1X_CP_RL_RESP_LR, TM_1X_CP_FL_REQ_LR);
      break;

    default:
      MSG_HIGH("No action needed for Cancel type=%d", cancel_type_1x,0,0);
      *cancel_no_out_req         = 0;
      break;
  } /* End switch for cancel type */
  return;
}

/*===========================================================================
FUNCTION i_tm_1x_cp_handle_is801_1_cancel_request

DESCRIPTION
  This function is used to handle cancellation request from Is801-1 PDE. If 
  there is a pending request, that request is deleted and appropriate action is 
  taken. The cancellation response depends upon whether there was a 
  pending request corsp to cancellation request or not.
     
  The cancel_req_bmap stored in tm_1x_cp_session_info is initialized to the
  new cancel requests sent.
DEPENDENCIES
  
RETURN VALUE  
  TRUE: If a cack response is to be sent
  FALSE: If no cack response is to be sent

SIDE EFFECTS

===========================================================================*/
boolean i_tm_1x_cp_handle_is801_1_cancel_request(void)
{
  uint8 i                 = 0;
  uint8 cancel_no_out_req = 0;
  is801_fwd_link_req_s_type  *req_rd          = NULL;
  tm_1x_cp_fl_req_type        cancel_type_1x  = 0x0;
  boolean                     ret_val         = FALSE;

  MSG_MED("Handle cancel request",0,0,0);

  /* Look for a cancel Requests among the req bit mask */
  for(i = 0; i < tm_1x_cp_session_info.fwd_link_pddm.hdr.num_requests; i++)
  {
    req_rd = &tm_1x_cp_session_info.fwd_link_pddm.req_rd[i];
    if(req_rd->request_type != IS801_FWD_LINK_REQ_MASK_CAN)
    {
      continue;
    }

    /* Set the default values for cack response.
    */
    tm_1x_cp_session_info.cancel_req_bmap = 0x0;      
    cancel_no_out_req = 1;
    
    /* xlate from 801-1 to generic 1x cancel req type */
    cancel_type_1x = 
      xlate_cancel_req_is801_1_to_1x_cp(
        (pde_to_ms_req_type) req_rd->req_msg_payload.cancel_req.cancel_type);

    /* If req could not be translated, look at the next req */
    if(cancel_type_1x == TM_1X_CP_FL_REQ_NONE)
    {
      MSG_ERROR("Could not translate cancel req %d", 
                   req_rd->req_msg_payload.cancel_req.cancel_type, 0, 0);
      continue;
    }

    /* NOTE: 801-1 cancel reqs are enums, not bit masks. */
    MSG_MED("Found cancel request %d at %d", cancel_type_1x, i, 0);

    /* if the req to be cancelled is present in the resp_to_send mask, clear it
     * and initialze value of cancel_no_out and cancel_req_bmap */ 
    i_tm_1x_cp_cancel_req( cancel_type_1x, 
                          &tm_1x_cp_session_info.cancel_req_bmap, 
                          &cancel_no_out_req);

    /* Create CACK response 
    */
    ret_val = i_tm_1x_cp_create_is801_1_cack_resp(
                cancel_no_out_req,
                req_rd->req_msg_payload.cancel_req.cancel_type,
                is801_1_resp_parm_ptr);

    tm_1x_cp_session_info.fl_req_mask &= ~TM_1X_CP_FL_REQ_CAN;
  }
  return ret_val;
} /* end i_tm_1x_cp_handle_is801_1_cancel_request */

#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
#error code not present
#endif

/*===========================================================================
FUNCTION tm_1x_cp_handle_cancel_request

DESCRIPTION
  This function is used to handle cancellation request from PDE. Depending on
  whether the pde version is 801-1 or 801-B, appropriate functions are called.
     
DEPENDENCIES
  
RETURN VALUE  
  TRUE: If a cack response is to be sent
  FALSE: If no cack response is to be sent

SIDE EFFECTS

===========================================================================*/
boolean tm_1x_cp_handle_cancel_request(void)
{
  boolean   ret_val                 = FALSE;
  
  if ( (tm_1x_cp_session_info.pde_version != IS801_VERSION_801_1) &&
       (tm_1x_cp_session_info.pde_version != IS801_VERSION_801_B) )
  {
    MSG_ERROR("Unknown pde_version %d", tm_1x_cp_session_info.pde_version, 0, 0);
    return FALSE;
  }


  if (tm_1x_cp_session_info.pde_version == IS801_VERSION_801_1)
  {
    ret_val = i_tm_1x_cp_handle_is801_1_cancel_request();
  }
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif

  if(tm_1x_cp_session_info.cancel_req_bmap)
  {
    MSG_MED("Cancel bmap 0x%x", tm_1x_cp_session_info.cancel_req_bmap,0,0);
  }
  return ret_val;
} /* end tm_1x_cp_handle_cancel_request() */

/*===========================================================================

FUNCTION tm_1x_cp_create_rlink_req

DESCRIPTION
  This function is used create IS801 request for MT sessions. The i/p param
  decides which request to be created.

RETURN VALUE
  boolean

DEPENDENCIES

===========================================================================*/
boolean tm_1x_cp_create_rlink_req
(
  // is801_rev_link_req_mask_type req
  tm_1x_cp_rlink_req_type         req
)
{
  boolean ret_val = FALSE;
  is801_rev_link_req_mask_type  is801_1_req;
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif

  if( tm_1x_cp_session_info.pde_version == IS801_VERSION_801_1 )
  {
    is801_1_req = xlate_1x_cp_rl_req_to_is801_1(req);
    ret_val     = is801_new_rev_link_req( TM_PRTL_TYPE_1X_CP, is801_1_req);
  }
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif

  if ( ret_val )
  {
    tm_1x_cp_session_info.is801_req_wait_bmap |= (req);
    MSG_MED("Created req type=%d",req,0,0);
  }
  else
  {
    MSG_ERROR("RevLink Req 0x%x for pdeversion %d can not be created",
                 req, tm_1x_cp_session_info.pde_version, 0);
  }
  return ret_val;
} /* End tm_1x_cp_create_rlink_req() */

/*===========================================================================

FUNCTION tm_1x_cp_handle_ppm_req

DESCRIPTION
  This function is used to handle PPM req from PDE.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_cp_handle_ppm_req(void)
{
  if( !tm_1x_cp_session_active() || 
      (tm_1x_cp_session_info.is801_resp_to_send_bmap & TM_1X_CP_RL_RESP_PPM))
  {
    MSG_ERROR("PPM req when sess inactive %d or prev. PPM req 0x%x is pending",
               tm_1x_cp_session_active(),
               tm_1x_cp_session_info.is801_resp_to_send_bmap, 0);
    return;
  }

  /* if MT sess. is active and there is no outstanding PPM req. start PPM */
  tm_1x_cp_session_info.is801_resp_to_send_bmap |= TM_1X_CP_RL_RESP_PPM;
  /* If PPM is not going on, start it now.
  */
  if ( tm_1x_cp_session_info.mgp_state.ppm_on == FALSE) 
  {
    if( !tm_1x_cp_start_ppm(TM_1X_CP_SESS_CONTINUE) )
    {
      MSG_ERROR("Unable to request TM-Core to start PPM",0,0,0);
      tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE,TM_STOP_REASON_GENERAL_ERROR);
    }
  } /* End if PPM is not running */
}


/*===========================================================================

FUNCTION tm_1x_cp_handle_prm_req

DESCRIPTION
  This function is used to handle PRM req from PDE.

RETURN VALUE
  None

DEPENDENCIES
===========================================================================*/
void tm_1x_cp_handle_prm_req(const byte pref_resp_qual )
{
  /* if no MT session is active or there is an outstanding PRM request, bomb out */
  if( tm_1x_cp_session_active() == FALSE || 
      (tm_1x_cp_session_info.is801_resp_to_send_bmap & TM_1X_CP_RL_RESP_PRM))
  {
    MSG_ERROR("PRM req when session is not active or prev PRM req is pending",
                0, 0, 0);
    return;
  }

  tm_1x_cp_session_info.is801_resp_to_send_bmap |= TM_1X_CP_RL_RESP_PRM;
  tm_1x_cp_update_qos(pref_resp_qual);

  if (tm_1x_cp_session_info.fl_resp_mask & TM_1X_CP_FL_RESP_AA)
  {   
    MSG_MED("AA resp received, PRM already started",0,0,0);
    return;
  }

  /* PRM Req received without AA data */
  MSG_MED("AA not available. Send Req to PDE",0,0,0);
  if(tm_1x_cp_create_rlink_req(TM_1X_CP_RLINK_REQ_AA) == FALSE)
  {
    MSG_ERROR("Can not create AA req",0,0,0);
    tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE, TM_STOP_REASON_GENERAL_ERROR);
    return;
  }

  if(i_tm_1x_cp_send_rlink_pddm(0, tm_1x_cp_session_info.pde_version))
  {
    /* Start IS801 timer */
    if(!os_TimerStart(tm_1x_cp_session_info.tm_1x_cp_is801_timer,TM_IS801_TIMEOUT,NULL))
    {
      MSG_ERROR("Failed starting IS801 timer",0,0,0);
    }
  }
  else
  {
    tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE,TM_STOP_REASON_IS801_SEND_FAILURE);
    return;
  }
}

/*===========================================================================

FUNCTION tm_1x_cp_handle_sa_req

DESCRIPTION
  This function is used handle request by MGP for SA data. 

RETURN VALUE
  None

DEPENDENCIES
===========================================================================*/
void tm_1x_cp_handle_sa_req(void)
{
  boolean ret_val = FALSE;
  MSG_HIGH("SA request from MGP",0,0,0);

  /* Request PDE for SA data.
  */
  if (tm_1x_cp_session_active() != TRUE)
  {
    MSG_ERROR("1X CP session not active",0,0,0);
    return;
  }

  if(tm_1x_cp_create_rlink_req(TM_1X_CP_RLINK_REQ_GPS_SA) == FALSE)
  {
    MSG_ERROR("Can not create SA req",0,0,0);
    tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE, TM_STOP_REASON_GENERAL_ERROR);
    return;
  }

  
  ret_val = i_tm_1x_cp_send_rlink_pddm(0, tm_1x_cp_session_info.pde_version);

  if(ret_val)
  {
    /* Start IS801 timer */
    if(!os_TimerStart(tm_1x_cp_session_info.tm_1x_cp_is801_timer,TM_IS801_TIMEOUT,NULL))
    {
      MSG_ERROR("Failed starting IS801 timer",0,0,0);
    }
  }
  else
  {
    tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE,TM_STOP_REASON_IS801_SEND_FAILURE);
    return;
  }
} /* End tm_1x_cp_is801_drv_handle_sa_req() */

/*===========================================================================

FUNCTION tm_1x_cp_create_ppm_resp

DESCRIPTION: Perform a sanity check on the state of 1xcp. Perform sanity checks
             on parameters. Depending on session type, Create and transmit a 
             801-1 or 801-B ppm response using the given prms.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If successful
  FALSE:  If unsuccessful

SIDE EFFECTS
  Will terminate active session if ppm response can not be created

===========================================================================*/
boolean tm_1x_cp_create_ppm_resp
(
  srch_ppm_rpt_type          *ppm_ptr,
  uint8                       unsolicited, 
  byte                        pde_version
)
{
  boolean ret_val = TRUE;

  if(ppm_ptr == NULL)
  {
    MSG_ERROR("Null ptrs passed. ppm:%x", ppm_ptr, 0, 0);
    ret_val = FALSE;
  }


  /* PDE version must be 801-1 or 801-b, else declare error, terminate session
   * and return FALSE */
  if( ret_val == TRUE &&
      pde_version != IS801_VERSION_801_1 &&
      pde_version != IS801_VERSION_801_B )
  {
    MSG_ERROR("Unknown PDE version", 0, 0, 0);
    ret_val = FALSE;
  }

  if( ret_val == TRUE && 
      pde_version == IS801_VERSION_801_1 && is801_1_resp_parm_ptr != NULL)
  {
    ret_val = i_tm_1x_cp_create_is801_1_ppm_resp(
                                is801_1_resp_parm_ptr,
                                ppm_ptr,
                                unsolicited);
  }

#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif

  if(ret_val == FALSE)
  {
    MSG_ERROR("Could not create PPM resp. Terminate session", 0, 0, 0);
    tm_1x_cp_terminate_session( TM_1X_CP_TERM_SRC_NONE, 
                                TM_STOP_REASON_GENERAL_ERROR);
  }
  else
  {
    MSG_MED("Created PPM resp",0,0,0);
  }

  return ret_val;
}

/*===========================================================================

FUNCTION i_tm_1x_cp_handle_prefix_ppm_resp

DESCRIPTION: handle a prefix (before prms have been rcvd) ppm response. This 
             is done by sending a ppm response to the PDE. The ppm response
             is also saved for later steps, when the prefix ppm will be posted
             back to mgp along with the received AA data.

             If ppm resp cant be created, or sent, or saved for later posting
             to MGP, the return value is FALSE.

RETURN VALUE
  TRUE:      All steps in handling ppm resp from searcher successful
  FALSE:     Could not handle ppm resp from searcher

DEPENDENCIES

===========================================================================*/
static boolean i_tm_1x_cp_handle_prefix_ppm_resp( srch_ppm_rpt_type *ppm_ptr )
{
  /* create solicited ppm */
  if( tm_1x_cp_create_ppm_resp( ppm_ptr, 
                                0, /* solicited ppm */
                                tm_1x_cp_session_info.pde_version) != TRUE)
  {
    return FALSE;
  }

  tm_1x_cp_session_info.is801_resp_to_send_bmap &= ~(TM_1X_CP_RL_RESP_PPM);
  /* send PPM resp */
  if(!i_tm_1x_cp_send_rlink_pddm(0, tm_1x_cp_session_info.pde_version))
  {
    return FALSE;
  }

  MSG_HIGH("Save pre-fix PPM data till AA data is available",0,0,0);
  /* allocate prefix ppm buffer if unavailable */
  if ( cp_prefix_ppm_buff_ptr == NULL )
  {
    cp_prefix_ppm_buff_ptr = 
      (gps_PreFixPpmSructType*) pdsm_getbuf(sizeof(*cp_prefix_ppm_buff_ptr));

    if(cp_prefix_ppm_buff_ptr == NULL)
    {
      MSG_ERROR("No buffer for PPM.", 0, 0, 0);
      return FALSE;
    } /* End else if no buffer is available */
  }

  /* copy over allocated ppm buffer */
  cp_prefix_ppm_buff_ptr->l_RfCalibGCx1024 = 
    is801_get_rf_delay(ppm_ptr->band_class);

  memscpy( (void *)  &(cp_prefix_ppm_buff_ptr->z_PpmRpt), sizeof(cp_prefix_ppm_buff_ptr->z_PpmRpt),
          (const void *)  ppm_ptr, sizeof(*ppm_ptr));

  return TRUE;
}

/*===========================================================================

FUNCTION tm_1x_cp_create_prm_resp

DESCRIPTION: Perform a sanity check on the state of 1xcp. Perform sanity checks
             on parameters. Depending on session type, Create and transmit a 
             801-1 or 801-B prm response using the given prms.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If successful
  FALSE:  If unsuccessful

SIDE EFFECTS
  Will terminate active session if prm response can not be created

===========================================================================*/
boolean tm_1x_cp_create_prm_resp
(
  srch_ppm_rpt_type          *ppm_ptr,
  uint8                       unsolicited, 
  byte                        pde_version
)
{
  boolean ret_val = TRUE;

  /* Its possible for the ppm ptr to be NULL */
  if(cp_prm_buff_ptr_gps == NULL && cp_prm_buff_ptr_glo == NULL)
  {
    MSG_ERROR("Null ptrs passed. ppm:%x prm_gps:%x prm_glo", 
                 ppm_ptr, cp_prm_buff_ptr_gps, cp_prm_buff_ptr_glo);
    ret_val = FALSE;
  }

  /* PDE version must be 801-1 or 801-b, else declare error, terminate session
   * and return FALSE */
  if( ret_val == TRUE &&
      pde_version != IS801_VERSION_801_1 &&
      pde_version != IS801_VERSION_801_B )
  {
    MSG_ERROR("Unknown PDE version", 0, 0, 0);
    ret_val = FALSE;
  }

  if( ret_val == TRUE                    && 
      pde_version == IS801_VERSION_801_1 && 
      is801_1_resp_parm_ptr != NULL                   &&
      cp_prm_buff_ptr_gps != NULL)
  {
    ret_val = i_tm_1x_cp_create_is801_1_prm_resp(
                                is801_1_resp_parm_ptr,
                                ppm_ptr,
                                unsolicited, 
                                cp_prm_buff_ptr_gps);
  }

#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif

  if(ret_val == FALSE)
  {
    MSG_ERROR("Could not create PRM resp. Terminate session", 0, 0, 0);
  }
  else
  {
    MSG_MED("Created PRM resp",0,0,0);
  }

  return ret_val;
}

/*===========================================================================

FUNCTION tm_1x_cp_create_and_tx_tom_resp

DESCRIPTION: Perform a sanity check on the state of 1xcp. Perform sanity checks
             on parameters. Depending on session type, Create and transmit a 
             801-1 or 801-B prm response using the given ppms.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If successful
  FALSE:  If unsuccessful

SIDE EFFECTS
  Will terminate active session if prm response can not be created

===========================================================================*/
boolean tm_1x_cp_create_and_tx_tom_resp
(
  srch_ppm_rpt_type          *ppm_ptr,
  uint8                       unsolicited, 
  byte                        pde_version
)
{
  boolean ret_val = TRUE;

  /* Its possible for the ppm ptr to be NULL */
  if(ppm_ptr == NULL)
  {
    MSG_ERROR("Null ptrs passed. ppm:%x", ppm_ptr, 0, 0);
    ret_val = FALSE;
  }

  /* PDE version must be 801-1 or 801-b, else declare error, terminate session
   * and return FALSE */
  if( ret_val == TRUE &&
      pde_version != IS801_VERSION_801_1 &&
      pde_version != IS801_VERSION_801_B )
  {
    MSG_ERROR("Unknown PDE version", 0, 0, 0);
    ret_val = FALSE;
  }

  if( ret_val == TRUE                    && 
      pde_version == IS801_VERSION_801_1 && 
      is801_1_resp_parm_ptr != NULL)
  {
    ret_val = i_tm_1x_cp_create_and_tx_is801_1_tom_resp(
                     is801_1_resp_parm_ptr,
                     ppm_ptr,
                     unsolicited );
  }
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif

  if(ret_val == FALSE)
  {
    MSG_ERROR("Could not create/tx TOM resp. Terminate session", 0, 0, 0);
  }
  else
  {
    MSG_MED("Created PRM resp",0,0,0);
  }

  return ret_val;
}


/*===========================================================================

FUNCTION i_tm_1x_cp_handle_postfix_ppm_resp

DESCRIPTION: handle a postfix (after prms have been rcvd) ppm response. This 
             is done by sending a ppm and prm response to the PDE, along with
             a location req and a time offset measurement

RETURN VALUE
TRUE  : PPM response handled successfully
FALSE : An error occurred in handling PPM response.

DEPENDENCIES

===========================================================================*/
static boolean i_tm_1x_cp_handle_postfix_ppm_resp( srch_ppm_rpt_type *ppm_ptr )
{
  /* Waiting to send out both PPM and PRM. check if PRMs are available in buffer */
  if(cp_prm_buff_ptr_gps == NULL && cp_prm_buff_ptr_glo == NULL)
  {
    MSG_MED("No PRM data in the buffer, ignoring PPM resp",0,0,0);
    return TRUE;
  }

  /* buffers have prm, create a solicited prm response */
  if( tm_1x_cp_create_prm_resp( ppm_ptr, 
                                0, /* solicited PRM resp */
                                tm_1x_cp_session_info.pde_version) != TRUE)
  {
    return FALSE;
  }
  tm_1x_cp_session_info.is801_resp_to_send_bmap &= ~(TM_1X_CP_RL_RESP_PRM);

  /* Create solicited Post-fix PPM */
  if( tm_1x_cp_create_ppm_resp( ppm_ptr, 
                                0, /* solicited PPM resp */
                                tm_1x_cp_session_info.pde_version) != TRUE)
  {
    return FALSE;
  }
  tm_1x_cp_session_info.is801_resp_to_send_bmap &= ~(TM_1X_CP_RL_RESP_PPM);

  /* Create Location Request and send it to IS801 queue.
  */
  if(tm_1x_cp_create_rlink_req(TM_1X_CP_RLINK_REQ_LR) == FALSE)
  {
    MSG_ERROR("Can not create LR req",0,0,0);
    return FALSE;
  }

  /* End PPM */
  if(tm_1x_cp_session_info.mgp_state.ppm_on == TRUE)
  {
    (void)tm_1x_cp_end_ppm();
  }

  /* Send Unsol TOM message to PDE.
  */
  return tm_1x_cp_create_and_tx_tom_resp( ppm_ptr, 1, 
                                          tm_1x_cp_session_info.pde_version);
}

/*===========================================================================

FUNCTION i_tm_1x_cp_handle_ppm_resp_error_check

DESCRIPTION
  This function is used to perform initial error checks before the actual 
  handling of the PPMs. 
  
  For PPM handling to begin, 
    The session must be active
    Pointer to ppms must be non null
    A buffer to store the PRMs must be successfully allocated.

  In case of any error conditions, the function prints a diag msg and 
  terminates any ongoing session.

RETURN VALUE
  TRUE: No Errors
  FALSE: Errors
DEPENDENCIES

===========================================================================*/
static boolean i_tm_1x_cp_handle_ppm_resp_error_check
( 
  const srch_ppm_rpt_type *ppm_ptr
)
{
  if(tm_1x_cp_session_active() != TRUE)
  {
    MSG_ERROR("PPM resp not expected",0,0,0);
    return FALSE;
  }

  if(ppm_ptr == NULL)
  {
    MSG_ERROR("Null ppm_ptr",0,0,0);
    tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE, TM_STOP_REASON_GENERAL_ERROR);
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

FUNCTION tm_1x_cp_handle_ppm_resp

DESCRIPTION

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
boolean tm_1x_cp_handle_ppm_resp( srch_ppm_rpt_type *ppm_ptr )
{
  boolean ret_val = FALSE;
  (void)os_TimerStop(tm_1x_cp_session_info.tm_1x_cp_ppm_timer);  

  MSG_MED("Handle PPM resp. resp_to_send_bmap 0x%x",
             tm_1x_cp_session_info.is801_resp_to_send_bmap, 0, 0);

  if(i_tm_1x_cp_handle_ppm_resp_error_check( ppm_ptr ) != TRUE)
    return FALSE;

  /* We must be expecting PPM to send to the PDE, if not, ignore 
   * - drop ppm on the floor */
  if(!(tm_1x_cp_session_info.is801_resp_to_send_bmap & TM_1X_CP_RL_RESP_PPM))
  {
    MSG_ERROR("PPM resp when not requested req_mask 0x%x",
                 tm_1x_cp_session_info.fl_req_mask,0,0);
    return TRUE;
  }

  /* We are waiting to send only PPM and no PRM, i.e. prefix PPM */
  if(!(tm_1x_cp_session_info.is801_resp_to_send_bmap & TM_1X_CP_RL_RESP_PRM))
  {
    ret_val = i_tm_1x_cp_handle_prefix_ppm_resp( ppm_ptr );
  } 
  /* Need to send both PPM and PRM, i.e. post fix PPM */
  else if(tm_1x_cp_session_info.is801_resp_to_send_bmap & TM_1X_CP_RL_RESP_PRM)
  {
    ret_val = i_tm_1x_cp_handle_postfix_ppm_resp( ppm_ptr );
  } 

  /* Terminate the session if for some reason we could not handle the
   * PPM response */
  if(ret_val == FALSE)
  {
    tm_1x_cp_terminate_session( TM_1X_CP_TERM_SRC_NONE, 
                                TM_STOP_REASON_GENERAL_ERROR );
  }
  else
  {
    /* Start IS801 timer */
    if( !os_TimerStart( tm_1x_cp_session_info.tm_1x_cp_is801_timer, 
                        TM_IS801_TIMEOUT, NULL))
    {
      MSG_ERROR("Failed starting IS801 timer",0,0,0);
    }
  }

  return ret_val;
}

/*===========================================================================

FUNCTION i_tm_1x_cp_handle_prm_resp_error_check

DESCRIPTION
  This function is used to perform initial error checks before the actual 
  handling of the PRMs. 
  
  For PRM handling to begin, 
    The session must be active
    Pointer to prms must be non null

  In case of any error conditions, the function prints a diag msg and 
  terminates any ongoing session.

RETURN VALUE
  TRUE: No Errors
  FALSE: Errors
DEPENDENCIES

===========================================================================*/
static boolean i_tm_1x_cp_handle_prm_resp_error_check
( 
  const sm_GpsMeasRptStructType *prm_ptr
)
{
  if(tm_1x_cp_session_active() != TRUE)
  {
    MSG_ERROR("PRM resp not expected",0,0,0);
    /* Since session is not active, no need to terminate session here */
    return FALSE;
  }

  if(prm_ptr == NULL)
  {
    MSG_ERROR("Null prm data",0,0,0);
    tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE, TM_STOP_REASON_GENERAL_ERROR);
    return FALSE;
  }

  return TRUE;
} /* i_tm_1x_cp_handle_prm_resp_error_check */

/*===========================================================================

FUNCTION tm_1x_cp_save_prm_part

DESCRIPTION
  This function saves the prm reports from MGP. Only GPS and Glonass PRMS are
  saved. The buffers to save the prm reports are dynamically allocated. 

RETURN VALUE
  TRUE  : PRM successfully saved
  FALSE : Unknown error condition

DEPENDENCIES

===========================================================================*/
boolean tm_1x_cp_save_prm_part
(
  const sm_GpsMeasRptStructType  *prm_ptr
)
{
  sm_GpsMeasRptStructType *prm_buff_ptr  = NULL;
  
  MSG_HIGH("Save PRM data",0,0,0);

  /* First perform all error checks, we only handle GPS/GLO PRMs */
  if( (prm_ptr->z_MeasBlk.e_MeasBlkSrc != GNSS_MEAS_BLK_SRC_GPS) &&
      (prm_ptr->z_MeasBlk.e_MeasBlkSrc != GNSS_MEAS_BLK_SRC_GLO) )
  {
    ERR_FATAL("Unknown constellation passed in prm %d", 
                 (uint8) prm_ptr->z_MeasBlk.e_MeasBlkSrc, 0, 0);
  }

  /* Check that the prior buffer has been freed */
  if( (prm_ptr->z_MeasBlk.e_MeasBlkSrc == GNSS_MEAS_BLK_SRC_GPS) &&
      (cp_prm_buff_ptr_gps != NULL ) )
  {
    MSG_ERROR("Memory leak in storing gps prm buffers", 0, 0, 0);
  }

#ifdef FEATURE_GNSS_SM_TM_1X_CP_GLONASS_ENABLE
  #error code not present
#else /*FEATURE_GNSS_SM_TM_1X_CP_GLONASS_ENABLE*/  
  if(prm_ptr->z_MeasBlk.e_MeasBlkSrc == GNSS_MEAS_BLK_SRC_GLO)
  {
    MSG_MED("Glonass: Not supported in 1x CP",0,0,0);
    return TRUE;
  }
#endif/*FEATURE_GNSS_SM_TM_1X_CP_GLONASS_ENABLE*/  

  /* Try to get a new prm buffer */
  prm_buff_ptr = (sm_GpsMeasRptStructType*) pdsm_getbuf(
                            sizeof(*prm_buff_ptr));

  if(prm_buff_ptr)
  {
     if(prm_ptr->z_MeasBlk.u_NumSvs == 0)
     {
       /* create bogus PRMs */
       memset( prm_buff_ptr, 0, sizeof(*prm_buff_ptr)); /* sm_GpsMeasRptStructType */
       prm_buff_ptr->z_MeasBlk.u_NumSvs = 0;
       prm_buff_ptr->z_MeasBlk.z_MeasClock.z_Time.z_GpsTime.w_GpsWeek 
                                           = C_GPS_WEEK_UNKNOWN;
     }
     else
     {
       memscpy((void *)prm_buff_ptr, sizeof(*prm_buff_ptr), (const void *) prm_ptr, 
              sizeof(*prm_ptr)); /* sizeof sm_GpsMeasRptStructType */
     }

     if(prm_ptr->z_MeasBlk.e_MeasBlkSrc == GNSS_MEAS_BLK_SRC_GPS)
     {
       cp_prm_buff_ptr_gps = prm_buff_ptr;
     }
#ifdef FEATURE_GNSS_SM_TM_1X_CP_GLONASS_ENABLE
     #error code not present
#endif
     return TRUE;
  }
  else
  {
    MSG_ERROR("No buffer for PRM", 0, 0, 0);
    return FALSE;
  } /* End else if no buffer is available */
} /* end tm_1x_cp_save_prm_part */

/*===========================================================================

FUNCTION tm_1x_cp_handle_prm_resp

DESCRIPTION
  This function handles the prm reports from MGP, until all prm parts are rcvd.
  PRM parts are saved until all parts are rcvd, and no processing is done on 
  the rcvd PRMs until all parts are rcvd. If there are no PRMs rcvd, 
  bogus prm measurements are sent to the server.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
boolean tm_1x_cp_handle_prm_resp
( 
  const sm_GpsMeasRptStructType  *prm_ptr
)
{
  boolean ret_val = TRUE;

  MSG_MED("Handle PRM resp. resp_to_send_bmap 0x%x",
             tm_1x_cp_session_info.is801_resp_to_send_bmap, 0, 0);

  /* Perform sanity checks on 1xCP state machine and the rcvd PRM */
  if( i_tm_1x_cp_handle_prm_resp_error_check( prm_ptr ) != TRUE )
    ret_val = FALSE;

  /* Handle PRM parts */
  if(prm_ptr->z_MeasBlk.u_SeqNum < prm_ptr->z_MeasBlk.u_MaxMessageNum)
  {
    /* If more prm parts are to follow, simply save this part and exit */
    return tm_1x_cp_save_prm_part(prm_ptr);
  }
  else if(prm_ptr->z_MeasBlk.u_SeqNum == prm_ptr->z_MeasBlk.u_MaxMessageNum)
  {
    /* If this is the last prm part, save this part and continue processing.
     * If this part could not be saved, the ret_val will be set to FALSE 
     * and we will terminate session later */
    ret_val = tm_1x_cp_save_prm_part(prm_ptr);
    tm_1x_cp_session_info.mgp_state.prm_on = FALSE;
  }
  else
  {
    /* Sanity Check if prm part information is incorrect */
    MSG_ERROR("PRM part num %d > max parts %d", 
                 prm_ptr->z_MeasBlk.u_SeqNum, 
                 prm_ptr->z_MeasBlk.u_MaxMessageNum, 0);
    tm_1x_cp_terminate_session( TM_1X_CP_TERM_SRC_NONE, 
                                (tm_sess_stop_reason_e_type)PDSM_PD_END_IS801_PAYLOAD_ERROR);
    ret_val = FALSE;
  }

  /**** All PRM parts delivered, start processing PRMs ****/

  /* Check if PPM req is made by the PDE, if yes, start post fix ppm, and 
   * exit. Processing will continue when post fix ppm is rcvd */
  if( ret_val == TRUE &&
      (tm_1x_cp_session_info.is801_resp_to_send_bmap & TM_1X_CP_RL_RESP_PPM))
  {
    MSG_MED("Requesting POST-FIX PPM", 0, 0, 0);

    tm_1x_cp_session_info.doing_postfix_ppm = TRUE;

    /* If ppm is not on, start ppm, else get ppm */
    if ( tm_1x_cp_session_info.mgp_state.ppm_on == FALSE) 
    {
      MSG_MED("PPM not running, start it",0,0,0);
      (void)tm_1x_cp_start_ppm(TM_1X_CP_SESS_CONTINUE);    
      if(!os_TimerStart(tm_1x_cp_session_info.tm_1x_cp_ppm_trigger_timer,1*1000,NULL))
      {
        MSG_ERROR("Failed starting PPM trigger timer",0,0,0);
      }
    }
    else
    {
      /* If msg to get PPM cant be sent, terminate session */
      ret_val = tm_1x_cp_get_ppm();
    }
  }
  else if(ret_val == TRUE) /* No req for PPM by PDE, simply send PRM+LR req */
  {
    MSG_MED("No req for PPM, create PRM resp + LR req", 0, 0, 0);
    ret_val = tm_1x_cp_create_prm_resp( 
                    NULL, 
                    0, /* Send solicited PRM resp */
                    tm_1x_cp_session_info.pde_version);
    
    /* Create Location Request and queue it to IS801 queue. If LR req creation
     * fails, terminate session
    */
    if(ret_val)
    {
      ret_val = tm_1x_cp_create_rlink_req(TM_1X_CP_RLINK_REQ_LR);
    }

    if(ret_val)
    {
      ret_val = i_tm_1x_cp_send_rlink_pddm(0, tm_1x_cp_session_info.pde_version);
    }

    if(ret_val)
    {
      /* Start IS801 timer */
      if( !os_TimerStart( tm_1x_cp_session_info.tm_1x_cp_is801_timer,
                          TM_IS801_TIMEOUT, NULL ))
      {
        MSG_ERROR("Failed starting IS801 timer",0,0,0);
      }
    }
  }

  if(ret_val == FALSE)
  {
    tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE,TM_STOP_REASON_GENERAL_ERROR);
    return ret_val;
  }
  else
  {
    MSG_MED("Handled PRM response from MGP successfuly", 0, 0, 0);
    return ret_val;
  }
} /* End tm_1x_cp_handle_prm_resp( ) */

/*===========================================================================

FUNCTION tm_1x_cp_handle_rej_resp

DESCRIPTION
  This function is used to handle reject reponse from PDE. The session proceeds
  only when we are rejected for SA data, for all other cases we abort the session.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_cp_handle_rej_resp
(
  tm_1x_cp_rlink_req_type           reject_req,
  byte                              sess_end
)
{
  tm_post_data_payload_type        *aiding_data_ptr = NULL;

  if ( !tm_1x_cp_session_active()) 
  {
    MSG_ERROR("Reject resp %d not expected", reject_req, 0, 0);
    return;
  }

  switch( reject_req )
  {
   case TM_1X_CP_RLINK_REQ_LR:

    tm_1x_cp_session_info.is801_req_wait_bmap &= ~(TM_1X_CP_RLINK_REQ_LR);
    MSG_ERROR("LR resp rejected",0,0,0);
    if(sess_end != 0x01)
    {
      /* No other request pending, so clear the timer.
      */
      (void)os_TimerStop(tm_1x_cp_session_info.tm_1x_cp_is801_timer);  
    }
    else
    {
      MSG_HIGH("Ending 1X CP session",0,0,0);
      tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE,TM_STOP_REASON_PDE_REJECT);
    }
    break;

  case TM_1X_CP_RLINK_REQ_AA:
    tm_1x_cp_session_info.is801_req_wait_bmap &= ~(TM_1X_CP_RLINK_REQ_AA);    /*lint !e641 */
    tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE,TM_STOP_REASON_PDE_REJECT);
    break;

  case TM_1X_CP_RLINK_REQ_GPS_SA:
    MSG_HIGH("SA request Rejected",0,0,0);

    if ( (tm_1x_cp_session_info.is801_req_wait_bmap & TM_1X_CP_RLINK_REQ_GPS_SA) == 0 )
    {
      MSG_ERROR("Reject not expected???",0,0,0);
      return;
    }

    tm_1x_cp_session_info.is801_req_wait_bmap &= ~(TM_1X_CP_RLINK_REQ_GPS_SA);
    if ( tm_1x_cp_session_info.is801_req_wait_bmap == NULL )
    {
      /* No further IS801 requests's are pending, so clear the IS801 timer.
      */
      (void)os_TimerStop(tm_1x_cp_session_info.tm_1x_cp_is801_timer);  
    }
    /* MGP is waiting for SA, proceed with rejecting the request and
    ** allow the session to complete.
    */
    if (tm_1x_cp_session_info.mgp_state.prm_on != TRUE)
    {
      MSG_ERROR("SA reject Not expected..??",0,0,0);
      return;
    }

    aiding_data_ptr = (tm_post_data_payload_type *)pdsm_getbuf(sizeof(tm_post_data_payload_type));

    if(aiding_data_ptr == NULL)
    {
      MSG_ERROR("No buffer for aiding data",0,0,0);
      tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE,TM_STOP_REASON_GENERAL_ERROR);
      return;
    }
    aiding_data_ptr->sa_data.u_NumSvs = 0;
    MSG_HIGH("Give SA data to TM-Core",0,0,0);
    if(tm_post_data(TM_PRTL_TYPE_1X_CP,
              TM_SESS_HANDLE_1X_CP_MT,
              TM_POST_DATA_TYPE_SA,
              aiding_data_ptr) == FALSE)
    {
      MSG_ERROR("Post data for SA failed",0,0,0);
    }
    (void)pdsm_freebuf((char *)aiding_data_ptr);
    break;

  default:
    MSG_ERROR ("Bad Rej cmd %d", reject_req, 0, 0);
    break;
  } /* End switch for reject reason */
  return;
} /* End tm_1x_cp_handle_is801_1_rej_resp() */

/*===========================================================================

FUNCTION tm_1x_cp_handle_aa_resp

DESCRIPTION
  This function is used to handle AA response from PDE. The function performs
  checks on parameters passed, checks for correct state, clears any pending 
  timers, sets up required buffers and invokes i_tm_1x_cp_post_aa
  to post the aa data to mgp

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_cp_handle_aa_resp
( 
  is801_fwd_link_resp_s_type *is801_1_aa,
  is801b_flink_ie_gnss_aa    *is801_b_aa
)
{
  tm_post_data_payload_type        *aiding_data_ptr = NULL;
  boolean                           unsolicited     = FALSE;

  MSG_HIGH("Handling AA data...",0,0,0);

  if(tm_1x_cp_session_active() != TRUE)
  {
    MSG_ERROR("1X CP session not active. Ignore AA resp",0,0,0);
  }

  /* Initialize whether the resp was unsolicited/solicited.
   * perform sanity check of pde version and aa response being consistent
   * (both 801-1 or both 801-B)
   */
  if( (tm_1x_cp_session_info.pde_version == IS801_VERSION_801_1) &&
      (is801_1_aa != NULL))
  {
    unsolicited = (is801_1_aa->unsolicited == 0) ? FALSE : TRUE;
  }
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif
  else
  {
    MSG_ERROR("Inconsistent state pde version %d 801_1 aa %x 801b aa %x",
                  tm_1x_cp_session_info.pde_version, is801_1_aa, is801_b_aa);
    tm_1x_cp_terminate_session( TM_1X_CP_TERM_SRC_NONE, 
                                TM_STOP_REASON_GENERAL_ERROR );
    return;
  }

  /*************** PDE version and AA response are consistent ***************/

  /* check if we were waiting for AA data, if waiting clear the req_wait_bmap and
   * if we were only waiting for AA data, clear the IS801 timer as well */
  if( (tm_1x_cp_session_info.is801_req_wait_bmap & TM_1X_CP_RLINK_REQ_AA) &&
      (unsolicited == FALSE))
  {
    tm_1x_cp_session_info.is801_req_wait_bmap &= ~(TM_1X_CP_RLINK_REQ_AA);
    if ( tm_1x_cp_session_info.is801_req_wait_bmap == NULL )
    {
      /* No further IS801 requests's are pending, so clear the IS801 timer.
      */
      (void)os_TimerStop(tm_1x_cp_session_info.tm_1x_cp_is801_timer);  
    }
  }
  else if (unsolicited == TRUE)
  {
    MSG_HIGH("Rxed Unsol AA resp",0,0,0);
  }
  else
  {
    MSG_ERROR("Rxed Unexpected AA resp",0,0,0);
  }

  /* allocate buffers for data to be posted to MGP, abort session if no buffer */
  aiding_data_ptr = 
    (tm_post_data_payload_type *)pdsm_getbuf(sizeof(tm_post_data_payload_type));
  if(aiding_data_ptr == NULL)
  {
    MSG_ERROR("Fail to get buffer", 0, 0, 0);
    tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE,TM_STOP_REASON_GENERAL_ERROR);
    return;
  }

  /*********** now that we have the buffers, post them to mgp ***********/
  i_tm_1x_cp_post_aa(aiding_data_ptr, 
                     is801_1_aa, 
                     is801_b_aa, 
                     cp_prefix_ppm_buff_ptr);

  if(cp_prefix_ppm_buff_ptr)
  {
    (void)pdsm_freebuf((char *)cp_prefix_ppm_buff_ptr);
    cp_prefix_ppm_buff_ptr = NULL;
  }
  (void)pdsm_freebuf((char *)aiding_data_ptr);
} /* End handle_aa_resp() */

#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
#error code not present
#endif

/*===========================================================================

FUNCTION tm_1x_cp_handle_sa_resp

DESCRIPTION
  This function is used to handle SA response from PDE

RETURN VALUE
  None

DEPENDENCIES
===========================================================================*/

void tm_1x_cp_handle_sa_resp
( 
  is801_fwd_link_resp_s_type *is801_1_sa,
  is801b_flink_ie_gps_sa     *is801_b_sa
)
{
  tm_post_data_payload_type               *aiding_data_ptr   = NULL;
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif
  is801_sa_resp_type                      *sa_resp           = NULL;

  /* SA data parameters */
  uint16  ref_bit_num       = 0;   /* Reference bit number   */
  byte    dr_size           = 0;   /* Data record size       */
  uint8   num_data_records  = 0;   /* number of data records */
  boolean unsolicited       = FALSE;

  MSG_HIGH("Handling SA data...",0,0,0);

  /* Check if we had requested for SA data and if the response is Solicited.
  */
  if(tm_1x_cp_session_active() == FALSE)
  {
    MSG_ERROR("1X CP session not active. Ignore SA resp",0,0,0);
  }

  /* Initialize whether the resp was unsolicited/solicited.
   * perform sanity check of pde version and aa response being consistent
   * (both 801-1 or both 801-B)
   */
  if( (tm_1x_cp_session_info.pde_version == IS801_VERSION_801_1) &&
      (is801_1_sa != NULL))
  {
    sa_resp          = &is801_1_sa->resp_msg_payload.sa_resp;
    unsolicited      = (is801_1_sa->unsolicited == 0) ? FALSE : TRUE;
    num_data_records = sa_resp->num_data_records;
    ref_bit_num      = sa_resp->data[num_data_records].ref_bit_num;
    dr_size          = sa_resp->data[num_data_records].dr_size;
  }
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif
  else
  {
    MSG_ERROR("Inconsistent state pde version %d 801_1 aa %x 801b aa %x",
                  tm_1x_cp_session_info.pde_version, is801_1_sa, is801_b_sa);
    tm_1x_cp_terminate_session( TM_1X_CP_TERM_SRC_NONE, 
                                TM_STOP_REASON_GENERAL_ERROR );
    return;
  }
  
  /* Report EVENT SA received
  */
  sm_report_event_gps_sa_received(ref_bit_num, dr_size);

  /* check if we were waiting for SA data, if waiting clear the req_wait_bmap and
   * if we were only waiting for SA data, clear the IS801 timer as well */
  if(tm_1x_cp_session_info.is801_req_wait_bmap & TM_1X_CP_RLINK_REQ_GPS_SA)
  {
    tm_1x_cp_session_info.is801_req_wait_bmap &= ~(TM_1X_CP_RLINK_REQ_GPS_SA);
    if ( tm_1x_cp_session_info.is801_req_wait_bmap == NULL )
    {
      /* No further IS801 requests's are pending, so clear the IS801 timer.
      */
      (void)os_TimerStop(tm_1x_cp_session_info.tm_1x_cp_is801_timer);  
    }
  }

  if (unsolicited)
  {
    MSG_MED("Rxed unsol SA. Dump it",0,0,0);
    return;
  }

  /* If PRM is still on we should hand over the
  ** data to lower layers, else it is too late.
  */
  if ( tm_1x_cp_session_info.mgp_state.prm_on != TRUE )
  {
    /* PRM not ON */
    MSG_ERROR("PRM not on, ignore SA data",0,0,0);
  }

  aiding_data_ptr = 
    (tm_post_data_payload_type *)pdsm_getbuf(sizeof(tm_post_data_payload_type));

  if(aiding_data_ptr == NULL)
  {
    MSG_ERROR("No buffer for aiding data",0,0,0);
    tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE,TM_STOP_REASON_GENERAL_ERROR);
    return;
  }

  if(tm_1x_cp_session_info.pde_version == IS801_VERSION_801_1)
  {
    (void)is801_util_flink_convert_sa( 
              &(is801_1_sa->resp_msg_payload.sa_resp),
              &(aiding_data_ptr->sa_data));
  }
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif

  MSG_HIGH("Give SA data to TM-Core",0,0,0);
  if( tm_post_data(TM_PRTL_TYPE_1X_CP,
                   TM_SESS_HANDLE_1X_CP_MT,
                   TM_POST_DATA_TYPE_SA,
                   aiding_data_ptr) == FALSE )
  {
    MSG_ERROR("Post data for SA failed",0,0,0);
  }
  (void)pdsm_freebuf((char *)aiding_data_ptr);
  return;
} /* End tm_1x_cp_handle_sa_resp() */

/*===========================================================================

FUNCTION tm_1x_cp_handle_lr_resp

DESCRIPTION
  This function is used to handle LR response from PDE.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_cp_handle_lr_resp
(
  const is801_fwd_link_pddm_info_s_type *lr_resp_801_1,
  is801b_flink_ie_adv_lr                *lr_resp_801_b
)
{
  boolean sess_end = FALSE;
  MSG_HIGH("Rxed LR data from PDE",0,0,0);

  if(tm_1x_cp_session_active() == FALSE)
  {
    MSG_ERROR("1X CP session not active, ignore LR",0,0,0);
  }

  /* Initialize whether the resp was unsolicited/solicited.
   * perform sanity check of pde version and aa response being consistent
   * (both 801-1 or both 801-B)
   */
  if( (tm_1x_cp_session_info.pde_version == IS801_VERSION_801_1) &&
      (lr_resp_801_1 != NULL))
  {
    sess_end = (lr_resp_801_1->hdr.sess_end != 0) ? 
                    TRUE : FALSE;
  }
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif
  else
  {
    MSG_ERROR("Inconsistent state pde version %d 801_1 aa %x 801b aa %x",
                  tm_1x_cp_session_info.pde_version, lr_resp_801_1, lr_resp_801_b);
    tm_1x_cp_terminate_session( TM_1X_CP_TERM_SRC_NONE, 
                                TM_STOP_REASON_GENERAL_ERROR );
    return;
  }
  
  if(lr_resp_801_1 == NULL)
  {
    MSG_ERROR("Null lr_resp_801_1",0,0,0);
    tm_1x_cp_terminate_session( TM_1X_CP_TERM_SRC_NONE,
                                TM_STOP_REASON_GENERAL_ERROR);
    return;
  }

  /* Send an event to indicate that we got a fix */
  event_report(EVENT_GPS_PD_POSITION);

  /* Generate CGPS event */
  sm_log_event_report(SM_LOG_EVENT_ID_GPS_COMPLETE,0);

  if(tm_1x_cp_session_info.is801_req_wait_bmap & TM_1X_CP_RLINK_REQ_LR )
  {
    /* Clear the request bit corsp to LR request.
    */
    tm_1x_cp_session_info.is801_req_wait_bmap &= ~(TM_1X_CP_RLINK_REQ_LR );
  }
  if (tm_1x_cp_session_info.is801_req_wait_bmap != NULL)
  {
    MSG_ERROR("req_wait=%d, should be NULL",
                tm_1x_cp_session_info.is801_req_wait_bmap,0,0);
  }
  else
  {
    /* Clear the IS801 response timer.
    */
    (void)os_TimerStop(tm_1x_cp_session_info.tm_1x_cp_is801_timer);  
  }

  if (sess_end)
  {
    MSG_HIGH("Session Ended",0,0,0);
    tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE,TM_STOP_REASON_COMPLETED);
  } /* end sess_end */
  else
  {
    /* No session End, remain in active session till timeout.
    */
    MSG_HIGH("End=0,remain in Session",0,0,0);
  }
} /* End tm_1x_cp_handle_lr_resp() */

/*===========================================================================

FUNCTION: tm_1x_cp_handle_send_success

DESCRIPTION:
  
PARAMETERS:
  
===========================================================================*/
void tm_1x_cp_handle_send_success(void)
{
  MSG_MED("Handle DBM send success. Sess_state = %d",tm_1x_cp_session_info.sess_state,0,0);
#ifdef FEATURE_CGPS_RELEASE_PD_CALL
  if (tm_1x_cp_session_info.release_pd_call == TRUE) 
  {
    MSG_MED("Releasing PD call immediately",0,0,0);
    tm_1x_cp_session_info.release_pd_call = FALSE; 

    (void)tm_cm_iface_stop_req(TM_PRTL_TYPE_1X_CP,NULL );
  }
#endif 
}

/*===========================================================================

FUNCTION: i_tm_1x_cp_handle_is801_1_resp

DESCRIPTION: This function handles IS801-1 responses at any time (start of 
             session or in an ongoing session). By invoking
             the specific resp handler functions AA, SA, LR and REJ responses
             are handled.

             Unexpected responses are dropped on the floor
  
PARAMETERS:
  
===========================================================================*/
static void i_tm_1x_cp_handle_is801_1_resp(void)
{
  uint8    j;
  is801_fwd_link_pddm_info_s_type  *p_fl_pddm  = NULL;
  is801_rej_resp_type              *rej_resp   = NULL;
  tm_1x_cp_rlink_req_type           reject_req = TM_1X_CP_RLINK_REQ_NONE;

  for(  j = 0; 
        (j < tm_1x_cp_session_info.fwd_link_pddm.hdr.num_responses)  && 
        (j < MAX_RESP_PER_PDDM); 
        j++
     )
  {
    p_fl_pddm = &tm_1x_cp_session_info.fwd_link_pddm;

    switch(tm_1x_cp_session_info.fwd_link_pddm.resp_rd[j].resp_type)
    {
      case IS801_FWD_LINK_RESP_MASK_AA:
        tm_1x_cp_handle_aa_resp(&(p_fl_pddm->resp_rd[j]), NULL);
        break;

      case IS801_FWD_LINK_RESP_MASK_SA:
        tm_1x_cp_handle_sa_resp(&(p_fl_pddm->resp_rd[j]), NULL);
        break;

      case IS801_FWD_LINK_RESP_MASK_LR:
        tm_1x_cp_handle_lr_resp(p_fl_pddm, NULL);
        break;

      case IS801_FWD_LINK_RESP_MASK_REJ:
        /* Translate the IS801-1 reject response into the generic 1xCP Req
         * that the resp corresponds to */
        rej_resp   = &p_fl_pddm->resp_rd[j].resp_msg_payload.rej_resp;
        reject_req = tm_1x_cp_xlate_is801_1_rlink_req_to_1x(
                       (ms_to_pde_req_type) rej_resp->rej_req_type);
  
        if(reject_req != TM_1X_CP_RLINK_REQ_NONE)
        {
          tm_1x_cp_handle_rej_resp(reject_req, p_fl_pddm->hdr.sess_end);
        }
        break;

      default:        
        MSG_MED("Unexpected resp_type %d",
                  p_fl_pddm->resp_rd[j].resp_type, 0, 0);
        break;
    }
  }
}

/*===========================================================================

FUNCTION: i_tm_1x_cp_handle_is801_1_req

DESCRIPTION: This function handles IS801-1 requests
             Only PRM and PPM reqs are handled, rest are dropped on the floor
             by this function

PARAMETERS:
  
===========================================================================*/
void i_tm_1x_cp_handle_is801_1_req
(
  boolean              sess_start,
  tm_1x_cp_fl_req_type req_mask
)
{
  uint8 j;
  is801_fwd_link_req_s_type    *req_rd         = NULL;
  tm_1x_cp_fl_req_type          cp_req_type    = 0x0;
  is801_fwd_link_req_mask_type  pddm_req_type  = 0x0;

  /* Requests */
  for(j = 0; j < tm_1x_cp_session_info.fwd_link_pddm.hdr.num_requests; j++)
  {
    req_rd = &tm_1x_cp_session_info.fwd_link_pddm.req_rd[j];

    pddm_req_type  = req_rd->request_type;
    cp_req_type = xlate_fl_req_mask_is801_1_to_1x_cp( &pddm_req_type );

    /* Check if this req_type is marked for CANCEL */
    if(cp_req_type & tm_1x_cp_session_info.cancel_req_bmap)
    {
      MSG_MED("Req 0x%x marked for CANCEL", cp_req_type, 0, 0);
      continue;
    }
    
    switch(req_rd->request_type)
    {
      case IS801_FWD_LINK_REQ_MASK_PRM:
        MSG_MED("PRM is requested",0,0,0);
        tm_1x_cp_handle_prm_req(req_rd->req_msg_payload.prm_req.pref_resp_qual);
        break;

      case IS801_FWD_LINK_REQ_MASK_PPM:
        MSG_MED("PPM is requested",0,0,0);
        tm_1x_cp_handle_ppm_req();
        if(sess_start == FALSE)
        {
          /* If only ppm was requested, then start a 2s timer */
          if (!(req_mask & TM_1X_CP_FL_REQ_PRM)) 
          {
            MSG_MED("Only PPM is requested, start 2s timer",0,0,0);
            if(!os_TimerStart(tm_1x_cp_session_info.tm_1x_cp_ppm_trigger_timer,2*1000,NULL))
            {
              MSG_ERROR("Failed starting PPM trigger timer",0,0,0);
            }
          }
        }
        break;


      default:
        MSG_MED("req_type %d not handled",req_rd->request_type, 0, 0);
        break;
    }
  }
} /* end i_tm_1x_cp_handle_is801_1_req */

#if 0
/*===========================================================================

FUNCTION: i_tm_1x_cp_handle_is801_1_req_in_ongoing_session

DESCRIPTION: This function handles IS801-1 requests at session start.
             Only PRM and PPM reqs are handled, rest are dropped on the floor
             by this function

PARAMETERS:
TBD: Merge with   i_tm_1x_cp_handle_is801_1_req_at_sess_start. Only thing
thats different in both is starting a 2 sec timer.
===========================================================================*/
void i_tm_1x_cp_handle_is801_1_req_in_ongoing_session
(
  tm_1x_cp_fl_req_type req_mask
)
{
  uint8 j;
  is801_fwd_link_req_s_type    *req_rd         = NULL;
  tm_1x_cp_fl_req_type          cp_req_type = 0x0;
  is801_fwd_link_req_mask_type  pddm_req_type  = 0x0;

  /* Requests */
  for(j = 0; j < tm_1x_cp_session_info.fwd_link_pddm.hdr.num_requests; j++)
  {
    req_rd = &tm_1x_cp_session_info.fwd_link_pddm.req_rd[j];

    pddm_req_type  = req_rd->request_type;
    cp_req_type = xlate_fl_req_mask_is801_1_to_1x_cp( &pddm_req_type );

    /* Check if this req_type is marked for CANCEL */
    if(cp_req_type & tm_1x_cp_session_info.cancel_req_bmap)
    {
      MSG_MED("Req 0x%x marked for CANCEL", cp_req_type, 0, 0);
      continue;
    }

    /* Check if PPM is requested */
    if (cp_req_type & TM_1X_CP_FL_REQ_PPM) 
    {
      tm_1x_cp_handle_ppm_req();
      /* If only ppm was requested, then start a 2s timer */
      if (!(req_mask & TM_1X_CP_FL_REQ_PRM)) 
      {
        MSG_MED("Only PPM is requested, start 2s timer",0,0,0);
        if(!os_TimerStart(tm_1x_cp_session_info.tm_1x_cp_ppm_trigger_timer,2*1000,NULL))
        {
          MSG_ERROR("Failed starting PPM trigger timer",0,0,0);
        }
      }
    } 
    /* Check if PRM is requested */
    else if (cp_req_type & TM_1X_CP_FL_REQ_PRM) 
    {
      MSG_MED("PRM is requested",0,0,0);
      tm_1x_cp_handle_prm_req(req_rd->req_msg_payload.prm_req.pref_resp_qual);
    }
    else
    {
      MSG_MED("Req 0x%x not handled", cp_req_type,0,0);
    }
  }

}

/*===========================================================================

FUNCTION: i_tm_1x_cp_handle_is801_b_req_at_sess_start

DESCRIPTION: This function handles IS801-B requests at session start.
             Only PRM and PPM reqs are handled, rest are dropped on the floor
             by this function

PARAMETERS:
  
===========================================================================*/
void i_tm_1x_cp_handle_is801_b_req_at_sess_start(void)
{
  uint32                         j        = 0x1;
  is801b_flink_ie_req           *pddm_ptr = NULL;
  is801B_fwd_link_req_mask_type  v1_req   = tm_1x_cp_session_info.is801_b_v1_req;
  is801B_fwd_link_req_mask_type  v3_req   = tm_1x_cp_session_info.is801_b_v3_req;

  pddm_ptr = (is801b_flink_ie_req *) tm_1x_cp_session_info.is801b_pddm_ptr; 

  if(pddm_ptr == NULL)
  {
    MSG_ERROR("NULL Is801B PDDM!!", 0, 0, 0);
    return;
  }

  /* We only handle two types of IS801B Requests in this function. 
   * 1) V1 Is801B PPM req
   * 2) V3 Is801B PRM req
  */
  if(v1_req & IS801B_FWD_LINK_REQ_MASK_PPM)
  {
    tm_1x_cp_handle_ppm_req();
    v1_req &= ~IS801B_FWD_LINK_REQ_MASK_PPM;
  }
  if(v3_req & IS801B_FWD_LINK_REQ_MASK_GNSS_PRM)
  {
    tm_1x_cp_handle_prm_req(pddm_ptr->prm_req.prefer_resp_qual);
    v3_req &= ~IS801B_FWD_LINK_REQ_MASK_GNSS_PRM;
  }

  /* Now step through all other requests to see what else might be there 
   * These reqs are not handled, simply print a diag msg
   */
  while(j != 0)
  {
    if(j & v1_req)
    {
      /* Check if this req_type is marked for CANCEL */
      if(j & tm_1x_cp_session_info.cancel_req_bmap)
      {
        MSG_MED("Req 0x%x marked for CANCEL", j, 0, 0);
      }
      else
      {
        MSG_MED("req_type %d not handled", (j & v1_req), 0, 0);
      }
    }

    if(j & v3_req)
    {
      /* Check if this req_type is marked for CANCEL */
      if(j & tm_1x_cp_session_info.cancel_req_bmap)
      {
        MSG_MED("Req 0x%x marked for CANCEL", j, 0, 0);
      }
      else 
      {
        MSG_MED("req_type %d not handled",
                   (j & tm_1x_cp_session_info.is801_b_v3_req), 0, 0);
      }
    }

    j = j << 1;
  }

  return;
}
#endif // #if 0

#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
#error code not present
#endif /* FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE */

/*===========================================================================

FUNCTION: tm_1x_cp_handle_is801_req_resp_at_sess_start

DESCRIPTION: This function handles IS801-1 req/responses at session start.
             Only PRM and PPM reqs are handled, rest are dropped on the floor
             by this function

             responses are passed on to i_tm_1x_cp_handle_is801_1_resp for
             handling
  
PARAMETERS:
  
===========================================================================*/
void tm_1x_cp_handle_is801_req_resp_at_sess_start
(
  tm_1x_cp_fl_req_type *fl_req
)
{
  if( tm_1x_cp_session_info.pde_version == IS801_VERSION_801_1 )
  {
    i_tm_1x_cp_handle_is801_1_req(TRUE, *fl_req);
    i_tm_1x_cp_handle_is801_1_resp();
  }
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif
  else
  {
    MSG_ERROR("Unknown pde_version %d", tm_1x_cp_session_info.pde_version, 0, 0);
  }
  return;
}

/*===========================================================================

FUNCTION: tm_1x_cp_handle_req_resp_during_session

DESCRIPTION: 1xCP handles Is801 requests and responses (both -1 and -B) in 
             this function.

             1) Cancel requests are handled
             2) MSI/MSI requests are handled
             3) Requests not handled are rejected
             4) PRM and PPM requests are handled
             5) Everything that remains is dropped on the floor.
  
PARAMETERS:
  
===========================================================================*/
void tm_1x_cp_handle_req_resp_during_session(void)
{
  boolean    send_cack    = FALSE;
  boolean    send_msi     = FALSE;
  boolean    send_reject  = FALSE;

  tm_1x_cp_fl_req_type      *req_mask      = &tm_1x_cp_session_info.fl_req_mask;

  /* Handle CANCEL, Initialize any reqs to be cancelled in cancel_req_bmap */
  if(*req_mask & TM_1X_CP_FL_REQ_CAN || *req_mask & TM_1X_CP_FL_REQ_ADV_CAN)
  {
    send_cack = tm_1x_cp_handle_cancel_request();
  }

  /* Create MSInfo/Adv MSInfo if requested */
  send_msi = i_tm_1x_cp_create_msi_resp( 
                  is801_1_resp_parm_ptr,
                 *req_mask,
                  tm_1x_cp_session_info.pde_version);

  /* Check if anything needs to be rejected
   * Reject requests other than MSI, PPM and PRM 
   */
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#else
  if( tm_1x_cp_session_info.is801_1_rej_req != 0x0 )
  {
    send_reject = tm_1x_cp_reject_is801_requests(
                    tm_1x_cp_session_info.is801_1_rej_req, 0x0, 0x0);

    *req_mask   = (*req_mask & TM_1X_CP_FL_REQ_MT);
  }
#endif
  

  /* Handle all Requests */
  if( tm_1x_cp_session_info.pde_version == IS801_VERSION_801_1 )
  {
    i_tm_1x_cp_handle_is801_1_req(FALSE, *req_mask);
  }
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif
  else
  {
    MSG_ERROR("Unknown PDE version %d", tm_1x_cp_session_info.pde_version, 0, 0);
  }

  if(send_cack || send_msi || send_reject)
  {
    MSG_MED("Sending Rlink PDDM",0,0,0);
    (void)i_tm_1x_cp_send_rlink_pddm(0, tm_1x_cp_session_info.pde_version);
  }

  /* Handle all Responses 
   */
  i_tm_1x_cp_handle_is801_1_resp();
}

/*===========================================================================

FUNCTION: i_tm_1x_cp_free_rlink_buffer

DESCRIPTION: This function frees a previously allocated IS801-1 or -B rlink 
             buffer.
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
  
===========================================================================*/
static void i_tm_1x_cp_free_rlink_buffer(void)
{
  if(is801_1_resp_parm_ptr)
  {
    (void) pdsm_freebuf((char *) is801_1_resp_parm_ptr);
    is801_1_resp_parm_ptr = NULL;
  }

#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif
}

/*===========================================================================
  
FUNCTION: i_tm_1x_cp_allocate_rlink_buffer

DESCRIPTION: This function allocates an IS801-1 or -B rlink buffer, depending
             on the type of session we are in. If a buffer can not be allocated
             the ongoing session is terminated.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE:      Buffer allocated
  FALSE:     Could not allocate buffer

SIDE EFFECTS
  None.
  
===========================================================================*/
static boolean i_tm_1x_cp_allocate_rlink_buffer(byte pde_version)
{
  boolean no_buf       = FALSE;

  if( tm_1x_cp_session_info.pde_version == IS801_VERSION_801_1 )
  {
    if(is801_1_resp_parm_ptr == NULL)
    {
      is801_1_resp_parm_ptr = 
        (is801_rev_link_resp_s_type *) pdsm_getbuf(sizeof(*is801_1_resp_parm_ptr));
      /* If we couldnt get a buffer, set no_buf flag to TRUE */
      no_buf   = is801_1_resp_parm_ptr ? FALSE : TRUE;
    }
  }
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif
  /* If no buffers available, terminate session */
  if(no_buf)
  {
    MSG_ERROR("No buffer for rlink buf",0,0,0);
    tm_1x_cp_terminate_session( TM_1X_CP_TERM_SRC_NONE,
                                TM_STOP_REASON_GENERAL_ERROR );
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================

FUNCTION: tm_1x_cp_handle_start_session

DESCRIPTION: This function looks at the initial requests that the PDE has sent
             and creates MSI, adv MSI, starts PPM and rejects everything else 
             apart from PRM. The funcion handles start session for both IS801-1
             and IS801-B
  
DEPENDENCIES
  None.

RETURN VALUE
  NONE

SIDE EFFECTS
  None.
  
===========================================================================*/
void tm_1x_cp_handle_start_session(void)
{
  boolean ret_val = FALSE;
  boolean send_reject  = FALSE; /* whether a rej response needs to be sent */
  boolean send_cack    = FALSE; /* whether a CACK resp needs to be sent    */
  boolean send_now     = FALSE; /* will be set to true if cack resp is to be
                                   sent or if session is not AFLT */
  boolean send_msinfo  = FALSE; /* whether an msi resp needs to be sent    */

  tm_1x_cp_fl_req_type   *req_mask = &tm_1x_cp_session_info.fl_req_mask;
  
  /* Session is active now, set new global state */
  tm_1x_cp_session_info.sess_active = TRUE;

  if(!os_TimerStart(tm_1x_cp_session_info.tm_1x_cp_sess_timer,
                    TM_1X_CP_SESSION_TIMEOUT,NULL))
  {
    MSG_ERROR("Failed starting session timer",0,0,0);
  }

  /* Handle CANCEL, Initialize any reqs to be cancelled in cancel_req_bmap 
   */
  if(*req_mask & TM_1X_CP_FL_REQ_CAN || *req_mask & TM_1X_CP_FL_REQ_ADV_CAN)
  {
    send_cack = tm_1x_cp_handle_cancel_request();
    send_now  = TRUE;
  }
  else
  {
    /* If ppm is on, and GPS or Glonass PRM has not been requested
     * but PPM and MSI have been requested, its an AFLT session */
    if ( (tm_1x_cp_session_info.mgp_state.ppm_on == TRUE) && 
         (!(*req_mask & TM_1X_CP_FL_REQ_PRM) ) && 
         ( (*req_mask & TM_1X_CP_FL_REQ_MT) == 
                       (TM_1X_CP_FL_REQ_PPM | TM_1X_CP_FL_REQ_MSI)) )
    {
      MSG_MED("Send MSInfo and PPM together. Start 500ms timer",0,0,0);
      send_now = FALSE;  
      /* Start 500ms timer */
      tm_1x_cp_session_info.ppm_timeout = TM_MGP_GET_PPM_TIMEOUT_MIN;
    }
    else
    {
      send_now = TRUE;
    }
  }
  
  /* Create MSInfo or Adv MSInfo if requested 
   */
  if ( (*req_mask & TM_1X_CP_FL_REQ_MSI)      || 
       (*req_mask & TM_1X_CP_FL_REQ_ADV_MSI) )
  {
    if(i_tm_1x_cp_create_msi_resp(
                  is801_1_resp_parm_ptr,
                 *req_mask,
                  tm_1x_cp_session_info.pde_version))
    {
      send_msinfo = TRUE;
    }
  }

  /* Start PPM if requested 
   */
  if(*req_mask & TM_1X_CP_FL_REQ_PPM) 
  {
    tm_1x_cp_handle_ppm_req();
  }

  /* Check if anything needs to be rejected. The reject req mask is initialized
   * before this function is called. Reject all requests other than MSI, PPM 
   * and PRM */
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#else
  if( tm_1x_cp_session_info.is801_1_rej_req != 0x0 )
  {
    send_reject = tm_1x_cp_reject_is801_requests(
                    tm_1x_cp_session_info.is801_1_rej_req, 0x0, 0x0);

    *req_mask   = (*req_mask & TM_1X_CP_FL_REQ_MT);
  }
#endif

  /* If send_now is set to true (session is not AFLT or a CACK resp needs to 
   * be sent) AND any of reject/msi/cack responses need to be sent, send a pddm
   * now 
   */
  if(send_now && (send_reject || send_cack || send_msinfo))
  {
    MSG_MED("Sending PDDM now",0,0,0);
    if(!i_tm_1x_cp_send_rlink_pddm(0, tm_1x_cp_session_info.pde_version))
    {
      tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE, (tm_sess_stop_reason_e_type)PDSM_PD_END_IS801_SEND_FAILURE);
      return;
    }
  }

  /* If we rcvd no cancel reqs, only then proceed with handling ppm and prm */
  if(send_cack == FALSE)
  {
    /* Check if we are connected */
    if(tm_cm_iface_start_req(TM_PRTL_TYPE_1X_CP,NULL) == TRUE)
    {
      /* Handle rest of the IS801 req & resp */
      tm_1x_cp_handle_is801_req_resp_at_sess_start(&tm_1x_cp_session_info.fl_req_mask);
    }
    else
    {
      MSG_HIGH("TM CM Iface rejects start req",0,0,0);
      /*  Send Reject to all requests other than MSInfo as that resp is already created */
      ret_val = tm_1x_cp_reject_is801_requests((tm_1x_cp_session_info.fwd_link_pddm.req_mask) & (~((IS801_FWD_LINK_REQ_MASK_MSI))), 0, 0);
      if(ret_val)
      {
        (void)i_tm_1x_cp_send_rlink_pddm(0, tm_1x_cp_session_info.pde_version);
      }
    }
  }
} /* tm_1x_cp_handle_start_session */

#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
#error code not present
#endif
/*===========================================================================

FUNCTION    : i_tm_1x_cp_sess_start_allowed

DESCRIPTION : Check if a session can be started. If a session can be started
              change session state to TM_1X_CP_SESS_STATE_WAIT and invoke
              handle_start_session.
DEPENDENCIES

ARGUMENTS
RETURN VALUE
TRUE        : Session can be started
FALSE       : Session could not be started.

SIDE EFFECTS
===========================================================================*/
boolean i_tm_1x_cp_sess_start_allowed(uint8 sess_tag)
{
  MSG_HIGH("1X CP session trigger. sess_tag=%d", sess_tag, 0, 0);

  if(tm_1x_cp_session_info.tm_core_session_started == FALSE)
  {
    /* Check with TM-Core if session is allowed or not.
    */
    if( !tm_1x_cp_start_sess_req() )
    {
      MSG_ERROR("Unable to start 1X CP session",0,0,0);
#ifdef FEATURE_CGPS_RELEASE_PD_CALL
      if(tm_cm_iface_is_pd_call_connected() == TRUE)
      {
        /* Release PD call once L2 Ack for end session is received */
        tm_1x_cp_session_info.release_pd_call = TRUE;
      }
#endif
      return FALSE;
    }
  }

  /* Session is allowed */
  tm_1x_cp_session_info.release_pd_call = FALSE;

  tm_1x_cp_session_info.sess_state = TM_1X_CP_SESS_STATE_WAIT;
  return TRUE;
}
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
#error code not present
#endif

/*===========================================================================

FUNCTION: tm_1x_cp_is801_1_msg_cb

DESCRIPTION:
  This function is called by IS801 module to inform
  PDDM data is available.
  
DEPENDENCIES
  None.

RETURN VALUE
  prtl_start_sess_req_result_e_type

SIDE EFFECTS
  None.
  
===========================================================================*/
boolean tm_1x_cp_is801_1_msg_cb
(
  is801_fwd_pddm_info_s_type     pddm_info
)
{
  MSG_HIGH("# of PDDM = %d ", pddm_info.num_pddm, 0, 0);

  tm_1x_cp_session_info.num_is801_1_pddm = pddm_info.num_pddm;
  return TRUE;
}

/*===========================================================================

FUNCTION: tm_1x_cp_proc_is801_1_msg

DESCRIPTION:
  This function will process decoded IS801-1 pddms.
  
DEPENDENCIES
  None.

RETURN VALUE
  prtl_start_sess_req_result_e_type

SIDE EFFECTS
  None.
  
===========================================================================*/
boolean tm_1x_cp_proc_is801_1_msg
(
  uint8 num_pddm
)
{
  uint8 i,j;
  boolean ret_val = FALSE;
          
  MSG_HIGH("Processing IS801-1 PDDMs", 0, 0, 0);

  /* Get and process all PDDMs, one after the other 
   */
  for(i = 0; i < num_pddm; i++)
  {
    if(!is801_get_fwd_link_pddm( TM_PRTL_TYPE_1X_CP,  
                                &tm_1x_cp_session_info.fwd_link_pddm ))
    {
      MSG_ERROR("IS801 failed to get Fwd Link PDDM",0,0,0);
      if( tm_1x_cp_session_active() == TRUE )
      {
        tm_1x_cp_terminate_session( TM_1X_CP_TERM_SRC_NONE, 
                                    TM_STOP_REASON_GENERAL_ERROR );
      }
      return FALSE;
    }

    /* Init the req and reject req masks */
    tm_1x_cp_session_info.is_801_1_req      = 
             tm_1x_cp_session_info.fwd_link_pddm.req_mask;
    tm_1x_cp_session_info.is801_1_rej_req  = 
             tm_1x_cp_session_info.fwd_link_pddm.req_mask;

    /* xlate 801-1 req/resp mask into 1x_cp (generalized for 801-1/B) req mask 
     */
    tm_1x_cp_session_info.fl_req_mask  = 
       xlate_fl_req_mask_is801_1_to_1x_cp(  
         &tm_1x_cp_session_info.is801_1_rej_req);

    tm_1x_cp_session_info.fl_resp_mask =
       xlate_fl_resp_mask_is801_1_to_1x_cp(  
         tm_1x_cp_session_info.fwd_link_pddm.resp_mask);

    /* Initialize session tag and session source */
    last_pddm_hdr.sess_tag    = tm_1x_cp_session_info.fwd_link_pddm.hdr.sess_tag;
    last_pddm_hdr.sess_source = tm_1x_cp_session_info.fwd_link_pddm.hdr.sess_source;

    MSG_HIGH("Got %d Fwd Link Req. Req Mask 0x%x", 
              tm_1x_cp_session_info.fwd_link_pddm.hdr.num_requests, 
              tm_1x_cp_session_info.fwd_link_pddm.req_mask, 0); 
    MSG_HIGH("Got %d Fwd Link Resp. Resp Mask 0x%x", 
              tm_1x_cp_session_info.fwd_link_pddm.hdr.num_responses, 
              tm_1x_cp_session_info.fwd_link_pddm.resp_mask, 0); 

    /* Report IS801 event for requests */
    for(j = 0; j < tm_1x_cp_session_info.fwd_link_pddm.hdr.num_requests; j++)
    {
      is801_util_report_is801_event_rx(
                   IS801_REQ, 
                   tm_1x_cp_session_info.fwd_link_pddm.req_rd[j].request_type);
    }

    /* Report IS801 event for responses */
    for(j = 0; j < tm_1x_cp_session_info.fwd_link_pddm.hdr.num_responses; j++)
    {
      is801_util_report_is801_event_rx(
                   IS801_RESP, 
                   tm_1x_cp_session_info.fwd_link_pddm.resp_rd[j].resp_type);
    }

    /* If required, Dynamically allocate a rev link response buffer for 801-1. 
     * Even if we reject the session, this may still be needed to send reject 
     * msgs. If no buffers are available, the function will terminate the sess. 
     */
    if(!i_tm_1x_cp_allocate_rlink_buffer(tm_1x_cp_session_info.pde_version))
    {
      MSG_ERROR("Could not allocate buffer for reject msg", 0, 0, 0);
      return TRUE;
    }
    
    /* A pddm can be proccessed only if it has a sess_start flag set, or 1x cp
     * is already in an active session, or it has a sess_end flag set. everything
     * else is to be rejected as an unexpected call flow */
    if(tm_1x_cp_session_info.fwd_link_pddm.hdr.sess_start == 1)
    {
      if(i_tm_1x_cp_sess_start_allowed(last_pddm_hdr.sess_tag) == FALSE)
      {
        ret_val = tm_1x_cp_reject_is801_requests(
                    tm_1x_cp_session_info.is_801_1_req, 0, 0);
        if(ret_val)
        {
          (void)i_tm_1x_cp_send_rlink_pddm(1, tm_1x_cp_session_info.pde_version);
        }
        i_tm_1x_cp_free_rlink_buffer();
        return TRUE;
      }
      tm_1x_cp_send_pd_comm_event (PDSM_PD_EVENT_COMM_BEGIN);
      /* session start allowed, handle request responses */
      tm_1x_cp_handle_start_session();
    } /* End if sess_start == 1, try handling ongoing session */
    else if ( tm_1x_cp_session_active() == TRUE )
    { 
      MSG_HIGH("Handle req & resp during session",0,0,0);
      tm_1x_cp_handle_req_resp_during_session();
    } /* not an ongoing session, check if sess_end flag is set */
    else if (tm_1x_cp_session_info.fwd_link_pddm.hdr.sess_end == 0x01 ) 
    {
      MSG_HIGH("PDE requesting for End session",0,0,0);
      if( tm_1x_cp_session_active() == TRUE )
      {
        tm_1x_cp_terminate_session( TM_1X_CP_TERM_SRC_NONE,
                                    TM_STOP_REASON_PDE_END_SESSION );
      }
    }
    else
    {
      MSG_HIGH("Unexpected call flow, rejecting requests",0,0,0);
      ret_val = tm_1x_cp_reject_is801_requests(
                    tm_1x_cp_session_info.is_801_1_req, 0, 0);
      if(ret_val)
      {
        (void)i_tm_1x_cp_send_rlink_pddm(0, tm_1x_cp_session_info.pde_version);
      }
      i_tm_1x_cp_free_rlink_buffer();
    }
  } /* for(i = 0; i < num_pddm; i++) */
  return TRUE;
}

/*===========================================================================

FUNCTION tm_1x_cp_start_sess_req_handler

DESCRIPTION
This function is a callback registered with tm core, that is invoked if tm 
core wants to start a session. This function always returns 
SESS_REQ_NOT_ALLOWED since no MO 1xCP sessions are supported.
DEPENDENCIES
  None.

RETURN VALUE
  prtl_start_sess_req_result_e_type

SIDE EFFECTS
  None.

===========================================================================*/
prtl_start_sess_req_result_e_type tm_1x_cp_start_sess_req_handler
(
  tm_sess_handle_type               sess_handle,
  prtl_start_sess_req_param_s_type  *start_sess_req_param,
  tm_sess_req_start_param_s_type    *action_ptr
)
{
  (void)sess_handle;
  (void)start_sess_req_param;
  (void)action_ptr;
  return PRTL_START_SESS_REQ_NOT_ALLOWED;
}

/*===========================================================================

FUNCTION tm_1x_cp_handle_prm_timeout

DESCRIPTION
  This function gets called once MGP timeout has expired.

RETURN VALUE

DEPENDENCIES

===========================================================================*/
void tm_1x_cp_handle_prm_timeout(void)
{

  MSG_HIGH("Handle PRM timeout",0,0,0);

  (void)tm_1x_cp_end_prm();
} /* End tm_1x_cp_handle_prm_timeout() */

/*===========================================================================

FUNCTION tm_1x_cp_stop_sess_req_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_1x_cp_stop_sess_req_handler
(
  tm_sess_handle_type           sess_handle,
  prtl_sess_stop_type           stop_type,  
  tm_sess_stop_reason_e_type    stop_reason
)
{
  boolean ret_val = FALSE;
  (void)stop_type;

  MSG_HIGH("Handle stop session rcvd from TM-Core. Reason %d",stop_reason,0,0);
  if (sess_handle == TM_SESS_HANDLE_NONE)
  {
    ret_val = TRUE;
    return ret_val;
  }

  switch(stop_reason)
  {
    case TM_STOP_REASON_PRM_TIMEOUT:
      if (tm_1x_cp_session_active() && (tm_1x_cp_session_info.mgp_state.prm_on))
      {
        tm_1x_cp_handle_prm_timeout();
        /* Return FALSE so that TM_Core does not abort the session */
        ret_val = FALSE;
      }
      break;

    case TM_STOP_REASON_PPM_TIMEOUT:
      tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_TM_CORE,TM_STOP_REASON_PPM_TIMEOUT);
      ret_val = TRUE;
      break;

    case TM_STOP_REASON_GENERAL_ERROR:
    default:
      tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_TM_CORE,stop_reason);
      ret_val = TRUE;
      break;
  }
  return ret_val;
}

/*===========================================================================


FUNCTION tm_1x_cp_sess_req_data_handler

DESCRIPTION

DEPENDENCIES 
  None.

RETURN VALUE

SIDE EFFECTS
 None.
===========================================================================*/

boolean tm_1x_cp_sess_req_data_handler

(
  tm_sess_handle_type  session_handle,
  prtl_data_req_type   req_data
)
{
  (void)session_handle;

  if(req_data & PRTL_AIDING_DATA_REQ_SA)
  {
    tm_1x_cp_handle_sa_req();
    return TRUE;
  }
  else
  {
    MSG_ERROR("REq for data %d not handled", req_data,0,0);
    return FALSE;
  }
}

/*===========================================================================
FUNCTION tm_1x_cp_sess_info_handler

DESCRIPTION: This function handles information from tm_core (that is sent 
             by mgp). Only PPM and PRM responses are handled, all other
             kinds of messages are dropped on the floor

DEPENDENCIES 
  None.

RETURN VALUE

SIDE EFFECTS
 None.
===========================================================================*/
boolean tm_1x_cp_sess_info_handler

(
  tm_sess_handle_type          session_handle,
  prtl_sess_info_e_type        sess_info_type,
  prtl_sess_info_param_u_type  *sess_info_param

)
{
    boolean ret_val = TRUE;
    (void)session_handle;
    if(sess_info_param == NULL)
    {
      MSG_ERROR("Null sess_info_param pointer", 0, 0, 0);   
      return FALSE;
    }

    switch(sess_info_type)
    {
      case PRTL_SESS_INFO_PPM:
        (void)tm_1x_cp_handle_ppm_resp(&(sess_info_param->ppm_data));
      break;

      case PRTL_SESS_INFO_PRM:
        if ( sess_info_param->prm_data.e_MeasRptReason == GPS_MEAS_RPT_MEAS_DONE )
        {
          (void) tm_1x_cp_handle_prm_resp(&(sess_info_param->prm_data));
        }
      break;

      default:
        ret_val = FALSE;
        MSG_ERROR("Unexpected sess_info %d",sess_info_type,0,0);
      break;
    }
    return ret_val;
}

/*===========================================================================
FUNCTION tm_1x_cp_data_pipe_handler

DESCRIPTION

DEPENDENCIES 
  None.

RETURN VALUE

SIDE EFFECTS
 None.

===========================================================================*/
boolean tm_1x_cp_data_pipe_handler
(
  pd_comms_client_ipc_message_type  msg_type,
  void                              *msg_data_ptr
)

{
  (void)msg_type;
  (void)msg_data_ptr;
  return TRUE;
}

/*===========================================================================

FUNCTION tm_1x_cp_handle_unknown_pde_version

DESCRIPTION
  This function is used to handle the first messages from the PDE, when its 
  not known what IS801 version the PDE supports. The function first passes the
  message from PDE to the Is801 decoder, and if any bytes are left to the 
  Is801B decoder. If any bytes are decoded by the Is801B decoder, the version
  the PDE is set to IS801B, otherwise IS801-1.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
#error code not present
#endif

/*===========================================================================

FUNCTION tm_1x_cp_proc_801_msgs

DESCRIPTION
  This function is used to process decoded 801 msgs. If both 801B and 801-1 
  msgs are decoded, and 801B is enabled, the callflow is handled as an 
  801B callflow and 801-1 msgs are dropped on the floor. 
  
  In all other cases, callflows are processed as 801-1 callflows.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
boolean tm_1x_cp_proc_801_msgs(void)
{
  boolean   ret_val = TRUE;

#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#else
  if(tm_1x_cp_session_info.num_is801_1_pddm)
  {
    MSG_MED("Process %d Is801-1 pddms", 
               tm_1x_cp_session_info.num_is801_1_pddm, 0, 0);
    (void) tm_1x_cp_proc_is801_1_msg(tm_1x_cp_session_info.num_is801_1_pddm);
  }
#endif
  else
  {
    MSG_ERROR("Could not decode Unknown pddm type", 0, 0, 0);
    ret_val = FALSE;
  }

  return ret_val;
}

/*===========================================================================

FUNCTION tm_1x_cp_handle_event_mt_msg

DESCRIPTION
  This function is used to handle an MT message from the PDE. If this is the 
  first message in a session, the PDE version is unknown and the message is
  checked for both IS801-1 and IS801-B PDDMs.

  If the PDE version is known because this message is not the first in the
  session, then its handed over to the respective decoder modules.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
boolean tm_1x_cp_handle_event_mt_msg(tm_cm_rcv_data_s_type *msg)
{
  boolean   ret_val = FALSE;
  byte      offset  = 0;

#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif

  MSG_LOW("handling mt msg. PDE version %d 801b support %d", 
             tm_1x_cp_session_info.pde_version,
             tm_1x_cp_config.support_is801b, 0);

 /* If IS801B is defined, then a version negotiation step must happen with the
  * first pddm sent by the PDE. If the PDE is an IS801B PDE, the first PDDM
  * will have IS801-1 encoded PDDMS, followed by IS801B encoded PDDMs.
  * 
  * If there are any IS801B encoded PDDMs in the first message sent by the PDE
  * to the MS, 1xCP shall start a IS801B call flow, and if not, 1xCP shall
  * start an IS801-1 callflow.
  */
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#else
  /* MS only supports Is801-1, so always fwd PDDM to 801-1 decoder */
  tm_1x_cp_session_info.pde_version = IS801_VERSION_801_1;
  is801_util_log_flink_msg((void *)msg->payload, msg->length);
  /* Forward it to IS801-1 for decoding */
  ret_val  = is801_decode_fwd_link_pddm ( 
                  TM_PRTL_TYPE_1X_CP, 
                  (byte *)msg->payload,
                  msg->length, 
                  &offset);
#endif

  return ret_val;
}

/*===========================================================================


FUNCTION tm_1x_cp_event_cb_handler

DESCRIPTION

DEPENDENCIES

  None.

RETURN VALUE

SIDE EFFECTS

  None.
===========================================================================*/
boolean tm_1x_cp_event_cb_handler
(
  prtl_event_e_type  event_type,
  prtl_event_u_type  *event_payload
)
{
  boolean ret_val;
  tm_cm_phone_state_info_s_type phone_state_info;
  MSG_HIGH("Event received %d",event_type,0,0);
  if(event_payload == NULL)
  {
    MSG_ERROR("Null event_payload",0,0,0);
    return FALSE;
  }

  if(event_type != PRTL_EVENT_TYPE_PHONE)
  {
    MSG_LOW("Ignore events other than EVENT_TYPE_PHONE",0,0,0);
    return FALSE;
  }

  tm_cm_iface_get_phone_state(&phone_state_info);

  switch(event_payload->phone_event_payload.event)
  {
    case TM_PHONE_EVENT_TC_CONNECTED:
      MSG_HIGH("Connected to PDE",0,0,0);
      tm_1x_cp_session_info.sess_state = TM_1X_CP_SESS_STATE_CONNECTED;

      if(tm_1x_cp_session_info.fl_req_mask & TM_1X_CP_FL_REQ_PPM)
      {
        if(tm_1x_cp_session_info.mgp_state.ppm_on)
        {
          if ( !tm_1x_cp_get_ppm( ) )
          {
            MSG_ERROR("Unable to Obtain Post-fix PPM",0,0,0);
            tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE, TM_STOP_REASON_GENERAL_ERROR);
          }
        }
        else
        {
          if(!tm_1x_cp_start_ppm(TM_1X_CP_SESS_CONTINUE))
          {
            MSG_ERROR("Unable to request TM-Core to start PPM",0,0,0);
            tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE,TM_STOP_REASON_GENERAL_ERROR);
          }
        }
      }
      break;

    case TM_PHONE_EVENT_TC_DISCONNECTED:
      MSG_HIGH("Disonnected",0,0,0);
      if(tm_1x_cp_session_active())
      {
        MSG_HIGH("Session aborted",0,0,0);
        tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_CM_IFACE,TM_STOP_REASON_GENERAL_ERROR);
      }
      break;

    case TM_PHONE_EVENT_MT_MSG:
      MSG_HIGH("MT Msg of length %d received", 
                  event_payload->phone_event_payload.payload.dbm_msg.length,0,0);

      ret_val = tm_1x_cp_handle_event_mt_msg(
      &event_payload->phone_event_payload.payload.dbm_msg);

      if(ret_val)
      {
        MSG_LOW("Begin proc of decoded Is801 msgs", 0, 0, 0);
        ret_val = tm_1x_cp_proc_801_msgs();
      }
      else
      {
        MSG_LOW("Unable to decode MT msg, terminate active session", 0, 0, 0);
      }

      if(ret_val == FALSE && tm_1x_cp_session_active())
      {
        tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE,TM_STOP_REASON_IS801_PAYLOAD_ERROR);
      }
      break;

    case TM_PHONE_EVENT_SENT_MSG_STATUS:
      MSG_HIGH("MT Sent Msg status %d for Id 0x%x received",
             event_payload->phone_event_payload.payload.sent_msg_info.msg_status,
             event_payload->phone_event_payload.payload.sent_msg_info.user_data,0);
      if(event_payload->phone_event_payload.payload.sent_msg_info.msg_status == TM_CM_STATUS_OK)
      {
        MSG_MED("L2 Ack for last pkt received",0,0,0);
        tm_1x_cp_handle_send_success();
      }
      else
      {
        event_report(EVENT_GPS_DBM_SEND_FAILURE);
        if(tm_1x_cp_session_active())
        {
          tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE,TM_STOP_REASON_IS801_SEND_FAILURE);
        }
      }
      break;

    case TM_PHONE_EVENT_E911CB_ENTER:
      MSG_HIGH("Enter E911CB",0,0,0);
      if (phone_state_info.srv_system == CGPS_SRV_SYS_CDMA)
      {
      if(tm_1x_cp_session_info.mgp_state.ppm_on == FALSE)
      {
        if(tm_1x_cp_start_ppm(TM_1X_CP_SESS_START))
        {
          tm_1x_cp_session_info.tm_core_session_started = TRUE;
        }
      }
      }
      break;

    case TM_PHONE_EVENT_E911CB_EXIT:
      MSG_HIGH("Exit E911CB",0,0,0);
      if(tm_1x_cp_session_info.tm_core_session_started == TRUE)
      {
        /* Clean up our state if we exit E911 CB. No more positioning! */
        tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE,
                                   TM_STOP_REASON_COMPLETED);
      }
      break;

    default:
      MSG_MED("event %d not handled",event_payload->phone_event_payload.event,0,0);
      break;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION   tm_1x_cp_dbm_resp_is_within_t2m

DESCRIPTION
  Determine if a PD request needs a time constraint response

DEPENDENCIES
  None

RETURNS
  TRUE / FALSE

SIDE EFFECTS
  None
===========================================================================*/
static boolean tm_1x_cp_dbm_resp_is_within_t2m
(
  byte*  start,
  /* msg buffer pointer */
  uint32 length
  /* msg length */
)
{
  return is801_util_is_resp_within_t2m(start, length);
}

/*===========================================================================


FUNCTION: tm_1x_cp_init_prtl_iface_tbl

DESCRIPTION:

PARAMETERS:

===========================================================================*/

void tm_1x_cp_init_prtl_iface_tbl(void)
{
  tm_1x_cp_prtl_iface_table.start_sess_req_fp   = tm_1x_cp_start_sess_req_handler; 
  tm_1x_cp_prtl_iface_table.stop_sess_req_fp    = tm_1x_cp_stop_sess_req_handler;
  tm_1x_cp_prtl_iface_table.sess_req_data_fp    = tm_1x_cp_sess_req_data_handler;
  tm_1x_cp_prtl_iface_table.sess_info_fp        = tm_1x_cp_sess_info_handler; 
  tm_1x_cp_prtl_iface_table.data_pipe_handle_fp = tm_1x_cp_data_pipe_handler; 
  tm_1x_cp_prtl_iface_table.timer_cb_fp         = tm_1x_cp_timer_cb_handler;
  tm_1x_cp_prtl_iface_table.event_cb_fp         = tm_1x_cp_event_cb_handler;
  tm_1x_cp_prtl_iface_table.general_query_fp    = NULL;
  tm_1x_cp_prtl_iface_table.gm_event_handle_fp  = NULL;
}

/*===========================================================================
FUNCTION: tm_1x_cp_read_nv

DESCRIPTION:

PARAMETERS:

===========================================================================*/

void tm_1x_cp_read_nv(void)
{
    /* Read the DBM PDDM Size */
    (void)sm_nv_read( NV_GPS1_NET_DBM_SIZE_I,(void *) &tm_1x_cp_nv_read_buffer );
    if (tm_1x_cp_nv_read_buffer.gps1_net_dbm_size == 0)
    {
      MSG_HIGH("MAX nv_gps1_net_dbm_size = %d.", TM_1X_CP_MAX_PDDM_DEFAULT_VAL, 0, 0);
      tm_1x_cp_config.dbm_pddm_size = TM_1X_CP_MAX_PDDM_DEFAULT_VAL;
    }
    else if (tm_1x_cp_nv_read_buffer.gps1_net_dbm_size < TM_1X_CP_MIN_PDDM_DEFAULT_VAL)
    {
      MSG_HIGH("MIN nv_gps1_net_dbm_size = %d.", TM_1X_CP_MIN_PDDM_DEFAULT_VAL, 0, 0);
      tm_1x_cp_config.dbm_pddm_size = TM_1X_CP_MIN_PDDM_DEFAULT_VAL;
    }
    else if (tm_1x_cp_nv_read_buffer.gps1_net_dbm_size > TM_1X_CP_MAX_PDDM_DEFAULT_VAL)
    {
      MSG_HIGH("MAX nv_gps1_net_dbm_size = %d.", TM_1X_CP_MAX_PDDM_DEFAULT_VAL, 0, 0);
      tm_1x_cp_config.dbm_pddm_size = TM_1X_CP_MAX_PDDM_DEFAULT_VAL;
    }
    else
    {
      tm_1x_cp_config.dbm_pddm_size = tm_1x_cp_nv_read_buffer.gps1_net_dbm_size;
    }
    /* For now, turn off is801b and glonass support */
    tm_1x_cp_config.support_is801b  = FALSE;
    tm_1x_cp_config.support_glonass = FALSE;
}

/*===========================================================================
FUNCTION: tm_1x_cp_init

DESCRIPTION:

PARAMETERS:

===========================================================================*/

void tm_1x_cp_init(void)

{
  /* Read NV items */
  tm_1x_cp_read_nv();

  /* Initialize TM-Core / 1x CP function call back table */
  tm_1x_cp_init_prtl_iface_tbl();

  /* Register with CM for 1X CP  specific events */
  (void)tm_cm_iface_submodule_init(TM_PRTL_TYPE_1X_CP,tm_1x_cp_dbm_resp_is_within_t2m);

  (void)is801_prtl_reg( TM_PRTL_TYPE_1X_CP, tm_1x_cp_is801_1_msg_cb);
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif

  tm_1x_cp_session_info.sess_state    = TM_1X_CP_SESS_STATE_NONE;
  tm_1x_cp_session_info.sess_active   = FALSE;
  tm_1x_cp_session_info.tm_core_session_started = FALSE;
  tm_1x_cp_session_info.is801_req_wait_bmap     = 0;
  tm_1x_cp_session_info.is801_resp_to_send_bmap = 0;
  tm_1x_cp_session_info.cancel_req_bmap         = 0;
  tm_1x_cp_session_info.fl_req_mask    = 0;
  tm_1x_cp_session_info.fl_resp_mask   = 0;


  tm_1x_cp_session_info.bs_capabilities_valid   = FALSE;
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#else
  tm_1x_cp_session_info.pde_version             = IS801_VERSION_801_1;
#endif

  tm_1x_cp_session_info.ppm_timeout   = TM_MGP_GET_PPM_TIMEOUT_MAX;

  tm_1x_cp_session_info.tm_1x_cp_sess_timer  = 
          os_TimerCreateInCgpsTimerGroup((uint32)TM_1X_CP_SESS_TIMER_ID,
                                         (uint32)THREAD_ID_SM_TM);
  tm_1x_cp_session_info.tm_1x_cp_ppm_timer   = 
          os_TimerCreateInCgpsTimerGroup((uint32)TM_1X_CP_PPM_TIMER_ID,
                                         (uint32)THREAD_ID_SM_TM);
  tm_1x_cp_session_info.tm_1x_cp_is801_timer = 
          os_TimerCreateInCgpsTimerGroup((uint32)TM_1X_CP_IS801_TIMER_ID,
                                         (uint32)THREAD_ID_SM_TM);
  tm_1x_cp_session_info.tm_1x_cp_ppm_trigger_timer = 
          os_TimerCreateInCgpsTimerGroup((uint32)TM_1X_CP_PPM_TRIGGER_TIMER_ID,
                                         (uint32)THREAD_ID_SM_TM);

  memset((void *)&tm_1x_cp_session_info.fwd_link_pddm, 0, 
          sizeof(is801_fwd_link_pddm_info_s_type));
  memset((void *)&tm_1x_cp_session_info.rev_link_pddm, 0, 
          sizeof(is801_rev_link_pddm_info_s_type));

}

/*===========================================================================

FUNCTION tm_1x_cp_prtl_init

DESCRIPTION
  This function is called by TM-Core to initialize 1x CP module

DEPENDENCIES

  None.

RETURN VALUE

SIDE EFFECTS

  None.
===========================================================================*/

void tm_1x_cp_prtl_init(void)

{
  MSG_MED("tm_1x_cp_prtl_init", 0, 0, 0);
  
  /* Initialize 1x CP internal Data Structure, NV settings, prtl callback table, ... etc */
  tm_1x_cp_init();

  /* Set up callback functions in TM-Core */
  (void)tm_prtl_reg(TM_PRTL_TYPE_1X_CP, &tm_1x_cp_prtl_iface_table);
}

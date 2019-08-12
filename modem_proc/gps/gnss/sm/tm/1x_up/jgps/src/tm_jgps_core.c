/*===========================================================================

                  TM - JGPS Core Module

DESCRIPTION
  This file contains implementations for TM Japan-GPS (KDDI) Core.

  Responsible for handling KDDI specific protocol.  Exchanges messages
  with KDDI server unit.

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/jgps/src/tm_jgps_core.c#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/14/09   atien   Fix compilation error in is801_decode_fwd_link_pddm()
                   for FEATURE_JCDMA_GPSONE
05/28/09   ank     For PRM timeout during MS-A fix, don't disconnect the socket.
05/27/09   ank     Abort MS-B fix if Dload attempt fails.
12/10/08   rw      Allow state machine to transition to init state from all 
                   other states.
03/06/06   rw      Initial Release
===========================================================================*/
#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"
#include "msg.h"
#include <stdio.h>

#include "sm_nv.h"
#include "pdapibuf.h"

#include "tm_1x_up_is801_drv_iface.h"

/*---------------------------------------------------------------------------

   This section contain code which is only compiled when 
   FEATURE_JCDMA_GPSONE is defined.
   
----------------------------------------------------------------------------*/
#ifdef FEATURE_JCDMA_GPSONE
#include "time_svc.h"
#include "tm_1x_up.h"

#include "tm_is801_util.h"
#include "sm_nv.h"

#include "tm_jgps_msg.h"
#include "tm_jgps_core.h"
#include "tm_jgps_core_i.h"

/*=============================================================================

                                Data Structures

=============================================================================*/
/* General JGPS module information
*/
tm_jgps_gen_info_s_type   tm_jgps_gen_info;

/* NV buffer, used to read NV items */
static nv_item_type      tm_jgps_nv_read_buf;

/* Location Uncertainty Table */
const float loc_uncrtnty_tab[] =
{
        0.5,    /* 0  */
        0.75,   /* 1  */
        1,      /* 2  */
        1.5,    /* 3  */
        2,      /* 4  */
        3,      /* 5  */
        4,      /* 6  */
        6,      /* 7  */
        8,      /* 8  */
        12,     /* 9  */
        16,     /* 10 */
        24,     /* 11 */
        32,     /* 12 */
        48,     /* 13 */
        64,     /* 14 */
        96,     /* 15 */
        128,    /* 16 */
        192,    /* 17 */
        256,    /* 18 */
        384,    /* 19 */
        512,    /* 20 */
        768,    /* 21 */
        1024,   /* 22 */
        1536,   /* 23 */
        2048,   /* 24 */
        3072,   /* 25 */
        4092,   /* 26 */
        6144,   /* 27 */
        8192,   /* 28 */
        9999,   /* 29 */
        9999,   /* 30 */
        9999    /* 31 */
};

/*=============================================================================

                                Macros

=============================================================================*/
#define Asc2degit(a)  (0x0f & (('1'<=(a)&&(a)<='9')? ((a)-'0') : (((a)=='0')? 0xa: 0x00)) )

/*=============================================================================
===============================================================================

                                Internal Functions
                                
===============================================================================
=============================================================================*/


/*===========================================================================
FUNCTION: tm_jgps_core_sess_init

DESCRIPTION:

 
PARAMETERS:

===========================================================================*/
void tm_jgps_core_sess_init(void)
{
  /* Local Variables */
  const  char def_url[] 
     = "url=http://aaa.co.jp/bbb.cgi&ver=1&datum=0&unit=1&acry=0&number=0";
  
  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();
  
  /*------------------------------------------------------------------------
     Initialize Session Information
  -----------------------------------------------------------------------*/
  /* Set the Session State*/
  sess_info_ptr->state = TM_JGPS_SESS_STATE_INIT;

  /* Set the Start session request parameters */
  memset((void *)&(sess_info_ptr->smod_req_param),
         0,
         sizeof( prtl_start_sess_req_param_s_type ));
  
  /* Set the Request Mode (NONE, MSA, MSB) */
  sess_info_ptr->req_mode = TM_JGPS_SESS_REQ_MODE_NONE;
    
  /* Set TM-Core session handle */
  sess_info_ptr->tm_core_sess_handle = TM_SESS_HANDLE_NONE;

  /* Set URL information sent during POS req or ACQ req */
  memset((void *)&(sess_info_ptr->url_info),
          0,
          sizeof(tm_jgps_url_info_s_type));

  /* Restart session flag */
  sess_info_ptr->restart = TM_JGPS_RESTART_INIT;
  
  /*---------------------------------------------------------------------------
     Intialize Packet Data
  ---------------------------------------------------------------------------*/
  
  /* Packet Header Data */
  memset((void *)&(sess_info_ptr->pkt_hdr),
          0,
          sizeof(tm_jgps_i_hdr_s_type));
    
  /* Request to SCU (POS or ACQ) - Contains MS IP address, RF info, URL, etc */
  memset((void *)&(sess_info_ptr->req_param),
          0,
          sizeof(tm_jgps_i_ms_req_s_type));

  /* Response packet from SCU (SCU->MS)*/
  memset((void *)&(sess_info_ptr->resp_pkt),
          0,
          sizeof(tm_jgps_packet_s_type));

  /* Set the URL data to default string */
  if( sizeof(def_url) <= TM_JGPS_MAX_URL)
  {
    memscpy( sess_info_ptr->req_param.url, sizeof(sess_info_ptr->req_param.url, (void *)def_url, sizeof(def_url) );
    sess_info_ptr->req_param.url_length = GNSS_STRLEN( sess_info_ptr->req_param.url );
  }
  
  /*---------------------------------------------------------------------------
     LR information
  ---------------------------------------------------------------------------*/
  /* Used to save PD info from 1x801 driver module for position report */
  memset((void *)&(sess_info_ptr->lr_resp),
          0,
          sizeof(pdsm_pd_info_s_type));


  memset((void *)&(sess_info_ptr->lr_time),
          0,
          sizeof(qword));


  memset((void *)&(sess_info_ptr->scu_lr_resp),
          0,
          sizeof(pm_scu_location_resp_type));

  /*---------------------------------------------------------------------------
     Request information
  ---------------------------------------------------------------------------*/
  memset((void *)&(sess_info_ptr->tm_sess_req_param),
          0,
          sizeof(tm_sess_req_param_u_type));

  /* Set the RF info request state */
  sess_info_ptr->rf_info_req_state = TM_JGPS_REQ_INFO_STATE_INIT;

  /* Set the RF info request state */
  sess_info_ptr->ms_ip_req_state = TM_JGPS_REQ_INFO_STATE_INIT;

  /*---------------------------------------------------------------------------
     Timers
  ---------------------------------------------------------------------------*/
  /* From sending (5 seconds) */
  sess_info_ptr->t02_timer
       = os_TimerCreate((uint32)TM_JGPS_TIMER_ID_T02, (uint32)THREAD_ID_SM_TM);

  /* From sending of Pos Report to receipt of Pos Report Ack (120 seconds) */
  sess_info_ptr->t04_timer
       = os_TimerCreate((uint32)TM_JGPS_TIMER_ID_T04, (uint32)THREAD_ID_SM_TM);


  /* Request RF info timer with TM-Core */
  sess_info_ptr->rf_info_req_timer
       = os_TimerCreate((uint32)TM_JGPS_TIMER_ID_RF_INFO_REQ, (uint32)THREAD_ID_SM_TM);
  
  /* Request MS IP addr timer with PD-Comm */
  sess_info_ptr->ms_ip_req_timer
       = os_TimerCreate((uint32)TM_JGPS_TIMER_ID_MS_IP_REQ, (uint32)THREAD_ID_SM_TM);

  return;

} /* tm_jgps_core_sess_init */

  
/*===========================================================================
FUNCTION: tm_jgps_core_app_info_init

DESCRIPTION:

 
PARAMETERS:

===========================================================================*/
void tm_jgps_core_app_info_init(void)
{
  /* Local Variables */
  tm_jgps_gen_info_s_type *gen_info_ptr = TM_JGPS_GEN_INFO_PTR();
  
  /*---------------------------------------------------------------------------
     Initialize the Application Information
  ---------------------------------------------------------------------------*/
  gen_info_ptr->app_info.app_type = PDSM_CLIENT_TYPE_NONE;

  memset((void *)&(gen_info_ptr->app_info.tm_core_start_req),
          0,
          sizeof(prtl_start_sess_req_param_s_type));

  return;
} /* tm_jgps_core_app_info_init */


/*===========================================================================
FUNCTION: tm_jgps_core_comm_info_init

DESCRIPTION:

 
PARAMETERS:

===========================================================================*/
void tm_jgps_core_comm_info_init(void)
{
  /* Local Variables */
  tm_jgps_gen_info_s_type *gen_info_ptr = TM_JGPS_GEN_INFO_PTR();
  
  /* Init TCP/IP */
  gen_info_ptr->comm_info.close_flag = FALSE;
  gen_info_ptr->comm_info.link_type = LINK_TCP;
  gen_info_ptr->comm_info.comm_state = TM_JGPS_COMM_STATE_INIT;
  gen_info_ptr->comm_info.recv_state = TM_JGPS_COMM_RECV_STATE_HEADER;
  gen_info_ptr->comm_info.incoming_data_length = 0;
  
} /* tm_jgps_core_comm_info_init */


/*===========================================================================
FUNCTION: tm_jgps_core_read_nv

DESCRIPTION:

PARAMETERS:

===========================================================================*/
void tm_jgps_core_read_nv(void)
{
  /* Local Variables */
  int    i;
  byte   nam;

  tm_jgps_gen_info_s_type   *gen_info_ptr = TM_JGPS_GEN_INFO_PTR();
  
  /*------------------------------------------------------------------------
     Read the Local Phone Number from NV
  ------------------------------------------------------------------------*/
  (void)sm_nv_read(NV_CURR_NAM_I,(void *)&tm_jgps_nv_read_buf);
  if ( tm_jgps_nv_read_buf.curr_nam > NV_MAX_NAMS )
  {
    nam = 0;
  }
  else
  {
    nam = tm_jgps_nv_read_buf.curr_nam;
  }

  /* Set the NAM and read the local phone number from NV */
  tm_jgps_nv_read_buf.dir_number.nam = nam;
  (void)sm_nv_read(NV_DIR_NUMBER_I,(void *)&tm_jgps_nv_read_buf);

  for ( i = 0; i < NV_DIR_NUMB_SIZ; i++ )
  {
    gen_info_ptr->sess_info.req_param.msnum[i]
        = Asc2degit( (tm_jgps_nv_read_buf.dir_number.dir_number[i]) );
  }

  /*------------------------------------------------------------------------
     Read the ESN
  ------------------------------------------------------------------------*/
  (void)sm_nv_read(NV_ESN_I,(void *)&tm_jgps_nv_read_buf);
  gen_info_ptr->sess_info.req_param.esn = tm_jgps_nv_read_buf.esn.esn;
  

  return;
} /* tm_jgps_core_read_nv */


/*===========================================================================

FUNCTION tm_jgps_core_translate_end_status

DESCRIPTION
  This function translate external end status to internal abort reason.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/ 
tm_sess_stop_reason_e_type tm_jgps_core_translate_end_status
(
  pdsm_pd_end_e_type   end_status
)
{
  tm_sess_stop_reason_e_type ret_val = TM_STOP_REASON_NONE;
  
  switch(end_status)
  {
    case PDSM_PD_END_E911:
      ret_val = TM_STOP_REASON_E911;
      break;

    case PDSM_PD_END_CLIENT_END:
      ret_val = TM_STOP_REASON_USER_TERMINATED;
      break;
      
    case PDSM_PD_END_FIX_ERROR :
      ret_val = TM_STOP_REASON_GENERAL_ERROR;      
      break;

    case PDSM_PD_END_CON_FAIL:
      ret_val = TM_STOP_REASON_JGPS_PPP;
      break;

    /*------------------------------------------------------------------
        JGPS specific end status reason
    ------------------------------------------------------------------*/
    case PDSM_PD_END_JGPS_POSRESP_NG_LIS:
      ret_val = TM_STOP_REASON_JGPS_POSRESP_NG_LIS;      
      break;
      
    case PDSM_PD_END_JGPS_POSRESP_NG_LSU:
      ret_val = TM_STOP_REASON_JGPS_POSRESP_NG_LSU;      
      break;

    case PDSM_PD_END_JGPS_POSRESP_NG_MSI:
      ret_val = TM_STOP_REASON_JGPS_POSRESP_NG_MSI;      
      break;

    case PDSM_PD_END_JGPS_STOPPOS_NG_LIS:
      ret_val = TM_STOP_REASON_JGPS_STOPPOS_NG_LIS;      
      break;
      
    case PDSM_PD_END_JGPS_STOPPOS_NG_MSI:
      ret_val = TM_STOP_REASON_JGPS_STOPPOS_NG_MSI;      
      break;   

    case PDSM_PD_END_JGPS_POSCOMP_NG:
      ret_val = TM_STOP_REASON_JGPS_POSCOMP_NG;      
      break;  
      
    case PDSM_PD_END_JGPS_TIMEOUT_T04:
      ret_val = TM_STOP_REASON_JGPS_TIMEOUT_T04;      
      break;  

    case PDSM_PD_END_JGPS_TIMEOUT_STOPPOS:
      ret_val = TM_STOP_REASON_JGPS_TIMEOUT_STOPPOS;      
      break;  

    case PDSM_PD_END_JGPS_TIMEOUT_T02:
      ret_val = TM_STOP_REASON_JGPS_TIMEOUT_T02;      
      break;  

  case PDSM_PD_END_JGPS_TIMEOUT_IS801:
      ret_val = TM_STOP_REASON_JGPS_TIMEOUT_IS801;      
      break;  

    case PDSM_PD_END_JGPS_IS801_LRREJ:   /* LR Reject  reception */ 
      ret_val = TM_STOP_REASON_JGPS_IS801_LRREJ;
      break;

    case PDSM_PD_END_JGPS_IS801_SEEDREJ: /* SEED Reject  reception */
      ret_val = TM_STOP_REASON_JGPS_IS801_SEEDREJ;
      break; 

    case PDSM_PD_END_JGPS_IS801_AAREJ:
      ret_val = TM_STOP_REASON_JGPS_IS801_AAREJ;      /* AA Reject  reception */
      break;

    case PDSM_PD_END_JGPS_IS801_EPHREJ:
      ret_val = TM_STOP_REASON_JGPS_IS801_EPHREJ;     /* EPH Reject  reception */
      break;

    case PDSM_PD_END_JGPS_IS801_ALMREJ:
      ret_val = TM_STOP_REASON_JGPS_IS801_ALMREJ;     /* ALM Reject  reception */
      break;

    case PDSM_PD_END_JGPS_IS801_AA_FAIL:
      ret_val = TM_STOP_REASON_JGPS_IS801_AA_FAIL;
      break;

    case PDSM_PD_END_JGPS_PPP:
      ret_val = TM_STOP_REASON_JGPS_PPP;
      break;

    case PDSM_PD_END_JGPS_TCP_MS:
      ret_val = TM_STOP_REASON_JGPS_TCP_MS;
      break;

    case PDSM_PD_END_JGPS_TCP_SERV:
      ret_val = TM_STOP_REASON_JGPS_TCP_SERV;
      break;

    case PDSM_PD_END_JGPS_ACQRESP_NG_LIS:
      ret_val = TM_STOP_REASON_JGPS_ACQRESP_NG_LIS;
      break;

    case PDSM_PD_END_JGPS_ACQRESP_NG_LSU:
      ret_val = TM_STOP_REASON_JGPS_ACQRESP_NG_LSU;
      break;

    case PDSM_PD_END_JGPS_ACQRESP_NG_MSI:
      ret_val = TM_STOP_REASON_JGPS_ACQRESP_NG_MSI;
      break;

    case PDSM_PD_END_JGPS_STOPACQ_NG_LIS:
      ret_val = TM_STOP_REASON_JGPS_STOPACQ_NG_LIS;
      break;

    case PDSM_PD_END_JGPS_STOPACQ_NG_MSI:
      ret_val = TM_STOP_REASON_JGPS_STOPACQ_NG_MSI;
      break;

    case PDSM_PD_END_JGPS_TIMEOUT_T20:
      ret_val = TM_STOP_REASON_JGPS_TIMEOUT_T20;
      break;

    case PDSM_PD_END_JGPS_TIMEOUT_STOPACQ:
      ret_val = TM_STOP_REASON_JGPS_TIMEOUT_STOPACQ;
      break;

    case PDSM_PD_END_RESET_LOCATION_SERVICE:
        ret_val = TM_STOP_REASON_ON_RESET_LOCATION_SERVICE;
        break;
        
    default:
      ret_val = TM_STOP_REASON_NONE;
      break;      
  }


  /* Translate JGPS specific error codes */
  if (    end_status >= PDSM_PD_END_JGPS_DMSB_ERROR  
       && end_status <= (PDSM_PD_END_JGPS_DPOSCOMP_NG 
                            + PDSM_PD_DLOAD_EPHEMERIS
                            + PDSM_PD_DLOAD_ALMANAC
                            + PDSM_PD_DLOAD_LOCATION)
     ) 
  {
    ret_val = (tm_sess_stop_reason_e_type)end_status;  
  }
  
  return ret_val;
}

/*===========================================================================

FUNCTION   tm_jgps_core_update_sess_state

DESCRIPTION
  This function is used to save JGPSONE specific info for use.

RETURN VALUE
  TRUE - JGPS core grants the session start request
  FALSE - otherwise
============================================================================*/
boolean tm_jgps_core_update_sess_state
(
  tm_jgps_sess_state_e_type new_state
)
{
  /* Local Variables */
  boolean                      state_tran = FALSE;
  tm_jgps_sess_state_e_type    curr_state = TM_JGPS_SESS_STATE_INIT;
  
  tm_jgps_sess_info_s_type     *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();
  
  /*------------------------------------------------------------------------
     Check the TM-JGPS Core session state transition
  -------------------------------------------------------------------------*/
  /* Current Session State */
  curr_state = sess_info_ptr->state;

  /* Determine if the session state can be modified*/
  switch(new_state)
  {  
  case TM_JGPS_SESS_STATE_INIT:
    state_tran = TRUE;
    break;
  case TM_JGPS_SESS_STATE_START_REQ:
    if(   curr_state == TM_JGPS_SESS_STATE_INIT
       || curr_state == TM_JGPS_SESS_STATE_WAIT_TCP_DISCONNECT
       || sess_info_ptr->restart == TM_JGPS_RESTART_TM_CORE)
    {
      state_tran = TRUE;
    }
    break;
  case TM_JGPS_SESS_STATE_WAIT_SCU_START_REQ_RESP:
    if(curr_state == TM_JGPS_SESS_STATE_START_REQ)
    {
      state_tran = TRUE;
    }
    break;
  case TM_JGPS_SESS_STATE_START_REQ_GRANT:
    if(curr_state == TM_JGPS_SESS_STATE_WAIT_SCU_START_REQ_RESP)
    {
      state_tran = TRUE;
    }
    break;

  /* Can enter abort and recovery state from any current state */
  case TM_JGPS_SESS_STATE_ABORT_RECOVER:
    state_tran = TRUE;
    break;

  /* Request from IS-801 Driver to stop session */
  case TM_JGPS_SESS_STATE_STOP_REQ:
    if(   curr_state == TM_JGPS_SESS_STATE_ABORT_RECOVER
       || curr_state == TM_JGPS_SESS_STATE_START_REQ_GRANT)
    {
      state_tran = TRUE;
    }
    break;
    
  /* Wait for response from SCU before ending the session */
  case TM_JGPS_SESS_STATE_WAIT_SCU_STOP_REQ_RESP:
    state_tran = TRUE;
    break;

  /* Must wait for the TCP disconnection before allowing next session */
  case TM_JGPS_SESS_STATE_WAIT_TCP_DISCONNECT:
    state_tran = TRUE;
    break;
  default:
    state_tran = FALSE;
    break;    
  }

  /*------------------------------------------------------------------------
     Update the TM-JGPS Core session state
  -------------------------------------------------------------------------*/
  if(state_tran)
  {
    sess_info_ptr->state = new_state;
    MSG_MED("JGPS: state transition [%d] -> [%d] for session [%d]",
             curr_state, 
             sess_info_ptr->state,
             sess_info_ptr->tm_core_sess_handle);
  }
  else
  {
    MSG_MED( "JGPS: state transition failed cur state [%d], req state [%d]",
             curr_state, 
             new_state,
             0);
  }
  
  return state_tran;

} /* tm_jgps_core_update_sess_state */


/*===========================================================================

FUNCTION   tm_jgps_core_restart_session

DESCRIPTION
  This function will kick-start a new session and wake-up TM-Core. Previous, 
  TM-Core request was placed in wait state because JGPS was in the process
  of closing the TCP connection.
  
RETURN VALUE
  None

============================================================================*/
boolean tm_jgps_core_restart_session( void )
{
  /* Local Variables */
  boolean ret_val = FALSE;
  tm_1x_up_is801_drv_req_param_u_type  req_param;

  tm_jgps_gen_info_s_type   *gen_info_ptr = TM_JGPS_GEN_INFO_PTR();
   
  /*------------------------------------------------------------------------
    Now kick of gpsOne session by setting wake_tm_core flag to TRUE
    which will make is801 driver module to use continue to wake up TM-Core 
    (TM-Core is in wait state )
  -------------------------------------------------------------------------*/
  if( gen_info_ptr->sess_info.restart == TM_JGPS_RESTART_TM_CORE 
     && tm_jgps_core_update_sess_state(TM_JGPS_SESS_STATE_START_REQ)  )
  {
    req_param.start_param.wake_tm_core = TRUE; /*lint !e506 */
    req_param.start_param.actions      = NULL;

    req_param.start_param.start_sess_req_param 
                 = &(gen_info_ptr->app_info.tm_core_start_req);

    ret_val = tm_1x_up_is801_drv_req(
                                      gen_info_ptr->sess_info.tm_core_sess_handle, 
                                      TM_IS801_DRV_REQ_START,
                                      req_param
                                    );

    /* Reset the restart indication */
    gen_info_ptr->sess_info.restart = TM_JGPS_RESTART_INIT;
    
    ret_val = TRUE;
  }

  return ret_val;
  
} /* tm_jgps_core_restart_session */


/*===========================================================================

FUNCTION   tm_jgps_core_reinit

DESCRIPTION
  This function is used to abort the current sesssion and notify the 
  proper sub-modules (i.e. IS-801 driver), TM-Core, and client

RETURN VALUE
  None

============================================================================*/
void tm_jgps_core_reinit(void)
{
  /* Local Variables */
  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();

  MSG_HIGH("JGPS: Re-Init core data",0,0,0);
                                        
  /* Clear the T02 timer and then process the response*/
  os_TimerStop(sess_info_ptr->t02_timer);

  /* Clear the T02 timer and then process the response */
  os_TimerStop(sess_info_ptr->t04_timer);

  /* Clear the RF info req timer */
  os_TimerStop(sess_info_ptr->rf_info_req_timer);

  /* Clear the MS IP address req timer */
  os_TimerStop(sess_info_ptr->ms_ip_req_timer);
  
  /*------------------------------------------------------------------------
     Set the session state to Init state
  -------------------------------------------------------------------------*/
  (void)tm_jgps_core_update_sess_state(TM_JGPS_SESS_STATE_INIT);

  /* Set the Start session request parameters */
  memset((void *)&(sess_info_ptr->smod_req_param),
         0,
         sizeof( prtl_start_sess_req_param_s_type ));
  
  /* Set the Request Mode (NONE, MSA, MSB) */
  sess_info_ptr->req_mode = TM_JGPS_SESS_REQ_MODE_NONE;
    
  
  /*---------------------------------------------------------------------------
     Intialize Packet Data
  ---------------------------------------------------------------------------*/
    
  /* Response packet from SCU (SCU->MS)*/
  memset((void *)&(sess_info_ptr->resp_pkt),
          0,
          sizeof(tm_jgps_packet_s_type));

  
  /*---------------------------------------------------------------------------
     LR information
  ---------------------------------------------------------------------------*/
  /* Used to save PD info from 1x801 driver module for position report */
  memset((void *)&(sess_info_ptr->lr_resp),
          0,
          sizeof(pdsm_pd_info_s_type));


  memset((void *)&(sess_info_ptr->lr_time),
          0,
          sizeof(qword));


  memset((void *)&(sess_info_ptr->scu_lr_resp),
          0,
          sizeof(pm_scu_location_resp_type));

  /*---------------------------------------------------------------------------
     Request information
  ---------------------------------------------------------------------------*/
  memset((void *)&(sess_info_ptr->tm_sess_req_param),
          0,
          sizeof(tm_sess_req_param_u_type));

  /* Set the RF info request state */
  sess_info_ptr->rf_info_req_state = TM_JGPS_REQ_INFO_STATE_INIT;

  /* Set the RF info request state */
  sess_info_ptr->ms_ip_req_state = TM_JGPS_REQ_INFO_STATE_INIT;
  
  /*---------------------------------------------------------------------------
     Comm info
  ---------------------------------------------------------------------------*/  
  tm_jgps_core_comm_info_init();

  sess_info_ptr->lr_resp.pd_info.dload_info.jgps_msdload_info.run = FALSE;

  return;
} /* tm_jgps_core_reinit */


/*===========================================================================

FUNCTION   tm_jgps_core_abort_recover

DESCRIPTION
  This function is used to abort the current sesssion and notify the 
  proper sub-modules (i.e. IS-801 driver), TM-Core, and client

RETURN VALUE
  None

============================================================================*/
void tm_jgps_core_abort_recover
(
  tm_sess_stop_reason_e_type  abort_reason,
  prtl_sess_stop_type         stop_type
)
{
  /* Local Variables */
  tm_1x_up_is801_drv_req_param_u_type  req_param;

  tm_jgps_gen_info_s_type *gen_info_ptr = TM_JGPS_GEN_INFO_PTR();

  MSG_HIGH("Abort and recover reason=%d type=%d",abort_reason,stop_type,0);
  if( gen_info_ptr->sess_info.state != TM_JGPS_SESS_STATE_ABORT_RECOVER)
  {
    if( abort_reason == TM_STOP_REASON_COMPLETED )
    {
      MSG_HIGH("JGPS: Abort and recover: Session Complete (curr_state=%d, data state=%d)",
               gen_info_ptr->sess_info.state,
               gen_info_ptr->comm_info.comm_state,
               0);
    }
    else
    {
      MSG_ERROR("JGPS: Abort and recover (curr_state=%d, data state=%d)",
                 gen_info_ptr->sess_info.state,
                 gen_info_ptr->comm_info.comm_state,
                 0);
    }
    
    /* For MS-A fix, for PRM timeout, don't disconnect the socket. */
    if(abort_reason == TM_STOP_REASON_PRM_TIMEOUT)
    {
      MSG_HIGH("PRM timeout",0,0,0);    
      req_param.stop_param.stop_reason = abort_reason;
      req_param.stop_param.stop_type   = stop_type;
    
      (void) tm_1x_up_is801_drv_req(  gen_info_ptr->sess_info.tm_core_sess_handle,
                                      TM_IS801_DRV_REQ_STOP,
                                      req_param );
      return;
    }

    /*------------------------------------------------------------------------
       Cleanup timers
    -------------------------------------------------------------------------*/
    /* Clear the T02 timer */
    os_TimerStop(gen_info_ptr->sess_info.t02_timer);
  
    /* Clear the T04 timer */
    os_TimerStop(gen_info_ptr->sess_info.t04_timer);

    /* Clear the RF info req timer */
    os_TimerStop(gen_info_ptr->sess_info.rf_info_req_timer);

    /* Clear the MS IP address req timer */
    os_TimerStop(gen_info_ptr->sess_info.ms_ip_req_timer);
    
    /*------------------------------------------------------------------------
       Call IS-801 drv function to close connection
    -------------------------------------------------------------------------*/
    if( !tm_1x_up_is801_drv_disconnect(FALSE) )
    {
      tm_jgps_core_reinit();
    }
    else
    {
      /*------------------------------------------------------------------------
         Update the session state to Abort and recover
      -------------------------------------------------------------------------*/ 
      if(!tm_jgps_core_update_sess_state(TM_JGPS_SESS_STATE_ABORT_RECOVER) )
      {
       MSG_ERROR("JGPS: Failed to enter abort and recover state (curr_state %d)",
                 gen_info_ptr->sess_info.state,0,0);
      }
    
      /*------------------------------------------------------------------------
         If COMMM state is disconnected, Re-Init
      -------------------------------------------------------------------------*/ 
      if(gen_info_ptr->comm_info.comm_state == TM_JGPS_COMM_STATE_DISCONNECTED)
      {
        tm_jgps_core_reinit();
      }
    }

      /*------------------------------------------------------------------------
       Inform 1x UP IS-801 driver of cancellation, 
       This call will send TM-Core the stop reason
      -------------------------------------------------------------------------*/
      req_param.stop_param.stop_reason = abort_reason;
      req_param.stop_param.stop_type   = stop_type;
    
      (void) tm_1x_up_is801_drv_req(  gen_info_ptr->sess_info.tm_core_sess_handle,
                                      TM_IS801_DRV_REQ_STOP,
                                      req_param );
  }
  else
  {
    MSG_HIGH("JGPS: Already in abort and recover state (curr_state %d)",
              gen_info_ptr->sess_info.state,0,0);    
  }
} /* tm_jgps_core_abort_recover */


/*===========================================================================

FUNCTION tm_jgps_core_abort_scu_resp

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
tm_sess_stop_reason_e_type tm_jgps_core_abort_scu_resp
(
  byte  cause
)
{
  /* Local Variables */
  tm_sess_stop_reason_e_type  end_status = TM_STOP_REASON_NONE;
  byte                        cmd_id;
  
  tm_jgps_sess_info_s_type *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();

  /* Set the cmd_id*/
  cmd_id = sess_info_ptr->resp_pkt.hdr.cmd_id;

  /*------------------------------------------------------------------------ 
      Determine the error to be sent to JPGS clients
  ------------------------------------------------------------------------*/
  switch( cause )
  {
    case TM_JGPS_SCU_CAUSE_NG_LIS:
      if( cmd_id == TM_JGPS_CMD_ID_POS_RESPONSE)
      {
        end_status = TM_STOP_REASON_JGPS_POSRESP_NG_LIS;
      }
      else
      {
        end_status = TM_STOP_REASON_JGPS_ACQRESP_NG_LIS;
      }

      break;

    case TM_JGPS_SCU_CAUSE_NG_LSU:
      if( cmd_id == TM_JGPS_CMD_ID_POS_RESPONSE)
      {
        end_status = TM_STOP_REASON_JGPS_POSRESP_NG_LSU;
      }
      else
      {
        end_status = TM_STOP_REASON_JGPS_ACQRESP_NG_LSU;
      }

      break;

    case TM_JGPS_SCU_CAUSE_NG_MSI:
      if( cmd_id == TM_JGPS_CMD_ID_POS_RESPONSE)
      {
        end_status = TM_STOP_REASON_JGPS_POSRESP_NG_MSI;
      }
      else
      {
        end_status = TM_STOP_REASON_JGPS_ACQRESP_NG_MSI;
      }

      break;

    default:
      if( cmd_id == TM_JGPS_CMD_ID_POS_RESPONSE)
      {
        end_status = TM_STOP_REASON_JGPS_POSRESP_NG_LIS;
      }
      else
      {
        end_status = TM_STOP_REASON_JGPS_ACQRESP_NG_LIS;
      }

      break;
  }

  return end_status;

} /* tm_jgps_core_abort_scu_resp */


/*===========================================================================

FUNCTION tm_jgps_core_abort_stop_scu

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
tm_sess_stop_reason_e_type tm_jgps_core_abort_stop_scu
(
  byte  cause
)
{
  /* Local Variables */
  tm_sess_stop_reason_e_type  end_status = TM_STOP_REASON_NONE;
  byte                        cmd_id;
  
  tm_jgps_sess_info_s_type *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();

  /* Set the cmd_id*/
  cmd_id = sess_info_ptr->resp_pkt.hdr.cmd_id;

  /*------------------------------------------------------------------------ 
      Determine the error to be sent to JPGS clients
  ------------------------------------------------------------------------*/
  switch(cause)
  {
    case TM_JGPS_SCU_CAUSE_NG_LIS:
      if( cmd_id == TM_JGPS_CMD_ID_STOP_POS_REQUEST)
      {
        end_status = TM_STOP_REASON_JGPS_STOPPOS_NG_LIS;
      }
      else
      {
        end_status = TM_STOP_REASON_JGPS_STOPACQ_NG_LIS;
      }

      break;

    case TM_JGPS_SCU_CAUSE_NG_MSI:
      if( cmd_id == TM_JGPS_CMD_ID_STOP_POS_REQUEST)
      {
        end_status = TM_STOP_REASON_JGPS_STOPPOS_NG_MSI;
      }
      else
      {
        end_status = TM_STOP_REASON_JGPS_STOPACQ_NG_MSI;
      }

      break;

    case TM_JGPS_SCU_CAUSE_NG_TO:
      if( cmd_id == TM_JGPS_CMD_ID_STOP_POS_REQUEST)
      {
        end_status = TM_STOP_REASON_JGPS_TIMEOUT_STOPPOS;
      }
      else
      {
        end_status = TM_STOP_REASON_JGPS_TIMEOUT_STOPACQ;
      }

      break;

    default:
      if( cmd_id == TM_JGPS_CMD_ID_STOP_POS_REQUEST)
      {
        end_status = TM_STOP_REASON_JGPS_STOPPOS_NG_LIS;
      }
      else
      {
        end_status = TM_STOP_REASON_JGPS_STOPACQ_NG_LIS;
      }

      break;

  }

  return end_status;

} /* tm_jgps_core_abort_stop_scu */


/*===========================================================================

FUNCTION tm_jgps_core_abort_pos_comp

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

tm_sess_stop_reason_e_type tm_jgps_core_abort_pos_comp
(
  byte   cause
)
{
  /* Local Variables */
  tm_sess_stop_reason_e_type  end_status = TM_STOP_REASON_NONE;
  
  tm_jgps_sess_info_s_type *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();
  
  /*------------------------------------------------------------------------ 
      If data dload has occured, but MS-A fix error 
  ------------------------------------------------------------------------*/
  if(sess_info_ptr->lr_resp.pd_info.dload_info.jgps_msdload_info.run)
  {
    end_status =  TM_STOP_REASON_JGPS_DPOSCOMP_NG
      + (sess_info_ptr->lr_resp.pd_info.dload_info.jgps_msdload_info.data_type) - 1;

    sess_info_ptr->lr_resp.pd_info.dload_info.jgps_msdload_info.run = FALSE;
  }
  else /* MSA: no fix with dload data */
  {
    end_status =  TM_STOP_REASON_JGPS_POSCOMP_NG;
  }
  
  return end_status;
  
} /* tm_jgps_core_abort_pos_comp */


/*===========================================================================

FUNCTION tm_jgps_core_abort_trans_scu_error

DESCRIPTION
   This function calls functions to translate the error codes to JGPS
   errors, and eventually calls tm_jgps_core_abort_recover() to 
   send the error back to JGPS clients and abort the current IS-801 session

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_jgps_core_abort_trans_scu_error
(
  byte   cause
)
{
  /* Local Variables */
  tm_sess_stop_reason_e_type abort_reason = TM_STOP_REASON_NONE;
  
  tm_jgps_sess_info_s_type *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();

  /*------------------------------------------------------------------------ 
    Determine the error, then abort and recover the session
  ------------------------------------------------------------------------*/
  switch( sess_info_ptr->resp_pkt.hdr.cmd_id )
  {
    case TM_JGPS_CMD_ID_POS_RESPONSE:
    case TM_JGPS_CMD_ID_ACQ_RESPONSE:
      abort_reason = tm_jgps_core_abort_scu_resp( cause );
      break;

    case TM_JGPS_CMD_ID_STOP_POS_REQUEST:
    case TM_JGPS_CMD_ID_STOP_ACQ_REQUEST:
      abort_reason = tm_jgps_core_abort_stop_scu( cause );
      break;

    case TM_JGPS_CMD_ID_POS_COMPLETE:
      abort_reason = tm_jgps_core_abort_pos_comp( cause );
      break;

    default:
      MSG_ERROR("JGPS: Unhandled error cmd_id(%d)",
                sess_info_ptr->resp_pkt.hdr.cmd_id,0,0);
      break;
  }

  /* Abort and Recover the JGPS session */
  if (abort_reason != TM_STOP_REASON_NONE)
  {
    /* Abort and recover timer for DMSB_ERROR */
    tm_jgps_core_abort_recover( abort_reason,
                                PRTL_SESS_STOP_SINGLE_FIX );
  }
  
  return;
} /* tm_jgps_core_abort_trans_scu_error */

/*===========================================================================

FUNCTION tm_jgps_core_t02_timer_handler

DESCRIPTION
  This function handles the T02 timer expiration
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
void tm_jgps_core_t02_timer_handler(void)
{
  /* Local Variables */
  tm_sess_stop_reason_e_type  abort_reason;
  
  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();

  MSG_MED("tm_jgps_core_t02_timer_handler",0,0,0);

  /* Send Cancel msg to SCU */
  tm_jgps_core_send_msg_cancel( TM_JGPS_CANCEL_CAUSE_TIMEOUT );

  /* Determine the abort reason */
  if( sess_info_ptr->req_mode == TM_JGPS_SESS_REQ_MODE_MSB_DLOAD )
  {
    abort_reason = TM_STOP_REASON_JGPS_TIMEOUT_T20;
  }
  else
  {
    abort_reason = TM_STOP_REASON_JGPS_TIMEOUT_T02;
  }

  /* Abort and recover timer for t02*/
  tm_jgps_core_abort_recover( abort_reason,
                              PRTL_SESS_STOP_SINGLE_FIX );
    
} /* tm_jgps_core_t02_timer_handler */


/*===========================================================================

FUNCTION tm_jgps_cpre_t04_timer_handler

DESCRIPTION
  This function handles the T04 timer expiration
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
void tm_jgps_core_t04_timer_handler(void)
{
  MSG_MED("tm_jgps_core_t04_timer_handler",0,0,0);
  /* Send Cancel msg to SCU */
  tm_jgps_core_send_msg_cancel( TM_JGPS_CANCEL_CAUSE_TIMEOUT );

  /* Abort and recover timer for t02*/
  tm_jgps_core_abort_recover( TM_STOP_REASON_JGPS_TIMEOUT_T04,
                              PRTL_SESS_STOP_SINGLE_FIX );
  
} /* tm_jgps_core_t04_timer_handler */


/*===========================================================================

FUNCTION tm_jgps_core_info_req_timer_handler

DESCRIPTION
  This function handles the RF info request  or MS IP address 
  info request timer expiration
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
void tm_jgps_core_info_req_timer_handler(void)
{
  /* Local Variables */
  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();

  MSG_MED("tm_jgps_core_info_req_timer_handler",0,0,0);
  /* Send Cancel msg to SCU */
  if( sess_info_ptr->state == TM_JGPS_SESS_STATE_WAIT_SCU_START_REQ_RESP )
  {
    tm_jgps_core_send_msg_cancel( TM_JGPS_CANCEL_CAUSE_TIMEOUT );
  }


  /* Abort and recover timer for t02*/
  tm_jgps_core_abort_recover( TM_STOP_REASON_GENERAL_ERROR,
                              PRTL_SESS_STOP_SINGLE_FIX );
  
} /* tm_jgps_core_info_req_timer_handler */


/*===========================================================================

FUNCTION tm_jgps_core_req_info_dispatch

DESCRIPTION
  This function is used to handle IOCTL ACK from PD COMM.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
void tm_jgps_core_req_info_dispatch(void)
{
  /* Local Variables */
  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();

  /*-------------------------------------------------------------------
    Check the session request state, only if RF info and MS IP addr
    have been recv then send the POS or ACQ req
  --------------------------------------------------------------------*/
  if( sess_info_ptr->rf_info_req_state == TM_JGPS_REQ_INFO_STATE_GRANT
      && sess_info_ptr->ms_ip_req_state == TM_JGPS_REQ_INFO_STATE_GRANT)
  {  
    /* Now send the session info request to SCU */
    tm_jgps_core_send_pos_acq_req();

    /* Reset the sess req info state */
    sess_info_ptr->rf_info_req_state = TM_JGPS_REQ_INFO_STATE_INIT;
    sess_info_ptr->ms_ip_req_state = TM_JGPS_REQ_INFO_STATE_INIT;
  }
 
  return;
} /* tm_jgps_core_req_info_dispatch */


/*===========================================================================

FUNCTION tm_jgps_core_req_ms_ip_info

DESCRIPTION
  This function requests MS IP address for KDDI POS req or ACQ req 
  packets.  Ask TM-Core to query LM which asks MGP to return a RF info
  data structure. 
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
boolean tm_jgps_core_req_ms_ip_info(void)
{
  /* Local Variables */
  boolean ret_val = FALSE;
  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();

  /*-------------------------------------------------------------------
    Request the MS IP address from PDCOMM
  --------------------------------------------------------------------*/
  ret_val = tm_1x_up_is801_drv_app_ioctl(PDCOMM_IOCTL_CMD_GET_IP_ADDR);
  
  if( ret_val )
  {
    MSG_MED("JGPS: Req for MS IP address Succeeded, start recovery timer (%d)", 0, 0, 0);

    /* Start the RF info recovery timer */
    (void)os_TimerStart(sess_info_ptr->ms_ip_req_timer, 
                       TM_JGPS_MS_IP_REQ_TIMER_CNT,
                       NULL);
  }
  else
  {
    MSG_ERROR("JGPS: Req for MS IP address Failed", 0, 0, 0);
  }  

  return ret_val;
} /* tm_jgps_core_req_ms_ip_info */

/*===========================================================================

FUNCTION tm_jgps_core_trans_ms_ip_addr

DESCRIPTION
  This function is used to handle IOCTL ACK from PD COMM.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
boolean tm_jgps_core_trans_ms_ip_addr
(
  byte     *ms_ip_ptr,
  uint32   ms_ip_len
)
{
  /* Local Variables */
  boolean ret_val = FALSE;

  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();


  /* Clear the MS IP address req timer */
  os_TimerStop(sess_info_ptr->ms_ip_req_timer);
  
  /*----------------------------------------------------------------------
     Check the input parameters
  ----------------------------------------------------------------------*/
  if(ms_ip_ptr == NULL || ms_ip_len == 0x0 )
  {
    MSG_ERROR("JGPS: Null msg_data_ptr",0,0,0);
  }
  else if( ms_ip_len <= sizeof(dword) )
  {
    /*----------------------------------------------------------------------
     Save the MS IP address
    ----------------------------------------------------------------------*/
    memscpy( (void *)&(sess_info_ptr->req_param.ipaddr),
             sizeof(sess_info_ptr->req_param.ipaddr),
            (void *)ms_ip_ptr,
            ms_ip_len );

    ret_val = TRUE;
  }

  /*----------------------------------------------------------------------
     Check the input parameters
  ----------------------------------------------------------------------*/  
  if( ret_val == FALSE )
  {
    /* Set default IP address info */
    sess_info_ptr->req_param.ipaddr = 0x1010101;
  }

  /* Recv and format MS IP addr grant */
  sess_info_ptr->ms_ip_req_state = TM_JGPS_REQ_INFO_STATE_GRANT;
   
  /*------------------------------------------------------------------------
     Indicate that the MS IP addr is available
  ------------------------------------------------------------------------*/
  tm_jgps_core_req_info_dispatch();
  
  return ret_val;
} /* tm_jgps_core_trans_ms_ip_addr */


/*===========================================================================

FUNCTION tm_jgps_core_req_rf_info

DESCRIPTION
  This function requests RF info specific for KDDI POS req or ACQ req 
  packets.  Ask TM-Core to query LM which asks MGP to return a RF info
  data structure. 
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
boolean tm_jgps_core_req_rf_info(void)
{
  /* Local Variables */
  boolean ret_val = FALSE;
  
  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();

  /*-------------------------------------------------------------------
    Request the RF infor from TM-Core 
  --------------------------------------------------------------------*/
  sess_info_ptr->tm_sess_req_param.continue_param.op_req  = TM_OP_REQ_RF_INFO;

  ret_val = tm_sess_req( TM_PRTL_TYPE_1X_UP,
                         sess_info_ptr->tm_core_sess_handle,
                         TM_SESS_REQ_CONTINUE,
                        &sess_info_ptr->tm_sess_req_param);

    
  if( ret_val )
  {
    MSG_MED("JGPS: Req for RF info Succeeded, start recovery timer (%d)", 0, 0, 0);

    /* Start the RF info recovery timer */
    (void)os_TimerStart(sess_info_ptr->rf_info_req_timer, 
                       TM_JGPS_RF_INFO_REQ_TIMER_CNT,
                       NULL);
  }
  else
  {
    MSG_ERROR("JGPS: Req for RF info Failed", 0, 0, 0);
  }     

  return ret_val;
} /* tm_jgps_core_req_rf_info */

/*===========================================================================

FUNCTION   tm_jgps_core_trans_rf_info

DESCRIPTION
  This function handles the session information data returned by TM-Core

RETURN VALUE
  None

============================================================================*/
boolean tm_jgps_core_trans_rf_info
(
  sm_ReportRfInfoStructType   *rf_info
)
{
  /* Local Variables */
  boolean ret_val = FALSE;
  dword fer;
  byte system_id, sw_revision, fer_offset;
  int8 i;
  
  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();

  /* Clear the RF info req timer */
  os_TimerStop(sess_info_ptr->rf_info_req_timer);

  /*------------------------------------------------------------------------
     Check Input Parameters
  ------------------------------------------------------------------------*/
  if (rf_info == NULL)
  {
    MSG_ERROR("JPGS: Input param null ptr",0,0,0);
  }
  /*------------------------------------------------------------------------
     Format the RF info data from MGP
  ------------------------------------------------------------------------*/
  else
  {
    /* Set the sw revision used
    */
    sw_revision = TM_JGPS_MS_SW_REV;

    /* Format the RF info data from MGP
    */
    /* If system in TC is HDR */
    if(rf_info->RfInfoSource == SOURCE_EVDO)
    {
      /* Set System ID to HDR (EvDO)
      */
      system_id = TM_JGPS_SYSTEM_ID_HDR;
      
      /* Init frame structure with PER info.
      */
      MSG_MED("JGPS: Err=%d,total=%d", 
              rf_info->RfInfoData.EVDORfInfo.z_PER.w_ErrPkts,
              rf_info->RfInfoData.EVDORfInfo.z_PER.w_MeasPkts,0);
  
      /* Tx power
      */
      sess_info_ptr->req_param.tx 
        = rf_info->RfInfoData.EVDORfInfo.b_TxPower / 256;
      

      MSG_MED("JGPS EVDO: TxPower=%d,tx=%d", 
               rf_info->RfInfoData.EVDORfInfo.b_TxPower,
               sess_info_ptr->req_param.tx,0);

      /* Init RSSI.
      */
      sess_info_ptr->req_param.rssi 
        = - (int8)rf_info->RfInfoData.EVDORfInfo.b_RSSI;
      
      /* Init Pilot info.
      */
      sess_info_ptr->req_param.num_pn = rf_info->RfInfoData.EVDORfInfo.u_PilotCnt;
      
      if (  sess_info_ptr->req_param.num_pn > TM_JGPS_MAX_ASET )
      {
        sess_info_ptr->req_param.num_pn = TM_JGPS_MAX_ASET;
      }
      
      for ( i = 0; i < TM_JGPS_MAX_ASET; i++ )
      {
        if ( i < sess_info_ptr->req_param.num_pn )
        {
          sess_info_ptr->req_param.pn[i].pn   
            = rf_info->RfInfoData.EVDORfInfo.z_Pilot[i].w_PN;
          sess_info_ptr->req_param.pn[i].ecio 
            = -1 * (rf_info->RfInfoData.EVDORfInfo.z_Pilot[i].b_Ecio / 2);
        }
        else
        {
          sess_info_ptr->req_param.pn[i].pn   = 0;
          sess_info_ptr->req_param.pn[i].ecio = 0;
        } /* end else if num_pn */
      } /* end for loop for i */

      /* Init channel info
      */
      sess_info_ptr->req_param.ch 
          = rf_info->RfInfoData.EVDORfInfo.w_Channel;
  
      /* Init the FER data
      */
      if ( (rf_info->RfInfoData.EVDORfInfo.z_PER.w_MeasPkts == 0) 
           || (rf_info->RfInfoData.EVDORfInfo.z_PER.w_ErrPkts 
                 > rf_info->RfInfoData.EVDORfInfo.z_PER.w_MeasPkts) )
      {
        fer = 0;
      }
      else
      {
        fer = ((dword)(rf_info->RfInfoData.EVDORfInfo.z_PER.w_ErrPkts ) * 100000 )
                      / ((dword)(rf_info->RfInfoData.EVDORfInfo.z_PER.w_MeasPkts));
      }  
    }
    /* If system in TC is 1x */
    else if(rf_info->RfInfoSource == SOURCE_1X)
    {
      /* Set System ID to 1x
      */
      system_id = TM_JGPS_SYSTEM_ID_1X;
      
      /* Init frame structure with PER info.
      */
      MSG_MED("JGPS: Err=%d,total=%d", 
               rf_info->RfInfoData.CDMARfInfo.z_FER.w_ErrPkts,
               rf_info->RfInfoData.CDMARfInfo.z_FER.w_MeasPkts,0);
  
      /* Tx power
      */
      sess_info_ptr->req_param.tx 
            = (int8)((rf_info->RfInfoData.CDMARfInfo.b_TxPower - 627) / 12);
  
      MSG_MED("JGPS 1X: TxPower=%d,tx=%d", 
               rf_info->RfInfoData.CDMARfInfo.b_TxPower,
               sess_info_ptr->req_param.tx,0);

      /* Init RSSI.
      */
      sess_info_ptr->req_param.rssi 
            = - (int8)rf_info->RfInfoData.CDMARfInfo.b_RSSI;
  
      /* Init Pilot info.
      */
      sess_info_ptr->req_param.num_pn = rf_info->RfInfoData.CDMARfInfo.u_PilotCnt;
  
      if (  sess_info_ptr->req_param.num_pn > TM_JGPS_MAX_ASET )
      {
        sess_info_ptr->req_param.num_pn = TM_JGPS_MAX_ASET;
      }
  
      for ( i = 0; i < TM_JGPS_MAX_ASET; i++ )
      {
        if ( i < sess_info_ptr->req_param.num_pn )
        {
          sess_info_ptr->req_param.pn[i].pn   
            = rf_info->RfInfoData.EVDORfInfo.z_Pilot[i].w_PN;
          sess_info_ptr->req_param.pn[i].ecio 
            = -1 * (rf_info->RfInfoData.EVDORfInfo.z_Pilot[i].b_Ecio / 2);
        }
        else
        {
          sess_info_ptr->req_param.pn[i].pn   = 0;
          sess_info_ptr->req_param.pn[i].ecio = 0;
        } /* end else if num_pn */
      } /* end for loop for i */
  
      /* Init channel info
      */
      sess_info_ptr->req_param.ch 
          = rf_info->RfInfoData.CDMARfInfo.w_Channel;
  
      /* Init the FER data
      */
      if ( (rf_info->RfInfoData.CDMARfInfo.z_FER.w_MeasPkts == 0) 
           || (rf_info->RfInfoData.CDMARfInfo.z_FER.w_ErrPkts 
                 > rf_info->RfInfoData.CDMARfInfo.z_FER.w_MeasPkts) )
      {
        fer = 0;
      }
      else
      {
        fer = ((dword)(rf_info->RfInfoData.CDMARfInfo.z_FER.w_ErrPkts ) * 100000 )
                      / ((dword)(rf_info->RfInfoData.CDMARfInfo.z_FER.w_MeasPkts));
      }    
    }
    /* Default values */
    else
    {
      MSG_ERROR("JGPS: RF info not handled for this system type: %d",
                rf_info->RfInfoSource,0,0);
      
      /* Set System ID to 1x
      */
      system_id = TM_JGPS_SYSTEM_ID_1X;
      
      /* Tx power
      */
      sess_info_ptr->req_param.tx  = 0;
  
      /* Init RSSI.
      */
      sess_info_ptr->req_param.rssi = 0;
  
      /* Init pilot info.
      */
      sess_info_ptr->req_param.num_pn = 0;
  
  
      for ( i = 0; i < TM_JGPS_MAX_ASET; i++ )
      {
        sess_info_ptr->req_param.pn[i].pn   = 0;
        sess_info_ptr->req_param.pn[i].ecio = 0;
      } /* end for loop for i */

      /* Init channel info
      */
      sess_info_ptr->req_param.ch = 0;
  
      fer_offset = 0;
      fer = 0;
    }
  
  
    /* shift the system id left 2 bits and or with sw revision.
    */
    fer_offset = (sw_revision) | (system_id<<2);
    
    if ( fer < 10 )
    {
      sess_info_ptr->req_param.fer = 0;
    }
    else if ( fer < 100 )
    {
      sess_info_ptr->req_param.fer = 1;
    }
    else if ( fer < 500 )
    {
      sess_info_ptr->req_param.fer = 2;
    }
    else if ( fer < 1000 )
    {
      sess_info_ptr->req_param.fer = 3;
    }
    else if ( fer < 2000 )
    {
      sess_info_ptr->req_param.fer = 4;
    }
    else if ( fer < 3000 )
    {
      sess_info_ptr->req_param.fer = 5;
    }
    else if ( fer < 4000 )
    {
      sess_info_ptr->req_param.fer = 6;
    }
    else if ( fer < 8000 )
    {
      sess_info_ptr->req_param.fer = 7;
    }
    else
    {
      sess_info_ptr->req_param.fer = 8;
    }
    
    /* Update the computed fer with fer_offset info into higher nibble.
    */
    sess_info_ptr->req_param.fer =  (sess_info_ptr->req_param.fer) | (fer_offset<<4);

    ret_val = TRUE;
  }

  /* Recv and format RF info grant */
  sess_info_ptr->rf_info_req_state = TM_JGPS_REQ_INFO_STATE_GRANT;
    
  /*------------------------------------------------------------------------
     Indicate that the RF info is available
  ------------------------------------------------------------------------*/
  tm_jgps_core_req_info_dispatch();
  
  return ret_val;
} /* tm_jgps_core_trans_rf_info */
      
/*===========================================================================

FUNCTION tm_jgps_core_calc_timestamp

DESCRIPTION
  This function calculates Time in JST format.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
void tm_jgps_core_calc_timestamp (void)
{
  time_julian_type julian_data;
  dword  systemtime; /* CDMA system timestamp  */
  char tmp[15];

  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();
  
  systemtime = time_get_secs();

  time_jul_from_secs( systemtime, &julian_data);


  /* Format the time */
  (void)GNSS_SNPRINTF (tmp, 15, "%04d%02d%02d%02d%02d%02d",
                        julian_data.year,
                        julian_data.month,
                        julian_data.day,
                        julian_data.hour, 
                        julian_data.minute, 
                        julian_data.second);
  
  (void)GNSS_STRLCPY( sess_info_ptr->scu_lr_resp.time,
                     tmp,
                     sizeof(sess_info_ptr->scu_lr_resp.time));
} /* tm_jgps_core_calc_timestamp */


/*===========================================================================

FUNCTION tm_jgps_core_calc_latitude

DESCRIPTION
  This function calculates Lattitude in WGS-84 format, [degree].
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
double tm_jgps_core_calc_latitude (sint31 latitude)
{
  return ( double)latitude * 180 / ( 2 << 24);
} /* tm_jgps_core_calc_latitude */


/*===========================================================================

FUNCTION tm_jgps_core_calc_longitude

DESCRIPTION
  This function calculates Longitude in WGS-84 format, [degree].
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
double tm_jgps_core_calc_longitude (sint31 longitude)
{
  return ( double)longitude * 360 / ( 2 << 25);
} /* tm_jgps_core_calc_longitude */


/*===========================================================================

FUNCTION tm_jgps_core_calc_majaa

DESCRIPTION
  This function calculates Majaa    Major Axis angle from true north ( 0 .. 180 [Degree] )  
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
int tm_jgps_core_calc_majaa (byte loc_uncrtnty_ang)
{
  if( loc_uncrtnty_ang > 0xF)
    loc_uncrtnty_ang = 0xF;

  return ( int)( loc_uncrtnty_ang * 5.625);
} /* tm_jgps_core_calc_majaa */


/*===========================================================================

FUNCTION tm_jgps_core_calc_smaj

DESCRIPTION
   This function calculates Smaj Long axis error ( Range: 0 .. 9999 [m] )
 
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
int tm_jgps_core_calc_smaj (byte loc_uncrtnty_a)
{
  if( loc_uncrtnty_a > 0x1F)
    loc_uncrtnty_a = 0x1F;

  return (int)loc_uncrtnty_tab[loc_uncrtnty_a];
} /* tm_jgps_core_calc_smaj */


/*===========================================================================

FUNCTION tm_jgps_core_calc_smin

DESCRIPTION
   This function calculates Smin Short axis error ( Range: 0 .. 9999 [m] )
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
int tm_jgps_core_calc_smin(byte loc_uncrtnty_p)
{
  if( loc_uncrtnty_p > 0x1F)
    loc_uncrtnty_p = 0x1F;

  return (int)loc_uncrtnty_tab[loc_uncrtnty_p];
} /* tm_jgps_core_calc_smin */


/*===========================================================================

FUNCTION tm_jgps_core_calc_altitude

DESCRIPTION
  This function calculates  altitude    Range: -999..+9999[m]
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
int tm_jgps_core_calc_altitude (sint15 alt)
{
  alt -= 500;

  if( alt > 9999)
  {
    alt = 9999;
  }
  else if( alt < -999)
  {
    alt = -999;
  }

  return (int)alt;
} /* tm_jgps_core_calc_altitude */


/*===========================================================================

FUNCTION tm_jgps_core_calc_vert_err

DESCRIPTION
  This function calculates Vert ( Altitude error )   ( Range: 0 .. 9999 [m] ) 
    
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
int tm_jgps_core_calc_vert_err (byte loc_uncrtnty_v)
{
  if( loc_uncrtnty_v > 0x1F)
    loc_uncrtnty_v = 0x1F;

  return (int)loc_uncrtnty_tab[loc_uncrtnty_v];
} /*l tm_jgps_core_calc_vert_err */


/*===========================================================================

FUNCTION tm_jgps_core_convert_lr_data

DESCRIPTION
  This function converts LR data from IS801.1 format to KDDI compliant format
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
void tm_jgps_core_convert_lr_data 
( 
  pdsm_pd_info_s_type  *pd_info_ptr
)
{
  /* Local Variables */
  char ctemp[15]; 
  pdsm_pd_data_type *lr_data_ptr = &pd_info_ptr->pd_info.pd_data;
  
  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();

  /*------------------------------------------------------------------------
    Check input parameters
  -------------------------------------------------------------------------*/
  if(pd_info_ptr == NULL)
  {
    MSG_ERROR("JGPS: Invalid input parameter, null ptr", 0,0,0);
    return;
  }

  /*------------------------------------------------------------------------
    Format the PM data
  -------------------------------------------------------------------------*/
  (void)GNSS_SNPRINTF( ctemp, 15,         "%-d", TM_JGPS_URL_VER1);
  (void)GNSS_STRLCPY( &sess_info_ptr->scu_lr_resp.ver,   
                     ctemp, 
                     sizeof(sess_info_ptr->scu_lr_resp.ver));
  
  (void)GNSS_SNPRINTF( ctemp, 15,         "%-d", TM_JGPS_URL_DATUM_WGS84);
  (void)GNSS_STRLCPY( &sess_info_ptr->scu_lr_resp.datum, 
                     ctemp, 
                     sizeof(sess_info_ptr->scu_lr_resp.datum));

  (void)GNSS_SNPRINTF( ctemp, 15,         "%-d", TM_JGPS_URL_UNIT_DEGREE);
  (void)GNSS_STRLCPY( &sess_info_ptr->scu_lr_resp.unit,  
                     ctemp, 
                     sizeof(sess_info_ptr->scu_lr_resp.unit));

  (void)GNSS_SNPRINTF( ctemp, 15,         "%-d", TM_JGPS_URL_NUM_FIX_DEFAULT);
  (void)GNSS_STRLCPY( &sess_info_ptr->scu_lr_resp.fm,    
                     ctemp, 
                     sizeof(sess_info_ptr->scu_lr_resp.fm));

  /* Get the current timestamp */
  tm_jgps_core_calc_timestamp();

  (void)snprintf( ctemp, 15,"%+.5f",tm_jgps_core_calc_latitude( lr_data_ptr->lat));
  (void)GNSS_STRLCPY( sess_info_ptr->scu_lr_resp.lat,   
                     ctemp, 
                     sizeof(sess_info_ptr->scu_lr_resp.lat));

  (void)snprintf( ctemp, 15,"%+.5f",tm_jgps_core_calc_longitude( lr_data_ptr->lon));
  (void)GNSS_STRLCPY( sess_info_ptr->scu_lr_resp.lon,   
                     ctemp, 
                     sizeof(sess_info_ptr->scu_lr_resp.lon));

  (void)GNSS_SNPRINTF( ctemp, 15,"%-d",tm_jgps_core_calc_majaa( lr_data_ptr->loc_uncertainty_ang));
  (void)GNSS_STRLCPY( sess_info_ptr->scu_lr_resp.majaa, 
                     ctemp, 
                     sizeof(sess_info_ptr->scu_lr_resp.majaa));

  (void)GNSS_SNPRINTF( ctemp, 15,"%-d",tm_jgps_core_calc_smaj( lr_data_ptr->loc_uncertainty_a));
  (void)GNSS_STRLCPY( sess_info_ptr->scu_lr_resp.smaj,
                     ctemp, 
                     sizeof(sess_info_ptr->scu_lr_resp.smaj));

  (void)GNSS_SNPRINTF( ctemp, 15,"%-d",tm_jgps_core_calc_smin( lr_data_ptr->loc_uncertainty_p));
  (void)GNSS_STRLCPY( sess_info_ptr->scu_lr_resp.smin,
                     ctemp, 
                     sizeof(sess_info_ptr->scu_lr_resp.smin));


  (void)GNSS_SNPRINTF( ctemp, 15,"%-d",tm_jgps_core_calc_altitude( lr_data_ptr->altitude));
  (void)GNSS_STRLCPY( sess_info_ptr->scu_lr_resp.alt,
                     ctemp,
                     sizeof(sess_info_ptr->scu_lr_resp.alt));

  (void)GNSS_SNPRINTF( ctemp, 15,"%-d",tm_jgps_core_calc_vert_err( lr_data_ptr->loc_uncertainty_v));
  (void)GNSS_STRLCPY( sess_info_ptr->scu_lr_resp.vert,
                     ctemp,
                     sizeof(sess_info_ptr->scu_lr_resp.vert));

  return;

} /* tm_jgps_core_convert_lr_data */


/*===========================================================================

FUNCTION tm_jgps_core_get_fixdata

DESCRIPTION
    
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void tm_jgps_core_get_fixdata
(
  char *data,
  int  len,
  char *key,
  char *buf,
  int  size
)
{
  int   key_len = 0;
  int   cnt = 0;
  char  *p;

  /*------------------------------------------------------------------------
    Get the next field in the POS resp
  -------------------------------------------------------------------------*/
  memset( ( char *)buf, 0, size);

  key_len = GNSS_STRLEN( key);

  for( p = data ; p <= data + len - key_len ; p++)
  {
    if( strncmp( p, key, key_len) == 0)
    {
      for( p = p + key_len, cnt = 0 ; p < data + len ; p++)
      {
        if( ( *p == '&') || ( cnt == size))
          break;

        buf[ cnt++] = *( byte *)p;
      }

      break;

    }
  }

} /* tm_jgps_core_get_fixdata */


/*===========================================================================

FUNCTION tm_jgps_core_set_pd_data

DESCRIPTION
    
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void tm_jgps_core_set_pd_data
(
  char *data,
  char *buf,
  int  size
)
{
  int   data_len = 0;
  int   i = 0;

  /*------------------------------------------------------------------------
    Set the POS data data byte position 
  -------------------------------------------------------------------------*/
  data[ size] = 0x00;
  data_len = GNSS_STRLEN( ( char *)data);

  if( data_len == 0)
  {
    memset( ( char *)buf, 'F', size);

    return;
  }

  memset( ( char *)buf, '0', size);

  if( *data == '-')
  {
    *buf = '-';
    data++;
    data_len -= 1;
  }

  for( i = size - data_len ; i < size ; i++)
  {
    buf[ i] = *data++;
  }

} /* tm_jgps_core_set_pd_data */


/*===========================================================================

FUNCTION tm_jgps_core_convert_scu_fix_data

DESCRIPTION
    This function translates the POS response data into PDAPI
    format for all JGPS clients.
    
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tm_jgps_core_convert_scu_fix_data
(
  byte flength,
  char *fixdata
)
{
  /* Local Variables */
  char                         tmp[5];
  dword                        systemtime; /* CDMA system timestamp  */
  pm_scu_location_resp_type    *pm_data;

  tm_jgps_gen_info_s_type *gen_info_ptr = TM_JGPS_GEN_INFO_PTR();


  /* Copy the POS complete data to global variable */
  pm_data = &gen_info_ptr->sess_info.scu_lr_resp;
  
  /*------------------------------------------------------------------------
    Format the data returned to JGPS clients
  -------------------------------------------------------------------------*/
  memset(pm_data, 0xFF, sizeof(pm_scu_location_resp_type));

  tm_jgps_core_get_fixdata( fixdata, flength, "ver=",   &pm_data->ver,   TM_JGPS_MAX_VER_LEN);
  tm_jgps_core_get_fixdata( fixdata, flength, "datum=", &pm_data->datum, TM_JGPS_MAX_DATUM_LEN);
  tm_jgps_core_get_fixdata( fixdata, flength, "unit=",  &pm_data->unit,  TM_JGPS_MAX_UNIT_LEN);
  
  tm_jgps_core_get_fixdata( fixdata, flength, "lat=",   pm_data->lat,    TM_JGPS_MAX_LAT_LEN);
  tm_jgps_core_get_fixdata( fixdata, flength, "lon=",   pm_data->lon,    TM_JGPS_MAX_LON_LEN);
  
  tm_jgps_core_get_fixdata( fixdata, flength, "alt=",   tmp,            TM_JGPS_MAX_ALT_LEN);
  tm_jgps_core_set_pd_data( tmp,                        pm_data->alt,    TM_JGPS_MAX_ALT_LEN);
  
  tm_jgps_core_get_fixdata( fixdata, flength, "time=",  pm_data->time,   TM_JGPS_MAX_TSTAMP_LEN);
  
  tm_jgps_core_get_fixdata( fixdata, flength, "smaj=",  tmp,            TM_JGPS_MAX_SMAJ_LEN);
  tm_jgps_core_set_pd_data( tmp,                        pm_data->smaj,   TM_JGPS_MAX_SMAJ_LEN);
  
  tm_jgps_core_get_fixdata( fixdata, flength, "smin=",  tmp,            TM_JGPS_MAX_SMIN_LEN);
  tm_jgps_core_set_pd_data( tmp,                        pm_data->smin,   TM_JGPS_MAX_SMIN_LEN);
  
  tm_jgps_core_get_fixdata( fixdata, flength, "vert=",  tmp,            TM_JGPS_MAX_VERT_LEN);
  tm_jgps_core_set_pd_data( tmp,                        pm_data->vert,   TM_JGPS_MAX_VERT_LEN);
  
  tm_jgps_core_get_fixdata( fixdata, flength, "majaa=", tmp,            TM_JGPS_MAX_MAJAA_LEN);
  tm_jgps_core_set_pd_data( tmp,                        pm_data->majaa,  TM_JGPS_MAX_MAJAA_LEN);
  
  tm_jgps_core_get_fixdata( fixdata, flength, "fm=",    &pm_data->fm,    TM_JGPS_MAX_FM_LEN);



  /* If the "jgps_msdload_info.msa_seed"is TRUE, 
  ** this function stores the download data type and the timestamp to
  ** the "jgps_msdload_info.data_type"
  ** and the "jgps_msdload_info.timestamp".
  ** The KDDI GPS APP uses these parameters when PDSM_PD_EVENT_POSITION is received
  */
  if( TRUE
        == gen_info_ptr->sess_info.lr_resp.pd_info.dload_info.jgps_msdload_info.msa_seed)
  {
    /* LR info available */
    gen_info_ptr->sess_info.lr_resp.pd_info.dload_info.jgps_msdload_info.data_type
                  |= PDSM_PD_DLOAD_LOCATION;

    /* Set the MSA seed flag to false, becuse this is MSA POS resp from SCU */
    gen_info_ptr->sess_info.lr_resp.pd_info.dload_info.jgps_msdload_info.msa_seed 
                  = FALSE;

    /* Get timestamp */
    systemtime = time_get_secs();

    time_jul_from_secs( systemtime, 
                        (time_julian_type *)
                          &(gen_info_ptr->sess_info.lr_resp.pd_info.dload_info.jgps_msdload_info.timestamp) );
  }


  return;
} /* tm_jgps_core_convert_scu_fix_data */


/*===========================================================================

FUNCTION tm_jgps_core_save_lr_data_msa

DESCRIPTION
  This function converts LR data from IS801.1 format to KDDI compliant format
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
boolean tm_jgps_core_save_lr_data
(
  pdsm_pd_info_s_type  *pd_info_ptr
)
{
  /* Local Varaibles */
  boolean ret_val = FALSE;

  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();
  
  /*------------------------------------------------------------------------
    Check input parameters
  -------------------------------------------------------------------------*/
  if(pd_info_ptr == NULL)
  {
    MSG_ERROR("JGPS: Invalid input parameter, null ptr", 0,0,0);
    return FALSE;
  }

  /* Copy the PD info to be sent to clients when SCU sends a completion response */
  (void)memscpy((void *)&sess_info_ptr->lr_resp, sizeof(sess_info_ptr->lr_resp),
               (void *)pd_info_ptr, sizeof(*pd_info_ptr));

  ret_val = TRUE;

  return ret_val;
} /* tm_jgps_core_save_lr_data_msa */


/*===========================================================================

FUNCTION tm_jgps_core_send_pos_acq_req

DESCRIPTION
  This function sends the POS req or an ACQ after successfully requesting
  and recieving 
  
  1. RF info
  2. MS IP address
  3. URL data.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
boolean tm_jgps_core_send_pos_acq_req(void)
{
  /* Local Variables */
  boolean ret_val = FALSE;
  void  *scu_data = NULL;      /* No Cmd specific data for SCU */
  
  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();
  
  /*-----------------------------------------------------------------------
    Based the request mode, TM-JGPS will send POS request or ACQ request.
  ------------------------------------------------------------------------*/
  if (sess_info_ptr->req_mode == TM_JGPS_SESS_REQ_MODE_MSA)
  {
    /* Since MSAssisted operation, send POS request to SCU.
    */
    ret_val = tm_jgps_core_send_msg( TM_JGPS_CMD_ID_START_POS_REQUEST, scu_data );
  }
  else if (sess_info_ptr->req_mode == TM_JGPS_SESS_REQ_MODE_MSB_DLOAD)
  {       
    /* Send ACQ request to SCU and wait for response
    */
    ret_val = tm_jgps_core_send_msg( TM_JGPS_CMD_ID_START_ACQ_REQUEST, scu_data );
  } /* end start message type */

  return ret_val;
} /* tm_jgps_core_send_pos_acq_req */


/*===========================================================================

FUNCTION tm_jgps_core_start_pkt_data_req

DESCRIPTION
  This function starts the process of sending either a POS req or an ACQ
  req to SCU.  First, JPGS module must get RF info from TM-Core (async call)
  MS IP address from PDCOMM (async call), and URL data (sync).  
  JGPS must wait for all data to return before sending out a request to SCU
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
boolean tm_jgps_core_start_pkt_data_req(void) 
{
  /* Local Variables */
  boolean ret_val = FALSE;
  
  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();

  do{
    /*------------------------------------------------------------------------
       MS IP address, if not already requested
    -------------------------------------------------------------------------*/
    if(sess_info_ptr->ms_ip_req_state == TM_JGPS_REQ_INFO_STATE_INIT)
    {
  
      if( tm_jgps_core_req_ms_ip_info() == TRUE)
      {
        sess_info_ptr->ms_ip_req_state = TM_JGPS_REQ_INFO_STATE_WAIT;
      }
      else
      {
        /* Set default IP address info */
        sess_info_ptr->req_param.ipaddr = 0x1010101;
        sess_info_ptr->ms_ip_req_state = TM_JGPS_REQ_INFO_STATE_GRANT;
      }
    }
    
    /*------------------------------------------------------------------------
       RF information, if not already requested
    -------------------------------------------------------------------------*/    
    if(sess_info_ptr->rf_info_req_state == TM_JGPS_REQ_INFO_STATE_INIT)
    {
      if(tm_jgps_core_req_rf_info() == TRUE)
      {
        sess_info_ptr->rf_info_req_state = TM_JGPS_REQ_INFO_STATE_WAIT;
      }
      else
      {
        sess_info_ptr->rf_info_req_state = TM_JGPS_REQ_INFO_STATE_GRANT;
      }
    }
  
    /*------------------------------------------------------------------------
       Set the URL data
    -------------------------------------------------------------------------*/
    if( FALSE == tm_jgps_make_url() )
    {
      MSG_ERROR("JGPS failed to create URL, abort req (%d)", 
                sess_info_ptr->resp_pkt.hdr.cmd_id, 0, 0);
      break;
    }
  
    ret_val = TRUE;
  }while (0);
  
  /*------------------------------------------------------------------------
     Send the POS or ACQ request packet if all request parameters have
     been gathered (RF info, MS IP address, URL data)
  ------------------------------------------------------------------------*/
  tm_jgps_core_req_info_dispatch();

  return ret_val;
} /* tm_jgps_core_start_pkt_data_req */


/*===========================================================================

FUNCTION   tm_jgps_core_make_msb_def_url

DESCRIPTION
  This function is used to set default URL data for MSBased sessions.

RETURN VALUE
  None

============================================================================*/
static boolean tm_jgps_core_make_msb_def_url( void )
{
  /* Local Variables */
  const  char url[] = "url=http://aaa.co.jp/bbb.cgi&ver=1";

  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();
  
  /*------------------------------------------------------------------------
    Set the default URL
    Set the URL length in the request message to be sent
  -------------------------------------------------------------------------*/
  sess_info_ptr->req_param.url_length = memscpy( sess_info_ptr->req_param.url,
       sizeof(sess_info_ptr->req_param.url), (void *)url, sizeof(url) );
  
  return TRUE;
} /* tm_jgps_core_make_msb_def_url */


/*===========================================================================

FUNCTION   tm_jgps_core_make_msa_def_url

DESCRIPTION
  This function is used to set default URL data for MSAssisted sessions.

RETURN VALUE
  None

============================================================================*/
static boolean tm_jgps_core_make_msa_def_url( void )
{
  /* Local Variables */
  const  char url[] 
     = "url=http://aaa.co.jp/bbb.cgi&ver=1&datum=0&unit=1&acry=0&number=0";

  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();
  
  /*------------------------------------------------------------------------
    Set the default URL
    Set the URL length in the request message to be sent
  -------------------------------------------------------------------------*/
  sess_info_ptr->req_param.url_length = memscpy( sess_info_ptr->req_param.url,
       sizeof(sess_info_ptr->req_param.url), (void *)url, sizeof(url) );

  return TRUE; 
} /* tm_jgps_core_make_msa_def_url */


/*===========================================================================

FUNCTION   tm_jgps_make_url

DESCRIPTION
  This function is used to set URL data

RETURN VALUE
  None

============================================================================*/
boolean tm_jgps_make_url( void )
{
  /* Local Variables */
  boolean status = TRUE;
  const char url_hdr[]="url=";
  int url_len;
  char url_buf[TM_JGPS_REQ_ULENGTH_MAX+TM_JGPS_REQ_ULENGTH_NULL];

  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();

  do
  {
    /*------------------------------------------------------------------------
       Use Default MS-Assisted URL parameters if app did not initialize data
    -------------------------------------------------------------------------*/
    if( sess_info_ptr->url_info.app_url_enable == FALSE )
    {
      /* Set default URL address, app did not call set parameters */
      if( sess_info_ptr->req_mode == TM_JGPS_SESS_REQ_MODE_MSA )
      {
        MSG_HIGH("JGPS: Using def MSA url.",0,0,0);
        status = tm_jgps_core_make_msa_def_url();
      }
      else if( sess_info_ptr->req_mode == TM_JGPS_SESS_REQ_MODE_MSB_DLOAD )
      {
        MSG_HIGH("JGPS: Using def MSB url.",0,0,0);
        status = tm_jgps_core_make_msb_def_url();
      }
      else
      {
        status = FALSE;
        break;
      }
    }
    else
    { 
      /*------------------------------------------------------------------------
         Set the Version field in URL data
      -------------------------------------------------------------------------*/
      if ( sess_info_ptr->url_info.ver != TM_JGPS_URL_VER1 )
      {
        sess_info_ptr->url_info.ver = TM_JGPS_URL_VER1;
      }
    
      /*------------------------------------------------------------------------
         Set the datum and unit in URL
      -------------------------------------------------------------------------*/
      if ( sess_info_ptr->url_info.datum > TM_JPGS_URL_DATUM_TOKYO )
      {
        sess_info_ptr->url_info.datum = TM_JGPS_URL_DATUM_WGS84;
      }
    
      /*------------------------------------------------------------------------
         Set the datum and unit in URL
      -------------------------------------------------------------------------*/
      if ( sess_info_ptr->url_info.datum > TM_JGPS_URL_UNIT_DEGREE )
      {
        sess_info_ptr->url_info.datum = TM_JGPS_URL_UNIT_DMS;
      }
    
      /*------------------------------------------------------------------------
         Set the Accuracy in URL data
      -------------------------------------------------------------------------*/
      if ( sess_info_ptr->url_info.acry > TM_JGPS_URL_ACRY_MIN )
      {
        sess_info_ptr->url_info.acry = TM_JGPS_URL_ACRY_FREE;
      }
    
      /*------------------------------------------------------------------------
         Set the number of fixes in URL data
      -------------------------------------------------------------------------*/
      if (  sess_info_ptr->url_info.num > TM_JGPS_URL_NUM_FIX_DEFAULT )
      {
        sess_info_ptr->url_info.num = TM_JGPS_URL_NUM_FIX_DEFAULT;
      }
    
  
      /*------------------------------------------------------------------------
         Create the URL data
      -------------------------------------------------------------------------*/
      if( sess_info_ptr->req_mode == TM_JGPS_SESS_REQ_MODE_MSA )
      {
        /* POS request: set ver, datum, unit, acry, num fields in URL*/
        GNSS_SNPRINTF( url_buf, 
                        TM_JGPS_REQ_ULENGTH_MAX + TM_JGPS_REQ_ULENGTH_NULL,
                        "&ver=%d&datum=%d&unit=%d&acry=%d&number=%d", 
                        sess_info_ptr->url_info.ver,
                        sess_info_ptr->url_info.datum, 
                        sess_info_ptr->url_info.unit, 
                        sess_info_ptr->url_info.acry, 
                        sess_info_ptr->url_info.num );
  
      }
      else if ( sess_info_ptr->req_mode == TM_JGPS_SESS_REQ_MODE_MSB_DLOAD )
      {
        /* POS request: set ver, datum, unit, acry, num fields in URL*/
        GNSS_SNPRINTF( url_buf, 
                        TM_JGPS_REQ_ULENGTH_MAX + TM_JGPS_REQ_ULENGTH_NULL,
                        "&ver=%d", 
                        sess_info_ptr->url_info.ver );      
      }
      else
      {
        status = FALSE;
        break;
      }
      
      /* The string length consists of the header, URL address, and parameters */
      if( sess_info_ptr->url_info.url != NULL )
      {
        url_len = GNSS_STRLEN( url_hdr ) 
                  + GNSS_STRLEN( sess_info_ptr->url_info.url ) 
                  + GNSS_STRLEN( url_buf );
      }
      else
      {
        MSG_ERROR("JGPS URL is invalid",0,0,0);
        status = FALSE;
        break;
      }

      /* Set the URL address in the global session structure */
      if ( url_len <= TM_JGPS_REQ_ULENGTH_MAX )
      {
        /* Set the URL data to be sent */
        GNSS_STRLCPY( sess_info_ptr->req_param.url, 
                     url_hdr, 
                     sizeof(sess_info_ptr->req_param.url) );
        
        GNSS_STRLCAT( sess_info_ptr->req_param.url, 
                     sess_info_ptr->url_info.url,
                     sizeof(sess_info_ptr->req_param.url) );
        
        GNSS_STRLCAT( sess_info_ptr->req_param.url, 
                     url_buf,
                     sizeof(sess_info_ptr->req_param.url) );

        /* Set the URL length in the request message to be sent*/
        sess_info_ptr->req_param.url_length = url_len;
      }
      else
      {
        MSG_ERROR("URL length error:%d", url_len,0,0);
        status = FALSE;
        break;
      }
    }
  }while (0);
  
  return status;
} /* End tm_jgps_make_url */


/*===========================================================================

FUNCTION tm_jgps_core_add_pkt_hdr_and_send

DESCRIPTION
  This function pre-pends the KDDI specific header to the data and sends 
  out the data through the IS-801 Driver
  
DEPENDENCIES

RETURN VALUE
   Number of bytes sent
   
SIDE EFFECTS
  None 

===========================================================================*/

int tm_jgps_core_add_pkt_hdr_and_send
(
    tm_jgps_scu_cmd_id_type cmd_id,
    byte*  start,
    uint16 length
)
{  
  /* Local Variables */
  int                   ret_val = -1;
  tm_jgps_i_hdr_s_type  scu_header;
  byte                  *ms_scu_pkt_ptr;
  int16                 ms_scu_pkt_size = 0;

  /*------------------------------------------------------------------------
     Check input parameters
  -------------------------------------------------------------------------*/
  if( start == NULL )
  {
    MSG_ERROR("Null data pointer during TM-JGPS send attempt (len=%d)", length, 0, 0);  
    return -1;
  }

  /* Allocate memory for length of data and SCU header (6 bytes) */
  ms_scu_pkt_ptr = (byte *)pdsm_getbuf( length+TM_JGPS_HDR_SIZE );

  if(ms_scu_pkt_ptr == NULL)
  {
    MSG_ERROR("Can not get buffer", 0, 0, 0);
    return -1;
  }
  
  /*------------------------------------------------------------------------
     Init the MS-SCU header
  -------------------------------------------------------------------------*/
  scu_header.service_id[0] = TM_JGPS_SERV_ID_S;
  scu_header.service_id[1] = TM_JPGS_SERV_ID_T;
  /* Assume C-GPS (Aries) is always capable of MS-B, otherwise prot_version
     should be TM_JGPS_PROT_VER_MSA_ONLY
  */
  scu_header.prot_version  = TM_JGPS_PROT_VER_MSB; 
  
  /* Header length include 1 byte for Command ID */
  scu_header.length        = length + TM_JGPS_CMD_ID_SIZE; 
  scu_header.cmd_id        = cmd_id;

  /* Convert the MS-SCU header information */
  tm_jgps_xlate_int_ms_scu_hdr( ms_scu_pkt_ptr, &scu_header );

  /*------------------------------------------------------------------------
     Append the data portion of the packet after the SCU header (6 bytes)
  -------------------------------------------------------------------------*/    
  memscpy((void *)&(ms_scu_pkt_ptr[TM_JGPS_HDR_SIZE]),
         length,
         start, 
         length);

  /* Size of MS-SCU header (6 bytes) and length of data*/
  ms_scu_pkt_size = TM_JGPS_HDR_SIZE + length;

  /*------------------------------------------------------------------------
     Send the Data through 1x UP (IS-801 Drv) open socket
  -------------------------------------------------------------------------*/    
  if( tm_1x_up_is801_drv_send_data( ms_scu_pkt_ptr, ms_scu_pkt_size ) )
  {
    MSG_HIGH("JGPS: Success sending SCU pkt length:%d, ",ms_scu_pkt_size,0,0); 
    ret_val = length;
  }
  else
  {
    MSG_ERROR("JGPS: Failed to send SCU pkt length:%d, ",ms_scu_pkt_size,0,0); 
  }

  (void)pdsm_freebuf((char *)ms_scu_pkt_ptr);

  return ret_val;
} /* tm_jgps_core_add_pkt_hdr_and_send */

/*===========================================================================

FUNCTION tm_jpgs_core_send_msg

DESCRIPTION
  This function sends a message to SCU for POS req, ACQ req, POS cancel, 
  ACQ cancel, ACQ complete.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
boolean tm_jgps_core_send_msg
(
  tm_jgps_scu_cmd_id_type   cmd_id, 
  void                      *data_ptr
)
{
  /* Local Variables */
  boolean   status = FALSE;
  byte      pm_data[TM_JGPS_MAX_DATA_SIZE];
  int       pm_data_length = 0; /* Total length of data to send */
  
  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();

  /* Init the data
  */
  memset( pm_data, 0x00, sizeof(pm_data));
  
  MSG_HIGH("MS to SCU msg, cmd_id=0x%x",cmd_id,0,0);

  switch( cmd_id )
  {
    /*------------------------------------------------------------------------
         Position Request (MS-A) or Acquisition Request (MS-B Download)
    -------------------------------------------------------------------------*/
    case TM_JGPS_CMD_ID_START_ACQ_REQUEST:
    case TM_JGPS_CMD_ID_START_POS_REQUEST:
  
      /*------------------------------------------------------------------------
         Post the JGPS progress event
      -------------------------------------------------------------------------*/

      MSG_HIGH("JGPS: Sent request (cmd_id=0x%x), wait'g for response from SCU",
               cmd_id,0,0);
      
      /* Use the session info. lr resp to temporarily fire payload, this
         struct will be overwritten when IS-801 drv provide LR data
      */
      sess_info_ptr->lr_resp.client_id = -1; /* Indicate to client ID other than self */
  
      if (cmd_id  == TM_JGPS_CMD_ID_START_ACQ_REQUEST )
      {
        sess_info_ptr->lr_resp.pd_info.jgps_prog_status = PDSM_PD_JGPS_PROG_SEND_ACQ_REQ;
      }
      else
      {
        sess_info_ptr->lr_resp.pd_info.jgps_prog_status = PDSM_PD_JGPS_PROG_SEND_POS_REQ;
      }
  
      /* Post the event to TM-Core*/
      (void)tm_post_pd_event( TM_PRTL_TYPE_1X_UP,
                              sess_info_ptr->tm_core_sess_handle,
                              PDSM_PD_EVENT_JGPS_PROGRESS, 
                              &sess_info_ptr->lr_resp,
                              0);
  
      /*------------------------------------------------------------------------
         Start the KDDI specific timers
      -------------------------------------------------------------------------*/    
      /* Set T02 timer for response from SCU to this request */
      (void)os_TimerStart(sess_info_ptr->t02_timer,
                          TM_JGPS_SCU_T02_TIMER_CNT,
                          NULL);
        
      /* Set T04 timer for Positioning completion */
      (void)os_TimerStart(sess_info_ptr->t04_timer,
                          TM_JGPS_SCU_T04_TIMER_CNT,
                          NULL);
      
      /*------------------------------------------------------------------------
         Translate the data into external SCU format
      -------------------------------------------------------------------------*/
      tm_jgps_xlate_int_ms_scu_req( pm_data, 
                                    &sess_info_ptr->req_param );
      
      /* Length of this capsule is, 38 + ulength
      */
      pm_data_length = TM_JGPS_REQ_URL_OFFSET + sess_info_ptr->req_param.url_length;
      break;
  
    /*------------------------------------------------------------------------
           Position Cancel Request (MS-A) 
           or Acquisition Cancel Request (MS-B Download)
    -------------------------------------------------------------------------*/
    case TM_JGPS_CMD_ID_CANCEL_POS_REQUEST:
    case TM_JGPS_CMD_ID_CANCEL_ACQ_REQUEST:
      /* Init the cause code from the calling function.
      */
      tm_jgps_xlate_int_cancel_req( 
        &pm_data[0],
        (tm_jgps_i_cancel_req_s_type *)data_ptr );

      MSG_HIGH("JGPS: Send cancel request (cmd_id=0x%x, cause=%d)",
                cmd_id,
                ((tm_jgps_i_cancel_req_s_type *)data_ptr)->cause,
                0 );

      pm_data_length = sizeof(tm_jgps_i_cancel_req_s_type); 
      break;
      
   /*------------------------------------------------------------------------
          Acquisition Complete Response to SCU (MS-B Download)
   -------------------------------------------------------------------------*/
    case TM_JGPS_CMD_ID_ACQ_COMPLETE:
      pm_data_length = 0;   /* There is no payload which goes along with GPS data acquistion. */
      break;
      
   /*------------------------------------------------------------------------
             Unknown command ID
   -------------------------------------------------------------------------*/
    default:
      MSG_ERROR("CMD ID not supported",0,0,0);
      break;
  }

  
  /*------------------------------------------------------------------------
     Send the Data to SCU
  -------------------------------------------------------------------------*/
  if (pm_data_length != 0 || cmd_id == TM_JGPS_CMD_ID_ACQ_COMPLETE)
  {
    /* Message exchanges (non IS801) between MS and SCU */
    MSG_HIGH("SCU specific message=>SCU",0,0,0);

    /* Pre-pend the MS-SCU header and send the message */
    if(pm_data_length == 
            tm_jgps_core_add_pkt_hdr_and_send( cmd_id, pm_data, pm_data_length))
    {
      status = TRUE;
    }
  }
  
  return status;
} /* tm_jgps_core_send_msg() */


/*===========================================================================

FUNCTION tm_jgps_core_send_message_cancel

DESCRIPTION
   This function send the correct cancel message to SCU.  Determines if this
   cancel message is for MS-A (POS cancel) or MS-B dload (ACQ cancel)

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_jgps_core_send_msg_cancel
(
  byte cancel_reason
)
{
  /* Local Variables */
  boolean ret_val = FALSE;
  tm_jgps_i_cancel_req_s_type cancel_data;

  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();
  
  /* Set the Cancel reason */
  cancel_data.cause = cancel_reason;
  
  /*-----------------------------------------------------------------------
      Cancellation message to be sent to SCU for error scenarios
  ------------------------------------------------------------------------*/
  if (sess_info_ptr->req_mode == TM_JGPS_SESS_REQ_MODE_MSA)
  {
    /* POS Cancel */
    ret_val = tm_jgps_core_send_msg( TM_JGPS_CMD_ID_CANCEL_POS_REQUEST, 
                                     (void *)&cancel_data );
  }
  else if (sess_info_ptr->req_mode == TM_JGPS_SESS_REQ_MODE_MSB_DLOAD)
  {
    /* ACQ Cancel */
    ret_val = tm_jgps_core_send_msg( TM_JGPS_CMD_ID_CANCEL_ACQ_REQUEST,
                                     (void *)&cancel_data );
  }
  else
  {
    MSG_ERROR("Request mode (%d), is not handled", 
              sess_info_ptr->req_mode,0,0);
  }

  return ret_val;
} /* tm_jgps_core_send_msg_cancel */


/*===========================================================================

FUNCTION tm_jgps_core_send_msg_cancel_others

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_jgps_core_send_msg_cancel_others
(
  pdsm_pd_end_e_type error
)
{
  /* Send cancel request for reason not defined */
  if( error != PDSM_PD_END_SESS_NONE)
  {
    return tm_jgps_core_send_msg_cancel( TM_JGPS_CANCEL_CAUSE_OTHERS );
  }

  return FALSE;
  
} /* tm_jgps_core_send_msg_cancel_others */


/*===========================================================================

FUNCTION tm_jgps_core_send_msg_cancel_rej

DESCRIPTION
    Determine the cause for the cancellation from MS->SCU. Send the
    cancellation message.
    
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_jgps_core_send_msg_cancel_rej
(
  pdsm_pd_end_e_type            error,
  pdsm_pd_dload_data_s_type     *dload_ptr
)
{
  /* Local Variables */
  boolean ret_val = FALSE;
  byte    cause = TM_JGPS_CANCEL_CAUSE_OTHERS;
  tm_sess_stop_reason_e_type abort_reason = TM_STOP_REASON_NONE;
  
  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();
  
  /*----------------------------------------------------------------------
     Check the input parameters
  ----------------------------------------------------------------------*/
  if(dload_ptr == NULL)
  {
    MSG_ERROR("Received Null dload_ptr",0,0,0);
    return FALSE;
  }

  /*----------------------------------------------------------------------
     Determine the cause for reject
  ----------------------------------------------------------------------*/
  if( error != PDSM_PD_END_SESS_NONE )
  {
    /* LR reject */
    if( PDSM_PD_DLOAD_LOCATION & dload_ptr->jgps_msdload_info.data_type)
    {
      cause = TM_JGPS_CANCEL_CAUSE_REJECT_LR;
      if(sess_info_ptr->req_mode ==  TM_JGPS_SESS_REQ_MODE_MSB_DLOAD)
      {
        abort_reason = TM_STOP_REASON_JGPS_IS801_SEEDREJ;
      }
      else
      {
        abort_reason = TM_STOP_REASON_JGPS_IS801_LRREJ;
      }
    }

    /* AA reject */
    if( PDSM_PD_DLOAD_AA & dload_ptr->jgps_msdload_info.data_type)
    {
      cause = TM_JGPS_CANCEL_CAUSE_REJECT_ASSIST;
      abort_reason = TM_STOP_REASON_JGPS_IS801_AAREJ;
    }

    /* SA reject */
    if( PDSM_PD_DLOAD_SA & dload_ptr->jgps_msdload_info.data_type)
    {
      cause = TM_JGPS_CANCEL_CAUSE_OTHERS;
    }

    /* Ephemeris reject */
    if( PDSM_PD_DLOAD_EPHEMERIS & dload_ptr->jgps_msdload_info.data_type)
    {
      cause = TM_JGPS_CANCEL_CAUSE_REJECT_EPH;
      abort_reason = TM_STOP_REASON_JGPS_IS801_EPHREJ;
    }

    /* Almanac reject */
    if( PDSM_PD_DLOAD_ALMANAC & dload_ptr->jgps_msdload_info.data_type)
    {
      cause = TM_JGPS_CANCEL_CAUSE_REJECT_ALM;
      abort_reason = TM_STOP_REASON_JGPS_IS801_ALMREJ;
    }

    /* Send the cancel msg to SCU */
    ret_val = tm_jgps_core_send_msg_cancel(cause);
  }

  if( abort_reason == TM_STOP_REASON_NONE)
  {
    /* Determine the stop reason MSB, no fix with dload data*/
    if(sess_info_ptr->req_mode ==  TM_JGPS_SESS_REQ_MODE_MSB_DLOAD)
    {
      abort_reason =  TM_STOP_REASON_JGPS_DMSB_ERROR
                         + dload_ptr->jgps_msdload_info.data_type - 1;
    }
    else /* MSA: no fix with dload data */
    {
      abort_reason =  TM_STOP_REASON_JGPS_DPOSCOMP_NG 
                        + dload_ptr->jgps_msdload_info.data_type - 1;
    }
  }
  
  /* Abort and recover timer for DMSB_ERROR */
  tm_jgps_core_abort_recover( abort_reason,
                              PRTL_SESS_STOP_SINGLE_FIX );
  return ret_val;
}/* tm_jgps_core_send_msg_cancel_rej */


/*===========================================================================

FUNCTION tm_jgps_core_send_msg_cancel_timeout

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tm_jgps_core_send_msg_cancel_timeout(void)
{
  return tm_jgps_core_send_msg_cancel( TM_JGPS_CANCEL_CAUSE_TIMEOUT );
} /* tm_jgps_core_send_msg_cancel_timeout */


/*===========================================================================

FUNCTION tm_jgps_core_scu_msg_hdr_handler

DESCRIPTION
  This function is used handle the SCU->MS data packet header

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
boolean tm_jgps_core_scu_msg_hdr_handler
(
  byte     *hdr_ptr,
  uint16   hdr_ptr_len
)
{
  /* Local Variables */
  boolean ret_val = TRUE;

  tm_jgps_gen_info_s_type *gen_info_ptr = TM_JGPS_GEN_INFO_PTR();
  
  /*----------------------------------------------------------------------
     Check the input parameters
  ----------------------------------------------------------------------*/
  if( hdr_ptr == NULL || 
      hdr_ptr_len < TM_JGPS_HDR_SIZE )
  {
    MSG_ERROR( "Received Null data_ptr or incorrect length (%d)",
               hdr_ptr_len,0,0 );
    
    return FALSE;
  }

  /*----------------------------------------------------------------------
     MS-SCU header need to be translated to internal format.
  ----------------------------------------------------------------------*/
  tm_jgps_xlate_ext_ms_scu_hdr(hdr_ptr,&(gen_info_ptr->sess_info.resp_pkt.hdr));

  MSG_HIGH("JGPS: Pkt hdr cmd=%d,len=%d,prot_ver=%d from SCU",
           gen_info_ptr->sess_info.resp_pkt.hdr.cmd_id,
           gen_info_ptr->sess_info.resp_pkt.hdr.length,
           gen_info_ptr->sess_info.resp_pkt.hdr.prot_version);

  /* Set the incoming payload data length */
  gen_info_ptr->comm_info.incoming_data_length 
             = gen_info_ptr->sess_info.resp_pkt.hdr.length - TM_JGPS_CMD_ID_SIZE;

  return ret_val;
}/* tm_jgps_core_scu_msg_hdr_handler */


/*===========================================================================

FUNCTION tm_jgps_core_scu_msg_payload_handler

DESCRIPTION
  This function is used handle the SCU->MS data packet

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
boolean tm_jgps_core_scu_msg_payload_handler
(
  byte     *data_ptr,
  uint16   data_ptr_len
)
{
  /* Local Variables */
  boolean                              ret_val = TRUE;
  tm_jgps_i_pos_resp_s_type            pos_resp;
  tm_jgps_i_pos_comp_s_type            lr_resp;
  tm_jgps_i_stop_req_s_type            stop_resp;
  tm_1x_up_is801_drv_req_param_u_type  jgps_req_param;
   
  tm_jgps_gen_info_s_type *gen_info_ptr = TM_JGPS_GEN_INFO_PTR();
  byte   bytes_consumed_801_1  = 0;

  /*----------------------------------------------------------------------
     Check the input parameters
  ----------------------------------------------------------------------*/
  if(data_ptr == NULL || data_ptr_len == 0)
  {
    MSG_ERROR("Received Null data_ptr",0,0,0);
    return FALSE;
  }

  /*----------------------------------------------------------------------
    Validate the header
  ----------------------------------------------------------------------*/
  /* Check the Service ID fields */
  if( !( gen_info_ptr->sess_info.resp_pkt.hdr.service_id[0] == TM_JGPS_SERV_ID_S
         && gen_info_ptr->sess_info.resp_pkt.hdr.service_id[1] == TM_JPGS_SERV_ID_T )
    )
  {
    MSG_ERROR("JGPS: Recv hdr service id (%d %d) invalid, Reject msg", 
              gen_info_ptr->sess_info.resp_pkt.hdr.service_id[0],
              gen_info_ptr->sess_info.resp_pkt.hdr.service_id[1],
              0);
    
    return FALSE;
  }  
  
  /* Check the Protocol Version */
  if ( gen_info_ptr->sess_info.resp_pkt.hdr.prot_version != TM_JGPS_PROT_VER_MSB)
  {
    MSG_ERROR("JGPS: Recv hdr prot ver %d invalid, Reject msg", 
              gen_info_ptr->sess_info.resp_pkt.hdr.prot_version,0,0);
    return FALSE;
  }

  /*----------------------------------------------------------------------
    Translate the message, based on the header cmd_id
  ----------------------------------------------------------------------*/
  switch (gen_info_ptr->sess_info.resp_pkt.hdr.cmd_id)
  {
    /*------------------------------------------------------------------------
          IS-801 Packet
    -------------------------------------------------------------------------*/
    case TM_JGPS_CMD_ID_IS801_FLINK:
      if (data_ptr_len <= TM_JGPS_CMD_ID_SIZE) 
      {
        MSG_ERROR("JGPS: Recv data length %d invalid", data_ptr_len,0,0);
        return FALSE;
      }

      MSG_HIGH("JGPS: Forward data of length %d to IS801 module for decoding",
               data_ptr_len-TM_JGPS_CMD_ID_SIZE,0,0);

      /* Log the IS-801 message */
      is801_util_log_flink_msg((void *)&(data_ptr[TM_JGPS_CMD_ID_SIZE]), 
                               data_ptr_len-TM_JGPS_CMD_ID_SIZE);

      /* Forward it to IS801 for decoding */
      (void)is801_decode_fwd_link_pddm(TM_PRTL_TYPE_1X_UP, 
                                       (byte *)&(data_ptr[TM_JGPS_CMD_ID_SIZE]),
                                       data_ptr_len-TM_JGPS_CMD_ID_SIZE,
                                       &bytes_consumed_801_1);
  
      break;

    /*------------------------------------------------------------------------
          POS or ACQ Response
    -------------------------------------------------------------------------*/
    case TM_JGPS_CMD_ID_POS_RESPONSE:
    case TM_JGPS_CMD_ID_ACQ_RESPONSE:
       MSG_HIGH("JGPS: Start resp rxed from SCU %d",
                gen_info_ptr->sess_info.resp_pkt.hdr.cmd_id,0,0);

       /* Clear the T02 timer and then process the response. */
       os_TimerStop(gen_info_ptr->sess_info.t02_timer);

       if (data_ptr_len < TM_JGPS_EXT_POS_RESP_S_TYPE_SZ)
       {
         MSG_ERROR("JGPS: Recv data length %d invalid", data_ptr_len,0,0);
         return FALSE;
       }

       /* Translate the POS/ACQ response */
       tm_jgps_xlate_ext_pos_resp(data_ptr, &pos_resp);

       /*----------------------------------------------------------------------
           Check the session state
       ----------------------------------------------------------------------*/
       if( gen_info_ptr->sess_info.state == TM_JGPS_SESS_STATE_WAIT_SCU_START_REQ_RESP )
       { 
         /* Result is OK */
         if ( pos_resp.result == TM_JGPS_SCU_MS_POS_RESP_RESULT_OK ) 
         {
           MSG_HIGH("JGPS: Resp result OK from SCU (cmd_id=%h, result=%d, mode=%d",
                    gen_info_ptr->sess_info.resp_pkt.hdr.cmd_id,
                    pos_resp.result,
                    gen_info_ptr->sess_info.req_mode);
           
           /* Inform 1x UP (IS-801 Drv) to continue the session*/
           jgps_req_param.start_param.wake_tm_core = TRUE; /*lint !e506 */
           jgps_req_param.start_param.actions      = NULL;
  
           jgps_req_param.start_param.start_sess_req_param 
                 = &(gen_info_ptr->sess_info.smod_req_param);

           ret_val = tm_1x_up_is801_drv_req(
                                            gen_info_ptr->sess_info.tm_core_sess_handle, 
                                            TM_IS801_DRV_REQ_CONT,
                                            jgps_req_param
                                           );
           
           /* Update the session state, to session grant MS-A or MS-B dload request */
           ret_val = tm_jgps_core_update_sess_state(TM_JGPS_SESS_STATE_START_REQ_GRANT);
                                            
         }
         else if (pos_resp.result == TM_JGPS_SCU_MS_POS_RESP_RESULT_NG )
         {
           /* Result is No Good (NG) Error scenario */
           tm_jgps_core_abort_trans_scu_error(pos_resp.cause);
         }
         else 
         {
           /* Unknown error scenario */
           tm_jgps_core_abort_trans_scu_error(pos_resp.cause);
         }
       } /* MS_OK */
       else
       {
         MSG_ERROR("JGPS: SCU resp, not allowed, wrong state",0,0,0);

         tm_jgps_core_abort_trans_scu_error(pos_resp.cause);
       }
       break;

    /*------------------------------------------------------------------------
             POS Complete
    -------------------------------------------------------------------------*/
    case TM_JGPS_CMD_ID_POS_COMPLETE:
       MSG_HIGH("JGPS: Pos complete rxed from SCU",0,0,0);
       
       /* Clear the T04 timer and then process the response. */
       os_TimerStop(gen_info_ptr->sess_info.t04_timer);

       if (data_ptr_len < TM_JGPS_EXT_POS_COMP_S_TYPE_SZ)
       {
         MSG_ERROR("JGPS: Recv data length %d invalid", data_ptr_len,0,0);
         return FALSE;
       }

       /* Translate the POS complete response */
       tm_jgps_xlate_ext_pos_comp(data_ptr,&lr_resp);

       /* Check the result of the SCU->MS POS complete packet */
       if ( lr_resp.result == TM_JGPS_SCU_MS_POS_COMP_RESULT_OK )
       {
         /* Make sure JGPS module is waiting the POS complete response */
         if (gen_info_ptr->sess_info.state == TM_JGPS_SESS_STATE_WAIT_SCU_STOP_REQ_RESP)
         {
           /* make sure fix data length is valid */
           if (lr_resp.flength > 0 && lr_resp.flength <= sizeof(lr_resp.data) &&
               data_ptr_len >= TM_JGPS_EXT_POS_COMP_S_TYPE_SZ + lr_resp.flength)
           {
             /* copy the fix data to lr_resp.data */
             memscpy((void *)lr_resp.data, sizeof(lr_resp.data), data_ptr + TM_JGPS_EXT_POS_COMP_S_TYPE_SZ, lr_resp.flength);
           }
           else
           {
             MSG_ERROR("JGPS: Recv data length %d invalid (flength=%d)", data_ptr_len, lr_resp.flength,0);
             return FALSE;
           }

           /* Format the fix data returned to JGPS clients */
           tm_jgps_core_convert_scu_fix_data(lr_resp.flength, lr_resp.data);
           
           /* Set the PD info data to be sent to TM-Core and client */
           gen_info_ptr->sess_info.lr_resp.pd_info.pd_data.opt_field_mask 
                                                            |= PDSM_PM_DATA_AVAIL;
           gen_info_ptr->sess_info.lr_resp.pd_info.pd_data.pm_data 
                                           = &gen_info_ptr->sess_info.scu_lr_resp;
           /* Post the Position event to TM-Core */
           (void)tm_post_pd_event( TM_PRTL_TYPE_1X_UP,
                                   gen_info_ptr->sess_info.tm_core_sess_handle,
                                   PDSM_PD_EVENT_POSITION, 
                                   &gen_info_ptr->sess_info.lr_resp,
                                   0);

       
           /* Close the TCP connection held by IS-801 Driver for pos complete */
           (void)tm_jgps_core_abort_recover( TM_STOP_REASON_COMPLETED, 
                                             PRTL_SESS_STOP_SINGLE_FIX );
         }
         else
         {
           MSG_ERROR("JGPS: Late response",0,0,0);
           tm_jgps_core_abort_trans_scu_error(lr_resp.cause);
         }
       }
       else
       {
         MSG_ERROR("JGPS: Pos response Bad",0,0,0);
         tm_jgps_core_abort_trans_scu_error(lr_resp.cause);
       }

       /* Update the session state */
       ret_val = tm_jgps_core_update_sess_state(TM_JGPS_SESS_STATE_WAIT_TCP_DISCONNECT);
       break;

    /*------------------------------------------------------------------------
                Stop Request from SCU
    -------------------------------------------------------------------------*/
    case TM_JGPS_CMD_ID_STOP_POS_REQUEST:
    case TM_JGPS_CMD_ID_STOP_ACQ_REQUEST:
      if (data_ptr_len < TM_JGPS_EXT_CANCEL_REQ_S_TYPE_SZ)
      {
        MSG_ERROR("JGPS: Recv data length %d invalid", data_ptr_len,0,0);
      }
      else
      {
        tm_jgps_xlate_ext_stop_req( data_ptr, &stop_resp );

        MSG_MED("JGPS:Recv Stop Cause %d",stop_resp.cause,0,0);
      }

      /* End the session and close the TCP connection */
      tm_jgps_core_abort_trans_scu_error(stop_resp.cause);
      break;


    default:
      MSG_ERROR("JGPS: Message=%d not handled",
                gen_info_ptr->sess_info.resp_pkt.hdr.cmd_id,0,0);
      break;
  } /* cmd_id */

  return ret_val;
  
} /* tm_jgps_core_scu_msg_payload_handler */


/*===========================================================================

FUNCTION tm_jgps_core_comm_notify_handler

DESCRIPTION
  This function is used to handle notifications from PD COMM.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
boolean tm_jgps_core_comm_notify_handler
(
  pd_comms_client_ipc_message_type  msg_type,
  void *msg_data_ptr
)
{
  /* Local Variables */
  pd_comms_ipc_notification_hdr_type*  notify_hdr_ptr;
  boolean                              ret_val = FALSE;
  uint16                               read_data_len = 0;
  
  tm_jgps_gen_info_s_type *gen_info_ptr = TM_JGPS_GEN_INFO_PTR();
  
  /*----------------------------------------------------------------------
     Check the input parameters
  ----------------------------------------------------------------------*/
  if(msg_data_ptr == NULL)
  {
    MSG_ERROR("Received Null msg_data_ptr",0,0,0);
    return FALSE;
  }

  /*----------------------------------------------------------------------
     Cast the void data ptr to known data structure
  ----------------------------------------------------------------------*/
  notify_hdr_ptr = (pd_comms_ipc_notification_hdr_type*)msg_data_ptr;

  /*----------------------------------------------------------------------
     Process the comm notification
  ----------------------------------------------------------------------*/
  switch(notify_hdr_ptr->e_type)
  {
    /*----------------------------------------------------------------------
       Read Data indication
    ----------------------------------------------------------------------*/
    case READ_DATA:
      MSG_MED("JGPS: TCP/IP comm recv'd read data notification", 0, 0, 0);
  
      if(gen_info_ptr->comm_info.comm_state == TM_JGPS_COMM_STATE_CONNECTED)
      {
        /* Read the header data */
        if(gen_info_ptr->comm_info.recv_state == TM_JGPS_COMM_RECV_STATE_HEADER)
        {
          read_data_len = TM_JGPS_HDR_SIZE;
          MSG_HIGH("JGPS: Reading JGPS header size %d bytes ",read_data_len,0,0);
        }
        else /* Read the pkt data */
        {
          /* Only read payload if size is greater than 0 */
          if ( gen_info_ptr->comm_info.incoming_data_length > 0 )
          {
            read_data_len = gen_info_ptr->comm_info.incoming_data_length;
            MSG_HIGH("JGPS: Reading JGPS packet payload %d bytes", read_data_len, 0, 0);
          }
        }

        /* Call 1x UP (IS-801 driver) func to read data from PDCOMM */
        if(read_data_len > 0)
        {
          ret_val = tm_1x_up_is801_drv_read_data(read_data_len, FALSE);
        }
      }
      else
      {
        MSG_ERROR("JGPS: Read data in error state %d", 
                  gen_info_ptr->comm_info.comm_state, 0, 0);
        ret_val = FALSE;
      }
      break;

    /*----------------------------------------------------------------------
       State change
    ----------------------------------------------------------------------*/
    case STATE_CHANGE:
      MSG_MED("JGPS: TCP/IP comm recv'd state change notification, new state = %d", 
              notify_hdr_ptr->z_params.e_new_state, 0, 0);
  
      /* Update the Comm info state */
      if(notify_hdr_ptr->z_params.e_new_state == PDCOMM_STATE_CONNECTED)
      {
        gen_info_ptr->comm_info.comm_state = TM_JGPS_COMM_STATE_CONNECTED;
      }
      else if(notify_hdr_ptr->z_params.e_new_state == PDCOMM_STATE_DISCONNECTED)
      {
        gen_info_ptr->comm_info.comm_state = TM_JGPS_COMM_STATE_DISCONNECTED;

        /* Init the comm info data */
        tm_jgps_core_comm_info_init();
         
        /* If request has already come in from TM-Core, kick start the session */
        if(FALSE == tm_jgps_core_restart_session() )
        {
          /* Reset the session state */
          tm_jgps_core_reinit();          
        }
      }
      else if(notify_hdr_ptr->z_params.e_new_state == PDCOMM_STATE_CONNECTING)
      {
        gen_info_ptr->comm_info.comm_state = TM_JGPS_COMM_STATE_CONNECTING;
      }
      else if(notify_hdr_ptr->z_params.e_new_state == PDCOMM_STATE_DISCONNECTING)
      {
        gen_info_ptr->comm_info.comm_state = TM_JGPS_COMM_STATE_DISCONNECTING;

        tm_jgps_core_update_sess_state(TM_JGPS_SESS_STATE_WAIT_TCP_DISCONNECT);
      }
      else
      {
        MSG_ERROR("Unknown state %d",notify_hdr_ptr->z_params.e_new_state,0,0);

      }

      /* Send notification command to IS-801 Driver */
      ret_val = tm_1x_up_is801_drv_data_pipe_handler(msg_type, msg_data_ptr);
      
      break;

      
    /*----------------------------------------------------------------------
       Default
    ----------------------------------------------------------------------*/
    default:
      MSG_HIGH("JGPS: Does not handle comm notifiy %d", 
               notify_hdr_ptr->e_type, 0, 0);
      break;
  }

  return ret_val;
} /* tm_jgps_core_comm_notify_handler */

/*===========================================================================

FUNCTION tm_jgps_core_comm_write_ack_handler

DESCRIPTION
  This function is used to handle write ack notification from PDCOMM.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
boolean tm_jgps_core_comm_write_ack_handler
(
  pd_comms_client_ipc_message_type  msg_type,
  void                              *msg_data_ptr
)
{
  /* Local Variables */
  boolean                              ret_val = FALSE;
  
  tm_jgps_gen_info_s_type *gen_info_ptr = TM_JGPS_GEN_INFO_PTR();
  
  /*----------------------------------------------------------------------
     Check the input parameters
  ----------------------------------------------------------------------*/
  if(msg_data_ptr == NULL)
  {
    MSG_ERROR("Received Null msg_data_ptr",0,0,0);
    return FALSE;
  }

  /* Make sure JGPS module is waiting the POS complete response */
  if (gen_info_ptr->sess_info.state == TM_JGPS_SESS_STATE_WAIT_SCU_STOP_REQ_RESP)
  {
    MSG_MED("JGPS: write ack recv, while waiting for LR resp from SCU",0,0,0);
    ret_val = TRUE;
  }

  return ret_val;

} /* tm_jgps_core_comm_write_ack_handler */


/*===========================================================================

FUNCTION tm_jgps_core_comm_ioctl_ack_handler

DESCRIPTION
  This function is used to handle IOCTL ACK from PD COMM.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
boolean tm_jgps_core_comm_ioctl_ack_handler
(
  pd_comms_client_ipc_message_type  msg_type,
  void *msg_data_ptr
)
{
  /* Local Variables */
  boolean ret_val = FALSE;
  pd_comms_ipc_ioctl_ack_hdr_type* ioctl_data_ptr;

  /*----------------------------------------------------------------------
     Check the input parameters
  ----------------------------------------------------------------------*/
  if(msg_data_ptr == NULL)
  {
    MSG_ERROR("Null msg_data_ptr",0,0,0);
    return FALSE;
  }

  ioctl_data_ptr = (pd_comms_ipc_ioctl_ack_hdr_type*)msg_data_ptr;
  MSG_MED("JGPS:IOCTL Ack for req %d",ioctl_data_ptr->q_ioctl_request,0,0);

  /*----------------------------------------------------------------------
     Handle the IOCTL command
  ----------------------------------------------------------------------*/
  switch(ioctl_data_ptr->q_ioctl_request)
  {
    case PDCOMM_IOCTL_CMD_GET_IP_ADDR:
      ret_val = tm_jgps_core_trans_ms_ip_addr( (byte *)ioctl_data_ptr->p_data,
                                               ioctl_data_ptr->q_argument_len);
      break;
    default:
      MSG_MED("JGPS: pass IOCTL %d to IS-801 drv",ioctl_data_ptr->q_ioctl_request,0,0);
      ret_val = tm_1x_up_is801_drv_data_pipe_handler(msg_type, msg_data_ptr);
      break;
  }
 
  return ret_val;
} /* tm_jgps_core_comm_ioctl_ack_handler */


/*===========================================================================

FUNCTION tm_jgps_core_comm_read_data_handler

DESCRIPTION
  This function is used to handle READ DATA from PD COMM.

RETURN VALUE
  TRUE: No error
  FALSE: Error

DEPENDENCIES

===========================================================================*/
boolean tm_jgps_core_comm_read_data_handler(void *msg_data_ptr)
{
  /* Local Variables */
  boolean ret_val= TRUE;
  pd_comms_ipc_read_data_hdr_type *read_hdr_ptr;

  tm_jgps_gen_info_s_type *gen_info_ptr = TM_JGPS_GEN_INFO_PTR();
  
  /*----------------------------------------------------------------------
     Check the input parameters
  ----------------------------------------------------------------------*/
  if(msg_data_ptr == NULL)
  {
    MSG_ERROR("Received Null msg_data_ptr",0,0,0);
    return FALSE;
  }

  /*----------------------------------------------------------------------
     Cast the input parameter to PDCOMM read data structure
  ----------------------------------------------------------------------*/
  read_hdr_ptr = (pd_comms_ipc_read_data_hdr_type*)msg_data_ptr;

  /*----------------------------------------------------------------------
      A read event has occured, invoked PDCOMM function to read out the data
  ----------------------------------------------------------------------*/
  if(read_hdr_ptr->w_data_len != 0)
  {
    /* Now read the actual data */
    if(gen_info_ptr->comm_info.recv_state  == TM_JGPS_COMM_RECV_STATE_HEADER)
    {
      MSG_HIGH("JGPS: Read header data of length %d",read_hdr_ptr->w_data_len,0,0);

      if (read_hdr_ptr->w_data_len >= TM_JGPS_HDR_SIZE )
      {
        /* Process the SCU->MS header */
        tm_jgps_core_scu_msg_hdr_handler((byte *)read_hdr_ptr->p_data, read_hdr_ptr->w_data_len);

        
        MSG_HIGH("JGPS: Read payload data of length %d",
                 gen_info_ptr->comm_info.incoming_data_length,0,0);
        
        /* After reading header, Set next recv state to payload */
        gen_info_ptr->comm_info.recv_state = TM_JGPS_COMM_RECV_STATE_PAYLOAD;
      }
      else
      {
        MSG_ERROR("JGPS: Error data length %d",read_hdr_ptr->w_data_len,0,0);
        ret_val = FALSE;
      }
    }
    else /* JGPS packet data */
    {
      MSG_HIGH("JGPS: Process and decode packet length %d",
               read_hdr_ptr->w_data_len,0,0);

      /* Process the SCU->MS packet */
      tm_jgps_core_scu_msg_payload_handler((byte *)read_hdr_ptr->p_data, read_hdr_ptr->w_data_len);
      
      /* After reading payload, Set next recv state to header */
      gen_info_ptr->comm_info.recv_state = TM_JGPS_COMM_RECV_STATE_HEADER;

      /* Reset the incoming data length */
      gen_info_ptr->comm_info.incoming_data_length = 0;
    }
  }
  else
  {
    /* Received data of zero length from TM-Core */
    MSG_ERROR("JGPS: Received data of zero length from TM-Core",0,0,0);
    ret_val = FALSE;
  }

  
  return ret_val;
} /* tm_jgps_core_comm_read_data_handler */


/*===========================================================================

FUNCTION tm_jgps_core_set_mode_info

DESCRIPTION
   Set the PPM and PRM mode info, support monitor mode
   
RETURN VALUE
  
DEPENDENCIES

===========================================================================*/
void tm_jgps_core_set_mode_info
(
  pdsm_pd_option_s_type    *option_ptr,
  pdsm_client_type_e_type  client_type
)
{
  /*----------------------------------------------------------------------
     Check the input parameters
  ----------------------------------------------------------------------*/
  if(option_ptr == NULL)
  {
    MSG_ERROR("Received Null option_ptr",0,0,0);
    return;
  }


  /*----------------------------------------------------------------------
     Set the Mode info data, num, sort type, etc
  ----------------------------------------------------------------------*/
  if( option_ptr->session == PDSM_PD_SESS_TYPE_NEW 
      && option_ptr->operation_mode == PDSM_SESSION_OPERATION_MSASSISTED 
      && client_type == PDSM_CLIENT_TYPE_TEST3 )
  {
    switch( option_ptr->mode_info.mode)
    {
      /* GPS only MODE */
      case PDSM_SESS_JGPS_TYPE_GPS:

        option_ptr->mode_info.ppm.num       = TM_JGPS_PPM_PN_MIN;    
        option_ptr->mode_info.ppm.sort_type = TM_JGPS_SORT_HIGH;

        option_ptr->mode_info.prm.num       = TM_JGPS_PRM_PRN_ALL;
        option_ptr->mode_info.prm.sort_type = TM_JGPS_SORT_HIGH;
        break;

      /* AFLT MODE */
      case PDSM_SESS_JGPS_TYPE_AFLT:

        option_ptr->mode_info.ppm.num       = TM_JGPS_PPM_PN_ALL;
        option_ptr->mode_info.ppm.sort_type = TM_JGPS_SORT_HIGH;

        option_ptr->mode_info.prm.num       = TM_JGPS_PRM_PRN_MIN;
        option_ptr->mode_info.prm.sort_type = TM_JGPS_SORT_HIGH;

        break;

      /* TEST MODE */
      case PDSM_SESS_JGPS_TYPE_JGPS_TEST:
          if( option_ptr->mode_info.ppm.num < TM_JGPS_PPM_PN_MIN)
          {
            option_ptr->mode_info.ppm.num = TM_JGPS_PPM_PN_MIN;
          }
          if( option_ptr->mode_info.ppm.sort_type > TM_JGPS_SORT_HIGH)
          {
            option_ptr->mode_info.ppm.sort_type = TM_JGPS_SORT_HIGH;
          }
          if( option_ptr->mode_info.prm.sort_type > TM_JGPS_SORT_HIGH)
          {
            option_ptr->mode_info.prm.sort_type = TM_JGPS_SORT_HIGH;
          }
          break;

      default:
        option_ptr->mode_info.mode = PDSM_SESS_JGPS_TYPE_HYBRID;

        option_ptr->mode_info.ppm.num       = TM_JGPS_PPM_PN_ALL;
        option_ptr->mode_info.ppm.sort_type = TM_JGPS_SORT_HIGH;

        option_ptr->mode_info.prm.num       = TM_JGPS_PRM_PRN_ALL;
        option_ptr->mode_info.prm.sort_type = TM_JGPS_SORT_HIGH;
        break;
    }
  }
  else
  {
    /* NORMAL MODE */
    option_ptr->mode_info.mode = PDSM_SESS_JGPS_TYPE_HYBRID;

    option_ptr->mode_info.ppm.num       = TM_JGPS_PPM_PN_ALL;
    option_ptr->mode_info.ppm.sort_type = TM_JGPS_SORT_HIGH;

    option_ptr->mode_info.prm.num       = TM_JGPS_PRM_PRN_ALL;
    option_ptr->mode_info.prm.sort_type = TM_JGPS_SORT_HIGH;
  }

} /* tm_jgps_core_set_mode_info */


/*===========================================================================

FUNCTION tm_jgps_core_mode_is_not_normal

DESCRIPTION

RETURN VALUE
  True if not normal (GPS/AFLT/TEST). False otherwise. (HYBRID)

DEPENDENCIES

===========================================================================*/
boolean tm_jgps_core_mode_is_not_normal( void )
{
  /* Local Variables */
  boolean    except_normal = FALSE;

  tm_jgps_gen_info_s_type   *gen_info_ptr = TM_JGPS_GEN_INFO_PTR();

  /*----------------------------------------------------------------------
     Only check for MS-A session, fail otherwise
  ----------------------------------------------------------------------*/
  if(   gen_info_ptr->sess_info.state != TM_JGPS_SESS_STATE_INIT
     && gen_info_ptr->sess_info.req_mode == TM_JGPS_SESS_REQ_MODE_MSA )
  {
    if( gen_info_ptr->app_info.tm_core_start_req.pd_option.session 
                                             == PDSM_PD_SESS_TYPE_NEW )
    {
      if( gen_info_ptr->app_info.tm_core_start_req.pd_option.mode_info.mode 
                                                  == PDSM_SESS_JGPS_TYPE_GPS  
          || gen_info_ptr->app_info.tm_core_start_req.pd_option.mode_info.mode 
                                                  == PDSM_SESS_JGPS_TYPE_AFLT 
          || gen_info_ptr->app_info.tm_core_start_req.pd_option.mode_info.mode 
                                                   == PDSM_SESS_JGPS_TYPE_JGPS_TEST )
      {
          except_normal = TRUE;
      }
    }
  }

  return except_normal;

} /* tm_jgps_core_mode_is_not_normal */


/*===========================================================================

FUNCTION tm_jgps_core_arrange_ppm_data

DESCRIPTION
 The GPS mode, the AFLT mode and TEST mode restrict the number of 
 Pilots and SVs to be used by the calculation of MS Assisted. 
 Also, TEST mode is to select the pilots and the SVs by ascending 
 or descending order. The PPM data is changed here depending on 
 PD mode setting.

RETURN VALUE
  None.

DEPENDENCIES

===========================================================================*/ 
void tm_jgps_core_arrange_ppm_data
(
  srch_ppm_rpt_type *ppm_ptr
)
{
  /* Local Variables */
  byte                  srch_ppm;
  byte                  num_ppm;
  int                   i, j;
  byte                  sort_f = FALSE;
  int4                  tmp_int4;
  byte                  tmp_byte;
  uint16                tmp_uint16;

  tm_jgps_gen_info_s_type   *gen_info_ptr = TM_JGPS_GEN_INFO_PTR();
  
  /*----------------------------------------------------------------------
     Check the input parameters
  ----------------------------------------------------------------------*/
  if(ppm_ptr == NULL)
  {
    MSG_ERROR("Received Null ppm_ptr",0,0,0);
    return;
  }

  /*----------------------------------------------------------------------
     Arrange the PPM data according to the mode, 
     *** only for monitor mode apps ***
  ----------------------------------------------------------------------*/
  if( tm_jgps_core_mode_is_not_normal()
      && gen_info_ptr->app_info.tm_core_start_req.client_type
                                       == PDSM_CLIENT_TYPE_TEST3 
    )
  {
    srch_ppm = ppm_ptr->num_pilots;

    
    if( gen_info_ptr->app_info.tm_core_start_req.pd_option.mode_info.ppm.num
                                                            != TM_JGPS_PPM_PN_ALL )
    {
      sort_f = TRUE;

      if( srch_ppm 
             > gen_info_ptr->app_info.tm_core_start_req.pd_option.mode_info.ppm.num
        )
      {
        ppm_ptr->num_pilots 
          = gen_info_ptr->app_info.tm_core_start_req.pd_option.mode_info.ppm.num;
      }

    }

    num_ppm = ppm_ptr->num_pilots;

    /* Sort the PPM data */
    if( sort_f == TRUE)
    {
      for( i = 0; i < srch_ppm - 1; i++)
      {
        for( j = i + 1; j < srch_ppm; j++)
        {
          if( gen_info_ptr->app_info.tm_core_start_req.pd_option.mode_info.ppm.sort_type ^
              ( ppm_ptr->pilot_results[i].peak_ecio
                > ppm_ptr->pilot_results[j].peak_ecio))
          {
            tmp_int4 = ppm_ptr->pilot_results[ i].peak_pos;
            ppm_ptr->pilot_results[i].peak_pos
                     = ppm_ptr->pilot_results[ j].peak_pos;
            ppm_ptr->pilot_results[j].peak_pos = tmp_int4;

            tmp_uint16 = ppm_ptr->pilot_results[ i].peak_pn;
            ppm_ptr->pilot_results[i].peak_pn
                     = ppm_ptr->pilot_results[ j].peak_pn;
            ppm_ptr->pilot_results[j].peak_pn = tmp_uint16;

            tmp_byte = ppm_ptr->pilot_results[ i].peak_rmse;
            ppm_ptr->pilot_results[i].peak_rmse
                     = ppm_ptr->pilot_results[ j].peak_rmse;
            ppm_ptr->pilot_results[j].peak_rmse = tmp_byte;

            tmp_byte = ppm_ptr->pilot_results[ i].peak_ecio;
            ppm_ptr->pilot_results[i].peak_ecio
                     = ppm_ptr->pilot_results[ j].peak_ecio;
            ppm_ptr->pilot_results[j].peak_ecio = tmp_byte;
          }
        }
      }
    }

    if( num_ppm != srch_ppm)
    {
      for( i = 0; i < num_ppm; i++)
      {
        if( ( word)( ( ppm_ptr->pilot_results[ i].peak_pos / 16 + 32) / 64)
            == ppm_ptr->ref_pn)
        {
          break;
        }
      }

      if( i == num_ppm)
      {
        for( i = num_ppm; i < srch_ppm; i++)
        {
          if( ( word)( ( ppm_ptr->pilot_results[i].peak_pos / 16 + 32) / 64)
              == ppm_ptr->ref_pn)
          {
            tmp_int4 = ppm_ptr->pilot_results[num_ppm - 1].peak_pos;
            ppm_ptr->pilot_results[num_ppm - 1].peak_pos
                     = ppm_ptr->pilot_results[i].peak_pos;
            ppm_ptr->pilot_results[i].peak_pos = tmp_int4;

            tmp_uint16 = ppm_ptr->pilot_results[num_ppm - 1].peak_pn;
            ppm_ptr->pilot_results[num_ppm - 1].peak_pn
                     = ppm_ptr->pilot_results[i].peak_pn;
            ppm_ptr->pilot_results[i].peak_pn = tmp_uint16;

            tmp_byte = ppm_ptr->pilot_results[num_ppm - 1].peak_rmse;
            ppm_ptr->pilot_results[num_ppm - 1].peak_rmse
                     = ppm_ptr->pilot_results[i].peak_rmse;
            ppm_ptr->pilot_results[i].peak_rmse = tmp_byte;

            tmp_byte = ppm_ptr->pilot_results[num_ppm - 1].peak_ecio;
            ppm_ptr->pilot_results[num_ppm - 1].peak_ecio
                     = ppm_ptr->pilot_results[i].peak_ecio;
            ppm_ptr->pilot_results[i].peak_ecio = tmp_byte;

            break;
          }
        }

        if( i == srch_ppm)
        {
          MSG_MED( "JGPS:Not found Reference PN (%d)", ppm_ptr->num_pilots,0,0);
        }
      }
    }
  }
  else
  {
    MSG_MED( "JPGS: Monitor mode not active (client_type=%d)", 
              gen_info_ptr->app_info.tm_core_start_req.client_type ,0,0);
  }

  return;
} /* tm_jgps_core_arrange_ppm_data */


/*===========================================================================

FUNCTION tm_jgps_core_arrange_prm_data

DESCRIPTION
 The GPS mode, the AFLT mode and TEST mode restrict the number of 
 Pilots and SVs to be used by the calculation of MS Assisted. 
 Also, TEST mode is to select the pilots and the SVs by ascending 
 or descending order. The PRM data is changed here depending 
 on PD mode setting.

RETURN VALUE
  None.

DEPENDENCIES

===========================================================================*/
void tm_jgps_core_arrange_prm_data
(
  sm_GpsMeasRptStructType *prm_ptr
)
{
  /* Local Variables */
  byte                  srch_prm;
  int                   i, j;
  byte                  sort_f = FALSE;
  gps_MeasStructType    tmp_meas_data;
  
  tm_jgps_gen_info_s_type   *gen_info_ptr = TM_JGPS_GEN_INFO_PTR();
  gnss_MeasStructType       *p_GpsMeas    = NULL;
	  
  /*----------------------------------------------------------------------
     Check the input parameters
  ----------------------------------------------------------------------*/
  if(prm_ptr == NULL)
  {
    MSG_ERROR("Received Null ppm_ptr",0,0,0);
    return;
  }

  p_GpsMeas = prm_ptr->z_MeasBlk.z_Gnss;

  /*----------------------------------------------------------------------
     Arrange the PPM data according to the mode, 
     *** only for monitor mode apps ***
  ----------------------------------------------------------------------*/
  if( tm_jgps_core_mode_is_not_normal()
      && gen_info_ptr->app_info.tm_core_start_req.client_type
                                       == PDSM_CLIENT_TYPE_TEST3 
    )
  {
    /* If the application indicates that it needs to use 1 PRM, reset
    */
    tm_jgps_core_reset_prm_satnum(prm_ptr);

    srch_prm = prm_ptr->z_MeasBlk.u_NumSvs;

    if( gen_info_ptr->app_info.tm_core_start_req.pd_option.mode_info.prm.num 
                                                   != TM_JGPS_PRM_PRN_ALL )
    {
      sort_f = TRUE;

      if( srch_prm 
            > gen_info_ptr->app_info.tm_core_start_req.pd_option.mode_info.prm.num)
      {
        prm_ptr->z_MeasBlk.u_NumSvs
          = gen_info_ptr->app_info.tm_core_start_req.pd_option.mode_info.prm.num;
      }

    }

    /* Sort the PRM data */
    if( sort_f == TRUE)
    {
      for( i = 0; i < srch_prm - 1; i++)
      {
        for( j = i + 1; j < srch_prm; j++)
        {
          if( gen_info_ptr->app_info.tm_core_start_req.pd_option.mode_info.prm.sort_type ^
              ( p_GpsMeas[i].w_Cno < p_GpsMeas[j].w_Cno))
          {
            /* Save a temp copy */
            memscpy((void *)&tmp_meas_data, sizeof(tmp_meas_data), (void *)&(p_GpsMeas[i]), sizeof(p_GpsMeas[i]) );

            /* Set the current array index */
            memscpy((void *)&(p_GpsMeas[j]), sizeof(p_GpsMeas[j]), (void *)&(p_GpsMeas[i]), sizeof(p_GpsMeas[i]) );

            /* Save the temp copy */
            memscpy((void *)&(p_GpsMeas[j]), sizeof(p_GpsMeas[j]), (void *)&tmp_meas_data, sizeof(tmp_meas_data) );
          }
        } /* end for j */
      } /* end for i */
    } /* end if sort_f */
  }

  return;
} /* tm_jgps_core_arrange_prm_data */


/*===========================================================================

FUNCTION tm_jgps_core_reset_prm_satnum

DESCRIPTION
  This function is used to reset PRM info if PRM num is set to 
  TM_JGPS_PRM_PRN_MIN.

RETURN VALUE
  None.

DEPENDENCIES

===========================================================================*/
void tm_jgps_core_reset_prm_satnum
(
  sm_GpsMeasRptStructType *prm_ptr
)
{
  /* Local Variables */
  tm_jgps_gen_info_s_type   *gen_info_ptr = TM_JGPS_GEN_INFO_PTR();
	  
  /*----------------------------------------------------------------------
     Check the input parameters
  ----------------------------------------------------------------------*/
  if(prm_ptr == NULL)
  {
    MSG_ERROR("Received Null prm_ptr",0,0,0);
    return;
  }

  /*----------------------------------------------------------------------
     Arrange the PPM data according to the mode, 
     *** only for monitor mode apps ***
  ----------------------------------------------------------------------*/
  if( TM_JGPS_PRM_PRN_MIN ==
            gen_info_ptr->app_info.tm_core_start_req.pd_option.mode_info.prm.num 
    )
  {
    prm_ptr->z_MeasBlk.u_NumSvs = 1;

    /* Clear the PRM meas data, at index 0 */
    memset((void *)&prm_ptr->z_MeasBlk.z_Gnss[0], 0x0, sizeof(prm_ptr->z_MeasBlk.z_Gnss[0]) );

    gen_info_ptr->app_info.tm_core_start_req.pd_option.mode_info.prm.num = 1;
  }
} /* tm_jgps_core_reset_prm_satnum */


/*=============================================================================
===============================================================================

         Extern Functions: 1x UP (IS-801 Drv) => TM-JGPS-Core

===============================================================================
=============================================================================*/


/*===========================================================================

FUNCTION   tm_jgps_core_start_sess_req_handler

DESCRIPTION
  This function is used to save JGPSONE specific info for use.

RETURN VALUE
  TRUE - JGPS core grants the session start request
  FALSE - otherwise
============================================================================*/
boolean tm_jgps_core_start_sess_req_handler(void)
{
  /* Local Variables */
  boolean ret_status = FALSE;
  
  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();

  /*-----------------------------------------------------------------------
    Service the start session request
  ------------------------------------------------------------------------*/
  do
  {
    MSG_HIGH("Start Session Request from IS-801 Drv (mode=%d)", 
             sess_info_ptr->req_mode,0,0);


    /*-----------------------------------------------------------------------
      Check the session state
    ------------------------------------------------------------------------*/
    if(sess_info_ptr->state != TM_JGPS_SESS_STATE_START_REQ)
    {
      MSG_ERROR("Wrong State (%d), Start sess req from IS-801 Drv denied", 
               sess_info_ptr->state,0,0);
      break;
    }

    /*-----------------------------------------------------------------------
      Based the request mode, TM-JGPS will send POS request or ACQ request.
    ------------------------------------------------------------------------*/
    if (   sess_info_ptr->req_mode == TM_JGPS_SESS_REQ_MODE_MSA
        || sess_info_ptr->req_mode == TM_JGPS_SESS_REQ_MODE_MSB_DLOAD
       )
    {

      /* Kick off the process of sending a POS req or ACQ req */
      ret_status = tm_jgps_core_start_pkt_data_req();
    } 
    else
    {
      MSG_ERROR("Request mode (%d), is not handled", 
                sess_info_ptr->req_mode,0,0);
      break;
    }
   
    /*-----------------------------------------------------------------------
      Determine if request is sent (start recover timers)
    ------------------------------------------------------------------------*/    
    if ( ret_status == FALSE )
    {
      MSG_ERROR("JGPS: Start request failed to send data to SCU, abort",0,0,0);

      /* Abort and send notifiaction to IS-801 driver and TM-Core */
      tm_jgps_core_abort_recover(TM_STOP_REASON_GENERAL_ERROR,
                                 PRTL_SESS_STOP_SINGLE_FIX);
      ret_status = FALSE;
      break;
    }
    else
    {
      /* Update the current session state */
      if( !tm_jgps_core_update_sess_state(TM_JGPS_SESS_STATE_WAIT_SCU_START_REQ_RESP) )
      {
        /* Abort and send notifiaction to IS-801 driver and TM-Core */
        tm_jgps_core_abort_recover(TM_STOP_REASON_GENERAL_ERROR,
                                 PRTL_SESS_STOP_SINGLE_FIX);
        ret_status = FALSE;
        break;
      }
    } /* end status */

  } while ( 0 );

  return ret_status;
  
} /* tm_jgps_core_start_sess_req_handler */


/*===========================================================================

FUNCTION   tm_jgps_core_stop_sess_req_handler

DESCRIPTION
  This function handles the request from IS-801 driver to stop the session

RETURN VALUE
  TRUE - JGPS core grants the session stop request
  FALSE - otherwise
============================================================================*/
boolean tm_jgps_core_stop_sess_req_handler
(
  pdsm_pd_end_e_type            status,
  pdsm_pd_dload_data_s_type     *dload_ptr
)
{
  /* Local Variables */
  void  *scu_data = NULL;      /* No Cmd specific data for SCU */
  boolean ret_status = FALSE;
  tm_sess_stop_reason_e_type stop_reason = TM_STOP_REASON_NONE;

  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();

  /*-----------------------------------------------------------------------
    Service the stop session request
  ------------------------------------------------------------------------*/
  do
  {
    MSG_HIGH("Stop Session Request from IS-801 Drv mode=%d status=%d", 
             sess_info_ptr->req_mode,status,0);


    /*-----------------------------------------------------------------------
      Check the session state
    ------------------------------------------------------------------------*/
    if( sess_info_ptr->state == TM_JGPS_SESS_STATE_ABORT_RECOVER )
    {
      MSG_ERROR("Wrong State (%d), Stop sess req from IS-801 Drv denied", 
               sess_info_ptr->state,0,0);
      break;
    }

    /*-----------------------------------------------------------------------
      Based on status either send ACQ complete or CANCEL msg to SCU
    ------------------------------------------------------------------------*/
    switch(status)
    {
      /*-----------------------------------------------------------------------
        ACQ completion message must be sent to SCU for DLOAD end
      ------------------------------------------------------------------------*/
      case PDSM_PD_END_SESS_NONE:
        if(sess_info_ptr->req_mode == TM_JGPS_SESS_REQ_MODE_MSB_DLOAD)
        {
          ret_status = tm_jgps_core_send_msg(TM_JGPS_CMD_ID_ACQ_COMPLETE, scu_data);
        }
        else
        {
          ret_status = TRUE;
        }
        break;
        
      /*-----------------------------------------------------------------------
        Cancellation message to be sent to SCU for error scenarios
      ------------------------------------------------------------------------*/
      case PDSM_PD_END_PDE_REJECT:
        ret_status = tm_jgps_core_send_msg_cancel_rej(status, dload_ptr); 
        break;
        /* If there are connectivity issues, do not send Cancel to SCU */
      case PDSM_PD_END_CON_FAIL:
        ret_status = FALSE;
        break;
      case PDSM_PD_END_TIMEOUT:
        ret_status = tm_jgps_core_send_msg_cancel_timeout();
        break;
      default:
        ret_status = tm_jgps_core_send_msg_cancel_others(status);
        break;
    }

    /* Translate end_status to stop_reason */
    stop_reason = tm_jgps_core_translate_end_status(status);

    /*-----------------------------------------------------------------------
      Determine if request is sent (start recover timers)
    ------------------------------------------------------------------------*/    
    if ( ret_status == FALSE )
    {
      MSG_ERROR("JGPS: Stop request failed to send data to SCU, abort",0,0,0);
      
      /* Abort and send notifiaction to IS-801 driver and TM-Core */
      tm_jgps_core_abort_recover(stop_reason,
                                 PRTL_SESS_STOP_SINGLE_FIX);
      ret_status = FALSE;
      break;
    }
    else
    {

      /*------------------------------------------------------------------------
         Set the session state to wait response only for MS-A
      -------------------------------------------------------------------------*/
      if( sess_info_ptr->req_mode == TM_JGPS_SESS_REQ_MODE_MSA)
      {
        ret_status 
          = tm_jgps_core_update_sess_state(TM_JGPS_SESS_STATE_WAIT_SCU_STOP_REQ_RESP);
        MSG_MED("JGPS MSA: Sent request, wait'g for response from SCU",0,0,0);
      }
      else
      {
        ret_status = tm_jgps_core_update_sess_state(TM_JGPS_SESS_STATE_STOP_REQ);
        MSG_MED("JGPS MSB_DLOAD: Sent request",0,0,0);
        if(status != PDSM_PD_END_SESS_NONE)
        {
          /* Abort and send notifiaction to IS-801 driver and TM-Core */
          tm_jgps_core_abort_recover( stop_reason,
                                      PRTL_SESS_STOP_SINGLE_FIX);
        }
      }

      
      /* Abort the session if error
      */
      if( ret_status == FALSE )
      {
        /* Abort and send notifiaction to IS-801 driver and TM-Core */
        tm_jgps_core_abort_recover( stop_reason,
                                    PRTL_SESS_STOP_SINGLE_FIX);
      }
    } /* end status */
  } while ( 0 );

  return ret_status;
  
} /* tm_jgps_core_stop_sess_req_handler */


/*===========================================================================

FUNCTION   tm_jgps_core_info_available_handler

DESCRIPTION
  This function is used to format the PD data to clients or save the data
  until SCU returns a completion reponse packet at the end of MS-A.

RETURN VALUE
  TRUE - JGPS core services the info available request
  FALSE - otherwise
============================================================================*/
boolean tm_jgps_core_info_available_handler
(
  pdsm_pd_info_s_type   *pd_info_ptr
)
{
  /* Local Variables */
  boolean ret_status = FALSE;

  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();
  
  do{
    /*-----------------------------------------------------------------------
      Check input parameters
    ------------------------------------------------------------------------*/
    if(pd_info_ptr == NULL)
    {
      MSG_ERROR("JGPS: Invalid input param, info avail handler",0,0,0);
      break;
    }

    /*-----------------------------------------------------------------------
       For MS-A fix, save the info and wait for response from SCU
    ------------------------------------------------------------------------*/
    /* MS-A Session is active */
    if( FALSE == tm_jgps_core_save_lr_data(pd_info_ptr) )
    {
      MSG_ERROR("JGPS: Saving PD data failed",0,0,0);
      break;
    }

    /*-----------------------------------------------------------------------
       Format the PD data for JGPS clients
    ------------------------------------------------------------------------*/
    tm_jgps_core_convert_lr_data(pd_info_ptr);

    /* Set the mask and data pointer to the JGPS format LR data */
    pd_info_ptr->pd_info.pd_data.opt_field_mask |= PDSM_PM_DATA_AVAIL;
    pd_info_ptr->pd_info.pd_data.pm_data 
                 = &sess_info_ptr->scu_lr_resp;

    ret_status = TRUE;
    
  }while( 0 );

  return ret_status;  
}

/*===========================================================================

FUNCTION: tm_jgps_core_data_pipe_req_handler

DESCRIPTION:
  This function is called by TM-JGPS module to append KDDI specfic header
  to outgoing IS-801 data.

   
PARAMETERS:

RETURN VALUES:
  0 - Failure
  NUM - number of bytes sent
===========================================================================*/
int tm_jgps_core_data_pipe_req_handler
(
  boolean   close_flag,
  byte      *data_ptr,
  int16     data_ptr_len
)
{
  /* Local Variables */
  int ret_val =  0;
  
  tm_jgps_gen_info_s_type *gen_info_ptr = TM_JGPS_GEN_INFO_PTR();

  /*---------------------------------------------------------------------
     If IS-801 driver request to close connection, and JGPS module 
     is waiting for SCU POS response, delay the close connection
  ---------------------------------------------------------------------*/
  if( close_flag
      && gen_info_ptr->sess_info.state ==  TM_JGPS_SESS_STATE_WAIT_SCU_STOP_REQ_RESP )
  {
    /* Set the close flag, to close TCP connection when JGPS recv POS resp */
    gen_info_ptr->comm_info.close_flag = close_flag;

    /* Tell IS-801 drv do not close connection, JPGS module is waiting for resp */
    ret_val =  TM_JGPS_DATA_PIPE_REQ_FAIL; /* TM_1X_UP_SMOD_DATA_PIPE_REQ_FAIL */
  }
  else if( close_flag 
           && data_ptr_len == 0 )
  {
    /* Tell IS-801 drv to close the connection immediately */
    ret_val =  TM_JGPS_DATA_PIPE_REQ_GRANT; /*TM_1X_UP_SMOD_DATA_PIPE_REQ_CLOSE_GRANT */
  }
  else
  {
    /*---------------------------------------------------------------------
      Send IS-801 Packet data.
    ---------------------------------------------------------------------*/
    ret_val =  tm_jgps_core_add_pkt_hdr_and_send( TM_JGPS_CMD_ID_IS801_RLINK,
                                                  data_ptr, 
                                                  data_ptr_len );

  }

  return ret_val;
} /* tm_jgps_core_data_pipe_req_handler */


/*=============================================================================
===============================================================================

              Extern Functions: TM-Core => TM-JGPS-Core
              
===============================================================================
=============================================================================*/

/*===========================================================================
FUNCTION: tm_jgps_init

DESCRIPTION:
  This function is called at boot up time to initialize TM JGPS module.

 
PARAMETERS:

===========================================================================*/
void tm_jgps_core_init(void)
{
  /* Initialize General JGPS session info */
  tm_jgps_core_sess_init();
  
  /* Initialize app info */
  tm_jgps_core_app_info_init(); 

  /* Initialize the comm info */
  tm_jgps_core_comm_info_init();
    
  /* Read NV items */
  tm_jgps_core_read_nv();

} /* tm_jgps_core_init */


/*===========================================================================

FUNCTION   tm_jgps_core_prtl_start_sess_req_handler

DESCRIPTION
  This function is used to save JGPSONE specific info for use.

RETURN VALUE
  None

============================================================================*/
prtl_start_sess_req_result_e_type tm_jgps_core_prtl_start_sess_req_handler
(
  tm_sess_handle_type                  sess_handle,
  prtl_start_sess_req_param_s_type     *start_sess_req_param,
  tm_sess_req_start_param_s_type       *actions
)
{
  /* Local Variables */
  boolean                              is801_ret_val = FALSE;
  tm_1x_up_is801_drv_req_param_u_type  req_param;
  prtl_start_sess_req_result_e_type    ret_val = PRTL_START_SESS_REQ_NOT_ALLOWED;
  
  tm_jgps_gen_info_s_type   *gen_info_ptr = TM_JGPS_GEN_INFO_PTR();

  /*------------------------------------------------------------------------
    Set the PPM and PRM mode info, support monitor mode 
  -------------------------------------------------------------------------*/
  tm_jgps_core_set_mode_info( &start_sess_req_param->pd_option,
                              start_sess_req_param->client_type );

  /*------------------------------------------------------------------------
     TM-Core Sent session request, update TM-JGPS-Core state
  -------------------------------------------------------------------------*/
  if ( gen_info_ptr->sess_info.state == TM_JGPS_SESS_STATE_WAIT_TCP_DISCONNECT
       || gen_info_ptr->sess_info.state == TM_JGPS_SESS_STATE_ABORT_RECOVER )
  {
    MSG_HIGH("JGPS: Wait for TCP close, before allowing next session (state=%d, comm_state=%d)",
             gen_info_ptr->sess_info.state,
             gen_info_ptr->comm_info.comm_state,0);

    /* Restart the session after TCP close success */
    gen_info_ptr->sess_info.restart = TM_JGPS_RESTART_TM_CORE;
    
    /* Tell TM-Core to wait for response */
    ret_val = PRTL_START_SESS_REQ_WAIT;    
  }
  else if( tm_jgps_core_update_sess_state(TM_JGPS_SESS_STATE_START_REQ) )
  {
    /*-----------------------------------------------------------------------
      Set wake_tm_core to false, so that is801 module can instruct 
      TM-Core what to do immediately.
    -----------------------------------------------------------------------*/
    req_param.start_param.wake_tm_core         = FALSE;  
    req_param.start_param.actions              = actions;



    req_param.start_param.start_sess_req_param = start_sess_req_param;


    is801_ret_val = tm_1x_up_is801_drv_req(
                                            sess_handle, 
                                            TM_IS801_DRV_REQ_START,
                                            req_param
                                          );
    
    if(is801_ret_val == TRUE) /*lint !e506 !e731 */
    {
      ret_val = PRTL_START_SESS_REQ_ALLOWED;
    }
    else
    {    
      ret_val = PRTL_START_SESS_REQ_NOT_ALLOWED;
    }
  }

  /*-----------------------------------------------------------------------
     Re-initialize the JGPS module if the request is denied
  -----------------------------------------------------------------------*/
  if(ret_val == PRTL_START_SESS_REQ_NOT_ALLOWED)
  {
    /* Re-init the JGPS layer if there is a failure */
    tm_jgps_core_reinit();
  }
  else
  {
    /*------------------------------------------------------------------------
       Store the Session start request information from TM-Core
    -------------------------------------------------------------------------*/
    gen_info_ptr->sess_info.tm_core_sess_handle = sess_handle;

    gen_info_ptr->app_info.app_type = start_sess_req_param->client_type;

    /* Save the incoming request parameters */
    if( start_sess_req_param != NULL )
    {
      memscpy((void *)&gen_info_ptr->app_info.tm_core_start_req,
             sizeof(gen_info_ptr->app_info.tm_core_start_req),
             start_sess_req_param,
             sizeof(*start_sess_req_param) );
    } 
  }

  return ret_val;
  
} /* tm_jgps_core_prtl_start_sess_req_handler */


/*===========================================================================

FUNCTION   tm_jgps_core_prtl_stop_sess_req_handler

DESCRIPTION
  This function is used to handle user terminated stop session from
  TM-Core.

RETURN VALUE
  None

============================================================================*/
boolean tm_jgps_core_prtl_stop_sess_req_handler
(
  tm_sess_stop_reason_e_type    reason,
  prtl_sess_stop_type           stop_type
)
{
  /* Local Variables */
  boolean ret_val = FALSE;
  
  /*------------------------------------------------------------------------
     User terminated stop session
  -------------------------------------------------------------------------*/
  if(reason == TM_STOP_REASON_USER_TERMINATED)
  {
    ret_val = tm_jgps_core_send_msg_cancel(TM_JGPS_CANCEL_CAUSE_USR_DISCONNECT);
  }
  
  /* Stop the session based on the abort reason*/
  tm_jgps_core_abort_recover(reason, PRTL_SESS_STOP_SINGLE_FIX);
  
  return ret_val;
} /* tm_jgps_core_prtl_stop_sess_req_handler */


/*===========================================================================

FUNCTION   tm_jgps_core_prtl_save_app_info

DESCRIPTION
  This function is used to save JGPSONE specific info for use.

RETURN VALUE
  None

============================================================================*/
boolean tm_jgps_core_prtl_save_app_info
(  
  pdsm_pa_info_type  *pa_info_ptr
)
{
  /* Local Variables */
  boolean ret_val = TRUE;

  tm_jgps_sess_info_s_type   *sess_info_ptr = TM_JGPS_GEN_INFO_SESS_PTR();

  /*------------------------------------------------------------------------
     Save Application Info from Client
  ------------------------------------------------------------------------*/
  /* Fill in the application info to indicate acry, datum, num, unit.
  */
  sess_info_ptr->url_info.ver = pa_info_ptr->app_info.ver;
  sess_info_ptr->url_info.acry = pa_info_ptr->app_info.acry;
  sess_info_ptr->url_info.datum = pa_info_ptr->app_info.datum;
  sess_info_ptr->url_info.num  = pa_info_ptr->app_info.num;
  sess_info_ptr->url_info.unit = pa_info_ptr->app_info.unit;
  
  /* Save URL data.
  */
  if ( GNSS_STRLEN( sess_info_ptr->url_info.url ) <= TM_JGPS_REQ_ULENGTH_MAX )
  {
    GNSS_STRLCPY( sess_info_ptr->url_info.url, 
                 pa_info_ptr->app_info.url, 
                 sizeof(sess_info_ptr->url_info.url)  );

    sess_info_ptr->url_info.app_url_enable = TRUE;
  }
  else
  {
    ret_val = FALSE;
  }

  return ret_val;
} /* tm_jgps_core_save_app_info */

/*===========================================================================

FUNCTION   tm_jgps_core_prtl_sess_info_handler

DESCRIPTION
  This function handles the session information data returned by TM-Core

RETURN VALUE
  None

============================================================================*/
boolean tm_jgps_core_prtl_sess_info_handler
(
  prtl_sess_info_e_type         sess_info_type,
  prtl_sess_info_param_u_type   *sess_info_param
)
{
  /* Local Variables */
  boolean ret_val = FALSE;


  /*------------------------------------------------------------------------
     Check Input Parameters
  ------------------------------------------------------------------------*/
  if (sess_info_param == NULL)
  {
    MSG_ERROR("JPGS: Input param null ptr",0,0,0);
    return FALSE;
  }

  /*------------------------------------------------------------------------
     Handle the session info data
  ------------------------------------------------------------------------*/
  switch (sess_info_type)
  {
    /* RF info reponse handler */
    case PRTL_SESS_INFO_RF_INFO:
      ret_val = tm_jgps_core_trans_rf_info(&sess_info_param->rf_info_data);
      break;
      
    /* PRM data handler for monitor mode */
    case PRTL_SESS_INFO_PRM:
      if ( sess_info_param->prm_data.e_MeasRptReason == GPS_MEAS_RPT_MEAS_DONE )
      {
        tm_jgps_core_arrange_prm_data(&sess_info_param->prm_data);
        /* Send data to IS-801 drv */
        ret_val = FALSE;
      }
      break;
      
    /* PPM data handler for monitor mode */
    case PRTL_SESS_INFO_PPM:
      tm_jgps_core_arrange_ppm_data(&sess_info_param->ppm_data);
      /* Send data to IS-801 drv */
      ret_val = FALSE;
      break;
    
    default:
      MSG_HIGH("JGPS: Pass unhandled sess info to IS-801 Drv (data=%d)",sess_info_type,0,0);
      break;
  }       
  
  return ret_val;
} /* tm_jgps_core_prtl_sess_info_handler */


/*===========================================================================

FUNCTION   tm_jgps_core_prtl_data_pipe_handler

DESCRIPTION
  This function is used to save JGPSONE specific info for use.

RETURN VALUE
  None

============================================================================*/
boolean tm_jgps_core_prtl_data_pipe_handler
(
  pd_comms_client_ipc_message_type   msg_type,  
  void                               *msg_data_ptr
)
{
  /* Local Variables */
  boolean ret_val = FALSE;
  
  /*------------------------------------------------------------------------
     Check Input Parameters
  ------------------------------------------------------------------------*/
  if(msg_data_ptr == NULL)
  {
    MSG_ERROR("Null msg_data_ptr", 0, 0, 0);
    return FALSE;
  }

  /*------------------------------------------------------------------------
     Handle the Comm event
  ------------------------------------------------------------------------*/
  switch(msg_type)
  {
  case C_PD_COMMS_CLIENT_MSG_NOTIFY:
      ret_val = tm_jgps_core_comm_notify_handler(msg_type, msg_data_ptr);
      break;

  case C_PD_COMMS_CLIENT_MSG_WRITE_ACK:
      ret_val = tm_jgps_core_comm_write_ack_handler(msg_type, msg_data_ptr);
      break;

  case C_PD_COMMS_CLIENT_MSG_IOCTL_ACK:
      ret_val = tm_jgps_core_comm_ioctl_ack_handler(msg_type, msg_data_ptr);
      break;

  case C_PD_COMMS_CLIENT_MSG_READ_DATA:
      ret_val = tm_jgps_core_comm_read_data_handler(msg_data_ptr);
      break;

  default:
      MSG_ERROR("Received unknown message", 0, 0, 0);
      ret_val = FALSE;
      break;
  }

  return ret_val;
  
} /* tm_jgps_core_prtl_data_pipe_handler */

/*===========================================================================

FUNCTION   tm_jgps_core_prtl_timer_handler

DESCRIPTION
  This function handles the timer call back.

RETURN VALUE
  None

============================================================================*/
void tm_jgps_core_prtl_timer_handler(const os_TimerExpiryType *timer_param)
{
  /*------------------------------------------------------------------------
     Service the timer expiration
  ------------------------------------------------------------------------*/
  if(timer_param != NULL)
  {
    if((timer_param->q_TimerId & TM_1X_UP_TIMER_ID_DISPATCH_MASK) 
                                           == TM_1X_UP_KDDI_TIMER_ID_BASE)
    {
      MSG_ERROR("JGPS: Time expiration: Timer ID: 0x%h",
                timer_param->q_TimerId, 0,0);
      
      switch(timer_param->q_TimerId)
      {
        case TM_JGPS_TIMER_ID_T02:
          tm_jgps_core_t02_timer_handler();
          break;
        
        case TM_JGPS_TIMER_ID_T04:
          tm_jgps_core_t04_timer_handler();
          break;
          
        case TM_JGPS_TIMER_ID_RF_INFO_REQ:
        case TM_JGPS_TIMER_ID_MS_IP_REQ:
          tm_jgps_core_info_req_timer_handler();
          break;     
           
        default:
          MSG_ERROR("JGPS: Unknown timer expired", 0, 0, 0);
          break;
      }
    }
    else
    {
      MSG_ERROR("JGPS: Can not proess invalid timer event", 0, 0, 0);
    }
  }
  else
  {
    MSG_ERROR("JGPS: Can not proess null timer msg", 0, 0, 0);  
  }
} /* tm_jgps_core_prtl_timer_handler */


#endif /* FEATURE_JCDMA_GPSONE */

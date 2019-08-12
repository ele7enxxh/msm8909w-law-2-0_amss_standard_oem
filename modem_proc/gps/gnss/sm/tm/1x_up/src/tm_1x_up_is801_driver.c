
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM 1X UP Interface

General Description
  This file contains implementations for TM 1X UP Interface
  
  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2013 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/src/tm_1x_up_is801_driver.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/13/15   rh      Fixed SRLTE's V2 issue 
03/14/15   rh      Added support for MultiSIM NV 
11/07/14   sj      Fix Memory Leak when Is801 aborts.
05/13/14   mj      Add featurization to fix SMART compile/link issues
04/17/14   skm    NV Async Read changes 
10/09/12   rh      Added a few error code mappings for IS801 session 
06/16/11   rh      Added TM_PHONE_EVENT_OTASP_STATUS_COMMITTED event processing
01/03/11   gk      Eph throttling rework
08/23/10   gk      Added new handling for optimal modes
12/2/09    gk      intermediate position fixes
05/27/09   ank     In JCDMA call flow, allow fallback from MS-A to MS-B and vice-a-versa for optimal modes.
12/10/08   rw      If TCP connection is already disconnected return status to 
                   protocol sub-module.
11/21/08   ank     For Speed-opt fix, if Mobile is in service, has seed position but No EPH data and Punc is < 12km,
                   then proceed with Dload session.
11/07/08   ank     For QoS=0, do not turn MGP to ON for MS-B fix.
07/11/08    cl     Abort ongoing session when switching to WCDMA system
04/28/08   ank     Handle PRM while waiting for SA.
11/08/07   ank     Added support for REF_POSITION mode.
08/14/07   ank     Inform MGP about nonexistent SVs.
                   Removed code under FEATURE_GPSONE_PRE_ARIES.
07/31/07   ank     Added Dynamic QoS and Override logic.
07/30/07    ank    Added support for ext_pos_data in Position Report.
06/15/07    ank    For MSB-Dload set lr_qos to 255 sec. Once Dload is complete, continue fix with QoS from API.
06/15/07    ank    Delay sending start measurement req to LM till AFLT during MSB-Dload is finished.
06/15/07    ank    Update state to WAIT_PR when SA data is received.
05/27/07    rw     Add support for JGPS (KDDI) module
05/09/07    ank    Changed prototype of tm_util_pd_info_data_init
12/27/06    ank    Added support for IS801 Rx and Tx events.
12/22/06    ank    Added support for MSBased Throttling feature.
12/20/06    ank    Changes to send PRM and post-fix PPM together.
12/13/06    jw     Add temporary code to set the time_ref of the Post Fix PPMs.
12/01/06    jw     Made changes to support modified struct sm_GpsMeasRptStructType
11/15/06    ank    Fixed lint errors.
11/02/06    ank    Initial Release
===========================================================================*/
#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"
#include "math.h"
#include "msg.h"
#include "event_defs.h"
#include "event.h"
#include "time_svc.h"
#include "gps_consts.h"
#include "aries_os_api.h"
#include "mccdma.h"
#ifdef FEATURE_CMI
#include "mccdma_v.h"
#endif
#include "sm_api.h"
#include "sm_dm_event.h"
#include "sm_log.h"
#include "pdapibuf.h"
#include "paixlate.h"
#include "tm_paii.h"
#include "tm_is801.h"
#include "tm_is801b.h"
#include "tm_is801_util.h"
#include "tm_is801b_util.h"
#include "tm_common.h"
#include "tm_cm_iface.h"
#include "tm_1x_up.h"
#include "tm_1x_up_is801_drv_iface.h"
#include "tm_1x_up_is801_driver_i.h"
#include "tm_1x_up_comm.h"
#include "tm_1x_up_smod_iface.h"
#include "time_jul.h"

#ifdef FEATURE_JCDMA_GPSONE
#include "../jgps/src/tm_jgps_core.h"
#endif /* FEATURE_JCDMA_GPSONE */
#include "qw.h"

#include "tm_data.h"
#include "tm_umts_up_supl.h"

/* quotient = the result from the division */
/* dividend = the number to be divided     */
/* num_bits = number of bits to shift      */
/*lint -e{10} error due to qc_qword */
/*lint -e{101} error due to qc_qword */
/*lint -e{132} error due to qc_qword */
word qw_div_by_power_of_2(qc_qword quotient, qc_qword dividend, 
  unsigned short num_bits);

/* shifticand = qword to shift  */
/* shiftidend = bits to shift, positive = shift left, negative = shift right */
/*lint -e{10} error due to qc_qword */
/*lint -e{101} error due to qc_qword */
/*lint -e{132} error due to qc_qword */
extern void qw_shift(qc_qword shifticand, int shiftidend);
                        
tm_sess_req_param_u_type           tm_1x_up_sess_req_param;
tm_1x_up_session_info_s_type       tm_1x_up_session_info;  /* PD session information */
pdsm_pd_info_s_type                tm_1x_up_info_buffer; 
static pdsm_pd_dload_data_s_type   tm_1x_up_dload_info;
tm_pd_position_info_s_type         tm_1x_up_pos_info;
tm_ext_pos_data_s_type             tm_1x_up_ext_pos_data;
int1 sess_tag = -1;

extern tm_1x_up_pdcomm_info_s_type   pdcomm_info;        /* Transport layer information */

sm_GpsMeasRptStructType            *up_prm_buff_ptr = NULL;
gps_PreFixPpmSructType             *up_prefix_ppm_buff_ptr = NULL;
is801_lr_resp_type                 *ref_position_ptr = NULL;


LOCAL boolean tm_1x_up_is801_drv_msa_fix_allowed(void);
#ifdef FEATURE_JCDMA_GPSONE
extern boolean tm_jgps_core_update_sess_state(tm_jgps_sess_state_e_type new_state);
#endif /* FEATURE_JCDMA_GPSONE */

#ifndef FEATURE_CGPS_UMTS_UP_SUPL
void tm_umts_up_supl_is801_session_stop (const tm_sess_stop_reason_e_type	stop_reason)
{
  return;
}  
void tm_umts_up_supl_is801_pos_reported (void)
{
  return;
}
void tm_umts_up_supl_is801_inform_1x_pos_method (supl_session_agps_mode_enum_type mode)
{
  return;
}    
#endif /* ifndef FEATURE_CGPS_USES_UMTS: dummy function for builds without UMTS-SUPL */


/* (Sv line of sight vector calculation works for up to 75 km. The last
   but one entry in the table below is specified in IS-801 as unc. >
   12,288m. Our algorithm works for up to 75Km, so set that entry to
   70000.0 m.) */
const FLT f_PosUncArray[MAX_POS_UNC_IND] =
{0.5,0.75,1.0,1.5,2.0,3.0,4.0,6.0,8.0,12.0,16.0,24.0,32.0,48.0,64.0,96.0,
  128.0,192.0,256.0,384.0,512.0,768.0,1024.0,1536.0,2048.0,3072.0,4096.0,
  6144.0,8192.0,12288.0,70000.0,999999.0};

#if 0
/* This function not used anywhere */
/*===========================================================================

FUNCTION tm_1x_up_is801_drv_start_fallback_msb_fix

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_1x_up_is801_drv_start_fallback_msb_fix(void)
{

  tm_1x_up_sess_req_param.continue_param.op_req  = TM_OP_REQ_AIDING_DATA_STATUS;
  tm_1x_up_sess_req_param.continue_param.op_mode  = TM_SESS_OPERATION_MODE_MSB;
  tm_1x_up_sess_req_param.continue_param.dynamic_qos_enabled = TRUE;
  
  if(tm_sess_req(TM_PRTL_TYPE_1X_UP,
             tm_1x_up_session_info.sess_handle,
             TM_SESS_REQ_CONTINUE,
             &tm_1x_up_sess_req_param))
  {
    MSG_MED("tm_1x_up_is801_drv_start_fallback_msb_fix Succeeded", 0, 0, 0);  
    return TRUE;
  }
  else
  {
    MSG_ERROR("tm_1x_up_is801_drv_start_fallback_msb_fix Failed", 0, 0, 0);
    return FALSE;
  }                  
}
#endif

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_is_optimal_mode

DESCRIPTION
  This function is used to indicate if mode is optimal or not.

RETURN VALUE
  TRUE if Optimal, else FALSE.

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_is801_drv_is_optimal_mode(void)
{
  boolean ret_val = FALSE;

  /* operation mode requested by the pdapi client */
  pdsm_pd_session_operation_e_type op_mode = 
          tm_1x_up_session_info.api_param.pd_api_info.pd_option.operation_mode;

  if ((op_mode > PDSM_SESSION_OPERATION_MSASSISTED) &&
       (op_mode < PDSM_SESSION_OPERATION_REF_POSITION))
  {
    ret_val = TRUE;
  }
  return ret_val;
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_continue_msb_fix

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_1x_up_is801_drv_continue_msb_fix(tm_1x_up_msb_op_e_type op_type)
{
  tm_1x_up_sess_req_param.continue_param.op_req  = TM_OP_REQ_LR;
  tm_1x_up_sess_req_param.continue_param.op_mode  = TM_SESS_OPERATION_MODE_MSB;
  tm_1x_up_sess_req_param.continue_param.dynamic_qos_enabled = TRUE;
  
  /* For Dload pass 255 sec. Once Dload is complete, pass actual timeout from API QoS */
  if(op_type == DLOAD_START)
  {
    tm_1x_up_sess_req_param.continue_param.lr_qos  = TM_MAX_SESSION_TIMEOUT;
  }
  else
  {
    tm_1x_up_sess_req_param.continue_param.lr_qos  = 
          tm_1x_up_session_info.api_param.pd_api_info.pd_qos.gps_session_timeout;
  }
  tm_1x_up_sess_req_param.continue_param.accuracy_threshold = 
          tm_1x_up_session_info.api_param.pd_api_info.pd_qos.accuracy_threshold;

  if(tm_sess_req(TM_PRTL_TYPE_1X_UP,
             tm_1x_up_session_info.sess_handle,
             TM_SESS_REQ_CONTINUE,
             &tm_1x_up_sess_req_param))
  {
    MSG_MED("tm_1x_up_is801_drv_continue_msb_fix Succeeded", 0, 0, 0);
    tm_1x_up_session_info.mode_info.waiting_msb_lr   = TRUE;
   /* Start session recovery timer */
   (void)os_TimerStart(tm_1x_up_session_info.tm_1x_up_sess_timer, 
                       (tm_1x_up_sess_req_param.continue_param.lr_qos+2) * 1000,
                       NULL);

    return TRUE;
  }
  else
  {
    MSG_ERROR("tm_1x_up_is801_drv_continue_msb_fix Failed", 0, 0, 0);
    return FALSE;
  }                  
}
/*===========================================================================

FUNCTION tm_1x_up_is801_drv_start_ppm

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_1x_up_is801_drv_start_ppm()
{
  tm_1x_up_sess_req_param.continue_param.op_req  = TM_OP_REQ_PPM;
  if(tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_MSA)
  {
    tm_1x_up_sess_req_param.continue_param.op_mode = TM_SESS_OPERATION_MODE_MSA;
  }
  else
  {
    tm_1x_up_sess_req_param.continue_param.op_mode = TM_SESS_OPERATION_MODE_MSB;
  }
  tm_1x_up_sess_req_param.continue_param.ppm_qos = TM_MGP_PPM_TIMEOUT_MAX/1000;
  tm_1x_up_sess_req_param.continue_param.num_fixes = tm_1x_up_session_info.api_param.pd_api_info.pd_option.fix_rate.num_fixes;
  tm_1x_up_sess_req_param.continue_param.tbf_ms = tm_1x_up_session_info.api_param.pd_api_info.pd_option.fix_rate.time_between_fixes_ms;

  if(tm_sess_req(TM_PRTL_TYPE_1X_UP,
             tm_1x_up_session_info.sess_handle,
             TM_SESS_REQ_CONTINUE,
             &tm_1x_up_sess_req_param))
  {
    MSG_MED("tm_1x_up_is801_drv_start_ppm Succeeded", 0, 0, 0);  
    tm_1x_up_session_info.mgp_info.ppm_on = TRUE;
    return TRUE;
  }
  else
  {
    MSG_ERROR("tm_1x_up_is801_drv_start_ppm Failed", 0, 0, 0);
    return FALSE;
  }                  
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_get_ppm

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_1x_up_is801_drv_get_ppm(void)
{
   tm_1x_up_sess_req_param.polling_data_param.op_req = TM_OP_REQ_PPM;
                          
   if(tm_sess_req(TM_PRTL_TYPE_1X_UP,
              tm_1x_up_session_info.sess_handle,
              TM_SESS_REQ_POLLING_DATA,
              &tm_1x_up_sess_req_param))
   {
     MSG_MED("tm_1x_up_is801_drv_get_ppm Succeeded", 0, 0, 0);  
     if(!os_TimerStart(tm_1x_up_session_info.tm_1x_up_mgp_timer,TM_MGP_GET_PPM_TIMEOUT_MAX,NULL))
     {
       MSG_ERROR("Failed starting MGP PPM timer",0,0,0);
     }
     return TRUE;
   }
   else
   {
     MSG_ERROR("tm_1x_up_is801_drv_get_ppm Failed", 0, 0, 0);
     return FALSE;
   }                  
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_end_ppm

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_1x_up_is801_drv_end_ppm()
{
    tm_1x_up_sess_req_param.stop_param.stop_type    = TM_STOP_TYPE_OPERATION;
    tm_1x_up_sess_req_param.stop_param.stop_reason  = TM_STOP_REASON_COMPLETED;
    tm_1x_up_sess_req_param.stop_param.op_to_stop   = TM_OP_REQ_PPM;

    if(tm_sess_req(TM_PRTL_TYPE_1X_UP,
               tm_1x_up_session_info.sess_handle,
               TM_SESS_REQ_STOP,
               &tm_1x_up_sess_req_param))
    {
      MSG_MED("tm_1x_up_is801_drv_end_ppm Succeeded", 0, 0, 0);  
      tm_1x_up_session_info.mgp_info.ppm_on = FALSE;
      (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_mgp_timer);
      return TRUE;
    }
    else
    {
      MSG_ERROR("tm_1x_up_is801_drv_end_ppm Failed", 0, 0, 0);
      return FALSE;
    }                  
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_end_prm

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_1x_up_is801_drv_end_prm(void)
{
    tm_1x_up_sess_req_param.stop_param.stop_type    = TM_STOP_TYPE_OPERATION;
    tm_1x_up_sess_req_param.stop_param.stop_reason  = TM_STOP_REASON_COMPLETED;
    tm_1x_up_sess_req_param.stop_param.op_to_stop   = TM_OP_REQ_PRM;

    if(tm_sess_req(TM_PRTL_TYPE_1X_UP,
               tm_1x_up_session_info.sess_handle,
               TM_SESS_REQ_STOP,
               &tm_1x_up_sess_req_param))
    {
      MSG_MED("tm_1x_up_is801_drv_end_prm Succeeded", 0, 0, 0);  
      tm_1x_up_session_info.mgp_info.prm_on = FALSE;
      return TRUE;
    }
    else
    {
      MSG_ERROR("tm_1x_up_is801_drv_end_prm Failed", 0, 0, 0);
      return FALSE;
    }                  
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_trans_op_mode

DESCRIPTION
  This function is used to translate the IS-801 Driver session mode 
  (MSA, MSB, MSB Download) to the Sub-Module current session mode.

  
RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
tm_1x_up_smod_curr_op_mode_e_type tm_1x_up_is801_drv_trans_op_mode 
(
  tm_1x_up_mode_e_type  cur_sess_mode 
    /* Current IS-801 Driver Session mode*/
)
{
  tm_1x_up_smod_curr_op_mode_e_type ret_mode = TM_1X_UP_SMOD_CURR_OP_MODE_NONE;

  MSG_MED("tm_1x_up_is801_drv_trans_op_mode: curr_sess_mode=%d",cur_sess_mode,0,0);
  switch( cur_sess_mode )
  {
  case TM_1X_UP_MODE_NONE:
    ret_mode = TM_1X_UP_SMOD_CURR_OP_MODE_NONE;
    break;
  case TM_1X_UP_MODE_MSA:
    ret_mode = TM_1X_UP_SMOD_CURR_OP_MODE_MSA;
    break;
  case TM_1X_UP_MODE_MSB:
    ret_mode = TM_1X_UP_SMOD_CURR_OP_MODE_MSB;
    break;
  case TM_1X_UP_MODE_DLOAD:
    ret_mode = TM_1X_UP_SMOD_CURR_OP_MODE_DLOAD;
    break;
  default:
    ret_mode = TM_1X_UP_SMOD_CURR_OP_MODE_NONE;
    break;

  }

  return ret_mode;
} /* tm_1x_up_is801_drv_trans_op_mode */


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_session_stop

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_1x_up_is801_drv_session_stop(tm_sess_stop_e_type         stop_type,
                           tm_sess_stop_reason_e_type  stop_reason,
                                        tm_op_req_type              op_to_stop)
{

  tm_1x_up_sess_req_param.stop_param.stop_type    = stop_type;
  tm_1x_up_sess_req_param.stop_param.stop_reason  = stop_reason;
  tm_1x_up_sess_req_param.stop_param.op_to_stop   = op_to_stop;

   if(tm_sess_req(TM_PRTL_TYPE_1X_UP,
                 tm_1x_up_session_info.sess_handle,
                 TM_SESS_REQ_STOP,
                 &tm_1x_up_sess_req_param))
   {
     MSG_MED("tm_1x_up_is801_drv_session_stop Succeeded", 0, 0, 0);  
     return TRUE;
   }
   else
   {
     MSG_HIGH("tm_1x_up_is801_drv_session_stop Failed", 0, 0, 0);
     return FALSE;
   }                  
}

#ifdef FEATURE_GPSONE_UPDATE_BSID
/*===========================================================================

FUNCTION tm_1x_up_is801_drv_init_bs_info

DESCRIPTION
  This function is used initialize the pdsm_bs_info_update_db. It checks
  whether the BS info is staled and registers whether a BS info update is
  required.

RETURN VALUE
  None

DEPENDENCIES
  Should be called before mi-initiated session if fresh BS info is required.

===========================================================================*/
void tm_1x_up_is801_drv_init_bs_info( void )
{
  MSG_MED("Check bs_info_update_reqd",0,0,0);
  if( mcc_pd_ft_dm_needed() )
  {
    tm_1x_up_session_info.bs_info.bs_info_update_reqd = TRUE;
  }
  else
  {
    tm_1x_up_session_info.bs_info.bs_info_update_reqd = FALSE;
  }
}
#endif /* FEATURE_GPSONE_UPDATE_BSID */

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_update_session_state

DESCRIPTION
  This function determines if a state transition request is valid and 
  set session state accordingly.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_1x_up_is801_drv_update_session_state
(
  tm_1x_up_session_state_e_type new_state
)
{
  if(new_state != tm_1x_up_session_info.session_state)
  {
    MSG_MED("tm_1x_up state transition [%d] -> [%d]", 
            tm_1x_up_session_info.session_state,
            new_state, 
            0);
  
    tm_1x_up_session_info.session_state = new_state;
  }
}
/*===========================================================================

FUNCTION tm_1x_up_is801_drv_reset_session_info

DESCRIPTION
  This function is used initialize session struct after the Session is Over.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_reset_session_info(void)
{
  /* Session is init state, no active sessions going on.
  */
  tm_1x_up_is801_drv_update_session_state(TM_1X_UP_SESSION_STATE_INIT);

  /* Indicate that we are not waiting or requesting any IS801 request's.
  */
  tm_1x_up_session_info.is801_req_wait_bmap = 0;

  /* init the Substate info.
  */
  tm_1x_up_session_info.mode_info.mode                = TM_1X_UP_MODE_NONE;
  tm_1x_up_session_info.mode_info.resp_to_send        = 0;
  tm_1x_up_session_info.mode_info.req_to_send         = 0;
  tm_1x_up_session_info.mode_info.last_error          = TM_1X_UP_MODE_NONE;
  tm_1x_up_session_info.mode_info.err_count_fix       = 0;
  tm_1x_up_session_info.mode_info.err_count_fix_msb   = 0;
  tm_1x_up_session_info.mode_info.err_count_fix_msa   = 0;
  tm_1x_up_session_info.mode_info.waiting_msb_lr     = FALSE;
  tm_1x_up_session_info.mode_info.pending_fallback_msa_fix     = FALSE;

  tm_1x_up_session_info.tm_core_initiate_session = FALSE;
  tm_1x_up_session_info.dload_activated  = FALSE;
  tm_1x_up_session_info.stop_reason = TM_STOP_REASON_NONE;
  qw_set(tm_1x_up_session_info.lr_timestamp,0,0);

  tm_1x_up_session_info.aiding_data_status.gps_alm_validity = 1;
  tm_1x_up_session_info.aiding_data_status.gps_eph_validity = 1;
  tm_1x_up_session_info.aiding_data_status.time_unc = TM_TIME_THRESHOLD_NO_TIME;
  tm_1x_up_session_info.aiding_data_status.pos_unc = TM_LR_THRESHOLD_NO_POS;
  tm_1x_up_session_info.aiding_data_status.gps_eph_requested_sv_list = 0;
  tm_1x_up_session_info.aiding_data_status.gps_alm_delivered_sv_list = 0;
  tm_1x_up_session_info.aiding_data_status.gps_eph_delivered_sv_list = 0;
#ifdef FEATURE_GNSS_SM_TM_1X_UP_GLONASS_ENABLE
  #error code not present
#endif
#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#endif
  tm_1x_up_session_info.dormancy_event_mask = TM_1X_UP_DORMANCY_EVENT_MASK_NONE;
  tm_1x_up_session_info.sess_end_sent = FALSE;


  memset( &tm_1x_up_info_buffer, 0, sizeof(pdsm_pd_info_s_type));
  memset( &tm_1x_up_session_info.fwd_link_pddm, 0, sizeof(is801_fwd_link_pddm_info_s_type));
  memset( &tm_1x_up_pos_info, 0, sizeof(tm_pd_position_info_s_type));
  memset( &tm_1x_up_ext_pos_data, 0, sizeof(tm_ext_pos_data_s_type));
  tm_1x_up_dload_info.num_data = 0;

  if(up_prm_buff_ptr)
  {
    (void)pdsm_freebuf((char *)up_prm_buff_ptr);
    up_prm_buff_ptr = NULL;
  }

  if(up_prefix_ppm_buff_ptr)
  {
    (void)pdsm_freebuf((char *)up_prefix_ppm_buff_ptr);
    up_prefix_ppm_buff_ptr = NULL;
  }
  
  if(ref_position_ptr)
  {
    (void)pdsm_freebuf((char *)ref_position_ptr);
    ref_position_ptr = NULL;
  }
} /* End tm_1x_up_is801_drv_reset_session_info() */

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_cleanup

DESCRIPTION   

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_cleanup(boolean abort_connection)
{
  tm_sess_stop_reason_e_type         cached_stop_reason = TM_STOP_REASON_COMPLETED;

  MSG_MED("1X UP Cleanup",0,0,0);
  if(abort_connection)
  {
    tm_1x_up_comm_abort_connection();
  }
  else
  {
    tm_1x_up_comm_close_connection();
  }

  cached_stop_reason = tm_1x_up_session_info.stop_reason;


  /* Provide info to 1x UP Sub-modules */
  if(tm_1x_up_session_info.stop_reason == TM_STOP_REASON_COMPLETED)
  {
    (void) tm_1x_up_smod_info_available( &tm_1x_up_info_buffer,
                                  PDSM_PD_END_SESS_NONE,
                                  tm_1x_up_session_info.lr_timestamp,
                                  NULL);
  }
  else
  {
    (void) tm_1x_up_smod_info_available( NULL,
                                  PDSM_PD_END_NO_SRV,
                                  NULL,
                                  NULL);
  }

  tm_1x_up_is801_drv_reset_session_info();

  tm_state_machine_execution_clean_up();

  is801_reset_data(TM_PRTL_TYPE_1X_UP);
#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#endif

  if(TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable))
  {
    /* Since IS801 was using TM-Core handle provided by SUPL,
      Tell SUPL that IS801 session is over, so that it can stop the TM-Core session*/
    tm_umts_up_supl_is801_session_stop(cached_stop_reason);
    return;
  }

  /* Tell TM-Core to stop the session */
	(void)tm_1x_up_is801_drv_session_stop(TM_STOP_TYPE_SESSION,cached_stop_reason,TM_OP_REQ_NONE);
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_stop_session

DESCRIPTION   This function handles stop session from TM-Core.

RETURN VALUE
  None

DEPENDENCIES
TBD: here cleanup the IS801B encoder decoder
===========================================================================*/
void tm_1x_up_is801_drv_handle_stop_session(tm_sess_stop_reason_e_type stop_reason)
{
    if(tm_1x_up_session_info.session_state == TM_1X_UP_SESSION_STATE_INIT)
    {
      /*Nothing to stop*/
      return;
    }
    
    tm_1x_up_session_info.stop_reason = stop_reason;

    if (tm_1x_up_session_info.dload_activated == TRUE)
    {
      tm_1x_up_session_info.dload_activated = FALSE;
      /* fire a dload end event.
      */
      sm_report_event_gps_pd_download_end(stop_reason); /*lint !e641 */
    }

  #ifdef FEATURE_GPSONE_UPDATE_BSID
    tm_1x_up_session_info.bs_info.bs_info_update_reqd = FALSE;
  #endif

    (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_sess_timer);
    (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_mgp_timer);
    (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_is801_timer);  
    (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_connect_timer);  

    if ( tm_1x_up_session_info.mgp_info.ppm_on )
    {
      (void)tm_1x_up_is801_drv_end_ppm( );
    }

    if (tm_1x_up_session_info.mgp_info.prm_on == TRUE)
    {
      (void)tm_1x_up_is801_drv_end_prm();
    }

    if(stop_reason == TM_STOP_REASON_E911)
    {
      tm_1x_up_is801_drv_cleanup(TRUE);
    }
    else
    {
      tm_1x_up_is801_drv_cleanup(FALSE);
    }
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_prm_timeout

DESCRIPTION
  This function gets called once MGP timeout has expired.

RETURN VALUE

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_handle_prm_timeout(void)
{

  MSG_HIGH("Handle PRM timeout",0,0,0);

  if (tm_1x_up_session_info.mgp_info.prm_on == TRUE)
  {
    MSG_HIGH("Aborting MGP PR meas",0,0,0);

    (void)tm_1x_up_is801_drv_end_prm();

  }
} /* End tm_1x_up_handle_prm_timeout() */

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_end_session

DESCRIPTION   This function tells TM-Core that session is ended.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_end_session(tm_sess_stop_reason_e_type stop_reason)
{
  MSG_HIGH("Ending Session. Reason %d",stop_reason,0,0);

  if (tm_1x_up_session_info.dload_activated == TRUE)
  {
    tm_1x_up_session_info.dload_activated = FALSE;
    /* fire a dload end event.
    */
    sm_report_event_gps_pd_download_end(stop_reason); /*lint !e641 */
  }

#ifdef FEATURE_GPSONE_UPDATE_BSID
  tm_1x_up_session_info.bs_info.bs_info_update_reqd = FALSE;
#endif

  (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_sess_timer);
  (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_mgp_timer);
  (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_is801_timer);  
  (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_connect_timer);  

  MSG_HIGH("Starting close connection procedure",0,0,0);
  tm_1x_up_comm_close_connection();

  if ( tm_1x_up_session_info.mgp_info.ppm_on )
  {
    (void)tm_1x_up_is801_drv_end_ppm( );
  }

  if (tm_1x_up_session_info.mgp_info.prm_on == TRUE)
  {
    (void)tm_1x_up_is801_drv_end_prm();
  }

  tm_1x_up_session_info.stop_reason = stop_reason;

  if(pdcomm_info.wait_for_sess_end_ack == TRUE)
  {
    MSG_HIGH("Delay sending STOP to TM_Core as waiting for sess_end ACK",0,0,0);
  }
  else
  {
    tm_1x_up_is801_drv_cleanup(FALSE);
  }
  return;
} /* End tm_1x_up_is801_drv_end_session() */

uint32 i_tm_1x_up_is801_drv_set_req_mask
(
  boolean gps_eph_valid,
  boolean gps_alm_valid,
  boolean glo_eph_valid,
  boolean glo_alm_valid,
  boolean pos_valid
)
{
  uint32 ret_val = 0;

  ret_val |= gps_eph_valid ? 0 : TM_1X_UP_RLINK_REQ_GPS_EPH;
  ret_val |= gps_alm_valid ? 0 : TM_1X_UP_RLINK_REQ_GPS_ALM;
  ret_val |= pos_valid     ? 0 : TM_1X_UP_RLINK_REQ_LR;

#ifdef FEATURE_GNSS_SM_TM_1X_UP_GLONASS_ENABLE
#error code not present
#endif
  return ret_val;
}

uint32 i_tm_1x_up_is801_drv_set_resp_mask
(
  boolean pos_valid
)
{
  uint32  ret_val = 0;
  if(pos_valid)
  {
    ret_val    =  IS801_REV_LINK_RESP_MASK_LR;
  }
  else
  {
    ret_val    |= IS801_REV_LINK_RESP_MASK_PPM;
  }

  return ret_val;
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_set_mode

DESCRIPTION

RETURN VALUE
  Boolean

DEPENDENCIES
==============================================================================*/

boolean tm_1x_up_is801_drv_set_mode(void)
{
  boolean ret_val       = TRUE;
  boolean pos_valid     = FALSE;
  boolean time_valid    = FALSE;

  uint32  *req_to_send  = &(tm_1x_up_session_info.mode_info.req_to_send);
  uint32  *resp_to_send = &(tm_1x_up_session_info.mode_info.resp_to_send);

  tm_1x_up_aiding_data_status_s_type  *status_ptr = &(tm_1x_up_session_info.aiding_data_status);  
  tm_cm_phone_state_info_s_type        phone_state_info;
  

  pdsm_pd_session_operation_e_type     operation_mode = 
          tm_1x_up_session_info.api_param.pd_api_info.pd_option.operation_mode;


  MSG_HIGH("Pos_Unc=%dm,Eph_validity=%d,Alm_validity=%d",
                              status_ptr->pos_unc,
                              status_ptr->gps_eph_validity, 
                              status_ptr->gps_alm_validity);

  /* Initialize validity flags for posn and time */
  if(status_ptr->pos_unc < TM_LR_THRESHOLD_NO_SEED_POS)
    pos_valid   = TRUE;

  if ( (status_ptr->pos_unc + status_ptr->time_unc) < TM_TIME_THRESHOLD_NO_TIME)
    time_valid  = TRUE;

  if ( tm_1x_up_session_info.api_param.pd_api_info.pd_option.session == PDSM_PD_SESS_TYPE_DATA_DLOAD)
  {
    MSG_HIGH("Process Dload request",0,0,0);

    *resp_to_send                = 0;
    *req_to_send                 = 0;

    /* Set the request mask based on current status of Eph/Alm/Loc.
    */
    *req_to_send |= i_tm_1x_up_is801_drv_set_req_mask(
                               status_ptr->gps_eph_validity,
                               status_ptr->gps_alm_validity,
                               status_ptr->glo_eph_validity,
                               status_ptr->glo_alm_validity, 
                               pos_valid);

    /* If MS needs Dload data, set the MS state properly and the response bits.
    */
    if(*req_to_send)
    {
      /* Dload of data from PDE is required, proceed.
      */
      MSG_HIGH("Dload operation req,mask=%d",*req_to_send,0,0);

      tm_1x_up_session_info.mode_info.mode            = TM_1X_UP_MODE_DLOAD;

      *resp_to_send |= i_tm_1x_up_is801_drv_set_resp_mask(pos_valid);

      MSG_HIGH("Resp to send=%d",*resp_to_send,0,0);
    } /* end if req_to_send */
    else
    {
      MSG_HIGH("Dload from PDE not required",0,0,0);

      tm_1x_up_session_info.mode_info.mode = TM_1X_UP_MODE_NONE;
      /* Dload operation is completed, exit the dload session now.
      */
      tm_1x_up_is801_drv_end_session(TM_STOP_REASON_COMPLETED);
    } /* end else req_to_send */
    return ret_val;
  } /* DATA_DLOAD */

  /****************************************************************************
   * Fix is needed, proceed with session settings.
   ***************************************************************************/
  MSG_HIGH("Process Fix request from Client",0,0,0);

  /* If last operation was an error, change the mode for Optimal operation modes for next attempt.
  */
  if (tm_1x_up_is801_drv_is_optimal_mode())
  {
    if (tm_1x_up_session_info.mode_info.last_error == TM_1X_UP_MODE_MSB)
    {
      /* Since MS-Based failed, try MS-Assisted session.
      */
      MSG_HIGH("MS-B failed last time,force MS-A",0,0,0);
      tm_1x_up_session_info.mode_info.mode          = TM_1X_UP_MODE_MSA;
      return ret_val;
    }
    else if ( (tm_1x_up_session_info.mode_info.last_error == TM_1X_UP_MODE_MSA) || 
              (tm_1x_up_session_info.mode_info.last_error == TM_1X_UP_MODE_DLOAD) )
    {
      /* Since PDE session failed previously, try MS-Based session.
      */
      MSG_HIGH("MS-A/DLOAD failed last time,force MS-B",0,0,0);
      tm_1x_up_session_info.mode_info.mode          = TM_1X_UP_MODE_MSB;
      return ret_val;
    }
  } /* end if optimal modes only */

  if (operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_SPEED )
  {
    MSG_MED("Process Speed Optimal mode",0,0,0);

    tm_cm_iface_get_phone_state(&phone_state_info);

    if ((status_ptr->gps_eph_validity == 1) &&
        (phone_state_info.srv_system == CGPS_SRV_SYS_CDMA) &&
        (status_ptr->pos_unc  < TM_PUNC_THRESHOLD_IN_SERVICE))
    {
      /* Mobile has seed position and EPH data and Punc is < 12km. so proceed with MS-Based session
      */
      MSG_MED("1X in service. Forcing MS-Based(punc=%d)",
                  tm_1x_up_session_info.aiding_data_status.pos_unc,0,0);

      tm_1x_up_session_info.mode_info.mode          = TM_1X_UP_MODE_MSB;
    }
    else if ((status_ptr->gps_eph_validity == 1) && (time_valid == TRUE))
    {
      /* Mobile has seed position and EPH data and Punc+ Tunc < 24Km so proceed with MS-Based session
      */
      MSG_MED("Forcing MS-Based(lr+clk=%d)",
                 status_ptr->pos_unc + status_ptr->time_unc, 0, 0);
      tm_1x_up_session_info.mode_info.mode          = TM_1X_UP_MODE_MSB;
    }
    else
    {
      /* We don't have all the data for doing MS-Based fixes, proceed based on available data
      ** and system settings.
      */
      if( ((phone_state_info.srv_system == CGPS_SRV_SYS_CDMA) &&
           (status_ptr->pos_unc  <= TM_PUNC_THRESHOLD_IN_SERVICE)) ||
          (time_valid == TRUE)
        )
      {
        /* Mobile is in service AND Punc is < 12km  OR
        ** Mobile is OOS and punc+tunc < 24km, then
        ** proceed with Dload session.
        */
        MSG_MED("1X in service=%d(1=Y/0=N). punc=%d tunc=%d",
             phone_state_info.srv_system, 
             status_ptr->pos_unc,
             status_ptr->time_unc);
          
        *resp_to_send       = 0;
        *req_to_send        = 0;

        *req_to_send |= i_tm_1x_up_is801_drv_set_req_mask(
                                   FALSE, /* req GPS Eph */
                                   status_ptr->gps_alm_validity,
                                   FALSE, /* req Glo Eph */
                                   status_ptr->glo_alm_validity, 
                                   pos_valid);

        *resp_to_send |= i_tm_1x_up_is801_drv_set_resp_mask(pos_valid);
    
        MSG_HIGH("Dload data,req=%d,resp=%d", *req_to_send, *resp_to_send, 0);
        tm_1x_up_session_info.mode_info.mode          = TM_1X_UP_MODE_DLOAD;
      }
      else
      {
        /* Either single-shot fix or Tracking session with no seed position. If ephemeris data
        ** is required it will be downloaded after current operation is completed.
        */
        MSG_MED("Force MS-Assist mode(lr+clk=%d)",
                   status_ptr->pos_unc + status_ptr->time_unc,0,0);
        tm_1x_up_session_info.mode_info.mode          = TM_1X_UP_MODE_MSA;

        /* Set the dload info mode for MS-A seed */
        tm_1x_up_dload_info.jgps_msdload_info.msa_seed = TRUE;
      }
    }
    /* if we decide to do MSB or DLOAD after receiving the wish list update, send a dummy wish
       list request to PE so that the ephemeris request will be tagged in PE. This will be the
       case for optimal data or speed since the first time we request wish list it is a query
       so PE does not tag the svs as needing eph, but if after we get the wish list response
       from PE, TM decides to do MSB, then PE needs to be informed that we are going to the
       network for requesting eph. */
    if (tm_1x_up_session_info.mode_info.mode != TM_1X_UP_MODE_MSA)
    {
      tm_1x_up_sess_req_param.continue_param.op_req  = TM_OP_REQ_AIDING_DATA_STATUS;
      tm_1x_up_sess_req_param.continue_param.op_mode  = TM_SESS_OPERATION_MODE_MSB;
      tm_1x_up_sess_req_param.continue_param.dynamic_qos_enabled = TRUE;
      tm_1x_up_sess_req_param.continue_param.num_fixes = tm_1x_up_session_info.api_param.pd_api_info.pd_option.fix_rate.num_fixes;
      tm_1x_up_sess_req_param.continue_param.tbf_ms = tm_1x_up_session_info.api_param.pd_api_info.pd_option.fix_rate.time_between_fixes_ms;

      if (tm_sess_req(TM_PRTL_TYPE_1X_UP,
                      tm_1x_up_session_info.sess_handle,
                      TM_SESS_REQ_CONTINUE,
                      &tm_1x_up_sess_req_param))
      {
        MSG_MED("Continue optimal as MSB sent ", 0, 0, 0);  
      }
      else
      {
        MSG_ERROR("Continue optimal as MSB not sent", 0, 0, 0);
      }
    }
  }
  else if (operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_DATA)
  {
    MSG_MED("Process Data Optimal mode",0,0,0);

    if ( (status_ptr->gps_eph_validity == 1) && (status_ptr->pos_unc < TM_LR_THRESHOLD_NO_POS))
    {
      MSG_MED("Force MS-Based session",0,0,0);
      tm_1x_up_session_info.mode_info.mode          = TM_1X_UP_MODE_MSB;
    }
    else
    {
      if ( status_ptr->pos_unc >= TM_LR_THRESHOLD_NO_POS )
      {
        MSG_MED("Force MS-Assist session",0,0,0);
        tm_1x_up_session_info.mode_info.mode          = TM_1X_UP_MODE_MSA;
      }
      else
      {
        /* MS does have some idea where it is, but it doesn't have Eph data.
        */
        *resp_to_send        = 0;
        *req_to_send         = 0;

        *req_to_send |= i_tm_1x_up_is801_drv_set_req_mask(
                                                         FALSE, /* req GPS Eph */
                                                         status_ptr->gps_alm_validity,
                                                         FALSE, /* req Glo Eph */
                                                         status_ptr->glo_alm_validity, 
                                                         pos_valid);


        *resp_to_send |= i_tm_1x_up_is801_drv_set_resp_mask(pos_valid);

        MSG_HIGH("Dload data,req=%d,resp=%d",*req_to_send,*resp_to_send,0);
        tm_1x_up_session_info.mode_info.mode          = TM_1X_UP_MODE_DLOAD;
      } /* end EPH_DATA  */
    } /* end else no data required */    
    /* if we decide to do MSB or DLOAD after receiving the wish list update, send a dummy wish
       list request to PE so that the ephemeris request will be tagged in PE. This will be the
       case for optimal data or speed since the first time we request wish list it is a query
       so PE does not tag the svs as needing eph, but if after we get the wish list response
       from PE, TM decides to do MSB, then PE needs to be informed that we are going to the
       network for requesting eph. */
    if (tm_1x_up_session_info.mode_info.mode != TM_1X_UP_MODE_MSA)
    {
      tm_1x_up_sess_req_param.continue_param.op_req  = TM_OP_REQ_AIDING_DATA_STATUS;
      tm_1x_up_sess_req_param.continue_param.op_mode  = TM_SESS_OPERATION_MODE_MSB;
      tm_1x_up_sess_req_param.continue_param.dynamic_qos_enabled = TRUE;
      tm_1x_up_sess_req_param.continue_param.num_fixes = tm_1x_up_session_info.api_param.pd_api_info.pd_option.fix_rate.num_fixes;
      tm_1x_up_sess_req_param.continue_param.tbf_ms = tm_1x_up_session_info.api_param.pd_api_info.pd_option.fix_rate.time_between_fixes_ms;

      if (tm_sess_req(TM_PRTL_TYPE_1X_UP,
                      tm_1x_up_session_info.sess_handle,
                      TM_SESS_REQ_CONTINUE,
                      &tm_1x_up_sess_req_param))
      {
        MSG_MED("Continue optimal as MSB sent ", 0, 0, 0);  
      }
      else
      {
        MSG_ERROR("Continue optimal as MSB not sent", 0, 0, 0);
      }
    }
  } /* end else DATA OPT*/
  else if (operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_ACCURACY)
  {
    MSG_MED("Process Accuracy optimal mode",0,0,0);

    tm_1x_up_session_info.mode_info.mode          = TM_1X_UP_MODE_MSA;
  } /* End ACCURACY */
  else if (operation_mode == PDSM_SESSION_OPERATION_MSASSISTED)
  {
    MSG_MED("Process MS-Assist only session",0,0,0);

    tm_1x_up_session_info.mode_info.mode          = TM_1X_UP_MODE_MSA;

    /* For PDE only session check the  QoS, if QoS = 0 we can just do an AFLT only session.
    */
    if(tm_1x_up_session_info.api_param.pd_api_info.pd_qos.gps_session_timeout == 0 )
    {
      MSG_MED("Forcing AFLT only session",0,0,0);
      tm_1x_up_session_info.api_param.session_type = TM_1X_UP_SESSION_TYPE_AFLT;
    }
  } /* End else PDE_CALC_ONLY */
  else if (operation_mode == PDSM_SESSION_OPERATION_MSBASED)
  {
    MSG_MED("Process MS based only session ..",0,0,0);

    if ( (status_ptr->gps_eph_validity == 1) && (status_ptr->pos_unc < TM_LR_THRESHOLD_NO_POS))
    {
      /* MS has some notion of current position and valid Eph data, proceed with MS-Based session.
      */
      MSG_MED("Force MS-Based session",0,0,0);
      tm_1x_up_session_info.mode_info.mode          = TM_1X_UP_MODE_MSB;
    }
    else
    {
      MSG_HIGH("Dload data for MS-based only",0,0,0);
      /* We don't have all the data for doing MS-Based fixes, proceed based on available data
      ** and system settings. In MSBased only MS-Assisted GPS is not allowed.
      */
      *resp_to_send        = 0;
      *req_to_send         = 0;

      /* Check what needs to be downloaded from PDE.
      */
      *req_to_send |= i_tm_1x_up_is801_drv_set_req_mask(
                                 FALSE, /* req GPS Eph */
                                 status_ptr->gps_alm_validity,
                                 FALSE, /* req Glo Eph */
                                 status_ptr->glo_alm_validity, 
                                 pos_valid);

      if (*req_to_send)
      {
        *resp_to_send |= i_tm_1x_up_is801_drv_set_resp_mask(pos_valid);

        MSG_HIGH("Dload data,req=%d,resp=%d",*req_to_send,*resp_to_send,0);
        tm_1x_up_session_info.mode_info.mode          = TM_1X_UP_MODE_DLOAD;
      }
      else
      {
        /* This shouldn't happen, but we can recover so try MSBased session.
        */
        MSG_ERROR("Not possible, try MSB anyways",0,0,0);
        tm_1x_up_session_info.mode_info.mode          = TM_1X_UP_MODE_MSB;
      } /* req_to_send */
    } /* end else */
  } /* end else if MS-Based */
  else if(operation_mode == PDSM_SESSION_OPERATION_REF_POSITION)
  {
    MSG_MED("Process REF_Position mode",0,0,0);
    *req_to_send = i_tm_1x_up_is801_drv_set_req_mask(
                               FALSE, /* req GPS Eph */
                               status_ptr->gps_alm_validity,
                               FALSE, /* req Glo Eph */
                               status_ptr->glo_alm_validity,
                               FALSE /* req Ref LR */ );

    /* We will always send PPM in this case */
    *resp_to_send = i_tm_1x_up_is801_drv_set_resp_mask(FALSE);

    MSG_HIGH("Dload data,req=%d,resp=%d",*req_to_send,*resp_to_send,0);
    tm_1x_up_session_info.mode_info.mode          = TM_1X_UP_MODE_DLOAD;
  }
  else
  {
    MSG_ERROR("Mode not supported",0,0,0);
    ret_val = FALSE;
  }
  return ret_val;
} /* end operation_mode() */


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_session_active

DESCRIPTION
  This function is used to indicate if session is active or not.

RETURN VALUE
  TRUE if Active, else FALSE.

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_is801_drv_session_active( void )
{
  boolean ret_val = FALSE;
  if (tm_1x_up_session_info.session_state > TM_1X_UP_SESSION_STATE_INIT)
  {
    ret_val = TRUE;
  }
  return ret_val;
} /* End tm_1x_up_is801_drv_session_active() */


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_set_mpc_throttle_check

DESCRIPTION
  This function is used to set the msb_throttling_info.mpc_throttle_check flag

RETURN VALUE
  NONE

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_set_mpc_throttle_check(boolean bMPCThrottleCheck)
{
  tm_1x_up_config.msb_throttling_info.mpc_throttle_check = bMPCThrottleCheck;
}


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_check_msb_throttling

DESCRIPTION
  This function is used to check if the next connection to PDE/MPC should
  be throttled or not.

RETURN VALUE
  TRUE: Yes, throttle this connection
  FALSE: No, don't throttle this connection.

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_is801_drv_check_msb_throttling(boolean bConnectToMPC)
{
  boolean status = FALSE;                  /* By default don't throttle */
  qword current_sys_time, qw_time;
#ifndef FEATURE_CALL_THROTTLE          
  tm_cm_phone_state_info_s_type phone_state_info;
#endif /* FEATURE_CALL_THROTTLE */    

  /* Check if connection is for PDE or MPC
     If MPC: Perform throttling checks
     If PDE: Skip throttle checks for non-trusted sessions
  */
  if (bConnectToMPC==TRUE)
  {
    MSG_HIGH("MPC Connect: Throttle non-trusted MPC sessions",0,0,0);
    tm_1x_up_config.msb_throttling_info.mpc_throttle_check = TRUE;
  }
  else
  {
    if (tm_1x_up_config.msb_throttling_info.mpc_throttle_check==TRUE)
    {
      MSG_HIGH("PDE Connect: Don't throttle non-trusted PDE sessions",0,0,0);
      return status;
    }
  }
  
  if ((bConnectToMPC==TRUE) || 
      ((tm_1x_up_is801_drv_session_active()) && (tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_DLOAD)) )
  {
    MSG_HIGH("MSBased download, check if throttling is required",0,0,0);

    if (tm_1x_up_config.msb_throttling_info.msb_throttling_enabled)
    {
      MSG_HIGH("MSB throttling enabled, verify values.....",0,0,0);

      (void) time_get(current_sys_time);
      qw_equ(qw_time,tm_1x_up_config.msb_throttling_info.back_off_reset_duration);
        
      /* Calculate system time for safe eph download. (curr_time + reset duration)
      */
      qw_add(qw_time,qw_time,tm_1x_up_config.msb_throttling_info.msb_last_request_time);

      if ( qw_cmp(qw_time,current_sys_time) <= 0 )
      {
        MSG_HIGH("Request after safe delay, back-off not required",0,0,0);

        /* Set the back-off time back to min value
        */
        qw_equ(tm_1x_up_config.msb_throttling_info.back_off_time,tm_1x_up_config.msb_throttling_info.back_off_min_duration);
        qw_add(tm_1x_up_config.msb_throttling_info.throttle_end_time,current_sys_time,tm_1x_up_config.msb_throttling_info.back_off_time);
        
#ifdef FEATURE_CALL_THROTTLE                
        tm_1x_up_config.msb_throttling_info.safe_dload_req = TRUE;
#endif /* FEATURE_CALL_THROTTLE */
      }
      else  
      {                            

#ifdef FEATURE_CALL_THROTTLE                
        tm_1x_up_config.msb_throttling_info.safe_dload_req = FALSE;
#endif /* FEATURE_CALL_THROTTLE */

        MSG_HIGH("Early request, verify throttle time first",0,0,0);

        /* Somehow the request for new Eph came sooner, check if after Throttle time
        */
        if (qw_cmp(current_sys_time,tm_1x_up_config.msb_throttling_info.throttle_end_time) > 0) 
        {
          MSG_HIGH("Request after Throttle end time, allow download",0,0,0);

#ifdef FEATURE_CALL_THROTTLE          
          MSG_HIGH("DCTM on, increase back_off_time when ppp opened",0,0,0);
#else         
          tm_cm_iface_get_phone_state(&phone_state_info);
          if(phone_state_info.srv_system == CGPS_SRV_SYS_CDMA ||
             phone_state_info.srv_system == CGPS_SRV_SYS_HDR)
          {
            /* Current sys time is more than throttling time. So, increment
            ** the back off time and proceed with the request.
            */
            qw_mul(tm_1x_up_config.msb_throttling_info.back_off_time,tm_1x_up_config.msb_throttling_info.back_off_time,tm_1x_up_config.msb_throttling_info.back_off_factor);
            qw_equ(qw_time,tm_1x_up_config.msb_throttling_info.back_off_max_duration);

            if ( qw_cmp(tm_1x_up_config.msb_throttling_info.back_off_time,qw_time) > 0 )
            {
              /* We are hitting max back-off time.
              */
              MSG_HIGH("Back off time set to maximum",0,0,0);
              qw_equ(tm_1x_up_config.msb_throttling_info.back_off_time,qw_time);
            }
            qw_add(tm_1x_up_config.msb_throttling_info.throttle_end_time,current_sys_time,tm_1x_up_config.msb_throttling_info.back_off_time);
          } /* end in_service() */
          else
          {
            MSG_HIGH("MS is OOS, don't change back-off time",0,0,0);
          }
#endif /* FEATURE_CALL_THROTTLE */                    
        } /* end else qw_cmp() */
        else
        {
          MSG_HIGH("Request during Throttle time,disable download",0,0,0);
          status = TRUE;
          /* start debug...
          */
          qw_sub(qw_time,tm_1x_up_config.msb_throttling_info.throttle_end_time,current_sys_time);
          (void)qw_div_by_power_of_2( qw_time, qw_time, 16 );

          qw_mul( qw_time, qw_time, ( dword )5);
          qw_inc( qw_time, 2);
          (void)qw_div( qw_time, qw_time, (word) 4 );
          (void)qw_div( qw_time,qw_time,1000 );

          MSG_MED("Time remaining=%dsecs",qw_time[0],0,0);
          /* End debug...
          */
        } 
      } /* End else curr_sys_time */
      /* Set the msb last request time to current time.
      */
      qw_equ(tm_1x_up_config.msb_throttling_info.msb_last_request_time,current_sys_time);

      /* Just add debug to show how much is the back off time.
      */
      qw_mul(qw_time,tm_1x_up_config.msb_throttling_info.back_off_time,5);
      (void)qw_div(qw_time,qw_time,4);
      qw_shift(qw_time,-16);
      (void)qw_div(qw_time,qw_time,1000);
      MSG_MED("Throttle duration set to=%dsecs",qw_time[0],0,0);
    } /* If MSB throttling is enabled */
  } /* end if dload */
  return status;
} /* End tm_1x_up_is801_drv_check_msb_throttling() */


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_translate_stop_reason

DESCRIPTION
   This function translate GPS session's PDAPI end status to TM's stop reason 
  
RETURN VALUE
  TM session stop reason

DEPENDENCIES
  Please make this routine consistent with tm_core_translate_end_status()

===========================================================================*/
static tm_sess_stop_reason_e_type tm_1x_up_is801_drv_translate_stop_reason(pdsm_pd_end_e_type status)
{
  tm_sess_stop_reason_e_type ret_val = TM_STOP_REASON_GENERAL_ERROR;
  
  switch(status)
  {
    case PDSM_PD_END_SESS_NONE:
      ret_val = TM_STOP_REASON_COMPLETED; // success case
      break;
    case PDSM_PD_END_FIX_ERROR:
      ret_val = TM_STOP_REASON_GENERAL_ERROR;      
      break;
    case PDSM_PD_END_E911:
      ret_val = TM_STOP_REASON_E911;
      break;
    case PDSM_PD_END_CLIENT_END:
      ret_val = TM_STOP_REASON_USER_TERMINATED;
      break;
    case PDSM_PD_END_TIMEOUT:
      ret_val = TM_STOP_REASON_FIX_TIMEOUT;
      break;
    case PDSM_PD_END_VX_AUTH_FAIL:
      ret_val = TM_STOP_REASON_MPC_REJECTED;
      break;
    case PDSM_PD_END_NO_CON:
      ret_val = TM_STOP_REASON_PDE_NO_CON;
      break;
    case PDSM_PD_END_CON_FAIL:
      ret_val = TM_STOP_REASON_PDE_CON_FAIL;
      break;
    case PDSM_PD_END_PDE_REJECT:
      ret_val = TM_STOP_REASON_PDE_REJECT;
      break;
    case PDSM_PD_END_STALE_BS_INFO:
      ret_val = TM_STOP_REASON_STALE_BS_INFO;
      break;
    case PDSM_PD_END_PPM_POST_FIX_TIMEOUT:
      ret_val = TM_STOP_REASON_PPM_POST_FIX_TIMEOUT;
      break;
    case PDSM_PD_END_PPM_PRE_FIX_TIMEOUT:
      ret_val = TM_STOP_REASON_PPM_PRE_FIX_TIMEOUT;
      break;
    case PDSM_PD_END_IS801_SERVER_TIMEOUT:
      ret_val = TM_STOP_REASON_IS801_SERVER_TIMEOUT;
      break;
    case PDSM_PD_END_IS801_PAYLOAD_ERROR:
      ret_val = TM_STOP_REASON_IS801_PAYLOAD_ERROR;
      break;
    case PDSM_PD_END_IS801_SEND_FAILURE:
      ret_val = TM_STOP_REASON_IS801_SEND_FAILURE;
      break;
    case PDSM_PD_END_PDE_END_SESSION:
      ret_val = TM_STOP_REASON_PDE_END_SESSION;
      break;

    /*------------------------------------------------------------------
          SS_MOLR_ERRORS
    ------------------------------------------------------------------*/
    case PDSM_PD_END_SS_MOLR_ERRORS_SYSTEM_FAILURE:
      ret_val = TM_STOP_REASON_SS_MOLR_ERRORS_SYSTEM_FAILURE; 
      break;
    case PDSM_PD_END_SS_MOLR_ERRORS_UNEXPECTED_DATA_VALUE:
      ret_val = TM_STOP_REASON_SS_MOLR_ERRORS_UNEXPECTED_DATA_VALUE;
      break;
    case PDSM_PD_END_SS_MOLR_ERRORS_DATA_MISSING:
      ret_val = TM_STOP_REASON_SS_MOLR_ERRORS_DATA_MISSING;
      break;
    case PDSM_PD_END_SS_MOLR_ERRORS_FACILITY_NOT_SUPPORTED:
      ret_val = TM_STOP_REASON_SS_MOLR_ERRORS_FACILITY_NOT_SUPPORTED;
      break;
    case PDSM_PD_END_SS_MOLR_ERRORS_SS_SUBSCRIPTION_VIOLATION:
      ret_val = TM_STOP_REASON_SS_MOLR_ERRORS_SS_SUBSCRIPTION_VIOLATION;
      break;
    case PDSM_PD_END_SS_MOLR_ERRORS_POSITION_METHOD_FAILURE:
      ret_val = TM_STOP_REASON_SS_MOLR_ERRORS_POSITION_METHOD_FAILURE;
      break;
    case PDSM_PD_END_SS_MOLR_ERRORS_UNDEFINED:
      ret_val = TM_STOP_REASON_SS_MOLR_ERRORS_UNDEFINED;
      break;

    case PDSM_PD_END_RESET_LOCATION_SERVICE:
      ret_val = TM_STOP_REASON_ON_RESET_LOCATION_SERVICE;
      break;

    default:
/*  
#ifdef FEATURE_JCDMA_GPSONE
      // JGPS specific end status reason
      if (status >= PDSM_PD_END_JGPS_POSRESP_NG_LIS &&  
          status <= (PDSM_PD_END_JGPS_DPOSCOMP_NG 
                              + PDSM_PD_DLOAD_EPHEMERIS
                              + PDSM_PD_DLOAD_ALMANAC
                              + PDSM_PD_DLOAD_LOCATION)
      {
        ret_val = tm_jgps_core_translate_end_status(status);      
      }
#endif
*/
      // map JGPS-specific code to GENERAL_ERROR for now
      ret_val = TM_STOP_REASON_GENERAL_ERROR;
      break;
  }

  return ret_val;
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_opn_completed

DESCRIPTION
  This function is called when a given operation /mode is over. 1X UP decides
  what to do next. The state can be enetered because of error in previous session
  or successfull completion of session. 
  
RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_opn_completed(pdsm_pd_end_e_type status)
{
  boolean optimal_mode;
  boolean ret_val = TRUE;
  tm_1x_up_smod_req_status_e_type   req_status = TM_1X_UP_SMOD_REQ_STATUS_NONE;
  tm_1x_up_smod_curr_op_mode_e_type cur_op_mode = TM_1X_UP_SMOD_CURR_OP_MODE_NONE;
  tm_sess_stop_reason_e_type  stop_reason;

  MSG_HIGH("op Complete,status=%d,mode=%d",status,tm_1x_up_session_info.mode_info.mode,0);

  stop_reason = tm_1x_up_is801_drv_translate_stop_reason(status);

  /* (1) Clean up ppm, prm, timers */
  if (tm_1x_up_session_info.mgp_info.ppm_on == TRUE)
  {
    (void)tm_1x_up_is801_drv_end_ppm();
  }

  if (tm_1x_up_session_info.mgp_info.prm_on == TRUE)
  {
    (void)tm_1x_up_is801_drv_end_prm();
  }

  if (tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_DLOAD || tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_MSA)
  {

    tm_1x_up_session_info.is801_req_wait_bmap  = 0;

    /*  IS801 timer could still be running, clear it first.
    */
    (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_is801_timer);  
    (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_connect_timer);  
  }


  /*-------------------------------------------------------------------------
     Stop a session with Sub-Modules: i.e. stop request sent to Japan-GPS
     module.
  -------------------------------------------------------------------------*/
  cur_op_mode 
    = tm_1x_up_is801_drv_trans_op_mode(tm_1x_up_session_info.mode_info.mode);

  (void)tm_1x_up_smod_stop_sess_req(status, 
                                    &tm_1x_up_dload_info,
                                    cur_op_mode, 
                                    &req_status);
 
  /* If MS was doing dload operation for a MO fix, proceed with the fix else if there is
  ** a user dload request, stay in idle.
  */
  if (tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_DLOAD)
  {
    tm_1x_up_session_info.dload_activated = FALSE;

    /* Indicate DIAG of download status
    */
    sm_report_event_gps_pd_download_end(status); /*lint !e641 */

    if (status != PDSM_PD_END_SESS_NONE)
    {
      MSG_ERROR("Error in Dload operation",0,0,0);
      /* Increment fix error count also so that we don't allow more that x error's per fix.
      */
      tm_1x_up_session_info.mode_info.err_count_fix++;
      if ( (tm_1x_up_is801_drv_session_active() == TRUE) && 
           (tm_1x_up_session_info.api_param.pd_api_info.pd_option.session == PDSM_PD_SESS_TYPE_DATA_DLOAD ))
      {
        MSG_ERROR("Max error for Dload..exit",0,0,0);
        tm_1x_up_is801_drv_end_session(stop_reason);
        return;
      }
    } /* end status */

    /* If waiting for LR from PE, close connection with PDE and continue. */
    if(tm_1x_up_session_info.mode_info.waiting_msb_lr)
    {
      /* Irrespective of previous status of dload, if we want a fix proceed.
      */
      MSG_HIGH("Waiting for LR in MSB fix",0,0,0);
      tm_1x_up_session_info.mode_info.mode = TM_1X_UP_MODE_MSB;

      /* Update LR  QoS */
      if(tm_1x_up_is801_drv_continue_msb_fix(MSB_START) == FALSE)
      {
        tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
        return;
      }

      MSG_HIGH("Starting close connection procedure",0,0,0);
      tm_1x_up_comm_close_connection();
    }
    else if(tm_1x_up_session_info.mode_info.pending_fallback_msa_fix)
    {
      /* If Dload failed and MSA fallback is pending, abort the fix */
      if (status != PDSM_PD_END_SESS_NONE)
      {
        tm_1x_up_is801_drv_end_session(stop_reason);
        return;
      }
      else
      {
        /* Pending MS-Assited session, so change the mode. Fix will be started after receiving SESS_END ack 
       */
        MSG_HIGH("Pending MSA fix",0,0,0);
        tm_1x_up_session_info.mode_info.mode = TM_1X_UP_MODE_MSA;
      }
    }
    else
    {
      MSG_HIGH("Dload over. No fix operation needed",0,0,0);
      tm_1x_up_is801_drv_end_session(TM_STOP_REASON_COMPLETED);
    }
  } /* DLOAD */
  else if ( (tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_MSA) || ( tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_MSB ) )
  {
    MSG_HIGH("Fix operation completed...",0,0,0);

    if (status != PDSM_PD_END_SESS_NONE)
    {
      /* Send UPDATE_FAILURE event */
      tm_1x_up_info_buffer.client_id = tm_1x_up_session_info.api_param.pd_api_info.client_id;
      (void)tm_post_pd_event( TM_PRTL_TYPE_1X_UP,
                              tm_1x_up_session_info.sess_handle,
                              PDSM_PD_EVENT_UPDATE_FAILURE,
                              &tm_1x_up_info_buffer,
                              NULL);

      /* Failure in operation increment the cumulative failure count.
      */
      tm_1x_up_session_info.mode_info.err_count_fix++;

      /* set the Error threshold based on whether we are doing MS-Based or MS-Assisted fixes.
      */
      if (tm_1x_up_session_info.mode_info.last_error == TM_1X_UP_MODE_MSB)
      {
        tm_1x_up_session_info.mode_info.err_count_fix_msb++;
      }
      else
      {
        tm_1x_up_session_info.mode_info.err_count_fix_msa++;
      }

      MSG_HIGH("err_c(A)=%d,err_c(B)=%d,total=%d",tm_1x_up_session_info.mode_info.err_count_fix_msa,
                      tm_1x_up_session_info.mode_info.err_count_fix_msb,tm_1x_up_session_info.mode_info.err_count_fix);

      if (tm_1x_up_is801_drv_is_optimal_mode())
      {
        optimal_mode = TRUE;
      }
      else
      {
        optimal_mode = FALSE;
      }

      /* For Optimal modes (seed/data/accuracy) change the mode dynamically.
      */
      if (optimal_mode == TRUE)
      {
        if ( tm_1x_up_session_info.mode_info.err_count_fix > TM_1X_UP_MAX_FIX_ERROR )
        {
          /* Allow at least one mode swith
          */
          MSG_ERROR("Error_cnt > threshold..Exit sess (stop_reason=%d)", stop_reason,0,0);
          MSG_MED("last_error=%d",tm_1x_up_session_info.mode_info.last_error,0,0);
          tm_1x_up_is801_drv_end_session(stop_reason);
          return;
        }
      }
      else if ( tm_1x_up_session_info.mode_info.err_count_fix >= TM_1X_UP_MAX_FIX_ERROR )
      {
        /* For force modes check >= for max error.
        */
        MSG_ERROR("Error_cnt >= threshold..Exit sess (stop_reason=%d)", stop_reason,0,0);
        MSG_MED("last_error=%d",tm_1x_up_session_info.mode_info.last_error,0,0);
        tm_1x_up_is801_drv_end_session(stop_reason);
        return;
      }

      if ( tm_1x_up_is801_drv_session_active() == TRUE)
      {
        ret_val = tm_1x_up_is801_drv_set_mode();

        /* Fallback in optimal mode */
        if ( (optimal_mode == TRUE) && (tm_1x_up_session_info.mode_info.mode == tm_1x_up_session_info.mode_info.last_error) )
        {
          /* Mode was not changed for optimal modes, so end the fix.
          */
          MSG_ERROR("No mode switch performed..Exit (stop_reason=%d)", stop_reason,0,0);
          tm_1x_up_is801_drv_end_session(stop_reason);
        }
        else
        {
          if(ret_val)
          {

#ifdef FEATURE_JCDMA_GPSONE
            tm_jgps_core_update_sess_state(TM_JGPS_SESS_STATE_START_REQ);
#endif /* FEATURE_JCDMA_GPSONE */
            /* Check if force dormancy is required or not.
            */
        #ifdef FEATURE_GPSONE_UPDATE_BSID
            tm_1x_up_is801_drv_init_bs_info();
        #endif
            tm_1x_up_session_info.tm_core_initiate_session = FALSE;
            ret_val = tm_1x_up_is801_drv_initiate_session();
          } /* end set_state end_event */

          if (ret_val == FALSE)
          {
            MSG_ERROR("Exit New session not allowed (stop_reason=%d)", stop_reason,0,0);
            tm_1x_up_is801_drv_end_session(stop_reason);
          }
        }
      }
      else
      {
        MSG_ERROR("Wrong state?? (stop_reason=%d)", stop_reason,0,0);
        tm_1x_up_is801_drv_end_session(stop_reason);
      }
    } /* end if status */
    else
    {
      MSG_MED("Good fix=%d",0,0,0);
      /* Reset the error count
      */
      tm_1x_up_session_info.mode_info.err_count_fix = 0;

      MSG_HIGH("Fix over",0,0,0);
      tm_1x_up_is801_drv_end_session(TM_STOP_REASON_COMPLETED);

    }
  }
} /* end tm_1x_up_is801_drv_opn_completed() */


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_msa_fix_allowed

DESCRIPTION
  This function is used to check if MS-A fix is allowed.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_is801_drv_msa_fix_allowed()
{
    if ( (tm_1x_up_session_info.api_param.pd_api_info.pd_option.operation_mode != PDSM_SESSION_OPERATION_STANDALONE_ONLY) &&
         (tm_1x_up_session_info.api_param.pd_api_info.pd_option.operation_mode != PDSM_SESSION_OPERATION_MSBASED) &&
         (tm_1x_up_session_info.mode_info.last_error != TM_1X_UP_MODE_DLOAD && (tm_1x_up_session_info.mode_info.last_error != TM_1X_UP_MODE_MSA)) &&
         (tm_1x_up_session_info.mode_info.err_count_fix_msa < TM_1X_UP_ERROR_THRESHOLD_MSA) )
    {
      return TRUE;
    }
    return FALSE;
}


/*===========================================================================

FUNCTION tm_1x_up_send_ref_position

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_1x_up_send_ref_position(void)
{
  tm_post_data_payload_type *aiding_data_ptr = NULL;
  pdsm_pd_event_type        pd_event;

  MSG_MED("Send Ref position",0,0,0);
  if(ref_position_ptr == NULL)
  {
    MSG_ERROR("Ref_position is unavailable",0,0,0);
    return FALSE;
  }

  if((ref_position_ptr->latitude == 0) ||
     (ref_position_ptr->longitude == 0))
  {
    MSG_ERROR("Either lat or long is zero",0,0,0);
    return FALSE;
  }

  aiding_data_ptr = (tm_post_data_payload_type *)pdsm_getbuf(sizeof(tm_post_data_payload_type));
  if(aiding_data_ptr == NULL) 
  {
    MSG_ERROR("No buffer for aiding data",0,0,0);
    return FALSE;
  }


  tm_1x_up_dload_info.num_data = 1;
  tm_1x_up_dload_info.dload_info[0].data   = PDSM_PD_DLOAD_LOCATION;
  tm_1x_up_dload_info.dload_info[0].source = PDSM_PD_DLOAD_SOURCE_PDE;

  tm_1x_up_pos_info.position_mode = PDSM_PD_POSITION_MODE_MSBASED;
  tm_1x_up_pos_info.position_type = PDSM_PD_POSITION_TYPE_NEW;
  tm_1x_up_pos_info.positioning_source = PDSM_PD_LOCATION_SOURCE_HYBRID;
  tm_1x_up_pos_info.gps_utc_offset = 0;

  tm_1x_up_ext_pos_data.ext_lat = (ref_position_ptr->latitude) * DEG2RAD / LAT_LONG_SCALE;
  tm_1x_up_ext_pos_data.ext_lon = (ref_position_ptr->longitude) * DEG2RAD / LAT_LONG_SCALE;

  /* Range check uncertainty values and use table to convert value to meters */
  if (ref_position_ptr->loc_uncrtnty_a >= MAX_POS_UNC_IND)
  {
    ref_position_ptr->loc_uncrtnty_a = MAX_POS_UNC_IND-1;
  }
  tm_1x_up_ext_pos_data.ext_loc_uncrtnty_a = f_PosUncArray[ref_position_ptr->loc_uncrtnty_a];
  if (ref_position_ptr->loc_uncrtnty_p >= MAX_POS_UNC_IND)
  {
    ref_position_ptr->loc_uncrtnty_p = MAX_POS_UNC_IND-1;
  }
  tm_1x_up_ext_pos_data.ext_loc_uncrtnty_p = f_PosUncArray[ref_position_ptr->loc_uncrtnty_p];
      
  if(ref_position_ptr->velocity_incl)
  {
    tm_1x_up_ext_pos_data.ext_vel_horiz = ref_position_ptr->velocity_hor;
    tm_1x_up_ext_pos_data.ext_vel_vert = ref_position_ptr->velocity_ver;
    /* Can not get Horiz and vert unc from IS-801 */
    tm_1x_up_ext_pos_data.ext_vel_horiz_unc = 0; 
    tm_1x_up_ext_pos_data.ext_vel_vert_unc = 0;
  }

  pd_event = is801_util_pd_info_data_init( &tm_1x_up_info_buffer,
                                            ref_position_ptr, 
                                            tm_1x_up_session_info.api_param.pd_api_info.client_id, 
                                           &tm_1x_up_dload_info, 
                                           &tm_1x_up_pos_info, 
                                           &tm_1x_up_ext_pos_data );

  qw_equ(tm_1x_up_session_info.lr_timestamp,ref_position_ptr->timestamp);
  /* clear the num_data for next fix.
  */
  tm_1x_up_dload_info.num_data = 0;

  (void)tm_post_pd_event( TM_PRTL_TYPE_1X_UP,
                          tm_1x_up_session_info.sess_handle,
                          pd_event,
                          &tm_1x_up_info_buffer,
                          NULL);

  if(TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable))
  {
    /*If working under SUPL, let SUPL know that pos
      is reported*/
    tm_umts_up_supl_is801_pos_reported();
  }

  /* Convert from IS801 to LM format */
  (void)is801_util_flink_convert_seed(ref_position_ptr,
                         &aiding_data_ptr->seed_data);
  MSG_MED("Send LR data to TM-Core", 0, 0, 0);
  if(tm_post_data(TM_PRTL_TYPE_1X_UP,
                    tm_1x_up_session_info.sess_handle,
                    TM_POST_DATA_TYPE_LR,
                    aiding_data_ptr) == FALSE)
  {
    MSG_ERROR("Post data for LR failed",0,0,0);
    (void)pdsm_freebuf((char *)aiding_data_ptr);
    return FALSE;
  }

  (void)pdsm_freebuf((char *)aiding_data_ptr);

  return TRUE;
}


/*===========================================================================

FUNCTION tm_1x_up_send_ref_position_as_ext_pos

DESCRIPTION : Sends the reference position from the network as an intermediate 
              position

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_1x_up_send_ref_position_as_ext_pos(void)
{

  MSG_MED(  "Send Ref position",0,0,0);
  if(ref_position_ptr != NULL)
  {
    if((ref_position_ptr->latitude != 0) &&
       (ref_position_ptr->longitude != 0))
    {
      tm_1x_up_pos_info.position_mode = PDSM_PD_POSITION_MODE_UNKNOWN;
      tm_1x_up_pos_info.position_type = PDSM_PD_POSITION_TYPE_NEW;
      tm_1x_up_pos_info.positioning_source = PDSM_PD_POSITION_SOURCE_DEFAULT;
      tm_1x_up_pos_info.gps_utc_offset = 0;

      tm_1x_up_ext_pos_data.ext_lat = (ref_position_ptr->latitude) * DEG2RAD / LAT_LONG_SCALE;
      tm_1x_up_ext_pos_data.ext_lon = (ref_position_ptr->longitude) * DEG2RAD / LAT_LONG_SCALE;

      /* Range check uncertainty values and use table to convert value to meters */
      if (ref_position_ptr->loc_uncrtnty_a >= MAX_POS_UNC_IND)
      {
        ref_position_ptr->loc_uncrtnty_a = MAX_POS_UNC_IND-1;
      }
      tm_1x_up_ext_pos_data.ext_loc_uncrtnty_a = f_PosUncArray[ref_position_ptr->loc_uncrtnty_a];
      if (ref_position_ptr->loc_uncrtnty_p >= MAX_POS_UNC_IND)
      {
        ref_position_ptr->loc_uncrtnty_p = MAX_POS_UNC_IND-1;
      }
      tm_1x_up_ext_pos_data.ext_loc_uncrtnty_p = f_PosUncArray[ref_position_ptr->loc_uncrtnty_p];

      if(ref_position_ptr->velocity_incl)
      {
        tm_1x_up_ext_pos_data.ext_vel_horiz = ref_position_ptr->velocity_hor;
        tm_1x_up_ext_pos_data.ext_vel_vert = ref_position_ptr->velocity_ver;
        /* Can not get Horiz and vert unc from IS-801 */
        tm_1x_up_ext_pos_data.ext_vel_horiz_unc = 0; 
        tm_1x_up_ext_pos_data.ext_vel_vert_unc = 0;
      }

      (void) is801_util_pd_info_data_init( &tm_1x_up_info_buffer,
                                            ref_position_ptr, 
                                            tm_1x_up_session_info.api_param.pd_api_info.client_id, 
                                            &tm_1x_up_dload_info, 
                                            &tm_1x_up_pos_info, 
                                            &tm_1x_up_ext_pos_data );

      qw_equ(tm_1x_up_session_info.lr_timestamp,ref_position_ptr->timestamp);
      /* clear the num_data for next fix.
      */
      tm_1x_up_dload_info.num_data = 0;

      tm_post_ext_status_ref_loc_rep_event( 
        TM_PRTL_TYPE_1X_UP,
                 tm_1x_up_session_info.sess_handle,
                  &tm_1x_up_info_buffer.pd_info.pd_data);

      return TRUE;
    }
  }
  return FALSE;
}


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_fix_timeout

DESCRIPTION   This function handles MS-B Fix timeout.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_handle_fix_timeout(void)
{
  if (tm_1x_up_is801_drv_session_active())
  {
    MSG_MED("Handle MSB fix timeout",0,0,0);
    if(tm_1x_up_session_info.mode_info.waiting_msb_lr)
    {
      tm_1x_up_session_info.mode_info.waiting_msb_lr = FALSE;
      tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_MSB;

      if(tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_DLOAD)
      {
        MSG_MED("Wait for download",0,0,0);
        /* For optimal modes, try MS-Assisted fix if allowed.
        */
        if(tm_1x_up_is801_drv_msa_fix_allowed() == TRUE)
        {
          tm_1x_up_session_info.mode_info.pending_fallback_msa_fix = TRUE;
         }
      }
      else
      {
        if(tm_1x_up_session_info.api_param.pd_api_info.pd_option.operation_mode == PDSM_SESSION_OPERATION_REF_POSITION)
        {
          if(tm_1x_up_send_ref_position())
          {
            tm_1x_up_is801_drv_opn_completed(PDSM_PD_END_SESS_NONE);
            return;
          }
        }
        tm_1x_up_is801_drv_opn_completed(PDSM_PD_END_TIMEOUT);
      }
    }
  }
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_stop_sess_req

DESCRIPTION   This function handles stop session from TM-Core.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_is801_drv_handle_stop_sess_req(tm_sess_stop_reason_e_type stop_reason)
{
  boolean ret_val = FALSE;

  pdsm_pd_end_e_type end_status = PDSM_PD_END_SESS_NONE;

	MSG_HIGH("Handle stop session rcvd from TM-Core. Reason %d",stop_reason,0,0);

 
#ifdef FEATURE_JCDMA_GPSONE
  if( (tm_1x_up_is801_drv_is_optimal_mode()) &&
      (((stop_reason >= TM_STOP_REASON_JGPS_ABORT_SESS) &&
        (stop_reason <= TM_STOP_REASON_JGPS_TIMEOUT_STOPACQ)) ||
        (stop_reason == TM_STOP_REASON_JGPS_DMSB_ERROR) ||
        (stop_reason == TM_STOP_REASON_JGPS_DPOSCOMP_NG)))
  {
    /* For JGPS KDDI module, for optimal modes, for MS-B fix failures, force fallback to MS-A  */
    if((tm_1x_up_session_info.mode_info.waiting_msb_lr == TRUE) &&
       (tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_MSB))
    {
      tm_1x_up_is801_drv_handle_fix_timeout();
    }
    else if(tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_MSA)
    {
      tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_MSA;
      end_status = tm_core_translate_end_status(stop_reason);
      tm_1x_up_is801_drv_opn_completed(end_status);
    }
    return ret_val;
  }
#endif /* FEATURE_JCDMA_GPSONE */

  if(TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable))
  {
    if(tm_1x_up_is801_drv_is_optimal_mode())
    {    
      tm_1x_up_session_info.mode_info.last_error = tm_1x_up_session_info.mode_info.mode; 
    }
    
    if(stop_reason == TM_STOP_REASON_GENERAL_ERROR)
    {
      end_status = tm_core_translate_end_status(stop_reason);
      tm_1x_up_is801_drv_opn_completed(end_status);
      return TRUE;
    }
  }

  switch(stop_reason)
  {
    case TM_STOP_REASON_PRM_TIMEOUT:
        tm_1x_up_is801_drv_handle_prm_timeout();
        break;

    case TM_STOP_REASON_FIX_TIMEOUT:
        (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_sess_timer);
        tm_1x_up_is801_drv_handle_fix_timeout();
        break;

    default: 
        tm_1x_up_is801_drv_handle_stop_session(stop_reason);
        ret_val = TRUE;
        break;
  }

  return ret_val;
} /* End tm_1x_up_is801_drv_handle_stop_sess_req() */

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_send_dload_start_event

DESCRIPTION

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_send_dload_start_event(void)
{
    uint8                      data_type = 0;
    if ( tm_1x_up_session_info.mode_info.req_to_send & TM_1X_UP_RLINK_REQ_GPS_EPH )  /*lint !e641 */
    {
      data_type |= EPHEMERIS_DL;  /*lint !e641 */
    }
    if ( tm_1x_up_session_info.mode_info.req_to_send & TM_1X_UP_RLINK_REQ_GPS_ALM )  /*lint !e641 */
    {
      data_type |= ALMANAC_DL;  /*lint !e641 */
    }
    if ( tm_1x_up_session_info.mode_info.req_to_send & TM_1X_UP_RLINK_REQ_LR )  /*lint !e641 */
    {
      data_type |= LOCATION_DL;  /*lint !e641 */
    }
    sm_report_event_gps_pd_download_start(data_type, 0xFFFFFFFF);
}

/*===========================================================================

FUNCTION connect_to_pde

DESCRIPTION
RETURN VALUE
  Boolean

DEPENDENCIES

===========================================================================*/
static boolean connect_to_pde(tm_1x_up_mode_e_type mode)
{
  tm_cm_iface_cmd_info_s_type cm_cmd_info;
  boolean ret_val = TRUE;

  if ( tm_1x_up_comm_connect_with_pde() == FALSE )
  {
    /* Fire COMM_FAILURE event */
    tm_1x_up_info_buffer.client_id = tm_1x_up_session_info.api_param.pd_api_info.client_id;
    tm_1x_up_info_buffer.pd_info.end_status = PDSM_PD_END_NO_CON;
    (void)tm_post_pd_event( TM_PRTL_TYPE_1X_UP,
                            tm_1x_up_session_info.sess_handle,
                            PDSM_PD_EVENT_COMM_FAILURE,
                            &tm_1x_up_info_buffer,
                            NULL);

    tm_1x_up_session_info.mode_info.last_error = mode;
    tm_1x_up_is801_drv_opn_completed(PDSM_PD_END_NO_CON);
    ret_val = FALSE;
  }
  else 
  {
    if (tm_1x_up_session_info.mgp_info.ppm_on == TRUE || mode == TM_1X_UP_MODE_MSA) 
    {
      /* For AFLT, wakeup 1X */
      cm_cmd_info.cmd_type = TM_CM_CMD_WAKEUP_CDMA;
      (void)tm_cm_iface_send_cmd(&cm_cmd_info);
    }

    /* Start recovery timer. 
    */
    if(!os_TimerStart(tm_1x_up_session_info.tm_1x_up_connect_timer,TM_1X_UP_CONNECT_TIMEOUT,NULL))
    {
      MSG_ERROR("Failed starting Connect timer. mode = %d", mode,0,0);
    }
  } /* End else if waiting for connection */
  return ret_val;
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_initiate_session

DESCRIPTION
  This function is used to initiate a session or continue with a tracking session.
  If MS is already connected with PDE then PPM results are requested in MSA mode
  or MGP is started in MSB mode.

RETURN VALUE
  Boolean

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_is801_drv_initiate_session(void)
{
  boolean                     ret_val = FALSE;

  MSG_HIGH("New ses,mode=%d",tm_1x_up_session_info.mode_info.mode,0,0);

  tm_1x_up_is801_drv_update_session_state(TM_1X_UP_SESSION_STATE_ENTRY);

  MSG_HIGH("New session initiated",0,0,0);

  if (tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_MSA)
  {
    MSG_HIGH("PDE needs to be contacted,open con",0,0,0);

    if(TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable))
    {
      if(tm_1x_up_session_info.api_param.session_type == TM_1X_UP_SESSION_TYPE_AFLT)
      {
        tm_umts_up_supl_is801_inform_1x_pos_method(C_SUPL_AFLT_MODE);
      }
      else
      {
        tm_umts_up_supl_is801_inform_1x_pos_method(C_SUPL_AGPS_MODE_UE_ASSISTED);
      }
    }

    /* For TM-Core sess_start, do not start PPM here, return action = REQ_PPM */
    if(tm_1x_up_session_info.tm_core_initiate_session == FALSE)
    {
      /* start PPM for MS-Assisted session.
      */
      if (tm_1x_up_session_info.mgp_info.ppm_on == FALSE) 
      {
        if( !tm_1x_up_is801_drv_start_ppm() )
        {
          MSG_ERROR("Unable to request TM-Core to start PPM",0,0,0);
          tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
          return FALSE;
        }
      }
    }

    ret_val = connect_to_pde(TM_1X_UP_MODE_MSA);

    return ret_val;
  } /* MS-Assisted */
  else if (tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_DLOAD)
  {
  
    if(TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable))
    {
      tm_umts_up_supl_is801_inform_1x_pos_method(C_SUPL_AGPS_MODE_UE_BASED);
    }
    
    if (tm_1x_up_session_info.dload_activated == FALSE)
    {
      tm_1x_up_session_info.dload_activated = TRUE;
      /* Fire Dload start event here.
      */
      tm_1x_up_is801_drv_send_dload_start_event();
    }

    /* Request LR from MGP if its not DLOAD mode and AFLT for Dload is not requested */
    if(tm_1x_up_session_info.api_param.pd_api_info.pd_option.session != PDSM_PD_SESS_TYPE_DATA_DLOAD)
    {
      /* In case of AFLT during Dload, GPS measurements are not started here, they will be started once Dload is over
      */
      if(tm_1x_up_session_info.mode_info.resp_to_send & IS801_REV_LINK_RESP_MASK_PPM)
      {
        tm_1x_up_session_info.mode_info.waiting_msb_lr = TRUE;
      }
      else
      {
        if(tm_1x_up_is801_drv_continue_msb_fix(DLOAD_START) == FALSE)
        {
          tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
          return FALSE;
        }
      }
    }

    MSG_HIGH("PDE needs to be contacted",0,0,0);

    /* Before opening connection, check if MSB throttling wants to throttle the connection
    ** at this time or not.
    */
    if (tm_1x_up_is801_drv_check_msb_throttling(FALSE) == TRUE) 
    {
      MSG_HIGH("Throttle this connection to PDE",0,0,0);
      tm_1x_up_is801_drv_opn_completed(PDSM_PD_END_NO_CON); 
      return TRUE;
    }
    else
    {
      MSG_MED("No throttling necessary...",0,0,0);
    }
    
    if (tm_1x_up_session_info.mode_info.resp_to_send & IS801_REV_LINK_RESP_MASK_PPM)
    {
      /* Now start PPM because Dload resp needs it.
      */
      if (tm_1x_up_session_info.mgp_info.ppm_on == FALSE) 
      {
        if( !tm_1x_up_is801_drv_start_ppm() )
        {
          MSG_ERROR("Unable to request TM-Core to start PPM",0,0,0);
          tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
          return FALSE;
        }
      }
    }

    (void) connect_to_pde(TM_1X_UP_MODE_DLOAD);
    return TRUE;
  }
  else if(tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_MSB)
  {
    MSG_HIGH("Close conection if Active...",0,0,0);
    
    if(TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable))
    {
      tm_umts_up_supl_is801_inform_1x_pos_method(C_SUPL_AGPS_MODE_UE_BASED);
    }

    tm_1x_up_comm_close_connection();

    /* Request LR */
    if(tm_1x_up_is801_drv_continue_msb_fix(MSB_START) == FALSE)
    {
      tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
      return FALSE;
    }
  } /* End else if MODE_MSB */
  else
  {
    MSG_HIGH("Error mode=%d",tm_1x_up_session_info.mode_info.mode,0,0);
    tm_1x_up_comm_close_connection();
    if (tm_1x_up_is801_drv_session_active() == FALSE)
    {
      tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
      return FALSE;
    }
  }
  return TRUE;
} /* End tm_1x_up_is801_drv_initiate_session() */

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_start_fallback_msa_fix

DESCRIPTION

RETURN VALUE
  pdsm_pd_end_e_type

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_start_fallback_msa_fix(void)
{
  boolean  ret_val = TRUE;

  MSG_MED("Start fallback MSA fix",0,0,0);
  tm_1x_up_session_info.mode_info.pending_fallback_msa_fix = FALSE;
  tm_1x_up_session_info.mode_info.mode = TM_1X_UP_MODE_MSA;
  /* Check if force dormancy is required or not.
  */
#ifdef FEATURE_GPSONE_UPDATE_BSID
  tm_1x_up_is801_drv_init_bs_info();
#endif

  tm_1x_up_session_info.tm_core_initiate_session = FALSE;
  ret_val = tm_1x_up_is801_drv_initiate_session();

  if (ret_val == FALSE)
  {
    MSG_ERROR("Exit New session not allowed",0,0,0);
    tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
  }
  return;
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_start_session_wishlist_update

DESCRIPTION

RETURN VALUE
  boolean

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_is801_drv_start_session_wishlist_update(void)
{
  boolean ret_val = TRUE;

  if(tm_1x_up_session_info.api_param.pd_api_info.pd_option.operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_DATA ||
     tm_1x_up_session_info.api_param.pd_api_info.pd_option.operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_SPEED)
  {
    
    if(tm_1x_up_session_info.session_state >TM_1X_UP_SESSION_STATE_INIT)
    {
      /* ignore the wishlist update as we just sent an wishlist request to PE to make
      sure PE goes into MSB. we have arleady started the MSA session */
      return ret_val;
    }
  }
  ret_val = tm_1x_up_is801_drv_set_mode();

  if(tm_1x_up_session_info.mode_info.mode != TM_1X_UP_MODE_NONE)
  {
      if(ret_val)
      {
        /* Check if force dormancy is required or not.
        */
  #ifdef FEATURE_GPSONE_UPDATE_BSID
        tm_1x_up_is801_drv_init_bs_info();
  #endif
        tm_1x_up_session_info.tm_core_initiate_session = FALSE;
        ret_val = tm_1x_up_is801_drv_initiate_session();
      } /* end set_state end_event */

      if (ret_val == FALSE)
      {
        MSG_ERROR("Exit New session not allowed",0,0,0);
        tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
      }
      return ret_val;
  }

  return ret_val;
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_comm_error

DESCRIPTION
  This function is called when there is an Error during communcation w/ PDE. 
  
RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_handle_comm_error(void)
{
  if( tm_1x_up_is801_drv_session_active() )
  {
    /* Fire COMM_FAILURE event */
    tm_1x_up_info_buffer.client_id = tm_1x_up_session_info.api_param.pd_api_info.client_id;
    tm_1x_up_info_buffer.pd_info.end_status = PDSM_PD_END_CON_FAIL;
    (void)tm_post_pd_event( TM_PRTL_TYPE_1X_UP,
                            tm_1x_up_session_info.sess_handle,
                            PDSM_PD_EVENT_COMM_FAILURE,
                            &tm_1x_up_info_buffer,
                            NULL);

    if (tm_1x_up_session_info.session_state == TM_1X_UP_SESSION_STATE_WAIT_FD)
    {
      tm_1x_up_is801_drv_handle_dormancy_failure(TM_1X_UP_DORMANCY_FAILURE_TYPE_OPN_COMPLETE);
    }  
    else
    {         
      if (tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_MSB)
      {
        MSG_ERROR("Transport error..Close con..",0,0,0);
        tm_1x_up_comm_close_connection();
        return;
      }
      else if (tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_MSA)
      {
        tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_MSA;
      }
      else if (tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_DLOAD)
      {
        tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_DLOAD;
      }
      tm_1x_up_is801_drv_opn_completed(PDSM_PD_END_CON_FAIL);
    }
  }
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_report_lr_resp_msb

DESCRIPTION
  This function is used to handle PL response from PE.

RETURN VALUE
  Boolean

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_is801_drv_report_lr_resp_msb(sm_GnssFixRptStructType* lr_ptr)
{
    pdsm_pd_event_type  pd_event;
    is801_lr_resp_type *lr_resp_ptr = NULL;
    tm_1x_up_smod_req_status_e_type    req_status;
	utc_time z_utc_time;

    if(lr_ptr == NULL)
    {
      MSG_ERROR("Null lr_ptr",0,0,0);
      return FALSE;
    }
    /* Generate CGPS event */
    sm_log_event_report(SM_LOG_EVENT_ID_GPS_COMPLETE,0);

    lr_resp_ptr = (is801_lr_resp_type *)pdsm_getbuf(sizeof(is801_lr_resp_type));

    if(lr_resp_ptr != NULL)
    {
      //Convert from LM to IS801 format
      if(is801_util_convert_tm_lr_to_is801_lr(lr_ptr,lr_resp_ptr))
      {
          tm_1x_up_dload_info.num_data = 1;
          tm_1x_up_dload_info.dload_info[0].data   = PDSM_PD_DLOAD_LOCATION;
          tm_1x_up_dload_info.dload_info[0].source = PDSM_PD_DLOAD_SOURCE_MS;

          tm_1x_up_pos_info.position_mode = PDSM_PD_POSITION_MODE_MSBASED;

          tm_util_xlate_nav_soln_to_tm_pos_info( NULL,
                                                 &tm_1x_up_pos_info,
                                                 &lr_ptr->z_NavPos,
                                                 PDSM_PD_DLOAD_LOCATION,         /* dont care */
                                                 PDSM_PD_DLOAD_SOURCE_STANDALONE /* dont care */ );

          tm_1x_up_ext_pos_data.ext_lat = lr_ptr->z_NavPos.d_PosLla[0];
          tm_1x_up_ext_pos_data.ext_lon = lr_ptr->z_NavPos.d_PosLla[1];
          tm_1x_up_ext_pos_data.ext_loc_uncrtnty_a = lr_ptr->z_NavPos.f_ErrorEllipse[1];
          tm_1x_up_ext_pos_data.ext_loc_uncrtnty_p = lr_ptr->z_NavPos.f_ErrorEllipse[2];
          tm_1x_up_ext_pos_data.ext_vel_horiz = sqrt(lr_ptr->z_NavPos.f_VelEnu[0] * 
                                                     lr_ptr->z_NavPos.f_VelEnu[0] +
                                                     lr_ptr->z_NavPos.f_VelEnu[1] * 
                                                     lr_ptr->z_NavPos.f_VelEnu[1]);
          tm_1x_up_ext_pos_data.ext_vel_vert = lr_ptr->z_NavPos.f_VelEnu[2];

          /* calculate horiz and vert velocity unc */
          tm_1x_up_ext_pos_data.ext_vel_horiz_unc = 
            tm_util_calc_vel_horiz_uncertainty(&(lr_ptr->z_NavPos)); 
          tm_1x_up_ext_pos_data.ext_vel_vert_unc = 
            tm_util_calc_vel_vert_uncertainty(&(lr_ptr->z_NavPos)); 
		  
		  tm_1x_up_ext_pos_data.h_dop = lr_ptr->z_NavPos.f_HDOP;
          tm_1x_up_ext_pos_data.v_dop = lr_ptr->z_NavPos.f_VDOP;
          tm_1x_up_ext_pos_data.p_dop = lr_ptr->z_NavPos.f_PDOP;
          tm_1x_up_ext_pos_data.pos_hepe = 
             sqrt((lr_ptr->z_NavPos.f_ErrorEllipse[1] * 
                   lr_ptr->z_NavPos.f_ErrorEllipse[1]) + 
                  (lr_ptr->z_NavPos.f_ErrorEllipse[2] * 
                   lr_ptr->z_NavPos.f_ErrorEllipse[2]));

          z_utc_time = tm_util_construct_utc_time(lr_ptr->z_NavPos.w_GpsWeek,
                                                  lr_ptr->z_NavPos.q_GpsTimeMs,
                                                  lr_ptr->z_NavPos.q_UtcTimeMs);
          tm_1x_up_ext_pos_data.utc_time.hour = z_utc_time.q_hours;
          tm_1x_up_ext_pos_data.utc_time.minute = z_utc_time.q_mins;
          tm_1x_up_ext_pos_data.utc_time.sec = (uint16)((z_utc_time.q_secs * 100) + (z_utc_time.q_tenths * 10));

          tm_1x_up_ext_pos_data.utc_time.date = (uint32) ((z_utc_time.q_utc_days * 1000000) + (z_utc_time.q_month * 10000) + z_utc_time.q_year);
          pd_event = is801_util_pd_info_data_init( &tm_1x_up_info_buffer,
                                                    lr_resp_ptr, 
                                                    tm_1x_up_session_info.api_param.pd_api_info.client_id, 
                                                   &tm_1x_up_dload_info, 
                                                   &tm_1x_up_pos_info, 
                                                   &tm_1x_up_ext_pos_data );

          qw_equ(tm_1x_up_session_info.lr_timestamp,lr_resp_ptr->timestamp);
          /* clear the num_data for next fix.
          */
          tm_1x_up_dload_info.num_data = 0;

          /* Query SMOD iface to determine if 1x UP can post event to TM-Core */
          if( TM_1X_UP_SMOD_REQ_SUCCESS == 
                         tm_1x_up_smod_info_available(&tm_1x_up_info_buffer,
                                                      PDSM_PD_END_SESS_NONE,
                                                      NULL,
                                                      &req_status)
            )
          {
            /* Post position event to TM-Core */
            (void)tm_post_pd_event( TM_PRTL_TYPE_1X_UP,
                                    tm_1x_up_session_info.sess_handle,
                                    pd_event, 
                                    &tm_1x_up_info_buffer,
                                    &lr_ptr->z_NavPos.z_PosFlags);

            if(TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable))
            {
              /*If working under SUPL, let SUPL know that pos
                is reported*/
              tm_umts_up_supl_is801_pos_reported();
            }
          }
      }
      (void)pdsm_freebuf((char *)lr_resp_ptr);
    }
    else
    {
      MSG_ERROR("No buffer for lr_resp_ptr",0,0,0);
      return FALSE;
    }
    return TRUE;
}


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_v2_lr_resp_msbased

DESCRIPTION
  This function is used to handle LR response from PE for a V2 session. This
  function is called by Vx module if a position was received before the MPC connection
  was up for MSB.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

boolean tm_1x_up_is801_drv_handle_v2_lr_resp_msbased(sm_GnssFixRptStructType* lr_ptr)
{
  dword lr_validity = PDSM_NO_ACCURACY_THRESHOLD;

  MSG_HIGH("Processing LR resp from PE",0,0,0);


  tm_1x_up_session_info.mode_info.waiting_msb_lr = FALSE;
  (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_sess_timer);
  if (lr_ptr != NULL)
  {
    lr_validity = (uint32)lr_ptr->z_NavPos.f_PuncLatLonMeters;
    MSG_HIGH("Fix accuracy=%dm",lr_validity,0,0);
    tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_NONE;
  }
  else
  {
    MSG_ERROR("Null Loc from PE",0,0,0);
    tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_MSB;

    return TRUE;
  } /* else NULL location from PE */


  MSG_MED("Good fix, proceed with session",0,0,0);
  /* Now that Gps info is available inform TM-Core about it.
  */
  (void)tm_1x_up_is801_drv_report_lr_resp_msb(lr_ptr);

  tm_1x_up_is801_drv_opn_completed(PDSM_PD_END_SESS_NONE);

  return TRUE;
} /* End tm_1x_up_is801_drv_handle_v2_lr_resp_msbased() */

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_lr_resp_msbased

DESCRIPTION
  This function is used to handle LR response from PE.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

boolean tm_1x_up_is801_drv_handle_lr_resp_msbased(sm_GnssFixRptStructType* lr_ptr)
{
  dword lr_validity = PDSM_NO_ACCURACY_THRESHOLD;

  MSG_HIGH("Processing LR resp from PE",0,0,0);

  if((tm_1x_up_is801_drv_session_active() != TRUE) || (tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_MSA))
  {
    MSG_ERROR("LR resp from PE not expected in mode %d",tm_1x_up_session_info.mode_info.mode,0,0);
    return FALSE;
  }

  tm_1x_up_session_info.mode_info.waiting_msb_lr = FALSE;
  (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_sess_timer);
  if (lr_ptr != NULL)
  {
    lr_validity = (uint32)lr_ptr->z_NavPos.f_PuncLatLonMeters;
    MSG_HIGH("Fix accuracy=%dm",lr_validity,0,0);
    tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_NONE;
  }
  else
  {
    MSG_ERROR("Null Loc from PE",0,0,0);
    tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_MSB;

    if(tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_DLOAD)
    {
      MSG_MED("Wait for download",0,0,0);
    }
    else
    {
      tm_1x_up_is801_drv_opn_completed(PDSM_PD_END_FIX_ERROR);
    }
    return TRUE;
  } /* else NULL location from PE */


  /* Check position uncertainity and redo the fix if required.
  */
  if (lr_validity > tm_1x_up_session_info.api_param.pd_api_info.pd_qos.accuracy_threshold)
  {
    MSG_HIGH("Fix above set threshold",0,0,0);

    /* For optimal modes, try MS-Assisted fix if allowed.
    */
    if(tm_1x_up_is801_drv_msa_fix_allowed() == TRUE)
    {
      /* Check if Dload is in progress. Wait to fallback only after Dload finishes */
      if(tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_DLOAD)
      {
        MSG_MED("Wait for download to finish before falling back to MSA",0,0,0);
        tm_1x_up_session_info.mode_info.pending_fallback_msa_fix = TRUE;
      }
      else
      {
        /* Last error allows MS-Assited session, so change the mode.
        */
        tm_1x_up_is801_drv_start_fallback_msa_fix();
        return TRUE;
      }
    } /* end if */
    else
    {
      MSG_HIGH("Mode change not alwd,report fix to TM-Core",0,0,0);
      (void)tm_1x_up_is801_drv_report_lr_resp_msb(lr_ptr);

      if(tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_DLOAD)
      {
        MSG_MED("Wait for download",0,0,0);
      }
      else
      {
        tm_1x_up_is801_drv_opn_completed(PDSM_PD_END_SESS_NONE);
      }
    }
  } /* end if lr_validity */
  else
  {
    MSG_MED("Good fix, proceed with session",0,0,0);
    /* Now that Gps info is available inform TM-Core about it.
    */
    (void)tm_1x_up_is801_drv_report_lr_resp_msb(lr_ptr);

    if(tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_DLOAD)
    {
      MSG_MED("Wait for download",0,0,0);
    }
    else
    {
      tm_1x_up_is801_drv_opn_completed(PDSM_PD_END_SESS_NONE);
    }
  }
  return TRUE;
} /* End tm_1x_up_is801_drv_handle_lr_resp_msbased() */


/*===========================================================================

FUNCTION xlate_is801_b_flink_resp_to_1x_up_req_type

DESCRIPTION
  This function is used to xlate IS801B flink responses to 1X UP state machine
  requests, that these responses correspond to.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

tm_1x_up_rlink_req_type xlate_is801_b_flink_resp_to_1x_up_req_type
(
  is801B_fwd_link_resp_mask_type in_resp
)
{
  switch(in_resp)
  {
    case(IS801B_FWD_LINK_RESP_MASK_GPS_EPH):
      return (TM_1X_UP_RLINK_REQ_GPS_EPH);

    case(IS801B_FWD_LINK_RESP_MASK_GPS_ALM):
      return TM_1X_UP_RLINK_REQ_GPS_ALM;

    case(IS801B_FWD_LINK_RESP_MASK_GLO_EPH):
      return TM_1X_UP_RLINK_REQ_GLO_EPH;

    case(IS801B_FWD_LINK_RESP_MASK_GLO_ALM):
      return TM_1X_UP_RLINK_REQ_GLO_ALM;

    case(IS801B_FWD_LINK_RESP_MASK_GPS_SA):
      return TM_1X_UP_RLINK_REQ_GPS_SA;

    case(IS801B_FWD_LINK_RESP_MASK_GNSS_AA):
      return TM_1X_UP_RLINK_REQ_AA;

    case(IS801B_FWD_LINK_RESP_MASK_ADV_LR):
      return TM_1X_UP_RLINK_REQ_LR;

    case(IS801B_FWD_LINK_RESP_MASK_ADV_SVHI):
      return TM_1X_UP_RLINK_REQ_ADV_SVHI;

    case(IS801B_FWD_LINK_RESP_MASK_DGNSS_ASST):
      return TM_1X_UP_RLINK_REQ_DGNSS_ASST;

    case(IS801B_FWD_LINK_RESP_MASK_BSC):
      return TM_1X_UP_RLINK_REQ_ADV_BSC;

    case(IS801B_FWD_LINK_RESP_MASK_GGTO):
      return TM_1X_UP_RLINK_REQ_GGTO;

    default:
      return TM_1X_UP_RLINK_REQ_NONE;
  }
}

/*===========================================================================

FUNCTION xlate_is801_1_flink_resp_to_1x_up_req_type

DESCRIPTION
  This function is used to xlate IS801-1 flink responses to 1X UP state machine
  requests, that these responses correspond to.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
tm_1x_up_rlink_req_type xlate_is801_1_flink_resp_to_1x_up_req_type
(
  is801_fwd_link_resp_mask_type in_resp
)
{
  switch(in_resp)
  {
    case(IS801_FWD_LINK_RESP_MASK_EPH):
      return (TM_1X_UP_RLINK_REQ_GPS_EPH);

    case(IS801_FWD_LINK_RESP_MASK_ALM):
      return (TM_1X_UP_RLINK_REQ_GPS_ALM);

    case(IS801_FWD_LINK_RESP_MASK_SA):
      return (TM_1X_UP_RLINK_REQ_GPS_SA);

    case(IS801_FWD_LINK_RESP_MASK_AA):
      return (TM_1X_UP_RLINK_REQ_AA);

    case(IS801_FWD_LINK_RESP_MASK_LR):
      return (TM_1X_UP_RLINK_REQ_LR);

    default:
      return TM_1X_UP_RLINK_REQ_NONE;
  }
}

/*===========================================================================

FUNCTION xlate_1x_up_to_is801_1_req_type

DESCRIPTION
  This function is used to xlate 1X UP state machine requests to IS801-1 
  reverse link requests.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
is801_rev_link_req_mask_type xlate_1x_up_to_is801_1_req_type
(
  tm_1x_up_rlink_req_type      in_req
)
{
  switch(in_req)
  {
    case(TM_1X_UP_RLINK_REQ_GPS_EPH):
      return IS801_REV_LINK_REQ_MASK_EPH;

    case(TM_1X_UP_RLINK_REQ_GPS_ALM):
      return IS801_REV_LINK_REQ_MASK_ALM;

    case(TM_1X_UP_RLINK_REQ_GPS_SA):
      return IS801_REV_LINK_REQ_MASK_SA;

    case(TM_1X_UP_RLINK_REQ_AA):
      return IS801_REV_LINK_REQ_MASK_AA;

    case(TM_1X_UP_RLINK_REQ_LR):
      return IS801_REV_LINK_REQ_MASK_LR;

    default:
      return IS801_REV_LINK_REQ_MASK_NONE;
  }
}
   
/*===========================================================================

FUNCTION xlate_1x_up_to_is801_1_req_type

DESCRIPTION
  This function is used to xlate 1X UP state machine requests to IS801-B 
  reverse link requests.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
is801B_rev_link_req_mask_type xlate_1x_up_to_is801_b_req_type
(
  tm_1x_up_rlink_req_type      in_req
)
{
  switch(in_req)
  {
    case(TM_1X_UP_RLINK_REQ_GPS_EPH):
      return IS801B_REV_LINK_REQ_MASK_GPS_EPH;

    case(TM_1X_UP_RLINK_REQ_GPS_ALM):
      return IS801B_REV_LINK_REQ_MASK_GPS_ALM;

    case(TM_1X_UP_RLINK_REQ_GPS_SA):
      return IS801B_REV_LINK_REQ_MASK_GNSS_SA;

    case(TM_1X_UP_RLINK_REQ_AA):
      return IS801B_REV_LINK_REQ_MASK_GNSS_AA;

    case(TM_1X_UP_RLINK_REQ_LR):
      return IS801B_REV_LINK_REQ_MASK_ADV_LR;

    case(TM_1X_UP_RLINK_REQ_GLO_EPH):
      return IS801B_REV_LINK_REQ_MASK_GLO_EPH;

    case(TM_1X_UP_RLINK_REQ_GLO_ALM):
      return IS801B_REV_LINK_REQ_MASK_GLO_ALM;

    case(TM_1X_UP_RLINK_REQ_GGTO):
      return IS801B_REV_LINK_REQ_MASK_GGTO;

    case(TM_1X_UP_RLINK_REQ_ADV_SVHI):
      return IS801B_REV_LINK_REQ_MASK_GNSS_SVHI;

    case(TM_1X_UP_RLINK_REQ_DGNSS_ASST):
      return IS801B_REV_LINK_REQ_MASK_DGNSS_ASST;

    default:
      return IS801B_REV_LINK_REQ_MASK_NONE;
  }
}
   
/*===========================================================================

FUNCTION tm_1x_up_is801_drv_create_rlink_req

DESCRIPTION
  This function is used create IS801 request for MI sessions. The i/p param
  decides which request to be created.

RETURN VALUE
  boolean

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_is801_drv_create_rlink_req
(
  tm_1x_up_rlink_req_type      req,
  boolean                      is_801_1
)
{
  boolean ret_val               = TRUE;

  if(is_801_1)
  {
    is801_rev_link_req_mask_type  is801_1_req;

    is801_1_req = xlate_1x_up_to_is801_1_req_type(req);
  
    ret_val = is801_new_rev_link_req( TM_PRTL_TYPE_1X_UP, is801_1_req);
 
  }
  #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#endif

  if(ret_val)
  {
    /* Set req_wat_bmap, this will be used as a sanity check when we get 
     * responses */
    tm_1x_up_session_info.is801_req_wait_bmap |= (req);
    /* No need to clear the req bit mask, its only used to init what reqs
     * need to be sent to PDE. The reqs themselves may be 801-1 or 801-B
    tm_1x_up_session_info.mode_info.req_to_send &= ~(req); */
    MSG_MED("Created req type 0x%x using %d (1=801-1 / 0=801-B)", 
                 req, is_801_1, 0);
  }
  else
  {
    MSG_ERROR("Create RLink Req 0x%x using %d (1=801-1 / 0=801-B) failed",
                 req, is_801_1, 0);
  }
  return ret_val;
} /* End tm_1x_up_is801_drv_create_rlink_req() */


#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
#error code not present
#endif

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_send_and_log_rlink_pddm

DESCRIPTION

RETURN VALUE
  boolean

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_is801_drv_send_and_log_rlink_pddm(void)
{
    boolean ret_val = TRUE;
    /* TBD Comment out the logging part of this function. Logs need to be moved elsewhere anyway */
#if 0
      is801_util_log_rlink_msg((void *)&(pdcomm_info.rev_pddm.msg_payload[0]), pdcomm_info.rev_pddm.msg_size);

      if(pdcomm_info.rev_pddm.req_mask)
      {
        is801_util_report_is801_event_tx(IS801_REQ,pdcomm_info.rev_pddm.req_mask);
      }
      if(pdcomm_info.rev_pddm.resp_mask)
      {
        is801_util_report_is801_event_tx(IS801_RESP,pdcomm_info.rev_pddm.resp_mask);
      }

    if(pdcomm_info.last_pddm_type == IS801_VERSION_801_1)
    {
    }
    if(pdcomm_info.last_pddm_type == IS801_VERSION_801_B)
    {
      is801_util_log_rlink_msg((void *) pdcomm_info.rev_pddm_is801b, 
                                        pdcomm_info.last_pddm_len);
      /* TBD: figure out how to do TX events for 801b. the problem is, I cant
       * peek into the sent pddm to figure out what req/resps were sent because
       * of use of asn1 encoder. This needs to be done somewhere else. */
    }
#endif
    MSG_MED("Num PDDMs left to be sent= 801-1 = %d 801-B = %d",
             pdcomm_info.num_rev_pddm_801_1,
             pdcomm_info.num_rev_pddm_801_b, 0);

    if(pdcomm_info.num_rev_pddm_801_1 > 0)
    {
      if(i_1x_up_is801_1_drv_send_pddm() == FALSE)
      {
        MSG_ERROR("Could not send RLink PDDM from IS801-1 module",0,0,0);
        tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
        ret_val = FALSE;
      }
    }
#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
    #error code not present
#endif
    return ret_val;
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_create_rlink_pddm

DESCRIPTION

RETURN VALUE
  int

DEPENDENCIES

===========================================================================*/
int tm_1x_up_is801_drv_create_rlink_pddm(byte sess_start, byte sess_end)
{
  is801_pddm_sess_info_s_type     rev_pddm_hdr; 
  int                             ret_val;

  rev_pddm_hdr.sess_start   = sess_start;
  rev_pddm_hdr.sess_end     = sess_end;
  rev_pddm_hdr.sess_source  = 0x1;
  rev_pddm_hdr.sess_tag     = sess_tag;
  rev_pddm_hdr.pd_msg_type  = PD_MSG_TYPE;

  ret_val = is801_create_rev_link_pddm(
               TM_PRTL_TYPE_1X_UP,
               rev_pddm_hdr,
               MAX_IS801_PDDM_SIZE);

  if(sess_end)
  {
    pdcomm_info.wait_for_sess_end_ack = TRUE;
    tm_1x_up_session_info.sess_end_sent = TRUE;
  }

  return ret_val;
}

#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
#error code not present
#endif

/*===========================================================================

FUNCTION    : tm_1x_up_is801_1_get_and_log_rlink_pddm

DESCRIPTION : Internal Function to get ONE is801-1 pddm from the encoder layer
              and log it via diag.

DEPENDENCIES

ARGUMENTS
RETURN VALUE:
SIDE EFFECTS
Gets one is801-1 pddm from 801-1 encoder.
===========================================================================*/
boolean tm_1x_up_is801_1_get_and_log_rlink_pddm
(
  is801_rev_link_pddm_info_s_type *rlink_pddm
)
{
  if(is801_get_rev_link_pddm( TM_PRTL_TYPE_1X_UP, rlink_pddm) == FALSE )
  {
    MSG_ERROR("Can not get RLink PDDM from IS801 module",0,0,0);
    return FALSE;
  }

  is801_util_log_rlink_msg((void *) &(rlink_pddm->msg_payload[0]), 
                                      rlink_pddm->msg_size);

  if(rlink_pddm->req_mask)
  {
    is801_util_report_is801_event_tx(IS801_REQ, rlink_pddm->req_mask);
  }
  if(rlink_pddm->resp_mask)
  {
    is801_util_report_is801_event_tx(IS801_RESP, rlink_pddm->resp_mask);
  }
  return TRUE;
}


/*===========================================================================

FUNCTION    : i_1x_up_is801_1_drv_send_pddm

DESCRIPTION : Internal Function to get ONE is801-1 pddm from the encoder layer
              and send it to tm_1x_up_comm, to be sent to the PDE.

DEPENDENCIES

ARGUMENTS
RETURN VALUE
SIDE EFFECTS
Gets one is801-1 pddm from 801-1 encoder.
===========================================================================*/
boolean i_1x_up_is801_1_drv_send_pddm(void)
{
  if(pdcomm_info.num_rev_pddm_801_1 <= 0)
  {
    return TRUE;
  }

  if(tm_1x_up_is801_1_get_and_log_rlink_pddm(&pdcomm_info.rev_pddm) == FALSE)
  {
    MSG_ERROR("Can not get RLink PDDM from IS801 module",0,0,0);
    return FALSE;
  }

  /* Command PDCOMM to send data across to PDE.
  */
  if ( tm_1x_up_comm_get_state()!= PDCOMM_STATE_CONNECTED )
  {
    MSG_ERROR("pdcomm_info.curr_state %d not connected", 
                      tm_1x_up_comm_get_state(), 0, 0);
    return FALSE;
  }

  if(tm_1x_up_comm_send(
        (byte *)&(pdcomm_info.rev_pddm.msg_payload[0]), 
        pdcomm_info.rev_pddm.msg_size,
        IS801_VERSION_801_1) )
          
  {
    pdcomm_info.num_rev_pddm_801_1--;
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION tm_1x_up_is801_send_concatenated_pddms

DESCRIPTION: This function sends all pending requests and responses. First
             a pddm is created, and then its handed over to 1x up comm for
             sending it to the PDE.

             In case the session start flag is set, all 801-1 PDDMs and 1 
             801-B PDDM is sent. Subsequent calls to this function create
             PDDMs one by one, first 801-1 and then 801-B, and sends the 
             PDDMs one by one.

ARGUMENTS
  sess_flags : pointer to session flags
  tx_is801_b : whether to send is801-b pddms.
  tx_is801_1 : whether to send is801-1 pddms.

RETURN VALUE
  boolean

DEPENDENCIES

===========================================================================*/
#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
#error code not present
#endif
static uint16   bytes_copied = 0;

boolean tm_1x_up_is801_send_concatenated_pddms
(
  boolean sess_start
)
{
  #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#endif /* FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE */
  {
    /* first send all Is801-1 pddms, then if any 801-B are there, send those */
    if(pdcomm_info.num_rev_pddm_801_1 > 0)
    {
      if(i_1x_up_is801_1_drv_send_pddm() == FALSE)
      {
        MSG_ERROR("Could not send RLink PDDM from IS801-1 module",0,0,0);
        tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
        return FALSE;
      }
    }
    #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE 
    #error code not present
#endif
  }

  (void) bytes_copied;
  return TRUE;
}
/*===========================================================================

FUNCTION tm_1x_up_is801_drv_send_rlink_pddm

DESCRIPTION: This function sends all pending requests and responses. First
             a pddm is created, and then its handed over to 1x up comm for
             sending it to the PDE.

             In case the session start flag is set, all 801-1 PDDMs and 1 
             801-B PDDM is sent. Subsequent calls to this function create
             PDDMs one by one, first 801-1 and then 801-B, and sends the 
             PDDMs one by one.

ARGUMENTS
  sess_flags : pointer to session flags
  tx_is801_b : whether to send is801-b pddms.
  tx_is801_1 : whether to send is801-1 pddms.

RETURN VALUE
  boolean

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_is801_drv_send_rlink_pddm
(
  const byte*    sess_flags,
  boolean        tx_is801_b,
  boolean        tx_is801_1
)
{
  /* If both Is801B and IS801-1 PDDMs are to be sent, this is the first PDDM
   * to be sent in the session, set the sess_start flag */
  boolean    sess_start = (((*sess_flags) & TM_1X_UP_IS801_B_PDDM) &&
                           ((*sess_flags) & TM_1X_UP_IS801_1_PDDM));

  MSG_MED("Num 801-1 %d 801-B %d PDDMs to be sent= %d",
                  pdcomm_info.num_rev_pddm_801_1,
                  pdcomm_info.num_rev_pddm_801_b,0);

  if(tm_1x_up_is801_send_concatenated_pddms(sess_start) == FALSE)
  {
    MSG_ERROR("Could not send RLink PDDM from IS801-1 module",0,0,0);
    tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
    return FALSE;
  }
  /* succesfully copied all 801-1 and at most 1 801-B PDDM, reset state */
  bytes_copied = 0;

  return TRUE;
}

/*===========================================================================

FUNCTION tm_1x_up_is801_1_drv_send_rlink_pddm

DESCRIPTION

RETURN VALUE
  boolean

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_is801_1_drv_send_rlink_pddm(byte sess_start, byte sess_end,  uint8 num_pddm)
{
    int num_rlink_pddm;
    is801_pddm_sess_info_s_type     rev_pddm_hdr; 

    rev_pddm_hdr.sess_start  = sess_start;
    rev_pddm_hdr.sess_end    = sess_end;
    rev_pddm_hdr.sess_source = 0x1;
    rev_pddm_hdr.sess_tag    = sess_tag;
    rev_pddm_hdr.pd_msg_type = PD_MSG_TYPE;

    /* Instruct IS801-1 to send all req and rsp as a single pkt to PDCOMM
    */
    num_rlink_pddm = is801_create_rev_link_pddm(
                           TM_PRTL_TYPE_1X_UP,
                           rev_pddm_hdr,
                           MAX_IS801_PDDM_SIZE);
    if(num_rlink_pddm < 0)
    {
      MSG_ERROR("IS801-1 returned PDDMs = %d",num_rlink_pddm,0,0);
      return FALSE;
    }

    /* init the total number of pddms to be sent, num_pddm created previously
     * and num_rlink_pddm created in last step */
    pdcomm_info.num_rev_pddm_801_1 = num_pddm + num_rlink_pddm;
    MSG_MED("Num 801-1 PDDMs to be sent= %d",pdcomm_info.num_rev_pddm_801_1,0,0);

    if(sess_end == TRUE)
    {
      pdcomm_info.wait_for_sess_end_ack = TRUE;
      tm_1x_up_session_info.sess_end_sent = TRUE;
    }

    if(i_1x_up_is801_1_drv_send_pddm() == FALSE)
    {
      MSG_ERROR("Could not send RLink PDDM from IS801-1 module",0,0,0);
      tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
      return FALSE;
    }

    return TRUE;
}

#if 0
/*===========================================================================

FUNCTION tm_1x_up_is801_b_drv_send_rlink_pddm

DESCRIPTION

RETURN VALUE
  boolean

DEPENDENCIES
===========================================================================*/
boolean tm_1x_up_is801_b_drv_send_rlink_pddm(byte sess_flags,  uint8 num_pddm)
{
    tm_pdu_buf_type       *rlink_pddm      = NULL;
    is801b_pddm_hdr_type  *pddm_hdr        = NULL;
    int                    num_rlink_pddm  = 0;
    uint16                 total_length    = 0;

    /* Instruct IS801 to send all req and rsp as a single pkt to PDCOMM
    */
    num_rlink_pddm = is801b_create_rev_link_pddm(
                            TM_PRTL_TYPE_1X_UP,
                            sess_flags,
                            sess_tag,
                            MAX_IS801_PDDM_SIZE);

    if(num_rlink_pddm < 0)
    {
      MSG_ERROR("IS801B returned PDDMs = %d",num_rlink_pddm,0,0);
      return FALSE;
    }

    pdcomm_info.num_rev_pddm_801_b = num_pddm + num_rlink_pddm;
    MSG_MED("Num 801B PDDMs to be sent= %d",pdcomm_info.num_rev_pddm_801_b,0,0);

    if(sess_flags & TM_1X_UP_SESS_END_MASK)
    {
      pdcomm_info.wait_for_sess_end_ack = TRUE;
      tm_1x_up_session_info.sess_end_sent = TRUE;
    }

    if(i_1x_up_is801_b_drv_send_pddm() == FALSE)
    {
      MSG_ERROR("Could not send is801b pddm", 0, 0, 0);
      tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
      return FALSE;
    }
    
    return TRUE;
}
#endif

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_ref_loc

DESCRIPTION
  This function handles the ref location supplied by MGP. The ref location is
  requested in handle_pde_connected_dload, that also requests the encoder 
  layers to encode all the req/resp elements (801-1 and 801-B) except location
  response. These req/resps are created into PDDMs in this function and sent to 
  the PDE.
  
  The function sends an IS801-1 location response to the PDE and also saves the
  ref location in a local buffer. 

  The IS801-B ref locn response is sent only when the PDE responds with a Base 
  Station Capabilities message indicating that it supports IS801-B.

RETURN VALUE
  boolean

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_handle_ref_loc(sm_GnssFixRptStructType  *lr_ptr)
{
  static sm_GnssFixRptStructType *lr;

  static const pddm_op_mask     cr_mask  = ( TM_1X_UP_SESS_START_MASK  | 
                                             TM_1X_UP_IS801_1_PDDM     |
                                             TM_1X_UP_IS801_B_PDDM);

  static const pddm_op_mask     tx_mask  = ( TM_1X_UP_IS801_TIMER_MASK |
// TBD check if ok to delete this            TM_1X_UP_SESS_START_MASK  | 
                                             TM_1X_UP_IS801_1_PDDM     |
                                             TM_1X_UP_IS801_B_PDDM);

  is801_operation  ref_loc_ops[]  =  
    { RL_RESP_REF_LOC, SAVE_REF_LOC, CR_PDDM,  TX_PDDM, OPS_END };
  const void*      ref_loc_par[]  =  
    { &lr,             &lr,          &cr_mask, &tx_mask };

  is801_operation  *ops = ref_loc_ops;
  void             *par = ref_loc_par;

  MSG_MED("Handling ref_loc",0,0,0);
  if (lr_ptr == NULL)
  {
    MSG_ERROR("Null ref_loc",0,0,0);
    tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
    return;
  }

  lr = lr_ptr;

  (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_mgp_timer);

  if(tm_1x_up_session_info.session_state != TM_1X_UP_SESSION_STATE_WAIT_REF_LOC)
  {
    MSG_MED("Received RefLoc in wrong state %d",tm_1x_up_session_info.session_state,0,0);
    return;
  }

  /* TBD BUG? This is not really start of a new session since start is at
   * handle_pde_connected_dload */
   /* start of a new session */
  sess_tag = ( sess_tag + 1 ) % 32;

  if(!tm_state_transition(ops, par, TM_1X_UP_SESSION_STATE_WAIT_DLOAD_RESP))
  {
    MSG_ERROR("Could not transition to WAIT_DLOAD_RESP state", 0, 0, 0);
    /* If sucessfully handled ref loc and made the state transition, clear LR_MASK 
     * from resp_to_send bit mask
     *
    tm_1x_up_session_info.mode_info.resp_to_send &= ~(IS801_REV_LINK_RESP_MASK_LR); */
  }
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_pde_connected_dload

DESCRIPTION
  This function is used to handle Dload operation. This function gets called
  once MS is connected with PDE. 
  
  This function handles the case when MGP does not send a ref locn to the PDE,
  I.e. MGP either has a ref location to send to the PDE, or we are in data 
  dload ops, where we dont send a ref locn. to the PDE, we just want to 
  download all assistance data.

  In case this is a data dload session, 1X UP will prepare IS801 request's and 
  IS801 response based on Dload bit's transmit the PDDM and wait for a dload
  response

RETURN VALUE
  None.

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_handle_pde_connected_dload(void)
{
  static const pddm_op_mask  tx_mask  = ( TM_1X_UP_IS801_TIMER_MASK |
// TBD check if this is ok to remove      TM_1X_UP_SESS_START_MASK  |
                                          TM_1X_UP_IS801_1_PDDM     |
                                          TM_1X_UP_IS801_B_PDDM);

  static const pddm_op_mask  cr_mask =  ( TM_1X_UP_SESS_START_MASK  |
                                          TM_1X_UP_IS801_1_PDDM     |
                                          TM_1X_UP_IS801_B_PDDM);

  static       tm_1x_up_rlink_req_type  req_mask        = 0x0;

  /* ref_lr_ops are used in case we are sending seed location to the PDE */
  is801_operation  ref_lr_ops[]  =  
    { RL_RESP_MSI,    RL_REQ_ASST_DATA_1,  RL_REQ_B_BSC,  RL_RESP_B_MSI, 
      RL_RESP_B_SPI,  MGP_GET_REF_LOC,     OPS_END };

  const void*      ref_lr_par[]  =  
    { NULL,           &req_mask,           NULL,          NULL,          
      NULL,           NULL,            };

  /* just download assistance data */
  is801_operation  data_dload_ops[]     =  
    { RL_RESP_MSI,    RL_REQ_ASST_DATA_1,  RL_REQ_B_BSC,  RL_RESP_B_MSI, 
      RL_RESP_B_SPI,  CR_PDDM,             TX_PDDM,       OPS_END };

  const void*      data_dload_par[]     =  
    { NULL,           &req_mask,           NULL,          NULL,          
      NULL,           &cr_mask,            &tx_mask };

  /* Initialize operations and next_state to msb call flow where ppm is sent to the PDE */
  tm_1x_up_session_state_e_type  next_state   = TM_1X_UP_SESSION_STATE_WAIT_DLOAD_RESP; 
  is801_operation                *operations  = data_dload_ops;
  void                           *parameters  = data_dload_par;


  MSG_HIGH("Connected for Dload operation",0,0,0);

  if(tm_1x_up_session_info.session_state != TM_1X_UP_SESSION_STATE_ENTRY)
  {
    MSG_ERROR("Connected..Wrong state. %d", tm_1x_up_session_info.session_state,0,0);
    return;
  }

  req_mask       = tm_1x_up_session_info.mode_info.req_to_send;
  /* New session. Increment sess_tag */
  sess_tag = ( sess_tag + 1 ) % 32;
   
  /* If we need to send the ref location to PDE, change next state and operations */
  if (tm_1x_up_session_info.mode_info.resp_to_send & IS801_REV_LINK_RESP_MASK_LR)
  {
    operations = ref_lr_ops;
    parameters = ref_lr_par;
    next_state = TM_1X_UP_SESSION_STATE_WAIT_REF_LOC;
  }

  /* If state transition is not successful, tm_state_transition will terminate 
   * the session. Nothing more to be done, wo we can ignore the ret_val  */
  if(tm_state_transition(operations, parameters, next_state) == FALSE)
  { 
    MSG_ERROR("State transition to %d failed", next_state,0,0);
  }

  return;
} /* tm_1x_up_is801_drv_handle_pde_connected_dload() */

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_ppm_dload

DESCRIPTION
  This function is used to handle Dload operation. This function gets called
  once PPM response is available. 
  
  1X UP will prepare IS801-1/-B request's and IS801-1/-B response based on 
  Dload bit's and wait for dload operation to complete.

  In the 801-B case, PPM measurements are not sent to PDE before receiving a
  Baste Station Capabilities message from the PDE, indicating that it supports
  801-B. For this case, PPM measurements are stored in a local buffer.

  Entry State: TM_1X_UP_SESSION_STATE_ENTRY
  Exit  State: TM_1X_UP_SESSION_STATE_WAIT_DLOAD_RESP
RETURN VALUE
  Events to be set for Client's.

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_is801_drv_handle_ppm_dload(srch_ppm_rpt_type *ppm_ptr)
{
  static const pddm_op_mask             tx_mask   = ( TM_1X_UP_IS801_TIMER_MASK | 
                                                      TM_1X_UP_IS801_1_PDDM     |
                                                      TM_1X_UP_IS801_B_PDDM );

  static const tm_1x_up_msb_op_e_type   op        = DLOAD_START;
  static       tm_1x_up_rlink_req_type  req_mask  = 0x0;
  static       srch_ppm_rpt_type        *ppm_par  = NULL;
  static       pddm_op_mask        cr_msk_801_1   = TM_1X_UP_IS801_1_PDDM;
  static       pddm_op_mask        cr_msk_801_b   = TM_1X_UP_IS801_B_PDDM; 

  /* ops and parameters for data download sessions (only download aiding data,
   * no positioning session required */
  is801_operation  data_dload_ops[]  =  
    { RL_RESP_MSI,  RL_RESP_PRDM,  RL_RESP_PPM,   RL_REQ_ASST_DATA_1,
      CR_PDDM,      RL_RESP_B_MSI, RL_RESP_B_SPI, RL_REQ_B_BSC, SAVE_PPM_TO_BUF,
      CR_PDDM,      TX_PDDM,      OPS_END };

  const void*      data_dload_par[]  =  
    { NULL,         NULL,         &ppm_par,      &req_mask,
     &cr_msk_801_1, NULL,          NULL,          NULL,        &ppm_par,
     &cr_msk_801_b, &tx_mask };

  /* ops and parameters for msb positioning sessions that need aiding data */
  is801_operation  msb_ppm_ops[]     =  
    { MGP_REQ_LR,          RL_RESP_MSI,  RL_RESP_PRDM,  RL_RESP_PPM,    
      RL_REQ_ASST_DATA_1,  CR_PDDM,      RL_RESP_B_MSI, RL_REQ_B_BSC,  SAVE_PPM_TO_BUF,
      CR_PDDM,             TX_PDDM,      OPS_END };

  const void*      msb_ppm_par[]     =  
    { &op,                 NULL,         NULL,         &ppm_par,
      &req_mask,          &cr_msk_801_1, NULL,          NULL,          &ppm_par,
      &cr_msk_801_b,      &tx_mask };

  is801_operation  *operations = msb_ppm_ops;
  void             *parameters = msb_ppm_par;

  ppm_par  = ppm_ptr;
  req_mask = tm_1x_up_session_info.mode_info.req_to_send;

  MSG_HIGH("Handle PPM during Dload operation",0,0,0);

  /* Perform sanity checks on parameters */
  if(tm_1x_up_session_info.session_state != TM_1X_UP_SESSION_STATE_ENTRY)
  {
    MSG_ERROR("Connected..Wrong state. state = %d", tm_1x_up_session_info.session_state,0,0);
    return TRUE;
  }

  if(ppm_ptr == NULL)
  {
    MSG_ERROR("NULL ppm ptr passed.",0,0,0);
    return FALSE;
  }

  if( (tm_1x_up_session_info.api_param.pd_api_info.pd_option.session != PDSM_PD_SESS_TYPE_DATA_DLOAD) &&
      !(tm_1x_up_session_info.mode_info.resp_to_send & IS801_REV_LINK_RESP_MASK_PPM))
  {
    MSG_ERROR("Incorrect session (%d) or resp to send (%x)", 
                    tm_1x_up_session_info.api_param.pd_api_info.pd_option.session,
                    tm_1x_up_session_info.mode_info.resp_to_send, 0);
    return FALSE;
  }

  /* 
   * If coarse location is not enabled, the session start mask will be set
   * with the rest of the reqs/responses in the first PDDM to be sent.
   *
   * If coarse location is enabled, the coarse location resp. goes in its own
   * PDDM, that has the session start flag set
   */
  if(tm_1x_up_config.coarse_location_enabled != TRUE)
  {
    cr_msk_801_1 = ( TM_1X_UP_SESS_START_MASK | TM_1X_UP_IS801_1_PDDM );
    cr_msk_801_b = ( TM_1X_UP_SESS_START_MASK | TM_1X_UP_IS801_1_PDDM );
  }
  else
  {
    cr_msk_801_1 = TM_1X_UP_IS801_1_PDDM;
    cr_msk_801_b = TM_1X_UP_IS801_B_PDDM;
  }

  /* This is start of a new session. Increment the sess_tag */
  sess_tag = ( sess_tag + 1 ) % 32;

  if(tm_1x_up_session_info.api_param.pd_api_info.pd_option.session == PDSM_PD_SESS_TYPE_DATA_DLOAD)
  {
    operations = data_dload_ops;
    parameters = data_dload_par;
  }

  /* Update the state to Indicate that 1X UP is waiting for Dload data. */
  return (tm_state_transition(operations, parameters, TM_1X_UP_SESSION_STATE_WAIT_DLOAD_RESP));
  /* TBD: Shouldnt we negate PPM from resp_to_send like in handle_ref_loc? */
} /* tm_1x_up_is801_drv_handle_ppm_dload() */

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_grant_session

DESCRIPTION
  This function is used to handle the state when we are connected
  with PDE.

   For Call flow 1, 2, 3: Connected to PDE is trigger to REQ_PPM to TM-Core 
   For Call flow 7: Conencted to PDE is trigger to REQ_LR to TM-Core
   
RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_handle_grant_session
(
  boolean sess_start
)
{
  if (tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_MSA)
  {
    MSG_HIGH("Connected for MS-Assisted fix",0,0,0);
    
    /* For TM-Core sess_start, do not start PPM here, return action = REQ_PPM */
    if(sess_start == FALSE)
    {
      /* Request PPM once connected.
      */
      if (tm_1x_up_session_info.mgp_info.ppm_on) 
      { 
        if(!tm_1x_up_is801_drv_get_ppm())
        {
          MSG_ERROR("Unable to get PPM results",0,0,0);
          tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
        }
      }
      else
      {
        if(!tm_1x_up_is801_drv_start_ppm())
        {
          MSG_ERROR("Unable to request TM-Core to start PPM",0,0,0);
          tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
        } /* if start_ppm success */
      }
    }
  } /* if MS-Assisted */
  else if ( (tm_1x_up_is801_drv_session_active()) && (tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_DLOAD ) )
  {
    /* If PPM response needs to sent, get PPM */
    if (tm_1x_up_session_info.mode_info.resp_to_send & IS801_REV_LINK_RESP_MASK_PPM)
    {
      if (tm_1x_up_session_info.mgp_info.ppm_on) 
      { 
        if(!tm_1x_up_is801_drv_get_ppm())
        {
          MSG_ERROR("Unable to get PPM results",0,0,0);
          tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
        }
      }
      else
      {
        if(!tm_1x_up_is801_drv_start_ppm())
        {
          MSG_ERROR("Unable to request TM-Core to start PPM",0,0,0);
          tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
        } /* if start_ppm success */
      }
    }
    else
    {
      tm_1x_up_is801_drv_handle_pde_connected_dload();
    }
  } /* if Dload */
  else
  {
    MSG_HIGH("PDE Connection not expected!",0,0,0);
  }

} /* tm_1x_up_is801_drv_handle_grant_session */


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_pde_connected

DESCRIPTION
  This function is used to handle the state when we are connected
  with PDE.

   For Call flow 1, 2, 3: Connected to PDE is trigger to REQ_PPM to TM-Core 
   For Call flow 7: Conencted to PDE is trigger to REQ_LR to TM-Core
   
RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_handle_pde_connected(boolean sess_start)
{
  /* Local Varaiables */
  tm_1x_up_smod_req_result_e_type  start_sess_req_result = TM_1X_UP_SMOD_REQ_FAIL;
  tm_1x_up_smod_req_status_e_type  req_status = TM_1X_UP_SMOD_REQ_STATUS_NONE;
  tm_1x_up_smod_curr_op_mode_e_type cur_op_mode = TM_1X_UP_SMOD_CURR_OP_MODE_NONE;

  /* Set the client ID */
  tm_1x_up_info_buffer.client_id = tm_1x_up_session_info.api_param.pd_api_info.client_id;
 
  (void)tm_post_pd_event( TM_PRTL_TYPE_1X_UP,
                          tm_1x_up_session_info.sess_handle,
                          PDSM_PD_EVENT_COMM_CONNECTED,
                          &tm_1x_up_info_buffer,
                          NULL);

  /* First stop connection establisment timer.
  */
  (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_connect_timer);  


  /*-------------------------------------------------------------------------
     Request a session with Sub-Modules: i.e. start request sent to Japan-GPS
     module.
  -------------------------------------------------------------------------*/
  cur_op_mode 
    = tm_1x_up_is801_drv_trans_op_mode(tm_1x_up_session_info.mode_info.mode);
  
  start_sess_req_result 
    = tm_1x_up_smod_start_sess_req( 
                                   &tm_1x_up_session_info.api_param.pd_api_info,
                                   cur_op_mode,
                                   &req_status 
                                  );

  /*-------------------------------------------------------------------------
     Process the Sub-Module response
  -------------------------------------------------------------------------*/
  if( TM_1X_UP_SMOD_REQ_SUCCESS == start_sess_req_result )
  {
    switch( req_status )
    {
    /*-------------------------------------------------------------------------
       Granted, sessions start request
    -------------------------------------------------------------------------*/
    case TM_1X_UP_SMOD_REQ_STATUS_GRANT:
     tm_1x_up_is801_drv_handle_grant_session(sess_start);
     break;

    /*-------------------------------------------------------------------------
       Wait for session start response from Sub-Module
    -------------------------------------------------------------------------*/
    case TM_1X_UP_SMOD_REQ_STATUS_WAIT:
      MSG_HIGH("wait for response from 1x UP sub-module not handled (status:%d)", 
               req_status, 0, 0);
      break;

    /*-------------------------------------------------------------------------
       Response status not handled
    -------------------------------------------------------------------------*/      
    default:
      MSG_ERROR("start sess req status not handled (status:%d)", req_status, 0, 0);
      break;
    }    
  }
  else
  {
    MSG_ERROR("start sess req error (status:%d)", req_status, 0, 0);
  }

  return;

} /* tm_1x_up_is801_drv_handle_pde_connected */

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_end_session_with_pde

DESCRIPTION
  This function is used to send End Session to PDE

RETURN VALUE
  None

DEPENDENCIES
===========================================================================*/
void tm_1x_up_is801_drv_end_session_with_pde(int session_tag)
{
  #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#endif

  static const uint8 cr_mask_1  = ( TM_1X_UP_SESS_END_MASK | 
                                    TM_1X_UP_IS801_1_PDDM ); 

  static const uint8 tx_mask_1  =   TM_1X_UP_IS801_1_PDDM; 

  is801_operation    sess_end_ops_1[]  =  
    { RL_RESP_MSI,    CR_PDDM,       TX_PDDM,  OPS_END };
  const void*        sess_end_par_1[]  =  
    { NULL,           &cr_mask_1,    &tx_mask_1 };

  #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#endif

  is801_operation  *ops = sess_end_ops_1;
  void             *par = sess_end_par_1;

  #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#endif

  if( tm_1x_up_execute_fns(ops, par) == FALSE)
  {
    tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
    return;
  }
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_prm

DESCRIPTION
  This function is used to handle GPS results from Searcher. The PRMs are saved 
  into a local buffer. Once all PRM parts are received, a req is sent to obtain
  PPM results.
  
RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

boolean tm_1x_up_is801_drv_handle_prm( sm_GpsMeasRptStructType  *info_ptr)
{
  static const sm_GpsMeasRptStructType *prm_ptr = NULL;
  
  /* PRM ops if rcvd part is not the final part (PRMs are rcvd 1 part per
   * constellation */
  is801_operation   prm_part_ops[]   = { SAVE_PRM_TO_BUF, OPS_END };
  const void*       prm_part_par[]   = {&prm_ptr  };

  /* PRM ops for final PRM part. Once the last part is rcvd, req PPM from MGP */
  is801_operation   prm_last_ops[]   = { SAVE_PRM_TO_BUF, MGP_REQ_PPM, OPS_END };
  const void*       prm_last_par[]   = {&prm_ptr,         NULL };

  is801_operation  *operations = prm_part_ops;
  void             *parameters = prm_part_par;

  
  if(info_ptr == NULL)
  {
    MSG_HIGH("Null PRM data reported by MGP",0,0,0);
    tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
    return FALSE;
  }

  if(info_ptr->z_MeasBlk.u_SeqNum == info_ptr->z_MeasBlk.u_MaxMessageNum)
  {
    tm_1x_up_session_info.mgp_info.prm_on = FALSE;
    operations  = prm_last_ops;
    parameters  = prm_last_par;
  }

  prm_ptr = info_ptr;

  if ( (tm_1x_up_session_info.session_state != TM_1X_UP_SESSION_STATE_WAIT_PR) &&
       (tm_1x_up_session_info.session_state != TM_1X_UP_SESSION_STATE_WAIT_SA))
  {
    MSG_ERROR("GPS resp in Wrong state=%d",tm_1x_up_session_info.session_state,0,0);
    return FALSE;
  }

  /* Generate CGPS event */
  sm_log_event_report(SM_LOG_EVENT_ID_GPS_COMPLETE,0);

  return (tm_state_transition(operations, parameters, TM_1X_UP_SESSION_STATE_PR));
} /* End tm_1x_up_is801_drv_handle_prm( ) */

/*===========================================================================

FUNCTION i_tm_1x_up_is801_drv_post_msa_pd_event

DESCRIPTION
  We have rcvd a location response from the PDE, post a PD event and a ref
  location to MGP

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
static void i_tm_1x_up_is801_drv_post_msa_pd_event
(
  tm_post_data_payload_type        *aiding_data_ptr,
  pdsm_pd_event_type                pd_event
)
{
  tm_1x_up_smod_req_status_e_type   req_status;

  if (aiding_data_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL ptr");
    return;
  }

  MSG( MSG_SSID_GPSSM, MSG_LEGACY_MED, "Send LR data to TM-Core");
  if(tm_post_data(TM_PRTL_TYPE_1X_UP,
                    tm_1x_up_session_info.sess_handle,
                    TM_POST_DATA_TYPE_LR,
                    aiding_data_ptr) == FALSE)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Post data for LR failed");
    return;
  }


  /* Query SMOD iface to determine if 1x UP can post event to TM-Core */
  if( TM_1X_UP_SMOD_REQ_SUCCESS == 
                   tm_1x_up_smod_info_available(&tm_1x_up_info_buffer,
                                                PDSM_PD_END_SESS_NONE,
                                                NULL,
                                                &req_status)
    )
  {
    /* Send the LR data to TM-Core, if the status is NOT wait,
       otherwise, SMOD iface will post event to TM-Core once
       it has received additional data (i.e. JPGS module gets comp
       response from SCU)
    */
    if(req_status !=  TM_1X_UP_SMOD_REQ_STATUS_WAIT)
    {
      (void)tm_post_pd_event( TM_PRTL_TYPE_1X_UP,
                              tm_1x_up_session_info.sess_handle,
                              pd_event,
                              &tm_1x_up_info_buffer,
                              NULL);

      if(TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable))
      {
        /*If working under SUPL, let SUPL know that pos
          is reported*/
        tm_umts_up_supl_is801_pos_reported();
      }
    } 
  }
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_msa_post_pd_event

DESCRIPTION
  take LR response from PDE and post a PD event

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_msa_post_pd_event
(
  const is801_lr_resp_type *lr_resp_ptr
)
{
  pdsm_pd_event_type                pd_event;
  tm_post_data_payload_type        *aiding_data_ptr = NULL;
  byte                              unc_index;

  if((lr_resp_ptr->latitude == 0) &&
     (lr_resp_ptr->longitude == 0))
  {
    MSG_ERROR("801B PDE sent locn with lat long 0", 0, 0, 0);
    return;
  }

  tm_1x_up_dload_info.num_data = 1;
  tm_1x_up_dload_info.dload_info[0].data   = PDSM_PD_DLOAD_LOCATION;
  tm_1x_up_dload_info.dload_info[0].source = PDSM_PD_DLOAD_SOURCE_PDE;

  tm_1x_up_pos_info.position_mode = PDSM_PD_POSITION_MODE_MSASSISTED;
  tm_1x_up_pos_info.position_type = PDSM_PD_POSITION_TYPE_NEW;
  /* Check if its a AFLT fix */
  if ( tm_1x_up_session_info.api_param.session_type == TM_1X_UP_SESSION_TYPE_AFLT )
  { 
    tm_1x_up_pos_info.positioning_source = PDSM_PD_POSITION_SOURCE_AFLT;
  }
  else
  {
    tm_1x_up_pos_info.positioning_source = PDSM_PD_LOCATION_SOURCE_HYBRID;
  }
  tm_1x_up_pos_info.gps_utc_offset = 0;

  tm_1x_up_ext_pos_data.ext_lat = (lr_resp_ptr->latitude) * DEG2RAD / LAT_LONG_SCALE;
  tm_1x_up_ext_pos_data.ext_lon = (lr_resp_ptr->longitude) * DEG2RAD / LAT_LONG_SCALE;

  /* Range check uncertainty values and use table to convert value to meters */
  unc_index = lr_resp_ptr->loc_uncrtnty_a;
  if (unc_index >= MAX_POS_UNC_IND)
  {
    unc_index = MAX_POS_UNC_IND-1;
  }
  tm_1x_up_ext_pos_data.ext_loc_uncrtnty_a = f_PosUncArray[unc_index];
  unc_index = lr_resp_ptr->loc_uncrtnty_p;
  if (unc_index >= MAX_POS_UNC_IND)
  {
    unc_index = MAX_POS_UNC_IND-1;
  }
  tm_1x_up_ext_pos_data.ext_loc_uncrtnty_p = f_PosUncArray[unc_index];

  if(lr_resp_ptr->velocity_incl)
  {
    tm_1x_up_ext_pos_data.ext_vel_horiz = lr_resp_ptr->velocity_hor;
    tm_1x_up_ext_pos_data.ext_vel_vert = lr_resp_ptr->velocity_ver;
    /* For MS-A, can not get Horiz and vert unc from IS-801 */
    tm_1x_up_ext_pos_data.ext_vel_horiz_unc = 0; 
    tm_1x_up_ext_pos_data.ext_vel_vert_unc = 0;
  }

  pd_event = is801_util_pd_info_data_init( &tm_1x_up_info_buffer,
                                        lr_resp_ptr, 
                                        tm_1x_up_session_info.api_param.pd_api_info.client_id, 
                                        &tm_1x_up_dload_info, 
                                        &tm_1x_up_pos_info, 
                                        &tm_1x_up_ext_pos_data );


  memscpy(tm_1x_up_session_info.lr_timestamp, sizeof(tm_1x_up_session_info.lr_timestamp),
  	      lr_resp_ptr->timestamp, sizeof(lr_resp_ptr->timestamp));
  /* clear the num_data for next fix.
  */
  tm_1x_up_dload_info.num_data = 0;

  aiding_data_ptr = (tm_post_data_payload_type *)pdsm_getbuf(sizeof(tm_post_data_payload_type));

  if(aiding_data_ptr == NULL) 
  {
    MSG_ERROR("No buffer for aiding data",0,0,0);
    tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
    return;
  }

  /* Convert from IS801 to LM format */
  (void)is801_util_flink_convert_seed(lr_resp_ptr,
                         &aiding_data_ptr->seed_data);

  i_tm_1x_up_is801_drv_post_msa_pd_event(aiding_data_ptr, pd_event);
  
  (void)pdsm_freebuf((char *)aiding_data_ptr);
  return;
}

#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
#error code not present
#endif

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_lr_resp_msa

DESCRIPTION
  This function is used to handle LR response from PDE.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
static void tm_1x_up_is801_drv_handle_lr_resp_msa
(
  const is801_fwd_link_pddm_info_s_type *resp_ptr, 
  uint8                                  index,
  is801b_flink_ie_adv_lr                *adv_lr_ptr,
  boolean                                send_sess_end
)
{
  const   is801_lr_resp_type     *lr_resp_ptr = NULL;
  boolean locn_present  = FALSE;
  uint8   u_sess_tag      = 0;

  MSG_HIGH("Rxed LR data from PDE",0,0,0);

  if( (resp_ptr == NULL && adv_lr_ptr == NULL) ||
      (resp_ptr != NULL && adv_lr_ptr != NULL))
  {
    MSG_ERROR("Inconsistent 801-1 %x and 801-B %x responses present", 
                 resp_ptr, adv_lr_ptr, 0);
  }

  if ( tm_1x_up_session_info.session_state != TM_1X_UP_SESSION_STATE_WAIT_PD )
  {
    MSG_ERROR("LR in wrong state=%d",tm_1x_up_session_info.session_state,0,0);
    return; 
  }

  
  /* Whether 801-1 or 801-B,
   * Complete MS-Assisted operation in this function, except connection tear down.
  */
  tm_1x_up_is801_drv_update_session_state(TM_1X_UP_SESSION_STATE_WAIT_NEXT_EVENT);
  tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_NONE;

  /* Clear the request bit corsp to LR request.
  */
  tm_1x_up_session_info.is801_req_wait_bmap &= ~(TM_1X_UP_RLINK_REQ_LR);
  /* Clear the IS801 response timer for MS-Assisted mode.
  */
  (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_is801_timer);  
  if (tm_1x_up_session_info.is801_req_wait_bmap != NULL)
  {
    MSG_ERROR("req_wait=%d, should be NULL",
                     tm_1x_up_session_info.is801_req_wait_bmap,0,0);
  }


  /* Depending on resp if from 801-B or 801-1, find out if location is present
   * inside the resp, the sess_tag for the session and post PD event */
  if(resp_ptr)
  {
    lr_resp_ptr   = &resp_ptr->resp_rd[index].resp_msg_payload.lr_resp;
    locn_present  = ((lr_resp_ptr->latitude != 0) &&
                     (lr_resp_ptr->longitude != 0));

    /* post 801-1 pd event */
    tm_1x_up_is801_drv_msa_post_pd_event(lr_resp_ptr);

    u_sess_tag      = resp_ptr->hdr.sess_tag;
    /* If PDE has not sent sess_end, we send sess_end */
    send_sess_end = (resp_ptr->hdr.sess_end != 0x00) ? FALSE : TRUE;
  }
  else
  {
    #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
    #error code not present
#else
    MSG_ERROR("NULL resp ptr sent. no 801-1 data to decode", 0, 0, 0);
    return;
    #endif
  }


  if (send_sess_end)
  {
    MSG_HIGH("Send End sess to PDE",0,0,0);
    tm_1x_up_is801_drv_end_session_with_pde(u_sess_tag);

    /* Generate CGPS event */
    sm_log_event_report( SM_LOG_EVENT_ID_POST_ASSISTANCE_DATA_COMM,
                         SM_LOG_EVENT_DATA_POST_ASSISTANCE_DATA_COMM_END );
  } /* end sess_end */

  if(locn_present)
  {
    tm_1x_up_is801_drv_opn_completed(PDSM_PD_END_SESS_NONE);
  }
  else
  {
    MSG_ERROR("Lat and Long reported by PDE is zero!",0,0,0);
    tm_1x_up_is801_drv_opn_completed(PDSM_PD_END_FIX_ERROR);
  }

  return;
} /* End tm_1x_up_is801_drv_handle_lr_resp_msa() */

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_ppm_ms_assist

DESCRIPTION: This function handles PPM measurements for MS assisted callflows.
             PPMs received are either PreFix PPMs (state machine state is 
             STATE_ENTRY) or PostFix PPMs (state machine state is STATE_PR).

             The PreFix PPMs are used by the PDE to calculate an AFLT fix for
             the handset, which may be used for ref or final location.

             The PostFix PPMs are used by the PDE to calculate hybrid fixes.

             The NEXT STATE in the state machine is:~

                 STATE_ENTRY (AFLT FIX) ---> TM_1X_UP_SESSION_STATE_WAIT_PD
                 STATE_PR    (MSA FIX)  ---> TM_1X_UP_SESSION_STATE_WAIT_PD
                                             (wait for posn response from PDE)

                 STATE_ENTRY (MSA FIX)  ---> TM_1X_UP_SESSION_STATE_WAIT_AA
                                             (wait for AA data from PDE)
RETURN VALUE
  TRUE:      State transition sucessful
  FALSE:     State transition failed

DEPENDENCIES

STATE_ENTRY: MSA  OPs {MSI, PRDM, CR_PDDM, PPM, AA, TX_PDDM}
STATE_ENTRY: AFLT OPs {MSI, PRDM, CR_PDDM, PPM, REQ LR, TX_PDDM}
STATE_PR:    MSA  Ops {PRM_FRM_BUF, PPM, REQ_LR, TOM, TX_PDDM}
===========================================================================*/
boolean tm_1x_up_is801_drv_handle_ppm_ms_assist( srch_ppm_rpt_type *ppm_ptr )
{
  static       uint8 cr_mask_801_1               = ( TM_1X_UP_IS801_1_PDDM );

  static const uint8 cr_start_mask_801_b         = ( TM_1X_UP_SESS_START_MASK  | 
                                                     TM_1X_UP_IS801_B_PDDM );
#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE 
  #error code not present
#endif

  static const uint8 tx_mask                     = ( TM_1X_UP_IS801_TIMER_MASK |
                                                     TM_1X_UP_IS801_1_PDDM     |
                                                     TM_1X_UP_IS801_B_PDDM );
  
  static const uint8 tx_mask_801_1               = ( TM_1X_UP_IS801_TIMER_MASK |
                                                     TM_1X_UP_IS801_1_PDDM     );

#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE 
  #error code not present
#endif

  static const tm_1x_up_rlink_req_type rl_req_aa =   TM_1X_UP_RLINK_REQ_AA;
  static const tm_1x_up_rlink_req_type rl_req_lr =   TM_1X_UP_RLINK_REQ_LR;
  static srch_ppm_rpt_type *ppm_ptr_param;

  /* in state entry, we dont know the version of the PDE, so we need to 
   * send both 801-1 and 801-B PDDMs */
  is801_operation  state_entry_msa_ops[]         = 
      { RL_RESP_MSI,           RL_RESP_PRDM,        RL_RESP_PPM,           
        RL_REQ_ASST_DATA_1,    SAVE_PPM_TO_BUF,     CR_PDDM,               
        RL_REQ_B_BSC,          RL_RESP_B_MSI,       CR_PDDM,               
        TX_PDDM,               OPS_END };
  const void*      state_entry_msa_par[]         = 
      { NULL,                  NULL,                &ppm_ptr_param,        
        &rl_req_aa,            &ppm_ptr_param,      &cr_mask_801_1,        
        NULL,                  NULL,                &cr_start_mask_801_b,  
        &tx_mask};

  /* in state prm, we know the version of the PDE, so we need to send either
   * 801-1 or 801-B PDDMs, depending on the version of the PDE */
  is801_operation  state_pr_msa_ops_801_1[]      = 
      { RL_RESP_PRM_FRM_BUF,   RL_RESP_PPM,         RL_REQ_ASST_DATA_1, 
        RL_RESP_TOM,           CR_PDDM,             TX_PDDM,            OPS_END};
  const void*      state_pr_msa_par_801_1[]      = 
      { &ppm_ptr_param,        &ppm_ptr_param,      &rl_req_lr,         
        &ppm_ptr_param,        &cr_mask_801_1,      &tx_mask_801_1 };

  #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#endif

  /* AFLT is not done using 801-B */
  is801_operation  state_entry_msa_aflt_ops[]    = 
      { RL_RESP_MSI,           RL_RESP_PRDM,        RL_RESP_PPM,           
        RL_REQ_ASST_DATA_1,    CR_PDDM,             TX_PDDM,               
        OPS_END };
  const void*      state_entry_msa_aflt_par[]    = 
      { NULL,                  NULL,                &ppm_ptr_param,        
        &rl_req_lr,            &cr_mask_801_1,      &tx_mask_801_1 };

  is801_operation                 *operations = NULL;
  void                            *parameters = NULL;
  boolean                          end_ppm    = FALSE;
  tm_1x_up_session_state_e_type    next_state = TM_1X_UP_SESSION_STATE_NONE; 

  if(ppm_ptr == NULL)
  {
    MSG_ERROR("Null PPM ptr",0,0,0);
    tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
    return FALSE;
  }

  ppm_ptr_param = ppm_ptr;

  if ( tm_1x_up_session_info.session_state == TM_1X_UP_SESSION_STATE_ENTRY )
  {
    MSG_HIGH("MS-Assist PPM pre-fix resp",0,0,0);

    /* This is start of a new session. Increment the sess_tag */
    sess_tag = ( sess_tag + 1 ) % 32;

    /* 
     * If coarse location is not enabled, the session start mask will be set
     * with the rest of the reqs/responses in the first PDDM to be sent.
     *
     * If coarse location is enabled, the coarse location resp. goes in its own
     * PDDM, that has the session start flag set
     */
    if(tm_1x_up_config.coarse_location_enabled != TRUE)
    {
      cr_mask_801_1 = ( TM_1X_UP_SESS_START_MASK | TM_1X_UP_IS801_1_PDDM );
    }
    else
    {
      cr_mask_801_1 = TM_1X_UP_IS801_1_PDDM;
    }

    /* Check if its a AFLT fix */
    if ( tm_1x_up_session_info.api_param.session_type == TM_1X_UP_SESSION_TYPE_AFLT
         && tm_1x_up_session_info.api_param.pd_api_info.pd_option.mode_info.mode 
                                                      != PDSM_SESS_JGPS_TYPE_AFLT 
       )
    { 
      /* Position results using AFLT only */
      operations = state_entry_msa_aflt_ops;
      parameters = state_entry_msa_aflt_par;
      /* Wait for PosnDetermination from PDE */
      next_state = TM_1X_UP_SESSION_STATE_WAIT_PD; 
      end_ppm    = TRUE;
    } /* AFLT */
    else
    { 
      /* Using Hybrid solution, request PDE for Aiding data */
      operations = state_entry_msa_ops;
      parameters = state_entry_msa_par;
      next_state = TM_1X_UP_SESSION_STATE_WAIT_AA; /* Wait for AA data from PDE */
    } /* End else if session_type is AFLT */
  }   /* End if session entry state */
  else if ( tm_1x_up_session_info.session_state == TM_1X_UP_SESSION_STATE_PR )
  {
    /* Any PDDM being sent in state PR never has the session start flag set */
    cr_mask_801_1 = ( TM_1X_UP_IS801_1_PDDM );

    #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
    #error code not present
#endif
    {
      operations = state_pr_msa_ops_801_1;
      parameters = state_pr_msa_par_801_1;
    }

    next_state = TM_1X_UP_SESSION_STATE_WAIT_PD; /* Wait for Posn Determination from PDE */
    end_ppm    = TRUE;

    sm_log_event_report(
                 SM_LOG_EVENT_ID_POST_ASSISTANCE_DATA_COMM,
                 SM_LOG_EVENT_DATA_POST_ASSISTANCE_DATA_COMM_START);

  } /* End else if STATE_WAIT_PR */
  else
  {
    MSG_ERROR("PPM resp in Wrong state %d",tm_1x_up_session_info.session_state,0,0);
  }

  if (end_ppm == TRUE && tm_1x_up_session_info.mgp_info.ppm_on )
  {
    (void)tm_1x_up_is801_drv_end_ppm();
  }
 
  return (tm_state_transition(operations, parameters, next_state));
}  /* End tm_1x_up_is801_drv_handle_ppm_ms_assist( ) */

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_dormancy_success

DESCRIPTION

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_handle_dormancy_success(void)
{
  event_report(EVENT_GPS_DORMANCY_END);

  if( tm_1x_up_comm_unforce_dormancy() )
  {
    MSG_HIGH("Register/Wait for TC UP", 0, 0, 0);
    tm_1x_up_session_info.tm_cm_register_bmap |= (tm_cm_register_events_bmap_e_type)TM_CM_REGISTER_TC_UP_EVENT; /*lint !e641 !e655 */
    /* Send bmap to TM_CM_Iface */
    (void) tm_cm_iface_handle_reg_events_req(tm_1x_up_session_info.tm_cm_register_bmap);
  }
  else
  {
    MSG_ERROR("Unable to unforce dormancy", 0, 0, 0);
    tm_1x_up_is801_drv_handle_dormancy_failure(TM_1X_UP_DORMANCY_FAILURE_TYPE_ABORT);
  }
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_tc_exit

DESCRIPTION
  This function pre-empts the session termination if MS exits traffic channel.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_handle_tc_exit(void)
{
  MSG_HIGH("Received TC End...", 0, 0, 0);

#if defined (FEATURE_GPSONE_UPDATE_BSID)
  if ((tm_1x_up_is801_drv_session_active() == TRUE) && (tm_1x_up_session_info.session_state == TM_1X_UP_SESSION_STATE_WAIT_FD))
  {
    /* if we force dormancy and PPP is opened by us, we need to unforce
    ** dormancy
    */
    MSG_HIGH("DeRegister TC EXIT", 0, 0, 0);
    tm_1x_up_session_info.tm_cm_register_bmap &= (tm_cm_register_events_bmap_e_type)~(TM_CM_REGISTER_TC_DOWN_EVENT); /*lint !e641 !e655 */

    tm_1x_up_session_info.dormancy_event_mask |= TM_1X_UP_DORMANCY_EVENT_MASK_RECVD_TC_EXIT;

    /* If PHYS_LINK down is received too, try to do unforce dormancy */
    if(tm_1x_up_session_info.dormancy_event_mask & TM_1X_UP_DORMANCY_EVENT_MASK_RECVD_PHY_LINK_DOWN)
    {
      tm_1x_up_is801_drv_handle_dormancy_success();
      tm_1x_up_session_info.dormancy_event_mask = TM_1X_UP_DORMANCY_EVENT_MASK_NONE;
    }
    return;
  }
  else
  {
    MSG_HIGH("Ignore TC Exit",0,0,0);
  }
#endif
} /* End tm_1x_up_handle_tx_exit() */

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_tc_up

DESCRIPTION
  Called when the traffice channel comes up.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_handle_tc_up(void)
{
  MSG_HIGH("Received TC UP...", 0, 0, 0);

#ifdef FEATURE_GPSONE_UPDATE_BSID

  if (((tm_1x_up_is801_drv_session_active() == TRUE) && (tm_1x_up_session_info.session_state == TM_1X_UP_SESSION_STATE_WAIT_FD)))
  {
    MSG_HIGH("DeRegister TC UP Event", 0, 0, 0);
    tm_1x_up_session_info.tm_cm_register_bmap &= (tm_cm_register_events_bmap_e_type)~(TM_CM_REGISTER_TC_UP_EVENT); /*lint !e641 !e655 */
    (void) tm_cm_iface_handle_reg_events_req(tm_1x_up_session_info.tm_cm_register_bmap);

    MSG_HIGH("Exit dormancy wait state",0,0,0);
    (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_connect_timer);

    if (tm_1x_up_session_info.mgp_info.ppm_on == TRUE)
    {
      /* Start the trigger timer for PPM */
      if (tm_1x_up_is801_drv_get_ppm() == FALSE )
      {
        MSG_ERROR("Can't req PPM",0,0,0);
        tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
      }
      else
      {
        if ((tm_1x_up_is801_drv_session_active() == TRUE) && (tm_1x_up_session_info.session_state == TM_1X_UP_SESSION_STATE_WAIT_FD))
        {
          tm_1x_up_is801_drv_update_session_state(TM_1X_UP_SESSION_STATE_ENTRY);
        }
        else
        {
          MSG_ERROR("Wrong state",0,0,0);
          tm_1x_up_is801_drv_handle_dormancy_failure(TM_1X_UP_DORMANCY_FAILURE_TYPE_ABORT);
        }
        /* Reset flag to avoid FD again */
        tm_1x_up_session_info.bs_info.bs_info_update_reqd = FALSE;
      }
    }
  }
  else
  {
    /* In all other cases ignore TC UP event.
    */
    MSG_HIGH("Ignore TC UP",0,0,0);
  }
#endif /* FEATURE_GPSONE_UPDATE_BSID */
} /* End tm_1x_up_is801_drv_handle_tc_up() */

#ifdef FEATURE_GPSONE_UPDATE_BSID
/*===========================================================================

FUNCTION tm_1x_up_is801_drv_force_dormancy

DESCRIPTION
  This function is used to force DS socket into dormancy.

RETURN VALUE
  boolean : True if force dormancy succeeded this has been added to support
  the new IOCTL interface.

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_is801_drv_force_dormancy(void)
{
  boolean ret_val;

  /* Perform Force Dormancy to update BS_ID;
  */
  ret_val = tm_1x_up_comm_force_dormancy();

  /* If we were successful in forcing dormancy continue with changing state
  ** else, return
  */

  if( ret_val == FALSE )
  {
    MSG_ERROR("Unable to force dormancy",0,0,0);
  }
  else
  {
    /*Change Session state to Waiting for Force Dormancy state. */
    tm_1x_up_is801_drv_update_session_state(TM_1X_UP_SESSION_STATE_WAIT_FD);

    event_report(EVENT_GPS_DORMANCY_BEGIN);

    /* Start recovery timer */
    if(!os_TimerStart(tm_1x_up_session_info.tm_1x_up_connect_timer,TM_1X_UP_FORCE_DORMANCY_RECONNECT_TIMEOUT,NULL))
    {
      MSG_ERROR("Failed starting Dormancy timer",0,0,0);
    }
  }

  return ret_val;
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_force_dormancy_failed_handler

DESCRIPTION
  Force dormancy attempt is failed, usually due to QNC or voice call.
  Remember the force dormancy is failed and therefore BS Info is
  stale. Fall back to 1X UP state machine to select next possible mode
  or bail out.

RETURN VALUE
  None

DEPENDENCIES
  None.

===========================================================================*/
void tm_1x_up_is801_drv_handle_dormancy_failure(tm_1x_up_dormancy_failure_e_type failure_type)
{
  MSG_HIGH("Force dormancy attempt failed",0,0,0);

  /* Deregister TC Exit/Up events */
  MSG_HIGH("DeRegister TC Exit/UP Event", 0, 0, 0);
  tm_1x_up_session_info.tm_cm_register_bmap &=
      (tm_cm_register_events_bmap_e_type)~(TM_CM_REGISTER_TC_UP_EVENT|TM_CM_REGISTER_TC_DOWN_EVENT); /*lint !e641 !e655 */
  (void) tm_cm_iface_handle_reg_events_req(tm_1x_up_session_info.tm_cm_register_bmap);

  /* Clear DS timer */
  (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_connect_timer);

  if(failure_type == TM_1X_UP_DORMANCY_FAILURE_TYPE_OPN_COMPLETE)
  {
    if (tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_MSA)
    {
      tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_MSA;
    }
    else if (tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_DLOAD)
    {
      tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_DLOAD;
    }
    tm_1x_up_is801_drv_opn_completed(PDSM_PD_END_STALE_BS_INFO);
  }
  else if(failure_type == TM_1X_UP_DORMANCY_FAILURE_TYPE_ABORT)
  {
    tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
  }
} /* tm_1x_up_is801_drv_force_dormancy_failed_handler() */

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_force_dormancy_in_ppm_resp

DESCRIPTION
  This helper function for tm_1x_up_bs_info_update_proc_in_ppm_res() to
  force dormancy if necessary.

RETURN VALUE
  Boolean

DEPENDENCIES
  tm_1x_up_session_info.bs_info.bs_info_update_reqd

  Helper function of pdsm_bs_info_update_proc_in_ppm_res().
  Should be used only in pdsm_bs_info_update_proc_in_ppm_res()

===========================================================================*/
static boolean tm_1x_up_is801_drv_force_dormancy_in_ppm_resp(void)
{
  boolean retval = TRUE;

  switch (tm_1x_up_session_info.session_state) 
  {
    case TM_1X_UP_SESSION_STATE_ENTRY:
      if ( tm_1x_up_session_info.bs_info.bs_info_update_reqd == TRUE &&
           (tm_1x_up_is801_drv_session_active()))
      {
        tm_1x_up_session_info.bs_info.bs_info_update_reqd = FALSE;

        MSG_HIGH("FD required",0,0,0);
        if ( tm_1x_up_is801_drv_force_dormancy() )
        {
          MSG_HIGH("Register and Wait for TC_EXIT event",0,0,0);
          tm_1x_up_session_info.tm_cm_register_bmap |= TM_CM_REGISTER_TC_DOWN_EVENT; /*lint !e641 !e655 */
          (void) tm_cm_iface_handle_reg_events_req(tm_1x_up_session_info.tm_cm_register_bmap);
          retval = FALSE;

        }
        else
        {
          tm_1x_up_is801_drv_handle_dormancy_failure(TM_1X_UP_DORMANCY_FAILURE_TYPE_OPN_COMPLETE);
          retval = FALSE;
        }
      }
      else
      {
        MSG_HIGH("FD not required",0,0,0);
      }
      break;

    default:
      /* pass through - no force dormancy expected
      */
      break;
  }

  return retval;

} /* tm_1x_up_is801_drv_force_dormancy_in_ppm_resp */

#endif /* FEATURE_GPSONE_UPDATE_BSID */

/* START EXT_MEAS_PPM */
/*===========================================================================

FUNCTION tm_1x_up_init_ext_meas_ppm_data

DESCRIPTION

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

void tm_1x_up_init_ext_meas_ppm_data(srch_ppm_rpt_type *ppm_ptr)
{
  int i;
  mcc_pd_bs_info_type   is801_bs_info;
  pdsm_srch_ppm_rpt_type      tm_1x_up_ext_ppm_data;

  if(ppm_ptr == NULL)
  {
    MSG_ERROR("Null PPM ptr",0,0,0);
    return;
  }
  /* Obtain latest BS info.
  */
  memset(&is801_bs_info, 0, sizeof(is801_bs_info));
  (void) mcc_pd_get_bs_info( &is801_bs_info );

  tm_1x_up_ext_ppm_data.total_rx_pwr     = ppm_ptr->total_rx_pwr;
  tm_1x_up_ext_ppm_data.ts[0]            = ppm_ptr->ts[0];
  tm_1x_up_ext_ppm_data.ts[1]            = ppm_ptr->ts[1];
  tm_1x_up_ext_ppm_data.offset_incl      = ppm_ptr->offset_incl;
  tm_1x_up_ext_ppm_data.mob_sys_t_offset = ppm_ptr->mob_sys_t_offset;
  tm_1x_up_ext_ppm_data.ref_pn           = ppm_ptr->ref_pn;
  tm_1x_up_ext_ppm_data.ref_pn_ecio      = ppm_ptr->ref_pn_ecio;
  tm_1x_up_ext_ppm_data.num_pilots       = ppm_ptr->num_pilots;

  for ( i=0; i < ppm_ptr->num_pilots; i++ )
  {
    tm_1x_up_ext_ppm_data.pilot_results[i].pilot_pn_phase = ppm_ptr->pilot_results[i].peak_pos;
    tm_1x_up_ext_ppm_data.pilot_results[i].pilot_ecio     = ppm_ptr->pilot_results[i].peak_ecio;
    tm_1x_up_ext_ppm_data.pilot_results[i].pilot_rmse     = ppm_ptr->pilot_results[i].peak_rmse;
  }

  /* Now fill BS information and additional information.
  */
  tm_1x_up_ext_ppm_data.band_class       = is801_bs_info.band_class;
  tm_1x_up_ext_ppm_data.cdma_freq        = is801_bs_info.cdma_ch;
  tm_1x_up_ext_ppm_data.base_id          = is801_bs_info.base_id;
  tm_1x_up_ext_ppm_data.sid              = is801_bs_info.sid;
  tm_1x_up_ext_ppm_data.nid              = is801_bs_info.nid;

  tm_1x_up_ext_ppm_data.add_band_class   = tm_1x_up_ext_ppm_data.band_class;
  tm_1x_up_ext_ppm_data.add_cdma_freq    = tm_1x_up_ext_ppm_data.cdma_freq;

  (void)tm_post_ext_meas_rep_event_deprecated(
                             TM_PRTL_TYPE_1X_UP,
                             tm_1x_up_session_info.sess_handle,
                             PDSM_EXT_STATUS_EVENT_MEASUREMENT, 
                             &tm_1x_up_ext_ppm_data,
                             1);
} /* end tm_1x_up_init_ext_meas_ppm_data() */

/* END EXT_MEAS_PPM */
/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_ppm

DESCRIPTION
  This function handle PPM results from Searcher. If the session manager is in
  Entry state, it sends the MS info, PPM results, along with the request for
  aiding data. If user wants low accuracy fix, the Session manager request's
  PDE for Location response. Once PPM results are obtained after GPS results are
  obtained PPM results are send to PDE along with a request for getting Postion
  information.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_is801_drv_handle_ppm(srch_ppm_rpt_type *ppm_ptr )
{
  boolean ret_val = FALSE;

  (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_mgp_timer);
#ifdef FEATURE_GPSONE_UPDATE_BSID
  if ( (tm_1x_up_is801_drv_session_active()) && !tm_1x_up_is801_drv_force_dormancy_in_ppm_resp())
  {
    MSG_MED("FD for BS info update required",0,0,0);
    return TRUE;
  }
#endif /* FEATURE_GPSONE_UPDATE_BSID */

  MSG_HIGH("Handling PPM response from SRCH",0,0,0);

  if(tm_1x_up_is801_drv_session_active())
  {
    MSG_HIGH("Handling ppm in active session",0,0,0);
    if (tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_MSA)
    {
      ret_val = tm_1x_up_is801_drv_handle_ppm_ms_assist(ppm_ptr);
      tm_1x_up_init_ext_meas_ppm_data(ppm_ptr); /* start ext_meas_ppm */
    } /* MS-Assisted session */
    else if (tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_DLOAD)
    {
      ret_val = tm_1x_up_is801_drv_handle_ppm_dload(ppm_ptr);
      if(tm_1x_up_session_info.mgp_info.ppm_on == TRUE)
      {
        (void)tm_1x_up_is801_drv_end_ppm();
      } /* if PPM on in MS-Based session */
      tm_1x_up_init_ext_meas_ppm_data(ppm_ptr); /* start ext_meas_ppm */
    }
    else 
    {
      MSG_ERROR("PPM not expected. END PPM if On",0,0,0);
      if(tm_1x_up_session_info.mgp_info.ppm_on == TRUE)
      {
        (void)tm_1x_up_is801_drv_end_ppm();
      } /* if PPM on in MS-Based session */
    }
  } /* if session active */
  else
  {
    MSG_ERROR("PPM resp not expected",0,0,0);
    if ( tm_1x_up_session_info.mgp_info.ppm_on)
    {
      (void)tm_1x_up_is801_drv_end_ppm();
    }
  } /* end else PPM resp states */
  return ret_val;
} /* end tm_1x_up_is801_drv_handle_ppm() */

/* perform a sanity check on the input params and init the protocol type */
#define INIT_UNIFIED_RESP(p_801_1_resp, p_801_b_resp, unified_resp)            \
do {                                                                           \
  if( (p_801_1_resp == NULL && p_801_b_resp == NULL) ||                        \
      (p_801_1_resp != NULL && p_801_b_resp != NULL) )                         \
  {                                                                            \
    MSG_ERROR("Unexpected resp_ptrs. 801_1 %x 801_b %x",                       \
                      p_801_1_resp, p_801_b_resp, 0);                          \
    tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);              \
    return;                                                                    \
  }                                                                            \
  else if(p_801_1_resp != NULL)                                                \
  {                                                                            \
    unified_resp.prot = IS801_VERSION_801_1;                                   \
  }                                                                            \
  else                                                                         \
  {                                                                            \
    unified_resp.prot = IS801_VERSION_801_B;                                   \
  }                                                                            \
}while(0);                                                                     \

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_aa_resp

DESCRIPTION
  This function is used to handle AA response from PDE. AA data is stored
  in a dynamic buffer and passed to Searcher once SA data is received. AA
  data is received once MI session is going on.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_handle_aa_resp
( 
  is801_fwd_link_resp_s_type  *resp_ptr, 
  is801b_flink_ie_gnss_aa     *aa_801_B
)
{
  static is801_unified_aa_resp_type         aa;
  static const tm_1x_up_rlink_req_type      req_mask = TM_1X_UP_RLINK_REQ_AA;

  static const prtl_start_sess_req_param_s_type  *pdapi_info_ptr = 
      &(tm_1x_up_session_info.api_param.pd_api_info);

  is801_operation  aa_resp_ops[]  =  
    { CLR_REQ_MASK,            MGP_POST_PREFIX_PPM_BUF,  MGP_START_PRM,   
      MGP_POST_AA_AND_SV_DIR,  OPS_END };   
  const void*      aa_resp_par[]  =  
    { &req_mask,               NULL,                     &pdapi_info_ptr, 
      &aa     };
        
  is801_operation    *ops    = aa_resp_ops;
  void               *params = aa_resp_par;

  MSG_HIGH("Handling AA data. 801-1 Resp %x 801-B Resp %x", 
                     resp_ptr, aa_801_B, 0);

  /* perform a sanity check on the input params and init the protocol type */
  INIT_UNIFIED_RESP( resp_ptr, aa_801_B, aa );

  if(resp_ptr != NULL)
  {
    aa.u.aa_801_1  = &(resp_ptr->resp_msg_payload.aa_resp);
  }
  else
  {
    aa.u.aa_801_B  = aa_801_B;
  }

  if ( tm_1x_up_session_info.session_state != TM_1X_UP_SESSION_STATE_WAIT_AA )
  {
    MSG_ERROR("AA data in wrong state %d",
                     tm_1x_up_session_info.session_state, 0, 0);
    return;
  }

  MSG_MED("Transition to WAIT_PR", 0, 0, 0);
  if( tm_state_transition(ops, params, TM_1X_UP_SESSION_STATE_WAIT_PR) == FALSE)
  {
    MSG_ERROR("State Transition to WAIT_PR failed", 0, 0, 0);
  }
}/* End tm_1x_up_is801_drv_handle_aa_resp() */

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_gps_sa_resp

DESCRIPTION
  This function is used to handle SA response from PDE

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_handle_gps_sa_resp
( 
  is801_fwd_link_resp_s_type *resp_ptr, 
  is801b_flink_ie_gps_sa     *sa_801_B 
)
{
  static is801_unified_gps_sa_resp_type   sa;
  static const tm_1x_up_rlink_req_type    req_mask = TM_1X_UP_RLINK_REQ_GPS_SA;

  is801_operation   sa_resp_ops[]  =  { CLR_REQ_MASK,  MGP_POST_SA_PTR, OPS_END};   
  const void*       sa_resp_par[]  =  { &req_mask,     &sa     };
        
  is801_operation  *ops            =  sa_resp_ops;
  void             *params         =  sa_resp_par;
  uint32            data_record_index;
  
  #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#endif

  MSG_HIGH("Handling SA data. 801-1 Resp %x 801-B Resp %x", 
                    resp_ptr, sa_801_B, 0);
  /* perform a sanity check on the input params and init the protocol type */
  INIT_UNIFIED_RESP(resp_ptr, sa_801_B, sa);

  if(resp_ptr != NULL)
  {
    sa.u.sa_801_1  = &(resp_ptr->resp_msg_payload.sa_resp);
    /* Report EVENT SA received
     * data_record_index does not need to be decremented by 1, since the 801-1
     * protocol does it for us.
    */
    data_record_index = resp_ptr->resp_msg_payload.sa_resp.num_data_records;

    sm_report_event_gps_sa_received(
        resp_ptr->resp_msg_payload.sa_resp.data[data_record_index].ref_bit_num,
        resp_ptr->resp_msg_payload.sa_resp.data[data_record_index].dr_size);
  }
  #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#endif

  /* Now we received SA response from PDE. If PRM is still on we should hand over the
  ** data to  lower layers, else it is too late.
  */
  if ( tm_1x_up_session_info.session_state != TM_1X_UP_SESSION_STATE_WAIT_SA )
  {
    MSG_ERROR("SA data in wrong state %d",tm_1x_up_session_info.session_state,0,0);
    return;
  }
  
  MSG_MED("Transition to WAIT_PR", 0, 0, 0);
  if( tm_state_transition(ops, params, TM_1X_UP_SESSION_STATE_WAIT_PR) == FALSE)
  {
    MSG_ERROR("State Transition to WAIT_PR failed", 0, 0, 0);
  }
} /* End tm_1x_up_is801_drv_handle_gps_sa_resp() */


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_jgps_get_timestamp

DESCRIPTION
  This function is used to get timestamp in JST format.

RETURN VALUE
  None.

DEPENDENCIES
===========================================================================*/
void tm_1x_up_is801_drv_jgps_get_timestamp
(
  void
)
{
  dword  systemtime; /* CDMA system timestamp  */

  systemtime = time_get_secs();

  /* Get the time stamp */
  time_jul_from_secs( 
                     systemtime, 
                     (time_julian_type *)&tm_1x_up_dload_info.jgps_msdload_info.timestamp
                    );

} /* tm_1x_up_is801_drv_jgps_get_timestamp */


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_jgps_set_dload_info

DESCRIPTION
  This function is used to set DLOAD data type, timestamp and run flag

RETURN VALUE
  None.

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_set_dload_info
(
  unsigned int        dload_data,
  pdsm_pd_end_e_type   error
)
{

  if(( error == PDSM_PD_END_SESS_NONE) || ( error == PDSM_PD_END_PDE_REJECT)) 
  {
    /* Dload success */
    tm_1x_up_dload_info.jgps_msdload_info.run = TRUE;

    tm_1x_up_is801_drv_jgps_get_timestamp( );

    /* SEED Position */
    if( dload_data & TM_1X_UP_RLINK_REQ_LR)
    {
      tm_1x_up_dload_info.jgps_msdload_info.data_type |= PDSM_PD_DLOAD_LOCATION;
    }

    /* Ephemeris */
    if( dload_data & TM_1X_UP_RLINK_REQ_GPS_EPH)
    {
      tm_1x_up_dload_info.jgps_msdload_info.data_type |= PDSM_PD_DLOAD_EPHEMERIS;
    }

    /* Almanac */
    if( dload_data & TM_1X_UP_RLINK_REQ_GPS_ALM)
    {
      tm_1x_up_dload_info.jgps_msdload_info.data_type |= PDSM_PD_DLOAD_ALMANAC;
    }

    /* AA */
    if( dload_data & IS801_FWD_LINK_RESP_MASK_AA) 
    { 
      tm_1x_up_dload_info.jgps_msdload_info.data_type |= PDSM_PD_DLOAD_AA; 
    } 

    /* SA */
    if( dload_data & IS801_FWD_LINK_RESP_MASK_SA) 
    { 
      tm_1x_up_dload_info.jgps_msdload_info.data_type |= PDSM_PD_DLOAD_SA;
    }

  }
} /* tm_1x_up_is801_drv_jgps_set_dload_info */


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_assistance_data

DESCRIPTION
  This function is used to handle Assistance Data responses from PDE.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_handle_assistance_data
( 
  tm_1x_up_rlink_req_type                resp_type, 
  byte                                   sess_end,
  byte                                   session_tag,
  pdsm_pd_end_e_type                     status 
)
{
  uint32 eph_unhealthy_list;

  MSG_HIGH("Resp from PDE=%d",resp_type,0,0);

  /* Populate the dload info for JGPS module, regardless if JGPS module is defined */
  tm_1x_up_is801_drv_set_dload_info(resp_type, status);
  
  /* First handle individual data from PDE. no check for state required(already done)
  */
  switch(resp_type)
  {
    case TM_1X_UP_RLINK_REQ_LR:
    case TM_1X_UP_RLINK_REQ_GLO_EPH:
    case TM_1X_UP_RLINK_REQ_GLO_ALM:
    case TM_1X_UP_RLINK_REQ_GGTO:
    case TM_1X_UP_RLINK_REQ_ADV_SVHI:
    case TM_1X_UP_RLINK_REQ_DGNSS_ASST:
    case TM_1X_UP_RLINK_REQ_GPS_EPH:
    case TM_1X_UP_RLINK_REQ_GPS_ALM:
    case TM_1X_UP_RLINK_REQ_AA:
    case TM_1X_UP_RLINK_REQ_GPS_SA:
      tm_1x_up_session_info.is801_req_wait_bmap &= ~(resp_type);
    break;

    default:
      MSG_ERROR("Response not expected",0,0,0);
      break;
  } /* switch end resp_type */

  if (tm_1x_up_session_info.is801_req_wait_bmap == NULL)
  {
    MSG_HIGH("All responses processed",0,0,0);

    if (tm_1x_up_session_info.session_state == TM_1X_UP_SESSION_STATE_WAIT_DLOAD_RESP)
    {
      /* Dload is over, if success erase the last error type.
      */
      if (status == PDSM_PD_END_SESS_NONE)
      {
        if (tm_1x_up_session_info.mode_info.last_error == TM_1X_UP_MODE_DLOAD)
        {
          /* clear the last error type.
          */
          tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_NONE;
        }

        if(tm_1x_up_session_info.api_param.pd_api_info.pd_option.session == PDSM_PD_SESS_TYPE_DATA_DLOAD)
        {
          /* Send DLOAD event */
          tm_1x_up_info_buffer.client_id = tm_1x_up_session_info.api_param.pd_api_info.client_id;
          (void)tm_post_pd_event( TM_PRTL_TYPE_1X_UP,
                                  tm_1x_up_session_info.sess_handle,
                                  PDSM_PD_EVENT_DLOAD,
                                  &tm_1x_up_info_buffer,
                                  NULL);
        }
      } /* status = TRUE */
    } /* end if DLOAD_RESP */

    /* Change the state from Dload operation.
    */
    tm_1x_up_is801_drv_update_session_state(TM_1X_UP_SESSION_STATE_WAIT_NEXT_EVENT);

    if( sess_end == 0x00)
    {
      MSG_HIGH("Send End sess to PDE",0,0,0);
      tm_1x_up_is801_drv_end_session_with_pde(session_tag);
    }

    /* Determine if we have enough info. about some SVs' nonexistence and
       inform MGP about such info., if available */
    tm_util_post_non_exist_svs ( 
             TM_PRTL_TYPE_1X_UP, 
             tm_1x_up_session_info.sess_handle,
             tm_1x_up_session_info.aiding_data_status.gps_alm_delivered_sv_list, 
             tm_1x_up_session_info.aiding_data_status.gps_eph_delivered_sv_list 
             );

    /* unhealthy list is created based on what was requested to the server
       and what was returned from the server. any SVs not returned by the 
       server is treated as unhealthy 
    */
    eph_unhealthy_list = (tm_1x_up_session_info.aiding_data_status.gps_eph_requested_sv_list) &  
                                (~tm_1x_up_session_info.aiding_data_status.gps_eph_delivered_sv_list);
    MSG_HIGH("unhealthy list 0x%1x, delivered 0x%1x, requested 0x%1x",eph_unhealthy_list,
      tm_1x_up_session_info.aiding_data_status.gps_eph_delivered_sv_list,
      tm_1x_up_session_info.aiding_data_status.gps_eph_requested_sv_list);
    
    /* Determine if we have enough info. about some SVs' health and
       inform MGP about such info., if available */
    tm_util_post_unhealthy_svs ( TM_PRTL_TYPE_1X_UP, 
                                tm_1x_up_session_info.sess_handle,
                                eph_unhealthy_list
                               );

    /* Fail the operation if Ephemeris rejected even if other assistance data is good.
    */
    if (tm_1x_up_session_info.is801_eph_reject == TRUE)
    {
      tm_1x_up_is801_drv_opn_completed(PDSM_PD_END_PDE_REJECT);
      tm_1x_up_session_info.is801_eph_reject = FALSE;
    }
    else
    {
      tm_1x_up_is801_drv_opn_completed(status);
    }
  } /* bmap == NULL */
} /* End tm_1x_up_is801_drv_handle_assistance_data() */


#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
#error code not present
#endif /* FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE */

#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
#error code not present
#endif /* FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE */

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_gps_alm_resp

DESCRIPTION
  This function is used to handle Almanac response from PDE

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

void tm_1x_up_is801_drv_handle_gps_alm_resp
( 
  is801_fwd_link_pddm_info_s_type *resp_ptr, 
  uint8 index,
  is801b_flink_ie_gps_alm *alm_801_B  
)
{
  static is801_unified_gps_alm_resp_type    alm;

  is801_operation  alm_resp_sol_ops[]  =  { MGP_POST_ALM, OPS_END };   
  const void*      alm_resp_sol_par[]  =  { &alm };
        
  uint32  is801_req_wait_bmap  = tm_1x_up_session_info.is801_req_wait_bmap;
  tm_1x_up_session_state_e_type  current_state  
                               = tm_1x_up_session_info.session_state;
        
  is801_operation *ops    = alm_resp_sol_ops;
  void            *params = alm_resp_sol_par;
  boolean          unsolicited;
  byte             sess_end = 0;
  byte             session_tag = 0;

  MSG_HIGH("Handling GPS Alm data. 801-1 Resp %x 801-B Resp %x",
                  resp_ptr, alm_801_B, 0);
  /* perform a sanity check on the input params and init the protocol type */
  INIT_UNIFIED_RESP(resp_ptr, alm_801_B, alm);

  /* Set alm param pointer based on protocol version in operation. */
  if(resp_ptr != NULL)
  {
    alm.u.alm_801_1  = &(resp_ptr->resp_rd[index].resp_msg_payload.alm_resp);
    unsolicited      = resp_ptr->resp_rd[index].unsolicited;
    sess_end         = resp_ptr->hdr.sess_end;
    session_tag      = resp_ptr->hdr.sess_tag;
  }
  
  #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#else
  else
  {
    return;
  }
  #endif  

  /* If response is a solicited response, or if we are waiting for almanac, 
   * (i.e. we expect the response and the pddm is solicited)
   * AND state is not WAIT_DLOAD_RESP, state machine must throw an error 
   */
  if( ((unsolicited  != 0x01) || 
       ((is801_req_wait_bmap & TM_1X_UP_RLINK_REQ_GPS_ALM) != 0)) &&
      (current_state != TM_1X_UP_SESSION_STATE_WAIT_DLOAD_RESP)  
    )
  {
    MSG_ERROR("Resp update error,state=%d,mask=%d",
                     current_state, is801_req_wait_bmap,0);
    return;
  }

  if ( ((is801_req_wait_bmap & TM_1X_UP_RLINK_REQ_GPS_ALM) == 0) && 
       (unsolicited == 0x01) )
  {
    MSG_HIGH("Update TM-Core with Unsol ALM resp",0,0,0);
  }
  else if(current_state == TM_1X_UP_SESSION_STATE_WAIT_DLOAD_RESP)
  {
    ops    = alm_resp_sol_ops;
    params = alm_resp_sol_par;
    if ( !(is801_req_wait_bmap & TM_1X_UP_RLINK_REQ_GPS_ALM) )
    {
      MSG_ERROR("Response not requested(mask=%d)", is801_req_wait_bmap,0,0);
    } /* end if ALM_REQ */
  } /* end of DLAOD_RESP state */


  if( tm_1x_up_execute_fns(ops, params) == FALSE)
  {
    tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
    return;
  }

  if(current_state == TM_1X_UP_SESSION_STATE_WAIT_DLOAD_RESP)
  {
    tm_1x_up_is801_drv_handle_assistance_data( TM_1X_UP_RLINK_REQ_GPS_ALM,
                                               sess_end, session_tag,
                                               PDSM_PD_END_SESS_NONE );
  } /* end of DLAOD_RESP state */
} /* End handle_gps_alm_resp() */

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_gps_eph_resp

DESCRIPTION
  This function is used to handle 801-1 and 801-B GPS Ephemeris response from 
  PDE. The actions taken are:~
  1) Send the Ephemeris to MGP
  2) If this was the last request to be sent by the server, handle end of 
     session processing.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_handle_gps_eph_resp
( 
  is801_fwd_link_pddm_info_s_type *resp_ptr, 
  uint8 index,  
  is801b_flink_ie_gps_eph *eph_801_B
)
{
  static is801_unified_gps_eph_resp_type    eph;

  is801_operation  eph_resp_sol_ops[]  =  { MGP_POST_EPH_IONO, OPS_END };   
  const void*      eph_resp_sol_par[]  =  { &eph };
        
  uint32  is801_req_wait_bmap  = tm_1x_up_session_info.is801_req_wait_bmap;
  tm_1x_up_session_state_e_type  current_state  
                               = tm_1x_up_session_info.session_state;
        
  is801_operation *ops         = eph_resp_sol_ops;
  void            *params      = eph_resp_sol_par;
  boolean          unsolicited;
  byte             sess_end = 0;
  byte             session_tag = 0;

  MSG_HIGH("Handling GPS Eph data. 801-1 Resp %x 801-B Resp %x",
                                 resp_ptr, eph_801_B, 0);

  /* perform a sanity check on the input params and init the protocol type */
  INIT_UNIFIED_RESP(resp_ptr, eph_801_B, eph);  

  if(resp_ptr != NULL)
  {
    eph.u.eph_801_1  = &(resp_ptr->resp_rd[index].resp_msg_payload.eph_resp);
    unsolicited      = resp_ptr->resp_rd[index].unsolicited;    
    sess_end         = resp_ptr->hdr.sess_end;
    session_tag      = resp_ptr->hdr.sess_tag;
  }
  #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#else
  else
  {
    return; /* resp_ptr == NULL is only for IS801_B */
  }  
  #endif 

  /* If response is a solicited response, or if we are waiting for ephemeris, 
   * (i.e. we expect the response and the pddm is solicited)
   * AND state is not WAIT_DLOAD_RESP, state machine must throw an error 
   */
  if( ((unsolicited != 0x01) || 
       ((is801_req_wait_bmap & TM_1X_UP_RLINK_REQ_GPS_EPH) != 0) ) &&
      (current_state != TM_1X_UP_SESSION_STATE_WAIT_DLOAD_RESP) 
    )
  {
    MSG_ERROR("Resp update error,state=%d,mask=%d",
                       current_state, is801_req_wait_bmap,0);
    return;
  }

  if ( (!(is801_req_wait_bmap & TM_1X_UP_RLINK_REQ_GPS_EPH)) && 
         (unsolicited == 0x01) )
  {
    ops    = eph_resp_sol_ops;
    params = eph_resp_sol_par;
  }
  else if(current_state == TM_1X_UP_SESSION_STATE_WAIT_DLOAD_RESP)
  {
    if ( !(is801_req_wait_bmap & TM_1X_UP_RLINK_REQ_GPS_EPH) )
    {
      MSG_ERROR("Response not requested(mask=%d)",is801_req_wait_bmap,0,0);
    }
  } /* end of DLAOD_RESP state */

  if( tm_1x_up_execute_fns(ops, params) == FALSE)
  {
    tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
    return;
  }

  if(current_state == TM_1X_UP_SESSION_STATE_WAIT_DLOAD_RESP)
  {
    tm_1x_up_is801_drv_handle_assistance_data(
                    TM_1X_UP_RLINK_REQ_GPS_EPH,
                    sess_end, session_tag,
                    PDSM_PD_END_SESS_NONE);
  } /* end of DLAOD_RESP state */
} /* End handle_eph_resp() */


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_lr_resp_dload

DESCRIPTION
  This function is used to handle LR response from 801-1 or 801-B PDE.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

void tm_1x_up_is801_drv_handle_lr_resp_dload
(
  is801_fwd_link_pddm_info_s_type *resp_ptr, 
  uint8                            index ,
  is801b_flink_ie_adv_lr          *lr_801_B
)
{
  static is801_unified_loc_resp_type       locn;

  is801_operation  lr_resp_ops[]    =  { MGP_POST_SEED, SAVE_SEED, OPS_END };   
  const void*      lr_resp_par[]    =  { &locn,         &locn };
        
  is801_operation *ops    = lr_resp_ops;
  void            *params = lr_resp_par;
  byte             sess_end = 0;
  byte             session_tag = 0;

  MSG_HIGH("LR resp for Dload only",0,0,0);

  if( (resp_ptr == NULL && lr_801_B == NULL) ||
      (resp_ptr != NULL && lr_801_B != NULL) )
  {
    MSG_ERROR("Inconsistent location resp pointers 801-1 %x 801-B %x",
                       resp_ptr, lr_801_B, 0);
    tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
    return;
  }

  if(tm_1x_up_session_info.session_state != 
                       TM_1X_UP_SESSION_STATE_WAIT_DLOAD_RESP)
  {
    MSG_ERROR("Dload LR not expected in state %d",
                       tm_1x_up_session_info.session_state,0,0);
    return;
  } /* if DLOAD_RESP */

  /* perform a sanity check on the input params and init the protocol type */
  INIT_UNIFIED_RESP(resp_ptr, lr_801_B, locn);  

  if(resp_ptr != NULL)
  {
    sess_end         = resp_ptr->hdr.sess_end;
    session_tag      = resp_ptr->hdr.sess_tag;
    locn.u.lr_801_1  = &(resp_ptr->resp_rd[index].resp_msg_payload.lr_resp);
  }
  #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#else
  else
  {
    MSG_ERROR("Resp pointer is null",0,0,0);
    return; /* resp_ptr == NULL is for IS801_B only */
  }  
  #endif  


  if( tm_1x_up_execute_fns(ops, params) == FALSE)
  {
    tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
    return;
  }

  tm_1x_up_is801_drv_handle_assistance_data(
                  TM_1X_UP_RLINK_REQ_LR,
                  sess_end, session_tag,
                  PDSM_PD_END_SESS_NONE);
}

tm_1x_up_rlink_req_type xlate_is801_1_rej_resp_to_1x_up_req
(
  const is801_fwd_link_resp_s_type *resp_ptr
)
{
  switch(resp_ptr->resp_msg_payload.rej_resp.rej_req_type)
  {
    case LOC_RESP_REQ_CODE:
      return TM_1X_UP_RLINK_REQ_LR;

    case GPS_AA_REQ_CODE:
      return TM_1X_UP_RLINK_REQ_AA;

    case GPS_SA_REQ_CODE:
      return TM_1X_UP_RLINK_REQ_GPS_SA;

    case GPS_ALM_REQ_CODE:
      return TM_1X_UP_RLINK_REQ_GPS_ALM;

    case GPS_EPH_REQ_CODE:
      return TM_1X_UP_RLINK_REQ_GPS_EPH;

    default:
      return TM_1X_UP_RLINK_REQ_NONE;
  }
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_rej_resp

DESCRIPTION
  This function is used to handle reject reponse from PDE. The session proceeds
  only when we are rejected for SA data, for all other cases we abort the session.

RETURN VALUE
  None

DEPENDENCIES
TBD Handle rej responce from IS801B
===========================================================================*/
void tm_1x_up_is801_drv_handle_rej_resp
(  
  //const is801_fwd_link_pddm_info_s_type *resp_ptr, 
  //uint8 index
  tm_1x_up_rlink_req_type  rej_req_type,
  byte                     sess_end,
  byte                     session_tag
)
{
  tm_post_data_payload_type        *aiding_data_ptr = NULL;
#if 0
  if(resp_ptr == NULL)
  {
    MSG_ERROR("Null resp_ptr",0,0,0);
    return;
  }

  if(resp_ptr != NULL)
  {
    sess_end         = resp_ptr->hdr.sess_end;
    sess_tag         = resp_ptr->hdr.sess_tag;
    rej_req_type     = resp_ptr->resp_rd[index].resp_msg_payload.rej_resp.rej_req_type;
  }
#endif
  switch( rej_req_type )
  {
    case TM_1X_UP_RLINK_REQ_LR:

      tm_1x_up_session_info.is801_req_wait_bmap &= ~(TM_1X_UP_RLINK_REQ_LR);

      if (tm_1x_up_session_info.session_state == TM_1X_UP_SESSION_STATE_WAIT_DLOAD_RESP)
      {
        tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_DLOAD;
        tm_1x_up_is801_drv_handle_assistance_data(TM_1X_UP_RLINK_REQ_LR, 
                                                   sess_end, session_tag,
                                                   PDSM_PD_END_PDE_REJECT);
      }
      else if (tm_1x_up_session_info.session_state == TM_1X_UP_SESSION_STATE_WAIT_PD)
      {
        /* Update a failure in MS-Assisted sessions.
        */
        tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_MSA;
        /* Send the End session and proceed.
        */
        tm_1x_up_is801_drv_handle_assistance_data(TM_1X_UP_RLINK_REQ_LR,
                                                  sess_end, session_tag,
                                                  PDSM_PD_END_PDE_REJECT);
      }
      else
      {
        MSG_ERROR("Reject for LR not expected",0,0,0);
      }
    break;

  case TM_1X_UP_RLINK_REQ_AA:
      tm_1x_up_session_info.is801_req_wait_bmap &= ~(TM_1X_UP_RLINK_REQ_AA);

      if ( tm_1x_up_session_info.session_state == TM_1X_UP_SESSION_STATE_WAIT_AA )
      {
        MSG_ERROR("AA resp rejected",0,0,0);
        /* Update a failure in MS-Assisted sessions.
        */
        tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_MSA;
        tm_1x_up_is801_drv_handle_assistance_data(TM_1X_UP_RLINK_REQ_AA,
                                                  sess_end, session_tag,
                                                  PDSM_PD_END_PDE_REJECT);
      }
      else
      {
        MSG_ERROR("Reject for AA not exp",0,0,0);
      }
      if ( tm_1x_up_session_info.is801_req_wait_bmap == NULL )
      {
        /* No further responses, clear the timer.
        */
        (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_is801_timer);  
      }
    break;

  case TM_1X_UP_RLINK_REQ_GPS_SA:
    MSG_HIGH("SA request Rejected",0,0,0);
    if ( tm_1x_up_session_info.is801_req_wait_bmap & TM_1X_UP_RLINK_REQ_GPS_SA )
    {
      tm_1x_up_session_info.is801_req_wait_bmap &= ~(TM_1X_UP_RLINK_REQ_GPS_SA);
      if ( tm_1x_up_session_info.is801_req_wait_bmap == NULL )
      {
        /* No further IS801 requests's are pending, so clear the IS801 timer.
        */
        (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_is801_timer);  
      }
      /* Searcher is waiting for SA, proceed with rejecting the request and
      ** allow the session to complete.
      */
      if (tm_1x_up_session_info.mgp_info.prm_on == TRUE)
      {
        aiding_data_ptr = (tm_post_data_payload_type *)pdsm_getbuf(sizeof(tm_post_data_payload_type));

        if(aiding_data_ptr != NULL)
        {          
          aiding_data_ptr->sa_data.u_NumSvs = 0;
          MSG_HIGH("Give SA data to TM-Core",0,0,0);

          tm_1x_up_is801_drv_update_session_state(TM_1X_UP_SESSION_STATE_WAIT_PR);

          if(tm_post_data(TM_PRTL_TYPE_1X_UP,
                    tm_1x_up_session_info.sess_handle,
                    TM_POST_DATA_TYPE_SA,
                    aiding_data_ptr) == FALSE)
          {
            MSG_ERROR("Post data for SA failed",0,0,0);
            (void)pdsm_freebuf((char *)aiding_data_ptr);
            return;
          }
          (void)pdsm_freebuf((char *)aiding_data_ptr);
        }
        else
        {
          MSG_ERROR("Fail to get buffer", 0, 0, 0);
          tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
        }
      }
      else
      {
        MSG_ERROR("PRM not on.",0,0,0);
      }
    } /* end wait SA_REQ */
    else
    {
      MSG_ERROR("SA Reject not expected.",0,0,0);
    }
    break;

  case TM_1X_UP_RLINK_REQ_GPS_ALM:
  case TM_1X_UP_RLINK_REQ_GLO_EPH:
  case TM_1X_UP_RLINK_REQ_GLO_ALM:
  case TM_1X_UP_RLINK_REQ_ADV_SVHI:
  case TM_1X_UP_RLINK_REQ_DGNSS_ASST:
  case TM_1X_UP_RLINK_REQ_GGTO:
    MSG_ERROR("Reject for Asst data 0x%x", rej_req_type, 0, 0);
    if(tm_1x_up_session_info.session_state == TM_1X_UP_SESSION_STATE_WAIT_DLOAD_RESP)
    {
      tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_DLOAD;
      tm_1x_up_is801_drv_handle_assistance_data( rej_req_type,
                                                 sess_end, session_tag,
                                                 PDSM_PD_END_PDE_REJECT);
    }
    else
    {
      MSG_ERROR("ALM reject not expected",0,0,0);
    }
    break;

  case TM_1X_UP_RLINK_REQ_GPS_EPH:
    MSG_ERROR("Reject for EPH data",0,0,0);
    tm_1x_up_session_info.is801_eph_reject = TRUE;

    if(tm_1x_up_session_info.session_state == TM_1X_UP_SESSION_STATE_WAIT_DLOAD_RESP)
    {
      tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_DLOAD;
      tm_1x_up_is801_drv_handle_assistance_data(TM_1X_UP_RLINK_REQ_GPS_EPH,
                                                sess_end, session_tag,
                                                PDSM_PD_END_PDE_REJECT);
    }
    else
    {
      MSG_ERROR("Eph reject not expected",0,0,0);
    }
    break;

  default:
    MSG_ERROR ("Bad Rej req_type %d", rej_req_type, 0, 0);
    break;
  } /* End switch for reject reason */
} /* End tm_1x_up_is801_drv_handle_rej_resp() */

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_lr_resp

DESCRIPTION
  This function is used to handle PL response from PDE.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
static void tm_1x_up_is801_drv_handle_lr_resp
(
   is801_fwd_link_pddm_info_s_type *resp_ptr, 
   uint8                            index,
   is801b_flink_ie_adv_lr          *adv_lr
)
{
  boolean send_sess_end = FALSE;
  MSG_HIGH("Rxed LR data from PDE",0,0,0);

  if(resp_ptr == NULL && adv_lr == NULL)
  {
    MSG_ERROR("Null resp_ptr",0,0,0);
    return;
  }

  if(tm_1x_up_is801_drv_session_active() == TRUE)
  {
    if(tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_MSA)
    {
      tm_1x_up_is801_drv_handle_lr_resp_msa(resp_ptr, index, adv_lr, send_sess_end);
    }
    else if(tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_DLOAD)
    {
      tm_1x_up_is801_drv_handle_lr_resp_dload(resp_ptr, index, adv_lr);
    }
  }
  else
  {
    MSG_ERROR("LR in wrong state=%d",tm_1x_up_session_info.session_state,0,0);
  }
} /* End tm_1x_up_is801_drv_handle_lr_resp() */


/*===========================================================================

FUNCTION tm_1x_up_is801_1_drv_handle_msinfo_req

DESCRIPTION
  This function is used to handle MSINFO req from PDE.

RETURN VALUE
  None

DEPENDENCIES
===========================================================================*/
void tm_1x_up_is801_1_drv_handle_msinfo_req(void)
{
  static const uint8 tx_mask  = TM_1X_UP_IS801_1_PDDM;

  is801_operation    msi_resp_ops[]  =  { RL_RESP_MSI, TX_PDDM, OPS_END };
  const void*        msi_resp_par[]  =  { NULL,        &tx_mask };

  is801_operation  *ops = msi_resp_ops;
  void             *par = msi_resp_par;

  if( tm_1x_up_execute_fns(ops, par) == FALSE)
  {
    tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
    return;
  }
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_rej_req

DESCRIPTION
  This function is used to Reject req from PDE.

RETURN VALUE
  None

DEPENDENCIES
TBD handle IS801B
===========================================================================*/
void tm_1x_up_is801_drv_rej_pde_req(uint32 req_type )
{
    is801_rev_link_resp_s_type *resp_ptr = NULL;

    resp_ptr = (is801_rev_link_resp_s_type *)pdsm_getbuf( sizeof(is801_rev_link_resp_s_type));
    if(resp_ptr != NULL)
    {
        resp_ptr->response_type = IS801_REV_LINK_RESP_MASK_REJ;
        resp_ptr->unsolicited = 1;

        resp_ptr->resp_msg_payload.rej_resp.rej_reason   = 1;
        resp_ptr->resp_msg_payload.rej_resp.rej_req_type = req_type;

        if ( is801_new_rev_link_resp( TM_PRTL_TYPE_1X_UP,resp_ptr))
        {
          MSG_MED("Created Reject resp for type %d",req_type,0,0);
        }
        else
        {
          MSG_ERROR("Can't create Rej resp",0,0,0);
        }
        (void)tm_1x_up_is801_1_drv_send_rlink_pddm(0,0,0);
        (void)pdsm_freebuf((char *)resp_ptr);
    }
    else
    {
      MSG_ERROR("No buffer for REJ",0,0,0);
    }
}


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_sa_req

DESCRIPTION
  This function is used handle request by searcher for SA data. The SA data should
  be available in Database.

RETURN VALUE
  None

DEPENDENCIES
<TBD: Parametrize?
===========================================================================*/
void tm_1x_up_is801_drv_handle_sa_req(void)
{
  boolean is_801_1 = TRUE;
  MSG_HIGH("SA request from MGP",0,0,0);

#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#else
  is_801_1   = TRUE;
#endif
  /* Request PDE for SA data.
  */
  if (tm_1x_up_is801_drv_session_active())
  {
    if(tm_1x_up_is801_drv_create_rlink_req( TM_1X_UP_RLINK_REQ_GPS_SA, 
                                            is_801_1) == FALSE)
    {
      MSG_ERROR("Can not create SA req",0,0,0);
      tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
      return;
    }
    tm_1x_up_is801_drv_update_session_state(TM_1X_UP_SESSION_STATE_WAIT_SA);
  }
  else
  {
    MSG_HIGH("wait for SA data...",0,0,0);
    return;
  }

  /* TBD: check if the function call below results in the right thing happening
   * which is, a tx of the Is801B or IS801-1 PDDM depending on the state of the
   * session */
  if(tm_1x_up_is801_1_drv_send_rlink_pddm(0, 0,0))
  {
    if(!os_TimerStart(tm_1x_up_session_info.tm_1x_up_is801_timer,TM_IS801_TIMEOUT,NULL))
    {
      MSG_ERROR("Failed starting IS801 timer",0,0,0);
    }
  }
  else
  {
    tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
    return;
  }
} /* End tm_1x_up_is801_drv_handle_sa_req() */

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_mgp_timeout

DESCRIPTION

RETURN VALUE

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_handle_mgp_timeout(void)
{
  MSG_HIGH("Handle MGP timeout",0,0,0);
  if (tm_1x_up_is801_drv_session_active())
  {
    if(tm_1x_up_session_info.mgp_info.ppm_on)
    {
      MSG_ERROR("Get PPM timed out",0,0,0);

      if(tm_1x_up_session_info.mode_info.mode != TM_1X_UP_MODE_MSB)
      {
        if (tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_MSA)
        {
          tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_MSA;
        }
        else if (tm_1x_up_session_info.mode_info.last_error == TM_1X_UP_MODE_DLOAD)
        {
          tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_DLOAD;
        }
        tm_1x_up_is801_drv_opn_completed( PDSM_PD_END_TIMEOUT );
      }
    }
    else if(tm_1x_up_session_info.session_state == TM_1X_UP_SESSION_STATE_WAIT_REF_LOC)
    {
      MSG_ERROR("Timed out waiting for Ref Loc",0,0,0);
      tm_1x_up_is801_drv_update_session_state(TM_1X_UP_SESSION_STATE_WAIT_DLOAD_RESP);

      /* TBD also send 801-B pddm */
      if(tm_1x_up_is801_1_drv_send_rlink_pddm(1,0,0))
      {
        /* Now start a response IS801 timer
        */
        if(!os_TimerStart(tm_1x_up_session_info.tm_1x_up_is801_timer,TM_IS801_TIMEOUT,NULL))
        {
          MSG_ERROR("Failed starting IS801 timer",0,0,0);
        }
      }
    }
  }
} /* End tm_1x_up_is801_drv_handle_mgp_timeout() */

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_is801_timeout

DESCRIPTION
  This function is used to handle IS801 timeout.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_handle_is801_timeout( void )
{
  MSG_HIGH("Handle IS801 timeout",0,0,0);
  if ( tm_1x_up_is801_drv_session_active() && (tm_1x_up_session_info.mode_info.mode != TM_1X_UP_MODE_MSB) )
  {
    event_report(EVENT_GPS_IS801_TIMEOUT);

    if (tm_1x_up_session_info.is801_req_wait_bmap)
    {
      /* Clear the request bit map, since we have only one timer for all responses.
      */
      tm_1x_up_session_info.is801_req_wait_bmap = 0;
      pdcomm_info.wait_for_sess_end_ack = FALSE;
      /* It could be eithe Dload operation or MS-Assisted session.
      */
      if (tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_MSA)
      {
        tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_MSA;
      }
      else if (tm_1x_up_session_info.mode_info.last_error == TM_1X_UP_MODE_DLOAD)
      {
        tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_DLOAD;
      }

#ifdef FEATURE_JCDMA_GPSONE
      tm_1x_up_is801_drv_opn_completed( PDSM_PD_END_JGPS_TIMEOUT_IS801 );
#else
      tm_1x_up_is801_drv_opn_completed( PDSM_PD_END_TIMEOUT );
#endif /* FEATURE_JCDMA_GPSONE */

    } /* end if wait_bmap */
  }
  else
  {
    MSG_ERROR("Timer event not expected",0,0,0);
  }
} /* End tm_1x_up_is801_drv_handle_is801_timeout() */


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_connect_timeout

DESCRIPTION
  This function is used to handle Connect timeout.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_handle_connect_timeout( void )
{
  MSG_HIGH("Handle Connect timeout",0,0,0);
  if ( tm_1x_up_is801_drv_session_active() && (tm_1x_up_session_info.mode_info.mode != TM_1X_UP_MODE_MSB) )
  {
    /* Fire COMM_FAILURE event */
    tm_1x_up_info_buffer.client_id = tm_1x_up_session_info.api_param.pd_api_info.client_id;
    tm_1x_up_info_buffer.pd_info.end_status = PDSM_PD_END_CON_FAIL;
    (void)tm_post_pd_event( TM_PRTL_TYPE_1X_UP,
                            tm_1x_up_session_info.sess_handle,
                            PDSM_PD_EVENT_COMM_FAILURE,
                            &tm_1x_up_info_buffer,
                            NULL);

    pdcomm_info.abort = TRUE;

    if (tm_1x_up_session_info.session_state == TM_1X_UP_SESSION_STATE_WAIT_FD)
    {
      tm_1x_up_is801_drv_handle_dormancy_failure(TM_1X_UP_DORMANCY_FAILURE_TYPE_OPN_COMPLETE);
    }  
    else
    {         
      event_report(EVENT_GPS_PD_CONNECTION_TIMEOUT);
      /* It could be eithe Dload operation or MS-Assisted session.
      */
      if (tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_MSA)
      {
        tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_MSA;
      }
      else if (tm_1x_up_session_info.mode_info.last_error == TM_1X_UP_MODE_DLOAD)
      {
        tm_1x_up_session_info.mode_info.last_error = TM_1X_UP_MODE_DLOAD;
      }
      tm_1x_up_is801_drv_opn_completed( PDSM_PD_END_CON_FAIL );
    } /* end if connect_timer_active */
  }
  else
  {
    MSG_ERROR("Timer event not expected",0,0,0);
  }
} /* End tm_1x_up_is801_drv_handle_connect_timeout() */


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_disconnect_timeout

DESCRIPTION
  This function is used to handle Disconnect timeout.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_is801_drv_handle_disconnect_timeout( void )
{
  MSG_HIGH("Handle Disconnect timeout",0,0,0);
  //akench TODO tm_1x_up_comm_abort_connection();
}

#if defined(FEATURE_CALL_THROTTLE)
/*===========================================================================

FUNCTION tm_1x_up_set_throttling_backoff_time

DESCRIPTION
  This function is used to set pdsm throttling backoff time

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_set_throttling_backoff_time(void)
{
  qword                      current_sys_time, qwtime;

  if (((tm_1x_up_is801_drv_session_active()) && (tm_1x_up_session_info.mode_info.mode == TM_1X_UP_MODE_DLOAD)))
  {
    if(tm_1x_up_config.msb_throttling_info.safe_dload_req == FALSE)
    {
      MSG_HIGH("Dload request allowed before safe dload duration, increase back_off_time",0,0,0);
            
      (void) time_get(current_sys_time);

      /* Current sys time is more than throttling time. So, increment
      ** the back off time and proceed with the request.
      */
      qw_mul(tm_1x_up_config.msb_throttling_info.back_off_time,tm_1x_up_config.msb_throttling_info.back_off_time,tm_1x_up_config.msb_throttling_info.back_off_factor);
      qw_equ(qwtime,tm_1x_up_config.msb_throttling_info.back_off_max_duration);

      if ( qw_cmp(tm_1x_up_config.msb_throttling_info.back_off_time,qwtime) > 0 )
      {
        /* We are hitting max back-off time.
        */
        MSG_HIGH("Back off time set to maximum",0,0,0);
        qw_equ(tm_1x_up_config.msb_throttling_info.back_off_time,qwtime);
      }
      qw_add(tm_1x_up_config.msb_throttling_info.throttle_end_time,current_sys_time,tm_1x_up_config.msb_throttling_info.back_off_time);   
      
      /* Just add debug to show how much is the back off time.
      */
      qw_mul(qwtime,tm_1x_up_config.msb_throttling_info.back_off_time,5);
      (void)qw_div(qwtime,qwtime,4);
      qw_shift(qwtime,-16);
      (void)qw_div(qwtime,qwtime,1000);
      MSG_HIGH("setting back off time to = %d seconds",qwtime[0],0,0);       
    }
    else
    {
      MSG_HIGH("Dload request allowed after safe dload duration, do not increase back_off_time",0,0,0);
    }
  }
}



/*===========================================================================

FUNCTION tm_1x_up_mpc_set_throttling_backoff_time

DESCRIPTION
  This function is used to set pdsm throttling backoff time

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_1x_up_mpc_set_throttling_backoff_time(void)
{
  qword                      current_sys_time, qwtime;
    
  MSG_HIGH("Set MPC Back off throttling (factor=%d)",tm_1x_up_config.msb_throttling_info.back_off_factor,0,0);
            
  (void) time_get(current_sys_time);

  /* Current sys time is more than throttling time. So, increment
  ** the back off time and proceed with the request.
  */
  qw_mul(tm_1x_up_config.msb_throttling_info.back_off_time,tm_1x_up_config.msb_throttling_info.back_off_time,
         tm_1x_up_config.msb_throttling_info.back_off_factor);
  qw_equ(qwtime,tm_1x_up_config.msb_throttling_info.back_off_max_duration);

  if ( qw_cmp(tm_1x_up_config.msb_throttling_info.back_off_time,qwtime) > 0 )
  {
    /* We are hitting max back-off time.
    */
    MSG_HIGH("MPC Back off time set to maximum",0,0,0);
    qw_equ(tm_1x_up_config.msb_throttling_info.back_off_time,qwtime);
  }
  qw_add(tm_1x_up_config.msb_throttling_info.throttle_end_time,current_sys_time,tm_1x_up_config.msb_throttling_info.back_off_time);   
  
  /* Just add debug to show how much is the back off time.
  */
  qw_mul(qwtime,tm_1x_up_config.msb_throttling_info.back_off_time,5);
  (void)qw_div(qwtime,qwtime,4);
  qw_shift(qwtime,-16);
  (void)qw_div(qwtime,qwtime,1000);
  MSG_HIGH("Setting MPC back off time to = %d seconds",qwtime[0],0,0);       
}
#endif /* FEATURE_CALL_THROTTLE */

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_wish_list_update

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_1x_up_is801_drv_handle_wish_list_update(const tm_aiding_data_wishlist_s_type* aiding_data_status_ptr)
{
  const sm_InternalAssistDataStatusStructType     *gps_status_ptr = NULL;
  const sm_GnssAssistDataStruct           *glo_status_ptr = NULL;
  tm_1x_up_aiding_data_status_s_type      *status_out_ptr = NULL;
  boolean  ret_val = TRUE;
  uint32   q_GPSWishListMask = 0x0;
  uint32   q_GLOWishListMask = 0x0;

  if(aiding_data_status_ptr == NULL)
  {
    MSG_ERROR("Null aiding_data_status_ptr",0,0,0); 
    return FALSE;
  }

  gps_status_ptr     = &(aiding_data_status_ptr->z_assist_data_status); 
  glo_status_ptr     = &(aiding_data_status_ptr->z_assist_data_status.z_GloAssistData); 
  q_GPSWishListMask  = gps_status_ptr->q_WishListMask;

  /* TBD check if glonass wishlist is valid */
  q_GLOWishListMask  = glo_status_ptr->q_WishListMask;

  status_out_ptr = &(tm_1x_up_session_info.aiding_data_status);

  /* GPS Ephemeris */
  status_out_ptr->gps_eph_validity = (q_GPSWishListMask & C_GPS_ASSIS_NAVMODL) ? 0 : 1;
  if (!status_out_ptr->gps_eph_validity)
  {
    /* save the need list as what is needed from the visible list only if no Visible SVs */
    if(aiding_data_status_ptr->z_assist_data_status.q_VisibleSVList != 0)
    {
      tm_1x_up_session_info.aiding_data_status.gps_eph_requested_sv_list = aiding_data_status_ptr->z_assist_data_status.q_EphNeedList & aiding_data_status_ptr->z_assist_data_status.q_VisibleSVList;
      MSG_MED("Eph requested. List 0x%1x",tm_1x_up_session_info.aiding_data_status.gps_eph_requested_sv_list,0,0);
    }
  }
  MSG_MED("Eph needed. %d (0=Y/1=N) NeedList 0x%x HaveList %x",
             status_out_ptr->gps_eph_validity,
             gps_status_ptr->q_EphNeedList, 
             gps_status_ptr->q_EphHaveList);

  /* GPS Almanac */
  status_out_ptr->gps_alm_validity = (q_GPSWishListMask & C_GPS_ASSIS_ALM) ? 0 : 1;
  MSG_MED("Alm needed. %d (0=Y/1=N) NeedList 0x%x HaveList %x",
             status_out_ptr->gps_alm_validity,
             gps_status_ptr->q_AlmNeedList, 
             gps_status_ptr->q_AlmHaveList);

  /* GLO Ephemeris */
  status_out_ptr->glo_eph_validity = (q_GLOWishListMask & C_GPS_ASSIS_NAVMODL) ? 0 : 1;
  MSG_MED("Eph needed. %d (0=Y/1=N) NeedList 0x%x HaveList %x",
             status_out_ptr->glo_eph_validity,
             glo_status_ptr->q_EphNeedList, 
             glo_status_ptr->q_EphHaveList);

  /* GLO Almanac */
  status_out_ptr->glo_alm_validity = (q_GLOWishListMask & C_GPS_ASSIS_ALM) ? 0 : 1;
  MSG_MED("Alm needed. %d (0=Y/1=N) NeedList 0x%x HaveList %x",
             status_out_ptr->glo_alm_validity,
             glo_status_ptr->q_AlmNeedList, 
             glo_status_ptr->q_AlmHaveList);

  /* Ref Location */
  if(q_GPSWishListMask & C_GPS_ASSIS_REFLOC)
  {
    MSG_MED("Ref Loc needed",0,0,0);
    status_out_ptr->pos_unc = TM_LR_THRESHOLD_NO_POS;
  }
  else
  {
    /* Position Uncertainty */
    if(gps_status_ptr->z_PosUnc.u_Valid)
    {
      status_out_ptr->pos_unc = gps_status_ptr->z_PosUnc.f_PosUnc;
    }
    else
    {
      status_out_ptr->pos_unc = TM_LR_THRESHOLD_NO_POS;
    }
  }

  MSG_MED("Pos Uncertainty %dm",status_out_ptr->pos_unc,0,0);

  /* Ref Time */
  if(q_GPSWishListMask & C_GPS_ASSIS_REFTIME)
  {
    MSG_MED("Ref Time needed",0,0,0);
    status_out_ptr->time_unc = TM_TIME_THRESHOLD_NO_TIME; 
  }
  else
  {
    /* Time Uncertainty */
    if(gps_status_ptr->z_TimeUnc.u_Valid)
    {
      status_out_ptr->time_unc = 
                  (FLT) (LIGHT_MSEC * gps_status_ptr->z_TimeUnc.f_TimeUnc); 
    }
    else
    {
      status_out_ptr->time_unc = TM_TIME_THRESHOLD_NO_TIME; 
    }
  }
  MSG_MED("Time Uncertainty %f",status_out_ptr->time_unc,0,0);

  ret_val =  tm_1x_up_is801_drv_start_session_wishlist_update();

  if (ret_val == FALSE)
  {
     MSG_ERROR("Failed to Initiate",0,0,0);
  }
  return ret_val;
}

/*===========================================================================

FUNCTION tm_1x_up_init_session_info

DESCRIPTION
  This function is used set initialize the requested session inforamtion. The
  session request is either Dload request or a Fix request.

RETURN VALUE
  None

DEPENDENCIES
==============================================================================*/

void tm_1x_up_is801_drv_init_session_info
(
  tm_sess_handle_type                        sess_handle,
  const prtl_start_sess_req_param_s_type    *sess_req_param_ptr
)
{
  pdsm_server_address_s_type *server_adrs = 
     &tm_1x_up_session_info.api_param.pd_api_info.pd_option.lsinfo.server_adrs;

  pdsm_pd_fix_rate_s_type    *fix_rate    =
     &tm_1x_up_session_info.api_param.pd_api_info.pd_option.fix_rate;

  (void) server_adrs;

  tm_1x_up_session_info.sess_handle = sess_handle; 

  if(sess_req_param_ptr == NULL)
  {
    MSG_ERROR("Null sess_req_param_ptr",0,0,0);
    return;
  }

  #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#endif

  memscpy( (void *) &(tm_1x_up_session_info.api_param.pd_api_info), 
          sizeof(tm_1x_up_session_info.api_param.pd_api_info),
          (void *) sess_req_param_ptr, sizeof(*sess_req_param_ptr));

  MSG_HIGH("Num_fixes= %u, tbf= %u", fix_rate->num_fixes,
                                     fix_rate->time_between_fixes_ms,0);

  if( tm_1x_up_session_info.api_param.pd_api_info.pd_option.lsinfo.server_option == 
      PDSM_SERVER_OPTION_USE_DEFAULT)
  {
    MSG_MED("Server Addr=0x%x Port=%d", 
               tm_1x_up_config.pde_adrs, tm_1x_up_config.port_id, 0);
  }
  else
  {
    MSG_MED("Server Addr=0x%x Port=%d",
               server_adrs->server_adrs.server_adrs_v4.server_adrs,
               server_adrs->server_adrs.server_adrs_v4.port_id,0);
  }
  if ( sess_req_param_ptr->pd_qos.gps_session_timeout == 0 )
  {
    tm_1x_up_session_info.api_param.session_type = TM_1X_UP_SESSION_TYPE_AFLT;
  }
  else
  {
    tm_1x_up_session_info.api_param.session_type = TM_1X_UP_SESSION_TYPE_HYBRID;
  }
} /* end tm_1x_up_init_session_info() */

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_start_sess_req

DESCRIPTION
  This function is used to indicate if session is active or not.

RETURN VALUE
  TRUE if Active, else FALSE.

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_is801_drv_handle_start_sess_req
(
  tm_sess_req_start_param_s_type    *actions,
  boolean                           wake_tm_core
)
{
    boolean  ret_val = TRUE;

    /* For Call flow 3, 7:  Ask TM_Core for AIDING_DATA */
    if( (tm_1x_up_session_info.api_param.pd_api_info.pd_option.session == PDSM_PD_SESS_TYPE_DATA_DLOAD) ||
        (tm_1x_up_session_info.api_param.pd_api_info.pd_option.operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_SPEED) ||
        (tm_1x_up_session_info.api_param.pd_api_info.pd_option.operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_DATA) ||
        (tm_1x_up_session_info.api_param.pd_api_info.pd_option.operation_mode == PDSM_SESSION_OPERATION_MSBASED) ||
        (tm_1x_up_session_info.api_param.pd_api_info.pd_option.operation_mode == PDSM_SESSION_OPERATION_REF_POSITION)
      )
    {
      if(wake_tm_core == FALSE)
      {
        if(actions == NULL)
        {
          MSG_ERROR("Null actions pointer",0,0,0);
          return FALSE;
        }
        actions->op_req = TM_OP_REQ_AIDING_DATA_STATUS;
        /* for optimal mode, add a new mode as MSB_OPT, this way when TM
           requests PE for wish list it be a query to PE so PE does not 
           mark any eph as waiting from the server. when the wish list response
           is received, then we will send another dummy wishlist request if we 
           decide to do a MSB session
        */
        if((tm_1x_up_session_info.api_param.pd_api_info.pd_option.operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_SPEED) ||
           (tm_1x_up_session_info.api_param.pd_api_info.pd_option.operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_DATA))
        {
          actions->op_mode = TM_SESS_OPERATION_MODE_MSB_OPT;
        }
        else
        {
          actions->op_mode = TM_SESS_OPERATION_MODE_MSB;
        }
        actions->num_fixes = tm_1x_up_session_info.api_param.pd_api_info.pd_option.fix_rate.num_fixes;
        actions->tbf_ms = tm_1x_up_session_info.api_param.pd_api_info.pd_option.fix_rate.time_between_fixes_ms;
        MSG_HIGH("Returning actions op_req 0x%x",actions->op_req,0,0);
        return TRUE;
      }
      else /* VX module already replied to TM-Core for the request, so now ask TM-Core to continue */
      {
        tm_1x_up_sess_req_param.continue_param.op_req  = TM_OP_REQ_AIDING_DATA_STATUS | TM_OP_REQ_LR;
        tm_1x_up_sess_req_param.continue_param.op_mode  = TM_SESS_OPERATION_MODE_MSB;
        tm_1x_up_sess_req_param.continue_param.dynamic_qos_enabled = TRUE;
        tm_1x_up_sess_req_param.continue_param.num_fixes = tm_1x_up_session_info.api_param.pd_api_info.pd_option.fix_rate.num_fixes;
        tm_1x_up_sess_req_param.continue_param.tbf_ms = tm_1x_up_session_info.api_param.pd_api_info.pd_option.fix_rate.time_between_fixes_ms;

        if(tm_sess_req(TM_PRTL_TYPE_1X_UP,
                     tm_1x_up_session_info.sess_handle,
                     TM_SESS_REQ_CONTINUE,
                     &tm_1x_up_sess_req_param))
        {
          MSG_MED("continue_msb_fix Succeeded", 0, 0, 0);  
          return TRUE;
        }
        else
        {
          MSG_ERROR("continue_msb_fix Failed", 0, 0, 0);
          return FALSE;
        }                  
      }
    }
    /* For Call flow 1, 2:  Ask TM_Core to START_PPM */
    else if((tm_1x_up_session_info.api_param.pd_api_info.pd_option.operation_mode == PDSM_SESSION_OPERATION_MSASSISTED) ||
            (tm_1x_up_session_info.api_param.pd_api_info.pd_option.operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_ACCURACY))
    {
      ret_val = tm_1x_up_is801_drv_set_mode();

      if ( ret_val == TRUE)
      {
        /* Check if force dormancy is required or not.
        */
#ifdef FEATURE_GPSONE_UPDATE_BSID
        tm_1x_up_is801_drv_init_bs_info();
#endif
        tm_1x_up_session_info.tm_core_initiate_session = TRUE;
        /* Check if Core state m/c can proceed with the session or return error to TM-Core.
        */
        ret_val = tm_1x_up_is801_drv_initiate_session();

        if (ret_val == FALSE)
        {
          MSG_ERROR("Failed to Initiate",0,0,0);
          return FALSE;
        }

        if(wake_tm_core == FALSE)
        {
          if(actions == NULL)
          {
            MSG_ERROR("Null actions pointer",0,0,0);
            return FALSE;
          }
          actions->op_req = TM_OP_REQ_PPM;
          actions->ppm_qos = TM_MGP_PPM_TIMEOUT_MAX/1000;
          actions->op_mode = TM_SESS_OPERATION_MODE_MSA;
          actions->num_fixes = tm_1x_up_session_info.api_param.pd_api_info.pd_option.fix_rate.num_fixes;
          actions->tbf_ms = tm_1x_up_session_info.api_param.pd_api_info.pd_option.fix_rate.time_between_fixes_ms;

          tm_1x_up_session_info.mgp_info.ppm_on = TRUE;
          MSG_HIGH("Returning actions op_req 0x%x",actions->op_req,0,0);
          return TRUE;
        }
        else /* VX module already replied to TM-Core for the request, so now ask TM-Core to continue */
        {
          /*Start PPM if PPM is not going on already*/
          if(tm_1x_up_session_info.mgp_info.ppm_on == FALSE)
          {
            if(!tm_1x_up_is801_drv_start_ppm())
            {
              MSG_ERROR("Unable to request TM-Core to start PPM",0,0,0);
              tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
              return FALSE;
            }
          } /*PPM is not going on*/
          else /*PPM is going on, so get PPM results*/
          {
            if(!tm_1x_up_is801_drv_get_ppm())
            {
              MSG_ERROR("Unable to get PPM results",0,0,0);
              tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
            }
          }
          return TRUE;
        }
      }
      else
      {
        return FALSE;
      }
    }
    else
    {
      MSG_ERROR("Incorrect operation mode %d",tm_1x_up_session_info.api_param.pd_api_info.pd_option.operation_mode,0,0);
      return FALSE;
    }
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_handle_start_sess_nodwld_req

DESCRIPTION
  This function is a wrapper around tm_1x_up_is801_drv_handle_start_sess_req,
  used in the case where we want to start a session which should not trigger AD
  download.

RETURN VALUE
  TRUE if session could be started, else FALSE.

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_is801_drv_handle_start_sess_nodwld_req
(
  tm_sess_req_start_param_s_type    *actions,
  boolean                           wake_tm_core
)
{
  boolean ret_val;

  tm_1x_up_session_info.mode_info.req_to_send   = 0x0;
  tm_1x_up_session_info.mode_info.resp_to_send  = 0x0;
  tm_1x_up_session_info.mode_info.mode          = TM_1X_UP_MODE_MSB;
                    
  ret_val = tm_1x_up_is801_drv_initiate_session();
  return ret_val;
}


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_req

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_1x_up_is801_drv_req
(
  tm_sess_handle_type                  sess_handle,
  tm_1x_up_is801_drv_req_e_type        req_type,
  tm_1x_up_is801_drv_req_param_u_type  req_param
)
{
  boolean ret_val = TRUE;

  if(req_type == TM_IS801_DRV_REQ_START_NODWLD)
  {
    /* Session is started right now and we have all the required info
    */
    tm_1x_up_is801_drv_init_session_info(sess_handle,req_param.start_param.start_sess_req_param);

    ret_val = tm_1x_up_is801_drv_handle_start_sess_nodwld_req(req_param.start_param.actions, req_param.start_param.wake_tm_core);
    MSG_HIGH("Returning %d",ret_val,0,0);
  }
  else if(req_type == TM_IS801_DRV_REQ_START)
  {
    /* Session is started right now and we have all the required info
    */
    tm_1x_up_is801_drv_init_session_info(sess_handle,req_param.start_param.start_sess_req_param);

    ret_val = tm_1x_up_is801_drv_handle_start_sess_req(req_param.start_param.actions, req_param.start_param.wake_tm_core);
    MSG_HIGH("Returning %d",ret_val,0,0);
  }
  else if(req_type == TM_IS801_DRV_REQ_CONT)
  {
    tm_1x_up_is801_drv_handle_grant_session(FALSE);
  }
  else if(req_type == TM_IS801_DRV_REQ_STOP)
  {    
    tm_1x_up_session_info.sess_handle = sess_handle; 
    ret_val = tm_1x_up_is801_drv_handle_stop_sess_req(req_param.stop_param.stop_reason);
  }

  return ret_val;
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_req_aiding_data

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_1x_up_is801_drv_req_aiding_data
(
  tm_sess_handle_type  session_handle,
  prtl_data_req_type   req_data
)
{
  if(session_handle == tm_1x_up_session_info.sess_handle)
  {
    if(req_data & PRTL_AIDING_DATA_REQ_SA)
    {
      tm_1x_up_is801_drv_handle_sa_req();
    }
    return TRUE;
  }
  else
  {
    MSG_ERROR("Incorrect sess_handle", 0, 0, 0);   
    return FALSE;
  }
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_sess_info_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_1x_up_is801_drv_sess_info_handler
(
  tm_sess_handle_type          session_handle,
  prtl_sess_info_e_type        sess_info_type,
  prtl_sess_info_param_u_type  *sess_info_param
)
{
  boolean ret_val = TRUE;
  if(sess_info_param == NULL)
  {
    MSG_ERROR("Null sess_info_param pointer", 0, 0, 0);   
    return FALSE;
  }

  if(session_handle == tm_1x_up_session_info.sess_handle)
  {
   switch(sess_info_type)
   {
     case PRTL_SESS_INFO_STATUS:
       (void)tm_1x_up_is801_drv_handle_wish_list_update(&sess_info_param->aiding_data_status);
       break;

     case PRTL_SESS_INFO_PPM:
       (void)tm_1x_up_is801_drv_handle_ppm(&sess_info_param->ppm_data);
       break;

     case PRTL_SESS_INFO_PRM:
       if ( sess_info_param->prm_data.e_MeasRptReason == GPS_MEAS_RPT_MEAS_DONE )
       {
         (void)tm_1x_up_is801_drv_handle_prm(&sess_info_param->prm_data);
       }
       break;

     case PRTL_SESS_INFO_LR:
       (void)tm_1x_up_is801_drv_handle_lr_resp_msbased(&sess_info_param->lr_data);
       break;

     case PRTL_SESS_INFO_REF_LR:
       tm_1x_up_is801_drv_handle_ref_loc(&sess_info_param->ref_lr_data);
       break;

     default:
       break;
   }
   return ret_val;
  }
  else
  {
    MSG_ERROR("Incorrect sess_handle", 0, 0, 0);   
    return FALSE;
  }
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_data_pipe_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_1x_up_is801_drv_data_pipe_handler
(
  pd_comms_client_ipc_message_type  msg_type,
  void                              *msg_data_ptr
)
{
  MSG_HIGH("PDCOMM message type %d", msg_type, 0, 0);  
  //process it for Trusted module.
  if(!tm_1x_up_handle_comm_msg(msg_type, msg_data_ptr))
  {
     MSG_HIGH("Handle error here",0,0,0); 
     tm_1x_up_is801_drv_handle_comm_error();
  }
  return TRUE;
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_timer_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_1x_up_is801_drv_timer_handler
(
  void *timerData
)
{
    os_TimerExpiryType *timer_param = (os_TimerExpiryType *)timerData;

    if(timer_param != NULL)
    {
        MSG_HIGH("timerId 0x%x expired", timer_param->q_TimerId, 0, 0);                    
        if((timer_param->q_TimerId & TM_1X_UP_TRUSTED_TIMER_DISPATCH_MASK))
        {
          switch(timer_param->q_TimerId)
          {
          case TM_1X_UP_SESS_TIMER_ID:
              tm_1x_up_is801_drv_handle_fix_timeout();
              break;

          case TM_1X_UP_IS801_TIMER_ID:
              tm_1x_up_is801_drv_handle_is801_timeout();
              break;

          case TM_1X_UP_CONNECT_TIMER_ID:
              tm_1x_up_is801_drv_handle_connect_timeout();
              break;

          case TM_1X_UP_DISCONNECT_TIMER_ID:
              tm_1x_up_is801_drv_handle_disconnect_timeout();
              break;

          case TM_1X_UP_MGP_TIMER_ID:
              tm_1x_up_is801_drv_handle_mgp_timeout();
              break;

          default:
              MSG_ERROR("Unknown timer Id",0,0,0);
              break;
          }
        }
    }
    else
    {
      MSG_ERROR("Can not dispatch null timer msg", 0, 0, 0);  
    }  
    return;
}

/*===========================================================================

FUNCTION tm_1x_up_is801_drv_event_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_1x_up_is801_drv_event_handler
(
  prtl_event_e_type  event_type,
  const prtl_event_u_type  *event_payload
)
{ 
  MSG_MED("Handle event %d",event_type,0,0);
  if(event_payload == NULL)
  {
    MSG_ERROR("Null event_payload",0,0,0);
    return;
  }
  switch (event_type) 
  {
    case PRTL_EVENT_TYPE_PHONE:
      if(event_payload->phone_event_payload.event == TM_PHONE_EVENT_TC_UP)
      {
        tm_1x_up_is801_drv_handle_tc_up();
      }
      else if(event_payload->phone_event_payload.event == TM_PHONE_EVENT_TC_EXIT)
      {
        tm_1x_up_is801_drv_handle_tc_exit();
      }
      else if(event_payload->phone_event_payload.event == TM_PHONE_EVENT_IN_1X_COVERAGE)
      {
        tm_1x_up_read_msid_fields();
      }
      else if(event_payload->phone_event_payload.event == TM_PHONE_EVENT_IN_UMTS_COVERAGE)
      {
        /*Close the session if SUPL is not User Plane protocol for 1x*/
        if(!TM_CORE_CONFIG_DD(gnss_1x_up_supl_enable))
        {
          MSG_MED("System changed to UMTS, aborting ongoing session", 0, 0, 0);
          tm_1x_up_is801_drv_handle_stop_session(TM_STOP_REASON_SERVING_SYSTEM_CHANGE);
        }
      }
      else if(event_payload->phone_event_payload.event == TM_PHONE_EVENT_OTASP_STATUS_COMMITTED)
      {
        tm_1x_up_read_msid_fields();  // in case OTASP has changed MSID, re-read NVs
      }
      else
      {
        MSG_MED("event %d not handled",event_payload->phone_event_payload.event,0,0);
      }
      break;
      
    case PRTL_EVENT_TYPE_PA:
      if(event_payload->pa_event_payload.pa_event == PDSM_PA_1X_PDE_NETWORK_ADRS)
      {
        if(event_payload->pa_event_payload.pa_info_ptr != NULL)
        {
          tm_1x_up_config.pde_adrs = event_payload->pa_event_payload.pa_info_ptr->server_address.server_adrs.server_adrs_v4.server_adrs;
          tm_1x_up_config.port_id  = event_payload->pa_event_payload.pa_info_ptr->server_address.server_adrs.server_adrs_v4.port_id;
          MSG_MED("Updated 1x PDE address %x and port %d", tm_1x_up_config.pde_adrs, tm_1x_up_config.port_id, 0);          
        }
      }      
      else if (event_payload->pa_event_payload.pa_event == PDSM_PA_DELETE_PARAMS_EX ||
               event_payload->pa_event_payload.pa_event == PDSM_PA_DELETE_PARAMS ||
               event_payload->pa_event_payload.pa_event == PDSM_PA_DELETE_PARAMS_EX1 )
      {
        MSG_MED("Clear throttling timers", 0, 0, 0);          
        qw_set(tm_1x_up_config.msb_throttling_info.throttle_end_time,0,0);
        qw_set(tm_1x_up_config.msb_throttling_info.msb_last_request_time,0,0);
        tm_1x_up_config.msb_throttling_info.mpc_throttle_check = FALSE;

#ifdef FEATURE_CALL_THROTTLE                
        tm_1x_up_config.msb_throttling_info.safe_dload_req = FALSE;
#endif /* FEATURE_CALL_THROTTLE */    
      }
      break;

  case PRTL_EVENT_TYPE_RUIM_NV_READ:

     if (event_payload->ruim_read_nv_payload.ruim_item_group == PRTL_EVENT_RUIM_MSID_NV_READ)
     {
          (void)tm_1x_up_read_msid_fields_complete(event_payload);
     }
#ifdef FEATURE_UIM_SUPPORT_LBS
     else if (event_payload->ruim_read_nv_payload.ruim_item_group == PRTL_EVENT_RUIM_1X_NV_READ) 
     {
          (void)tm_1x_up_ruim_read_complete(event_payload);
     }
#endif /* FEATURE_UIM_SUPPORT_LBS*/
     else
     {
        MSG_MED("RUIM ASYNC READ NV item not handled here",0,0,0);
     }
     break;
      
    default:
      break; 
  }
  return;
}


/*===========================================================================

FUNCTION: tm_1x_up_is801_drv_send_data

DESCRIPTION:
  This function is called by Vx or KDDI to tell IS801 session module to send 
  the data on the current TCP/IP connection
  
PARAMETERS:

RETURN VALUE:
  TRUE - If packet is sent successfully to PD COMM
  FALSE - All other error cases.
    
===========================================================================*/
boolean tm_1x_up_is801_drv_send_data
(
  byte    *send_data_ptr,
  int16   send_data_len
)
{
  /* Call 1x UP comm function to send this data */
  return tm_1x_up_comm_write(send_data_ptr, (uint16)send_data_len);
} /* tm_1x_up_is801_drv_send_data */

/*===========================================================================

FUNCTION: tm_1x_up_is801_drv_read_data

DESCRIPTION:
  This function is called by Vx or KDDI to tell IS801 session module to read 
  the data on the current TCP/IP connection
  
PARAMETERS:

RETURN VALUE:
  TRUE - If packet is sent successfully to PD COMM
  FALSE - All other error cases.
    
===========================================================================*/
boolean tm_1x_up_is801_drv_read_data
(
  uint16    len_to_read,
  boolean   wait_all_data
)
{
  /* Call 1x UP comm function to read data */
  return tm_1x_up_comm_read(len_to_read, wait_all_data);

} /* tm_1x_up_is801_drv_read_data */

/*===========================================================================

FUNCTION: tm_1x_up_is801_drv_disconnect

DESCRIPTION:
  This function is called by Vx or KDDI to tell IS801 session module to 
  disconnect the any PDCOMM connections
  
PARAMETERS:

RETURN VALUE:
  TRUE - If packet is sent successfully to PD COMM
  FALSE - All other error cases.
    
===========================================================================*/
boolean tm_1x_up_is801_drv_disconnect
(
  boolean b_abort
)
{
  /* Call 1x UP comm function to read data */
  if(PDCOMM_RESULT_OK == tm_1x_up_comm_disconnect(b_abort)) {
    return TRUE;
  }
  return FALSE;
} /* tm_1x_up_is801_drv_disconnect */

/*===========================================================================

FUNCTION: tm_1x_up_is801_drv_app_ioctl

DESCRIPTION:
  This function is called by Vx or KDDI to send an IOCTL
  
PARAMETERS:

RETURN VALUE:
  TRUE - If packet is sent successfully to PD COMM
  FALSE - All other error cases.
    
===========================================================================*/
boolean tm_1x_up_is801_drv_app_ioctl
(
  pd_comms_ioctl_cmd_type cmd
)
{
  /* Call 1x UP comm function to send IOCTL */
  return tm_1x_up_comm_app_ioctl(cmd);
} /* tm_1x_up_is801_drv_app_ioctl */

#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
#error code not present
#endif

/*===========================================================================

FUNCTION: tm_1x_up_is801_msg_cb

DESCRIPTION:
  This function is called by IS801 module to inform
  PDDM data is available.
  
PARAMETERS:
  
===========================================================================*/
boolean tm_1x_up_is801_msg_cb
(
  is801_fwd_pddm_info_s_type     pddm_info
)
{
  uint8 i,j;
  tm_1x_up_rlink_req_type rej_req;
          
  MSG_HIGH("# of PDDM = %d ", pddm_info.num_pddm, 0, 0);

  tm_1x_up_session_info.bs_capabilities_valid = FALSE;
  /* Get Requests */
  for(i = 0; i < pddm_info.num_pddm; i++)
  {
    if(!is801_get_fwd_link_pddm(TM_PRTL_TYPE_1X_UP,  &tm_1x_up_session_info.fwd_link_pddm))
    {
      MSG_ERROR("IS801 failed to get Fwd Link PDDM",0,0,0);
      if( tm_1x_up_is801_drv_session_active() == TRUE )
      {
        tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
      }
      return FALSE;
    }

    MSG_HIGH("Got Fwd Link Req %d Req Mask", tm_1x_up_session_info.fwd_link_pddm.hdr.num_requests, tm_1x_up_session_info.fwd_link_pddm.req_mask, 0); 
    MSG_HIGH("Got Fwd Link Resp %d Resp Mask", tm_1x_up_session_info.fwd_link_pddm.hdr.num_responses, tm_1x_up_session_info.fwd_link_pddm.resp_mask, 0); 

    /* Report IS801 event for requests */
    for(j = 0; j < tm_1x_up_session_info.fwd_link_pddm.hdr.num_requests; j++)
    {
      is801_util_report_is801_event_rx(IS801_REQ, tm_1x_up_session_info.fwd_link_pddm.req_rd[j].request_type);
    }
    /* Report IS801 event for responses */
    for(j = 0; j < tm_1x_up_session_info.fwd_link_pddm.hdr.num_responses; j++)
    {
      is801_util_report_is801_event_rx(IS801_RESP, tm_1x_up_session_info.fwd_link_pddm.resp_rd[j].resp_type);
    }

    /* if sess_end is set to 1, terminate session */
    if (tm_1x_up_session_info.fwd_link_pddm.hdr.sess_end == 0x01 ) 
    {
      MSG_ERROR("PDE requesting for End session",0,0,0);
      if( tm_1x_up_is801_drv_session_active() == TRUE )
      {
        tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
      }
      return TRUE;
    }
    /* Requests */
    for(j = 0; j < tm_1x_up_session_info.fwd_link_pddm.hdr.num_requests; j++)
    {

      switch(tm_1x_up_session_info.fwd_link_pddm.req_rd[j].request_type)
      {
        case IS801_FWD_LINK_REQ_MASK_MSI:
            // TODO tm_1x_up_is801_1_drv_handle_msinfo_req(&tm_1x_up_session_info.fwd_link_pddm.req_rd[j]);
          break;

        case IS801_FWD_LINK_REQ_MASK_AMWF:
        case IS801_FWD_LINK_REQ_MASK_PRM:
        case IS801_FWD_LINK_REQ_MASK_PPM:
        case IS801_FWD_LINK_REQ_MASK_LR:
        case IS801_FWD_LINK_REQ_MASK_TOM:
          tm_1x_up_is801_drv_rej_pde_req(tm_1x_up_session_info.fwd_link_pddm.req_rd[j].request_type);
          break;
        case IS801_FWD_LINK_REQ_MASK_CAN:
          //akench TODO

        default:
          MSG_MED("Req_type not handled %d",tm_1x_up_session_info.fwd_link_pddm.req_rd[j].request_type,0,0);
          break;
      }
    }

    /* Multiple Responses possible in case of MS-Based. E.g. LR, Eph & Alm in one PDDM */
    for(j = 0; (j < tm_1x_up_session_info.fwd_link_pddm.hdr.num_responses) && (j < MAX_RESP_PER_PDDM); j++)
    {
      switch(tm_1x_up_session_info.fwd_link_pddm.resp_rd[j].resp_type)
      {
        case IS801_FWD_LINK_RESP_MASK_AA:
          tm_1x_up_is801_drv_handle_aa_resp(&tm_1x_up_session_info.fwd_link_pddm.resp_rd[j], NULL);
          break;

        case IS801_FWD_LINK_RESP_MASK_SA:
          tm_1x_up_is801_drv_handle_gps_sa_resp(&tm_1x_up_session_info.fwd_link_pddm.resp_rd[j], NULL);
          break;

        case IS801_FWD_LINK_RESP_MASK_EPH:
          tm_1x_up_is801_drv_handle_gps_eph_resp(&tm_1x_up_session_info.fwd_link_pddm, j, NULL);
          break;

        case IS801_FWD_LINK_RESP_MASK_ALM:
          tm_1x_up_is801_drv_handle_gps_alm_resp(&tm_1x_up_session_info.fwd_link_pddm, j, NULL);
          break;

        case IS801_FWD_LINK_RESP_MASK_LR:
          tm_1x_up_is801_drv_handle_lr_resp(&tm_1x_up_session_info.fwd_link_pddm, j, NULL);
          break;

        case IS801_FWD_LINK_RESP_MASK_REJ:
          rej_req = xlate_is801_1_rej_resp_to_1x_up_req(&tm_1x_up_session_info.fwd_link_pddm.resp_rd[j]); 
          tm_1x_up_is801_drv_handle_rej_resp(
                    rej_req,
                    tm_1x_up_session_info.fwd_link_pddm.hdr.sess_end,
                    tm_1x_up_session_info.fwd_link_pddm.hdr.sess_tag);
          break;

        default:        
          break;
      }
    }
  }
  return TRUE;
}


/*===========================================================================

FUNCTION tm_1x_up_is801_drv_init

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_1x_up_is801_drv_init(void)
{
  MSG_MED("tm_1x_up_is801_drv_init", 0, 0, 0);

  /* Register with CM for 1X CP  specific events */
  (void)tm_cm_iface_submodule_init(TM_PRTL_TYPE_1X_UP, NULL);

  (void)is801_prtl_reg(TM_PRTL_TYPE_1X_UP, tm_1x_up_is801_msg_cb);
#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#else
#endif

  tm_1x_up_session_info.tm_1x_up_sess_timer = os_TimerCreateInCgpsTimerGroup((uint32)TM_1X_UP_SESS_TIMER_ID,(uint32)THREAD_ID_SM_TM);
  tm_1x_up_session_info.tm_1x_up_mgp_timer = os_TimerCreateInCgpsTimerGroup((uint32)TM_1X_UP_MGP_TIMER_ID,(uint32)THREAD_ID_SM_TM);
  tm_1x_up_session_info.tm_1x_up_is801_timer = os_TimerCreateInCgpsTimerGroup((uint32)TM_1X_UP_IS801_TIMER_ID,(uint32)THREAD_ID_SM_TM);
  tm_1x_up_session_info.tm_1x_up_connect_timer = os_TimerCreateInCgpsTimerGroup((uint32)TM_1X_UP_CONNECT_TIMER_ID,(uint32)THREAD_ID_SM_TM);
  /* Disconnect timer can be deferred */

  tm_1x_up_session_info.tm_1x_up_disconnect_timer = os_TimerCreate((uint32)TM_1X_UP_DISCONNECT_TIMER_ID,(uint32)THREAD_ID_SM_TM);

  tm_1x_up_is801_drv_reset_session_info();

  tm_1x_up_config.support_gps     = TRUE;
  tm_1x_up_comm_init();
}

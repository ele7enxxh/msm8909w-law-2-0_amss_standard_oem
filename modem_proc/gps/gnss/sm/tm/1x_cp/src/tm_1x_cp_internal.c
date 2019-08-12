
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM 1X CP Internal

General Description
  This file contains implementations for utility functions that are needed
  for TM 1X CP 
  
  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/16/2013 mj     Handling Failure in converting PPM

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
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
#include "tm_is801b.h"
#include "tm_is801b_util.h"
#include "tm_1x_cp_internal.h"

#define   TM_POST_DATA_PRTL_CP(data_type)                                     \
do {                                                                          \
  if(tm_post_data( TM_PRTL_TYPE_1X_CP,                                        \
                   TM_SESS_HANDLE_1X_CP_MT,                                   \
                   data_type,                                                 \
                   post_data_ptr) == FALSE)                                   \
  {                                                                           \
    MSG_ERROR("Post data type %d failed",data_type,0,0);                      \
    return;                                                                   \
  }                                                                           \
}while(0);      

/****************************************************************************
 *                      LOCAL FUNCTION DEFINITIONS                          *
 ****************************************************************************/
static boolean i_tm_1x_cp_create_801_1_msi_resp
(
  is801_rev_link_resp_s_type *resp_ptr
);

static boolean i_tm_1x_cp_send_is801_1_rlink_pddm(byte sess_end);
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
#error code not present
#endif

static tm_sess_req_param_u_type    tm_1x_cp_sess_req_param;
/*===========================================================================

FUNCTION tm_1x_cp_start_sess_req

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_1x_cp_start_sess_req()
{
  boolean ret_val = TRUE;
  tm_1x_cp_sess_req_param.start_param.op_req  = TM_OP_REQ_NONE;
  tm_1x_cp_sess_req_param.start_param.op_mode = TM_SESS_OPERATION_MODE_MSA;
  tm_1x_cp_sess_req_param.start_param.ppm_qos = TM_MGP_PPM_TIMEOUT_MAX/1000;
  tm_1x_cp_sess_req_param.start_param.num_fixes = 1;
  tm_1x_cp_sess_req_param.start_param.tbf_ms = 0;


#ifdef FEATURE_JCDMA_GPSONE
  tm_1x_cp_sess_req_param.start_param.abort_curr_sess = TRUE;
#else
  tm_1x_cp_sess_req_param.start_param.abort_curr_sess = FALSE;
#endif

  if(tm_sess_req(TM_PRTL_TYPE_1X_CP,
             TM_SESS_HANDLE_1X_CP_MT,
             TM_SESS_REQ_START,
             &tm_1x_cp_sess_req_param))
  {
    MSG_MED("tm_1x_cp_start_sess_req Succeeded", 0, 0, 0);  
    tm_1x_cp_session_info.tm_core_session_started = TRUE;
  }
  else
  {
    MSG_ERROR("tm_1x_cp_start_sess_req Failed", 0, 0, 0);
    ret_val = FALSE;
  }          
  return ret_val;
}

/*===========================================================================

FUNCTION tm_1x_cp_terminate_sess_req

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_1x_cp_terminate_sess_req(tm_sess_stop_reason_e_type reason)
{
  tm_1x_cp_sess_req_param.stop_param.stop_type    = TM_STOP_TYPE_SESSION;
  tm_1x_cp_sess_req_param.stop_param.stop_reason  = reason;
  if( tm_sess_req(TM_PRTL_TYPE_1X_CP,
                  TM_SESS_HANDLE_1X_CP_MT,
                  TM_SESS_REQ_STOP,
                 &tm_1x_cp_sess_req_param))
  {
    MSG_MED("tm_1x_cp_terminate_sess_req Succeeded", 0, 0, 0);  
    return TRUE;
  }
  else
  {
    MSG_ERROR("tm_1x_cp_terminate_sess_req Failed", 0, 0, 0);  
    return FALSE;
  }
}

/*===========================================================================

FUNCTION tm_1x_cp_start_ppm

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_1x_cp_start_ppm(tm_1x_cp_sess_req_e_type req_type)
{
  boolean ret_val = TRUE;
  tm_sess_req_type sess_req_type;

  if(req_type == TM_1X_CP_SESS_START)
  {
      tm_1x_cp_sess_req_param.start_param.op_req  = TM_OP_REQ_PPM;
      tm_1x_cp_sess_req_param.start_param.op_mode = TM_SESS_OPERATION_MODE_MSA;
      tm_1x_cp_sess_req_param.start_param.ppm_qos = TM_MGP_PPM_TIMEOUT_MAX/1000;
      tm_1x_cp_sess_req_param.start_param.num_fixes = 1;
      tm_1x_cp_sess_req_param.start_param.tbf_ms = 0;
      tm_1x_cp_sess_req_param.start_param.abort_curr_sess = FALSE;

      sess_req_type = TM_SESS_REQ_START;
  }
  else
  {
      tm_1x_cp_sess_req_param.continue_param.op_req  = TM_OP_REQ_PPM;
      tm_1x_cp_sess_req_param.continue_param.op_mode = TM_SESS_OPERATION_MODE_MSA;
      tm_1x_cp_sess_req_param.continue_param.ppm_qos = TM_MGP_PPM_TIMEOUT_MAX/1000;
      tm_1x_cp_sess_req_param.continue_param.num_fixes = 1;
      tm_1x_cp_sess_req_param.continue_param.tbf_ms = 0;
      sess_req_type = TM_SESS_REQ_CONTINUE;
  }

  if(tm_sess_req(TM_PRTL_TYPE_1X_CP,
             TM_SESS_HANDLE_1X_CP_MT,
             sess_req_type,
             &tm_1x_cp_sess_req_param))
  {
    MSG_MED("tm_1x_cp_start_ppm Succeeded", 0, 0, 0);  
    tm_1x_cp_session_info.mgp_state.ppm_on = TRUE;
  }
  else
  {
    MSG_ERROR("tm_1x_cp_start_ppm Failed", 0, 0, 0);
    ret_val = FALSE;
  }                  
  return ret_val;
}
/*===========================================================================

FUNCTION tm_1x_cp_get_ppm

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_1x_cp_get_ppm()
{
   boolean ret_val = TRUE;
   tm_1x_cp_sess_req_param.polling_data_param.op_req = TM_OP_REQ_PPM;
                          
   if(tm_sess_req(TM_PRTL_TYPE_1X_CP,
              TM_SESS_HANDLE_1X_CP_MT,
              TM_SESS_REQ_POLLING_DATA,
              &tm_1x_cp_sess_req_param))
   {
     MSG_MED("tm_1x_cp_get_ppm Succeeded", 0, 0, 0);
     if(!os_TimerStart( tm_1x_cp_session_info.tm_1x_cp_ppm_timer,
                        tm_1x_cp_session_info.ppm_timeout,NULL ))
     {
       MSG_ERROR("Failed starting MGP PPM timer",0,0,0);
     }
   }
   else
   {
     MSG_ERROR("tm_1x_cp_get_ppm Failed", 0, 0, 0);
     ret_val = FALSE;
   }                  
   return ret_val;
}

/*===========================================================================

FUNCTION tm_1x_cp_end_ppm

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_1x_cp_end_ppm()
{
    boolean ret_val = TRUE;
    tm_1x_cp_sess_req_param.stop_param.stop_type    = TM_STOP_TYPE_OPERATION;
    tm_1x_cp_sess_req_param.stop_param.stop_reason  = TM_STOP_REASON_COMPLETED;
    tm_1x_cp_sess_req_param.stop_param.op_to_stop   = TM_OP_REQ_PPM;

    if(tm_sess_req(TM_PRTL_TYPE_1X_CP,
               TM_SESS_HANDLE_1X_CP_MT,
               TM_SESS_REQ_STOP,
               &tm_1x_cp_sess_req_param))
    {
      MSG_MED("tm_1x_cp_end_ppm Succeeded", 0, 0, 0);  
      (void)os_TimerStop(tm_1x_cp_session_info.tm_1x_cp_ppm_timer);  
      tm_1x_cp_session_info.mgp_state.ppm_on = FALSE;
    }
    else
    {
      MSG_ERROR("tm_1x_cp_end_ppm Failed", 0, 0, 0);
      ret_val = FALSE;
    }        
    return ret_val;
}

/*===========================================================================

FUNCTION tm_1x_cp_start_prm

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_1x_cp_start_prm()
{
    boolean ret_val = TRUE;
    tm_1x_cp_sess_req_param.continue_param.op_req  = TM_OP_REQ_PRM;
    tm_1x_cp_sess_req_param.continue_param.op_mode = TM_SESS_OPERATION_MODE_MSA;
    tm_1x_cp_sess_req_param.continue_param.prm_qos = tm_1x_cp_session_info.gps_session_timeout;
    tm_1x_cp_sess_req_param.continue_param.prm_prq = tm_1x_cp_session_info.prm_prq;
    tm_1x_cp_sess_req_param.continue_param.num_fixes = 1;
    tm_1x_cp_sess_req_param.continue_param.tbf_ms = 0;
    tm_1x_cp_sess_req_param.continue_param.multi_report_msa = FALSE;

    if(tm_sess_req(TM_PRTL_TYPE_1X_CP,
                 TM_SESS_HANDLE_1X_CP_MT,
                 TM_SESS_REQ_CONTINUE,
                 &tm_1x_cp_sess_req_param))
    {
      MSG_MED("tm_1x_cp_start_prm Succeeded", 0, 0, 0);  
      tm_1x_cp_session_info.mgp_state.prm_on = TRUE;
    }
    else
    {
      MSG_ERROR("tm_1x_cp_start_prm Failed", 0, 0, 0);
      ret_val = FALSE;
    }                  
    return ret_val;
}
          
/*===========================================================================

FUNCTION tm_1x_cp_end_prm

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_1x_cp_end_prm()
{
    boolean ret_val = TRUE;
    tm_1x_cp_sess_req_param.stop_param.stop_type    = TM_STOP_TYPE_OPERATION;
    tm_1x_cp_sess_req_param.stop_param.stop_reason  = TM_STOP_REASON_COMPLETED;
    tm_1x_cp_sess_req_param.stop_param.op_to_stop   = TM_OP_REQ_PRM;

    if(tm_sess_req(TM_PRTL_TYPE_1X_CP,
               TM_SESS_HANDLE_1X_CP_MT,
               TM_SESS_REQ_STOP,
               &tm_1x_cp_sess_req_param))
    {
      MSG_MED("tm_1x_cp_end_prm Succeeded", 0, 0, 0);  
      tm_1x_cp_session_info.mgp_state.prm_on = FALSE;
    }
    else
    {
      MSG_ERROR("tm_1x_cp_end_prm Failed", 0, 0, 0);
      ret_val = FALSE;
    }                  
    return ret_val;
}

/*===========================================================================

FUNCTION xlate_fl_resp_mask_is801_1_to_1x_cp

DESCRIPTION: Translate from Is801-1 fwd link response type to generic 1x CP
             response type.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
tm_1x_cp_fl_resp_type xlate_fl_resp_mask_is801_1_to_1x_cp
(  
  is801_fwd_link_resp_mask_type resp_mask
)
{
  tm_1x_cp_fl_resp_type ret_mask = 0;

  if(resp_mask & IS801_FWD_LINK_RESP_MASK_REJ)
    ret_mask |= TM_1X_CP_FL_RESP_REJ;
  if(resp_mask & IS801_FWD_LINK_RESP_MASK_BSC)
    ret_mask |= TM_1X_CP_FL_RESP_BSC;
  if(resp_mask & IS801_FWD_LINK_RESP_MASK_AA)
    ret_mask |= TM_1X_CP_FL_RESP_AA;
  if(resp_mask & IS801_FWD_LINK_RESP_MASK_LASC)
    ret_mask |= TM_1X_CP_FL_RESP_LASC;
  if(resp_mask & IS801_FWD_LINK_RESP_MASK_LACC)
    ret_mask |= TM_1X_CP_FL_RESP_LACC;
  if(resp_mask & IS801_FWD_LINK_RESP_MASK_SA)
    ret_mask |= TM_1X_CP_FL_RESP_SA;
  if(resp_mask & IS801_FWD_LINK_RESP_MASK_BSA)
    ret_mask |= TM_1X_CP_FL_RESP_BSA;
  if(resp_mask & IS801_FWD_LINK_RESP_MASK_ALM)
    ret_mask |= TM_1X_CP_FL_RESP_ALM;
  if(resp_mask & IS801_FWD_LINK_RESP_MASK_EPH)
    ret_mask |= TM_1X_CP_FL_RESP_EPH;
  if(resp_mask & IS801_FWD_LINK_RESP_MASK_NMB)
    ret_mask |= TM_1X_CP_FL_RESP_NMB;
  if(resp_mask & IS801_FWD_LINK_RESP_MASK_LR)
    ret_mask |= TM_1X_CP_FL_RESP_LR;
  if(resp_mask & IS801_FWD_LINK_RESP_MASK_AC)
    ret_mask |= TM_1X_CP_FL_RESP_AC;
  if(resp_mask & IS801_FWD_LINK_RESP_MASK_SHI)
    ret_mask |= TM_1X_CP_FL_RESP_SHI;

  return ret_mask;
}

/*===========================================================================

FUNCTION xlate_fl_resp_mask_is801_b_to_1x_cp

DESCRIPTION: Translate from Is801-B fwd link response type to generic 1x CP
             response type.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
tm_1x_cp_fl_resp_type xlate_fl_resp_mask_is801_b_to_1x_cp
(  
  is801B_fwd_link_resp_mask_type resp_mask
)
{
  tm_1x_cp_fl_resp_type ret_mask = 0;

  if(resp_mask & IS801B_FWD_LINK_RESP_MASK_REJ)
    ret_mask |= TM_1X_CP_FL_RESP_REJ;
  if(resp_mask & IS801B_FWD_LINK_RESP_MASK_ADV_REJ)
    ret_mask |= TM_1X_CP_FL_RESP_ADV_REJ;
  if(resp_mask & IS801B_FWD_LINK_RESP_MASK_BSC)
    ret_mask |= TM_1X_CP_FL_RESP_BSC;
  if(resp_mask & IS801B_FWD_LINK_RESP_MASK_ADV_BSC)
    ret_mask |= TM_1X_CP_FL_RESP_ADV_BSC;
  if(resp_mask & IS801B_FWD_LINK_RESP_MASK_GNSS_AA)
    ret_mask |= TM_1X_CP_FL_RESP_AA;
  if(resp_mask & IS801B_FWD_LINK_RESP_MASK_GPS_SA)
    ret_mask |= TM_1X_CP_FL_RESP_SA;
  if(resp_mask & IS801B_FWD_LINK_RESP_MASK_GPS_ALM)
    ret_mask |= TM_1X_CP_FL_RESP_ALM;
  if(resp_mask & IS801B_FWD_LINK_RESP_MASK_GPS_EPH)
    ret_mask |= TM_1X_CP_FL_RESP_EPH;
  if(resp_mask & IS801B_FWD_LINK_RESP_MASK_GLO_ALM)
    ret_mask |= TM_1X_CP_FL_RESP_ALM;
  if(resp_mask & IS801B_FWD_LINK_RESP_MASK_GLO_EPH)
    ret_mask |= TM_1X_CP_FL_RESP_EPH;
  if(resp_mask & IS801B_FWD_LINK_RESP_MASK_ADV_LR)
    ret_mask |= TM_1X_CP_FL_RESP_LR;
  if(resp_mask & IS801B_FWD_LINK_RESP_MASK_ADV_SVHI)
    ret_mask |= TM_1X_CP_FL_RESP_SHI;
  if(resp_mask & IS801B_FWD_LINK_RESP_MASK_GGTO)
    ret_mask |= TM_1X_CP_FL_RESP_GGTO;
  if(resp_mask & IS801B_FWD_LINK_RESP_MASK_EXT_REJ ||
     resp_mask & IS801B_FWD_LINK_RESP_MASK_DGNSS_ASST )
  {
    MSG_ERROR("IS801B flink response %x not translated", 
                 resp_mask, 0, 0);
  }

  return ret_mask;
}

/*===========================================================================

FUNCTION xlate_fl_req_mask_is801_1_to_1x_cp

DESCRIPTION: Translate from Is801-1 fwd link request type to generic 1x CP
             request type. Only those req that are used by the 1xCP state 
             machine (MSI, PPM, CAN, Adv CAN, PRM) are translated, rest are 
         not.

             The untranslated reqs are returned in the pointers passed as 
         input, these will constitute the reqs to be rejected. 

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
tm_1x_cp_fl_req_type xlate_fl_req_mask_is801_1_to_1x_cp
(  
  is801_fwd_link_req_mask_type *req_mask
)
{
  tm_1x_cp_fl_req_type ret_mask = 0;

  if(*req_mask & IS801_FWD_LINK_REQ_MASK_MSI)
    ret_mask |= TM_1X_CP_FL_REQ_MSI;
  if(*req_mask & IS801_FWD_LINK_REQ_MASK_PRM)
    ret_mask |= TM_1X_CP_FL_REQ_PRM;
  if(*req_mask & IS801_FWD_LINK_REQ_MASK_PPM)
    ret_mask |= TM_1X_CP_FL_REQ_PPM;
  if(*req_mask & IS801_FWD_LINK_REQ_MASK_CAN)
    ret_mask |= TM_1X_CP_FL_REQ_CAN;

  /* all other reqs are to be rejected. Clear the bits for reqs that are to be
   * responded to */
  *req_mask &= ~( IS801_FWD_LINK_REQ_MASK_MSI |
                  IS801_FWD_LINK_REQ_MASK_PRM |
                  IS801_FWD_LINK_REQ_MASK_PPM | 
                  IS801_FWD_LINK_REQ_MASK_CAN);
  
  /* AMWF and LR req are always to be rejected
  if(req_mask & IS801_FWD_LINK_REQ_MASK_TOM)
    ret_mask |= TM_1X_CP_FL_REQ_TOM;
  if(req_mask & IS801_FWD_LINK_REQ_MASK_AMWF)
    ret_mask |= TM_1X_CP_FL_REQ_AMWF;
  if(req_mask & IS801_FWD_LINK_REQ_MASK_LR)
    ret_mask |= TM_1X_CP_FL_REQ_LR;
  */
  return ret_mask;
}

/*===========================================================================

FUNCTION xlate_fl_req_mask_is801_b_to_1x_cp

DESCRIPTION: Translate from Is801-B fwd link request type to generic 1x CP
             request type. Only those req that are used by the 1xCP state 
             machine (MSI, PPM, CAN, Adv CAN, PRM) are translated, rest are 
         not.

             The untranslated reqs are returned in the pointers passed as 
         input, these will constitute the reqs to be rejected. 

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
tm_1x_cp_fl_req_type xlate_fl_req_mask_is801_b_to_1x_cp
(  
  is801B_fwd_link_req_mask_type *req_mask_v1,
  is801B_fwd_link_req_mask_type *req_mask_v3
)
{
  tm_1x_cp_fl_req_type ret_mask = 0;

  /* These are the IS801B reqs that are handled by 1xCP state machine */
  if(*req_mask_v1 & IS801B_FWD_LINK_REQ_MASK_MSI)
    ret_mask |= TM_1X_CP_FL_REQ_MSI;
  if(*req_mask_v1 & IS801B_FWD_LINK_REQ_MASK_PPM)
    ret_mask |= TM_1X_CP_FL_REQ_PPM;
  if(*req_mask_v1 & IS801B_FWD_LINK_REQ_MASK_CAN)
    ret_mask |= TM_1X_CP_FL_REQ_CAN;

  if(*req_mask_v3 & IS801B_FWD_LINK_REQ_MASK_GNSS_PRM)
    ret_mask |= TM_1X_CP_FL_REQ_PRM;
  if(*req_mask_v3 & IS801B_FWD_LINK_REQ_MASK_ADV_MSI)
    ret_mask |= TM_1X_CP_FL_REQ_ADV_MSI;
  if(*req_mask_v3 & IS801B_FWD_LINK_REQ_MASK_ADV_CAN)
    ret_mask |= TM_1X_CP_FL_REQ_ADV_CAN;

  /* all other reqs are to be rejected. Clear the bits for reqs that are to be
   * responded to */
  *req_mask_v1 &= ~( IS801B_FWD_LINK_REQ_MASK_MSI |
                     IS801B_FWD_LINK_REQ_MASK_PPM |
                     IS801B_FWD_LINK_REQ_MASK_CAN );
              
  *req_mask_v3 &= ~( IS801B_FWD_LINK_REQ_MASK_ADV_MSI  |
                     IS801B_FWD_LINK_REQ_MASK_GNSS_PRM |
                     IS801B_FWD_LINK_REQ_MASK_ADV_CAN  );
  return ret_mask;
}

#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
#error code not present
#endif

/*===========================================================================

FUNCTION xlate_cancel_req_is801_1_to_1x_cp

DESCRIPTION: Translate from Is801-1 cancellation message request type to 
             generic 1x CP request type. 

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
tm_1x_cp_fl_req_type xlate_cancel_req_is801_1_to_1x_cp
(
  pde_to_ms_req_type            cancel_type
)
{
  switch(cancel_type)
  {
    case(LOC_RESP_PDE_REQ_CODE):      
      return TM_1X_CP_FL_REQ_LR;

    case(MS_INFO_REQ_CODE):            
      return TM_1X_CP_FL_REQ_MSI;

    case(AUTO_MEAS_WGHT_FACTR_REQ_CODE):
      return TM_1X_CP_FL_REQ_AMWF;

    case(PR_MEAS_REQ_CODE):         
      return TM_1X_CP_FL_REQ_GPS_L1_PRM;

    case(PPM_MEAS_REQ_CODE):         
      return TM_1X_CP_FL_REQ_PPM;

    case(TIME_OFFSET_MEAS_REQ_CODE):    
      return TM_1X_CP_FL_REQ_TOM;

    case(CANCEL_REQ_CODE):
      return TM_1X_CP_FL_REQ_CAN;

    default:
      return TM_1X_CP_FL_REQ_NONE;
  }
}
/*===========================================================================

FUNCTION i_tm_1x_cp_post_aa

DESCRIPTION: post 801 (Both 801-1 and 801-B) gps aa to mgp. 

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void i_tm_1x_cp_post_aa
(
  tm_post_data_payload_type   *post_data_ptr,
  is801_fwd_link_resp_s_type  *is801_1_aa,
  is801b_flink_ie_gnss_aa     *is801_b_aa,
  gps_PreFixPpmSructType      *cp_prefix_ppm_buff_ptr
)
{
  /* Send prefix-PPM along with AA data */
  if(cp_prefix_ppm_buff_ptr)
  {
    memscpy( (void*) &(post_data_ptr->ppm_data), 
            sizeof(post_data_ptr->ppm_data),
            (const void *)cp_prefix_ppm_buff_ptr, 
            sizeof(*cp_prefix_ppm_buff_ptr)); /* gps_PreFixPpmSructType */

    MSG_MED("Send pre-fix PPM data to TM-Core", 0, 0, 0);

    /* post prefix ppm to TM, return on error in post operation */
    TM_POST_DATA_PRTL_CP(TM_POST_DATA_TYPE_PREFIX_PPM);
  }
  else
  {
    /* No need to exit call flow here as in initial GPS fix call flow, AA resp 
     * is received with PRM req */
    MSG_MED("prefix_ppm buffer empty",0,0,0);
  }

  /* Check if PDE asked for PRM response before AA is posted */
  if(( tm_1x_cp_session_info.is801_resp_to_send_bmap & 
       TM_1X_CP_RL_RESP_PRM) == 0)
  {
    MSG_ERROR("PRM not requested, no need to post AA. req_send_bmap 0x%x",
               tm_1x_cp_session_info.is801_resp_to_send_bmap,0,0);
    return;
  }

  /* If PRM is already ON, no need to post AA a second time */
  if(tm_1x_cp_session_info.mgp_state.prm_on == TRUE )
  {
    MSG_HIGH("prm already on", 0, 0, 0);
    return;
  }

  /* PRM not on, start PRM */
  if(!tm_1x_cp_start_prm())
  {
    MSG_HIGH("Could not start prm", 0, 0, 0);
    return;
  }
  
  /* PRM started, post AA to MGP */
  /* Convert data from protocol format to mgp format */
  MSG_MED("Send AA data to TM-Core", 0, 0, 0);

  if( tm_1x_cp_session_info.pde_version == IS801_VERSION_801_1 )
  {
    /*** Convert AA from 801-1 to MGP format ***/
    (void)is801_util_flink_convert_aa( 
                  &(is801_1_aa->resp_msg_payload.aa_resp),
                  &(post_data_ptr->gnss_aa_data),
                    NULL );

    /* post AA data to TM, return on error in post operation */
    TM_POST_DATA_PRTL_CP(TM_POST_DATA_TYPE_AA);

    (void)is801_util_flink_convert_aa( 
                  &(is801_1_aa->resp_msg_payload.aa_resp),
                  NULL,
                  &post_data_ptr->gnss_sv_dir);

    /* post Sv Dir data to TM, return on error in post operation */
    TM_POST_DATA_PRTL_CP(TM_POST_DATA_TYPE_SV_DIR);
  }
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif
  else
  {
    MSG_ERROR("Unknown pde version %d", tm_1x_cp_session_info.pde_version, 0,0);
    return;
  }

  return;
}

/*===========================================================================

FUNCTION i_tm_1x_cp_create_is801_1_ppm_resp

DESCRIPTION: Create a ppm response using the given ppms and the rlink buffer.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If successful
  FALSE:  If unsuccessful

SIDE EFFECTS
  Will terminate active session if ppm response can not be created

===========================================================================*/
boolean i_tm_1x_cp_create_is801_1_ppm_resp
(
  is801_rev_link_resp_s_type *resp_ptr,
  srch_ppm_rpt_type          *ppm_ptr,
  uint8                       unsolicited 
)
{
  if(resp_ptr == NULL)
    return FALSE;

  resp_ptr->response_type = IS801_REV_LINK_RESP_MASK_PPM;
  resp_ptr->unsolicited   = unsolicited;

  if(!is801_util_rlink_convert_ppm(
          ppm_ptr,
          &(resp_ptr->resp_msg_payload.ppm_resp)))
  {
    MSG_ERROR("Can not convert Rlink resp for PPM",0,0,0);
    return FALSE;    
  }

  if( !is801_new_rev_link_resp(TM_PRTL_TYPE_1X_CP, resp_ptr) )
  {
    MSG_ERROR("Can not create Rlink resp for PRM",0,0,0);
    return FALSE;
  }

  MSG_MED("Created PPM resp",0,0,0);
  return TRUE;
}

/*===========================================================================

FUNCTION i_tm_1x_cp_send_is801_1_rlink_pddm

DESCRIPTION

RETURN VALUE
  boolean

DEPENDENCIES

===========================================================================*/
static boolean i_tm_1x_cp_send_is801_1_rlink_pddm(byte sess_end)
{
  int       num_rev_pddm;
  uint8     i;
  is801_pddm_sess_info_s_type      rev_pddm_hdr; 

  rev_pddm_hdr.pd_msg_type  = PD_MSG_TYPE;
  rev_pddm_hdr.sess_start   = 0;
  rev_pddm_hdr.sess_end     = sess_end;
  rev_pddm_hdr.sess_source  = last_pddm_hdr.sess_source;
  rev_pddm_hdr.sess_tag     = last_pddm_hdr.sess_tag;

  /* Instruct IS801 to send all req and rsp as a single pkt to CM
  */
  num_rev_pddm = is801_create_rev_link_pddm(
                              TM_PRTL_TYPE_1X_CP,
                              rev_pddm_hdr,
                              tm_1x_cp_config.dbm_pddm_size);

  if(num_rev_pddm < 0)
  {
    MSG_ERROR("IS801 returned PDDMs = %d",num_rev_pddm,0,0);
    return FALSE;
  }

  MSG_MED("Num PDDMs to be sent= %d",num_rev_pddm,0,0);

  for(i = 0; i < num_rev_pddm; i++)
  {
    if(is801_get_rev_link_pddm( TM_PRTL_TYPE_1X_CP, 
                               &tm_1x_cp_session_info.rev_link_pddm))
    {
      is801_util_log_rlink_msg(
            (void *)&(tm_1x_cp_session_info.rev_link_pddm.msg_payload[0]), 
            tm_1x_cp_session_info.rev_link_pddm.msg_size);

      if(tm_1x_cp_session_info.rev_link_pddm.req_mask)
      {
        is801_util_report_is801_event_tx(
            IS801_REQ, tm_1x_cp_session_info.rev_link_pddm.req_mask);
      }
      if(tm_1x_cp_session_info.rev_link_pddm.resp_mask)
      {
        is801_util_report_is801_event_tx(
            IS801_RESP, tm_1x_cp_session_info.rev_link_pddm.resp_mask);
      }
      /* Command CM to send data across to PDE.
      */
      if(tm_cm_iface_send_data(
                TM_PRTL_TYPE_1X_CP, 
                (byte *)&(tm_1x_cp_session_info.rev_link_pddm.msg_payload[0]), 
                tm_1x_cp_session_info.rev_link_pddm.msg_size) == FALSE)
      {
        MSG_ERROR("Can not send data to CM",0,0,0);
        return FALSE;
      }
    }
    else
    {
      MSG_ERROR("Can not get Rlink PDDM from IS801 module",0,0,0);
      return FALSE;
    }
  }
  return TRUE;
}


/*===========================================================================

FUNCTION i_tm_1x_cp_send_rlink_pddm

DESCRIPTION

RETURN VALUE
  boolean

DEPENDENCIES

===========================================================================*/
boolean i_tm_1x_cp_send_rlink_pddm
(
  byte sess_end,
  byte pde_version
)
{
  boolean ret_val = FALSE;

  if( pde_version == IS801_VERSION_801_1 )
  {
    ret_val = i_tm_1x_cp_send_is801_1_rlink_pddm(sess_end);
  }
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif
  else
  {
    MSG_ERROR("Unknown pde version %d", pde_version, 0, 0);
  }

  return ret_val;
} /* i_tm_1x_cp_send_rlink_pddm */

#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
#error code not present
#endif /* FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE */

/*===========================================================================

FUNCTION i_tm_1x_cp_create_and_tx_is801_1_tom_resp

DESCRIPTION: Create and transmit a tom response using the given ppms and 
             the rlink buffer.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If successful
  FALSE:  If unsuccessful

SIDE EFFECTS

===========================================================================*/
boolean i_tm_1x_cp_create_and_tx_is801_1_tom_resp
(
  is801_rev_link_resp_s_type *resp_ptr,
  srch_ppm_rpt_type          *ppm_ptr,
  uint8                       unsolicited 
)
{
  if(resp_ptr == NULL)
    return FALSE;

  resp_ptr->response_type = IS801_REV_LINK_RESP_MASK_TOM;
  resp_ptr->unsolicited = unsolicited;
  (void)is801_util_rlink_convert_msto(ppm_ptr,&(resp_ptr->resp_msg_payload.tom_resp));
  if (!is801_new_rev_link_resp( TM_PRTL_TYPE_1X_CP,resp_ptr))
  {
    MSG_ERROR("Can not create Rlink resp for TOM",0,0,0);
    return FALSE;
  }

  MSG_MED("Created TOM resp",0,0,0);

  if(!i_tm_1x_cp_send_rlink_pddm(0, tm_1x_cp_session_info.pde_version))
  {
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

FUNCTION i_tm_1x_cp_create_is801_1_prm_resp

DESCRIPTION: Create and transmit a prm response using the given prms and 
             the rlink buffer.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If successful
  FALSE:  If unsuccessful

SIDE EFFECTS

===========================================================================*/
boolean i_tm_1x_cp_create_is801_1_prm_resp
(
  is801_rev_link_resp_s_type *resp_ptr,
  srch_ppm_rpt_type          *ppm_ptr,
  uint8                       unsolicited, 
  sm_GpsMeasRptStructType    *gps_prm_ptr
)
{
  /* Its possible for the ppm ptr to be NULL */
  if(resp_ptr == NULL || gps_prm_ptr == NULL)
  {
    MSG_ERROR("NULL ptrs passed resp %x prm %x", resp_ptr, gps_prm_ptr, 0);
    return FALSE;
  }

  resp_ptr->response_type = IS801_REV_LINK_RESP_MASK_PRM;
  resp_ptr->unsolicited   = unsolicited;                  

  (void)is801_util_rlink_convert_prm(
            ppm_ptr,
            gps_prm_ptr,
            &(resp_ptr->resp_msg_payload.prm_resp));   

  if(!is801_new_rev_link_resp(TM_PRTL_TYPE_1X_CP, resp_ptr))
  {
    MSG_ERROR("Can not create Rlink resp for PRM, terminate session",0,0,0);
    return FALSE;
  }

  MSG_MED("Created PRM resp",0,0,0);

  return TRUE;
}

#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
#error code not present
#endif /* FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE */

/*===========================================================================
FUNCTION i_tm_1x_cp_reject_is801_request

DESCRIPTION
  This function is used to create a reject response
  
DEPENDENCIES
  
RETURN VALUE  
  TRUE: Reject response created
  FALSE: Reject response creation failed

SIDE EFFECTS
  NONE
===========================================================================*/

static boolean i_tm_1x_cp_reject_is801_request
(
  is801_rev_link_resp_s_type *resp_ptr,
  pde_to_ms_req_type          rej_req_mask,
  byte                        reason
)
{
  resp_ptr->response_type = IS801_REV_LINK_RESP_MASK_REJ;
  resp_ptr->unsolicited   = 0; /* rejects are always solicited */

  resp_ptr->resp_msg_payload.rej_resp.rej_req_type = (byte) rej_req_mask;
  resp_ptr->resp_msg_payload.rej_resp.rej_reason   = reason;
  if( !is801_new_rev_link_resp( TM_PRTL_TYPE_1X_CP,resp_ptr))
  {
    MSG_ERROR("Could not create rej resp for req 0x%x", rej_req_mask, 0, 0);
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION i_tm_1x_cp_reject_is801_1_requests

DESCRIPTION
  This function is used to queue IS801-1 reject responses for PDE. The return
  value is true if any IS801 Req needs to be rejected. An MSI req however,
  is not rejected
  
DEPENDENCIES
  
RETURN VALUE  
TRUE:  Some response needs to be sent to PDE
FALSE: No response needs to be sent to PDE


SIDE EFFECTS

===========================================================================*/
boolean i_tm_1x_cp_reject_is801_1_requests
(
  is801_fwd_link_req_mask_type     is801_1_v1_rej_req,
  is801_rev_link_resp_s_type      *resp_ptr
)
{
  boolean                        ret_val         = FALSE;
  is801_fwd_link_req_mask_type   reject_req_bmap =0;

  MSG_MED("Handle reject request is801_1_v1_rej_req=0x%x ",
             is801_1_v1_rej_req, 0, 0);

  if ( is801_1_v1_rej_req & IS801_FWD_LINK_REQ_MASK_MSI ) 
  {
    if(i_tm_1x_cp_create_801_1_msi_resp(resp_ptr))
    {
        ret_val = TRUE;
    }
  } /* if BMAP_MSI */

  if( is801_1_v1_rej_req & IS801_FWD_LINK_REQ_MASK_PPM)
  {
    if(i_tm_1x_cp_reject_is801_request( 
                         resp_ptr,
                         PPM_MEAS_REQ_CODE, 1))
    {
      MSG_MED("Rejecting PPM request",0,0,0);
      reject_req_bmap |= IS801_FWD_LINK_REQ_MASK_PPM;
      ret_val = TRUE;
    }
  } /* End if PPM is requested */

  if( is801_1_v1_rej_req & IS801_FWD_LINK_REQ_MASK_PRM )
  {
    if(i_tm_1x_cp_reject_is801_request( 
                         resp_ptr,
                         PR_MEAS_REQ_CODE, 1))
    {
      MSG_MED("Rejecting PRM request",0,0,0);
      reject_req_bmap |= IS801_FWD_LINK_REQ_MASK_PRM;
      ret_val = TRUE;
    }
  } /* End if PR was requested */

  if ( is801_1_v1_rej_req & IS801_FWD_LINK_REQ_MASK_LR )   
  {
    if(i_tm_1x_cp_reject_is801_request( 
                         resp_ptr,
                         (pde_to_ms_req_type)LOC_RESP_REQ_CODE, 0))
    {
      MSG_MED("Rejecting LR request",0,0,0);
      reject_req_bmap |= IS801_FWD_LINK_REQ_MASK_LR;
      ret_val = TRUE;
    }
  }      

  if ( is801_1_v1_rej_req & IS801_FWD_LINK_REQ_MASK_TOM )   
  {
    if(i_tm_1x_cp_reject_is801_request( 
                         resp_ptr,
                         TIME_OFFSET_MEAS_REQ_CODE, 0))
    {
      MSG_MED("Rejecting TOM request",0,0,0);
      reject_req_bmap |= IS801_FWD_LINK_REQ_MASK_TOM;
      ret_val = TRUE;
    }
  }

  if ( is801_1_v1_rej_req & IS801_FWD_LINK_REQ_MASK_AMWF )   
  {
    if(i_tm_1x_cp_reject_is801_request( 
                         resp_ptr,
                         AUTO_MEAS_WGHT_FACTR_REQ_CODE, 0))
    {
      MSG_MED("Rejecting MWF request",0,0,0);
      reject_req_bmap |= IS801_FWD_LINK_REQ_MASK_AMWF;
      ret_val = TRUE;
    }
  }

  if(ret_val)
  {
    MSG_MED("reject bmap 0x%x", reject_req_bmap, 0, 0);
  }
  return ret_val;
} /* i_tm_1x_cp_reject_is801_1_requests */


/*===========================================================================
FUNCTION i_tm_1x_cp_create_801_1_msi_resp

DESCRIPTION: Depending on pde version, create a 801-1 MSI response or 801-Bv1
             or 801-Bv3 MSI response

DEPENDENCIES
  
RETURN VALUE  
TRUE  : MSI response created
FALSE : MSI response could not be created

SIDE EFFECTS
Terminates session if MSI response cant be created
===========================================================================*/
static boolean i_tm_1x_cp_create_801_1_msi_resp
(
  is801_rev_link_resp_s_type *resp_ptr
)
{
  boolean ret_val;
  resp_ptr->response_type = IS801_REV_LINK_RESP_MASK_MSI;
  resp_ptr->unsolicited = 0;

  resp_ptr->resp_msg_payload.msi_resp.gps_acq_cap  = GPS_ACQ_ASSIST | 
                                                       GPS_SEN_ASSIST;
  resp_ptr->resp_msg_payload.msi_resp.loc_calc_cap = NULL;
  /* Standard Revision num should be set as 000000 */
  resp_ptr->resp_msg_payload.msi_resp.ms_ls_rev    = NULL;
  /* Eight Chip Measurement Capability */
  resp_ptr->resp_msg_payload.msi_resp.pilot_ph_cap = SIXTEENTH_CHIP_MEAS_CAP;
  /* Digital Mode Indicator should be set to 0000 */
  resp_ptr->resp_msg_payload.msi_resp.ms_mode      = NULL; 
  ret_val =  is801_new_rev_link_resp( TM_PRTL_TYPE_1X_CP, resp_ptr);
  if(ret_val)
  {
    MSG_MED("Created MSI resp",0,0,0);
  }
  return ret_val;
}

/*===========================================================================
FUNCTION tm_1x_cp_create_msinfo_resp

DESCRIPTION: Depending on pde version, create a 801-1 MSI response or 801-Bv1
             or 801-Bv3 MSI response

DEPENDENCIES
  
RETURN VALUE  
TRUE  : MSI response created
FALSE : MSI response could not be created

SIDE EFFECTS
Terminates session if MSI response cant be created
===========================================================================*/
boolean i_tm_1x_cp_create_msi_resp
(
  is801_rev_link_resp_s_type *resp_ptr,
  tm_1x_cp_fl_req_type        fl_req,
  byte                        pde_version
)
{
  boolean ret_val = TRUE;

  /* First perform sanity */
  if(resp_ptr == NULL)
  {
    MSG_ERROR("no 801-1 rlink buffer ptr", 0, 0, 0);
    return FALSE;
  }

  /* PDE version must be 801-1 or 801-b, else declare error, terminate session
   * and return FALSE */
  if( pde_version != IS801_VERSION_801_1 &&
      pde_version != IS801_VERSION_801_B )
  {
    MSG_ERROR("Unknown PDE version, terminating session", 0, 0, 0);
    tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE, 
                               TM_STOP_REASON_GENERAL_ERROR);
    return FALSE;
  }

  /* If pde_version is 801-1, only req possible is MSInfo, handle that */
  if( (pde_version == IS801_VERSION_801_1) && 
      (fl_req & TM_1X_CP_FL_REQ_MSI) != 0 )
  {
    ret_val = i_tm_1x_cp_create_801_1_msi_resp(resp_ptr);
  }
#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
  #error code not present
#endif

  if(ret_val == FALSE)
  {
    MSG_ERROR("Can't create MSInfo. req mask %x", fl_req, 0, 0);
    tm_1x_cp_terminate_session( TM_1X_CP_TERM_SRC_NONE, 
                                TM_STOP_REASON_GENERAL_ERROR );
  }
  return ret_val;
}

#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
#error code not present
#endif /* FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE */

/*===========================================================================

FUNCTION i_tm_1x_cp_create_is801_1_cack_resp

DESCRIPTION: Create an IS801-1 cancellation acknowledgement response

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If successful
  FALSE:  If unsuccessful

SIDE EFFECTS

===========================================================================*/
boolean i_tm_1x_cp_create_is801_1_cack_resp
(
  uint8    cancel_no_out_req,
  byte     cancel_type,
  is801_rev_link_resp_s_type *resp_ptr
)
{
  if(resp_ptr == NULL)
  {
    MSG_ERROR("NULL pointer passed", 0, 0, 0);
    return FALSE;
  }

  resp_ptr->response_type = IS801_REV_LINK_RESP_MASK_CACK;
  resp_ptr->unsolicited = 0;

  resp_ptr->resp_msg_payload.cack_resp.cancel_type_ack = cancel_type;
  resp_ptr->resp_msg_payload.cack_resp.no_out_req      = cancel_no_out_req;

  if ( is801_new_rev_link_resp( TM_PRTL_TYPE_1X_CP,resp_ptr))
  {
    MSG_MED("Created CACK resp",0,0,0);
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/*===========================================================================

FUNCTION xlate_1x_cp_rl_req_to_is801_1

DESCRIPTION: translate from generic 1xcp reverse link requests to IS801-1 
             reverse link request. Only the desired IS801-1 requests that
         are used by 1xCP state machine are translated

DEPENDENCIES
  None.

RETURN VALUE
Translated IS801-1 reverse link request

SIDE EFFECTS

===========================================================================*/
is801_rev_link_req_mask_type  xlate_1x_cp_rl_req_to_is801_1
(
  tm_1x_cp_rlink_req_type req
)
{
  switch(req)
  {
    case(TM_1X_CP_RLINK_REQ_GPS_SA):
      return IS801_REV_LINK_REQ_MASK_SA;
    case(TM_1X_CP_RLINK_REQ_AA):    
      return IS801_REV_LINK_REQ_MASK_AA;
    case(TM_1X_CP_RLINK_REQ_LR):
      return IS801_REV_LINK_REQ_MASK_LR;
    default:
      return IS801_REV_LINK_REQ_MASK_NONE;
  }
}

/*===========================================================================

FUNCTION xlate_1x_cp_rl_req_to_is801_B

DESCRIPTION: translate from generic 1xcp reverse link requests to IS801-B 
             reverse link request. Only the desired IS801-B requests that
         are actually used by 1xCP state machine are translated

DEPENDENCIES
  None.

RETURN VALUE
Translated IS801-B reverse link request

SIDE EFFECTS

===========================================================================*/
is801B_rev_link_req_mask_type xlate_1x_cp_rl_req_to_is801_B
(
  tm_1x_cp_rlink_req_type req
)
{
  switch(req)
  {
    case(TM_1X_CP_RLINK_REQ_GPS_SA):
      return IS801B_REV_LINK_REQ_MASK_GNSS_SA;
    case(TM_1X_CP_RLINK_REQ_AA):    
      return IS801B_REV_LINK_REQ_MASK_GNSS_AA;
    case(TM_1X_CP_RLINK_REQ_LR):
      return IS801B_REV_LINK_REQ_MASK_ADV_LR;
    default:
      return IS801B_REV_LINK_REQ_MASK_NONE;
  }
}

/*===========================================================================

FUNCTION tm_1x_cp_xlate_is801_1_rlink_req_to_1x

DESCRIPTION: Translate IS801-1 fwd link resp reject type, to the 1xCP rlink
             request that it corresponded to. Only those requests that 1xCP
         actually sends are actually translated. Others are translated
         to 0 / TM_1X_CP_RLINK_REQ_NONE.

RETURN VALUE
tm_1x_cp_rlink_req_type : TM_1X_CP_RLINK_REQ_NONE if the type of rej resp is
                          for a req of type that is never used by 1xcp
            : Appropriate bit mask otherwise

DEPENDENCIES

===========================================================================*/
tm_1x_cp_rlink_req_type tm_1x_cp_xlate_is801_1_rlink_req_to_1x
(
  ms_to_pde_req_type rejected_req_type
)
{
  switch(rejected_req_type)
  {
    case(LOC_RESP_REQ_CODE):
      return  TM_1X_CP_RLINK_REQ_LR;

    case(GPS_AA_REQ_CODE):
      return  TM_1X_CP_RLINK_REQ_AA;

    case(GPS_SA_REQ_CODE):
      return  TM_1X_CP_RLINK_REQ_GPS_SA;

    /* these IS801-1 req types are never used by 1xCP, hence they are not
     * translated to 1xcp requests */
    case(BS_CAP_REQ_CODE):
    case(BS_ALM_REQ_CODE):
    case(GPS_LA_S_REQ_CODE):
    case(MS_RESERVED_REQ_CODE_2):
    case(GPS_ALM_REQ_CODE):
    case(GPS_EPH_REQ_CODE):
    case(GPS_NAV_MSG_REQ_CODE):
    case(GPS_ALM_COR_REQ_CODE):
    case(GPS_SAT_HLTH_REQ_CODE):
      MSG_ERROR("Unexpected IS801-1 reject req cmd %d from pde", 
                   rejected_req_type, 0, 0);
      return TM_1X_CP_RLINK_REQ_NONE;

    default:
      MSG_ERROR("Invalid IS801-1 reject req cmd %d from pde", 
                   rejected_req_type, 0, 0);
      return TM_1X_CP_RLINK_REQ_NONE;
  }
}

/*===========================================================================

FUNCTION tm_1x_cp_xlate_is801_b_rlink_req_to_1x

DESCRIPTION: Translate IS801-B fwd link resp reject type, to the 1xCP rlink
             request that it corresponded to. Only those requests that 1xCP
         actually sends are actually translated. Others are translated
         to 0 / TM_1X_CP_RLINK_REQ_NONE.

RETURN VALUE
tm_1x_cp_rlink_req_type : TM_1X_CP_RLINK_REQ_NONE if the type of rej resp is
                          for a req of type that is never used by 1xcp
            : Appropriate bit mask otherwise

DEPENDENCIES

===========================================================================*/
tm_1x_cp_rlink_req_type tm_1x_cp_xlate_is801_b_rlink_req_to_1x
(
  is801B_fwd_link_resp_mask_type reject_resp
)
{
  switch(reject_resp)
  {
    case(IS801B_FWD_LINK_RESP_MASK_ADV_LR):
      return  TM_1X_CP_RLINK_REQ_LR;

    case(IS801B_FWD_LINK_RESP_MASK_GNSS_AA):
      return  TM_1X_CP_RLINK_REQ_AA;

    case(IS801B_FWD_LINK_RESP_MASK_GPS_SA):
      return  TM_1X_CP_RLINK_REQ_GPS_SA;

    /* these IS801-1 req types are never used by 1xCP, hence they are not
     * translated to 1xcp requests */
    case(IS801B_FWD_LINK_RESP_MASK_BSC):
    case(IS801B_FWD_LINK_RESP_MASK_ADV_BSC):
    case(IS801B_FWD_LINK_RESP_MASK_GPS_ALM):
    case(IS801B_FWD_LINK_RESP_MASK_GPS_EPH): 
    case(IS801B_FWD_LINK_RESP_MASK_GLO_ALM):  
    case(IS801B_FWD_LINK_RESP_MASK_GLO_EPH):   
    case(IS801B_FWD_LINK_RESP_MASK_GGTO):  
    case(IS801B_FWD_LINK_RESP_MASK_ADV_SVHI):
    case(IS801B_FWD_LINK_RESP_MASK_DGNSS_ASST):
      MSG_ERROR("Unexpected IS801-B reject req cmd %d from pde", 
                   reject_resp, 0, 0);
      return TM_1X_CP_RLINK_REQ_NONE;

    default:
      MSG_ERROR("Invalid IS801-B reject req cmd %d from pde", 
                   reject_resp, 0, 0);
      return TM_1X_CP_RLINK_REQ_NONE;
  }
}



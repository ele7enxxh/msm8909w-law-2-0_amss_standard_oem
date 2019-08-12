
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM IS801 Execution

General Description
  This file contains implementations for utility functions that are needed
  for TM 1X UP Is801 implementation
  
  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2013 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/07/14   sj      Fix Memory Leak when Is801 aborts.
05/16/2013 mj     Handling Failure in converting PPM and PRDM

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"
#include "time_svc.h"
#include "tm_is801.h"
#include "tm_is801_util.h"
#include "tm_is801b_util.h"
#include "tm_common.h"
#include "tm_cm_iface.h"
#include "tm_1x_up.h"
#include "tm_1x_up_is801_drv_iface.h"
#include "tm_1x_up_is801_driver_i.h"
#include "tm_1x_up_comm.h"
#include "tm_1x_up_smod_iface.h"


extern tm_1x_up_pdcomm_info_s_type   pdcomm_info;        /* Transport layer information */

static gps_PreFixPpmSructType  *up_prefix_ppm_buff_ptr = NULL;
static sm_GpsMeasRptStructType *up_prm_buff_ptr_gps    = NULL;
static sm_GpsMeasRptStructType *up_prm_buff_ptr_glo    = NULL;


#define   TM_POST_DATA(data_type)                                             \
do {                                                                          \
  if(tm_post_data( TM_PRTL_TYPE_1X_UP,                                        \
                   tm_1x_up_session_info.sess_handle,                         \
                   data_type,                                                 \
                   post_data_ptr) == FALSE)                                   \
  {                                                                           \
    MSG_ERROR("Post data type %d failed",data_type,0,0);                      \
    return FALSE;                                                             \
  }                                                                           \
}while(0);      

/****************************************************************************
 *                          LOCAL DECLARAIONS                               *
 ***************************************************************************/
static boolean create_rlink_pddm(byte* sess_flags);

/*===========================================================================

FUNCTION create_msi_resp_pddm

DESCRIPTION: This function creates an IS801-1 or Is801-B MS Information response 
             depending on its input.

             In case the request is for an Is801B MSI creation, and IS801B is
             disabled via NV, or featurized off, no response is created.

RETURN VALUE
  TRUE     : No Errors in response creation.
  FALSE    : Response creation failed.

DEPENDENCIES

===========================================================================*/
static boolean create_msi_resp_pddm
(
  is801_rev_link_resp_s_type *resp_ptr,   
  is801_operation             op
)
{
  boolean ret_val = FALSE;
  /* create MSI response for all MS-Based call flows.
  */
  if(op == RL_RESP_MSI)
  {
    MSG_MED("Creating 801-1 MSI response", 0, 0, 0);
    resp_ptr->response_type = IS801_REV_LINK_RESP_MASK_MSI;
    resp_ptr->unsolicited = 1;
    (void)is801_util_rlink_create_msinfo(&(resp_ptr->resp_msg_payload.msi_resp));
    ret_val = is801_new_rev_link_resp( TM_PRTL_TYPE_1X_UP, resp_ptr);
  }
  else /* if(op == RL_RESP_B_MSI) */
  {
    #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
    #error code not present
#endif
    {
      MSG_MED("Ignoring disabled IS801B req to create adv MSI",0,0,0);
      ret_val = TRUE;
    }
  }

  if (ret_val)
  {
    MSG_MED("Created MSI response", 0, 0, 0);
  }
  else
  {
    MSG_ERROR("Can't create MSI response",0,0,0);
  }
  return ret_val;
}

#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
#error code not present
#endif

/*===========================================================================

FUNCTION create_prdm_bsp_resp_pddm

DESCRIPTION: This function creates a PRDM_BSP response (the Qcom proprietary
             System Parameter Information response), and creates a new IS801-1
             PDDM with the session start bit set. This is because the PRDM_BSP
             response is only sent at the start of a session, and must be sent
             in its own PDDM along with MS Information.
RETURN VALUE
  TRUE     : No Errors in PDDM creation
  FALSE    : PDDM creation failed.

DEPENDENCIES

===========================================================================*/
static boolean create_prdm_bsp_resp_pddm(is801_rev_link_resp_s_type *resp_ptr)
{
  byte    session_flags  = ( TM_1X_UP_SESS_START_MASK | 
                             TM_1X_UP_IS801_1_PDDM );
  boolean ret_val        =   TRUE;

  if(tm_1x_up_config.coarse_location_enabled == TRUE)
  {   
    resp_ptr->response_type = IS801_REV_LINK_RESP_MASK_PRDM_BSP;
    resp_ptr->unsolicited = 1;
    if(is801_util_rlink_create_prdm_bsp(&(resp_ptr->resp_msg_payload.prdm_bsp_resp)) == FALSE)
    {   
      MSG_ERROR("Can't create PRDM for LAT/LONG MSG",0,0,0);
      ret_val = FALSE;
      return ret_val;
    }

    
    if ( is801_new_rev_link_resp( TM_PRTL_TYPE_1X_UP,resp_ptr))
    {
      MSG_MED("Created PRDM BSP response", 0, 0, 0);
      ret_val = TRUE;
    }
    else
    {
      MSG_ERROR("Can't create PRDM for LAT/LONG MSG",0,0,0);
      ret_val = FALSE;
    }
        
    if(ret_val == TRUE)
    {
      ret_val = create_rlink_pddm(&session_flags);
    }
  }
  return ret_val;
}

/*===========================================================================

FUNCTION create_ppm_resp_pddm

DESCRIPTION: This function creates an IS801-1 or Is801-B PPM response depending 
             on its input.

             In case the request is for an Is801B PPM creation, and IS801B is
             disabled via NV, or featurized off, no PDDM is created.

RETURN VALUE
  TRUE     : No Errors in response creation.
  FALSE    : Response creation failed.

DEPENDENCIES

===========================================================================*/
static boolean create_ppm_resp_pddm
(
  is801_rev_link_resp_s_type *resp_801_1, 
  u_new_rev_link_resp_params *resp_801_b,
  srch_ppm_rpt_type          *ppm_ptr,
  is801_operation             op
)
{
  boolean ret_val = FALSE;

  if(op == RL_RESP_PPM)
  {
    resp_801_1->response_type = IS801_REV_LINK_RESP_MASK_PPM;
    resp_801_1->unsolicited = 1;

    ret_val = is801_util_rlink_convert_ppm(ppm_ptr,&(resp_801_1->resp_msg_payload.ppm_resp));

    if(ret_val == TRUE)
    {
      ret_val = is801_new_rev_link_resp( TM_PRTL_TYPE_1X_UP,resp_801_1);
    }
  }
  else /* if(op == RL_RESP_B_PPM) */
  {
    #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
    #error code not present
#endif
    {
      MSG_MED("Ignoring disabled IS801B req",0,0,0);
      ret_val = TRUE;
    }
  }

  if (ret_val)
  {
    MSG_MED("Created PPM resp",0,0,0);
  }
  else
  {
    MSG_ERROR("Can't create PPM resp",0,0,0);
  }
  return ret_val;
}

/*===========================================================================

FUNCTION create_tom_resp_pddm

DESCRIPTION: This function creates an IS801-1 or Is801-B Time Offset Measurement 
             response depending on its input.

             In case the request is for an Is801B response creation, and IS801B is
             disabled via NV, or featurized off, no response is created.

RETURN VALUE
  TRUE     : No Errors in response creation.
  FALSE    : Response creation failed.

DEPENDENCIES

===========================================================================*/
static boolean create_tom_resp_pddm
(
  is801_rev_link_resp_s_type *resp_801_1, 
  u_new_rev_link_resp_params *resp_801_b,
  srch_ppm_rpt_type          *ppm_ptr,
  is801_operation             op
)
{
  boolean ret_val = FALSE;

  if(op == RL_RESP_TOM)
  {
    /* Send Unsol TOM message to PDE.
    */
    resp_801_1->response_type = IS801_REV_LINK_RESP_MASK_TOM;
    resp_801_1->unsolicited   = 1;
    (void)is801_util_rlink_convert_msto(
                                ppm_ptr, 
                                &(resp_801_1->resp_msg_payload.tom_resp));

    ret_val = is801_new_rev_link_resp( TM_PRTL_TYPE_1X_UP, resp_801_1);
  }
  else /* if(op == RL_RESP_B_TOM) */
  {
    #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
    #error code not present
#endif
    {
      MSG_MED("Ignoring disabled IS801B req",0,0,0);
      ret_val = TRUE;
    }
  }

  if (ret_val)
  {
    MSG_MED("Created TOM resp",0,0,0);
  }
  else
  {
    MSG_ERROR("Failed to Create TOM response",0,0,0);
  } 
  return ret_val;
}

/*===========================================================================

FUNCTION create_rl_req_asst_data

DESCRIPTION: This function creates an IS801-1 or Is801-B request for assistance
             data, based on the request mask supplied as input.

             In case the request is for an Is801B request creation, and IS801B is
             disabled via NV, or featurized off, no request is created.

RETURN VALUE
  TRUE     : No Errors in request creation.
  FALSE    : Request creation failed.

DEPENDENCIES

===========================================================================*/
static boolean create_rl_req_asst_data
(
  tm_1x_up_rlink_req_type req_mask, 
  boolean                 is_801_1   
)
{
  /* If req is for 801-B and support for 801-b is turned off, ignore req */
  if(is_801_1 == FALSE && tm_1x_up_config.support_is801_b == FALSE)
  {
    MSG_MED("Ignoring disabled IS801B rlink req creation",0,0,0);
    return TRUE;
  }
  /* Now check what needs to be requested from PDE for Dload operation.
  */
  if ( req_mask & TM_1X_UP_RLINK_REQ_GPS_ALM )
  {
    if( tm_1x_up_is801_drv_create_rlink_req(
              TM_1X_UP_RLINK_REQ_GPS_ALM, is_801_1) == FALSE)
    {
      MSG_ERROR("Can not create ALM req",0,0,0);
      return FALSE;
    }
  }
  if ( req_mask & TM_1X_UP_RLINK_REQ_GPS_EPH )
  {
    if(tm_1x_up_is801_drv_create_rlink_req(
              TM_1X_UP_RLINK_REQ_GPS_EPH, is_801_1) == FALSE)
    {
      MSG_ERROR("Can not create EPH req",0,0,0);
      return FALSE;
    }
  }
  if ( req_mask & TM_1X_UP_RLINK_REQ_LR )
  {
    if(tm_1x_up_is801_drv_create_rlink_req(
              TM_1X_UP_RLINK_REQ_LR, is_801_1) == FALSE)
    {
      MSG_ERROR("Can not create LR req",0,0,0);
      return FALSE;
    }
  }
  if ( req_mask & TM_1X_UP_RLINK_REQ_AA )
  {
    if(tm_1x_up_is801_drv_create_rlink_req(
              TM_1X_UP_RLINK_REQ_AA, is_801_1) == FALSE)
    {
      MSG_ERROR("Can not create AA req",0,0,0);
      return FALSE;
    }
  }

  /* We must support both IS801B and Glonass for us to req Glonass
   * ephemeris and almanac and the req must be is_801_B */
#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
#error code not present
#endif
 
  return TRUE;
}

/*===========================================================================

FUNCTION mgp_start_prm

DESCRIPTION: Send a START PRM request to MGP.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
static boolean mgp_start_prm
(
  tm_sess_req_param_u_type *sess_req_param, 
  prtl_start_sess_req_param_s_type *pdapi_info_ptr
)
{
  sess_req_param->continue_param.op_req  = TM_OP_REQ_PRM;
  sess_req_param->continue_param.op_mode = TM_SESS_OPERATION_MODE_MSA;
  sess_req_param->continue_param.prm_prq = pdapi_info_ptr->pd_qos.gps_session_timeout;
  sess_req_param->continue_param.prm_qos = (pdapi_info_ptr->pd_qos.gps_session_timeout > TM_1X_UP_PR_TIMEOUT_MAX_MSA) ? 
                                               TM_1X_UP_PR_TIMEOUT_MAX_MSA : pdapi_info_ptr->pd_qos.gps_session_timeout;

  sess_req_param->continue_param.num_fixes        = pdapi_info_ptr->pd_option.fix_rate.num_fixes;
  sess_req_param->continue_param.tbf_ms           = pdapi_info_ptr->pd_option.fix_rate.time_between_fixes_ms;
  sess_req_param->continue_param.multi_report_msa = FALSE;

  if(tm_sess_req(TM_PRTL_TYPE_1X_UP,
             tm_1x_up_session_info.sess_handle,
             TM_SESS_REQ_CONTINUE,
             sess_req_param))
  {
    MSG_MED("mgp_start_prm Succeeded", 0, 0, 0);  
    tm_1x_up_session_info.mgp_info.prm_on = TRUE;
    return TRUE;
  }
  else
  {
    MSG_ERROR("mgp_start_prm Failed", 0, 0, 0);
    return FALSE;
  }                  
}


/*===========================================================================

FUNCTION mgp_post_prefix_ppm_buf

DESCRIPTION: Post previously stored Prefix PPM buffer to MGP. This is done so
             MGP can use this Prefix PPM to apply adjustments to the AA data 
             thats received from the PDE. The AA data from the PDE is sent
             based on the Prefix PPM.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE     : No errors in posting PPM data to MGP
  FALSE    : An error occurred in posting PPM data to MGP.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean mgp_post_prefix_ppm_buf
( 
  tm_post_data_payload_type  *post_data_ptr
)
{
  /* Send prefix-PPM along with AA data */
  if(up_prefix_ppm_buff_ptr)
  {
    memscpy((void*)&(post_data_ptr->ppm_data), sizeof(post_data_ptr->ppm_data),
            (const void *)up_prefix_ppm_buff_ptr, sizeof(*up_prefix_ppm_buff_ptr)); /* sizeof gps_PreFixPpmSructType */

    MSG_MED("Send pre-fix PPM data to TM-Core", 0, 0, 0);
    (void)pdsm_freebuf((char *)up_prefix_ppm_buff_ptr);
    up_prefix_ppm_buff_ptr = NULL;

    if(tm_post_data(TM_PRTL_TYPE_1X_UP,
                      tm_1x_up_session_info.sess_handle,
                      TM_POST_DATA_TYPE_PREFIX_PPM,
                      post_data_ptr) == FALSE)
    {
      MSG_ERROR("Post data for prefix_ppm failed",0,0,0);
      return FALSE;
    }
  }
  else
  {
    MSG_ERROR("prefix_ppm buffer empty",0,0,0);
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================

FUNCTION save_ppm_to_buf

DESCRIPTION: Save Prefix PPM buffer in a local buffer. This PPM data will be
             posted to MGP when the AA data is received from the PDE. This is 
             done so MGP can use this Prefix PPM to apply adjustments to the AA 
             data thats received from the PDE. The AA data from the PDE is sent
             based on the Prefix PPM.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE     : No errors in saving PPM data locally
  FALSE    : An error occurred in saving PPM data locally

SIDE EFFECTS
  None.

===========================================================================*/
static boolean save_ppm_to_buf( srch_ppm_rpt_type *ppm_ptr)
{
  MSG_HIGH("Save pre-fix PPM data till AA data is available",0,0,0);
  if ( up_prefix_ppm_buff_ptr == NULL )
  {
    up_prefix_ppm_buff_ptr = (gps_PreFixPpmSructType*) pdsm_getbuf(sizeof(gps_PreFixPpmSructType));
  }

  if(up_prefix_ppm_buff_ptr)
  {
    up_prefix_ppm_buff_ptr->l_RfCalibGCx1024 = is801_get_rf_delay(ppm_ptr->band_class);
    memscpy((void *)&(up_prefix_ppm_buff_ptr->z_PpmRpt), sizeof(up_prefix_ppm_buff_ptr->z_PpmRpt),
            (const void *)ppm_ptr, sizeof(*ppm_ptr)); /* sizeof(srch_ppm_rpt_type) */
    return TRUE;
  }
  else
  {
    MSG_ERROR("No buf for prefix PPM", 0, 0, 0);
    return FALSE;
  } /* End else if no buffer is available */
}

#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
#error code not present
#endif

/*===========================================================================

FUNCTION mgp_post_aa

DESCRIPTION: This function posts Is801-1 or IS801-B AA data to MGP, depending
             on the version of the AA data passed to it.

             IS801B AA data is posted only if Is801B is feature enabled.
             Glonass AA data is posted only if Glonass is feature enabled.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE     : No errors in creating posting AA data.
  FALSE    : An error occurred in creating AA data.

SIDE EFFECTS
  Frees the Prefix PPM buffer

===========================================================================*/
static boolean mgp_post_aa
( 
  tm_post_data_payload_type   *post_data_ptr,
  is801_unified_aa_resp_type  *aa
)
{
  is801_aa_resp_type         *aa_801_1;
  #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#endif
  gnss_SvAcqAssistCommonStructType  *aa_gps_data = &post_data_ptr->gnss_aa_data;

  if(aa->prot == IS801_VERSION_801_1)
  {
    aa_801_1 = aa->u.aa_801_1;
    (void)is801_util_flink_convert_aa(aa_801_1, aa_gps_data, NULL );

    MSG_MED("Send AA data to TM-Core", 0, 0, 0);
    TM_POST_DATA(TM_POST_DATA_TYPE_AA);
   
    (void)is801_util_flink_convert_aa(aa_801_1, NULL, &(post_data_ptr->gnss_sv_dir) );
    /* post sv dir */
    MSG_MED("Send SV DIR to TM-Core", 0, 0, 0);
    TM_POST_DATA(TM_POST_DATA_TYPE_SV_DIR);  
  }
  #ifdef  FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE 
  #error code not present
#endif
  else
  {
    MSG_ERROR("Unsupported protocol passed %d", aa->prot, 0, 0);
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

FUNCTION mgp_post_gps_alm

DESCRIPTION: This function posts Is801-1 or IS801-B GPS Alm data to MGP, 
             depending on the version of the Alm data passed to it.

             IS801B Alm data is posted only if Is801B is feature enabled 
             via NV and featurization.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE     : No errors in posting GPS Alm data.
  FALSE    : An error occurred in posting GPS Alm data.

SIDE EFFECTS

===========================================================================*/
static boolean mgp_post_gps_alm
( 
  tm_post_data_payload_type        *post_data_ptr,
  is801_unified_gps_alm_resp_type  *alm_resp_ptr
)
{
  uint8                    i;
  is801_alm_resp_type     *alm_801_1;
  #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#endif

  if(alm_resp_ptr->prot == IS801_VERSION_801_1)
  {
    alm_801_1 = alm_resp_ptr->u.alm_801_1;
    for( i= 0; 
         i < alm_801_1->fix.u_TotalSats && i < MAX_NUM_ALM_SATS; 
         i++ )
    {            
      tm_1x_up_session_info.aiding_data_status.gps_alm_delivered_sv_list |= 
                    1L << (alm_801_1->var[i].u_Sv_Prn_Num -1); 

      (void)is801_util_flink_convert_alm(alm_801_1, &post_data_ptr->alm_data,i);
  
      MSG_MED("Send ALM data to TM-Core", 0, 0, 0);
      TM_POST_DATA(TM_POST_DATA_TYPE_ALM);
    }
  }
  #ifdef  FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE 
  #error code not present
#endif
  else
  {
    MSG_ERROR("Unsupported protocol passed %d", alm_resp_ptr->prot, 0, 0);
    return FALSE;
  }

  return TRUE;
}

#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
#error code not present
#endif  /* FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE  */

/*===========================================================================

FUNCTION mgp_post_gps_eph_iono

DESCRIPTION: Function to post Is801-1 or IS801-B GPS Eph and Iono data to MGP 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE     : No errors in posting GPS Eph+Iono data.
  FALSE    : An error occurred in posting GPS Eph+Iono Eph data.

SIDE EFFECTS

===========================================================================*/
static boolean mgp_post_gps_eph_iono
( 
  tm_post_data_payload_type       *post_data_ptr,
  is801_unified_gps_eph_resp_type *eph
)
{
  uint8 i;
  is801_eph_resp_type     *eph_801_1;
  #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#endif

  MSG_HIGH("Update TM-Core with EPH resp",0,0,0);
  if(eph->prot == IS801_VERSION_801_1)
  {
    eph_801_1 = eph->u.eph_801_1;
    if(eph_801_1->fix.u_AB_Par_Incl)
    {
      (void)is801_util_flink_convert_iono(eph_801_1, 
                                        &(post_data_ptr->iono_data));

      MSG_MED("Send IONO data to TM-Core", 0, 0, 0);
      TM_POST_DATA(TM_POST_DATA_TYPE_IONO);
    }
    /* update the LM with Eph data.
    */
    for( i= 0; 
         i < eph_801_1->fix.u_TotalSats && i < MAX_NUM_EPH_SATS; 
         i++ )
    {            
      tm_1x_up_session_info.aiding_data_status.gps_eph_delivered_sv_list |= 1L << (eph_801_1->var[i].u_Sv_Prn_Num - 1); 
      (void)is801_util_flink_convert_eph(eph_801_1, &(post_data_ptr->eph_data), i);

      MSG_MED("Send EPH data to TM-Core", 0, 0, 0);
      TM_POST_DATA(TM_POST_DATA_TYPE_EPH);
    }
  }
  #ifdef  FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE 
  #error code not present
#endif  /* FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE  */
  else
  {
    MSG_ERROR("Unsupported protocol passed %d", eph->prot, 0, 0);
    return FALSE;
  }
  return TRUE;
}

static boolean save_seed
( 
  is801_unified_loc_resp_type *lr_resp_ptr
)
{
  pdsm_pd_session_operation_e_type op_mode = 
          tm_1x_up_session_info.api_param.pd_api_info.pd_option.operation_mode;

  is801_lr_resp_type *lr_801_1_ptr;
  #ifdef  FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE 
  #error code not present
#endif

  if(op_mode != PDSM_SESSION_OPERATION_REF_POSITION)
  {
    return TRUE;
  }
  /* op_mode is PDSM_SESSION_OPERATION_REF_POSITION */

  /* allocate a buffer to save the location in */
  if ( ref_position_ptr == NULL )
  {
    ref_position_ptr = 
      (is801_lr_resp_type*) pdsm_getbuf(sizeof(is801_lr_resp_type));
  }
  
  if( ref_position_ptr == NULL )
  {
    MSG_ERROR("No buffer for Ref Position", 0, 0, 0);
    return FALSE;
  } /* End if no buffer is available */

  if(lr_resp_ptr->prot == IS801_VERSION_801_1)
  {
    lr_801_1_ptr = lr_resp_ptr->u.lr_801_1;
  }
  #ifdef  FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE 
  #error code not present
#endif
  else
  {
    MSG_ERROR("Unsupported protocol passed %d", lr_resp_ptr->prot, 0, 0);
    return FALSE;
  }
  
  memset((void*) ref_position_ptr, 0, sizeof(*ref_position_ptr));
  *ref_position_ptr = *lr_801_1_ptr;

  return TRUE;
}
/*===========================================================================

FUNCTION mgp_post_seed

DESCRIPTION: Function to post seed location sent by the PDE (Is801-1/Is801-B)
             to MGP 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE     : No errors in posting seed data.
  FALSE    : An error occurred in posting seed data.

SIDE EFFECTS

===========================================================================*/
static boolean mgp_post_seed
( 
  tm_post_data_payload_type   *post_data_ptr,
  is801_unified_loc_resp_type *lr_resp_ptr
)
{
  /* Convert from IS801 to LM format */
  if(lr_resp_ptr->prot == IS801_VERSION_801_1)
  {
    (void)is801_util_flink_convert_seed(
                    lr_resp_ptr->u.lr_801_1,
                    &(post_data_ptr->seed_data));
  }
  #ifdef  FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE 
  #error code not present
#endif
  else
  {
    MSG_ERROR("Unsupported protocol passed %d", lr_resp_ptr->prot, 0, 0);
    return FALSE;
  }
  

  MSG_MED("Send LR data to TM-Core", 0, 0, 0);
  if(tm_post_data(TM_PRTL_TYPE_1X_UP,
                  tm_1x_up_session_info.sess_handle,
                  TM_POST_DATA_TYPE_SEED,
                  post_data_ptr) == FALSE)
  {
    MSG_ERROR("Post data for SEED failed",0,0,0);
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================

FUNCTION mgp_post_sa

DESCRIPTION: Function to post Is801-1 / Is801-B GPS Sensitivity Assistance data 
             to MGP 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE     : No errors in posting GPS SA data.
  FALSE    : An error occurred in posting GPS SA data.

SIDE EFFECTS

===========================================================================*/
static boolean mgp_post_sa
( 
  tm_post_data_payload_type      *post_data_ptr,
  is801_unified_gps_sa_resp_type *sa_resp
)
{
  is801_sa_resp_type      *sa_801_1 = NULL;
  #ifdef  FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE 
  #error code not present
#endif
  
  if (tm_1x_up_session_info.mgp_info.prm_on != TRUE)
  {
    MSG_MED("Ignore SA data as PRMs are done",0,0,0);
    return TRUE;
  } /* End else PR_ON */

  /* PRMs are on, send SA data */
  if(sa_resp->prot == IS801_VERSION_801_1)
  {
    sa_801_1 = sa_resp->u.sa_801_1;
    MSG_HIGH("Give SA data to TM-Core",0,0,0);

    (void)is801_util_flink_convert_sa( sa_801_1, &(post_data_ptr->sa_data));
    
    TM_POST_DATA(TM_POST_DATA_TYPE_SA);
  }
  #ifdef  FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE 
  #error code not present
#endif
  else
  {
    MSG_ERROR("Unknown protocol choice for SA data %d", sa_resp->prot, 0, 0);
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================

FUNCTION mgp_get_ref_loc

DESCRIPTION : Send message to MGP to get a reference location. This will be
              sent to the PDE so the PDE will know what steering/ephemeris
              etc to send

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
static boolean mgp_get_ref_loc(tm_sess_req_param_u_type *sess_req_param)
{
   sess_req_param->continue_param.op_req = TM_OP_REQ_REF_LOC;
   sess_req_param->continue_param.dynamic_qos_enabled = TRUE;
                          
   if(tm_sess_req(TM_PRTL_TYPE_1X_UP,
              tm_1x_up_session_info.sess_handle,
              TM_SESS_REQ_CONTINUE,
              sess_req_param))
   {
     MSG_MED("tm_1x_up_is801_drv_get_ref_loc Succeeded", 0, 0, 0);  
     if(!os_TimerStart(tm_1x_up_session_info.tm_1x_up_mgp_timer,TM_1X_UP_MGP_RESP_TIMEOUT,NULL))
     {
       MSG_ERROR("Failed starting timer for getting ref_loc",0,0,0);
     }
     return TRUE;
   }
   else
   {
     MSG_ERROR("tm_1x_up_is801_drv_get_ref_loc Failed", 0, 0, 0);
     return FALSE;
   }                  
}


/*===========================================================================

FUNCTION save_prm_to_buf

DESCRIPTION : Save PRM buffer locally. This locally saved PRM buffer will be
              sent to the PDE, after the post fix PPMs are received from MGP.
              Till that time, the PRMs are saved in a local buffer.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
static boolean save_prm_to_buf(sm_GpsMeasRptStructType *prm_meas)
{
  sm_GpsMeasRptStructType *up_prm_buff_ptr  = NULL;
  
  MSG_HIGH("Save PRM data till Post-fix PPM is available",0,0,0);

  /* First perform all error checks */
  if( (prm_meas->z_MeasBlk.e_MeasBlkSrc != GNSS_MEAS_BLK_SRC_GPS) &&
      (prm_meas->z_MeasBlk.e_MeasBlkSrc != GNSS_MEAS_BLK_SRC_GLO) )
  {
    ERR_FATAL("Unknown constellation passed in prm %d", 
                 (uint8) prm_meas->z_MeasBlk.e_MeasBlkSrc, 0, 0);
  }

  /* Check that the prior buffer has been freed */
  if( (prm_meas->z_MeasBlk.e_MeasBlkSrc == GNSS_MEAS_BLK_SRC_GPS) &&
      (up_prm_buff_ptr_gps != NULL ) )
  {
    MSG_ERROR("Memory leak in storing gps prm buffers", 0, 0, 0);
  }

  if( (prm_meas->z_MeasBlk.e_MeasBlkSrc == GNSS_MEAS_BLK_SRC_GLO) &&
      (up_prm_buff_ptr_glo != NULL ) )
  {
    MSG_ERROR("Memory leak in storing glo prm buffers", 0, 0, 0);
  }

  up_prm_buff_ptr = (sm_GpsMeasRptStructType*) pdsm_getbuf(
                            sizeof(*up_prm_buff_ptr));

  if(up_prm_buff_ptr)
  {
     if(prm_meas->z_MeasBlk.u_NumSvs == 0)
     {
       /* create bogus PRMs */
       memset( up_prm_buff_ptr, 0, sizeof(*up_prm_buff_ptr)); /* sizeof sm_GpsMeasRptStructType */
       up_prm_buff_ptr->z_MeasBlk.u_NumSvs = 0;
       up_prm_buff_ptr->z_MeasBlk.z_MeasClock.z_Time.z_GpsTime.w_GpsWeek 
                                           = C_GPS_WEEK_UNKNOWN;
     }
     else
     {
       memscpy((void *)up_prm_buff_ptr, sizeof(*up_prm_buff_ptr),
               (const void *) prm_meas, sizeof(*prm_meas)); /* sizeof sm_GpsMeasRptStructType */
     }

     if(prm_meas->z_MeasBlk.e_MeasBlkSrc == GNSS_MEAS_BLK_SRC_GPS)
     {
       up_prm_buff_ptr_gps = up_prm_buff_ptr;
     }
     if(prm_meas->z_MeasBlk.e_MeasBlkSrc == GNSS_MEAS_BLK_SRC_GLO)
     {
       up_prm_buff_ptr_glo = up_prm_buff_ptr;
     }

     return TRUE;
  }
  else
  {
    MSG_ERROR("No buffer for PRM", 0, 0, 0);
    return FALSE;
  } /* End else if no buffer is available */
}

#if 0
TBD enable this to do dynamic allocation of the ref loc, but remember to free
when we know that this buffer is not needed any more.
a) when we send the ref loc in 801b call flow
b) we get to know that the server is not 801b, its 8011
sm_GpsFixRptStructType *up_ref_loc_buf_ptr = NULL;
static boolean save_ref_loc_to_buf(sm_GpsFixRptStructType *lr_ptr)
{
  if(up_ref_loc_buf_ptr != NULL)
  {
    MSG_ERROR("Memory leak in storing ref locn for 801b", 0, 0, 0);
  }

  /* sizeof sm_GpsFixRptStructType */
  up_ref_loc_buf_ptr = (sm_GpsFixRptStructType *) pdsm_getbuf(
                            sizeof(*up_ref_loc_buf_ptr));

  if(up_ref_loc_buf_ptr == NULL)
  {
    ERR_FATAL("Could not allocate memory for ref locn", 0, 0, 0);
    return FALSE;
  }
  *up_ref_loc_buf_ptr = *lr_ptr;
  return TRUE;
}

static boolean rl_resp_801_b_ref_loc_frm_buf(u_new_rev_link_resp_params  *resp_801_b)
{
  boolean ret_val = FALSE;
  
  if(tm_1x_up_config.support_is801_b != TRUE)
  {
    MSG_MED("Ignoring disabled IS801B req",0,0,0);
    return TRUE;
  }

  if(up_ref_loc_buf_ptr == NULL)
  {
    ERR_FATAL("No buffered ref locn for 801b", 0, 0, 0);
  }

  resp_801b->ref_loc = up_ref_loc_buf_ptr;
  ret_val =  is801b_new_rev_link_resp( 
                    TM_PRTL_TYPE_1X_UP,
                    IS801B_REV_LINK_RESP_MASK_REF_LOC,
                    TRUE,
                    tm_1x_up_config.assisted_session, 
                    resp_801_b);

  (void) pdsm_freebuf(up_ref_loc_buf_ptr);
  up_ref_loc_buf_ptr = NULL;
  return ret_val;
}


#endif

sm_GnssFixRptStructType up_ref_loc;
/*===========================================================================

FUNCTION save_ref_loc_to_buf

DESCRIPTION: Function to save refernce location proovided by LM into a local 
             buffer. This will be used if both the PDE/MS support IS801B. Once
             the version negotiation is complete, the locally stored reference
             location will be sent to the PDE.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
static boolean save_ref_loc_to_buf(sm_GnssFixRptStructType *lr_ptr)
{
  if(lr_ptr == NULL)
  {
    MSG_ERROR("NULL ptr passed for ref loc", 0, 0, 0);
    return FALSE;
  }
  up_ref_loc = *lr_ptr;
  return TRUE;
}

#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
#error code not present
#endif

/*===========================================================================

FUNCTION rl_resp_ref_loc

DESCRIPTION: Function to create one IS801-1 reverse link reference location 
             response. This is sent to the PDE so the PDE can respond with 
             Eph/Alm depending on the visibility from the ref location.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
static boolean rl_resp_ref_loc
(
  is801_rev_link_resp_s_type *resp_ptr, 
  sm_GnssFixRptStructType *lr_ptr
)
{
  resp_ptr->unsolicited = 1;
  resp_ptr->response_type = IS801_REV_LINK_RESP_MASK_LR;
  if(is801_util_convert_tm_lr_to_is801_lr(lr_ptr,&(resp_ptr->resp_msg_payload.lr_resp)))
  {
    if ( is801_new_rev_link_resp( TM_PRTL_TYPE_1X_UP,resp_ptr))
    {
      MSG_MED("Created LR resp",0,0,0);
      return TRUE;
    }
    else
    {
      MSG_ERROR("Can't create RefLR resp",0,0,0);
      return FALSE;
    }
  }
  return FALSE;
}

#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
#error code not present
#endif

/*===========================================================================

FUNCTION rl_resp_prm_frm_buf

DESCRIPTION: Function to create a reverse link PRM response from PRMs earlier
             stored locally. The PRMs are stored locally to wait for the PPMs,
             once PPMs are received, both PRMs and PPMs are sent together to 
             the PDE so the PDE can compute a hybrid fix. 

             The function creates PRMs for both IS801-1 and Is801-B depending
             on input params.

             Is801B PRMs are created only if Is801B is supported via NV and
             via featurization. 

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
static boolean rl_resp_prm_frm_buf
( 
  is801_rev_link_resp_s_type  *resp_801_1, 
  u_new_rev_link_resp_params  *resp_801_b,
  srch_ppm_rpt_type           *ppm_ptr,
  is801_operation              op
)
{
  boolean ret_val = FALSE;

  if(up_prm_buff_ptr_gps == NULL && up_prm_buff_ptr_glo == NULL)
  {
    MSG_ERROR("No PRM data in the buffer",0,0,0);
    return FALSE;
  }
          
  if(op == RL_RESP_PRM_FRM_BUF)
  {
    /* Create 801-1 Resp */
    MSG_MED("Creating 801-1 PRM response",0,0,0);
    resp_801_1->response_type = IS801_REV_LINK_RESP_MASK_PRM;
    resp_801_1->unsolicited = 0x1;     /* mark as unsolicited response */
 
    (void)is801_util_rlink_convert_prm(
                    ppm_ptr,
                    up_prm_buff_ptr_gps, 
                    &(resp_801_1->resp_msg_payload.prm_resp));

    ret_val = is801_new_rev_link_resp( TM_PRTL_TYPE_1X_UP, resp_801_1);
  }
  else /* if(op == RL_RESP_PRM_B_FRM_BUF) */
  {
    #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
    #error code not present
#else  /* FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE  */
    ret_val = TRUE;
    #endif
  }

  if(up_prm_buff_ptr_gps)
  {
    (void)pdsm_freebuf((char *)up_prm_buff_ptr_gps);
    up_prm_buff_ptr_gps = NULL;
  }
  
#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#endif

  if (ret_val)
  {
    MSG_MED("Created PRM response",0,0,0);
    return TRUE;
  }
  else
  {
    MSG_ERROR("Failed to Create PRM response",0,0,0);
    return FALSE;
  } 
}

/*===========================================================================

FUNCTION clear_req_bmap

DESCRIPTION: Function to clear req bmap from tm_1x_up_session_info as response
             to requests are received from the PDE. The request mask consists
             of generic requests (i.e. common to both Is801-1 and Is801-B) sent
             by 1x UP.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
Stops IS801 Response Timer if all responses are received.
===========================================================================*/
static boolean clear_req_bmap(tm_1x_up_rlink_req_type *req)
{
  if ( (tm_1x_up_session_info.is801_req_wait_bmap & *req) )
       // && (resp_ptr->unsolicited == 0) )
  {
    tm_1x_up_session_info.is801_req_wait_bmap &= ~(*req);
    if ( tm_1x_up_session_info.is801_req_wait_bmap == NULL )
    {
      /* No further IS801 requests's are pending, so clear the IS801 timer.
      */
      (void)os_TimerStop(tm_1x_up_session_info.tm_1x_up_is801_timer);  
    }
  }
  /*
  else if (resp_ptr->unsolicited == 1)
  {
    MSG_HIGH("Rxed Unsol resp. mask %x", req,0,0);
  }
  */
  return TRUE;
}

/*===========================================================================

FUNCTION create_rlink_pddm

DESCRIPTION: Function to create a reverse link PDDMs for both Is801-1 and 
             Is801-B. IS801B PDDMs are only created if IS801B is enabled
             via NV and featurization.

             PDDMs are created by invoking the IS801-1/-B encoding layers
             PDDM creation function.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE     : PDDM created 
  FALSE    : Error

SIDE EFFECTS
  Increments the number of PDDMs stored in pdcomm_info by the number of PDDMs
  created for Is801-1 and Is801-B respectively.
===========================================================================*/
static boolean create_rlink_pddm(byte* sess_flags)
{
  byte  sess_start   = 0;
  byte  sess_end     = 0;
  int   n_rlink_pddm = 0;

  if(sess_flags == NULL)
  {
    MSG_ERROR("Null pointer passed for sess_flags", 0, 0, 0);
    return FALSE;
  }
  

  sess_start = ((*sess_flags) & TM_1X_UP_SESS_START_MASK) ? 1: 0;
  sess_end   = ((*sess_flags) & TM_1X_UP_SESS_END_MASK)   ? 1: 0;

  if(*sess_flags & TM_1X_UP_IS801_1_PDDM)
  {
    n_rlink_pddm = tm_1x_up_is801_drv_create_rlink_pddm(sess_start, sess_end);
    if(n_rlink_pddm < 0)
    {
      MSG_ERROR("IS801 returned PDDMs = %d", n_rlink_pddm,0,0);
      n_rlink_pddm = 0;
      return FALSE;
    }
    pdcomm_info.num_rev_pddm_801_1 +=  n_rlink_pddm;
  }

  #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#endif

  return TRUE;
}

/*===========================================================================

FUNCTION tx_rlink_pddm

DESCRIPTION: Function to transmit all created reverse link PDDMs. If requested
             both Is801-1 and Is801-B PDDMs shall be transmitted. For e.g. in
             the first exchange of PDDMs at the start of a session.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE     : PDDM transmitted 
  FALSE    : Error

SIDE EFFECTS
May start IS801 Response Timer if 1xUP expects a response back from the PDE.
===========================================================================*/
static boolean tx_rlink_pddm(byte* sess_flags)
{
  boolean timer      = ((*sess_flags) & TM_1X_UP_IS801_TIMER_MASK) ? TRUE: FALSE;
  boolean tx_is801_b = ((*sess_flags) & TM_1X_UP_IS801_B_PDDM)     ? TRUE: FALSE;
  boolean tx_is801_1 = ((*sess_flags) & TM_1X_UP_IS801_1_PDDM)     ? TRUE: FALSE;

  if( (*sess_flags & (TM_1X_UP_IS801_B_PDDM|TM_1X_UP_IS801_1_PDDM)) == 0)
  {
    MSG_ERROR("ERR in sess_flags mask 0x%x Nothing to tx", *sess_flags, 0, 0);
    return FALSE;
  }

  if(!tm_1x_up_is801_drv_send_rlink_pddm( sess_flags, tx_is801_b, tx_is801_1))
  {
    return FALSE;
  }

  if(timer)
  {
    if(!os_TimerStart(tm_1x_up_session_info.tm_1x_up_is801_timer,
                      TM_IS801_TIMEOUT,NULL))
    {
      MSG_ERROR("Failed starting IS801 timer",0,0,0);
    }
  }

  return TRUE;
}

typedef union
{
  is801_rev_link_resp_s_type  rl_resp;
  tm_post_data_payload_type   post_data;
  tm_sess_req_param_u_type    sess_req_param;
  u_new_rev_link_resp_params  rl_resp_801b;
} post_data;

/*===========================================================================

FUNCTION tm_state_machine_execution_clean_up

DESCRIPTION
  This function cleans up any resources allocated by the state machine

RETURN VALUE

DEPENDENCIES

===========================================================================*/
void tm_state_machine_execution_clean_up(void)
{
  if(up_prefix_ppm_buff_ptr != NULL)
  {
    (void)pdsm_freebuf((char *)up_prefix_ppm_buff_ptr);
    up_prefix_ppm_buff_ptr = NULL;
  }
  if(up_prm_buff_ptr_gps != NULL)
  {
    (void)pdsm_freebuf((char *)up_prm_buff_ptr_gps);
    up_prm_buff_ptr_gps = NULL;
  }
#ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
  #error code not present
#endif
}

/*===========================================================================

FUNCTION tm_1x_up_execute_fns

DESCRIPTION
  This function takes two arrays, one of operations and the second of void* 
  to parameters for those operations, and executes these operations one by 
  one.

  If all operations are successfully executed, the function returns TRUE
  else FALSE

RETURN VALUE

DEPENDENCIES

===========================================================================*/
boolean tm_1x_up_execute_fns(const is801_operation ops[], void *params[])
{
  int         idx = 0;
  boolean     ret_val = TRUE;
  post_data   u;

  is801_rev_link_resp_s_type *resp_801_1     = &u.rl_resp;
  tm_post_data_payload_type  *post_data_ptr  = &u.post_data;
  tm_sess_req_param_u_type   *sess_req_ptr   = &u.sess_req_param;
  u_new_rev_link_resp_params *resp_801_b     = &u.rl_resp_801b;

  /*Check for ops and params being NULL*/
  if(ops == NULL || params == NULL)
  {
    MSG_ERROR("Received NULL ops[%x] or params[%x]",ops,params,0);
    return FALSE;
  }
  
  while(ops[idx] != OPS_END)
  {
    switch(ops[idx])
    {
      case(RL_RESP_B_MSI):
      case(RL_RESP_MSI):
        ret_val = create_msi_resp_pddm(resp_801_1, ops[idx]);
        break;

      case(RL_RESP_PRDM):
        ret_val = create_prdm_bsp_resp_pddm(resp_801_1);
        break;

      case(RL_RESP_B_SPI):
        #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
        #error code not present
#else
        ret_val = TRUE;
        #endif
        break;

      case(RL_RESP_PPM):
      case(RL_RESP_B_PPM):
        ret_val = create_ppm_resp_pddm(
                        resp_801_1, resp_801_b, 
                        *(srch_ppm_rpt_type**) params[idx], 
                        ops[idx]);
        break;

      case(RL_RESP_TOM):
      case(RL_RESP_B_TOM):
        ret_val = create_tom_resp_pddm(
                        resp_801_1, resp_801_b, 
                        *(srch_ppm_rpt_type**) params[idx], 
                        ops[idx]);
        break;

      case(RL_RESP_REF_LOC):
        ret_val = rl_resp_ref_loc(
                        resp_801_1, 
                        *(sm_GnssFixRptStructType**) params[idx]);
        break;

      case(RL_RESP_PRM_FRM_BUF):
      case(RL_RESP_B_PRM_FRM_BUF):
        ret_val = rl_resp_prm_frm_buf(
                        resp_801_1, resp_801_b, 
                        *(srch_ppm_rpt_type**) params[idx], 
                        ops[idx]);
        break;

      case(RL_RESP_B_PPM_FRM_BUF):
        #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
        #error code not present
#else
        ret_val = TRUE;
        #endif
        break;

      case(RL_RESP_B_REF_LOC_FRM_BUF):
        #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
        #error code not present
#else
        ret_val = TRUE;
        #endif
        break;

      case(RL_REQ_ASST_DATA_1):
        ret_val = create_rl_req_asst_data( *(uint32*) params[idx], TRUE);
        break;

      case(RL_REQ_ASST_DATA_B):
        #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
        #error code not present
#else
        ret_val = TRUE;
        #endif
        break;

      case(RL_REQ_B_BSC):
        #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
        #error code not present
#else
        ret_val = TRUE;
        #endif
        break;

      case(SAVE_PRM_TO_BUF):
        ret_val = save_prm_to_buf( *(sm_GpsMeasRptStructType **) params[idx]);
        break;

      case(SAVE_PPM_TO_BUF):
        ret_val = save_ppm_to_buf( *(srch_ppm_rpt_type **) params[idx]);
        break;

      case(SAVE_REF_LOC):
        ret_val = save_ref_loc_to_buf( *(sm_GnssFixRptStructType**) params[idx]);
        break;

      case(SAVE_SEED):
        ret_val = save_seed((is801_unified_loc_resp_type *) params[idx] );
        break;

      case(CR_PDDM):
        ret_val = create_rlink_pddm((byte *) params[idx]);
        break;

      case(TX_PDDM):
        ret_val = tx_rlink_pddm((byte *) params[idx]);
        break;

      case(CLR_REQ_MASK):
        ret_val = clear_req_bmap((tm_1x_up_rlink_req_type *) params[idx]);
        break;

      case(MGP_POST_AA_AND_SV_DIR):
        ret_val = mgp_post_aa(
                        post_data_ptr, 
                        (is801_unified_aa_resp_type *) params[idx] );
        break;

      case(MGP_POST_EPH_IONO):
        ret_val = mgp_post_gps_eph_iono(post_data_ptr, 
                        (is801_unified_gps_eph_resp_type *) params[idx] );
        break;
      
      case(MGP_POST_ALM):
        ret_val = mgp_post_gps_alm(post_data_ptr, 
                        (is801_unified_gps_alm_resp_type *) params[idx] );
        break;

      case(MGP_POST_PREFIX_PPM_BUF):
        ret_val = mgp_post_prefix_ppm_buf(post_data_ptr);
        break;

      case(MGP_POST_SA_PTR):
        ret_val = mgp_post_sa(post_data_ptr, 
                        (is801_unified_gps_sa_resp_type *) params[idx] );
        break;

      case(MGP_POST_SEED):
        ret_val = mgp_post_seed(post_data_ptr, 
                        (is801_unified_loc_resp_type *) params[idx] );
        break;

      #ifdef FEATURE_GNSS_SM_TM_1X_UP_GLONASS_ENABLE
      #error code not present
#endif

      case(MGP_POST_SVHI):
      #ifdef FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE
        #error code not present
#else
        MSG_ERROR("SVHI not expected in non IS801B callflow", 0, 0, 0);
        ret_val = TRUE;
        break;
      #endif

      case(MGP_GET_REF_LOC):
        ret_val = mgp_get_ref_loc(sess_req_ptr);
        break;

      case(MGP_REQ_PPM):
        ret_val = tm_1x_up_is801_drv_get_ppm();
        break;

      case(MGP_START_PRM):
        ret_val = mgp_start_prm(sess_req_ptr, 
                        *(prtl_start_sess_req_param_s_type **) params[idx]);
        break;

      case(MGP_REQ_LR):
        ret_val = tm_1x_up_is801_drv_continue_msb_fix( 
                        *(tm_1x_up_msb_op_e_type *) params[idx]);
        break;

      default:
        MSG_ERROR("Unknown Operation %d passed to execute fns params %x\n", ops[idx], params[idx], 0);
    }

    idx++;
    
    if(ret_val == FALSE)
    {
      return FALSE;
    }
  }
  return ret_val;
}


/*===========================================================================

FUNCTION tm_state_transition

DESCRIPTION
  This function is used to handle state transition. It takes two arrays, one
  of operations and the second of void* to parameters for those operations.

  If all operations are successfully executed, the state is changed to the
  next state.

RETURN VALUE
  TRUE : All operations successfully executed
  FALSE: 1 operation failed, state machine aborted

DEPENDENCIES

===========================================================================*/
boolean tm_state_transition
(
  const is801_operation ops[], 
  void *params[], 
  tm_1x_up_session_state_e_type next_state
)
{
  if( tm_1x_up_execute_fns(ops, params) )
  {
    /* Update the state */
    tm_1x_up_is801_drv_update_session_state(next_state);
    return TRUE;
  }
  else
  {
    tm_1x_up_is801_drv_end_session(TM_STOP_REASON_GENERAL_ERROR);
    return FALSE;
  }
}


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TM-UMTS-UP-SS sub-module 

GENERAL DESCRIPTION
  This file implements TM UMTS-CP SS sub-module.
   
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_cp/ss/src/tm_umts_cp_ss.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  06/09/15   mj      Add support for W+W feature
  08/18/09   gk      IF we receive MMREJ, send PDAPI event.
  07/23/09   gk      Add handling of MO_LR completed message
  08/28/08   LT      More notif. optional items in CP now passed on to PDAPI.
  01/11/07   LT      Removed LCS notif. to MM when MOLR-Inoved is qued at NAS.
  05/03/07   LT      Initial version

============================================================================*/

#include "gps_variation.h"
#include "customer.h"   /* Customer configuration file */

#ifdef FEATURE_CGPS_UMTS_CP


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "msg.h"
#include <limits.h>
#include "tm_cm_iface.h"
#include "tm_umts_common_utils.h"
#include "sm_log.h"
#include "sm_nv.h"
#include "tm_data.h"
#include "tm_umts_cp_gsm.h"
#include "tm_umts_cp_wcdma.h"

/*===========================================================================
                 Data Definition
===========================================================================*/


/* Log Message Defines */
enum 
{
  SS_TYPE_MOLR_INVOKE=0,
  SS_TYPE_MOLR_RETURN_RESULT,
  SS_TYPE_MOLR_RETURN_ERROR,
  SS_TYPE_LOC_NOTIFICATION_INVOKE,
  SS_TYPE_LOC_NOTIFICATION_RETURN_RESULT,
  SS_TYPE_LOC_NOTIFICATION_RETURN_ERROR
};

static uint8  u_next_ref_num; /* ref. num. for MoLrInvoke; keeps incrementing */

typedef struct
{
  boolean molr_on; /* turns TRUE after send_molr_invoke() called */
  uint8   ss_ref_num; /* the ref. num. of the molr_invoke sent */
  boolean third_party; /* if getting NAS confirmation of molr_invoke 
                          queued without molr_on TRUE, third party is TRUE */
  boolean molr_comp;  /* turns TURE after receiving MOLR complete */
  boolean molr_conf;  /* turns TRUE after receiving MOLR CONF */
} umts_cp_ss_session_status_s_type;

static umts_cp_ss_session_status_s_type   z_umts_cp_ss_session_status;



/* =======================================================================
**           Local Function Prototypes       
** ======================================================================= */

static void cp_ss_log_lcs_molr_invoke( cm_lcs_molr_params_s_type *ss_ptr );
static void cp_ss_log_lcs_molr_return_result( lcs_molr_res_data_T  *res_ptr );
static void cp_ss_log_lcs_molr_return_error ( ie_cm_ss_error_T *err_ptr );
static void cp_ss_log_lcs_location_notification_invoke ( location_notification_s_type *ss_ptr );
static void cp_ss_convert_coding_to_pdapi( byte coding_scheme, pdsm_lcs_notify_verify_datacoding_scheme_e_type *pdapi_scheme);
static void cp_ss_pass_notification_verifcation_to_pdapi(tm_cm_sups_data_s_type* p_sups_data );
static void cp_ss_log_lcs_location_notification_resp (cm_lcs_location_notification_res_params_s_type *ss_ptr );
#ifdef FEATURE_CGPS_DUAL_WCDMA
static void cp_ss_notify_session_status_to_mm(boolean session_started);
#endif


/* =======================================================================
**                            Function Definitions
** ======================================================================= */


/********************************************************************
*
* tm_umts_cp_ss_session_init
*
* Function description:
*   this function initializes CP-SS session related variables.. 
*
* Parameters:
*   None.
*
* Return:
*  None
*  
*********************************************************************
*/

void tm_umts_cp_ss_session_init(void)
{
  memset( (void *)&z_umts_cp_ss_session_status, 0, sizeof(umts_cp_ss_session_status_s_type) );
}


/*===========================================================================

FUNCTION 

DESCRIPTION
  This function is called by UMTS_CP to initialize SS. 
  
DEPENDENCIES: none.

RETURN VALUE: .

SIDE EFFECTS: none.

===========================================================================*/

void tm_umts_cp_ss_init (void)
{

  MSG_MED("tm_cp_ss init", 0, 0, 0);
  tm_umts_cp_ss_session_init();

} 

/*===========================================================================

FUNCTION 

DESCRIPTION
  This function sends MOLR-Invoke to CM. 
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/


boolean tm_umts_cp_ss_send_molr_invoke 
( prtl_start_sess_req_param_s_type *start_sess_req_param, 
  molr_type_e_type mo_lr_type,
  sm_InternalAssistDataStatusStructType   *p_assist_data_status 
)
{
  boolean ret_val;
  tm_cm_start_info_u_type z_cm_start_info;
  tm_cm_start_info_u_type *p_cm_start_info = &z_cm_start_info;

  GpsAssisData_Gsm0931  z_gps_assist_data_wishlist_gsm0931;

  /* init. the structure z_cm_start_info */
  memset( p_cm_start_info, 0, sizeof ( tm_cm_start_info_u_type ) );

  u_next_ref_num++;
  p_cm_start_info->lcs_molr_cmd_params.ss_ref = u_next_ref_num;  

  p_cm_start_info->lcs_molr_cmd_params.mlc_number.present = FALSE;
  p_cm_start_info->lcs_molr_cmd_params.external_id.present = FALSE; 
   
  p_cm_start_info->lcs_molr_cmd_params.molr_type = mo_lr_type;


  if ( mo_lr_type == LOCATION_ESTIMATE )
  {
    p_cm_start_info->lcs_molr_cmd_params.lcs_qos.present = TRUE;

    p_cm_start_info->lcs_molr_cmd_params.lcs_qos.horiz_accuracy.present = TRUE;
    p_cm_start_info->lcs_molr_cmd_params.lcs_qos.horiz_accuracy.data = 
      tm_umts_common_utils_meter_to_GADK( TRUE, start_sess_req_param->pd_qos.accuracy_threshold );

    p_cm_start_info->lcs_molr_cmd_params.lcs_qos.vertical_accuracy.present = FALSE;
    p_cm_start_info->lcs_molr_cmd_params.lcs_qos.vertical_coord_req.present = FALSE;

    p_cm_start_info->lcs_molr_cmd_params.lcs_qos.response_time.present = TRUE;
    p_cm_start_info->lcs_molr_cmd_params.lcs_qos.response_time.response_time_category = DELAY_TOLERANT;

    p_cm_start_info->lcs_molr_cmd_params.location_method.present = FALSE;
    p_cm_start_info->lcs_molr_cmd_params.supported_gad_shapes.present = FALSE;

  }

  else if ( mo_lr_type ==  ASSISTANCE_DATA) 
  {
    p_cm_start_info->lcs_molr_cmd_params.location_method.present = TRUE;
    p_cm_start_info->lcs_molr_cmd_params.location_method.data = ASSISTED_GPS;
    p_cm_start_info->lcs_molr_cmd_params.gps_assistance_data.present = TRUE;
    p_cm_start_info->lcs_molr_cmd_params.supported_gad_shapes.present = TRUE;
    p_cm_start_info->lcs_molr_cmd_params.supported_gad_shapes.data = 0xFF; /* declare supporting all shapes */


    tm_umts_common_utils_gsm0931_gps_assist_data_ie_build ( p_assist_data_status,
                                                            &z_gps_assist_data_wishlist_gsm0931 );

    p_cm_start_info->lcs_molr_cmd_params.gps_assistance_data.size = 
           memscpy(&p_cm_start_info->lcs_molr_cmd_params.gps_assistance_data.data[0],
                   sizeof (p_cm_start_info->lcs_molr_cmd_params.gps_assistance_data.data ),
                   &z_gps_assist_data_wishlist_gsm0931.u_Octet3_Whishlist_AH,
                   z_gps_assist_data_wishlist_gsm0931.u_Octet2_LengthIndicator);
  }

  else
  {
    /* should never get here */
    MSG_ERROR("UMTS-CP-SS: invalid mo_lr_type: %u", mo_lr_type, 0, 0 );
    return FALSE;
  }


  /* ask TmCore to send MOLR-Invoke to CM */
  ret_val = tm_cm_iface_start_req(TM_PRTL_TYPE_UMTS_CP, p_cm_start_info);

  if ( ret_val )
  {
    MSG_MED( "Sending MOLR-Inovke w/ ref_num = %u", 
             p_cm_start_info->lcs_molr_cmd_params.ss_ref, 0, 0 );

    cp_ss_log_lcs_molr_invoke( &p_cm_start_info->lcs_molr_cmd_params );

    z_umts_cp_ss_session_status.molr_on = TRUE;
    z_umts_cp_ss_session_status.molr_conf = FALSE;
    z_umts_cp_ss_session_status.molr_comp = FALSE;
    
    z_umts_cp_ss_session_status.ss_ref_num = p_cm_start_info->lcs_molr_cmd_params.ss_ref;

  }

  return ret_val;

}

/*===========================================================================

FUNCTION 

DESCRIPTION
  This function sends LocationNotificationReturnResult to CM via TmCore 
  
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/

boolean tm_umts_ss_send_lcs_loc_notif_return_result(const pdsm_lcs_notification_resp_s_type *p_user_resp )
{
  tm_cm_stop_info_s_type                           z_cm_stop_info;

  cm_lcs_location_notification_res_params_s_type   *p_location_notification_res = 
    &z_cm_stop_info.stop_params.lcs_loc_notif_return_result_params;

  boolean ret_val = FALSE;


  p_location_notification_res->invoke_id = p_user_resp->lcs_info.umts_cp_req.invoke_id;

  z_cm_stop_info.stop_reason = TM_CM_STOP_REASON_LOC_NOTIF_RET_RES;

  if ( p_user_resp->resp == PDSM_LCS_NOTIFY_VERIFY_DENY )
  {
    p_location_notification_res->location_notification_res.present = TRUE;
    p_location_notification_res->location_notification_res.location_notification_res = PERMISSION_DENIED; /* 0 */
  } 
  else if (p_user_resp->resp == PDSM_LCS_NOTIFY_VERIFY_ACCEPT)
  {
    p_location_notification_res->location_notification_res.present = TRUE;
    p_location_notification_res->location_notification_res.location_notification_res =
            PERMISSION_GRANTED;   /* 1 */
  } 
  else  /* if OEM response invalid, reply with no reponse present */
  {
    p_location_notification_res->location_notification_res.present = FALSE;
  }

  /* ask TmCore to send LCS Location Notification Return Result to CM */
  ret_val = tm_cm_iface_stop_req(TM_PRTL_TYPE_UMTS_CP, &z_cm_stop_info);
  if ( ret_val )
  {
    /* log the response */
    cp_ss_log_lcs_location_notification_resp( p_location_notification_res );
  }
  else
  {
    MSG_ERROR("TM-CM iface fails to send LN-notif-verif response", 0, 0, 0);
  }

  return ret_val;

}

/*===========================================================================

FUNCTION 

DESCRIPTION
  This function processes SS LcsMoLr 
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/

void tm_umts_cp_ss_lcs_molr_proc(void)
{
  /* CM confirms LCS-MOLR-Invoke queued to NAS */
  MSG_MED("UMTS-CP-SS: CM confirms molr-invoke queued to NAS", 0, 0, 0);

  /* if TM has not attempted to send MoLrInvoke, and it receives this
     event from CM, a third-party session has started */
  if ( z_umts_cp_ss_session_status.molr_on == FALSE )
  {
    z_umts_cp_ss_session_status.third_party = TRUE;
  }
  else
  {
    #ifdef FEATURE_CGPS_DUAL_WCDMA
    cp_ss_notify_session_status_to_mm(TRUE);    
	#else
	tm_umts_common_utils_mm_session_status_notify( TRUE );
	#endif
  }
}

/*===========================================================================

FUNCTION 

DESCRIPTION
  This function processes SS LcsMoLR Complete 
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/

void tm_umts_cp_ss_lcs_molr_comp_proc(void)
{
  z_umts_cp_ss_session_status.molr_comp = TRUE;
}


/*===========================================================================

FUNCTION 

DESCRIPTION
  This function checks to see if we got a release complete indication from CM 
  before getting the facility message or before TM has initiated the process 
  to "release complete", after MOLR-invoke has been passed to CM
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/
boolean tm_umts_cp_ss_is_molr_on_comp(void)
{
  if ((z_umts_cp_ss_session_status.molr_on == TRUE) && (z_umts_cp_ss_session_status.molr_comp == TRUE))
  {
    return TRUE;
  }
  if (z_umts_cp_ss_session_status.molr_conf == TRUE)
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION
  tm_umts_cp_ss_lcs_loc_notif_ind_proc

DESCRIPTION
  Processing SS LCS_Location_Notification_Ind

PARAMETERS
   p_sup_data - Pointer to an SS data structure. 

RETURN VALUE
  Void

===========================================================================*/

void tm_umts_cp_ss_lcs_loc_notif_ind_proc (tm_cm_sups_data_s_type* p_sups_data)
{

  pdsm_lcs_notification_resp_s_type          z_dummy_user_resp; /* used for immediate response */
  emergency_handling_e_type  e_e911_effect = C_EMERGENCY_HANDLING_NULL;

  cp_ss_log_lcs_location_notification_invoke (&p_sups_data->lcs_mess.location_notification_ind);

  e_e911_effect = tm_umts_common_utils_emergency_effect();

  if ( p_sups_data->lcs_mess.location_notification_ind.notification_type == NOTIFY_LOCATION_ABORTED )
  {
    /* A Release Complete or Reject has been received from the network */
    MSG_MED("Confirmation of releaseComplete to NotifInd received", 0, 0, 0); 
  }
  else
  {
    if   ( (tm_core_mtlr_auto_accept() == FALSE)  ||
           (e_e911_effect == C_EMERGENCY_HANDLING_NULL) )
    {
      /* If not in emergency call or if the special MTLR processing is not enabled,
         check if GPS NI is locked. If NI is locked send a deny to the network, otherwise pass
         the notification to AP */
      if ( (z_umts_nv_item_ram_copy.gps_lock != PDSM_GPS_LOCK_ALL) &&
           (z_umts_nv_item_ram_copy.gps_lock != PDSM_GPS_LOCK_MT)
           )
      {
        /* regular case: LBS not locked by NV_Item */
        cp_ss_pass_notification_verifcation_to_pdapi(p_sups_data);

      }
      else /* NV_Item gps1_lock locks up LBS, reject MT LCS request */
      {
        z_dummy_user_resp.resp = PDSM_LCS_NOTIFY_VERIFY_DENY;
        z_dummy_user_resp.lcs_info.umts_cp_req.invoke_id = p_sups_data->u_invoke_id;
        (void)tm_umts_ss_send_lcs_loc_notif_return_result(&z_dummy_user_resp);
      }
    }
    else
    {
      /* If in emergency state and special handling is required to auto accept the MTLR, check NV 1930 to ensure the
         MT can be processed.*/
      if ( (e_e911_effect == C_EMERGENCY_HANDLING_ACCEPT_ONLY_HIGH_PRIORITY) ||
           (e_e911_effect == C_EMERGENCY_HANDLING_ACCEPT_ALL)
           )
      {
        /* E911 case: Process regardless of LBS lock. Respond
           immediately. NO AP notification */
        MSG_MED("MTLR auto accepted e911_setting = %d, auto_accept set to %d",
                e_e911_effect, tm_core_mtlr_auto_accept(), 0);
        z_dummy_user_resp.resp = PDSM_LCS_NOTIFY_VERIFY_ACCEPT;
        z_dummy_user_resp.lcs_info.umts_cp_req.invoke_id = p_sups_data->u_invoke_id;
        (void)tm_umts_ss_send_lcs_loc_notif_return_result(&z_dummy_user_resp);
      }
      else
      {
        MSG_MED("RRC message rejected due to NV item %d",
                e_e911_effect, 0, 0);
        /* deny the MTLR even though Auto accept is set */
        z_dummy_user_resp.resp = PDSM_LCS_NOTIFY_VERIFY_DENY;
        z_dummy_user_resp.lcs_info.umts_cp_req.invoke_id = p_sups_data->u_invoke_id;
        (void)tm_umts_ss_send_lcs_loc_notif_return_result(&z_dummy_user_resp);
      }
    }
  }
}


/*===========================================================================

FUNCTION
  tm_umts_cp_ss_lcs_molr_conf_proc

DESCRIPTION
  Processing SS LCS_MOLR_CONF

PARAMETERS
   p_sup_data - Pointer to MOLR Conf. data

RETURN VALUE
  Void

===========================================================================*/

void tm_umts_cp_ss_lcs_molr_conf_proc (tm_cm_sups_data_s_type *p_sups_data)
{
  tm_cm_stop_info_s_type z_cm_stop_info;
  lcs_molr_res_s_type *p_molr_res = &p_sups_data->lcs_mess.lcs_molr_res;

  z_cm_stop_info.stop_reason = TM_CM_STOP_REASON_MOLR_COMPLETE; 

  z_umts_cp_ss_session_status.molr_on = FALSE;
  z_umts_cp_ss_session_status.molr_conf = TRUE;

  /* inform MM LCS session over */
  #ifdef FEATURE_CGPS_DUAL_WCDMA
  cp_ss_notify_session_status_to_mm(FALSE); 
  #else
  tm_umts_common_utils_mm_session_status_notify( FALSE );
  #endif

  if ( z_umts_cp_ss_session_status.third_party == FALSE ) 
  {

    /* Close out MO-LR transaction */
    z_cm_stop_info.stop_params.lcs_molr_completed_cmd_params.ss_ref = 
      p_sups_data->u_ss_ref_num;

     if ( tm_cm_iface_stop_req(TM_PRTL_TYPE_UMTS_CP, &z_cm_stop_info) )
     {
       MSG_MED("SS realseComp req.", 0, 0, 0);
     }
     else
     {
       MSG_HIGH("Failed releaseComp", 0, 0, 0);
     }
  }

  if ( p_molr_res->data_type == LCS_MOLR_INFO )
  {
    /* MOLR-ReturnResult */
    MSG_MED( "recvd CONF-RetRes refNum=%u", p_sups_data->u_ss_ref_num, 0, 0);
    cp_ss_log_lcs_molr_return_result ( &p_molr_res->data.molr_res_data );
  }

  else if ( p_molr_res->data_type == SS_ERROR_INFO )
  {
    MSG_MED( "recvd CONF_SS_Error refNum=%u", p_sups_data->u_ss_ref_num, 0, 0);
    cp_ss_log_lcs_molr_return_error ( &p_molr_res->data.cm_ss_error );
  }

  else if ( p_molr_res->data_type == CC_CAUSE_INFO )
  {
    MSG_MED( "recv CC_CAUSE refNum=%u", p_sups_data->u_ss_ref_num, 0, 0);
  }
 
  else
  {
    MSG_MED("UMTS-CP-SS: ignore molr_conf of %u", p_molr_res->data_type, 0, 0 );
  }  

}


/*===========================================================================

FUNCTION
  tm_umts_cp_ss_ss_completion_req

DESCRIPTION
  Used to req. SS to complete.

PARAMETERS
  none.

RETURN VALUE
  Void

===========================================================================*/
void tm_umts_cp_ss_ss_completion_req (void)
{
  
  if ( z_umts_cp_ss_session_status.molr_on )
  {
    #ifdef FEATURE_SS_RELEASE_COMPLETE_BEFORE_FACILITY_MESSAGE_ALLOWED
    tm_cm_stop_info_s_type z_cm_stop_info;

    /* inform MM LCS session over */
	#ifdef FEATURE_CGPS_DUAL_WCDMA
    cp_ss_notify_session_status_to_mm(FALSE); 
	#else
	tm_umts_common_utils_mm_session_status_notify( FALSE );
	#endif

    /* Close out MO-LR transaction */
    z_cm_stop_info.stop_params.lcs_molr_completed_cmd_params.ss_ref = 
      z_umts_cp_ss_session_status.ss_ref_num;

    z_cm_stop_info.stop_reason = TM_CM_STOP_REASON_MOLR_COMPLETE; 

    if ( tm_cm_iface_stop_req(TM_PRTL_TYPE_UMTS_CP, &z_cm_stop_info) )
    {
      MSG_MED("SS realseComp req.", 0, 0, 0);
    }
    else
    {
      MSG_HIGH("Failed releaseComp", 0, 0, 0);
    }

    z_umts_cp_ss_session_status.molr_on = FALSE;


    #endif

  }

}




/*
******************************************************************************
* cp_ss_pass_notification_verifcation_to_pdapi 
*
* Function description:
*   sending of notification verification to PDAPI via TmCore
* 
* Parameters:
*
*  
*
* Return value:
*  None
*
*
******************************************************************************
*/
static void cp_ss_pass_notification_verifcation_to_pdapi(tm_cm_sups_data_s_type *p_sups_data )
{ 
   
  pdsm_lcs_info_s_type z_notification_info;

  pdsm_lcs_umts_cp_notify_verify_req_s_type *p_umts_cp_ss_pdapi_notif_verif = 
    &z_notification_info.lcs_info.umts_cp_req;

  location_notification_s_type *p_notification_ind = &p_sups_data->lcs_mess.location_notification_ind;
  
  uint8 text[PDSM_LCS_CLIENT_NAME_LEN]; /* used for lcsClientName only */
  uint8 *data_ptr = NULL;
  uint8 length = 0;
  pdsm_lcs_notify_verify_datacoding_scheme_e_type     datacoding_scheme;
  
  memset( &z_notification_info, 0, sizeof ( pdsm_lcs_info_s_type ) );

  MSG_MED("UMTS-CP-SS: pass notif_verif to pdapi", 0, 0, 0);

  z_notification_info.client_id = pdsm_ni_client_id_get(); 
  z_notification_info.lcs_cb_cmd_type = PDSM_LCS_CMD_TYPE_UMTS_CP_REQ;
  z_notification_info.lcs_info.umts_cp_req.invoke_id = p_sups_data->u_invoke_id;  

  /* check if optional notification/verification IE is present */
  if ( p_notification_ind->notification_type == NOTIFY_LOCATION_ALLOWED )
  {
    p_umts_cp_ss_pdapi_notif_verif->notification_priv_type = PDSM_LCS_USER_NOTIFY_ONLY;
  }
  else if ( p_notification_ind->notification_type == NOTIFY_AND_VERIFY_LCS_ALLOWED_IF_NO_RESPONSE )
  {
    p_umts_cp_ss_pdapi_notif_verif->notification_priv_type = PDSM_LCS_USER_NOTIFY_VERIFY_ALLOW_NO_RESP;
  }
  else if ( p_notification_ind->notification_type == NOTIFY_AND_VERIFY_LCS_NOT_ALLOWED_IF_NO_RESPONSE )
  {
    p_umts_cp_ss_pdapi_notif_verif->notification_priv_type = PDSM_LCS_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP;
  }
  else
  {
    return; /* other types should not get here */
  }


  if ( p_notification_ind->client_name.present ) /* copy client name */
  {
    if ( p_notification_ind->client_name.namestring.present )
    {
      data_ptr = p_notification_ind->client_name.namestring.data;
      length = p_notification_ind->client_name.namestring.size;  
      
      if ( length > PDSM_LCS_CLIENT_NAME_LEN )
        length = PDSM_LCS_CLIENT_NAME_LEN;
      
      (void)GNSS_STRLCPY((char *)&text[0], (char *)data_ptr, PDSM_LCS_CLIENT_NAME_LEN );
      
      p_umts_cp_ss_pdapi_notif_verif->flags |= PDSM_LCS_CLIENT_NAME_PRESENT;
      p_umts_cp_ss_pdapi_notif_verif->notification_length = length;
      p_umts_cp_ss_pdapi_notif_verif->notification_text = (char*)text;
      if ( length < PDSM_LCS_CLIENT_NAME_LEN )
      {
        text[length] = '\0';
      }
      else
      {
        text[ PDSM_LCS_CLIENT_NAME_LEN - 1 ] = '\0'; 
      }  

      cp_ss_convert_coding_to_pdapi ( p_notification_ind->client_name.data_coding_scheme,
                                      &p_umts_cp_ss_pdapi_notif_verif->datacoding_scheme
                                    );
      p_umts_cp_ss_pdapi_notif_verif->flags |= PDSM_LCS_ENCODING_TYPE_PRESENT;
    }
  } /* optional lcsClientName present */
  
  if ( p_notification_ind->requestor_id.present ) /* copy requestorID */
  {
    if ( p_notification_ind->requestor_id.requestorIDString.present )
    {
      p_umts_cp_ss_pdapi_notif_verif->flags |= PDSM_LCS_REQUESTOR_ID_PRESENT;
      
      /*  copy the string locally */
      length = memscpy(p_umts_cp_ss_pdapi_notif_verif->requestor_id.requestor_id_string,
                       sizeof(p_umts_cp_ss_pdapi_notif_verif->requestor_id.requestor_id_string),
              p_notification_ind->requestor_id.requestorIDString.data,
                       p_notification_ind->requestor_id.requestorIDString.size);
      p_umts_cp_ss_pdapi_notif_verif->requestor_id.requestor_id_string[PDSM_LCS_REQUESTOR_ID_LEN - 1] = NULL;
      p_umts_cp_ss_pdapi_notif_verif->requestor_id.string_len = length; 
      
      cp_ss_convert_coding_to_pdapi ( 
        p_notification_ind->requestor_id.data_coding_scheme, 
        &datacoding_scheme );
      p_umts_cp_ss_pdapi_notif_verif->requestor_id.data_coding_scheme = (byte)datacoding_scheme;
    } 
  } /* optional requestorID present */
  
  if ( p_notification_ind->codeword_string.present )
  {
    if ( p_notification_ind->codeword_string.lcsCodewordString.present )
    {
      p_umts_cp_ss_pdapi_notif_verif->flags |= PDSM_LCS_CODEWORD_PRESENT;
    
      /*  copy the string locally */
      length = memscpy(p_umts_cp_ss_pdapi_notif_verif->codeword_string.lcs_codeword_string,
                       sizeof(p_umts_cp_ss_pdapi_notif_verif->codeword_string.lcs_codeword_string),
              p_notification_ind->codeword_string.lcsCodewordString.data,
                       p_notification_ind->codeword_string.lcsCodewordString.size);
      p_umts_cp_ss_pdapi_notif_verif->codeword_string.lcs_codeword_string[PDSM_LCS_CODEWORD_LEN-1] = NULL;
      p_umts_cp_ss_pdapi_notif_verif->codeword_string.string_len = length; 
      
      cp_ss_convert_coding_to_pdapi ( 
        p_notification_ind->codeword_string.data_coding_scheme,
        &datacoding_scheme );
      p_umts_cp_ss_pdapi_notif_verif->codeword_string.data_coding_scheme = (byte)datacoding_scheme;
    }
  }  /* optional clsCodeword present */
  
  if ( p_notification_ind->service_type.present )
  {
    p_umts_cp_ss_pdapi_notif_verif->flags |= PDSM_LCS_SERVICE_TYPE_ID_PRESENT;
    p_umts_cp_ss_pdapi_notif_verif->service_type_id.lcs_service_type_id =
      p_notification_ind->service_type.lcsServiceTypeID;
  } /* optional lcsServiceTypeID present */
  
  if ( p_notification_ind->external_id.present )
  {
    if ( p_notification_ind->external_id.external_address.present )
    {
      p_umts_cp_ss_pdapi_notif_verif->flags |= PDSM_LCS_CLIENT_EXTADDR_PRESENT;
    
      /*  copy the string locally */
      length = memscpy(p_umts_cp_ss_pdapi_notif_verif->ext_client_address_data.ext_client_address,
                       sizeof(p_umts_cp_ss_pdapi_notif_verif->ext_client_address_data.ext_client_address),
              p_notification_ind->external_id.external_address.data,
                       p_notification_ind->external_id.external_address.size);
      p_umts_cp_ss_pdapi_notif_verif->ext_client_address_data.ext_client_address[PDSM_LCS_MAX_EXT_CLIENT_ADDRESS-1] = NULL;
      p_umts_cp_ss_pdapi_notif_verif->ext_client_address_data.ext_client_address_len = length; 
    }
  } /* optional lcsClientExternalID present */
  
  if ( p_notification_ind->deferred_location_type.present )
  {
    p_umts_cp_ss_pdapi_notif_verif->flags |= PDSM_LCS_DEF_LOCATION_TYPE_PRESENT;
    p_umts_cp_ss_pdapi_notif_verif->deferred_location.ms_available =
      p_notification_ind->deferred_location_type.ms_available;
    p_umts_cp_ss_pdapi_notif_verif->deferred_location.unused_bits =
      p_notification_ind->deferred_location_type.unused_bits;
  } /* optional deferredLocationEventType present */
  

  /* location type */
  if ( p_notification_ind->location_type == CURRENT_LOCATION )
  {
    p_umts_cp_ss_pdapi_notif_verif->location_type = PDSM_LOCATIONTYPE_CURRENT_LOCATION;
  }
  else if ( p_notification_ind->location_type == CURRENT_OR_LAST_KNOWN_LOCATION )
  {
    p_umts_cp_ss_pdapi_notif_verif->location_type = PDSM_LOCATIONTYPE_CURRENT_OR_LAST_KNOWN_LOCATION;
  }
  else if ( p_notification_ind->location_type == INITIAL_LOCATION )
  {
    p_umts_cp_ss_pdapi_notif_verif->location_type = PDSM_LOCATIONTYPE_INITIAL_LOCATION;
  }
  else
  {
    /* QQQ: TBD */
    MSG_HIGH("UMTS-CP-SS: unsupported loc type: %u", p_umts_cp_ss_pdapi_notif_verif->location_type, 0, 0);
    return;
  }


  (void) tm_request_app_response ( TM_PRTL_TYPE_UMTS_CP,
                            PDSM_LCS_EVENT_UMTS_CP_NOTIFY_VERIFY_REQ,
                            &z_notification_info    
                          );

}


/*
 ******************************************************************************
 * cp_ss_convert_coding_to_pdapi 
 *
 * Function description:
 *   Converts the Supplemetary services coding scheme to PDPAI value 
 * 
 * Parameters:
 *
 *  ss coding scheme : input, value of the SS coding scheme 
 *  pdapi scheme : output, value of the pdapi coding value
 *
 * Return value:
 *  None
 *
 *
 ******************************************************************************
*/
void cp_ss_convert_coding_to_pdapi( byte coding_scheme,
        pdsm_lcs_notify_verify_datacoding_scheme_e_type *pdapi_scheme)
{
  switch (coding_scheme)
    {
    case 0x00:
      *pdapi_scheme = PDSM_SS_GERMAN;
      break;
    case 0x01:
      *pdapi_scheme = PDSM_SS_ENGLISH;
      break;
    case 0x02:
      *pdapi_scheme = PDSM_SS_ITALIAN;
      break;
    case 0x03:
      *pdapi_scheme = PDSM_SS_FRENCH ;
      break;
    case 0x04:
      *pdapi_scheme = PDSM_SS_SPANISH ;
      break;
    case 0x05:
      *pdapi_scheme = PDSM_SS_DUTCH ;
      break;
    case 0x06:
      *pdapi_scheme = PDSM_SS_SWEDISH ;
      break;
    case 0x07:
      *pdapi_scheme = PDSM_SS_DANISH ;
      break;
    case 0x08:
      *pdapi_scheme = PDSM_SS_PORTUGUESE ;
      break;
    case 0x09:
      *pdapi_scheme = PDSM_SS_FINNISH ;
      break;
    case 0x0a:
      *pdapi_scheme = PDSM_SS_NORWEGIAN ;
      break;
    case 0x0b:
      *pdapi_scheme = PDSM_SS_GREEK ;
      break;
    case 0x0c:
      *pdapi_scheme = PDSM_SS_TURKISH ;
      break;
    case 0x0d:
      *pdapi_scheme = PDSM_SS_HUNGARIAN ;
      break;
    case 0x0e:
      *pdapi_scheme = PDSM_SS_POLISH ;
      break;
    case 0x0f:
      *pdapi_scheme = PDSM_SS_LANGUAGE_UNSPEC ;
      break;
    default:
      *pdapi_scheme = PDSM_SS_LANGUAGE_UNSPEC;
      break;

    }
}



 
/*===========================================================================

FUNCTION
  cp_ss_log_lcs_molr_invoke

DESCRIPTION
  Logs the MOLR invoke message using DM.

PARAMETERS
   ss_ptr - Pointer to MOLR Invoke Structure

RETURN VALUE
  Void

===========================================================================*/
static void cp_ss_log_lcs_molr_invoke( cm_lcs_molr_params_s_type *ss_ptr )
{
  uint8 log_buff[150], *log_ptr = (uint8 *)log_buff;

  /* SS Type */
  *log_ptr++ = SS_TYPE_MOLR_INVOKE; 

  /* SS Ref */
  *log_ptr++ = ss_ptr->ss_ref;
 
  /* MO-LR Type */
  *log_ptr++ = ss_ptr->molr_type;

  /* Location Method Present*/
  *log_ptr++ = ss_ptr->location_method.present;

  /* Location Method */
  if ( ss_ptr->location_method.present )
    *log_ptr++ = ss_ptr->location_method.data;

  /* LCS QOS Present */
  *log_ptr++ = ss_ptr->lcs_qos.present;

  /* Fill in QOS if present */
  if ( ss_ptr->lcs_qos.present )
  {
    lcs_qos_T *qos_ptr = &ss_ptr->lcs_qos;

    /* Horiz Accuracy Present */
    *log_ptr++ = qos_ptr->horiz_accuracy.present;

    /* Horiz Accuracy */
    if ( qos_ptr->horiz_accuracy.present )
      *log_ptr++ = qos_ptr->horiz_accuracy.data;

    /* Vert Coordinate Request Present */
    *log_ptr++ = qos_ptr->vertical_coord_req.present;

    /* Vert Accuracy Present */
    *log_ptr++ = qos_ptr->vertical_accuracy.present;

    /* Vert Accuracy */
    if ( qos_ptr->vertical_accuracy.present )
      *log_ptr++ = qos_ptr->vertical_accuracy.data;

    /* Response Time Present */
    *log_ptr++ = qos_ptr->response_time.present;

    /* Response Time */
    if ( qos_ptr->response_time.present )
      *log_ptr++ = qos_ptr->response_time.response_time_category;   
  }

  /* MLC Number Present */
  *log_ptr++ = ss_ptr->mlc_number.present;

  /* MLC Number */
  if ( ss_ptr->mlc_number.present )
  {
    uint32 cnt;
    uint8 *mlc_ptr = &ss_ptr->mlc_number.data[0];

    /* The DM spec mistakenly allocates a fixed 20 bytes for MLC number. However,
       the number is actually be between 1-9 bytes. Because of this, prepend
       the necessary amount of '0' bytes before the number to fill out all 20
       bytes of the log payload */

    /* Fill in zeroes */
	for ( cnt = ( 20 - ss_ptr->mlc_number.size ); cnt; cnt-- )
	  *log_ptr++ = 0;
	  
    /* Copy MLC Number */
    for ( cnt = ss_ptr->mlc_number.size; cnt; cnt-- )
      *log_ptr++ = *mlc_ptr++;
  }

  /* GPS Assistance Data Present */
  *log_ptr++ = ss_ptr->gps_assistance_data.present;

  /* GPS Assistance Data */
  if ( ss_ptr->gps_assistance_data.present )
  {
    uint8 cnt;
	uint8 size;
	uint8 *data_ptr = ss_ptr->gps_assistance_data.data;

    /* GPS Assistance Data Length */
    size = ss_ptr->gps_assistance_data.size;
    
    /* Limit size if necessary */
    if ( size > 38 )
	  size = 38;

    *log_ptr++ = size;

    /* Copy the data */
    for ( cnt = size; cnt; cnt-- )
      *log_ptr++ = *data_ptr++;
  }

  /* Supported GAD Shapes Present */
  *log_ptr++ = ss_ptr->supported_gad_shapes.present;

  /* Supported GAD Shapes */
  if ( ss_ptr->supported_gad_shapes.present )
    *log_ptr++ = ss_ptr->supported_gad_shapes.data;

  if ( ss_ptr->external_id.present )
  {
    /* External Address Present */
    *log_ptr++ = ss_ptr->external_id.external_address.present;

    /* External Address */
    if ( ss_ptr->external_id.external_address.present )
    {
      uint8 cnt;
      uint8 size;
      uint8 *data_ptr = ss_ptr->external_id.external_address.data;

      size = ss_ptr->external_id.external_address.size;

      if ( size > 20 )
        size = 20;

      *log_ptr++ = size;

      for ( cnt = size; cnt; cnt-- )
      {
        *log_ptr++ = *data_ptr++;
      }
    }
  }
  else
  {
    /* External Address is not present */
	*log_ptr++ = FALSE;
  }


  /* log the message: note that the 2nd argument is not used other than for Snapper to
     display a corresponding string to indicate the "connection type", a concept that no longer
     is relevant. Snapper uses 3 for the string "control plane" */
  sm_log_svr_txrx_rep( C_LSM_LOG_SVR_TX_TYPE, 
                       3, 
                       C_LSM_SVR_PROTOCOL_TYPE_SUP_SVCS, 
                       log_ptr - &log_buff[0],
                       &log_buff[0] );

}



/*===========================================================================

FUNCTION
  CP_SS_LOG_LCS_MOLR_RETURN_RESULT

DESCRIPTION
  Logs the MOLR Return Result message using DM.

PARAMETERS
   ss_ptr - Pointer to MOLR Return Result Structure

RETURN VALUE
  Void

===========================================================================*/
static void cp_ss_log_lcs_molr_return_result( lcs_molr_res_data_T  *res_ptr )
{
  uint8 log_buff[150], *log_ptr = (uint8 *)log_buff;

  /* SS Type */
  *log_ptr++ = SS_TYPE_MOLR_RETURN_RESULT;

  /* Location Estimate Present */
  *log_ptr++ = res_ptr->location_estimate.present;

  /* Location Estimate */
  if ( res_ptr->location_estimate.present )
  {
    uint8 size;
    uint8 cnt;
    uint8 *data_ptr = &res_ptr->location_estimate.data[0];

    /* Length */
    size = res_ptr->location_estimate.size;

    /* Boundary Check */
    if ( size > 20 )
	  size = 20;

    *log_ptr++ = size;

    /* Location Estimate */
    for ( cnt = size; cnt; cnt-- )
    *log_ptr++ = *data_ptr++;
  }

  /* Deciphering Keys Present */
  *log_ptr++ = res_ptr->deciphering_keys.present;

  /* Deciphering Keys */
  if ( res_ptr->deciphering_keys.present )
  {
    uint8 cnt;
    uint8 *data_ptr = &res_ptr->deciphering_keys.data[0];

    for ( cnt = 15; cnt; cnt-- )
      *log_ptr++ = *data_ptr++;
  }

  /* Additional Location Estimate Present */
  *log_ptr++ = res_ptr->add_location_estimate.present;

  /* Additional Location Estimate */
  if ( res_ptr->add_location_estimate.present )
  {
    uint8 size;
    uint8 cnt;
    uint8 *data_ptr = &res_ptr->add_location_estimate.data[0];

    /* Length */
    size = res_ptr->add_location_estimate.size;

    if ( size > 90 )
	  size = 90;

    *log_ptr++ = size;

    /* Copy Data */
    for ( cnt = size; cnt; cnt-- )
      *log_ptr++ = *data_ptr++;
  }

  /* Send the log message */
  sm_log_svr_txrx_rep( C_LSM_LOG_SVR_RX_TYPE, 
                       3,
                       C_LSM_SVR_PROTOCOL_TYPE_SUP_SVCS, 
                       log_ptr - &log_buff[0], 
                       &log_buff[0] );
}


/*===========================================================================

FUNCTION
  LSM_SUPS_LOG_LCS_MOLR_RETURN_ERROR

DESCRIPTION
  Logs the MOLR Return Error message using DM.

PARAMETERS
   ss_ptr - Pointer to MOLR Return Error Structure

RETURN VALUE
  Void

===========================================================================*/
static void cp_ss_log_lcs_molr_return_error ( ie_cm_ss_error_T *err_ptr )
{
  uint8 log_buff[5], *log_ptr = (uint8 *)log_buff;

  /* SS Type */
  *log_ptr++ = SS_TYPE_MOLR_RETURN_ERROR;

  /* Error Code */
  *log_ptr++ = err_ptr->error_code;

  if ( err_ptr->error_code == 34 ) /* System Failure */
  {
    /* Network Resource */
    *log_ptr++ = err_ptr->parameter.cm_system_failure_error.network_resource;
  }
  else if ( err_ptr->error_code == 54 ) /* Position Method Failure */
  {
    /* Position Method Failure Diagnostic */
    *log_ptr++ = err_ptr->parameter.cm_position_method_failure_error.position_method_failure_diagnostic;
  }

  sm_log_svr_txrx_rep( C_LSM_LOG_SVR_RX_TYPE, 
                       3,
                       C_LSM_SVR_PROTOCOL_TYPE_SUP_SVCS, 
                       log_ptr - &log_buff[0], 
                       &log_buff[0] );

}



/*===========================================================================

FUNCTION
  cp_ss_log_lcs_location_notification_invoke

DESCRIPTION
  Logs the location notification invoke message using DM.

PARAMETERS
   p_sups_data - Pointer to SS structure.

RETURN VALUE
  Void

===========================================================================*/
static void cp_ss_log_lcs_location_notification_invoke ( location_notification_s_type *ss_ptr )
{
  uint8 log_buff[150], *log_ptr = (uint8 *)log_buff;

  /* Byte 0 is SS Type */
  *log_ptr++ = SS_TYPE_LOC_NOTIFICATION_INVOKE; 

  /* Byte 1 is Notification Type */
  *log_ptr++ = ss_ptr->notification_type;

  /* Byte 2 is Location Type */
  *log_ptr++ = ss_ptr->location_type;

  /* External Address */
  if ( ss_ptr->external_id.present )
  {
    /* External Address Present */
    *log_ptr++ = ss_ptr->external_id.external_address.present;

    /* External Address */
    if ( ss_ptr->external_id.external_address.present )
    {
      uint8 cnt;
      uint8 size;
      uint8 *data_ptr = ss_ptr->external_id.external_address.data;

      size = ss_ptr->external_id.external_address.size;

      if ( size > 20 )
          size = 20;

      *log_ptr++ = size;

      for ( cnt = size; cnt; cnt-- )
      {
        *log_ptr++ = *data_ptr++;
      }
    }
  }
  else
  {
    /* External Address is not present */
    *log_ptr++ = FALSE;
  }

  /* LCS Client Name */
  *log_ptr++ = ss_ptr->client_name.present;

  if ( ss_ptr->client_name.present )
  {
    /* Data Coding Scheme */
    *log_ptr++ = ss_ptr->client_name.data_coding_scheme;

    /* NameString */
	*log_ptr++ = ss_ptr->client_name.namestring.present;

    if ( ss_ptr->client_name.namestring.present )
	{
	  uint8 size;
	  uint8 cnt;
	  uint8 *data_ptr = ss_ptr->client_name.namestring.data;

      size = ss_ptr->client_name.namestring.size;

      if ( size > 63 )
	    size = 63;

      *log_ptr++ = size;

      for ( cnt = size; cnt; cnt-- )
	  {
	    *log_ptr++ = *data_ptr++;
	  }
    }
  }

  /* Send the log message */
  sm_log_svr_txrx_rep( C_LSM_LOG_SVR_RX_TYPE, 
                       3,
                       C_LSM_SVR_PROTOCOL_TYPE_SUP_SVCS, 
                       log_ptr - &log_buff[0], 
                       &log_buff[0] );

}


/*===========================================================================

FUNCTION
  CP_SS_LOG_LCS_LOCATION_NOTIFICATION_RESP

DESCRIPTION
  Logs the location notification response message using DM.

PARAMETERS
   ss_ptr - Pointer to LCS Location Notification Response Structure

RETURN VALUE
  Void

===========================================================================*/

void cp_ss_log_lcs_location_notification_resp
(cm_lcs_location_notification_res_params_s_type *ss_ptr )
{
  uint8 log_buff[2], *log_ptr = (uint8 *)log_buff;

  /* Byte 0 is SS Type */
  *log_ptr++ = SS_TYPE_LOC_NOTIFICATION_RETURN_RESULT;

  /* Byte 1 is Location Notification Response */
  *log_ptr++ = ss_ptr->location_notification_res.location_notification_res;

  /* Send the log message */
  sm_log_svr_txrx_rep( C_LSM_LOG_SVR_TX_TYPE, 
                       3,
                       C_LSM_SVR_PROTOCOL_TYPE_SUP_SVCS, 
                       log_ptr - &log_buff[0], &log_buff[0] );
}

#ifdef FEATURE_CGPS_DUAL_WCDMA
/*===========================================================================

FUNCTION
  cp_ss_notify_session_status_to_mm

DESCRIPTION
  Finds the current serving system and notifies MM of the session status

PARAMETERS
   session_started - If true, indicates a session has started, otherwise indicates that a session has ended.
   

RETURN VALUE
  Void

===========================================================================*/

void cp_ss_notify_session_status_to_mm(boolean session_started)
{
  tm_cm_phone_state_info_s_type phone_state_info;
  
  memset(&phone_state_info, 0, sizeof(tm_cm_phone_state_info_s_type));
	
  tm_cm_iface_get_phone_state(&phone_state_info);

  if (phone_state_info.srv_system == CGPS_SRV_SYS_GSM)
  {
    tm_umts_common_utils_mm_session_status_notify( session_started, z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id ); 
  }
  else if (phone_state_info.srv_system == CGPS_SRV_SYS_WCDMA)
  {
	tm_umts_common_utils_mm_session_status_notify( session_started, z_umts_cp_wcdma_session_status.z_rrc_as_id_info.e_cached_rrc_as_id ); 
  }
  else
  {
	MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Attempting to notify MM layer for non-UMTS RAT %u", (uint8)phone_state_info.srv_system);
  }
}
#endif

#endif  /* FEATURE_CGPS_UMTS_CP */

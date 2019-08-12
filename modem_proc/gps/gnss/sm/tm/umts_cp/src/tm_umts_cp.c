/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         TM_UMTS_CP 

GENERAL DESCRIPTION
  UMTS_CP module: interacts with TmCore and all UMTS-CP protocols.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_cp/src/tm_umts_cp.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  07/10/15   mj      Add changes to support correct function call back during DD changes
  04/30/15   mj      Add support for early session start for WCDMA for CSFB during E911
  05/21/09   gk      Treat PDSM_SESS_TYPE_TRACK_IND as new
  04/21/09   gk      Check if we are in UMTS/GSM before starting 911 processing
  02/09/09   atien   Fix compiler warning
  01/12/09   atien   MOLR vs AD call flow select support
  05/3/07    LT      Initial version

============================================================================*/

#include "gps_variation.h"
#include "customer.h"   /* Customer configuration file */
#ifdef FEATURE_CGPS_UMTS_CP

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "msg.h"
#include <limits.h>
#include "tm_prtl_iface.h"
#include "tm_umts_common_utils.h"
#include "tm_cm_iface.h"
#include "tm_data.h"
#include "sm_nv.h"

#include "tm_umts_cp_ss.h"

#ifdef FEATURE_CGPS_UMTS_CP_WCDMA
#define TM_UMTS_CP_WCDMA_API_USED_BY_UMTS_CP
#include "tm_umts_cp_wcdma_api.h"
#undef  TM_UMTS_CP_WCDMA_API_USED_BY_UMTS_CP
#endif

#ifdef FEATURE_CGPS_UMTS_CP_GSM
#include "tm_umts_cp_gsm_api.h"
#endif

#define extern
#include "tm_umts_cp_submodule_iface.h"
#undef extern


/*===========================================================================
                 Data Definition
===========================================================================*/


typedef struct
{

  prtl_start_sess_req_result_e_type ( *start_sess_req_handler ) ( tm_sess_handle_type               sess_handle,
                                                                  prtl_start_sess_req_param_s_type  *start_sess_req_param,
                                                                  tm_sess_req_start_param_s_type    *actions );

  boolean ( *stop_sess_req_handler)( tm_sess_handle_type           sess_handle,
                                     prtl_sess_stop_type           stop_type,  
                                     tm_sess_stop_reason_e_type    stop_reason );

  boolean ( *sess_req_data_handler ) ( tm_sess_handle_type  session_handle,
                                       prtl_data_req_type   req_data );

  boolean ( *sess_info_handler ) ( tm_sess_handle_type          session_handle,
                                   prtl_sess_info_e_type        sess_info_type,
                                   prtl_sess_info_param_u_type  *p_sess_info_param );

  void ( *timer_cb_handler ) ( void *timerData );

  boolean ( *event_cb_handler ) ( prtl_event_e_type  event_type,
                                  prtl_event_u_type  *p_event_payload );

  boolean ( *general_query_handler ) ( prtl_query_e_type query_type, 
                                       prtl_ans_u_type   *answer_ptr ); 

}umts_cp_function_table_s_type;   /* function pointer structure for switching between WCDMA and GSM */


#ifdef FEATURE_CGPS_UMTS_CP_WCDMA 
static umts_cp_function_table_s_type   z_umts_cp_fcn_table[]
= 
{
  { tm_umts_cp_wcdma_start_sess_req_handler,
    tm_umts_cp_wcdma_stop_sess_req_handler,
    tm_umts_cp_wcdma_sess_req_data_handler,
    tm_umts_cp_wcdma_sess_info_handler,
    tm_umts_cp_wcdma_timer_cb_handler,
    tm_umts_cp_wcdma_event_cb_handler,
    tm_umts_cp_wcdma_general_query_handler
  }
  #ifdef FEATURE_CGPS_UMTS_CP_GSM  
  ,
  { tm_umts_cp_gsm_start_sess_req_handler,
    tm_umts_cp_gsm_stop_sess_req_handler,
    tm_umts_cp_gsm_sess_req_data_handler,
    tm_umts_cp_gsm_sess_info_handler,
    tm_umts_cp_gsm_timer_cb_handler,
    tm_umts_cp_gsm_event_cb_handler,
    tm_umts_cp_gsm_general_query_handler
  }
  #endif
};
#else

#ifdef FEATURE_CGPS_UMTS_CP_GSM 
static umts_cp_function_table_s_type   z_umts_cp_fcn_table[]
= 
{
  { tm_umts_cp_gsm_start_sess_req_handler,
    tm_umts_cp_gsm_stop_sess_req_handler,
    tm_umts_cp_gsm_sess_req_data_handler,
    tm_umts_cp_gsm_sess_info_handler,
    tm_umts_cp_gsm_timer_cb_handler,
    tm_umts_cp_gsm_event_cb_handler,
    tm_umts_cp_gsm_general_query_handler
  }
};
#endif

#endif  



/* Set default to map to the first RAT interface. */
static const umts_cp_function_table_s_type *p_umts_cp_fcn_table = &z_umts_cp_fcn_table[0];


/* a structure that contains pointer to API functions UMTS-CP provides to TmCore,
   for handling TmCore messages to UMTS-CP  */
tm_prtl_cb_s_type     tm_umts_cp_prtl_iface_table;  



/* =======================================================================
**           Local Function Prototypes       
** ======================================================================= */

static void tm_umts_cp_init_prtl_iface_table(void);


static prtl_start_sess_req_result_e_type tm_umts_cp_start_sess_req_handler
(
  tm_sess_handle_type               sess_handle,
  prtl_start_sess_req_param_s_type  *start_sess_req_param,
  tm_sess_req_start_param_s_type    *actions
);

static boolean tm_umts_cp_stop_sess_req_handler
(
  tm_sess_handle_type           sess_handle,
  prtl_sess_stop_type           stop_type,  
  tm_sess_stop_reason_e_type    stop_reason
);

static boolean tm_umts_cp_sess_req_data_handler
(
  tm_sess_handle_type  session_handle,
  prtl_data_req_type   req_data
);

static boolean tm_umts_cp_sess_info_handler
(
  tm_sess_handle_type          session_handle,
  prtl_sess_info_e_type        sess_info_type,
  prtl_sess_info_param_u_type  *p_sess_info_param
);

static void tm_umts_cp_timer_cb_handler ( void *timer_data );

static boolean tm_umts_cp_event_cb_handler
(
  prtl_event_e_type  event_type,
  prtl_event_u_type  *p_event_payload
);

static boolean tm_umts_cp_general_query_handler
( prtl_query_e_type query_type, 
  prtl_ans_u_type *answer_ptr 
);



/* =======================================================================
**                            Function Definitions
** ======================================================================= */


/*===========================================================================

FUNCTION tm_umts_cp_prtl_iface_init

DESCRIPTION
  This function is used by TmCore which calls it when TM task starts, 
  initiating the UMTS_CP submodule.
  
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/

void tm_umts_cp_prtl_iface_init(void)
{

  MSG_MED("tm_umts_cp_init", 0, 0, 0);
  
  /* init. TmCore's api functions */
  tm_umts_cp_init_prtl_iface_table(); 
  
  /* register umts_cp with tmCore */
  (void) tm_prtl_reg (TM_PRTL_TYPE_UMTS_CP, &tm_umts_cp_prtl_iface_table);  


  /* read in umts common NV items: if UMTS_UP is defined, it has been done by tm_umts_up */
  #ifndef FEATURE_CGPS_UMTS_UP
  tm_umts_common_utils_read_nv();
  #endif

  /* register UMTS-CP with TmCore for CM */
  (void)tm_cm_iface_submodule_init(TM_PRTL_TYPE_UMTS_CP, NULL);

  /* also, use this opportunity to init. SS, WCDMA and GSM submodule */
  tm_umts_cp_ss_init();

  #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
  tm_umts_cp_wcdma_init();
  #endif

  #ifdef FEATURE_CGPS_UMTS_CP_GSM
  tm_umts_cp_gsm_init();
  #endif

} 



/*===========================================================================

FUNCTION tm_umts_cp_init_prtl_iface_table

DESCRIPTION
  This function initializes the UMTS_CP protocol interface table with pointers
  to the functions TmCore will call. 
  
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/

static void tm_umts_cp_init_prtl_iface_table(void)
{
  /** At this reset time, TM does not know which RAT the mobile will be 
      camped on. Initialize to WCDMA if both RATs supported or only
      WCDMA supported; Initialize to GSM only GSM is supported ***/
  tm_umts_cp_prtl_iface_table.start_sess_req_fp   = tm_umts_cp_start_sess_req_handler; 
  tm_umts_cp_prtl_iface_table.stop_sess_req_fp    = tm_umts_cp_stop_sess_req_handler;
  tm_umts_cp_prtl_iface_table.sess_req_data_fp    = tm_umts_cp_sess_req_data_handler;
  tm_umts_cp_prtl_iface_table.sess_info_fp        = tm_umts_cp_sess_info_handler; 
  #ifdef FEATURE_CGPS_UMTS_UP
  tm_umts_cp_prtl_iface_table.data_pipe_handle_fp = NULL; /* pipe is used by user plane only */ 
  #endif
  tm_umts_cp_prtl_iface_table.timer_cb_fp         = tm_umts_cp_timer_cb_handler;
  tm_umts_cp_prtl_iface_table.event_cb_fp         = tm_umts_cp_event_cb_handler;
  tm_umts_cp_prtl_iface_table.general_query_fp    = tm_umts_cp_general_query_handler;
  tm_umts_cp_prtl_iface_table.gm_event_handle_fp  = NULL;
}


/*===========================================================================

FUNCTION tm_umts_cp_request_molr

DESCRIPTION
  This function is provided to WCDMA and GSM submodules for them to request
  UMTS-CP to contact SS in order to send MOLR-Invoke . 
  
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/

boolean tm_umts_cp_request_molr
( prtl_start_sess_req_param_s_type *start_sess_req_param, 
  molr_type_e_type mo_lr_type,
  sm_InternalAssistDataStatusStructType   *p_assist_data_status 
)
{
  boolean ret_val; 

  ret_val = tm_umts_cp_ss_send_molr_invoke (start_sess_req_param, mo_lr_type, p_assist_data_status);

  return ret_val;

}

/*===========================================================================

FUNCTION tm_umts_cp_request_lcs_loc_notif_return_result

DESCRIPTION
  This function is provided to WCDMA and GSM submodules for them to request
  UMTS-CP to contact SS in order to send LCS Location Notification Return Result.  
  
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
boolean tm_umts_cp_request_lcs_loc_notif_return_result (const pdsm_lcs_notification_resp_s_type *p_user_resp)
{
  boolean ret_val;

  ret_val = tm_umts_ss_send_lcs_loc_notif_return_result( p_user_resp );

  return ret_val;
}

/*===========================================================================

FUNCTION tm_umts_cp_submodule_end_session

DESCRIPTION
  This function is provided to WCDMA and GSM submodules for them to inform
  UMTS-CP the end of the current session.  
  
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
void tm_umts_cp_submodule_end_session (void)
{
  tm_umts_cp_ss_ss_completion_req();  
  tm_umts_cp_ss_session_init();
}




/*===========================================================================

FUNCTION tm_umts_cp_start_sess_req_handler

DESCRIPTION
  This function handles the start session request from TM-core. 
  
DEPENDENCIES: none.

RETURN VALUE: prtl_start_sess_req_result_e_type.

SIDE EFFECTS: none.

===========================================================================*/

static prtl_start_sess_req_result_e_type tm_umts_cp_start_sess_req_handler
(
  tm_sess_handle_type               sess_handle,
  prtl_start_sess_req_param_s_type  *start_sess_req_param,
  tm_sess_req_start_param_s_type    *actions
)
{
  emergency_handling_e_type e_e911_effect;

  /* disallow if NV item has LBS locked */
  if ( ( z_umts_nv_item_ram_copy.gps_lock == PDSM_GPS_LOCK_ALL ) ||
       ( z_umts_nv_item_ram_copy.gps_lock == PDSM_GPS_LOCK_MI )
     )
  {
    return PRTL_START_SESS_REQ_NOT_ALLOWED;
  }

  e_e911_effect = tm_umts_common_utils_emergency_effect();
  if ( ( e_e911_effect == C_EMERGENCY_HANDLING_REJ_ALL ) ||
       ( e_e911_effect == C_EMERGENCY_HANDLING_ACCEPT_ONLY_HIGH_PRIORITY )
     )
  {
    return PRTL_START_SESS_REQ_NOT_ALLOWED;
  }
  if (start_sess_req_param->pd_option.session == PDSM_PD_SESS_TYPE_TRACK_IND)
  {
    start_sess_req_param->pd_option.session = PDSM_PD_SESS_TYPE_NEW;
  }

  return ( p_umts_cp_fcn_table->start_sess_req_handler ( sess_handle, start_sess_req_param, actions ) ); 
}


/*===========================================================================

FUNCTION tm_umts_cp_stop_sess_req_handler

DESCRIPTION
  This function handles the stop session request from TM-core. 
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/

static boolean tm_umts_cp_stop_sess_req_handler
(
  tm_sess_handle_type           sess_handle,
  prtl_sess_stop_type           stop_type,  
  tm_sess_stop_reason_e_type    stop_reason
)
{
  return ( p_umts_cp_fcn_table->stop_sess_req_handler (sess_handle, stop_type, stop_reason ) );
}



/*===========================================================================

FUNCTION tm_umts_cp_sess_req_data_handler

DESCRIPTION
  This function handles the session data request from TM-core. 
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/

static boolean tm_umts_cp_sess_req_data_handler
(
  tm_sess_handle_type  session_handle,
  prtl_data_req_type   req_data
)
{
  return ( p_umts_cp_fcn_table->sess_req_data_handler (session_handle, req_data) );
}


/*===========================================================================

FUNCTION tm_umts_cp_sess_info_handler

DESCRIPTION
  This function handles the session info. request from TM-core. 
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/

static boolean tm_umts_cp_sess_info_handler
(
  tm_sess_handle_type          session_handle,
  prtl_sess_info_e_type        sess_info_type,
  prtl_sess_info_param_u_type  *p_sess_info_param
)
{

  if ( sess_info_type == PRTL_SESS_INFO_STATUS )
  {
    tm_umts_common_assist_wishlist_build( &p_sess_info_param->aiding_data_status.z_assist_data_status );
  }                                                    

  return ( p_umts_cp_fcn_table->sess_info_handler ( session_handle, sess_info_type, p_sess_info_param ) );
}



/*===========================================================================

FUNCTION tm_umts_cp_timer_cb_handler

DESCRIPTION
  This function handles the session data pipe from TM-core. 
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/

static void tm_umts_cp_timer_cb_handler ( void *timer_data )
{
  p_umts_cp_fcn_table->timer_cb_handler ( timer_data );
}


/*===========================================================================

FUNCTION tm_umts_cp_event_cb_handler

DESCRIPTION
  This function handles the session data pipe from TM-core. 
  
DEPENDENCIES: none.

RETURN VALUE: 
	TRUE:  If the UMTS CP event is handled successfully
	FALSE: If the UMTS CP event is not handled

SIDE EFFECTS: none.

===========================================================================*/

static boolean tm_umts_cp_event_cb_handler
(
  prtl_event_e_type  event_type,
  prtl_event_u_type  *p_event_payload
)
{
  int                         tbl_ndx = 0;
  tm_cm_phone_state_info_s_type phone_state_info;

  if (NULL == p_event_payload)
  {
     MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL Pointer passed to tm_umts_cp_event_cb_handler");
     return FALSE;
  }  
  
  if ( ( event_type != PRTL_EVENT_TYPE_PHONE ) && 
       ( event_type != PRTL_EVENT_TYPE_RRC ) &&
       ( event_type != PRTL_EVENT_TYPE_RR ) &&
       ( event_type != PRTL_EVENT_TYPE_MM ) &&
       ( event_type != PRTL_EVENT_TYPE_LCS ) &&
       ( event_type != PRTL_EVENT_TYPE_PA )
     )
  {
    /* event unknown to UMTS_CP, don't process it */
    MSG_HIGH("event_type %u not supported", event_type, 0, 0 );
    return FALSE;
  }

  tm_cm_iface_get_phone_state(&phone_state_info);
  if (phone_state_info.srv_system == CGPS_SRV_SYS_GSM)
  {
    #if (defined(FEATURE_CGPS_UMTS_CP_GSM) && defined (FEATURE_CGPS_UMTS_CP_WCDMA))
      tbl_ndx = 1;
	#endif	   
  }
  /* If WCDMA, then the tbl_ndx = 0, which is already initialized */
  
  p_umts_cp_fcn_table = &z_umts_cp_fcn_table[tbl_ndx];
  	
  if ( event_type == PRTL_EVENT_TYPE_PHONE )
  {
    if ((p_event_payload->phone_event_payload.event == TM_PHONE_EVENT_SS_SRV_STATUS) ||
		(p_event_payload->phone_event_payload.event == TM_PHONE_EVENT_IN_UMTS_COVERAGE))
    {
      if ( p_event_payload->phone_event_payload.payload.ss_info.sys_mode == SYS_SYS_MODE_WCDMA )
      {
        #ifdef FEATURE_CGPS_UMTS_CP_WCDMA /* as long as WCDMA is included, use fcn_table[0] */ 
        p_umts_cp_fcn_table = &z_umts_cp_fcn_table[0]; 
        #endif
      }
      else if ( p_event_payload->phone_event_payload.payload.ss_info.sys_mode == SYS_SYS_MODE_GSM )
      {
        #if defined ( FEATURE_CGPS_UMTS_CP_WCDMA ) && defined ( FEATURE_CGPS_UMTS_CP_GSM )
        /* if both WCDMA and GSM are included, use fcn_table[1] */  
        p_umts_cp_fcn_table = &z_umts_cp_fcn_table[1]; 
        #else
        #if defined ( FEATURE_CGPS_UMTS_CP_GSM )
        /* if only GSM are included, use fcn_table[0] */  
        p_umts_cp_fcn_table = &z_umts_cp_fcn_table[0]; 
        #endif
        #endif
      }
    }
    else if( (p_event_payload->phone_event_payload.event == 
                                          TM_PHONE_EVENT_E911_CALL_ORIG) ||
             (p_event_payload->phone_event_payload.event == 
                                          TM_PHONE_EVENT_E911_CALL_END) )
    {
      #ifdef FEATURE_CGPS_UMTS_CP_GSM
        #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
          tbl_ndx = 1;
        #else
          tbl_ndx = 0;
        #endif

        /*
         *  IF camped to GSM with full service THEN
         *    forward this event to the GSM protocol module
         *  ENDIF
         */

        tm_cm_iface_get_phone_state(&phone_state_info);
        if (phone_state_info.srv_system == CGPS_SRV_SYS_GSM)
        {
          (void) z_umts_cp_fcn_table[tbl_ndx].event_cb_handler(  event_type,
                                                          p_event_payload );
		  return TRUE;
        }
		
      #endif
    }	
  }

  else if ( event_type == PRTL_EVENT_TYPE_MM )
  {
    if ( p_event_payload->mm_event_payload.mm_event_data_ptr->e_event_type == 
         MM_CGPS_MSG_EVENT_RESET_UE_POS_INFO )
    {

      if(!tm_core_session_active())
      {

        tm_sess_req_param_u_type   req_param;

        /* OTA delete all command */

        req_param.start_param.op_req = TM_OP_REQ_DEL_ASSIST;

        /* irrelevant, but populate them anyway */
        req_param.start_param.op_mode = TM_SESS_OPERATION_MODE_MSB; 
        req_param.start_param.lr_qos = TM_MAX_SESSION_TIMEOUT;
        req_param.start_param.accuracy_threshold = 0;
        req_param.start_param.abort_curr_sess = FALSE;
        req_param.start_param.num_fixes = 1;
        req_param.start_param.tbf_ms = 1000;
        req_param.start_param.dynamic_qos_enabled = TRUE;

        (void) tm_sess_req ( TM_PRTL_TYPE_UMTS_CP,
                             TM_SESS_HANDLE_UMTS_CP_NI,
                             TM_SESS_REQ_START, 
                             &req_param
                            );

        req_param.stop_param.stop_type = TM_STOP_TYPE_SESSION;
        req_param.stop_param.stop_reason = TM_STOP_REASON_COMPLETED; 
        (void) tm_sess_req ( TM_PRTL_TYPE_UMTS_CP,
                             TM_SESS_HANDLE_UMTS_CP_NI,
                             TM_SESS_REQ_STOP, 
                             &req_param
                           ); 

        MSG_MED("OTA delete assist. processed", 0, 0, 0);

        #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
        tm_umts_cp_wcdma_clear_all_holdoff();
        #endif
      }   /*TM CORE Session is not active*/
      else
  {
        tm_sess_req_param_u_type   req_param;

        /* OTA delete all command */

        req_param.start_param.op_req = TM_OP_REQ_DEL_ASSIST;

        /* irrelevant, but populate them anyway */
        req_param.start_param.op_mode = TM_SESS_OPERATION_MODE_MSB; 
        req_param.start_param.lr_qos = TM_MAX_SESSION_TIMEOUT;
        req_param.start_param.accuracy_threshold = 0;
        req_param.start_param.abort_curr_sess = FALSE;
        req_param.start_param.num_fixes = 1;
        req_param.start_param.tbf_ms = 1000;
        req_param.start_param.dynamic_qos_enabled = TRUE;

        (void) tm_sess_req ( TM_PRTL_TYPE_UMTS_CP,
                             TM_SESS_HANDLE_UMTS_CP_NI,
                             TM_SESS_REQ_CONTINUE, 
                             &req_param
                            );
        
        MSG_MED("OTA delete assist. processed while in session", 0, 0, 0);

    #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
    tm_umts_cp_wcdma_clear_all_holdoff();
    #endif
      }
	  
    return TRUE;

    }

  }
  
  else if ( event_type == PRTL_EVENT_TYPE_PA )
  {
    if (p_event_payload->pa_event_payload.pa_event == PDSM_PA_MT_LR_SUPPORT)
    {
      /* for UMTS CP use only */
      z_umts_nv_item_ram_copy.mt_lr_support = 
        p_event_payload->pa_event_payload.pa_info_ptr->mt_lr_support;
    }
    else if (p_event_payload->pa_event_payload.pa_event == PDSM_PA_POSITION_MODE)
    {
      /* for UMTS CP and UP, it is also updated in the UP event handler 
         in case only not both CP and UP are included in the build */         
      TM_CORE_CONFIG_DD(positioning_modes_supported) = 
        p_event_payload->pa_event_payload.pa_info_ptr->position_mode;
    }
    else if (p_event_payload->pa_event_payload.pa_event == PDSM_PA_CALL_FLOW_SELECT)
    {
      (p_event_payload->pa_event_payload.pa_info_ptr->call_flow_select_mask & PDSM_PA_CALL_FLOW_SELECT_2G) ?
      (z_umts_nv_item_ram_copy.mo_lr_2g_support &= ~NV_AAGPS_2G_MO_LR_ASSIST_DATA ) :
      (z_umts_nv_item_ram_copy.mo_lr_2g_support |= NV_AAGPS_2G_MO_LR_ASSIST_DATA);

      (p_event_payload->pa_event_payload.pa_info_ptr->call_flow_select_mask & PDSM_PA_CALL_FLOW_SELECT_3G) ?
      (z_umts_nv_item_ram_copy.mo_lr_support &= ~NV_AAGPS_3G_MO_LR_ASSIST_DATA ) :
      (z_umts_nv_item_ram_copy.mo_lr_support |= NV_AAGPS_3G_MO_LR_ASSIST_DATA);
    }
  } /* PRTL_EVENT_TYPE_PA */

  if ( p_umts_cp_fcn_table == NULL )
  {
    MSG_ERROR("fcn_table NULL", 0, 0, 0);
    return FALSE;
  }
 
  return ( p_umts_cp_fcn_table->event_cb_handler (event_type, p_event_payload) );

}


/*===========================================================================

FUNCTION tm_umts_cp_general_query_handler

DESCRIPTION
  This function handles the start session request from TM-core. 
  
DEPENDENCIES: none.

RETURN VALUE: TRUE/FALSE.

SIDE EFFECTS: none.

===========================================================================*/
static boolean tm_umts_cp_general_query_handler
( prtl_query_e_type query_type, 
  prtl_ans_u_type   *answer_ptr 
)
{
  return ( p_umts_cp_fcn_table->general_query_handler ( query_type, answer_ptr ) ); 
}




#endif /* ifdef FEATURE_CGPS_UMTS_CP */



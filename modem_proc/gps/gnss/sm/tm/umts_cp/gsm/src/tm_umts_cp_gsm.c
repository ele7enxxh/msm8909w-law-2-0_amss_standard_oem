/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TM-UMTS-CP-GSM sub-module 

GENERAL DESCRIPTION
  This file implements TM UMTS-CP GSM sub-module.
   
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 
  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_cp/gsm/src/tm_umts_cp_gsm.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  01/12/16   mc      Notify NAS when RRLP session starts
  08/26/15   gk      90% conf reporting changes
  08/05/15   rh      Moved carrier-specific NV 1920 (pos mode support) to TM core 
  07/31/15   mj      Changes to fix E911 on second SUB in G+G config
  06/09/15   mj      Add support for W+W feature
  04/02/15   mj      Handle GSM RR events with AS_ID
  09/11/14   mj      Introduce function to cache asid from RR events
  12/06/13   ah      Support for non-deferrable timers
  08/29/09   atien   Add tm_umts_cp_gsm_stop_prm()
  05/7/09    gk      Segemented APU processing fixes
  04/21/09   gk      Check if we are in GSM before starting 911 processing
  09/19/08   LT      Buffering ACQ-assist. for injecting at right time.
  05/03/07   LT      Initial version

============================================================================*/

#include "gps_variation.h"
#include "customer.h"

#ifdef FEATURE_CGPS_UMTS_CP_GSM

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "msg.h"
#include <limits.h>
#include "sm_nv.h"
#include "tm_data.h"
#include "tm_common.h"
#include "tm_umts_common_utils.h"
#include "tm_umts_cp_ss.h"
#include "tm_umts_cp_gsm_rrlp.h"
#include "tm_umts_cp_submodule_iface.h"
#include "math.h"
#include "rr_lsm_if.h"
#include "ms.h"
#include "gs.h"
#include "mm_lsm.h"
#include "pdapibuf.h"
#include "sm_log.h"
#include "sm_dm_event.h"

#define UMTS_CP_GSM_MAIN_VARIABLES_DEFINED
#include "tm_umts_cp_gsm.h"
#undef UMTS_CP_GSM_MAIN_VARIABLES_DEFINED  

#include "sm_dm_event.h"


/* =======================================================================
**           Local Data Types, Macros, Constants, etc.
** ======================================================================= */
#define MAX_APDU_MSG      1024
#define APDU_ID_RRLP      0

#define APDU_LAST_FLAG                  0x1
#define APDU_FIRST_FLAG                 0x2

typedef enum
{
  APDU_STATE__IDLE,
  APDU_STATE__IN_PROGRESS
} apdu_state_e_type;

typedef struct
{
  apdu_state_e_type   state;
  uint16              length;
  uint16              ndx;
  byte                rx_buffer[ MAX_APDU_MSG ];
} apdu_state_s_type;


#ifdef MSG_LOW
#undef MSG_LOW
#endif
#define MSG_LOW(s,p1,p2,p3)   MSG_MED(s,p1,p2,p3)



/* =======================================================================
**           Local Variables
** ======================================================================= */
static apdu_state_s_type      apdu;
static rr_apdu_req_T          rr_apdu_req;
static pdsm_pd_info_s_type    z_umts_cp_gsm_pd_info_buffer;



/* =======================================================================
**           Local Function Prototypes       
** ======================================================================= */

static void tm_umts_cp_gsm_notif_verif_resp_proc
(
  const pdsm_lcs_notification_resp_s_type *p_user_resp 
);
static void continue_session( 
  boolean need_wishlist 
);



/* =======================================================================
**                            Function Definitions
** ======================================================================= */


/*===========================================================================

FUNCTION tm_umts_cp_gsm_module_init

DESCRIPTION
  This function initializes the UMTS_CP_GSM module
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_module_init
( 
  void 
)
{
  /*
   *  Register with RR for events
   */
  tm_umts_cp_gsm_session_init();

  z_umts_cp_gsm_data.p_gsm_timer = os_TimerCreateInCgpsTimerGroup(TM_UMTS_CP_GSM_CF_TIMER_ID,
                                                                  (int) THREAD_ID_SM_TM);
  if( z_umts_cp_gsm_data.p_gsm_timer == NULL )
  {
    MSG_ERROR( "CP-GSM: Unable to create timer", 0, 0, 0 );
  }
  
  z_umts_cp_gsm_data.p_max_mgp_on_timer = os_TimerCreateInCgpsTimerGroup(TM_UMTS_CP_GSM_CF_MAX_MGP_ON_TIMER_ID,
                                                                         (int) THREAD_ID_SM_TM);
  if( z_umts_cp_gsm_data.p_max_mgp_on_timer == NULL )
  {
    MSG_ERROR( "CP-GSM: Unable to create E911 timer", 0, 0, 0 );
  }
  
  z_umts_cp_gsm_data.p_acc_reset_timer = os_TimerCreateInCgpsTimerGroup(TM_UMTS_CP_GSM_ACC_RESET_TIMER_ID,
                                                                        (int) THREAD_ID_SM_TM);
  if( z_umts_cp_gsm_data.p_acc_reset_timer == NULL )
  {
    MSG_ERROR( "CP-GSM: Unable to AccuracyReset timer", 0, 0, 0 );
  }
  
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_session_init

DESCRIPTION
  This function initializes UMTS_CP_GSM session related variables
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_session_init
(
  void
)
{
  tm_network_assist_s_type    *p_net_assist;
  rrlp_params_s_type          *p_rrlp_params;

  MSG_MED("GSM-CP: session_init", 0, 0, 0);

  /*
   *  Zero out all session parameters to begin with.  Some parameters
   *  may then need to be initialized to non-zero values.  This will
   *  be done below.
   */
  memset( &z_umts_cp_gsm_session_status,
          0,
          sizeof( z_umts_cp_gsm_session_status ) );

  /*
   *  Zero out the APDU buffer that is used to reassemble segmented
   *  APDU's
   */
  memset( &apdu, 0, sizeof(apdu) );

  /*
   *  Set the RefNum to -1 to denote the new session.  This is used 
   *  during RRLP decoding to determine if newly arrived data is part
   *  of an existing session or note.
   */
  p_net_assist  = &z_umts_cp_gsm_session_status.z_network_assist;
  p_rrlp_params = &p_net_assist->radio_link_prot_params.z_rrlp_params;

  p_rrlp_params->b_RefNum = -1;

  /*
   *  Stop any ongoing timer
   */
  tm_umts_cp_gsm_cancel_cf_timer();
  tm_umts_cp_gsm_cancel_max_mgp_on_timer();
  tm_umts_cp_gsm_cancel_acc_reset_timer();
  
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_pdapi_mapping

DESCRIPTION
  This function interprets PDAPI provided session parameters for
  the GSM CP protocol module. 
  
DEPENDENCIES: none

RETURN VALUE: TRUE if successful; else FALSE

SIDE EFFECTS: none

===========================================================================*/

boolean tm_umts_cp_gsm_pdapi_mapping
(
  const prtl_start_sess_req_param_s_type *start_sess_req_param
)
{
  pdsm_pd_session_operation_e_type          operation_mode;
  umts_cp_gsm_session_agps_mode_enum_type   e_agps_mode;

  /*
   *  Initialize some local variables to make the remaining code
   *  in this function a bit easier to read.  This will reduce some of the
   *  really long lines that would exist had this not been done.
   */
  operation_mode  = start_sess_req_param->pd_option.operation_mode;
  e_agps_mode     = C_UMTS_CP_GSM_AGPS_MODE_UE_BASED;
 
  /*
   *  Initialize the session's AGPS Mode based upon the mode requested
   *  via PDAPI.
   */ 
  if ( ( operation_mode == PDSM_SESSION_OPERATION_MSBASED ) ||
       ( operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_SPEED )
     )
  {
    if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_2G_CP_MS_BASED) )
    {
      e_agps_mode = C_UMTS_CP_GSM_AGPS_MODE_UE_BASED;
    }
    else if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_STANDALONE) )
    {
      e_agps_mode = C_UMTS_CP_GSM_AGPS_MODE_CONV;
    }
    else
    {
      return FALSE;
    }
  }
  else if ( ( operation_mode == PDSM_SESSION_OPERATION_MSASSISTED ) ||
            ( operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_ACCURACY )
          )
  {
    if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_2G_CP_MS_ASSISTED) )
    {
      e_agps_mode = C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED;
    }
    else
    {
      return FALSE;
    }
  }
  else if  ( operation_mode == PDSM_SESSION_OPERATION_REF_POSITION )
  {
    if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_2G_CP_MS_BASED) )
    {
      e_agps_mode = C_UMTS_CP_GSM_AGPS_MODE_UE_BASED;
      z_umts_cp_gsm_session_status.u_force_ref_loc = TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  else if ( ( operation_mode == PDSM_SESSION_OPERATION_STANDALONE_ONLY ) ||
            ( operation_mode == PDSM_SESSION_OPERATION_OPTIMAL_DATA )
          )
  {
    /* note standalone_only should not reach here as it is handled by TM core and is not passed down to 
       protocol modules; but in case it gets here, handle it gracefully */
    if ( TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_STANDALONE) )
    {
      e_agps_mode = C_UMTS_CP_GSM_AGPS_MODE_CONV;
    }
    else
    {
      return FALSE;
    }
  }

  else 
  {
    MSG_ERROR("CP-GSM: Invalid PDAPI operation mode", 0, 0, 0);
    return FALSE;
  }

  /*
   *  Set the AGPS mode
   */
  z_umts_cp_gsm_session_status.e_agps_mode = e_agps_mode;
  
  /*
   * Initialize remaining session variables
   */

  z_umts_cp_gsm_session_status.q_pdapi_client_id = 
                            start_sess_req_param->client_id;
  z_umts_cp_gsm_session_status.z_session_qos.u_gps_session_timeout_value_secs
                          = start_sess_req_param->pd_qos.gps_session_timeout; 
  z_umts_cp_gsm_session_status.z_session_qos.q_hor_acc_meters = 
                            start_sess_req_param->pd_qos.accuracy_threshold;

  return TRUE;
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_inform_core_nt_assist_end

DESCRIPTION
  This function informs the TM core that network communication is over. 
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_inform_core_nt_assist_end
(
  void
)
{
  MSG_MED("CP-GSM: inform_core_nt_assist_end", 0, 0, 0);

  /*
   *  IF a FIX has already been reported during this session THEN
   *    just return.  Don't send a continue or change the state
   *  ENDIF
   */
  if ( z_umts_cp_gsm_session_status.u_pdapi_fix_reported )
    return;

  MSG_MED("CP-GSM: Sv Alm/Eph Info for MGP; handle=%d, alm=0x%lx; eph=0x%lx", 
          z_umts_cp_gsm_session_status.tm_active_session_handle,
          z_umts_cp_gsm_session_status.q_alm_sv_list,
          z_umts_cp_gsm_session_status.q_eph_sv_list );

  /*
   *  Provide information about missing SVs to MGP
   *  NOTE - If no alm or eph information has been received, then
   *  the SV list(s) will be 0.  The tm_util_post_non_exist_svs()
   *  function will ignore the lists if the number of SVs in the almanac
   *  list is < some threshold (currently 24).  It is ok to call this
   *  function if no data has been received.  The only case where it it
   *  not OK to call this is if data has been partially received.  This 
   *  would only happen in the case of some failure condition.  Failure
   *  conditions should be handled by forcing these values to 0 before
   *  calling this routine.  
   */
  tm_util_post_non_exist_svs(   
                      TM_PRTL_TYPE_UMTS_CP,
                      z_umts_cp_gsm_session_status.tm_active_session_handle,
                      z_umts_cp_gsm_session_status.q_alm_sv_list,
                      z_umts_cp_gsm_session_status.q_eph_sv_list );

  /*
   *  Send a session continue command to TM-Core
   */
  continue_session( FALSE );

  /*
   *  IF this is an MO-Assisted Call THEN
   *    set the state to TIMER_RRLP_MEAS_RESP_TIME because we are
   *      waiting for measurements
   *  ENDIF
   */
  if( z_umts_cp_gsm_session_status.e_agps_mode ==
                                      C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED )
  {
    tm_umts_cp_gsm_set_cf_state( 
              C_UMTS_CP_GSM_CF_STATE_TIMER_RRLP_MEAS_RESP_TIME );
  }

  /*
   *  When this function is called, it is assumed that the UE has
   *  received all of the assistance data that it will receive.  Based
   *  on the alm and eph SVs, the protocol module can derive which SVs
   *  don't exist and inform the MGP.
   */
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_inform_core_nt_assist_failure

DESCRIPTION
  This function informs the TM core that network communication is over,
  (just like tm_umts_cp_gsm_inform_core_nt_assist_end(), but does so in
  a way that is proper when there has been some sort of network communications
  failure which may leave the assistance data in a "partially received" state.
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

static void tm_umts_cp_gsm_inform_core_nt_assist_failure
(
  void
)
{
  MSG_MED("GSM-CP: nt_assist_failure", 0, 0, 0);

  /*  
   *  Force the Alm and Eph SV lists to 0 so that MGP won't get informed
   *  (possibly incorrectly) about missing SVs.  This is a conservative
   *  approach.
   */
  z_umts_cp_gsm_session_status.q_alm_sv_list = 0;
  z_umts_cp_gsm_session_status.q_eph_sv_list = 0;

  /*
   *  Call the usual "assist_end" handling function
   */
  tm_umts_cp_gsm_inform_core_nt_assist_end();
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_cancel_cf_timer

DESCRIPTION
  This function cancels the callflow timer
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/
void tm_umts_cp_gsm_cancel_cf_timer( void )
{
  (void) os_TimerStop( z_umts_cp_gsm_data.p_gsm_timer );
  z_umts_cp_gsm_session_status.e_cf_state = C_UMTS_CP_GSM_CF_STATE_NONE;
  MSG_MED("CP-GSM: Cancel cf timer", 0, 0, 0 );
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_start_max_mgp_on_timer

DESCRIPTION
  This function starts the max_MGP_ON timer
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/
void tm_umts_cp_gsm_start_max_mgp_on_timer( uint32  q_time_ms )
{
  (void) os_TimerStart( z_umts_cp_gsm_data.p_max_mgp_on_timer, q_time_ms, NULL );
  
  MSG_MED("CP-GSM: Start max. MGP_ON timer; t=%d", q_time_ms, 0, 0);
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_cancel_max_mgp_on_timer

DESCRIPTION
  This function cancels the max. MGP_ON timer
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/
void tm_umts_cp_gsm_cancel_max_mgp_on_timer( void )
{
  (void) os_TimerStop( z_umts_cp_gsm_data.p_max_mgp_on_timer );
  
  MSG_MED("CP-GSM: Cancel max. MGP_ON timer", 0, 0, 0 );
}

 
/*===========================================================================

FUNCTION tm_umts_cp_gsm_start_acc_reset_timer

DESCRIPTION
  This function starts the AccuracyReset timer
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/
void tm_umts_cp_gsm_start_acc_reset_timer( uint32  q_time_ms )
{
  (void) os_TimerStart( z_umts_cp_gsm_data.p_acc_reset_timer, q_time_ms, NULL );
  
  MSG_MED("CP-GSM: Start AccuracyReset timer; t=%d", q_time_ms, 0, 0);
}

/*===========================================================================

FUNCTION tm_umts_cp_gsm_cancel_acc_reset_timer

DESCRIPTION
  This function cancels AccuracyReset timer
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/
void tm_umts_cp_gsm_cancel_acc_reset_timer( void )
{
  (void) os_TimerStop( z_umts_cp_gsm_data.p_acc_reset_timer );
  
  MSG_MED("CP-GSM: Cancel AccuracyReset timer", 0, 0, 0 );
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_set_cf_state

DESCRIPTION
  This function sets the callflow state and enables/disables the timer
  based on the new callflow state.
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_set_cf_state
(
  umts_cp_gsm_cf_state_enum_type e_new_cf_state
)
{
  uint32  q_time_ms = 0;
  
  tm_network_assist_s_type  *p_net_assist;
  rrlp_params_s_type        *p_rrlp_params;

  p_net_assist = &z_umts_cp_gsm_session_status.z_network_assist;
  p_rrlp_params = &p_net_assist->radio_link_prot_params.z_rrlp_params;
 
  /*
   *  Go ahead and always stop the callflow timer regardless of what state
   *  we are setting.  We may be going to the "idle" (ie NONE) state or
   *  we may even be staying in the current state.  This function is a 
   *  convenient place to stop the timer and set the state at the same time.
   */
  tm_umts_cp_gsm_cancel_cf_timer();

  if( (e_new_cf_state == C_UMTS_CP_GSM_CF_STATE_WAIT_RRLP ) ||
      (e_new_cf_state == C_UMTS_CP_GSM_CF_STATE_WAIT_RETURN_RESULT ) ||
      (e_new_cf_state == C_UMTS_CP_GSM_CF_STATE_TIMER_RRLP_MEAS_RESP_TIME ))
  {
    if( e_new_cf_state == C_UMTS_CP_GSM_CF_STATE_TIMER_RRLP_MEAS_RESP_TIME )
    {
      /* this is un-modified RRLP_respTime; MS will have to respond to the
         network when this time is up */
      q_time_ms = 
        p_rrlp_params->u_measure_response_time_sec * 1000;  
    } 
    else
    {
      q_time_ms = z_umts_nv_item_ram_copy.default_smlc_comm_timeout_sec * 1000;
    }
 
    (void) os_TimerStart( z_umts_cp_gsm_data.p_gsm_timer, q_time_ms, NULL );
  
    MSG_MED("CP-GSM: set_cf_state; cf state=%u, t=%d", 
            (uint8)e_new_cf_state, 
            q_time_ms, 
            0);
  }
  else
  {
    MSG_MED("CP-GSM: set_cf_state; cf state=%u", (uint8) e_new_cf_state, 0, 0);
  }
  
  z_umts_cp_gsm_session_status.e_cf_state = e_new_cf_state;

}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_callflow_disrupt_handler

DESCRIPTION
  This function properly handles call flow disruption. 
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_callflow_disrupt_handler( )
{
  MSG_MED("CP-GSM: callflow_disrupt_handler()", 0, 0, 0);
  
  z_umts_cp_gsm_session_status.u_network_session_on = FALSE;

  if ( (z_umts_cp_gsm_session_status.e_agps_mode == C_UMTS_CP_GSM_AGPS_MODE_UE_BASED ) &&
      ( z_umts_cp_gsm_session_status.e_callflow_kind == C_UMTS_CP_GSM_SESSION_UI ) 
    )
  {
    (void) tm_post_pd_event( TM_PRTL_TYPE_UMTS_CP,
                             z_umts_cp_gsm_session_status.tm_active_session_handle,
                             PDSM_PD_EVENT_UPDATE_FAILURE,
                             NULL,
                             NULL);

    /*
     *  Tell TmCore to proceed with the session without additional 
     *  assistance.
     */
    tm_umts_cp_gsm_inform_core_nt_assist_failure();
  } 
  else
  {
    tm_umts_cp_gsm_inform_core_stop_session(TM_STOP_REASON_GENERAL_ERROR);
  } 
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_lcs_molr_conf_proc

DESCRIPTION
  This function properly handles information in SS lcs_molr_conf 
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

static void tm_umts_cp_gsm_lcs_molr_conf_proc
(
  tm_cm_sups_data_s_type* p_sup_data 
)
{
  umts_cp_gsm_session_status_s_type   *p_session_status;
  lcs_molr_res_s_type                 *p_molr_res;
  pdsm_pd_info_s_type                 *p_pd_info_buffer;
  uint8                               *p_data;
  lcs_molr_res_data_T                 *p_lcs_molr_res_data;
  msa_pos_time_stamp_type             *p_msa_pos_time_stamp;
  uint32                              q_time_stamp_sec = 0;
  pdsm_pd_event_type                  q_pd_event;
  boolean                             u_pd_info_ok = FALSE;
  boolean                             u_ref_loc_ok = FALSE;
  tm_post_data_payload_type           z_tm_payload;
  gnss_NavSlnFlagsStructType          z_pos_flags;

  MSG_LOW("CP-GSM: lcs_molr_conf_proc", 0, 0, 0 );

  if (NULL == p_sup_data)
  {
	MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL Pointer sent to tm_umts_cp_gsm_lcs_molr_conf_proc");
	return;
  }

  /*
   *  Initialize pointers to make code easier read/write
   */
  p_session_status    = &z_umts_cp_gsm_session_status;
  p_molr_res          = &p_sup_data->lcs_mess.lcs_molr_res;
  p_pd_info_buffer    = &z_umts_cp_gsm_pd_info_buffer;
  p_data              = NULL;
  p_lcs_molr_res_data = &p_sup_data->lcs_mess.lcs_molr_res.data.molr_res_data;
  p_msa_pos_time_stamp= &p_session_status->z_msa_pos_time_stamp;

  memset(&z_tm_payload, 0x00, sizeof(z_tm_payload));
  memset(&q_pd_event,   0x00, sizeof(q_pd_event));
  memset(&z_pos_flags,  0x00, sizeof(z_pos_flags));
  /*
   *  Reject this event if in an MT callflow.
   */
  if( p_session_status->e_callflow_kind != C_UMTS_CP_GSM_SESSION_UI )
  {
    MSG_ERROR(  "CP-GSM: lcs_molr_conf rejected; k=%d", 
                (int) p_session_status->e_callflow_kind, 0, 0 );
    return; 
  }

  /*
   *  network session is over
   */
  z_umts_cp_gsm_session_status.u_network_session_on = FALSE;

  /*
   *  Terminate the callflow timer
   */
  tm_umts_cp_gsm_cancel_cf_timer();

  /*
   *  If this MOLR-CONF message contains position, then assume that
   *  this is an MS-Assisted callflow.  Provide the position to PDAPI
   *  and do whatever else is necessary when receiving MOLR-CONF in
   *  an MS-Assisted callflow.
   */
  if( p_molr_res->data_type == LCS_MOLR_INFO )
  {
    MSG_MED("CP-GSM: Received LCS_MOLR_INFO", 0, 0, 0);

    if( p_lcs_molr_res_data->location_estimate.present )
    {
      p_data = &p_lcs_molr_res_data->location_estimate.data[0];
    }
    else if( p_lcs_molr_res_data->add_location_estimate.present )
    {
      p_data = &p_lcs_molr_res_data->add_location_estimate.data[0];
    }

    if ( !p_session_status->u_pdapi_fix_reported )
    {
      tm_umts_cp_gsm_inform_core_nt_assist_end();  

      if( p_data != NULL )
      {
        MSG_MED("CP-GSM: MOLR_RES LocEst received", 0, 0, 0);

        memset( p_pd_info_buffer, 0, sizeof( pdsm_pd_info_s_type ) );

        q_time_stamp_sec = 0;
      
        if( p_msa_pos_time_stamp->u_pos_time_stamp_valid )
        {
          q_time_stamp_sec = p_msa_pos_time_stamp->q_pos_time_stamp;

          p_msa_pos_time_stamp->u_pos_time_stamp_valid = FALSE;
          p_msa_pos_time_stamp->u_ref_time_week_num_valid = FALSE;
        }
  
        /*
         *  Attempt to parse position information into a structure that
         *  can be used to POST position information to TM.  This will
         *  be used for 2 purposes:
         *    1) To provide EXTENDED PD data to PDAPI
         *    2) To provide SEED position to MGP
         */
        u_ref_loc_ok = 
            tm_umts_common_utils_gad_to_ref_loc(
                                      p_data,
                                      q_time_stamp_sec,
                                      &z_tm_payload.seed_data );
       
        /*
         *  Attempt to parse position information into a structure that
         *  can be used to POST a nosition event to PDAPI.
         */
        u_pd_info_ok = tm_umts_common_utils_gad_to_pdsm_pd_info
            (
                p_session_status->q_pdapi_client_id,
                p_data,
                PDSM_PD_POSITION_TYPE_NEW,
                PDSM_PD_POSITION_MODE_MSASSISTED,
                PDSM_PD_POSITION_SOURCE_GPS | PDSM_PD_POSITION_SOURCE_GLO,
                q_time_stamp_sec,
                p_pd_info_buffer,
                &q_pd_event 
            );
      } /* pos. included in returnResult */
    } /* if no pdapi position report is done */  

    /*
     *  IF parsing was successful THEN
     *    post data to TM
     *    post event to PDAPI
     */
    if( u_ref_loc_ok && u_pd_info_ok )
    {
      (void) tm_post_data( TM_PRTL_TYPE_UMTS_CP,
                    p_session_status->tm_active_session_handle,
                    TM_POST_DATA_TYPE_LR,
                    &z_tm_payload );
      
      if( q_pd_event & PDSM_PD_EVENT_HEIGHT )
      {
        z_pos_flags.b_Is3D = TRUE;
      }

      if( q_pd_event & PDSM_PD_EVENT_VELOCITY )
      {
        z_pos_flags.b_IsVelValid = TRUE;
      }
  
      (void) tm_post_pd_event( TM_PRTL_TYPE_UMTS_CP,
                               p_session_status->tm_active_session_handle,
                               q_pd_event,
                               p_pd_info_buffer,
                               &z_pos_flags);
    }
    else  /* No good position, don't use the position, or no position at all */
    {
      if(p_session_status->e_agps_mode == C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED)
      {
        tm_umts_cp_gsm_inform_core_stop_session( TM_STOP_REASON_GENERAL_ERROR );
      } 
      else /* msBased */
      {
        if ( p_session_status->u_pdapi_fix_reported )
        {
          tm_umts_cp_gsm_inform_core_stop_session(TM_STOP_REASON_COMPLETED);
        }
        else
        {
          /*
           *  Notify MM that the LCS session has completed
           */
          #ifdef FEATURE_CGPS_DUAL_WCDMA
          tm_umts_common_utils_mm_session_status_notify( FALSE, z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id );
		  #else
		  tm_umts_common_utils_mm_session_status_notify( FALSE );
		  #endif
  
          /*
           *  Notify RR that the LCS session has completed
           */
          tm_umts_common_utils_rr_session_status_notify( FALSE, z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id );
          
        } /* no position report to pdapi is done */
      } /* Not MsBased */
    } /* No usable position, don't use this position, or no position at all */
  } /* if this is LCS_MOLR_INFO */
  
  else /* If other than LCS_MOLR_INFO: could be SS_ERROR, CC_CAUSE, etc. */
  {
    if(p_session_status->e_agps_mode == C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED)
    {
      tm_umts_cp_gsm_inform_core_stop_session( TM_STOP_REASON_GENERAL_ERROR );
    } 
    else 
    {
      if ( p_session_status->u_pdapi_fix_reported )
      {
        tm_umts_cp_gsm_inform_core_stop_session(TM_STOP_REASON_COMPLETED);
      }
      else  
      {
        /*
         *  Notify MM that the LCS session has completed
         */
        #ifdef FEATURE_CGPS_DUAL_WCDMA
        tm_umts_common_utils_mm_session_status_notify( FALSE, z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id );
		#else
		tm_umts_common_utils_mm_session_status_notify( FALSE );
		#endif
    
        /*
         *  Notify RR that the LCS session has completed
         */
        tm_umts_common_utils_rr_session_status_notify( FALSE, z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id );
        
      } /* no pdapi report sent yet */ 
    } /* msBased */ 
  } /* If other than LCS_MOLR_INFO: could be SS_ERROR, CC_CAUSSE, etc. */
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_handle_lr_data

DESCRIPTION 
  Handles TmCore provided LR data.

DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_handle_lr_data
(
  sm_GnssFixRptStructType* lr_data_ptr
)
{
  pdsm_pd_info_s_type                       *p_pd_info_buffer;
  pdsm_pd_event_type                        pd_event = PDSM_PD_EVENT_POSITION;
  umts_cp_gsm_session_agps_mode_enum_type   e_agps_mode;
  gad_ellip_alt_unc_ellip_type              z_ellipse;
  const sm_GnssNavPosition                  *p_NavPos;
  tm_lr_resp_type                           *p_lr_resp;
  boolean                                   ok;
  tm_pd_position_info_s_type                z_tm_pd_data;
  tm_ext_pos_data_s_type                    z_tm_ext_pd_data;
  utc_time                                  z_utc_time;
  boolean                                   v_sess_type_ni = FALSE;

  MSG_LOW("CP-GSM: handle_lr_data() called",0,0,0);

  if ( z_umts_cp_gsm_session_status.u_pdapi_fix_reported )
  {
    MSG_MED("CP-GSM: pdapi fix already reported",0,0,0);

    /*
     *  If position has been passed on to pdapi via TmCore already 
     *  in this session, then don't send another
     */
    return;
  }

  if ( NULL == lr_data_ptr )
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null data buffer");
    return;
  }
  /*
   *  Initialize some local variables to make the code easier to read
   */
  pd_event          = PDSM_PD_EVENT_POSITION;
  e_agps_mode       = z_umts_cp_gsm_session_status.e_agps_mode;
  p_pd_info_buffer  = &z_umts_cp_gsm_pd_info_buffer;
  p_NavPos          = &lr_data_ptr->z_NavPos;
  if ( C_UMTS_CP_GSM_SESSION_NI == z_umts_cp_gsm_session_status.e_callflow_kind ) 
  {
    v_sess_type_ni =  TRUE;
  }
  /*
   *  Allocate a PDAPI buffer to be used to hold PDAPI position info
   */
  p_lr_resp = (tm_lr_resp_type *) pdsm_getbuf( sizeof(*p_lr_resp) );

  if( p_lr_resp != NULL )
  {
    /*
     *  Initialize buffer for pdapi
     */
    memset( p_pd_info_buffer, 0, sizeof( pdsm_pd_info_s_type ) );

    /*
     *  Convert from LM to IS801 format
     */
    ok = tm_util_convert_lr( lr_data_ptr, p_lr_resp );
    if( ok )
    {
      tm_util_xlate_nav_soln_to_tm_pos_info( NULL,
                                             &z_tm_pd_data,
                                             p_NavPos,
                                             PDSM_PD_DLOAD_LOCATION,         /* dont care */
                                             PDSM_PD_DLOAD_SOURCE_STANDALONE /* dont care */ );
      
      /*
       *  Set proper position mode in the PDAPI report 
       */
      switch( e_agps_mode )
      {
        case C_UMTS_CP_GSM_AGPS_MODE_UE_BASED:
          z_tm_pd_data.position_mode = PDSM_PD_POSITION_MODE_MSBASED;
          break;
  
        case C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED:
          z_tm_pd_data.position_mode = PDSM_PD_POSITION_MODE_MSASSISTED;
          break;

        case C_UMTS_CP_GSM_AGPS_MODE_CONV:
          z_tm_pd_data.position_mode = PDSM_PD_POSITION_MODE_STANDALONE;
          break;

        default:
        case C_UMTS_CP_GSM_AGPS_MODE_UNDEF:
          z_tm_pd_data.position_mode = PDSM_PD_POSITION_MODE_UNKNOWN;
          break;
      }
      tm_util_xlate_nav_soln_to_tm_ext_pos(&z_tm_ext_pd_data, p_NavPos);

      z_tm_ext_pd_data.h_dop = p_NavPos->f_HDOP;
      z_tm_ext_pd_data.v_dop = p_NavPos->f_VDOP;
      z_tm_ext_pd_data.p_dop = p_NavPos->f_PDOP;;
      z_tm_ext_pd_data.pos_hepe = 
         sqrt((lr_data_ptr->z_NavPos.f_ErrorEllipse[1] * 
               lr_data_ptr->z_NavPos.f_ErrorEllipse[1]) + 
              (lr_data_ptr->z_NavPos.f_ErrorEllipse[2] * 
               lr_data_ptr->z_NavPos.f_ErrorEllipse[2]));

      z_utc_time = tm_util_construct_utc_time(p_NavPos->w_GpsWeek,
                                              p_NavPos->q_GpsTimeMs,
                                              p_NavPos->q_UtcTimeMs);
      z_tm_ext_pd_data.utc_time.hour = z_utc_time.q_hours;
      z_tm_ext_pd_data.utc_time.minute = z_utc_time.q_mins;
      z_tm_ext_pd_data.utc_time.sec = (uint16)((z_utc_time.q_secs * 100) + (z_utc_time.q_tenths * 10));
      
      
      z_tm_ext_pd_data.utc_time.date = (uint32) ((z_utc_time.q_utc_days * 1000000) + (z_utc_time.q_month * 10000) + z_utc_time.q_year);

      pd_event = tm_util_pd_info_data_init( 
                                p_pd_info_buffer,
                                p_lr_resp,
                                z_umts_cp_gsm_session_status.q_pdapi_client_id,
                                NULL,
                                &z_tm_pd_data,
                                &z_tm_ext_pd_data );

      if( (z_umts_cp_gsm_session_status.u_network_session_on) &&
          (z_umts_cp_gsm_session_status.e_mo_lr_type == LOCATION_ESTIMATE) )
      {
        p_pd_info_buffer->pd_info.pd_data.ext_pos_data.pos_reported_to_network = TRUE;
      }

      (void) tm_post_pd_event( TM_PRTL_TYPE_UMTS_CP,
                               z_umts_cp_gsm_session_status.tm_active_session_handle,
                               pd_event,
                               &z_umts_cp_gsm_pd_info_buffer,
                               &lr_data_ptr->z_NavPos.z_PosFlags);

      sm_log_event_report( SM_LOG_EVENT_ID_GPS_COMPLETE, 0 );

      z_umts_cp_gsm_session_status.u_pdapi_fix_reported = TRUE; 
    }

    /*
     *  Be sure to free the PDSM buffer!!!!
     */
    (void) pdsm_freebuf( (char *) p_lr_resp );
  }

  /*******  check to see if reporting to the network is needed *************/

  if( z_umts_cp_gsm_session_status.u_final_mpr_rcvd )
  {
    MSG_MED("CP-GSM: send pos. to RRLP", 0, 0, 0);

    (void) tm_umts_common_utils_construct_ellip_alt_unc_ellip_from_lr ( 
                                               &z_ellipse, lr_data_ptr, TM_CONF_REPORTED_FROM_PE, v_sess_type_ni );
    tm_umts_cp_gsm_send_pos_rrlp( &z_ellipse, 
                                  rrlp_threeDFix, 
                                  lr_data_ptr->z_NavPos.q_GpsTimeMs);
                                  
    if ( z_umts_cp_gsm_data.u_e911_in_progress && 
         !z_umts_cp_gsm_data.u_e911_subsequent_rrlp_sess &&
          tm_umts_common_utils_cm_emergency_call_status())
    {
      /* If E911 is going on, from this point on, any subsequent RRLP 
         sessions will subject to the E911 NILR multiple session rules. */ 
      z_umts_cp_gsm_data.u_e911_subsequent_rrlp_sess = TRUE;
    }
                                    
    /* send DONE event to pdapi and stop the session */
    tm_umts_cp_gsm_inform_core_stop_session( TM_STOP_REASON_COMPLETED );
  }
  else if( z_umts_cp_gsm_session_status.e_callflow_kind == 
                                                C_UMTS_CP_GSM_SESSION_NI )
  {
    MSG_MED("CP-GSM: NI LR Received", 0, 0, 0);
    
    if ( lr_data_ptr->z_NavPos.z_PosFlags.b_Is3D )
    {
      /*
       *  No Measured Position Request has been received yet, but a network
       *  session is already ongoing.  Save the position and return it
       *  later if/when the network requests position.
       */
      MSG_MED("CP-GSM: Caching Position", 0, 0, 0);
    
      (void) tm_umts_common_utils_construct_ellip_alt_unc_ellip_from_lr ( 
                                                &z_ellipse, lr_data_ptr, TM_CONF_REPORTED_FROM_PE, v_sess_type_ni );

      z_umts_cp_gsm_session_status.z_pending_pos.e_fix_type = rrlp_threeDFix;

      z_umts_cp_gsm_session_status.z_pending_pos.q_time_stamp_msec = 
        lr_data_ptr->z_NavPos.q_GpsTimeMs % C_FOUR_HRS_MSEC;

      memscpy ( &z_umts_cp_gsm_session_status.z_pending_pos.z_pos,
                sizeof(z_umts_cp_gsm_session_status.z_pending_pos.z_pos),
               &z_ellipse, sizeof(z_ellipse) );

    }
  }
  else
  {
    MSG_MED("CP-GSM: UI LR Received", 0, 0, 0);

    /*
     *  IF running an MOLR "Assistance Data" callflow THEN
     *    This is the end of the session, if MS is not waiting for more
     *    RRLP-assistData;
     *  ELSE
     *    This may not be the end of the session.
     *    TBD work to be done later when this is supported
     *  ENDIF
     */
    if( z_umts_cp_gsm_session_status.e_mo_lr_type == ASSISTANCE_DATA )
    {
      if ( !z_umts_cp_gsm_session_status.u_network_session_on )
      {
        tm_umts_cp_gsm_inform_core_stop_session( TM_STOP_REASON_COMPLETED );
      }
    }
    else
    {
      /* TBD */
    }
  }
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_handle_sess_info_status

DESCRIPTION
  Handles the SESS_INFO_STATUS event which provides Wishlist data to
  the GSM CP protocol sub-module from MGP.

DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/
void    tm_umts_cp_gsm_handle_sess_info_status
(
  tm_aiding_data_wishlist_s_type  *p_wishlist
)
{
  sm_InternalAssistDataStatusStructType   *p_assist;
  uint32                          q_OriginalWishListMask;
  uint32                          q_WishListMask;

  p_assist = &p_wishlist->z_assist_data_status;
  q_OriginalWishListMask = p_assist->q_WishListMask;
  q_WishListMask = q_OriginalWishListMask;

  MSG_MED( "CP-GSM: handle_sess_info_status; WL=0x%08X; Rcvd=0x%08X", 
            q_OriginalWishListMask, 
            z_umts_cp_gsm_session_status.q_agps_data_rcvd, 
            0 );

  /*
   *  IF this is an MS-Assisted session THEN
   *    ignore the Wishlist from MGP (since it can't be relied upon)
   *    and just force it to be REF_TIME and ACQ_ASSIST if we haven't
   *    received them already.
   *  ELSE IF this is an E911 session THEN
   *    don't request Almanac, Iono, or UTC
   *  ENDIF
   */
  if( z_umts_cp_gsm_session_status.e_agps_mode == 
                                          C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED )
  {
    q_WishListMask = ( C_GPS_ASSIS_REFTIME | C_GPS_ASSIS_ACQ );
  }
  else if( z_umts_cp_gsm_data.u_e911_in_progress )
  {
    q_WishListMask &= C_GPS_ASSIS_NAVMODL | C_GPS_ASSIS_REFLOC | 
                      C_GPS_ASSIS_REFTIME;
  }

  q_WishListMask &= ~z_umts_cp_gsm_session_status.q_agps_data_rcvd;

  /*
   *  TM-Core forces RTI to be requested any time another quantity is
   *  requested.  If RTI is the only thing being requested, then set
   *  the WishListMask to 0.  We shouldn't ask for ONLY RTI.
   */
  if( q_WishListMask == C_GPS_ASSIS_RTI )
  {
    q_WishListMask = 0;
  }

  p_assist->q_WishListMask = q_WishListMask;

  if( q_WishListMask != q_OriginalWishListMask )
  {
    MSG_MED("CP-GSM: Override WL.  New WL=0x%08X; Old WL=0x%08X", 
            q_WishListMask, 
            q_OriginalWishListMask,
            0 );
  }

  /*
   *  IF there is at least 1 item in the WishList THEN
   *    IF this is an MO session and no MOLR has been sent yet THEN
   *      generate an MOLR request
   *    ELSE
   *      request assistance data with an MPR_ERR message now
   *    ENDIF
   *  ELSE
   *    Inform PDAPI that we are done with network comms
   *    Start the QoS timer
   *    IF the "u_mpr_rcvd" flag is set THEN
   *      set the "u_final_mpr_rcvd" flag
   *    ENDIF
   *  ENDIF
   */ 
  if ( q_WishListMask )
  {
    if( ( z_umts_cp_gsm_session_status.e_callflow_kind == 
                                              C_UMTS_CP_GSM_SESSION_UI ) &&
        ( !z_umts_cp_gsm_session_status.u_molr_invoke_sent ) )
    {
      MSG_MED("CP-GSM: Sending MOLR-Invoke; WL=0x%lx", q_WishListMask, 0, 0 );
      
      (void) tm_umts_cp_request_molr(  
                      &z_umts_cp_gsm_session_status.z_start_sess_req_param,
                      z_umts_cp_gsm_session_status.e_mo_lr_type,
                      p_assist );

      z_umts_cp_gsm_session_status.u_molr_invoke_sent = TRUE;

      z_umts_cp_gsm_session_status.u_network_session_on = TRUE;
    }
    else
    {
      /*
       *  Convert the assistance data wishlist into a 
       *  GSM 09.31 format and store in the session info structure.
       *  It will be packed into an RRLP message inside the 
       *  tm_umts_cp_gsm_tx_rrlp_mpr_err() function.
       */

      if ( z_umts_cp_gsm_session_status.e_agps_mode == 
           C_UMTS_CP_GSM_AGPS_MODE_UE_BASED 
         )
      {
        /* when requesting additional assistance, do not request IONO or UTC */
        p_assist->q_WishListMask &= C_GPS_ASSIS_REFTIME | C_GPS_ASSIS_REFLOC |
                                    C_GPS_ASSIS_NAVMODL | C_GPS_ASSIS_ALM;
      }

      if ( p_assist->q_WishListMask != 0 ) /* if it still has something to request */
      {

        MSG_MED("CP-GSM: Requesting additional WL=0x%lx",p_assist->q_WishListMask, 0, 0 );

        tm_umts_common_utils_gsm0931_gps_assist_data_ie_build(
                          p_assist,
                          &z_umts_cp_gsm_session_status.z_GpsAssistData );

        (void) tm_umts_cp_gsm_tx_rrlp_mpr_err( rrlp_gpsAssDataMissing );
        tm_umts_cp_gsm_set_cf_state( C_UMTS_CP_GSM_CF_STATE_WAIT_RRLP );

      } /* if still q_WishListMask != 0 */
      else
      {
        q_WishListMask = 0; /* so that it will be processed the same way as if the request
                               were suppressed in early part of this function */  
      }
    }  /* NI or MOLR-Invoke-already-sent */
  }  /* if q_WishListMask != 0 */

  if ( q_WishListMask == 0 )
  {
    MSG_MED("CP-GSM: Final wishlist empty", 0, 0, 0 );

    if( z_umts_cp_gsm_session_status.u_mpr_rcvd )
    {
      z_umts_cp_gsm_session_status.u_final_mpr_rcvd = TRUE;
      tm_umts_cp_gsm_set_cf_state( C_UMTS_CP_GSM_CF_STATE_TIMER_RRLP_MEAS_RESP_TIME );
    }
 
    tm_umts_common_send_pdapi_comm_event(
                     PDSM_PD_COMM_PROTOCOL_UMTS_CP_GSM,
                     C_COMM_DONE,
                     z_umts_cp_gsm_session_status.tm_active_session_handle ,
                     z_umts_cp_gsm_session_status.q_pdapi_client_id
                     );
                     
                     
    if ( z_umts_cp_gsm_session_status.u_final_mpr_rcvd &&
         z_umts_cp_gsm_session_status.u_pdapi_fix_reported
       )
    {
    
      MSG_MED("CP-GSM: Sending Cached Position to Network", 0, 0, 0);

      tm_umts_cp_gsm_send_pos_rrlp( 
                  &z_umts_cp_gsm_session_status.z_pending_pos.z_pos,
                  z_umts_cp_gsm_session_status.z_pending_pos.e_fix_type,
                  z_umts_cp_gsm_session_status.z_pending_pos.q_time_stamp_msec );

      tm_umts_cp_gsm_inform_core_stop_session( TM_STOP_REASON_COMPLETED );
    
    }   
    else
    {
      tm_umts_cp_gsm_inform_core_nt_assist_end();
      
      /* If this RRLP is of MSA, check if there is cached ACQ. assist., 
         inject the cached if available. */
      if ((z_umts_cp_gsm_session_status.e_agps_mode == 
             C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED ) &&
           z_umts_cp_gsm_session_status.z_cached_acq_assist.u_valid 
         )
      {  
        tm_umts_cp_gsm_post_cached_acq_assist();
      } 
      
    }  
   
  }
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_callflow_kind_set

DESCRIPTION
  Sets the nature of the call flow for the current session.

DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_callflow_kind_set
(
  umts_cp_gsm_callflow_kind_enum_type e_callflow_kind
)
{
  z_umts_cp_gsm_session_status.e_callflow_kind = e_callflow_kind;

  if ( e_callflow_kind == C_UMTS_CP_GSM_SESSION_NI )
  {
    z_umts_cp_gsm_session_status.q_pdapi_client_id = pdsm_ni_client_id_get();
    z_umts_cp_gsm_session_status.tm_active_session_handle = 
                                                  TM_SESS_HANDLE_UMTS_CP_NI;
    z_umts_cp_gsm_session_status.u_network_session_on = TRUE;
  }
  else if( z_umts_cp_gsm_session_status.e_agps_mode == 
                                      C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED )
  {
    z_umts_cp_gsm_session_status.u_network_session_on = TRUE;
  }
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_inform_core_stop_session

DESCRIPTION
  This function informs the TM core that the current session should 
  terminate.
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void  tm_umts_cp_gsm_inform_core_stop_session
( 
  tm_sess_stop_reason_e_type stop_reason
)
{
  tm_sess_req_param_u_type  req_param;

  req_param.stop_param.stop_type = TM_STOP_TYPE_SESSION; 
  req_param.stop_param.stop_reason = stop_reason; 

  MSG_MED("CP-GSM: Stop TmCore session", 0, 0, 0);

  (void) tm_sess_req ( 
                TM_PRTL_TYPE_UMTS_CP,
                z_umts_cp_gsm_session_status.tm_active_session_handle,
                TM_SESS_REQ_STOP, 
                &req_param
              ); 

  /*
   *  Tell MM that we have killed the LCS session
   *  Tell RR that we have killed the LCS session
   */
  #ifdef FEATURE_CGPS_DUAL_WCDMA 
  tm_umts_common_utils_mm_session_status_notify( FALSE, z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id );
  #else
  tm_umts_common_utils_mm_session_status_notify( FALSE );
  #endif
  tm_umts_common_utils_rr_session_status_notify( FALSE, z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id );

  /*
   *  Clear all protocol submodule session related variables
   */
  tm_umts_cp_gsm_session_init();
}




/*===========================================================================

FUNCTION tm_umts_cp_gsm_handle_lcs_event

DESCRIPTION
  This function handles LCS events
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_handle_lcs_event
(
  prtl_lcs_event_s_type   *p_event
)
{
  int   cf_state = (int) z_umts_cp_gsm_session_status.e_cf_state;

  (void) cf_state;

  MSG_MED("CP-GSM: handle_lcs_event() called; s=%d; ev=%d", 
          cf_state, (int) p_event->lcs_event, 0);

  switch( p_event->lcs_event )
  {
    case PRTL_LCS_EVENT_TYPE_RESP:
      tm_umts_cp_gsm_notif_verif_resp_proc( 
                    &p_event->lcs_event_payload.clnt_resp_payload );
      break;

    case PRTL_LCS_EVENT_TYPE_CLINET_INIT:
    case PRTL_LCS_EVENT_TYPE_CLINET_RELEASE:
      MSG_MED("CP-GSM: Unhandled LCS Event", 0, 0, 0 );
      break;

    default:
      MSG_MED("CP-GSM: Invalid LCS Event", 0, 0, 0 );
      break;
  }
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_handle_phone_event

DESCRIPTION
  This function handles PHONE events
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_handle_phone_event
(
  prtl_phone_event_s_type   *p_event
)
{
  umts_cp_gsm_session_status_s_type   *p_session_status;
  int                                 cf_state;

  tm_cm_phone_state_info_s_type phone_state_info;
  p_session_status = &z_umts_cp_gsm_session_status;
  cf_state = (int) p_session_status->e_cf_state;

  MSG_MED("CP-GSM: handle_phone_event() called; s=%d; ev=%d", 
          cf_state, p_event->event, 0);


  switch( p_event->event )
  {
    case TM_PHONE_EVENT_E911_CALL_ORIG:
      /*
       *  This event is received when an E911 call is brought up.
       *  This event must be handled by starting a session, starting
       *  the GPS engine, and starting a timer that will expire (and
       *  shut off the GPS engine) if no RRLP positioning session is
       *  started within a reasonable time after bringing up the E911 call.
       *
       *  Start out in UE-Based mode if the NV item for Positioning modes
       *  support GSM MSBi because this will get the engine
       *  up and running immediately in the hope that this will speed up
       *  signal acquisition and decrease TTF.  
       *
       *  Start a timer so we can shut off MGP if no RRLP session is 
       *  started within a reasonable period of time.
       */

      tm_cm_iface_get_phone_state(&phone_state_info);
      if ( (phone_state_info.srv_system == CGPS_SRV_SYS_GSM) &&
           TM_CONFIG_POS_MODE_BIT_IS_SET(PDSM_POS_MODE_2G_CP_MS_BASED) )
      {
        #ifdef FEATURE_MO_GPS_FOR_ECALL
        /* Do not do an early start if we can also accept MO in E911 */
        if (tm_umts_common_utils_emergency_effect() == C_EMERGENCY_HANDLING_ACCEPT_ALL)
        {
          MSG_MED("CP-GSM: No Early Start for EMERGENCY_HANDLING_ACCEPT_ALL ", 0, 0, 0 );
        }
        else if( z_umts_cp_gsm_session_status.e_cf_state == C_UMTS_CP_GSM_CF_STATE_NONE )
        {
          MSG_MED("CP-GSM: Handle E911_CALL_ORIG", 0, 0, 0 );
        
          (void) tm_umts_cp_gsm_start_sess_req(  C_UMTS_CP_GSM_AGPS_MODE_UE_BASED );

          tm_core_send_pdapi_emerg_sess_type(PDSM_PD_EXT_STATUS_EMERG_SESS_START);
          /* Starting E911: MGP will be on for at most 40 seconds. */
          tm_umts_cp_gsm_start_max_mgp_on_timer( 
           TM_UMTS_CP_GSM_MAX_MGP_ON_IN_MS );
         
          /* As E911 call starts, this timer is activated and and it affects the 
             dynamic QOS and accuracy threshold for MGP, prior and post its 
             expiration. A typical value is 24 seconds. */
          tm_umts_cp_gsm_start_acc_reset_timer(
            TM_UMTS_CP_GSM_ACC_RESET_DYNAMIC_QOS_DISABLE_TIME_IN_MS);
        } 
        #else
        if( z_umts_cp_gsm_session_status.e_cf_state == C_UMTS_CP_GSM_CF_STATE_NONE )
        {
          MSG_MED("CP-GSM: Handle E911_CALL_ORIG", 0, 0, 0 );
        
          (void) tm_umts_cp_gsm_start_sess_req(  C_UMTS_CP_GSM_AGPS_MODE_UE_BASED );

          tm_core_send_pdapi_emerg_sess_type(PDSM_PD_EXT_STATUS_EMERG_SESS_START);
          /* Starting E911: MGP will be on for at most 40 seconds. */
          tm_umts_cp_gsm_start_max_mgp_on_timer( 
           TM_UMTS_CP_GSM_MAX_MGP_ON_IN_MS );
         
          /* As E911 call starts, this timer is activated and and it affects the 
             dynamic QOS and accuracy threshold for MGP, prior and post its 
             expiration. A typical value is 24 seconds. */
          tm_umts_cp_gsm_start_acc_reset_timer(
            TM_UMTS_CP_GSM_ACC_RESET_DYNAMIC_QOS_DISABLE_TIME_IN_MS);
        } 
        #endif

        z_umts_cp_gsm_data.u_e911_in_progress = TRUE;  /* mark the start of E911 */

        /* for the use of multiple session handling in E911 */ 
        z_umts_cp_gsm_data.u_e911_subsequent_rrlp_sess = FALSE;
      }
      else 
      {
        MSG_MED("CP-GSM: Not in GSM mode. Ignore E911_CALL_ORIG", 0,0,0);
      }
      break;

    case TM_PHONE_EVENT_E911_CALL_END:
      MSG_MED("CP-GSM: Handle E911_CALL_END", 0, 0, 0 );
      z_umts_cp_gsm_data.u_e911_in_progress = FALSE; /* mark the end of E911 */
      /* for the use of multiple session handling in E911 */ 
      z_umts_cp_gsm_data.u_e911_subsequent_rrlp_sess = FALSE;
      break;

    case TM_PHONE_EVENT_LCS_MOLR:
      /*
       *  This event is received when CM confirms that the LCS-MOLR-Invoke
       *  has been queued to NS.
       *
       *  Set the callflow state to indicate that we are waiting for
       *  an RRLP message and set a timer to timeout in case nothing
       *  is received.
       */
      MSG_MED("CP-GSM: CM confirms molr-invoke queued to NAS", 0, 0, 0);
      
      tm_umts_cp_gsm_set_cf_state( C_UMTS_CP_GSM_CF_STATE_WAIT_RRLP );
      break;

    case TM_PHONE_EVENT_LCS_MOLR_CONF:
      /*
       *  cancel the timeout timer
       *  set the callflow state back to "IDLE"
       *  notify SS that LCS has completed
       *  notify MM and RR that LCS has completed
       */
      tm_umts_cp_gsm_cancel_cf_timer();

      tm_umts_cp_ss_lcs_molr_conf_proc( 
                &p_event->payload.sups_data );

      tm_umts_cp_gsm_lcs_molr_conf_proc( 
                &p_event->payload.sups_data );
      break;

    case TM_PHONE_EVENT_SS_SRV_DOMAIN:
    case TM_PHONE_EVENT_SS_SRV_STATUS:
      MSG_LOW("CP-GSM: Unhandled Phone Event %d", 
                (int)p_event->event, 0, 0);
      break;

    
    case TM_PHONE_EVENT_LCS_MTLR_NTFY_IND:
      MSG_MED("CP-GSM: CM delivers MTLR_NTFY_IND", 0, 0, 0);
      tm_umts_cp_ss_lcs_loc_notif_ind_proc( &p_event->payload.sups_data );
      break;

    case TM_PHONE_EVENT_LCS_MTLR_NTFY_RES:
      MSG_MED("CP-GSM: CM confirms LCS-LocNotifResp queued to NAS",0,0,0);
      break;
 
    case TM_PHONE_EVENT_LCS_MOLR_COMPLETED:
      MSG_MED("CP-GSM: CM confirms LCS_MOLR_COMPLETED", 0, 0, 0);
      if( (p_session_status->e_agps_mode == 
                                    C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED) ||
          (p_session_status->e_mo_lr_type == LOCATION_ESTIMATE) )
      {
        tm_umts_cp_gsm_inform_core_stop_session( TM_STOP_REASON_COMPLETED );
      }
      break;

    default:
      MSG_HIGH("CP-GSM: Invalid Phone Event %d", 
                (int)p_event->event, 0, 0);
      break;
  }
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_handle_apdu_data_seg

DESCRIPTION
  This function handles RR APDU MSG events
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/
static void tm_umts_cp_gsm_process_apdu_seg
( 
  const prtl_rr_apdu_data_seg_type *p_apdu 
)
{
  if (NULL == p_apdu)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "p_apdu is NULL inside tm_umts_cp_gsm_process_apdu_seg");
    return;
  }
  /*
   *  Check to make sure this segment won't cause buffer overrun
   */
  if( (apdu.length + p_apdu->length) > sizeof( apdu.rx_buffer ) )
  {
    MSG_ERROR("CP-GSM: APDU Segment Overrun", 0, 0, 0);
  
    apdu.state  = APDU_STATE__IDLE;
    apdu.length = 0;
    apdu.ndx  = 0;
    return;
  }

  /*
   *  Copy data to a local APDU buffer
   */
  memscpy( apdu.rx_buffer+apdu.ndx, (sizeof(apdu.rx_buffer) - apdu.ndx), p_apdu->data, p_apdu->length );

  /*
   *  Update the apdu length
   */
  apdu.length += p_apdu->length;

  apdu.ndx += p_apdu->length;
  /*
   *  IF this is the last segment THEN 
   *    IF this is the first RRLP packet received THEN
   *      Inform RR that LCS is in session
   *    Parse and process the packet
   *    Increment a packet count for use mainly by the if check above
   *  ENDIF
   */
  if( !(p_apdu->flags & APDU_LAST_FLAG) )
  {
    apdu.state = APDU_STATE__IDLE;

    MSG_MED("CP-GSM: APDU_LAST; start=%d", 
            z_umts_cp_gsm_session_status.u_tm_core_session_started,
            0, 
            0 );

/* Notify MM that RRLP messaging has started. This will set a 300 second
   guard timer in NAS that will protect against keeping the signaling channel
   up indefinately. If we did not notify NAS here, it would drop the channel
   10 seconds after NAS sends back the location notification return result */
#ifdef FEATURE_CGPS_DUAL_WCDMA
	tm_umts_common_utils_mm_session_status_notify( TRUE, z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id );
#else
	tm_umts_common_utils_mm_session_status_notify( TRUE );
#endif

    tm_umts_common_utils_rr_session_status_notify( TRUE, z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id );

    tm_umts_cp_gsm_parse_rrlp( apdu.rx_buffer, apdu.length );
  }
  else
  {
    apdu.state = APDU_STATE__IN_PROGRESS;
  }
}

/*===========================================================================

FUNCTION tm_umts_cp_gsm_handle_apdu_data_seg

DESCRIPTION
  This function handles RR APDU MSG events
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/
static void tm_umts_cp_gsm_handle_apdu_data_seg
( 
  const prtl_rr_apdu_data_seg_type *p_apdu 
)
{
  boolean u_proc_msg = TRUE;
  boolean u_init_vars = FALSE;
  
  MSG_LOW("CP-GSM: handle_apdu_data_seg", 0, 0, 0);

  /*
   *  Validate the APDU ID
   */
  if( p_apdu->id != APDU_ID_RRLP )
  {
    MSG_ERROR("CP-GSM: Invalid APDU ID", 0, 0, 0);
    return;
  }

  switch (apdu.state)
  {
    case APDU_STATE__IN_PROGRESS:
    {
      /* If this message is flagged as a first message, reset the rx size */
      if( !(p_apdu->flags & APDU_FIRST_FLAG) )
      {
        u_init_vars = TRUE;
      }
      break;
    }
    case APDU_STATE__IDLE:
    {
       /* Check to see if this segment is flagged as a first segment */
      if( (p_apdu->flags & APDU_FIRST_FLAG) )
      {
        MSG_ERROR("CP-GSM: Invalid APDU_FIRST_FLAG", 0, 0, 0);
        break;
      }
      u_init_vars = TRUE;
      break;
    }
    default:
      u_proc_msg = FALSE;
      break;
  }
  if (u_init_vars == TRUE)
  {
    apdu.length  = 0; 
    apdu.ndx     = 0; 
  }
  if (u_proc_msg == TRUE)
  {
    tm_umts_cp_gsm_process_apdu_seg(p_apdu);
  }
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_handle_rr_session_abort

DESCRIPTION
  This function handles RR preemption (ie. often handover) events
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_handle_rr_session_abort
(
  void
)
{
  MSG_HIGH("RR session abort revd", 0, 0, 0);
  sm_report_event_preemption_flag();
  /*
   *  Set a flag indicating that at least one preemption has occurred
   *  during this session.
   */
  z_umts_cp_gsm_session_status.u_preemption_flag    = TRUE;

  /*Clear away the emergency flag*/
   z_umts_cp_gsm_data.u_e911_in_progress = FALSE;
  /*
   *  Clear the "final_mpr_rcvd" flag.  This will prevent position from
   *  being reported to the network until another MPR is sent.  If a position
   *  happens to be reported before another MPR, then that position
   *  will be cached.  This should be a rare occurrence since QOS
   *  accuracy gets set to 0m when a preemption occurs.  This is just here
   *  to handle the small race window in which a position can be reported
   *  before the accuracy gets changed to 0m.
   */
  z_umts_cp_gsm_session_status.u_final_mpr_rcvd = FALSE;

  /*  Clear the "mpr_rcvd" flag. This will prevent the accuracy being set to 
      250m at AccuracyReset timer times out while still waiting for re-bid */
  z_umts_cp_gsm_session_status.u_mpr_rcvd = FALSE;

  /*
   *  IF a session has been started THEN
   *    Force QOS accuracy to 0 to force MGP to run to timeout
   *    This will get "undone" as soon as the next MPR is received.
   *  ENDIF
   */
  z_umts_cp_gsm_session_status.z_session_qos.q_hor_acc_meters = 0;
  
  /* will be able to accept new RRLP of any refNum now */  
  z_umts_cp_gsm_session_status.z_network_assist.radio_link_prot_params.z_rrlp_params.b_RefNum = -1;

  /* When preemption happens, set accuracy threshold to 0 to prevent 
     reporting, set timeout to the max. of 255 to let the receiver running
     until interupted by other sources or to max. of 255 seconds; 
     and if this happens before AccuracyResetTimer expiration, 
     keep using dynamic_qos; if this happens after that expiration, 
     disable dynamic_qos. 
     
     tm_umts_cp_gsm_send_continue_session_to_tm ( 
      accuracy_threshold, timeout_sec, need_wishlist, use_dynamic_qos )
      
     This is only to be conducted if RR_abort happens when the MS is either 
     expecting RRLP from the network or is expect to send RRLP to report to 
     the network whenever a position/meas. is ready.
      
  */
  
  if ( ( z_umts_cp_gsm_session_status.e_cf_state == 
         C_UMTS_CP_GSM_CF_STATE_WAIT_RRLP ) ||
       ( z_umts_cp_gsm_session_status.e_cf_state ==  
         C_UMTS_CP_GSM_CF_STATE_TIMER_RRLP_MEAS_RESP_TIME
       )
     )    
  {   
    if ( !z_umts_cp_gsm_session_status.u_accuracy_reset_timer_expired )
    {
      tm_umts_cp_gsm_send_continue_session_to_tm( 0, 255, FALSE, TRUE );
    }
    else 
    {
      tm_umts_cp_gsm_send_continue_session_to_tm( 0, 255, FALSE, FALSE );
    }
  } 
  
  /* terminate any call flow state */ 
  tm_umts_cp_gsm_cancel_cf_timer();  

  /* clear wishlist_requested flag, so that if the re-bid MPR does not have all 
     the needed assistance data, the UE will request for additional */
  z_umts_cp_gsm_session_status.u_wishlist_requested = FALSE;
  
}

/*===========================================================================

FUNCTION tm_umts_cp_gsm_validate_as_id

DESCRIPTION
  This function validates as_id for RR events to be sure that the as_id matches with the ongoing session's as_id
  
DEPENDENCIES: none

RETURN VALUE: TRUE: If as_id matches with ongoing session's as_id
			   FALSE: If as_id does not match with ongoing session's as_id

SIDE EFFECTS: none

===========================================================================*/

boolean tm_umts_cp_gsm_validate_as_id
(
  const prtl_rr_event_s_type  *p_rr_event
)
{
  boolean retValue = FALSE;

  if (NULL == p_rr_event)
  {
	MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "CP-GSM:NULL Pointer passed to function tm_umts_cp_gsm_validate_as_id");
	return FALSE;
  }
  	
  MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_LOW, "CP-GSM: tm_umts_cp_gsm_validate_as_id() called with as_id %u", (uint32)p_rr_event->as_id);
  
  /* Confirm if the new event is related to an ongoing session */
  if (z_umts_cp_gsm_session_status.u_tm_core_session_started)
  {
    if (!z_umts_cp_gsm_session_status.z_rr_as_id_info.v_rr_event_as_id_received)
    {
      /* This is the first event from RR with as_id. Cache the as_id, so that we can use it for rest of the session */
	  z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id = z_umts_cp_gsm_session_status.z_rr_as_id_info.e_current_rr_as_id;
      z_umts_cp_gsm_session_status.z_rr_as_id_info.v_rr_event_as_id_received = TRUE;
	  MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "CP-GSM:First RR event with as_id. AS_ID cached %u", (uint32)p_rr_event->as_id);
      retValue = TRUE;
    }
	else if (p_rr_event->as_id != z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id)
    {
      /* Events for new session on a different sub, do not handle them */
	  MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "CP-GSM: Dropping event for as_id %u, ongoing session's as_id is %u",
	  			(uint32)p_rr_event->as_id, (uint8)z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id);
	  retValue = FALSE;
    }
	else
	{
	  /* Received event for ongoing session */
	  MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "CP-GSM:Received event for ongoing CP-GSM session and cached as_id matches %u", (uint32)p_rr_event->as_id);
	  retValue = TRUE;
	}
  }
  else /* New session on-going, let it through*/
  {
    /* Cache the as_id, so that we can use it for rest of the session */
    z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id = z_umts_cp_gsm_session_status.z_rr_as_id_info.e_current_rr_as_id;
    z_umts_cp_gsm_session_status.z_rr_as_id_info.v_rr_event_as_id_received = TRUE;
	MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "CP-GSM: New session. AS_ID cached to %u", (uint32)z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id);
    retValue = TRUE;
  }
  
  return retValue;
}

/*===========================================================================

FUNCTION tm_umts_cp_gsm_handle_rr_event

DESCRIPTION
  This function handles RR events
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_handle_rr_event
(
  const prtl_rr_event_s_type  *p_rr_event
)
{
  if (p_rr_event == NULL)
  {
    MSG_HIGH("NULL Pointer: p_rr_event", 0,0,0);
	return;
  }
  
  MSG_LOW("CP-GSM: handle_rr_event() called with as_id %u ", (uint32)p_rr_event->as_id, 0, 0);
  z_umts_cp_gsm_session_status.z_rr_as_id_info.e_current_rr_as_id = p_rr_event->as_id;

  /* Validate AS_ID */
  if (!tm_umts_cp_gsm_validate_as_id(p_rr_event))
  {
    return;
  }	

  switch( p_rr_event->event_id )
  {
    case PRTL_RR_APDU_MSG_EVENT:
      tm_umts_cp_gsm_handle_apdu_data_seg( &p_rr_event->event_payload.apdu );
      break;

    case PRTL_RR_APDU_ABORT_EVENT:
      break;

    case PRTL_RR_SESSION_ABORT_EVENT:
      tm_umts_cp_gsm_handle_rr_session_abort();
      break;

    default:
      MSG_ERROR("CP-GSM: invalid RR Event", 0, 0, 0);
      return;
  }
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_handle_mm_event

DESCRIPTION
  This function handles MM events
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_handle_mm_event
(
  const prtl_mm_event_s_type  *p_mm_event
)
{
  MSG_LOW("CP-GSM: handle_mm_event() called", 0, 0, 0);

  switch( p_mm_event->mm_event_data_ptr->e_event_type )
  {
    case MM_CGPS_MSG_EVENT_ABORT:
      MSG_HIGH("CP-GSM: got LCS abort from MM", 0, 0, 0);
      break;

    case MM_CGPS_MSG_EVENT_RESET_UE_POS_INFO:
      MSG_HIGH("CP-GSM: got OTA cold start from MM", 0, 0, 0);
      break;

    default:
      MSG_ERROR("CP-GSM: invalid MM Event", 0, 0, 0);
      break;
  }
}

/*===========================================================================

FUNCTION tm_umts_cp_gsm_write_data

DESCRIPTION
  This function sends data to RR
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

uint16 tm_umts_cp_gsm_write_data
(
  const byte *  p_data, uint16 length 
)
{
  gs_status_T   gs_stat;

  MSG_MED("CP-GSM: gsm_write_data called for as_id %d", (uint8)z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id, 0, 0);

  if (NULL == p_data)
  {
    MSG_ERROR("NULL pointer passed to tm_umts_cp_gsm_write_data", 0, 0, 0);
    return 0;
  }
  
  if( length > MAX_APDU_DATA_LEN )
  {
    MSG_ERROR("RRLP Message exceeds maximum buffer length", 0, 0, 0);
    return 0;
  }

  memscpy( rr_apdu_req.apdu_data, length, p_data, length );

  rr_apdu_req.message_header.message_set  = MS_LSM_RR;
  rr_apdu_req.message_header.message_id   = (int) RR_APDU_REQ;
  rr_apdu_req.as_id						  = z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id;

  rr_apdu_req.apdu_len  = length;

  PUT_IMH_LEN
  (
    sizeof(rr_apdu_req_T) - sizeof(IMH_T),
    &rr_apdu_req.message_header
  );

  gs_stat = gs_send_message( GS_QUEUE_RR, &rr_apdu_req, TRUE );
  if( gs_stat != GS_SUCCESS )
  {
    MSG_ERROR("CP-GSM: gs_send_message(apdu) failed", 0, 0, 0);
    return 0;
  }

  return length; 
}


/********************************************************************
*
* tm_umts_cp_gsm_start_sess_req
*
* Function description:
*   Sends a START session request to TM-Core.  This is only used by
*   MT callflows since the START session request is done inside TM
*   when doing MO callflows.
*
* Parameters: 
*   p_req_data : pointer to a structure holding information from
*                pdapi .
*
* Return:
*   TRUE if Successful, else FALSE
*  
*********************************************************************
*/
boolean tm_umts_cp_gsm_start_sess_req
(
  umts_cp_gsm_session_agps_mode_enum_type   e_agps_mode
)
{
  boolean                                   accepted;
  tm_sess_req_param_u_type                  req_param;
  uint8                                     acc_thresh;
  tm_sess_req_start_param_s_type            *p_start_params;

  if( e_agps_mode == C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED )
  {
    MSG_LOW("CP-GSM: gsm_start_sess_req MTLR/NILR UE_ASSISTED", 0, 0, 0);
  }  
  else
  {
    MSG_LOW("CP-GSM: gsm_start_sess_req MTLR/NILR UE_BASED", 0, 0, 0);
  }  

  z_umts_cp_gsm_session_status.e_agps_mode = e_agps_mode;
  z_umts_cp_gsm_session_status.z_session_qos.u_gps_session_timeout_value_secs = 255;
  z_umts_cp_gsm_session_status.z_session_qos.q_hor_acc_meters = 0;

  /*
   *  Create a pointer to the START SESSION request parameters to make
   *  the code a bit cleaner and easier to read later in this function.
   */
  p_start_params = &req_param.start_param;
  
  /*
   *  Start out with an accuracy threshold of 0 meters when starting
   *  an MT session so that the MGP receiver will stay on indefinitely.
   *  The accuracy threshold will get set to the desired value later
   *  when we receive the MsrPosReq message from the SMLC.
   */
  acc_thresh = 0;

  /*
   *  Set up parameters differently for UE-Assisted vs UE-Based
   */
  if( e_agps_mode == C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED )
  {
    p_start_params->op_mode = TM_SESS_OPERATION_MODE_MSA;
    p_start_params->op_req  = TM_OP_REQ_PRM;
    p_start_params->prm_qos = TM_MAX_SESSION_TIMEOUT;
  }    
  else
  {
    p_start_params->op_mode = TM_SESS_OPERATION_MODE_MSB;
    p_start_params->op_req  = TM_OP_REQ_LR;
    p_start_params->lr_qos  = TM_MAX_SESSION_TIMEOUT;
  }

  /*
   *  Set up parameters that are common to all modes
   */
  p_start_params->abort_curr_sess     = FALSE;
  p_start_params->num_fixes           = 1;
  p_start_params->tbf_ms              = 1000;
  p_start_params->accuracy_threshold  = acc_thresh;
  p_start_params->ppm_qos             = 0;
  p_start_params->prm_prq             = 0;
  p_start_params->dynamic_qos_enabled = FALSE;
  p_start_params->multi_report_msa = FALSE;

  /*
   *  Set the Callflow Kind in our session status structure
   *  because this will also assign a session handle to be used below
   */
  tm_umts_cp_gsm_callflow_kind_set( C_UMTS_CP_GSM_SESSION_NI );
 
  /*
   *  Tell TM-Core to start a session now
   */
  accepted = tm_sess_req(  
                        TM_PRTL_TYPE_UMTS_CP,
                        z_umts_cp_gsm_session_status.tm_active_session_handle,
                        TM_SESS_REQ_START,
                        &req_param );

  if( accepted )
  {
    MSG_MED("CP-GSM: tm_sess_req() STARTED", 0, 0, 0);

    z_umts_cp_gsm_session_status.u_tm_core_session_started = TRUE;

    tm_umts_cp_gsm_set_cf_state( C_UMTS_CP_GSM_CF_STATE_STARTED );
  }
  else
  {
    MSG_MED("CP-GSM: tm_sess_req() REJECTED", 0, 0, 0);
    
    tm_umts_cp_gsm_inform_core_stop_session( TM_STOP_REASON_GENERAL_ERROR );
  }

  return accepted;
}

/********************************************************************
*
* cp_gsm_action_upon_ni_notif_verif_accepted
*
* Function description:
*   actions to be taken once an NI request is accepted by the OEM layer.
*
* Parameters: 
*   None
*
* Return:
*  None
*  
*********************************************************************
*/
static void cp_gsm_action_upon_ni_notif_verif_accepted ( void )
{
  #ifdef FEATURE_CGPS_DUAL_WCDMA
  tm_umts_common_utils_mm_session_status_notify( TRUE, z_umts_cp_gsm_session_status.z_rr_as_id_info.e_cached_rr_as_id );
  #else
  tm_umts_common_utils_mm_session_status_notify( TRUE );
  #endif
}


/********************************************************************
*
* cp_gsm_action_upon_ni_notif_verif_denied
*
* Function description:
*   actions to be taken once an NI request is denied
*   by the OEM layer.
*
* Parameters: 
*
* Return:
*  None
*  
*********************************************************************
*/

static void cp_gsm_action_upon_ni_notif_verif_denied ( void )
{
  #ifdef TBD
    if( ( p_zSessionStatus->u_CollectInitiator == C_COLLECT_INITIATOR_NULL ) &&
        ( p_zSessionStatus->u_ThirdPartyMOLRInProgress == FALSE ) &&
        ( p_SessionConfig->u_periodic_app_tracking_on == FALSE ) 
      )
    {
      p_SessionConfig->u_allow_rrc = p_LsmNvItems->aagps_default_allow_rrc;
    }
  #endif
}

/********************************************************************
 *
 * tm_umts_cp_gsm_notif_verif_resp_proc
 *
 * Function description:
 *  Processing notify-verify response from pdapi
 *
 * Parameters: 
 *   pdapi_ver_resp_lcs_ptr: pointer to the structure of the pdapi 
 *     command notif_verification_response
 *             
 * Return: NONE.
 *   
 *
 *********************************************************************
 */
static void tm_umts_cp_gsm_notif_verif_resp_proc
(
  const pdsm_lcs_notification_resp_s_type *p_user_resp 
)
{                                           
  const pdsm_lcs_umts_cp_notify_verify_req_s_type   *p_req_data;
  boolean                                           u_accepted = FALSE;
  pdsm_lcs_notify_verify_e_type                     notif_priv_type;

  /*
   *  Set up local data to make the code easier to read later
   */
  p_req_data      = &p_user_resp->lcs_info.umts_cp_req;
  notif_priv_type = p_req_data->notification_priv_type;
   
  if ( p_user_resp->resp == PDSM_LCS_NOTIFY_VERIFY_ACCEPT )
  {
    u_accepted = TRUE;
    MSG_MED("CP-GSM: Notif/Verif ACCEPT", 0, 0, 0);
  }
  else if ( p_user_resp->resp == PDSM_LCS_NOTIFY_VERIFY_DENY )
  {
    if ( ( notif_priv_type == PDSM_LCS_NO_NOTIFY_NO_VERIFY ) ||
         ( notif_priv_type == PDSM_LCS_PRIVACY_OVERRIDE ) ||
         ( notif_priv_type == PDSM_LCS_USER_NOTIFY_ONLY ) )
    {
      /*
       *  NOTE: regardless of what the OEM says, always accept the request; 
       *  in fact, by design, pdapi should always send ACCEPT 
       */
      u_accepted = TRUE;

      MSG_MED("CP-GSM: Notif/Verif REJECT overiden as ACCEPT due to priv_type",0,0,0);
    }
    else
    {
      u_accepted = FALSE;
      MSG_MED("CP-GSM: Notif/Verif REJECT", 0, 0, 0);
    }
  }
  else
  {
    /* 
     *   No application response or invalid response
     */
    if ( ( notif_priv_type == PDSM_LCS_NO_NOTIFY_NO_VERIFY ) ||
         ( notif_priv_type == PDSM_LCS_PRIVACY_OVERRIDE ) ||
         ( notif_priv_type == PDSM_LCS_USER_NOTIFY_ONLY ) ||
         ( notif_priv_type == PDSM_LCS_USER_NOTIFY_VERIFY_ALLOW_NO_RESP ) )
    {
      /*
       *  NOTE: for the top three, regardless of what the OEM says, 
       *  always accept the request; in fact, by design in these cases,
       *  pdapi should always send ACCEPT;  For the last case, no response
       *  is to be treated as ACCEPT.
       */
      u_accepted = TRUE;

      if ( p_user_resp->resp == PDSM_LCS_NOTIFY_VERIFY_NORESP )
      {
        MSG_MED("CP-GSM: Notif/Verif NORESP taken as ACCEPT due to priv_type", 
                0, 0, 0);
      }
      else
      {
        MSG_MED("CP-GSM: Notif/Verif invalid resp. taken as ACCEPT due to priv_type", 
                0, 0, 0);
      }
    }
    else if ( p_req_data->notification_priv_type == 
                            PDSM_LCS_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP )
    {
      u_accepted = FALSE;

      if ( p_user_resp->resp == PDSM_LCS_NOTIFY_VERIFY_NORESP )
      {
        MSG_MED("CP-GSM: Notif/Verif NORESP taken as REJECT due to priv_type", 0, 0, 0);
      }
      else
      {
        MSG_MED("CP-GSM: Notif/Verif invalid resp. taken as REJECT due to priv_type", 
                0, 0, 0);
      }
    }
    else /* should never get here, only for completeness */
    {
      u_accepted = FALSE;

      MSG_MED("CP-GSM: Notif/Verif no-resp with unexpected priv_type taken as REJECT", 
              0, 0, 0);
    }
  } 

  /* ask SS to send the response */
  (void) tm_umts_cp_request_lcs_loc_notif_return_result( p_user_resp );

  if ( u_accepted )
  {
    cp_gsm_action_upon_ni_notif_verif_accepted( );
  }
  else 
  {
    cp_gsm_action_upon_ni_notif_verif_denied( );
  }
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_send_continue_session_to_tm

DESCRIPTION
  This function sends a session continue command to TM-Core and optionally
  requests wishlist information.
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_send_continue_session_to_tm( uint8 u_acc_thresh,
                                                 uint8 u_timeout_secs,
                                                 boolean u_need_wishlist,
                                                 boolean u_use_dynamic_qos )
{
  tm_sess_req_param_u_type    req_param;

  if( z_umts_cp_gsm_session_status.e_agps_mode == 
                                        C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED )
  {
    req_param.continue_param.op_mode  = TM_SESS_OPERATION_MODE_MSA;
    req_param.continue_param.op_req   = TM_OP_REQ_PRM;
    req_param.continue_param.prm_qos  = u_timeout_secs;
    
    /* overloading this multi_report_msa that is defined to indicate
       control plane multiple reporting, for the purpose of letting 
       MGP to run all the way to timeout without declaring DONE when
       a qualified measurement is reached */
    if ( u_acc_thresh != 0 )
    {
      req_param.continue_param.multi_report_msa = FALSE;
    }
    else
    {
      req_param.continue_param.multi_report_msa = TRUE;
    }  
  }
  
  else
  {
    req_param.continue_param.op_mode  = TM_SESS_OPERATION_MODE_MSB;
    req_param.continue_param.op_req   = TM_OP_REQ_LR;
    req_param.continue_param.lr_qos   = u_timeout_secs;
    req_param.continue_param.multi_report_msa = FALSE;
  }

  req_param.continue_param.num_fixes            = 1;
  req_param.continue_param.tbf_ms               = 1000;
  req_param.continue_param.ppm_qos              = 0;
  req_param.continue_param.prm_prq              = 0;
  req_param.continue_param.accuracy_threshold   = u_acc_thresh;
  req_param.continue_param.dynamic_qos_enabled  = u_use_dynamic_qos;

  if( u_need_wishlist )
  {    
    req_param.continue_param.op_req |= TM_OP_REQ_AIDING_DATA_STATUS;

    MSG_MED(  "CP-GSM: requesting wishlist continuation; qos=%u, acc=%lu", 
              u_timeout_secs,
              u_acc_thresh,
              0 ); 
  }
  else
  {
    MSG_MED(  "CP-GSM: ntwk over; qos=%u, acc=%lu", 
              u_timeout_secs,
              u_acc_thresh,
              0 ); 
  }
  
  (void) tm_sess_req(  TM_PRTL_TYPE_UMTS_CP,
                z_umts_cp_gsm_session_status.tm_active_session_handle,
                TM_SESS_REQ_CONTINUE,
                &req_param );
}


/*===========================================================================

FUNCTION continue_session

DESCRIPTION
  This function sends a session continue command to TM-Core and optionally
  requests wishlist information.
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

static void continue_session( boolean need_wishlist )
{
  uint8   u_acc_thresh;
  uint8   u_timeout_secs;

  u_timeout_secs = 255; /* for wishlist request, do not set timer MPG timer
                           to the one given by the LCS client; instead only
                           the max. safeguard time is set */ 
  if ( !need_wishlist )
  {
    u_timeout_secs =
    z_umts_cp_gsm_session_status.z_session_qos.u_gps_session_timeout_value_secs;
  }    

  /*
   *  Limit accuracy threshold to 255 meters since it must be stored
   *  into an 8-bit quantity.
   */
  if( z_umts_cp_gsm_session_status.z_session_qos.q_hor_acc_meters <= 255 )
  {
    u_acc_thresh = 
      (uint8) z_umts_cp_gsm_session_status.z_session_qos.q_hor_acc_meters;
  }
  else
  {
    u_acc_thresh = 255;
  }

  if ( ( z_umts_cp_gsm_session_status.e_callflow_kind == C_UMTS_CP_GSM_SESSION_NI ) &&
       ( z_umts_cp_gsm_session_status.u_final_mpr_rcvd == FALSE )
     )
  {
    u_acc_thresh = 0; /* before the final MPR, set acc_threshold to 0, 
                         preventing LM from terminating the session if a 
                         qualified position happens occur prior to the final
                         MPR. */
  }

  tm_umts_cp_gsm_send_continue_session_to_tm( u_acc_thresh, 
                                              u_timeout_secs, 
                                              need_wishlist,
                                              TRUE );
}



/*===========================================================================

FUNCTION tm_umts_cp_gsm_request_wishlist

DESCRIPTION
  This function requests the aiding data wishlist from TM-Core
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_request_wishlist( void )
{
  MSG_MED("CP-GSM: Requesting Wishlist", 0, 0, 0);

  continue_session( TRUE );

  z_umts_cp_gsm_session_status.u_wishlist_requested = TRUE;
}


/*===========================================================================
FUNCTION tm_umts_cp_gsm_handle_max_mgp_on_timer_cb

DESCRIPTION
  This function handles max. MGP_ON timer callbacks
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_handle_max_mgp_on_timer_cb( void )
{
  MSG_HIGH("CP-GSM: MaxMgpOn timer expired", 0, 0, 0); 

  /*
   *  IF the max_mgp_on timer (currently 40 secs) expired THEN
   *    Turn off MGP and abort the session
   */
    tm_umts_cp_gsm_callflow_disrupt_handler();
}

/*===========================================================================

FUNCTION tm_umts_cp_gsm_handle_acc_reset_timer_cb

DESCRIPTION
  This function handles AccuracyReset timer callbacks
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_handle_acc_reset_timer_cb( void )
{
  MSG_HIGH("GSM-CP: AccReset timer expired - %d, %d", 
            z_umts_cp_gsm_session_status.u_preemption_flag, 
            z_umts_cp_gsm_session_status.u_mpr_rcvd, 0); 

  /* mark the expiration of AccuracyResetTimer ( typically 24-sec) */
  z_umts_cp_gsm_session_status.u_accuracy_reset_timer_expired = TRUE;

  if ( z_umts_cp_gsm_session_status.u_preemption_flag ) /* has had preemption */
  {
    if ( z_umts_cp_gsm_session_status.u_mpr_rcvd ) /* inside RRLP-mpr sess */
    {

      /* If at the time this AccuracyResetTimer expiration, and there has 
         been preemption, and is inside an RRLP-mpr session, do the following: 
         Stop dynamic_qos, alter acc_threshold to 250 meters while keeping
         response time unchanged. */

      tm_umts_cp_gsm_send_continue_session_to_tm(
        250, 
        z_umts_cp_gsm_session_status.z_session_qos.u_gps_session_timeout_value_secs, 
        FALSE, 
        FALSE );
    }
    else /* outside RRLP-mpr sess */
    {

      /* If at the time this AccuracyResetTimer expiration, and there has 
         been preemption, but is OUTSIDE of an RRLP-mpr session, do the 
         following: Stop dynamic_qos, keep acc_threshold and response time 
         unchanged ( 0 and 255, respectively ). */

      tm_umts_cp_gsm_send_continue_session_to_tm( 0, 255, FALSE, FALSE );
    }
  } /* has had preemption */
  
}


/*===========================================================================

FUNCTION tm_umts_cp_gsm_handle_cf_timer_cb

DESCRIPTION
  This function handles cf (callflow) timer callbacks
  
DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/

void tm_umts_cp_gsm_handle_cf_timer_cb( void )
{
  umts_cp_gsm_callflow_kind_enum_type       e_callflow_kind;
  umts_cp_gsm_session_agps_mode_enum_type   e_agps_mode;
  
  /*
   *  Create some local variables to make the code a bit easier to read
   */
  e_callflow_kind = z_umts_cp_gsm_session_status.e_callflow_kind;
  e_agps_mode     = z_umts_cp_gsm_session_status.e_agps_mode;

  /*
   *  Our "normal" state machine timer expired
   *  Handle the failure in different ways depending on the current
   *  callflow state
   */
  MSG_HIGH("CP-GSM: callflow timer expired", 0, 0, 0); 

  switch( z_umts_cp_gsm_session_status.e_cf_state )
  {
    case C_UMTS_CP_GSM_CF_STATE_WAIT_RRLP:
      /*
       *  No hope of completing the session, force early termination
       */
      tm_umts_cp_gsm_callflow_disrupt_handler();
      break;

    case C_UMTS_CP_GSM_CF_STATE_TIMER_RRLP_MEAS_RESP_TIME:
    
      /* If there is cached GPS measrement, in a MSA session, report that
         cached measurement. This is used for E911 MsAssisted mode because
         no measurement is to be sent to network until RRLP responseTime times
         out. A latest measurement is cached in case reception deteriorates 
         approaching the end of the session. Note for MSB, similar caching 
         mechnism is done in LM */   
      if ( ( e_agps_mode == C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED ) &&
           ( z_umts_cp_gsm_session_status.z_cached_gps_meas.u_valid )
         )  
      {
        tm_umts_cp_gsm_rrlp_send_cached_gps_meas();
      }
      
      else
      {
    
        /*
         *  Send an error message to the network
         */
        (void) tm_umts_cp_gsm_tx_rrlp_mpr_err( rrlp_notEnoughSats );

        /*
         *  IF MOMA session THEN
         *    just go to the WAIT_RETURN_RESULT state and wait for a fix
         *  ELSE IF MT session THEN
         *    Stop the session and return the appropriate error to TM
         *  ELSE this is an MOMB session THEN
         *    Do nothing here.  Just let PDAPI QOS take over
         *  ENDIF
         */
        if( (e_callflow_kind == C_UMTS_CP_GSM_SESSION_UI) &&
            (e_agps_mode == C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED) )
        {
          tm_umts_cp_gsm_set_cf_state(C_UMTS_CP_GSM_CF_STATE_WAIT_RETURN_RESULT);
        }
        else if (e_callflow_kind == C_UMTS_CP_GSM_SESSION_NI)
        {
          if( e_agps_mode == C_UMTS_CP_GSM_AGPS_MODE_UE_ASSISTED )
          {
            tm_umts_cp_gsm_inform_core_stop_session( TM_STOP_REASON_PRM_TIMEOUT );
          }
          else
          {
            tm_umts_cp_gsm_inform_core_stop_session( TM_STOP_REASON_FIX_TIMEOUT );
          }
        }
        else
        {
          /*
           *  MOMB - Do nothing and let PDAPI QOS take over
           */
        }
      } /* if NOT ( msAssisted && gps-meas. cached ) */
        
      break;

    case C_UMTS_CP_GSM_CF_STATE_WAIT_RETURN_RESULT:
      /*
       *  IF Location Estimate session THEN
       *    Kill the session.  
       *  ELSE
       *    Done with assistance data, continue with session
       *  ENDIF
       */
      if( z_umts_cp_gsm_session_status.e_mo_lr_type == LOCATION_ESTIMATE )  /* enum 0 */
      {
        tm_umts_cp_gsm_inform_core_stop_session( TM_STOP_REASON_FIX_TIMEOUT );
      }
      else
      {
        if ( z_umts_cp_gsm_session_status.u_pdapi_fix_reported )
        {
         tm_umts_cp_gsm_inform_core_stop_session( TM_STOP_REASON_COMPLETED );
        }
        else
        {
          tm_umts_cp_gsm_inform_core_nt_assist_end();
        }
      }
      break;

    case C_UMTS_CP_GSM_CF_STATE_NONE:
    case C_UMTS_CP_GSM_CF_STATE_STARTED:
    default:
      MSG_HIGH("CP-GSM: Timer Expiration Ignored", 0, 0, 0); 
      break;
  }
}

/*===========================================================================

FUNCTION tm_umts_cp_gsm_stop_prm

DESCRIPTION  This function sends a STOP PRM request to LM via TM Core.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - PRM Stop request sent successfully

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_umts_cp_gsm_stop_prm (void)
{
  tm_sess_req_param_u_type req_param;

  req_param.stop_param.stop_type    = TM_STOP_TYPE_OPERATION;
  req_param.stop_param.stop_reason  = TM_STOP_REASON_COMPLETED;
  req_param.stop_param.op_to_stop   = TM_OP_REQ_PRM;

  if(tm_sess_req(TM_PRTL_TYPE_UMTS_CP,
             z_umts_cp_gsm_session_status.tm_active_session_handle,
             TM_SESS_REQ_STOP,
             &req_param))
  {
    MSG_MED("tm_umts_cp_gsm_stop_prm Succeeded", 0, 0, 0);  
    return TRUE;
  }
  else
  {
    MSG_ERROR("tm_umts_cp_gsm_stop_prm Failed", 0, 0, 0);
    return FALSE;
  }     
} /* tm_umts_cp_gsm_stop_prm */


#endif  /* FEATURE_CGPS_UMTS_CP_GSM */

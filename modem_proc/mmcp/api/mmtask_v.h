#ifndef _MM_TASK_V_H
#define _MM_TASK_V_H
/*===========================================================================
                          MM Interface

DESCRIPTION

 This module defines contains the defines, structures and function
 prototypes required to send a command to the MM task.


EXTERNALIZED FUNCTIONS

 mm_put_cmd           Put a MM message on the MM command queue
 mm_get_cmd_buf       Get a MM command buffer

 mm_rrc_get_cmd_buf   RRC specific get - fills in msg set and length

 mm_timer_expiry      Callback function when a MM timer expires

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001,2002,2003 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmtask.h_v   1.20   18 Jul 2002 11:41:56   vdrapkin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/mmtask_v.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/25/12   us      Definig new MACRO STOP_TDSCDMA_MODE_CNF_VAL & assigning corresponding value to RRC_STOP_TDSCDMA_MODE_CNF.
30/08/11   MNK     Add dual RX CSFB support 
12/11/00   TS      Initial Revision.
12/18/00   SK      Added RRC commands, enum types and rrcmm_get_cmd_buf
04/12/01   jca     Added more RRC commands.
05/03/01   jca     Added ref to rrc_act_cnf_type.
05/11/01   jca     Added RRC_DEACT_CNF.
05/23/01   jca     Added ref to rrc_close_session_cnf_type.
05/25/01   jca     Added SM/SMS/RABM interface definitions.
06/13/01   jca     Removed RRC_UNIT_DATA_IND and RRC_OPEN_SESSION_CNF.
08/15/01   ab      Renamed CM to CNM.
10/19/01   sbk     New function prototypes for plmn matching to be used by RRC
02/20/02   sbk     Added TC types to the mm command type
06/05/02   ks      Added new primitives and commands for supporting dual mode operation.
07/01/02   jca     Removed timer_enquiry from mm_cmd_type.
07/08/02   vdr     Added LLC_UNITDATA_IND primitive
07/18/02   vdr     Renamed FEATURE_GPRS to be FEATURE_GSM_GPRS compilation flag
07/31/02   jca     Externed mm_main() for use by TMC task.
09/10/02   vdr     Added prototype of the mm_gllc_get_cmd_buf()
09/23/02   mks     Added mmr_act_req as a member in union mm_int_cmd_type_T
10/10/02   mks     Added RRC_ACTIVATION_IND primitive to support GSM to WCDMA
                   Inter-RAT procedures
10/11/02   cd      Added rrc_stop_wcdma_mode_cnf to the command union
10/17/02   cd      enclosed rrc_stop_wcdma_mode_cnf within #ifdef FEATURE_INTERRAT_HANDOVER_WTOG
11/12/02   jca     Added RRC_CLOSE_SESSION_CNF.
12/06/02   mks     Added rr_gmm_gprs_suspension to the command union
01/10/03   jca     Added rr_activation_ind to the command union.
01/16/03   ks      Added gmmsm_session_close_req primitive between GMM-SM
02/12/03   vdr     Added sms_bearer_selection_req primitive between MM-WMS
02/12/03   vdr     Added CM->MM interface definitions to support PS Data Continuity feature
05/13/03   npr     Added gllcmm_llgmm_new_tlli_ind_type to mm_int_cmd_type_T.
06/04/03   ks      Added RRC_MODE_CHANGE_CNF for support MODE_CHANGE in UMTS
07/16/03   jca     Added rr_gmm_gprs_resumption_ind_T to the command union.
11/17/03   vdr     Added rrc_deact_cnf_type to the command union.
04/06/04   cd      LCS support
06/08/04   cd      LCS support. Removed rr_lcs_active_ind and added lcs_active_ind
08/12/04   cd      Replaced FEATURE_LCS by FEATURE_MMGPS
11/16/04   sv      Addition of a primitive - RRC_SERVICE_IND_FROM_OTA_MSG
06/06/05   abe     Added code for CCBS support
07/21/05   up      Added mmr_eq_plmn_change_ind in mm_cmd_type.
07/22/05   abe     Added support for RRC_WTOG_CCO_COMPLETE_IND.
10/08/05   abe     Added support for FEATURE_INTERRAT_PCCO_GTOW
12/14/05   abe     Supported changes for FEATURE_GSM_DTM
01/02/06   up      Added mmr_plmn_search_abort_reg in mm_cmd_type.
04/06/06   sn      Added RRC_HSDPA_HSUPA_CELL_IND in rrc_to_mm_sink_e_type.
                   Added rrc_3g_availability_cell_ind in mm_int_cmd_type_T
03/06/07   vsm   Added a primitive from RABM to GMM gmmrabm_rab_rel_ind_T to indicate RAB release by
                   AS for an active PDP.
10/10/11   HC      Removed de-featurization for the following structs in mm_int_cmd_type_tag 
                   rrc_hsdpa_hsupa_cell_ind_type,rrc_hspa_call_status_ind_type,
                   rrc_conn_mode_service_status_ind_type,rrc_wtog_cco_complete_ind_type,
                   rrc_fast_dormancy_cnf_type,rrc_disable_bplmn_ind_type  
11/21/11   gps     Removed refrence for header file "gsdi_exp.h" 
12/21/11   gps     FREE FLOAT FEATURE changes 
03/13/12   gps     defeaturization of public header files
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "rex.h"
#include "queue.h"
#include "sys_type.h"
#include "sys_cnst.h"
#include "sys_stru.h"
#include "timers.h"
#include "rrcmmif.h"
#include "timers.h"
#include "ms_timer.h"
#include "ms.h"      /* for the message set id MS_MM_RRC */
#include "mm_rr.h"
#include "reg_mm.h"
#include "cc_iei.h"
#include "ms_cc_mm.h"
#include "sm_gmm.h"
#include "rabm_gmm.h"
#include "sms_gmm.h"

#include "gllcmmif.h"
#include "gllcsmsif.h"


#include "tc_mm.h"
#include "tc_gmm.h"


#include "mm_lsm.h"

#include "mmgsdilib_common.h"
#include "modem_mem.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


typedef struct 
{
  byte               message_set;
  byte               message_id;
  byte               message_len_lsb;
  byte               message_len_msb;
  mmgsdi_return_enum_type     gsdi_status;   /* Status of Command              */
  uint32                      client_ref;    /* User Data assigned by client   */
} sim_mm_gsdi_rsphdr_T;

typedef struct 
{
  sim_mm_gsdi_rsphdr_T    message_header;
  byte                    returned_data_len;  /* Number of bytes returned in data */
  byte                    data[MMGSDI_MAX_DATA_BLOCK_LEN];   /* Data Buffer */
  sys_modem_as_id_e_type  as_id;

} sim_mm_auth_type_T;

typedef union
{
  sim_mm_gsdi_rsphdr_T    message_header;
  sim_mm_auth_type_T      run_gsm_alg_cnf;
  sim_mm_auth_type_T      usim_auth_cnf;
} sim_mm_gsdi_cnf_T;

#define STOP_TDSCDMA_MODE_CNF_VAL 116

typedef enum
{
   /* RRC  ==> MM */
   RRC_EST_CNF = 100,
   RRC_REL_IND,
   RRC_SYNC_IND,
   RRC_SERVICE_CNF,
   RRC_SERVICE_IND,
   RRC_SERVICE_IND_FROM_OTA_MSG,
   RRC_PLMN_LIST_CNF,
   RRC_ABORT_IND,
   RRC_PAGE_IND,
   RRC_DATA_IND,
   RRC_OPEN_SESSION_CNF,
   RRC_CLOSE_SESSION_IND,
   RRC_CLOSE_SESSION_CNF,
   RRC_ABORT_CNF,
   RRC_DEACT_CNF,
   RRC_MODE_CHANGE_CNF,
   RRC_STOP_WCDMA_MODE_CNF,
   RRC_STOP_TDSCDMA_MODE_CNF = STOP_TDSCDMA_MODE_CNF_VAL,
   RRC_ACTIVATION_IND,
   RRC_INTERRAT_CELL_CHANGE_IND,

   RRC_HSDPA_HSUPA_CELL_IND,

   RRC_HSPA_CALL_STATUS_IND,



   RRC_CONNECTED_MODE_SERVICE_STATUS_IND,


   RRC_FAST_DORMANCY_CNF,


   /*
    * MM Internal cmd same as RR_DATA_IND
    * RRC DATA IND will be transformed into MM_DATA_IND
    */
   MM_DATA_IND,
   

   /* RRC sends this primitive after WtoG CCO is successful 
   */
   RRC_WTOG_CCO_COMPLETE_IND, 


  RRC_MBMS_CONN_REQ_IND,


  RRC_SUITABLE_SEARCH_END_IND,
  RRC_DISABLE_BPLMN_IND,
  RRC_SRVCC_HANDOVER_FAILURE_IND,
  RRC_PCH_STATE_IND,

   RRC_MM_SINK_CMD_MAX
} rrc_to_mm_sink_e_type;

typedef struct
{
   IMH_T message_header;
   byte  *L3_data;
} mm_data_ind_T;

typedef enum
{
  MMCM_PS_DATA_AVAILABLE_REQ,
  MMCM_DUAL_RX_1XCSFB_TAU_REQ,
  MMCM_IMS_REGISTRATION_INFO,
  MMCM_CS_CALL_STATUS,
  MMCM_UE_CAPABILITIES_INFO,
  MMCM_MMTEL_CALL_INFO,
  MMCM_RPM_UPDATE_FTAI_LIST,
  MMCM_CMD_MAX
} cm_mm_cmd_e_T ;

typedef struct
{
  IMH_T message_header;

  sys_modem_as_id_e_type              as_id;

} cm_mm_ps_data_ready_req_T ;

typedef struct
{
  IMH_T message_header;

  sys_modem_as_id_e_type as_id;

} cm_mm_dual_rx_1xcsfb_tau_req_T ;

typedef struct
{
  IMH_T message_header;

  //currently this will be used for providing voice tag registration status
  boolean ims_registration_status; 

  sys_modem_as_id_e_type as_id; 
} cm_mm_ims_registration_info_T ;

typedef struct
{
  IMH_T message_header;

  boolean status; 

  sys_modem_as_id_e_type as_id; 
} cm_mm_cs_call_status_T ;

typedef struct
{
  IMH_T message_header;

  boolean srvcc_on_fly_value;/*0-disable 1-enable*/

  sys_modem_as_id_e_type as_id; 

} cm_mm_ue_capabilities_info_T ;

typedef struct
{
  IMH_T message_header;

  boolean voice_call_status;/*0-inactive 1-active*/

  boolean video_call_status;/*0-inactive 1-active*/

  boolean sms_status;/*0-inactive 1-active*/

  sys_modem_as_id_e_type as_id;

  sys_sys_mode_e_type  sys_mode;

} cm_mm_mmtel_call_info_T ;

typedef struct
{
  IMH_T message_header;

  sys_rpm_ftai_list_action_e_type ftai_list_action;
  sys_modem_as_id_e_type as_id;

} cm_mm_update_ftai_list_T;

/* Used for file attribute or file read/write  to REG from MMGSDI*/
typedef struct mm_sim_get_file_attr_cnf_s
{
  IMH_T               message_header;
  sys_modem_as_id_e_type              as_id;
  mmgsdi_get_file_attr_cnf_type          cnf_data;
}mm_sim_get_file_attr_cnf_s_type;


typedef struct mm_sim_read_file_cnf_s
{
  IMH_T               message_header;
  sys_modem_as_id_e_type              as_id;
  mmgsdi_read_cnf_type          cnf_data;
}mm_sim_read_file_cnf_s_type;

typedef struct mm_mcfg_refresh_ind_s
{
  IMH_T                      IMH;
  sys_modem_as_id_e_type     as_id;
  uint32                     reference_id;
}mm_mcfg_refresh_ind_s_type;

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/* mm internal command type */
typedef union mm_int_cmd_type_tag {
   /* Message with no data ex: RRC_EST_CNF */
   IMH_T hdr;

   /* REG ==> MM  */
   mmr_reg_req_s_type                 mmr_reg_req;
   mmr_gstk_service_ind_s_type        mmr_gstk_service_ind;
   mmr_sim_available_req_s_type       mmr_sim_available_req;
   mmr_sim_not_available_req_s_type   mmr_sim_not_available_req;
   mmr_stop_mode_req_s_type           mmr_stop_mode_req;
   mmr_plmn_search_req_s_type         mmr_plmn_search_req;
   mmr_eq_plmn_change_ind_s_type      mmr_eq_plmn_change_ind;
   mmr_plmn_search_abort_req_s_type   mmr_plmn_search_abort_reg;
   mmr_ph_status_change_req_s_type    mmr_ph_status_change_req;

   mmr_exit_from_pwr_save_s_type      mmr_exit_from_pwr_save;


    mmr_ecall_mode_ind_s_type          mmr_ecall_mode_ind;
	mmr_profile_type_ind_s_type         mmr_profile_type_ind;


  mmr_ps_detach_req_s_type             mmr_ps_detach_req;


   mmr_ds_stat_chgd_req_s_type        mmr_ds_stat_chgd_req;

   mmr_multimode_subs_chgd_req_s_type mmr_multimode_subs_chgd_req;

   mmr_ds_tuneaway_status_change_ind_s_type mmr_ds_tuneaway_status_change_ind;

   mmr_set_drx_req_s_type                  mmr_set_drx_req;
   mmr_get_drx_req_s_type                  mmr_get_drx_req;
   mmr_file_refresh_ind_s_type             mmr_file_refresh_ind;
   mmr_sim_state_update_ind_s_type         mmr_sim_state_update_ind;
   mmr_block_plmn_req_s_type               mmr_block_plmn_req;
   mmr_clear_lai_reject_list_req_s_type    mmr_clear_lai_reject_list_req;
   mmr_net_sel_mode_change_ind_s_type      mmr_net_sel_mode_change_ind;
   mm_ue_mode_ind_s_type                   mm_ue_mode_ind;
   mmr_sim_busy_ind_s_type                 mmr_sim_busy_ind;
   mmr_csg_select_config_update_ind_s_type mmr_csg_sel_cfg_update_ind;
   mmr_data_priority_ind_s_type             mmr_data_prio_ind;
   mmr_pseudo_list_update_ind_type         mmr_pseudo_list_update_ind;
   mmr_dds_switch_ind_s_type               mmr_dds_switch_ind;
   mmr_psm_ready_req_s_type                mmr_psm_ready_req;
   mmr_gw_plmn_sel_end_ind_s_type          mmr_gw_plmn_sel_end_ind;

   /* RRC ==> MM */
   rrc_est_cnf_type                 rrc_est_cnf;
   rrc_rel_ind_type                 rrc_rel_ind;
   rrc_sync_ind_type                rrc_sync_ind;
   rrc_service_cnf_type             rrc_service_cnf;
   rrc_service_ind_type             rrc_service_ind;
   rrc_service_ind_type             rrc_service_ind_from_ota_msg; 
   rrc_plmn_list_cnf_type           rrc_plmn_list_cnf;
   rrc_abort_ind_type               rrc_abort_ind;
   rrc_page_ind_type                rrc_page_ind;
   rrc_data_ind_type                rrc_data_ind;
   rrc_open_session_cnf_type        rrc_open_session_cnf;
   rrc_close_session_ind_type       rrc_close_session_ind;
   rrc_close_session_cnf_type       rrc_close_session_cnf;
   rrc_abort_cnf_type               rrc_abort_cnf;
   rrc_deact_cnf_type               rrc_deact_cnf;

   rrc_stop_wcdma_mode_cnf_type     rrc_stop_wcdma_mode_cnf;

   rrc_interrat_cell_change_ind_type rrc_interrat_cell_change_ind;
   rrc_activation_ind_type          rrc_activation_ind;
   rrc_hsdpa_hsupa_cell_ind_type    rrc_3g_availability_cell_ind;
   rrc_hspa_call_status_ind_type    rrc_hspa_call_status_ind;
   rrc_conn_mode_service_status_ind_type rrc_conn_mode_service_status_ind;
   rrc_wtog_cco_complete_ind_type   rrc_wtog_cco_complete_ind;
   rrc_fast_dormancy_cnf_type  rrc_fast_dormancy_cnf;
   rrc_disable_bplmn_ind_type rrc_disable_bplmn_ind;
   rrc_suitable_search_end_ind_type rrc_suitable_search_end_ind;
   rrc_srvcc_handover_failure_ind_type rrc_srvcc_handover_failure_ind;
   rrc_pch_state_ind_type     rrc_pch_state_ind;
   /* RR ==> MM */
   rr_abort_cnf_T                   rr_abort_cnf;
   rr_abort_ind_T                   rr_abort_ind;
   rr_gmm_gprs_service_cnf_T        rr_gprs_service_cnf;
   rr_gmm_gprs_multislot_class_ind_T          rr_gmm_gprs_multislot_class_ind;
   rr_ds_data_ind_T                 rr_ds_data_ind;
   rr_reselection_ind_T             rr_reselection_ind;
   rr_gprs_pccch_camp_cnf_T         rr_gprs_pccch_camp_cnf;
   rr_ps_abort_cnf_T                rr_ps_abort_cnf;

   rr_data_ind_T                    rr_data_ind;
   rr_deact_cnf_T                   rr_deact_cnf;
   rr_est_cnf_T                     rr_est_cnf;
   rr_paging_ind_T                  rr_paging_ind;
   rr_plmn_list_cnf_T               rr_plmn_list_cnf;
   rr_plmn_select_cnf_T             rr_plmn_select_cnf;
   rr_rel_ind_T                     rr_rel_ind;
   rr_service_ind_T                 rr_service_ind;

   rr_service_ind_T                 rr_service_ind_from_ota_msg;

   rr_sync_ind_T                    rr_sync_ind;
   rr_stop_gsm_mode_cnf_T           rr_stop_gsm_mode_cnf;
   rr_activation_ind_T              rr_activation_ind;

   rr_gmm_gprs_suspension_T         rr_gmm_gprs_suspension;
   rr_gmm_gprs_resumption_ind_T     rr_gmm_gprs_resumption_ind;



   rr_gtow_cco_complete_ind_T       rr_gtow_cco_complete_ind; 


   rr_est_ind_T                     rr_est_ind;



   rr_psho_ind_T                     rr_psho_ind;
   rr_ps_status_change_ind_T         rr_ps_status_change_ind;
   rr_srvcc_handover_failure_ind_T   rr_srvcc_handover_failure_ind;

   rr_multimode_subs_chgd_cnf_T      rr_multimode_subs_chgd_cnf;


    gllcmm_llgmm_status_ind_type       llgmm_status_ind;
    gllcmm_llgmm_trigger_ind_type      llgmm_trigger_ind;
    gllcmm_llgmm_rdy_tmr_ind_type      llgmm_rdy_tmr_ind;
    gllcmm_llgmm_suspend_cnf_type      llgmm_suspend_cnf;
    gllcmm_llgmm_new_tlli_ind_type     llgmm_new_tlli_ind;
    gllcmm_ll_unitdata_ind_type        ll_unitdata_ind;
    gllcmm_ll_xid_ind_type             ll_xid_ind;
    gllcmm_ll_xid_cnf_type             ll_xid_cnf;
    gllcmm_ll_reset_ind_type           ll_reset_ind;
    gllcmm_ll_status_ind_type          ll_status_ind;

   gllcmm_ll_psho_cnf_type             llgmm_psho_cnf;
   

    gllcsms_ll_unitdata_ind_type       llsms_unitdata_ind;
    gllcsms_ll_xid_ind_type            llsms_xid_ind;
    gllcsms_ll_xid_cnf_type            llsms_xid_cnf;
    gllcsms_ll_reset_ind_type          llsms_reset_ind;
    gllcsms_ll_status_ind_type         llsms_status_ind;


   /* CM/CC ==> MM */
   mmcnm_data_req_T            mmcnm_data_req;
   mmcnm_est_req_T             mmcnm_est_req;
   mmcc_reest_req_T            mmcc_reest_req;
   mmcnm_rel_req_T             mmcnm_rel_req;

   mmcnm_prompt_res_T          mmcnm_prompt_res;


   /* SM ==> GMM */
   gmmsm_establish_req_T      gmmsm_establish_req;
   gmmsm_unitdata_req_T       gmmsm_unitdata_req;
   gmmsm_session_close_req_T  gmmsm_session_close_req;
   gmmsm_rat_resume_cnf_T     gmmsm_rat_resume_cnf;
   gmmsm_ok_to_tuneaway_ind_T gmmsm_ok_to_tuneaway_ind;


    /* RABM ==> GMM */
   gmmrabm_reestablish_req_T  gmmrabm_reestablish_req;

   gmmrabm_rab_rel_ind_T      gmmrabm_rab_rel_ind;
   gmmrabm_rab_est_ind_T 	 gmmrabm_rab_est_ind;


   /* Timer messages from REX */
   timer_expiry_T             timer_expiry;

   /* GSDI ==> MM */

   sim_mm_gsdi_cnf_T          gsdi_cnf;


   /* Test Control ==> MM */
   mmtc_unitdata_req_T        mmtc_unitdata_req;

   /* Test Control ==> MM */
   gmmtc_unitdata_req_T       gmmtc_unitdata_req;



   /* LSM ==> MM */
   lcs_active_ind_T                 lcs_active_ind;


   /* SMS/WMS ==> GMM, for bearer selection request */
   mm_sms_bearer_selection_req_T sms_bearer_selection_req;

   wms_mm_sms_call_info_T  wms_mm_sms_call_info;

   /* CM ==> GMM, indicates that PS data is available */
   cm_mm_ps_data_ready_req_T  cm_mm_ps_data_ready_req ;

   /* MM Internal */
   mm_data_ind_T              mm_data_ind;


   sim_subs_ready_ind_s_type  sim_subs_ready_ind;

   rr_gmm_gprs_ms_ra_cap_req_T rr_gmm_gprs_ms_ra_cap_req;
   sim_mm_gsdi_cnf_T          sim_mm_gsdi_cnf;
   mm_sim_get_file_attr_cnf_s_type    mmgsdi_get_file_attr_cnf;
   mm_sim_read_file_cnf_s_type       mmgsdi_read_file_cnf;
   cm_mm_dual_rx_1xcsfb_tau_req_T  cm_mm_dual_rx_1xcsfb_tau_req;
   cm_mm_ims_registration_info_T  cm_mm_ims_registration_info;
   cm_mm_cs_call_status_T  cm_mm_cs_call_status;
   cm_mm_ue_capabilities_info_T  cm_mm_ue_capabilities_info;
   cm_mm_mmtel_call_info_T  cm_mm_mmtel_call_info;
   cm_mm_update_ftai_list_T cm_mm_update_ftai_list;

/* MCFG ==> MM */
   mm_mcfg_refresh_ind_s_type       mcfg_refresh_ind;

} mm_int_cmd_type_T;


typedef struct {

   q_link_type      link;

   mm_int_cmd_type_T cmd;

}  mm_cmd_type;



/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

extern mm_cmd_type *mm_get_cmd_buf(void);

/*===========================================================================

FUNCTION nas_mem_alloc

DESCRIPTION
  This function is called by the external modules like RRC in order to allocate 
  dynamic memory. This function simply calls mem_malloc and selects suitable
  heap and returs pointer to the allocated memory.

DEPENDENCIES
  The memory manager must have been previously initialized.

RETURN VALUE
  A pointer to the allocated memory if the memory was successfully allocated,
  otherwise NULL.

SIDE EFFECTS
  None.

===========================================================================*/

extern void * nas_mem_alloc(size_t size);


/* For WCDMA RRC's special usage */
extern mm_cmd_type *mm_rrc_get_cmd_buf(rrc_to_mm_sink_e_type cmd);

/* For TD-SCDMA RRC's special usage */
extern mm_cmd_type *mm_tdsrrc_get_cmd_buf(rrc_to_mm_sink_e_type cmd);

/* For CM commands */
extern mm_cmd_type *mm_cm_get_cmd_buf(cm_mm_cmd_e_T cmd_type);


extern mm_cmd_type *mm_gllc_get_cmd_buf( uint8 cmd );


extern void mm_put_cmd(mm_cmd_type *cmd_ptr);

extern void mm_free_cmd_buf( mm_cmd_type *cmd_buf );


/*
 * Function used by RRC to extract LAC from CN common information
 */
boolean  mm_extract_lac_from_sib_info(rrc_lai_type *lai,
                                      rrc_gsm_map_nas_sys_info_type *nas_common_info);


/* Is this broadcasted PLMN, a HPLMN? */
boolean Is_bcch_plmn_an_hplmn(rrc_plmn_identity_type plmn);

boolean mm_per_subs_is_bcch_plmn_an_hplmn
(
  rrc_plmn_identity_type plmn,
  sys_modem_as_id_e_type as_id
);

#ifdef FEATURE_FEMTO_CSG
/*===========================================================================
FUNCTION  mm_is_csg_search_allowed

DESCRIPTION
 This function accepts CSG search type, CSG selection configuration as
 parameters and decides if CSG search should be allowed.
 Search result is also populated to REG_MM_PLMN_SEARCH_ABORTED in case of
 search failure

DEPENDENCIES
  None 

RETURN VALUE
  TRUE: If CSG search allowed
  FALSE: If CSG search not allowed

SIDE EFFECTS
  None
===========================================================================*/
extern boolean mm_is_csg_search_allowed
(
  sys_csg_search_type_e_type          csg_search_type,
  sys_csg_selection_config_e_type     config_option,
  reg_mm_plmn_search_status_e_type    *search_result
);
#endif

#endif /* _MM_TASK_H */

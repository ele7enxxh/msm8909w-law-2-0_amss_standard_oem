#ifndef REG_MM_H
#define REG_MM_H
/*==============================================================================


                     R E G  -  M M   H E A D E R   F I L E


                                  DESCRIPTION

  This header file contains message definitions for the REG to MM interface.

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_mm.h_v   1.12   07 Jun 2002 14:22:12   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/reg_mm.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/04/01   jca     Fixed LINT errors.
05/25/01   sbk     Updated to reflect changes to REG<->MM interface spec.
08/16/01   ab      Renamed mmi_reg.h references to reg_cm.  Renamed MMI to CM.
02/13/02   kwa     CR19136 - Added reg_svc_type to mmr_service_ind.
02/20/02   kwa     Updated interface to support dual-mode design requirements.
05/20/02   kwa     Replaced mode_pref with dual_mode in mmr_reg_req.
05/29/02   kwa     Replaced rat_search_order with rat in mmr_reg_req.
06/05/02   kwa     Added update_equivalent_plmn_list to mmr_reg_cnf.
06/06/02   kwa     Replaced mode_pref with dual_mode and rat in
                   mmr_plmn_search_req.
06/27/02   kwa     Updated mmr_plmn_search_req, mmr_reg_req, mmr_reg_cnf, and
                   mmr_service_ind.
09/19/02   kwa     Added mmr_act_req.
12/31/02   kwa     Added update_equivalent_plmn_list and equivalent_plmn_list
                   to the mmr_service_ind message.
05/22/03   kwa     Modified message verification functions to pass const
                   pointers to eliminate lint warnings. 
08/12/03   kwa     Added MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED.
09/18/03   kwa     Added sys_band_mask_type to cm_act_req_s_type, 
                   cm_network_list_req_s_type, and cm_service_req_s_type.
12/03/03   kwa     Added plmn_search_result parameter to mmr_plmn_search_cnf.
02/04/04   kwa     Added ps_for_request parameter to mmr_reg_req_s_type.
04/06/04   kwa     Added lai_forbidden parameter to mmr_reg_req_s_type and
                   mmr_service_ind_s_type.  This change is featurized using
                   FEATURE_NAS_REMAIN_ON_FORBIDDEN_LAI.
05/12/04   kwa     Added cs_for_request parameter to mmr_reg_req_s_type.
                   Added mmr_camped_ind.
07/19/04   kwa     Removed service_state parameter from mmr_camped_ind.
07/26/04   kwa     Added active_rat and active_band parameters to
                   mmr_camped_ind.
11/04/04   kwa     Added higher_priority_plmn_list parameter to
                   mmr_plmn_search_req.
07/15/05   up      Added function mmr_eq_plmn_change_ind_is_valid and primitive
                   MMR_EQ_PLMN_CHANGE_IND to indicate EPLMN change ind to MM.
10/24/05   up      Added network_selection_mode in mmr_plmn_search_cnf.
                   Kept under FEATURE_WTOW/WTOG_BACKGROUND_PLMN_SEARCH.
10/29/05   ajt     Added data-types to communicate emergency numbers (valid in 
                   the country of currently registered PLMN) from MM to REG.                   
12/07/05   up      Added MMR_CAUSE_FAILURE_GPRS_FORBIDDEN_REMAIN_ON_PLMN
                   casue in mmr_cause_e_type.
01/02/06   up      Added message mmr_plmn_search_abort_req_s_type and
                   primitive MMR_PLMN_SEARCH_ABORT_REQ.
01/18/06   up      transaction_id is added in mmr_plmn_search_req/cnf_s_type.
02/23/06   up      Added MMR_CAUSE_FAILURE_REJECT_REMAIN_ON_PLMN
04/06/06   sn      Added MMR_CELL_SERVICE_IND in reg_to_mm_cmd_type
                   Added mmr_cell_service_ind_s_type,
                   mmr_cell_service_ind_is_valid.
07/27/06   sn      Added Uniform OOS changes.
                   FEATURE_UMTS_UNIFORM_OOS_HANDLING
09/20/06   sn      Added MMR_CAUSE_MAX_ATTEMPTED.
10/12/06   sn      Added WCDMA_1X_support.
11/23/06   sn      Added feature FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
12/15/09   abhi   Removing UOOS code
01/31/11   rajesh  voice domain preference and UE usage setting related changes
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "comdef.h"
#include "customer.h"
#include "sys.h"
#include "sys_v.h"
#include "sys_plmn_selection.h"
#include "sys_stru.h"

#include "reg_sim.h"
#include "rex.h"

#include "mmgsdilib.h"



/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

typedef enum
{
  MMR_PLMN_SEARCH_REQ       = 0x02,
  MMR_REG_REQ               = 0x03,
  MMR_SIM_AVAILABLE_REQ     = 0x04,
  MMR_SIM_NOT_AVAILABLE_REQ = 0x05,
  MMR_STOP_MODE_REQ         = 0x06,
  MMR_EQ_PLMN_CHANGE_IND    = 0x07,
  MMR_PLMN_SEARCH_ABORT_REQ = 0x08,
  MMR_PH_STATUS_CHANGE_REQ  = 0x09,

  MMR_EXIT_FROM_PWR_SAVE    = 0x0A,

  MMR_DS_STAT_CHGD_REQ      = 0x0B,

  MMR_DS_TUNEAWAY_STATUS_CHANGE_IND = 0x0C,

  MMR_PS_DETACH_REQ = 0x0D,
  MMR_GSTK_SERVICE_IND = 0x0E,
  MMR_SET_DRX_REQ = 0x0F,
  MMR_GET_DRX_REQ = 0x10,
  MMR_CLEAR_LAI_REJECT_LIST_REQ = 0x11,
  MMR_UE_MODE_CHANGE_IND = 0x12,
  MMR_BLOCK_PLMN_REQ = 0x13,
  MMR_SIM_STATE_UPDATE_IND  = 0x14,
  MMR_MULTIMODE_SUBS_CHGD_REQ = 0x15,
  MMR_DATA_PRIORITY_IND       = 0x16,
  MMR_DDS_SWITCH_IND          = 0x18,
  MMR_GW_PLMN_SEL_END_IND     = 0x1A,

  MMR_MM_INFORMATION_IND    = 0x81,
  MMR_PLMN_SEARCH_CNF       = 0x82,
  MMR_REG_CNF               = 0x83,
  MMR_SERVICE_IND           = 0x84,
  MMR_SIM_AVAILABLE_CNF     = 0x85,
  MMR_SIM_NOT_AVAILABLE_CNF = 0x86,
  MMR_STOP_MODE_CNF         = 0x87,
  MMR_CAMPED_IND            = 0x88,
  MMR_EMERGENCY_NUM_LIST_IND= 0x89,
  MMR_CELL_SERVICE_IND      = 0x8A,

  MMR_HSPA_CALL_STATUS_IND  = 0x8B,

  MMR_PH_STATUS_CHANGE_CNF  = 0x8C 

  ,MMR_CONN_MODE_SERVICE_STATUS_IND = 0x8D

  ,MMR_REG_REJECT_IND = 0x8E

 ,MMR_SUITABLE_SEARCH_END_IND = 0x8F


  ,MMR_ECALL_MODE_IND = 0x90 


  ,MMR_DISABLE_BPLMN_IND = 0x91

  ,MMR_DS_STAT_CHGD_CNF     = 0x92
  ,MMR_CELL_ACCESS_IND      = 0x93
  ,MMR_PS_DETACH_CNF = 0x94
  ,MMR_FPLMN_LIST_CHANGE_IND = 0x95
  ,MMR_SET_DRX_CNF = 0x96
  ,MMR_GET_DRX_CNF = 0x97
  ,MMR_FILE_REFRESH_IND = 0x98
  ,MMR_ATTACH_FAILED_IND = 0x99
  ,MMR_UPDATE_CURR_RAT_LIST_IND = 0x9A  
  ,MMR_STACK_DEACT_IND= 0x9B
  ,MMR_STACK_ACT_IND=0x9C
  ,MMR_SERVICE_STAT_IND=0x9D
  ,MMR_LTE_CELL_INFO_IND = 0x9E
  ,MMR_HOME_NODEB_NAME_IND = 0x9F
  ,MMR_MULTIMODE_SUBS_CHGD_CNF = 0xA0  
  ,MMR_NET_SEL_MODE_CHANGE_IND = 0xA1  
  ,MMR_SIM_BUSY_IND   = 0xA2
  ,MMR_CSG_SELECT_CONFIG_UPDATE_IND = 0xA3
  ,MMR_CSG_SEARCH_TRIGGER_IND = 0xA4
  ,MMR_CSG_LIST_READ_COMPLETE_IND = 0xA5
  ,MMR_PSEUDO_LIST_UPDATE_IND = 0xA6
  ,MMR_T3346_TIMER_IND = 0xA7
  ,MMR_RRC_SIB6_INFO_IND = 0xA8
  ,MMR_RRC_SIB7_INFO_IND = 0xA9
  ,MMR_PROFILE_TYPE_IND = 0xAA
  ,MMR_LTE_PESUDO_RESEL_LOW_PRIORITY_IND = 0xAB
  ,MMR_REG_REQ_IGNORED_IND = 0xAC
  ,MMR_PSM_READY_REQ = 0xAD
  ,MMR_PSM_READY_RSP = 0xAE
  ,MMR_PSM_READY_IND = 0xAF
 } reg_to_mm_cmd_type;


typedef enum mmr_cause_e
{
  MMR_CAUSE_NONE = -1,                              /* FOR INTERNAL USE ONLY! */
  MMR_CAUSE_NOT_SPECIFIED,
  MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED,
  MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED_FOR_GPRS_SERVICES,
  MMR_CAUSE_FAILURE_INITIATE_PLMN_SELECTION,
  MMR_CAUSE_FAILURE_REMAIN_ON_PLMN,
  MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED,
  MMR_CAUSE_FAILURE_GPRS_FORBIDDEN_REMAIN_ON_PLMN,
  MMR_CAUSE_FAILURE_REJECT_REMAIN_ON_PLMN,    /* Used when MM rejects REG_REQ */

  MMR_CAUSE_FAILURE_FOREGROUND_PLMN_SEARCH,

  MMR_CAUSE_FAILURE_MAX_ATTEMPTED,
  MMR_CAUSE_INITIATE_PLMN_SELECTION_ON_GW,
  MMR_CAUSE_ABORT_PLMN_SELECTION,
  MMR_CAUSE_PLMN_SEL_CALL_END,
  MMR_CAUSE_REQ_CAMPED_PLMN_MISMATCH,
  MMR_CAUSE_MAX                                     /* FOR INTERNAL USE ONLY! */
} mmr_cause_e_type;

typedef enum mmr_reg_req_e
{
  MMR_REG_REQ_NONE = -1,
  MMR_REG_REQ_USER_SELECTION,
  MMR_REG_REQ_SYSTEM_CHANGE,
  MMR_REG_REQ_NORMAL,
  MMR_REG_REQ_IRAT_TO_LTE,
  MMR_REG_REQ_BETTER_SYSTEM_RESELECTION,
  MMR_REG_REQ_EF_RAT_REFRESH,
  MMR_REG_REQ_ACQ_GW,
  MMR_REG_REQ_SRV_MODE_UPDATE,
  MMR_REG_REQ_SGLTE_PSTIMER_EXPIRY,
  MMR_REG_REQ_SGLTE_PS_ONLY_ACQUISITION,
  MMR_REG_REQ_ACQ_DB_SCAN,
  MMR_REG_REQ_CSG_SELECTION_APP,
  /* Maps to REG_CM_SERVICE_REQ_FULL_RAT_MODE
   * service type for full RAT mode - specific to Limited Service 
   * initial reason to introduce - to let NAS know that current service req is to determine MCC
   * NAS need not send IDLE event to card at this point.
   * */  
  MMR_REG_REQ_FULL_RAT_MODE,
  MMR_REG_REQ_ACQ_HPLMN_OR_RPLMN_RAT_SEARCH_ORDER,
  MMR_REG_REQ_MAX
} mmr_reg_req_e_type;


typedef enum mmr_ecall_mode_e
{
  MMR_ECALL_MODE_NONE = -1,               /* FOR INTERNAL USE ONLY! */
  MMR_ECALL_MODE_NORMAL,
  MMR_ECALL_MODE_ECALL_ONLY,
  MMR_ECALL_MODE_MAX                      /* FOR INTERNAL USE ONLY! */
} mmr_ecall_mode_e_type;

typedef enum
{
  REG_MM_PLMN_SEARCH_NONE = -1,       /* FOR INTERNAL USE ONLY */
  REG_MM_PLMN_SEARCH_SUCCESS,
  REG_MM_PLMN_SEARCH_TIME_OUT,
  REG_MM_PLMN_SEARCH_ABORTED,
  REG_MM_PLMN_SEARCH_ABORTED_ON_RLF,
  REG_MM_PLMN_SEARCH_PARTIAL                           
} reg_mm_plmn_search_status_e_type;


/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/*                           REG to MM Primitives                             */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/


/******************************************************************************/
/*                                                                            */
/*  REG_MM_LTE_CELL_INFO_S                                                       */
/*                                                                            */
/******************************************************************************/

typedef struct reg_mm_lte_cell_info_s
{
  boolean  emergency_access_barred;
}reg_mm_lte_cell_info_s_type;



/******************************************************************************/
/*                                                                            */
/*  MMR_PLMN_SEARCH_REQ                                                       */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_plmn_search_req_s
{
  IMH_T                                     message_header;
  sys_network_selection_mode_e_type         network_selection_mode;
  boolean                                   dual_mode;
  sys_radio_access_tech_e_type              rat;
  sys_band_mask_type                        band_pref;
  sys_plmn_rat_list_s_type                  higher_priority_plmn_list;
  byte                                      transaction_id;
  sys_service_search_e_type                 service_search;  
  /*BST(Band Search Table) requirement for Lower layer to search on Limited bands depending upon mcc*/

  sys_rat_pri_list_info_s_type              mcc_mode_band_filter;     
  sys_csg_search_type_e_type                csg_search_type;
  sys_rat_pri_list_info_s_type              rat_pri_list_info;
  sys_scan_scope_e_type                     scan_scope; 
  sys_modem_as_id_e_type                    as_id;
} mmr_plmn_search_req_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_REG_REQ                                                               */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_reg_req_s
{
  IMH_T                                     message_header;
  sys_network_selection_mode_e_type         network_selection_mode;
  sys_plmn_id_s_type                        plmn;
  sys_srv_domain_e_type                     req_service_domain;
  boolean                                   dual_mode;
  sys_radio_access_tech_e_type              rat;
  boolean                                   cs_for_request;
  boolean                                   ps_for_request;

  sys_band_mask_type                        band_pref;


  sys_rat_pri_list_info_s_type              rat_pri_list_info;

  sys_rat_pri_list_info_s_type              original_rat_pri_list_info;

  sys_wcdma_1x_mode_pref_e_type             wcdma_1x_mode;

  mmr_reg_req_e_type                        type;
  boolean                                   is_split_needed;               
  boolean                                   grat_scan_done;
  sys_mode_pref_e_type                      mode_pref;

  sys_modem_as_id_e_type                    as_id;

  boolean                                   sms_only;
  sys_lte_cs_domain_param_s_type            lte_cs_domain_param;

  sys_scan_scope_e_type                     scan_scope;    
  sys_csg_id_type                           csg_id;

  uint32                                    additional_info;

  uint32                                   trm_timeout;
  boolean                                  is_volte_only_ps;
} mmr_reg_req_s_type;


typedef struct mmr_gstk_service_ind_s
{
  IMH_T                                     message_header;
  boolean                                   service_available ;
  sys_modem_as_id_e_type                    as_id;
} mmr_gstk_service_ind_s_type;

typedef struct mmr_stack_deact_ind_s
{
IMH_T message_header;
sys_modem_as_id_e_type as_id;
}mmr_stack_deact_ind_s_type;

typedef struct mmr_stack_act_ind_s
{
IMH_T message_header;
sys_modem_as_id_e_type as_id;
}mmr_stack_act_ind_s_type;

typedef struct mmr_service_stat_ind_s
{
IMH_T message_header;
sys_plmn_service_state_s_type service_state;

sys_modem_as_id_e_type as_id;

} mmr_service_stat_ind_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_SIM_AVAILABLE_REQ                                                     */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_sim_available_req_s
{
  IMH_T                                     message_header;

  uint8                                     nv_context_id;
  sys_mode_pref_e_type                      mode_pref;
  mmgsdi_session_type_enum_type             mmgsdi_session_type;
  mmgsdi_client_id_type                     mmgsdi_client_id;
  sys_modem_as_id_e_type                    as_id;

} mmr_sim_available_req_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_SIM_NOT_AVAILABLE_REQ                                                 */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_sim_not_available_req_s
{
  IMH_T                                     message_header;

  sys_modem_as_id_e_type                    as_id;

} mmr_sim_not_available_req_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_STOP_MODE_REQ                                                         */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_stop_mode_req_s
{
  IMH_T                                     message_header;
  sys_stop_mode_reason_e_type               stop_mode_reason;

  sys_modem_as_id_e_type                    as_id;

} mmr_stop_mode_req_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_PH_STAUS_CHANGE_REQ                                                         */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_ph_status_change_req_s
{
  IMH_T                                     message_header;
  sys_stop_mode_reason_e_type               stop_mode_reason;

  sys_modem_as_id_e_type                    as_id;

} mmr_ph_status_change_req_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_EXIT_FROM_PWR_SAVE                                                         */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_exit_from_pwr_save_s
{
  IMH_T                                     message_header;

  sys_modem_as_id_e_type                    as_id;

} mmr_exit_from_pwr_save_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_DS_STAT_CHGD_REQ                                                         */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_ds_stat_chgd_req_s
{
  IMH_T                                     message_header;
  boolean                                   dual_standby_mode;
  sys_modem_dual_standby_pref_e_type        dual_standby_mode_info;
  sys_modem_device_mode_e_type    device_mode;
} mmr_ds_stat_chgd_req_s_type;

/******************************************************************************/
/*                                                                            */
/*  MMR_MULTIMODE_SUBS_CHGD_REQ                                                         */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_multimode_subs_chgd_req_s
{
  IMH_T                                     message_header;
  subs_capability_e_type                    subs_capability[MAX_AS_IDS];
}mmr_multimode_subs_chgd_req_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_DDS_SWITCH_IND                                                        */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_dds_switch_ind_s
{
  IMH_T                                     message_header;
  sys_modem_as_id_e_type                    target_dds_sub;
}mmr_dds_switch_ind_s_type;

/******************************************************************************/
/*                                                                            */
/*  MMR_GW_PLMN_SEL_END_IND                                                        */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_gw_plmn_sel_end_ind_s
{
  IMH_T                                     message_header;
  sys_modem_as_id_e_type                    as_id;
}mmr_gw_plmn_sel_end_ind_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_DS_TUNEAWAY_STATUS_CHANGE_IND                                                         */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_ds_tuneaway_status_change_ind_s
{
  IMH_T                                     message_header;
  boolean                                   tuneaway_status;
} mmr_ds_tuneaway_status_change_ind_s_type;

/******************************************************************************/
/*                                                                            */
/*  MMR_EQ_PLMN_CHANGE_IND                                                    */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_eq_plmn_change_ind_s
{
  IMH_T                                     message_header;

  sys_modem_as_id_e_type                    as_id;

} mmr_eq_plmn_change_ind_s_type;

/******************************************************************************/
/*                                                                            */
/*  MMR_NET_SEL_MODE_CHANGE_IND                                                    */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_net_sel_mode_change_ind_s
{
  IMH_T                                     message_header;
  sys_network_selection_mode_e_type 		network_selection_mode;
  sys_modem_as_id_e_type                    as_id;

} mmr_net_sel_mode_change_ind_s_type;

/******************************************************************************/
/*                                                                            */
/*  MMR_PLMN_SEARCH_ABORT_REQ                                                 */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_plmn_search_abort_req_s
{
  IMH_T                                     message_header;

  sys_modem_as_id_e_type                    as_id;

} mmr_plmn_search_abort_req_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_ECALL_MODE_IND                                                 */
/*                                                                            */
/******************************************************************************/
typedef struct mmr_ecall_mode_ind_s
{
  IMH_T                                    message_header;
  mmr_ecall_mode_e_type                    mode_type;
  sys_modem_as_id_e_type                   as_id;
} mmr_ecall_mode_ind_s_type;

/******************************************************************************/
/*                                                                            */
/*  MMR_ECALL_MODE_IND                                                 */
/*                                                                            */
/******************************************************************************/
typedef struct mmr_profile_type_ind_s
{
  IMH_T                                    message_header;
  mmgsdi_profile_type_enum_type            profile_type;
  sys_modem_as_id_e_type                   as_id;
} mmr_profile_type_ind_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_SET_DRX_REQ                                                 */
/*                                                                            */
/******************************************************************************/
typedef struct mmr_set_drx_req_s 
{
  IMH_T                                     message_header;
  sys_drx_cn_coefficient_s1_e_type                    drx_coefficient;
  sys_modem_as_id_e_type                    as_id;
} mmr_set_drx_req_s_type;

/******************************************************************************/
/*                                                                            */
/*  MMR_GET_DRX_REQ                                          */
/*                                                                            */
/******************************************************************************/
typedef struct mmr_get_drx_req_s
{
  IMH_T                                      message_header;
  sys_modem_as_id_e_type                    as_id;
} mmr_get_drx_req_s_type;

/******************************************************************************/
/*                                                                            */
/*  MMR_FILE_REFRESH_IND                                                 */
/*                                                                            */
/******************************************************************************/
typedef struct mmr_file_refresh_ind_s
{
  IMH_T                                    message_header;
  sys_modem_as_id_e_type                    as_id;
  mmgsdi_file_enum_type       file_name;
} mmr_file_refresh_ind_s_type;

/******************************************************************************/
/*                                                                            */
/*  MMRM_BLOCK_PLMN_REQ                                                      */
/*                                                                            */
/******************************************************************************/
typedef struct mmr_block_plmn_req_s
{
  IMH_T                     message_header;
  sys_plmn_id_s_type        plmn;
  rex_timer_cnt_type        timer_count; /* In mili seconds*/
  boolean                   is_plmn_blocked_for_eplmn_check;
  sys_block_plmn_cause_e_type cause;
  sys_modem_as_id_e_type    as_id;
} mmr_block_plmn_req_s_type;

/******************************************************************************/
/*  MMR_SIM_STATE_UPDATE_IND                                                      */
/*                                                                            */
/******************************************************************************/
typedef struct mmr_sim_state_update_ind_s
{
  IMH_T                                      message_header;
  sys_modem_as_id_e_type                     as_id;
  sys_sim_state_e_type                       sim_state;
} mmr_sim_state_update_ind_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_CLEAR_LAI_REJECT_LIST_REQ                                             */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_clear_lai_reject_list_s
{
  IMH_T                                     message_header;

  sys_modem_as_id_e_type                    as_id;

} mmr_clear_lai_reject_list_req_s_type;

/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/*                           MM to REG Primitives                             */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*  MMR_CAMPED_IND                                                            */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_camped_ind_s
{
  IMH_T                                     message_header;
  sys_radio_access_tech_e_type              active_rat;
  /* Band for a given RAT is defined in a union type */
  sys_active_band_u_type                   active_band;

  sys_modem_as_id_e_type                    as_id;

  sys_plmn_id_s_type                        plmn;
  
  sys_lac_type                              lac_id;
  boolean                                     policy_change_pending;

} mmr_camped_ind_s_type;

/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/*                           MM to REG Primitives                             */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*  MMR_ATTACH_FAILED_IND                                                            */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_attach_failed_ind_s                                  
{
  IMH_T                                     message_header;

  sys_modem_as_id_e_type                    as_id;

} mmr_attach_failed_ind_s_type;
/******************************************************************************/
/*                                                                            */
/*  MMR_MM_INFORMATION_IND                                                    */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_mm_information_ind_s
{
  IMH_T                                     message_header;
  sys_mm_information_s_type                 info;

  sys_modem_as_id_e_type                    as_id;

} mmr_mm_information_ind_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_PLMN_SEARCH_CNF                                                       */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_plmn_search_cnf_s
{
  IMH_T                                     message_header;

  sys_network_selection_mode_e_type         network_selection_mode;

  reg_mm_plmn_search_status_e_type          plmn_search_result;
  sys_found_plmn_list_u_type                found_plmn_list;
  byte                                      transaction_id;

  sys_service_search_e_type                 service_search;  


  sys_modem_as_id_e_type                    as_id;

} mmr_plmn_search_cnf_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_LTE_CELL_INFO_IND                                                       */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_lte_cell_info_ind_s
{
  IMH_T                                     message_header;
  reg_mm_lte_cell_info_s_type               lte_cell_info;
  sys_modem_as_id_e_type                    as_id;
} mmr_lte_cell_info_ind_s_type;



/******************************************************************************/
/*                                                                            */
/*  MMR_LTE_PESUDO_RESEL_LOW_PRIORITY_IND                                                       */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_rrc_pseudo_resel_low_priority_ind_s
{
  IMH_T                                     message_header;
  uint32               earfcn;
  sys_modem_as_id_e_type                    as_id;
} mmr_rrc_pseudo_resel_low_priority_ind_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_REG_CNF                                                               */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_reg_cnf_s
{
  IMH_T                                     message_header;
  mmr_cause_e_type                          cause;
  sys_plmn_service_state_s_type             service_state;

  boolean                                   lai_forbidden;


  /*
  ** The PLMN_list should be populated with the list of available PLMNs
  ** provided to MM by RR and RRC.  REG will determine whether to use
  ** the list based on the current network selection mode and the
  ** CS_service_state and PS_service_state received in this message.
  */
  sys_detailed_plmn_list_s_type             available_plmn_list;
  boolean                                   update_equivalent_plmn_list;
  sys_plmn_list_s_type                      equivalent_plmn_list;
  boolean                                   ue_oos;
  boolean                                   only_for_reg;
  routing_area_code_T               rac;

  boolean                              search_done;
   word                              cell_id;

  sys_modem_as_id_e_type                    as_id;

} mmr_reg_cnf_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_SERVICE_IND                                                           */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_service_ind_s
{
  IMH_T                                     message_header;
  mmr_cause_e_type                          cause;
  sys_plmn_service_state_s_type             service_state;

  boolean                                   lai_forbidden;

  boolean                                   update_equivalent_plmn_list;
  sys_plmn_list_s_type                      equivalent_plmn_list;
  boolean                                   only_for_reg;  
   word                                      cell_id;  
  sys_modem_as_id_e_type                    as_id;

} mmr_service_ind_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_SIM_AVAILABLE_CNF                                                     */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_sim_available_cnf_s
{
  IMH_T                                     message_header;

  sys_modem_as_id_e_type                    as_id;

} mmr_sim_available_cnf_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_SIM_NOT_AVAILABLE_CNF                                                 */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_sim_not_available_cnf_s
{
  IMH_T                                     message_header;

  sys_modem_as_id_e_type                    as_id;

} mmr_sim_not_available_cnf_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_STOP_MODE_CNF                                                         */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_stop_mode_cnf_s
{
  IMH_T                                     message_header;

  sys_modem_as_id_e_type                    as_id;

} mmr_stop_mode_cnf_s_type;

/******************************************************************************/
/*                                                                            */
/*  MMR_PH_STATUS_CHANGE_CNF                                                         */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_ph_status_change_cnf_s
{
  IMH_T                                     message_header;

  sys_modem_as_id_e_type                    as_id;

} mmr_ph_status_change_cnf_s_type;

/******************************************************************************/
/*                                                                            */
/*  MMR_CELL_SERVICE_IND                                                      */
/*                                                                            */
/******************************************************************************/
typedef struct mmr_cell_service_ind_s
{
  IMH_T                                               message_header;
  sys_hs_ind_e_type                                   hsdpa_hsupa_support; /* CM should refer this value if the RAT is W */
  sys_dtm_support_e_type                              dtm_support; /* CM should refer this value if RAT is G */
  sys_egprs_support_e_type                            egprs_support;

  sys_modem_as_id_e_type                              as_id;

} mmr_cell_service_ind_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_HSPA_CALL_STATUS_IND                                                  */
/*                                                                            */
/******************************************************************************/
typedef struct mmr_hspa_call_status_ind_s
{
  IMH_T                                               message_header;
  sys_hs_ind_e_type                                   hs_call_status_ind;

  sys_modem_as_id_e_type                              as_id;

} mmr_hspa_call_status_ind_s_type;



/******************************************************************************/
/*                                                                            */
/*  MMR_CONN_MODE_SERVICE_STATUS_IND                                                  */
/*                                                                            */
/******************************************************************************/
typedef struct mmr_conn_mode_service_status_ind_s
{
  IMH_T                                               message_header;
  sys_conn_mode_ss_ind_s_type                         conn_mode_ss_ind;

  sys_modem_as_id_e_type                              as_id;

} mmr_conn_mode_service_status_ind_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_REG_REJECT_IND                                                      */
/*                                                                            */
/******************************************************************************/
typedef struct mmr_reg_reject_ind_s
{
  IMH_T                                               message_header;
  sys_srv_domain_e_type                               reject_domain;
  byte                                                reject_cause;
  sys_plmn_id_s_type                                  plmn;
  sys_lac_type                                        tac;
  sys_radio_access_tech_e_type                  reject_rat;
  sys_csg_id_type                                     csg_id;
  sys_modem_as_id_e_type                              as_id;

} mmr_reg_reject_ind_s_type;

/******************************************************************************/
/*                                                                            */
/*  MMR_DS_STAT_CHGD_CNF                                                         */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_ds_stat_chgd_cnf_s
{
  IMH_T                                     message_header;
} mmr_ds_stat_chgd_cnf_s_type;

/******************************************************************************/
/*                                                                            */
/*  MMR_multimode_subs_CHGD_CNF                                                         */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_multimode_subs_chgd_cnf_s
{
  IMH_T                                     message_header;
} mmr_multimode_subs_chgd_cnf_s_type;

/******************************************************************************/
/*                                                                            */
/*  MMR_PS_DETACH_REQ_S                                                         */
/*                                                                            */
/******************************************************************************/
typedef struct mmr_ps_detach_req_s
{
  IMH_T                                     message_header;
 sys_ps_detach_e_type                    detach_type;
  sys_modem_as_id_e_type                  as_id;
} mmr_ps_detach_req_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_PS_DETACH_CNF_S                                                         */
/*                                                                            */
/******************************************************************************/
typedef struct mmr_ps_detach_cnf_s
{
  IMH_T                                     message_header;
  sys_modem_as_id_e_type                    as_id;
} mmr_ps_detach_cnf_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_SET_DRX_CNF                                                         */
/*                                                                            */
/******************************************************************************/
typedef struct mmr_set_drx_cnf_s
{
   IMH_T            message_header; 
   boolean         result; 
   sys_modem_as_id_e_type as_id; 
} mmr_set_drx_cnf_s_type;

/******************************************************************************/
/*                                                                            */
/*  MMR_GET_DRX_CNF                                                         */
/*                                                                            */
/******************************************************************************/
typedef struct mmr_get_drx_cnf_s
{
  IMH_T                                                 message_header;
  sys_drx_cn_coefficient_s1_e_type    drx_coefficient;
  sys_modem_as_id_e_type as_id;
} mmr_get_drx_cnf_s_type;

/******************************************************************************/
/*                                                                            */
/*  MMR_UPDATE_CURR_RAT_LIST_IND_TYPE                                                         */
/*                                                                            */
/******************************************************************************/
typedef struct mmr_update_curr_rat_list_ind_s
{
  IMH_T                                     message_header;
  sys_rat_pri_list_info_s_type              rat_pri_list_info;
  sys_modem_as_id_e_type          as_id;
} mmr_update_curr_rat_list_ind_type;

typedef struct mmr_ue_mode_ind_s
{  
  IMH_T                          message_header;  
  sys_ue_mode_e_type             ue_mode;
  sys_modem_as_id_e_type         as_id;
  boolean                        ue_mode_substate_srlte;
}mm_ue_mode_ind_s_type;

typedef struct mmr_data_priority_ind_s
{
  IMH_T                          message_header;
  sys_data_priority_e_type       priority;
  sys_modem_as_id_e_type         as_id;
} mmr_data_priority_ind_s_type;


/******************************************************************************/
/*                                                                            */
/*  MMR_PSEUDO_LIST_UPDATE_IND                                                */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_pseudo_list_update_ind_s
{  
  IMH_T                          message_header;  
  sys_radio_access_tech_e_type   rat;
  boolean  can_use_pseudo;
  sys_modem_as_id_e_type         as_id;
}mmr_pseudo_list_update_ind_type;

/******************************************************************************/
/*                                                                            */
/*  MMR_EMERGENCY_NUM_LIST_IND                                                */
/*                                                                            */
/******************************************************************************/

#define MMR_EMERGENCY_BCD_NUM_LENGTH 41
#define MMR_MAX_EMERGENCY_NUM_COUNT 16
/*Why 16 ??
 *Emergency list IE is max of 50 bytes. One byte of IEI and one byte of List Length leaves    
 *with max of 48 bytes. Each individual emergency number will have one byte of length 
 *indicator, one byte of type indicator (if number is Hospital or Police etc.) and at least 
 *one byte of BCD digits for the emergency number. This means that each individual emergency 
 *number will take at least 3 bytes. Therefore, max number of emergency numbers that can be 
 *sent by network in this IE is 48/3 = 16.
 */

typedef struct 
{
   byte length;
   byte type; /*Emergency number type category e.g. Hospital, Police etc*/
   byte number[MMR_EMERGENCY_BCD_NUM_LENGTH]; /*Every byte contains 2 BCD Digits*/
} mmr_emergency_num_s_type;

typedef struct 
{
   byte emergency_number_count;
   mmr_emergency_num_s_type emergency_number[MMR_MAX_EMERGENCY_NUM_COUNT];
} mmr_emergency_num_list_s_type;

typedef struct
{                                                                
   IMH_T                              message_header;
   mmr_emergency_num_list_s_type      emergency_number_list;

   sys_modem_as_id_e_type             as_id;

} mmr_emergency_num_list_ind_s_type;



typedef struct nas_mmgsdi_info_type_s
{
/* Client id to be used in MMGSDI API call */
  mmgsdi_client_id_type   client_id;
/* Session id to be used in MMGSDI API call */
  mmgsdi_session_id_type  session_id;
/* Session type to be used while opening MMGSDI Session */
  mmgsdi_session_type_enum_type session_type;
  mmgsdi_slot_id_enum_type  slot_id;
}nas_mmgsdi_info_type;


/* Used to send SUBS READY IND to MM/REG from GSDI */

typedef struct sim_subs_ready_ind_s
{
  IMH_T                                   message_header;

  sys_modem_as_id_e_type                  as_id;

  mmgsdi_slot_id_enum_type                slot_id;
}sim_subs_ready_ind_s_type;

typedef struct sim_client_id_ind_s
{
  IMH_T                                   message_header;
  mmgsdi_client_id_type                   cid;
}sim_client_id_ind_s_type;








typedef struct mmr_suitable_search_end_ind_s
{
  IMH_T                                     message_header;
  sys_modem_as_id_e_type            as_id;
} mmr_suitable_search_end_ind_s_type;



typedef struct mmr_disable_bplmn_ind_s
{
  IMH_T                                     message_header;
  boolean                                   disable_status;
  sys_modem_as_id_e_type                    as_id;
} mmr_disable_bplmn_ind_s_type;


typedef struct mmr_cell_access_ind_s
{                                                                
   IMH_T                              message_header;
   sys_domain_access_bar_s_type       cell_access_status;

   sys_modem_as_id_e_type             as_id;

} mmr_cell_access_ind_s_type;

typedef struct mmr_fplmn_list_change_ind_s
{                                                                
   IMH_T                              message_header;
   sys_forbidden_list_type_e_type    list_type; 
   sys_modem_as_id_e_type            as_id; 
} mmr_fplmn_list_change_ind_s_type;



typedef struct mmr_home_nodeb_name_ind_s
{
  IMH_T                                               message_header;
  sys_plmn_id_s_type                        plmn;
  sys_csg_info_s_type                     csg_info;
  sys_modem_as_id_e_type                as_id; 
} mmr_home_nodeb_name_ind_s_type;

typedef struct mmr_sim_busy_ind_s
{
  IMH_T                                     message_header;
  boolean                                  is_sim_busy;

  sys_modem_as_id_e_type                    as_id;

} mmr_sim_busy_ind_s_type;

/******************************************************************************/
/*  MMR_CSG_SELECT_CONFIG_UPDATE_IND                                          */
/*  Primitive to update CSG selection configuration to MM                     */
/******************************************************************************/

typedef struct mmr_csg_select_config_update_ind_s
{
  IMH_T                                     message_header;
  sys_csg_selection_config_e_type           csg_selection_config;
  sys_modem_as_id_e_type                    as_id;
} mmr_csg_select_config_update_ind_s_type;

/******************************************************************************/
/*  MMR_CSG_SEARCH_TRIGGER_IND                                                */
/*  Primitive used by MM to indicate REG to trigger CSG Search by CM          */
/******************************************************************************/

typedef struct mmr_csg_search_trigger_ind_s
{
  IMH_T                                     message_header;
  sys_modem_as_id_e_type                    as_id;
} mmr_csg_search_trigger_ind_s_type;

/******************************************************************************/
/*  MMR_CSG_LIST_READ_COMPLETE_IND                                            */
/*  Primitive used by MM to indicate REG that CSG list is read                */
/******************************************************************************/

typedef struct mmr_csg_list_read_complete_ind_s
{
  IMH_T                                     message_header;
  sys_modem_as_id_e_type                    as_id;
} mmr_csg_list_read_complete_ind_s_type;

/******************************************************************************/
/*                                                                            */
/*  MMR_REG_REQ_IGNORED_IND                                                   */
/*                                                                            */
/******************************************************************************/

typedef struct mmr_reg_req_ignored_ind_s                                  
{
  IMH_T                                     message_header;

  sys_modem_as_id_e_type                    as_id;

} mmr_reg_req_ignored_ind_s_type;

/******************************************************************************/
/*  MMR_T3346_TIMER_IND                                                       */
/*  Primitive used by MM to update CM with T3346 timer start, stop and expiry */
/*  notifications                                                             */
/******************************************************************************/
typedef struct mmr_t3346_timer_ind_s
{
  IMH_T                                    message_header;
  sys_radio_access_tech_e_type             active_rat;
  sys_nas_t3346_status_e_type              t3346_notification;
  sys_modem_as_id_e_type                   as_id;
} mmr_t3346_timer_ind_s_type;

#define MMR_LTE_RRC_INVALID_SIB_PRIORITY 0xFF

/******************************************************************************/
/*  MMR_RRC_SIB6_INFO_IND                                                     */
/*  Primitive used by MM to send SIB6 info to REG                             */
/******************************************************************************/

typedef struct mmr_rrc_sib6_info_ind_s
{
  IMH_T                                     message_header;
  sys_modem_as_id_e_type                    as_id;
  boolean                                   sib6_present;          /*!< Is SIB-6 present? */
  int16                                     priority_UTRAN_FDD;    /*!< Highest priority of WCDMA nbr */
  int16                                     priority_UTRAN_TDD;    /*!< Highest priority of TDSCDMA nbr */
} mmr_rrc_sib6_info_ind_s_type;

/******************************************************************************/
/*  MMR_RRC_SIB7_INFO_IND                                                     */
/*  Primitive used by MM to send SIB7 info to REG                             */
/******************************************************************************/

typedef struct mmr_rrc_sib7_info_ind_s
{
  IMH_T                                     message_header;
  sys_modem_as_id_e_type                    as_id;
  boolean                                   sib7_present; /*!< Is SIB-7 present? */
  int16                                     priority;     /*!< Highest priority of GERAN nbr */
} mmr_rrc_sib7_info_ind_s_type;

/**********************************************/
/*  MMR_PSM_READY_REQ                         */
/*  Primitive to send PSM Ready Request to MM */
/**********************************************/
typedef struct mmr_psm_ready_req_s
{
  IMH_T                                     message_header;
  sys_modem_as_id_e_type                    as_id;
} mmr_psm_ready_req_s_type;


/************************************************/
/*  MMR_PSM_READY_RSP                           */
/*  Primitive to send PSM Ready Response to REG */
/************************************************/
typedef struct mmr_psm_ready_rsp_s
{
  IMH_T                                     message_header;
  sys_modem_as_id_e_type                    as_id;
  sys_psm_status_e_type                     status;
} mmr_psm_ready_rsp_s_type;


/**************************************************/
/*  MMR_PSM_READY_IND                             */
/*  Primitive to send PSM Ready Indication to REG */
/**************************************************/
typedef struct mmr_psm_ready_ind_s
{
  IMH_T                                     message_header;
  sys_modem_as_id_e_type                    as_id;
} mmr_psm_ready_ind_s_type;
/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/






#endif

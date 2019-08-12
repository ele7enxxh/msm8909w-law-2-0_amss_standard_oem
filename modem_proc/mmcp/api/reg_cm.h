#ifndef REG_CM_H
#define REG_CM_H
/*==============================================================================


                     R E G  -  C M   H E A D E R   F I L E


                                  DESCRIPTION

  This header file contains message definitions for the REG and CM interface.

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_cm.h_v   1.3   10 May 2002 15:12:18   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/reg_cm.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
04/30/13   skk     Enhance Interface For Packet Attach/Detach
04/25/01   jca     Fixed Lint errors.
05/23/01   sbk     Updated to reflect changes to REG<->MM interface.
08/16/01   ab      Renamed mmi_reg.h to cm_reg.h.
08/22/01   TS      Removed enums of inter-task commands going to Call Manager.
                   These are now located in cmll.h.
02/07/02   kwa     Updated interface to support dual mode design requirements.
09/19/02   kwa     Added cm_act_req.
10/18/02   kwa     Added cm_deep_sleep_ind.
05/22/03   kwa     Modified message verification functions to pass const
                   pointers to eliminate lint warnings. 
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
09/18/03   kwa     Added sys_band_mask_type to cm_act_req_s_type, 
                   cm_network_list_req_s_type, and cm_service_req_s_type.
09/18/03   kwa     Replaced user_selected_plmn in cm_service_req_manual_s_type
                   with selected_plmn_type to allow CM to initiate a manual
                   search for the HPLMN in addition to the RPLMN and a user
                   specified PLMN.  Change was featurized using
                   FEATURE_MANUAL_HPLMN_SELECTION.
11/07/03   kwa     Removed support for REG_CM_MANUALLY_SELECTED_RPLMN.  The
                   capability for CM to request that REG search for the RPLMN
                   at power up in manual network selection mode is provided by
                   sending an undefined PLMN along with
                   REG_CM_MANUALLY_SELECTED_SPECIFIED_PLMN.
02/04/04   kwa     Added ps_for_request parameter to cm_service_req_s_type.
05/06/04   kwa     Added cs_for_request parameter to cm_service_req_s_type.
                   Added cm_camped_ind.
07/19/04   kwa     Removed service_state parameter from cm_camped_ind.
07/28/04   kwa     Added CM_REG_CMD_NONE to cm_to_reg_cmd_type.
                   Added active_rat and active_band parameters to cm_camped_ind.
01/11/05   up      Mainlining FEATURE_MANUAL_HPLMN_SELECTION.
03/31/05   hj      Added support for CM_CIPHER_IND. 
04/12/05   hj      Reverting enum type for cipher_domain in cm_cipher_ind_s_type
                   to sys_srv_domain_e_type.
09/19/05   up      Added cm_hplmn_search_req_s_type, cm_hplmn_search_cnf_s_type
                   and CM_HPLMN_SEARCH_REQ to support on demand HPLMN Search.
10/29/05   ajt     Added data-types to send emergency numbers (valid in this country)
                   to CM.                   
12/06/05   up      Added status field in cm_network_list_cnf_s_type to indicate
                   the abort of manual plmn search.
                   Added new enum reg_cm_network_list_status_e_type.
04/06/06   sn      Added cm_cell_service_ind_s_type
07/27/06   sn      Added Uniform OOS changes.
                   FEATURE_UMTS_UNIFORM_OOS_HANDLING
11/23/06   sn      Added feature FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH 
12/15/09   abhi  Removing UOOS code
01/31/11   rajesh  voice domain preference and UE usage setting related changes
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "comdef.h"
#include "customer.h"
#include "sys.h"
#include "sys_plmn_selection.h"
#include "sys_stru.h"


#include "mmgsdilib.h"
#include "rex.h"
#include "rrcmmif.h"


/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

typedef uint32 reg_cm_transaction_id_type;

typedef enum
{
  /* CM ==> REG */
  CM_REG_CMD_NONE,
  CM_NETWORK_LIST_REQ,
  CM_SERVICE_REQ,
  CM_SIM_AVAILABLE_REQ,
  CM_SIM_NOT_AVAILABLE_REQ,
  CM_STOP_MODE_REQ,
  CM_HPLMN_SEARCH_REQ,
  CM_PH_STATUS_CHGD_REQ

  ,CM_EXIT_FROM_PWR_SAVE

  ,CM_DS_STAT_CHGD_REQ
  ,CM_DS_TUNEAWAY_STATUS_CHANGE_IND
  ,CM_PS_DETACH_REQ
  ,CM_BLOCK_PLMN_REQ 
  ,CM_SET_DRX_REQ
  ,CM_GET_DRX_REQ  
  ,CM_SET_HPLMN_IRAT_SEARCH_TIMER_REQ
  ,CM_UE_MODE_IND
  ,CM_SET_HPLMN_SEARCH_TIMER_REQ
  ,CM_GET_HPLMN_SEARCH_TIMER_REQ
  ,CM_MULTIMODE_SUBS_CHGD_REQ
  ,CM_CSG_SELECT_CONFIG_UPDATE_IND
  ,CM_DATA_PRIORITY_IND
  ,CM_DATA_ROAMING_IND
  ,CM_DDS_SWITCH_IND
  ,CM_PSM_READY_REQ
  ,CM_ROAMING_IND
} cm_to_reg_cmd_type;

/*
**  Enumerated type used to specify the PLMN to search for during manual
**  PLMN selection.
*/
typedef enum
{
  REG_CM_MANUALLY_SELECTED_NONE = -1,               /* FOR INTERNAL USE ONLY! */
  REG_CM_MANUALLY_SELECTED_SPECIFIED_PLMN,
  REG_CM_MANUALLY_SELECTED_HPLMN,
  REG_CM_MANUALLY_SELECTED_MAX                      /* FOR INTERNAL USE ONLY! */
} reg_cm_manually_selected_plmn_e_type;

/*
** Enum type to specify the CM_SERVICE_REQ type
**/
typedef enum {
  REG_CM_SERVICE_REQ_NONE = -1,
  REG_CM_SERVICE_REQ_SYNC_UP,        
  REG_CM_SERVICE_REQ_USER_SELECTION,  
  REG_CM_SERVICE_REQ_SYSTEM_CHANGE, 
  REG_CM_SERVICE_REQ_PERIODIC,     
  REG_CM_SERVICE_REQ_NORMAL,
  REG_CM_SERVICE_REQ_BETTER_SYSTEM_RESELECTION,
  REG_CM_SERVICE_REQ_SUITABLE_SEARCH,
  REG_CM_SERVICE_REQ_IRAT_TO_LTE,
  REG_CM_SERVICE_REQ_EF_RAT_REFRESH,
  REG_CM_SERVICE_REQ_MMSS_START_HPLMN,
  REG_CM_SERVICE_REQ_CAMPED_PLMN,
  REG_CM_SERVICE_REQ_USER_SELECTION_CSG_RPLMN,
  REG_CM_SERVICE_REQ_NORMAL_RPLMN,
  REG_CM_SERVICE_REQ_SRV_MODE_UPDATE,
  REG_CM_SERVICE_REQ_MMSS_START_RPLMN,
  REG_CM_SERVICE_REQ_USER_PREFERRED_PLMN_RAT,
  REG_CM_SERVICE_REQ_CSG_SELECTION_APP,
  REG_CM_SERVICE_REQ_ROAMING_PLMN,
  REG_CM_SERVICE_REQ_LAST_CAMPED_RAT,
  /* service type for full RAT mode - specific to Limited Service 
   * initial reason to introduce - to let NAS know that current service req is to determine MCC
   * NAS need not send IDLE event to card at this point.
   * */
  REG_CM_SERVICE_REQ_FULL_RAT_MODE, 
  REG_CM_SERVICE_REQ_MAX        
} reg_cm_service_req_e_type;

/*
** Enumerated type used to specify the status of Manual PLMN Search.
*/
typedef enum
{
  REG_CM_NETWORK_LIST_NODE = -1,       /* FOR INTERNAL USE ONLY */
  REG_CM_NETWORK_LIST_SUCCESS,
  REG_CM_NETWORK_LIST_AS_ABORT,
  REG_CM_NETWORK_LIST_ABORTED_ON_RLF,
  REG_CM_NETWORK_LIST_PARTIAL,         
  REG_CM_NETWORK_LIST_MAX              /* FOR INTERNAL USE ONLY */
} reg_cm_network_list_status_e_type;

/* 
** Enumeration of scan status
*/
typedef enum {
  REG_CM_SCAN_STATUS_NONE = -1,
  REG_CM_SCAN_STATUS_COMPLETE_NO_PLMN,     /* scan completed in its allotted time and NO PLMN found */
  REG_CM_SCAN_STATUS_COMPLETE_PLMN_FOUND,  /* scan completed and some PLMNs found */
  REG_CM_SCAN_STATUS_ABORTED,              /* Scan aborted/incomplete due to timer expiry */
  REG_CM_SCAN_STATUS_MAX
} reg_cm_scan_status_e_type;


/*
** Enumeration of Network List Request type
*/
typedef enum 
{
  REG_CM_NETWORK_LIST_NONE = -1,       /* FOR INTERNAL USE ONLY */
  REG_CM_NETWORK_LIST_REQ_PLMN_LIST_SEARCH,
  REG_CM_NETWORK_LIST_REQ_SRV_SEARCH_BGND,
  REG_CM_NETWORK_LIST_REQ_CSG_LIST_SEARCH,
  REG_CM_NETWORK_LIST_REQ_PRIORITY_PLMN_SEARCH_BGND, /* Search high priority SBM PLMN in bgnd when card loaded with EM IMSI */
  REG_CM_NETWORK_LIST_REQ_PCI_PLMN_LIST_SEARCH,
  REG_CM_NETWORK_LIST_REQ_MAX              /* FOR INTERNAL USE ONLY */
} reg_cm_network_list_req_e_type;

typedef enum
{
  CM_MM_MT_CALL_FAIL_CAUSE_ESR_FAILURE,
  CM_MM_MT_CALL_FAIL_CAUSE_CONNECTION_FAILURE,
  CM_MM_MT_CALL_FAIL_CAUSE_CONNECTION_EST_FAILURE,
  CM_MM_MT_CALL_FAIL_CAUSE_NO_RESPONSE_FROM_NW,
  CM_MM_MT_CALL_FAIL_CAUSE_MT_CSFB_NO_RESPONSE_FROM_NW
}sys_modem_mt_call_fail_cause_type;

/*******************************************************************************
********************************************************************************
**                                                                            **
**                           CM to REG Primitives                             **
**                                                                            **
********************************************************************************
*******************************************************************************/
                                                                            
/*******************************************************************************
**                                                                            **
**  CM_NETWORK_LIST_REQ                                                       **
**                                                                            **
*******************************************************************************/

typedef struct cm_network_list_req_s
{
  IMH_T                                               message_header;
  reg_cm_transaction_id_type                          transaction_id;
  sys_mode_pref_e_type                                mode_pref;
  sys_srv_domain_e_type                               req_service_domain;
  sys_band_mask_type                                  band_pref;
  reg_cm_network_list_req_e_type                      list_search_type;
  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                           stack_id;
  sys_rat_pri_list_info_s_type                        rat_pri_list_info; 
  /* following rat pri list to be used as a filter 
  ** in bplmn search by lower layers*/  
  sys_rat_pri_list_info_s_type    mcc_mode_band_filter; 

  /* Scan scope for BPLMN Search - OOS Optimization */
  sys_scan_scope_e_type                               scan_scope;  
  sys_csg_search_type_e_type                          csg_search_type;
} cm_network_list_req_s_type;
/******************************************************************************/
/*                                                                            */
/*  CM_SERVICE_REQ                                                            */
/*                                                                            */
/******************************************************************************/
/*
** Structure containing the message parameters for a cm_service_req message
** if the network selection mode is SYS_NETWORK_SELECTION_MODE_AUTOMATIC.
*/
typedef struct cm_service_req_automatic_s
{
  reg_cm_service_req_e_type                      type;
  sys_csg_id_type                                csg_id;  
  sys_plmn_id_s_type                             plmn; 
  sys_radio_access_tech_e_type                   rat;  
} cm_service_req_automatic_s_type;

/*
** Structure containing the message parameters for a cm_service_req message
** if the network selection mode is SYS_NETWORK_SELECTION_MODE_MANUAL.
*/
typedef struct cm_service_req_manual_s
{
  reg_cm_service_req_e_type                           type;
  reg_cm_manually_selected_plmn_e_type                selected_plmn_type;
  sys_plmn_id_s_type                                  plmn;
  sys_csg_id_type                                     csg_id;  
  sys_radio_access_tech_e_type                        rat; 
} cm_service_req_manual_s_type;

/*
** Structure containing the message parameters for a cm_service_req message
** if the network selection mode is SYS_NETWORK_SELECTION_MODE_LIMITED.
*/
typedef struct cm_service_req_limited_s
{
  reg_cm_service_req_e_type  type;
} cm_service_req_limited_s_type;


/*
** Define a structure for the cm_service_req primitive.
*/
typedef struct cm_service_req_s
{
  IMH_T                                               message_header;
  reg_cm_transaction_id_type                          transaction_id;
  sys_network_selection_mode_e_type                   network_selection_mode;
  sys_mode_pref_e_type                                mode_pref;
  sys_srv_domain_e_type                               req_service_domain;
  boolean                                             cs_for_request;
  boolean                                             ps_for_request;

  sys_band_mask_type                                  band_pref;
  sys_mmss_ue_capability_s_type                       ue_capability;

  union
  {
    cm_service_req_automatic_s_type                   automatic;
    cm_service_req_manual_s_type                      manual;
    cm_service_req_limited_s_type                     limited;
  } parameters;

  sys_rat_pri_list_info_s_type    rat_pri_list_info; 

  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;

  boolean                                            sms_only; 
  sys_lte_cs_domain_param_s_type                     lte_cs_domain_param;
  uint32                                             additional_info;
  
  /* Scan scope for GWL Acquisition - OOS Optimization */
  sys_scan_scope_e_type   scan_scope;  
  
  /* This flag is needed by NAS to convey that PS service needs to be acquired for VoLTE only.*/
  boolean                                            is_volte_only_ps;
} cm_service_req_s_type;


/******************************************************************************/
/*                                                                            */
/*  CM_SIM_AVAILABLE_REQ                                                      */
/*                                                                            */
/******************************************************************************/

typedef struct cm_sim_available_req_s
{
  IMH_T                                               message_header;
  reg_cm_transaction_id_type                          transaction_id;

  sys_mode_pref_e_type                                mode_pref;
  mmgsdi_session_type_enum_type                       session_type;
  uint8                                               nv_context_id;
  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_sim_available_req_s_type;


/******************************************************************************/
/*                                                                            */
/*  CM_SIM_NOT_AVAILABLE_REQ                                                  */
/*                                                                            */
/******************************************************************************/

typedef struct cm_sim_not_available_req_s
{
  IMH_T                                               message_header;
  reg_cm_transaction_id_type                          transaction_id;

  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_sim_not_available_req_s_type;


/******************************************************************************/
/*                                                                            */
/*  CM_STOP_MODE_REQ                                                          */
/*                                                                            */
/******************************************************************************/

typedef struct cm_stop_mode_req_s
{
  IMH_T                                               message_header;
  reg_cm_transaction_id_type                          transaction_id;
  sys_stop_mode_reason_e_type                         stop_mode_reason;

  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_stop_mode_req_s_type;

/******************************************************************************/
/*                                                                            */
/*  CM_PH_STATUS_CHGD_REQ                                                          */
/*                                                                            */
/******************************************************************************/
typedef struct cm_ph_status_chgd_req_s

{
  IMH_T                                               message_header;
  reg_cm_transaction_id_type                          transaction_id;
  sys_oprt_mode_e_type                                oprt_mode;

  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_ph_status_chgd_req_s_type;


/******************************************************************************/
/*                                                                            */
/*  CM_EXIT_FROM_PWR_SAVE                                            */
/*                                                                            */
/******************************************************************************/
typedef struct cm_exit_from_pwr_save_s
{
  IMH_T                                               message_header;

  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_exit_from_pwr_save_s_type;



/******************************************************************************/
/*                                                                            */
/*  CM_HPLMN_SEARCH_REQ                                                       */
/*                                                                            */
/******************************************************************************/

typedef struct cm_hplmn_search_req_s
{
  IMH_T                                     message_header;

  sys_modem_as_id_e_type                    as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_hplmn_search_req_s_type;
/******************************************************************************/
/*                                                                            */
/*  CM_DS_STAT_CHGD_REQ                                                       */
/*                                                                            */
/******************************************************************************/
typedef struct cm_ds_stat_chgd_req_s
{
  IMH_T                          message_header;
  reg_cm_transaction_id_type     transaction_id;
  sys_modem_dual_standby_pref_e_type  dual_standby_mode;
  uint8			                 active_subs;
  sys_modem_device_mode_e_type   device_mode;
}cm_ds_stat_chgd_req_s_type;

/******************************************************************************/
/*                                                                            */
/*  CM_MULTIMODE_SUBS_CHGD_REQ                                                       */
/*                                                                            */
/******************************************************************************/

typedef struct cm_multimode_subs_chgd_req_s
{
  IMH_T                                      message_header;
  reg_cm_transaction_id_type                 transaction_id;
  subs_capability_e_type                     subs_capability[MAX_AS_IDS];
}cm_multimode_subs_chgd_req_s_type;

/******************************************************************************/
/*                                                                            */
/*  CM_DS_TUNEAWAY_STATUS_CHANGE_IND                                                       */
/*                                                                            */
/******************************************************************************/
typedef struct cm_ds_tuneaway_stat_chgd_ind_s
{
  IMH_T                          message_header;
  boolean                        tuneaway_status;
}cm_ds_tuneaway_stat_chgd_ind_s_type;

/******************************************************************************/
/*                                                                            */
/*  CM_SET_DRX_REQ                                                       */
/*                                                                            */
/******************************************************************************/
typedef struct cm_set_drx_req_s
{
  IMH_T                                               message_header;
  sys_drx_cn_coefficient_s1_e_type                    drx_coefficient;
  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_set_drx_req_s_type;


/******************************************************************************/
/*                                                                            */
/*  CM_GET_DRX_REQ                                            */
/*                                                                            */
/******************************************************************************/
typedef struct cm_get_drx_req_s
{
  IMH_T                                               message_header;
  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_get_drx_req_s_type;


/******************************************************************************/
/*                                                                            */
/*  CM_SET_HPLMN_IRAT_SEARCH_TIMER_REQ                                            */
/*                                                                            */
/******************************************************************************/
typedef struct cm_set_hplmn_irat_search_timer_req_s
{
  IMH_T                                               message_header;

  /* Timer value = 0 means to trigger immediate search and disable timer
  ** Time value  = 0xFFFFFFFF means timer is disabled 
  ** Value is in milli-seconds
  **/
  uint32                                             timer_value; 
  sys_modem_as_id_e_type                             as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_set_hplmn_irat_search_timer_req_s_type;


/******************************************************************************/
/*                                                                            */
/*  CM_SET_HPLMN_SEARCH_TIMER_REQ                                            */
/*                                                                            */
/******************************************************************************/
typedef struct cm_set_hplmn_search_timer_req_s
{
  IMH_T                                               message_header;

  /*  
  ** Value is in minutes
  **  Time value  = 0xFFFFFFFF means use SIM defined timer
  **/
  uint32                                             timer_value; 

  sys_modem_as_id_e_type                             as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_set_hplmn_search_timer_req_s_type;



/******************************************************************************/
/*                                                                            */
/*  CM_GET_HPLMN_SEARCH_TIMER_REQ                                            */
/*                                                                            */
/******************************************************************************/
typedef struct cm_get_hplmn_search_timer_req_s
{
  IMH_T                                               message_header;
  sys_modem_as_id_e_type                            as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_get_hplmn_search_timer_req_s_type;


 /******************************************************************************/
/*                                                                            */
/*  CM_DATA_ROAMING_IND                                            */
/*                                                                            */
/******************************************************************************/
typedef struct cm_data_roaming_ind
{
   IMH_T                        message_header;
   sys_data_roaming_e_type      data_roaming;
   sys_modem_as_id_e_type       as_id;
}cm_data_roaming_ind_s_type;


typedef struct cm_roaming_ind
{
   IMH_T                        message_header;
   boolean                      international_voice_roaming_allowed;
   sys_modem_as_id_e_type       as_id;
} cm_roaming_ind_s_type;


/******************************************************************************/
/*                                                                            */
/*  CM_DDS_SWITCH_IND                                                         */
/*                                                                            */
/******************************************************************************/
typedef struct cm_dds_switch_ind_s
{
  IMH_T                                     message_header;
  sys_modem_as_id_e_type                    target_dds_sub;
} cm_dds_switch_ind_s_type;


/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/*                           REG to CM Primitives                             */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*  CM_CAMPED_IND                                                             */
/*                                                                            */
/******************************************************************************/

typedef struct cm_camped_ind_s
{
  IMH_T                                               message_header;
  sys_radio_access_tech_e_type                        active_rat;
  /* Active band for a given RAT is defined in a union type */
  sys_active_band_u_type                              active_band;


  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
  sys_plmn_id_s_type                        plmn;
  
  sys_lac_type                              lac_id;
} cm_camped_ind_s_type;

/******************************************************************************/
/*                                                                            */
/*  CM_PS_ATTACH_FAILED_IND                                                             */
/*                                                                            */
/******************************************************************************/

typedef struct cm_ps_attach_failed_ind_s
{
  IMH_T                                               message_header;

  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_ps_attach_failed_ind_s_type;


/******************************************************************************/
/*                                                                            */
/*  CM_MM_INFORMATION_IND                                                     */
/*                                                                            */
/******************************************************************************/

typedef struct cm_mm_information_ind_s
{
  IMH_T                                               message_header;
  sys_mm_information_s_type                           info;

  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_mm_information_ind_s_type;


/******************************************************************************/
/*                                                                            */
/*  CM_NETWORK_LIST_CNF                                                       */
/*                                                                            */
/******************************************************************************/

typedef struct cm_network_list_cnf_s
{
  IMH_T                                               message_header;
  reg_cm_transaction_id_type                          transaction_id;
  sys_plmn_id_s_type                                  rplmn;
  reg_cm_network_list_status_e_type                   status;
  sys_found_plmn_list_u_type                          found_plmn_list;
  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_network_list_cnf_s_type;


/******************************************************************************/
/*                                                                            */
/*  CM_SERVICE_CNF                                                            */
/*                                                                            */
/******************************************************************************/

typedef struct cm_service_cnf_s
{
  IMH_T                                               message_header;
  reg_cm_transaction_id_type                          transaction_id;
  sys_plmn_service_state_s_type                       service_state;
  reg_cm_scan_status_e_type                           scan_status;

  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_service_cnf_s_type;


/******************************************************************************/
/*                                                                            */
/*  CM_SERVICE_IND                                                            */
/*                                                                            */
/******************************************************************************/

typedef struct cm_service_ind_s
{
  IMH_T                                               message_header;
  sys_plmn_service_state_s_type                       service_state;

  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_service_ind_s_type;


/******************************************************************************/
/*                                                                            */
/*  CM_SIM_AVAILABLE_CNF                                                      */
/*                                                                            */
/******************************************************************************/

typedef struct cm_sim_available_cnf_s
{
  IMH_T                                               message_header;
  reg_cm_transaction_id_type                          transaction_id;

  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_sim_available_cnf_s_type;


/******************************************************************************/
/*                                                                            */
/*  CM_SIM_NOT_AVAILABLE_CNF                                                  */
/*                                                                            */
/******************************************************************************/

typedef struct cm_sim_not_available_cnf_s
{
  IMH_T                                               message_header;
  reg_cm_transaction_id_type                          transaction_id;

  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_sim_not_available_cnf_s_type;


/******************************************************************************/
/*                                                                            */
/*  CM_STOP_MODE_CNF                                                          */
/*                                                                            */
/******************************************************************************/

typedef struct cm_stop_mode_cnf_s
{
  IMH_T                                               message_header;
  reg_cm_transaction_id_type                          transaction_id;

  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_stop_mode_cnf_s_type;

/******************************************************************************/
/*                                                                            */
/*  CM_PH_STATUS_CHGD_CNF                                                     */
/*                                                                            */
/******************************************************************************/

typedef struct cm_ph_status_chgd_cnf_s
{
  IMH_T                                               message_header;
  reg_cm_transaction_id_type                          transaction_id;
  sys_ph_status_chgd_status_e_type                    ph_status_chgd_status;

  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_ph_status_chgd_cnf_s_type;


/******************************************************************************/
/*                                                                            */
/*  CM_GET_HPLMN_SEARCH_TIMER_CNF                                            */
/*                                                                            */
/******************************************************************************/
typedef struct cm_get_hplmn_search_timer_cnf_s
{
  IMH_T                                               message_header;
  uint32                                               timer_value;
  sys_modem_as_id_e_type                               as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_get_hplmn_search_timer_cnf_s_type;


/******************************************************************************/
/*                                                                            */
/*  CM_CIPHER_IND                                                             */
/* Primitive to indicate the start of ciphering in lower layers to CM         */ 
/*                                                                            */
/******************************************************************************/

typedef struct
{                                                                
  IMH_T                                               message_header;
  sys_srv_domain_e_type                               cipher_domain;
  boolean                                             cipher_status;

  sys_modem_as_id_e_type                               as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_cipher_ind_s_type;

typedef struct
{                                                                
  IMH_T                                               message_header;
  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_mm_ps_data_suspend_ind_s_type;

typedef struct
{
  IMH_T                                               message_header;
  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
  sys_modem_mt_call_fail_cause_type                   mt_call_fail_cause ;
  sys_radio_access_tech_e_type                        rat;
  rrc_paging_cause_e_type                             paging_cause;

}cm_mm_mt_call_fail_ind_s_type;
/******************************************************************************/
/*                                                                            */
/*  CM_BEARER_CTXT_TRANSFER_IND                                              */
/* Primitive to inform about rat change to CM/DS. On this message         */
/* CM/DS will transfer context to new rat                                           */
/*                                                                            */
/******************************************************************************/
typedef struct
{
  IMH_T                                               message_header;
  sys_radio_access_tech_e_type            rat;
  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_bearer_ctxt_transfer_ind_s_type;



/******************************************************************************/
/*                                                                            */
/*  CM_HPLMN_SEARCH_CNF                                                       */
/*                                                                            */
/******************************************************************************/

typedef struct cm_hplmn_search_cnf_s
{
  IMH_T                                               message_header;
  boolean                                             search_initiated;
  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_hplmn_search_cnf_s_type;


/******************************************************************************/
/*                                                                            */
/*  CM_CELL_SERVICE_IND                                                       */
/*                                                                            */
/******************************************************************************/
typedef struct cm_cell_service_ind_s
{
  IMH_T                                               message_header;
  sys_hs_ind_e_type                                   hsdpa_hsupa_support; /* CM should refer this value if the RAT is W */
  sys_dtm_support_e_type                              dtm_support; /* CM should refer this value if RAT is G */
  sys_egprs_support_e_type                            egprs_support;

  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_cell_service_ind_s_type;


/******************************************************************************/
/*                                                                            */
/*  CM_CONN_MODE_SS_IND                                                       */
/*                                                                            */
/******************************************************************************/
typedef struct cm_conn_mode_ss_ind_s
{
  IMH_T                                               message_header;
  sys_conn_mode_ss_ind_s_type                         conn_mode_ss_ind;
  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_conn_mode_ss_ind_s_type;




/******************************************************************************/
/*                                                                            */
/*  CM_HSPA_CALL_STATUS_IND                                                       */
/*                                                                            */
/******************************************************************************/

typedef struct cm_hspa_call_status_ind_s
{
  IMH_T                                               message_header;
  sys_hs_ind_e_type                                   hs_call_status_ind;
  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_hspa_call_status_ind_s_type;


/******************************************************************************/
/*                                                                            */
/*  CM_REG_REJECT_IND                                                       */
/*                                                                            */
/******************************************************************************/
typedef struct cm_reg_reject_ind_s
{
  IMH_T                                               message_header;
  sys_srv_domain_e_type                               reject_domain; /* The domain on which the reject cause is received */
  byte                                                reject_cause;       /* value of reject cause-2,3,6,11,12,13 only as of now */
  sys_plmn_id_s_type                                  plmn;
  sys_radio_access_tech_e_type                        rat;
  sys_lac_type                                        tac;
  sys_csg_id_type                                     csg_id;
  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_reg_reject_ind_s_type;

/******************************************************************************/
/*                                                                            */
/*  CM_DS_STATUS_CHGD_CNF                                                     */
/*                                                                            */
/******************************************************************************/
typedef struct cm_ds_stat_chgd_cnf_s
{
  IMH_T                                               message_header;
  reg_cm_transaction_id_type                          transaction_id;
} cm_ds_stat_chgd_cnf_s_type;
/******************************************************************************/
/*                                                                            */
/*  CM_MULTIMODE_SUBS_CHGD_CNF                                                     */
/*                                                                            */
/******************************************************************************/
typedef struct cm_multimode_subs_chgd_cnf_s
{
  IMH_T                                               message_header;
} cm_multimode_subs_chgd_cnf_s_type;

/******************************************************************************/
/*                                                                            */
/*  CM_PS_DETACH_REQ_S                                                     */
/*                                                                            */
/******************************************************************************/
typedef struct cm_ps_detach_req_s
{
  IMH_T                                     message_header;
  sys_ps_detach_e_type                    detach_type;
  sys_modem_as_id_e_type                  as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_ps_detach_req_s_type;

/******************************************************************************/
/*                                                                            */
/*  CM_PS_DETACH_CNF_S                                                     */
/*                                                                            */
/******************************************************************************/
typedef struct cm_ps_detach_cnf_s
{
  IMH_T                                               message_header;
  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_ps_detach_cnf_s_type;

/******************************************************************************/
/*                                                                            */
/*  CM_BLOCK_PLMN_REQ                                                      */
/*                                                                            */
/******************************************************************************/
typedef struct cm_block_plmn_req_s
{
  IMH_T                     message_header;
  sys_block_plmn_e_type     type;
  sys_plmn_id_s_type        plmn;
  rex_timer_cnt_type        timer_count; /* In mili seconds*/
  sys_block_plmn_cause_e_type cause;
  sys_modem_as_id_e_type    as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_block_plmn_req_s_type;

/******************************************************************************/
/*                                                                            */
/*  CM_MM_SRVCC_HANDOVER_FAILURE_IND                                                      */
/*                                                                            */
/******************************************************************************/

typedef struct cm_mm_srvcc_handover_failure_ind_s
{                                                                
  IMH_T                                               message_header;
  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_mm_srvcc_handover_failure_ind_s_type;

/******************************************************************************/
/*                                                                            */
/*  CM_FPLMN_LIST_IND                                                     */
/*                                                                            */
/******************************************************************************/
typedef struct cm_fplmn_list_ind_s
{                                                                
  IMH_T                                               message_header;
  sys_forbidden_list_type_e_type    list_type;  /* type of list*/
  sys_forbidden_plmn_list_s_type    plmn_list;   /*forbidden PLMN*/
  sys_modem_as_id_e_type as_id;
  sys_modem_stack_id_e_type               stack_id;
} cm_fplmn_list_ind_s_type;

/******************************************************************************/
/*                                                                            */
/*  CM_SET_DRX_CNF                                               */
/*                                                                            */
/******************************************************************************/
typedef struct cm_set_drx_cnf_s
{
   IMH_T            message_header; 
   boolean         result; 
   sys_modem_as_id_e_type as_id;
   sys_modem_stack_id_e_type       stack_id;
} cm_set_drx_cnf_s_type;

/******************************************************************************/
/*                                                                            */
/*  CM_GET_DRX_CNF                                               */
/*                                                                            */
/******************************************************************************/
typedef struct cm_get_drx_cnf_s
{
  IMH_T                                               message_header;
  sys_drx_cn_coefficient_s1_e_type                    drx_coefficient;
  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_get_drx_cnf_s_type;

/******************************************************************************/
/*                                                                            */
/*  CM_HOME_NODEB_NAME_IND                                               */
/*                                                                            */
/******************************************************************************/
typedef struct cm_home_nodeb_name_ind_s
{
  IMH_T                                               message_header;
  sys_plmn_id_s_type                        plmn;
  sys_csg_info_s_type                     csg_info;
  sys_modem_as_id_e_type                  as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_home_nodeb_name_ind_s_type;


/******************************************************************************/
/*                                                                            */
/*  CM_PLMN_LIST_CHANGE_IND                                               */
/*                                                                            */
/******************************************************************************/
typedef struct cm_plmn_list_change_ind_s
{
  IMH_T                                         message_header;
  sys_plmn_list_type_e_type                     changed_list_type;
  sys_modem_as_id_e_type                        as_id;
} cm_plmn_list_change_ind_s_type;


/******************************************************************************/
/*                                                                            */
/*  CM_UPDATE_SIM_STATE                                                       */
/*                                                                            */
/******************************************************************************/
typedef struct cm_sim_state_update_ind_s
{
  IMH_T                                               message_header;
  sys_modem_as_id_e_type                              as_id;
  sys_modem_stack_id_e_type                          stack_id;
  sys_sim_state_e_type                                sim_state;
} cm_sim_state_update_ind_s_type;

/******************************************************************************/
/*                                                                            */
/* CM_EMERGENCY_NUM_LIST_IND                                                  */
/* Primitive to indicate the emergency numbers valid in this country to CM    */ 
/*                                                                            */
/******************************************************************************/

#define CM_EMERGENCY_BCD_NUM_LENGTH 41
#define REG_MAX_EMERGENCY_NUM_COUNT  16

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
   byte number[CM_EMERGENCY_BCD_NUM_LENGTH]; /*Every byte contains 2 BCD Digits*/
} cm_emergency_num_s_type;

typedef struct 
{
   byte emergency_num_count;
   cm_emergency_num_s_type emergency_number[REG_MAX_EMERGENCY_NUM_COUNT];
} cm_emergency_num_list_s_type;

typedef struct
{                                                                
   IMH_T                              message_header;
   cm_emergency_num_list_s_type       emergency_number_list;

   sys_modem_as_id_e_type                       as_id;
   sys_modem_stack_id_e_type                          stack_id;

} cm_emergency_num_list_ind_s_type;

typedef enum
{
   CM_EMERGENCY_NUM_TYPE_POLICE = 0x01,
   CM_EMERGENCY_NUM_TYPE_AMBULANCE = 0x02,
   CM_EMERGENCY_NUM_TYPE_FIRE_BRIGADE = 0x04,
   CM_EMERGENCY_NUM_TYPE_MARINE_GUARD = 0x08,
   CM_EMERGENCY_NUM_TYPE_MOUNTAIN_RESCUE = 0x10       
} cm_emergency_no_e_type;

typedef struct cm_suitable_search_end_ind_s 
{
   IMH_T                                               message_header;
   sys_modem_as_id_e_type                              as_id;
   sys_modem_stack_id_e_type                          stack_id;
} cm_suitable_search_end_ind_s_type;


typedef struct cm_disable_bplmn_ind_s
{
  IMH_T                                     message_header;
  boolean                                   status;
  sys_modem_as_id_e_type                    as_id;
  sys_modem_stack_id_e_type                          stack_id;
} cm_disable_bplmn_ind_s_type;

typedef struct cm_cell_access_ind_s
{
 IMH_T                              message_header;
 sys_domain_access_bar_s_type       cell_access_status;

 sys_modem_as_id_e_type             as_id;
 sys_modem_stack_id_e_type                          stack_id;
}cm_cell_access_ind_s_type;



typedef struct cm_ue_mode_ind_s
{  
  IMH_T                          message_header;  
  sys_ue_mode_e_type             ue_mode;
  sys_modem_as_id_e_type         as_id;
  boolean                        is_ue_mode_substate_srlte;
}cm_ue_mode_ind_s_type;

typedef struct cm_data_priority_ind_s
{
  IMH_T                          message_header;
  sys_data_priority_e_type       priority;
  sys_modem_as_id_e_type         as_id;
} cm_data_priority_ind_s_type;

typedef struct cm_stack_deact_ind_s
{
  IMH_T                          message_header;
  sys_modem_as_id_e_type         as_id;
  sys_modem_stack_id_e_type                          stack_id;
}cm_stack_deact_ind_s_type;

typedef struct cm_stack_act_ind_s
{
  IMH_T                          message_header;
  sys_modem_as_id_e_type         as_id;
  sys_modem_stack_id_e_type                          stack_id;
}cm_stack_act_ind_s_type;

/******************************************************************************/
/* CM_CSG_SELECT_CONFIG_UPDATE_IND                                            */
/* Primitive to update CSG selection configuration to NAS                     */ 
/******************************************************************************/
typedef struct cm_csg_select_config_update_ind_s
{
  IMH_T                                    message_header;
  sys_csg_selection_config_e_type          csg_selection_config;
  sys_modem_as_id_e_type                   as_id;
  sys_modem_stack_id_e_type                stack_id;
}cm_csg_select_config_update_ind_s_type;

/******************************************************************************/
/* CM_CSG_SEARCH_TRIGGER_IND                                                  */
/* Primitive used by NAS to indicate CM to trigger CSG Search                 */ 
/******************************************************************************/
typedef struct cm_csg_search_trigger_ind_s
{
  IMH_T                                    message_header;
  sys_modem_as_id_e_type                   as_id;
  sys_modem_stack_id_e_type                stack_id;
}cm_csg_search_trigger_ind_s_type;

/******************************************************************************/
/*                                                                            */
/* CM_CSG_NW_SEL_MODE_UPDATE_IND                                              */
/* Primitive used by NAS to update CM with Manual CSG network selection mode  */ 
/*                                                                            */
/******************************************************************************/
typedef struct cm_csg_nw_sel_mode_update_ind_s
{
  IMH_T                                    message_header;
  sys_network_selection_mode_e_type        nw_sel_mode;
  sys_modem_as_id_e_type                   as_id;
  sys_modem_stack_id_e_type                stack_id;
}cm_csg_nw_sel_mode_update_ind_s_type;


/******************************************************************************/
/* CM_NAS_T3346_TIMER_IND                                                     */
/* Primitive used by NAS to update CM with T3346 timer start, stop and expiry */ 
/* notifications                                                              */ 
/******************************************************************************/
typedef struct cm_nas_t3346_timer_ind_s
{
  IMH_T                                    message_header;
  sys_radio_access_tech_e_type             active_rat;
  sys_nas_t3346_status_e_type              t3346_notification;
  sys_modem_as_id_e_type                   as_id;
  sys_modem_stack_id_e_type                stack_id;
} cm_nas_t3346_timer_ind_s_type;


/***************************************************/
/* CM_PSM_READY_REQ                                */
/* Primitive to check if NAS is ready to enter PSM */ 
/***************************************************/
typedef struct cm_psm_ready_req_s
{
  IMH_T                  message_header;
  sys_modem_as_id_e_type as_id;
}cm_psm_ready_req_s_type;


/*******************************************************/
/* CM_PSM_READY_RSP                                    */
/* Primitive to inform CM if NAS is ready to enter PSM */ 
/*******************************************************/
typedef struct cm_psm_ready_rsp_s
{
  IMH_T                      message_header;
  sys_psm_status_e_type      status;
  sys_modem_as_id_e_type     as_id;
}cm_psm_ready_rsp_s_type;


/*********************************************************/
/* CM_PSM_READY_IND                                      */
/* Primitive to inform CM that NAS is ready to enter PSM */ 
/*********************************************************/
typedef struct cm_psm_ready_ind_s
{
  IMH_T                   message_header;
  sys_modem_as_id_e_type  as_id;
}cm_psm_ready_ind_s_type;


/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

/*==============================================================================

                             FUNCTION DECLARATIONS

==============================================================================*/

#endif

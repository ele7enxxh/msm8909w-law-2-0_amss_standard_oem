
/*!
  @file
  emm_utility.h

  @brief
  EMM internal data structure definitions.

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/21/10   zren    Updated EMM to include correct MME routing info in conn est 
                   req for TAU after IRAT   
05/26/10   zren    Updated to send LIMITED SERVICE status in REG CNF for 
                   LIMITED SERVICE REQ when get service available service ind 
                   from RRC
05/19/10   zren    Added support for GSTK EMM REGISTERED EVT and NW REJ EVT
05/18/10   zren    Added NONCE UE support in TAU request
05/07/10   zren    Added temp FPLMN feature
                   Fixed and consolidated equivalent PLMN list handling
                   Added code to handle found plmn list in RRC SERVICE IND
04/22/10   zren    Updated to update service domain upon detach completion
04/09/10   zren    Added combined procedure feature
02/24/10   zren    Added support for network selection modes of LIMITED MANUAL
                   ACQUISITION and LIMITED ROUTINE ACQUISITION
02/05/10   zren    Updated processing of connection abort in waiting for RRC 
                   confirmation state
01/21/10   zren    Added EMM/ESM STATUS feature
01/07/10   zren    Updated release client and pended message processing
11/04/09   MNK     Preserve the security context upon i-RAT & Cleanup emm_ctrl_data as necessary
10/06/09   RI      Fix new line warning at EOF.
07/15/09   zren    Modified message list handling functions to use message 
                   pointer as parameter
06/23/09   zren    Added support of processing EPLMN list, forbidden PLMN list,
                   forbidden TAI list for roaming and forbidden TAI list for
                   regional provision of service 
05/29/09   zren    Added support for TAU feature
04/17/09   zren    Modified to support saving NAS messages in pended message 
                   list in the format of lte_nas_outgoing_msg_type
                   Added one parameter to function emm_send_rrc_connection_establishment_request() 
                   Sepearted RRC_DATA_CNF and RRC_EST_CNF for RRC_EST_REQ
                   Modified EMM connection state machines
02/23/09   vdr     Fixed Lint errors & warnings
03/26/09   RI      Added support for GS Timers in LTE instead of REX signals
===========================================================================*/
#ifndef _EMM_UTILITY_H_
#define _EMM_UTILITY_H_
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "emm_connection_type.h"
#include "emm_connection_handler.h"
#include "emm_database.h"
#include "sys.h"
#include "lte_rrc_ext_msg.h"
#include "gs_v.h"
#include "lte_nas_emm_message_description.h"
#include "rex.h"
#include "gmm_v.h"
#ifdef FEATURE_GSTK  
#include "gstk_exp_v.h"
#endif
#include "emm_ext_msg.h"
#include "dsm_init.h"

/*===========================================================================

                        MACROS DEFINITION

===========================================================================*/

#define BIT_MASK(bit)      (1 << (bit))
#define BIT_CLR(x,bit)     ((x) &= ~BIT_MASK (bit))
#define BIT_SET(x,bit)     ((x) |= BIT_MASK (bit))
#define IS_BIT_SET(x,bit)  ((x) & BIT_MASK (bit))
#define UPPER_BCD(x)       (((x) & 0xF0) >> 4)
#define LOWER_BCD(x)       ( (x) & 0x0F)

/* gets the free bytes availabel in small item pool */
#define NAS_REM_SMALL_ITEM_POOL_SIZE (DSM_DS_SMALL_ITEM_SIZ * DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL))

/*===========================================================================

                        TYPES DEFINITION

===========================================================================*/

/*===========================================================================

                        EXTERNAL DEFINITION

===========================================================================*/


/*===========================================================================

FUNCTION    : EMM_PRINT_EMM_CAUSE

DESCRIPTION:  This function will print the causes recieved by EMM module for any procedure
 
DEPENDENCIES
  None

RETURN VALUE
none

SIDE EFFECTS
  None

===========================================================================*/

extern void emm_print_emm_cause(lte_nas_emm_cause_type   emm_cause);


/*===========================================================================

FUNCTION    : EMM_VALIDATE_ATTACH_PLMN

DESCRIPTION:  This function waits for UIM response to check if camped PLMN is
              fine to proceed with ATTACH or to be in LIMITED_SERVICE until card 
              refreshes for another ATTACH
 
DEPENDENCIES
  None

RETURN VALUE
 TRUE: Proceed with ATTACH
 FALSE: Current PLMN is invalid, ABORT ATTACH procedure & continue to be in LIMITED_SERVICE

SIDE EFFECTS
  None

===========================================================================*/
extern
boolean emm_validate_attach_plmn
(
  emm_ctrl_data_type *emm_ctrl_data_ptr,
  sys_plmn_id_s_type *plmn_to_attach
);

/*===========================================================================

FUNCTION    : EMM_RESET_POWEROFF_DETACH_PARAMS

DESCRIPTION:  This function will reset all the detach related params during poweroff
  
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern
void emm_reset_poweroff_detach_params
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
);

extern 
emm_pended_msg_list_type *emm_cache_new_message
(
  lte_nas_outgoing_msg_type   *emm_message_ptr,
  dword                        sequence_number,
  msgr_tech_module_type        client,
  rrc_l2_callback_type         message_callback,
  rrc_connection_failure_type  tx_failure_callback,
  emm_ctrl_data_type          *emm_ctrl_data_ptr
) ;

extern
void emm_send_all_pended_messages
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
) ;

extern
void emm_process_rrc_deactivate_cnf
(
  lte_rrc_deactivate_cnf_s *rrc_deactivate_cnf,
  emm_ctrl_data_type       *emm_ctrl_data_ptr
);

extern
void emm_send_limited_svc_to_gstk
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

extern
void emm_send_full_svc_to_gstk
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);
/*===========================================================================
FUNCTION    EMM_PROCESS_UNSENT_MESSAGES

DESCRIPTION
  This function traverse the pended message list and processes pended messages
  if needed.
  - This function removes processed messages
  - This funtion removes client without any pended message left

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_unsent_messages
(
  lte_emm_connection_cause_type  cause,
  emm_failure_type               emm_failure_cause,
  emm_ctrl_data_type            *emm_ctrl_data_ptr
);

/*===========================================================================
FUNCTION    EMM_GET_REG_INIT_RRC_TRANS_ID

DESCRIPTION
  This function generates transaction ID for EMM outgoing RRC primitive which 
  is initiated by REG CMD.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
dword emm_get_reg_init_rrc_trans_id
(
  void
);

/*===========================================================================
FUNCTION EMM_UPDATE_GW_SEC_CAPABILITIES

DESCRIPTION
  Update the LTE OTA message and / OR EMM internal data structure (emm_ctrl_data) 
  with the corresponding Security capabilties
  1. For UMTS, EMM_CTRL_DATA & OTA message structure are updated
  2. For GPRS, only EMM_CTRL_DATA is updated (existing code will update the OTA structure)

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None
===========================================================================*/
extern
void emm_update_gw_sec_capabilities
(
  emm_ctrl_data_type              *emm_ctrl_data_ptr,
  lte_nas_emm_ue_capability_type  *ue_capability
);

/*===========================================================================
FUNCTION EMM_CONVERT_RRC_PLMN_ID_TO_NAS_PLMN_ID

DESCRIPTION
  This function converts from the RRC PLMN ID format (1 BCD digit per byte)
  to the NAS PLMN_id_T format (2 BCD digits per byte).

DEPENDENCIES
  None

RETURN VALUE
  PLMN ID converted to PLMN_id_T format (3 byte array - 2 BCD digits/byte)

SIDE EFFECTS
  None
===========================================================================*/
extern
sys_plmn_id_s_type emm_convert_rrc_plmn_id_to_nas_plmn_id
( 
  lte_rrc_plmn_s plmn_id 
) ;

/*===========================================================================
FUNCTION EMM_GET_PLMN_STATE_INFO

DESCRIPTION
  This function populates all the parameter of PLMN_STATE_INFO, in emm_ctrl_data_ptr

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_get_plmn_state_info
(
  sys_srv_status_e_type  srv_status,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
);

/*===========================================================================
FUNCTION EMM_CONVERT_NAS_PLMN_ID_TO_RRC_PLMN_ID

DESCRIPTION
  This function converts from the NAS PLMN ID format (2 BCD digits per byte)
  to the RRC PLMN_id_T format (1 BCD digit per byte).
 
DEPENDENCIES
  None

RETURN VALUE
  PLMN ID converted to rrc_plmn_id_type format

SIDE EFFECTS
  None
===========================================================================*/
extern
lte_rrc_plmn_s emm_convert_nas_plmn_id_to_rrc_plmn_id
(
  sys_plmn_id_s_type plmn_id
) ;

extern
void emm_stop_all_timers
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
) ;

extern 
void emm_local_detach
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
);

extern
void mm_send_message
( 
  IMH_T * p_message, 
  gs_queue_id_T queue
) ;

extern
boolean emm_already_camped
(
 mmr_reg_req_s_type *reg_req_ptr, 
 emm_ctrl_data_type *emm_ctrl_data_ptr
) ;

extern
void emm_stop_cell_barring_timers
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
#ifdef FEATURE_NAS_EAB
  , boolean          stop_eab_barring
#endif
) ;

extern
emm_pended_msg_list_type *emm_find_pended_message
(
  dword               trans_id,
  emm_ctrl_data_type *emm_ctrl_data_ptr
) ;

extern
lte_emm_connection_cause_type emm_translate_ul_data_cnf_status
(
  lte_rrc_ul_data_cnf_status_e rrc_status
) ;

extern
lte_nas_tai_lst1_type emm_convert_ta
(
  lte_rrc_tracking_area_s ta
) ;

/*===========================================================================

FUNCTION EMM_DELETE_PENDED_MESSAGE_WITH_MSG_ID

DESCRIPTION
  This function deletes the pended message of specified message type in the queue.
  This function is called on successful transmission of message or 
  when we are done with the message in failure cases.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void emm_delete_pended_message_with_msg_id
(
  lte_nas_message_id  pended_msg_id,
  emm_ctrl_data_type *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION EMM_DELETE_PENDED_MESSAGE

DESCRIPTION
  This function deletes the pended message in the queue.
  This function is called on successful transmission of message or 
  when we are done with the message in failure cases.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern
void emm_delete_pended_message
(
  emm_pended_msg_list_type *message_tobe_deleted, 
  emm_ctrl_data_type       *emm_ctrl_data_ptr
) ;

extern
void emm_start_state_timer
(
  emm_ctrl_data_type *emm_ctrl_data_ptr, 
  word                timer
) ;

extern
dword emm_get_nas_transaction_id
(
  void
) ;

/*===========================================================================

FUNCTION EMM_ADD_FORBIDDEN_TAI

DESCRIPTION
  This function adds TAI to the appropriate "Forbidden" list either to:
    o rrc_borbidden_list, or
    o forbidden_for_service_list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern
void emm_add_forbidden_tai
(
  lte_nas_tai_lst2_type *forbidden_list,     /* Pointer to the list               */
  lte_nas_tai_lst1_type  forbidden_tai,
  emm_ctrl_data_type    *emm_ctrl_data_ptr   /* Pointer to Common Global EMM Data */
) ;

extern
lte_emm_connection_cause_type emm_translate_rel_cause
(
  lte_rrc_conn_rel_reason_e rrc_rel_reason
) ;

/*===========================================================================

FUNCTION    EMM_SEARCH_TAI_LIST

DESCRIPTION
  This checks wheather provided TAI is belonging to the stored TAI list

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - provided TAI IS belonging to the stored TAI list
  FALSE - provided TAI is NOT belonging to the stored TAI list

SIDE EFFECTS
  None

===========================================================================*/
extern
boolean emm_search_tai_list
(
  /* Provided TAI */
  lte_nas_tai_lst1_type *provided_tai,

  /* Stored TAI list */
  lte_nas_emm_tai_lst_info_type *tai_list
) ;

/*===========================================================================

FUNCTION    EMM_UPDATE_EPS_STATUS

DESCRIPTION
  This function changes EPS UPDATE STATUS

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - MMR_REG_CNF or MMR_SERVICE_IND IS required
  FALSE - MMR_REG_CNF or MMR_SERVICE_IND is NOT required

SIDE EFFECTS
  None

===========================================================================*/
extern
boolean emm_update_eps_status
(
  /* Current EMM EPS Update Status */
  emm_eps_update_status_type current_eps_update_status,

  /* EMM control data */
  emm_ctrl_data_type             *emm_ctrl_data_ptr
) ;

/*===========================================================================

FUNCTION    EMM_UPDATE_PLMN_SERVICE_STATUS

DESCRIPTION
  This function changes current PLMN Service Status

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - MMR_REG_CNF or MMR_SERVICE_IND IS required
  FALSE - MMR_REG_CNF or MMR_SERVICE_IND is NOT required

SIDE EFFECTS
  None

===========================================================================*/
extern
boolean emm_update_plmn_service_status
(
  /* Current EMM EPS Update Status */
  sys_srv_status_e_type          current_sys_srv_status,

  /* EMM control data */
  emm_ctrl_data_type             *emm_ctrl_data_ptr
) ;

/*===========================================================================

FUNCTION EMM_SEARCH_PLMN_LIST

DESCRIPTION
  This function searches through the list of forbidden PLMNs

DEPENDENCIES
  None

RETURN VALUE
  [0..(List Length-1)] - wanted PLMN has been found in the searched list
  0xFF                 - otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern
byte emm_search_plmn_list
(
  /* pointer to PLMN which needs to be deleted from the list */
  sys_plmn_id_s_type        *plmn_tobe_searched, 

  /* pointer to a PLMN list */
  lte_nas_emm_plmn_lst_type *plmn_list 
) ;

/*===========================================================================
FUNCTION    EMM_UPDATE_EQ_PLMN_LIST

DESCRIPTION
  This function updates the equivalent plmn list entries in the emm database, 
  based on the forbidden TAI LIST received during ATTACH_ACCEPT & TAU_ACCEPT

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_update_eq_plmn_list
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION EMM_GET_CAMPED_TAI

DESCRIPTION
  This function returns the camped TAI

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
lte_nas_tai_lst1_type emm_get_camped_tai
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
);


/*===========================================================================

FUNCTION EMM_DELETE_FORBIDDEN_TAI

DESCRIPTION
  This function deletes TAI from the appropriate "Forbidden" list either from:
    o rrc_borbidden_list, or
    o forbidden_for_service_list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern
void emm_delete_forbidden_tai
(
  lte_nas_tai_lst1_type *tai_tobe_deleted, /* Pointer to the TAI */
  lte_nas_tai_lst2_type *forbidden_list    /* Pointer to a list  */
) ;

/*===========================================================================

FUNCTION    EMM_PROCESS_TAI_LIST

DESCRIPTION
  This processes newly received TAI list in order to reprocess existing 
  forbidden lists: 
    o rrc_forbidden_list
    o forbidden_for_service_list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function may modify:
    o rrc_forbidden_list
    o forbidden_for_service_list
===========================================================================*/
extern
void emm_process_tai_list
(
  /* Stored TAI list */
  lte_nas_emm_tai_lst_info_type *tai_list,

  /* EMM control data */
  emm_ctrl_data_type            *emm_ctrl_data_ptr
) ;

/*===========================================================================

FUNCTION    EMM_DELETE_TAI_FROM_LIST

DESCRIPTION
  This function deletes provided TAI from the TAI list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern
void emm_delete_tai_from_list
(
  /* Provided TAI */
  lte_nas_tai_lst1_type *provided_tai,

  /* Stored TAI list */
  lte_nas_emm_tai_lst_info_type *tai_list
) ;

extern
void emm_reset_ctrl_data
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr,
  boolean              reset_ftai_list       
);

/*===========================================================================
FUNCTION    EMM_CONN_ABORT_MARK_PENDED_MESSAGE

DESCRIPTION
  This function marks all existing pended messages to be processed upon 
  receiving release indication with cause of aborted 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_conn_abort_mark_pended_message
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  EMM_SEND_EMM_STATUS

DESCRIPTION
  This function sends EMM_STATUS message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_emm_status
(
  emm_ctrl_data_type      *emm_ctrl_data_ptr,
  lte_nas_emm_cause_type   emm_cause
);

/*===========================================================================

FUNCTION  emm_write_rplmn_type_to_efs

DESCRIPTION
  This function writes RPLMN TYPE to EFS 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_write_rplmn_type_to_efs
(
  mm_rplmn_type_enum_T rplmn_type
);


/*===========================================================================
FUNCTION    EMM_SEND_IDLE_INFO_TO_GSTK

DESCRIPTION
  This function sends NO_SVC Indication to GSTK.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Sent IDLE_INFO info sent to GSTK
  FALSE: Could not send IDLE_INFO to GSTK

SIDE EFFECTS
  None
===========================================================================*/
extern
void emm_send_idle_info_to_gstk
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
);
/*===========================================================================
FUNCTION    EMM_SEND_NO_SVC_TO_GSTK

DESCRIPTION
  This function sends NO_SVC Indication to GSTK.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Successully send the info to GSTK
  FALSE: EMM_NO_SVC indication not sent to GSTK

SIDE EFFECTS
  None
===========================================================================*/
extern
void emm_send_no_svc_to_gstk
(
  void
);

/*===========================================================================
FUNCTION    EMM_SEND_REGISTERED_EVT_TO_GSTK

DESCRIPTION
  This function sends GSTK EMM REGISTERED EVENT to GSTK.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_registered_evt_to_gstk
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

#ifdef FEATURE_GSTK
/*===========================================================================
FUNCTION    EMM_SEND_NW_REJ_EVT_TO_GSTK

DESCRIPTION
  This function sends NW REJ EVENT to GSTK.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_nw_rej_evt_to_gstk
(
  gstk_nw_rej_enum_type   nw_rej_type,
  lte_nas_emm_cause_type  emm_cause,
  emm_ctrl_data_type     *emm_ctrl_data_ptr
);
#endif

/*===========================================================================
FUNCTION    EMM_TRANSLATE_CNF_CAUSE

DESCRIPTION
  This function translates a given RRC_CONN_EST_CNF_CAUSE to EMM connection 
  cause type

DEPENDENCIES
  None

RETURN VALUE
  The translated EMM connection cause

SIDE EFFECTS
  None
===========================================================================*/
lte_emm_connection_cause_type  emm_translate_cnf_cause
(
  lte_rrc_conn_est_status_e  rrc_conn_est_status
);

void emm_set_and_write_tin_type(nas_mm_tin_type tin);
void emm_write_tin_to_efs(emm_ctrl_data_type *emm_ctrl_data_ptr);

void emm_write_lte_accepted_status_to_efs(emm_ctrl_data_type *emm_ctrl_data_ptr);
void emm_write_lte_accepted_vplmn_status_to_efs(emm_ctrl_data_type *emm_ctrl_data_ptr);

/*===========================================================================

FUNCTION EMM_UPDATE_NW_SELECTION_MODE

DESCRIPTION
  This function checks the network selection mode in last received REG REQ and
  update the network selection mode if needed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_update_nw_selection_mode
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION    EMM_TAU_COMPLETE_SUCCEEDED

DESCRIPTION
  This function is called upon successful TAU COMPLETE message delivery

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void emm_tau_complete_succeeded
(
  dword  trans_id
);

/*===========================================================================

FUNCTION EMM_GET_MO_DETACH_TYPE

DESCRIPTION
  This function returns MO detach type according to current service domain

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
emm_mo_detach_type  emm_get_mo_detach_type
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION EMM_GET_EPS_UPDATE_TYPE

DESCRIPTION
  This function returns TAU type according to current service domain


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
lte_nas_emm_eps_update_val_type  emm_get_eps_update_type
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION EMM_UPDATE_SRV_DOMAIN_MO_DETACH

DESCRIPTION
  This function updates service domain info upon MO detach procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_update_srv_domain_mo_detach
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  EMM_IS_PLMN_FORBIDDEN

DESCRIPTION
  This function checks if a given PLMN belongs to any list of 
  - Forbidden PLMNs list 
  - Forbidden PLMNs for GPRS service list
  - Temporary forbidden PLMNs list 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: PLMN is in forbidden lists
  FALSE: PLMN is not in forbidden lists

SIDE EFFECTS
  None

===========================================================================*/
boolean  emm_is_plmn_forbidden
(
  sys_plmn_id_s_type   sys_plmn,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  EMM_HANDLE_TEMPORARY_FORBIDDEN_PLMN

DESCRIPTION
  This function adds current PLMN to temporary forbidden PLMN list upon the 
  starting of T3402 if backoff time is set

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_handle_temporary_forbidden_plmn
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  EMM_REMOVE_FORBIDDEN_PLMN

DESCRIPTION
  This function removes PLMN from following lists
  - Forbidden PLMN list
  - Forbidden PLMN for GPRS service list
  - Temporary forbidden PLMN list 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_remove_forbidden_plmn
(
  sys_plmn_id_s_type   sys_plmn,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  EMM_SEARCH_TAI_LIST2

DESCRIPTION
  This function will check if the requested TAI is present in TAI_LIST_2

DEPENDENCIES
  None

RETURN VALUE
  TRUE: TAI is found in TAI_LIST2
  FALSE: TAI is not found in TAI_LIST2

SIDE EFFECTS
  None

===========================================================================*/
extern
boolean emm_search_tai_list2
(
  lte_nas_tai_lst1_type   tai_to_be_searched,  
  lte_nas_tai_lst2_type   *tai_list
);

/*===========================================================================

FUNCTION  EMM_IS_TAI_FORBIDDEN

DESCRIPTION
  This function will check if the requested TAI is present in the forbiden tai list
1. Forbidden for Roaming
2. Forbidden for regional service

DEPENDENCIES
  None

RETURN VALUE
  TRUE: TAI is forbidden
  FALSE: TAI is not forbidden

SIDE EFFECTS
  None

===========================================================================*/
extern
boolean emm_is_tai_forbidden
(
  lte_nas_tai_lst1_type tai,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  EMM_SET_NONCE_UE

DESCRIPTION
  This function generates a 32 bits random number and sets the nonce ue 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_set_nonce_ue
(
  byte                *nonce_ue_ptr,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  EMM_GET_TAU_REQ_MME_ROUTING_INFO

DESCRIPTION
  This function returns MME routing info to be used in connection setup request
  for TAU REQUEST message

DEPENDENCIES
  None

RETURN VALUE
  MME Routing Info type

SIDE EFFECTS
  None

===========================================================================*/
lte_nas_conn_req_routing_info  emm_get_tau_req_mme_routing_info
(
  emm_ctrl_data_type     *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION    EMM_CHK_TA_CHANGE

DESCRIPTION
 compares current rrc req plmn with the last rrc plmn as well as the TAC.
 if they are equal TA has not changed. else would compare the tai in the
 tai list
 
DEPENDENCIES
  None

RETURN VALUE
  FALSE - If TA has not changed
  TRUE  - If TA has changed

SIDE EFFECTS
  None

===========================================================================*/
extern 
boolean emm_chk_ta_change
( 
  lte_rrc_service_ind_s  *rrc_service_ind,
  emm_ctrl_data_type     *emm_ctrl_data_ptr
);

#ifdef FEATURE_LTE_REL10
/*===========================================================================
FUNCTION    EMM_SAVE_T3346_PLMN

DESCRIPTION
  LAST attempted plmn is stored in the EMM_CTRL_DATA
  shared between GWLT

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_save_t3346_plmn
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
);
#endif 

/*===========================================================================

FUNCTION    EMM_SAVE_LAST_ATTEMPTED_TAI

DESCRIPTION
LAST attempted TAI is stored in the EMM_CTRL_DATA
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern
void emm_save_last_attempted_tai
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  EMM_RAT_CHANGE_STOP_TIMERS

DESCRIPTION
  This function stops EMM timers when UE is switch to another RAT
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern
void  emm_rat_change_stop_timers(void);

/*===========================================================================

FUNCTION  EMM_SEND_REG_REG_IND

DESCRIPTION
  This function sends REG REJ IND.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern
void  emm_send_reg_rej_ind
(
  lte_nas_emm_cause_type  emm_cause,
  emm_ctrl_data_type     *emm_ctrl_data_ptr
);


/*=========================================================================== 
 
FUNCTION    EMM_CLEAN_MSG_BUFFER

DESCRIPTION
  This function cleans message buffer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
extern void emm_clean_msg_buffer
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*=========================================================================== 
 
FUNCTION  EMM_LOCAL_SWITCHOFF_DETACH

DESCRIPTION 
  This function performs local switch off detach

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
extern void emm_local_switchoff_detach
(
  sys_stop_mode_reason_e_type   stop_mode_reason,
  emm_ctrl_data_type           *emm_ctrl_data_ptr 
);

/*===========================================================================
FUNCTION    EMM_TIMER_T3402_UPDATE_EFS

DESCRIPTION
  updates the t3402 value in efs if its different than the current value

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_timer_t3402_update_efs
(
  dword emm_t3402_value
);

/*===========================================================================

FUNCTION  EMM_IS_COMBINED_REG_ALLOWED

DESCRIPTION
  This function checks if combined registration is allowed with LTE RAT
 
DEPENDENCIES
  None

RETURN VALUE
  TRUE: Combined registration IS allowed 
  FALSE: Combined registration IS NOT allowed 

SIDE EFFECTS
  None

===========================================================================*/
extern boolean  emm_is_combined_reg_allowed
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION EMM_ADD_COMBINED_REG_FORBIDDEN_PLMN

DESCRIPTION
  This function adds the given PLMN to the forbidden PLMN list for combined
  registration

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
extern void emm_add_combined_reg_forbidden_plmn
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr 
);

/*=========================================================================== 
 
FUNCTION  EMM_SEND_PLMN_CHANGE_IND

DESCRIPTION
  This function sends NAS_EMM_PLMN_CHANGE_IND w/ new PLMN information

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
extern void emm_send_plmn_change_ind
(
  sys_plmn_id_s_type  sys_plmn
);


/*=========================================================================== 
 
FUNCTION  EMM_SEND_RESET_APN_SWITCH_IND

DESCRIPTION
  This function sends NAS_EMM_RESET_APN_SWITCH_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
extern void emm_send_reset_apn_switch_ind
(
  void
);


/*=========================================================================== 
 
FUNCTION  EMM_RELEASE_ATTACH_PDN_CONN_REQ

DESCRIPTION
  This function reset PDN CONN REQ transaction ID and notify ESM to terminate
  the pending PDN CONN REQ procedure for attach

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
extern void emm_release_attach_pdn_conn_req
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*=========================================================================== 
 
FUNCTION  EMM_GET_CURRENT_TAI

DESCRIPTION
  This function get the current TAI

DEPENDENCIES
  None

RETURN VALUE
  Serving TAI

SIDE EFFECTS
  None
 
===========================================================================*/
extern lte_nas_tai_lst1_type  emm_get_current_tai
(
  lte_rrc_service_ind_s  *rrc_service_ind
);

/*=========================================================================== 
 
FUNCTION  EMM_IS_MANUAL_REQ_PENDING

DESCRIPTION
  This function checks if a manual service request is still pending

DEPENDENCIES
  None

RETURN VALUE
  Serving TAI

SIDE EFFECTS
  None
 
===========================================================================*/
extern boolean  emm_is_manual_req_pending
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*=========================================================================== 
 
FUNCTION  EMM_IS_MANUAL_REG_ALLOWED

DESCRIPTION
  This function checks if a registration in manual mode is allowed

DEPENDENCIES
  None

RETURN VALUE
  Serving TAI

SIDE EFFECTS
  None
 
===========================================================================*/
boolean  emm_is_manual_reg_allowed
(
  lte_nas_tai_lst1_type  srving_tai,  
  emm_ctrl_data_type    *emm_ctrl_data_ptr
);


/*=========================================================================== 
 
FUNCTION  EMM_SEND_TAI_LIST_IND

DESCRIPTION
  This function sends NAS_EMM_TAI_LIST_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_send_tai_list_ind
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
);


/*===========================================================================

FUNCTION    EMM_ATTACH_COMPLETE_SUCCEEDED

DESCRIPTION
  This function is called upon successful ATTACH COMPLETE message delivery

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void emm_attach_complete_succeeded
(
  dword  trans_id
);

/*=========================================================================== 
 
FUNCTION  EMM_SET_RRC_EPLMN_LIST

DESCRIPTION
  This function populates the EPLMN list to be sent to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void  emm_set_rrc_eplmn_list
(
  lte_rrc_plmn_list_s  *rrc_eplmn_list_ptr,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
);

/*=========================================================================== 
 
FUNCTION  EMM_SET_RRC_RPLMN

DESCRIPTION
  This function sets the RPLMN field of the primitives that to be sent to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void  emm_set_rrc_rplmn
(
  boolean             *rplmn_valid_ptr,
  lte_rrc_plmn_s      *rplmn_ptr,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*=========================================================================== 
 
FUNCTION  emm_add_to_rpm_ftai_list

DESCRIPTION
  Updates rpm ftai list and rrc forbidden ftai list 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_add_to_rpm_ftai_list
(
  lte_nas_tai_lst1_type  tai,
  emm_ctrl_data_type *emm_ctrl_data_ptr
);


/*=========================================================================== 
 
FUNCTION  emm_clear_rpm_ftai_list

DESCRIPTION
  clears the rpm ftai list 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_clear_rpm_ftai_list
(
  boolean            remove_from_ftai_list,
  emm_ctrl_data_type *emm_ctrl_data_ptr
);


/*=========================================================================== 
 
FUNCTION  emm_ctrl_data_mem_alloc

DESCRIPTION
  Allocate the memmory for ctrl data pointers variables

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_ctrl_data_mem_alloc
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
);

/*=========================================================================== 
 
FUNCTION  emm_ctrl_data_mem_free

DESCRIPTION
  Free the memory allocated dynamically 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_ctrl_data_mem_free
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  emm_ctrl_data_mem_alloc_check

DESCRIPTION
  This functions asserts if memory for emm_ctrl_data pointer variables is not allocated correctly.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_ctrl_data_mem_alloc_check
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
);

/*=========================================================================== 
 
FUNCTION  emm_procedure_message_mem_alloc

DESCRIPTION
  Allocate the memmory for ctrl data pointers variables

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_procedure_message_mem_alloc
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
);

/*=========================================================================== 
 
FUNCTION  emm_procedure_message_mem_free

DESCRIPTION
  Free the memory allocated dynamically 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_procedure_message_mem_free
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION EMM_ADD_MANUAL_FTAI

DESCRIPTION
  This function adds the given TAI to forbidden TAI list for manual selection 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_add_manual_ftai
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  EMM_IS_TAI_FORBIDDEN_FOR_MANUAL

DESCRIPTION
  This function will check if the requested TAI is present in the forbiden TAI
  list for manual slection

DEPENDENCIES
  None

RETURN VALUE
  TRUE: TAI is forbidden
  FALSE: TAI is not forbidden

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_is_tai_forbidden_for_manual
(
  lte_nas_tai_lst1_type  tai,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
);

/*=========================================================================== 
 
FUNCTION  EMM_SEND_PS_DETACH_CNF

DESCRIPTION
  This function sends PS DETACH CNF and cleans up PS DETACH state machine

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void  emm_send_ps_detach_cnf
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*=========================================================================== 
 
FUNCTION  EMM_PS_DETACH_CLEAN_UP

DESCRIPTION
  This function cleans up pending PS_DETACH_REQ

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void  emm_ps_detach_clean_up
(
  emm_ctrl_data_type    *emm_ctrl_data_ptr
);

/*===========================================================================
FUNCTION    EMM_SET_TRM_PRIORITY

DESCRIPTION
  Set the TRM PRIORTIY 
DEPENDENCIES
  None

RETURN VALUE
 None 

SIDE EFFECTS
  None
===========================================================================*/
void emm_set_trm_priority
(
  lte_trm_priority_e   trm_priority
);


#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
/*=========================================================================== 
 
FUNCTION  EMM_TIMER_RADIO_RETRY_EXPIRE

DESCRIPTION
  This function handles radio retry timer expiry
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_timer_radio_retry_expire
(
  emm_ctrl_data_type    *emm_ctrl_data_ptr
);
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
/*===========================================================================
FUNCTION    emm_get_lte_trm_priority

DESCRIPTION
  This function returns the TRM priority as HIGH is Attach or CSFB is going on. 

DEPENDENCIES
  None

RETURN VALUE
TRM priority

SIDE EFFECTS
  None
===========================================================================*/
lte_trm_priority_e emm_get_lte_trm_priority
(
  emm_ctrl_data_type      *emm_ctrl_data_ptr
);

/*===========================================================================
FUNCTION    EMM_UPDATE_LTE_TRM_PRIORITY

DESCRIPTION
  This function re-evaluate TRM priorty and send update to LRRC if required 

DEPENDENCIES
  None

RETURN VALUE
TRM priority

SIDE EFFECTS
  None
===========================================================================*/
void emm_update_lte_trm_priorty
(
  emm_ctrl_data_type      *emm_ctrl_data_ptr
);

#endif 

/*===========================================================================
FUNCTION    emm_set_tau_trm_priority

DESCRIPTION
  This function sets the TAU TRM priority and updates the access stratum if TRM priority is chaning to low

DEPENDENCIES
  None

RETURN VALUE
TRM priority

SIDE EFFECTS
  None
===========================================================================*/

void emm_set_tau_trm_priority
(
  boolean tau_trm_priority
);


/*===========================================================================

FUNCTION  EMM_CACHE_REG_CMD

DESCRIPTION: This function caches REG CMD

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_cache_reg_cmd
(
  mm_cmd_type         *mm_cmd_ptr,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  EMM_FREE_DSM_MEMORY_AND_CLEAR_ATT_PTR

DESCRIPTION: This function frees the memory allocated to DSM item & clears the attach_ptr 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_free_dsm_memory_and_clear_att_ptr
(
  msgr_attach_struct_type *att_ptr,
  dsm_item_type *dsm_ptr
);

/*===========================================================================

FUNCTION  LTE_NAS_NV_READ

DESCRIPTION: This function reads all the NV items, required for LTE-NAS 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void lte_nas_nv_read
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  EMM_CHK_EPS_BEARER_SYNC_RECOVERY

DESCRIPTION: This function performs TAU when both of following conditions are
             meet to bring UE and NW back in sync of the EPS bearer status
             - UE receives PDN CONN REJ with cause - Multiple PDN connections
               for a given APN not allowed (55)
             - UE is in REGISTERED or SERVICE REQUEST INITED state
             - NW has extra active bearer(s) that UE does not have

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_chk_eps_bearer_sync_recovery
(
  lte_nas_esm_incoming_msg_type  *msg_ptr,
  emm_ctrl_data_type             *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  emm_add_plmn_to_backoff_fplmn_list

DESCRIPTION
If PLMN is not present in backoff FPLMN list then 
        a)Add PLMN the list 
         b) send change indication to REG


DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void emm_add_plmn_to_backoff_fplmn_list
(
  sys_plmn_id_s_type plmn,
  rex_timer_cnt_type  timer,
  sys_block_plmn_cause_e_type cause
);

/*===========================================================================

FUNCTION  emm_add_eplmn_to_backoff_fplmn_list

DESCRIPTION
This function adds EPLMN passed as the parameter to the backoff list. 
  The timer value passed is not the actual timer value but the current running timer plus
  the timer value passed.
  This function simply adds the PLMN to backoff list with the timer value of the PLMN which
  is already present in the list.


DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void emm_add_eplmn_to_backoff_fplmn_list
(
  sys_plmn_id_s_type plmn,
  rex_timer_cnt_type  timer,
  sys_block_plmn_cause_e_type cause
);


/*===========================================================================

FUNCTION  emm_remove_plmn_from_backoff_fplmn_list

DESCRIPTION
If PLMN is present in backoff FPLMN list then 
        a)remove PLMN the list 
         b) send change indication to REG


DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void emm_remove_plmn_from_backoff_fplmn_list
(
  sys_plmn_id_s_type plmn
);

/*===========================================================================
FUNCTION      EMM_IS_PLMN_REG_ALLOWED

DESCRIPTION 
 it would check if plmn/tai is forbidden or not for the ue to register on
 the plmn
  
DEPENDENCIES
  None 

RETURN VALUE
  TRUE   -  If registration is allowed
  FALSE  -  If registration is not allowed
 
SIDE EFFECTS
  None
===========================================================================*/
boolean emm_is_plmn_reg_allowed
(
  lte_rrc_service_ind_s     *rrc_service_ind,
  emm_ctrl_data_type        *emm_ctrl_data_ptr
);

#ifdef FEATURE_LTE_REL10
/*===========================================================================
FUNCTION      EMM_PLMN_EQUIVALENT_T3346

DESCRIPTION
 it would check if plmn is same as t3346 or is in eplmn list
 
DEPENDENCIES
  None 

RETURN VALUE
  TRUE   -  If plmn is t3346 or in eplmn list
  FALSE  -  If plmn is a new plmn and not in eplmn list
 
SIDE EFFECTS
  None
===========================================================================*/
boolean emm_plmn_equivalent_t3346
(
  lte_rrc_service_ind_s     *rrc_service_ind,
  emm_ctrl_data_type        *emm_ctrl_data_ptr
);

#endif  /* FEATURE_LTE_REL10 */

#ifdef FEATURE_FEMTO_CSG
/*===========================================================================
FUNCTION      EMM_IS_MANUAL_CSG_SELECTION_SUCCESS

DESCRIPTION
 it would check if manual csg selection is successful
 
DEPENDENCIES
  None 

RETURN VALUE
  TRUE   -  If manual CSG selection success
  FALSE  -  If not manual_csg_selection
 
SIDE EFFECTS
  None
===========================================================================*/
boolean emm_is_manual_csg_selection_success
(
  emm_ctrl_data_type        *emm_ctrl_data_ptr
);
#endif
/*===========================================================================
FUNCTION      emm_dsm_pushdown

DESCRIPTION
 creates a dsm packet by putting into appropriate pool
 
DEPENDENCIES
  None 

RETURN VALUE
  None
 
SIDE EFFECTS
  None
===========================================================================*/
void emm_dsm_pushdown
(
   uint16              required_len,
   dsm_item_type       **dsm_ptr,
   byte                *message_ptr,
   emm_ctrl_data_type  *emm_ctrl_data_pr
);


/*===========================================================================
FUNCTION  EMM_CONVERT_EPS_BC_STATUS_TO_BIT_MASK

DESCRIPTION: This function coverts lte_nas_eps_bearer_context_status_type eps
             bearer context status to bit mask of two bytes
                          EBI/bit
             LSB   7  6  5  4  3  2 1 0 
             MSB  15 14 13 12 11 10 9 8

DEPENDENCIES
  None

RETURN VALUE
  unit16 

SIDE EFFECTS
  None

===========================================================================*/
uint16  emm_convert_eps_bc_status_to_bit_mask
(
  lte_nas_eps_bearer_context_status_type  eps_bc_status
);

/*===========================================================================
FUNCTION    EMM_RESIZE_MEM_FOR_OTA_MSG_PTR

DESCRIPTION
  Allocates memory for the ota_msg_ptr if ul_msg is greater than 
  EMM_MAX_MESSAGE_LENGTH
 
DEPENDENCIES
  None

RETURN VALUE
 NONE

SIDE EFFECTS
  None
===========================================================================*/
void emm_resize_mem_for_ota_msg_ptr
( 
  emm_ctrl_data_type      *emm_ctrl_data_ptr
);

/*===========================================================================
FUNCTION    EMM_REALLOC_MEM_FOR_OTA_MSG_PTR

DESCRIPTION
  This function reallocates the memory (2k) for ota_msg_ptr which was
  earlier allocated dynamically while processing UL_GENERIC_NAS_TRANSPORT 
  if the length of the OTA message was estimated to be greater than 2k

DEPENDENCIES
  None

RETURN VALUE
 NONE

SIDE EFFECTS
  None
===========================================================================*/
void emm_realloc_mem_for_ota_msg_ptr
( 
  emm_ctrl_data_type      *emm_ctrl_data_ptr
);

/*===========================================================================
FUNCTION  EMM_POPULATE_IMEI

DESCRIPTION
  This function populates IMEI to given structure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_populate_imei
(
  lte_nas_emm_imei_type  *imei_ptr,
  boolean                 eps_mobile_id
);

/*===========================================================================

FUNCTION  EMM_CHK_LIMITED_SERVICE_STATE

DESCRIPTION
  This function checkes if UE is in limited service state (23.122 3.5)

DEPENDENCIES
  None

RETURN VALUE
  TRUE - UE is in limited service state
  FALSE - UE is not in limited service state

SIDE EFFECTS
  None

===========================================================================*/
boolean  emm_chk_limited_service_state
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  EMM_RESET_EMC_SRV_STATE

DESCRIPTION
  This function reset emergency bearer service states

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_reset_emc_srv_state
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  EMM_CACHE_REGISTRATION_CTXT

DESCRIPTION
  This function caches current registration conetxt including last registered 
  TAI, GUTI, TIN type and EPS update status when UE is emergency attached for 
  a later restoration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_cache_registration_ctxt
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  EMM_RESTORE_REGISTRATION_CTXT

DESCRIPTION
  This function restores cached registration conetxt including last registered 
  TAI, GUTI, TIN type and EPS update status when UE is no longer emergency 
  attached.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_restore_registration_ctxt
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);


/*===========================================================================
FUNCTION  EMM_SEND_T3402_CHANGED_IND

DESCRIPTION
  This function sends NAS_EMM_T3402_CHANGED_IND with T3402 value

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_t3402_changed_ind
(
  dword  t3402_value
);

/*===========================================================================
FUNCTION    emm_bit_mask_check

DESCRIPTION
  Function checks value passed to see if particular element  is set

DEPENDENCIES
  None

RETURN VALUE
  returns TRUE if particular bit is set, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/

boolean emm_bit_mask_check
(
  uint32 value,
  byte bit_mask
);

/*===========================================================================
FUNCTION    emm_bit_mask_reset

DESCRIPTION
  Function resets a particular bit in the value feild.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None
===========================================================================*/

void emm_bit_mask_reset
(
  uint32 *value,
  byte bit_mask
);

/*===========================================================================
FUNCTION    emm_is_srvcc_supported

DESCRIPTION
  This function checks if UE supports SRVCC to GERAN or UMTS with
  the current configuration and EFS settings

DEPENDENCIES
  None

RETURN VALUE
  TRUE if SRVCC is supported, FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern boolean emm_is_srvcc_supported
(
  emm_ctrl_data_type      *emm_ctrl_data_ptr
);


/*===========================================================================
FUNCTION    emm_is_1xsrvcc_supported

DESCRIPTION
  This function checks if UE supports SRVCC to 1x with
  the current configuration and EFS settings

DEPENDENCIES
  None

RETURN VALUE
  TRUE if SRVCC is supported, FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/

extern boolean emm_is_1xsrvcc_supported
(
  emm_ctrl_data_type      *emm_ctrl_data_ptr
);

/*===========================================================================
FUNCTION  gmm_force_set_state

DESCRIPTION: 

This function forcefully sets GMM state and substate only when LTE is not active
  
DEPENDENCIES:
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void gmm_force_set_state
(
  gmm_state_type     gmm_state_arg, 
  gmm_substate_type  gmm_sub_state
);

/*===========================================================================
FUNCTION      EMM_REGISTRATION_ALLOWANCE_CHK

DESCRIPTION
  This function checks if a registration is allowed 
 
DEPENDENCIES
  None 

RETURN VALUE
  TRUE   -  If registration is allowed
  FALSE  -  If registration is not allowed
 
SIDE EFFECTS
  None
===========================================================================*/
boolean emm_registration_allowance_chk
(
  lte_rrc_service_ind_s     *rrc_service_ind,
  emm_ctrl_data_type        *emm_ctrl_data_ptr
);

#ifdef FEATURE_1XSRLTE
/*===========================================================================
FUNCTION    emm_ue_is_in_srlte_state

DESCRIPTION
  This function checks if srlte esr is allowed

DEPENDENCIES
  None

RETURN VALUE
  TRUE if it is allowed, FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_ue_is_in_srlte_state(emm_ctrl_data_type *emm_ctrl_data_ptr);
#endif

#if defined FEATURE_1XSRLTE||defined FEATURE_SGLTE
/*===========================================================================
FUNCTION    update_emm_sub_ue_mode

DESCRIPTION
  This function update ue submode 
 
DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
void emm_update_sub_ue_mode(boolean current_ue_submode_srlte);
#endif

/*===========================================================================
FUNCTION    emm_get_tmsi_based_nri

DESCRIPTION
  This function generate tmsi based nri

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
gmm_ie_tmsi_based_NRI_T emm_get_tmsi_based_nri(emm_ctrl_data_type      *emm_ctrl_data_ptr);

#ifdef FEATURE_LTE_REL9
/*===========================================================================
FUNCTION  EMM_IS_REGISTRATION_FOR_EMC_PDN_CONN

DESCRIPTION
  This function checks whether registration is for initiating PDN connection
  for emergency bearer services

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean  emm_is_registration_for_emc_pdn_conn
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================
FUNCTION  EMM_SEND_DEACT_NON_EMC_BEARER_IND

DESCRIPTION
  This function sends NAS_EMM_DEACT_NON_EMC_BEARER_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_deact_non_emc_bearer_ind
(
  emm_ctrl_data_type        *emm_ctrl_data_ptr
);
#endif /* FEATURE_LTE_REL9 */



/*===========================================================================

FUNCTION  emm_reject_cs_page_service_notification
DESCRIPTION: This function is called by EMM when it receives CS service nf

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_reject_cs_service_notification
(
  emm_ctrl_data_type        *emm_ctrl_data_ptr
);
/*===========================================================================

FUNCTION  emm_send_esr_csfb_reject_msg

DESCRIPTION: This function is called by EMM when it receives CS service nf and volte call 
             start indication is active 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_esr_csfb_reject_msg
(
  emm_ctrl_data_type        *emm_ctrl_data_ptr
);

void emm_send_diag_event(event_id_enum_type event,nas_event_end_result end_result,nas_event_end_reason end_reason,lte_nas_emm_cause_type cause);


/*===========================================================================
FUNCTION  emm_is_s1_mode_only

DESCRIPTION
  This function checks whether UE supports S1 mode only

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean  emm_is_s1_mode_only(emm_ctrl_data_type  *emm_ctrl_data_ptr);/* emm_is_s1_mode_only */

/*===========================================================================
FUNCTION  emm_read_carrier_specific_efs

DESCRIPTION
  This function reads carrier specific EFS/NV values

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_read_carrier_specific_efs(void);

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
FUNCTION  emm_process_mcfg_refresh

DESCRIPTION
  This function processes MCFG NV refresh indication for EMM specific EFS/NVs

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_mcfg_refresh(void);
#endif

/*===========================================================================

FUNCTION  emm_check_drop_cs_page_service_notification

DESCRIPTION: This function is called by EMM when it receives MT page or CS service nf

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_ignore_cs_page_cs_service_notification
(
  emm_ctrl_data_type        *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  emm_mmtel_status

DESCRIPTION: This function is called by EMM to check MMTEL status

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_mmtel_status
(
 emm_ctrl_data_type  *emm_ctrl_data_ptr
);
/*===========================================================================

FUNCTION  emm_mmtel_wms_status

DESCRIPTION: This function is called by EMM to check WMS or MMTEL status

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_wms_sms_status
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  emm_set_call_type_based_on_sib2

DESCRIPTION: This function is called by EMM to get call type used in RRC connection establishment request 
             triggered by service request cause as MO DATA and tau procedure cause as MO SIGNALING
             The call type is choosed for skipping ACB is most likely type when there is multiple indication
DEPENDENCIES
  None

RETURN VALUE
  Boolean 

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_set_call_type_based_on_sib2
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);


/*===========================================================================

FUNCTION  emm_set_call_type_per_mmtel_status
 
DESCRIPTION: set call type based on mmtel or wms_sms status 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_set_call_type_per_mmtel_and_sms_status
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  emm_check_mplmn_in_progress

DESCRIPTION: Check if Manual PLMN search is in progress

DEPENDENCIES
  True: in progress
  False: otherwise

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_check_mplmn_in_progress
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);
/*===========================================================================

FUNCTION  emm_skip_emc_retry

DESCRIPTION: This function is call to check if EMC retry should be skipped or not

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_skip_emc_retry
(
  void
);

#if defined  FEATURE_DUAL_SIM 
/*===========================================================================

FUNCTION  emm_skip_emc_retry

DESCRIPTION: This function is call to check if EMC retry should be skipped or not 
based on cs rat availability and NV lteonly_mcc and plmn is in whitelist or not 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_skip_emc_retry_per_subs
(
   sys_modem_as_id_e_type sub_id
);
#endif

/*===========================================================================

FUNCTION  emm_mmtel_status

DESCRIPTION: This function is called by EMM to check MMTEL status

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
boolean emm_mmtel_status
(
 emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================
FUNCTION  emm_check_ready_for_psm

DESCRIPTION: This function checks if NAS is ready to enter PSM mode

DEPENDENCIES
  None

RETURN VALUE
  PSM ready status

SIDE EFFECTS
  None
===========================================================================*/
extern sys_psm_status_e_type emm_check_ready_for_psm(emm_ctrl_data_type  *emm_ctrl_data_ptr);


/*===========================================================================
FUNCTION  emm_check_esr_for_ps

DESCRIPTION
  This function checks whether ESR for PS shall be started

DEPENDENCIES
  None

RETURN VALUE
  TRUE if ESR for PS can be started else FALSE

SIDE EFFECTS
  None
===========================================================================*/
extern boolean emm_check_esr_for_ps
(
   emm_ctrl_data_type *emm_ctrl_data_ptr
);


#ifdef FEATURE_LTE_REL10
/*===========================================================================
FUNCTION  emm_evaluate_low_priority_indicator_value

DESCRIPTION
  This function evaluates low priority indicator value for Device Properties IE

DEPENDENCIES
  None

RETURN VALUE
    TRUE if low priority, otherwise FALSE

SIDE EFFECTS
  None
===========================================================================*/
extern boolean emm_evaluate_low_priority_indicator_value
(
  emm_ctrl_data_type *emm_ctrl_data_ptr,
  lte_rrc_est_cause_e conn_est_cause
);
#endif

#ifdef FEATURE_NAS_EAB
/*===========================================================================
FUNCTION  emm_eab_cat_c_chk

DESCRIPTION: This function checks whether EAB is applicable for the UE of category C,
             i.e, the UE is neither in the PLMN listed as most preferred PLMN of
             the country where the UEs are roaming in the operator-defined PLMN
             selector list on the USIM, nor in their HPLMN nor in a PLMN that is
             equivalent to their HPLMN

DEPENDENCIES
  None

RETURN VALUE
  TRUE if EAB is applicable else FALSE

SIDE EFFECTS
  None
===========================================================================*/

extern boolean emm_eab_cat_c_chk(  emm_ctrl_data_type     *emm_ctrl_data_ptr);
#endif

/*=========================================================================== 
 
FUNCTION  EMM_SEND_TAU_COMPLETE_IND

DESCRIPTION
  This function sends NAS_EMM_TAU_COMPLETE_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
extern void emm_send_tau_complete_ind(void);
#endif /*FEATURE_LTE*/

#endif /* _EMM_UTILITY_H_ */


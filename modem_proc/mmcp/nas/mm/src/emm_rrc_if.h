
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_rrc_if.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/29/12   pahire  Added changes for UE Specific DRX Cycle for QChat/LT
02/24/10   zren    Added support for network selection modes of LIMITED MANUAL
                   ACQUISITION and LIMITED ROUTINE ACQUISITION
02/12/10   zren    Updated to always abort the existing connection before 
                   sending RRC DEACT REQ
12/10/09   zren    Updated to make routing information inclusion in 
                   rrc conn req spec compliant
10/06/09   vdr     Compiler warnings fixed
09/21/09   MNK     Changes due to IRAT design
07/15/09   zren    Modified message list handling functions to use message 
                   pointer as parameter
6/13/09    RI      Modified emm_send_rrc_data_request() to include EMM
                   ctrl data
04/14/09   RI      Added cell barring flag to emm_rrc_abort_connection()
04/17/09   zren    Modified to support saving NAS messages in pended message 
                   list in the format of lte_nas_outgoing_msg_type
                   Added one parameter to function emm_send_rrc_connection_establishment_request() 
                   Sepearted RRC_DATA_CNF and RRC_EST_CNF for RRC_EST_REQ
                   Modified EMM connection state machines
01/29/09   vdr     Added two new function prototypes emm_send_sim_update() and 
                   emm_send_rrc_system_update
===========================================================================*/
#ifndef _EMM_RRC_IF_H_
#define _EMM_RRC_IF_H_
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "lte_rrc_ext_msg.h"
#include "emm_database.h"
#include "lte_nas.h"
#include "emm_update_lib.h"

/*===========================================================================

                        EXTERNAL DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION    EMM_SEND_RRC_CONNECTION_ESTABLISHMENT_REQUEST

DESCRIPTION
  This function sends RRC_CONN_EST_REQ to RRC to setup the connection

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_rrc_connection_establishment_request
(
  lte_rrc_est_cause_e             lte_rrc_est_cause,
  lte_nas_outgoing_msg_type      *emm_message_ptr,
  dword                           sequence_number,
  lte_nas_conn_req_routing_info   routing_info,
  emm_ctrl_data_type             *emm_ctrl_data_ptr
) ;

extern
void emm_send_rrc_data_request
(
  lte_nas_outgoing_msg_type   *emm_message_ptr,
  dword                        sequence_number,
  emm_ctrl_data_type           *emm_ctrl_data_ptr
) ;

extern
void emm_rrc_abort_connection
(
  emm_ctrl_data_type         *emm_ctrl_data_ptr,
  boolean                    cell_barring_reqd,
  lte_rrc_conn_abort_cause_e abort_cause
) ;

/*===========================================================================

FUNCTION EMM_SEND_RRC_SERVICE_REQ

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern
void emm_send_rrc_service_req
(
  //lte_rrc_svc_req_type_e svc_req_type, /*!< Type of request */

  /* network select mode */
  sys_network_selection_mode_e_type network_select_mode,

  boolean req_plmn_info_is_valid, /*!< Is requested PLMN valid */

  /* valid for svc_req_type of LTE_RRC_SVC_REQ_NORMAL only */
  sys_plmn_id_s_type *req_plmn, /*!< Requested PLMN */

  boolean hplmn_info_is_valid,  

  boolean ehplmn_info_is_valid,  

  boolean eplmn_info_is_valid,  

  boolean scan_is_new, /*!< Is service request new or not */

  boolean use_timer, /*!< True = use timer (OOOS), False = don't use timer 
                          (Non-UOOS) */

  /* valid only if use_timer is TRUE */
  dword lte_scan_time, /*!< UOOS search timer for LTE in ms */

  /* service search type TBD */
  emm_ctrl_data_type *emm_ctrl_data_ptr,

  sys_rat_pri_list_info_s_type *rat_pri_list,

  /* request type from reg */
  mmr_reg_req_e_type reg_req_type

#ifdef FEATURE_LTE_REL9
  ,
  uint32  additional_info
#endif                
#ifdef FEATURE_FEMTO_CSG
  ,sys_csg_id_type  csg_id
#endif 
  ,sys_scan_scope_e_type scan_scope  
  ,uint32   trm_timeout
) ;

/*===========================================================================

FUNCTION    EMM_SEND_RRC_DEACT_REQ

DESCRIPTION
  This function sends LTE_RRC_DEACTIVATE_REQ to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_rrc_deact_req
(
  sys_stop_mode_reason_e_type   stop_mode_reason,
  emm_ctrl_data_type            *emm_ctrl_data_ptr
); 

/*===========================================================================

FUNCTION    EMM_INIT_RRC_DEACT_REQ

DESCRIPTION
  This function sends RRC_DEACT_REQ or aborts the connection depending on EMM
  connection state    

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_init_rrc_deact_req
(
  sys_stop_mode_reason_e_type   stop_mode_reason,
  emm_ctrl_data_type           *emm_ctrl_data_ptr
);

extern
void emm_send_sim_update
(
   emm_ctrl_data_type *emm_ctrl_data_ptr
) ;

extern
void emm_send_rrc_system_update
(
   emm_ctrl_data_type *emm_ctrl_data_ptr
) ;

/*===========================================================================

FUNCTION EMM_SEND_RRC_NW_SEL_MODE_RESET_REQ

DESCRIPTION
  This function sends LTE_RRC_NW_SEL_MODE_RESET_REQ to RRC using the given 
  network selection mode as payload

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_rrc_nw_sel_mode_reset_req
(
  sys_network_selection_mode_e_type  network_select_mode,
  sys_rat_pri_list_info_s_type      *rat_pri_list_ptr,
  emm_ctrl_data_type                *emm_ctrl_data_ptr
);

void emm_send_rrc_act_rsp
(
  boolean  status,
  emm_ctrl_data_type                *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION EMM_SEND_RRC_PLMN_SEARCH_REQ

DESCRIPTION
  This function builds and sends LTE_RRC_PLMN_SEARCH_REQ to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_rrc_plmn_search_req
(
  mmr_plmn_search_req_s_type  *plmn_search_req_ptr,
  emm_ctrl_data_type          *emm_ctrl_data_ptr,
  boolean                      hplmn_info_is_valid
);

/*===========================================================================

FUNCTION EMM_SEND_RRC_PLMN_SEARCH_ABORT_REQ

DESCRIPTION
  This function builds and sends LTE_RRC_PLMN_SEARCH_ABORT_REQ to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_rrc_plmn_search_abort_req
(
  void
);

/*===========================================================================

FUNCTION EMM_SEND_NAS_UMTS_KEY_RSP

DESCRIPTION
  This function sends LTE_RRC_NAS_UMTS_KEY_RSP TO RRC 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_nas_umts_key_rsp
(
  auth_gprs_cipherkey_type    Ck,
  auth_gprs_integritykey_type Ik,
  auth_gprs_cipherkey_type    Ck_srvcc,
  auth_gprs_integritykey_type Ik_srvcc
);

/*===========================================================================

FUNCTION EMM_SEND_NAS_GSM_KEY_RSP

DESCRIPTION
  This function sends LTE_RRC_NAS_GSM_KEY_RSP TO RRC 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_nas_gsm_key_rsp
(
  auth_gprs_cipherkey_type    Ck,
  auth_gprs_integritykey_type Ik,
  auth_gprs_cipherkey_type    Ck_srvcc,
  auth_gprs_integritykey_type Ik_srvcc,
  auth_gsm_cipherkey_type     Kc,
  auth_gsm_cipherkey_type     gsm_kc_128bit_in,
  byte                        gsm_kcsn
);

/*===========================================================================

FUNCTION EMM_SEND_NAS_LTE_KEY_RSP

DESCRIPTION
  This function sends LTE_RRC_NAS_LTE_KEY_RSP TO RRC 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_nas_lte_key_rsp
(
  byte *nas_kasme,
  byte *nas_ul_cnt
);

/*===========================================================================

FUNCTION    EMM_SEND_RRC_DRX_UPDATE

DESCRIPTION
  This function sends the new DRX value that was requested
  by QCHAT application to L-RRC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS  

===========================================================================*/
void emm_send_rrc_drx_update
(
  void
);


/*===========================================================================

FUNCTION    emm_send_rrc_csfb_call_status_ind

DESCRIPTION
  This function sends rc_csfb_call_status to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_rrc_csfb_call_status_ind
(
  lte_rrc_csfb_call_status_type_e call_status
) ;
/*===========================================================================
FUNCTION    EMM_CHECK_HIGH_PRIORITY

DESCRIPTION
  This function determinates whether a call is high priority access or not 
  according to Annex D of NAS Spec 
DEPENDENCIES
  None

RETURN VALUE
  TRUE: High priority
  FALSE: Not high priority

SIDE EFFECTS
  None
===========================================================================*/
boolean    emm_check_high_priority
(
  emm_ctrl_data_type               *emm_ctrl_data_ptr
);

/*===========================================================================
FUNCTION    EMM_SEND_RRC_CELL_BAR_REQ

DESCRIPTION
  This function sends LTE_RRC_CELL_BAR_REQ to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_rrc_cell_bar_req
(
  emm_ctrl_data_type               *emm_ctrl_data_ptr
);
/*===========================================================================

FUNCTION    EMM_SEND_RRC_FORBIDDEN_TA_LA_LIST_RESET

DESCRIPTION
  This function sends the forbidden TA/LA list reset to RRC
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void emm_send_rrc_forbidden_ta_la_list_reset
(
  void
);

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION    EMM_SEND_DS_STAT_CHANGE_IND

DESCRIPTION
  This function sends the DS stat change Indication to RRC

  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void emm_send_rrc_ds_stat_change_req
(
  boolean dual_standby_mode,
  sys_modem_dual_standby_pref_e_type dual_standby_mode_info,
  sys_modem_device_mode_e_type   device_mode
);
#endif


#if defined FEATURE_DUAL_SIM|| defined FEATURE_1XSRLTE
/*===========================================================================

FUNCTION    EMM_SEND_RRC_TRM_PRIORITY

DESCRIPTION
  This function sends the TRM priority to LRRC 

  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void emm_send_rrc_trm_priority
(
  lte_trm_priority_e trm_priority
);
#endif

/*===========================================================================

FUNCTION    EMM_SEND_RRC_UE_CAP_CHANGE_INFO

DESCRIPTION
  This function sends the UE capabilities info to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void emm_send_rrc_ue_cap_change_info
(
  mm_cmd_type  *cmd_ptr,
  emm_ctrl_data_type *emm_ctrl_data_ptr
);

#ifdef FEATURE_DUAL_DATA
/*===========================================================================

FUNCTION    EMM_SEND_RRC_MODE_PREF_UPDATE_IND

DESCRIPTION
  This function sends an indication to L-RRC to inform service domain (MS op mode) for Non-LTE subscription. 

  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void emm_send_rrc_nas_domain_pref_ind
(
  void
);
#endif /* FEATURE_DUAL_DATA */

#endif /* _EMM_RRC_IF_H_ */
#endif


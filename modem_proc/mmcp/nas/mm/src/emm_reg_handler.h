
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_reg_handler.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/29/12   pahire  Added changes for UE Specific DRX Cycle for QChat/LTE 
09/12/11   HC      Replacing the #include "reg_mm.h" with #include "reg_mm_v.h" 
                   as a part of CMI activity 
07/04/11   abhi    added processing of ph_status_change_req
05/18/10   zren    Updated fun param to set only_for_reg flag in REG CNF and 
                   REG IND
10/06/09   RI      Fix new line warning at EOF.
05/07/09   zren    Added support to set reg container instead of sending out 
                   RRC_SERVICE_REQUEST if EMM is in EMM_CONNECTED_STATE or 
                   EMM_WAITING_FOR_RRC_CONFIRMATION_STATE
01/22/09   vdr     Start using mmtask.h
===========================================================================*/
#ifndef _EMM_REG_HANDLER_H_
#define _EMM_REG_HANDLER_H_
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "sys.h"
#include "sys_plmn_selection_v.h"
#include "reg_mm_v.h"
#include "lte_rrc_ext_msg.h"
#include "emm_database.h"
#include "mmtask.h"
#include "lte_rrc_irat_msg.h"

/*===========================================================================

                        MACROS DEFINITION

===========================================================================*/


/*===========================================================================

                        TYPES DEFINITION

===========================================================================*/

typedef
void (*reg_service_report_type)
  (
    lte_rrc_service_ind_s *rrc_service_ind
  ) ; /* end reg_service_report_type() */

/* typedef union emm_reg_cmd_type_tag {
   IMH_T hdr;

   mmr_reg_req_s_type                 mmr_reg_req;
   mmr_sim_available_req_s_type       mmr_sim_available_req;
   mmr_sim_not_available_req_s_type   mmr_sim_not_available_req;
   mmr_stop_mode_req_s_type           mmr_stop_mode_req;
   mmr_plmn_search_req_s_type         mmr_plmn_search_req;
   mmr_act_req_s_type                 mmr_act_req;
   mmr_plmn_search_abort_req_s_type   mmr_plmn_search_abort_reg;
}  mm_int_cmd_type_T ;

typedef struct {

   q_link_type      link;

   mm_int_cmd_type_T cmd;

}  mm_cmd_type; */

typedef
void (*emm_reg_container_type)
  (
    emm_ctrl_data_type           *emm_ctrl_data_ptr
  ) ; /* end of rrc_connection_failure_type() */

/*===========================================================================

FUNCTION EMM_SEND_EMPTY_PLMNSEARCH_CNF

DESCRIPTION
  This function sends MMR_PLMN_SEARCH_CNF with status of incomplete

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_empty_plmnsearch_cnf
(
  mmr_plmn_search_req_s_type  *plmn_search_req_ptr
);

extern
void emm_send_reg_cnf
(
  mmr_cause_e_type     mmr_cause,
  boolean              only_for_reg,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
) ;

extern void emm_send_reg_ind
(
  mmr_cause_e_type     mmr_cause,
  boolean              only_for_reg,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

extern
void emm_reject_service_req
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
) ;

extern
void emm_process_abort_plmnsearch
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
) ;

extern
void emm_process_reg_command
( 
  mm_cmd_type        *reg_msg_ptr, 
  emm_ctrl_data_type *emm_ctrl_data_ptr 
) ;

extern
void emm_send_reg_camped_ind
(
  sys_radio_access_tech_e_type active_rat,
  lte_rrc_camped_svc_info_s  camped_srv_info,
  boolean     policy_change_pending
) ;

void emm_process_pended_reg_req(emm_ctrl_data_type *emm_ctrl_data_ptr);

/*===========================================================================

FUNCTION EMM_SEND_MMR_PLMN_SEARCH_CNF

DESCRIPTION
  This function builds MMR_PLMN_SEARCH_CNF with data from LTE RRC PLMN SEARCH
  CNF and sends it to REG 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_send_mmr_plmn_search_cnf
(
  lte_rrc_plmn_search_cnf_s  *rrc_plmn_search_cnf_ptr
);

#ifdef FEATURE_LTE_REL9
/*===========================================================================

FUNCTION EMM_SEND_MMR_LTE_CELL_INFO_IND

DESCRIPTION
  This function builds EMM_SEND_MMR_LTE_CELL_INFO_IND with data 
  from LTE RRC PLMN SERVICE IND and sends it to REG  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_send_mmr_lte_cell_info_ind
(
  boolean emergency_access_barred
);
#endif

void emm_send_mmr_lte_pseudo_resel_low_priority_ind(uint32 earfcn);

/*===========================================================================

FUNCTION  EMM_PROCESS_PS_DETACH_REQ

DESCRIPTION
  This function processes PS DETACH REQ from REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_ps_detach_req
(
  mmr_ps_detach_req_s_type  ps_detach_req, 
  emm_ctrl_data_type       *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  EMM_PROCESS_SET_DRX_REQ

DESCRIPTION
  This function processes SET DRX REQ from REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_set_drx_req
(
  mmr_set_drx_req_s_type    set_drx_req, 
  emm_ctrl_data_type       *emm_ctrl_data_ptr
);

extern void emm_process_ph_status_change_req
(
  mm_cmd_type              *reg_cmd_ptr, 
  emm_ctrl_data_type       *emm_ctrl_data_ptr
);

#ifdef FEATURE_DUAL_DATA 
extern void  emm_process_data_prio_ind(mmr_data_priority_ind_s_type mmr_data_prio_ind, 
                                emm_ctrl_data_type       *emm_ctrl_data_ptr);
#endif

#ifdef FEATURE_FEMTO_CSG
/*===========================================================================

FUNCTION  emm_send_reg_home_nodeb_name_ind

DESCRIPTION
  This function sends  MMR_HOME_NODEB_NAME_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void emm_send_reg_home_nodeb_name_ind
(
  lte_rrc_csg_info_update_ind_s  *rrc_csg_info_update
);
#endif /*FEATURE_FEMTO_CSG*/

/*===========================================================================

FUNCTION  EMM_PROCESS_BLOCK_PLMN_REQ

DESCRIPTION
  This function processes block PLMN request  from REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void emm_process_block_plmn_req
(
  mmr_block_plmn_req_s_type *block_plmn_req_ptr, 
  emm_ctrl_data_type       *emm_ctrl_data_ptr
);

/*===========================================================================
FUNCTION  EMM_SEND_MMR_RRC_SIB6_INFO_IND

DESCRIPTION
  This function sends SIB6 INFO to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void emm_send_mmr_rrc_sib6_info_ind
(
  lte_rrc_sib6_info_ind_s  *rrc_sib6_info_ind_ptr
);

/*===========================================================================
FUNCTION  EMM_SEND_MMR_RRC_SIB7_INFO_IND

DESCRIPTION
  This function sends SIB7 INFO to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void emm_send_mmr_rrc_sib7_info_ind
(
  lte_rrc_sib7_info_ind_s  *rrc_sib7_info_ind_ptr
);
#endif /* _EMM_REG_HANDLER_H_ */

#endif /*FEATURE_LTE*/


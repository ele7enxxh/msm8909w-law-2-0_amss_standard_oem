
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_rrc_emm_if.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/27/11   PA      Added changes for Security Key generation during W2L PS HO
02/01/10   PA      Added changes for Security Key generation during L2W PS HO
10/06/09   vdr     Compiler warnings fixed
01/30/09   vdr     Added processing of new lte_rrc_conn_est_timer_update_ind_s
===========================================================================*/
#ifndef _EMM_RRC_EMM_IF_H_
#define _EMM_RRC_EMM_IF_H_
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "msgr_umid.h"
#include "msgr_types.h"
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_irat_msg.h"

/*===========================================================================

                        MACROS DEFINITION

===========================================================================*/


/*===========================================================================

                        TYPES DEFINITION

===========================================================================*/


typedef union
{  
  msgr_hdr_s                             msg_hdr; /*!< Message router header */
  lte_rrc_conn_est_cnf_s                 rrc_conn_est_cnf ;
  lte_rrc_ul_data_cnf_s                  rrc_ul_data_cnf ;
  lte_rrc_deactivate_cnf_s               rrc_deactivate_cnf ;
  lte_rrc_act_ind_s                      rrc_act_ind ;
  lte_rrc_service_ind_s                  rrc_service_ind ;
  lte_rrc_barring_update_ind_s           rrc_barring_update_ind;
  lte_rrc_pseudo_resel_low_priority_ind_s rrc_pseudo_resel_low_priority_ind;
  lte_rrc_conn_rel_ind_s                 rrc_conn_rel_ind ;
  lte_rrc_dl_data_ind_s                  rrc_dl_data_ind ;
  lte_rrc_page_ind_s                     rrc_page_ind ;
  lte_rrc_active_eps_bearer_update_ind_s rrc_active_eps_bearer_update_ind ;
  lte_rrc_act_rsp_s                      rrc_act_rsp ;
  lte_rrc_conn_est_timer_update_ind_s    rrc_conn_est_timer_update_ind ;
  lte_rrc_plmn_search_cnf_s              rrc_plmn_search_cnf;
  lte_rrc_irat_tunnel_ul_msg_cnf_s       rrc_irat_tunnel_ul_msg_cnf;
  lte_rrc_irat_tunnel_dl_msg_ind_s       rrc_irat_tunnel_dl_msg_ind;
  
#ifdef FEATURE_LTE_TO_HDR_OH
  lte_irat_cdma_tunnel_dl_msg_ind_s      rrc_irat_cdma_tunnel_dl_msg_ind;
#endif
  lte_rrc_1xcsfb_ho_started_ind_s        rrc_1xcsfb_ho_started_ind;
  lte_rrc_1xcsfb_ho_failed_ind_s         rrc_1xcsfb_ho_failed_ind;
  lte_rrc_1xsrvcc_ho_started_ind_s       rrc_1xsrvcc_ho_started_ind;
  lte_rrc_1xsrvcc_ho_failed_ind_s        rrc_1xsrvcc_ho_failed_ind;
  lte_rrc_nas_umts_key_ind_s             rrc_umts_key_ind;
  lte_rrc_nas_lte_key_ind_s              rrc_lte_key_ind;
  lte_rrc_csg_info_update_ind_s          rrc_csg_info_update_ind;
  lte_rrc_cco_nacc_completed_ind_s       rrc_cco_nacc_completed_ind;
  lte_rrc_sib6_info_ind_s                rrc_sib6_info_ind;
  lte_rrc_sib7_info_ind_s                rrc_sib7_info_ind;
#ifdef FEATURE_NAS_EAB
  lte_rrc_eab_update_ind_s               rrc_eab_update_ind;
#endif
} rrc_msgr_cmd_type ;


typedef struct emm_cmd

{

  q_link_type         link; /*!< Queue link. This must be the first element of the cmd_type. 
                            MSGR expects q_link to be the first element and the msgr_hdr_s 
                            as the second element.So, after link_type no element should be 
                            added and the only element is the union below*/

 
  /*! @brief List all the incoming messages/commands*/ 
  rrc_msgr_cmd_type rrc_msgr_cmd ;

} emm_rrc_cmd_type ;

#endif /* _EMM_RRC_EMM_IF_H_ */

#endif /*FEATURE_LTE*/


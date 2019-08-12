#ifndef REG_TASK_V_H
#define REG_TASK_V_H
/*==============================================================================


                   R E G  -  T A S K   H E A D E R   F I L E


                                  DESCRIPTION

  This module defines the types and functions necessary to send a command
  to the REG task.

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_task.h_v   1.1   06 Jun 2002 10:45:00   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/reg_task_v.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
06/20/01   sbk     The command names from CM has changed to include cm_ prefix
08/16/01   ab      Renamed mmi_reg.h references to reg_cm.  Renamed MMI to CM.
10/21/01   sbk     mmr_reg_rej_T, mmr_service_ind_T added to the reg_cmd_type
10/31/01   jca     Added mmr_act_cnf_T to reg_cmd_type.
02/13/02   kwa     Updated to support dual mode requirements.
05/31/02   kwa     Made reg_wait an external function.
09/19/02   kwa     Added cm_act_req to reg_cmd_type.
05/17/04   kwa     Added mmr_camped_ind to reg_cmd_type.
02/22/05   up      Updated for Cingular ENS feature.
                   refresh_fcn_ind command is added to the reg command union.
03/01/05   up      ENS Fix - added sim_refresh_ind in reg command.
09/19/05   up      Added cm_hplmn_search_req in reg_cmd_type to support
                   on demand HPLMN search from user.
11/07/05   ajt     Added msg type mmr_emergency_num_list_ind to reg_cmd_type.                   
04/06/06   sn      Added mmr_cell_service_ind in reg_cmd_type.
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "queue.h"
#include "ms_timer.h"
#include "reg_cm.h"
#include "reg_mm.h"
#include "rex.h"
#include "reg_sim.h"

#include "reg_ecall.h"

/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

typedef struct reg_mcfg_refresh_ind_s
{
  IMH_T                      IMH;
  sys_modem_as_id_e_type     as_id;
  uint32                     reference_id;
}reg_mcfg_refresh_ind_s_type;

/*
** Union of all the input command types.
*/
typedef struct
{
  q_link_type      link;

  union cmd_u_type
  {
    /*
    ** hdr is defined to allow easy access to the header fields.
    */
    IMH_T hdr;

    /*
    ** MM ==> REG
    */
    mmr_camped_ind_s_type            mmr_camped_ind;
    mmr_mm_information_ind_s_type    mmr_mm_information_ind;
    mmr_plmn_search_cnf_s_type       mmr_plmn_search_cnf;
    mmr_reg_cnf_s_type               mmr_reg_cnf;
    mmr_service_ind_s_type           mmr_service_ind;
    mmr_sim_available_cnf_s_type     mmr_sim_available_cnf;
    mmr_sim_not_available_cnf_s_type mmr_sim_not_available_cnf;
    mmr_stop_mode_cnf_s_type         mmr_stop_mode_cnf;
    mmr_emergency_num_list_ind_s_type  mmr_emergency_num_list_ind;    
    mmr_attach_failed_ind_s_type     mmr_attach_failed_ind;

    mmr_cell_service_ind_s_type      mmr_cell_service_ind;

    mmr_hspa_call_status_ind_s_type      mmr_hspa_call_status_ind;

    mmr_ph_status_change_cnf_s_type  mmr_ph_status_change_cnf;
    

    mmr_conn_mode_service_status_ind_s_type mmr_conn_mode_service_status_ind;

    mmr_reg_reject_ind_s_type        mmr_reg_reject_ind;   

    mmr_suitable_search_end_ind_s_type mmr_suitable_search_end_ind;
    mmr_ps_detach_cnf_s_type           mmr_ps_detach_cnf;

    mmr_disable_bplmn_ind_s_type mmr_disable_bplmn_ind;

    mmr_cell_access_ind_s_type       mmr_cell_access_ind;

    mmr_ds_stat_chgd_cnf_s_type      mmr_ds_stat_chgd_cnf;

    mmr_fplmn_list_change_ind_s_type      mmr_fplmn_list_change_ind;

    mmr_set_drx_cnf_s_type                  mmr_set_drx_cnf;
    mmr_get_drx_cnf_s_type                  mmr_get_drx_cnf;
    mmr_home_nodeb_name_ind_s_type          mmr_home_nodeb_name_ind;
    mmr_lte_cell_info_ind_s_type            mmr_lte_cell_info_ind;
    mmr_service_stat_ind_s_type             mmr_service_stat_ind;
    mmr_stack_deact_ind_s_type              mmr_stack_deact_ind;
    mmr_stack_act_ind_s_type                mmr_stack_act_ind;
    mmr_update_curr_rat_list_ind_type       mmr_update_curr_rat_list_ind;
    mmr_multimode_subs_chgd_req_s_type      mmr_multimode_subs_chgd_cnf;
    mmr_csg_search_trigger_ind_s_type       mmr_csg_search_trigger_ind;
    mmr_csg_list_read_complete_ind_s_type   mmr_csg_list_read_complete_ind;
    mmr_t3346_timer_ind_s_type              mmr_t3346_timer_ind;
    mmr_rrc_pseudo_resel_low_priority_ind_s_type  mmr_pseudo_resel_low_priority_ind;
    mmr_reg_req_ignored_ind_s_type          mmr_reg_req_ignored_ind;
    mmr_rrc_sib6_info_ind_s_type            mmr_rrc_sib6_info_ind;
    mmr_rrc_sib7_info_ind_s_type            mmr_rrc_sib7_info_ind;
    mmr_psm_ready_rsp_s_type                mmr_psm_ready_rsp;
    mmr_psm_ready_ind_s_type                mmr_psm_ready_ind;
    /*
    ** CM ==> REG
    */
    cm_network_list_req_s_type       cm_network_list_req;
    cm_service_req_s_type            cm_service_req;
    cm_sim_available_req_s_type      cm_sim_available_req;
    cm_sim_not_available_req_s_type  cm_sim_not_available_req;
    cm_stop_mode_req_s_type          cm_stop_mode_req;

    cm_hplmn_search_req_s_type       cm_hplmn_search_req;

    cm_ph_status_chgd_req_s_type     cm_ph_status_chgd_req;    

    cm_exit_from_pwr_save_s_type     cm_exit_from_pwr_save;


    cm_ds_stat_chgd_req_s_type       cm_ds_stat_chgd_req;
    cm_multimode_subs_chgd_req_s_type cm_multimode_subs_chgd_req;

    cm_ds_tuneaway_stat_chgd_ind_s_type  cm_ds_tuneaway_stat_chgd_ind;


    cm_ps_detach_req_s_type                      cm_ps_detach_req;
    cm_block_plmn_req_s_type                     cm_block_plmn_req;
    cm_set_drx_req_s_type                        cm_set_drx_req;
    cm_get_drx_req_s_type                        cm_get_drx_req;
    cm_set_hplmn_irat_search_timer_req_s_type    cm_set_hplmn_irat_search_timer_req;
    cm_set_hplmn_search_timer_req_s_type         cm_set_hplmn_search_timer_req;
    cm_get_hplmn_search_timer_req_s_type         cm_get_hplmn_search_timer_req;
    cm_ue_mode_ind_s_type                        cm_ue_mode_ind;
    cm_csg_select_config_update_ind_s_type       cm_csg_select_config_update_ind;
    cm_data_priority_ind_s_type                  cm_data_prio_ind;
    cm_data_roaming_ind_s_type                   cm_data_roaming_ind;
    cm_dds_switch_ind_s_type                     cm_dds_switch_ind;
    cm_psm_ready_req_s_type                      cm_psm_ready_req;
    cm_roaming_ind_s_type                        cm_roaming_ind;
    /*
    ** Timers
    */
    timer_expiry_T                   timer_expiry;

    /*
    ** REFRESH & FCN indication GSDI => REG
    */
    sim_refresh_ind_s_type           sim_refresh_ind;
    sim_refresh_fcn_ind_s_type       sim_refresh_fcn_ind;

    sim_subs_ready_ind_s_type        sim_subs_ready_ind;
    sim_client_id_ind_s_type         sim_client_id_ind;
    reg_sim_card_unavailable_ind_s_type  sim_card_unavailable_ind;

   reg_sim_card_busy_ind_s_type   sim_busy_ind;

    ecall_reg_mode_ind_s_type        ecall_reg_mode_ind;

    /* MCFG ==> REG */
    reg_mcfg_refresh_ind_s_type       mcfg_refresh_ind;
 } cmd;
   
} reg_cmd_type;

/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

/*==============================================================================

                             FUNCTION DECLARATIONS

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================

FUNCTION NAME

  reg_put_cmd

DESCRIPTION

  Function that places the input command on the REG command queue.

RETURN VALUE

  None

==============================================================================*/

extern void reg_put_cmd
(
  reg_cmd_type *cmd_ptr
);

/*==============================================================================

FUNCTION NAME

  reg_cm_get_cmd_buf

DESCRIPTION

  Function that allocates memory for the specified REG command.

RETURN VALUE

  reg_cmd_type* Pointer to the allocated reg_cmd_type.

==============================================================================*/

extern reg_cmd_type* reg_cm_get_cmd_buf
(
  cm_to_reg_cmd_type cmd
);

/*==============================================================================

FUNCTION NAME

  reg_ecall_get_cmd_buf

DESCRIPTION

  Function that allocates memory for the specified REG command.

RETURN VALUE

  reg_cmd_type* Pointer to the allocated reg_cmd_type.

==============================================================================*/
reg_cmd_type* reg_ecall_get_cmd_buf
(
  ecall_to_reg_cmd_type cmd_type
);



#ifdef __cplusplus
}
#endif


#endif

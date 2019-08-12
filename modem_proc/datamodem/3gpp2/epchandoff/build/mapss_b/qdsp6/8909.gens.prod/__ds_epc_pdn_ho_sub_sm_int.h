/*=============================================================================

    __ds_epc_pdn_ho_sub_sm_int.h

Description:
  This file contains the machine generated header file for the state machine
  specified in the file:
  ds_epc_pdn_ho_sub_sm.stm

=============================================================================*/

/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies Incorporated.
  All Rights Reserved
  Qualcomm Confidential and Proprietary

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


#ifndef __DS_EPC_PDN_HO_SUB_SM_INT_H
#define __DS_EPC_PDN_HO_SUB_SM_INT_H

#ifdef __cplusplus
/* If compiled into a C++ file, ensure symbols names are not mangled */
extern "C"
{
#endif

/* Include external state machine header */
#include "__ds_epc_pdn_ho_sub_sm.h"

/* Begin machine generated internal header for state machine array: epc_pdn_ho_sub_sm[] */

/* Suppress Lint suggestions to const-ify state machine and payload ptrs */
/*lint -esym(818,sm,payload) */

/* Define a macro for the number of SM instances */
#define epc_pdn_ho_sub_sm_NUM_INSTANCES 16

/* Define a macro for the number of SM states */
#define epc_pdn_ho_sub_sm_NUM_STATES 7

/* Define a macro for the number of SM inputs */
#define epc_pdn_ho_sub_sm_NUM_INPUTS 12

#ifndef STM_DATA_STRUCTURES_ONLY
/* State Machine entry/exit function prototypes */
void epc_pdn_ho_sub_sm_init(stm_state_machine_t *sm,void *payload);
void epc_pdn_ho_sub_sm_cleanup(stm_state_machine_t *sm,void *payload);


/* State entry/exit function prototypes */
void epc_pdn_ho_sub_sm_enter_idle(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_pdn_ho_sub_sm_exit_idle(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_pdn_ho_sub_sm_enter_ho_started(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_pdn_ho_sub_sm_exit_ho_started(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_pdn_ho_sub_sm_enter_ho_trat_going_down(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_pdn_ho_sub_sm_exit_ho_trat_going_down(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_pdn_ho_sub_sm_enter_prereg_started(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_pdn_ho_sub_sm_exit_prereg_started(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_pdn_ho_sub_sm_enter_prereg_completed(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_pdn_ho_sub_sm_exit_prereg_completed(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_pdn_ho_sub_sm_enter_prereg_ho_waiting_rsp(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_pdn_ho_sub_sm_exit_prereg_ho_waiting_rsp(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_pdn_ho_sub_sm_enter_srat_down(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_pdn_ho_sub_sm_exit_srat_down(stm_state_machine_t *sm,stm_state_t _state,void *payload);


/* Transition function prototypes */
stm_state_t epc_pdn_ho_sub_sm_proc_ho_in_idle(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_switch_ho_in_idle(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_prereg_in_idle(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_srat_rcvrd_in_idle(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_ho_rsp_success_in_idle(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_ho_rsp_failure_in_idle(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_trat_up_in_idle(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_trat_down_in_idle(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_srat_down_in_idle(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_tmr_srat_tmr_exp_in_idle(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_comp_trat_down_in_idle(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_comp_trat_up_in_idle(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_ho_in_ho_started(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_switch_ho_in_ho_started(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_prereg_in_ho_started(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_srat_rcvrd_in_ho_started(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_ho_rsp_success_in_ho_started(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_ho_rsp_failure_in_ho_started(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_trat_up_in_ho_started(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_trat_down_in_ho_started(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_srat_down_in_ho_started(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_tmr_srat_tmr_exp_in_ho_started(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_comp_trat_down_in_ho_started(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_comp_trat_up_in_ho_started(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_ho_in_ho_trat_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_switch_ho_in_trat_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_prereg_in_ho_trat_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_srat_rcvrd_in_ho_trat_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_ho_rsp_success_in_ho_trat_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_ho_rsp_failure_in_ho_trat_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_trat_up_in_ho_trat_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_trat_down_in_ho_trat_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_srat_down_in_ho_trat_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_tmr_srat_tmr_exp_in_ho_trat_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_comp_trat_down_in_ho_trat_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_comp_trat_up_in_ho_trat_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_ho_in_prereg_started(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_switch_ho_in_prereg_started(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_prereg_in_prereg_started(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_srat_rcvrd_in_prereg_started(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_ho_rsp_success_in_prereg_started(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_ho_rsp_failure_in_prereg_started(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_trat_up_in_prereg_started(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_trat_down_in_prereg_started(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_srat_down_in_prereg_started(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_tmr_srat_tmr_exp_in_prereg_started(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_ho_in_prereg_completed(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_switch_ho_in_prereg_completed(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_prereg_in_prereg_completed(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_srat_rcvrd_in_prereg_completed(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_ho_rsp_success_in_prereg_completed(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_ho_rsp_failure_in_prereg_completed(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_trat_up_in_prereg_completed(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_trat_down_in_prereg_completed(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_srat_down_in_prereg_completed(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_tmr_srat_tmr_exp_in_prereg_completed(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_ho_in_prereg_ho_waiting_rsp(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_switch_ho_in_ho_waiting_rsp(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_prereg_in_prereg_ho_waiting_rsp(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_srat_rcvrd_in_prereg_ho_waiting_rsp(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_ho_rsp_success_in_prereg_ho_waiting_rsp(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_ho_rsp_failure_in_prereg_ho_waiting_rsp(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_trat_up_in_prereg_ho_waiting_rsp(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_trat_down_in_prereg_ho_waiting_rsp(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_srat_down_in_prereg_ho_waiting_rsp(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_tmr_srat_tmr_exp_in_prereg_ho_waiting_rsp(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_ho_in_srat_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_switch_ho_in_srat_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_prereg_in_srat_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_srat_rcvrd_in_srat_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_ho_rsp_success_in_srat_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_ho_rsp_failure_in_srat_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_trat_up_in_srat_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_trat_down_in_srat_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_if_srat_down_in_srat_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_ho_sub_sm_proc_tmr_srat_tmr_exp_in_srat_down(stm_state_machine_t *sm, void *payload);


/* State enumeration */
enum
{
  EPC_PDN_HO_STATE_IDLE,
  EPC_PDN_HO_STATE_HO_STARTED,
  EPC_PDN_HO_STATE_HO_TRAT_GOING_DOWN,
  EPC_PDN_HO_STATE_PREREG_STARTED,
  EPC_PDN_HO_STATE_PREREG_COMPLETED,
  EPC_PDN_HO_STATE_PREREG_HO_WAITING_RSP,
  EPC_PDN_HO_STATE_SRAT_DOWN,
};

#endif /* !STM_DATA_STRUCTURES_ONLY */

/* End machine generated internal header for state machine array: epc_pdn_ho_sub_sm[] */


#ifdef __cplusplus
} /* extern "C" {...} */
#endif

#endif /* ! __DS_EPC_PDN_HO_SUB_SM_INT_H */

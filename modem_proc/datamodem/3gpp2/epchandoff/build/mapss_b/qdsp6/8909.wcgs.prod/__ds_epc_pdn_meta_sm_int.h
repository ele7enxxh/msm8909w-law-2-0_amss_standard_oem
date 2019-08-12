/*=============================================================================

    __ds_epc_pdn_meta_sm_int.h

Description:
  This file contains the machine generated header file for the state machine
  specified in the file:
  ds_epc_pdn_meta_sm.stm

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


#ifndef __DS_EPC_PDN_META_SM_INT_H
#define __DS_EPC_PDN_META_SM_INT_H

#ifdef __cplusplus
/* If compiled into a C++ file, ensure symbols names are not mangled */
extern "C"
{
#endif

/* Include external state machine header */
#include "__ds_epc_pdn_meta_sm.h"

/* Begin machine generated internal header for state machine array: epc_pdn_meta_sm[] */

/* Suppress Lint suggestions to const-ify state machine and payload ptrs */
/*lint -esym(818,sm,payload) */

/* Define a macro for the number of SM instances */
#define epc_pdn_meta_sm_NUM_INSTANCES 16

/* Define a macro for the number of SM states */
#define epc_pdn_meta_sm_NUM_STATES 3

/* Define a macro for the number of SM inputs */
#define epc_pdn_meta_sm_NUM_INPUTS 17

#ifndef STM_DATA_STRUCTURES_ONLY
/* State Machine entry/exit function prototypes */
void epc_pdn_meta_sm_init(stm_state_machine_t *sm,void *payload);
void epc_pdn_meta_sm_cleanup(stm_state_machine_t *sm,void *payload);


/* State entry/exit function prototypes */
void epc_pdn_meta_sm_enter_down(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_pdn_meta_sm_exit_down(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_pdn_meta_sm_enter_up(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_pdn_meta_sm_exit_up(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_pdn_meta_sm_enter_going_down(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_pdn_meta_sm_exit_going_down(stm_state_machine_t *sm,stm_state_t _state,void *payload);


/* Transition function prototypes */
stm_state_t epc_pdn_meta_sm_proc_ssm_oos_in_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_lte_attaching_in_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_lte_in_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_lte_tunneling_in_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_ehrpd_in_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_hrpd_in_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_1x_in_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_gsm_in_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_wcdma_in_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_tdscdma_in_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_other_in_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_iwlan_in_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_wlan_local_in_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_if_epc_up_in_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_if_epc_down_in_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_if_epc_going_down_in_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_if_teardown_in_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_oos_in_up(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_lte_attaching_in_up(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_lte_in_up(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_lte_tunneling_in_up(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_ehrpd_in_up(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_hrpd_in_up(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_1x_in_up(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_gsm_in_up(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_wcdma_in_up(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_tdscdma_in_up(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_other_in_up(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_iwlan_in_up(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_wlan_local_in_up(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_if_epc_up_in_up(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_if_epc_down_in_up(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_if_epc_going_down_in_up(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_if_teardown_in_up(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_oos_in_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_lte_attaching_in_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_lte_in_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_lte_tunneling_in_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_ehrpd_in_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_hrpd_in_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_1x_in_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_gsm_in_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_wcdma_in_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_tdscdma_in_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_other_in_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_iwlan_in_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_ssm_wlan_local_in_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_if_epc_up_in_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_if_epc_down_in_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_if_epc_going_down_in_going_down(stm_state_machine_t *sm, void *payload);
stm_state_t epc_pdn_meta_sm_proc_if_teardown_in_going_down(stm_state_machine_t *sm, void *payload);


/* State enumeration */
enum
{
  EPC_PDN_META_STATE_DOWN,
  EPC_PDN_META_STATE_UP,
  EPC_PDN_META_STATE_GOING_DOWN,
};

#endif /* !STM_DATA_STRUCTURES_ONLY */

/* End machine generated internal header for state machine array: epc_pdn_meta_sm[] */


#ifdef __cplusplus
} /* extern "C" {...} */
#endif

#endif /* ! __DS_EPC_PDN_META_SM_INT_H */

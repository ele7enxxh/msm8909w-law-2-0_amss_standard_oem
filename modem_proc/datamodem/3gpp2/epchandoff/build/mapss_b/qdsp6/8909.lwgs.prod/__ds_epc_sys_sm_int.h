/*=============================================================================

    __ds_epc_sys_sm_int.h

Description:
  This file contains the machine generated header file for the state machine
  specified in the file:
  ds_epc_sys_sm.stm

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


#ifndef __DS_EPC_SYS_SM_INT_H
#define __DS_EPC_SYS_SM_INT_H

#ifdef __cplusplus
/* If compiled into a C++ file, ensure symbols names are not mangled */
extern "C"
{
#endif

/* Include external state machine header */
#include "__ds_epc_sys_sm.h"

/* Begin machine generated internal header for state machine array: epc_sys_sm[] */

/* Suppress Lint suggestions to const-ify state machine and payload ptrs */
/*lint -esym(818,sm,payload) */

/* Define a macro for the number of SM instances */
#define epc_sys_sm_NUM_INSTANCES 1

/* Define a macro for the number of SM states */
#define epc_sys_sm_NUM_STATES 8

/* Define a macro for the number of SM inputs */
#define epc_sys_sm_NUM_INPUTS 8

#ifndef STM_DATA_STRUCTURES_ONLY
/* State Machine entry/exit function prototypes */
void epc_sys_sm_init(stm_state_machine_t *sm,void *payload);
void epc_sys_sm_cleanup(stm_state_machine_t *sm,void *payload);


/* State entry/exit function prototypes */
void epc_sys_sm_enter_init(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_sys_sm_exit_init(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_sys_sm_enter_lte_attaching(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_sys_sm_exit_lte_attaching(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_sys_sm_enter_lte(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_sys_sm_exit_lte(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_sys_sm_enter_1x(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_sys_sm_exit_1x(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_sys_sm_enter_hrpd(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_sys_sm_exit_hrpd(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_sys_sm_enter_ehrpd(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_sys_sm_exit_ehrpd(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_sys_sm_enter_lte_tunneling(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_sys_sm_exit_lte_tunneling(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_sys_sm_enter_other(stm_state_machine_t *sm,stm_state_t _state,void *payload);
void epc_sys_sm_exit_other(stm_state_machine_t *sm,stm_state_t _state,void *payload);


/* Transition function prototypes */
stm_state_t epc_sys_sm_proc_lte_attach_init_in_init(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_in_init(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_ehrpd_in_init(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_hrpd_in_init(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_1x_in_init(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_tunneling_enable_in_init(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_tunneling_disable_in_init(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_other_in_init(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_attach_init_in_lte_attaching(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_in_lte_attaching(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_ehrpd_in_lte_attaching(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_hrpd_in_lte_attaching(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_1x_in_lte_attaching(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_tunneling_enable_in_lte_attaching(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_tunneling_disable_in_lte_attaching(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_other_in_lte_attaching(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_attach_init_in_lte(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_in_lte(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_ehrpd_in_lte(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_hrpd_in_lte(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_1x_in_lte(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_tunneling_enable_in_lte(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_tunneling_disable_in_lte(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_other_in_lte(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_attach_init_in_1x(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_in_1x(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_ehrpd_in_1x(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_hrpd_in_1x(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_1x_in_1x(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_tunneling_enable_in_1x(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_tunneling_disable_in_1x(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_other_in_1x(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_attach_init_in_hrpd(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_in_hrpd(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_ehrpd_in_hrpd(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_hrpd_in_hrpd(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_1x_in_hrpd(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_tunneling_enable_in_hrpd(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_tunneling_disable_in_hrpd(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_other_in_hrpd(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_attach_init_in_ehrpd(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_in_ehrpd(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_ehrpd_in_ehrpd(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_hrpd_in_ehrpd(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_1x_in_ehrpd(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_tunneling_enable_in_ehrpd(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_tunneling_disable_in_ehrpd(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_other_in_ehrpd(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_attach_init_in_lte_tunneling(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_in_lte_tunneling(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_ehrpd_in_lte_tunneling(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_hrpd_in_lte_tunneling(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_1x_in_lte_tunneling(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_tunneling_enable_in_lte_tunneling(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_tunneling_disable_in_lte_tunneling(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_other_in_lte_tunneling(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_attach_init_in_other(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_in_other(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_ehrpd_in_other(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_hrpd_in_other(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_1x_in_other(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_tunneling_enable_in_other(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_lte_tunneling_disable_in_other(stm_state_machine_t *sm, void *payload);
stm_state_t epc_sys_sm_proc_other_in_other(stm_state_machine_t *sm, void *payload);


/* State enumeration */
enum
{
  EPC_SSM_STATE_INIT,
  EPC_SSM_STATE_LTE_ATTACHING,
  EPC_SSM_STATE_LTE,
  EPC_SSM_STATE_1X,
  EPC_SSM_STATE_HRPD,
  EPC_SSM_STATE_EHRPD,
  EPC_SSM_STATE_LTE_TUNNELING,
  EPC_SSM_STATE_OTHER,
};

#endif /* !STM_DATA_STRUCTURES_ONLY */

/* End machine generated internal header for state machine array: epc_sys_sm[] */


#ifdef __cplusplus
} /* extern "C" {...} */
#endif

#endif /* ! __DS_EPC_SYS_SM_INT_H */

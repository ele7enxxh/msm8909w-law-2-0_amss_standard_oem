/*=============================================================================

    __ds_epc_pdn_ho_sub_sm_int.c

Description:
  This file contains the machine generated source file for the state machine
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


/* Include STM compiler generated external and internal header files */
#include "__ds_epc_pdn_ho_sub_sm.h"
#include "__ds_epc_pdn_ho_sub_sm_int.h"

/* Include INPUT_DEF_FILE specified files */
#include <ds_epc_pdn_ho_sub_sm.h>

/* Begin machine generated internal source for state machine array: epc_pdn_ho_sub_sm[] */

#ifndef STM_DATA_STRUCTURES_ONLY
/* Transition table */
static const stm_transition_fn_t
  epc_pdn_ho_sub_sm_transitions[ epc_pdn_ho_sub_sm_NUM_STATES * epc_pdn_ho_sub_sm_NUM_INPUTS ] =
{
  /* Transition functions for state EPC_PDN_HO_STATE_IDLE */
  epc_pdn_ho_sub_sm_proc_ho_in_idle,    /* EPC_PDN_HO_SM_HO_EV */
  epc_pdn_ho_sub_sm_proc_switch_ho_in_idle,    /* EPC_PDN_HO_SM_SWITCH_HO_EV */
  epc_pdn_ho_sub_sm_proc_prereg_in_idle,    /* EPC_PDN_HO_SM_PREREG_EV */
  epc_pdn_ho_sub_sm_proc_srat_rcvrd_in_idle,    /* EPC_PDN_HO_SM_SRAT_RCVRD_EV */
  epc_pdn_ho_sub_sm_proc_ho_rsp_success_in_idle,    /* EPC_PDN_HO_SM_HO_RSP_SUCCESS_EV */
  epc_pdn_ho_sub_sm_proc_ho_rsp_failure_in_idle,    /* EPC_PDN_HO_SM_HO_RSP_FAILURE_EV */
  epc_pdn_ho_sub_sm_proc_if_trat_up_in_idle,    /* EPC_PDN_HO_IF_TRAT_UP_EV */
  epc_pdn_ho_sub_sm_proc_if_trat_down_in_idle,    /* EPC_PDN_HO_IF_TRAT_DOWN_EV */
  epc_pdn_ho_sub_sm_proc_if_srat_down_in_idle,    /* EPC_PDN_HO_IF_SRAT_DOWN_EV */
  epc_pdn_ho_sub_sm_proc_tmr_srat_tmr_exp_in_idle,    /* EPC_PDN_HO_TMR_SRAT_TMR_EXP_EV */
  epc_pdn_ho_sub_sm_proc_if_comp_trat_down_in_idle,    /* EPC_PDN_HO_IF_COMP_TRAT_DOWN_EV */
  epc_pdn_ho_sub_sm_proc_if_comp_trat_up_in_idle,    /* EPC_PDN_HO_IF_COMP_TRAT_UP_EV */

  /* Transition functions for state EPC_PDN_HO_STATE_HO_STARTED */
  epc_pdn_ho_sub_sm_proc_ho_in_ho_started,    /* EPC_PDN_HO_SM_HO_EV */
  epc_pdn_ho_sub_sm_proc_switch_ho_in_ho_started,    /* EPC_PDN_HO_SM_SWITCH_HO_EV */
  epc_pdn_ho_sub_sm_proc_prereg_in_ho_started,    /* EPC_PDN_HO_SM_PREREG_EV */
  epc_pdn_ho_sub_sm_proc_srat_rcvrd_in_ho_started,    /* EPC_PDN_HO_SM_SRAT_RCVRD_EV */
  epc_pdn_ho_sub_sm_proc_ho_rsp_success_in_ho_started,    /* EPC_PDN_HO_SM_HO_RSP_SUCCESS_EV */
  epc_pdn_ho_sub_sm_proc_ho_rsp_failure_in_ho_started,    /* EPC_PDN_HO_SM_HO_RSP_FAILURE_EV */
  epc_pdn_ho_sub_sm_proc_if_trat_up_in_ho_started,    /* EPC_PDN_HO_IF_TRAT_UP_EV */
  epc_pdn_ho_sub_sm_proc_if_trat_down_in_ho_started,    /* EPC_PDN_HO_IF_TRAT_DOWN_EV */
  epc_pdn_ho_sub_sm_proc_if_srat_down_in_ho_started,    /* EPC_PDN_HO_IF_SRAT_DOWN_EV */
  epc_pdn_ho_sub_sm_proc_tmr_srat_tmr_exp_in_ho_started,    /* EPC_PDN_HO_TMR_SRAT_TMR_EXP_EV */
  epc_pdn_ho_sub_sm_proc_if_comp_trat_down_in_ho_started,    /* EPC_PDN_HO_IF_COMP_TRAT_DOWN_EV */
  epc_pdn_ho_sub_sm_proc_if_comp_trat_up_in_ho_started,    /* EPC_PDN_HO_IF_COMP_TRAT_UP_EV */

  /* Transition functions for state EPC_PDN_HO_STATE_HO_TRAT_GOING_DOWN */
  epc_pdn_ho_sub_sm_proc_ho_in_ho_trat_going_down,    /* EPC_PDN_HO_SM_HO_EV */
  epc_pdn_ho_sub_sm_proc_switch_ho_in_trat_going_down,    /* EPC_PDN_HO_SM_SWITCH_HO_EV */
  epc_pdn_ho_sub_sm_proc_prereg_in_ho_trat_going_down,    /* EPC_PDN_HO_SM_PREREG_EV */
  epc_pdn_ho_sub_sm_proc_srat_rcvrd_in_ho_trat_going_down,    /* EPC_PDN_HO_SM_SRAT_RCVRD_EV */
  epc_pdn_ho_sub_sm_proc_ho_rsp_success_in_ho_trat_going_down,    /* EPC_PDN_HO_SM_HO_RSP_SUCCESS_EV */
  epc_pdn_ho_sub_sm_proc_ho_rsp_failure_in_ho_trat_going_down,    /* EPC_PDN_HO_SM_HO_RSP_FAILURE_EV */
  epc_pdn_ho_sub_sm_proc_if_trat_up_in_ho_trat_going_down,    /* EPC_PDN_HO_IF_TRAT_UP_EV */
  epc_pdn_ho_sub_sm_proc_if_trat_down_in_ho_trat_going_down,    /* EPC_PDN_HO_IF_TRAT_DOWN_EV */
  epc_pdn_ho_sub_sm_proc_if_srat_down_in_ho_trat_going_down,    /* EPC_PDN_HO_IF_SRAT_DOWN_EV */
  epc_pdn_ho_sub_sm_proc_tmr_srat_tmr_exp_in_ho_trat_going_down,    /* EPC_PDN_HO_TMR_SRAT_TMR_EXP_EV */
  epc_pdn_ho_sub_sm_proc_if_comp_trat_down_in_ho_trat_going_down,    /* EPC_PDN_HO_IF_COMP_TRAT_DOWN_EV */
  epc_pdn_ho_sub_sm_proc_if_comp_trat_up_in_ho_trat_going_down,    /* EPC_PDN_HO_IF_COMP_TRAT_UP_EV */

  /* Transition functions for state EPC_PDN_HO_STATE_PREREG_STARTED */
  epc_pdn_ho_sub_sm_proc_ho_in_prereg_started,    /* EPC_PDN_HO_SM_HO_EV */
  epc_pdn_ho_sub_sm_proc_switch_ho_in_prereg_started,    /* EPC_PDN_HO_SM_SWITCH_HO_EV */
  epc_pdn_ho_sub_sm_proc_prereg_in_prereg_started,    /* EPC_PDN_HO_SM_PREREG_EV */
  epc_pdn_ho_sub_sm_proc_srat_rcvrd_in_prereg_started,    /* EPC_PDN_HO_SM_SRAT_RCVRD_EV */
  epc_pdn_ho_sub_sm_proc_ho_rsp_success_in_prereg_started,    /* EPC_PDN_HO_SM_HO_RSP_SUCCESS_EV */
  epc_pdn_ho_sub_sm_proc_ho_rsp_failure_in_prereg_started,    /* EPC_PDN_HO_SM_HO_RSP_FAILURE_EV */
  epc_pdn_ho_sub_sm_proc_if_trat_up_in_prereg_started,    /* EPC_PDN_HO_IF_TRAT_UP_EV */
  epc_pdn_ho_sub_sm_proc_if_trat_down_in_prereg_started,    /* EPC_PDN_HO_IF_TRAT_DOWN_EV */
  epc_pdn_ho_sub_sm_proc_if_srat_down_in_prereg_started,    /* EPC_PDN_HO_IF_SRAT_DOWN_EV */
  epc_pdn_ho_sub_sm_proc_tmr_srat_tmr_exp_in_prereg_started,    /* EPC_PDN_HO_TMR_SRAT_TMR_EXP_EV */
  NULL,    /* EPC_PDN_HO_IF_COMP_TRAT_DOWN_EV */
  NULL,    /* EPC_PDN_HO_IF_COMP_TRAT_UP_EV */

  /* Transition functions for state EPC_PDN_HO_STATE_PREREG_COMPLETED */
  epc_pdn_ho_sub_sm_proc_ho_in_prereg_completed,    /* EPC_PDN_HO_SM_HO_EV */
  epc_pdn_ho_sub_sm_proc_switch_ho_in_prereg_completed,    /* EPC_PDN_HO_SM_SWITCH_HO_EV */
  epc_pdn_ho_sub_sm_proc_prereg_in_prereg_completed,    /* EPC_PDN_HO_SM_PREREG_EV */
  epc_pdn_ho_sub_sm_proc_srat_rcvrd_in_prereg_completed,    /* EPC_PDN_HO_SM_SRAT_RCVRD_EV */
  epc_pdn_ho_sub_sm_proc_ho_rsp_success_in_prereg_completed,    /* EPC_PDN_HO_SM_HO_RSP_SUCCESS_EV */
  epc_pdn_ho_sub_sm_proc_ho_rsp_failure_in_prereg_completed,    /* EPC_PDN_HO_SM_HO_RSP_FAILURE_EV */
  epc_pdn_ho_sub_sm_proc_if_trat_up_in_prereg_completed,    /* EPC_PDN_HO_IF_TRAT_UP_EV */
  epc_pdn_ho_sub_sm_proc_if_trat_down_in_prereg_completed,    /* EPC_PDN_HO_IF_TRAT_DOWN_EV */
  epc_pdn_ho_sub_sm_proc_if_srat_down_in_prereg_completed,    /* EPC_PDN_HO_IF_SRAT_DOWN_EV */
  epc_pdn_ho_sub_sm_proc_tmr_srat_tmr_exp_in_prereg_completed,    /* EPC_PDN_HO_TMR_SRAT_TMR_EXP_EV */
  NULL,    /* EPC_PDN_HO_IF_COMP_TRAT_DOWN_EV */
  NULL,    /* EPC_PDN_HO_IF_COMP_TRAT_UP_EV */

  /* Transition functions for state EPC_PDN_HO_STATE_PREREG_HO_WAITING_RSP */
  epc_pdn_ho_sub_sm_proc_ho_in_prereg_ho_waiting_rsp,    /* EPC_PDN_HO_SM_HO_EV */
  epc_pdn_ho_sub_sm_proc_switch_ho_in_ho_waiting_rsp,    /* EPC_PDN_HO_SM_SWITCH_HO_EV */
  epc_pdn_ho_sub_sm_proc_prereg_in_prereg_ho_waiting_rsp,    /* EPC_PDN_HO_SM_PREREG_EV */
  epc_pdn_ho_sub_sm_proc_srat_rcvrd_in_prereg_ho_waiting_rsp,    /* EPC_PDN_HO_SM_SRAT_RCVRD_EV */
  epc_pdn_ho_sub_sm_proc_ho_rsp_success_in_prereg_ho_waiting_rsp,    /* EPC_PDN_HO_SM_HO_RSP_SUCCESS_EV */
  epc_pdn_ho_sub_sm_proc_ho_rsp_failure_in_prereg_ho_waiting_rsp,    /* EPC_PDN_HO_SM_HO_RSP_FAILURE_EV */
  epc_pdn_ho_sub_sm_proc_if_trat_up_in_prereg_ho_waiting_rsp,    /* EPC_PDN_HO_IF_TRAT_UP_EV */
  epc_pdn_ho_sub_sm_proc_if_trat_down_in_prereg_ho_waiting_rsp,    /* EPC_PDN_HO_IF_TRAT_DOWN_EV */
  epc_pdn_ho_sub_sm_proc_if_srat_down_in_prereg_ho_waiting_rsp,    /* EPC_PDN_HO_IF_SRAT_DOWN_EV */
  epc_pdn_ho_sub_sm_proc_tmr_srat_tmr_exp_in_prereg_ho_waiting_rsp,    /* EPC_PDN_HO_TMR_SRAT_TMR_EXP_EV */
  NULL,    /* EPC_PDN_HO_IF_COMP_TRAT_DOWN_EV */
  NULL,    /* EPC_PDN_HO_IF_COMP_TRAT_UP_EV */

  /* Transition functions for state EPC_PDN_HO_STATE_SRAT_DOWN */
  epc_pdn_ho_sub_sm_proc_ho_in_srat_down,    /* EPC_PDN_HO_SM_HO_EV */
  epc_pdn_ho_sub_sm_proc_switch_ho_in_srat_down,    /* EPC_PDN_HO_SM_SWITCH_HO_EV */
  epc_pdn_ho_sub_sm_proc_prereg_in_srat_down,    /* EPC_PDN_HO_SM_PREREG_EV */
  epc_pdn_ho_sub_sm_proc_srat_rcvrd_in_srat_down,    /* EPC_PDN_HO_SM_SRAT_RCVRD_EV */
  epc_pdn_ho_sub_sm_proc_ho_rsp_success_in_srat_down,    /* EPC_PDN_HO_SM_HO_RSP_SUCCESS_EV */
  epc_pdn_ho_sub_sm_proc_ho_rsp_failure_in_srat_down,    /* EPC_PDN_HO_SM_HO_RSP_FAILURE_EV */
  epc_pdn_ho_sub_sm_proc_if_trat_up_in_srat_down,    /* EPC_PDN_HO_IF_TRAT_UP_EV */
  epc_pdn_ho_sub_sm_proc_if_trat_down_in_srat_down,    /* EPC_PDN_HO_IF_TRAT_DOWN_EV */
  epc_pdn_ho_sub_sm_proc_if_srat_down_in_srat_down,    /* EPC_PDN_HO_IF_SRAT_DOWN_EV */
  epc_pdn_ho_sub_sm_proc_tmr_srat_tmr_exp_in_srat_down,    /* EPC_PDN_HO_TMR_SRAT_TMR_EXP_EV */
  NULL,    /* EPC_PDN_HO_IF_COMP_TRAT_DOWN_EV */
  NULL,    /* EPC_PDN_HO_IF_COMP_TRAT_UP_EV */

};
#endif /* !STM_DATA_STRUCTURES_ONLY */

/* State { name, entry, exit, child SM } table */
static const stm_state_map_t
  epc_pdn_ho_sub_sm_states[ epc_pdn_ho_sub_sm_NUM_STATES ] =
{
  {"EPC_PDN_HO_STATE_IDLE",
#ifndef STM_DATA_STRUCTURES_ONLY
    epc_pdn_ho_sub_sm_enter_idle, epc_pdn_ho_sub_sm_exit_idle,
#else /* STM_DATA_STRUCTURES_ONLY */
    NULL, NULL,
#endif /* STM_DATA_STRUCTURES_ONLY */
    NULL},
  {"EPC_PDN_HO_STATE_HO_STARTED",
#ifndef STM_DATA_STRUCTURES_ONLY
    epc_pdn_ho_sub_sm_enter_ho_started, epc_pdn_ho_sub_sm_exit_ho_started,
#else /* STM_DATA_STRUCTURES_ONLY */
    NULL, NULL,
#endif /* STM_DATA_STRUCTURES_ONLY */
    NULL},
  {"EPC_PDN_HO_STATE_HO_TRAT_GOING_DOWN",
#ifndef STM_DATA_STRUCTURES_ONLY
    epc_pdn_ho_sub_sm_enter_ho_trat_going_down, epc_pdn_ho_sub_sm_exit_ho_trat_going_down,
#else /* STM_DATA_STRUCTURES_ONLY */
    NULL, NULL,
#endif /* STM_DATA_STRUCTURES_ONLY */
    NULL},
  {"EPC_PDN_HO_STATE_PREREG_STARTED",
#ifndef STM_DATA_STRUCTURES_ONLY
    epc_pdn_ho_sub_sm_enter_prereg_started, epc_pdn_ho_sub_sm_exit_prereg_started,
#else /* STM_DATA_STRUCTURES_ONLY */
    NULL, NULL,
#endif /* STM_DATA_STRUCTURES_ONLY */
    NULL},
  {"EPC_PDN_HO_STATE_PREREG_COMPLETED",
#ifndef STM_DATA_STRUCTURES_ONLY
    epc_pdn_ho_sub_sm_enter_prereg_completed, epc_pdn_ho_sub_sm_exit_prereg_completed,
#else /* STM_DATA_STRUCTURES_ONLY */
    NULL, NULL,
#endif /* STM_DATA_STRUCTURES_ONLY */
    NULL},
  {"EPC_PDN_HO_STATE_PREREG_HO_WAITING_RSP",
#ifndef STM_DATA_STRUCTURES_ONLY
    epc_pdn_ho_sub_sm_enter_prereg_ho_waiting_rsp, epc_pdn_ho_sub_sm_exit_prereg_ho_waiting_rsp,
#else /* STM_DATA_STRUCTURES_ONLY */
    NULL, NULL,
#endif /* STM_DATA_STRUCTURES_ONLY */
    NULL},
  {"EPC_PDN_HO_STATE_SRAT_DOWN",
#ifndef STM_DATA_STRUCTURES_ONLY
    epc_pdn_ho_sub_sm_enter_srat_down, epc_pdn_ho_sub_sm_exit_srat_down,
#else /* STM_DATA_STRUCTURES_ONLY */
    NULL, NULL,
#endif /* STM_DATA_STRUCTURES_ONLY */
    NULL},
};

/* Input { name, value } table */
static const stm_input_map_t
  epc_pdn_ho_sub_sm_inputs[ epc_pdn_ho_sub_sm_NUM_INPUTS ] =
{
  { "EPC_PDN_HO_SM_HO_EV" , (stm_input_t) EPC_PDN_HO_SM_HO_EV },
  { "EPC_PDN_HO_SM_SWITCH_HO_EV" , (stm_input_t) EPC_PDN_HO_SM_SWITCH_HO_EV },
  { "EPC_PDN_HO_SM_PREREG_EV" , (stm_input_t) EPC_PDN_HO_SM_PREREG_EV },
  { "EPC_PDN_HO_SM_SRAT_RCVRD_EV" , (stm_input_t) EPC_PDN_HO_SM_SRAT_RCVRD_EV },
  { "EPC_PDN_HO_SM_HO_RSP_SUCCESS_EV" , (stm_input_t) EPC_PDN_HO_SM_HO_RSP_SUCCESS_EV },
  { "EPC_PDN_HO_SM_HO_RSP_FAILURE_EV" , (stm_input_t) EPC_PDN_HO_SM_HO_RSP_FAILURE_EV },
  { "EPC_PDN_HO_IF_TRAT_UP_EV" , (stm_input_t) EPC_PDN_HO_IF_TRAT_UP_EV },
  { "EPC_PDN_HO_IF_TRAT_DOWN_EV" , (stm_input_t) EPC_PDN_HO_IF_TRAT_DOWN_EV },
  { "EPC_PDN_HO_IF_SRAT_DOWN_EV" , (stm_input_t) EPC_PDN_HO_IF_SRAT_DOWN_EV },
  { "EPC_PDN_HO_TMR_SRAT_TMR_EXP_EV" , (stm_input_t) EPC_PDN_HO_TMR_SRAT_TMR_EXP_EV },
  { "EPC_PDN_HO_IF_COMP_TRAT_DOWN_EV" , (stm_input_t) EPC_PDN_HO_IF_COMP_TRAT_DOWN_EV },
  { "EPC_PDN_HO_IF_COMP_TRAT_UP_EV" , (stm_input_t) EPC_PDN_HO_IF_COMP_TRAT_UP_EV },
};


/* Constant all-instance state machine data */
static const stm_state_machine_constdata_t epc_pdn_ho_sub_sm_constdata =
{
  epc_pdn_ho_sub_sm_NUM_INSTANCES, /* number of state machine instances */
  epc_pdn_ho_sub_sm_NUM_STATES, /* number of states */
  epc_pdn_ho_sub_sm_states, /* array of state mappings */
  epc_pdn_ho_sub_sm_NUM_INPUTS, /* number of inputs */
  epc_pdn_ho_sub_sm_inputs, /* array of input mappings */
#ifndef STM_DATA_STRUCTURES_ONLY
  epc_pdn_ho_sub_sm_transitions, /* array of transition function mappings */
  epc_pdn_ho_sub_sm_init, /* state machine entry function */
  epc_pdn_ho_sub_sm_cleanup, /* state machine exit function */
  NULL, /* state machine error hook function */
  NULL, /* state machine debug hook function */
  EPC_PDN_HO_STATE_IDLE /* state machine initial state */
#else /* STM_DATA_STRUCTURES_ONLY */
  NULL, /* array of transition function mappings */
  NULL, /* state machine entry function */
  NULL, /* state machine exit function */
  NULL, /* state machine error hook function */
  NULL, /* state machine debug hook function */
  0 /* state machine initial state */
#endif /* STM_DATA_STRUCTURES_ONLY */
};

/* Constant per-instance state machine data */
static const stm_state_machine_perinst_constdata_t
  epc_pdn_ho_sub_sm_perinst_constdata[ epc_pdn_ho_sub_sm_NUM_INSTANCES ] =
{
  {
    &epc_pdn_ho_sub_sm_constdata, /* state machine constant data */
    "epc_pdn_ho_sub_sm[0]", /* state machine name */
    0x18b4adb4, /* state machine unique ID (md5("epc_pdn_ho_sub_sm[0]") & 0xFFFFFFFF) */
    0  /* this state machine instance */
  },
  {
    &epc_pdn_ho_sub_sm_constdata, /* state machine constant data */
    "epc_pdn_ho_sub_sm[1]", /* state machine name */
    0x716cd729, /* state machine unique ID (md5("epc_pdn_ho_sub_sm[1]") & 0xFFFFFFFF) */
    1  /* this state machine instance */
  },
  {
    &epc_pdn_ho_sub_sm_constdata, /* state machine constant data */
    "epc_pdn_ho_sub_sm[2]", /* state machine name */
    0x7625b534, /* state machine unique ID (md5("epc_pdn_ho_sub_sm[2]") & 0xFFFFFFFF) */
    2  /* this state machine instance */
  },
  {
    &epc_pdn_ho_sub_sm_constdata, /* state machine constant data */
    "epc_pdn_ho_sub_sm[3]", /* state machine name */
    0x67899384, /* state machine unique ID (md5("epc_pdn_ho_sub_sm[3]") & 0xFFFFFFFF) */
    3  /* this state machine instance */
  },
  {
    &epc_pdn_ho_sub_sm_constdata, /* state machine constant data */
    "epc_pdn_ho_sub_sm[4]", /* state machine name */
    0x8e0be007, /* state machine unique ID (md5("epc_pdn_ho_sub_sm[4]") & 0xFFFFFFFF) */
    4  /* this state machine instance */
  },
  {
    &epc_pdn_ho_sub_sm_constdata, /* state machine constant data */
    "epc_pdn_ho_sub_sm[5]", /* state machine name */
    0x19b6027a, /* state machine unique ID (md5("epc_pdn_ho_sub_sm[5]") & 0xFFFFFFFF) */
    5  /* this state machine instance */
  },
  {
    &epc_pdn_ho_sub_sm_constdata, /* state machine constant data */
    "epc_pdn_ho_sub_sm[6]", /* state machine name */
    0xc242134c, /* state machine unique ID (md5("epc_pdn_ho_sub_sm[6]") & 0xFFFFFFFF) */
    6  /* this state machine instance */
  },
  {
    &epc_pdn_ho_sub_sm_constdata, /* state machine constant data */
    "epc_pdn_ho_sub_sm[7]", /* state machine name */
    0x65066ef6, /* state machine unique ID (md5("epc_pdn_ho_sub_sm[7]") & 0xFFFFFFFF) */
    7  /* this state machine instance */
  },
  {
    &epc_pdn_ho_sub_sm_constdata, /* state machine constant data */
    "epc_pdn_ho_sub_sm[8]", /* state machine name */
    0x0e0e9e81, /* state machine unique ID (md5("epc_pdn_ho_sub_sm[8]") & 0xFFFFFFFF) */
    8  /* this state machine instance */
  },
  {
    &epc_pdn_ho_sub_sm_constdata, /* state machine constant data */
    "epc_pdn_ho_sub_sm[9]", /* state machine name */
    0x4c8bd142, /* state machine unique ID (md5("epc_pdn_ho_sub_sm[9]") & 0xFFFFFFFF) */
    9  /* this state machine instance */
  },
  {
    &epc_pdn_ho_sub_sm_constdata, /* state machine constant data */
    "epc_pdn_ho_sub_sm[10]", /* state machine name */
    0x323aa0cd, /* state machine unique ID (md5("epc_pdn_ho_sub_sm[10]") & 0xFFFFFFFF) */
    10  /* this state machine instance */
  },
  {
    &epc_pdn_ho_sub_sm_constdata, /* state machine constant data */
    "epc_pdn_ho_sub_sm[11]", /* state machine name */
    0x383e9e87, /* state machine unique ID (md5("epc_pdn_ho_sub_sm[11]") & 0xFFFFFFFF) */
    11  /* this state machine instance */
  },
  {
    &epc_pdn_ho_sub_sm_constdata, /* state machine constant data */
    "epc_pdn_ho_sub_sm[12]", /* state machine name */
    0xf9e2c522, /* state machine unique ID (md5("epc_pdn_ho_sub_sm[12]") & 0xFFFFFFFF) */
    12  /* this state machine instance */
  },
  {
    &epc_pdn_ho_sub_sm_constdata, /* state machine constant data */
    "epc_pdn_ho_sub_sm[13]", /* state machine name */
    0x67a91b8e, /* state machine unique ID (md5("epc_pdn_ho_sub_sm[13]") & 0xFFFFFFFF) */
    13  /* this state machine instance */
  },
  {
    &epc_pdn_ho_sub_sm_constdata, /* state machine constant data */
    "epc_pdn_ho_sub_sm[14]", /* state machine name */
    0xf7ddbae6, /* state machine unique ID (md5("epc_pdn_ho_sub_sm[14]") & 0xFFFFFFFF) */
    14  /* this state machine instance */
  },
  {
    &epc_pdn_ho_sub_sm_constdata, /* state machine constant data */
    "epc_pdn_ho_sub_sm[15]", /* state machine name */
    0xfe2877c3, /* state machine unique ID (md5("epc_pdn_ho_sub_sm[15]") & 0xFFFFFFFF) */
    15  /* this state machine instance */
  },
};

/* State machine instance array definition */
stm_state_machine_t
  epc_pdn_ho_sub_sm[ epc_pdn_ho_sub_sm_NUM_INSTANCES ] =
{
  {
    &epc_pdn_ho_sub_sm_perinst_constdata[ 0 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_ho_sub_sm_perinst_constdata[ 1 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_ho_sub_sm_perinst_constdata[ 2 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_ho_sub_sm_perinst_constdata[ 3 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_ho_sub_sm_perinst_constdata[ 4 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_ho_sub_sm_perinst_constdata[ 5 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_ho_sub_sm_perinst_constdata[ 6 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_ho_sub_sm_perinst_constdata[ 7 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_ho_sub_sm_perinst_constdata[ 8 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_ho_sub_sm_perinst_constdata[ 9 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_ho_sub_sm_perinst_constdata[ 10 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_ho_sub_sm_perinst_constdata[ 11 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_ho_sub_sm_perinst_constdata[ 12 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_ho_sub_sm_perinst_constdata[ 13 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_ho_sub_sm_perinst_constdata[ 14 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_ho_sub_sm_perinst_constdata[ 15 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
};

#ifndef STM_DATA_STRUCTURES_ONLY
/* User called 'reset' routine.  Should never be needed, but can be used to
   effect a complete reset of all a given state machine's instances. */
void epc_pdn_ho_sub_sm_reset(void)
{
  uint32 idx;
  void **tricky;

  /* Reset all the child SMs (if any) */
  

  /* Reset the parent */
  for( idx = 0; idx < epc_pdn_ho_sub_sm_NUM_INSTANCES; idx++)
  {
    tricky = (void **)&epc_pdn_ho_sub_sm[ idx ].pi_const_data; /* sleight of hand to assign to const ptr below */
    *tricky = (void *)&epc_pdn_ho_sub_sm_perinst_constdata[ idx ]; /* per instance constant data array */
    epc_pdn_ho_sub_sm[ idx ].current_state = STM_DEACTIVATED_STATE; /* current state */
    epc_pdn_ho_sub_sm[ idx ].curr_input_index = -1; /* current input index */
    epc_pdn_ho_sub_sm[ idx ].propagate_input = FALSE; /* propagate input to parent */
    epc_pdn_ho_sub_sm[ idx ].is_locked = FALSE; /* locked flag */
    epc_pdn_ho_sub_sm[ idx ].user_data = NULL; /* user defined per-instance data */
    epc_pdn_ho_sub_sm[ idx ].debug_mask = 0; /* user defined debug mask */
  }

}
#endif /* !STM_DATA_STRUCTURES_ONLY */

/* End machine generated internal source for state machine array: epc_pdn_ho_sub_sm[] */



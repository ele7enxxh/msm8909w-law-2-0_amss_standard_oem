/*=============================================================================

    __ds_epc_pdn_meta_sm_int.c

Description:
  This file contains the machine generated source file for the state machine
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


/* Include STM compiler generated external and internal header files */
#include "__ds_epc_pdn_meta_sm.h"
#include "__ds_epc_pdn_meta_sm_int.h"

/* Include INPUT_DEF_FILE specified files */
#include <ds_epc_pdn_meta_sm.h>

/* Begin machine generated internal source for state machine array: epc_pdn_meta_sm[] */

#ifndef STM_DATA_STRUCTURES_ONLY
/* Transition table */
static const stm_transition_fn_t
  epc_pdn_meta_sm_transitions[ epc_pdn_meta_sm_NUM_STATES * epc_pdn_meta_sm_NUM_INPUTS ] =
{
  /* Transition functions for state EPC_PDN_META_STATE_DOWN */
  epc_pdn_meta_sm_proc_ssm_oos_in_down,    /* EPC_PDN_META_SSM_OOS_EV */
  epc_pdn_meta_sm_proc_ssm_lte_attaching_in_down,    /* EPC_PDN_META_SSM_LTE_ATTACHING_EV */
  epc_pdn_meta_sm_proc_ssm_lte_in_down,    /* EPC_PDN_META_SSM_LTE_EV */
  epc_pdn_meta_sm_proc_ssm_lte_tunneling_in_down,    /* EPC_PDN_META_SSM_LTE_TUNNELING_EV */
  epc_pdn_meta_sm_proc_ssm_ehrpd_in_down,    /* EPC_PDN_META_SSM_EHRPD_EV */
  epc_pdn_meta_sm_proc_ssm_hrpd_in_down,    /* EPC_PDN_META_SSM_HRPD_EV */
  epc_pdn_meta_sm_proc_ssm_1x_in_down,    /* EPC_PDN_META_SSM_1X_EV */
  epc_pdn_meta_sm_proc_ssm_gsm_in_down,    /* EPC_PDN_META_SSM_GSM_EV */
  epc_pdn_meta_sm_proc_ssm_wcdma_in_down,    /* EPC_PDN_META_SSM_WCDMA_EV */
  epc_pdn_meta_sm_proc_ssm_tdscdma_in_down,    /* EPC_PDN_META_SSM_TDSCDMA_EV */
  epc_pdn_meta_sm_proc_ssm_other_in_down,    /* EPC_PDN_META_SSM_OTHER_EV */
  epc_pdn_meta_sm_proc_ssm_iwlan_in_down,    /* EPC_PDN_META_SSM_IWLAN_EV */
  epc_pdn_meta_sm_proc_ssm_wlan_local_in_down,    /* EPC_PDN_META_SSM_WLAN_LOCAL_EV */
  epc_pdn_meta_sm_proc_if_epc_up_in_down,    /* EPC_PDN_META_IF_EPC_UP_EV */
  epc_pdn_meta_sm_proc_if_epc_down_in_down,    /* EPC_PDN_META_IF_EPC_DOWN_EV */
  epc_pdn_meta_sm_proc_if_epc_going_down_in_down,    /* EPC_PDN_META_IF_EPC_GOING_DOWN_EV */
  epc_pdn_meta_sm_proc_if_teardown_in_down,    /* EPC_PDN_META_IF_TEARDOWN_EV */

  /* Transition functions for state EPC_PDN_META_STATE_UP */
  epc_pdn_meta_sm_proc_ssm_oos_in_up,    /* EPC_PDN_META_SSM_OOS_EV */
  epc_pdn_meta_sm_proc_ssm_lte_attaching_in_up,    /* EPC_PDN_META_SSM_LTE_ATTACHING_EV */
  epc_pdn_meta_sm_proc_ssm_lte_in_up,    /* EPC_PDN_META_SSM_LTE_EV */
  epc_pdn_meta_sm_proc_ssm_lte_tunneling_in_up,    /* EPC_PDN_META_SSM_LTE_TUNNELING_EV */
  epc_pdn_meta_sm_proc_ssm_ehrpd_in_up,    /* EPC_PDN_META_SSM_EHRPD_EV */
  epc_pdn_meta_sm_proc_ssm_hrpd_in_up,    /* EPC_PDN_META_SSM_HRPD_EV */
  epc_pdn_meta_sm_proc_ssm_1x_in_up,    /* EPC_PDN_META_SSM_1X_EV */
  epc_pdn_meta_sm_proc_ssm_gsm_in_up,    /* EPC_PDN_META_SSM_GSM_EV */
  epc_pdn_meta_sm_proc_ssm_wcdma_in_up,    /* EPC_PDN_META_SSM_WCDMA_EV */
  epc_pdn_meta_sm_proc_ssm_tdscdma_in_up,    /* EPC_PDN_META_SSM_TDSCDMA_EV */
  epc_pdn_meta_sm_proc_ssm_other_in_up,    /* EPC_PDN_META_SSM_OTHER_EV */
  epc_pdn_meta_sm_proc_ssm_iwlan_in_up,    /* EPC_PDN_META_SSM_IWLAN_EV */
  epc_pdn_meta_sm_proc_ssm_wlan_local_in_up,    /* EPC_PDN_META_SSM_WLAN_LOCAL_EV */
  epc_pdn_meta_sm_proc_if_epc_up_in_up,    /* EPC_PDN_META_IF_EPC_UP_EV */
  epc_pdn_meta_sm_proc_if_epc_down_in_up,    /* EPC_PDN_META_IF_EPC_DOWN_EV */
  epc_pdn_meta_sm_proc_if_epc_going_down_in_up,    /* EPC_PDN_META_IF_EPC_GOING_DOWN_EV */
  epc_pdn_meta_sm_proc_if_teardown_in_up,    /* EPC_PDN_META_IF_TEARDOWN_EV */

  /* Transition functions for state EPC_PDN_META_STATE_GOING_DOWN */
  epc_pdn_meta_sm_proc_ssm_oos_in_going_down,    /* EPC_PDN_META_SSM_OOS_EV */
  epc_pdn_meta_sm_proc_ssm_lte_attaching_in_going_down,    /* EPC_PDN_META_SSM_LTE_ATTACHING_EV */
  epc_pdn_meta_sm_proc_ssm_lte_in_going_down,    /* EPC_PDN_META_SSM_LTE_EV */
  epc_pdn_meta_sm_proc_ssm_lte_tunneling_in_going_down,    /* EPC_PDN_META_SSM_LTE_TUNNELING_EV */
  epc_pdn_meta_sm_proc_ssm_ehrpd_in_going_down,    /* EPC_PDN_META_SSM_EHRPD_EV */
  epc_pdn_meta_sm_proc_ssm_hrpd_in_going_down,    /* EPC_PDN_META_SSM_HRPD_EV */
  epc_pdn_meta_sm_proc_ssm_1x_in_going_down,    /* EPC_PDN_META_SSM_1X_EV */
  epc_pdn_meta_sm_proc_ssm_gsm_in_going_down,    /* EPC_PDN_META_SSM_GSM_EV */
  epc_pdn_meta_sm_proc_ssm_wcdma_in_going_down,    /* EPC_PDN_META_SSM_WCDMA_EV */
  epc_pdn_meta_sm_proc_ssm_tdscdma_in_going_down,    /* EPC_PDN_META_SSM_TDSCDMA_EV */
  epc_pdn_meta_sm_proc_ssm_other_in_going_down,    /* EPC_PDN_META_SSM_OTHER_EV */
  epc_pdn_meta_sm_proc_ssm_iwlan_in_going_down,    /* EPC_PDN_META_SSM_IWLAN_EV */
  epc_pdn_meta_sm_proc_ssm_wlan_local_in_going_down,    /* EPC_PDN_META_SSM_WLAN_LOCAL_EV */
  epc_pdn_meta_sm_proc_if_epc_up_in_going_down,    /* EPC_PDN_META_IF_EPC_UP_EV */
  epc_pdn_meta_sm_proc_if_epc_down_in_going_down,    /* EPC_PDN_META_IF_EPC_DOWN_EV */
  epc_pdn_meta_sm_proc_if_epc_going_down_in_going_down,    /* EPC_PDN_META_IF_EPC_GOING_DOWN_EV */
  epc_pdn_meta_sm_proc_if_teardown_in_going_down,    /* EPC_PDN_META_IF_TEARDOWN_EV */

};
#endif /* !STM_DATA_STRUCTURES_ONLY */

/* State { name, entry, exit, child SM } table */
static const stm_state_map_t
  epc_pdn_meta_sm_states[ epc_pdn_meta_sm_NUM_STATES ] =
{
  {"EPC_PDN_META_STATE_DOWN",
#ifndef STM_DATA_STRUCTURES_ONLY
    epc_pdn_meta_sm_enter_down, epc_pdn_meta_sm_exit_down,
#else /* STM_DATA_STRUCTURES_ONLY */
    NULL, NULL,
#endif /* STM_DATA_STRUCTURES_ONLY */
    NULL},
  {"EPC_PDN_META_STATE_UP",
#ifndef STM_DATA_STRUCTURES_ONLY
    epc_pdn_meta_sm_enter_up, epc_pdn_meta_sm_exit_up,
#else /* STM_DATA_STRUCTURES_ONLY */
    NULL, NULL,
#endif /* STM_DATA_STRUCTURES_ONLY */
    NULL},
  {"EPC_PDN_META_STATE_GOING_DOWN",
#ifndef STM_DATA_STRUCTURES_ONLY
    epc_pdn_meta_sm_enter_going_down, epc_pdn_meta_sm_exit_going_down,
#else /* STM_DATA_STRUCTURES_ONLY */
    NULL, NULL,
#endif /* STM_DATA_STRUCTURES_ONLY */
    NULL},
};

/* Input { name, value } table */
static const stm_input_map_t
  epc_pdn_meta_sm_inputs[ epc_pdn_meta_sm_NUM_INPUTS ] =
{
  { "EPC_PDN_META_SSM_OOS_EV" , (stm_input_t) EPC_PDN_META_SSM_OOS_EV },
  { "EPC_PDN_META_SSM_LTE_ATTACHING_EV" , (stm_input_t) EPC_PDN_META_SSM_LTE_ATTACHING_EV },
  { "EPC_PDN_META_SSM_LTE_EV" , (stm_input_t) EPC_PDN_META_SSM_LTE_EV },
  { "EPC_PDN_META_SSM_LTE_TUNNELING_EV" , (stm_input_t) EPC_PDN_META_SSM_LTE_TUNNELING_EV },
  { "EPC_PDN_META_SSM_EHRPD_EV" , (stm_input_t) EPC_PDN_META_SSM_EHRPD_EV },
  { "EPC_PDN_META_SSM_HRPD_EV" , (stm_input_t) EPC_PDN_META_SSM_HRPD_EV },
  { "EPC_PDN_META_SSM_1X_EV" , (stm_input_t) EPC_PDN_META_SSM_1X_EV },
  { "EPC_PDN_META_SSM_GSM_EV" , (stm_input_t) EPC_PDN_META_SSM_GSM_EV },
  { "EPC_PDN_META_SSM_WCDMA_EV" , (stm_input_t) EPC_PDN_META_SSM_WCDMA_EV },
  { "EPC_PDN_META_SSM_TDSCDMA_EV" , (stm_input_t) EPC_PDN_META_SSM_TDSCDMA_EV },
  { "EPC_PDN_META_SSM_OTHER_EV" , (stm_input_t) EPC_PDN_META_SSM_OTHER_EV },
  { "EPC_PDN_META_SSM_IWLAN_EV" , (stm_input_t) EPC_PDN_META_SSM_IWLAN_EV },
  { "EPC_PDN_META_SSM_WLAN_LOCAL_EV" , (stm_input_t) EPC_PDN_META_SSM_WLAN_LOCAL_EV },
  { "EPC_PDN_META_IF_EPC_UP_EV" , (stm_input_t) EPC_PDN_META_IF_EPC_UP_EV },
  { "EPC_PDN_META_IF_EPC_DOWN_EV" , (stm_input_t) EPC_PDN_META_IF_EPC_DOWN_EV },
  { "EPC_PDN_META_IF_EPC_GOING_DOWN_EV" , (stm_input_t) EPC_PDN_META_IF_EPC_GOING_DOWN_EV },
  { "EPC_PDN_META_IF_TEARDOWN_EV" , (stm_input_t) EPC_PDN_META_IF_TEARDOWN_EV },
};


/* Constant all-instance state machine data */
static const stm_state_machine_constdata_t epc_pdn_meta_sm_constdata =
{
  epc_pdn_meta_sm_NUM_INSTANCES, /* number of state machine instances */
  epc_pdn_meta_sm_NUM_STATES, /* number of states */
  epc_pdn_meta_sm_states, /* array of state mappings */
  epc_pdn_meta_sm_NUM_INPUTS, /* number of inputs */
  epc_pdn_meta_sm_inputs, /* array of input mappings */
#ifndef STM_DATA_STRUCTURES_ONLY
  epc_pdn_meta_sm_transitions, /* array of transition function mappings */
  epc_pdn_meta_sm_init, /* state machine entry function */
  epc_pdn_meta_sm_cleanup, /* state machine exit function */
  NULL, /* state machine error hook function */
  NULL, /* state machine debug hook function */
  EPC_PDN_META_STATE_DOWN /* state machine initial state */
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
  epc_pdn_meta_sm_perinst_constdata[ epc_pdn_meta_sm_NUM_INSTANCES ] =
{
  {
    &epc_pdn_meta_sm_constdata, /* state machine constant data */
    "epc_pdn_meta_sm[0]", /* state machine name */
    0xed1cf702, /* state machine unique ID (md5("epc_pdn_meta_sm[0]") & 0xFFFFFFFF) */
    0  /* this state machine instance */
  },
  {
    &epc_pdn_meta_sm_constdata, /* state machine constant data */
    "epc_pdn_meta_sm[1]", /* state machine name */
    0x7cdc4b79, /* state machine unique ID (md5("epc_pdn_meta_sm[1]") & 0xFFFFFFFF) */
    1  /* this state machine instance */
  },
  {
    &epc_pdn_meta_sm_constdata, /* state machine constant data */
    "epc_pdn_meta_sm[2]", /* state machine name */
    0x6e1e974b, /* state machine unique ID (md5("epc_pdn_meta_sm[2]") & 0xFFFFFFFF) */
    2  /* this state machine instance */
  },
  {
    &epc_pdn_meta_sm_constdata, /* state machine constant data */
    "epc_pdn_meta_sm[3]", /* state machine name */
    0x304d6a8a, /* state machine unique ID (md5("epc_pdn_meta_sm[3]") & 0xFFFFFFFF) */
    3  /* this state machine instance */
  },
  {
    &epc_pdn_meta_sm_constdata, /* state machine constant data */
    "epc_pdn_meta_sm[4]", /* state machine name */
    0x93bedf98, /* state machine unique ID (md5("epc_pdn_meta_sm[4]") & 0xFFFFFFFF) */
    4  /* this state machine instance */
  },
  {
    &epc_pdn_meta_sm_constdata, /* state machine constant data */
    "epc_pdn_meta_sm[5]", /* state machine name */
    0xbafb625e, /* state machine unique ID (md5("epc_pdn_meta_sm[5]") & 0xFFFFFFFF) */
    5  /* this state machine instance */
  },
  {
    &epc_pdn_meta_sm_constdata, /* state machine constant data */
    "epc_pdn_meta_sm[6]", /* state machine name */
    0x5e3b4b3e, /* state machine unique ID (md5("epc_pdn_meta_sm[6]") & 0xFFFFFFFF) */
    6  /* this state machine instance */
  },
  {
    &epc_pdn_meta_sm_constdata, /* state machine constant data */
    "epc_pdn_meta_sm[7]", /* state machine name */
    0x503abefb, /* state machine unique ID (md5("epc_pdn_meta_sm[7]") & 0xFFFFFFFF) */
    7  /* this state machine instance */
  },
  {
    &epc_pdn_meta_sm_constdata, /* state machine constant data */
    "epc_pdn_meta_sm[8]", /* state machine name */
    0x45a019be, /* state machine unique ID (md5("epc_pdn_meta_sm[8]") & 0xFFFFFFFF) */
    8  /* this state machine instance */
  },
  {
    &epc_pdn_meta_sm_constdata, /* state machine constant data */
    "epc_pdn_meta_sm[9]", /* state machine name */
    0x44f6d24f, /* state machine unique ID (md5("epc_pdn_meta_sm[9]") & 0xFFFFFFFF) */
    9  /* this state machine instance */
  },
  {
    &epc_pdn_meta_sm_constdata, /* state machine constant data */
    "epc_pdn_meta_sm[10]", /* state machine name */
    0x172e2529, /* state machine unique ID (md5("epc_pdn_meta_sm[10]") & 0xFFFFFFFF) */
    10  /* this state machine instance */
  },
  {
    &epc_pdn_meta_sm_constdata, /* state machine constant data */
    "epc_pdn_meta_sm[11]", /* state machine name */
    0xf60ff6f2, /* state machine unique ID (md5("epc_pdn_meta_sm[11]") & 0xFFFFFFFF) */
    11  /* this state machine instance */
  },
  {
    &epc_pdn_meta_sm_constdata, /* state machine constant data */
    "epc_pdn_meta_sm[12]", /* state machine name */
    0x5a1e4dfc, /* state machine unique ID (md5("epc_pdn_meta_sm[12]") & 0xFFFFFFFF) */
    12  /* this state machine instance */
  },
  {
    &epc_pdn_meta_sm_constdata, /* state machine constant data */
    "epc_pdn_meta_sm[13]", /* state machine name */
    0x12114c4b, /* state machine unique ID (md5("epc_pdn_meta_sm[13]") & 0xFFFFFFFF) */
    13  /* this state machine instance */
  },
  {
    &epc_pdn_meta_sm_constdata, /* state machine constant data */
    "epc_pdn_meta_sm[14]", /* state machine name */
    0x5c398185, /* state machine unique ID (md5("epc_pdn_meta_sm[14]") & 0xFFFFFFFF) */
    14  /* this state machine instance */
  },
  {
    &epc_pdn_meta_sm_constdata, /* state machine constant data */
    "epc_pdn_meta_sm[15]", /* state machine name */
    0x51d46221, /* state machine unique ID (md5("epc_pdn_meta_sm[15]") & 0xFFFFFFFF) */
    15  /* this state machine instance */
  },
};

/* State machine instance array definition */
stm_state_machine_t
  epc_pdn_meta_sm[ epc_pdn_meta_sm_NUM_INSTANCES ] =
{
  {
    &epc_pdn_meta_sm_perinst_constdata[ 0 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_meta_sm_perinst_constdata[ 1 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_meta_sm_perinst_constdata[ 2 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_meta_sm_perinst_constdata[ 3 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_meta_sm_perinst_constdata[ 4 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_meta_sm_perinst_constdata[ 5 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_meta_sm_perinst_constdata[ 6 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_meta_sm_perinst_constdata[ 7 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_meta_sm_perinst_constdata[ 8 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_meta_sm_perinst_constdata[ 9 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_meta_sm_perinst_constdata[ 10 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_meta_sm_perinst_constdata[ 11 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_meta_sm_perinst_constdata[ 12 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_meta_sm_perinst_constdata[ 13 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_meta_sm_perinst_constdata[ 14 ], /* per instance constant data array */
    STM_DEACTIVATED_STATE, /* current state */
    -1, /* current input index */
    NULL, /* calling SM instance */
    FALSE, /* propagate input to parent */
    FALSE, /* locked flag */
    NULL, /* user defined per-instance data */
    0, /* user defined debug mask */
  },
  {
    &epc_pdn_meta_sm_perinst_constdata[ 15 ], /* per instance constant data array */
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
void epc_pdn_meta_sm_reset(void)
{
  uint32 idx;
  void **tricky;

  /* Reset all the child SMs (if any) */
  

  /* Reset the parent */
  for( idx = 0; idx < epc_pdn_meta_sm_NUM_INSTANCES; idx++)
  {
    tricky = (void **)&epc_pdn_meta_sm[ idx ].pi_const_data; /* sleight of hand to assign to const ptr below */
    *tricky = (void *)&epc_pdn_meta_sm_perinst_constdata[ idx ]; /* per instance constant data array */
    epc_pdn_meta_sm[ idx ].current_state = STM_DEACTIVATED_STATE; /* current state */
    epc_pdn_meta_sm[ idx ].curr_input_index = -1; /* current input index */
    epc_pdn_meta_sm[ idx ].propagate_input = FALSE; /* propagate input to parent */
    epc_pdn_meta_sm[ idx ].is_locked = FALSE; /* locked flag */
    epc_pdn_meta_sm[ idx ].user_data = NULL; /* user defined per-instance data */
    epc_pdn_meta_sm[ idx ].debug_mask = 0; /* user defined debug mask */
  }

}
#endif /* !STM_DATA_STRUCTURES_ONLY */

/* End machine generated internal source for state machine array: epc_pdn_meta_sm[] */



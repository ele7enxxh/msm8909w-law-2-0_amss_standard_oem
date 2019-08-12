/*=============================================================================

    __ds_epc_sys_sm_int.c

Description:
  This file contains the machine generated source file for the state machine
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


/* Include STM compiler generated external and internal header files */
#include "__ds_epc_sys_sm.h"
#include "__ds_epc_sys_sm_int.h"

/* Include INPUT_DEF_FILE specified files */
#include <ds_epc_sys_sm.h>

/* Begin machine generated internal source for state machine array: epc_sys_sm[] */

#ifndef STM_DATA_STRUCTURES_ONLY
/* Transition table */
static const stm_transition_fn_t
  epc_sys_sm_transitions[ epc_sys_sm_NUM_STATES * epc_sys_sm_NUM_INPUTS ] =
{
  /* Transition functions for state EPC_SSM_STATE_INIT */
  epc_sys_sm_proc_lte_attach_init_in_init,    /* EPC_SSM_LTE_ATTACH_INIT_EV */
  epc_sys_sm_proc_lte_in_init,    /* EPC_SSM_LTE_EV */
  epc_sys_sm_proc_ehrpd_in_init,    /* EPC_SSM_EHRPD_EV */
  epc_sys_sm_proc_hrpd_in_init,    /* EPC_SSM_HRPD_EV */
  epc_sys_sm_proc_1x_in_init,    /* EPC_SSM_1X_EV */
  epc_sys_sm_proc_lte_tunneling_enable_in_init,    /* EPC_SSM_LTE_TUNNELING_ENABLE_EV */
  epc_sys_sm_proc_lte_tunneling_disable_in_init,    /* EPC_SSM_LTE_TUNNELING_DISABLE_EV */
  epc_sys_sm_proc_other_in_init,    /* EPC_SSM_OTHER_EV */

  /* Transition functions for state EPC_SSM_STATE_LTE_ATTACHING */
  epc_sys_sm_proc_lte_attach_init_in_lte_attaching,    /* EPC_SSM_LTE_ATTACH_INIT_EV */
  epc_sys_sm_proc_lte_in_lte_attaching,    /* EPC_SSM_LTE_EV */
  epc_sys_sm_proc_ehrpd_in_lte_attaching,    /* EPC_SSM_EHRPD_EV */
  epc_sys_sm_proc_hrpd_in_lte_attaching,    /* EPC_SSM_HRPD_EV */
  epc_sys_sm_proc_1x_in_lte_attaching,    /* EPC_SSM_1X_EV */
  epc_sys_sm_proc_lte_tunneling_enable_in_lte_attaching,    /* EPC_SSM_LTE_TUNNELING_ENABLE_EV */
  epc_sys_sm_proc_lte_tunneling_disable_in_lte_attaching,    /* EPC_SSM_LTE_TUNNELING_DISABLE_EV */
  epc_sys_sm_proc_other_in_lte_attaching,    /* EPC_SSM_OTHER_EV */

  /* Transition functions for state EPC_SSM_STATE_LTE */
  epc_sys_sm_proc_lte_attach_init_in_lte,    /* EPC_SSM_LTE_ATTACH_INIT_EV */
  epc_sys_sm_proc_lte_in_lte,    /* EPC_SSM_LTE_EV */
  epc_sys_sm_proc_ehrpd_in_lte,    /* EPC_SSM_EHRPD_EV */
  epc_sys_sm_proc_hrpd_in_lte,    /* EPC_SSM_HRPD_EV */
  epc_sys_sm_proc_1x_in_lte,    /* EPC_SSM_1X_EV */
  epc_sys_sm_proc_lte_tunneling_enable_in_lte,    /* EPC_SSM_LTE_TUNNELING_ENABLE_EV */
  epc_sys_sm_proc_lte_tunneling_disable_in_lte,    /* EPC_SSM_LTE_TUNNELING_DISABLE_EV */
  epc_sys_sm_proc_other_in_lte,    /* EPC_SSM_OTHER_EV */

  /* Transition functions for state EPC_SSM_STATE_1X */
  epc_sys_sm_proc_lte_attach_init_in_1x,    /* EPC_SSM_LTE_ATTACH_INIT_EV */
  epc_sys_sm_proc_lte_in_1x,    /* EPC_SSM_LTE_EV */
  epc_sys_sm_proc_ehrpd_in_1x,    /* EPC_SSM_EHRPD_EV */
  epc_sys_sm_proc_hrpd_in_1x,    /* EPC_SSM_HRPD_EV */
  epc_sys_sm_proc_1x_in_1x,    /* EPC_SSM_1X_EV */
  epc_sys_sm_proc_lte_tunneling_enable_in_1x,    /* EPC_SSM_LTE_TUNNELING_ENABLE_EV */
  epc_sys_sm_proc_lte_tunneling_disable_in_1x,    /* EPC_SSM_LTE_TUNNELING_DISABLE_EV */
  epc_sys_sm_proc_other_in_1x,    /* EPC_SSM_OTHER_EV */

  /* Transition functions for state EPC_SSM_STATE_HRPD */
  epc_sys_sm_proc_lte_attach_init_in_hrpd,    /* EPC_SSM_LTE_ATTACH_INIT_EV */
  epc_sys_sm_proc_lte_in_hrpd,    /* EPC_SSM_LTE_EV */
  epc_sys_sm_proc_ehrpd_in_hrpd,    /* EPC_SSM_EHRPD_EV */
  epc_sys_sm_proc_hrpd_in_hrpd,    /* EPC_SSM_HRPD_EV */
  epc_sys_sm_proc_1x_in_hrpd,    /* EPC_SSM_1X_EV */
  epc_sys_sm_proc_lte_tunneling_enable_in_hrpd,    /* EPC_SSM_LTE_TUNNELING_ENABLE_EV */
  epc_sys_sm_proc_lte_tunneling_disable_in_hrpd,    /* EPC_SSM_LTE_TUNNELING_DISABLE_EV */
  epc_sys_sm_proc_other_in_hrpd,    /* EPC_SSM_OTHER_EV */

  /* Transition functions for state EPC_SSM_STATE_EHRPD */
  epc_sys_sm_proc_lte_attach_init_in_ehrpd,    /* EPC_SSM_LTE_ATTACH_INIT_EV */
  epc_sys_sm_proc_lte_in_ehrpd,    /* EPC_SSM_LTE_EV */
  epc_sys_sm_proc_ehrpd_in_ehrpd,    /* EPC_SSM_EHRPD_EV */
  epc_sys_sm_proc_hrpd_in_ehrpd,    /* EPC_SSM_HRPD_EV */
  epc_sys_sm_proc_1x_in_ehrpd,    /* EPC_SSM_1X_EV */
  epc_sys_sm_proc_lte_tunneling_enable_in_ehrpd,    /* EPC_SSM_LTE_TUNNELING_ENABLE_EV */
  epc_sys_sm_proc_lte_tunneling_disable_in_ehrpd,    /* EPC_SSM_LTE_TUNNELING_DISABLE_EV */
  epc_sys_sm_proc_other_in_ehrpd,    /* EPC_SSM_OTHER_EV */

  /* Transition functions for state EPC_SSM_STATE_LTE_TUNNELING */
  epc_sys_sm_proc_lte_attach_init_in_lte_tunneling,    /* EPC_SSM_LTE_ATTACH_INIT_EV */
  epc_sys_sm_proc_lte_in_lte_tunneling,    /* EPC_SSM_LTE_EV */
  epc_sys_sm_proc_ehrpd_in_lte_tunneling,    /* EPC_SSM_EHRPD_EV */
  epc_sys_sm_proc_hrpd_in_lte_tunneling,    /* EPC_SSM_HRPD_EV */
  epc_sys_sm_proc_1x_in_lte_tunneling,    /* EPC_SSM_1X_EV */
  epc_sys_sm_proc_lte_tunneling_enable_in_lte_tunneling,    /* EPC_SSM_LTE_TUNNELING_ENABLE_EV */
  epc_sys_sm_proc_lte_tunneling_disable_in_lte_tunneling,    /* EPC_SSM_LTE_TUNNELING_DISABLE_EV */
  epc_sys_sm_proc_other_in_lte_tunneling,    /* EPC_SSM_OTHER_EV */

  /* Transition functions for state EPC_SSM_STATE_OTHER */
  epc_sys_sm_proc_lte_attach_init_in_other,    /* EPC_SSM_LTE_ATTACH_INIT_EV */
  epc_sys_sm_proc_lte_in_other,    /* EPC_SSM_LTE_EV */
  epc_sys_sm_proc_ehrpd_in_other,    /* EPC_SSM_EHRPD_EV */
  epc_sys_sm_proc_hrpd_in_other,    /* EPC_SSM_HRPD_EV */
  epc_sys_sm_proc_1x_in_other,    /* EPC_SSM_1X_EV */
  epc_sys_sm_proc_lte_tunneling_enable_in_other,    /* EPC_SSM_LTE_TUNNELING_ENABLE_EV */
  epc_sys_sm_proc_lte_tunneling_disable_in_other,    /* EPC_SSM_LTE_TUNNELING_DISABLE_EV */
  epc_sys_sm_proc_other_in_other,    /* EPC_SSM_OTHER_EV */

};
#endif /* !STM_DATA_STRUCTURES_ONLY */

/* State { name, entry, exit, child SM } table */
static const stm_state_map_t
  epc_sys_sm_states[ epc_sys_sm_NUM_STATES ] =
{
  {"EPC_SSM_STATE_INIT",
#ifndef STM_DATA_STRUCTURES_ONLY
    epc_sys_sm_enter_init, epc_sys_sm_exit_init,
#else /* STM_DATA_STRUCTURES_ONLY */
    NULL, NULL,
#endif /* STM_DATA_STRUCTURES_ONLY */
    NULL},
  {"EPC_SSM_STATE_LTE_ATTACHING",
#ifndef STM_DATA_STRUCTURES_ONLY
    epc_sys_sm_enter_lte_attaching, epc_sys_sm_exit_lte_attaching,
#else /* STM_DATA_STRUCTURES_ONLY */
    NULL, NULL,
#endif /* STM_DATA_STRUCTURES_ONLY */
    NULL},
  {"EPC_SSM_STATE_LTE",
#ifndef STM_DATA_STRUCTURES_ONLY
    epc_sys_sm_enter_lte, epc_sys_sm_exit_lte,
#else /* STM_DATA_STRUCTURES_ONLY */
    NULL, NULL,
#endif /* STM_DATA_STRUCTURES_ONLY */
    NULL},
  {"EPC_SSM_STATE_1X",
#ifndef STM_DATA_STRUCTURES_ONLY
    epc_sys_sm_enter_1x, epc_sys_sm_exit_1x,
#else /* STM_DATA_STRUCTURES_ONLY */
    NULL, NULL,
#endif /* STM_DATA_STRUCTURES_ONLY */
    NULL},
  {"EPC_SSM_STATE_HRPD",
#ifndef STM_DATA_STRUCTURES_ONLY
    epc_sys_sm_enter_hrpd, epc_sys_sm_exit_hrpd,
#else /* STM_DATA_STRUCTURES_ONLY */
    NULL, NULL,
#endif /* STM_DATA_STRUCTURES_ONLY */
    NULL},
  {"EPC_SSM_STATE_EHRPD",
#ifndef STM_DATA_STRUCTURES_ONLY
    epc_sys_sm_enter_ehrpd, epc_sys_sm_exit_ehrpd,
#else /* STM_DATA_STRUCTURES_ONLY */
    NULL, NULL,
#endif /* STM_DATA_STRUCTURES_ONLY */
    NULL},
  {"EPC_SSM_STATE_LTE_TUNNELING",
#ifndef STM_DATA_STRUCTURES_ONLY
    epc_sys_sm_enter_lte_tunneling, epc_sys_sm_exit_lte_tunneling,
#else /* STM_DATA_STRUCTURES_ONLY */
    NULL, NULL,
#endif /* STM_DATA_STRUCTURES_ONLY */
    NULL},
  {"EPC_SSM_STATE_OTHER",
#ifndef STM_DATA_STRUCTURES_ONLY
    epc_sys_sm_enter_other, epc_sys_sm_exit_other,
#else /* STM_DATA_STRUCTURES_ONLY */
    NULL, NULL,
#endif /* STM_DATA_STRUCTURES_ONLY */
    NULL},
};

/* Input { name, value } table */
static const stm_input_map_t
  epc_sys_sm_inputs[ epc_sys_sm_NUM_INPUTS ] =
{
  { "EPC_SSM_LTE_ATTACH_INIT_EV" , (stm_input_t) EPC_SSM_LTE_ATTACH_INIT_EV },
  { "EPC_SSM_LTE_EV" , (stm_input_t) EPC_SSM_LTE_EV },
  { "EPC_SSM_EHRPD_EV" , (stm_input_t) EPC_SSM_EHRPD_EV },
  { "EPC_SSM_HRPD_EV" , (stm_input_t) EPC_SSM_HRPD_EV },
  { "EPC_SSM_1X_EV" , (stm_input_t) EPC_SSM_1X_EV },
  { "EPC_SSM_LTE_TUNNELING_ENABLE_EV" , (stm_input_t) EPC_SSM_LTE_TUNNELING_ENABLE_EV },
  { "EPC_SSM_LTE_TUNNELING_DISABLE_EV" , (stm_input_t) EPC_SSM_LTE_TUNNELING_DISABLE_EV },
  { "EPC_SSM_OTHER_EV" , (stm_input_t) EPC_SSM_OTHER_EV },
};


/* Constant all-instance state machine data */
static const stm_state_machine_constdata_t epc_sys_sm_constdata =
{
  epc_sys_sm_NUM_INSTANCES, /* number of state machine instances */
  epc_sys_sm_NUM_STATES, /* number of states */
  epc_sys_sm_states, /* array of state mappings */
  epc_sys_sm_NUM_INPUTS, /* number of inputs */
  epc_sys_sm_inputs, /* array of input mappings */
#ifndef STM_DATA_STRUCTURES_ONLY
  epc_sys_sm_transitions, /* array of transition function mappings */
  epc_sys_sm_init, /* state machine entry function */
  epc_sys_sm_cleanup, /* state machine exit function */
  NULL, /* state machine error hook function */
  NULL, /* state machine debug hook function */
  EPC_SSM_STATE_INIT /* state machine initial state */
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
  epc_sys_sm_perinst_constdata[ epc_sys_sm_NUM_INSTANCES ] =
{
  {
    &epc_sys_sm_constdata, /* state machine constant data */
    "epc_sys_sm", /* state machine name */
    0xb722f626, /* state machine unique ID (md5("epc_sys_sm") & 0xFFFFFFFF) */
    0  /* this state machine instance */
  },
};

/* State machine instance array definition */
stm_state_machine_t
  epc_sys_sm[ epc_sys_sm_NUM_INSTANCES ] =
{
  {
    &epc_sys_sm_perinst_constdata[ 0 ], /* per instance constant data array */
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
void epc_sys_sm_reset(void)
{
  uint32 idx;
  void **tricky;

  /* Reset all the child SMs (if any) */
  

  /* Reset the parent */
  for( idx = 0; idx < epc_sys_sm_NUM_INSTANCES; idx++)
  {
    tricky = (void **)&epc_sys_sm[ idx ].pi_const_data; /* sleight of hand to assign to const ptr below */
    *tricky = (void *)&epc_sys_sm_perinst_constdata[ idx ]; /* per instance constant data array */
    epc_sys_sm[ idx ].current_state = STM_DEACTIVATED_STATE; /* current state */
    epc_sys_sm[ idx ].curr_input_index = -1; /* current input index */
    epc_sys_sm[ idx ].propagate_input = FALSE; /* propagate input to parent */
    epc_sys_sm[ idx ].is_locked = FALSE; /* locked flag */
    epc_sys_sm[ idx ].user_data = NULL; /* user defined per-instance data */
    epc_sys_sm[ idx ].debug_mask = 0; /* user defined debug mask */
  }

}
#endif /* !STM_DATA_STRUCTURES_ONLY */

/* End machine generated internal source for state machine array: epc_sys_sm[] */



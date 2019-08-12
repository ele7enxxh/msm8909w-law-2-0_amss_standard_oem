#ifndef GL1_HW_SLEEP_TEST_H
#define GL1_HW_SLEEP_TEST_H

/*===========================================================================
                      
            G L 1   H W   S L E E P   T E S T  
                      
  GENERAL DESCRIPTION
                      
    This module allows EFS controlled delays to be inserted into the sleep/wake 
    cycle for test purposes.

  EXTERNALIZED FUNCTIONS
                      
    gl1_hw_test_delay_init
    gl1_hw_inject_test_delay
                      
  COPYRIGHT INFORMATION

Copyright (c) 2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header
$ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
10/12/14   ws       CR768376 Q6 compile warning fixes
08/11/14   df       CR733947 Use EFS file settings to inject test delays in sleep code 
27/11/14   df       CR758398 Increase effectiveness sleep delay patterns
===========================================================================*/
#ifdef FEATURE_GSM_TEST_SLEEP_DELAYS

#include "geran_variation.h"
#include "slpc.h"
#include "geran_dual_sim.h"

/*----------------------------------------------------------------------------
  Locations where a test sleep delay can be injected in a sleep cycle
----------------------------------------------------------------------------*/
typedef enum {
  GL1_HW_SLP_DLY_NO_DELAY = 0,
  GL1_HW_SLP_DLY_SLPC_WARMUP_CB,
  GL1_HW_SLP_DLY_RF_CNF,
  GL1_HW_SLP_DLY_MCPM,
  GL1_HW_SLP_DLY_WAKEUP_ISR,
  GL1_HW_SLP_DLY_MAX_LOC
} gl1_hw_slp_delay_loc_enum;

/*----------------------------------------------------------------------------
  Delay modes for injected sleep delays
----------------------------------------------------------------------------*/
typedef enum {
  GL1_HW_SLP_DLY_DISABLED_MODE = 0,
  GL1_HW_SLP_DLY_RANDOM_MODE,   /* Next pattern is randomly chosen from all patterns */
  GL1_HW_SLP_DLY_CYCLE_MODE,    /* Cycle through all patterns */
  GL1_HW_SLP_DLY_SELECT_MODE,   /* Keep injecting the same pattern */
  GL1_HW_SLP_DLY_INCREMENT_MODE, /* Injects a slowly increasing delay at every location */
  GL1_HW_SLP_DLY_MAX_MODE
} gl1_hw_slp_delay_mode_enum;

/*----------------------------------------------------------------------------
  Complete description of a test sleep delay pattern to be injected
----------------------------------------------------------------------------*/
typedef struct
{
  /* Location in sleep/wake cycle where delay should be injected */
  gl1_hw_slp_delay_loc_enum delay_loc; 
  
  /* length of that delay (us) */
  uint32                    delay_length; 
  
  /* Instead of always using delay_length, randomly pick a delay from 1 to delay_length */
  boolean                   random_delay_length;

  /* Num sleep/wake cycles to execute without a delay before injecting delays */
  uint32                    num_cycles;

  /* Randomly pick num cycles without a delay from 1 to num_cycles */
  boolean                   random_num_cycles;

  /* Once delays start, number of consecutive delays to 
     inject before moving on to next delay pattern */
  uint32                    num_consec_dlys;
} gl1_hw_slp_delay_pttn_type;

/*----------------------------------------------------------------------------
  Data for control of injecting sleep test delays
----------------------------------------------------------------------------*/
typedef struct
{
  /* Mode as set in EFS */
  gl1_hw_slp_delay_mode_enum mode;

  /* set once we have injected the delay for current slep/wake cycle */
  boolean                    delay_injected;

  /* The current sleep test delay pattern being injected */
  uint32                     curr_slp_pttn; 

  /* Countdown num of sleep/wake cycles without a delay injected */
  uint32                     cycle_cnt;

  /* Countdown num of consecutive sleep delay cycles to be injected */
  uint32                     consec_cnt;

  /* The delay (us) to be injected */
  uint32                     delay;
}gl1_hw_slp_delay_data_type;

void gl1_hw_test_delay_init(uint32 EFS_data);
void gl1_hw_inject_test_delay(gl1_hw_slp_delay_loc_enum delay_loc, gas_id_t gas_id);

#define GL1_HW_INJECT_TEST_DELAY(arg1, arg2) gl1_hw_inject_test_delay(arg1, arg2)

#else /* FEATURE_GSM_TEST_SLEEP_DELAYS */

/* FEATURE_GSM_TEST_SLEEP_DELAYS not defined + possibly stripped, so this macro should do nothing */
#define GL1_HW_INJECT_TEST_DELAY(arg1, arg2)

#endif /* FEATURE_GSM_TEST_SLEEP_DELAYS */
#endif /* GL1_HW_SLEEP_TEST_H */
/* EOF */

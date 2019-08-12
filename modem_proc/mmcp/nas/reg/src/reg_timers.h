#ifndef REG_TIMERS_H
#define REG_TIMERS_H
/*==============================================================================


                 R E G  -  T I M E R S   H E A D E R   F I L E


                                  DESCRIPTION

This header file defines the functions required for starting and stopping the
timers required for the REG module.

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_timers.h_v   1.1   22 May 2002 09:31:14   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/src/reg_timers.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/13/02   kwa     Initial release.
05/21/02   kwa     Added watchdog timer.
10/18/02   kwa     Added FEATURE_REG_DEEP_SLEEP.
11/02/02   kwa     Added entered_deep_sleep parameter to 
                   reg_timers_start_plmn_search_period_timer.
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
02/22/05   up      Updated for Cingular ENS feature.
                   Added Function reg_timers_restart_hplmn_search_period_timer.
06/28/05   up      Function reg_timers_start_hplmn_search_timer_10sec is added
                   to start HPLMN timer of 10 sec in connected mode.
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "reg_sim_v.h"
#include "reg_state.h"
/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*
** This macro indicates the max reg timer value
*/
#define REG_TIMERS_MAX_TIMER_VALUE       (rex_timer_cnt_type)~0

#define REG_T3245_DEFUALT_VALUE           (rex_timer_cnt_type)0  
#define REG_TIMERS_UPDATE_LTE_CAP_VALUE       (rex_timer_cnt_type)1000

/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

/*==============================================================================

                             FUNCTION DECLARATIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  reg_timers_initialize

DESCRIPTION

  Function that initializes the REG timers.

RETURN VALUE

  None

==============================================================================*/
void reg_timers_initialize
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_timers_start_hplmn_search_timer_10sec

DESCRIPTION

  30 sec timer timer is started when higher priority search is rejected from lower layer.

RETURN VALUE

  None

==============================================================================*/

extern void reg_timers_start_hplmn_search_timer_10sec
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_timers_start_hplmn_search_period_timer

DESCRIPTION

  Function that starts the HPLMN Search Period Timer.

RETURN VALUE

  None

==============================================================================*/

extern void reg_timers_start_hplmn_search_period_timer
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_timers_stop_hplmn_search_period_timer

DESCRIPTION

  Function that stops the HPLMN Search Period Timer.

RETURN VALUE

  None

==============================================================================*/

extern void reg_timers_stop_hplmn_search_period_timer
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_timers_restart_hplmn_search_period_timer

DESCRIPTION

  Function to restart HPLMN timer if already running.

RETURN VALUE

  boolean - True if HPLMN timer is restarted.

==============================================================================*/
boolean reg_timers_restart_hplmn_search_period_timer
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_timers_stop_hplmn_irat_search_period_timer

DESCRIPTION

  Function that stops the HPLMN IRAT Search Period Timer.

RETURN VALUE

  None

==============================================================================*/

extern void reg_timers_stop_hplmn_irat_search_period_timer
(
  void
);

extern void reg_timers_stop_all_irat_search_period_timer(void);

/*==============================================================================

FUNCTION NAME

  reg_timer_bplmn_irat_running

==============================================================================*/


extern boolean reg_timer_bplmn_irat_running 
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_timers_start_hplmn_irat_search_period_timer

DESCRIPTION

  Function that starts the HPLMN IRAT Search Period Timer.

RETURN VALUE

  None

==============================================================================*/

extern void reg_timers_start_hplmn_irat_search_period_timer
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_timers_start_lte_irat_search_period_timer

DESCRIPTION

  Function that starts the HPLMN LTE IRAT Search Period Timer.

RETURN VALUE

  None

==============================================================================*/

void reg_timers_start_lte_irat_search_period_timer
(
  void
);

void reg_timers_stop_lte_irat_search_period_timer
(
  void
);
/*==============================================================================

FUNCTION NAME

  reg_timer_state_hplmn_irat_search_period_timer_10sec

DESCRIPTION

  Function that starts the HPLMN IRAT Search Period Timer for 10 seconds

RETURN VALUE

  None
==============================================================================*/
extern void reg_timers_start_hplmn_irat_search_period_timer_10sec
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_timer_state_hplmn_lte_irat_search_period_timer_10sec

DESCRIPTION

  Function that starts the HPLMN IRAT Search Period Timer for 10 seconds

RETURN VALUE

  None
==============================================================================*/

void reg_timers_start_lte_irat_search_period_timer_10sec
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_timer_restart_hplmn_irat_search_period_timer

DESCRIPTION

  Function that re-starts the HPLMN IRAT Search Period Timer for 10 seconds

RETURN VALUE

  None

==============================================================================*/
extern void reg_timers_restart_hplmn_irat_search_period_timer
(
  void
);


extern rex_timer_cnt_type reg_timers_get_remaining_lte_duration_timer(void);

extern void reg_timers_restart_backoff_lte_search_period_timer
(
  rex_timer_cnt_type     timer
);

void reg_timers_stop_backoff_lte_search_period_timer
(
  void
);

void reg_timers_adjust_backoff_lte_search_timer_count
(
  lte_cell_list_type* lte_list_ptr
);


/*===========================================================================
FUNCTION NAME 
         reg_is_lte_timer_running

DESCRIPTION
  Returns the boolean value to indicate if the LTE timer of a given parameter
  is running
  
RETURN VALUE
  None
===========================================================================*/
extern boolean reg_is_lte_timer_running(reg_lte_timer_e_type timer);

/*==============================================================================

FUNCTION NAME

  reg_timers_pet_watchdog

DESCRIPTION

  Function that pets the REG watchdog.

RETURN VALUE

  None

==============================================================================*/

extern void reg_timers_pet_watchdog
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_is_timer_active

==============================================================================*/
extern boolean reg_is_timer_active
(
  byte timer_id
);


/*==============================================================================

FUNCTION NAME

  reg_timers_start_t3245_timer
  
DESCRIPTION

  This function starts T3245 with the value provided if it is not zero. If the value provided to this function
  is zero then T3245 is started with random value.

RETURN VALUE

  None


==============================================================================*/

extern void reg_timers_start_t3245_timer
(
  dword t3245_timer
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif
);


/*==============================================================================

FUNCTION NAME

  reg_timers_get_rem_t3245_value

DESCRIPTION

  This function gets the remaining timer left for T3245 to expire

RETURN VALUE

  timer count


==============================================================================*/

extern rex_timer_cnt_type reg_timers_get_rem_t3245_value
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_timers_stop_t3245

DESCRIPTION

  This function stops T3245 if it is running.

RETURN VALUE

  None


==============================================================================*/

extern void reg_timers_stop_t3245
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_timers_start_validate_sim_timer

DESCRIPTION

  This function starts validate Sim timer with the default value of T3245 provided in EFS

RETURN VALUE

  None

==============================================================================*/

void reg_timers_start_validate_sim_timer
(
  void
);

#ifdef FEATURE_SGLTE

/*==============================================================================

FUNCTION NAME

  reg_timers_start_validate_sim_timer_per_stack

==============================================================================*/

void reg_timers_start_validate_sim_timer_per_stack
(
  rex_timer_cnt_type validate_sim_timer
);

rex_timer_cnt_type reg_timer_get_validate_sim_timer(void);
#endif

/*==============================================================================

FUNCTION NAME

  reg_timers_stop_validate_sim_timer

DESCRIPTION

  This function stops validate Sim timer

RETURN VALUE

  None


==============================================================================*/

void reg_timers_stop_validate_sim_timer(void);


/*==============================================================================

FUNCTION NAME

  reg_is_t3245_timer_active

DESCRIPTION

  This function checks if T3245 is running

RETURN VALUE

  boolean

==============================================================================*/
extern boolean reg_is_t3245_timer_active
(
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type as_id
#else
  void
#endif
);


#ifdef FEATURE_LTE
extern void reg_timers_restart_backoff_fplmn_search_period_timer
(
  rex_timer_cnt_type                     timer,
  reg_as_id_e_type                       as_id
);
extern void reg_timers_start_backoff_fplmn_search_period_timer
(
  reg_sim_backoff_fplmn_list_info_s_type *backoff_fplmn_list_info_p,
  rex_timer_cnt_type                     timer,
  reg_as_id_e_type                       as_id
);
extern void reg_timers_stop_backoff_fplmn_search_period_timer
(
  reg_as_id_e_type                       as_id
);
/*==============================================================================

FUNCTION NAME

  reg_timers_get_rem_backoff_fplmn_search_timer_value

DESCRIPTION

  This function gets the remaining timer left for backoff_fplmn_search_timer to expire

RETURN VALUE

  timer count


==============================================================================*/

extern rex_timer_cnt_type reg_timers_get_rem_backoff_fplmn_search_timer_value
(
  reg_as_id_e_type                       as_id
);
/*==============================================================================

FUNCTION NAME

  reg_timers_start_update_lte_cap_timer

==============================================================================*/

extern void reg_timers_start_update_lte_cap_timer
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_timers_stop_update_lte_cap_timer

==============================================================================*/

extern void reg_timers_stop_update_lte_cap_timer
(
  void
);
#endif 
#ifdef FEATURE_SGLTE
extern void reg_timers_start_sglte_ps_timer(void);
extern void reg_timers_stop_sglte_ps_timer
(
  void
);
extern void reg_timers_start_sglte_ps_timer_val(rex_timer_cnt_type timer_count);
#endif
/*===========================================================================
FUNCTION NAME 
         reg_timers_get_hplmn_search_timer

DESCRIPTION
  Function to update the HPLMN timer value
  
RETURN VALUE
  None
==========================================================================*/
extern uint32 reg_timers_get_hplmn_search_timer(void);

/*===========================================================================
FUNCTION NAME 
         reg_timers_set_hplmn_search_timer

DESCRIPTION
  Function to update the HPLMN timer value
  
RETURN VALUE
  None
===========================================================================*/
extern void reg_timers_set_hplmn_search_timer(uint32 timer_value);

#endif

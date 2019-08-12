/*!
  @file
  lte_ml1_irat_ext_api.h

  @brief
  This file describes the API interfaces to LTE ML1 for IRAT
 
  WARNING:
  THIS API IS TO BE CALLED FROM OTHER THREADS AND SHOULD NOT ACCESS ANY
  L1 DATA STRUCTURES. ONLY CONSTANT DATA OR PARAMETERS PASSED IN THE API
  SHOULD BE ACCESSED IN THIS FILE.

  For X2L APIs:
  Caller should call the lte_ml1_irat_gap_init() first which will initialize the gap state.
  Then lte_ml1_irat_gap_schedule() is to be called before every gap.

  If lte_ml1_irat_gap_init() is not called for any scenario where the gap state needs to be
  initialized, calling lte_ml1_irat_gap_schedule() thereafter will result in unknown behavior.

  lte_ml1_irat_gap_schedule() needs to be called with the timed_srch_meas_req struct with all
  applicable paramerters set. Depending on the next gap type, ML1 will modify the timed_srch_meas_req
  and then caller should send this request message back to ML1 via the message router interface.
*/

/*=============================================================================

  Copyright (c) 2010-2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who   what, where, why
--------   ---   -------------------------------------------------------------- 
05/01/13   sks   Added support for X2L gap scheduling.
03/05/13   mc    Initial version - IRAT to RF tune structure type conversion
=============================================================================*/
#ifndef LTE_ML1_IRAT_EXT_API_H
#define LTE_ML1_IRAT_EXT_API_H

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "lte_as.h"
#include "lte_irat_types.h"
#include "rfmeas_lte.h"
#include <lte.h>
#include <lte_cphy_irat_meas_msg.h>

/*=============================================================================

                         EXTERNAL DEFINITIONS AND TYPES

=============================================================================*/
struct rfm_meas_header_struct;
struct rfm_meas_lte_setup_and_build_scripts_param_struct;

typedef enum
{
  LTE_ML1_IRAT_SMALL_GAP_1, /* Small AGC Gap */
  LTE_ML1_IRAT_SMALL_GAP_2, /* Small AGC+Srch Gap */
  LTE_ML1_IRAT_SMALL_GAP_3, /* Small AGC+Srch in Vcell mode Gap */
  LTE_ML1_IRAT_SMALL_GAP_4, /* Small AGC+Srch+Meas Gap */
  LTE_ML1_IRAT_SMALL_GAP_5, /* Small AGC on F1 + Meas on F2 Gap */
  LTE_ML1_IRAT_MEDIUM_GAP_1, /* Medium AGC + Srch Gap */
  LTE_ML1_IRAT_MEDIUM_GAP_2, /* Medium AGC+Srch+Meas Gap */
  LTE_ML1_IRAT_MEDIUM_GAP_3, /* Medium AGC + Meas Gap */
  LTE_ML1_IRAT_FAT_GAP, /* Fat gap with AGC+Srch+meas */
  LTE_ML1_IRAT_INVALID_GAP,
  LTE_ML1_IRAT_MAX_GAP = LTE_ML1_IRAT_INVALID_GAP
}lte_ml1_irat_gap_schedule_gap_type_e;

typedef struct
{
  /* Last configured gap type. */
  lte_ml1_irat_gap_schedule_gap_type_e  last_gap_type;

  uint64  last_gap_time_stamp;

} lte_ml1_irat_gap_schedule_gap_state_s;

typedef struct
{

  /* Gap lenght configured for the current X2L gap as decided by ML1*/
  uint32  gap_length_config_for_x2l_in_us;

  /* boolean indicating if search/meas is done for the current frequency
     and from the next gap, caller should move to the next frequency. */
  boolean move_to_next_freq;

} lte_ml1_irat_gap_scheduled_info_s;

typedef enum
{
  LTE_ML1_IRAT_GAP_SUCCESS,
  LTE_ML1_IRAT_GAP_FAIL_AS_GAP_LENGH_INVALID
} lte_ml1_irat_gap_schedule_return_e;

typedef enum
{
  LTE_ML1_IRAT_GAP_SCHEDULE_HINT_AUTO ,
  LTE_ML1_IRAT_GAP_SCHEDULE_HINT_SEARCH_AND_MEAS,
  LTE_ML1_IRAT_GAP_SCHEDULE_HINT_MEAS_ONLY
} lte_ml1_irat_gap_schedule_hint_e;
/*=============================================================================

                        EXTERNAL FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================

  FUNCTION: lte_ml1_irat_convert_l2x_tuneaway_params_to_rfm_meas_types

===========================================================================*/
/*!
    @brief
    Populates the common LTE/RF structures needed for tuneaway.

*/
/*=========================================================================*/
void lte_ml1_irat_convert_l2x_tuneaway_params_to_rfm_meas_types
( 
  /*! IN: Pointer to the L2X tune parameters */
  const lte_irat_l2x_tuneaway_param_s *l2x_tuneaway_param,

  /*! OUT: Pointer to the RF header structure to populate */
  struct rfm_meas_header_struct *rf_header,

  /*! OUT: Pointer to RF LTE tuning structure to populate.*/
  struct rfm_meas_lte_setup_and_build_scripts_param_struct *rf_tuneaway_param
);

/*===========================================================================

  FUNCTION: lte_ml1_irat_request_chain

===========================================================================*/
/*!
    @brief
    Called by source RAT to reserve TRM resources for reselection

    @param[in] dl_earfcn The eUTRAN channel number to lock

    @retval TRUE if TRM reservation was successful
    @retval FALSE otherwise
*/
/*=========================================================================*/
boolean lte_ml1_irat_request_chain
(
  /*! The eUTRAN RF channel number to lock */
  lte_earfcn_t dl_earfcn
);

/*===========================================================================

  FUNCTION:  lte_ml1_irat_gap_schedule

===========================================================================*/
/*!
    @brief
    To be called by the caller for every X2L connected mode gap.
    This is a wrapper function for lte_ml1_irat_setup_schedule.

    @return
    This function will return a enum indicating X2L can be scheduled in the next
	gap or it cannot be because of certain invalid paramerters.

*/
/*=========================================================================*/

lte_ml1_irat_gap_schedule_return_e
lte_ml1_irat_gap_schedule
(
  /* The timed srch-meas request messge which caller sends to LTE.
     Caller is required to fill all the applicable fields (gap info,
     cell to meas etc..) */
  lte_cphy_irat_meas_timed_srch_meas_req_s *search_meas_req,

  /* Gap state struct which will get updated by this function. This shall 
     not be used by the caller.
     Needs to be stored by the caller and passed for every func call */
  lte_ml1_irat_gap_schedule_gap_state_s  *gap_state,
  
  /* Gap scheduled info which will get updated by this function and 
     can be used by the caller. */
  lte_ml1_irat_gap_scheduled_info_s *gap_sched_info,

  /* Caller needs to send the next freq to be searched. This will be used 
     for configuring and making decisions on gap type 
     LTE_ML1_SMALL_AGC_F1_MEAS_F2_GAP. If there is single freq, next freq is 
     to be set equal to the freq mentioned in the "search_meas_req" above. */
  lte_earfcn_t  next_freq
);

/*===========================================================================

  FUNCTION:  lte_ml1_irat_setup_schedule

===========================================================================*/
/*!
    @brief
    To be called by the caller for every X2L gap.
    LTE will configure the gap lenght to be used and also the gap type.

    @return
    This function will return a enum indicating X2L can be scheduled in the next
	gap or it cannot be because of certain invalid paramerters.

*/
/*=========================================================================*/

lte_ml1_irat_gap_schedule_return_e
lte_ml1_irat_setup_schedule
(
  /*! IN: Hint to schedule, AUTO indicates api will decide */
  lte_ml1_irat_gap_schedule_hint_e schedule_hint,

  /* The timed srch-meas request messge which caller sends to LTE.
     Caller is required to fill all the applicable fields (gap info,
     cell to meas etc..) */
  lte_cphy_irat_meas_timed_srch_meas_req_s *search_meas_req,

  /* Gap state struct which will get updated by this function. This shall
     not be used by the caller.
     Needs to be stored by the caller and passed for every func call */
  lte_ml1_irat_gap_schedule_gap_state_s  *gap_state,

  /* Gap scheduled info which will get updated by this function and
     can be used by the caller. */
  lte_ml1_irat_gap_scheduled_info_s *gap_sched_info,

  /* Caller needs to send the next freq to be searched. This will be used
     for configuring and making decisions on gap type
     LTE_ML1_SMALL_AGC_F1_MEAS_F2_GAP. If there is single freq, next freq is
     to be set equal to the freq mentioned in the "search_meas_req" above. */
  lte_earfcn_t  next_freq
);

/*===========================================================================

  FUNCTION:  lte_ml1_irat_gap_init

===========================================================================*/
/*!
    @brief
    Function to be called to init the gap state.

    @return
    None

*/
/*=========================================================================*/
void lte_ml1_irat_gap_init
(
  lte_ml1_irat_gap_schedule_gap_state_s* gap_state
);

/*===========================================================================

  FUNCTION:  lte_ml1_irat_gap_srch_meas_cnf_handling

===========================================================================*/
/*!
    @brief
    Function to be called after the caller recieves timed srch meas cnf message.

    @return
    None

*/
/*=========================================================================*/

void lte_ml1_irat_gap_srch_meas_cnf_handling
(
  /* Caller needs to send the timed srch meas cnf message pointer */
  lte_cphy_irat_meas_timed_srch_meas_cnf_s  *srch_meas_cnf,

  /* Gap state struct which will get updated by this function. This shall 
     not be used by the caller.
     Needs to be stored by the caller and passed for every func call */
  lte_ml1_irat_gap_schedule_gap_state_s  *gap_state
);

#endif /* LTE_ML1_IRAT_EXT_API_H */

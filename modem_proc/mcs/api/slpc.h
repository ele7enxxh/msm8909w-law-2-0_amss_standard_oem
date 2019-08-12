#ifndef SLPC_H
#define SLPC_H

/*=========================================================================
  FILE: slpc.h
 
  OVERVIEW:
 

  DEPENDENCIES:
 
 
  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
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
=========================================================================*/

/*==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/slpc.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who   what, where, why
----------   ---   ----------------------------------------------------------- 
05/02/2014   btl   Add notify wakeup callback
05/31/2013   cab   Added support for TSTS
04/24/2013   cab   Added support for N-way technology slam 
12/11/2012   cab   Add notify callback deregister capability 
12/04/2012   cab   Add alternate tstmr for W/G operation
07/16/2012   cab   Added alternate id for G/W simultaneous operation 
07/16/2012   cab   Added slpc_sufficient_warmup_time
09/14/2011   cab   Initial version 

============================================================================*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include "comdef.h"
#include "DDITimetick.h"


/*============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typedefs,
and other items needed by this module.

============================================================================*/

/*----------------------------------------------------------------------------
  Sleep controller identifiers
----------------------------------------------------------------------------*/

typedef enum
{
  SLPC_GSM,
  SLPC_1X,
  SLPC_HDR,
  SLPC_WCDMA,
  SLPC_WCDMA2,
  SLPC_LTE,
  SLPC_TDSCDMA,
  SLPC_GSM_ALT,
  SLPC_GSM2 = SLPC_GSM_ALT,
  SLPC_GSM3,

  SLPC_NUM_CLIENTS

} slpc_id_type;

typedef int64 ustmr_type;
typedef int64 tstmr_type;
typedef void (*slpc_event_callback_type) (void);
typedef void (*slpc_notify_cb_type) 
  (slpc_id_type id, uint64 wakeup_tick, boolean update, boolean extension);
typedef void (*slpc_notify_wakeup_cb_type)
  (slpc_id_type id, uint64 wakeup_tick);

typedef struct
{
  uint64 time;
  uint64 phase;
  uint64 alt_time;
  uint64 alt_phase;
} slpc_tstmr_type;

typedef enum
{
  SLPC_ERR_NO_ERR,
  SLPC_ERR_WAKEUP_IN_PAST
} slpc_err_type;

typedef enum
{
  SLPC_STMR_VSTMR,
  SLPC_STMR_HW
} slpc_stmr_mode_enum;

/******************************************************************************
  @brief SLPC_SET_STMR_MODE

  Set whether SLPC client uses HW TSTMR or virtual STMR (VSTMR).
  If not set, will default to VSTMR.

  @param id: identifies the calling client
  @param mode: is client using VSTMR or HW STMR?
  @return none.
******************************************************************************/
void slpc_set_stmr_mode( slpc_id_type id, slpc_stmr_mode_enum mode );

/******************************************************************************
  @brief SLPC_GET_STMR_MODE

  Return whether SLPC client uses HW TSTMR or virtual STMR (VSTMR).

  @param id: identifies the calling client
  @param mode: is client using VSTMR or HW STMR?
  @return slpc_stmr_mode_enum
******************************************************************************/
slpc_stmr_mode_enum slpc_get_stmr_mode( slpc_id_type id );

/******************************************************************************
  @brief SLPC_IS_ACTIVE

  Is sleep controller active for client (started/sleep/wakeup/warmup)?

  @param id: identifies the calling client
  @return TRUE if slpc active, else FALSE
******************************************************************************/
boolean slpc_is_active( slpc_id_type id );

/******************************************************************************
  @brief SLPC_START

  Get snapshot of current timer values for start of sleep cycle

  @param id: identifies the calling client
  @return start tick (TSTMR)
******************************************************************************/
slpc_tstmr_type slpc_start( slpc_id_type id );

/******************************************************************************
  @brief SLPC_GET_START_TSTMR

  Get start TSTMR value for alt tech

  @param id: identifies the calling client
  @return: alt tstmr
******************************************************************************/
slpc_tstmr_type slpc_get_start_tstmr( slpc_id_type id, slpc_id_type alt_id );

/******************************************************************************
  @brief SLPC_SET_DURATION
  Sets the sleep duration (in TSTMR ticks) and programs the wakeup timer

  @detail 
  The warmup period is subtracted from the sleep duration (ending/OLS) 
  to determine the wakeup point.

  @param id: identifies the calling client
  @param tstmr_dur: the duration of sleep requested, in tstmr ticks
  @return slpc_err_type
******************************************************************************/
slpc_err_type slpc_set_duration( slpc_id_type id, uint64 tstmr_dur_ts );

/******************************************************************************
  @brief SLPC_SET_NOTIFY_CALLBACK
  Set a callbacks for anyone wanting to track wakeup point changes, 
  called whenever any tech sets a wakeup point

  @param n_cb: callback
  @return none
******************************************************************************/
void slpc_set_notify_callback( slpc_notify_cb_type n_cb );

/******************************************************************************
  @brief SLPC_SET_NOTIFY_WAKEUP_CALLBACK

  This callback will be called at any tech's wakeup event. For anyone
  wanting to track wakeups.

  @param id: n_cb: notify callback

  @return none. 
******************************************************************************/
void slpc_set_notify_wakeup_callback( slpc_notify_wakeup_cb_type n_cb );

/******************************************************************************
  @brief SLPC_DEREGISTER_NOTIFY_CALLBACK

  Clear a previously set notify callback

  @param n_cb: callback
  @return none
******************************************************************************/
void slpc_deregister_notify_callback( slpc_notify_cb_type n_cb );

/******************************************************************************
  @brief SLPC_DEREGISTER_NOTIFY_WAKEUP_CALLBACK

  Deregister the wakeup callback, if previously set

  @param id: n_cb: notify callback

  @return none. 
******************************************************************************/
void slpc_deregister_notify_wakeup_callback( slpc_notify_wakeup_cb_type n_cb );

/******************************************************************************
  @brief SLPC_SET_OLS_CALLBACK

  Set the callback to call when the slpc ending point (online start) is
  reached.

  Note this callback is not used with VSTMR, because there is no hardware
  to "slam", so everything is ready after complete_wakeup.

  @param id: identifies the calling client
  @param e_cb: event callback
  @return none
******************************************************************************/
void slpc_set_ols_callback( slpc_id_type id, slpc_event_callback_type e_cb );

/******************************************************************************
  @brief SLPC_SET_WAKEUP_CALLBACK

  Sets the function to call at the wakeup event. Wakeup event will happen
  at (ending point - warmup time). In this callback, wakeup can either be
  extended (set_duration), or commited (commit_wakeup, at which point it
  cannot be changed).

  @param id: identifies the calling client
  @param e_cb: event callback
  @return none
******************************************************************************/
void slpc_set_wakeup_callback( slpc_id_type id, slpc_event_callback_type w_cb );

/******************************************************************************
  @brief SLPC_SET_DURATION_AND_WARMUP
  Sets the sleep duration (in TSTMR ticks), the required warmup length,
  and programs the wakeup timer

  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in tstmr ticks
  @param dur_usec: warmup length, in usec
  @return slpc_err_type
******************************************************************************/
slpc_err_type slpc_set_duration_and_warmup( slpc_id_type id, uint64 tstmr_dur, 
                                            uint16 dur_usec );

/******************************************************************************
  @brief SLPC_SET_WARMUP

  Set the warmup period. This determines amount of time prior to the 
  ending point that the wakeup callback should occur.

  @param id: identifies the calling client
  @param warmup_usec: warmup period in usec
  @return slpc_err_type
******************************************************************************/
slpc_err_type slpc_set_warmup( slpc_id_type id, uint16 warmup_usec );

/******************************************************************************
  @brief SLPC_SUFFICIENT_WARMUP_TIME

  May be called by the wakeup_cb to determine whether or not the wakeup
  event is being processed too late.

  @param id: identifies the calling client
  @param min_warmup: min required warmup time (usec)
  @return boolean
******************************************************************************/
boolean slpc_sufficient_warmup_time ( slpc_id_type id, uint32 min_warmup );

/******************************************************************************
  @brief SLPC_COMPLETE_WAKEUP

  May be called by the wakeup_cb to commit to the wakeup if there is
  sufficient warmup time remaining.

  @param id: identifies the calling client
  @param min_warmup: min required warmup time (usec)
  @param tstmr: pointer to TSTMR for SLPC to program, or NULL if client
                will handle this
  @return FALSE if not enough time or no HW resources available
  @return TRUE if succeeded
******************************************************************************/
boolean slpc_complete_wakeup ( slpc_id_type id, uint32 min_warmup, 
                               slpc_tstmr_type *tstmr );

/******************************************************************************
  @brief SLPC_ERROR_FEEDBACK

  Adjust the slow clock frequency estimate, based on the sleep duration
  and the reacquisition slews.

  @param id: identifies the calling client
  @param slew_err: slew error (TSTMR)
  @return none. 
******************************************************************************/
void slpc_error_feedback( slpc_id_type id, int64 slew_err );

/******************************************************************************
  @brief SLPC_GET_WAKEUP_TICK

  Get wakeup interrupt point, in Qtimer ticks

  @param id: identifies the calling client

  @return absolute timetick of the wakeup point
******************************************************************************/
uint64 slpc_get_wakeup_tick( slpc_id_type id );

/******************************************************************************
  @brief SLPC_GET_OLS_TICK

  Get the absolute timetick of the ending interrupt (Qtimer)

  @param id: identifies the calling client

  @return absolute timetick of the online start point
******************************************************************************/
uint64 slpc_get_ols_tick( slpc_id_type id );

/******************************************************************************
  @brief SLPC_GET_WAKEUP_AND_OLS_TICK

  Get the absolute timeticks of the wakeup and online start points

  @param id: identifies the calling client

  @return absolute timetick of the wakeup point and OLS point
******************************************************************************/
void slpc_get_wakeup_and_ols_tick( slpc_id_type id, uint64 *wakeup_tick, 
                                   uint64 *ols_tick );

/******************************************************************************
  @brief SLPC_GET_SYSCLK_COUNT

  Returns the number of TSTMR ticks the controller has been active for.

  @param id: identifies the calling client
  @return TSTMR ticks
******************************************************************************/
uint64 slpc_get_sysclk_count( slpc_id_type id );

/******************************************************************************
  @brief SLPC_ABORT

  Stop the current sleep cycle and reset the sleep controller state

  @param id: identifies the calling client
  @return none. 
******************************************************************************/
void slpc_abort ( slpc_id_type id );

/******************************************************************************
  @brief SLPC_GET_TICK

  Get 64-bit absolute timetick

  @param none
  @return timetick
******************************************************************************/
uint64 slpc_get_tick ( void );

/******************************************************************************
  @brief slpc_enable_alt_id

  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 

  @param id: identifies the calling client
  @return none. 
******************************************************************************/
void slpc_enable_alt_id( slpc_id_type id, slpc_id_type alt_id, 
                         slpc_tstmr_type *tstmr );

/******************************************************************************
  @brief slpc_disable_alt_id

  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 

  @param id: identifies the calling client
         alt_id:  id's the additional tech that should no longer be slammed
  @see 
  @return none. 
******************************************************************************/
void slpc_disable_alt_id( slpc_id_type id, slpc_id_type alt_id );

#endif

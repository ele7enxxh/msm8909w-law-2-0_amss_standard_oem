#ifndef SLPCI_H
#define SLPCI_H

/*=========================================================================
  FILE: slpci.h
 
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/slpc/src/slpci.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who   what, where, why
----------   ---   ----------------------------------------------------------- 
04/24/2013   cab   Added support for N-way technology slam 
03/29/13     cab   Added trace for sync failure
12/12/12     cab   Adds error feedback trace 
09/14/2011   cab   Initial version 

============================================================================*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include "DalDevice.h"

#include "comdef.h"
#include "HALhwio.h"
#include "diag.h"
#include "msgcfg.h"
#include "slpc.h"


/*============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typedefs,
and other items needed by this module.

============================================================================*/

/* Macro for passing logging data to logging function */
#define TRACE(id, event, a64, b64, c64) \
  slpc_trace_event(id, SCT_ ## event, \
    (uint64)(a64), (uint64)(b64), (uint64)(c64))
#define TRACE_RLE(id, event, a64, b64) \
  slpc_trace_event_rle(id, SCT_ ## event, \
    (uint64)(a64), (uint64)(b64) )

/* Number of events in circular buffer */
#define NUM_TRACE_EVENTS          128

/*============================================================================

                             DEBUG MESSAGE MACROS

============================================================================*/

#define DBG_0( LVL, FMT ) \
        MSG( MSG_SSID_SLEEP, MSG_LEGACY_ ## LVL, FMT)

#define DBG_1( LVL, FMT, A ) \
        MSG_1( MSG_SSID_SLEEP, MSG_LEGACY_ ## LVL, FMT, A)

#define DBG_2( LVL, FMT, A, B ) \
        MSG_2( MSG_SSID_SLEEP, MSG_LEGACY_ ## LVL, FMT, A, B)

#define DBG_3( LVL, FMT, A, B, C ) \
        MSG_3( MSG_SSID_SLEEP, MSG_LEGACY_ ## LVL, FMT, A, B, C)

#define DBG_4( LVL, FMT, A, B, C, D ) \
        MSG_4( MSG_SSID_SLEEP, MSG_LEGACY_ ## LVL, FMT, A, B, C, D)

/* sleepctl trace event types */
typedef enum
{
  SCT_NULL,
  SCT_WAKEUP_START,
  SCT_WAKEUP_END,
  SCT_OLS_START,
  SCT_OLS_END,
  SCT_START,
  SCT_UPDATE_WAKEUP,
  SCT_SET_WARMUP,
  SCT_SET_DURATION,
  SCT_TSTMR_TO_USTMR,
  SCT_GET_SYSCLK_COUNT,
  SCT_GET_WAKEUP_TICK,
  SCT_GET_OLS_TICK,
  SCT_COMPLETE_WAKEUP,
  SCT_ABORT,
  SCT_ERR_FEEDBACK,
  SCT_READ_SYNC_ERR,
  SCT_ALT_ID_SET,
  SCT_WAKEUP_TIMER_CB,
  SCT_VSTMR_SLAM,
  SCT_VSTMR_SLAM_CALC,
  SCT_VSMTR_READ,
  SCT_SET_PRI
}
slpc_trace_event_enum;

/* record for each event */
typedef struct
{
  slpc_id_type           id;
  slpc_trace_event_enum  event;
  uint64                 a;
  uint64                 b;
  uint64                 c;
  uint64                 ts;
}
slpc_trace_event_type;


/* SleepCtl trace structure type */
typedef struct
{
  /* Last location to written to in circular "events" buffer. */
  unsigned                        last;

  /* Circular "events" buffer */
  slpc_trace_event_type           events[ NUM_TRACE_EVENTS ];
}
slpc_trace_type;

/* sleep controller trace log storage */
extern slpc_trace_type slpc_trace;

/******************************************************************************
  @brief SLPC_TRACE_EVENT
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none. 
******************************************************************************/
void slpc_trace_event( slpc_id_type id, slpc_trace_event_enum event,
                       uint64 a64, uint64 b64, uint64 c64 );

/******************************************************************************
  @brief SLPC_TRACE_EVENT_RLE
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none. 
******************************************************************************/
void slpc_trace_event_rle( slpc_id_type id, slpc_trace_event_enum event,
                           uint64 a64, uint64 b64 );

/******************************************************************************
  @brief SLPC_TRACE_INIT
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none. 
******************************************************************************/
void slpc_trace_init( void );

#endif /* !SLPCI_H */



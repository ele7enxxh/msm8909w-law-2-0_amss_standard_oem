#ifndef ATS_LOG_V_H
#define ATS_LOG_V_H
/*=============================================================================

       A T S Private Header File

GENERAL DESCRIPTION
  Common internal defines, xx_fmtuctures and functions

REGIONAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2014 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/ats_log_v.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/22/13    ab     File created.

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "timer.h"

/*=============================================================================

                           TYPES DEFS

=============================================================================*/
/* Type of timer events to log */
typedef enum
{
  TIMER_TRACE_ISR,
  TIMER_TRACE_PROCESS_START,
  TIMER_TRACE_PROCESS_END,
  TIMER_TRACE_MATCH_VALUE_PROG,
} 
timer_trace_event_type;

/*This enum will be used to specify the event
 responsible for setting the match value*/
typedef enum
{
  TIMER_MVS_TIMER_PROCESSED = 0x1,
  TIMER_MVS_TIMER_SET,
  TIMER_MVS_DEFER_MATCH_INT,
  TIMER_MVS_UNDEFER_MATCH_INT,
  TIMER_MVS_GROUP_DISABLED,
  TIMER_MVS_GROUP_ENABLED,
  TIMER_MVS_TIMER_CLEARED,
  TIMER_MVS_TIMER_PAUSED,
  TIMER_MVS_TIMER_RESUMED,
  TIMER_MVS_TIMER_CLEAR_TASK_TIMERS,
  TIMER_MVS_SET_WAKEUP,
}
timer_match_interrupt_setter_type;

typedef enum
{
  TIMER_API_LOG_TIMER_INIT = 0x1,
  TIMER_API_LOG_TIMER_DEF,
  TIMER_API_LOG_TIMER_DEF2,
  TIMER_API_LOG_TIMER_SET,
  TIMER_API_LOG_TIMER_GET,
  TIMER_API_LOG_TIMER_CLR,
  TIMER_API_LOG_TIMER_CLR_64,
  TIMER_API_LOG_TIMER_STOP,
  TIMER_API_LOG_TIMER_UNDEF,
  TIMER_API_LOG_TIMER_CLR_TASK_TIMERS,
  TIMER_API_LOG_TIMER_GROUP_ENABLE,
  TIMER_API_LOG_TIMER_GROUP_DISABLE,
  TIMER_API_LOG_TIMER_GROUP_SET_DEFERRABLE,
  TIMER_API_LOG_TIMER_GET_FIRST_NON_DEFERRABLE_TIMER_EXPIRY,
  TIMER_API_LOG_TIMER_GET_TIME_TILL_FIRST_NON_DEFERRABLE_TIMER,
  TIMER_API_LOG_TIMER_DEFER_MATCH_INTERRUPT,
  TIMER_API_LOG_TIMER_UNDEFER_MATCH_INTERRUPT,
  TIMER_API_LOG_TIMER_RESTART,
  TIMER_API_LOG_TIMER_REG,
  TIMER_API_LOG_TIMER_GET_SCLK_TILL_EXPIRY,
  TIMER_API_LOG_TIMER_SET_WAKEUP,
  TIMER_API_LOG_TIMER_GET_QURT_SIG_REGISTRATION_ID,
  TIMER_API_LOG_TIMER_INIT_QURT_SIGNAL,
  TIMER_API_LOG_TIMER_UPDATE_ISR_TIMER,
  TIMER_API_LOG_TIMER_EXPIRES_AT,
  TIMER_API_LOG_TIMER_GET_START,
  TIMER_API_LOG_TIMER_PAUSE,
  TIMER_API_LOG_TIMER_RESUME,
  TIMER_API_LOG_TIMER_IS_ACTIVE
} timer_api_type;

/*=============================================================================

                           APIS

=============================================================================*/
void timer_dbg_log_init( void );

void timer_dbg_log_match_val( time_timetick_type                 match_value_supplied, 
                                     timer_ptr_type                     timer_address, 
                                     time_timetick_type                 timer_now_supplied,
                                     timer_match_interrupt_setter_type  mv_setter );

void timer_dbg_log_timer_set( timer_ptr_type          timer_to_be_set,
                                     timer_ptr_internal_type int_timer,
                                     time_timetick_type      ts,
                                     time_timetick_type      expiry,
                                     rex_tcb_type*           tcb_ptr );

void timer_dbg_log_timer_clr( timer_ptr_type          ext_timer, 
                                     timer_ptr_internal_type int_timer,
                                     time_timetick_type      ts,
                                     timer_error_type        ret_val,
                                     timer_state_struct_type timer_state,
                                     rex_tcb_type*           tcb_ptr );

void timer_dbg_log_timer_exp( uint32                  whileloopcnt,
                                     timer_ptr_type          timer_exp_ptr,
                                     timer_ptr_internal_type int_timer,
                                     time_timetick_type      ts,
                                     time_timetick_type      expiry,
                                     time_timetick_type      start,
                                     uint32                  tcb_or_func,
                                     uint8                   slave_assigned,
                                     uint8                   callback_type );

void timer_dbg_log_multi_not( rex_tcb_type*   tcb_ptr,
                                     timer_ptr_type  timer_ptr );

void timer_dbg_log_set_param(  timer_ptr_type           timer,
                                      time_timetick_type       ts,
                                      timer_state_struct_type  timer_state,
                                      timer_notify_type        notification );

void timer_dbg_log_trace_event( timer_trace_event_type event,
                                       uint64                 ts );

void timer_dbg_log_trace_mv_event( timer_trace_event_type  event,
                                          uint64                  ts,
                                          uint64                  mv );

void timer_dbg_log_api_log( timer_api_type           event, 
                                   timer_ptr_type           timer,  
                                   time_timetick_type       timetick,
                                   timer_ptr_internal_type  int_timer );


void timer_dbg_log_timer_isolation( time_timetick_type  processing_started,
                                           time_timetick_type  processing_time,
                                           uint64              data,
                                           timer_ptr_type      timer_expired,
                                           uint32              fn_address,
                                           uint8               call_back_type );


void timer_dbg_log_timer_defer_undefer( time_timetick_type                ts,
                                               int                               reference_count,
                                               timer_match_interrupt_setter_type function_called,
                                               time_timetick_type                undefer_entry_ts );

/* Macros to perform logging */
#ifdef FEATURE_TIMER_TRACE
  #define TIMER_TRACE_TS(event,ts)        timer_dbg_log_trace_event(TIMER_TRACE_##event, ts)
  #define TIMER_TRACE_MV_TS(event,ts,mv)  timer_dbg_log_trace_mv_event(TIMER_TRACE_##event, ts, mv)
#else
  #define TIMER_TRACE_TS(event,ts)
  #define TIMER_TRACE_MV_TS(event,ts,mv)
#endif

#ifdef FEATURE_TIMER_API_LOG
  #define TIMER_API_LOG(api,timer,timetick,int_timer) timer_dbg_log_api_log(api,timer,timetick,int_timer)
#else
  #define TIMER_API_LOG(api,timer,timetick,int_timer)  
#endif /* FEATURE_TIMER_API_LOG */


#endif /* ATS_LOG_V_H */


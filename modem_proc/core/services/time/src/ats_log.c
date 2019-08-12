/*=============================================================================

                  T I M E R   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION


REGIONAL FUNCTIONS 


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS



Copyright (c) 2014 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/ats_log.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/30/14   cpaulo  Initial revision.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "ats_log_v.h"

#define TIMER_INC_MOD(x,s)         ( x = ( x + 1 ) & ( s - 1 ) )


/**
 * Timer Debug Variable Sizes 
 * Note: These must be a power of 2! 
 */
#define MAX_MATCH_REC                 64
#define MAX_TIMER_SET                 128
#define MAX_TIMER_CLR_LOG             128
#define MAX_TIMER_EXPIRED_DEBUG       256
#define MAX_TIMER_MULTI_NOTIFY        16
#define MAX_TIMER_SET_PARAMS_LOG      16
#define MAX_TIMER_TRACE               256
#define MAX_TIMER_API_CALLS           16
#define MAX_TIMER_ISOLATED            64
#define MAX_TIMER_ASSERT_COMPARE_LIST 16
#define MAX_TIMER_REFERNCE_COUNT_LOG  8


typedef struct timer_debug_var_1
{
  uint64                            timer_now_supplied;
  uint64                            match_value_supplied;
  timer_ptr_type                    timer_address;
  timer_match_interrupt_setter_type mv_setter;  
}
timer_match_value_record_type;

typedef struct timer_debug_var_2
{
  time_timetick_type      ts;
  timer_ptr_type          timer_to_be_set;
  timer_ptr_internal_type int_timer;
  time_timetick_type      expiry;
  rex_tcb_type*           tcb_ptr;
}
timer_set_type;

typedef struct timer_clr_dbg_var
{
  time_timetick_type      ts;
  timer_ptr_type          ext_timer;
  timer_ptr_internal_type int_timer;
  timer_error_type        ret_val;
  timer_state_struct_type timer_state;
  rex_tcb_type*           tcb_ptr;
}
timer_clr_log_type;

typedef union
{
  rex_tcb_type*     tcb;
  timer_t1_cb_type  func1;
  timer_t2_cb_type  func2;
}
timer_notify_log_type;

typedef struct timer_expired_debug_struct
{
  time_timetick_type      ts;
  uint8                   whileloopcnt;
  timer_ptr_type          timer_exp_ptr;
  timer_ptr_internal_type int_timer;
  time_timetick_type      expiry;
  time_timetick_type      start;
  timer_notify_log_type   tcb_or_func;
  uint8                   slave_assigned;
  uint8                   callback_type;
}
timer_expired_debug_type;

/* Log of timers having multiple notifications */
typedef struct 
{
  rex_tcb_type*   tcb_ptr;
  timer_ptr_type  timer_ptr;
}
timer_multi_notify_type;

/* Log for the API timer_set_params */

typedef struct 
{
  time_timetick_type      ts;
  timer_ptr_type          timer;
  timer_state_struct_type timer_state;
  timer_notify_type       notification;
} 
timer_set_params_log_type;

/* A single timer event trace */
typedef struct
{
  timer_ptr_type          timer;
  rex_tcb_type            *tcb;
}
timer_trace_data_timer_data_type;

typedef union
{
  time_timetick_type                match_value_programmed;
  timer_trace_data_timer_data_type  timer_data;
}
timer_trace_data_union_type;

typedef struct
{
  time_timetick_type          ts;
  timer_trace_event_type      event;
  timer_trace_data_union_type data;
} 
timer_trace_data_type;


/* Structure of all timer log events */
typedef struct
{
  uint32                    index;
  timer_trace_data_type     events[MAX_TIMER_TRACE];
} timer_trace_type;


typedef struct
{
  /* Time Stamp */
  time_timetick_type      ts;
  /* API type that was called */
  timer_api_type          api;
  /* Pointer to TCB */
  rex_tcb_type*           tcb;
  /* Timer Pointer */
  timer_ptr_type          timer;
  /* Internal timer pointer */
  timer_ptr_internal_type int_timer;
} 
timer_api_log_type;

typedef struct timer_debug_var_4
{
  time_timetick_type  processing_started;
  time_timetick_type  processing_time;
  uint64              data;
  timer_ptr_type      timer_expired;
  uint32              thresh_exceed_count;
  uint32              fn_address;
  uint8               call_back_type;  
}
timer_isolate_type;

#ifdef FEATURE_DEBUG_OFFENDING_TIMERS_ISOLATION
uint32 timers_cnt_assert_threshold = 10;
int timer_isolated_cnt_global = 0;
uint32 timer_assert_compare_value_count;
uint32 timer_assert_compare_value_list[MAX_TIMER_ASSERT_COMPARE_LIST];
#endif /* FEATURE_DEBUG_OFFENDING_TIMERS_ISOLATION */

typedef struct 
{
  time_timetick_type                ts;
  int                               reference_count;
  timer_match_interrupt_setter_type function_called;
  time_timetick_type                undefer_entry_ts;
}
timer_defer_undefer_reference_count_log_type;

/* Definitions of logs for backwards compatibility */
#ifdef FEATURE_LOG_RECORD_MATCH_VAL
timer_match_value_record_type                record_match_val[MAX_MATCH_REC];
int                                          record_match_val_counter = 0;
#endif /*FEATURE_LOG_RECORD_MATCH_VAL*/

#ifdef FEATURE_LOG_TIMER_SETS
timer_set_type                               timer_sets[MAX_TIMER_SET];
int                                          timer_set_cnt = 0;
#endif /*FEATURE_LOG_TIMER_SETS*/

#ifdef FEATURE_LOG_TIMER_CLR_LOG
timer_clr_log_type                           timer_clr_log[MAX_TIMER_CLR_LOG];
uint32                                       timer_clr_log_cnt = 0;
#endif /*FEATURE_LOG_TIMER_CLR_LOG*/

#ifdef FEATURE_LOG_TIMER_EXPIRED_DEBUG
timer_expired_debug_type                     timer_expired_debug[MAX_TIMER_EXPIRED_DEBUG];
uint32                                       timer_expired_debug_cnt = 0;
#endif /*FEATURE_LOG_TIMER_EXPIRED_DEBUG*/

#ifdef FEATURE_LOG_TIMER_MULTI_NOTIFY
timer_multi_notify_type                      timer_multi_notify[MAX_TIMER_MULTI_NOTIFY];
uint32                                       timer_multi_notify_cnt = 0;
#endif /*FEATURE_LOG_TIMER_MULTI_NOTIFY*/

#ifdef FEATURE_LOG_TIMER_SET_PARAMS_LOG
timer_set_params_log_type                    timer_set_params_log[MAX_TIMER_SET_PARAMS_LOG];
uint32                                       timer_set_params_log_cnt = 0;
#endif /*FEATURE_LOG_TIMER_SET_PARAMS_LOG*/

#ifdef FEATURE_TIMER_TRACE
timer_trace_type                             timer_trace;
#endif /*FEATURE_TIMER_TRACE*/

#ifdef FEATURE_TIMER_API_LOG
timer_api_log_type                           timer_api_calls[MAX_TIMER_API_CALLS];
int                                          timer_api_log_cnt = 0;
#endif /*FEATURE_TIMER_API_LOG*/

#ifdef FEATURE_DEBUG_OFFENDING_TIMERS_ISOLATION
timer_isolate_type                           timers_isolated[MAX_TIMER_ISOLATED];
int                                          timer_isolated_cnt = 0;
#endif /*FEATURE_DEBUG_OFFENDING_TIMERS_ISOLATION*/

#ifdef FEATURE_LOG_TIMER_DEFER_UNDEFER
timer_defer_undefer_reference_count_log_type timer_defer_undefer_reference_count_log[MAX_TIMER_REFERNCE_COUNT_LOG];
int                                          timer_defer_undefer_reference_count_log_cnt = 0;
#endif /*FEATURE_LOG_TIMER_DEFER_UNDEFER*/

void timer_dbg_log_match_val( time_timetick_type      match_value_supplied, 
                                     timer_ptr_type          timer_address, 
                                     time_timetick_type      timer_now_supplied,
                                     uint8                   mv_setter )
{
  #ifdef FEATURE_LOG_RECORD_MATCH_VAL

  record_match_val[record_match_val_counter].match_value_supplied = match_value_supplied;
  record_match_val[record_match_val_counter].timer_address = timer_address;
  record_match_val[record_match_val_counter].timer_now_supplied = timer_now_supplied;
  record_match_val[record_match_val_counter].mv_setter = mv_setter;
  TIMER_INC_MOD(record_match_val_counter, MAX_MATCH_REC);
  /* This is a dummy statement to remove compiler warning */
  record_match_val[record_match_val_counter].timer_now_supplied = \
    record_match_val[record_match_val_counter].timer_now_supplied;

  #endif /*FEATURE_LOG_RECORD_MATCH_VAL*/
}


void timer_dbg_log_timer_set( timer_ptr_type          timer_to_be_set,
                                     timer_ptr_internal_type int_timer,
                                     time_timetick_type      ts,
                                     time_timetick_type      expiry,
                                     rex_tcb_type*           tcb_ptr )
{
  #ifdef FEATURE_LOG_TIMER_SETS

  timer_sets[timer_set_cnt].timer_to_be_set = timer_to_be_set;
  timer_sets[timer_set_cnt].int_timer = int_timer;
  timer_sets[timer_set_cnt].tcb_ptr = (rex_tcb_type *)tcb_ptr;
  timer_sets[timer_set_cnt].ts = ts;
  timer_sets[timer_set_cnt].expiry = expiry;
  TIMER_INC_MOD(timer_set_cnt, MAX_TIMER_SET);
  
  /* This is a dummy statement to remove compiler warnings */
  timer_sets[timer_set_cnt].ts = \
    timer_sets[timer_set_cnt].ts;

  #endif /*FEATURE_LOG_TIMER_SETS*/
}


void timer_dbg_log_timer_clr( timer_ptr_type          ext_timer, 
                                     timer_ptr_internal_type int_timer,
                                     time_timetick_type      ts,
                                     timer_error_type        ret_val,
                                     timer_state_struct_type timer_state,
                                     rex_tcb_type*           tcb_ptr )
{
  #ifdef FEATURE_LOG_TIMER_CLR_LOG

  timer_clr_log[timer_clr_log_cnt].ext_timer = ext_timer;
  timer_clr_log[timer_clr_log_cnt].int_timer = int_timer;
  timer_clr_log[timer_clr_log_cnt].ts = ts;
  timer_clr_log[timer_clr_log_cnt].timer_state = timer_state;
  timer_clr_log[timer_clr_log_cnt].tcb_ptr = (rex_tcb_type *)tcb_ptr;
  timer_clr_log[timer_clr_log_cnt].ret_val = ret_val;
  TIMER_INC_MOD(timer_clr_log_cnt, MAX_TIMER_CLR_LOG);

  #endif /*FEATURE_LOG_TIMER_CLR_LOG*/
}

void timer_dbg_log_timer_exp( uint32                  whileloopcnt,
                                     timer_ptr_type          timer_exp_ptr,
                                     timer_ptr_internal_type int_timer,
                                     time_timetick_type      ts,
                                     time_timetick_type      expiry,
                                     time_timetick_type      start,
                                     uint32                  tcb_or_func,
                                     uint8                   slave_assigned,
                                     uint8                   callback_type )
{
  #ifdef FEATURE_LOG_TIMER_EXPIRED_DEBUG

  timer_expired_debug[timer_expired_debug_cnt].whileloopcnt = whileloopcnt;
  timer_expired_debug[timer_expired_debug_cnt].timer_exp_ptr = timer_exp_ptr;
	timer_expired_debug[timer_expired_debug_cnt].int_timer = int_timer;
  timer_expired_debug[timer_expired_debug_cnt].ts = ts;
  timer_expired_debug[timer_expired_debug_cnt].start = start;
  timer_expired_debug[timer_expired_debug_cnt].expiry = expiry;
  timer_expired_debug[timer_expired_debug_cnt].callback_type = callback_type;
  timer_expired_debug[timer_expired_debug_cnt].tcb_or_func.func1 = (timer_t1_cb_type)tcb_or_func;
  timer_expired_debug[timer_expired_debug_cnt].slave_assigned = slave_assigned;
  TIMER_INC_MOD(timer_expired_debug_cnt, MAX_TIMER_EXPIRED_DEBUG);

  #endif /*FEATURE_LOG_TIMER_EXPIRED_DEBUG*/
}


void timer_dbg_log_multi_not( rex_tcb_type*   tcb_ptr,
                                     timer_ptr_type  timer_ptr )
{
  #ifdef FEATURE_LOG_TIMER_MULTI_NOTIFY

  timer_multi_notify[timer_multi_notify_cnt].tcb_ptr   = tcb_ptr;
	timer_multi_notify[timer_multi_notify_cnt].timer_ptr = timer_ptr;
  TIMER_INC_MOD(timer_multi_notify_cnt, MAX_TIMER_MULTI_NOTIFY);

  #endif /*FEATURE_LOG_TIMER_MULTI_NOTIFY*/
}

void timer_dbg_log_set_param ( timer_ptr_type           timer,
                                      time_timetick_type       ts,
                                      timer_state_struct_type  timer_state,
                                      timer_notify_type        notification )
{
  #ifdef FEATURE_LOG_TIMER_SET_PARAMS_LOG

  timer_set_params_log[timer_set_params_log_cnt].timer = timer;
  timer_set_params_log[timer_set_params_log_cnt].ts = ts;
  timer_set_params_log[timer_set_params_log_cnt].timer_state = timer_state;
  timer_set_params_log[timer_set_params_log_cnt].notification = notification;
  TIMER_INC_MOD(timer_set_params_log_cnt, MAX_TIMER_SET_PARAMS_LOG);

  #endif /*FEATURE_LOG_TIMER_SET_PARAMS_LOG*/
}


#ifdef FEATURE_TIMER_TRACE
/*=============================================================================

FUNCTION TIMER_TRACE_EVENT

DESCRIPTION
  This function saved a timer trace event packet.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/

void timer_dbg_log_trace_event( timer_trace_event_type  event,
                                       uint64                  ts )
{
  /* Pointer to the trace structure to fill in */
  timer_trace_data_type *trace;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the next trace structure */
  trace = &timer_trace.events[timer_trace.index];

  /* Go to the next index */
  TIMER_INC_MOD( timer_trace.index, MAX_TIMER_TRACE );

  /* Fill in the common fields */
  trace->event = event;

  /* Use a passed in timestamp */
  trace->ts = ts;

  /* No data required */
  trace->data.match_value_programmed = 0;

} /* timer_trace_event */


void timer_dbg_log_trace_mv_event( timer_trace_event_type  event,
                                          uint64                  ts,
                                          uint64                  mv )
{
  /* Pointer to the trace structure to fill in */
  timer_trace_data_type *trace;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the next trace structure */
  trace = &timer_trace.events[timer_trace.index];

  /* Go to the next index */
  TIMER_INC_MOD( timer_trace.index, MAX_TIMER_TRACE );

  /* Fill in the common fields */
  trace->event = event;

  /* Use a passed in timestamp */
  trace->ts = ts;

  /* Update match val */
  trace->data.match_value_programmed = mv;

} /* timer_trace_mv_event */

#endif /* FEATURE_TIMER_TRACE */


/*=============================================================================

FUNCTION TIMER_API_LOG

DESCRIPTION
  This function logs all of the api calls along with
  the thread ID.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/

void timer_dbg_log_api_log ( timer_api_type          event,
                                    timer_ptr_type          timer,
                                    time_timetick_type      timetick,
                                    timer_ptr_internal_type int_timer )
{
  #ifdef FEATURE_TIMER_API_LOG

  /* Record Event Type */
  timer_api_calls[timer_api_log_cnt].api = event;

  /* Record ptr to TCB */
  timer_api_calls[timer_api_log_cnt].tcb = rex_self();

  /* Record timetick value */
  timer_api_calls[timer_api_log_cnt].ts = timetick;
  
  /* Record ptr to timer */
  timer_api_calls[timer_api_log_cnt].timer = timer;

  /* Record ptr to internal timer */
  timer_api_calls[timer_api_log_cnt].int_timer = int_timer;
  
  TIMER_INC_MOD(timer_api_log_cnt, MAX_TIMER_API_CALLS);

  #endif /* FEATURE_TIMER_API_LOG */
} /* timer_api_log */


void timer_dbg_log_timer_isolation( time_timetick_type  processing_started,
                                           time_timetick_type  processing_time,
                                           uint64              data,
                                           timer_ptr_type      timer_expired,
                                           uint32              fn_address,
                                           uint8               call_back_type )
{
  #ifdef FEATURE_DEBUG_OFFENDING_TIMERS_ISOLATION
  uint32 count  = 0;
  uint32 index  = 0;

  /*first check whether the timer is already present in 
  the isolated timer list*/
  for(count = 0; count < timer_isolated_cnt; count++)
  {
    /*if the timer expired, callback fn and data is already present, then do not add
     to isolation list
     may be only first check is needed and remove the other .data and ,fn_address in formal check in */
    if((timers_isolated[count].timer_expired == timer_expired)
        && (timers_isolated[count].data == data)
        && (timers_isolated[count].fn_address == fn_address))
    {
      timers_isolated[count].thresh_exceed_count++;

      /*if we found the timer that we were looking for, make sure that we do not add
       an entry in case the latest processing time is lesser than the one previously 
       recorded.*/
      if(timers_isolated[count].processing_time < processing_time)
      {              
        timers_isolated[count].processing_started = processing_started;
        timers_isolated[count].processing_time = processing_time;
      }
      break;
    }
  }

  /*if we did not break prematurely above, it means we did not find the timer 
   in the isolated list. Therefore add it*/
  if(count == timer_isolated_cnt)
  {
    timers_isolated[count].thresh_exceed_count++;
    timers_isolated[count].timer_expired      = timer_expired;      
    timers_isolated[count].processing_started = processing_started;
    timers_isolated[count].processing_time    = processing_time;
    timers_isolated[count].fn_address         = fn_address;
    timers_isolated[count].data               = data;
    timers_isolated[count].call_back_type     = call_back_type;

    /*increment the count*/
    timer_isolated_cnt++;
  }

   /*if the isolated timer count has reached the max or threshold, then 
    error fatal out*/
   if((MAX_TIMER_ISOLATED <= timer_isolated_cnt)
       || timers_cnt_assert_threshold == timer_isolated_cnt)
   {
     ATS_MSG_ERROR(" # of timers exceeding warning count = 0x%x, threshold = 0x%x, ticks_threshold = 0x%x",
                timer_isolated_cnt, timers_cnt_assert_threshold,timer_processing_time_warning_threshold);

     /*isolated timer count wrap around. This is done to make the for loop for isolating the timers 
      managable*/
     timer_isolated_cnt = 0;

   }

   /*if the isolated timer is found in the timer_assert_compare_value_list, then 
    assert*/
   for (index = 0; index < timer_assert_compare_value_count; index++)
   {
     if ( ((uint32)(timers_isolated[count].timer_expired)) == timer_assert_compare_value_list[index]);
     {
       ERR_FATAL("timers_isolated[%d].timer_expired == timer_assert_compare_value_list[%d]",count,index,0);
     }
   }   
   #endif /*FEATURE_DEBUG_OFFENDING_TIMERS_ISOLATION*/
}


void timer_dbg_log_timer_defer_undefer( time_timetick_type                ts,
                                               int                               reference_count,
                                               timer_match_interrupt_setter_type function_called,
                                               time_timetick_type                undefer_entry_ts )
{
  #ifdef FEATURE_LOG_TIMER_DEFER_UNDEFER
  timer_defer_undefer_reference_count_log[timer_defer_undefer_reference_count_log_cnt].ts               = ts;
  timer_defer_undefer_reference_count_log[timer_defer_undefer_reference_count_log_cnt].undefer_entry_ts = undefer_entry_ts;
  timer_defer_undefer_reference_count_log[timer_defer_undefer_reference_count_log_cnt].reference_count  = reference_count;
  timer_defer_undefer_reference_count_log[timer_defer_undefer_reference_count_log_cnt].function_called  = function_called;

  TIMER_INC_MOD(timer_defer_undefer_reference_count_log_cnt, MAX_TIMER_REFERNCE_COUNT_LOG);
  #endif /*FEATURE_LOG_TIMER_DEFER_UNDEFER*/

}

/*=============================================================================
  Copyright (c) 2003-2013 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/**
@file timer_slaves.c
@brief
Implements timer slave tasks

Traditionally, timer task has been used to call the calllback 
functions associated with the timer. This has often led to 
abusive callback holding timer task at ransom, affecting timer 
accuracy. By implementing these slave tasks, the callbacks can 
be offloaded to these slave tasks, and hence timer task is free 
to services other expiring timers. 

In this file, there is support for three slave tasks, which can 
be enabled by defining FEATURE_TIMER_SLAVE1, 
FEATURE_TIMER_SLAVE2 and FEATURE_TIMER_SLAVE3 respectively. 
These defines can be made in <product>.builds file.

INITIALIZATION AND SEQUENCING REQUIREMENTS

timer_slave_cmd_queue_init() should be called before timer_init()

The slave tasks should be started before the timer task in tmc. 
The reason is that there might be expired timers needed to be serviced
when timer task starts. This would require that timer slave task(s) 
are already started to process these timer callbacks.* 
*/ 

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/timer_debug.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/02/2012 rks     Removed False Dependency to TMC.H in CoreBSP-Services
10/04/11   gparikh File created.

=============================================================================*/

#include "DDITimetick.h"
#include "timer.h"
#include "ats_v.h"
#include "err.h"
#include "assert.h"
#include "rex.h"
#include "stdlib.h"
#include "dog.h"
/*defines here*/

#define TIMER_DEBUG_SIGNAL       0x1
#define TIMER_DEBUG_TIME_SET     2500
#define TIMER_DEBUG_MAX_LATENCY  TIMER_DEBUG_TIME_SET+10 

/*end defines*/

#define CLK_FREQ_TIME 19200000
#define CLK_FREQ_TIME_DIV 19200
typedef DalTimetickTime64Type debug_tt_type;

static rex_tcb_type * timer_dbg_task_tcb;
static DalDeviceHandle * hTimetick = NULL;


/*function prototypes here*/

void timer_dbg_task
(
  void
);

void timer_dbg_callback
(
  void
);

void timer_dbg_callback_hard
(
  void
);

static debug_tt_type debug_tt_get
(
  void
);

static void debug_tt_init
(
  void
);

static uint32 debug_tt_calc_diff_ms
(
  debug_tt_type last, 
  debug_tt_type first
);

static void timer_dbg_dog_cb(int32 time_ms, timer_cb_data_type userData );
/*end function prototypes*/

/** 

@ingroup Timer Debug Functions

@brief 

Initializes debug task 

@return

None

@param[in]                                                               
None

@param[out]
None

@dependencies  
None

@sideeffects 
Should be started after timer task
*/
void timer_debug_task_init
(
  void
)
{
  RCINIT_INFO handle = rcinit_lookup("timer_dbg");

  if (RCINIT_NULL != handle)
  {
    rcinit_initfn_spawn_task(handle, (void *)timer_dbg_task);
  }
}

static debug_tt_type debug_tt_get(void)
{
  debug_tt_type ttRet = -1;
  ASSERT(hTimetick != NULL);
  DalTimetick_GetTimetick64(hTimetick, &ttRet);
  ASSERT(ttRet != -1 && ttRet != 0);
  return (debug_tt_type)ttRet;
}

static uint32 debug_tt_calc_diff_ms(debug_tt_type last, debug_tt_type first)
{
  return (uint32)((last - first)*1000/CLK_FREQ_TIME);
}

static void debug_tt_init(void)
{
  DalTimetick_Attach("SystemTimer", &hTimetick);
  ASSERT(hTimetick != NULL);
}

void timer_dbg_task(void)
{
  uint32                    uiDogID,uiDogTime,timer_dbg_late_cnt = 0;
  uint32                    time_start = 0;
  uint32                    time_end = 0;
  uint32                    iter = 0;

  timer_ptr_type            pTimer;
  timer_ptr_type            pTimerHard;
  timer_ptr_type            pTimerDog;

  rcinit_handshake_startup();

  timer_dbg_task_tcb = rex_self();
  debug_tt_init();

  /* allocate memory for dog timer */
  pTimerDog = (timer_type *)malloc(sizeof(timer_type));
  if (NULL != pTimerDog)
  {
    /* Initialize the watchdog report timer */
    uiDogID = dog_register(rex_self(),
                           DOG_DEFAULT_TIMEOUT, 
                           DOG_DEADLKCHK_ENABLE);

    /* get the dog timerout value */
    uiDogTime = dog_get_report_period( uiDogID );

    /* define and set up repeat dog timer to kick the dog */
    timer_def2(pTimerDog, NULL);
    timer_reg(pTimerDog,
              (timer_t2_cb_type)timer_dbg_dog_cb,
              (timer_cb_data_type)uiDogID, 
              uiDogTime, 
              uiDogTime);

    /* allocate memory for the timer */
    pTimer = (timer_ptr_type)malloc(sizeof (timer_type));
    pTimerHard = (timer_ptr_type)malloc(sizeof (timer_type));

    if (NULL != pTimer)
    {
      /* define the timer of type 1 */
      timer_def2(pTimer,NULL);
      timer_def(pTimerHard,NULL,NULL,0,(timer_t1_cb_type)timer_dbg_callback_hard,0);

      /* clear the signal TIMER_TEST_SIGNAL1 */
      rex_clr_sigs(rex_self(), TIMER_DEBUG_SIGNAL);
              
      for( ; ; )
      {
        time_start = debug_tt_get();
        
        if(iter%2)
        {
          /* set the timer */
          timer_reg(pTimer,
                    (timer_t2_cb_type)timer_dbg_callback, 
                    NULL, 
                    TIMER_DEBUG_TIME_SET, 
                    0);
        }
        else{
          timer_set_absolute(pTimerHard, debug_tt_get() + (time_timetick_type)(TIMER_DEBUG_TIME_SET*CLK_FREQ_TIME_DIV));
        }

        /* wait for the signal set by the timer */
        rex_wait(TIMER_DEBUG_SIGNAL);
        rex_clr_sigs(rex_self(), TIMER_DEBUG_SIGNAL);

        /* Get the ending uptime for calculating how much time it take since */
        /* the timer started.                                                */
        time_end = debug_tt_get();


        /* check if this pTimer is fired on time */
        if ( debug_tt_calc_diff_ms(time_end,time_start) > TIMER_DEBUG_MAX_LATENCY)
        {
          ATS_ERR("Late timer found, timer found");
          timer_dbg_late_cnt++;
        }            

        iter++;
      } /* end while */

      /* done with the timer, free its memory */
      free(pTimer);
      free(pTimerHard);

    } /* end if timer pTimer allocated */

    /* dereg the dog report so dog won't bark on us after thread ends */
    dog_deregister(uiDogID);

    /* done with dog timer.  clear and free memory */
    timer_clr(pTimerDog, T_MSEC);
    free(pTimerDog);

  } /* end if timer pTimerDog allocated */
}

void timer_dbg_callback(void)
{
  rex_set_sigs( timer_dbg_task_tcb , TIMER_DEBUG_SIGNAL);
}

void timer_dbg_callback_hard(void)
{
  rex_set_sigs( timer_dbg_task_tcb , TIMER_DEBUG_SIGNAL);
}

static void timer_dbg_dog_cb(int32 time_ms, timer_cb_data_type userData )
{
  uint32 uiDogID = (uint32) userData;

  dog_report(uiDogID);
}

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR Timer support

GENERAL DESCRIPTION
  Collection of functions to support use of REX timers within RR

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2014 Qualcomm Technologies, Inc.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_timer.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
31/07/12    sjw     Reworked rr_timer_extend() to avoid expiry collisions
04/04/12    sjw     Use memset() on all dynamic allocations from modem heap
10/02/12    sjw     Updates for DSDS
20/09/11    sjw     Perform context ID check in rr_timer_reset_all()
07/04/11    sjw     Merge DSDS fixes from 2H07 branches
10/02/11    sjw     Enhanced support for tuneaway from another RAT
01/11/10    sjw     New GRR timer API created
===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "rr_seg_load.h"
#include "rr_multi_sim.h"
#include "rr_timer.h"
#include "rr_defs.h"
#include "gprs_mem.h"
#include "amssassert.h"

/* theoretical max number of CONCURRENT timers used by the RR task */
#define RR_TIMER_POOL_SIZE       (NUM_GERAN_DATA_SPACES << 6)

#define RR_TIMER_INVALID_HANDLE   0xFFFFFFFFUL

#define RR_TIMER_MIN_DURATION_FOR_RESTART 3

#define RR_TIMER_GET_CID_FROM_HANDLE(hndl) ((hndl) >> RR_TIMER_CONTEXT_BIT_SHIFT)
#define RR_TIMER_GET_TID_FROM_HANDLE(hndl) ((hndl) & 0x000000FFUL)

#define RR_TIMER_GET_CONTEXT(cid) {(cid) = (rr_timer_cid_t)rr_utf_get_gas_id();}

/* these definitions are only ever used within this module */

/* A timer "handle" is a unique 32-bit integer generated from the:
 * - timer ID (tid);
 * - context ID (cid);
 * - unique ID (uid);
 */
typedef uint32 rr_timer_handle_t;
typedef uint8  rr_timer_cid_t;

typedef enum
{
  RR_TIMER_EXPIRY_NOT_POSTED,
  RR_TIMER_EXPIRY_POSTED_NOT_VALIDATED,
  RR_TIMER_EXPIRY_POSTED_AND_VALIDATED
} rr_timer_expiry_posted_status_e;

typedef struct
{
  rr_timer_handle_t                 rr_timer_hndl;
  rex_timer_type                   *rex_timer_ptr;
  rr_timer_expiry_posted_status_e   expiry_posted;
} rr_timer_t, *rr_timer_ptr_t;

/*************************** LOCAL DATA FOR THIS MODULE *********************/
static rex_crit_sect_type rr_timer_critical_section;
static rex_timer_type     rr_timer_dummy;
static rr_timer_t         rr_timer_pool[RR_TIMER_POOL_SIZE];

#define RR_EXPIRED_TIMER  (&rr_timer_dummy)
#define RR_TIMER_LOCK()   rex_enter_crit_sect(&rr_timer_critical_section)
#define RR_TIMER_UNLOCK() rex_leave_crit_sect(&rr_timer_critical_section)

/*********************** LOCAL FUNCTIONS FOR THIS MODULE ********************/

/****************************************************************************
 *   Function name:  rr_timer_generate_handle
 *   --------------
 *
 *   Description: Turns a timer ID and context ID into a RR timer handle
 *
 *   Parameters:  timer_id, context_id
 *   -----------
 *
 *   Returns:     RR timer handle (32-bit unsigned integer)
 *   --------
 *
 ****************************************************************************/
static rr_timer_handle_t rr_timer_generate_handle(rr_timer_id_T timer_id, rr_timer_cid_t context_id)
{
  rr_timer_handle_t tid = (rr_timer_handle_t)timer_id;
  rr_timer_handle_t cid = (rr_timer_handle_t)context_id;

  /* the timer ID is the lower 8-bits of the handle  */
  /* the context ID is the upper 12-bits of the handle */
  return((cid << RR_TIMER_CONTEXT_BIT_SHIFT) | (tid & 0x000000FFUL));
}

/****************************************************************************
 *   Function name:  rr_timer_find_handle
 *   --------------
 *
 *   Description: Searches the timer pool for an exact handle match
 *
 *   Parameters:  RR timer handle
 *   -----------
 *
 *   Returns:     Pointer to timer pool entry, or NULL if no handle matched
 *   --------
 *
 ****************************************************************************/
static rr_timer_ptr_t rr_timer_find_handle(rr_timer_handle_t timer_handle)
{
  rr_timer_ptr_t curr_ptr;
  int            i;

  curr_ptr = rr_timer_pool;
  for (i = 0; i < RR_TIMER_POOL_SIZE; i++, curr_ptr++)
  {
    if (curr_ptr->rr_timer_hndl == timer_handle)
    {
      return(curr_ptr);
    }
  }

  return(NULL);
}

/****************************************************************************
 *   Function name:  rr_timer_find_entry
 *   --------------
 *
 *   Description: Searches the timer pool for a matching timer & context ID
 *
 *   Parameters:  INPUTs Timer and context IDs
 *                OUTPUT Timer handle found in matching entry
 *   -----------
 *
 *   Returns:     Pointer to FIRST matching entry, or NULL
 *   --------
 *
 ****************************************************************************/
static rr_timer_ptr_t rr_timer_find_entry(rr_timer_id_T timer_id, rr_timer_cid_t context_id, rr_timer_handle_t *handle_ptr)
{
  rr_timer_handle_t timer_handle;
  rr_timer_ptr_t    curr_ptr;
  rr_timer_id_T     tid;
  rr_timer_cid_t    cid;
  int               i;

  curr_ptr = rr_timer_pool;
  for (i = 0; i < RR_TIMER_POOL_SIZE; i++, curr_ptr++)
  {
    timer_handle = curr_ptr->rr_timer_hndl;
    tid = (rr_timer_id_T)RR_TIMER_GET_TID_FROM_HANDLE(timer_handle);
    cid = (rr_timer_cid_t)RR_TIMER_GET_CID_FROM_HANDLE(timer_handle);

    if ((tid == timer_id) &&
        (cid == context_id))
    {
      /* found a matching entry */
      if (handle_ptr != NULL)
      {
        /* output the handle of the entry that was found */
        *handle_ptr = timer_handle;
      }
      return(curr_ptr);
    }
  }

  /* no matching timer ID and context found */
  if (handle_ptr != NULL)
  {
    *handle_ptr = RR_TIMER_INVALID_HANDLE;
  }

  return(NULL);
}

/****************************************************************************
 *   Function name:  rr_timer_create_entry
 *   --------------
 *
 *   Description: Creates a new timer in the first free pool entry
 *
 *   Parameters:  RR timer handle
 *   -----------
 *
 *   Returns:     Pointer to timer pool entry, or NULL if not created
 *   --------
 *
 ****************************************************************************/
static rr_timer_ptr_t rr_timer_create_entry(rr_timer_handle_t timer_handle)
{
  rr_timer_ptr_t new_ptr;

  /* find the first FREE entry in the RR timer pool */
  new_ptr = rr_timer_find_handle(RR_TIMER_INVALID_HANDLE);

  if (new_ptr != NULL)
  {
    rex_timer_type *rex_ptr;

    /* dynamically allocate a new REX timer */
    rex_ptr = (rex_timer_type *)GPRS_MEM_MALLOC(sizeof(rex_timer_type));

    if (rex_ptr != NULL)
    {
      /* initialise the memory before passing it to the timer services module */
      memset(rex_ptr, 0, sizeof(rex_timer_type));
      rex_def_timer_ex(rex_ptr, rr_timer_expiry_callback, (unsigned long)timer_handle);
      new_ptr->rr_timer_hndl = timer_handle;
      new_ptr->rex_timer_ptr = rex_ptr;
      new_ptr->expiry_posted = RR_TIMER_EXPIRY_NOT_POSTED;
    }
    else
    {
      /* failed to allocate memory, timer cannot be created */
      new_ptr = NULL;
      MSG_GERAN_ERROR_0("Unable to create REX timer");
    }
  }
  else
  {
    MSG_GERAN_ERROR_0("RR timer pool has run of out free space");
  }

  return(new_ptr);
}

/****************************************************************************
 *   Function name:  rr_timer_deallocate_entry
 *   --------------
 *
 *   Description: Performs specific actions required to free ALL resources
 *                associated with a timer, including those related to modem
 *                heap and timer API linked-list/instance management
 *
 *   Parameters:  Pointer to the RR timer structure and new pointer value
 *                to be assigned to the REX timer within it (after freeing)
 *   -----------
 *
 *   Returns:     Void
 *   --------
 *
 ****************************************************************************/
static void rr_timer_deallocate_entry(rr_timer_ptr_t rr_timer_ptr, rex_timer_type *next_rex_ptr)
{
  if (rr_timer_ptr != NULL)
  {
    /* if the timer was dynamically allocated, free it now */
    if ((rr_timer_ptr->rex_timer_ptr != NULL) &&
        (rr_timer_ptr->rex_timer_ptr != RR_EXPIRED_TIMER))
    {
      (void)rex_clr_timer(rr_timer_ptr->rex_timer_ptr);
#ifndef PERLUTF
      /* Timer Services updated here because REX veneer does NOT do it */
      (void)timer_undef(rr_timer_ptr->rex_timer_ptr);
#endif /* !PERLUTF */
      GPRS_MEM_FREE(rr_timer_ptr->rex_timer_ptr);
    }

    /* and then set the REX timer pointer as instructed */
    rr_timer_ptr->rex_timer_ptr = next_rex_ptr;
  }

  return;
}

/****************************************************************************
 *   Function name:  rr_timer_reset_entry
 *   --------------
 *
 *   Description: Resets an individual timer entry in the pool
 *
 *   Parameters:  Pointer to the timer entry
 *   -----------
 *
 *   Returns:     Void
 *   --------
 *
 ****************************************************************************/
static void rr_timer_reset_entry(rr_timer_ptr_t rr_timer_ptr)
{
  /* free all resources associated with the timer */
  rr_timer_deallocate_entry(rr_timer_ptr, NULL);

  /* and then reset the handle and expiry status */
  rr_timer_ptr->rr_timer_hndl = RR_TIMER_INVALID_HANDLE;
  rr_timer_ptr->expiry_posted = RR_TIMER_EXPIRY_NOT_POSTED;

  return;
}

/****************************************************************************
 *   Function name:  rr_timer_query_ms
 *   --------------
 *
 *   Description: Queries the number of milliseconds that a timer has left
 *
 *   Parameters:  Pointer to RR timer structure
 *   -----------
 *
 *   Returns:     Number of milliseconds left before expiry
 *   --------
 *
 ****************************************************************************/
static rex_timer_cnt_type rr_timer_query_ms(rr_timer_ptr_t rr_timer_ptr)
{
  rex_timer_cnt_type time_left = 0;

  if (rr_timer_ptr != NULL)
  {
    if ((rr_timer_ptr->rex_timer_ptr != RR_EXPIRED_TIMER) &&
        (rr_timer_ptr->expiry_posted == RR_TIMER_EXPIRY_NOT_POSTED))
    {
      time_left = rex_get_timer(rr_timer_ptr->rex_timer_ptr);
    }
  }

  return(time_left);
}

/*********************** EXPORTED FUNCTIONS FOR THIS MODULE *****************/

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

/****************************************************************************
 *   Function name:  rr_timer_expiry_callback
 *   --------------
 *
 *   Description:    This function is passed to REX as the handler for all
 *                   RR timer expiries
 *
 *   Parameters:     Callback parameter, which is a unique RR timer handle
 *   -----------
 *
 *   Returns:        VOID
 *   --------
 *
 ****************************************************************************/
void rr_timer_expiry_callback(unsigned long cb_param)
{
  rr_timer_handle_t timer_handle;
  rr_timer_ptr_t    rr_timer_ptr;
  timer_expiry_T    timer_cmd;

  RR_TIMER_LOCK();

  timer_handle = (rr_timer_handle_t)cb_param;

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

  /* scan the RR timer pool for the specified handle */
  rr_timer_ptr = rr_timer_find_handle(timer_handle);

  if (rr_timer_ptr != NULL)
  {
    if (rr_timer_ptr->expiry_posted == RR_TIMER_EXPIRY_NOT_POSTED)
    {
      /* set flag to indicate that an expiry has been posted */
      rr_timer_ptr->expiry_posted = RR_TIMER_EXPIRY_POSTED_NOT_VALIDATED;
    }
    else
    {
      /* no need to post a second expiry */
      timer_handle = RR_TIMER_INVALID_HANDLE;
    }
  }
#ifdef PERLUTF
  #error code not present
#endif /* PERLUTF */

  RR_TIMER_UNLOCK();

  if (timer_handle != RR_TIMER_INVALID_HANDLE)
  {
    timer_cmd.IMH.message_set = MS_TIMER;
    timer_cmd.IMH.message_id  = MID_TIMER_EXPIRY;
    timer_cmd.timer_id        = (timer_id_T)RR_TIMER_GET_TID_FROM_HANDLE(timer_handle);
    timer_cmd.data            = (byte)RR_TIMER_GET_CID_FROM_HANDLE(timer_handle);

    PUT_IMH_LEN(sizeof(timer_expiry_T) - sizeof(IMH_T), &(timer_cmd.IMH));

    /* post the expiration message to the main RR task queue */
    if (gs_send_message(GS_QUEUE_RR, (void *)&timer_cmd, FALSE) != GS_SUCCESS)
    {
      MSG_GERAN_ERROR_1("Failed to post timer %d expiry to RR queue",(int)timer_cmd.timer_id);
    }
  }
  else
  {
    MSG_GERAN_HIGH_1("Ignored expiry of timer 0x%02X", (int)(cb_param & 0xFF));
  }

  return;
}

/****************************************************************************
 *   Function name:  rr_timer_reset_all
 *   --------------
 *
 *   Description:    Resets all timers in the pool, ignoring those in an
 *                   exception list
 *
 *   Parameters:     List of timer IDs to ignore
 *                   Number of entries in the list
 *   -----------
 *
 *   Returns:        VOID
 *   --------
 *
 ****************************************************************************/
void rr_timer_reset_all(
  rr_timer_id_T *ignore_list_ptr,
  int num_to_ignore,
  const gas_id_t gas_id
)
{
  rr_timer_id_T  *list_ptr;
  rr_timer_ptr_t  rr_timer_ptr;
  rr_timer_cid_t  context_id;
  boolean         do_reset;
  int             i;
  int             j;

  /* set up variables for iteration across the timer pool */
  context_id = (rr_timer_cid_t)gas_id;
  rr_timer_ptr = rr_timer_pool;
  if (num_to_ignore > 0)
  {
    list_ptr = ignore_list_ptr;
    if (list_ptr == NULL)
    {
      num_to_ignore = 0;
    }
  }
  else
  {
    list_ptr = NULL;
  }

  RR_TIMER_LOCK();

  /* reset each entry that does not appear in the provided "ignore list" */
  for (i = 0; i < RR_TIMER_POOL_SIZE; i++, rr_timer_ptr++)
  {
    do_reset = TRUE;

    /* check whether the timer entry is for the active GRR context */
    if (context_id == (rr_timer_cid_t)RR_TIMER_GET_CID_FROM_HANDLE(rr_timer_ptr->rr_timer_hndl))
    {
      /* check whether the timer ID is in the ignore list */
      for (j = 0; j < num_to_ignore; j++)
      {
        if (list_ptr[j] == (rr_timer_id_T)RR_TIMER_GET_TID_FROM_HANDLE(rr_timer_ptr->rr_timer_hndl))
        {
          /* do not reset timers whose IDs appear in the list */
          do_reset = FALSE;
          break;
        }
      }
    }
    else
    {
      /* timer is for a different context, do NOT reset it */
      do_reset = FALSE;
    }

    if (do_reset == TRUE)
    {
      rr_timer_reset_entry(rr_timer_ptr);
    }
  }

  RR_TIMER_UNLOCK();

  return;
}

/****************************************************************************
 *   Function name:  rr_timer_init
 *   --------------
 *
 *   Description: One-time initialisation required at RR task startup
 *
 *   Parameters:  None
 *   -----------
 *
 *   Returns:     Void
 *   --------
 *
 ****************************************************************************/
void rr_timer_init(void)
{
  rr_timer_ptr_t rr_timer_ptr;
  int            i;

  memset(&rr_timer_critical_section, 0, sizeof(rr_timer_critical_section));
  memset(&rr_timer_dummy, 0, sizeof(rr_timer_dummy));
  memset(rr_timer_pool, 0, sizeof(rr_timer_pool));

  rr_timer_ptr = rr_timer_pool;
  for (i = 0; i < RR_TIMER_POOL_SIZE; i++, rr_timer_ptr++)
  {
    rr_timer_reset_entry(rr_timer_ptr);
  }

  /* initialise the resource locking mechanism for the timer pool */
  rex_init_crit_sect(&rr_timer_critical_section);

  return;

}

/****************************************************************************
 *   Function name:  rr_timer_run
 *   --------------
 *
 *   Description:    Internal function that runs a timer with the specified duration
 *
 *   Parameters:     Timer ID, duration in milliseconds, context ID
 *   -----------
 *
 *   Returns:        TRUE if the timer was created & started successfully
 *   --------
 *
 ****************************************************************************/
static boolean rr_timer_run(rr_timer_id_T timer_id, rex_timer_cnt_type millisecs, rr_timer_cid_t context_id)
{
  rr_timer_handle_t timer_handle;
  rr_timer_ptr_t    rr_timer_ptr;
  boolean           bStarted;

  RR_TIMER_LOCK();

  /* check whether the pool has any handle for this timer already */
  rr_timer_ptr = rr_timer_find_entry(timer_id, context_id, &timer_handle);

  /* if the timer handle exists, check whether the entry can be reused */
  if (rr_timer_ptr != NULL)
  {
    /* determine how long the existing timer has left to run */
    if (rr_timer_query_ms(rr_timer_ptr) > RR_TIMER_MIN_DURATION_FOR_RESTART)
    {
      /* clear the existing REX timer, ready for reuse */
      (void)rex_clr_timer(rr_timer_ptr->rex_timer_ptr);
    }
    else
    {
      /* timer expired, or just about to */
      if (rr_timer_ptr->rex_timer_ptr != RR_EXPIRED_TIMER)
      {
        /* for real timers, modify the handle so that the new entry */
        /* will be unique to avoid collisions between REX expiries  */
        timer_handle = timer_handle ^ RR_TIMER_HANDLE_UID_BIT;
      }

      /* then reset the existing entry */
      rr_timer_reset_entry(rr_timer_ptr);
      rr_timer_ptr = NULL;
    }
  }
  else
  {
    /* new instance, generate a new handle */
    timer_handle = rr_timer_generate_handle(timer_id, context_id);
  }

  /* if an existing entry cannot be reused, create a new one */
  if (rr_timer_ptr == NULL)
  {
    /* create a new timer instance with the specified handle */
    rr_timer_ptr = rr_timer_create_entry(timer_handle);
  }

  /* once the timer entry exists in the pool, set it running */
  if (rr_timer_ptr != NULL)
  {
    (void)rex_set_timer(rr_timer_ptr->rex_timer_ptr, millisecs);
    bStarted = TRUE;
  }
  else
  {
    bStarted = FALSE;
  }

  RR_TIMER_UNLOCK();

  return(bStarted);
}

/****************************************************************************
 *   Function name:  rr_timer_start
 *   --------------
 *
 *   Description:    Starts a timer for the given duration for a specific GAS ID.
 *
 *   Parameters:     timer ID, duration, gas_id
 *   -----------
 *
 *   Returns:        TRUE if the timer was started successfully
 *   --------
 *
 ****************************************************************************/
boolean rr_timer_start(rr_timer_id_T timer_id, rex_timer_cnt_type millisecs, const gas_id_t gas_id)
{
  return(rr_timer_run(timer_id, millisecs, (rr_timer_cid_t)gas_id));
}

/****************************************************************************
 *   Function name:  rr_timer_clear
 *   --------------
 *
 *   Description:    Clears down a timer: only for internal use by this module
 *
 *   Parameters:     timer ID and context ID
 *   -----------
 *
 *   Returns:        TRUE if the timer was running when this function is called
 *   --------
 *
 ****************************************************************************/
static boolean rr_timer_clear(rr_timer_id_T timer_id, rr_timer_cid_t context_id)
{
  rr_timer_ptr_t rr_timer_ptr;
  boolean        was_running = FALSE;

  RR_TIMER_LOCK();

  do
  {
    /* find any timer entry that matches the ID and context */
    rr_timer_ptr = rr_timer_find_entry(timer_id, context_id, NULL);
    if (rr_timer_ptr != NULL)
    {
      /* determine whether the timer was running or expired */
      if ((rr_timer_ptr->expiry_posted != RR_TIMER_EXPIRY_POSTED_AND_VALIDATED) &&
          (rr_timer_ptr->rex_timer_ptr != RR_EXPIRED_TIMER))
      {
        was_running = TRUE;
      }

      /* a "cancelled" timer is freed up completely and returns to the pool */
      rr_timer_reset_entry(rr_timer_ptr);
    }
  } while (rr_timer_ptr != NULL);

  /* NOTE: if an expiry message had just been posted by REX and */
  /* is pending in the RR queue, then it will NOT be serviced   */

  RR_TIMER_UNLOCK();

  return(was_running);
}

/****************************************************************************
 *   Function name:  rr_timer_cancel
 *   --------------
 *
 *   Description:    Cancels a timer for a specific GAS ID.
 *
 *   Parameters:     timer ID, gas_id
 *   -----------
 *
 *   Returns:        TRUE if the timer was running when this function is called
 *   --------
 *
 ****************************************************************************/
boolean rr_timer_cancel(
  rr_timer_id_T timer_id,
  const gas_id_t gas_id
)
{
  return(rr_timer_clear(timer_id, (rr_timer_cid_t)gas_id));
}

/****************************************************************************
 *   Function name:  rr_timer_restart
 *   --------------
 *
 *   Description:
 *
 *   Parameters:     Timer ID and duration
 *   -----------
 *
 *   Returns:        TRUE if the timer was restarted successfully
 *   --------
 *
 ****************************************************************************/
boolean rr_timer_restart(
  rr_timer_id_T timer_id,
  rex_timer_cnt_type millisecs,
  const gas_id_t gas_id
)
{
  /* simply starting the timer again will take care of any previous instance */
  return(rr_timer_start(timer_id, millisecs, gas_id));
}

/****************************************************************************
 *   Function name:  rr_timer_extend
 *   --------------
 *
 *   Description:    If a timer is running, extends it by a specified amount,
 *                   if timer has expired (or is just about to) restarts it
 *
 *   Parameters:     timer_id and number of milliseconds to extend for
 *   -----------
 *
 *   Returns:        TRUE if the extension was successful, FALSE otherwise
 *   --------
 *
 ****************************************************************************/
boolean rr_timer_extend(
  rr_timer_id_T timer_id,
  rex_timer_cnt_type millisecs,
  const gas_id_t gas_id
)
{
  rr_timer_ptr_t rr_timer_ptr;
  rr_timer_cid_t context_id;
  boolean        bExtended = FALSE;
  boolean        bStartNew = FALSE;

  context_id = (rr_timer_cid_t)gas_id;

  RR_TIMER_LOCK();

  rr_timer_ptr = rr_timer_find_entry(timer_id, context_id, NULL);

  if (rr_timer_ptr != NULL)
  {
    rex_timer_cnt_type time_remaining;

    /* determine how long the existing timer has left to run */
    time_remaining = rr_timer_query_ms(rr_timer_ptr);

    if (time_remaining > RR_TIMER_MIN_DURATION_FOR_RESTART)
    {
      /* clear the timer from the list */
      (void)rex_clr_timer(rr_timer_ptr->rex_timer_ptr);

      /* and then restart it with the extended duration */
      if (rex_set_timer(rr_timer_ptr->rex_timer_ptr, time_remaining + millisecs) != 0)
      {
        bExtended = TRUE;
      }
    }
    else
    {
      /* caller has tried to extend an expired timer  */
      /* so clear it here and start a new timer below */
      rr_timer_reset_entry(rr_timer_ptr);
      bStartNew = TRUE;
    }
  }
  else
  {
    bExtended = FALSE;
  }

  RR_TIMER_UNLOCK();

  if (bStartNew == TRUE)
  {
    /* start the same timer with the extension duration only */
    bExtended = rr_timer_run(timer_id, millisecs, context_id);
  }

  return(bExtended);
}


/****************************************************************************
 *   Function name:  rr_timer_is_running
 *   --------------
 *
 *   Description:    Determines whether a timer referenced by gas_id is
 *                   active.
 *
 *   Parameters:     timer ID, gas_id
 *   -----------
 *
 *   Returns:        TRUE if the timer is running
 *   --------
 *
 ****************************************************************************/
boolean rr_timer_is_running(rr_timer_id_T timer_id,
                            const gas_id_t gas_id)
{
  rr_timer_ptr_t rr_timer_ptr;
  rr_timer_cid_t context_id;
  boolean        bRunning;

  context_id = (rr_timer_cid_t)gas_id;

  RR_TIMER_LOCK();

  rr_timer_ptr = rr_timer_find_entry(timer_id, context_id, NULL);

  if (rr_timer_ptr != NULL)
  {
    if ((rr_timer_ptr->expiry_posted != RR_TIMER_EXPIRY_POSTED_AND_VALIDATED) &&
        (rr_timer_ptr->rex_timer_ptr != RR_EXPIRED_TIMER))
    {
      bRunning = TRUE;
    }
    else
    {
      bRunning = FALSE;
    }
  }
  else
  {
    /* unable to locate this timer in the pool, so it can't be running */
    bRunning = FALSE;
  }

  RR_TIMER_UNLOCK();

  return(bRunning);
}

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/****************************************************************************
 *   Function name:  rr_timer_truncate
 *   --------------
 *
 *   Description:    Shorten the duration of an existing timer to a maximum
 *
 *   Parameters:     timer ID, maximum duration, gas_id
 *   -----------
 *
 *   Returns:        TRUE if the timer was successfully truncated
 *   --------
 *
 ****************************************************************************/
boolean rr_timer_truncate(rr_timer_id_T      timer_id,
                          rex_timer_cnt_type max_duration,
                          const gas_id_t     gas_id)
{
  rr_timer_ptr_t rr_timer_ptr;
  rr_timer_cid_t context_id;
  boolean        bTruncated;

  context_id = (rr_timer_cid_t)gas_id;

  RR_TIMER_LOCK();

  rr_timer_ptr = rr_timer_find_entry(timer_id, context_id, NULL);

  if (rr_timer_ptr != NULL)
  {
    /* check whether the time remaining for this timer is more than the specified maximum */
    if (rr_timer_query_ms(rr_timer_ptr) > (max_duration+RR_TIMER_MIN_DURATION_FOR_RESTART))
    {
      /* clear the existing REX timer, and then restart it with the shorter duration */
      (void)rex_clr_timer(rr_timer_ptr->rex_timer_ptr);
      (void)rex_set_timer(rr_timer_ptr->rex_timer_ptr, max_duration);
      bTruncated = TRUE;
    }
    else
    {
      /* timer not running, or remaining duration less than the max */
      bTruncated = FALSE;
    }
  }
  else
  {
    /* timer does not exist */
    bTruncated = FALSE;
  }

  RR_TIMER_UNLOCK();

  return(bTruncated);
}
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */


/****************************************************************************
 *   Function name:  rr_timer_has_expired
 *   --------------
 *
 *   Description:    Determines whether a timer has expired (and not been cancelled)
 *
 *   Parameters:     timer ID
 *   -----------
 *
 *   Returns:        TRUE if the timer has expired
 *   --------
 *
 ****************************************************************************/
boolean rr_timer_has_expired(
  rr_timer_id_T timer_id,
  const gas_id_t gas_id
)
{
  rr_timer_ptr_t rr_timer_ptr;
  rr_timer_cid_t context_id;
  boolean        bExpired;

  context_id = (rr_timer_cid_t)gas_id;

  RR_TIMER_LOCK();

  rr_timer_ptr = rr_timer_find_entry(timer_id, context_id, NULL);

  if (rr_timer_ptr != NULL)
  {
    if ((rr_timer_ptr->expiry_posted != RR_TIMER_EXPIRY_NOT_POSTED) ||
        (rr_timer_ptr->rex_timer_ptr == RR_EXPIRED_TIMER))
    {
      /* timer callback has gone off, or none was set, state is EXPIRED */
      bExpired = TRUE;
    }
    else
    {
      bExpired = FALSE;
    }
  }
  else
  {
    /* unable to locate this timer in the pool, so it can't have expired */
    bExpired = FALSE;
  }

  RR_TIMER_UNLOCK();

  return(bExpired);
}

/****************************************************************************
 *   Function name:  rr_timer_is_active
 *   --------------
 *
 *   Description:    Determines whether a timer is active (running or expired)
 *
 *   Parameters:     timer ID
 *   -----------
 *
 *   Returns:        TRUE if the timer is present in the timer pool
 *   --------
 *
 ****************************************************************************/
boolean rr_timer_is_active(
  rr_timer_id_T timer_id,
  const gas_id_t gas_id
)
{
  rr_timer_ptr_t rr_timer_ptr;
  rr_timer_cid_t context_id;
  boolean        bActive;

  context_id = (rr_timer_cid_t)gas_id;

  RR_TIMER_LOCK();

  rr_timer_ptr = rr_timer_find_entry(timer_id, context_id, NULL);

  if (rr_timer_ptr != NULL)
  {
    bActive = TRUE;
  }
  else
  {
    bActive = FALSE;
  }

  RR_TIMER_UNLOCK();

  return(bActive);
}

/****************************************************************************
 *   Function name:  rr_timer_create_expired_timer
 *   --------------
 *
 *   Description:    Creates a timer pool entry that looks like it has expired
 *
 *   Parameters:     timer ID
 *   -----------
 *
 *   Returns:        TRUE if the expired timer was created successfully
 *   --------
 *
 ****************************************************************************/
boolean rr_timer_create_expired_timer(
  rr_timer_id_T timer_id,
  const gas_id_t gas_id
)
{
  rr_timer_handle_t timer_handle;
  rr_timer_ptr_t    expired_ptr;
  rr_timer_cid_t    context_id;
  boolean           bCreated;

  context_id = (rr_timer_cid_t)gas_id;

  RR_TIMER_LOCK();

  /* check whether the pool has any handle for this timer already */
  expired_ptr = rr_timer_find_entry(timer_id, context_id, &timer_handle);

  /* if the timer handle exists, ensure any expiry collisions are avoided */
  if (expired_ptr != NULL)
  {
    /* reset the existing entry */
    rr_timer_reset_entry(expired_ptr);

    /* then modify the handle so that the new entry will be unique */
    /* to avoid collisions between REX callbacks and this function */
    timer_handle = timer_handle ^ RR_TIMER_HANDLE_UID_BIT;
  }
  else
  {
    /* new instance, generate a new handle and find a free entry for it */
    timer_handle = rr_timer_generate_handle(timer_id, context_id);
    expired_ptr  = rr_timer_find_handle(RR_TIMER_INVALID_HANDLE);
  }

  if (expired_ptr != NULL)
  {
    expired_ptr->rex_timer_ptr = RR_EXPIRED_TIMER;
    expired_ptr->rr_timer_hndl = timer_handle;
    expired_ptr->expiry_posted = RR_TIMER_EXPIRY_POSTED_AND_VALIDATED;

    bCreated = TRUE;
  }
  else
  {
    MSG_GERAN_ERROR_0_G("Unable to create expired timer, pool full");
    bCreated = FALSE;
  }

  RR_TIMER_UNLOCK();

  return(bCreated);
}

/****************************************************************************
 *   Function name:  rr_timer_validate_expiry_message
 *   --------------
 *
 *   Description:    Called when a timer expiry message is handled by GRR
 *
 *   Parameters:     Timer ID
 *   -----------
 *
 *   Returns:        TRUE if the expiry message represents a valid timer
 *   --------
 *
 ****************************************************************************/
boolean rr_timer_validate_expiry_message(
  rr_timer_id_T timer_id,
  const gas_id_t gas_id
)
{
  rr_timer_ptr_t rr_timer_ptr;
  rr_timer_cid_t context_id;
  boolean        bValid;

  context_id = (rr_timer_cid_t)gas_id;

  RR_TIMER_LOCK();

  rr_timer_ptr = rr_timer_find_entry(timer_id, context_id, NULL);

  if (rr_timer_ptr != NULL)
  {
    if (RR_TIMER_EXPIRY_POSTED_NOT_VALIDATED == rr_timer_ptr->expiry_posted)
    {
      /* free all resources associated with the timer */
      rr_timer_deallocate_entry(rr_timer_ptr, RR_EXPIRED_TIMER);

      /* and update the expiry status as a result of this validation */
      rr_timer_ptr->expiry_posted = RR_TIMER_EXPIRY_POSTED_AND_VALIDATED;
      bValid = TRUE;
    }
    else
    if (rr_timer_ptr->expiry_posted == RR_TIMER_EXPIRY_POSTED_AND_VALIDATED)
    {
      bValid = TRUE;
    }
    else
    {
      bValid = FALSE;
    }
  }
  else
  {
    bValid = FALSE;
  }

  RR_TIMER_UNLOCK();

  return(bValid);
}

/****************************************************************************
 *   Function name:  rr_timer_get_time_remaining
 *   --------------
 *
 *   Description:    Determines how long the timer has left to run
 *
 *   Parameters:     Timer ID
 *   -----------
 *
 *   Returns:        Milliseconds remaining
 *   --------
 *
 ****************************************************************************/
rex_timer_cnt_type rr_timer_get_time_remaining(
  rr_timer_id_T timer_id,
  const gas_id_t gas_id
)
{
  rex_timer_cnt_type time_left;
  rr_timer_ptr_t     rr_timer_ptr;
  rr_timer_cid_t     context_id;

  context_id = (rr_timer_cid_t)gas_id;

  RR_TIMER_LOCK();

  rr_timer_ptr = rr_timer_find_entry(timer_id, context_id, NULL);
  time_left    = rr_timer_query_ms(rr_timer_ptr);

  RR_TIMER_UNLOCK();

  return(time_left);
}

/****************************************************************************
 *   Function name:  rr_timer_get_state
 *   --------------
 *
 *   Description:    Determines the current state of a timer
 *
 *   Parameters:     Timer ID
 *   -----------
 *
 *   Returns:        One of the timer states (STOPPED, RUNNING, EXPIRED)
 *   --------
 *
 ****************************************************************************/
rr_timer_state_T rr_timer_get_state(
  rr_timer_id_T timer_id,
  const gas_id_t gas_id
)
{
  rr_timer_state_T timer_state;
  rr_timer_ptr_t   rr_timer_ptr;
  rr_timer_cid_t   context_id;

  context_id = (rr_timer_cid_t)gas_id;

  RR_TIMER_LOCK();

  rr_timer_ptr = rr_timer_find_entry(timer_id, context_id, NULL);

  if (rr_timer_ptr == NULL)
  {
    /* unable to find a matching timer in the pool, default state is STOPPED */
    timer_state = RR_TIMER_STOPPED;
  }
  else
  {
    if ((rr_timer_ptr->expiry_posted != RR_TIMER_EXPIRY_NOT_POSTED) ||
        (rr_timer_ptr->rex_timer_ptr == RR_EXPIRED_TIMER))
    {
      /* timer callback has gone off, or none was set, state is EXPIRED */
      timer_state = RR_TIMER_EXPIRED;
    }
    else
    {
      /* all other conditions imply the timer is still RUNNING */
      timer_state = RR_TIMER_RUNNING;
    }
  }

  RR_TIMER_UNLOCK();

  return(timer_state);
}

/**
 * Start barred cell timer. Cell barring here is due to authentication or
 * authentication and ciphering failure.
 */
void rr_start_cell_barred_timer(
  const gas_id_t gas_id
)
{
  if(!rr_timer_is_running(RR_CELL_BARRED_TIMER, gas_id))
  {
    MSG_GERAN_HIGH_0_G("Barred cell timer started");
    (void)rr_timer_start(RR_CELL_BARRED_TIMER,RR_CELL_BARRED_TIMEOUT, gas_id);
  }
  else
  {
    MSG_GERAN_HIGH_0_G("Barred cell_timer is already running ");
  }
} /* rr_start_barred_cell_timer() */



/**
 * Stop barred cell timer. Cell barring here is due to authentication or
 * authentication and ciphering failure.
 */
void rr_stop_barred_cell_timer(
  const gas_id_t gas_id
)
{
  if (rr_timer_cancel(RR_CELL_BARRED_TIMER, gas_id))
  {
    MSG_GERAN_HIGH_0_G("Barred cell timer stopped");
  }
  else
  {
    MSG_GERAN_HIGH_0_G("Barred cell_timer is not running");
  }
} /* rr_stop_barred_cell_timer() */



const char * rr_timer_name(
  rr_timer_id_e timer_id
)
{
  switch (timer_id)
  {
    case RR_GSM_SYS_INFO_TIMER:
      return "RR_GSM_SYS_INFO_TIMER";

    case T3110:
      return "T3110";

    case T3126:
      return "T3126";

    case T3122:
      return "T3122";

    case T3124:
      return "T3124";

    case SIGNAL_STRENGTH_TIMER:
      return "SIGNAL_STRENGTH_TIMER";

    case RR_CELL_SELECTION_ABORT_TIMER:
      return "RR_CELL_SELECTION_ABORT_TIMER";

    case RR_GPRS_SYS_INFO_TIMER:
      return "RR_GPRS_SYS_INFO_TIMER";

    case RR_BCCH_DECODE_COMPLETE_TIMER:
      return "RR_BCCH_DECODE_COMPLETE_TIMER";

    case RR_INVALIDATE_AVAILABLE_PLMN_DB_TIMER:
      return "RR_INVALIDATE_AVAILABLE_PLMN_DB_TIMER";

    case RR_INVALIDATE_PSCAN_RESULTS_TIMER:
      return "RR_INVALIDATE_PSCAN_RESULTS_TIMER";

    case RR_FIVE_SEC_C2_TIMER_1:
      return "RR_FIVE_SEC_C2_TIMER_1";

    case RR_FIVE_SEC_C2_TIMER_2:
      return "RR_FIVE_SEC_C2_TIMER_2";

    case RR_FIVE_SEC_C2_TIMER_3:
      return "RR_FIVE_SEC_C2_TIMER_3";

    case RR_FIVE_SEC_C2_TIMER_4:
      return "RR_FIVE_SEC_C2_TIMER_4";

    case RR_FIVE_SEC_C2_TIMER_5:
      return "RR_FIVE_SEC_C2_TIMER_5";

    case RR_FIVE_SEC_C2_TIMER_6:
      return "RR_FIVE_SEC_C2_TIMER_6";

    case RR_SERV_FIVE_SEC_C1_TIMER:
      return "RR_SERV_FIVE_SEC_C1_TIMER";

    case RR_W2G_SERVICE_REDIRECTION_WAIT_TIMER:
      return "RR_W2G_SERVICE_REDIRECTION_WAIT_TIMER";

    case RR_CELL_BARRED_TIMER:
      return "RR_CELL_BARRED_TIMER";

    case RR_RESET_FORBIDDEN_CELL_TIMER_1:
      return "RR_RESET_FORBIDDEN_CELL_TIMER_1";

    case RR_RESET_FORBIDDEN_CELL_TIMER_2:
      return "RR_RESET_FORBIDDEN_CELL_TIMER_2";

    case RR_RESET_FORBIDDEN_CELL_TIMER_3:
      return "RR_RESET_FORBIDDEN_CELL_TIMER_3";

    case RR_RESET_FORBIDDEN_CELL_TIMER_4:
      return "RR_RESET_FORBIDDEN_CELL_TIMER_4";

    case RR_RESET_FORBIDDEN_CELL_TIMER_5:
      return "RR_RESET_FORBIDDEN_CELL_TIMER_5";

    case RR_RESET_FORBIDDEN_CELL_TIMER_6:
      return "RR_RESET_FORBIDDEN_CELL_TIMER_6";

    case RR_RESET_FORBIDDEN_CELL_TIMER_7:
      return "RR_RESET_FORBIDDEN_CELL_TIMER_7";

    case RR_INVALID_BAND_PREF_TIMER:
      return "RR_INVALID_BAND_PREF_TIMER";

    case T3178:
      return "T3178";

    case T3158:
      return "T3158";

    case T3174:
      return "T3174";

    case T3176:
      return "T3176";

    case CELL_RESELECTION_TIMER:
      return "CELL_RESELECTION_TIMER";

    case RESELECTION_HYSTERESIS:
      return "RESELECTION_HYSTERESIS";

    case PATH_LOSS_TIMER:
      return "PATH_LOSS_TIMER";

    case T3142:
      return "T3142";

    case T3146:
      return "T3146";

    case TMULTIFRM:
      return "TMULTIFRM";

    case GRR_GUARD_TIMER:
      return "GRR_GUARD_TIMER";

    case GRR_SYSTEM_INFO_TIMER:
      return "GRR_SYSTEM_INFO_TIMER";

    case GRR_SURROUND_MEAS_TIMER:
      return "GRR_SURROUND_MEAS_TIMER";

    case GRR_PENALTY_TIMER_1:
      return "GRR_PENALTY_TIMER_1";

    case GRR_PENALTY_TIMER_2:
      return "GRR_PENALTY_TIMER_2";

    case GRR_PENALTY_TIMER_3:
      return "GRR_PENALTY_TIMER_3";

    case GRR_PENALTY_TIMER_4:
      return "GRR_PENALTY_TIMER_4";

    case GRR_PENALTY_TIMER_5:
      return "GRR_PENALTY_TIMER_5";

    case GRR_PENALTY_TIMER_6:
      return "GRR_PENALTY_TIMER_6";

    case GRR_SYS_INFO_REFRESH_TIMER:
      return "GRR_SYS_INFO_REFRESH_TIMER";

    case GRR_SYS_INFO_ACQUISITION_TIMER:
      return "GRR_SYS_INFO_ACQUISITION_TIMER";

    case GRR_SYS_INFO_RETRY_TIMER:
      return "GRR_SYS_INFO_RETRY_TIMER";

    case GPRS_RECENT_ACTIVITY_TIMER:
      return "GPRS_RECENT_ACTIVITY_TIMER";

    case T3208:
      return "T3208";

    case T3210:
      return "T3210";

    case GRR_NACC_CONTAINER_TIMER_0:
      return "GRR_NACC_CONTAINER_TIMER_0";

    case GRR_NACC_CONTAINER_TIMER_1:
      return "GRR_NACC_CONTAINER_TIMER_1";

    case GRR_NACC_CONTAINER_TIMER_2:
      return "GRR_NACC_CONTAINER_TIMER_2";

    case GRR_NACC_CONTAINER_TIMER_3:
      return "GRR_NACC_CONTAINER_TIMER_3";

    case GRR_SYS_INFO_STATUS_TIMER_1:
      return "GRR_SYS_INFO_STATUS_TIMER_1";

    case GRR_SYS_INFO_STATUS_TIMER_2:
      return "GRR_SYS_INFO_STATUS_TIMER_2";

    case T3148:
      return "T3148";

    case T3196:
      return "T3196";

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)

    case RR_G2X_BARRED_FREQUENCY_TIMER_1:
      return "RR_G2X_BARRED_FREQUENCY_TIMER_1";

    case RR_G2X_BARRED_FREQUENCY_TIMER_2:
      return "RR_G2X_BARRED_FREQUENCY_TIMER_2";

    case RR_G2X_BARRED_FREQUENCY_TIMER_3:
      return "RR_G2X_BARRED_FREQUENCY_TIMER_3";

    case RR_G2X_BARRED_FREQUENCY_TIMER_4:
      return "RR_G2X_BARRED_FREQUENCY_TIMER_4";

    case RR_G2X_BARRED_FREQUENCY_TIMER_5:
      return "RR_G2X_BARRED_FREQUENCY_TIMER_5";

    case RR_G2X_BARRED_FREQUENCY_TIMER_6:
      return "RR_G2X_BARRED_FREQUENCY_TIMER_6";

    case RR_G2X_BARRED_CELL_TIMER_1:
      return "RR_G2X_BARRED_CELL_TIMER_1";

    case RR_G2X_BARRED_CELL_TIMER_2:
      return "RR_G2X_BARRED_CELL_TIMER_2";

    case RR_G2X_BARRED_CELL_TIMER_3:
      return "RR_G2X_BARRED_CELL_TIMER_3";

    case RR_G2X_BARRED_CELL_TIMER_4:
      return "RR_G2X_BARRED_CELL_TIMER_4";

    case RR_G2X_BARRED_CELL_TIMER_5:
      return "RR_G2X_BARRED_CELL_TIMER_5";

    case RR_G2X_BARRED_CELL_TIMER_6:
      return "RR_G2X_BARRED_CELL_TIMER_6";

#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS*/

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)

    case RR_G2WR_FIVE_SEC_TIMER_1:
      return "RR_G2WR_FIVE_SEC_TIMER_1";

    case RR_G2WR_FIVE_SEC_TIMER_2:
      return "RR_G2WR_FIVE_SEC_TIMER_2";

    case RR_G2WR_FIVE_SEC_TIMER_3:
      return "RR_G2WR_FIVE_SEC_TIMER_3";

    case RR_G2WR_FIVE_SEC_TIMER_4:
      return "RR_G2WR_FIVE_SEC_TIMER_4";

    case RR_G2WR_FIVE_SEC_TIMER_5:
      return "RR_G2WR_FIVE_SEC_TIMER_5";

    case RR_G2WR_FIVE_SEC_TIMER_6:
      return "RR_G2WR_FIVE_SEC_TIMER_6";

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/

#ifdef FEATURE_LTE

    case RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_1:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_1";

    case RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_2:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_2";

    case RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_3:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_3";

    case RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_4:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_4";

    case RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_5:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_5";

    case RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_6:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_6";

    case RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_7:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_7";

    case RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_8:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_8";

    case RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_9:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_9";

    case RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_10:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_10";

    case RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_11:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_11";

    case RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_12:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_12";

    case RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_1:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_1";

    case RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_2:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_2";

    case RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_3:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_3";

    case RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_4:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_4";

    case RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_5:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_5";

    case RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_6:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_6";

    case RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_7:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_7";

    case RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_8:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_8";

    case RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_9:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_9";

    case RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_10:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_10";

    case RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_11:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_11";

    case RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_12:
      return "RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_12";

    case RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_1:
      return "RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_1";

    case RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_2:
      return "RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_2";

    case RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_3:
      return "RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_3";

    case RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_4:
      return "RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_4";

    case RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_5:
      return "RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_5";

    case RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_6:
      return "RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_6";

    case RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_7:
      return "RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_7";

    case RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_8:
      return "RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_8";

    case RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_9:
      return "RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_9";

    case RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_10:
      return "RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_10";

    case RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_11:
      return "RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_11";

    case RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_12:
      return "RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_12";

#endif /* FEATURE_LTE */


    case T3230:
      return "T3230";

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
    case RR_DUAL_SIM_INTERFACE_TIMER:
      return "RR_DUAL_SIM_INTERFACE_TIMER";

    case RR_DUAL_SIM_HEARTBEAT_TIMER:
      return "RR_DUAL_SIM_HEARTBEAT_TIMER";

    case RR_DUAL_SIM_TERMINATE_TIMER:
      return "RR_DUAL_SIM_TERMINATE_TIMER";

    case RR_DUAL_SIM_BPLMN_LIST_TIMER:
      return "RR_DUAL_SIM_BPLMN_LIST_TIMER";

    case RR_DUAL_SIM_INACTIVATE_TIMER:
      return "RR_DUAL_SIM_INACTIVATE_TIMER";

    case RR_DUAL_SIM_PS_ABORT_TIMER_1:
      return "RR_DUAL_SIM_PS_ABORT_TIMER_1";

    case RR_DUAL_SIM_PS_ABORT_TIMER_2:
      return "RR_DUAL_SIM_PS_ABORT_TIMER_2";

    case RR_EPD_TIMER:
      return "RR_EPD_TIMER";
    
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

    case RR_ACQ_DUTY_CYCLE_TIMER:
      return "RR_ACQ_DUTY_CYCLE_TIMER";

#ifdef FEATURE_MPLMN_PARTIAL_UPDATING
    case RR_PLMN_LIST_SEARCH_GUARD_TIMER:
      return "RR_PLMN_LIST_SEARCH_GUARD_TIMER";

    case RR_PLMN_LIST_SEARCH_PARTIAL_RESULTS_TIMER:
      return "RR_PLMN_LIST_SEARCH_PARTIAL_RESULTS_TIMER";
#else
#ifdef FEATURE_LTE
    case RR_PLMN_LIST_SEARCH_GUARD_TIMER:
      return "RR_PLMN_LIST_SEARCH_GUARD_TIMER";
#endif /* FEATURE_LTE */
#endif /* FEATURE_MPLMN_PARTIAL_UPDATING */

#ifdef FEATURE_LTE_TO_GSM_CGI
    case RR_X2G_CGI_TIMER:
      return "RR_X2G_CGI_TIMER";
#endif /* FEATURE_LTE_TO_GSM_CGI */

    case RR_L1_GUARD_TIMER:
      return "RR_L1_GUARD_TIMER";

    case RR_GRR_CONTROL_STATE_TRANSITION_GUARD_TIMER:
      return "RR_GRR_CONTROL_STATE_TRANSITION_GUARD_TIMER";

    case RR_RESEL_CONTROL_STATE_TRANSITION_GUARD_TIMER:
      return "RR_RESEL_CONTROL_STATE_TRANSITION_GUARD_TIMER";

    case RR_CELL_ACQ_CONTROL_STATE_TRANSITION_GUARD_TIMER:
      return "RR_CELL_ACQ_CONTROL_STATE_TRANSITION_GUARD_TIMER";

    case RR_SGLTE_BAND_COEX_DO_NOT_DISABLE_B9_TIMER:
      return "RR_SGLTE_BAND_COEX_DO_NOT_DISABLE_B9_TIMER";

   case RR_MM_ACTIVATION_RSP_TIMER:
      return "RR_MM_ACTIVATION_RSP_TIMER";	  

    case RR_CONN_REL_CONTROL_WAIT_FOR_REL_CNF_TIMER:
      return "RR_CONN_REL_CONTROL_WAIT_FOR_REL_CNF_TIMER";

    case RR_CONNECTION_RELEASE_STATE_TRANSITION_GUARD_TIMER:
      return "RR_CONNECTION_RELEASE_STATE_TRANSITION_GUARD_TIMER";
	  
    case RR_IRAT_PROC_TIMER:
      return "RR_IRAT_PROC_TIMER";

#ifdef FEATURE_IDLE_DRX_SCALING
    case RR_IDLE_DRX_SCALING_SUSPEND_TIMER:
      return "RR_IDLE_DRX_SCALING_SUSPEND_TIMER";
#endif /* FEATURE_IDLE_DRX_SCALING */

    default:
      MSG_GERAN_ERROR_1("Unknown timer_id: %d", timer_id);
      return "?";
  }
}

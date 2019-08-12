/*===========================================================================

              U M T S   C S   D A T A   T I M E R S   D E F I N I T I O N      
                             S O U R C E   F I L E

DESCRIPTION
  This header file contains definitions for timer functionality used
  by UMTS CS data.  

Copyright (c) 2003 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsdtimer.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/11   SS      Q6 free floating support.
03/13/08   SA      Fixing Klocwork issues.
10/27/04   AR      Adjust timer msec counter to uint32
07/15/03   TMR     Initial release

===========================================================================*/



/*===========================================================================
Header files
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS)

#include "err.h"
#include "data_msg.h"
#include "dsucsdtimer.h"


#define DS_UCSD_TIMER_NULL_INDEX        0xFF
#define DS_UCSD_TIMER_NOT_IN_LIST_INDEX 0xFE


static ds_ucsd_timer_T    ds_ucsd_timer_array[DS_UCSD_MAX_TIMERS] = {{0}};

static uint8       ds_ucsd_timer_list_front_index = 0;


/*===========================================================================

FUNCTION  ds_ucsd_init_timers

DESCRIPTION
   Initializes all of the timer array elements.
   
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
void
ds_ucsd_timer_init(void)
{
   register uint8 i;
   
   ds_ucsd_timer_list_front_index = DS_UCSD_TIMER_NULL_INDEX;

   
   /* Initialize the array of timer structures */
   for (i = 0; i < DS_UCSD_MAX_TIMERS; i++)
   {                         
      ds_ucsd_timer_array[i].in_use  = FALSE;
      ds_ucsd_timer_array[i].expired = FALSE;
      ds_ucsd_timer_array[i].next    = DS_UCSD_TIMER_NOT_IN_LIST_INDEX;
   } 
}




/*===========================================================================

FUNCTION ds_ucsd_timer_cancel

DESCRIPTION
   Cancels a timer.  Note that timer can be actively timing, expired, or
   not set and this function will work correctly.
   
         
DEPENDENCIES
   ds_ucsd_timer_init() must be called first
  
RETURN VALUE
   None
    
SIDE EFFECTS
   Potentially removes a timer from the active timer list

===========================================================================*/
void 
ds_ucsd_timer_cancel
(
   uint8 id
)
{
   register uint8 index;
   register uint8 prev;
   register uint8 next;

   if (id >= DS_UCSD_MAX_TIMERS)
   {
      DATA_MSG1_ERROR("Bad timer ID = %d",id);
      return;
   }
   if ( ds_ucsd_timer_array[id].next != DS_UCSD_TIMER_NOT_IN_LIST_INDEX)
   {
      prev = DS_UCSD_TIMER_NULL_INDEX;
      
      /* Initialize index to front of list */
      index = ds_ucsd_timer_list_front_index;

      /* Search through list until we find the position in the list
      ** of the timer we want to cancel
      */
       
      while ((index != DS_UCSD_TIMER_NULL_INDEX) && (index != id))
      {
         prev  = index;
         index = ds_ucsd_timer_array[index].next;
      }
   
      if (index != DS_UCSD_TIMER_NULL_INDEX)
      {
         next = ds_ucsd_timer_array[index].next;
      
         /* Increment the next item in the list by the cancelled timers
         ** counter value if there is one...
         */
         if (next != DS_UCSD_TIMER_NULL_INDEX)
         {
            ds_ucsd_timer_array[next].counter += ds_ucsd_timer_array[index].counter;
         }
      
         /* Take the timer out of the list */
         if ((prev != DS_UCSD_TIMER_NULL_INDEX) && (prev < DS_UCSD_MAX_TIMERS))
         {
            ds_ucsd_timer_array[prev].next = next;
         }
         else
         {
            ds_ucsd_timer_list_front_index = next;
         }
      }
   }
   ds_ucsd_timer_array[id].counter = 0;
   ds_ucsd_timer_array[id].in_use  = FALSE;
   ds_ucsd_timer_array[id].expired = FALSE;
   ds_ucsd_timer_array[id].next    = DS_UCSD_TIMER_NOT_IN_LIST_INDEX;
}


/*===========================================================================

FUNCTION  ds_ucsd_timer_start()

DESCRIPTION
   Starts a timer with the specified ID for a the specified number of 
   milliseconds.  If the timer is already active, it will be reset
   to the new msec value
   
         
DEPENDENCIES
   ds_ucsd_timer_init() must be called first
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
void
ds_ucsd_timer_start
(
   uint8 id,
   int32 msec
)
{
   register uint8 index,
                  prev,
                  next;
                  
   register int32 counter;
   
   if (id >= DS_UCSD_MAX_TIMERS)
   {
      DATA_MSG1_ERROR("Bad timer ID = %d",id);
      return;
   }

   if (msec < 0)
   {
      DATA_MSG1_ERROR("Bad timer msec value = %d",msec);
      return;
   }

   counter  = msec / DS_GCSD_TIMER_MSEC_PER_TICK;
   prev     = DS_UCSD_TIMER_NULL_INDEX;
   
   /* If timer is already running, cancel it before 
   ** continuing
   */
   if (ds_ucsd_timer_array[id].in_use)
   {
           ds_ucsd_timer_cancel (id);
   }

   ds_ucsd_timer_array[id].in_use = TRUE;   
   
   /* If someone sets a 0 msec timer, set it up, but don't add 
   ** to list
   */

   if (counter == 0)
   {
      ds_ucsd_timer_array[id].expired = TRUE;
      return;
   }

   /* Now find the place for it in the list of timers, and
   ** set the counter accordingly 
   */
   /* Initialize index to front of list */
   index = ds_ucsd_timer_list_front_index;

   /* Search through list until we find the correct position for the new
   ** timer to go in the list.  Note that for each element in the list
   ** that we move past, we subtract the value of it's counter from
   ** the new elements final counter to keep the counters as differential
   ** counters
   */
    
   while ((index != DS_UCSD_TIMER_NULL_INDEX) && (counter >= ds_ucsd_timer_array[index].counter))
   {
      counter -= ds_ucsd_timer_array[index].counter;
      prev = index;
      index = ds_ucsd_timer_array[index].next;
   }
   
   /* We have found the spot for the new timer, set it's counter
   ** accordingly 
   */
   ds_ucsd_timer_array[id].counter = counter;
   
   /* Add new timer to front of list */
   if (prev == DS_UCSD_TIMER_NULL_INDEX)
   {
      /* List may or may not have been empty.  If list is not
      ** empty, then we need to link in the new element in the
      ** list
      */
      if (ds_ucsd_timer_list_front_index != DS_UCSD_TIMER_NULL_INDEX)
      {
         /* Subtract new timer counter value from the next one in the
         ** list to keep the counters differential
         */ 
         ds_ucsd_timer_array[ds_ucsd_timer_list_front_index].counter -= counter;
         
      }
      
      /* Set the next pointer in the new timer */
      ds_ucsd_timer_array[id].next = ds_ucsd_timer_list_front_index;
      
      /* Set front to point to new timer */
      ds_ucsd_timer_list_front_index = id;
   }
   
   /* Add to end of list.... and we know there is at least
   ** one other element in the list or we would have hit
   ** the above case
   */
   else if((index == DS_UCSD_TIMER_NULL_INDEX) && (prev < DS_UCSD_MAX_TIMERS))
   {
      ds_ucsd_timer_array[prev].next = id;
      ds_ucsd_timer_array[id].next = DS_UCSD_TIMER_NULL_INDEX;
   }
   
   /* We are adding to the middle of the list... link the new
   ** element in
   */
   else if(prev < DS_UCSD_MAX_TIMERS)
   {      
      next = ds_ucsd_timer_array[prev].next;
      
      /* Subtract new timer counter value from the next one in the
      ** list to keep the counters differential
      */
      ds_ucsd_timer_array[next].counter -= counter;
      
      /* Link in new timer */
      ds_ucsd_timer_array[id].next = next;
      ds_ucsd_timer_array[prev].next = id;
   }
}

     
/*===========================================================================

FUNCTION   ds_ucsd_timer_tick()

DESCRIPTION
    Function that will be called to advance the timers by a tick.  Note
    that this function needs to be called once every DS_GCSD_TIMER_MSEC_PER_TICK
    milliseconds for the timers to be accurate.
         
DEPENDENCIES
   None
  
RETURN VALUE
   ds_ucsd_timer_init() must be called first
    
SIDE EFFECTS
   None

===========================================================================*/         
void
ds_ucsd_timer_tick(void)
{
   register uint8 index;
    
   if (ds_ucsd_timer_list_front_index != DS_UCSD_TIMER_NULL_INDEX)
   {
      index = ds_ucsd_timer_list_front_index;
      /* The list is in sorted differetial order, so we subtract one from
      ** the front element and if is is 0, the timer has expired.  We also
      ** continue down the list processing all other timers with a counter
      ** value of 0 until we hit one that has a non-zero value 
      */
      if (--ds_ucsd_timer_array[index].counter == 0)
      {
         while((index != DS_UCSD_TIMER_NULL_INDEX) &&
               (ds_ucsd_timer_array[index].counter == 0))             
         {
            register uint8 old_index = index;
            ds_ucsd_timer_array[index].expired = TRUE;
            index = ds_ucsd_timer_array[index].next;
            ds_ucsd_timer_array[old_index].next = DS_UCSD_TIMER_NOT_IN_LIST_INDEX;
         }                             
         ds_ucsd_timer_list_front_index  = index;
      }
   }
}

/*===========================================================================

FUNCTION   ds_ucsd_is_timer_expired()

DESCRIPTION
    This function returns whether or not the timer specified by the timer_id 
    has expired or not.

DEPENDENCIES
   None
  
RETURN VALUE
   boolean

SIDE EFFECTS
   None

===========================================================================*/
boolean ds_ucsd_is_timer_expired
(
  uint32 timer_id
)
{
  return (ds_ucsd_timer_array[timer_id].expired);
}

/*===========================================================================

FUNCTION   ds_ucsd_is_timer_running()

DESCRIPTION
    This function returns whether or not the timer specified by the timer_id 
    is running.

DEPENDENCIES
   None
  
RETURN VALUE
   boolean

SIDE EFFECTS
   None

===========================================================================*/
boolean ds_ucsd_is_timer_running
(
  uint32 timer_id
)
{
  return (ds_ucsd_timer_array[timer_id].in_use);
}

#endif /* #if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS) */

   

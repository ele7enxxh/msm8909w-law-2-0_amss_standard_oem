/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             R T E   T I M E R   M A N A G E R   S E R V I C E
                             
GENERAL DESCRIPTION
  RTE timer services provide coarse timing for applications.
  These services are intended to provide "fast" timer services for
  potentially large numbers of timers which expire relatively
  infrequently.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/kernel/rte/src/tsvc.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
18-May-04  rwh     Changed FEATURE_REX_IPC to FEATURE_RTE_PROCESS
12-May-04  rwh     Reordered some functions to avoid 6050 object file problem.
                   Prepended file name to static data structures.
10-May-04  rwh     Feature-ized with FEATURE_REX_IPC. 
03/10/13   et      Created module.

===========================================================================*/


/*===================================================================
   INCLUDES AND VARIABLE DEFINITIONS
===================================================================*/

/*-------------------------------------------------------------------
   Include Files
-------------------------------------------------------------------*/

#ifdef FEATURE_RTE_PROCESS

#include "rex.h"  /* for rex crit section */
#include "msg.h"
#include "ipcdefi.h"
#include "rte_ipc.h"
#include "ipcrex.h"
#include "ipcns.h"
#include "ipcsvc.h"

#include "tsvc.h"

/*-------------------------------------------------------------------
   Defines
-------------------------------------------------------------------*/
#ifndef TSVC_T_RING_SIZE_EXPONENT
#define TSVC_T_RING_SIZE_EXPONENT    5
#endif

#ifndef TSVC_POOL_TIMER_COUNT
#define TSVC_POOL_TIMER_COUNT        4
#endif

#ifndef TSVC_TICK_RESOLUTION
#define TSVC_TICK_RESOLUTION         5
#endif

#define TSVC_T_RING_SIZE              (1<<TSVC_T_RING_SIZE_EXPONENT)
/*#define TSVC_TICK_TIMER_SIG        0x02*/

/* Example:
** Ring size == 2^5 == 32 bins
** Timer pool size == 512
** Timers per bin assuming uniform distribution == 512/32 == 16 
** Timers per KB == 1024*sizeof(timer_entry_type) = 1024*20 = 50 
** Timer pool used KB = 512/50 = 10
*/

/*-------------------------------------------------------------------
   Type Declarations
-------------------------------------------------------------------*/
/* A timer as it exists in a bin list */
typedef struct tsvc_timer_s
{
   tsvc_timer_handle_type   handle;    /*8bits for bin, 24bits for unique id*/
   uint32                   rounds;    /*  */
   tsvc_timer_duration_type duration;  /* The original duration requested */
   ipc_internal_msg_type*   msg_ptr;   /* The message to send back */
   ipc_node_id_type         client;
   struct tsvc_timer_s*     next_ptr;  /* pointer to the next timer in list */
} tsvc_timer_type;

/* The bin in the ring containing a list of timers*/
typedef struct
{
   tsvc_timer_type list;
   uint32          rolling_counter;
} tsvc_ring_bin_type;

/* Defines the timer pool */
typedef struct
{
   tsvc_timer_type  pool[TSVC_POOL_TIMER_COUNT];
   tsvc_timer_type* free_list_ptr;
   tsvc_timer_type* free_list_tail_ptr;
   uint32           free_count;
} tsvc_pool_type;

/*-------------------------------------------------------------------
   Global Constant Definitions
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
   Global Data Declarations
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
   Static Variable Definitions
-------------------------------------------------------------------*/
/* Unstructured pool of free and allocated timers */
static tsvc_pool_type     tsvc_timer_pool;

/* A ring containing active timers that need to be ticked */
static tsvc_ring_bin_type tsvc_timer_ring[TSVC_T_RING_SIZE];

/* The counter going around the ring with every tick */
static uint32             tsvc_timer_tick_index = 0;

/* Crit section to protect tsvc global variables: tsvc_timer_pool, tsvc_timer_ring, tsvc_timer_tick_index */
#ifdef ARCH_QDSP6
static rex_crit_sect_type tsvc_crit_sect;
#endif /* #ifdef ARCH_QDSP6 */

/*-------------------------------------------------------------------
   Function Prototypes
-------------------------------------------------------------------*/

/*===================================================================
   FUNCTION DEFINITIONS
===================================================================*/


/*===================================================================
FUNCTION:  
   TSVC_TIMER_ALLOC

DESCRIPTION:
   Allocate a timer from the free timer pool.

PARAMETERS
   None

RETURN VALUE:
   A pointer to a new timer.

SIDE EFFECTS:
   None.
===================================================================*/
tsvc_timer_type* tsvc_timer_alloc
(
   void
)
{
   tsvc_timer_type* timer_ptr;

   /* Crit section to protect tsvc global variables */
   #ifdef ARCH_QDSP6
   rex_enter_crit_sect(&tsvc_crit_sect);
   #else
   INTLOCK();
   #endif /* #ifdef ARCH_QDSP6 */

   timer_ptr = tsvc_timer_pool.free_list_ptr;
   if (timer_ptr != NULL)
   {
      tsvc_timer_pool.free_list_ptr = timer_ptr->next_ptr;
      timer_ptr->next_ptr      = NULL;
      --tsvc_timer_pool.free_count;
   }   

   #ifdef ARCH_QDSP6
   rex_leave_crit_sect(&tsvc_crit_sect);
   #else
   INTFREE();
   #endif /* #ifdef ARCH_QDSP6 */

   return timer_ptr;
}


/*===================================================================
FUNCTION:  
   TSVC_TIMER_FREE

DESCRIPTION:

PARAMETERS
   timer_ptr: the timer to free

RETURN VALUE:
   None.

SIDE EFFECTS:
   None.
===================================================================*/
void tsvc_timer_free
(
   tsvc_timer_type* timer_ptr
)
{
   /* reset timer fields */
   timer_ptr->handle   = 0;
   timer_ptr->next_ptr = NULL;

   /* Crit section to protect tsvc global variables */
   #ifdef ARCH_QDSP6
   rex_enter_crit_sect(&tsvc_crit_sect);
   #else
   INTLOCK();
   #endif /* #ifdef ARCH_QDSP6 */

   /* Attach timer at the end of the free list */
   if (tsvc_timer_pool.free_list_ptr == NULL)
   {
      /* The free list is empty */
      tsvc_timer_pool.free_list_ptr = timer_ptr;
   }
   else
   {
      /* Attach timer to end of free list */
      tsvc_timer_pool.free_list_tail_ptr->next_ptr = timer_ptr;
   }
   tsvc_timer_pool.free_list_tail_ptr = timer_ptr;
   
   ++tsvc_timer_pool.free_count;

   #ifdef ARCH_QDSP6
   rex_leave_crit_sect(&tsvc_crit_sect);
   #else
   INTFREE();
   #endif /* #ifdef ARCH_QDSP6 */
}


/*===================================================================
FUNCTION   TMSVR_TIMER_REQUEST

DESCRIPTION
   This function inserts a timer into the timer pool.

PARAMETERS
   timer_ptr: the timer to run
   duration:  the timer duration in milliseconds
   msg_ptr:   the message to post when the timer expire

RETURN VALUE
   Returns true if the timer could be added.  Returns false if
   the timer was already running on a CCLTM_TimerPool, in which
   case no action will be taken.

SIDE EFFECTS
   This function rounds up the microseconds time input
   to the nearest multiple of Granularity.  If microseconds is not
   a multiple of Granularity, the inaccuracy of the timer could
   be increased by a Granularity.
===================================================================*/
tsvc_timer_handle_type tsvc_timer_request
(
   tsvc_timer_duration_type duration,
   union ipc_msg_type**     msg_ptr
)
{
   tsvc_timer_type*       timer_ptr;
   tsvc_timer_type*       current_timer_ptr;
   tsvc_ring_bin_type*    bin_ptr;
   uint32                 ring_offset;
   uint32                 tick_count;
   uint32                 rounds_required;
   int                    lock_sav;
   /*-----------------------------------------------------------------------*/
   
   /* Check the message */
   if ( msg_ptr == NULL )
   {
      MSG_HIGH("Pointer to 'message pointer' is NULL", 0, 0, 0);
      return 0;
   }
   
   if ( *msg_ptr == NULL )
   {
      MSG_HIGH("Message pointer is NULL", 0, 0, 0);
      return 0;
   }

   /* Duration will be at least one tick */
   duration = MAX(duration, 1);
   
   /* Map duration to the number of ticks required.
   ** Tick count is rounded up if necessary */
   tick_count = duration/TSVC_TICK_RESOLUTION;
   if ( (duration % TSVC_TICK_RESOLUTION) >= 1)
   {
      tick_count++;
   }

   /* Calculate how many times we need to go around the timer ring.
   ** Add one so that it will expire when the count reaches zero.
   ** (See Tick() function) */
   rounds_required = (tick_count >> TSVC_T_RING_SIZE_EXPONENT) + 1;

   /* tsvc_timer_alloc is protected by tsvc_crit_sect */
   timer_ptr = tsvc_timer_alloc();

   /* Crit section to protect tsvc global variables */
   #ifdef ARCH_QDSP6
   rex_enter_crit_sect(&tsvc_crit_sect);
   #else
   INTLOCK_SAV(lock_sav);
   #endif /* #ifdef ARCH_QDSP6 */
   
   if (timer_ptr == NULL)
   {
      /* Leave the crit sect if returning here */
      #ifdef ARCH_QDSP6
      rex_leave_crit_sect(&tsvc_crit_sect);
      #else
      INTFREE_SAV(lock_sav);
      #endif /* #ifdef ARCH_QDSP6 */
      return 0;
   }      

   {
      /*------------------------------------------------------------------
      Calculate the offset in the ring (Modulo TSVC_RING_SIZE_EXPONENT)
      TSVC_RING_SIZE must be a power of 2.
      ------------------------------------------------------------------*/
      ring_offset =
      (
         (tick_count + tsvc_timer_tick_index) & (TSVC_T_RING_SIZE - 1)
      );
      
      /*------------------------------------------------------------------
      This is the bin where this timer is going
      ------------------------------------------------------------------*/
      
      bin_ptr = &tsvc_timer_ring[ring_offset];
      
      /*------------------------------------------------------------------
      Update the rolling counter for this bin
      ------------------------------------------------------------------*/
      bin_ptr->rolling_counter = MIN(bin_ptr->rolling_counter+1, 0xffffff);
      
      timer_ptr->msg_ptr        = IPC_MSG_EXT_TO_INT(*msg_ptr);
      timer_ptr->client         = timer_ptr->msg_ptr->owner;
      timer_ptr->msg_ptr->owner = IPCSVC_TSVC;
      *msg_ptr                  = NULL;
      timer_ptr->handle   = (ring_offset<<24) | bin_ptr->rolling_counter;
      timer_ptr->duration = duration;
      timer_ptr->rounds   = rounds_required;
      
      /*------------------------------------------------------------------
      Get the first timer from the chosen bin list
      ------------------------------------------------------------------*/
      current_timer_ptr = &bin_ptr->list;
      
      /*---------------------------------------------------------------
      Locate the timer that expires just before this new timer
      ---------------------------------------------------------------*/
      while (current_timer_ptr->next_ptr != NULL)
      {
         /* ONLY insert if the time left on the next node is greater than -
         ** thus if it is equal it will be inserted AFTER -
         ** this maintains FIFO ordering on dispatching timer callbacks.
         ** Note:  timer->time_left -= curr_timer->time_left cannot result
         ** in a negative number. */
         if (current_timer_ptr->next_ptr->rounds > timer_ptr->rounds)
         {
            break;
         }
         else
         {
            current_timer_ptr  = current_timer_ptr->next_ptr;
            timer_ptr->rounds -= current_timer_ptr->rounds;
         }
      }

      /* We are now pointing to the node after which
      ** we want to insert the timer so insert it. */
      timer_ptr->next_ptr         = current_timer_ptr->next_ptr;
      current_timer_ptr->next_ptr = timer_ptr;
      
      /* If there is a timer following the one we inserted,
      ** update its time, and make sure that it points to us */
      if(timer_ptr->next_ptr != NULL)
      {
         timer_ptr->next_ptr->rounds -= timer_ptr->rounds;
      }
   }
   #ifdef ARCH_QDSP6
   rex_leave_crit_sect(&tsvc_crit_sect);
   #else
   INTFREE_SAV(lock_sav);
   #endif /* #ifdef ARCH_QDSP6 */

   return timer_ptr->handle;
}  /* End of tsvc_timer_request() */


/*===================================================================
FUNCTION   TMSVR_CANCEL_TIMER

DESCRIPTION
   Cancel a running timer.

PARAMETERS
   timer_ptr: the timer to cancel

RETURN VALUE
   The message pointer associated with the timer.  NULL if the timer
   was not running.

SIDE EFFECTS
   None
===================================================================*/
union ipc_msg_type* tsvc_timer_cancel
(
   tsvc_timer_handle_type handle
)
{
   union ipc_msg_type* msg_ptr = NULL;
   tsvc_timer_type*    previous_ptr;
   tsvc_timer_type*    timer_ptr;
   uint32              ring_index;
   
   /*--------------------------------------------------------------------
   Confirm that the handle is within the range it should be.
   --------------------------------------------------------------------*/
   ring_index = handle >> 24;
   if( (handle == 0) || (ring_index >= TSVC_T_RING_SIZE))
   {
      return NULL;
   }

   /* Crit section to protect tsvc global variables */
   #ifdef ARCH_QDSP6
   rex_enter_crit_sect(&tsvc_crit_sect);
   #else 
   INTLOCK();
   #endif /* #ifdef ARCH_QDSP6 */
   
   {
      /*--------------------------------------------------------------------
      Locate the timer in the bin and remove it
      --------------------------------------------------------------------*/
      previous_ptr = &tsvc_timer_ring[ring_index].list;
      
      timer_ptr    = previous_ptr->next_ptr;
      while (timer_ptr != NULL)
      {
         if (timer_ptr->handle == handle)
         {
            /*Adjust the delta of the following timer if necessary
            **/
            if (timer_ptr->next_ptr != NULL)
            {
               timer_ptr->next_ptr->rounds += timer_ptr->rounds;
            }
         
            /*Adjust the preceding timer to point to
            **the timer following the one we are removing
            **/
            previous_ptr->next_ptr = timer_ptr->next_ptr;

            /*Return timer to the free pool
            **/
            msg_ptr = IPC_MSG_INT_TO_EXT(timer_ptr->msg_ptr);
            tsvc_timer_free(timer_ptr);            
            break;
         }
         
         previous_ptr = timer_ptr;
         timer_ptr    = timer_ptr->next_ptr;
      }
   }
   #ifdef ARCH_QDSP6
   rex_leave_crit_sect(&tsvc_crit_sect);
   #else
   INTFREE();
   #endif /* #ifdef ARCH_QDSP6 */

   return msg_ptr;
}  /* End of tsvc_cancelTimer */


/*===================================================================
FUNCTION   TSVC_TIMER_IS_RUNNING

DESCRIPTION
   Check whether a timer is currently running or not.

PARAMETERS
   timer_ptr: the timer to check

RETURN VALUE
   TRUE is this timer is running, FALSE otherwise

SIDE EFFECTS
   None
===================================================================*/
boolean tsvc_timer_is_running
(
   tsvc_timer_handle_type handle
)
{
   uint32            ring_index;
   tsvc_timer_type* timer_ptr;
   
   /*--------------------------------------------------------------------
   Confirm that the handle is within the range it should be.
   --------------------------------------------------------------------*/
   ring_index = handle >> 24;
   if( (handle == 0) || (ring_index >= TSVC_T_RING_SIZE))
   {
      return NULL;
   }
   
   /*--------------------------------------------------------------------
   Find the timer in the corresponding bin
   --------------------------------------------------------------------*/
   /* Crit section to protect tsvc global variables */
   #ifdef ARCH_QDSP6
   rex_enter_crit_sect(&tsvc_crit_sect);
   #else
   INTLOCK();
   #endif /* #ifdef ARCH_QDSP6 */
   
   {
      timer_ptr = tsvc_timer_ring[ring_index].list.next_ptr;
      while (timer_ptr != NULL)
      {
         if (timer_ptr->handle == handle)
         {
            break;
         }
         timer_ptr = timer_ptr->next_ptr;
      }
   } 
   #ifdef ARCH_QDSP6
   rex_leave_crit_sect(&tsvc_crit_sect);
   #else
   INTFREE();
   #endif/* #ifdef ARCH_QDSP6 */

   return (timer_ptr != NULL);
}


/*===================================================================
FUNCTION:
   TSVC_TIMER_TIME_LEFT
DESCRIPTION:
   If the timer is not running, this will return 0.  Otherwise, it
   will return the amount of time remaining before the timer
   expires, up to the owning CCLTM_TimerPool's granularity.
RETURN VALUE:
   See DESCRIPTION.
SIDE EFFECTS:
   none
===================================================================*/
tsvc_timer_duration_type tsvc_timer_time_left
(
   tsvc_timer_handle_type handle
)
{
   uint32           ring_index;
   uint32           ticks_left, rounds_left;
   tsvc_timer_type* timer_ptr;
   
   /*--------------------------------------------------------------------
   Confirm that the handle is within the range it should be.
   --------------------------------------------------------------------*/
   ring_index = handle >> 24;
   if( (handle == 0) || (ring_index >= TSVC_T_RING_SIZE))
   {
      return 0;
   }
   
   /*--------------------------------------------------------------------
   Go through the correct bin and find the timer, computing the number
   of ticks left in the process.
   --------------------------------------------------------------------*/
   /* Crit section to protect tsvc global variables */
   #ifdef ARCH_QDSP6
   rex_enter_crit_sect(&tsvc_crit_sect);
   #else
   INTLOCK();
   #endif /* #ifdef ARCH_QDSP6 */
   
   {
      ticks_left = 0;

      /*--------------------------------------------------------------------
      Find the timer in the corresponding list
      Compute how may rounds are left by adding the deltas preceding it.
      --------------------------------------------------------------------*/
      timer_ptr   = tsvc_timer_ring[ring_index].list.next_ptr;
      rounds_left = 0;
      while (timer_ptr != NULL)
      {
         rounds_left += timer_ptr->rounds;
         if (timer_ptr->handle == handle)
         {
            break;
         }
         timer_ptr    = timer_ptr->next_ptr;
      }

      /*-------------------------------------------------------------
      The timer was found.
      We need to add the ticks from the current tick index
      to the bin index where the timer is located.
      --------------------------------------------------------------*/
      if (timer_ptr != NULL)
      {
         /* Transform rounds into ticks */
         ticks_left = ((rounds_left - 1) * TSVC_T_RING_SIZE);

         /* We need to add the ticks existing between
         ** the current tick index and the bin where the timer is located */
         if (tsvc_timer_tick_index > ring_index)
         {
            ticks_left += TSVC_T_RING_SIZE - (tsvc_timer_tick_index-ring_index);
         }
         else
         {
            ticks_left += ring_index - tsvc_timer_tick_index;
         }
      }
   }
   #ifdef ARCH_QDSP6
   rex_leave_crit_sect(&tsvc_crit_sect);
   #else
   INTFREE();
   #endif /* #ifdef ARCH_QDSP6 */

   /* Convert result in milliseconds */
   return (ticks_left * TSVC_TICK_RESOLUTION);   
} /* End of tmsvr_timer_time_left() */


/*===================================================================
FUNCTION:  
   TSVC_TICK
DESCRIPTION:
   This helping operation increments the tick count and determines
   if any callback functions need to be called.
RETURN VALUE:
   None.
SIDE EFFECTS:
   See Description.
===================================================================*/
void tsvc_tick
(
   rex_timer_cnt_type p_ticks
)
{
   tsvc_timer_type* timer_ptr;
   tsvc_timer_type* previous_timer_ptr;
   /*------------------------------------------------------------------*/

   (void)p_ticks;

   /* Crit section to protect tsvc global variables */
   #ifdef ARCH_QDSP6
   rex_enter_crit_sect(&tsvc_crit_sect);
   #else
   INTLOCK();
   #endif /* #ifdef ARCH_QDSP6 */

   /*--------------------------------------------------------------------
   Get the first timer in the next bin
   --------------------------------------------------------------------*/
   timer_ptr = tsvc_timer_ring[tsvc_timer_tick_index].list.next_ptr;
   
   /*--------------------------------------------------------------------
   Get out if no timer to tick at this time
   --------------------------------------------------------------------*/
   if (timer_ptr != NULL)
   {
      /* Tick the timer */
      --timer_ptr->rounds;

      /*-------------------------------------------------------------
      We find the first timer T with more than zero round left.  All the
      timers between the head of the list and timer T are expired,
      and timer T becomes the new head of the list.
      Use cases:
      - bin empty ----> last == NULL
      - bin [a2] ----> last == NULL
      - bin [a1,b2,c3,d4] ----> last == a 
      - bin [a1] ----> last == a 
      - bin [a1,b1,c3,d4] ----> last == b
      - bin [a1,b1] ----> last == b
      --------------------------------------------------------------*/
      while ((timer_ptr != NULL) && (timer_ptr->rounds == 0))
      {
         void* destination_node_ptr;
         
         /** Get the info on the destination module id
         **/
         destination_node_ptr = ipcns_node_get(timer_ptr->client);
         if ( destination_node_ptr != NULL )
         {
            /** Update the internal message fields
            **/
            timer_ptr->msg_ptr->owner    = IPCSVC_SVC_RESERVED;
            timer_ptr->msg_ptr->sender   = IPCSVC_TSVC;
            timer_ptr->msg_ptr->receiver = timer_ptr->client;

            /** Post the message into the receiver queue
            **/
            ipc_post_msg( timer_ptr->msg_ptr, destination_node_ptr );
         }
         
         previous_timer_ptr = timer_ptr;
         timer_ptr          = timer_ptr->next_ptr;

         /** Adjust the head of the timer list
         **/
         tsvc_timer_ring[tsvc_timer_tick_index].list.next_ptr = timer_ptr;
         
         tsvc_timer_free(previous_timer_ptr);
      }
   }

   /*--------------------------------------------------------------------
   Increment Tick Index modulo the ring size
   --------------------------------------------------------------------*/
   tsvc_timer_tick_index = (tsvc_timer_tick_index + 1) & (TSVC_T_RING_SIZE - 1);   

   #ifdef ARCH_QDSP6
   rex_leave_crit_sect(&tsvc_crit_sect);
   #else
   INTFREE();
   #endif /* #ifdef ARCH_QDSP6 */
}  /* End of tsvc_tick() */



/*===================================================================
FUNCTION:  
   TSVC_INIT

DESCRIPTION:
   Configure the timer service.

PARAMETERS
   None.

RETURN VALUE:
   None.

SIDE EFFECTS:
   None.
===================================================================*/
void tsvc_init
(
   void
)
{
   uint32 i;

   /* Crit section to protect tsvc_timer_pool */
   #ifdef ARCH_QDSP6
   rex_init_crit_sect(&tsvc_crit_sect);
   #endif /* #ifdef ARCH_QDSP6 */

   /* Crit section to protect tsvc global variables */
   #ifdef ARCH_QDSP6
   rex_enter_crit_sect(&tsvc_crit_sect);
   #else
   INTLOCK();
   #endif /* #ifdef ARCH_QDSP6 */
   /*--------------------------------------------------------------------
   Initialize the free timer list   
   --------------------------------------------------------------------*/
   for(i=0; i<TSVC_POOL_TIMER_COUNT-1; i++)
   {
      tsvc_timer_pool.pool[i].next_ptr = &tsvc_timer_pool.pool[i+1];
   }   
   tsvc_timer_pool.pool[i].next_ptr = NULL;

   tsvc_timer_pool.free_list_ptr      = &tsvc_timer_pool.pool[0];/*First free timer*/
   tsvc_timer_pool.free_list_tail_ptr = &tsvc_timer_pool.pool[i];/*Last free timer*/
   tsvc_timer_pool.free_count         = TSVC_POOL_TIMER_COUNT;

   #ifdef ARCH_QDSP6
   rex_leave_crit_sect(&tsvc_crit_sect);
   #else
   INTFREE();
   #endif /* #ifdef ARCH_QDSP6 */
}

#else  /* FEATURE_RTE_PROCESS */
/*lint -e765 -e714 using by tmc.c*/
void tsvc_init(   
   void
)                
{}

#endif /* FEATURE_RTE_PROCESS */


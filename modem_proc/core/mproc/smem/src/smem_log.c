/*===========================================================================

            S M E M L O G . C 

DESCRIPTION

Copyright (c) 2005 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/mproc/smem/src/smem_log.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/17/14   bm      Add lockless SMEM LOG API to be called from single thread
                   environment.
01/31/14   rv      Replaced the memcpy and memmove functions with their 
                   respective secure memscpy and memsmove functions.
01/22/13   bt      Add cast to avoid compiler warning for negative value checks.
01/04/13   ss      Added negative value check for smem_log_area.
08/29/12   bt      Switch order of timetick_init and smem_log_open.
02/09/12   bt      Use Timetick OS abstraction.
05/11/11   hwu     Moved assert.h and err.h to os file. 
04/26/11   bm      Use proper header in case of Q6FW
04/21/11   hwu     Changed to use proper feature for timetick.h.
04/04/11   bt      Remove the Static log and its support, to reduce SMEM items.
01/25/11   rs      Renamed spinlock header file 
06/24/10   hwu     Configure the smem log to be a read-write log area. 
11/19/09   sa      warning cleanup, sync-up with symbian
10/19/09   rs      Removed compiler warnings.
04/06/09   hwu     Remove FEATURE_SMEM_LOG. 
04/01/09   ih      Include smem_target.h
02/13/09   bfc     Added a stub for smem_log_static to maintian backwards 
                   compatibility.
01/29/09   sa      wm warnings cleanup
01/09/09   tl      Fixed indentation during spinlock acquisition
01/09/09   tl      Generalized SMEM logging, allowing multiple log areas
10/30/08   hwu     Check smem_log is initialized before use it.
11/12/07   hwu     removed the err.h include. It's in smem_target.h.
09/13/07   hwu     Added QDSP6 log id.
08/16/07   ebb     Added smem_log_event_static().
07/18/07   hwu     Moved err.h and assert.h to smem_target.h, included in 
                   smem_log_v.h.
04/10/07   ptm     Add smem_ to spin lock symbols.
11/14/06   taw     Remove arguments to smem_log_init() & use smem_alloc for the
                   location of shared memory data.
03/24/06   ptm     Changed code to use pointers to shared memory.
02/22/06   ptm     Add 6 data value API.
03/17/05   ~SN     Initial Version.

===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smem_os.h"
#include "smem_log_v.h"
#include "spinlock.h"
#include "smem.h"
#include "smem_os_common.h"

/*===========================================================================
                    EXTERN DATA DECLARATIONS 
===========================================================================*/
/** Function pointers for internal spinlock implementation */
extern void (*spin_lock_internal_fn_ptr)(uint32);
extern void (*spin_unlock_internal_fn_ptr)(uint32);
/*===========================================================================
                    LOCAL DATA DECLARATIONS 
===========================================================================*/

/* Static and runtime configuration for SMEM log areas */
typedef struct {
  /* Static configuration: Only circular log type is supported. */
  enum {
    SMEM_LOG_TYPE_CIRCULAR
  } smem_log_type;

  /* Static configuration: Is this log area intended to be readable from
   * software? (That is, should the spinlock be locked for the duration of the
   * log write or only for the time necessary to update the write index? See
   * the discussion of race conditions below. The mproc circular log and the 
   * power log are read/write.)
   */
  enum {
    SMEM_LOG_WRITEONLY,
    SMEM_LOG_READWRITE
  } smem_log_readable;

  /* Static configuration: The number of entries in this. Defaults to
   * SMEM_LOG_NUM_ENTRIES. */
  uint32                         smem_num_entries;

  /* Static configuration: The smem entry that contains the event
   * table */
  smem_mem_type                  smem_entry_events;

  /* Static configuration: The smem entry that contains the index
   * of the next log entry */
  smem_mem_type                  smem_entry_idx;

  /* Static configuration: The smem entry that contains the number
   * of times the log has wrapped, or SMEM_INVALID if this is the
   * existing shared memory log and this information is not
   * available. */
  smem_mem_type                  smem_entry_wrap;

  /* Static configuration: The spinlock to acquire while updating
   * this log area. */
  uint32                         smem_log_spinlock;

  /* Runtime state: Pointer to the shared memory location for the
   * event table. */
  volatile smem_log_struct_type *smem_log_events;

  /* Runtime state: Pointer to the shared memory location for the
   * index of the next log entry. */
  volatile uint32               *smem_log_write_idx;

  /* Runtime state: Pointer to the shared memory location for the
   * number of times the log has wrapped, or NULL if the
   * information is not available. */
  volatile uint32               *smem_log_write_wrap;

  /* Runtime state: The index of the next log entry to read. Local
   * to each processor. */
  uint32                         smem_log_read_idx;

  /* Runtime state: The number of times the read index has wrapped
   * past the end of the buffer. */
  uint32                         smem_log_read_wrap;
} smem_log_table_type;

static smem_log_table_type smem_log_table[SMEM_LOG_AREA_COUNT] = {
  /* SMEM_LOG_AREA_CIRCULAR_MPROC */
  {
    /* This log must be circular or the static log rollover in smem_log_event
     * and smem_log_event6 will recurse infinitely. */
    SMEM_LOG_TYPE_CIRCULAR, /* smem_log_type */
    SMEM_LOG_READWRITE, /* smem_log_readable */
    SMEM_LOG_NUM_ENTRIES, /* smem_num_entries */
    SMEM_SMEM_LOG_EVENTS, /* smem_entry_events */
    SMEM_SMEM_LOG_IDX, /* smem_entry_idx */
    SMEM_SMEM_LOG_MPROC_WRAP, /* smem_entry_wrap */
    SMEM_SPINLOCK_SMEM_LOG, /* smem_log_spinlock */
  },
  /* SMEM_LOG_AREA_CIRCULAR_POWER */
  {
    SMEM_LOG_TYPE_CIRCULAR, /* smem_log_type */
    SMEM_LOG_READWRITE, /* smem_log_readable */
    SMEM_LOG_NUM_ENTRIES, /* smem_num_entries */
    SMEM_SMEM_LOG_POWER_EVENTS, /* smem_entry_events */
    SMEM_SMEM_LOG_POWER_IDX, /* smem_entry_idx */
    SMEM_SMEM_LOG_POWER_WRAP, /* smem_entry_wrap */
    SMEM_SPINLOCK_SMEM_LOG, /* smem_log_spinlock */
  },
};

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      smem_log_init

DESCRIPTION   Initializes the smem_log subsystem by opening
              SMEM_LOG_AREA_CIRCULAR_MPROC and SMEM_LOG_AREA_STATIC. Must be
              called before any other APIs are used.

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smem_log_init( void )
{
  /* As soon as the log is opened, writes are allowed, which will call 
   * timetick_get, so we must make sure to initialize the timetick handle 
   * before opening any SMEM logs. */
  (void)smem_timetick_init();
  
  /* Open the default log areas -- mproc circular */
  smem_log_open(SMEM_LOG_AREA_CIRCULAR_MPROC);
} /* smem_log_init */

/*===========================================================================
FUNCTION      smem_log_open

DESCRIPTION   Initializes the shared memory log area. Must be called before any
              other APIs are used with the same log area.

ARGUMENTS     log_area - The logging area to open

RETURN VALUE  0 - log open was successful
              -1 - log open failed

SIDE EFFECTS  Allocates SMEM entries for the logging area
===========================================================================*/
int smem_log_open( smem_log_area area )
{ 
  /* Validate parameters */
  if( (int)area < SMEM_LOG_AREA_FIRST || area >= SMEM_LOG_AREA_COUNT )
  {
    return -1;
  }

  /* Initialize the location of the shared data structures used in this
   * file. */
  smem_log_table[area].smem_log_events = ( smem_log_struct_type * )
    ( smem_alloc( smem_log_table[area].smem_entry_events, 
                  sizeof( smem_log_struct_type ) *
                    smem_log_table[area].smem_num_entries ) );

  smem_log_table[area].smem_log_write_idx    = ( uint32 * )
    ( smem_alloc( smem_log_table[area].smem_entry_idx, sizeof( uint32 ) ) );

  if(smem_log_table[area].smem_entry_wrap != SMEM_INVALID)
  {
    smem_log_table[area].smem_log_write_wrap    = ( uint32 * )
      ( smem_alloc( smem_log_table[area].smem_entry_wrap, sizeof( uint32 ) ) );
  }

  /* Make sure smem_alloc didn't return NULL */
  if ( smem_log_table[area].smem_log_events == NULL ||
       smem_log_table[area].smem_log_write_idx    == NULL )
  {
    ERR_FATAL( "Bad pointer from smem_alloc.", 0, 0, 0 );
  }

  return 0;
} /* smem_log_init */

/*===========================================================================
FUNCTION      smem_log_event

DESCRIPTION   Write an entry in the shared memory event log.

              The log idx is treated with extreme caution to prevent writing
              outside of the event array.

ARGUMENTS     area - the logging area to write to
              id - id tag for the event
              reserved - must be 0
              data1, data2, data3 - event related data items

              Uses spin lock for exclusive access to event log

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smem_log_event ( smem_log_area area, uint32 id, uint32 reserved,
    uint32 data1, uint32 data2, uint32 data3 )
{
  uint32 timetick;
  uint32 idx;
  uint32 next_idx;

  /* Validate parameters */
  if( (int)area < SMEM_LOG_AREA_FIRST || area >= SMEM_LOG_AREA_COUNT )
  {
    return;
  }

  /* Check that we have been initialized */
  if (smem_log_table[area].smem_log_write_idx == NULL) return;

  /* get timestamp. */
  timetick  = smem_timetick_get();

  /* Acquire spinlock for accessing shared memory. */
  smem_spin_lock(smem_log_table[area].smem_log_spinlock);

  /* get index of a smem_log_events struct. use local variable
     "next_idx" for performance smem_log_write_idx is volatile and uncached
   */
  idx = *smem_log_table[area].smem_log_write_idx;
  next_idx = idx + 1;

  /* Handle end-of-buffer conditions for circular logs. */
  if( next_idx >= smem_log_table[area].smem_num_entries )
  {
    /* Circular shared memory log buffer.
       Reached end of allocated space so reset index */
    next_idx = 0;
    if(smem_log_table[area].smem_log_write_wrap != NULL)
    {
      (*smem_log_table[area].smem_log_write_wrap)++;
    }
  }

  *smem_log_table[area].smem_log_write_idx = next_idx;

  /* Free acquired spinlock, if the logging area is tagged "write-only" */
  if(smem_log_table[area].smem_log_readable == SMEM_LOG_WRITEONLY)
  {
    smem_spin_unlock(smem_log_table[area].smem_log_spinlock);
  }

  if( idx < smem_log_table[area].smem_num_entries )
  {
    /* write log data into shared memory buffer with valid idx */
    smem_log_table[area].smem_log_events[idx].identifier= SMEM_LOG_PROC_ID | id;
    smem_log_table[area].smem_log_events[idx].timetick  = timetick;
    smem_log_table[area].smem_log_events[idx].data1     = data1;
    smem_log_table[area].smem_log_events[idx].data2     = data2;
    smem_log_table[area].smem_log_events[idx].data3     = data3;
  }

  /* Free acquired spinlock, if the logging area is not tagged "write-only" */
  if(smem_log_table[area].smem_log_readable != SMEM_LOG_WRITEONLY)
  {
    smem_spin_unlock(smem_log_table[area].smem_log_spinlock);
  }

} /* smem_log_event */

/*===========================================================================
FUNCTION      smem_log_event6

DESCRIPTION   Write two entries in the shared memory event log - allows caller
              to write more data items than smem_log_event.

              This function is not supported on static logs; attempts to use
              this function to log to static logs will instead be redirected
              to the mproc log.

              The log idx is treated with extreme caution to prevent writing
              outside of the event array.

ARGUMENTS     area - the logging area to write to
              id - id tag for the event
              reserved - must be 0
              data1, data2, data3, data4, data5, data6 - event related data
                                                         items

              Uses spin lock for exclusive access to event log

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smem_log_event6
(
  smem_log_area area,
  uint32 id,
  uint32 reserved,
  uint32 data1,
  uint32 data2,
  uint32 data3,
  uint32 data4,
  uint32 data5,
  uint32 data6
)
{
  smem_log_event6_stm (area,
                       id,
                       reserved,
                       data1,
                       data2,
                       data3,
                       data4,
                       data5,
                       data6,
                       FALSE);
} /* smem_log_event6 */

/*===========================================================================
FUNCTION      smem_log_event6_stm

DESCRIPTION   Write two entries in the shared memory event log - allows caller
              to write more data items than smem_log_event.

              This function is not supported on static logs; attempts to use
              this function to log to static logs will instead be redirected
              to the mproc log.

              The log idx is treated with extreme caution to prevent writing
              outside of the event array.

ARGUMENTS     area - the logging area to write to
              id - id tag for the event
              reserved - must be 0
              data1, data2, data3, data4, data5, data6 - event related data
                                                         items
              stm  - TRUE  -> API would execute in STM
                     FALSE -> API would lock OS lock     
                                                         

             This function uses OS and inter-processor HW mutex if stm is
             set to TRUE, and only inter-processor HW mutex if stm is set
             to FALSE, to write to SMEM log area.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smem_log_event6_stm
(
  smem_log_area area,
  uint32 id,
  uint32 reserved,
  uint32 data1,
  uint32 data2,
  uint32 data3,
  uint32 data4,
  uint32 data5,
  uint32 data6,
  uint32 stm
)
{
  uint32 timetick;
  uint32 idx;
  uint32 next_idx;

  /* Validate parameters */
  if( (int)area < SMEM_LOG_AREA_FIRST || area >= SMEM_LOG_AREA_COUNT )
  {
    return;
  }

  /* Check that we have been initialized */
  if (smem_log_table[area].smem_log_write_idx == NULL) return;

  /* get timestamp. */
  timetick  = smem_timetick_get();

  /* Acquire spinlock for accessing shared memory. */
  if(stm == FALSE)
  {
    /* Lock both OS spinlock and inter-proc mutex */
    smem_spin_lock(smem_log_table[area].smem_log_spinlock);
  }else if(spin_lock_internal_fn_ptr)
  {
    spin_lock_internal_fn_ptr(smem_log_table[area].smem_log_spinlock);
  }

  /* get index of a smem_log_events struct. use local variable
     "next_idx" for performance smem_log_write_idx is volatile and uncached
   */
  idx = *smem_log_table[area].smem_log_write_idx;
  next_idx = idx + 2;

  /* Circular shared memory log buffer.
     Reached end of allocated space so reset index */
  if( next_idx >= smem_log_table[area].smem_num_entries )
  {
    next_idx -= smem_log_table[area].smem_num_entries;

    if(smem_log_table[area].smem_log_write_wrap != NULL)
    {
      (*smem_log_table[area].smem_log_write_wrap)++;
    }

    if( next_idx >= smem_log_table[area].smem_num_entries )
    {
      /* next_idx not valid - force it to be valid */
      next_idx = 0;
    }
  }

  *smem_log_table[area].smem_log_write_idx = next_idx;

  /* Free acquired spinlock, if the logging area is tagged "write-only" */
  if(smem_log_table[area].smem_log_readable == SMEM_LOG_WRITEONLY)
  {
    if(stm == FALSE)
    {
      /* Unlock both OS spinlock and inter-proc mutex */
      smem_spin_unlock(smem_log_table[area].smem_log_spinlock);
    }else if(spin_unlock_internal_fn_ptr)
    {
      spin_unlock_internal_fn_ptr(smem_log_table[area].smem_log_spinlock);
    }
  }

  if( idx < smem_log_table[area].smem_num_entries )
  {
    /* write log data into shared memory buffer with valid idx */
    smem_log_table[area].smem_log_events[idx].identifier= SMEM_LOG_PROC_ID | id;

    smem_log_table[area].smem_log_events[idx].timetick  = timetick;
    smem_log_table[area].smem_log_events[idx].data1     = data1;
    smem_log_table[area].smem_log_events[idx].data2     = data2;
    smem_log_table[area].smem_log_events[idx].data3     = data3;

    idx++;
    if( idx == smem_log_table[area].smem_num_entries ) {
      idx = 0;
    }

    /* write continued log data into shared memory buffer */
    smem_log_table[area].smem_log_events[idx].identifier = ( SMEM_LOG_PROC_ID |
                                                          SMEM_LOG_CONTINUE_1 |
                                                          id );

    smem_log_table[area].smem_log_events[idx].timetick   = timetick;
    smem_log_table[area].smem_log_events[idx].data1      = data4;
    smem_log_table[area].smem_log_events[idx].data2      = data5;
    smem_log_table[area].smem_log_events[idx].data3      = data6;
  }

  /* Free acquired spinlock, if the logging area is not tagged "write-only" */
  if(smem_log_table[area].smem_log_readable != SMEM_LOG_WRITEONLY)
  {
    if(stm == FALSE)
    {
      /* Unlock both OS spinlock and inter-proc mutex */
      smem_spin_unlock(smem_log_table[area].smem_log_spinlock);
    }else if(spin_unlock_internal_fn_ptr)
    {
      spin_unlock_internal_fn_ptr(smem_log_table[area].smem_log_spinlock);
    }
  }

} /* smem_log_event6_stm */

/*===========================================================================
FUNCTION      smem_log_read

DESCRIPTION   Reads shared memory log events from the specified area into
              the provided buffer.

              This function will read up to event_count log entries from the
              log specified by log_area into the memory pointed to by
              events. This function maintains the last read position. If the
              log wrapped and overwrote log entries before they could be
              read on this processor, *lost_count will be set to the number
              of entries that were lost. The function returns the number of
              log entries that were read into local memory.

              The client is responsible for calling smem_log_read() at an
              appropiate interval so that log entries are read from the smem
              log before the log is overwritten by new log entries.
              smem_log_read() guarantees that all log entries returned from
              the function will be valid and complete, but does not
              guarantee that no log entries will be lost. The client may use
              the value returned by lost_count to request changes in the
              size of the log at compile time or to increase the frequency
              at which the log is read.

              Only log areas configured as SMEM_LOG_READWRITE may be read
              using smem_log_read() to prevent a race condition when
              smem_log_read() tries to read an entry that has not been fully
              written. The mproc circular log is configured as
              SMEM_LOG_READONLY to avoid spending too much additional time in
              the spinlock. The static log and new logs are configured as
              SMEM_LOG_READWRITE, which keeps the spinlock locked until the
              entire log entry has been written, eliminating the possibility
              for this race condition.

ARGUMENTS     area - the logging area to read from
              event_count - the number of events to read
              events - buffer containing at least event_count shared memory
                       log events
              lost_count - the number of events that were overwritten since
                           the log was read last
              events_remaining - the number of events that remain in this smem
                                 logging area. Clients may use this value to
                                 determine the buffer size to pass to this
                                 function.

RETURN VALUE  The number of smem log entries that were read into events, or
              a negative number if an error occurred.

SIDE EFFECTS  Advances the local read position on the log area.

              Uses spin lock for exclusive access to event log.
===========================================================================*/
int smem_log_read( smem_log_area area,
                   uint32 event_count,
                   smem_log_struct_type * events,
                   uint32 * lost_count,
                   uint32 * events_remaining )
{
  uint32 write_idx, write_wrap = 0;
  uint32 abs_write_idx, abs_read_idx;
  /* The index to start reading at, and the number of entries to try to read */
  uint32 read_idx, read_count, read_count1, read_count2;

  if (lost_count != NULL)
    *lost_count = 0;

  if (events_remaining != NULL)
    *events_remaining = 0;

  /* Validate parameters */
  if( (int)area < SMEM_LOG_AREA_FIRST || area >= SMEM_LOG_AREA_COUNT )
  {
    return -1;
  }

  /* Only allow reading from read/write logs */
  if (smem_log_table[area].smem_log_readable == SMEM_LOG_WRITEONLY)
    return -1;

  /* Check that we have been initialized */
  if (smem_log_table[area].smem_log_write_idx == NULL) return -1;

  if (event_count == 0)
    return 0;

  if (events == NULL)
    return -1;

  /* Lock the spinlock for just long enough to determine the current write
   * position from the log. Note that this exposes us to a race condition:
   * The write index may wrap around the oldest entries and overwrite them
   * before we have a chance to read them. We check the write pointer after
   * copying data out to detect this condition after the fact, and
   * invalidate log entries at the beginning of the buffer if necessary.
   */
  smem_spin_lock(smem_log_table[area].smem_log_spinlock);
  write_idx = *smem_log_table[area].smem_log_write_idx;
  if(smem_log_table[area].smem_log_write_wrap != NULL)
    write_wrap = *smem_log_table[area].smem_log_write_wrap;
  smem_spin_unlock(smem_log_table[area].smem_log_spinlock);

  /* Calculating the absolute number of entries written and read since the
   * dawn of time makes figuring out how many entries to read a bit easier.
   *
   * If entries were overwritten prior to reading, abs_read_idx is updated.
   */
  abs_write_idx = write_idx +
    write_wrap * smem_log_table[area].smem_num_entries;
  abs_read_idx = smem_log_table[area].smem_log_read_idx +
    smem_log_table[area].smem_log_read_wrap *
    smem_log_table[area].smem_num_entries;

  if(abs_write_idx == abs_read_idx)
  {
    /* No entries to be read */
    return 0;
  }
  else if(abs_write_idx < abs_read_idx)
  {
    /* Read pointer is _ahead_ of the write pointer. This shouldn't happen.
     * If it did, it suggests that the buffer has been corrupted, or a
     * client wrapped the write index without incrementing write count
     * (which would happen if the Q6 was running old code), or a bug in the
     * smem log code.
     *
     * The code here is just heuristics -- make a reasonable guess as to
     * what we should do and try to move forward even though something bad
     * happened. Err on the side of reading data that may have been read
     * already and read the entire buffer, starting with the write index.
     */
    read_idx = write_idx;
    read_count = smem_log_table[area].smem_num_entries;

    abs_read_idx = abs_write_idx - smem_log_table[area].smem_num_entries;
  }
  else if(abs_write_idx > abs_read_idx + smem_log_table[area].smem_num_entries)
  {
    /* The log wrapped and entries were lost. Start reading at the write
     * index and read the entire log. Set the lost count. */
    read_idx = write_idx;
    read_count = smem_log_table[area].smem_num_entries;

    if(lost_count != NULL)
      *lost_count = abs_write_idx - abs_read_idx -
        smem_log_table[area].smem_num_entries;

    abs_read_idx = abs_write_idx - smem_log_table[area].smem_num_entries;
    smem_log_table[area].smem_log_read_idx = write_idx;
    smem_log_table[area].smem_log_read_wrap = write_wrap - 1;
  }
  else
  {
    /* Normal case. Start reading at the read index and stop at the write
     * index. */
    read_idx = smem_log_table[area].smem_log_read_idx;
    read_count = abs_write_idx - abs_read_idx;
  }

  if(read_count > smem_log_table[area].smem_num_entries)
  {
    ERR_FATAL("smem_log_read internal error: "
        "read_count (%d) > smem_num_entries (%d)",
        read_count, smem_log_table[area].smem_num_entries, 0);
    return 0;
  }

  if(read_count > event_count)
    read_count = event_count;

  /* Read entries from the log */
  if(read_idx + read_count > smem_log_table[area].smem_num_entries)
  {
    read_count1 = smem_log_table[area].smem_num_entries - read_idx;
    read_count2 = read_idx + read_count - smem_log_table[area].smem_num_entries;
  }
  else
  {
    read_count1 = read_count;
    read_count2 = 0;
  }
  /* Start at read pointer until the end of the buffer */
  smem_mem_copy(events, event_count * sizeof(smem_log_struct_type),
                (void *)(smem_log_table[area].smem_log_events + read_idx),
                read_count1 * sizeof(smem_log_struct_type));
  if(read_count2 > 0)
  {
    /* If the read wraps around the beginning of the buffer */
    smem_mem_copy(events + read_count1, 
                  (event_count - read_count1) * sizeof(smem_log_struct_type),
                  (void *)smem_log_table[area].smem_log_events,
                  read_count2 * sizeof(smem_log_struct_type));
    smem_log_table[area].smem_log_read_idx = read_count2;
    smem_log_table[area].smem_log_read_wrap ++;
  }
  else
  {
    smem_log_table[area].smem_log_read_idx += read_count;
  }

  /* Check whether the beginning of the log was overwritten while we were
   * reading it */
  smem_spin_lock(smem_log_table[area].smem_log_spinlock);
  write_idx = *smem_log_table[area].smem_log_write_idx;
  if(smem_log_table[area].smem_log_write_wrap != NULL)
    write_wrap = *smem_log_table[area].smem_log_write_wrap;
  smem_spin_unlock(smem_log_table[area].smem_log_spinlock);

  abs_write_idx = write_idx +
    write_wrap * smem_log_table[area].smem_num_entries;

  if(events_remaining)
  {
    *events_remaining = abs_write_idx - abs_read_idx - read_count;
  }


  if(abs_write_idx > abs_read_idx + smem_log_table[area].smem_num_entries)
  {
    /* The write pointer advanced into the read pointer while we were
     * reading. Some entries were invalidated.
     */
    uint32 lost_retry = 0;
    uint32 invalidated = abs_write_idx - abs_read_idx -
      smem_log_table[area].smem_num_entries;

    read_count -= invalidated;
    if(lost_count != NULL)
      (*lost_count) += invalidated;

    smem_mem_move(events, event_count * sizeof(smem_log_struct_type),
                  events + invalidated, 
                  read_count * sizeof(smem_log_struct_type));
    /* Try to read the invalidated events to fill up the buffer */
    read_count += smem_log_read(area, event_count - read_count,
        events + read_count, &lost_retry, events_remaining);

    if(lost_count != NULL)
      (*lost_count) += lost_retry;
  }

  return read_count;
} /* smem_log_read */

#ifndef SRCH4DRV_H
#define SRCH4DRV_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              S E A R C H E R   4   D R I V E R    H E A D E R

GENERAL DESCRIPTION
   This file contains the hardware drivers for searcher 4.

EXTERNALIZED FUNCTIONS
   srch4_get_tasks              Returns a ptr requested queue's tasks
   srch4_queue_available        Checks if a queue is available
   srch4_tasks_left             Checks if a queue is active and returns
                                 number of tasks left to be programmed
                                 
   srch4_init                   Set up necessary parameters for srch4

   srch4_flush_s_ram            Sends a command to flush the sample RAM

   srch4_add_task               Adds task to a specified queue (HPQ/LPQ)
   srch4_start_search           Start a specified (HPQ/LPQ)
   srch4_abort_queue            Aborts a specified queue (HPQ/LPQ)
   srch4_halt                   Stops all searches and disables all callbacks

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
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/srch4/src/srch4drv.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/06/12   hh      Sample server cleanup
01/21/11   cjb     Moved unused declarations to internal header.
11/15/10   bb      Changes to support offline search parameters in 
                   CDMA_SRCH_FW_CMD_SRCH_START message
09/09/10   cjb     Fix to flush sample ram for all the VALID queues.
06/21/10   cjb     Featurization removal for CMMI-P4:
                   -> MACROS moved from srch4trans_t.h
                   -> External APIs moved-out to make this header internal
05/28/10   vks     Merge from 1H09:
                   12/03/09   vks     Decouple 1x from SRCH4. Include only
                   srch_srch4.h from 1x and remove other includes. Make
                   appropriate changes.
11/19/09   vks     Add support for Abort response from SRCH4 FW
09/08/09   vks     Export functionality to update shared memory base address
07/29/09   vks     First set of changes for MSGR implementation
06/15/06   pa      Added prototype for srch4_1x_get_assigned_queue()
01/16/06   awj     Lint cleanup
07/19/05   grl     Added flushing the sample ram of the requested chain.
05/20/05   ejv     Remove srch_pn.h.
03/22/05   sst     Removed references to srch4drv_t.h, changed reference to
                    srch4_srch_type to uint32 in logging functions
02/04/05   bt      Add scheduled sample ram flush.
11/17/04   sst     Update function header for srch4_halt
09/08/04   jcm     Rename exported genlog types to include srch_
09/03/04   jcm     Added generalized srch4 subpacket
09/03/04   jcm     Modified genlog to log multiple packet simultaneously
08/09/04   sst     Removed extern of srch4_dump_isr()
05/25/04   sst     Added parameter to abort_cb to be able to pass number
                    of partial results returned
05/04/04   sst/va  Merge from srch4 3Q dev branch (dev-changes)
04/14/04   sst     dev-Added partial results param to srch4_abort_queue()
03/04/04   sst     dev-Added function srch4_halt() to "virtually stop" srch4
02/19/04   sst     Lint corrections
                   Changed srch4_tasks_left() to return int32
02/02/04   sst     Added synchronous field to srch4_abort_queue() to stall
                    until the abort has completed
01/28/04   ejv     Mainline T_MSM6500.
12/15/03   sst     Added a technology parameter to srch4_tasks_left()
11/05/03   bt      Check whether different technologies try to abort 
                    each other and remove srch4_abort.
07/30/03   ejv     Removed sample server references.
07/08/03   sst     Moved SRCH4_MAX_NUM_TASKS, COH_TRUNC_*, and
                    SRCH2_SRCH4_OFFSET from srch4drvi.h to preserve scope
                    of srch4drvi.h
03/24/03   sst     Modified comment to reflect task peak pos stored in Cx16
01/13/03   sst     More return codes, added return values to some functions
12/05/02   sst     Code Clean up
11/14/02   sst     Added return type enum, Removed unused timers
11/11/02   sst     Removed srch4_remove_last_task, deemed unneeded
11/11/02   sst     Added srch4_remove_last_task
11/07/02   sst     Added support for Jaguar, moved MAX_SECTOR_GAIN_VAL here
                   Code cleanup
11/05/02   sst     Merge changes
11/04/02   sst     Moved srch4 win center functionality to srch4trans
10/30/02   bt      Moved SRCH4_NUM_PEAKS here
10/30/02   sst     Added support for srch4's own window center var
10/29/02   sst     Removed temporary functions
                   Separated file, created srch4trans.c .h
                   Made function calls more generic to queues
10/18/02   sst     Temporary functions to "ease" transition to tasks
10/18/02   sst     Changed HPQ to use generic task structure
                   Added support for Transmit Diversity in 1x searching
                   Added translation functions (skeletons) for 1x, HDR, AFLT
10/08/02   sst     Added support for callback functions upon dump/lost dump
10/04/02   sst     Added support for 4 & 6 searcher peaks
10/01/02   sst     Added $Header line
09/26/02   sst     Removal of debug functions.
                   MSM6100 implementation, testing, and code review changes
08/02/02   gs      initial merge into perforce
07/01/02   sst     Added support for LPQ
06/07/02   bt      initial implementation

===========================================================================*/

/*==========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

/* Common */
#include "mcs_variation.h"
#include "comdef.h"
#include "customer.h"

/* MCS */
#include "modem_mcs_defs.h"

/* SRCH4 */
#include "srch4drv_t.h"
#include "srch4trans.h"

/* FW */
#include "srchfw_msg.h"

/* 1X */
#include "srch_genlog.h"

/* Other */
#include "rex.h"


/*===========================================================================

         R E X  C R I T I C A L  S E C T I O N  M A C R O S

===========================================================================*/
/* The following defines create sections where other code is not allowed to
   run. The appropriate macro must be used depending on the type of critical
   section required. The following table provides a guide for selecting the
   correct macro:

   Q1. Will the critical section protect a shared resource?
       (Shared global data or shared code that more than one task can access
        at the same time.)

   Q2. Is the code time critical in nature?

   Q3. Can the code be called from interrupt context?

  /-----------------------------------------------------------------------\
  |   Question    |                       Resolution                      |
  |  -----------  |                      ------------                     |
  | 1    2    3   |                                                       |
  |---------------|-------------------------------------------------------|
  | No   No   -   | No critical section required.                         |
  |-----------------------------------------------------------------------|
  | No   Yes  -   | Use SRCH4_ENTER_ATOMIC_SECT()                          |
  |-----------------------------------------------------------------------|
  | Yes  No   No  | Use SRCH4_ENTER_ISR_SECT()                             |
  |-----------------------------------------------------------------------|
  | Yes  No   Yes | Use SRCH4_ENTER_CRIT_SECT()                            |
  |-----------------------------------------------------------------------|
  | Yes  Yes  No  | Use SRCH4_ENTER_ATOMIC_CRIT_SECT()                     |
  |-----------------------------------------------------------------------|
  | Yes  Yes  Yes | Use SRCH4_ENTER_ATOMIC_ISR_SECT()                      |
  \-----------------------------------------------------------------------/

*/
#if( defined(T_WINNT) || defined(T_QDSP6) || defined(FEATURE_QDSP6) )
/*-------------------------------------
 Q6 critical sections
-------------------------------------*/

extern rex_crit_sect_type srch4_crit_sect;
  /* The SRCH4 global lock for critical sections */

/* Atomic operations */
#define SRCH4_ENTER_ATOMIC_SECT()                      \
          REX_DISABLE_PREMPTION()

#define SRCH4_LEAVE_ATOMIC_SECT()                      \
          REX_ENABLE_PREMPTION()

/* Mutual exclusion between ISRs or between ISR(s) and task(s) */
#define SRCH4_ENTER_ISR_SECT()                         \
          REX_ISR_LOCK( &srch4_crit_sect )

#define SRCH4_LEAVE_ISR_SECT()                         \
          REX_ISR_UNLOCK( &srch4_crit_sect )

/* Mutual exclusion between two normal tasks */
#define SRCH4_ENTER_CRIT_SECT()                        \
          rex_enter_crit_sect( &srch4_crit_sect )

#define SRCH4_LEAVE_CRIT_SECT()                        \
          rex_leave_crit_sect( &srch4_crit_sect )

/* Mutual exclusion between two normal tasks and time critical code */
#define SRCH4_ENTER_ATOMIC_CRIT_SECT()                 \
          rex_enter_crit_sect( &srch4_crit_sect );     \
          REX_DISABLE_PREMPTION()

#define SRCH4_LEAVE_ATOMIC_CRIT_SECT()                 \
          REX_ENABLE_PREMPTION();                      \
          rex_leave_crit_sect( &srch4_crit_sect )

/* Mutual exclusion between two ISRs or between ISR(s) and normal task(s)
   and time critical code */
#define SRCH4_ENTER_ATOMIC_ISR_SECT()                  \
          REX_ISR_LOCK( &srch4_crit_sect );            \
          REX_DISABLE_PREMPTION()

#define SRCH4_LEAVE_ATOMIC_ISR_SECT()                  \
          REX_ENABLE_PREMPTION();                      \
          REX_ISR_UNLOCK( &srch4_crit_sect )


#else /* !T_QDSP6 */
/*-------------------------------------
 Legacy critical sections
-------------------------------------*/

/* The following defines create sections where other code is not allowed to
   run. Intlock and tasklock are called because calls within an intlocked
   section might cause a task switch which could would likely enable
   interrupts. */
#define SRCH4_ENTER_CRIT_SECT() \
        {                      \
          INTLOCK();           \
          TASKLOCK();          \
        }

#define SRCH4_LEAVE_CRIT_SECT() \
        {                      \
          TASKFREE();          \
          INTFREE();           \
        }

#define SRCH4_ENTER_ATOMIC_SECT()        INTLOCK()
#define SRCH4_LEAVE_ATOMIC_SECT()        INTFREE()

#define SRCH4_ENTER_ISR_SECT()           INTLOCK()
#define SRCH4_LEAVE_ISR_SECT()           INTFREE()

#define SRCH4_ENTER_ATOMIC_CRIT_SECT()   INTLOCK()
#define SRCH4_LEAVE_ATOMIC_CRIT_SECT()   INTFREE()

#define SRCH4_ENTER_ATOMIC_ISR_SECT()    INTLOCK()
#define SRCH4_LEAVE_ATOMIC_ISR_SECT()    INTFREE()

#endif /* T_QDSP6 */


/*===========================================================================
             DECLARATIONS FOR MODULE
===========================================================================*/

/* strings for F3 messages */
extern const char srch4_tech_str[ (SRCH4_NUM_TECH + 1) ][ 6 ];
#define SRCH4_TECH_STRING( tech )                                 \
  srch4_tech_str[ ( ( ( (uint32)tech & 0xF) >= (uint32)SRCH4_NUM_TECH ) ? \
                        (uint32)SRCH4_NUM_TECH : ( (uint32)tech & 0xF ) ) ]
extern const char srch4_queue_str[ (SRCH4_MAX_Q + 1) ][ 5 ];
extern const char srch4_prio_str[ SRCH4_PRIO_MAX ][ 10 ];


/*===========================================================================
              FUNCTION DECLARATIONS FOR MODULE
  ==========================================================================*/

/*===========================================================================

              S I M P L I F I C A T I O N   F U N C T I O N S

===========================================================================*/
/*===========================================================================

FUNCTION       SRCH4_GET_TASKS
DESCRIPTION    This function returns the requested task structure.
DEPENDENCIES   None

RETURN VALUE   Ptr to requested task structure
SIDE EFFECTS   None

===========================================================================*/
extern srch4_task_struct_type *srch4_get_tasks
(
  srch4_queue_type queue_type            /* queue to start                 */
);


/*===========================================================================

                   U T I L I T Y   F U N C T I O N S

===========================================================================*/
/*===========================================================================

FUNCTION       SRCH4_QUEUE_AVAILABLE
DESCRIPTION    This function checks to see if a specified queue of
                searcher 4 is available/empty
DEPENDENCIES   None

RETURN VALUE   TRUE   if the queue is available
               FALSE  if the queue is active or being programmed
SIDE EFFECTS   None

===========================================================================*/
extern boolean srch4_queue_available
(
  srch4_queue_type        queue_type      /* queue to check            */
);

/*===========================================================================

FUNCTION       SRCH4_QUEUE_IS_VALID
DESCRIPTION    This function checks to see if a specified queue of
                searcher 4 is valid
DEPENDENCIES   None

RETURN VALUE   TRUE   if the queue is valid
               FALSE  if the queue is invalid
SIDE EFFECTS   None

===========================================================================*/
extern boolean srch4_queue_is_valid
(
  srch4_queue_type        queue_type      /* queue to check            */
);

/*===========================================================================

FUNCTION       SRCH4_TASKS_LEFT
DESCRIPTION    This function checks if the specified searcher queue is active
                and returns how many searches can still be programmed
                It also checks to ensure that the requesting technology
                is using this queue
DEPENDENCIES   None

RETURN VALUE   Negative if searcher is active/tech mismatch/invalid/etc
               Zero if searcher is full
               Positive value is the number of tasks available to be
                programmed
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_tasks_left
(
  srch4_srch_type         task_srch_type, /* search type               */
  srch4_queue_type        queue_type      /* queue to check            */
);


/*===========================================================================

                   D R I V E R   F U N C T I O N S

===========================================================================*/
/*===========================================================================

FUNCTION       SRCH4_INIT
DESCRIPTION    This function initializes anything and everything srch4drv
DEPENDENCIES   None

RETURN VALUE   None.
SIDE EFFECTS   Timers, Completed Tasks variables, etc. are initialized

===========================================================================*/
extern void srch4_init( void );

/*===========================================================================

FUNCTION       SRCH4_SET_MEM_MAP_BASE_ADDR
DESCRIPTION    This function sets the address of the SW-FW shared memory base
               address
DEPENDENCIES   None

RETURN VALUE   None.
SIDE EFFECTS   None

===========================================================================*/
extern void srch4_set_mem_map_base_addr
(
  srchfw_sw_shared_memory_interface_t* addr
);


/*===========================================================================

FUNCTION       SRCH4_ADD_TASK
DESCRIPTION    This function adds a task to specified priority queue
DEPENDENCIES   None

RETURN VALUE   Srch4drv error value
SIDE EFFECTS   Srch4 structure is modified, tasks are added to the DSP

===========================================================================*/
extern int32 srch4_add_task
(
  srch4_srch_type         task_srch_type, /* search type                  */
  srch4_queue_type        queue_type,     /* queue to add tasks to        */
  srch4_task_struct_type *task            /* task to add                  */
);

/*===========================================================================

FUNCTION       SRCH4_START_SEARCH
DESCRIPTION    This function commands searcher4 to sweep using the
                specified priority queue.
DEPENDENCIES   None

RETURN VALUE   Srch4drv error value
SIDE EFFECTS   Srch4 structure is modified. A queue of searches is started

===========================================================================*/
extern int32 srch4_start_search
(
  srch4_srch_type  srch4_type,            /* type of search                 */
  srch4_queue_type queue_type,            /* queue to start                 */
  int16  freq_offset,                     /* freq offset ( 4.6875 Hz/LSB )  */
  uint16 priority,                        /* search priority (32767=highest)*/
  boolean offline_search,                 /* indicate offline search        */
  uint16  capturedRTCCnt,                 /* RTC count in CX2 at which sample
                                             capture is done               */
  void (*callback_func)(void *),          /* pointer to a callback function */
                                          /*  to be called upon status dump */
  void (*lost_dump_callback_func)(void *),/* pointer to a callback function */
                                          /*  to be called upon lost dump   */
  void *(cb_data)                        ,/* data to be passed to callback  */
  srch4_mode_type  mode                  /* mode data                      */
#ifdef FEATURE_MODEM_COEXISTENCE_FW
  ,boolean          atomic_search
#endif
);

/*===========================================================================

FUNCTION       SRCH4_ABORT_QUEUE
DESCRIPTION    This function aborts the specified priority queue
DEPENDENCIES   None

RETURN VALUE   Srch4drv error value
SIDE EFFECTS   One queue in srch4 is aborted

===========================================================================*/
extern int32 srch4_abort_queue
(
  srch4_srch_type         task_srch_type, /* search type                    */
  srch4_queue_type        queue_type,     /* queue to abort tasks to        */
  void                  (*abort_cb_func)( uint32, void * ),
                                          /* pointer to a callback function */
                                          /*  to be called upon abort dump  */
  void                  *(cb_data),       /* callback data                  */
  boolean                 sync,           /* synchronous abort flag         */
  boolean                 part_res        /* partial results flag           */
);

/*===========================================================================

FUNCTION       SRCH4_PROCESS_DUMP
DESCRIPTION    This function is used when a status dump is triggered.
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   Callbacks will be called

===========================================================================*/
extern void srch4_process_dump
(
  srchfw_search_rsp_msg_t* search_rsp,       /* Response msg from FW       */
  boolean                  flag_a            /* MDSP abort flag            */
);

/*===========================================================================

FUNCTION       SRCH4_HALT
DESCRIPTION    This function should only be called in preparation for the
                MDSP app being unregistered for 1x, HDR, and AFLT. Also all
                active searches will be aborted, without any callbacks
                specified. 
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   All pending searches will not be processed when a dump occurs
               Any task waiting for a callback from srch4 will NOT recieve 
               them, as all callbacks have been cleared

===========================================================================*/
extern void srch4_halt( void );

#ifdef FEATURE_MODEM_COEXISTENCE_FW
/*============================================================================

FUNCTION SRCH4_CLEAR_HELPER_QUEUE_PARAMS

DESCRIPTION
  This function initializes the helper parameters of the queue.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.  

============================================================================*/
extern void srch4_clear_helper_queue_params(
  srch4_queue_var_type *queue              /* to accommodate queues      */
);


/*============================================================================

FUNCTION SRCH4_CALCULATE_LEAD_TIME_TO_TASK_ESTIMATION

DESCRIPTION
  This function calculates the lead time to search w.r.t task estimation.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.  

============================================================================*/
extern int64 srch4_calculate_lead_time_to_task_estimation(
  srch4_srch_type search_type,               /* search type             */
  uint16          coherent_len,              /* coherent length          */
  uint8           non_coherent_len,          /* non-coherent length      */
  uint8           pilot_bursts               /* pilot burst length       */
);

/*============================================================================

FUNCTION SRCH4_CALCULATE_LEAD_TIME_TO_SEARCH_SCHEDULING

DESCRIPTION
  This function calculates the lead time to search w.r.t search scheduling.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.  

============================================================================*/
extern int64 srch4_calcuate_lead_time_to_search_scheduling(
  srch4_srch_type search_type,               /* search type             */
  uint16          coherent_len,              /* coherent length          */
  uint8           non_coherent_len,          /* non-coherent length      */
  uint8           pilot_bursts               /* pilot burst length       */
);



/*============================================================================

FUNCTION SRCH4_CALCULATE_LEAD_TIME_TO_SEARCH_SCHEDULING

DESCRIPTION
  This function returns search scheduling lead time for the tasks in a given
queue.
 
DEPENDENCIES
  None

RETURN VALUE
  uint64: search scheduling lead time.

SIDE EFFECTS
 None.  

============================================================================*/
extern int64 srch4_get_search_scheduling_lead_time(
  srch4_queue_var_type  *queue
);



/*============================================================================

FUNCTION srch4_get_task_estimation_lead_time

DESCRIPTION
  This function returns task estimation lead time for the tasks in a given
queue.
 
DEPENDENCIES
  None

RETURN VALUE
  uint64: task estimation lead time.

SIDE EFFECTS
 None.  

============================================================================*/
extern int64 srch4_get_task_estimation_lead_time(
  srch4_queue_var_type  *queue
);

#endif
#endif /* SRCH4DRV_H */

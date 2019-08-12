#ifndef SRCH4TRANS_H
#define SRCH4TRANS_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              S E A R C H E R   4   T R A N S L A T I O N S
                        H E A D E R F I L E

GENERAL DESCRIPTION
   This file contains the translation functions used by external tasks
   (1X, HDR, AFLT, etc) to translate their structures into srch4's
   generic task structure.

EXTERNALIZED FUNCTIONS
   srch4_mdsp_get_app_valid     Returns status of searcher 4's MDSP App
   srch4_enable                 Enables srch4 per technology
   srch4_mdsp_app_reg_cb        Registers a callback to be called on MDSP
                                 enable/disable
   srch4_register_mdsp_app      Registers srch4 app with the MDSP

   srch4_flush_sample_ram       Pass through function that flushes the
                                 sample RAM
   srch4_get_est_srch_time      Returns the search time for a (512,1) srch
   srch4_trans_cleanup_dump     Called from the drivers so the trans layer
                                 can cleanup after a search dump

   srch4_set_win_center         Sets searcher 4's window center
   srch4_get_win_center         Gets searcher 4's window center
   srch4_slew_win_center        Slews searcher 4's window center

   srch4_XXX_register           Registers technology XXX with srch4, sets
                                 priority and callbacks
   srch4_XXX_deregister         De-registers technology XXX with srch4
   srch4_XXX_active             Checks to see if technology XXX's has a
                                 srch in progress
   srch4_XXX_start_search       Starts technology XXX's search
   srch4_XXX_abort_search       Aborts technology XXX's search
   srch4_XXX_add_task           Adds a single technology XXX task 1x to the
                                 search queue

   srch4_aflt_search_pending    Checks to see if any AFLT searches are pending
   srch4_aflt_continue_search   Programs more searches and starts another
                                 AFLT search

   srch4trans_init              Initializes srch4's translation layer
INITIALIZATION AND SEQUENCING REQUIREMENTS

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/srch4trans.h#1 $

when       who     what, where, why 
--------   ---     ----------------------------------------------------------
11/12/12   ag      Added cx2_rtc and srch_mode parameters to 1x/HDR srch iface
03/06/12   hh      Sample server cleanup
06/03/11   adw     Add configurable carrier parameter in 1x add tasks.
04/12/11   smd     Moved search structure from HDR to srch4.
12/02/10   zm      Moved into mcs/api folder - SU API effort.
11/15/10   bb      Changes to support new APIs for offline searches
09/13/10   vks     Add support for AFLT and LPQ.
06/29/10   cjb     Mainline HDR related featurized code sections.
06/21/10   cjb     Featurization removal for CMMI-P4:
                   -> Mainlined all HDR and AFLT related APIs
                   -> Moved APIs & related structs to make srch4drv.h internal
05/28/10   vks     Merge from 1H09:
                   12/03/09   vks     Decouple 1x from SRCH4. Include only
                   srch_srch4.h from 1x and remove other includes. Make
                   appropriate changes.
                   11/20/09   vks     Define SRCH4_NOTUSED macro to avoid using
                   the one defined in srchi.h.
12/29/09   vks     Fix previous checkin.
12/29/09   vks     Add support for FW-FWS app config message.
11/19/09   vks     Add support for Abort response from SRCH4 FW
11/09/09   ag      Added an include modem_mcs_defs.h for 9K builds.
09/13/09   vks     Pass on pointers to the messages instead of the entire
                   message to the functions processing these messages
09/08/09   vks     Implement App enable msg
07/29/09   vks     First set of changes for MSGR implementation
09/08/08   adw     Added SRCH4_AFLT_ENABLED to featurize AFLT.
06/20/08   mca     Merge from //depot
05/27/08   mca     Modified srch4 differentiate between normal 1x and offtl
                   searching
08/14/07   mca     Featurized hdr functions
04/10/06   mt      Added 1x pilot search capability to srch4_hdr_add_task().
02/26/07   sst     Lint fixes
01/19/07   sst     Modified srch4 mdsp app request/release functionality
09/08/06   awj     Condensed offset param in add task to streamline interface
06/15/06   pa      Moved definition of srch4_1x_get_assigned_queue()
                   to srch4drv.h to avoid exporting srch4drv_t.h.
05/31/06   awj     Further sect and srch4 updates
01/16/06   awj     Lint cleanup
12/19/05   bt      Added srch4_1x_get_assigned_queue.
11/18/05   va      Added function to reset AFLT variables
07/19/05   grl     Added flushing the sample ram of the requested chain.
02/04/05   bt      Add scheduled sample ram flush.
01/17/05   sst     Added access function srch4_mdsp_get_app_valid()
12/07/04   sst     Changed srch4_1x_add_task() and srch_sect_srch4_append/_r()
                    to remove offset and add receive diversity field parameters
11/17/04   sst     Update dependency field of srch4_mdsp_app_reg_cb
09/07/04   sq      Changed srch4_hdr_abort_search definition
08/13/04   va      Changed function srch4_aflt_add_task to allow free_q and 
                   win center to be passed as parameters.
08/09/04   ejv     Add new function prototype srch4_1x_wait_and_discard.
07/14/04   jcm     Added srch4_1x_num_tasks_added() and 
                    srch4_1x_num_tasks_req()
07/12/04   sst     Added registration callbacks (implemented for 1x only),
                    this allows for "delayed" assignment of queues when none
                    are available, or un-registered queues are "abort active"
06/30/04   dna     Added a function srch4_disable_mdsp_app() that is to be  
                    called from an SRM object when 1x and HDR both vote they 
                    don't need the SRCH4 dsp image                           
06/25/04   jcm     Mainlined FEATURE_HDR, SRCH4_HDR_ENABLED, and 
                   SRCH4_AFLT_ENABLED
06/21/04   sst     Updated srch4_enable header
05/28/04   jcm     Sector module integration
05/25/04   sst     Added parameter to abort_cb to be able to pass number
                    of partial results returned
05/04/04   sst/va  Merge from srch4 3Q dev branch (dev-changes)
04/14/04   sst     dev-Added partial results param to srch4_1x_abort_search()
03/25/04   sst     dev-Moved (de)registering support into tech specific funcs
03/25/04   va      dev-Added function declarations for srch4_aflt_(de)register
03/18/04   sst     dev-Added support for srch4 (de)registering
                   dev-Modified srch4_1x_start_search() parameters
03/04/04   sst     dev-Changed return of srch4_enable() to be a boolean
02/12/04   ddh     Added corrections for lint.
02/02/04   sst     Added synchronous field to 1X abort function
01/28/04   ejv     Mainline T_MSM6500.
01/21/04   sst     Added function srch4_get_est_srch_time()
01/06/04   sst     Added parameter age_searches to srch4_1x_add_task to
                    facilitate proper use of search sector aging
11/04/03   bt      Change srch4 voting interface for MSM6500.
10/28/03   aar     Added declaration of function srch4_register_mdsp_app.
10/28/03   ddh     Replaced integer values in srch4_1x_disable with enums
09/29/03   dna     Get T_QUASAR to compile and work again for MSM6500 (w/o HDR)
06/17/03   ejv     Define SRCH4_HDR_ENABLED for 6500.
06/15/03   rdh     Added srch4_reserved.
02/18/03   va      SRCH4 AFLT support.
02/12/03   bt      Change sleep_1x_enable to select clk_src.
01/23/03   bt      Added clock voting/control scheme for various technologies.
01/13/03   sst     Added return values to start, add, and abort functions,
                    added func to translate drv return values to trans values,
                    added parameter to lost_dump_cb
12/05/02   sst     Code Clean up
11/07/02   sst     Moved #define SRCH4_WIN_CENTER (debug till we support this)
                   Code cleanup
11/05/02   sst     Merge changes
11/04/02   sst     srch4 win center function moved here from srch4drv
10/22/02   sst     Created File 

===========================================================================*/

/*==========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

/* Common */
#include "customer.h"
#include "comdef.h"

/* SRCH4 */
#include "srch4drv_t.h"
#include "srch4trans_t.h"

/* FW */
#include "srchfw_msg.h"
#include "fws_msg.h"

/* 1X */
#include "srch_genlog.h"
#include "srch_hw_t.h"
#include "srch_srch4.h"


/* Other */
#include "rex.h"

/* FEATURE DEPENDENT */
#include "queue.h"


/*===========================================================================
                    MACRO DEFINITIONS FOR MODULE
===========================================================================*/

/* macro to prevent compiler warnings when the payload parameter is not used
   by state entry, exit, and transition functions, or when a variable is only
   used in MSG_XXXX calls that may be compiled out */
#define SRCH4_NOTUSED(i) if(i){}


/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/
/* structure for storing the slotting info for the task. */
typedef struct
{
  uint64 time_for_execution;
  uint32 coh_non_coh_length;
  uint8  position_in_task_order;
} srch4_task_priority_order_type;


/* structure for searcher 4 queue information */
typedef struct
{
  srch4_srch_type   srch_type;    /* type of search  (set at start search) */
  int16      freq_offset;         /* freq offset ( 4.6875 Hz/LSB )         */
  uint16     priority;            /* search priority (32767=high, 1=low)
                                     (logging: 4 bit resolution, 0-16)     */
  boolean    qpch;                /* qpch bit, as yet undefined            */
  boolean    active;              /* flag - srch4 is active                */
  boolean    aborted;             /* flag - current search was aborted     */
  boolean    lost;                /* flag - current search was lost        */
  boolean    partial_res;         /* flag - partial results are wanted on
                                     current aborted search                */
  uint8      num_tasks;           /* # of tasks currently programmed       */
  uint8      max_num_tasks;       /* maximum number of tasks available
                                     in the queue                          */
  uint8      last_srch_res_cnt;   /* number of srch results returned from
                                     last search                           */
  uint32     task_cnt;            /* total number of tasks                 */
  uint32     srch_cnt;            /* total number of srches (whole queue)  */
  uint32     abort_cnt;           /* total number of aborted searches      */
  void     (*dump_cb_func)(void *);   /* callback function on status dump  */
  void     (*lost_cb_func)(void *);   /* callback function on lost dump    */
  void     (*abort_cb_func)(uint32, void *);
                                      /* callback function on aborted dump */
  void     (*srch_cb_data);           /* data to be passed to callback     */
  uint16     lost_dump_time_estimate; /* estimated lost dump timeout       */
  timer_type   *lost_dump_timer;      /* pointer to lost dump timer        */
  LOG_SRCH4_C_type *log_ptr;          /* pointer to diag alloc'ed log pkt  */
  LOG_SRCH4_C_type *log_pkt;          /* searcher4's log pkt               */
  uint16     log_pkt_siz;             /* size of the log pkt               */
  srch_genlog_srch4_sub_v1 *sub_log_pkt; /* searcher4's general log pkt    */
  srch4_task_struct_type *tasks;      /* ptr to queue's task struct        */

  /* Data Types added for slotted mode searching */

  /* Callback to call when partial dumps are available */
  timer_type   *schedule_task_timer;

  boolean  scheduled_for_search;

  srch4_queue_type        queue_type;     /* queue to add tasks to        */

  srch4_mode_type         mode;                   /* Srch mode */

  uint16  capturedRTCCnt;                 /* RTC count in CX2 at which sample 
                                             capture is done                */

											 
  srch4_mode_type   original_mode;                   /* Srch mode */
  
  /* Lead time to search for the largest task in the task list */
  uint64 lead_time_to_search;

  /* Number of tasks that have been scheduled already */
  uint8  num_tasks_scheduled;

  /* Number of tasks that have got its dumps back already */
  uint8   num_completed_search;

  /* Indicates if a repeat search is needed */
  boolean repeat_search;

  boolean atomic_search;

  uint64  time_slot_start;
  uint64  time_slot_duration;
  uint64  time_slot_lead_duration;
  uint64  expected_next_schedule_time;
  uint8   num_zero_result_searches;

} srch4_queue_var_type;


/* structure for HDR search information */
typedef struct
{
  int16  freq_offset;                   /* freq offset ( 4.6875 Hz/LSB )   */
  void (*dump_cb_func)(void *);         /* pointer to a callback function  */
                                        /*  to be called upon status dump  */
  void (*lost_cb_func)(void *);         /* pointer to a callback function  */
                                        /*  to be called upon lost dump    */
  void *(cb_data);                      /* callback data                   */
  srch4_srch_type  srch_type;           /* whether 1x or DO pilot search   */
  boolean  offline_search;              /* indicate offline                */
  uint16   capturedRTCCnt;              /* RTC count in CX2 at which sample 
                                           capture is done                 */
  srch4_mode_type  mode;                /* srch mode */
  boolean          atomic_search;       /* should reschedule or not */
}srch4_hdr_search_type;


/* structure for 1X search information */
typedef struct
{
  void *(cb_data);                      /* callback data                   */
  boolean  offline_search;              /* indicate offline                */
  uint16   capturedRTCCnt;              /* RTC count in CX2 at which sample 
                                           capture is done                 */
  srch4_mode_type  mode;                /* srch mode */
  boolean          atomic_search;       /* should reschedule or not */

}srch4_1x_search_type;

/*===========================================================================
             DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/


/*===========================================================================
              FUNCTION DEFINITIONS FOR MODULE
===========================================================================*/

/*===========================================================================

           M D S P    A P P    R E L A T E D    F U N C T I O N S

===========================================================================*/
/*===========================================================================

FUNCTION       SRCH4_MDSP_GET_APP_VALID

DESCRIPTION    This function returns TRUE the SRCH4 MDSP app is registered
                and enabled.

DEPENDENCIES   None

RETURN VALUE   Boolean indicating if SRCH4's MDSP App is valid.

SIDE EFFECTS   None

===========================================================================*/
extern boolean srch4_mdsp_get_app_valid( void );

/*===========================================================================

FUNCTION       SRCH4_ENABLE
DESCRIPTION    This function enables which technology needs searcher 4. If no
               technology needs searcher 4 it will vote for DSP sleep.
DEPENDENCIES   Only one technology's bit shall be set via a given call

RETURN VALUE   True if app is awake and valid
               False if app is not valid or if app was put to sleep
SIDE EFFECTS   None

===========================================================================*/
extern boolean srch4_enable
(
  srch4_tech_type tech,                    /* technology                   */
  boolean  enable                          /* whether to enable   */
);

/*===========================================================================

FUNCTION       SRCH4_MDSP_APP_REG_CB
DESCRIPTION    This function registers mdsp callbacks to be called upon 
                enable/disable of the srch4 mdsp app
DEPENDENCIES   The registered callback function must handle the case where
                the MDSP app is disabled and searches were pending. In this
                case the searches will be aborted without notification to
                the corresponding task, though this callback will be called

RETURN VALUE   None.
SIDE EFFECTS   None.

===========================================================================*/
extern void srch4_mdsp_app_reg_cb
(
  void     (*callback)( boolean ),
  boolean    reg_cb
);

/*===========================================================================

FUNCTION       SRCH4_MDSP_APP_MSG_HANDLER
DESCRIPTION    This function handles the SRCH4 APP enable/disable messages
               that are received and then forwarded by the technologies to 
               the srch4 trans layer for processing
DEPENDENCIES   MDSP task must be running.

RETURN VALUE   None.
SIDE EFFECTS   Registered MDSP app callbacks will be called
               All pending searches will be aborted without callbacks or
                any notifications to tasks. This must be taken care of in
                the task specific MDSP app callback.

===========================================================================*/
extern void srch4_mdsp_app_msg_handler
(
  fws_app_config_rsp_t *cfg_rsp_msg       /* mdsp app cfg response message */
);

/*===========================================================================

FUNCTION       SRCH4_REGISTER_MDSP_APP
DESCRIPTION    This function registers the SRCH4 MDSP application.
DEPENDENCIES   This is legacy support until the use of this interface is
                removed from tmc.c

RETURN VALUE   None.
SIDE EFFECTS   None.

===========================================================================*/
extern void srch4_register_mdsp_app
(
  rex_tcb_type *task_tcb,                 /* task requesting searcher 4    */
  rex_sigs_type task_sig                  /* signal to set upon completion */
);

/*===========================================================================

FUNCTION       SRCH4_REQUEST_MDSP_APP
DESCRIPTION    This function registers the SRCH4 MDSP application.
DEPENDENCIES   MDSP task must be running.

RETURN VALUE   None.
SIDE EFFECTS   None.

===========================================================================*/
extern void srch4_request_mdsp_app
(
  srch4_tech_type tech_type,              /* mask for technology           */
  rex_tcb_type   *task_tcb,               /* task requesting searcher 4    */
  rex_sigs_type   task_sig                /* signal to set upon completion */
);

/*===========================================================================

FUNCTION       SRCH4_RELEASE_MDSP_APP
DESCRIPTION    Disables the SRCH4 module entirely (not just for sleep)

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch4_release_mdsp_app
(
  srch4_tech_type   tech                  /* technology                    */
);


/*===========================================================================

           S R C H 4 T R A N S    U T I L I T Y    F U N C T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH4_GET_EST_SRCH_TIME
DESCRIPTION    This function returns the estimated search time of a single
                search based on the parameters given
DEPENDENCIES   Search Time estimates are for HPQ only
               To estimate total srch time, the queue's overhead 
                (SRCH4_SW_QUEUE_OVHD_US) must be added
               Must be passed coh_len/non_coh_len pairs [512,1]

RETURN VALUE   Estimated search time in us (value of "-1" indicates an error)
SIDE EFFECTS   None

===========================================================================*/
extern uint32 srch4_get_est_srch_time
(
  uint16    window_size,               /* window size (chips x8)   */
  uint16    coherent_len,              /* coherent length          */
  uint8     non_coherent_len           /* non-coherent length      */
);

/*===========================================================================

FUNCTION       SRCH4_TRANS_CLEANUP_DUMP
DESCRIPTION    This is used to do general cleanup for queues after a status
               dump is received (abort, lost, dump)
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch4_trans_cleanup_dump
(
  srch4_srch_type tech                  /* technology releasing it's queue */
);


/*===========================================================================

           S R C H _ W I N _ C E N T E R    F U N C T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH4_SET_WIN_CENTER
DESCRIPTION    This function sets searcher 4's window center value
DEPENDENCIES   Window center is in ChipX8

RETURN VALUE   None
SIDE EFFECTS   srch4_win_center is set

===========================================================================*/
extern void srch4_set_win_center
(
  int32 win_center                       /* new window center (ChipX8) */
);

/*===========================================================================

FUNCTION       SRCH4_GET_WIN_CENTER
DESCRIPTION    This function gets searcher 4's window center value (chipX8)
DEPENDENCIES   None

RETURN VALUE   srch4's window center (chipX8) (in the range 0 ... 262143)
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_get_win_center( void );

/*===========================================================================

FUNCTION       SRCH4_SLEW_WIN_CENTER
DESCRIPTION    This function slews searcher 4's window center value and
                returns the new window center (chipX8)
DEPENDENCIES   None

RETURN VALUE   New window center value for srch4  (chipX8)
SIDE EFFECTS   srch4_win_center is slewed

===========================================================================*/
extern int32 srch4_slew_win_center
(
  int32 slew_value                       /* slew amount */
);


/*===========================================================================

           1 X    T R A N S L A T I O N    F U N C T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH4_1X_REGISTER
DESCRIPTION    This function registers 1x with searcher 4. Once registered
                1x will "own" a queue, and it's callbacks will be set.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_1x_register
(
  srch4_priority_type priority,         /* search priority                 */
  int16  freq_offset,                   /* freq offset ( 2.34375 Hz/LSB )  */
  void (*dump_cb_func)(void *),         /* pointer to a callback function  */
                                        /*  to be called upon status dump  */
  void (*lost_cb_func)(void *),         /* pointer to a callback function  */
                                        /*  to be called upon lost dump    */
  void (*reg_cb_func)(void *),          /* pointer to a callback function  */
                                        /*  to be called upon successful   */
                                        /*  assignment to a queue          */
  void  *reg_cb_data                    /* register callback data          */
);

/*===========================================================================

FUNCTION       SRCH4_1X_DEREGISTER
DESCRIPTION    This function deregisters 1x from it's assigned queue
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch4_1x_deregister( void );

/*===========================================================================

FUNCTION       SRCH4_1X_OFFTL_REGISTER
DESCRIPTION    This function registers 1x with searcher 4. Once registered
                1x will "own" a queue, and its callbacks will be set.
DEPENDENCIES   If reg_cb_func() is not specified, this is a one shot attempt
                at registering for a queue, if reg_cb_func() is defined, then
                a queue will be assigned to 1X sometime in the future.

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_1x_offtl_register( void );

/*===========================================================================

FUNCTION       SRCH4_1X_OFFTL_DEREGISTER
DESCRIPTION    This function deregisters 1x from its assigned queue
               This function can also be used to unregister a reg_cb_func on
                a failed registration and before the queue has been assigned
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch4_1x_offtl_deregister ( void );

/*===========================================================================

FUNCTION       SRCH4_1X_ACTIVE
DESCRIPTION    This function checks the status of the assigned queue
DEPENDENCIES   None

RETURN VALUE   Returns TRUE if assigned queue is active
SIDE EFFECTS   None

===========================================================================*/
extern boolean srch4_1x_active( void );

/*===========================================================================

FUNCTION       SRCH4_1X_OFFL_START_SEARCH
DESCRIPTION    This function gets the queue and sends the command to start 
               the specified queue's searcher4 sweep. It is used to program
               offline searches
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_1x_offl_start_search
(
  void *(cb_data),                       /* callback data                   */
  boolean  offline_search,               /* indicate offline                */
  uint16   capturedRTCCnt                /* RTC count in CX2 at which sample
                                            capture is done                 */
);

/*===========================================================================

FUNCTION       SRCH4_1X_START_SEARCH
DESCRIPTION    This function gets the queue and sends the command to start 
                the specified queue's searcher4 sweep.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_1x_start_search
(
  void *(cb_data)                       /* callback data                   */
);

/*===========================================================================

FUNCTION       SRCH4_1X_START_SRCH
DESCRIPTION    This function gets the queue and sends the command to start 
                the specified queue's searcher4 sweep.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_1x_start_srch
(
  srch4_1x_search_type*   srch_details
);

/*===========================================================================

FUNCTION       SRCH4_1X_ABORT_SEARCH
DESCRIPTION    This function gets the queue and sends the command to abort 
                the specified queue's searcher4 sweep.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_1x_abort_search
(
  void    (*abort_cb_func)( uint32, void * ),
                                      /* pointer to a callback function  */
                                      /*  to be called upon abort dump   */
  void    *(cb_data),                 /* callback data                   */
  boolean   sync,                     /* synchronous abort flag          */
  boolean   part_res                  /* partial results flag            */
);

/*===========================================================================

FUNCTION       SRCH4_1X_PROCESS_DUMP
DESCRIPTION    This function is used by the 1X technology to pass on the
               search results message received by it to the srch4 driver
               for processing

DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None  

===========================================================================*/
extern void srch4_1x_process_dump
( 
  srchfw_search_rsp_msg_t*  search_rsp     /* Response MSG from SRCH4 FW    */
);

/*===========================================================================

FUNCTION       SRCH4_1X_PROCESS_ABORT_DUMP
DESCRIPTION    This function is used by the 1X technology to pass on the
               abort results message received by it to the srch4 driver
               for processing

DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch4_1x_process_abort_dump
(
  srchfw_search_rsp_msg_t*   search_rsp    /* Response MSG from SRCH4 FW   */
);

/*===========================================================================

FUNCTION       SRCH4_1X_ADD_TASK
DESCRIPTION    This function adds a 1x sector to a 1x srch4 task structure.
DEPENDENCIES   None

RETURN VALUE   Number of tasks that can still be programmed
               Negative value if an error occurred
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_1x_add_task
(
  srch_sect_parm_type    *parms,              /* search parameters          */
  srch_sect_result_type  *results,            /* search results             */
  boolean                 receive_diversity,  /* flag to indicate use of RD */
  boolean                 transmit_diversity, /* flag to indicate use of TD */
  srch4_carrier_type      carrier             /* carrier to use             */
);

/*===========================================================================

FUNCTION       SRCH4_1X_NUM_TASKS_ADDED
DESCRIPTION    This function returns the number of tasks that have been 
               successfully added and are not contained in the srch4 queue.
DEPENDENCIES   None

RETURN VALUE   Number of tasks in srch4 queue.
SIDE EFFECTS   None

===========================================================================*/
extern uint32 srch4_1x_num_tasks_added(void);

/*===========================================================================

FUNCTION       SRCH4_BUILD_SRCH4_SUB
DESCRIPTION    Builds and commits a 1X searcher4 generalized subpacket
DEPENDENCIES   This logging is only valid for 1X

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch4_build_srch4_sub
(
  srch_genlog_packet_id_type  id,         /* id of packet to commit sub    */   
  sub_commit_func_type        commit_fp,  /* f(x) to commit the subpacket  */
  uint32                      queue_type, /* queue to log                  */
  int32                       num_results /* number of srches returned     */
);

/*===========================================================================

FUNCTION       SRCH4_1X_GET_ASSIGNED_QUEUE
DESCRIPTION    This function returns the assigned queue for 1X.
DEPENDENCIES   None

RETURN VALUE   which queue is assigned.
               
SIDE EFFECTS   None

===========================================================================*/
extern srch4_queue_type srch4_1x_get_assigned_queue( void );



/*===========================================================================

           H D R    T R A N S L A T I O N    F U N C T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH4_HDR_REGISTER
DESCRIPTION    This function registers hdr with searcher 4. Once registered
                hdr will "own" a queue, and it's priority will be set.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_hdr_register
(
  srch4_priority_type priority          /* search priority                 */
);

/*===========================================================================

FUNCTION       SRCH4_1X_DEREGISTER
DESCRIPTION    This function deregisters hdr from it's assigned queue
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch4_hdr_deregister ( void );

/*===========================================================================

FUNCTION       SRCH4_HDR_ACTIVE
DESCRIPTION    This function checks the status of the assigned queue
DEPENDENCIES   None

RETURN VALUE   Returns True if assigned queue is active
SIDE EFFECTS   None

===========================================================================*/
extern boolean srch4_hdr_active( void );

/*===========================================================================

FUNCTION       SRCH4_HDR_START_SEARCH
DESCRIPTION    This function commands searcher4 to sweep using the assigned
               queue. 
               This api is deprecated and should not be used. 
               srch4_hdr_start_srch should be used instead.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_hdr_start_search
(
  int16  freq_offset,                   /* freq offset ( 2.34375 Hz/LSB )  */
  void (*dump_cb_func)(void *),         /* pointer to a callback function  */
                                        /*  to be called upon status dump  */
  void (*lost_cb_func)(void *),         /* pointer to a callback function  */
                                        /*  to be called upon lost dump    */
  void *(cb_data),                      /* callback data                   */
  srch4_srch_type  srch_type           /* whether 1x or DO pilot search   */
);

/*===========================================================================

FUNCTION       SRCH4_HDR_START_SRCH
DESCRIPTION    This function commands searcher4 to sweep using the assigned
                queue
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_hdr_start_srch
(
  srch4_hdr_search_type*  srch_details
);

/*===========================================================================

FUNCTION       SRCH4_HDR_ABORT_SEARCH
DESCRIPTION    This function aborts a searcher4 sweep.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_hdr_abort_search
(
  void (*abort_cb_func)( uint32, void * ),
                                      /* pointer to a callback function  */
                                      /*  to be called upon abort dump   */
  void    *(cb_data),                 /* callback data                   */
  boolean   sync,                     /* synchronous abort flag          */
  boolean   part_res                  /* partial results flag            */
);

/*===========================================================================

FUNCTION       SRCH4_HDR_PROCESS_DUMP
DESCRIPTION    This function is used by the HDR technology to pass on the
               search results message received by it to the srch4 driver
               for processing

DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None  

===========================================================================*/
extern void srch4_hdr_process_dump
(
  srchfw_search_rsp_msg_t* search_rsp      /* Response MSG from SRCH4 FW   */
);

/*===========================================================================

FUNCTION       SRCH4_HDR_PROCESS_ABORT_DUMP
DESCRIPTION    This function is used by the HDR technology to pass on the
               abort results message received by it to the srch4 driver
               for processing

DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch4_hdr_process_abort_dump
(
  srchfw_search_rsp_msg_t*   search_rsp    /* Response MSG from SRCH4 FW   */
);

/*===========================================================================

FUNCTION       SRCH4_HDR_ADD_TASK
DESCRIPTION    This function adds an HDR sector to a task structure.
DEPENDENCIES   None

RETURN VALUE   If successfully added returns number of tasks on queue, else
               returns an error code, which is a negative number..
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_hdr_add_task
(
  srch4_srch_struct_type             *slist,
  srch4_srch_type                    srch_type
);



/*===========================================================================

           A F L T    T R A N S L A T I O N    F U N C T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH4_AFLT_RESET
DESCRIPTION    This function resets pending tasks and callback data for AFLT 
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch4_aflt_reset (void);

/*===========================================================================

FUNCTION       SRCH4_AFLT_REGISTER
DESCRIPTION    This function registers aflt with searcher 4. Once registered
                aflt will "own" a queue, and it's callbacks will be set.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_aflt_register
(
  srch4_priority_type priority,         /* search priority                 */
  int16  freq_offset,                   /* freq offset ( 2.34375 Hz/LSB )  */
  void (*dump_cb_func)(void *),         /* pointer to a callback function  */
                                        /*  to be called upon status dump  */
  void (*lost_cb_func)(void *)          /* pointer to a callback function  */
                                        /*  to be called upon lost dump    */
);

/*===========================================================================

FUNCTION       SRCH4_AFLT_DEREGISTER
DESCRIPTION    This function deregisters AFLT from it's assigned queue
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch4_aflt_deregister ( void );

/*===========================================================================

FUNCTION       SRCH4_AFLT_SEARCH_PENDING
DESCRIPTION    This function checks if there are any pending AFLT searches
DEPENDENCIES   None

RETURN VALUE   Returns True if AFLT searches are pending
SIDE EFFECTS   None

===========================================================================*/
extern boolean srch4_aflt_search_pending( void );

/*===========================================================================

FUNCTION       SRCH4_AFLT_ACTIVE
DESCRIPTION    This function checks the status of the assigned queue
DEPENDENCIES   None

RETURN VALUE   Returns True if assigned queue is active
SIDE EFFECTS   None

===========================================================================*/
extern boolean srch4_aflt_active( void );

/*===========================================================================

FUNCTION       SRCH4_AFLT_START_SEARCH
DESCRIPTION    This function commands searcher4 to sweep using the assigned
                queue
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_aflt_start_search
(
  void *(cb_data)                       /* callback data                   */
);

/*===========================================================================

FUNCTION       SRCH4_AFLT_ABORT_SEARCH
DESCRIPTION    This function aborts a searcher4 sweep.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_aflt_abort_search
(
  void (*abort_cb_func)( uint32, void * ),
                                      /* pointer to a callback function  */
                                      /*  to be called upon abort dump   */
  void *(cb_data)                     /* callback data                   */
);

/*===========================================================================

FUNCTION       SRCH4_AFLT_PROCESS_DUMP
DESCRIPTION    This function is used by the AFLT technology to pass on the
               search results message received by it to the srch4 driver
               for processing

DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None  

===========================================================================*/
extern void srch4_aflt_process_dump
(
  srchfw_search_rsp_msg_t* search_rsp      /* Response MSG from SRCH4 FW   */
);

/*===========================================================================

FUNCTION       SRCH4_AFLT_PROCESS_ABORT_DUMP
DESCRIPTION    This function is used by the AFLT technology to pass on the
               abort results message received by it to the srch4 driver
               for processing

DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch4_aflt_process_abort_dump
(
  srchfw_search_rsp_msg_t*   search_rsp    /* Response MSG from SRCH4 FW   */
);

/*===========================================================================

FUNCTION       SRCH4_AFLT_CONTINUE_SEARCH
DESCRIPTION    This function programs the searches on the pending queue and
               starts the searcher again.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_aflt_continue_search( void );

/*===========================================================================

FUNCTION       SRCH4_AFLT_ADD_TASK
DESCRIPTION    This function adds an AFLT sector to a task structure.
DEPENDENCIES   None

RETURN VALUE   If successfully added returns number of tasks on queue, else
               returns an error code, which is a negative number..
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_aflt_add_task
(
  q_type  *aflt_q_ptr,                      /* queue of aflt sectors to add*/
  q_type  *free_q_ptr,                      /* queue of free aflt sectors  */
  int32   ppm_win_center                    /* Win center reference for PPM*/  
);


/*===========================================================================

           S R C H 4 T R A N S    I N I T    F U N C T I O N

===========================================================================*/
/*===========================================================================

FUNCTION       SRCH4TRANS_INIT
DESCRIPTION    This function initializes the srch4 translation layer. 
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch4trans_init(void);

typedef struct
{
  uint64 starting_time; /* starting time of the inactivity slot [micro-seconds] */
  uint64 duration;        /* duration of the inactivity slot [micro-seconds] */
  boolean info_available;  /* info is not available.. at this point, this check is redundant */
} srch4_time_slot_type;

typedef struct
{
  /* This flag will be set to FALSE if the inactivity is not slotted and TRUE 
    otherwise. e.g if GSM Tx is off, then this flag will be FALSE. */
  boolean is_slotted;

  /* Number of valid Slots. This should be accessed only if is_slotted 
    is TRUE */
  int32 num_slots;

  /* List of inactivity slots. This should be accessed only if is_slotted 
    is TRUE */
  srch4_time_slot_type slots[15];

} srch4_cxm_inactivity_slot_type;


void srch4_update_cxm_inactivity_table(
  srch4_cxm_inactivity_slot_type* time_slots
);

#endif /* SRCH4TRANS_H */

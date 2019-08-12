/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              S E A R C H E R   4   D R I V E R

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
   srch4_process_dump           Process search dump

   srch4_dump_isr               Process search interrupt

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/srch4/src/srch4drv.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/23/15   mn      Debug changes to catch 
                    "srch4drv.c:4167 Assertion fw_index < SRCH4_MAX_TASKS failed" 
                    crash. (CR: 795387).
09/04/14   ag      Set mode to DSDA_OFFLINE for srch under sched timer expiry
09/04/14   ag      Added circular buffer for debug
06/23/14   sr      Avoid using sched timer if COEX_FW feature is not defined.
06/16/14   sr      Ignore schedule task callback if abort is already sent
                   and clear schedule task timer when-ever abort is sent.
05/29/14   sr      Clear schedule search timer once abort is handled(669308)
05/18/14   sr      If a tech is scheduled for seach, then task left should 
                   return ACTIVE (CR: 662603)
04/28/14   sr      When start is delayed due to GTx activity, if 1X asks for
                   queue status; we are not indicating it to be active. This
                   was needed to avoid the scenario where 1X could wind up
                   sending abort.
05/23/14   sr      Fixing Thor Compilation Errors(670326).
05/19/14   rj      Handle 24 Bit USTMR for BOLT using VSTMR API
04/14/14   sr      Abort is sent before start search (CR: 623924)
04/14/14   mn      Set the number of srch tasks to 0 when abort is sent before
                    search start happens (CR: 644430).
01/31/14   sr      KW Fixes (CR: 610111).
08/27/13   sr      Atomic search is rescheduled if GTx is updated(CR:531918) 
07/29/13   sr      AFLT is being slotted around GTx Inactivity. (CR: 518329)
06/20/13   rj      Fixing compiler error
02/27/13   ag      Remove Triton featurization
02/07/13   mn      Fixing compiler warnings.
05/22/12   sr      Additional Logging in Srch4 for triaging issues.
03/06/12   hh      Sample server cleanup
03/05/12   sr      Fix Logging Error in calculating peak window.
02/10/12   sr      Srch4 Cleanup and merging avg and sum search combining.
01/17/12   sr      Feature Cleanup
10/13/11   sr      Modify LPQ nominal timeout from 400 to 2000.
09/25/11   sr      Set peak_norm_dis to 0 for HDR. 
09/19/11   ag      Modify LPQ nominal timeout from 300 to 400. 
09/13/11   sr      Code Refactoring and fixing debug logging anamolies.
08/04/11   sr      Introduced locks during queue assignment and release.
06/15/11   adw     Log 1x RxAGC correctly in both SV and non-SV cases.
04/19/11   vks     Mainline SRCH4_CARRIER_SELECT_SUPPORT.
04/13/11   vks     Use new RF SVDO api for rx agc calls. 
01/25/11   cjb     Lint fixes.
12/02/10   bb      Changes to support not active or empty HPQ/LPQ/HPQ1 scenarios
11/23/10   cjb     SRCH4 specific feature for Offline Search support.
11/17/10   cjb     Featurize update of 'carrierSelect' field in search params.
11/15/10   bb      Changes to support offline search parameters in
                   CDMA_SRCH_FW_CMD_SRCH_START message.
11/10/10   cjb     Adding AFLT search request/response support.
09/21/10   vks     Do not send flush sample ram message for now as it is not
                   implemented in firmware.
09/09/10   cjb     Fix to flush sample ram for all the VALID queues.
08/24/10   cjb     Fixed compiler warnings.
07/28/10   vks     Use mapper functions to map between FW and SRCH4 variables
07/13/10   cjb     Fixed lint warnings/errors.
07/12/10   vks     Map SRCH4 variables to FW types and vice versa.
                   Mainline the 'string variables' as the corresponding enums
                   have been mailined as part of CMMI P4 (to fix logging)
07/07/10   vks     Add missed SRCH4_LEAVE_CRIT_SECT().
06/21/10   cjb     Featurization removal for CMMI-P4:
                   -> Moved APIs & related structs to make srch4drv.h internal
06/01/10   vks     Fix compiler warning - remove unused variable. 
05/28/10   vks     Merge from 1H09:
                   12/03/09   vks     Decouple 1x from SRCH4. Include only
                   srch_srch4.h from 1x and remove other includes. Make
                   appropriate changes.
                   11/20/09   vks     Define SRCH4_NOTUSED macro to avoid using
                   the one defined in srchi.h.
04/23/09   grl     Resolved HDR searcher results too low by 3 dB issue
                   Removed Previous 2 checkin for now
04/15/10   vks     Fix compiler errors. 
04/14/10   sst     Update start search to fill in peak_[comp/norm]_dis
12/29/09   vks     Replace INTLOCKs.
12/29/09   vks     Increase lost dump timer values for bringup.
11/19/09   vks     Add support for Abort response from SRCH4 FW
09/30/09   vks     Fix compiler warnings
09/17/09   vks     Fix lint errors
09/13/09   vks     Pass on pointer to the message instead of the entire
                   message to the srch4_process_dump() function
09/08/09   vks     Export functionality to update shared memory base address
07/29/09   vks     First set of changes for MSGR implementation
07/09/09   adw     Replace msm_help.h include with msm.h.
05/29/09   vks/adw Bring back Apollo Featurization
27/03/09   ns      Clean up to support SCMM and 8650 builds only
09/09/08   sst     Update interpolate peak alg for better results
09/08/08   adw     Added SRCH4_USE_LPQ, SRCH4_AFLT_ENABLED & SRCH4_HDR_ENABLED.
07/28/08   aps     T_MSM8650B bringup changes
06/20/08   mca     Merge from //depot
05/27/08   mca     Modified srch4 differentiate between normal 1x and offtl
                   searching
04/24/08   ljl     Added carrier info to srch4 log
                   Obsolete AFLT log pkt
12/14/07   sst     Correct featurization around Sched SR Flush
11/09/07   aps     Disable Srch fing rotator for 7800 as there is a rotator in
                    the RX front-end. This is the same change as T_QSC60X5
07/22/07   ljl     Supported multi-carriers.
07/20/07   rkc     srch4_start_search() uses freq_offset if technology is HDR
04/18/07   tjc     Lint fixes
04/12/07   sst     Changes to support new VCTCXO image for QSC60x5
03/07/07   aps     Moved INTLOCK() up in srch4_add_task() to handle any race
                    condition that may occur.
12/19/06   sst     Merge the following change from 11.02.02B branch
12/01/06   sst     Disable "freq offset" in srch4 for FEATURE_XO targets
10/23/06   jyw/awj Added interface to allow HDR fetch interpolated srch results.
09/25/06   awj     Moved RxD energy translation to trans layer
09/08/06   awj     Fixed RxD energy translation
08/21/06   aps/awj removed references to trunc parameter
06/08/06   awj     Remove encapsulation violation for pilot set info
04/26/06   tjc     Check for overflow condition in peak energy interpolation
01/16/06   awj     Lint cleanup
11/28/05   sst     For logging HDR/AFLT call RF RxAGC funcs
11/14/05   awj     Lint compliance
09/23/05   ejv     Deprecate rex timer types/calls.
09/13/05   sst     Lint fixes
08/23/05   ejv     Signal MDSP after each flush command.
08/11/05   sst     Make LOG_SRCH4_1X_C and SRCH_LOG_TNG_1X_SEARCHER_DUMP_C
                    log packets mutually exclusive
07/19/05   grl     Added flushing the sample ram of the requested chain.
06/13/05   asj     Modified mask for peak position calculation to retain lsb.
05/31/05   rng     Merged from SHDR Sandbox.
05/20/05   ejv     Remove srch_pn.h.
05/13/05   ejv     Cast SRCH_SUB_ID_x to uint8.
03/22/05   sst     Removed references to srch4drv_t.h, changed reference to
                    srch4_srch_type to uint32 in logging functions
03/10/05   ddh     Changed loop over queue to start at SRCH4_MIN_Q
02/08/05   bt      Put error checking into the energy interpolation.
02/04/05   bt      Add scheduled sample ram flush.
01/18/05   sst     Interpolate peak energy from shoulders
11/17/04   sst     Update function header block of srch4_halt
                   Synchronous abort all active searches in srch4_halt
10/29/04   sq      Do not shift search results for HDR diversity search
10/07/04   kwo     Renamed gen log packets to SRCH_LOG_TNG...
09/08/04   jcm     Rename exported genlog types to include srch_
09/03/04   jcm     Added generalized srch4 subpacket
09/03/04   jcm     Modified genlog to log multiple packet simultaneously
07/28/04   sst     Added check and correction for min window size
07/13/04   sst     Added SRCH4_NO_QUEUE = SRCH4_MAX_Q in srch4_queue_type
                   Changed SRCH4_MAX_TYPE to SRCH4_MAX_TECH in srch4_srch_type
                   Added SRCH4_NO_TECH = SRCH4_MAX_TECH in srch4_srch_type
07/13/04   ejv     Add RXDIV_SHIFT to scale RxDiv results.
07/12/04   sst     Added "active" check to srch4_queue_available()
                   Moved technology check around in srch4_abort_queue()
07/08/04   sst     Added an ERR message to indicate when no queue
                    (SRCH4_MAX_Q) was passed (indicates un-registered tech)
06/21/04   sst     Corrected naming of srch_mdsp_check_cmd_ functions
06/15/04   bt      Carry over cb_data when losing a dump.
05/26/04   sst     Fixed compile error in last checkin
05/26/04   sst     Update freq offset field in log packet to adjust for its
                    wrong size. Also add range checking to it.
05/26/04   sq      Fixed add_task's check for valid BURST_LEN_MAX
                   Removed extra whitespaces
05/25/04   sst     Added parameter to abort_cb to be able to pass number
                    of partial results returned
                   Updated handling of lost dumps, so lost dumb cb is not
                    assigned to the abort dump cb on lost dump abort
05/04/04   sst/va  Merge from srch4 3Q dev branch (dev-changes)
04/26/04   sst     dev-Changed SRCH4DRV_RET_FREE to be queue specific
04/20/04   sst     Moved tech check in abort to top of if statements
04/14/04   sst     dev-Added partial results param to srch4_abort_queue()
                   dev-Added partial results processing
                   dev-Added ptr to task struct inside queue struct
                   dev-Moved all no-err status dump messages out to
                     srch4_status_dump()
04/12/04   sst     dev-Updated abort processing
04/08/04   sst     dev-Created SRCH4_HAS_HPQ1, SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE
                   dev-Created SRCH4_DEBUG, SRCH4_DEBUG_MSG()
03/25/04   sst     dev-Added string srch4_prio_str
                   dev-Moved calling of dump_cb until after all flags are cleared
03/11/04   sst     dev-Changed logging of priority field
02/25/04   sst     More lint corrections
03/04/04   sst     dev-Added function srch4_halt() to "virtually stop" srch4
03/02/04   sst     dev-Sample ram update
02/19/04   sst     Lint corrections
                   Changed srch4_tasks_left() to return int32
02/19/04   sst     Moved calling of rex_set_timer() in start and abort to
                    prevent a task switch until after all flags are set
02/02/04   sst     Added synchronous field to srch4_abort_queue() to stall
                    until the abort has completed
01/28/04   ejv     Mainline T_MSM6500.
01/20/04   sst     Removed log_free() calls in srch4_status_dump()
01/07/03   bt      Add missing INTFREE_SAV in srch4_tasks_left.
12/31/03   bt      Allocate log packet close to committing it and fix
                    truncation log param for MSM6500.
12/15/03   sst     Added a technology parameter to srch4_tasks_left()
12/03/03   ajn     Abort: Check if SRCH4 is active before check if wrong tech
11/05/03   bt      Check whether different technologies try to abort
                    each other and remove srch4_abort.
11/04/03   bt      Moved SRCH4/MDSP interface into srch4trans.
09/22/03   sst     Reworked coh_trunc field of the log packet
09/21/03   bt      Add MDSP download functionality.
09/09/03   sst     Fixed handling of invalid window sizes
07/31/03   sst     Fixed HPQ->LPQ typo
07/30/03   ejv     Removed sample server references.
07/29/03   abh     Added routine for changing Sample Server back to 1X mode
                    after tuneback from GPS. Also added some debug capability
                    for sample ram.
07/14/03   sq      Lowered message level for "upexpected srch4 dump".
07/17/03   sst     Updated srch4 log pkt, added rx_agc0/1, removed +1's from
                    num_peaks and num_tasks, now using multiple log codes
                    based on technology
07/14/03   ejv     Remove unnecessary extern.
07/01/03   sst     Added LOG_SRCH4_C log packet and processing
                   Added freq_offset, QPCH, and logging info to queue struct
06/27/03   ejv     Fixed HPQ->LPQ typo.  Update "7 Cx8" comment.
06/23/03   va      Interpolation algorithm fix
05/23/03   sst     Initialized var to remove warning
04/25/03   sst     Corrected catching lost dumps and using the callback func
04/14/03   sst     Added catch for minimum and maximum window sizes
03/24/03   sst     Modified peak position calcs to return pos in Cx16
02/18/03   va      Fixed LPQ support
01/13/03   sst     Added separate abort lost timer, moved srch4_trig_dmp_err,
                    added return values for most functions, chafed handling
                    of lost dumps
12/20/02   sst     Fixed INTLOCK error in srch4_tasks_left()
12/05/02   sst     Code Clean up
11/14/02   sst     Fixed dump logic error, Using srch4 return enum values
                   Removed unused timers
11/11/02   sst     Removed srch4_remove_last_task, deemed unneeded
11/11/02   sst     Code cleanup, Added srch4_remove_last_task
11/07/02   sst     Added support for Jaguar, moved coh_tr here
                   Code cleanup
11/05/02   sst     Merge changes, Combined HPQ and LPQ funcs to reduce
                    code duplication
11/04/02   sst     Moved srch4 win center functionality to srch4trans
10/30/02   sst     Added support for srch4's own window center var
10/30/02   sst     Implemented lost dumps and their callbacks (untested)
10/29/02   sst     Removed temporary functions
                   Separated file, created srch4trans.c .h
                   Made function calls more generic to queues
10/18/02   sst     Temporary functions to "ease" transition to tasks
10/18/02   sst     Changed HPQ to use generic task structure
                   Added support for Transmit Diversity in 1x searching
                   Added translation functions (skeletons) for 1x, HDR, AFLT
10/08/02   sst     Added support for callback functions upon dump/lost dump
                   Fixed implementation of searcher 4 aborts
10/04/02   sst     Added support for 4 & 6 searcher peaks
10/01/02   sst     Added $Header line
10/01/02   sst     Moved ERR's and MSG's out of INTLOCK context
09/26/02   sst     Removal of debug functions.
                   MSM6100 implementation, testing, and code review changes
08/02/02   gs      Initial merge for MSM6100.
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
#include "srch4drv.h"
#include "srch4drv_t.h"
#include "srch4trans_t.h"

#ifdef FEATURE_MODEM_COEXISTENCE_FW
  #include "srch4i.h"
#ifdef FEATURE_BOLT_MODEM
  #include "vstmr.h"
#endif
#endif

/* FW */
#include "srchfw_msg.h"

/* MSGR */
#include "msgr.h"

/* 1X */
#include "srch_genlog.h"
#include "srch_srch4.h"

/* HDR */

/* Other */
#include "assert.h"
#include "err.h"
#include "IxErrno.h"
#include "log.h"
#include "msg.h"
#include "msgcfg.h"
#include "rex.h"
#include "timetick.h"
#include "stringl.h"

/* FEATURE DEPENDENT */
#ifdef SRCH4_DEBUG
  #error code not present
#endif
#include "timer.h"

#ifdef FEATURE_RF_SVDO_API
  #include "rfm_1x.h"
  #include "rfm_hdr.h"
#else
  #include "rfcom.h"
#endif /* FEATURE_RF_SVDO_API */

#ifdef FEATURE_MODEM_COEXISTENCE_FW
// ToDo: might need to remove
  #include "msmhwioreg.h"
  #include "mcs_hwio_modem.h"
#endif


/*===========================================================================
             LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*-------------------------------------------------------------------------
      Constants and Macros
-------------------------------------------------------------------------*/

/* Status dump timeouts for lost searches in milliseconds */
/* The following formula is used to estimate lost dump times:
   (srch duration) = (setup times) +
                     ( (Win Size)/4 * (Coh_int + 18) * NonCoh_int * 1.5 ) /
                       (srch4 clock speed in KHz)
    (taken from HDR) */

/* window size to separate "Normal" and "Max" timeouts */
#define SRCH4_WINSIZE_NORM_TIMEOUT_LIMIT     500

/* SRCH4 HPQ lost dump times (per search) */
/* searches with window size < SRCH4_WINSIZE_NORM_TIMEOUT_LIMIT
   (standard allows max of 452 for normal searches)
    derived from window size = 500, coh = 512, noncoh = 1 */
#define SRCH4_HPQ_DUMP_TIMEOUT_NORMAL        80

/* Normal srch4 lpq timeout estimate */
uint16 srch4_lpq_normal_timeout_estimate = 2000;

/* searches with window size > SRCH4_WINSIZE_NORM_TIMEOUT_LIMIT
   (currently acq searches have window sizes >
                                        SRCH4_WINSIZE_NORM_TIMEOUT_LIMIT)
    derived from window size = 8192, coh = 2048, noncoh = 1 */
#define SRCH4_HPQ_DUMP_TIMEOUT_MAX          300

#define IS_QUEUE_ACTIVE(queue) (((queue)->active) || ((queue)->scheduled_for_search))

/* parameter limits */
#define WINSIZ_MIN              4  /* minimum valid window size (chips)      */
#define WINSIZ_MAX          32767  /* maximum valid window size (chips)      */

#define COH_LEN_MIN            48  /* minimum valid Coherent Length          */
#define COH_LEN_MAX          3584  /* maximum valid Coherent Length          */
#define COH_LEN_AFLT_MAX     2048  /* maximum valid AFLT Coherent Length     */

#define NON_COH_LEN_MIN         1  /* minimum valid Non-Coherent Length      */
#define NON_COH_LEN_MAX       127  /* maximum valid Non-Coherent Length      */
#define NON_COH_LEN_HDR_MAX     2  /* maximum valid HDR Non-Coherent Length  */

#define M_X_N_MAX            3948  /* maximum valid
                                       Coh Len (M) * Non Coh Len (N)         */

#define BURST_LEN_MAX           2  /* maximum valid Burst Length (HDR only)  */


#define MAX_RESCHEDULE_TIME_IN_MICROSEC   8000 /* maximum gap between two reschedule */

#define SRCH4_BOUNDS_CHECK(value, bound)  if(value >= bound) \
                                          ERR_FATAL("Bounds check failed. Value: %d, Bound: %d", \
                                          (value), (bound), 0);

/* strings for F3 messages */

/* technology string, see srch4_srch_type */
const char srch4_tech_str[ ( (uint16) SRCH4_NUM_TECH + 1 ) ][ 6 ] =
{
  "1X",                                /* SRCH4_1X             */
  "HDR_S",                             /* SRCH4_HDR_SHOULDERS  */
  "NA",                                /* not defined          */
  "HDR_A",                             /* SRCH4_HDR_ACCUM      */
  "AFLT",                              /* SRCH4_AFLT           */
  "None"                               /* SRCH4_NO_TECH        */
};

/* queue name string, see srch4_queue_type */
const char srch4_queue_str[ ( (uint16) SRCH4_MAX_Q + 1 ) ][ 5 ] =
{
  "HPQ0",                              /* SRCH4_HPQ0           */
  "LPQ",                               /* SRCH4_LPQ            */
  "HPQ1",                              /* SRCH4_HPQ1           */
  "None"                               /* SRCH4_MAX_Q          */
};

const char srch4_prio_str[ (uint16) SRCH4_PRIO_MAX ][ 10 ] =
{
  "MIN",                               /* min priority                   */
  "AFLT",                              /* AFLT                           */
  "ACQ",                               /* HDR Acq                        */
  "ACQ",                               /* 1x Acq                         */
  "TC-NS",                             /* HDR Traffic, HDR Non Slotted,
                                          1x Traffic, 1x Non Slotted     */
  "OFREQ-CFS",                         /* HDR Traffic CFS, HDR Ofreq,
                                          1x Traffic CFS, 1x Ofreq       */
  "REACQ_BC",                          /* HDR Reacq Broadcast            */
  "REACQ_BC",                          /* 1x Reacq Broadcast             */
  "REACQ_PG",                          /* HDR Reacq Paging               */
  "REACQ_PG"                           /* 1x Reacq Paging                */
};

/* Shift value to use on results when RxDiv is used for searching */
#define RXDIV_SHIFT  1

/* Version number for Search4 subpacket */
#define LOG_VERSION_SRCH4_DUMP  1

/* size of the header block (minus task) */
#define SRCH4_SUB_LOG_HEADER_SIZ  (FPOS(srch_genlog_srch4_sub_v1,tasks))

/* size of the parameters (minus the results) */
#define SRCH4_SUB_LOG_PARAM_SIZ  (FPOS(srch_genlog_srch4_task_type,results))

/* size of the results */
#define SRCH4_SUB_LOG_RESULTS_SIZ  (sizeof(srch_genlog_srch4_result_type))

#define SRCH4_SUB_LOG_SIZ(tasks,peaks) (SRCH4_SUB_LOG_HEADER_SIZ +        \
                             ((tasks) * (SRCH4_SUB_LOG_PARAM_SIZ +        \
                              ((peaks) * SRCH4_SUB_LOG_RESULTS_SIZ))))

/* checks the interpolated peak energy for saturation */
#define SRCH4_PEAK_ENG_SAT(peak_eng) \
  ((uint16)(((peak_eng) > 0xffff) ? 0xffff : (peak_eng)))

/*-------------------------------------------------------------------------
      Variables
-------------------------------------------------------------------------*/

/* searcher 4 task buffers */
static srch4_task_struct_type    srch4_hpq0_tasks[SRCH4_MAX_HPQ_TASKS];
static srch4_task_struct_type    srch4_lpq_tasks[SRCH4_MAX_LPQ_TASKS];
static srch4_task_struct_type    srch4_hpq1_tasks[SRCH4_MAX_HPQ_TASKS];

/* Rex lost dump timer for searcher queues */
static timer_type            srch4_hpq0_timer;
static timer_type            srch4_lpq_timer;
static timer_type            srch4_hpq1_timer;

#ifdef FEATURE_MODEM_COEXISTENCE_FW
/* used by HPQ0 to set Partial Dump Timer when GTX is active */
static timer_type            srch4_hpq0_schedule_task_timer;
/* used by LPQ0 to set Partial Dump Timer when GTX is active */
static timer_type            srch4_lpq_schedule_task_timer;
/* used by HPQ1 to set Partial Dump Timer when GTX is active */
static timer_type            srch4_hpq1_schedule_task_timer;
#endif

/* for searcher4 logging */
static LOG_SRCH4_C_type          srch4_hpq0_log_pkt;
static srch_genlog_srch4_sub_v1  srch4_hpq0_sub_log_pkt;
static LOG_SRCH4_C_type          srch4_lpq_log_pkt;
static srch_genlog_srch4_sub_v1  srch4_lpq_sub_log_pkt;

static LOG_SRCH4_C_type          srch4_hpq1_log_pkt;
static srch_genlog_srch4_sub_v1  srch4_hpq1_sub_log_pkt;


/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/

typedef struct
{
  srchfw_sw_shared_memory_interface_t *base_addr;
  /* Base address of the memory from
     which FW would be reading         */
} srch4_mem_map_var_type;

/* searcher 4 structure */
typedef struct
{
  srch4_queue_var_type hpq0;     /* var. struct for high priority queue 0 */
  srch4_queue_var_type lpq;      /* var. struct for low priority queue    */

  srch4_queue_var_type hpq1;     /* var. struct for high priority queue 1 */

  srch4_mem_map_var_type mem_map;/* var. struct for addresses             */
} srch4_var_type;

/* searcher 4 variable structure */
static srch4_var_type srch4 =
{
  /* HPQ 0 */
  {
    SRCH4_1X,   /* initially set to 1x                                */
    0,          /* initialize freq offset to 0                        */
    0,          /* initialize search priority to 0                    */
    FALSE,      /* initialize qpch bit to 0                           */
    FALSE,      /* initially not active                               */
    FALSE,      /* initially not aborted                              */
    FALSE,      /* initially not lost                                 */
    FALSE,      /* initially partial results are not wanted           */
    0,          /* # of tasks currently programmed                    */
    SRCH4_MAX_HPQ_TASKS,
    /* maximum number of tasks available                  */
    0,          /* num of tasks in last search "0"                    */
    0,          /* number of search results returned from last search */
    0,          /* total number of searches (whole queue)             */
    0,          /* total number of aborted searches                   */
    NULL,       /* callback_func pointer                              */
    NULL,       /* lost_dump_func pointer                             */
    NULL,       /* abort_dump_func pointer                            */
    NULL,       /* pointer callback function parameter                */
    0,          /* estimated time to set lost dump timeout            */
    &srch4_hpq0_timer,
    /* ptr to lost dump timer                             */
    NULL,       /* ptr to log pkt alloc'ed by diag                    */
    &srch4_hpq0_log_pkt,
    /* ptr to srch4 log pkt                               */
    0,          /* size of the log pkt                                */
    &srch4_hpq0_sub_log_pkt,
    /* ptr to srch4 log pkt                               */
    &(srch4_hpq0_tasks[0]),
    /* ptr to the tasks struct                            */
#ifdef FEATURE_MODEM_COEXISTENCE_FW
    &srch4_hpq0_schedule_task_timer,
#endif
    FALSE, /* scheduled for search*/
  },
  /* LPQ */
  {
    SRCH4_1X,   /* initially set to 1x                                */
    0,          /* initialize freq offset to 0                        */
    0,          /* initialize search priority to 0                    */
    FALSE,      /* initialize qpch bit to 0                           */
    FALSE,      /* initially not active                               */
    FALSE,      /* initially not aborted                              */
    FALSE,      /* initially not lost                                 */
    FALSE,      /* initially partial results are not wanted           */
    0,          /* # of tasks currently programmed                    */
    SRCH4_MAX_LPQ_TASKS,
    /* maximum number of tasks available                  */
    0,          /* num of tasks in last search "0"                    */
    0,          /* number of search results returned from last search */
    0,          /* total number of searches (whole queue)             */
    0,          /* total number of aborted searches                   */
    NULL,       /* callback_func pointer                              */
    NULL,       /* lost_dump_func pointer                             */
    NULL,       /* abort_dump_func pointer                            */
    NULL,       /* pointer callback function parameter                */
    0,          /* estimated time to set lost dump timeout            */
    &srch4_lpq_timer,
    /* ptr to lost dump timer                          */
    NULL,       /* ptr to log pkt alloc'ed by diag                    */
    &srch4_lpq_log_pkt,
    /* ptr to srch4 log pkt                               */
    0,          /* size of the log pkt                                */
    &srch4_lpq_sub_log_pkt,
    /* ptr to srch4 log pkt                               */
    &(srch4_lpq_tasks[0]),
    /* ptr to the tasks struct                            */
#ifdef FEATURE_MODEM_COEXISTENCE_FW
    &srch4_lpq_schedule_task_timer,   
#endif
    FALSE, /* scheduled for search*/
  },

  /* HPQ 1 */
  {
    SRCH4_1X,   /* initially set to 1x                                */
    0,          /* initialize freq offset to 0                        */
    0,          /* initialize search priority to 0                    */
    FALSE,      /* initialize qpch bit to 0                           */
    FALSE,      /* initially not active                               */
    FALSE,      /* initially not aborted                              */
    FALSE,      /* initially not lost                                 */
    FALSE,      /* initially partial results are not wanted           */
    0,          /* # of tasks currently programmed                    */
    SRCH4_MAX_HPQ_TASKS,
    /* maximum number of tasks available                  */
    0,          /* num of tasks in last search "0"                    */
    0,          /* number of search results returned from last search */
    0,          /* total number of searches (whole queue)             */
    0,          /* total number of aborted searches                   */
    NULL,       /* callback_func pointer                              */
    NULL,       /* lost_dump_func pointer                             */
    NULL,       /* abort_dump_func pointer                            */
    NULL,       /* pointer callback function parameter                */
    0,          /* estimated time to set lost dump timeout            */
    &srch4_hpq1_timer,
    /* ptr to lost dump timer                             */
    NULL,       /* ptr to log pkt alloc'ed by diag                    */
    &srch4_hpq1_log_pkt,
    /* ptr to srch4 log pkt                               */
    0,          /* size of the log pkt                                */
    &srch4_hpq1_sub_log_pkt,
    /* ptr to srch4 log pkt                               */
    &(srch4_hpq1_tasks[0]),
    /* ptr to the tasks struct                            */
#ifdef FEATURE_MODEM_COEXISTENCE_FW
    &srch4_hpq1_schedule_task_timer,
#endif
    FALSE, /* scheduled for search*/
  },

  /* Mem map */
  {
    NULL,
  },
};

#define TASK_PARMS_BASE_ADDR   srch4.mem_map.base_addr->task_params
#define AFLT_PARMS_BASE_ADDR   srch4.mem_map.base_addr->aflt_task_params
#define SRCH_RESULTS_BASE_ADDR srch4.mem_map.base_addr->srch_results
#define AFLT_RESULTS_BASE_ADDR srch4.mem_map.base_addr->aflt_srch_results

/* Srch4 history buffer */
#define SRCH4_MAX_HISTORY_ENTRIES  50

typedef enum
{
  FN_NONE,
  FN_L1_START_SRCH,
  FN_FW_PROCESS_DUMP,
  FN_L1_ABORT_SRCH,
  FN_SRCH4_ABORT_SRCH,
  FN_SCHED_TIMER_EXPIRY,
  FN_CXM_UPDATE_GTX
}srch4_activity_fn_type;

typedef struct 
{
  srch4_queue_var_type queue_copy;
  srch4_time_slot_type  cxm_time_slot;
  uint32   current_ustmr;
  srch4_activity_fn_type activity_fn;
}srch4_history_buffer_type;

srch4_history_buffer_type srch4_history_buffer[SRCH4_MAX_HISTORY_ENTRIES];
uint8 srch4_history_write_index = 0;

#ifdef FEATURE_MODEM_COEXISTENCE_FW
extern srch4i_slottedscheduler_type srch4i_slottedscheduler_info;
#endif

/*===========================================================================

FUNCTION       SRCH4_INSERT_HISTORY_REC
DESCRIPTION    This function inserts a record in the history buffer
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
void srch4_insert_history_rec
(
  srch4_queue_var_type* q,
  srch4_activity_fn_type activity_fn
)
{
  memscpy(&srch4_history_buffer[srch4_history_write_index].queue_copy, 
         sizeof(srch4_queue_var_type),
         q,
         sizeof(srch4_queue_var_type));

  srch4_history_buffer[srch4_history_write_index].activity_fn = activity_fn;
  
  #ifdef FEATURE_MODEM_COEXISTENCE_FW
  srch4_history_buffer[srch4_history_write_index].current_ustmr = 
                             srch4i_slottedscheduler_get_current_ustmr();
  srch4i_query_current_inactivity_info(&srch4_history_buffer[srch4_history_write_index].cxm_time_slot);
  #endif
  
  srch4_history_write_index = (srch4_history_write_index + 1) % SRCH4_MAX_HISTORY_ENTRIES;
}


#ifdef SRCH4_DEBUG
#error code not present
#endif /* SRCH4_DEBUG */


/*===========================================================================
              FUNCTION DEFINITIONS FOR MODULE
===========================================================================*/

/*===========================================================================

              S I M P L I F I C A T I O N   F U N C T I O N S

===========================================================================*/
/*===========================================================================

FUNCTION       SRCH4_MAP_SRCH4_QUEUE_TO_FW_QUEUE
DESCRIPTION    This function maps the srch4 queue type to fw queue type.
DEPENDENCIES   None

RETURN VALUE   FW queue type
SIDE EFFECTS   None

===========================================================================*/
uint16 srch4_map_srch4_queue_to_fw_queue
(
srch4_queue_type         queue_type
)
{
  uint16 fw_queue_num = 0;

  /*-----------------------------------------------------------------------*/

  switch ( queue_type )
  {
  case SRCH4_HPQ0:
    {
      fw_queue_num = 0;
      break;
    }
  case SRCH4_HPQ1:
    {
      fw_queue_num = 1;
      break;
    }
  case SRCH4_LPQ:
    {
      fw_queue_num = 2;
      break;
    }   
  default:
    {
      ERR_FATAL("Requesting invalid queue type (%d)", queue_type, 0, 0);
    }
  }

  return( fw_queue_num );

} /* srch4_map_srch4_queue_to_fw_queue */

/*===========================================================================

FUNCTION       SRCH4_MAP_FW_QUEUE_TO_SRCH4_QUEUE
DESCRIPTION    This function maps the firmware queue type to srch4 queue type.
DEPENDENCIES   None

RETURN VALUE   SRCH4 queue type
SIDE EFFECTS   None

===========================================================================*/
srch4_queue_type srch4_map_fw_queue_to_srch4_queue
(
uint16 fw_queue_num
)
{
  srch4_queue_type queue_type = SRCH4_NO_QUEUE;


  /*-----------------------------------------------------------------------*/

  switch ( fw_queue_num )
  {
  case 0:
    {
      queue_type = SRCH4_HPQ0;
      break;
    }
  case 1:
    {
      queue_type = SRCH4_HPQ1;
      break;
    }
  case 2:
    {
      queue_type = SRCH4_LPQ;
      break;
    }   
  default:
    {
      ERR_FATAL("Invalid FW queue type (%d)", fw_queue_num, 0, 0);
    }
  }

  return( queue_type );

} /* srch4_map_fw_queue_to_srch4_queue */

/*===========================================================================

FUNCTION       SRCH4_MAP_SRCH4_TECH_TO_FW_TECH_TYPE
DESCRIPTION    This function maps the SRCH4 tech type to firmware tech type.
DEPENDENCIES   None

RETURN VALUE   FW tech type
SIDE EFFECTS   None

===========================================================================*/
srch_type_t srch4_map_srch4_tech_to_fw_tech_type
(
srch4_srch_type srch_type
)
{
  srch_type_t fw_tech_type = CDMA_1X;  

  /*-----------------------------------------------------------------------*/

  if ( ( srch_type == SRCH4_1X)           ||
  ( srch_type == SRCH4_1X_NORM_MASK) ||
  ( srch_type == SRCH4_1X_OFFTL_MASK) )
  {
    fw_tech_type = CDMA_1X;
  }
  else if ( ( srch_type == SRCH4_HDR_SHOULDERS ) ||
  ( srch_type == SRCH4_HDR_ACCUM ) )
  {
    fw_tech_type = HDR;
  }
  else if ( srch_type == SRCH4_AFLT )
  {
    fw_tech_type = AFLT;
  }
  else
  {
    /* Invalid srch4 srch type */
    ERR_FATAL("Invaild srch4 srch type %d", srch_type, 0, 0);    
  }

  return( fw_tech_type );

} /* srch4_map_srch4_tech_to_fw_tech_type */



/*===========================================================================

FUNCTION       SRCH4_GET_QUEUE
DESCRIPTION    This function returns the requested queue.
DEPENDENCIES   Must check for a NULL return

RETURN VALUE   Ptr to requested queue structure
SIDE EFFECTS   None

===========================================================================*/
srch4_queue_var_type *srch4_get_queue
(
srch4_queue_type         queue_type         /* queue                 */
)
{
  srch4_queue_var_type *queue;

  /*-----------------------------------------------------------------------*/

  switch ( queue_type )
  {
  case SRCH4_HPQ0:
    {
      queue = &srch4.hpq0;
      break;
    }
  case SRCH4_LPQ:
    {
      queue = &srch4.lpq;
      break;
    }

  case SRCH4_HPQ1:
    {
      queue = &srch4.hpq1;
      break;
    }

  default:
    {
      /* error checking must be done outside this function */
      queue = NULL;
      break;
    }
  }

  return( queue );

} /* srch4_get_queue */


/*===========================================================================

FUNCTION       SRCH4_GET_TASKS
DESCRIPTION    This function returns the requested task structure.
               In the srch4 drivers, queue->tasks can also be used.
DEPENDENCIES   None

RETURN VALUE   Ptr to requested task structure
SIDE EFFECTS   None

===========================================================================*/
srch4_task_struct_type *srch4_get_tasks
(
srch4_queue_type queue_type            /* queue to start                 */
)
{
  srch4_task_struct_type *tasks;

  /*-----------------------------------------------------------------------*/

  switch ( queue_type )
  {
  case SRCH4_HPQ0:
    {
      tasks = srch4.hpq0.tasks;
      break;
    }
  case SRCH4_LPQ:
    {
      tasks = srch4.lpq.tasks;
      break;
    }

  case SRCH4_HPQ1:
    {
      tasks = srch4.hpq1.tasks;
      break;
    }

  default:
    {
      MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
      " Requesting invalid queue type (%d)", queue_type );
      tasks = NULL;
      break;
    }
  }

  return( tasks );

} /* srch4_get_tasks */


/*===========================================================================

                   U T I L I T Y   F U N C T I O N S

===========================================================================*/
/*===========================================================================

FUNCTION       SRCH4_QUEUE_AVAILABLE
DESCRIPTION    This function checks to see if a specified queue of
                searcher 4 is available/empty
DEPENDENCIES   This should be called from within a mutex lock

RETURN VALUE   TRUE   if the queue is available
               FALSE  if the queue is active or being programmed
SIDE EFFECTS   None

===========================================================================*/
boolean srch4_queue_available
(
srch4_queue_type        queue_type      /* queue to check            */
)
{
  srch4_queue_var_type   *queue;          /* to accommodate queues     */

  /*-----------------------------------------------------------------------*/

  /* grab the queue */
  queue = srch4_get_queue( queue_type );

  /* check to ensure a valid queue has been assigned */
  if ( queue == NULL )
  {
    return( FALSE );
  }

  /* check if the queue is active or being programed, i.e num of tasks > 0 */
  MSG_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Queue Active Info. Active: %d, Scheduled: %d ",
    queue->active, queue->scheduled_for_search );

  if ( ( queue->num_tasks > 0 ) ||
  ( IS_QUEUE_ACTIVE(queue) ) )
  {
    return( FALSE );
  }

  /* this queue is free */
  return( TRUE );

} /* srch4_queue_available */

/*===========================================================================

FUNCTION       SRCH4_QUEUE_IS_VALID
DESCRIPTION    This function checks to see if a specified queue of
                searcher 4 is valid
DEPENDENCIES   None

RETURN VALUE   TRUE   if the queue is valid
               FALSE  if the queue is invalid
SIDE EFFECTS   None

===========================================================================*/
boolean srch4_queue_is_valid
(
srch4_queue_type        queue_type      /* queue to check            */
)
{
  /*-----------------------------------------------------------------------*/

  switch ( queue_type )
  {
  case SRCH4_HPQ0:
    {
      return( TRUE );
    }
  case SRCH4_LPQ:
    {
      return( TRUE );
    }

  case SRCH4_HPQ1:
    {
      return( TRUE );
    }

  default:
    {
      /* Invalid queue */
      return( FALSE );
    }
  }
} /* srch4_queue_is_valid */


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
int32 srch4_tasks_left
(
srch4_srch_type         task_srch_type, /* search type               */
srch4_queue_type        queue_type      /* queue to check            */
)
{
  int32                   tasks_left = 0; /* return value              */
  srch4_queue_var_type   *queue;          /* to accommodate queues     */

  /*-----------------------------------------------------------------------*/

  /* check if this technology was not assigned a queue */
  if ( queue_type == SRCH4_NO_QUEUE )
  {
    /* this is not right, mark this as an ERR */
    MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,  
    "Tech: %d tried to req tasks left when not registered", 
    task_srch_type );

    return( (int32) SRCH4DRV_RET_INVALID_QUEUE );
  }

  /* check which queue to use, and store ptr */
  queue = srch4_get_queue( queue_type );

  /* check to ensure a valid queue has been assigned */
  if ( queue == NULL )
  {
    return( (int32) SRCH4DRV_RET_INVALID_QUEUE );
  }

  SRCH4_ENTER_CRIT_SECT();

  MSG_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
  "Queue Active Info. Active: %d, Scheduled: %d ",
  queue->active, queue->scheduled_for_search );


  /* Is this queue active, and assigned to the requesting technology? */
  if (  (IS_QUEUE_ACTIVE(queue)) && ( queue->srch_type == task_srch_type ) )
  {
    if ( queue->lost || queue->aborted )
    {
      /* Somehow we lost a search before and it wasn't cleaned up! */
      SRCH4_LEAVE_CRIT_SECT();
      return( (int32) SRCH4DRV_RET_UNCLEAN );
    }
    else
    {
      /* searcher queue is active, can't do anything till dump */
      SRCH4_LEAVE_CRIT_SECT();
      return( (int32) SRCH4DRV_RET_ACTIVE );
    }
  }

  tasks_left = queue->max_num_tasks - (int32)queue->num_tasks;

  SRCH4_LEAVE_CRIT_SECT();

  if ( tasks_left < 0 )
  {
    /* something is terribly wrong! */
    MSG_3( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "More than max # tasks proged(%d)(%d/%d)",
    queue, queue->num_tasks, queue->max_num_tasks );

    return( (int32) SRCH4DRV_RET_FATAL );
  }
  /* this queue is empty, shouldn't matter who owns the queue
     it is assumed here that the tech requesting this queue is assigned
     to it in srch4_tech_mappings in srch4trans.c */
  else if ( tasks_left == queue->max_num_tasks )
  {
    switch ( queue_type )
    {
    case SRCH4_HPQ0:
      {
        return( (int32) SRCH4DRV_RET_HPQ0_FREE );
      }
    case SRCH4_LPQ:
      {
        return( (int32) SRCH4DRV_RET_LPQ_FREE );
      }

    case SRCH4_HPQ1:
      {
        return( (int32) SRCH4DRV_RET_HPQ1_FREE );
      }

    default:
      {
        return( (int32) SRCH4DRV_RET_INVALID_QUEUE );
      }
    }
  }
  /* a different technology owns this queue */
  else if ( queue->srch_type != task_srch_type )
  {
    return( (int32) SRCH4DRV_RET_TECH_MISMATCH );
  }

  /* return how many tasks we can still program */
  return( tasks_left );

} /* srch4_tasks_left */


/*===========================================================================

FUNCTION       SRCH4_INTERPOLATE_PEAK
DESCRIPTION    This function interpolates the peak position and energy from
                the searcher data by using the shoulder energies
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
static void srch4_interpolate_peak
(
int32  pos,                  /* peak position in Cx8 with Cx2 res       */
uint32 eng,                  /* peak energy                             */
uint32 ls_eng,               /* left shoulder peak energy               */
uint32 rs_eng,               /* right shoulder peak energy              */
int32  *pos_int,             /* interpolated peak position (Cx16)       */
uint16 *eng_int              /* interpolated peak energy                */
)
{
  int32  num      = 0;         /* Numerator of interpolation algorithm    */
  int32  den      = 0;         /* Denominator of interpolation algorithm  */
  int32  offset   = 0;         /* calculated offsets                      */

/*-----------------------------------------------------------------------*/

  if ( ( pos_int == NULL ) || ( eng_int == NULL ) )
  {
    MSG( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "srch4_interpolate_peak got NULL ptrs" );

    return;
  }

  /* Change the pos to Cx16 for higher resolution*/
  pos *= 2;

  /* Set default position (in Cx16) and Energy */
  *pos_int = pos;
  *eng_int = SRCH4_PEAK_ENG_SAT(eng);

  /* prevent division by 0 */
  if ( ( ls_eng + rs_eng ) != ( 2 * eng ) )
  {

    /* use quadratic interpolation to determine the Cx16 position
       and energy of the peak */

    num = 2 * ( ls_eng - rs_eng );
    den = rs_eng + ls_eng - ( 2 * eng );

    /* calculate the offset (Cx32) of the peak wrt the peak position */
    offset  = ( 4 * num ) / den;

    /* Make bias proportional to offset */
    offset += offset * ( 8 - SRCH4_ABS( offset ) ) / 8;

    /* round down to Cx16 */
    offset  = ( offset < 0 ) ? ( ( offset - 1 ) / 2 ) : ( offset + 1 ) / 2;

    /* apply the offset */
    *pos_int = pos + offset;

    /* compute energy */
    if ( offset < 0 )
    {
      *eng_int = SRCH4_PEAK_ENG_SAT(
      eng +
      ( ( ( ( (int32)eng - (int32)rs_eng ) *
      ( offset * offset ) ) /
      ( 4 - offset ) ) / 16 ) );
    }
    else if ( offset > 0 )
    {
      *eng_int = SRCH4_PEAK_ENG_SAT(
      eng +
      ( ( ( ( (int32)eng - (int32)ls_eng ) *
      ( offset * offset ) ) /
      ( 4 + offset ) ) / 16 ) );
    }
    /* offset = 0, return the inital peak energy value */
  }

} /* srch4_interpolate_peak */

#ifdef FEATURE_MODEM_COEXISTENCE_FW
/*============================================================================

FUNCTION SRCH4I_SLOTTEDSCHEDULER_INIT

DESCRIPTION
  This function initializes the internal variables required for slotted
  scheduling of searches.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.  

============================================================================*/
void srch4_schedule_search(
srch4_queue_var_type  *queue           /* to accommodate queues          */
)
{

  srchfw_search_cmd_msg_t search_cmd;
  errno_enum_type         estatus;        /* error code                     */

/*-----------------------------------------------------------------------*/


  /* Set the variables to zero */
  memset( &search_cmd, 0, sizeof( srchfw_search_cmd_msg_t ) );

  /* Initialize the message header */
  msgr_init_hdr( &search_cmd.hdr,
  MSGR_CDMA_SRCH_FW,
  CDMA_SRCH_FW_CMD_SRCH_START );

  /* initialize time slot info */
  search_cmd.dsda_inactivity_start_ustmr = 0;
  search_cmd.dsda_inactivity_end_ustmr   = 0;
  search_cmd.max_sample_reqd_cx1 = 0;        


  /* Fill up the SRCH CMD message */
  search_cmd.queue       = srch4_map_srch4_queue_to_fw_queue( queue->queue_type );
  search_cmd.search_type = srch4_map_srch4_tech_to_fw_tech_type(
  queue->srch_type );
  search_cmd.num_tasks   = queue->num_tasks_scheduled - queue->num_completed_search;
  search_cmd.priority    = queue->priority;

  /* When technology is HDR, set the frequency offset, e.g. for binning */
  if ( ( queue->srch_type == SRCH4_HDR_SHOULDERS ) ||
  ( queue->srch_type == SRCH4_HDR_ACCUM ) )
  {
    /* program the frequency offset to apply to all searches */
    search_cmd.freq_offset = queue->freq_offset;
  }
  else
  {
    /* For non-HDR technologies, the search rotator should not be used
       as there is a Rotator in the RX front */
    search_cmd.freq_offset = 0;
  }

  /* For 1x and HDR SHOULDERS comp_dis = 0, for HDR ACCUM comp_dis = 1 */
  search_cmd.peak_comp_dis = 0;
  search_cmd.peak_norm_dis = 0;

  /* Set the mode and RTC tick... */
  switch (queue->mode)
  {
  case SRCH4_DSDA_OFFLINE:
    search_cmd.srch_mode = DSDA_OFFLINE;

    /* copy time slot info */
    search_cmd.dsda_inactivity_start_ustmr = queue->time_slot_start;

    search_cmd.dsda_inactivity_end_ustmr   = SRCH4_APPLY_OVERRUN(
    (queue->time_slot_start + queue->time_slot_duration));

    search_cmd.max_sample_reqd_cx1 = (queue->time_slot_lead_duration * 1000)/ 814;

    break;


  case SRCH4_MODE_HDR_TRK_PILOTS:
    search_cmd.srch_mode = HDR_SRCH_ON_TRK_PILOTS;
    break;

  case SRCH4_MODE_IRAT:
    search_cmd.srch_mode = IRAT_SRCH;
    break;

  case SRCH4_MODE_DEFAULT:
  default:      
    search_cmd.srch_mode = DEFAULT;
    break;
  }
  search_cmd.cx2_rtc = queue->capturedRTCCnt;

  MSG_6(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
  "Search started with search type %d on queue %d with %d tasks, %d mode, Atomicity: %d, Current Time: %ld", 
  queue->srch_type, 
  queue->queue_type, 
  queue->num_tasks_scheduled, 
  queue->mode, 
  queue->atomic_search,
  srch4i_slottedscheduler_get_current_ustmr() );

  /* Send the message */
  estatus = msgr_send( &search_cmd.hdr, sizeof( srchfw_search_cmd_msg_t ) );
  ASSERT( estatus == E_SUCCESS );

  /* mark searcher4 active, not aborted, and not lost */
  queue->active      = TRUE;
  queue->scheduled_for_search = TRUE;
  queue->aborted     = FALSE;
  queue->lost        = FALSE;
  queue->partial_res = FALSE;  
  
  /* Need to schedule the lost dump timer  */
  timer_set( queue->lost_dump_timer,
  queue->lost_dump_time_estimate, 0, T_MSEC );
}


/*============================================================================

FUNCTION SRCH4I_SLOTTEDSCHEDULER_INIT

DESCRIPTION
  This function initializes the internal variables required for slotted
  scheduling of searches.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.  

============================================================================*/
void srch4_copy_search_info(
srch4_queue_var_type  *queue           /* to accommodate queues          */
)
{
  srch4_time_slot_type time_slot;
  uint32 counter = 0;
  uint32                 fw_queue_num;     /* variable to map to FW queue
                                              number                     */

  uint32   time_available_for_scheduling; /* time left for scheduling tasks */

  uint32   time_for_task_execution; /* store time for execution for a task */

  boolean  cxm_tx_active;    /* cxm is active or not */

  uint32   fw_index;        /* offset for fw index */

  srch4_task_struct_type *task = NULL;

/*-----------------------------------------------------------------------*/

  /* Translate to fw queue type */
  fw_queue_num = srch4_map_srch4_queue_to_fw_queue( queue->queue_type );

  /* Initialize time left in micro seconds to max */
  time_available_for_scheduling = 0x0FFFFFFF;

  time_slot.duration = 0x0FFFFFFF;
  time_slot.info_available = FALSE;
  time_slot.starting_time = 0;

  /* if it is not an atomic search */
  if ( !queue->atomic_search )
  {
    /* get the current inactivity info */
    srch4i_slottedscheduler_get_current_inactivity_info(&time_slot); 
  }

  /* Get info from CxM table if CxM Tx is active or not in this slot */
  cxm_tx_active = time_slot.info_available;


  /* Initialize time for task execution to zero */
  time_for_task_execution = 0;

  /* time left for task execution is current inactivity duration.. in ticks */
  time_available_for_scheduling = time_slot.duration;

  /* Convert to micro seconds*/
  time_available_for_scheduling = 
  srch4i_slottedscheduler_conv_ticks_to_unit(time_available_for_scheduling, T_USEC);


  /* This variable is set if CxM Info changed and we need to reschedule
     the previously programmed search */
  if ( queue->repeat_search == TRUE )
  {
    /* Resume from previous to previous slot */
    queue->num_tasks_scheduled = queue->num_completed_search;
  }

  /* if cxm is not active, there is no lead time */
  if ( !cxm_tx_active )
  {
    queue->lead_time_to_search = 0;
    queue->mode = queue->original_mode;
    queue->time_slot_duration = 0;
    queue->time_slot_start    = 0;
    /* CXM is not active. Repeat Search because entire queue was flushed */
    if ( queue->repeat_search == TRUE )
    {
      /* repeat search flag cannot be set if G-Tx is not active */
      MSG_1(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, "CXM is not active. \
            Repeat Search because entire queue was flushed. Atomicity: %d", 
      queue->atomic_search);
    }
  }
  else
  {
    /* find k for current task to be scheduled */
    /* Calculate the lead time for searching and remove it from time left
       for execution */
    /* Schedule the task in the increasing order of execution time. */

    ASSERT(queue->num_tasks_scheduled < SRCH4_MAX_HPQ_TASKS);

    /* Store the lead to search scheduling */
    queue->lead_time_to_search = srch4_get_search_scheduling_lead_time(queue);

    /* get the lead time to task estimation */
    queue->time_slot_lead_duration = srch4_get_task_estimation_lead_time(queue);

    /* remove task execution lead time from time available for scheduling */
    time_available_for_scheduling -= queue->time_slot_lead_duration; 


    /* copy time slot info to be sent for search */
    queue->time_slot_duration = time_slot.duration;
    queue->time_slot_start    = time_slot.starting_time;

    /* Set the mode to DSDA Offline */
    queue->mode = SRCH4_DSDA_OFFLINE;
  }

  /* Reset the repeat search flag since search has already been programmed */
  queue->repeat_search = FALSE;

  for (counter = queue->num_tasks_scheduled, fw_index = 0; counter < queue->num_tasks; ++counter, ++fw_index)
  {

    ASSERT(counter < SRCH4_MAX_HPQ_TASKS && fw_index < SRCH4_MAX_HPQ_TASKS);

    /* get the task to be scheduled */
    task = &queue->tasks[counter];

    /* need to remove */
    time_for_task_execution = 0;

    if ( time_available_for_scheduling <  time_for_task_execution)
    {
      /* cannot schedule the next task */
      if ( counter == 0 )
      {
        ERR_FATAL("Could not schedule a single task", 0,0,0);
      }
      break;
    }

    /* If CxM is not active then the time for task execution
       will be zero and hence we will execute all tasks
       else we will subtract time for execution and see
       if we have more room */
    time_available_for_scheduling -= time_for_task_execution;

    /* Increment the number of tasks scheduled */
    queue->num_tasks_scheduled += 1;


    if ( (queue->srch_type == SRCH4_HDR_SHOULDERS ||
    queue->srch_type == SRCH4_HDR_ACCUM) && fw_queue_num < 2)
    {
      TASK_PARMS_BASE_ADDR[fw_queue_num][fw_index].
      hdr_srch_task_params.windowSize  = task->window_size;
      TASK_PARMS_BASE_ADDR[fw_queue_num][fw_index].
      hdr_srch_task_params.pnPos       = task->pn_pos;
      TASK_PARMS_BASE_ADDR[fw_queue_num][fw_index].
      hdr_srch_task_params.cohLen      = task->coherent_len;
      TASK_PARMS_BASE_ADDR[fw_queue_num][fw_index].
      hdr_srch_task_params.carrierSelect= task->carrier;
      TASK_PARMS_BASE_ADDR[fw_queue_num][fw_index].
      hdr_srch_task_params.nonCohLen   = task->non_coherent_len;
      TASK_PARMS_BASE_ADDR[fw_queue_num][fw_index].
      hdr_srch_task_params.burstLength = task->burst_len;
      TASK_PARMS_BASE_ADDR[fw_queue_num][fw_index].
      hdr_srch_task_params.pilotIndex  = task->pilot_inx;
      TASK_PARMS_BASE_ADDR[fw_queue_num][fw_index].
      hdr_srch_task_params.walshIndex  = task->walsh;
      TASK_PARMS_BASE_ADDR[fw_queue_num][fw_index].
      hdr_srch_task_params.qof         = task->qof;
      TASK_PARMS_BASE_ADDR[fw_queue_num][fw_index].
      hdr_srch_task_params.cohTrunc    = task->coherent_trunc;
      TASK_PARMS_BASE_ADDR[fw_queue_num][fw_index].
      hdr_srch_task_params.diversity   = task->diversity;
    }
    else if ( queue->srch_type == SRCH4_1X  && fw_queue_num < 2)
    {
      TASK_PARMS_BASE_ADDR[fw_queue_num][fw_index].
      cdma_srch_task_params.windowSize    = task->window_size;
      TASK_PARMS_BASE_ADDR[fw_queue_num][fw_index].
      cdma_srch_task_params.pnPos         = task->pn_pos;
      TASK_PARMS_BASE_ADDR[fw_queue_num][fw_index].
      cdma_srch_task_params.cohLen        = task->coherent_len;
      TASK_PARMS_BASE_ADDR[fw_queue_num][fw_index].
      cdma_srch_task_params.carrierSelect = task->carrier;
      TASK_PARMS_BASE_ADDR[fw_queue_num][fw_index].
      cdma_srch_task_params.nonCohLen     = task->non_coherent_len;
      TASK_PARMS_BASE_ADDR[fw_queue_num][fw_index].
      cdma_srch_task_params.pilotIndex    = task->pilot_inx;
      TASK_PARMS_BASE_ADDR[fw_queue_num][fw_index].
      cdma_srch_task_params.walshIndex    = task->walsh;
      TASK_PARMS_BASE_ADDR[fw_queue_num][fw_index].
      cdma_srch_task_params.qof           = task->qof;
      TASK_PARMS_BASE_ADDR[fw_queue_num][fw_index].
      cdma_srch_task_params.cohTrunc      = task->coherent_trunc;
      TASK_PARMS_BASE_ADDR[fw_queue_num][fw_index].
      cdma_srch_task_params.diversity     = task->diversity;
    }
    else if ( queue->srch_type == SRCH4_AFLT)
    {

      ASSERT(fw_index < SRCH4_MAX_LPQ_TASKS);

      AFLT_PARMS_BASE_ADDR[fw_index].
      aflt_srch_task_params.windowSize    = task->window_size;
      AFLT_PARMS_BASE_ADDR[fw_index].
      aflt_srch_task_params.pnPos         = task->pn_pos;
      AFLT_PARMS_BASE_ADDR[fw_index].
      aflt_srch_task_params.cohLen        = task->coherent_len;
      AFLT_PARMS_BASE_ADDR[fw_index].
      aflt_srch_task_params.carrierSelect = task->carrier;
      AFLT_PARMS_BASE_ADDR[fw_index].
      aflt_srch_task_params.nonCohLen     = task->non_coherent_len;
      AFLT_PARMS_BASE_ADDR[fw_index].
      aflt_srch_task_params.pilotIndex    = task->pilot_inx;
      AFLT_PARMS_BASE_ADDR[fw_index].
      aflt_srch_task_params.walshIndex    = task->walsh;
      AFLT_PARMS_BASE_ADDR[fw_index].
      aflt_srch_task_params.qof           = task->qof;
      AFLT_PARMS_BASE_ADDR[fw_index].
      aflt_srch_task_params.cohTrunc      = task->coherent_trunc;
      AFLT_PARMS_BASE_ADDR[fw_index].
      aflt_srch_task_params.diversity     = task->diversity;
    }

  }
}


/*============================================================================

FUNCTION SRCH4I_SLOTTEDSCHEDULER_INIT

DESCRIPTION
  This function initializes the internal variables required for slotted
  scheduling of searches.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.  

============================================================================*/
void srch4_copy_task_info(
srch4_task_struct_type *task,            /* task info to copy     */
srch4_queue_var_type   *queue            /* queue to add to       */
)
{
  uint32 position = queue->num_tasks;

  ASSERT(position < SRCH_MAX_HPQ_TASKS);

  /* Copy all the task info into the queue task variable */

  queue->tasks[position].window_size = task->window_size;
  queue->tasks[position].pn_pos = task->pn_pos;           
  queue->tasks[position].phase = task->phase;                
  queue->tasks[position].coherent_len = task->coherent_len;              
  queue->tasks[position].pilot_inx = task->pilot_inx;                 
  queue->tasks[position].burst_len = task->burst_len;                 
  queue->tasks[position].non_coherent_len = task->non_coherent_len;          
  queue->tasks[position].diversity = task->diversity;    
  queue->tasks[position].diversity_ant = task->diversity_ant;
  queue->tasks[position].carrier = task->carrier;      
  queue->tasks[position].coherent_trunc = task->coherent_trunc;            
  queue->tasks[position].qof = task->qof;                       
  queue->tasks[position].walsh = task->walsh;                     
  queue->tasks[position].window_pos = task->window_pos;                

}



/*============================================================================

FUNCTION SRCH4_SCHEDULE_TASKS_CB_WRAPPER

DESCRIPTION
  This function is a wrapper around the slotted mode task schedule timer.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.  

============================================================================*/
void srch4_schedule_tasks_cb_wrapper
(
uint32                  queue_type    /* queue to get cb from */
)
{

  srch4_queue_var_type   *queue     = NULL;/* to accommodate queues       */
  uint64                 next_inactivity;
  srch4_time_slot_type   time_slot_next;

  /*-----------------------------------------------------------------------*/

  SRCH4_ENTER_CRIT_SECT();

  /* check which queue to use, and store ptr */
  queue = srch4_get_queue( queue_type );

  if (queue != NULL)
  {
    MSG_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Queue Active Info. Active: %d, Scheduled: %d ",
    queue->active, queue->scheduled_for_search );

    if ( !(IS_QUEUE_ACTIVE(queue)) && (queue->num_tasks == 0))
    {
      MSG(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
      "Schedule Tasks Call-back Wrapper. Queue is not active anymore");
    }
    else if ( queue->aborted )
    {
      MSG(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
      "Schedule Tasks Call-back Wrapper. Queue aborted. Ignore the call-back");
    }
    else if ( queue->num_tasks_scheduled != queue->num_completed_search )
    {
      ERR_FATAL("Stray Schedule Task Call-back. Num Tasks Scheduled:%d, Num Completed Searches:%d, Num Tasks:%d",
      queue->num_tasks_scheduled, queue->num_completed_search, queue->num_tasks);
    }
    else
    {

      MSG_3(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
      "Schedule Tasks Call-back Wrapper. Tasks Scheduled: %d, Num Total Tasks: %d, Scheduled Tasks Completed: %d", 
      queue->num_tasks_scheduled, queue->num_tasks, queue->num_completed_search);

      if ( queue->repeat_search && 
      !srch4i_slottedscheduler_is_valid_starting_time(queue->expected_next_schedule_time) )
      {
        // need to reschedule to next slot + k..
        MSG_1(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
        "Schedule Tasks Call-back Wrapper. CxM info has changed. \
             Rescheduling the search to next slot: %ld", 
        queue->expected_next_schedule_time);

        next_inactivity = 
        srch4i_slottedscheduler_get_time_to_next_inactivity_slot(&time_slot_next);

        next_inactivity += queue->lead_time_to_search; /* In microseconds */

        next_inactivity += 100; /* for timer resistence */

        queue->expected_next_schedule_time = time_slot_next.starting_time;

        if (next_inactivity > MAX_RESCHEDULE_TIME_IN_MICROSEC)
        {
          MSG_2(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
          "Extremely high reschedule Time. duration: %d, lead time:%d", 
          next_inactivity, queue->lead_time_to_search);
        }

        timer_set(queue->schedule_task_timer, next_inactivity, 0, T_USEC );
      }
      else
      {

        /* Copy searches for the next slot scheduling */
        srch4_copy_search_info(queue);

        /*  Schedule the taks for the next slot */
        srch4_schedule_search(queue);
      }
    }

    srch4_insert_history_rec(queue, FN_SCHED_TIMER_EXPIRY);
  }

  SRCH4_LEAVE_CRIT_SECT();

}
#endif


/*===========================================================================

FUNCTION       SRCH4_LOST_DUMP_CB_WRAPPER
DESCRIPTION    This function is a wrapper around the lost dump callback
                function so that we can preserve the use of rex_def_timer_ex
DEPENDENCIES   None

RETURN VALUE   None.
SIDE EFFECTS   Calls the lost dump callback if one was provided
===========================================================================*/
void srch4_lost_dump_cb_wrapper
(
uint32                  queue_type    /* queue to get cb from */
)
{
  srch4_queue_var_type   *queue    = NULL; /* to accommodate queues      */
  uint32                  msg_flag = 0;    /* message flag               */

  /*-----------------------------------------------------------------------*/

  SRCH4_ENTER_CRIT_SECT();

  /* check which queue this is a dump for, then assign queue for brevity */
  queue = srch4_get_queue( (srch4_queue_type) queue_type );

  /* check to ensure a valid queue has been assigned */
  if ( queue == NULL )
  {
    msg_flag = 3;
  }
  else if ( ( queue->aborted == FALSE ) || ( queue->lost == FALSE ) )
  {
    queue->lost = TRUE;

    /* clear the timer */
    (void) timer_clr( queue->lost_dump_timer, T_NONE );

    MSG_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Queue Active Info. Active: %d, Scheduled: %d ",
    queue->active, queue->scheduled_for_search );

    if ( !(IS_QUEUE_ACTIVE(queue)) )
    {
      /* no search was active, somehow the timer wasn't stopped */
      msg_flag = 1;
    }
#ifdef FEATURE_MODEM_COEXISTENCE_FW
    MSG(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,"lost dump initiated");
#endif

    /* Since SRCH4 does not work like SRCH1/2 if we trigger a status dump
       ourselves we won't get any more meaningful data because we just read
       the DSP DMA memory. Instead we will let the task level deal with this
       problem, and for now just abort the search(es)
       Also set the lost dump callback function as the abort callback
       function to notify the task */
    (void) srch4_abort_queue( queue->srch_type,
    (srch4_queue_type) queue_type,
    NULL,
    queue->srch_cb_data,
    FALSE,
    FALSE );
  }
  else /* searcher in a lost state (lost dump timeout after an abort) */
  {
    /* odds are we're really in trouble, or
       no search was ever actually sent to the dsp
       or no abort was actually sent */
    /* send ERR to inform everyone */
    msg_flag = 2;

    MSG_1(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
    "Queue %d set to inactive in lost dump cb wrapper",
    (srch4_queue_type) queue_type);

    /* try to recover gracefully */
    queue->scheduled_for_search = FALSE;
    queue->active      = FALSE;
    queue->aborted     = FALSE;
    queue->lost        = FALSE;
    queue->partial_res = FALSE;
    queue->num_tasks   = 0;
    queue->log_ptr     = NULL;
    queue->lost_dump_time_estimate = 0;
    if ( queue->lost_cb_func != NULL )
    {
      (*queue->lost_cb_func)( queue->srch_cb_data );
      MSG_2(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
      "Lost callback initiated for tech %d, queue %d",
      queue->srch_type, queue_type);
    }
  }

  SRCH4_LEAVE_CRIT_SECT();

  /* send MSG's and ERR's outside of INTLOCKs */
  /*lint -save -e613 queue possibly null
                     queue == NULL only if msg_flag == 3 */
  switch ( msg_flag )
  {
  case 1:
    {
      MSG_SPRINTF_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
      "Srch4 lost, abort failed (%s on %s)",
      SRCH4_TECH_STRING( queue->srch_type ),
      srch4_queue_str[queue_type] );
      break;
    }
  case 2:
    {
      MSG_SPRINTF_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
      "Srch4 lost, srch not in progress (%s on %s)",
      SRCH4_TECH_STRING( queue->srch_type ),
      srch4_queue_str[queue_type] );
      break;
    }
  case 3:
    {
      MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
      "Srch4-Lost dump on an invalid queue (%d)", queue_type );
      break;
    }
  default:
    {
      MSG_SPRINTF_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
      "Srch4 lost (%s on %s), aborting",
      SRCH4_TECH_STRING( queue->srch_type ),
      srch4_queue_str[queue_type] );
      break;
    }
  }

#ifdef SRCH4_DEBUG
  #error code not present
#endif
  /*lint -restore */

} /* srch4_lost_dump_cb_wrapper */


/*===========================================================================

                   L O G G I N G   F U N C T I O N S

===========================================================================*/
/*===========================================================================

FUNCTION       SRCH4_LOG_PARAMS
DESCRIPTION    This function fills the log parameters of the just started
                 searcher 4's search queue
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
void srch4_log_params
(
srch4_queue_type queue_type             /* queue to start                */
)
{
  srch4_queue_var_type   *queue       = NULL; /* to accommodate queues     */
  LOG_SRCH4_C_type       *log_pkt     = NULL; /* ptr to log pkt to fill    */
  srch4_task_struct_type *tasks       = NULL; /* ptr to task struct        */
  uint32                  i           = 0;    /* loop counter              */
  uint32                  coh_trunc   = 0;    /* temp var for coh_trunc    */
  uint32                  tmp_queue   = 0;    /* temp var for queue type   */
  log_code_type           log_code    = LOG_LAST_C;
  /* run time log code set for
     each technology           */
  int16                   freq_offset_trunc;  /* log pkt freq offset val   */

  /*-----------------------------------------------------------------------*/

  /* check which queue to use, and store ptr */
  queue   = srch4_get_queue( queue_type );

  /* check to ensure a valid queue has been assigned */
  if ( queue == NULL )
  {
    MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Srch4-log parameters on an invalid queue (%d)", queue_type );
    return;
  }

  /* figure out which log packet should be sent, this is based on technology
     note this is run time, allowing filtering of the searcher4 log packet
     based on which technology is sending the search request */
  switch ( queue->srch_type )
  {
  /* 1X search */
  case SRCH4_1X:
    {
      /* this log pkt is now obsolete, for 1X, only the TNG packet
         is supported */
      log_code = LOG_SRCH4_1X_C;

      return;
    }

    /* HDR search interpolation using shoulders */
  case SRCH4_HDR_SHOULDERS:
    /* HDR search with actual accumulation energies */
  case SRCH4_HDR_ACCUM:
    {
      log_code = LOG_SRCH4_MC_HDR_C;
      break;
    }

    /* AFLT search */
  case SRCH4_AFLT:
    {
      /* this log pkt is now obsolete */
      log_code = LOG_SRCH4_AFLT_C;

      return;
    }

    /* invalid technologies */
  default:
    {
      MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
      "Invalid Technology Type (%d) in Logging", queue->srch_type );
      return;
    }
  }

  /* Is logging active for this log code? */
  if ( !log_status( log_code ) )
  {
    return;
  }

  /* get the log pkt */
  log_pkt = queue->log_pkt;

  /* log pkt ptr from diag, wasn't cleaned up properly last time, this
     shouldn't happen, but just in case, we'll clean it up */
  if ( queue->log_ptr != NULL )
  {
    MSG( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, "Log_ptr not NULL!!!" );
    log_free(queue->log_ptr);
    queue->log_ptr = NULL;
  }

  /* header byte  1
     Technology            - bits  7: 5
     QPCH                  - bit      4
     NumPeaks              - bits  3: 0
   */
  log_pkt->Tech_QPCH_NumPeaks =
  ( ( ( uint8 ) queue->srch_type & 0x7 ) << 5 ) |
  ( ( queue->qpch ? 0x1 : 0x0 ) << 4 ) |
  ( ( SRCH4_NUM_PEAKS ) & 0xf );

  /* header byte  2
     Queue                 - bits  7: 4
     Priority              - bits  3: 0
   */
  switch ( queue_type )
  {
  case SRCH4_HPQ0:
    {
      tmp_queue = 0;
      break;
    }
  case SRCH4_LPQ:
    {
      tmp_queue = 1;
      break;
    }

  case SRCH4_HPQ1:
    {
      tmp_queue = 3;
      break;
    }

  default:
    {
      MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
      "Invalid Queue (%d) in Logging", queue_type );
      return;
    }
  }
  log_pkt->Queue_Priority = (uint8)
  ( ( tmp_queue & 0xF ) << 4 ) |
  ( ( queue->priority ) & 0xF );

  /* header byte  3
     NumTasks              - bits  7: 0
   */
  log_pkt->NumTasks =
  ( queue->num_tasks ) & 0xFF;

  /* header byte  4
     FreqOffset            - bits  7: 6
   */
  /* Round and convert Freq offset from 4.6875 Hz/LSB to 37.5 Hz/LSB as
     needed for the log pkt (only 8-bits in the log packet) */
  if ( queue->freq_offset < 0 )
  {
    freq_offset_trunc = ( queue->freq_offset - 4 ) / 8 ;
  }
  else
  {
    freq_offset_trunc =  ( queue->freq_offset + 4 ) / 8;
  }

  /* Check if the freq offset is within range for the log packet */
  if ( ( freq_offset_trunc > 127 ) || ( freq_offset_trunc < -127 ) )
  {
    /* Out of range indicator */
    freq_offset_trunc = 0x80;
  }

  log_pkt->FreqOffset =
  (int8) freq_offset_trunc;

  /* use a task ptr for brevity */
  tasks = queue->tasks;

  /* load the parameters into the log pkt */
  for ( i=0; i<queue->num_tasks; i++ )
  {
    /* parameters bytes 1-2
       Pilot               - bits 15:7
       Diversity/Antenna   - bits 6 :5
       Burst Length        - bits 4 :3
       QOF                 - bits 2 :1
       WalshMSB            - bit  0
     */
    log_pkt->tasks[i].Pilot_Antenna_BurstLen_QOF_WalshMSB =
    ( ( tasks[i].pilot_inx & 0x1ff ) << 7 ) |
    ( ( ( uint16 )tasks[i].diversity & 0x3 ) << 5 ) |
    ( ( tasks[i].burst_len & 0x3 ) << 3 ) |
    ( ( tasks[i].qof & 0x3 ) << 1 ) |
    ( ( tasks[i].walsh & 0x01ff ) >> 8 );

    /* parameters bytes 3-4
       WalshLSB            - bit  14: 8
       Non-Coherent Length - bits 7 : 0
     */
    log_pkt->tasks[i].WalshLSB_NonCohLen =
    ( ( tasks[i].walsh & 0xff ) << 8 ) |
    ( tasks[i].non_coherent_len & 0xff);

    /* parameters bytes 5-6
       Coherent Truncation - bits 15:14
       Coherent Length     - bits 13: 0
     */
    /* for coherent truncation, output the number of bits truncated
       instead of what is programmed to the dsp
     */
    coh_trunc = (uint32) tasks[i].coherent_trunc + 1;

    log_pkt->tasks[i].CohTrunc_CohLen = (uint16)
    ( ( coh_trunc & 0x7 ) << 13 ) |
    ( ( tasks[i].coherent_len & 0x1fff ) );

    /* parameters bytes 7-8
       PN Position (chips) - bits 15: 0
     */
    log_pkt->tasks[i].PNPos =
    tasks[i].pn_pos;

    /* parameters bytes 9-10
       Window Size (chips) - bits 15:0
     */
    log_pkt->tasks[i].WinSize =
    tasks[i].window_size;

    /* parameters bytes 11-12
       Carrier - bits 1:0
     */
    log_pkt->tasks[i].carrier =
    (uint16)tasks[i].carrier;

  } /* for each task */

} /* srch4_log_params */


/*===========================================================================

FUNCTION       SRCH4_LOG_RESULTS
DESCRIPTION    This function requests and sends a searcher 4 log packet.
               It also fills the searcher 4's results into the packet
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   A log packet is requested and sent

===========================================================================*/
void srch4_log_results
(
srch4_queue_type queue_type,                /* queue to start            */
int32            num_results                /* number of srches returned */
)
{
  srch4_queue_var_type   *queue       = NULL; /* to accommodate queues     */
  srch4_task_struct_type *tasks       = NULL; /* ptr to task struct to use */
  LOG_SRCH4_C_type       *log_pkt     = NULL; /* ptr to log pkt to fill    */
  log_code_type           log_code    = LOG_LAST_C;
  /* run time log code set for
     each technology           */
  int32                   i=0, j=0;           /* loop counters             */

  /*-----------------------------------------------------------------------*/

  /* check which queue to use, and store ptr */
  queue = srch4_get_queue( queue_type );

  /* check to ensure a valid queue has been assigned */
  if ( queue == NULL )
  {
    MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Srch4-log results on an invalid queue (%d)", queue_type );
    return;
  }

  /* figure out which log packet should be sent, this is based on technology
     note this is run time, allowing filtering of the searcher4 log packet
     based on which technology is sending the search request */
  switch ( queue->srch_type )
  {
  /* 1X search */
  case SRCH4_1X:
    {
      /* this log pkt is now obsolete, for 1X, only the TNG packet
         is supported */
      log_code = LOG_SRCH4_1X_C;

      return;
    }

    /* HDR search interpolation using shoulders */
  case SRCH4_HDR_SHOULDERS:
    /* HDR search with actual accumulation energies */
  case SRCH4_HDR_ACCUM:
    {
      log_code = LOG_SRCH4_MC_HDR_C;
      break;
    }

    /* AFLT search */
  case SRCH4_AFLT:
    {
      /* this log pkt is now obsolete */
      log_code = LOG_SRCH4_AFLT_C;

      return;
    }

    /* invalid technologies */
  default:
    {
      MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
      "Invalid Technology Type (%d) in Logging", queue->srch_type );
      return;
    }
  }

  /* Compute the size of the log packet. We don't want to send the whole
     array, just the entries that we populated */
  /* size of all fields prior to the task field plus the size of task
     structure calculation for the number of tasks and the number of
     peaks returned */
  queue->log_pkt_siz = (uint16) SRCH4_LOG_SIZ( num_results, SRCH4_NUM_PEAKS );

  /* request log ptr from diag */
  queue->log_ptr = (LOG_SRCH4_C_type *) log_alloc( log_code,
  (unsigned int)queue->log_pkt_siz );

  /* Are we logging? */
  if ( queue->log_ptr != NULL )
  {
    /* get the log pkt */
    log_pkt = queue->log_pkt;

    /* header byte 1
       version               - bits  7:0
    */
    log_pkt->version = SRCH4_HDR_LOG_VERSION;

    /* header byte  7
       rx0_agc (ant 0)       - bits  7: 0

       header byte  8
       rx1_agc (ant 1)       - bits  7: 0
     */
    if ( queue->srch_type == SRCH4_1X 
    || queue->srch_type == SRCH4_AFLT 
    )
    {
#ifdef FEATURE_RF_SVDO_API
      /* Trigger on carrier to log AGC correctly for SV vs. non-SV */
      log_pkt->rx0_agc = ( log_pkt->tasks[0].carrier == SRCH4_CARRIER_3 ) ? 
      rfm_1x_get_rx_agc( RFM_DEVICE_2 ) : 
      rfm_1x_get_rx_agc( RFM_DEVICE_0 );

      log_pkt->rx1_agc = rfm_1x_get_rx_agc( RFM_DEVICE_1 );
#else
      log_pkt->rx0_agc = rfm_get_cdma_rx_agc( RFCOM_TRANSCEIVER_0 );
      log_pkt->rx1_agc = rfm_get_cdma_rx_agc( RFCOM_RECEIVER_1 );
#endif /* FEATURE_RF_SVDO_API */
    }

    if ( queue->srch_type == SRCH4_HDR_ACCUM ||
    queue->srch_type == SRCH4_HDR_SHOULDERS )
    {
#ifdef FEATURE_RF_SVDO_API
      /* REVISIT: Check if HDR parameters are correct */
      log_pkt->rx0_agc = rfm_hdr_get_rx_agc( RFM_DEVICE_0,
      RFM_CDMA_CARRIER_CUMULATIVE,
      RFM_HDR_AGC_FORMAT__DBM10 );
      log_pkt->rx1_agc = rfm_hdr_get_rx_agc( RFM_DEVICE_1,
      RFM_CDMA_CARRIER_CUMULATIVE,
      RFM_HDR_AGC_FORMAT__DBM10 );
#else
      log_pkt->rx0_agc = rfm_get_cdma_rx_agc( RFCOM_TRANSCEIVER_0 );
      log_pkt->rx1_agc = rfm_get_cdma_rx_agc( RFCOM_RECEIVER_1 );
#endif /* FEATURE_RF_SVDO_API */
    }

    /* use a task ptr for brevity */
    tasks = queue->tasks;

    for ( i=0; i<num_results; i++ )
    {

      for ( j=0; j<SRCH4_NUM_PEAKS; j++)
      {
        /* results byte  1
           Reserved ( No need to fill this in )
        log_pkt->tasks[i].results[j].Reserved =
          0xff;
         */

        /* results bytes 2-4
           Peak Position (ChipX16) - bits 19:0
         */
        log_pkt->tasks[i].results[j].PeakPos_MSB = (uint8)
        ( (tasks[i].peak_pos[j] >> 16) & 0xf );
        log_pkt->tasks[i].results[j].PeakPos_LSB = (uint16)
        ( tasks[i].peak_pos[j] & 0xffff );

        /* results bytes 5-6
           Peak Energy - bits 15:0
         */
        log_pkt->tasks[i].results[j].PeakEng =
        ( tasks[i].peak_eng[j] );
      } /* for each peak */
    } /* for each task */

    /* copy the assigned diag hdr into the local hdr
       this will make copying later easier */
    memscpy( log_pkt, sizeof(LOG_SRCH4_C_type), queue->log_ptr, sizeof(log_hdr_type) );

    /* copy the local log packet to the diag log pkt */
    /* this only works if the number of peaks returned is equal
       to SRCH4_NUM_PEAKS, which is how it is currently defined
       if this changes we will need to revisit */
    memscpy( queue->log_ptr, sizeof(LOG_SRCH4_C_type), queue->log_pkt, queue->log_pkt_siz );

    /* send command to diag to send the packet */
    log_commit( (void *)queue->log_ptr );

    /* clear the ptr out */
    queue->log_ptr = NULL;

  } /* queue->log_ptr != NULL */

} /* srch4_log_results */

/*===========================================================================

FUNCTION       SRCH4_LOG_SRCH4_SUB_PARAMS
DESCRIPTION    This function fills the log parameters of the just started
                 searcher 4's 1X search queue
DEPENDENCIES   This logging is only valid for 1X

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
void srch4_log_srch4_sub_params
(
srch4_queue_type queue_type                   /* queue to log            */
)
{
  srch4_queue_var_type     *queue       = NULL; /* to accommodate queues   */
  srch_genlog_srch4_sub_v1 *sub_log_pkt = NULL; /* ptr to log pkt to fill  */
  srch4_task_struct_type   *tasks       = NULL; /* ptr to task struct      */
  uint32                   i            = 0;    /* loop counter            */
  log_code_type            log_code     = LOG_LAST_C;
  /* run time log code set
     for each technology     */

  /*-----------------------------------------------------------------------*/

  /* check which queue to use, and store ptr */
  queue = srch4_get_queue( queue_type );
  if ( queue == NULL )
  {
    MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Srch4-log parameters on an invalid queue (%d)", queue_type );
    return;
  }

  /* figure out which log packet should be sent, this is based on technology
     note this is run time, allowing filtering of the searcher4 log packet
     based on which technology is sending the search request */
  switch ( queue->srch_type )
  {
  case SRCH4_1X:
    log_code = SRCH_LOG_TNG_1X_SEARCHER_DUMP_C;
    break;

  default:
    MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Invalid Technology Type (%d) in Logging", queue->srch_type );
    return;
  }

  /* is logging active for this log code? */
  if ( !log_status(log_code) )
  {
    return;
  }

  /* get the log pkt */
  sub_log_pkt = queue->sub_log_pkt;

  /* Fill in parameters common for all tasks */
  sub_log_pkt->tech        = ( uint8 )queue->srch_type;
  sub_log_pkt->queue       = ( uint8 )queue_type;
  sub_log_pkt->priority    = ( uint8 )queue->priority;
  sub_log_pkt->qpch_cntl   = queue->qpch;
  sub_log_pkt->freq_offset = queue->freq_offset;
  sub_log_pkt->num_tasks   = queue->num_tasks;
  sub_log_pkt->num_peaks   = SRCH4_NUM_PEAKS;

  /* use a task ptr for brevity */
  tasks = queue->tasks;

  ASSERT(queue->num_tasks <= SRCH4_MAX_HPQ_TASKS);
  /* load the parameters into the log pkt */
  for ( i = 0; i < queue->num_tasks; i++ )
  {
    /* Pilot               - bits 15:6
       Diversity/Antenna   - bits 5 :4
       Burst Length        - bits 3 :2
       QOF                 - bits 1 :0 */
    sub_log_pkt->tasks[i].pilot_ant_burst_len_qof =
    ( ( tasks[i].pilot_inx & 0x3ff ) << 6 ) |
    ( ( ( uint16 )tasks[i].diversity & 0x3 ) << 4 ) |
    ( ( tasks[i].burst_len & 0x3 ) << 2 ) |
    ( ( tasks[i].qof & 0x3 ) );

    /* Coherent Truncation - bits 15:14
       Coherent Length     - bits 13: 0 */
    /* for coherent truncation, output the number of bits truncated
       instead of what is programmed to the dsp */
    sub_log_pkt->tasks[i].coh_trunc_coh_len =
    ( ( ( tasks[i].coherent_trunc + 1 ) & 0x7 ) << 13 ) |
    ( ( tasks[i].coherent_len & 0x1fff ) );

    /* Fill in the non-packed parameters */
    sub_log_pkt->tasks[i].walsh_index    = tasks[i].walsh;
    sub_log_pkt->tasks[i].noncoh_len     = tasks[i].non_coherent_len;
    sub_log_pkt->tasks[i].phase          = tasks[i].phase;
    sub_log_pkt->tasks[i].win_start      = tasks[i].pn_pos;
    sub_log_pkt->tasks[i].win_size       = tasks[i].window_size;
    sub_log_pkt->tasks[i].pilot_set_mask = tasks[i].pilot_set_mask;

  } /* for each task */

} /* srch4_log_srch4_sub_params */


/*===========================================================================

FUNCTION       SRCH4_BUILD_SRCH4_SUB
DESCRIPTION    Builds and commits a 1X searcher4 generalized subpacket
DEPENDENCIES   This logging is only valid for 1X

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
void srch4_build_srch4_sub
(
srch_genlog_packet_id_type  id,         /* id of packet to commit sub     */
sub_commit_func_type        commit_fp,  /* f(x) to commit the subpacket   */
uint32                      queue_type, /* queue to log                   */
int32                       num_results /* number of srches returned      */
)
{
  srch4_queue_var_type     *queue       = NULL; /* to accommodate queues   */
  srch4_task_struct_type   *tasks       = NULL; /* ptr to task struct      */
  srch_genlog_srch4_sub_v1 *sub_log_pkt = NULL; /* ptr to gen log pkt      */
  int32                     i=0, j=0;           /* loop counters           */

  /*-----------------------------------------------------------------------*/

  if ( commit_fp == NULL )
  {
    MSG( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "srch4_build_srch4_sub got NULL commit_fp ptr" );

    return;
  }

  /* check which queue to use, and store ptr */
  queue = srch4_get_queue( (srch4_queue_type) queue_type );
  if ( queue == NULL )
  {
    MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Srch4-log results on an invalid queue (%d)", queue_type );
    return;
  }

  /* figure out which log packet should be sent, this is based on technology
     note this is run time, allowing filtering of the searcher4 log packet
     based on which technology is sending the search request */
  if ( queue->srch_type != SRCH4_1X )
  {
    MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Invalid Technology Type (%d) in Logging", queue->srch_type );
    return;
  }

  /* get the log pkt */
  sub_log_pkt = queue->sub_log_pkt;

  /* use a task ptr for brevity */
  tasks = queue->tasks;

  ASSERT(num_results <= SRCH4_MAX_HPQ_TASKS);
  for ( i = 0; i < num_results; i++ )
  {
    for ( j = 0; j < SRCH4_NUM_PEAKS; j++)
    {
      /* peak position (ChipX16) and energy*/
      sub_log_pkt->tasks[i].results[j].peak_pos  = tasks[i].peak_pos[j];
      sub_log_pkt->tasks[i].results[j].peak_eng  = tasks[i].peak_eng[j];

    } /* for each peak */
  } /* for each task */

  sub_log_pkt->hdr.id   = SRCH_SUB_ID_SEARCHER_4;
  sub_log_pkt->hdr.ver  = LOG_VERSION_SRCH4_DUMP;
  sub_log_pkt->hdr.size = (uint16)
  SRCH4_SUB_LOG_SIZ(num_results,SRCH4_NUM_PEAKS);

  /* commit the buffer */
  commit_fp(id,&sub_log_pkt->hdr);

} /* srch4_build_srch4_sub */

#ifdef FEATURE_MODEM_COEXISTENCE_FW
/*===========================================================================

FUNCTION       SRCH4_CXM_INFO_MODIFIED_CB
DESCRIPTION    This call-back is called by task schedule module when CXM info
               changes.
DEPENDENCIES   None.

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
static void srch4_cxm_info_modified_cb(void)
{

  srch4_queue_var_type *queue;
  uint32 counter = 0;
  srch4_time_slot_type   time_slot_next;
  /*-----------------------------------------------------------------------*/


  /* loop through the three queues: HPQ0, HPQ1, LPQ */
  for ( counter = SRCH4_MIN_Q; counter < SRCH4_MAX_Q; ++counter)
  {
    queue = srch4_get_queue( counter );

    MSG_4( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Queue Active Info. Active: %d, Scheduled: %d Num Tasks Sched %d, Completed %d",
    queue->active, queue->scheduled_for_search, 
    queue->num_tasks_scheduled, queue->num_completed_search );

    /* check to ensure a valid queue has been assigned */
    if ( queue != NULL && (IS_QUEUE_ACTIVE(queue)) && !queue->atomic_search &&
    (queue->num_tasks_scheduled >= queue->num_completed_search))
    {
      if ( queue->queue_type == SRCH4_LPQ )
      {
        /* Dont do anything for LPQ. */
        continue;
      }

      /* Repeat the previously programmed search */
      queue->repeat_search = TRUE;



      /* create a timer for next search processing */

      if ( queue->num_tasks_scheduled == queue->num_completed_search )
      {
        /* We are waiting inbetween GTX. Now the info has changed. */
        uint64 duration = srch4i_slottedscheduler_get_time_to_next_inactivity_slot(&time_slot_next);

        if ( duration > 0 )
        {
          duration += queue->lead_time_to_search; /* In microseconds */

          MSG_2(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
          "Reset the next search info in cxm info modified. Duration: %ld Lead Time: %d",
          duration, queue->lead_time_to_search);

          if (duration > MAX_RESCHEDULE_TIME_IN_MICROSEC)
          {
            MSG_2(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
            "Extremely high reschedule Time. duration: %d, lead time:%d", 
            duration, queue->lead_time_to_search);
          }
        }
      }

    }
    else if ( queue != NULL && queue->num_completed_search > queue->num_tasks_scheduled )
    {
      ERR_FATAL("Last Scheduled Search Count: %d greater than Num tasks scheduled: %d",
      queue->num_completed_search, queue->num_tasks_scheduled, 0);
    }

    if (queue != NULL && (IS_QUEUE_ACTIVE(queue)) )
    {
      srch4_insert_history_rec(queue, FN_CXM_UPDATE_GTX);
    }
  }
}
#endif

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
void srch4_init( void )
{
  srch4_queue_type        i;                /* loop var for queues    */
  srch4_queue_var_type   *queue     = NULL; /* to accommodate queues  */

  /*-----------------------------------------------------------------------*/

  /* loop through the three queues: HPQ0, HPQ1, LPQ */
  for ( i = SRCH4_MIN_Q; i < SRCH4_MAX_Q; i++ )
  {
    queue = srch4_get_queue( i );

    /* check to ensure a valid queue has been assigned */
    if ( queue != NULL )
    {
      /* Set up REX timers for catching lost status dumps.
         Callback function for both is the lost_dump_cb_wrapper */
      timer_def( queue->lost_dump_timer, NULL, rex_self(), 0,
      (timer_t1_cb_type)( srch4_lost_dump_cb_wrapper ),
      (timer_cb_data_type)i );

#ifdef FEATURE_MODEM_COEXISTENCE_FW
      /* Set up REX timers for catching partial dump over-time. */      
      timer_def( queue->schedule_task_timer, NULL, rex_self(), 0,
      (timer_t1_cb_type)( srch4_schedule_tasks_cb_wrapper ),
      (timer_cb_data_type)i );

      /* clear the helper params */
      srch4_clear_helper_queue_params(queue);
#else
      queue->srch_type = SRCH4_NO_TECH;
#endif
    }
  }
#ifdef FEATURE_MODEM_COEXISTENCE_FW
  /* Register the call-back to get invoked when CXM info changes */
  srch4i_slottedscheduler_register_change_info(srch4_cxm_info_modified_cb);
#endif

} /* srch4_init */

/*===========================================================================

FUNCTION       SRCH4_SET_MEM_MAP_BASE_ADDR
DESCRIPTION    This function sets the address of the SW-FW shared memory base
               address
DEPENDENCIES   None

RETURN VALUE   None.
SIDE EFFECTS   None

===========================================================================*/
void srch4_set_mem_map_base_addr
(
srchfw_sw_shared_memory_interface_t* addr
)
{

  if ( addr == NULL )
  {
    ERR_FATAL("SRCH4 FW SMEM Address passed as NULL",0,0,0);
  }

  srch4.mem_map.base_addr = addr;

} /* srch4_set_mem_map_base_addr */

/*===========================================================================

FUNCTION       SRCH4_ADD_TASK
DESCRIPTION    This function adds a task to specified priority queue
DEPENDENCIES   None

RETURN VALUE   Srch4drv error value
SIDE EFFECTS   Srch4 structure is modified, tasks are added to the DSP

===========================================================================*/
int32 srch4_add_task
(
srch4_srch_type         task_srch_type, /* search type                  */
srch4_queue_type        queue_type,     /* queue to add tasks to        */
srch4_task_struct_type *task            /* task to add                  */
)
{
  srch4_queue_var_type   *queue     = NULL;/* to accommodate queues       */
  uint16                  inv_win_size = 0;/* temp. var for window size if
                                               it is invalid              */

  int32                   ret_status = SRCH4DRV_RET_OK;
  /* save return value to be
     returned out of the INTLOCK()
     block */

  uint32                  msg_flag = 0;    /* to save the type of error message
                                              inside the INTLOCK() block */

#ifndef FEATURE_MODEM_COEXISTENCE_FW
  uint32                 fw_queue_num;     /* variable to map to FW queue
                                            number                     */
#endif

  /*-----------------------------------------------------------------------*/

  /* check for valid inputs */
  if ( task == NULL )
  {
    MSG_SPRINTF_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
    "Addition of invalid task to srch4 %s queue",
    srch4_queue_str[queue_type] );

    return( SRCH4DRV_RET_INVALID_PARAM );
  }

  /* check if this technology was not assigned a queue */
  if ( queue_type == SRCH4_NO_QUEUE )
  {
    /* this is not right, mark this as an ERR */
    MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Tech: %d tried to add a task a when not registered",
    task_srch_type );

    return( SRCH4DRV_RET_INVALID_QUEUE );
  }

  /* check which queue to use, and store ptr */
  queue = srch4_get_queue( queue_type );

  /* check to ensure a valid queue has been assigned */
  if ( queue == NULL )
  {
    MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Trying to add a task on an invalid queue (%d)", task_srch_type );

    return( SRCH4DRV_RET_INVALID_QUEUE );
  }

#ifndef FEATURE_MODEM_COEXISTENCE_FW
  /* Translate to fw queue type */
  fw_queue_num = srch4_map_srch4_queue_to_fw_queue( queue_type );
#else

  /* Copy the queue type Information */
  queue->queue_type = queue_type;
#endif

  /* lock the interrupts */
  SRCH4_ENTER_CRIT_SECT();

  MSG_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
  "Queue Active Info. Active: %d, Scheduled: %d ",
  queue->active, queue->scheduled_for_search );


  /* Did we overflow the queue? */
  if ( queue->num_tasks >= queue->max_num_tasks )
  {
    /* We are trying to program more tasks then fit in the queue */
    ret_status = SRCH4DRV_RET_FULL;
    msg_flag = 1;
  }

  /* is the queue active? */
  else if ( (IS_QUEUE_ACTIVE(queue)) )
  {
    if ( queue->lost || queue->aborted )
    {
      /* somehow we didn't clean up after the last search possibile lost
         and never aborted. Try and clean this up by aborting the
         current active search */
      ret_status = SRCH4DRV_RET_UNCLEAN;
      msg_flag = 2;
    }
    else
    {
      /* We are trying to add a task and the searcher is already running */
      ret_status = SRCH4DRV_RET_ACTIVE;
      msg_flag =3;
    }
  }
  /* check to make sure we have the same search types */
  else if ( (task_srch_type   != queue->srch_type) &&
  (queue->num_tasks != 0 ) )
  {
    /* trying to program different technologies on the same search */
    ret_status = SRCH4DRV_RET_TECH_MISMATCH;
    msg_flag = 4;
  }
  /* is AFLT and only AFLT on LPQ? */
  else if ( ( (task_srch_type == SRCH4_AFLT) &&
  (queue_type     != SRCH4_LPQ)     ) ||
  ( (task_srch_type != SRCH4_AFLT) &&
  (queue_type     == SRCH4_LPQ)     )    )
  {
    ret_status =  SRCH4DRV_RET_TECH_MISMATCH;
    msg_flag = 5;
  }
  /* Is the coherent length within bounds? */
  else if ( ( task->coherent_len < COH_LEN_MIN ) ||
  ( task->coherent_len > COH_LEN_MAX )
  ||
  ( ( task->coherent_len > COH_LEN_AFLT_MAX ) &&
  ( task_srch_type == SRCH4_AFLT ) )
  )
  {
    ret_status = SRCH4DRV_RET_INVALID_PARAM;
    msg_flag = 6;
  }
  /* Is the non coherent length within bounds? */
  else if ( ( task->non_coherent_len < NON_COH_LEN_MIN ) ||
  ( task->non_coherent_len > NON_COH_LEN_MAX )
  ||
  ( ( task->non_coherent_len > NON_COH_LEN_HDR_MAX ) &&
  ( ( task_srch_type == SRCH4_HDR_SHOULDERS ) ||
  ( task_srch_type == SRCH4_HDR_ACCUM ) ) )
  )
  {
    ret_status = SRCH4DRV_RET_INVALID_PARAM;
    msg_flag = 7;
  }
  /* is M*N within bounds? */
  else if ( ( ( task->non_coherent_len * task->coherent_len ) > M_X_N_MAX ) &&
  ( queue_type != SRCH4_LPQ ) )
  {
    ret_status =  SRCH4DRV_RET_INVALID_PARAM;
    msg_flag = 8;
  }
  /* is the diversity field valid? */
  else if ( task->diversity >= SRCH4_ANT_MAX )
  {
    ret_status =  SRCH4DRV_RET_INVALID_PARAM;
    msg_flag = 9;
  }
  /* is burst length valid and only used for HDR? */
  else if ( ( task->burst_len > BURST_LEN_MAX )
  &&
  ( ( task_srch_type == SRCH4_HDR_SHOULDERS ) ||
  ( task_srch_type == SRCH4_HDR_ACCUM ) )
  )
  {
    ret_status = SRCH4DRV_RET_INVALID_PARAM;
    msg_flag = 10;
  }
  /* all's well and good */
  else
  {
    /* set the search type */
    queue->srch_type = task_srch_type;

    /* catch to make sure window size is within range */
    if ( task->window_size > WINSIZ_MAX )
    {
      /* throw in the maximum window size so as not to confuse the MDSP */
      inv_win_size      = task->window_size;
      task->window_size = WINSIZ_MAX;

      msg_flag = 11;
    }
    if ( task->window_size < WINSIZ_MIN )
    {
      /* throw in the minimum window size so as not to confuse the MDSP */
      inv_win_size      = task->window_size;
      task->window_size = WINSIZ_MIN;

      msg_flag = 12;
    }

#ifdef FEATURE_MODEM_COEXISTENCE_FW
    /* copy all the task info into searcher4 queue structure */
    srch4_copy_task_info(task, queue);
#endif 

    /* queue_type and num_tasks array bound check done above.. so no need to
       do it here again */

#ifndef FEATURE_MODEM_COEXISTENCE_FW
    if ( task_srch_type == SRCH4_HDR_SHOULDERS ||
    task_srch_type == SRCH4_HDR_ACCUM )
    {
      TASK_PARMS_BASE_ADDR[fw_queue_num][queue->num_tasks].
      hdr_srch_task_params.windowSize  = task->window_size;
      TASK_PARMS_BASE_ADDR[fw_queue_num][queue->num_tasks].
      hdr_srch_task_params.pnPos       = task->pn_pos;
      TASK_PARMS_BASE_ADDR[fw_queue_num][queue->num_tasks].
      hdr_srch_task_params.cohLen      = task->coherent_len;
      TASK_PARMS_BASE_ADDR[fw_queue_num][queue->num_tasks].
      hdr_srch_task_params.carrierSelect= task->carrier;
      TASK_PARMS_BASE_ADDR[fw_queue_num][queue->num_tasks].
      hdr_srch_task_params.nonCohLen   = task->non_coherent_len;
      TASK_PARMS_BASE_ADDR[fw_queue_num][queue->num_tasks].
      hdr_srch_task_params.burstLength = task->burst_len;
      TASK_PARMS_BASE_ADDR[fw_queue_num][queue->num_tasks].
      hdr_srch_task_params.pilotIndex  = task->pilot_inx;
      TASK_PARMS_BASE_ADDR[fw_queue_num][queue->num_tasks].
      hdr_srch_task_params.walshIndex  = task->walsh;
      TASK_PARMS_BASE_ADDR[fw_queue_num][queue->num_tasks].
      hdr_srch_task_params.qof         = task->qof;
      TASK_PARMS_BASE_ADDR[fw_queue_num][queue->num_tasks].
      hdr_srch_task_params.cohTrunc    = task->coherent_trunc;
      TASK_PARMS_BASE_ADDR[fw_queue_num][queue->num_tasks].
      hdr_srch_task_params.diversity   = task->diversity;
    }
    else if ( task_srch_type == SRCH4_1X )
    {
      TASK_PARMS_BASE_ADDR[fw_queue_num][queue->num_tasks].
      cdma_srch_task_params.windowSize    = task->window_size;
      TASK_PARMS_BASE_ADDR[fw_queue_num][queue->num_tasks].
      cdma_srch_task_params.pnPos         = task->pn_pos;
      TASK_PARMS_BASE_ADDR[fw_queue_num][queue->num_tasks].
      cdma_srch_task_params.cohLen        = task->coherent_len;
      TASK_PARMS_BASE_ADDR[fw_queue_num][queue->num_tasks].
      cdma_srch_task_params.carrierSelect = task->carrier;
      TASK_PARMS_BASE_ADDR[fw_queue_num][queue->num_tasks].
      cdma_srch_task_params.nonCohLen     = task->non_coherent_len;
      TASK_PARMS_BASE_ADDR[fw_queue_num][queue->num_tasks].
      cdma_srch_task_params.pilotIndex    = task->pilot_inx;
      TASK_PARMS_BASE_ADDR[fw_queue_num][queue->num_tasks].
      cdma_srch_task_params.walshIndex    = task->walsh;
      TASK_PARMS_BASE_ADDR[fw_queue_num][queue->num_tasks].
      cdma_srch_task_params.qof           = task->qof;
      TASK_PARMS_BASE_ADDR[fw_queue_num][queue->num_tasks].
      cdma_srch_task_params.cohTrunc      = task->coherent_trunc;
      TASK_PARMS_BASE_ADDR[fw_queue_num][queue->num_tasks].
      cdma_srch_task_params.diversity     = task->diversity;
    }
    else if ( task_srch_type == SRCH4_AFLT )
    {
      AFLT_PARMS_BASE_ADDR[queue->num_tasks].
      aflt_srch_task_params.windowSize    = task->window_size;
      AFLT_PARMS_BASE_ADDR[queue->num_tasks].
      aflt_srch_task_params.pnPos         = task->pn_pos;
      AFLT_PARMS_BASE_ADDR[queue->num_tasks].
      aflt_srch_task_params.cohLen        = task->coherent_len;
      AFLT_PARMS_BASE_ADDR[queue->num_tasks].
      aflt_srch_task_params.carrierSelect = task->carrier;
      AFLT_PARMS_BASE_ADDR[queue->num_tasks].
      aflt_srch_task_params.nonCohLen     = task->non_coherent_len;
      AFLT_PARMS_BASE_ADDR[queue->num_tasks].
      aflt_srch_task_params.pilotIndex    = task->pilot_inx;
      AFLT_PARMS_BASE_ADDR[queue->num_tasks].
      aflt_srch_task_params.walshIndex    = task->walsh;
      AFLT_PARMS_BASE_ADDR[queue->num_tasks].
      aflt_srch_task_params.qof           = task->qof;
      AFLT_PARMS_BASE_ADDR[queue->num_tasks].
      aflt_srch_task_params.cohTrunc      = task->coherent_trunc;
      AFLT_PARMS_BASE_ADDR[queue->num_tasks].
      aflt_srch_task_params.diversity     = task->diversity;
    }
#endif

    if ( queue_type == SRCH4_LPQ )
    {
      queue->lost_dump_time_estimate += srch4_lpq_normal_timeout_estimate;
    }
    else /* HPQs */
    {
      /* update estimated hpq lost dump time */
      if ( task->window_size <= SRCH4_WINSIZE_NORM_TIMEOUT_LIMIT )
      {
        /* window size is less than SRCH4_WINSIZE_NORM_TIMEOUT_LIMIT,
           normal search times */
        queue->lost_dump_time_estimate += SRCH4_HPQ_DUMP_TIMEOUT_NORMAL;
      }
      else
      {
        /* window size is greater than SRCH4_WINSIZE_NORM_TIMEOUT_LIMIT,
           expect a longer search */
        queue->lost_dump_time_estimate += SRCH4_HPQ_DUMP_TIMEOUT_MAX;
      }
    }

    /* increase number of programmed tasks */
    queue->num_tasks++;

#ifdef SRCH4_DEBUG
    #error code not present
#endif

  }

  /* unlock the interrupts */
  SRCH4_LEAVE_CRIT_SECT();

  /* send MSG's and ERR's outside of INTLOCKs */
  switch ( msg_flag )
  {
  case 0:                   /* all tests passed */
    break;

  case 1:
    MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
    "Searcher4 overflow too many tasks (%s)(tasks=%d, max=%d)",
    srch4_queue_str[queue_type], queue->num_tasks,
    queue->max_num_tasks );
    break;

  case 2:
    MSG_SPRINTF_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
    "Srch4 was lost/aborted w/o cleanup (%s)",
    srch4_queue_str[(uint32)queue_type] );

    (void) srch4_abort_queue( queue->srch_type, queue_type,
    NULL, queue->srch_cb_data, FALSE, FALSE );
    break;

  case 3:
    MSG_SPRINTF_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
    "Searcher 4 already running (%s)",
    srch4_queue_str[(uint32)queue_type] );
    break;

  case 4:
    MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
    "Srch4 Prog diff srch types to same %s queue (%s/%s)",
    srch4_queue_str[queue_type],
    SRCH4_TECH_STRING( task_srch_type ),
    srch4_queue_str[(uint32)queue_type] );
    break;

  case 5:
    MSG_SPRINTF_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
    "Trying to program a tech (%s) into an invalid queue(%s)",
    SRCH4_TECH_STRING( task_srch_type ),
    srch4_queue_str[(uint32)queue_type] );
    break;

  case 6:
    MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
    "Invalid Srch4 param - coh_len %d (48-3584, 48-2048 AFLT) - %s on %s",
    task->coherent_len,
    SRCH4_TECH_STRING( task_srch_type ),
    srch4_queue_str[(uint32)queue_type] );
    break;

  case 7:
    MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
    "Invalid Srch4 param - non_coh_len %d (1-128, 1-2 HDR) - %s on %s",
    task->non_coherent_len,
    SRCH4_TECH_STRING( task_srch_type ),
    srch4_queue_str[(uint32)queue_type] );
    break;

  case 8:
    MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
    "Invalid Srch4 param - tot int len %d (3948 HPQ) - %s on %s",
    ( task->non_coherent_len * task->coherent_len ),
    SRCH4_TECH_STRING( task_srch_type ),
    srch4_queue_str[(uint32)queue_type] );
    break;

  case 9:
    MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
    "Invalid Srch4 param - diversity %d (0-2) - %s on %s",
    task->diversity,
    SRCH4_TECH_STRING( task_srch_type ),
    srch4_queue_str[(uint32)queue_type] );
    break;

  case 10:
    MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
    "Invalid Srch4 param - burst_len %d (1-2 HDR) - %s on %s",
    task->burst_len,
    SRCH4_TECH_STRING( task_srch_type ),
    srch4_queue_str[(uint32)queue_type] );
    break;

  case 11:
    MSG_3( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
    "Error : Invalid WIN SIZE %d for Srch4 (replaced w/ %d)(4-%d)",
    inv_win_size, task->window_size, WINSIZ_MAX );
    break;

  case 12:
    MSG_3( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
    "Error : Invalid WIN SIZE %d for Srch4 (replaced w/ %d)(4-%d)",
    inv_win_size, task->window_size, WINSIZ_MAX );
    break;

  default:                  /* switch statement should never come here */
    MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
    "Error : srch4_add_task(): Abnormal msg_flag (%d) ", msg_flag);
    break;

  }

#ifdef SRCH4_DEBUG
  #error code not present
#endif

  /* return relevant status */
  return( ret_status );

} /* srch4_add_task */


/*===========================================================================

FUNCTION       SRCH4_START_SEARCH
DESCRIPTION    This function commands searcher4 to sweep using the
                specified priority queue.
DEPENDENCIES   None

RETURN VALUE   Srch4drv error value
SIDE EFFECTS   Srch4 structure is modified. A queue of searches is started

===========================================================================*/
int32 srch4_start_search
(
srch4_srch_type  srch4_type,            /* type of search                 */
srch4_queue_type queue_type,            /* queue to start                 */
int16  freq_offset,                     /* freq offset ( 4.6875 Hz/LSB )  */
uint16 priority,                        /* search priority (32767=highest)*/
boolean offline_search,                 /* indicate offline search        */
uint16  capturedRTCCnt,                 /* RTC count in CX2 at which sample 
                                           capture is done                */
void (*callback_func)(void *),          /* pointer to a callback function */
/*  to be called upon status dump */
void (*lost_dump_callback_func)(void *),/* pointer to a callback function */
/*  to be called upon lost dump   */
void *(cb_data)                        ,/* data to be passed to callback  */
srch4_mode_type  mode                  /* Srch mode */
#ifdef FEATURE_MODEM_COEXISTENCE_FW
,boolean          atomic_search
#endif
)
{
  srch4_queue_var_type  *queue;           /* to accommodate queues          */
  uint32                msg_flag = 0;     /* flag for errors and messages   */

#ifdef FEATURE_MODEM_COEXISTENCE_FW
  srch4i_slottedscheduler_timetick_type current_time = srch4i_slottedscheduler_get_current_ustmr();
#endif

#ifndef FEATURE_MODEM_COEXISTENCE_FW
  srchfw_search_cmd_msg_t search_cmd;
  errno_enum_type         estatus;        /* error code                     */

  /* Set the variables to zero */
  memset( &search_cmd, 0, sizeof( srchfw_search_cmd_msg_t ) );

  /* Initialize the message header */
  msgr_init_hdr( &search_cmd.hdr,
  MSGR_CDMA_SRCH_FW,
  CDMA_SRCH_FW_CMD_SRCH_START );
#else
  /* next slot time type */
  srch4_time_slot_type time_slot_next;

  /* variable to store the next inactivity start time */
  uint64 next_inactivity;

#endif
  /*-----------------------------------------------------------------------*/

  /* check if this technology was not assigned a queue */
  if ( queue_type == SRCH4_NO_QUEUE )
  {
    /* this is not right, mark this as an ERR */
    MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Tech: %d tried to start a search when not registered", srch4_type);

    return( SRCH4DRV_RET_INVALID_QUEUE );
  }

  /* check which queue to use, and store ptr */
  queue = srch4_get_queue( queue_type );

  /* check to ensure a valid queue has been assigned */
  if ( queue == NULL )
  {
    MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Trying to start a search on an invalid queue (%d)", queue_type );

    return( SRCH4DRV_RET_INVALID_QUEUE );
  }

  SRCH4_ENTER_CRIT_SECT();
    MSG_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Queue Active Info. Active: %d, Scheduled: %d ",
    queue->active, queue->scheduled_for_search );
  if ( (IS_QUEUE_ACTIVE(queue)) )
  {
    /* trying to start a search while another one is active */
    msg_flag = 1;
  }
  else if ( queue->num_tasks == 0 )
  {
    /* trying to start a search without any searches in the queue */
    msg_flag = 2;
  }
  /* check to make sure we have the same search types */
  else if ( ( srch4_type != queue->srch_type ) &&
  ( queue->num_tasks != 0 ) )
  {
    msg_flag = 3;
  }
  else
  {

#ifdef FEATURE_MODEM_COEXISTENCE_FW
    /* initialize num tasks scheduled to zero */    
    queue->num_tasks_scheduled = 0;

    /* copy the mode and RTC Count Data */
    queue->mode = mode;
    queue->original_mode = mode;
    queue->capturedRTCCnt = capturedRTCCnt;
#endif

    /* store the frequency offset */
    queue->freq_offset = freq_offset;

    /* store the priority of this search */
    queue->priority    = priority;

#ifndef FEATURE_MODEM_COEXISTENCE_FW
    /* Fill up the SRCH CMD message */
    search_cmd.queue       = srch4_map_srch4_queue_to_fw_queue( queue_type );
    search_cmd.search_type = srch4_map_srch4_tech_to_fw_tech_type(
    queue->srch_type );


    search_cmd.num_tasks   = queue->num_tasks;
    search_cmd.priority    = queue->priority;

    /* When technology is HDR, set the frequency offset, e.g. for binning */
    if ( ( srch4_type == SRCH4_HDR_SHOULDERS ) ||
    ( srch4_type == SRCH4_HDR_ACCUM ) )
    {
      /* program the frequency offset to apply to all searches */
      search_cmd.freq_offset = freq_offset;
    }
    else
    {
      /* For non-HDR technologies, the search rotator should not be used
         as there is a Rotator in the RX front */
      search_cmd.freq_offset = 0;
    }

    /* For 1x and HDR SHOULDERS comp_dis = 0, for HDR ACCUM comp_dis = 1 */
    search_cmd.peak_comp_dis = 0;
    search_cmd.peak_norm_dis = 0;

    /* Set the mode and RTC tick... */
    switch (mode)
    {
    case SRCH4_MODE_HDR_TRK_PILOTS:
      search_cmd.srch_mode = HDR_SRCH_ON_TRK_PILOTS;
      break;

    case SRCH4_MODE_IRAT:
      search_cmd.srch_mode = IRAT_SRCH;
      break;

    case SRCH4_MODE_DEFAULT:
    default:      
      search_cmd.srch_mode = DEFAULT;
      break;
    }
    search_cmd.cx2_rtc = capturedRTCCnt;

    MSG_5(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
    "Search started with search type %d on queue %d with %d tasks, %d mode, 0x%x rtc", 
    srch4_type, queue_type, queue->num_tasks, mode, capturedRTCCnt );

    /* Send the message */
    estatus = msgr_send( &search_cmd.hdr, sizeof( srchfw_search_cmd_msg_t ) );
    ASSERT( estatus == E_SUCCESS );

    /* mark searcher4 active, not aborted, and not lost */
    queue->active      = TRUE;
    queue->aborted     = FALSE;
    queue->scheduled_for_search = TRUE;
    queue->lost        = FALSE;
    queue->partial_res = FALSE;

#endif
    /* store the callback functions (these could be NULL) */
    queue->dump_cb_func = callback_func;
    queue->lost_cb_func = lost_dump_callback_func;
    queue->srch_cb_data = cb_data;

#ifdef FEATURE_MODEM_COEXISTENCE_FW
    /* copy the atomicity of the search */
    queue->atomic_search = atomic_search;
#endif

    /* store parameters into the log pkt while we're searching */
    srch4_log_params( queue_type );
    if ( queue->srch_type == SRCH4_1X )
    {
      srch4_log_srch4_sub_params( queue_type );
    }

#ifdef SRCH4_DEBUG
    #error code not present
#endif

#ifndef FEATURE_MODEM_COEXISTENCE_FW
    /* Start the Lost Dump timer
       This must be done here to ensure that all of the flags (active, aborted,
       lost) get set before a possible task switch (and thus interrupt
       enabling) that can occur with a call to timer_set() */
    timer_set( queue->lost_dump_timer,
    queue->lost_dump_time_estimate, 0, T_MSEC );
#else

    /* Get the next inactivity info. if there is an inactivity entry
       then we need to schedule the search in the next cycle else
       we should it schedule the entire search now */


    ASSERT((queue->num_tasks - 1) < SRCH4_MAX_HPQ_TASKS);

    /* get the next inactivity period */
    next_inactivity = srch4i_slottedscheduler_get_time_to_next_inactivity_slot(&time_slot_next);
    queue->scheduled_for_search = TRUE;
    if (queue->srch_type != SRCH4_AFLT && 
    time_slot_next.info_available == TRUE && 
    atomic_search == FALSE)
    {
      /* use right k */
      /* Store the lead to search scheduling */
      queue->lead_time_to_search = srch4_get_search_scheduling_lead_time(queue);

      /* create a timer for next search processing */
      next_inactivity += queue->lead_time_to_search; /* In microseconds */

      next_inactivity += 100; /* for timer resistence */

      queue->expected_next_schedule_time = time_slot_next.starting_time;

      MSG_3(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
      "GTX Active. Scheduling in next time slot. Schedule Task Timer(micro) %d. Current Time(USTMR): %ld. Next Time Slot Start: %ld", 
      next_inactivity, current_time, time_slot_next.starting_time);


      if (next_inactivity > MAX_RESCHEDULE_TIME_IN_MICROSEC)
      {
        MSG_2(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
        "Extremely high reschedule Time. duration: %d, lead time:%d", 
        next_inactivity, queue->lead_time_to_search);
      }

      timer_set(queue->schedule_task_timer, next_inactivity, 0, T_USEC );

      /* Set the mode to DSDA Offline */
      queue->mode = SRCH4_DSDA_OFFLINE;

      msg_flag = 4;

    }
    else
    {

      /* Since there is no CXM Info. There is no lead time */
      queue->lead_time_to_search = 0;

      /* Copy Search Info */
      srch4_copy_search_info(queue);

      /* Schedule the search */
      srch4_schedule_search(queue);

    }
#endif

    srch4_insert_history_rec(queue, FN_L1_START_SRCH);
  }

  SRCH4_LEAVE_CRIT_SECT();

  /* send MSG's and ERR's outside of INTLOCKs */
  switch ( msg_flag )
  {
  case 1:
    {
      /* trying to start a search while another one is active */
      MSG_SPRINTF_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
      "Searcher 4 %s already running",
      srch4_queue_str[queue_type] );

      return( SRCH4DRV_RET_ACTIVE );
    }
  case 2:
    {
      /* trying to start a search without any searches in the queue */
      MSG_SPRINTF_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
      "Searcher 4 trying to start an empty %s",
      srch4_queue_str[queue_type] );

      switch ( queue_type )
      {
      case SRCH4_HPQ0:
        {
          return( SRCH4DRV_RET_HPQ0_FREE );
        }
      case SRCH4_LPQ:
        {
          return( SRCH4DRV_RET_LPQ_FREE );
        }

      case SRCH4_HPQ1:
        {
          return( SRCH4DRV_RET_HPQ1_FREE );
        }

      default:
        {
          return( SRCH4DRV_RET_INVALID_QUEUE );
        }
      }
    }
  case 3:
    {
      /* trying to start a search of a different technology */
      MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
      "Trying to srch multi types (%s, qst=%d, rst=%d)",
      srch4_queue_str[(uint32)queue_type],
      SRCH4_TECH_STRING((uint32)queue->srch_type),
      SRCH4_TECH_STRING((uint32) srch4_type) );

      return( SRCH4DRV_RET_TECH_MISMATCH );
    }
#ifdef FEATURE_MODEM_COEXISTENCE_FW
  case 4:

    MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
    "Search in next time slot for %s on %s with %d tasks",
    SRCH4_TECH_STRING((uint32) srch4_type),
    srch4_queue_str[(uint32)queue_type],
    queue->num_tasks );
    /* return that srch4 has started */
    return( SRCH4DRV_RET_OK );
#endif
  default:
    {
      /* no errors to report */

      MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
      "Started search for %s on %s with %d tasks",
      SRCH4_TECH_STRING((uint32) srch4_type),
      srch4_queue_str[(uint32)queue_type],
      queue->num_tasks );

#ifdef SRCH4_DEBUG
      #error code not present
#endif

      /* return that srch4 has started */
      return( SRCH4DRV_RET_OK );
    }
  }

} /* srch4_start_search */


/*===========================================================================

FUNCTION       SRCH4_ABORT_QUEUE
DESCRIPTION    This function aborts the specified priority queue
DEPENDENCIES   None

RETURN VALUE   Srch4drv error value
SIDE EFFECTS   One queue in srch4 is aborted

===========================================================================*/
int32 srch4_abort_queue
(
srch4_srch_type         task_srch_type, /* search type                    */
srch4_queue_type        queue_type,     /* queue to abort tasks to        */
void                  (*abort_cb_func)( uint32, void * ),
/* pointer to a callback function */
/*  to be called upon abort dump  */
void                  *(cb_data),       /* callback data                  */
boolean                 sync,           /* synchronous abort flag         */
boolean                 part_res        /* partial results flag           */
)
{
  srch4_queue_var_type   *queue;          /* to accommodate queues        */
  uint32                  num_tasks  = 0; /* number of tasks to be aborted*/
  uint32                  msg_flag   = 0; /* flag for errors and messages */
  srchfw_abort_cmd_msg_t  abort_cmd;
  errno_enum_type         estatus;        /* error code                   */
#ifdef FEATURE_MODEM_COEXISTENCE_FW
  // ToDO: Need to remove.. debug code
  uint32 current_time_ustmr = 0;
#endif

  /* Set the variables to zero */
  memset( &abort_cmd, 0, sizeof( srchfw_abort_cmd_msg_t ) );

  /* Initialize the message header */
  msgr_init_hdr( &abort_cmd.hdr,
  MSGR_CDMA_SRCH_FW,
  CDMA_SRCH_FW_CMD_SRCH_ABORT );

  /*-----------------------------------------------------------------------*/

  /* check if this technology was not assigned a queue */
  if ( queue_type == SRCH4_NO_QUEUE )
  {
    /* this is not right, mark this as an ERR */
    MSG_1(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
    "Tech: %d tried to abort a search when not registered", task_srch_type);

    return( SRCH4DRV_RET_INVALID_QUEUE );
  }

  /* check which queue to use, and store ptr */
  queue = srch4_get_queue( queue_type );

  /* check to ensure a valid queue has been assigned */
  if ( queue == NULL )
  {
    MSG_1(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
    "Trying to abort a search on an invalid queue (%d)", task_srch_type);

    return( SRCH4DRV_RET_INVALID_QUEUE );
  }

  SRCH4_ENTER_CRIT_SECT();

  num_tasks = queue->num_tasks;

      MSG_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Queue Active Info. Active: %d, Scheduled: %d ",
    queue->active, queue->scheduled_for_search );

  /* searcher is not active thus a "real" abort does not need to be issued */
  if ( !queue->active )
  {
    /* is there an unstarted queue? is it assigned to this technology? */
    if ( ( queue->num_tasks != 0 ) && ( queue->srch_type != task_srch_type ) )
    {
      /* one technology is trying to abort another's search */
      msg_flag = 2;
    }
    else
    {
      /* same technology or empty queue */

      /* send no f3 msg since no abort is sent in this case */
      msg_flag = 0;

      /* searcher 4 queue not active, or active but empty,
         just clear programmed searches */
      queue->num_tasks = 0;

      /* clear the lost dump timers */
      (void) timer_clr( queue->lost_dump_timer, T_NONE );

      /* clear the lost dump time estimate */
      queue->lost_dump_time_estimate = 0;


      /* If the queue had been scheduled to be active */

      /* clear the schedule search timers */
#ifdef FEATURE_MODEM_COEXISTENCE_FW	  
      (void) timer_clr( queue->schedule_task_timer, T_NONE );
#endif

      /* clear the scheduled for search flag */
      queue->scheduled_for_search = FALSE;

      queue->num_tasks_scheduled  = FALSE;

    }
  }
  /* one technology is trying to abort another's active search */
  else if ( queue->srch_type != task_srch_type )
  {
    msg_flag = 2;
  }
  /* searcher was not really running thus a "real" abort does not
     need to be issued */
  else if ( (IS_QUEUE_ACTIVE(queue)) && ( queue->num_tasks == 0 ) )
  {
    /* send no f3 msg since no abort is sent in this case */
    msg_flag = 0;

    /* searcher 4 queue not active, or active but empty,
       just clear programmed searches */
    queue->num_tasks = 0;

    /* clear the lost dump timers */
    (void) timer_clr( queue->lost_dump_timer, T_NONE );

    /* clear the lost dump time estimate */
    queue->lost_dump_time_estimate = 0;

#ifdef FEATURE_MODEM_COEXISTENCE_FW
    /* clear the schedule search timers */
    (void) timer_clr( queue->schedule_task_timer, T_NONE );
#endif

    /* clear the scheduled for search flag */
    queue->scheduled_for_search = FALSE;

    queue->num_tasks_scheduled  = FALSE;

  }
  else /* valid abort, searcher queue was active */
  {
    /* send an f3 msg to show abort */
    msg_flag = 1;

    /* store the callback data */
    queue->abort_cb_func = abort_cb_func;
    queue->srch_cb_data  = cb_data;

    abort_cmd.queue = srch4_map_srch4_queue_to_fw_queue( queue_type );
    abort_cmd.search_type = srch4_map_srch4_tech_to_fw_tech_type(
    queue->srch_type );

    /* Send the message */
    estatus = msgr_send( &abort_cmd.hdr, sizeof( srchfw_abort_cmd_msg_t ) );
    ASSERT( estatus == E_SUCCESS );

#ifdef FEATURE_MODEM_COEXISTENCE_FW
    /* USTMR Ticks */
#ifdef FEATURE_BOLT_MODEM
    current_time_ustmr = VSTMR_XO_READ();
#else
    current_time_ustmr = HWIO_IN(UNIV_STMR_MSTMR);
#endif
#endif

#ifdef SRCH4_DEBUG
    #error code not present
#endif

    /* increase number of aborted searches */
    queue->abort_cnt++;

    /* flag this search as aborted */
    queue->aborted = TRUE;

    /* flag if partial results are wanted */
    queue->partial_res = part_res;

    /* Re-Start the Lost Dump timer just in case the abort is lost
       This must be done here to ensure that all of the flags (active, aborted,
       lost) get set before a possible task switch (and thus interrupt
       enabling) that can occur with a call to timer_set() */
    timer_set( queue->lost_dump_timer,
    queue->lost_dump_time_estimate, 0, T_MSEC );

    /* is this a synchronous abort, i.e complete the abort before we continue */
    /* REVISIT: No sync abort possible with MSGR implementation.
                Change the api to remove sync parameter?? */
    SRCH4_NOTUSED( sync );

  }

#ifdef FEATURE_MODEM_COEXISTENCE_FW
  /* clear the helper params of the queue */
  srch4_clear_helper_queue_params(queue);
#endif

  srch4_insert_history_rec(queue, FN_SRCH4_ABORT_SRCH);

  SRCH4_LEAVE_CRIT_SECT();

  switch ( msg_flag )
  {
  case 1:
    {
      if ( part_res )
      {
        MSG_3(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
        "Aborting Srch4 (%d on %d) (%d tasks prog) (Partial Res)",
        task_srch_type,
        queue_type,
        num_tasks);
      }
      else
      {
        MSG_3(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
        "Aborting Srch4 (on %d) (%d tasks prog), USTMR TIME:0x%x",
#ifdef FEATURE_MODEM_COEXISTENCE_FW
        queue_type, num_tasks, current_time_ustmr);
#else
        task_srch_type, queue_type, num_tasks);
#endif
      }

      /* queue aborted */
      return( SRCH4DRV_RET_OK );
    }

  case 2:
    {
      MSG_2(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
      "No abort processed: %d tried to abort %d's srch",
      task_srch_type ,
      queue->srch_type);

      /* queue aborted */
      return( SRCH4DRV_RET_TECH_MISMATCH );
    }

  default:
    {
      /* queue not active, or empty */
      switch ( queue_type )
      {
      case SRCH4_HPQ0:
        {
          return( SRCH4DRV_RET_FREE );
        }
      case SRCH4_LPQ:
        {
          return( SRCH4DRV_RET_FREE );
        }

      case SRCH4_HPQ1:
        {
          return( SRCH4DRV_RET_FREE );
        }

      default:
        {
          return( SRCH4DRV_RET_INVALID_QUEUE );
        }
      }
    }
  } /* switch */

} /* srch4_abort_queue */


/*===========================================================================

FUNCTION       SRCH4_PROCESS_STATUS_DUMP
DESCRIPTION    This function retrieves the results of the specified searcher4
                sweep. It fills the task structure then calls the specified
                callback function to process the dump.
DEPENDENCIES   Type of dump should have been checked (srch4_status_dump)
                This should not be a lost or an aborted dump (unless partial
                results are requested for aborted searches)

RETURN VALUE   None
SIDE EFFECTS   Srch4 structure is modified, processing callback is called

===========================================================================*/
#ifdef FEATURE_MODEM_COEXISTENCE_FW
void srch4_process_status_dump
(
srch4_queue_type        queue_type,      /* queue to process             */
uint32                  num_completed_tasks /* # of completed tasks      */
)
{
  srch4_queue_var_type   *queue    = NULL; /* to accommodate queues        */
  srch4_task_struct_type *tasks    = NULL; /* ptr to task struct to use    */

  int32                   msg_flag    = 0; /* flag for errors and messages */

  int32                   res_inx     = 0; /* result index variable        */
  uint32                  peak_inx    = 0; /* peak index variable          */

  int32                   peak_pos    = 0; /* peak position in Cx8         */
  uint32                  peak_pos_n  = 0; /* peak_position (normalized)   */
  uint16                  peak_eng    = 0; /* peak energy                  */
  uint16                  peak_ls_eng = 0; /* left shoulder energy         */
  uint16                  peak_rs_eng = 0; /* right shoulder energy        */

  uint32                  peak_posx2_n_raw = 0;
  /* uninterpolated peak position */
  uint16                  peak_eng_raw = 0; /* Uninterpolated peak energy  */
  uint32                  fw_queue_num = 0; /* variable to map to FW queue
                                             number                        */


  uint32                  fw_index  = 0;

  srch4_task_struct_type  *task;
  /*-----------------------------------------------------------------------*/

  /* check which queue to use, and store ptr */
  queue = srch4_get_queue( queue_type );

  /* check to ensure a valid queue has been assigned */
  if ( ( queue == NULL ) || ( queue->tasks == NULL ) )
  {
    MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Trying to process a status dump on an invalid queue (%d)",
    queue_type );

    return;
  }

  if (  queue->srch_type != SRCH4_AFLT )
  {
    /* Translate to fw queue type */
    fw_queue_num = srch4_map_srch4_queue_to_fw_queue( queue_type );
    if ( fw_queue_num >=2 )
    {
      return;
    }
  }


  tasks = queue->tasks;

  SRCH4_ENTER_CRIT_SECT();

  /* store the number of results */
  queue->last_srch_res_cnt = (uint8) num_completed_tasks;

    MSG_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Queue Active Info. Active: %d, Scheduled: %d ",
    queue->active, queue->scheduled_for_search );

  /* Check to make sure the searcher is active, this is a sanity check as it
     was checked from the calling function */
  if ( (IS_QUEUE_ACTIVE(queue)) )
  {


    /* Initialize it to already completed searches */
    res_inx = queue->num_completed_search;

    ASSERT(res_inx >= 0);

    /* If repeat flag is enabled */
    if ( queue->repeat_search )
    {
      /* Initialize num scheduled to previous completed tasks */
      queue->num_tasks_scheduled = res_inx;
    }

    else
    {

      /* New Change */
      num_completed_tasks += queue->num_completed_search;

      ASSERT(num_completed_tasks <= SRCH4_MAX_TASKS);

      /* loop over all finished tasks */
      for ( ; res_inx < num_completed_tasks; ++res_inx, ++fw_index )
      {
        task = &tasks[res_inx];

        /* Make window pos positive 0 ... 262143 */
        task->window_pos &= SRCH4_PN_CIRCLE_MASK;

        MSG_2(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
        "DUMP INFO FOR TASK %d.. Num Completed Tasks: %d", res_inx,num_completed_tasks);

        /* loop over all peaks */
        for ( peak_inx = 0; peak_inx < SRCH4_NUM_PEAKS; peak_inx++ )
        {
          /* read out results for this peak and task */
          if ( queue->srch_type == SRCH4_AFLT )
          {
            SRCH4_BOUNDS_CHECK(fw_index, SRCH4_MAX_LPQ_TASKS);

            peak_eng_raw     = AFLT_RESULTS_BASE_ADDR[fw_index]
            [peak_inx].peakMag;
            peak_posx2_n_raw = AFLT_RESULTS_BASE_ADDR[fw_index]
            [peak_inx].peakPos;
            peak_ls_eng      = AFLT_RESULTS_BASE_ADDR[fw_index]
            [peak_inx].peakLeft;
            peak_rs_eng      = AFLT_RESULTS_BASE_ADDR[fw_index]
            [peak_inx].peakRight;
          }
          else
          {
            SRCH4_BOUNDS_CHECK(fw_index, SRCH4_MAX_TASKS);

            peak_eng_raw     = SRCH_RESULTS_BASE_ADDR[fw_queue_num][fw_index]
            [peak_inx].peakMag;
            peak_posx2_n_raw = SRCH_RESULTS_BASE_ADDR[fw_queue_num][fw_index]
            [peak_inx].peakPos;
            peak_ls_eng      = SRCH_RESULTS_BASE_ADDR[fw_queue_num][fw_index]
            [peak_inx].peakLeft;
            peak_rs_eng      = SRCH_RESULTS_BASE_ADDR[fw_queue_num][fw_index]
            [peak_inx].peakRight;
          }

          peak_pos  = peak_posx2_n_raw * SRCH4_CHIPX8_PER_HALF_CHIP;
          peak_eng    = peak_eng_raw;

          {
            /* If this was a receive divesity search */
            if ( tasks[res_inx].diversity == SRCH4_ANT_DUAL )
            {
              /* Shift all the results down since they were accumulated
                 from both antennae */
              peak_eng    >>= RXDIV_SHIFT;
              peak_ls_eng >>= RXDIV_SHIFT;
              peak_rs_eng >>= RXDIV_SHIFT;
            }

            /* Get peak position and energy via interpolation
                from shoulder energies */
            srch4_interpolate_peak( peak_pos, peak_eng, peak_ls_eng, peak_rs_eng,
            &peak_pos, &peak_eng );

            /* Make peak pos positive in Cx16 (0 ... 524286)
                Using ((pn mask for ChipX8) * (ChipX16 per ChipX8) + 1)*/
            peak_pos_n = peak_pos &
            ( SRCH4_PN_CIRCLE_MASK *
            SRCH4_CHIPX16_PER_EIGHTH_CHIP + 1);
          }

          /* store peak pos and peak eng */
          task->peak_pos[peak_inx]       = peak_pos_n;
          task->peak_eng[peak_inx]       = peak_eng;
          task->peak_posx2_raw[peak_inx] = peak_posx2_n_raw;
          task->peak_eng_raw[peak_inx]   = peak_eng_raw;

        } /* loop over the peaks */

        /* process the search with the given function */
        if ( task->process_results != NULL )
        {
          (*task->process_results)( &tasks[res_inx] );
        }

        /* one more search completed */
        queue->task_cnt++;
      } /* loop over results */

      /* we processed a queue, increase counter */
      queue->srch_cnt++;
    }
  }
  else /* searcher is not active */
  {
    msg_flag = 1;
  }

  SRCH4_LEAVE_CRIT_SECT();

  /* send MSG's and ERR's outside of INTLOCKs */
  switch ( msg_flag )
  {
  case 1:
    {
      MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
      "Srch4 %d Status Dump while inactive", queue_type );
      break;
    }
  default:
    {
      /* no errors to report */
    }
  }

} /* srch4_process_status_dump */
#else
void srch4_process_status_dump
(
srch4_queue_type        queue_type,      /* queue to process             */
uint32                  num_completed_tasks /* # of completed tasks      */
)
{
  srch4_queue_var_type   *queue    = NULL; /* to accommodate queues        */
  srch4_task_struct_type *tasks    = NULL; /* ptr to task struct to use    */

  int32                   msg_flag    = 0; /* flag for errors and messages */

  uint32                  res_inx     = 0; /* result index variable        */
  uint32                  peak_inx    = 0; /* peak index variable          */

  int32                   peak_pos    = 0; /* peak position in Cx8         */
  uint32                  peak_pos_n  = 0; /* peak_position (normalized)   */
  uint16                  peak_eng    = 0; /* peak energy                  */
  uint16                  peak_ls_eng = 0; /* left shoulder energy         */
  uint16                  peak_rs_eng = 0; /* right shoulder energy        */

  uint32                  peak_posx2_n_raw = 0;
  /* uninterpolated peak position */
  uint16                  peak_eng_raw = 0; /* Uninterpolated peak energy  */
  uint32                  fw_queue_num = 0; /* variable to map to FW queue
                                             number                        */
  /*-----------------------------------------------------------------------*/

  /* check which queue to use, and store ptr */
  queue = srch4_get_queue( queue_type );

  /* check to ensure a valid queue has been assigned */
  if ( ( queue == NULL ) || ( queue->tasks == NULL ) )
  {
    MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Trying to process a status dump on an invalid queue (%d)",
    queue_type );

    return;
  }

  if (  queue->srch_type != SRCH4_AFLT )
  {
    /* Translate to fw queue type */
    fw_queue_num = srch4_map_srch4_queue_to_fw_queue( queue_type );
    if ( fw_queue_num >=2 )
    {
      MSG_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
      "Non AFLT search (%d) is using Low priority queue (%d)",
      queue->srch_type, queue_type );
      return;
    }
  }


  tasks = queue->tasks;

  SRCH4_ENTER_CRIT_SECT();

  /* store the number of results */
  queue->last_srch_res_cnt = (uint8) num_completed_tasks;

    MSG_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Queue Active Info. Active: %d, Scheduled: %d ",
    queue->active, queue->scheduled_for_search );
  /* Check to make sure the searcher is active, this is a sanity check as it
     was checked from the calling function */
  if ( (IS_QUEUE_ACTIVE(queue)) )
  {
    /* loop over all finished tasks */
    for ( res_inx = 0; res_inx < num_completed_tasks; res_inx++ )
    {
      /* Make window pos positive 0 ... 262143 */
      tasks[res_inx].window_pos &= SRCH4_PN_CIRCLE_MASK;

      /* loop over all peaks */
      for ( peak_inx = 0; peak_inx < SRCH4_NUM_PEAKS; peak_inx++ )
      {
        /* read out results for this peak and task */
        if ( queue->srch_type == SRCH4_AFLT )
        {
          peak_eng_raw     = AFLT_RESULTS_BASE_ADDR[res_inx]
          [peak_inx].peakMag;
          peak_posx2_n_raw = AFLT_RESULTS_BASE_ADDR[res_inx]
          [peak_inx].peakPos;
          peak_ls_eng      = AFLT_RESULTS_BASE_ADDR[res_inx]
          [peak_inx].peakLeft;
          peak_rs_eng      = AFLT_RESULTS_BASE_ADDR[res_inx]
          [peak_inx].peakRight;
        }
        else
        {

          peak_eng_raw     = SRCH_RESULTS_BASE_ADDR[fw_queue_num][res_inx]
          [peak_inx].peakMag;
          peak_posx2_n_raw = SRCH_RESULTS_BASE_ADDR[fw_queue_num][res_inx]
          [peak_inx].peakPos;
          peak_ls_eng      = SRCH_RESULTS_BASE_ADDR[fw_queue_num][res_inx]
          [peak_inx].peakLeft;
          peak_rs_eng      = SRCH_RESULTS_BASE_ADDR[fw_queue_num][res_inx]
          [peak_inx].peakRight;
        }

        peak_pos  = peak_posx2_n_raw * SRCH4_CHIPX8_PER_HALF_CHIP;
        peak_eng    = peak_eng_raw;

        {
          /* If this was a receive divesity search */
          if ( tasks[res_inx].diversity == SRCH4_ANT_DUAL )
          {
            /* Shift all the results down since they were accumulated
               from both antennae */
            peak_eng    >>= RXDIV_SHIFT;
            peak_ls_eng >>= RXDIV_SHIFT;
            peak_rs_eng >>= RXDIV_SHIFT;
          }

          /* Get peak position and energy via interpolation
              from shoulder energies */
          srch4_interpolate_peak( peak_pos, peak_eng, peak_ls_eng, peak_rs_eng,
          &peak_pos, &peak_eng );

          /* Make peak pos positive in Cx16 (0 ... 524286)
              Using ((pn mask for ChipX8) * (ChipX16 per ChipX8) + 1)*/
          peak_pos_n = peak_pos &
          ( SRCH4_PN_CIRCLE_MASK *
          SRCH4_CHIPX16_PER_EIGHTH_CHIP + 1);
        }

        /* store peak pos and peak eng */
        tasks[res_inx].peak_pos[peak_inx]       = peak_pos_n;
        tasks[res_inx].peak_eng[peak_inx]       = peak_eng;
        tasks[res_inx].peak_posx2_raw[peak_inx] = peak_posx2_n_raw;
        tasks[res_inx].peak_eng_raw[peak_inx]   = peak_eng_raw;

      } /* loop over the peaks */

      /* process the search with the given function */
      if ( tasks[res_inx].process_results != NULL )
      {
        (*tasks[res_inx].process_results)( &tasks[res_inx] );
      }

      /* one more search completed */
      queue->task_cnt++;
    } /* loop over results */

    /* we processed a queue, increase counter */
    queue->srch_cnt++;
  }
  else /* searcher is not active */
  {
    msg_flag = 1;
  }

  SRCH4_LEAVE_CRIT_SECT();

  /* send MSG's and ERR's outside of INTLOCKs */
  switch ( msg_flag )
  {
  case 1:
    {
      MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
      "Srch4 %d Status Dump while inactive", queue_type );
      break;
    }
  default:
    {
      /* no errors to report */
    }
  }

} /* srch4_process_status_dump */
#endif


/*===========================================================================

FUNCTION       SRCH4_HANDLE_ABORT_DONE
DESCRIPTION    This function is called when an abort has finished.
DEPENDENCIES   None

RETURN VALUE   None.
SIDE EFFECTS   if partial results are wanted, the finished tasks results will
               be stored.
===========================================================================*/
void srch4_handle_abort_done
(
srch4_queue_type      queue_type,         /* HPQ0, HPQ1, or LPQ        */
#ifdef FEATURE_MODEM_COEXISTENCE_FW
srch4_queue_var_type
#else
const srch4_queue_var_type
#endif
*queue,              /* to accommodate queues     */
uint32                num_results,        /* >0 if partial results are
                                             wanted and are available  */
boolean               abort_flag          /* aborted dump              */
)
{
  uint32                i,j;                /* loop counter              */
  /*-----------------------------------------------------------------------*/

  if ( queue == NULL )
  {
    MSG( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "srch4_handle_abort_done got NULL queue ptr" );

    return;
  }


#ifdef SRCH4_DEBUG
  #error code not present
#endif

  /* Does the DSP think that this is an abort? */
  if ( !abort_flag )
  {
    MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Expected Abort, instead got normal status dump (%d)",
    queue_type );
  }

  /* either way, process this like we would an abort whether or not it
     was an abort, because that's what is expected */

  /* clear the lost dump timer, so it doesn't fire while we process */
  (void) timer_clr( queue->lost_dump_timer, T_NONE );

  /* store the partial results, if we've got it! */
  if ( num_results > 0 )
  {
    /* process the returned tasks */
    srch4_process_status_dump( queue_type, num_results );

    /* clear the remaining tasks so logging can indicate which weren't
       completed */
    for ( i = num_results; i < queue->num_tasks; i++ )
    {
      for ( j = 0; j < SRCH4_NUM_PEAKS; j++ )
      {
        queue->tasks[i].peak_eng[j] = 0;
        queue->tasks[i].peak_pos[j] = 0;
      }
    }
  }

#ifdef FEATURE_MODEM_COEXISTENCE_FW
  /* reset queue paramaeters */
  queue->num_completed_search = 0;
  queue->num_tasks_scheduled = 0;
  queue->active = FALSE;
  queue->scheduled_for_search = FALSE;
  queue->num_tasks = 0;
#endif

} /* srch4_handle_abort_done */


/*===========================================================================

FUNCTION       SRCH4_EVAL_STATUS_DUMP
DESCRIPTION    Checks a specified queue to determine if it triggered the
                status dump, then takes the appropriate action
DEPENDENCIES   None

RETURN VALUE   Number of tasks whose results were stored
               SRCH4_NO_DUMP (-1) if this queue did not cause the dump
SIDE EFFECTS   Search results are processed

===========================================================================*/
int32 srch4_eval_status_dump
(
srch4_queue_type      queue_type,         /* the queue type             */
#ifdef FEATURE_MODEM_COEXISTENCE_FW
srch4_queue_var_type
#else
const srch4_queue_var_type
#endif
*queue,              /* to accommodate queues      */
uint32                num_results,        /* number of results returned */
boolean               abort_flag          /* aborted dump               */
)
{
  int32                 stored_results = SRCH4_NO_DUMP;
  /* num of results stored      */
  /*-----------------------------------------------------------------------*/

  if ( queue == NULL )
  {
    MSG( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "srch4_eval_status_dump got NULL queue ptr" );

    return( SRCH4_NO_DUMP );
  }
    MSG_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Queue Active Info. Active: %d, Scheduled: %d ",
    queue->active, queue->scheduled_for_search );

  /* check to make sure this is the queue sending the dump */
  if ( ( (IS_QUEUE_ACTIVE(queue)) ) &&
  ( ( queue->lost ) ||
  ( queue->aborted ) ||
#ifdef FEATURE_MODEM_COEXISTENCE_FW
  ( queue->num_tasks_scheduled == (queue->num_completed_search + num_results) ) ) )
#else
  ( queue->num_tasks == num_results ) ) )
#endif
  {
    /* clear the lost dump timer, so it doesn't fire while we process */
    (void) timer_clr( queue->lost_dump_timer, T_NONE );

    /* lost queue was aborted and has now dumped */
    if ( ( queue->lost ) && ( queue->aborted ) )
    {
      /* Handle as you would an aborted search, but ensure that all results
         are thrown away (hence the "0" for number of results!
         Call the abort callback since the lost dump callback was assigned
         to in the lost dump callback wrapper */
      srch4_handle_abort_done( queue_type, queue, 0, abort_flag );

      /* search was lost, so no results were stored */
      stored_results = 0;
    }
    /* aborted queue has dumped */
    else if ( queue->aborted )
    {
      /* call the abort handler, with notification of whether partial results
         are wanted */
      if ( queue->partial_res )
      {
        srch4_handle_abort_done( queue_type, queue, num_results, abort_flag );

        /* search was aborted, and partial results was requested */
        stored_results = num_results;
      }
      else
      {
        srch4_handle_abort_done( queue_type, queue, 0, abort_flag );

        /* search was aborted, and partial results was not requested */
        stored_results = 0;
      }
    }
    else if ( queue->lost )
    {
      /* this case should never happen, if it's lost it should also always
         be aborted */
      MSG_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
      "Srch4 lost dump without an abort! (%d/%d)",
      queue_type, queue->srch_type );

      /* search was lost, so no results were stored */
      stored_results = 0;
    }
    /* normal dump */
#ifdef FEATURE_MODEM_COEXISTENCE_FW
    /* Do we have a result to process ? */
    else if ( num_results > 0 )
#else
    else if ( queue->num_tasks == num_results )
#endif
    {
      srch4_process_status_dump( queue_type, num_results );

      /* normal search dump, all results recorded */
      stored_results = num_results;
    }
#ifdef FEATURE_MODEM_COEXISTENCE_FW
    else if ( num_results == 0 )
    {
      MSG(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, "No results obtained and reprogramming all tasks");
      stored_results = num_results;
    }
#endif

#ifdef SRCH4_DEBUG
    #error code not present
#endif

    /* return the number of results recorded */
    return( stored_results );

  } /* queue caused dump? */
#ifdef FEATURE_MODEM_COEXISTENCE_FW
  else
  {
    MSG_3(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
    "Eval Status Dump Failed. Num Completed Search %d, Num Tasks Scheduled: %d, Num results %d", 
    queue->num_completed_search, queue->num_tasks_scheduled, num_results);
  }
#endif

  /* return value of SRCH4_NO_DUMP indicates this queue did
     not issue the dump */
  return( SRCH4_NO_DUMP );

} /* srch4_eval_status_dump */


/*===========================================================================

FUNCTION       SRCH4_PROCESS_DUMP
DESCRIPTION    This function is used when a status dump is triggered.
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   Callbacks will be called

===========================================================================*/
void srch4_process_dump
(
srchfw_search_rsp_msg_t* search_rsp,       /* Response msg from FW       */
boolean                  flag_a            /* MDSP abort flags           */
)
{
  srch4_queue_var_type *queue;               /* to accommodate queues      */
  int32                 num_results = 0;     /* number of results returned */
  int32                 dump;                /* indicates a processed dump */
  srch4_queue_type      queue_id = SRCH4_NO_QUEUE; 
  /* loop var for queues        */
  boolean               aborted = FALSE;     /* flag for callbacks         */
  boolean               lost    = FALSE;     /* flag for callbacks         */
  srch4_srch_type       tech;                /* Tech that has aborted */  
#ifdef FEATURE_MODEM_COEXISTENCE_FW
  uint32                counter;
  uint32                old_num_scheduled;
  uint64                duration;
#endif
  /*-----------------------------------------------------------------------*/

#ifdef FEATURE_MODEM_COEXISTENCE_FW
  srch4_time_slot_type time_slot_next;
#endif
  /* Convert FW queue to SRCH4 queue types */
  if ( search_rsp != NULL )
  {
    queue_id  = srch4_map_fw_queue_to_srch4_queue( search_rsp->queue );
  }
  else
  {
    ERR_FATAL("Null pointer: search_rsp", 0, 0, 0);
  }

  dump = SRCH4_NO_DUMP;

  queue   = srch4_get_queue( queue_id );
  /* check if a valid queue was returned */
  if ( queue != NULL )
  {
    MSG_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
    "Queue Active Info. Active: %d, Scheduled: %d ",
    queue->active, queue->scheduled_for_search );

    if (!(IS_QUEUE_ACTIVE(queue)))
    {
      MSG(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, "Redundant Process Dump due to abort/dump-complete race condition");
      return;
    }

    num_results = search_rsp->numResults;
#ifdef FEATURE_MODEM_COEXISTENCE_FW
    if (queue->mode == SRCH4_DSDA_OFFLINE)
    {
      MSG_2(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
      "Num Tasks Scheduled: %d, Num Completed Tasks: %d",
      queue->num_tasks_scheduled, queue->num_completed_search);
      for ( counter = 0; counter < (queue->num_tasks_scheduled- queue->num_completed_search ); ++counter )
      {
        if (!search_rsp->taskValid[counter])
        {
          if (counter == 0)
          {
            if (queue->num_zero_result_searches > 3)
            {
              MSG(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
              "No Search result obtained for 3 slots continuous. Call Lost Dump Call-back Wrapper");
              /* clear the timer */
              (void) timer_clr( queue->lost_dump_timer, T_NONE );
              srch4_lost_dump_cb_wrapper(queue_id);
              return;
            }
            else
            {
              ++queue->num_zero_result_searches;
              MSG_1(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
              "Zero result search obtained. Count: %d", queue->num_zero_result_searches);
            }
          }
          else
          {
            queue->num_zero_result_searches = 0;
          }

          num_results = counter;
          old_num_scheduled = queue->num_tasks_scheduled;
          queue->num_tasks_scheduled = queue->num_completed_search + num_results;
          MSG_3(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
          "Search over-run occurred. Task Invalid: %d.. Old num scheduled: %d.. New num scheduled: %d", 
          counter,old_num_scheduled,queue->num_tasks_scheduled);
          break;
        }
      }
    }

    if ( !(queue->aborted) && (num_results != (queue->num_tasks_scheduled- queue->num_completed_search )))
    {
      ERR_FATAL("Search result mismatch between num results and num valid searches. Current USTMR: 0x%x", 
      srch4i_slottedscheduler_get_current_ustmr(),0,0);
    }
#endif	

    dump = srch4_eval_status_dump(queue_id, queue, num_results, flag_a );

#ifdef FEATURE_MODEM_COEXISTENCE_FW
    MSG_5(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
    "Process dump initiated for tech %d, queue %d, num res %d, current time(USTMR) %ld, Search Mode: %d",
    queue->srch_type, queue_id, num_results, srch4i_slottedscheduler_get_current_ustmr(), queue->mode);
#else
    MSG_4(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,"Process dump initiated for tech %d, queue %d, num res %d, Search Mode: %d",
    queue->srch_type, queue_id, num_results, queue->mode);
#endif

    ASSERT(dump <= SRCH4_MAX_HPQ_TASKS);
    if ( dump != SRCH4_NO_DUMP)
    {
      /* reset aborted or lost flag */
      aborted = FALSE;
      lost    = FALSE;

      /* log and report only if we had a normal status dump */
      if ( !( queue->aborted || queue->lost ) )
      {

#ifndef FEATURE_MODEM_COEXISTENCE_FW
        /* log the results and send the log pkt */
        srch4_log_results( queue_id, dump );
        if ( queue->srch_type == SRCH4_1X )
        {
          srch_genlog_1x_srch4_dump( (uint32) queue_id, dump );
        }
#endif

      }
      /* was the search lost? */
      else if ( queue->lost )
      {
        MSG_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
        "Srch4 Lost for tech %d on %d",
        queue->srch_type, queue_id );

        /* set lost flag */
        lost = TRUE;

        /* clear the log ptr (it was never assigned) */
        queue->log_ptr = NULL;
      }
      /* if we are saving partial results, log but we've already sent a msg */
      else if ( queue->aborted && queue->partial_res )
      {
        MSG_4( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
        "Srch4 Partial dump for tech %d on %d with %d of %d tasks",
        queue->srch_type, queue_id, dump, queue->num_tasks );

        /* set aborted flag */
        aborted = TRUE;
      }
      /* was the search aborted? */
      else if ( queue->aborted )
      {
        MSG_2(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,"Srch4 successfully aborted for tech %d on %d",
        queue->srch_type, queue_id);

        /* set aborted or lost flag */
        aborted = TRUE;

        /* clear the log ptr (it was never assigned) */
        queue->log_ptr = NULL;
      }
      /* something weird, just clear log ptr and continue */
      else
      {
        queue->log_ptr = NULL;
      }

#ifdef FEATURE_MODEM_COEXISTENCE_FW
      /* Is there a callback we are supposed to call? */
      if ( lost )
      {
        /* ToDo: This chunk is repeated multiple times */
        /* give the translation layer a chance to clean up also */
        srch4_trans_cleanup_dump( queue->srch_type );

        MSG_1(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
        "Queue %d set to inactive and num tasks to zero in process dump",
        (srch4_queue_type) queue_id);

        /* clear num tasks */


        /* mark searcher as inactive and not aborted */
        queue->active  = FALSE;
        queue->scheduled_for_search = FALSE;
        queue->aborted = FALSE;
        queue->lost    = FALSE;
        queue->num_tasks = 0;
        /* clear the lost dump time estimate */
        queue->lost_dump_time_estimate = 0;

        /* save the tech type for logging */
        tech = queue->srch_type; 

        /* set the technology to "invalid" */
        queue->srch_type = SRCH4_NO_TECH;
        if ( queue->lost_cb_func != NULL )
        {
          /* Call the callback */
          (*queue->lost_cb_func)( queue->srch_cb_data );
          MSG_2(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,"Lost callback initiated for tech %d, queue %d",
          tech, queue_id);
        }
      }
      else if ( aborted )
      {
        /* give the translation layer a chance to clean up also */
        srch4_trans_cleanup_dump( queue->srch_type );

        MSG_1(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
        "Queue %d set to inactive and num tasks to zero in process dump",
        (srch4_queue_type) queue_id);

        /* mark searcher as inactive and not aborted */
        queue->active  = FALSE;
        queue->scheduled_for_search = FALSE;
        queue->aborted = FALSE;
        queue->lost    = FALSE;
        queue->num_tasks = 0;

        /* clear the lost dump time estimate */
        queue->lost_dump_time_estimate = 0;

        /* save the tech type for logging */
        tech = queue->srch_type; 

        /* set the technology to "invalid" */
        queue->srch_type = SRCH4_NO_TECH;
        if ( queue->abort_cb_func != NULL )
        {
          /* Call the callback */
          (*queue->abort_cb_func)( dump, queue->srch_cb_data );
          MSG_2(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,"Abort callback initiated for tech %d, queue %d",
          tech, queue_id);
        }
      }
      else /* normal status dump */
      {
		MSG(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,"normal status dump, MODEM_COEXISTENCE_FW");
        /* call the callback function */
        if ( (queue->num_tasks_scheduled == queue->num_tasks) && queue->dump_cb_func != NULL )
        {

          /* log the results and send the log pkt */
          srch4_log_results( queue_id, queue->num_tasks );
          if ( queue->srch_type == SRCH4_1X )
          {
            srch_genlog_1x_srch4_dump( (uint32) queue_id, queue->num_tasks );
          }

          /* give the translation layer a chance to clean up also */
          srch4_trans_cleanup_dump( queue->srch_type );

          /* mark searcher as inactive and not aborted */
          queue->active  = FALSE;
          queue->scheduled_for_search = FALSE;
          queue->aborted = FALSE;
          queue->lost    = FALSE;

          /* clear the lost dump time estimate */
          queue->lost_dump_time_estimate = 0;

          /* save the tech type for logging */
          tech = queue->srch_type; 

          /* set the technology to "invalid" */
          queue->srch_type = SRCH4_NO_TECH;

          MSG_3(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
          "Dump callback initiated for tech %d, queue %d num tasks scheduled %d",
          tech, queue_id, queue->num_tasks_scheduled); 

          /* clear queue parameters */
          queue->num_tasks = 0;
          queue->num_completed_search = 0;
          queue->num_tasks_scheduled = 0;

          /* clear queue helper parameters */
          srch4_clear_helper_queue_params(queue);
          (void) timer_clr( queue->lost_dump_timer, T_NONE );

          (*queue->dump_cb_func)( queue->srch_cb_data );

        }
        else
        {

          MSG_2(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
          "Partial Process Dump: Num Tasks Scheduled - %d, Num Tasks Total - %d", 
          queue->num_tasks_scheduled, queue->num_tasks);

          /* Number of completed search should be number of tasks scheduled */
          queue->num_completed_search = queue->num_tasks_scheduled;

          /* clear the lost dump timers */
          (void) timer_clr( queue->lost_dump_timer, T_NONE );

          /* create a timer for next search processing */




          duration = srch4i_slottedscheduler_get_time_to_next_inactivity_slot(&time_slot_next);
          queue->expected_next_schedule_time = time_slot_next.starting_time;
          duration += queue->lead_time_to_search; /* In microseconds */
          duration += 100; /* for timer resistence */
          MSG_2(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
          "Reset the next search info in process dump. Duration: %ld Lead Time: %d",
          duration, queue->lead_time_to_search);

          if (duration > MAX_RESCHEDULE_TIME_IN_MICROSEC)
          {
            MSG_2(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
            "Extremely high reschedule Time. duration: %d, lead time:%d", 
            duration, queue->lead_time_to_search);
          }

          /* set the next processing timer */
          timer_set(queue->schedule_task_timer, duration, 0, T_USEC);
        }
      }

      srch4_insert_history_rec(queue, FN_FW_PROCESS_DUMP);

#else
      /* give the translation layer a chance to clean up also */
      srch4_trans_cleanup_dump( queue->srch_type );

      MSG_1(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
      "Queue %d set to inactive and num tasks to zero in process dump",
      (srch4_queue_type) queue_id);

      /* clear num tasks */
      queue->num_tasks = 0;

      /* mark searcher as inactive and not aborted */
      queue->active  = FALSE;
      queue->scheduled_for_search = FALSE;
      queue->aborted = FALSE;
      queue->lost    = FALSE;

      /* clear the lost dump time estimate */
      queue->lost_dump_time_estimate = 0;

      /* save the tech type for logging */
      tech = queue->srch_type; 

      /* set the technology to "invalid" */
      queue->srch_type = SRCH4_NO_TECH;

      /* Is there a callback we are supposed to call? */
      if ( lost )
      {
        if ( queue->lost_cb_func != NULL )
        {
          /* Call the callback */
          (*queue->lost_cb_func)( queue->srch_cb_data );
          MSG_2(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,"Lost callback initiated for tech %d, queue %d",
          tech, queue_id);
        }
      }
      else if ( aborted )
      {
        if ( queue->abort_cb_func != NULL )
        {
          /* Call the callback */
          (*queue->abort_cb_func)( dump, queue->srch_cb_data );
          MSG_2(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,"Abort callback initiated for tech %d, queue %d",
          tech, queue_id);
        }
      }
      else /* normal status dump */
      {
	MSG(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,"normal status dump");
        /* call the callback function */
        if ( queue->dump_cb_func != NULL )
        {
          (*queue->dump_cb_func)( queue->srch_cb_data );
          MSG_2(MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,"Dump callback initiated for tech %d, queue %d",
          tech, queue_id);
        }
      }
#endif
    } /* dump != SRCH4_NO_DUMP */
  } /* queue != NULL */

} /* srch4_process_dump */


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
void srch4_halt( void )
{

  srch4_queue_var_type *queue;               /* to accommodate queues      */
  srch4_queue_type      i;                   /* loop var for queues        */

  /*-----------------------------------------------------------------------*/

  /* loop through all queues */
  for ( i = SRCH4_MIN_Q; i < SRCH4_MAX_Q; i++ )
  {
    queue = srch4_get_queue( i );

    /* check if a valid queue was returned */
    if ( queue != NULL )
    {
      MSG_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4, 
      "Queue Active Info. Active: %d, Scheduled: %d ",
      queue->active, queue->scheduled_for_search );

      /* abort any search that is active */
      if ( IS_QUEUE_ACTIVE(queue) )
      {
        /* send a synchrous abort to ensure that searcher 4 is completely
           inactive before this function is exited */
        (void)srch4_abort_queue( queue->srch_type, i, NULL, NULL, TRUE, FALSE );
      }

      /* clear every queue's status variables */
      queue->srch_type = SRCH4_NO_TECH;
      queue->active    = FALSE;
      queue->aborted   = FALSE;
      queue->scheduled_for_search = FALSE;
      queue->lost      = FALSE;
      queue->num_tasks = 0;
#ifdef FEATURE_MODEM_COEXISTENCE_FW
      /* clear all the helper parameters of the queue */
      srch4_clear_helper_queue_params(queue);
#endif

      queue->lost_dump_time_estimate = 0;

      /* clear out the callbacks so an inadvertant dump will not occur */
      queue->dump_cb_func  = NULL;
      queue->abort_cb_func = NULL;
      queue->lost_cb_func  = NULL;
    } /* queue != NULL */
  } /* queue loop */

#ifdef SRCH4_DEBUG
  #error code not present
#endif

} /* srch4_halt */

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
void srch4_clear_helper_queue_params(
srch4_queue_var_type *queue              /* to accommodate queues      */
)
{

  /*-----------------------------------------------------------------------*/

  /* Reinitialize all the queue paramaters */
  queue->repeat_search = FALSE;
  queue->original_mode = SRCH4_MODE_DEFAULT;
  queue->mode = SRCH4_MODE_DEFAULT;
  queue->capturedRTCCnt = 0; 
  queue->atomic_search  = FALSE;
  queue->time_slot_duration = 0;
  queue->time_slot_start    = 0;
  queue->lead_time_to_search = 0;
  queue->num_zero_result_searches = 0;
  queue->expected_next_schedule_time = 0;

  /* clear the reschudule timer. most likely a abort occurred */
  (void) timer_clr( queue->schedule_task_timer, T_NONE );

}



/*============================================================================

FUNCTION SRCH4_CALCULATE_LEAD_TIME_TO_SEARCH

DESCRIPTION
  This function calculates the lead time w.r.t task scheduling.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.  

============================================================================*/
int64 srch4_calcuate_lead_time_to_search_scheduling(
srch4_srch_type search_type,               /* search type             */
uint16          coherent_len,              /* coherent length          */
uint8           non_coherent_len,          /* non-coherent length      */
uint8           pilot_bursts               /* pilot burst length       */
)
{
  int64 lead_time = 0;
  const uint32 lead_time_slack = 16;

  /*-----------------------------------------------------------------------*/


  /* 
  Formula to calculate lead time for search: 
   1X: MAX(MAXn (M*N+8) - 16, 0) 
   DO: MAX(MAXn (1024*(P+1) - 96) - 16, 0)
 */

  if ( search_type == SRCH4_1X )
  {
    /* (M*N+8) – 16 */
    /* lead time in chips*/
    lead_time = ((coherent_len * non_coherent_len) + 8) - lead_time_slack; 
  }
  else
  {
    /* (1024*(P+1)-96) – 16 */
    lead_time = (((pilot_bursts + 1) * 1024) - 96) - lead_time_slack; /* lead time in chips*/
  }

  /* Lead time obtained in chips */


  if ( lead_time <= 0 )
  {
    /* We cannot have a negative lead time. make it zero */
    lead_time = 0;
  }
  else
  {
    /* Convert chips to microseconds */
    lead_time *= 814; /* convert chips to ns */
    lead_time /= 1000; /* convert ns to micro-seconds */
  }

  return lead_time;

}




/*============================================================================

FUNCTION SRCH4_CALCULATE_LEAD_TIME_TO_TASK_ESTIMATION

DESCRIPTION
  This function calculates the lead time to search.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.  

============================================================================*/
int64 srch4_calculate_lead_time_to_task_estimation(
srch4_srch_type search_type,               /* search type             */
uint16          coherent_len,              /* coherent length          */
uint8           non_coherent_len,          /* non-coherent length      */
uint8           pilot_bursts               /* pilot burst length       */
)
{
  int64 lead_time = 0;


  /*-----------------------------------------------------------------------*/
  /* 
  Formula to calculate lead time for search: 
   1X: MAX(MAXn (M*N+8), 0) 
   DO: MAX(MAXn (1024*(P), 0)
 */

  if ( search_type == SRCH4_1X )
  {
    /* (M*N+8) Ö 128 */
    lead_time = (coherent_len * non_coherent_len) + 8; /* lead time in chips*/
  }
  else
  {
    /* (1024*(P))-96 */
    lead_time = (pilot_bursts * 1024) - 96; /* lead time in chips*/
  }

  /* Lead time obtained in chips */


  if ( lead_time <= 0 )
  {
    /* We cannot have a negative lead time. make it zero */
    lead_time = 0;
  }
  else
  {
    /* Convert chips to microseconds */
    lead_time *= 814; /* convert chips to ns */
    lead_time /= 1000; /* convert ns to micro-seconds */
  }

  return lead_time;

}


/*============================================================================

FUNCTION SRCH4_GET_TASK_ESTIMATION_LEAD_TIME

DESCRIPTION
  This function returns the lead time to for task estimation.
 
DEPENDENCIES
  None

RETURN VALUE
  Lead Time w.r.t task estimation.

SIDE EFFECTS
 None.  

============================================================================*/
int64 srch4_get_task_estimation_lead_time(
srch4_queue_var_type  *queue
)
{
  uint32 counter = 0;
  int64  lead_time = 0;
  srch4_task_struct_type *task = NULL;
  int64  temp_lead_time = 0;

  /*-----------------------------------------------------------------------*/

  for (counter = queue->num_tasks_scheduled; counter < queue->num_tasks; ++counter)
  {
    task = &queue->tasks[counter];
    temp_lead_time = srch4_calculate_lead_time_to_task_estimation(
    queue->srch_type,
    task->coherent_len,
    task->non_coherent_len,
    task->burst_len
    );

    if (temp_lead_time > lead_time)
    {
      lead_time = temp_lead_time;
    }
  }

  return lead_time;
}

/*============================================================================

FUNCTION SRCH4_GET_SEARCH_SCHEDULING_LEAD_TIME

DESCRIPTION
  This function returns the lead time w.r.t. search scheduling.
 
DEPENDENCIES
  None

RETURN VALUE
  lead time w.r.t search scheduling.

SIDE EFFECTS
 None.  

============================================================================*/
int64 srch4_get_search_scheduling_lead_time(
srch4_queue_var_type  *queue
)
{
  uint32 counter = 0;
  int64  lead_time = 0;
  srch4_task_struct_type *task = NULL;
  int64  temp_lead_time = 0;

  /*-----------------------------------------------------------------------*/

  for (counter = queue->num_tasks_scheduled; counter < queue->num_tasks; ++counter)
  {
    task = &queue->tasks[counter];
    temp_lead_time = srch4_calcuate_lead_time_to_search_scheduling(
    queue->srch_type,
    task->coherent_len,
    task->non_coherent_len,
    task->burst_len
    );

    if (temp_lead_time > lead_time)
    {
      lead_time = temp_lead_time;
    }
  }

  return lead_time;
}


#endif

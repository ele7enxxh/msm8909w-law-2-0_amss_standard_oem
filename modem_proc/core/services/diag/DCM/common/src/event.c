
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            Event Service

General Description
  This file containsthe implementation for the event reporting service.

  An event consists of a 12 bit event ID and a timestamp.

  The requirement is that the DMSS will never drop events as long as it does
  not exceed bandwidth limitations with event reporting.  Event reporting
  has the highest priority of all unsolicited diagnostic data.

  To minimize impact of the caller's task, the event is generated and placed
  in a queue.  The DIAG task will build the final packet structure and send
  to the external device.

  Just in case an event is dropped, a "drop event" will be reported and a
  count of dropped events will be reproted as payload.

Initialization and Sequencing Requirements
  Call event_init() before reporting any events.

Copyright (c) 2000-2014 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*==========================================================================

                              Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/event.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/18/15   sa      Added support for time sync feature.
07/20/15   ph      Fixed compiler warnings.
12/22/14   rs      Updated the event_rpt_last check in event_rpt_prep/event_rpt_last_sec to include 
                   the last event in event report packet.
10/06/14   ph      event_rpt_last() update is optimized in event_stale().
10/01/14   sa      Check for DCI Channel Status while draining events on DCI.
07/30/14   ph      Fixed the event queue issue where incorrect events are being freed.
07/17/14   ph      Fixed KW error in event_ssm_set_pre_masks().
07/06/14   ph      Fixed KW errors.
05/01/14   ph      EVENT_POWERUP now appears on boot up.
04/11/14   ph      Compiler warning fixes.
04/01/14   rs      Added support for build mask centralization. 
01/30/14   ph      Removed the usage of macro DIAG_TASK_HANDLE().
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
11/13/13   sr      Fixed the bug in event drain where we were sending partial 
                   pkt in case we are flow controlled in the middle of draining
10/14/13   sr      Added support for extended event, log and F3 pkts 
09/18/13   sa      Support for listeners masks to check for a registered listener
08/30/13   rh      Event stale timer now does not get set when length is 0.
07/18/13   ph      Updated diag_event_stale_timer_pending when DIAG_EVENT_TIMER_SIG is reset.
07/14/13   sa      Fixed issues in updating the event_config variable.				   
06/08/13   sa      Fixed Issues responding to V1 and V2 Diag Test Commands.
05/15/13   sr      Fixed Klockwork warnings 
05/02/13   ph      Fixed compilation errors faced after enabling FEATURE_DEBUG_DIAG_HEAP
04/09/13   sa      Reset the event stale timers when mode is changed
                   from real time to NRT and viceversa.
04/03/13   sr      Replace memcpy() and memmove() with memscpy() and memsmove() 
03/28/13   sr      Fixed Klockwork warnings
03/11/13   is      Resolve crash with diag_event_mask before event_init() is called
02/08/13   sr      Fixed compiler  and Klockwork warnings
01/09/13   is      Fix event indexing issue in response.
01/04/13   rh      If event timer is pending and there is an item to consume in event queue
                   then only set the stale timer
01/03/13   sg      Command Response Channel separation
12/07/12   is      Send event mask response from the correct event mask
12/05/12   sr      Freed the event buffer that was allocated to SSM 
11/16/12   ph      Klocwork warning fixes
11/15/12   is      Support for preset masks
09/28/12   sr      Filtering of logs/events based on ssm permission file  
08/22/12   sr      Support for mask retrieval command  
09/28/12   sr      Filtering of logs/events based on ssm permission file  
08/22/12   sr      Support for mask retrieval command  
08/31/12   ra      Use accessor macros instead of accessing private task         
                   info directly; accomodated changes for User space
                   task priority changes  
08/22/12   sr      Support for mask retrieval command  
08/16/12   rh      If event timer is pending and there is an item to consume in event queue
                   then only set the stale timer
05/03/12   sa      Mainlining DIAG_CENTRAL_ROUTING
03/07/12   rh      Fixed Klockwork errors 
11/07/11   is      Fixed compiler warnings
11/02/11   hm      Added support to enable stream_2 events through
                   diag_ctrl_update_event_mask()
10/26/11   hm      Extended Event/Log Listener APIs added  
10/07/11   is      Master Diag saves and sends master event mask to slaves
09/28/11   is      Support dynamic sleep voting and non-deferrable Diag timers
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
09/15/11   is      Modifed "set mask" via CTRL msgs fot EA support
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
08/30/11   hm      Events crash fixed - added static stats of drained events
07/19/11   hm      Non-aggreagation feature and do not service event listeners 
                   for event stream 2  
06/23/11   vs      event_report and event_report payload allow event to be
                    generated if part of event listeners.
12/21/10   hm      Dual mask changes  
12/20/10   mad     Changes to write events to first (main) diag stream
11/12/10   mad     Created event_notify_timer and associated DIAG_EVENT_DRAIN_SIG 
                   with timer firing
09/28/10   mad     run-time configurable event stale timer and event report size
08/10/10   sg      Added a function event_set_all_rt_masks() to turn off the
                   event masks on modem when port is closed
09/17/10   is      Migrate from deprecated clk driver APIs
08/25/10   sg      Fixed compiler warnings
07/20/10   sg      Added mutexes to osal_set_timer and osal_reset_timer to prevent
                   deletion of qtimer handle which is already deleted in case of 
                   drain timer ,stale timer which are set in client's context
07/08/10   vs      Changed event_mask to diag_event_mask
02/25/10   sg      Merging back DCM WM files
06/20/10   sg      check whether diag is initialized before allocating space from
                   event heap when clients call diag event apis
06/16/10   sg      Save trace for events whether we are able to allocate heap space
                   for events or not 
06/16/10   JV      If ints are locked when clients call to submit events or responses,
                   then start a timer, do not set a signal that could cause a context
                   switch. Just like in diag_tx_notify().
06/07/10   is      Removed FEATURE_ERR_EXTENDED_STORE.
06/07/10   JV      Bug fix in event_drop_handler()
04/21/10   JV      Use a separate heap of size 4K for events. Called 
                   diag_event_heap.
04/18/10   JV      Check diagcomm_status before draining events.
04/15/10   JV      Fix for bug where the event_q length was not getting
                   updated in all cases.
03/31/10   JV      Add mutexes for event_q.length and event_rpt_last
03/26/10   JV      Eliminating the event_pending timer and event_pending_q
03/23/10   sg      EVENT_STALE_TIMER  is set when the mask is zero to drain
                   the events that are already present in the diag_heap
03/17/10   JV      Check if flow is enabled before draining events.
02/23/10   JV      Save trace for events only if able to allocate.
02/12/10   vs      Ported diag_time_get from depot
02/02/10   JV      Do not poll for events at every packet boundary. Set a signal
                   for events to be drained.
01/27/10   ps      Decouple Error functions from diag.
12/28/09   ps      Fixed Klocwork errors & compiler warnings.
11/04/09   cahn    DIAG Health
09/22/09   vs      Use OSAL for all rex calls on qdsp6.
09/09/09   JV      Changed signature of osal mutex calls.
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
08/05/09   JV      task.h is now included in osal.
07/31/09   JV      Removed all the KxMutex calls
07/31/09   JV      Merged Q6 diag code back to mainline
05/12/09   JV      Introduced the OS abstraction layer for rex.
05/07/09   vk      changed includes from external to internal headers
04/02/09   vg      Protected shared resource event_pending_q_pending
02/17/09   as      Fixed event drop handler allocation failure.
02/01/09   vg      Made trace buffer mutex consistent between event's and F3 msg
01/08/09   as      Fixed deadlock and thread protection issues.
10/03/08   vg      Updated code to use PACK() vs. PACKED
01/02/07   as      Increased event report packet length to 512
12/22/06   as      Modified code to compare with number of events packaged.
12/21/06   as      Fixed packet length in event_rpt_prep.
12/15/06   as      Fixed crical lint errors.
11/21/06   as      Modified code to use diag internal features.
07/13/06   as      Added RPC support for mobileview
12/12/05   tbg     Added flag to help prevent recursive event trace saving.
10/31/05    as     Fixed lint errors.
08/15/05   eav     Added a check for buffer corruption
03/09/05    as     Event config is not needed to a allocate an event.
02/18/04    as     Fixed critical lint errors.
02/10/05    as     Added FEATURE_DEBUG_DIAG_HEAP to debug memory corruption.
12/06/04    as     Included dog.h to prevent compiler warnings
09/01/04   eav     Added FEATURE_SAVE_DEBUG_TRACE.
08/11/04    as     Fixed corner cases for event masks.
06/11/04    gr     Added support for event masks.
09/23/03    gr     The drop handler now detects the "out of space" condition
                   more efficiently.
05/05/03    as     Event timestamp fix
12/31/02   lad     Implementation rewrite.  Previous history deleted.
07/01/99   lad     Created file.

===========================================================================*/

#include "core_variation.h"
#ifndef FEATURE_WINCE
#endif
#include "comdef.h"

#include <string.h>
#include "crc.h"
#include "queue.h"
#include "qw.h"
#include "msg.h"
#include "err.h"
#include "diag_v.h"
#include "diagcmd.h"
#include "diagcomm_v.h"
#include "diagdiag_v.h"
#include "diagi_v.h"
#include "diagtune.h" /* for DIAG_MAX_RX_PKT_SIZ */
#include "event.h"
#include "diagtarget.h"
#include "diagcomm_ctrl.h" /* For diagcomm_ctrl_send_mult_pkt, diag_ctrl_msg_type */
#include "stringl.h"

#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "assert.h"
#ifndef FEATURE_WINCE
#include "dog.h"
#endif 
#endif

#include "osal.h"
#include "eventi.h"

#ifdef FEATURE_WINCE
#error code not present
#endif

#include "diagbuf_v.h" /* For diag_send_desc_type */
#include "diag_cfg.h"
#include <stdlib.h> 
/*-------------------------------------------------------------------------
  Definitions and Declarations
-------------------------------------------------------------------------*/
extern boolean diag_event_stale_timer_pending ;
extern boolean diag_event_stale_timer_pending_sec ;

extern uint8 diag_cur_preset_id; /* from diag.c */

/* For use in event_mask_set_internal_handler() */
#define DIAG_OP_SET_EVENT_STREAM 1
#define DIAG_OP_SET_EVENT_PRESET 2

#if defined(DIAG_MP_MASTER)
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */

/* Defines and variables for saving to master event mask */
#define MSTR_EVENT_MASK_SIZE 500
#define MSTR_EVENT_LAST_ID (MSTR_EVENT_MASK_SIZE * 8)

typedef struct
{
  uint8 status;
  uint16 numbits;
  uint32 mask_len;
  unsigned char mask[MSTR_EVENT_MASK_SIZE];
} diag_event_mask_type;

/* Master event masks */
diag_event_mask_type diag_event_mask_mstr_preset[DIAG_MAX_PRESET_ID];    /* Stream 1 QXDM Master masks. */
diag_event_mask_type diag_event_mask_mstr_dci[DIAG_MAX_DCI_ID];          /* Stream 2 DCI Master mask. */

/* Point Master masks to initial Master preset masks. */
diag_event_mask_type * diag_event_mask_mstr[DIAG_MAX_STREAM_ID] = { &diag_event_mask_mstr_preset[DIAG_PRESET_MASK_1-1],
                                                                    &diag_event_mask_mstr_dci[DIAG_DCI_MASK_1-1] };
#else
  /* MP Slave or Standalone */
  uint8 event_preset_status[DIAG_MAX_PRESET_ID];
#endif /* DIAG_MP_MASTER */

#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
  static diag_ctrl_info_fwd_status bt_event_id_fwd_status;
#endif

static unsigned int event_stale_timer_len = EVENT_TIMER_LEN; /* Set default value */
unsigned int event_report_pkt_size = EVENT_RPT_PKT_SIZE;
static unsigned int event_report_pkt_len_size = EVENT_RPT_PKT_LEN_SIZE;
static unsigned int event_send_max_num = EVENT_SEND_MAX;

/* The event masks. */
unsigned char diag_event_preset_mask[DIAG_MAX_PRESET_ID][EVENT_MASK_SIZE];    /* Stream 1 QXDM masks. */
unsigned char diag_event_dci_mask[DIAG_MAX_DCI_ID][EVENT_MASK_SIZE];          /* Stream 2 DCI mask. */
static uint32 event_curr_version = 0;

/* Point local masks to initial preset masks. */
unsigned char * diag_event_mask[DIAG_MAX_STREAM_ID] = { &diag_event_preset_mask[DIAG_PRESET_MASK_1-1][0],
                                                        &diag_event_dci_mask[DIAG_DCI_MASK_1-1][0] };
/* Listeners event mask */
unsigned char diag_event_listener_mask[EVENT_MASK_SIZE];
unsigned char diag_event_listener_ext_mask[EVENT_MASK_SIZE];
/*the event mask for ssm*/
#ifdef FEATURE_SSM
unsigned char diag_event_mask_ssm[EVENT_MASK_SIZE];
extern diag_mask_ssm_buffer_type *diag_event_mask_range;
extern osal_mutex_arg_t diag_event_mask_ssm_mutex;
extern uint32 num_event_code_ssm;
extern event_id_enum_type event_ids_pre_boot[];
void event_ssm_set_pre_masks(event_id_enum_type event_ids[]);
#endif

#define EVENT_MASK_BIT_SET(id) \
  (diag_event_mask[0][(id)/8] & (1 << ((id) & 0x07)))

#define EVENT_MASK_BIT_SET_SEC(id) \
  (diag_event_mask[1][(id)/8] & (1 << ((id) & 0x07)))

#ifdef FEATURE_SSM
/*Check if the corresponding bit in diag_event_mask_ssm for event code 'id' is set or not*/
#define DIAG_EVENT_MASK_SSM_SET(id) \
  (diag_event_mask_ssm[(id)/8] & (1 << ((id) & 0x07)))
#endif

/* Event reporting config indicator. */
#define EVENT_CONFIG_COMM_MASK 0x01

#define EVENT_CONFIG_VALID_MASK 0x03

#if defined (FEATURE_WINCE)
#error code not present
#else
uint8 event_config = 0;
#endif

uint8 event_config_sec = 0;
/* Stale timer.  If this timer expires, the current event buffer will
   be retired and a new one may be started with the next event report. */
static osal_timer_t event_stale_timer;
static osal_timer_t event_stale_timer_sec;

static osal_timer_t event_notify_timer;

static osal_timer_t event_notify_timer_sec;

static struct
{
  /* The length field indicates the approximate packet size of the event
     report packet.  It is calculated based on the assumption of truncated
     time stamps, since most are truncated (except the first). */
  unsigned int length;
  byte version; /* version of the extended event pkt; currently we support 0 and 1 */
  q_type q;
}
event_q;
static struct
{
  /* The length field indicates the approximate packet size of the event
     report packet.  It is calculated based on the assumption of truncated
     time stamps, since most are truncated (except the first). */
  unsigned int length;

  q_type q;
}
event_q_sec;


#define EVENT_Q_SIZE_ADJ(xx_ev) \
  sizeof (event_trunc_type) + FPOS (event_payload_type, payload) + \
  event->payload.length;

/* The queued event to be last in the report a packet. */
static event_store_type *event_rpt_last = NULL;
static event_store_type *event_rpt_last_sec = NULL;
/* Using the same criteria, a flush completion is detected by matching
   the event at the time the flush was issued within event_rpt_prep(). */
static event_store_type *event_flush_last = NULL;
static event_store_type *event_flush_last_sec = NULL;

#ifdef FEATURE_DEBUG_DIAG_HEAP
uint32 event_drop_handler_malloc_cnt=0;
uint32 event_drop_handler_free_cnt=0;
uint32 event_q_alloc_malloc_cnt=0;
uint32 event_drain_free_cnt = 0;
uint32 event_q_get_cnt =0;

uint32 event_drop_handler_malloc_cnt_sec=0;
uint32 event_drop_handler_free_cnt_sec=0;
uint32 event_q_alloc_malloc_cnt_sec=0;
uint32 event_drain_free_cnt_sec = 0;
uint32 event_q_get_cnt_sec =0;

uint32 get_event_q_cnt(void)
{
  return q_cnt (&event_q.q);
}
uint32 get_event_q_cnt_sec(void)
{
  return q_cnt (&event_q_sec.q);
}
extern uint32 diag_event_sec_heap_used_bytes(void);
extern uint32 diag_event_heap_used_bytes(void);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

/* Internal variable to store the event drop count */
static uint32 drop_count_event = 0;
static uint32 drop_count_event_sec = 0;
/* Internal variable to store the event allocation requests count */
static uint32 alloc_count_event = 0;
static uint32 alloc_count_event_sec = 0;

static event_store_type * event_q_get (void);
static event_store_type * event_q_get_sec (void);


#include <stdlib.h> 

static osal_mutex_arg_t         event_alloc_cs;
static osal_mutex_arg_t         event_q_len_cs;
static osal_mutex_arg_t         event_rpt_last_cs;
static osal_mutex_arg_t         event_alloc_sec_cs;
static osal_mutex_arg_t         event_q_len_sec_cs;
static osal_mutex_arg_t         event_rpt_last_sec_cs;

#ifndef FEATURE_WINCE
extern void event_service_listener (event_store_type *event_store);
#endif 
byte event_read_stream_id(event_id_enum_type id);

static inline void diag_set_event_stale_timer(void);
static inline void diag_set_event_stale_timer_sec(void);

extern boolean event_searchlist_ext(uint16 event_id, diag_event_type *event);
/* Time Get Function Pointer from diag.c */
extern uint8 (*diag_time_get)(qword time);

/*-------------------------------------------------------------------------
  Function Implementations
-------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION EVENT_CYCLE

DESCRIPTION
    This function starts an event draining cycle if one is not currently
    in progress by setting the timer to trigger a drain.  Additionally, if
    there are enough events in the queue that we want to drain immediately,
    it calls event_stale() to start that immediately instead of waiting for
    the timer to run out to call event_stale().
===========================================================================*/
static inline void
event_cycle (void)
{
  unsigned int length;

  osal_lock_mutex(&event_q_len_cs);
  length = event_q.length;
  osal_unlock_mutex(&event_q_len_cs);

  /* Compare estimated packet size to the target event report size. */
  if(length >= event_report_pkt_size) {
    /* This will set event_rpt_last and trigger a (relatively) immediate send. */
    event_stale();
  }
  else if((q_cnt(&event_q.q) != 0)) {
    diag_set_event_stale_timer();
  }
}

/*===========================================================================
FUNCTION EVENT_CYCLE_SEC

DESCRIPTION
    This function starts an event draining cycle if one is not currently
    in progress by setting the timer to trigger a drain.  Additionally, if
    there are enough events in the queue that we want to drain immediately,
    it calls event_stale_sec() to start that immediately instead of waiting
    for the timer to run out to call event_stale_sec().  It is analogous to
    event_cycle() but is for the secondary (DCI) event stream.
===========================================================================*/
static inline void
event_cycle_sec (void)
{
  unsigned int length;

  osal_lock_mutex(&event_q_len_sec_cs);
  length = event_q_sec.length;
  osal_unlock_mutex(&event_q_len_sec_cs);

  /* Compare estimated packet size to the target event report size. */
  if(length >= event_report_pkt_size) {
    /* This will set event_rpt_last and trigger a (relatively) immediate send. */
    event_stale_sec();
  }
  else if((q_cnt(&event_q_sec.q) != 0)) {
    diag_set_event_stale_timer_sec();
  }
}

/*=========================================================================*/
void
event_init (void)
{
  int mask_index, return_val = 0;

  /* Define (initialize) the stale timer for event reporting. */
  return_val = osal_create_diag_timer(&event_stale_timer, diag_task_tcb, DIAG_EVENT_TIMER_SIG);
  ASSERT(OSAL_SUCCESS == return_val);
  return_val = osal_create_diag_timer(&event_stale_timer_sec, diag_task_tcb, DIAG_EVENT_TIMER_SEC_SIG);
  ASSERT(OSAL_SUCCESS == return_val);
   
  /* Timer is used to notify diag that events need to be drained, if INTS are locked */
  return_val = osal_create_diag_timer(&event_notify_timer, diag_task_tcb, DIAG_EVENT_DRAIN_SIG);
  ASSERT(OSAL_SUCCESS == return_val);

   /* Timer is used to notify diag that events need to be drained, if INTS are locked */
   return_val = osal_create_diag_timer(&event_notify_timer_sec, diag_task_tcb, DIAG_EVENT_DRAIN_SEC_SIG);
   ASSERT(OSAL_SUCCESS == return_val);
   

  event_q.length = 0;
  (void) q_init (&event_q.q);

  event_q_sec.length = 0;
  (void) q_init (&event_q_sec.q);
  /* Set all the event bits for Stream 1 Preset 1 by default */
  for( mask_index = 0; mask_index < EVENT_MASK_SIZE; mask_index++ )
  {
    diag_event_preset_mask[DIAG_PRESET_MASK_1-1][mask_index] = 0xFF;
  }
#if defined(DIAG_MP_MASTER)
  for( mask_index = 0; mask_index < MSTR_EVENT_MASK_SIZE; mask_index++ )
  {
    diag_event_mask_mstr_preset[DIAG_PRESET_MASK_1-1].mask[mask_index] = 0xFF;
  }
  diag_event_mask_mstr_preset[DIAG_PRESET_MASK_1-1].numbits = EVENT_LAST_ID+1;
  diag_event_mask_mstr_preset[DIAG_PRESET_MASK_1-1].mask_len = EVENT_MASK_SIZE;
  diag_event_mask_mstr_preset[DIAG_PRESET_MASK_1-1].status = DIAG_CTRL_MASK_ALL_ENABLED;
#else
  event_preset_status[DIAG_PRESET_MASK_1-1] = DIAG_CTRL_MASK_ALL_ENABLED;
#endif
  
  /* Set the bits for the other streams and presets to off by default. */
  for( mask_index = 0; mask_index < EVENT_MASK_SIZE; mask_index++ )
  {
    diag_event_preset_mask[DIAG_PRESET_MASK_2-1][mask_index] = 0;
    diag_event_dci_mask[DIAG_DCI_MASK_1-1][mask_index] = 0;
  }


#if defined(DIAG_MP_MASTER)
  for( mask_index = 0; mask_index < MSTR_EVENT_MASK_SIZE; mask_index++ )
  {
    diag_event_mask_mstr_preset[DIAG_PRESET_MASK_2-1].mask[mask_index] = 0;
    diag_event_mask_mstr_dci[DIAG_DCI_MASK_1-1].mask[mask_index] = 0;
  }
  diag_event_mask_mstr_preset[DIAG_PRESET_MASK_2-1].numbits = EVENT_LAST_ID+1;
  diag_event_mask_mstr_preset[DIAG_PRESET_MASK_2-1].mask_len = EVENT_MASK_SIZE;
  diag_event_mask_mstr_preset[DIAG_PRESET_MASK_2-1].status = DIAG_CTRL_MASK_ALL_DISABLED;
  diag_event_mask_mstr_dci[DIAG_DCI_MASK_1-1].numbits = EVENT_LAST_ID+1;
  diag_event_mask_mstr_dci[DIAG_DCI_MASK_1-1].mask_len = EVENT_MASK_SIZE;
  diag_event_mask_mstr_dci[DIAG_DCI_MASK_1-1].status = DIAG_CTRL_MASK_ALL_DISABLED;
#else
  event_preset_status[DIAG_PRESET_MASK_2-1] = DIAG_CTRL_MASK_ALL_DISABLED;
#endif

#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
  bt_event_id_fwd_status = DIAG_CTRL_INFO_FWD_STATUS_NOT_SENT;
#endif

   #ifdef FEATURE_SSM
   for (mask_index = 0; mask_index < EVENT_MASK_SIZE; mask_index++)
    {
      #ifdef DIAG_SSM_LOG_EVENT_WHITELIST
      diag_event_mask_ssm[mask_index] = 0x0;
      #else
      diag_event_mask_ssm[mask_index] = 0xff;
      #endif
    }
    /*set the SSM mask for the events to be seen before boot up */
    event_ssm_set_pre_masks(event_ids_pre_boot);
  #endif

     /* Initialize the event listeners masks to 0*/
  memset(diag_event_listener_mask, 0, sizeof(diag_event_listener_mask));
  memset(diag_event_listener_ext_mask, 0 ,sizeof(diag_event_listener_ext_mask));
    /* Initialize mutex */
  /* mutex names */
  event_alloc_cs.name = "MUTEX_EVENT_CS";
  return_val = osal_init_mutex(&event_alloc_cs);
  ASSERT(OSAL_SUCCESS == return_val);

  event_q_len_cs.name = "MUTEX_EVENT_Q_LEN_CS";
  return_val = osal_init_mutex(&event_q_len_cs);
  ASSERT(OSAL_SUCCESS == return_val);

  event_rpt_last_cs.name = "MUTEX_EVENT_RPT_LAST_CS";
  return_val = osal_init_mutex(&event_rpt_last_cs);
  ASSERT(OSAL_SUCCESS == return_val);

  event_alloc_sec_cs.name = "MUTEX_EVENT_SEC_CS";
  return_val = osal_init_mutex(&event_alloc_sec_cs);
  ASSERT(OSAL_SUCCESS == return_val);

  event_q_len_sec_cs.name = "MUTEX_EVENT_Q_LEN_SEC_CS";
  return_val = osal_init_mutex(&event_q_len_sec_cs);
  ASSERT(OSAL_SUCCESS == return_val);

  event_rpt_last_sec_cs.name = "MUTEX_EVENT_RPT_LAST_SEC_CS";
  return_val = osal_init_mutex(&event_rpt_last_sec_cs);
  ASSERT(OSAL_SUCCESS == return_val);

  diag_sleep_vote (DIAG_SLEEP_EVENT, TRUE);
}                               /* event_init */

/*===========================================================================
FUNCTION EVENT_RESET_DROP_COUNT_EVENT

DESCRIPTION
    This function resets the event packet drop count and the event allocation
    requests count.
===========================================================================*/
void event_reset_drop_count_event( void ) {
    drop_count_event = 0;
    alloc_count_event = 0;
}
/*===========================================================================
FUNCTION EVENT_GET_DROP_COUNT_EVENT

DESCRIPTION
    This function returns the event packet drop count.
===========================================================================*/
uint32 event_get_drop_count_event( void ) {
    return drop_count_event;
}
/*===========================================================================
FUNCTION EVENT_GET_ALLOC_COUNT_EVENT

DESCRIPTION
    This function returns the event packet allocation requests count.
===========================================================================*/
uint32 event_get_alloc_count_event( void ) {
    return alloc_count_event;
}
void event_reset_drop_count_event_sec( void ) {

    drop_count_event_sec = 0;
    alloc_count_event_sec = 0;
}
/*===========================================================================
FUNCTION EVENT_GET_DROP_COUNT_EVENT

DESCRIPTION
    This function returns the event packet drop count.
===========================================================================*/
uint32 event_get_drop_count_event_sec( void ) {
    return drop_count_event_sec;
}
/*===========================================================================
FUNCTION EVENT_GET_ALLOC_COUNT_EVENT

DESCRIPTION
    This function returns the event packet allocation requests count.
===========================================================================*/
uint32 event_get_alloc_count_event_sec( void ) {
    return alloc_count_event_sec;
}
/*===========================================================================

FUNCTION EVENT_DROP_HANDLER

DESCRIPTION
  This routine handles the case where events are dropping and the service
  is attempting to continue operation.  It attempts to allocate both a
  "dropped event event" and the requested event.  If both are not successful,
  NULL is returned, indicating the service continues to drop events,
    
DEPENDENCIES
  This must be called from INTLOCK() (within critical section).

RETURN VALUE
  A pointer to the allocated event Q item is returned.  
  NULL is returned if unsucessful.

===========================================================================*/
static event_store_type * 
event_drop_handler (uint32 drop_cnt, qword drop_ts, uint8 payload_length)
{
  event_store_type *event = NULL;
  event_store_type *drop_event = NULL;
  uint32 event_sizes[2];

  /* Do not try to malloc memory if there is not enough space for both the
  ** allocations we do here. This is an efficiency improvement that
  ** prevents iterations through the heap to allocate memory for one
  ** event only to free it immediately because there is no memory for
  ** the second one.
  */
  event_sizes[0] = FPOS (event_store_type, payload.payload) + 
                   FSIZ (event_drop_type, drop_cnt);
  event_sizes[1] = FPOS (event_store_type, payload.payload) + payload_length;
  if(!diag_event_alloc_check (2, event_sizes))
    return NULL;

  /* Since the event service is currently dropping, there must be space 
     for both the "dropped event" event and the next event. */
  drop_event = (event_store_type *) diag_event_malloc (
                                                      FPOS (event_store_type, payload.payload) + 
                                                      FSIZ (event_drop_type, drop_cnt));

  if(drop_event)
  {
#ifdef FEATURE_DEBUG_DIAG_HEAP
    event_drop_handler_malloc_cnt++;
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "MALLOC event_drop_handler cnt = %d, heap used = %d  drop_event",
              event_drop_handler_malloc_cnt,diag_event_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    event =
    (event_store_type *) diag_event_malloc (
                                           FPOS (event_store_type, payload.payload) + payload_length);

    if(event)
    {
      /* Event allocation successful.  This means dropping is over.  Queue the
        "dropped event" event and return the caller's event q buffer. */

#ifdef FEATURE_DEBUG_DIAG_HEAP
      event_drop_handler_malloc_cnt++;
      MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "MALLOC event_drop_handler cnt = %d, heapused = %d drop_event = %d",
                event_drop_handler_malloc_cnt, diag_event_heap_used_bytes(),EVENT_DROP_ID);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

      /* Unless both events can be queued, the dropped event will be freed. Therefore, we should only
         increment the alloc_count_event when both events have been successfully queued.
         This function is called within a critical section and thus an additional lock is not required. */
      if( alloc_count_event == MAX_VALUE_UINT32 ) {
        MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable alloc_count_event has reached maximum value.");
      }
      else {
        alloc_count_event+=2;
      }

      drop_event->event_id.id = EVENT_DROP_ID;

      qw_equ (drop_event->ts, drop_ts);

      drop_event->payload.length = FSIZ (event_drop_type, drop_cnt);

      memscpy((uint8*)(&drop_event->payload.payload), drop_event->payload.length, (uint8*)&drop_cnt, sizeof(drop_cnt));

      event_q_put(drop_event,DIAG_STREAM_1);

      drop_event = NULL;
    }
    else
    {
      diag_event_free (drop_event);

#ifdef FEATURE_DEBUG_DIAG_HEAP
      event_drop_handler_free_cnt++;
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "FREE event_drop_handler_free_cnt = %d, diag heap used = %d ",
            event_drop_handler_free_cnt,diag_event_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */
      drop_event = NULL;
    }
  }

  return event;
}
static event_store_type * 
event_drop_handler_sec (uint32 drop_cnt, qword drop_ts, uint8 payload_length)
{
  event_store_type *event = NULL;
  event_store_type *drop_event = NULL;
  uint32 event_sizes[2];

  /* Do not try to malloc memory if there is not enough space for both the
  ** allocations we do here. This is an efficiency improvement that
  ** prevents iterations through the heap to allocate memory for one
  ** event only to free it immediately because there is no memory for
  ** the second one.
  */
  event_sizes[0] = FPOS (event_store_type, payload.payload) + 
                   FSIZ (event_drop_type, drop_cnt);
  event_sizes[1] = FPOS (event_store_type, payload.payload) + payload_length;
  if(!diag_event_alloc_check_sec (2, event_sizes))
    return NULL;

  /* Since the event service is currently dropping, there must be space 
     for both the "dropped event" event and the next event. */
  drop_event = (event_store_type *) diag_event_malloc_sec (
                                                          FPOS (event_store_type, payload.payload) + 
                                                          FSIZ (event_drop_type, drop_cnt));

  if(drop_event)
  {
#ifdef FEATURE_DEBUG_DIAG_HEAP
    event_drop_handler_malloc_cnt_sec++;
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "MALLOC event_drop_handler cnt = %d, heap used = %d  drop_event",
            event_drop_handler_malloc_cnt_sec,diag_event_sec_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    event =
           (event_store_type *) diag_event_malloc_sec (
               FPOS (event_store_type, payload.payload) + payload_length);

    if(event)
    {
      /* Event allocation successful.  This means dropping is over.  Queue the
        "dropped event" event and return the caller's event q buffer. */

#ifdef FEATURE_DEBUG_DIAG_HEAP
      event_drop_handler_malloc_cnt_sec++;
      MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "MALLOC event_drop_handler cnt = %d, heapused = %d drop_event = %d",
                event_drop_handler_malloc_cnt_sec, diag_event_sec_heap_used_bytes(),EVENT_DROP_ID);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

      /* Unless both events can be queued, the dropped event will be freed. Therefore, we should only
         increment the alloc_count_event when both events have been successfully queued.
         This function is called within a critical section and thus an additional lock is not required. */
      if( alloc_count_event == MAX_VALUE_UINT32 ) {
        MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable alloc_count_event has reached maximum value.");
      }
      else {
        alloc_count_event+=2;
      }

      drop_event->event_id.id = EVENT_DROP_ID;

      qw_equ (drop_event->ts, drop_ts);

      drop_event->payload.length = FSIZ (event_drop_type, drop_cnt);

      memscpy((uint8*)(&drop_event->payload.payload), drop_event->payload.length, 
              (uint8*)&drop_cnt, sizeof(drop_cnt));

      event_q_put(drop_event,DIAG_STREAM_2);

      drop_event = NULL;
    }
    else
    {
      diag_event_free_sec (drop_event);

#ifdef FEATURE_DEBUG_DIAG_HEAP
      event_drop_handler_free_cnt_sec++;
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "FREE event_drop_handler_free_cnt = %d, diag heap used = %d ",
            event_drop_handler_free_cnt_sec,diag_event_sec_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */
      drop_event = NULL;
    }
  }

  return event;
}
byte event_read_stream_id(event_id_enum_type id)
{
 byte stream_id = 0;
 
 if(id <=EVENT_LAST_ID)
 {
     if ( EVENT_MASK_BIT_SET (id))
     stream_id = stream_id | DIAG_STREAM_1;
     if (EVENT_MASK_BIT_SET_SEC (id))
         stream_id = stream_id | DIAG_STREAM_2;
 }
  return stream_id;
}
/*==========================================================================

FUNCTION EVENT_Q_ALLOC

DESCRIPTION
  This routine allocates an event Q item from the DIAG heap and fills in 
  the following information common to all events:
  
  Event ID
  Time stamp
  Payload length field
  
  This routine also detects dropped events and handles the reporting of
  dropped events.
    
RETURN VALUE
  A pointer to the allocated  event from the Q is returned.  
  NULL is the Q is empty.

===========================================================================*/

extern diag_searchlist_type event_listeners;
/* externed from event_api.c */

event_store_type * event_q_alloc (event_id_enum_type id, uint8 payload_length,byte stream_id)
{  
  event_store_type *event = NULL;
  static uint32 drop_cnt = 0;
  static uint32 drop_cnt_sec = 0;
  static qword drop_ts,drop_ts_sec;

  if(!diag_services_initialized)
    return NULL;

  /* Verify that the event id is in the right range and that the
  ** corresponding bit is turned on in the event mask.
  ** and check if any listeners is listening on the event
  */

  if((id > EVENT_LAST_ID))
    return NULL;

  else
  {
    #ifdef FEATURE_SSM
    if(!DIAG_EVENT_MASK_SSM_SET(id))
    {
      return NULL;
    }
    #endif
    if( (stream_id & (DIAG_STREAM_1 | DIAG_STREAM_LISTENER)) )
    {

      /* Try to allocate an event if event mask is set or its part of listener structure */
      osal_lock_mutex(&event_alloc_cs);
      if(drop_cnt > 0)
      {
        event = event_drop_handler (drop_cnt, drop_ts, payload_length);

        if(event)
        {
          drop_cnt = 0;
        }
      }
      else
      {
        event = (event_store_type *) diag_event_malloc (
                    FPOS (event_store_type, payload.payload) + payload_length);

        if( event ) {
          /* There were no dropped events and we were able to successfully
          * allocate an event in diag_event_malloc, so increment the alloc_count_event. */
          if( alloc_count_event == MAX_VALUE_UINT32 ) {
            MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable alloc_count_event has reached maximum value.");
          }
          else {
            alloc_count_event++;
          }
        }
      }
      osal_unlock_mutex(&event_alloc_cs);

      if(event)
      {
#ifdef FEATURE_DEBUG_DIAG_HEAP
        event_q_alloc_malloc_cnt++;
        MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "MALLOC event_q_cnt = %d, diag heap used = %d event_id = %d",
                  event_q_alloc_malloc_cnt,diag_event_heap_used_bytes(),id);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

        diag_time_get (event->ts);

        event->event_id.id = id;
        event->payload.length = payload_length;
      }
      else
      {
        /* Drop the event */
        osal_lock_mutex(&event_alloc_cs);

        if(drop_cnt == 0)
        {
          diag_time_get (drop_ts);
        }
        drop_cnt++;

        if( drop_count_event == MAX_VALUE_UINT32 ) {
          MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable drop_count_event has reached maximum value.");
        }
        else {
          drop_count_event++;
        }

        osal_unlock_mutex(&event_alloc_cs);
      }
    } /* END DIAG_STREAM_1 || status */
    else if(stream_id & DIAG_STREAM_2)
    {
      /* Try to allocate an event if event mask is set */
      osal_lock_mutex(&event_alloc_sec_cs);
      if(drop_cnt_sec > 0)
      {
        event = event_drop_handler_sec (drop_cnt_sec, drop_ts_sec, payload_length);

        if(event)
        {
          drop_cnt_sec = 0;
        }
      }
      else
      {
        event = (event_store_type *) diag_event_malloc_sec(
                                                          FPOS (event_store_type, payload.payload) + payload_length);

        if( event ) {
          /* There were no dropped events and we were able to successfully
           * allocate an event in diag_event_malloc, so increment the alloc_count_event. */
          if( alloc_count_event_sec == MAX_VALUE_UINT32 ) {
            MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable alloc_count_event has reached maximum value.");
          }
          else {
            alloc_count_event_sec++;
          }
        }
      }
      osal_unlock_mutex(&event_alloc_sec_cs);

      if(event)
      {
#ifdef FEATURE_DEBUG_DIAG_HEAP
        event_q_alloc_malloc_cnt_sec++;
        MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "MALLOC event_q_cnt = %d, diag heap used = %d event_id = %d",
                  event_q_alloc_malloc_cnt_sec,diag_event_sec_heap_used_bytes(),id);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

        diag_time_get (event->ts);

        event->event_id.id = id;
        event->payload.length = payload_length;
      }
      else
      {
        /* Drop the event */
        osal_lock_mutex(&event_alloc_sec_cs);

        if(drop_cnt_sec == 0)
        {
          diag_time_get (drop_ts_sec);
        }
        drop_cnt_sec++;

        if( drop_count_event_sec == MAX_VALUE_UINT32 ) {
          MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable drop_count_event has reached maximum value.");
        }
        else {
          drop_count_event_sec++;
        }

        osal_unlock_mutex(&event_alloc_sec_cs);
      }
    } /* END DIAG_STREAM_2 */
  }

  return event;

} /*  event_q_alloc */


/*===========================================================================

FUNCTION EVENT_Q_PUT

DESCRIPTION
  This routine places the given event item in the event Q.  It also 
  maintains the event_q.length field.  If a set threshold is crossed,
  the stale timer is set to expire quickly so the queued events will be
  marked to be sent in the next packet.
  
===========================================================================*/
void
event_q_put (event_store_type * event,byte stream_id)

{  
  /* This queue is currently used for both listeners and STREAM_1 traffic */
  /* Need to check if either bit is set in the stream mask */
  if(stream_id & (DIAG_STREAM_1 | DIAG_STREAM_LISTENER))
  {
    if(event)
    {
      q_put (&event_q.q, q_link (event, &event->qlink));

#ifdef FEATURE_DEBUG_DIAG_HEAP
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "Q_PUT event_q_put event_q_cnt = %d diag heap used = %d",
            q_cnt (&event_q.q),  diag_event_heap_used_bytes()); 
#endif /* FEATURE_DEBUG_DIAG_HEAP */

      /* Increment the estimated data usage in the event q, assuming
         truncated time stamps. */
      osal_lock_mutex(&event_q_len_cs);

      event_q.length += EVENT_Q_SIZE_ADJ (event);

      osal_unlock_mutex(&event_q_len_cs);

      /* Start events cycling through the system */
      event_cycle();
    }
  }
  if(stream_id == DIAG_STREAM_2)
  {
    if(event)
    {

      q_put (&event_q_sec.q, q_link (event, &event->qlink));

#ifdef FEATURE_DEBUG_DIAG_HEAP
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "Q_PUT event_q_put event_q_cnt = %d diag heap used = %d",
             q_cnt (&event_q_sec.q),  diag_event_sec_heap_used_bytes()); 
#endif /* FEATURE_DEBUG_DIAG_HEAP */

      /* Increment the estimated data usage in the event q, assuming
         truncated time stamps. */
      osal_lock_mutex(&event_q_len_sec_cs);

      event_q_sec.length += EVENT_Q_SIZE_ADJ (event);

      osal_unlock_mutex(&event_q_len_sec_cs);

      /* Trigger event reporting cycle. */
      event_cycle_sec();
    }
  }
  return;
}

/*===========================================================================

FUNCTION EVENT_Q_GET

DESCRIPTION
  This routine retrieves an event stored in the event Q.  It also maintains
  the event_q.length field.  This field is is used as a trigger in 
  event_q_put() to detect when the queue starts to get full.
  
RETURN VALUE
  A pointer to a event from the Q is returned.  
  NULL is the Q is empty.

===========================================================================*/
static event_store_type *
event_q_get (void)
{
  event_store_type *event;

  event = (event_store_type *) q_get (&event_q.q);

  if(event)
  {
#ifdef FEATURE_DEBUG_DIAG_HEAP
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "Q_GET event_q_get event_q_cnt = %d diag heap used = %d ",
             q_cnt (&event_q.q),  diag_event_heap_used_bytes()); 
#endif /* FEATURE_DEBUG_DIAG_HEAP */
    osal_lock_mutex(&event_q_len_cs);
    event_q.length -= EVENT_Q_SIZE_ADJ (event);
    osal_unlock_mutex(&event_q_len_cs);
  }

  return event;
}
static event_store_type *
event_q_get_sec (void)
{
  event_store_type *event;

  event = (event_store_type *) q_get (&event_q_sec.q);

  if(event)
  {
#ifdef FEATURE_DEBUG_DIAG_HEAP
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "Q_GET event_q_get event_q_cnt = %d diag heap used = %d ",
              q_cnt (&event_q_sec.q),  diag_event_sec_heap_used_bytes()); 
#endif /* FEATURE_DEBUG_DIAG_HEAP */
    osal_lock_mutex(&event_q_len_sec_cs);
    event_q_sec.length -= EVENT_Q_SIZE_ADJ (event);
    osal_unlock_mutex(&event_q_len_sec_cs);
  }

  return event;
}


/*===========================================================================

FUNCTION EVENT_RPT_PREP

DESCRIPTION
  This routine performs preparation work on the items in the event Q prior
  to being sent.  This is necessary since the packet format requries a 
  length field at the beginning, and the packet is sent piece-wise.
  Therefore, the length must be calculated prior to sending the header 
  of the packet (header contains the length field).

  If listeners are enabled, each event is serviced by the listener system.
  
  Since traversal must occur, this routine also detects time truncation
  the event ID field for the final packet format.

DEPENDENCIES
  This routine must be called by the DIAG task at a TX packet boundary.
  
  This routine expects 'event_rpt_last' to be set, indicating a group
  of messages that are ready to be sent.

RETURN VALUE
  The length of the final event packet payload.  This value is used in the
  event packet header.
  
  0 is returned if not successful.

===========================================================================*/
uint32 event_pkg_count = 0;
static uint16
event_rpt_prep (void)
{
  event_store_type *event = NULL;
  unsigned int pkt_len = 0;     /* Length of 'events' member of event_rpt_type */
  uint32 time_context_hi = 0xFFFFFFFFU;  /* time context used for truncation */
  uint32 time_context_hi_new = 0xFFFFFFFFU;      /* time context used for truncation */
  uint16 time_context_lo = 0xFFFFU; /* time context used for truncation */
  uint16 time_context_lo_new = 0xFFFFU;     /* time context used for truncation */
  uint8 payload_length = 0;     /* Working var to optimize mem access */
  boolean flush_complete = FALSE, last_event = FALSE; /*last_event is set to TRUE when one event to be sent in report */
  uint8 num_events = 0;

  event_pkg_count = 0;

  if(event_rpt_last)
  {
    /* To initialize, traverse the queue to determine truncated time
       stamps and packet length.  This traversal is unavoidable given
       the packet design. */

    /* First, look at the item at the head of the queue.  Since 
       'event_rpt_last' is set, it is assumed the Q has at least 1 item */
    event = q_check (&event_q.q);

    /* If flush pending, the tool mask flag must be cleared after flush completes */ 
    if(event_flush_last && event_flush_last == event)
    {
      flush_complete = TRUE;
    }

    if(event)
    {
      event_curr_version = event->version;
#ifndef FEATURE_WINCE
      event_service_listener (event);
      if(event_mask_enabled(diag_event_listener_ext_mask, event->event_id.id))
      {
      (void)event_searchlist_ext (event->event_id.id, (diag_event_type *)&event->event_id.id);
      }
#endif 
        num_events++;

        event->event_id.event_id_field.id = event->event_id.id;

        payload_length = event->payload.length;

        /* The first event in the packet always contains a full time stamp. */
        pkt_len = sizeof (event_type) + payload_length;

        event->event_id.event_id_field.reserved = 0;

        if(payload_length > EVENT_PAY_TWO_BYTE)
        {
          event->event_id.event_id_field.payload_len = EVENT_PAY_LENGTH;
          /* Adjust payload length to include length field */
          pkt_len += FPOS (event_payload_type, payload);
        }
        else
        {
          event->event_id.event_id_field.payload_len = payload_length;
        }

        event->event_id.event_id_field.time_trunc_flag = FALSE;

        time_context_hi = qw_hi (event->ts);
        time_context_lo = ((uint16) (qw_lo (event->ts)>>(8*2))& 0xFFFF);     /* Get MSBytes of lower TS */
        if(event != event_rpt_last)
        {
          event = q_next (&event_q.q, &event->qlink);
          if(event)
          {
            if(event->version != event_curr_version)
            {
              return pkt_len;
            }
          }
        }
        else
        {
            last_event = TRUE;
        }
    }
    /* Calculate length for this and all other events in the queue until the
       packet threshold is reached. */
    /* Bail of the while loop if number of events is EVENT_SEND_MAX or if
       pkt length has crossed EVENT_RPT_PKT_LEN_SIZE or if the version ID of the next
       pkt is different*/
    while((event) && (last_event != TRUE) &&
          (num_events < event_send_max_num && pkt_len < event_report_pkt_len_size))
    {
      if(event->version != event_curr_version)
      {
        return pkt_len;
      }

      if(event)
      {
        /* If flush pending, the tool mask flag must be cleared after flush completes */ 
        if(event_flush_last && event_flush_last == event)
        {
          flush_complete = TRUE;
        }

#ifndef FEATURE_WINCE
        event_service_listener (event);
      if(event_mask_enabled(diag_event_listener_ext_mask, event->event_id.id))
      {
        (void)event_searchlist_ext (event->event_id.id, (diag_event_type *)&event->event_id.id);
      }
#endif 
          num_events++;
          event->event_id.event_id_field.id = event->event_id.id;

          event->event_id.event_id_field.reserved = 0;

          payload_length = event->payload.length;

          pkt_len += payload_length;

          if(payload_length > EVENT_PAY_TWO_BYTE)
          {
            event->event_id.event_id_field.payload_len = EVENT_PAY_LENGTH;
            /* Adjust payload length to include length field */
            pkt_len += FPOS (event_payload_type, payload);
          }
          else{
            event->event_id.event_id_field.payload_len = payload_length;
          }

          /* Check time stamp relative to the last event in the queue.  Truncate
             if the context (upper 5 bytes) are the same. */
          time_context_hi_new = qw_hi (event->ts);
          time_context_lo_new = ((uint16) (qw_lo (event->ts)>>(8*2))& 0xFFFF);     /* Get MSBytes of lower TS */

          if(time_context_hi == time_context_hi_new &&
             time_context_lo == time_context_lo_new)
          {
            event->event_id.event_id_field.time_trunc_flag = TRUE;

            /* Adjust pkt_len for a truncated time stamp */
            pkt_len += sizeof (event_trunc_type);
          }
          else
          {
            event->event_id.event_id_field.time_trunc_flag = FALSE;

            time_context_hi = time_context_hi_new;
            time_context_lo = time_context_lo_new;

            /* Adjust pkt_len for a full time stamp */
            pkt_len += sizeof (event_type);
          }

          if (event == event_rpt_last)
              break;

          event = q_next (&event_q.q, &event->qlink);
        }
      else
      {
        /* 'event_rpt_last' should be found before the queue is empty. */
        MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"Unexpected: event_rpt_last not in Q!");

        osal_lock_mutex(&event_rpt_last_cs);

        event_rpt_last = NULL;  /* Start over upon the next event_q_put() */

        osal_unlock_mutex(&event_rpt_last_cs);

        pkt_len = 0;
      }
    }
  }

  /* If a flush (to the external device) is complete, clear the config */
  if(flush_complete)
  {
    event_flush_last = NULL;
    event_control (0, EVENT_CONFIG_COMM_MASK);
  }

  event_pkg_count = num_events;
  return(uint16) pkt_len;

}
uint32 event_pkg_count_sec = 0;
static uint16
event_rpt_prep_sec (void)
{
  event_store_type *event = NULL;
  unsigned int pkt_len = 0;     /* Length of 'events' member of event_rpt_type */
  uint32 time_context_hi = 0xFFFFFFFFU;  /* time context used for truncation */
  uint32 time_context_hi_new = 0xFFFFFFFFU;      /* time context used for truncation */
  uint16 time_context_lo = 0xFFFFU; /* time context used for truncation */
  uint16 time_context_lo_new = 0xFFFFU;     /* time context used for truncation */
  uint8 payload_length = 0;     /* Working var to optimize mem access */
  boolean flush_complete = FALSE, last_event = FALSE;
  uint8 num_events = 0;

  event_pkg_count_sec = 0;

  if(event_rpt_last_sec)
  {
    /* To initialize, traverse the queue to determine truncated time
       stamps and packet length.  This traversal is unavoidable given
       the packet design. */

    /* First, look at the item at the head of the queue.  Since 
       'event_rpt_last' is set, it is assumed the Q has at least 1 item */
    event = q_check (&event_q_sec.q);

    /* If flush pending, the tool mask flag must be cleared after flush completes */ 
    if(event_flush_last_sec && event_flush_last_sec == event)
    {
      flush_complete = TRUE;
    }

    if(event)
    {

#ifndef FEATURE_WINCE
      /* ToDo: Fix event listeners for dual-mask. Listener callback should only be
         called once per event instance. */

      // event_service_listener (event);
#endif 
        num_events++;

        event->event_id.event_id_field.id = event->event_id.id;

        payload_length = event->payload.length;

        /* The first event in the packet always contains a full time stamp. */
        pkt_len = sizeof (event_type) + payload_length;

        event->event_id.event_id_field.reserved = 0;

        if(payload_length > EVENT_PAY_TWO_BYTE)
        {
          event->event_id.event_id_field.payload_len = EVENT_PAY_LENGTH;
          /* Adjust payload length to include length field */
          pkt_len += FPOS (event_payload_type, payload);
        }
        else{
          event->event_id.event_id_field.payload_len = payload_length;
        }

        event->event_id.event_id_field.time_trunc_flag = FALSE;

        time_context_hi = qw_hi (event->ts);
        time_context_lo = ((uint16) (qw_lo (event->ts)>>(8*2))& 0xFFFF);     /* Get MSBytes of lower TS */
        if(event != event_rpt_last_sec)
        {
          event = q_next (&event_q_sec.q, &event->qlink);
        }
        else
        {
          last_event = TRUE;
        }
      }
   
    /* Calculate length for this and all other events in the queue until the
       packet threshold is reached. */
    /* Bail of the while loop if number of events is EVENT_SEND_MAX or if
       pkt length has crossed EVENT_RPT_PKT_LEN_SIZE */
    while((event) && (last_event != TRUE) &&
          (num_events < event_send_max_num && pkt_len < event_report_pkt_len_size))
    {

      if(event)
      {
        /* If flush pending, the tool mask flag must be cleared after flush completes */ 
        if(event_flush_last_sec && event_flush_last_sec == event)
        {
          flush_complete = TRUE;
        }

#ifndef FEATURE_WINCE
        /* ToDo: Fix event listeners for dual-mask. Listener callback should only be
           called once per event instance. */

        //   event_service_listener (event);
#endif 
          num_events++;
          event->event_id.event_id_field.id = event->event_id.id;

          event->event_id.event_id_field.reserved = 0;

          payload_length = event->payload.length;

          pkt_len += payload_length;

          if(payload_length > EVENT_PAY_TWO_BYTE)
          {
            event->event_id.event_id_field.payload_len = EVENT_PAY_LENGTH;
            /* Adjust payload length to include length field */
            pkt_len += FPOS (event_payload_type, payload);
          }
          else{
            event->event_id.event_id_field.payload_len = payload_length;
          }

          /* Check time stamp relative to the last event in the queue.  Truncate
             if the context (upper 5 bytes) are the same. */
          time_context_hi_new = qw_hi (event->ts);
          time_context_lo_new = ((uint16) (qw_lo (event->ts)>>(8*2))& 0xFFFF);     /* Get MSBytes of lower TS */

          if(time_context_hi == time_context_hi_new &&
             time_context_lo == time_context_lo_new)
          {
            event->event_id.event_id_field.time_trunc_flag = TRUE;

            /* Adjust pkt_len for a truncated time stamp */
            pkt_len += sizeof (event_trunc_type);
          }
          else
          {
            event->event_id.event_id_field.time_trunc_flag = FALSE;

            time_context_hi = time_context_hi_new;
            time_context_lo = time_context_lo_new;

            /* Adjust pkt_len for a full time stamp */
            pkt_len += sizeof (event_type);
          }

          if (event == event_rpt_last_sec)
              break;

          event = q_next (&event_q_sec.q, &event->qlink);
        }
      else
      {
        /* 'event_rpt_last' should be found before the queue is empty. */
        MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"Unexpected: event_rpt_last not in Q!");

        osal_lock_mutex(&event_rpt_last_sec_cs);

        event_rpt_last_sec = NULL;  /* Start over upon the next event_q_put() */

        osal_unlock_mutex(&event_rpt_last_sec_cs);

        pkt_len = 0;
      }
    }
  }

  /* If a flush (to the external device) is complete, clear the config */
  if(flush_complete)
  {
    event_flush_last_sec = NULL;
    event_control_sec (0, EVENT_CONFIG_COMM_MASK);
  }

  event_pkg_count_sec = num_events;
  return(uint16) pkt_len;

}

/*===========================================================================

FUNCTION EVENT_DRAIN

DESCRIPTION
  This routine builds and sends the event report packet from the event queue.
  Time truncation, CRC calculation,

  This routine periodically checks the DIAG task signals against 'mask'.
  If 'mask' is found in DIAG's task signals, the routine will exit.

DEPENDENCIES
  This routine must be called by the DIAG task at a TX packet boundary.
  If this routine returns FALSE, it must be called again prior to any other
  data being sent to the comm layer.

RETURN VALUE
  TRUE if complete (no more events to send),
  FALSE if in progress (more to send) - must call again later.

===========================================================================*/
boolean
event_drain (osal_sigs_t mask)
{
  osal_sigs_t return_sigs;
  osal_sigs_t temp_get_sigs;
  unsigned int pkt_len = 0;     /* Length of 'events' member of event_rpt_type */
  uint8 num_events = 0;
  uint16 evt_prep_len = 0;
  int return_val = 0;
  /* Persistent data */
  static event_store_type *event_q_item_sav = NULL;
  static event_store_type *event = NULL;
  static event_rpt_type hdr = {0, 0, {0}};
  static diag_event_ext_rsp_type ext_rsp = { {0, 0, 0, 0}, 0, 0,{0}};
  static boolean finished_sav = TRUE;       /* return value */
  static diag_send_desc_type send = {NULL, NULL, DIAG_SEND_STATE_START, FALSE,FALSE,FALSE};
  static uint32 pkt_len_sav = 0;
  static uint8  num_events_sav = 0;
  boolean version_id = TRUE;

  /* Local copies of static variables for optimized access.  
     This makes this routine persistent but not re-entrant. */
  event_store_type *event_q_item = event_q_item_sav;
  uint8 *frag = (uint8 *) send.pkt;
  uint8 *frag_last = (uint8 *) send.last;
  boolean finished = finished_sav;       /* return value */

  /* Local variables */
  event_type *event_full = NULL;
  event_trunc_type *event_trunc = NULL;
  diagbuf_status_enum_type status = DIAGBUF_READY_S;

  (void) mask;     /* suppress compiler warning */

  num_events = num_events_sav;
  pkt_len    = pkt_len_sav;

  /* If finished (not in progress), event_drain() is at a packet boundary. */
  if(finished)
  {

    /* If packet boundary not established, check if the event queue has
       crossed its threshold.  This settles a corner case where the stale
       timer isn't serviced while trying to TX in diag_rsp_malloc(). */
    if (!event_rpt_last)
    {
      return_val = osal_get_sigs(diag_task_tcb, &temp_get_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
      if(temp_get_sigs & DIAG_EVENT_TIMER_SIG)
      {
        /* The stale timer expired and we aren't bust sending a packet.  
           Clear the sig and prep the next packet (calling event_stale()). */
        return_val = osal_reset_sigs(diag_task_tcb, DIAG_EVENT_TIMER_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);
        /*Update the flag to FALSE or else the stale timer is never set again*/
        diag_event_stale_timer_pending = FALSE;

        /* This will set event_rpt_last. */
        event_stale ();
      }
    }

    if(diagcomm_sio_flow_enabled() != TRUE)
    {
      return FALSE;
    }

  /* If set, a packet boundary has been established - prep the packet */
    if (event_rpt_last)
    {
        event = (event_store_type *) q_check (&event_q.q);
        if(!event)
        {
          /*there is nothing in the queue -- 
            error case since event_rpt_last is set*/
          event_rpt_last = NULL;
          return FALSE;
        }
        if(event->version == 0)
       {
      hdr.cmd_code = DIAG_EVENT_REPORT_F;
      hdr.length = 0;
      hdr.length = event_rpt_prep ();
          //event_curr_version = event->version;
      evt_prep_len = hdr.length;
      if (hdr.length > 0)
      {
        /* A report packet is now in progress */
        finished = FALSE;
        frag = (uint8 *) &hdr;
        frag_last = frag + FPOS (event_rpt_type, events) - 1;

        /* Reset send descriptor's state. */
        send.state = DIAG_SEND_STATE_START;
      }
      else
           frag = NULL;
       }
       else
       {
        ext_rsp.ext_hdr.cmd_code = DIAG_CMD_EXT_F;
        ext_rsp.ext_hdr.version = event->version;
        ext_rsp.ext_hdr.id = event->id;
        ext_rsp.ext_hdr.proc_id = DIAG_MY_PROC_ID;
        ext_rsp.event_cmd_code = DIAG_EVENT_REPORT_F;
        ext_rsp.length = event_rpt_prep ();
         
        evt_prep_len = ext_rsp.length;
        if(ext_rsp.length > 0)
      {
          /* A report packet is now in progress */
          finished = FALSE;

          frag = (uint8 *) &ext_rsp;
          frag_last = frag + FPOS (diag_event_ext_rsp_type, events) - 1;

          /* Reset send descriptor's state. */
          send.state = DIAG_SEND_STATE_START;
        }
        else
        frag = NULL;
      }
    }
  }
  

  while (diagcomm_status(DIAGCOMM_PORT_1) && version_id && frag && status == DIAGBUF_READY_S)
  {
    send.pkt = (void *) frag;
    send.last = (void *) frag_last;

    status = diagbuf_send_pkt (&send, DIAG_STREAM_1);

    /* Last does not change - only need to update 'pkt'. */
    frag = (uint8 *) send.pkt;

    if(send.state == DIAG_SEND_STATE_COMPLETE)
    {
      frag = NULL;

      /*  The event from the queue has completed processing.  This check
          will fail for the packet header. */
      if(event_q_item)
      {
        diag_event_free (event_q_item);
#ifdef FEATURE_DEBUG_DIAG_HEAP
        event_drain_free_cnt++;
        MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
                "FREE event_drain_free_cnt = %d, diag heap used = %d ",
                event_drain_free_cnt,diag_event_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */

        event_q_item = NULL;
      }

      if(send.terminate)
      {
        send.terminate = FALSE;

        /* Even if more events are ready, we must give up control to allow
           the packet service a chance to run.  Therefore, return "finished"
           and wait for DIAG to call again. */
        finished = TRUE;
        num_events = 0;
        pkt_len    = 0;

        osal_lock_mutex(&event_rpt_last_cs);
        /* Clear the packet termination flag. */
        event_rpt_last = NULL;

        osal_unlock_mutex(&event_rpt_last_cs);

        /* Trigger another event cycle if necessary.
           Set Diag for general drain. */
        event_cycle();
        diag_set_drain_timer();

        break; /* Exit function and wait to be called again. */
      } /* if send.terminate (pkt complete): finished, event_rpt_last = NULL */
    } /* if send complete: free q item, reset frag, handle terminate case */

    if(!frag)
    {
      /* If a packet is in progress, get the next event */
      event_q_item = event_q_get ();

      if(event_q_item)
      {
#ifdef FEATURE_DEBUG_DIAG_HEAP
        event_q_get_cnt++;
        MSG_4(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
                "event_q_item event_q_get_cnt = %d, diag heap used = %d, \
				EVENT ID = %d, EVENT Payload = %d ",
				event_q_get_cnt, diag_event_heap_used_bytes(),
				(*(event_q_item)).event_id.event_id_field.id,
                (*(event_q_item)).event_id.event_id_field.payload_len);

#endif /* FEATURE_DEBUG_DIAG_HEAP */

        /* Reset send descriptor's state */
        send.state = DIAG_SEND_STATE_BUSY;

        /* Format the event from the queue to as the packet format and assign
           the packet fragment pointer to the start of the correctly formatted
           packet fragment. */

        /* First, assign the packet fragment pointer to the start of the
           payload. */
        frag = event_q_item->payload.payload;

        /* Init the last byte ptr to the size of the payload.  Adjustments 
           will be made below. */
        frag_last = frag + event_q_item->payload.length - 1;

        /* If the payload is more than 2 bytes, include the length field. */
        if(event_q_item->payload.length > EVENT_PAY_TWO_BYTE)
        { 
          /* Adjust fragment pointer to include the length field. */
          frag -= FPOS (event_payload_type, payload);
        }

        /* Now that 'frag' is pointing to the start of the payload portion
           of the event (which follows the event ID/time structure), adjust
           'frag' to include the event itself.  This amount varies based on
           whether the time stamp is truncated or not. */

        /* Manipulate the event to match the packet format. */
        if(event_q_item->event_id.event_id_field.time_trunc_flag)
        {
          /* Adjust 'frag' and its length */
          frag -= sizeof (event_trunc_type);

          /* 'frag' now points to the start of the event */
          event_trunc = (event_trunc_type *) frag;

          /* Shift to capture 2nd and 3rd bytes for trunc time stamp */
          event_trunc->trunc_ts = (uint16)
                                  ((qw_lo (event_q_item->ts) << 8) >> (8 * 2));

          event_trunc->id = event_q_item->event_id.sized_field;

          event_trunc = NULL;     /* Done with this reference */
        }
        else
        {
          /* Adjust 'frag' and its length */
          frag -= sizeof (event_type);

          event_full = (event_type *) frag;

          /* Unless the length field is ommitted, the time stamp is already
             in place. */
          if(event_q_item->payload.length <= EVENT_PAY_TWO_BYTE)
          {
            (void) memsmove ((void *) event_full->ts, sizeof(event_full->ts), (void *) event_q_item->ts, sizeof (qword));
          }
          else
          {
            /* Time stamp is already in place. No work needed here. */
          }

          /* Copy the event ID before the time stamp */
          event_full->id = event_q_item->event_id.sized_field;

          event_full = NULL;      /* Done with this reference */
        }

        pkt_len += frag_last - frag + 1;
        num_events++;

        /* We have processed a new event. Check if this is the last for
           this packet. */
        if((event_q_item == event_rpt_last) || 
           (num_events >= event_pkg_count) || (pkt_len >= evt_prep_len))
        {
          /* Set flag to terminate the packet. */
          send.terminate = TRUE;
        }

      }
      else
      {
        ERR_FATAL ("event_q_get returned NULL!", 0 ,0, 0);
      }
    }
  }

  /* Commit local copies of persistent data back to static variables. */
  event_q_item_sav = event_q_item;
  send.pkt = (void *) frag;
  send.last = (void *) frag_last;
  finished_sav = finished;       /* return value */
  pkt_len_sav = pkt_len;
  num_events_sav = num_events;

  return finished;
}                               /* event_drain */

boolean
event_drain_sec (osal_sigs_t mask)
{
  osal_sigs_t return_sigs;
  osal_sigs_t temp_get_sigs;
  unsigned int pkt_len = 0;     /* Length of 'events' member of event_rpt_type */
  uint8 num_events = 0;
  uint16 evt_prep_len = 0;
  int return_val = 0;
  /* Persistent data */
  static event_store_type *event_q_item_sav = NULL;
  static event_rpt_type hdr = {0, 0, {0}};
  static boolean finished_sav = TRUE;       /* return value */
  static diag_send_desc_type send = {NULL, NULL, DIAG_SEND_STATE_START, FALSE,FALSE,FALSE};

  /* Local copies of static variables for optimized access.  
     This makes this routine persistent but not re-entrant. */
  event_store_type *event_q_item = event_q_item_sav;
  uint8 *frag = (uint8 *) send.pkt;
  uint8 *frag_last = (uint8 *) send.last;
  boolean finished = finished_sav;       /* return value */

  /* Local variables */
  event_type *event_full = NULL;
  event_trunc_type *event_trunc = NULL;
  diagbuf_status_enum_type status = DIAGBUF_READY_S;

  (void) mask;     /* suppress compiler warning */

  /* If finished (not in progress), event_drain() is at a packet boundary. */
  if(finished)
  {

    /* If packet boundary not established, check if the event queue has
       crossed its threshold.  This settles a corner case where the stale
       timer isn't serviced while trying to TX in diag_rsp_malloc(). */
    if(!event_rpt_last_sec)
    {
      return_val = osal_get_sigs(diag_task_tcb, &temp_get_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
      if(temp_get_sigs & DIAG_EVENT_TIMER_SEC_SIG)
      {
        /* The stale timer expired and we aren't bust sending a packet.  
           Clear the sig and prep the next packet (calling event_stale()). */
        return_val = osal_reset_sigs(diag_task_tcb, DIAG_EVENT_TIMER_SEC_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);
        
        /*Update the flag to FALSE or else the stale timer is never set again*/
        diag_event_stale_timer_pending_sec= FALSE;

        /* This will set event_rpt_last. */
        event_stale_sec ();
      }
    }

    /* If set, a packet boundary has been established - prep the packet */
    if(event_rpt_last_sec)
    {
      hdr.cmd_code = DIAG_EVENT_REPORT_F;
      hdr.length = event_rpt_prep_sec ();
      evt_prep_len = hdr.length;
      if(hdr.length > 0)
      {
        /* A report packet is now in progress */
        finished = FALSE;

        frag = (uint8 *) &hdr;
        frag_last = frag + FPOS (event_rpt_type, events) - 1;

        /* Reset send descriptor's state. */
        send.state = DIAG_SEND_STATE_START;
      }
      else
      {
        frag = NULL;
      }
    }
  }

  evt_prep_len = hdr.length;
  
  while (diagcomm_status(DIAGCOMM_PORT_2) && frag && status == DIAGBUF_READY_S && diagcomm_sio_flow_enabled() == TRUE)
  {
    send.pkt = (void *) frag;
    send.last = (void *) frag_last;

    status = diagbuf_send_pkt (&send, DIAG_STREAM_2);

    /* Last does not change - only need to update 'pkt'. */
    frag = (uint8 *) send.pkt;

    if(send.state == DIAG_SEND_STATE_COMPLETE)
    {
      frag = NULL;

      /*  The event from the queue has completed processing.  This check
          will fail for the packet header. */
      if(event_q_item)
      {
        diag_event_free_sec (event_q_item);
#ifdef FEATURE_DEBUG_DIAG_HEAP
        event_drain_free_cnt_sec++;
        MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
                    "FREE event_drain_free_cnt = %d, diag heap used = %d ",
                    event_drain_free_cnt_sec,diag_event_sec_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */

        event_q_item = NULL;
      }

      if(send.terminate)
      {
        send.terminate = FALSE;

        /* Even if more events are ready, we must give up control to allow
           the packet service a chance to run.  Therefore, return "finished"
           and wait for DIAG to call again. */
        finished = TRUE;

        osal_lock_mutex(&event_rpt_last_sec_cs);
        /* Clear the packet termination flag. */
        event_rpt_last_sec = NULL;

        osal_unlock_mutex(&event_rpt_last_sec_cs);

        event_cycle_sec();
        diag_set_drain_timer();

        break; /* Exit function and wait to be called again. */
      } /* if send.terminate (pkt complete): finished, event_rpt_last = NULL */
    } /* if send complete: free q item, reset frag, handle terminate case */

    if(!frag)
    {
      /* If a packet is in progress, get the next event */
      event_q_item = event_q_get_sec ();

      if(event_q_item)
      {
#ifdef FEATURE_DEBUG_DIAG_HEAP
        event_q_get_cnt_sec++;
        MSG_4(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
                    "event_q_item event_q_get_cnt = %d, diag heap used = %d \
                     EVENT ID = %d, EVENT Payload = %d ",
                    event_q_get_cnt_sec,diag_event_sec_heap_used_bytes(),
                  (*(event_q_item)).event_id.event_id_field.id,
					(*(event_q_item)).event_id.event_id_field.payload_len);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

        /* Reset send descriptor's state */
        send.state = DIAG_SEND_STATE_BUSY;

        /* Format the event from the queue to as the packet format and assign
           the packet fragment pointer to the start of the correctly formatted
           packet fragment. */

        /* First, assign the packet fragment pointer to the start of the
           payload. */
        frag = event_q_item->payload.payload;

        /* Init the last byte ptr to the size of the payload.  Adjustments 
           will be made below. */
        frag_last = frag + event_q_item->payload.length - 1;

        /* If the payload is more than 2 bytes, include the length field. */
        if(event_q_item->payload.length > EVENT_PAY_TWO_BYTE)
        { 
          /* Adjust fragment pointer to include the length field. */
          frag -= FPOS (event_payload_type, payload);
        }

        /* Now that 'frag' is pointing to the start of the payload portion
           of the event (which follows the event ID/time structure), adjust
           'frag' to include the event itself.  This amount varies based on
           whether the time stamp is truncated or not. */

        /* Manipulate the event to match the packet format. */
        if(event_q_item->event_id.event_id_field.time_trunc_flag)
        {
          /* Adjust 'frag' and its length */
          frag -= sizeof (event_trunc_type);

          /* 'frag' now points to the start of the event */
          event_trunc = (event_trunc_type *) frag;

          /* Shift to capture 2nd and 3rd bytes for trunc time stamp */
          event_trunc->trunc_ts = (uint16)
                                  ((qw_lo (event_q_item->ts) << 8) >> (8 * 2));

          event_trunc->id = event_q_item->event_id.sized_field;

          event_trunc = NULL;     /* Done with this reference */
        }
        else
        {
          /* Adjust 'frag' and its length */
          frag -= sizeof (event_type);

          event_full = (event_type *) frag;

          /* Unless the length field is ommitted, the time stamp is already
             in place. */
          if(event_q_item->payload.length <= EVENT_PAY_TWO_BYTE)
          {
            (void) memsmove ((void *) event_full->ts, sizeof(event_full->ts), (void *) event_q_item->ts, sizeof (qword));
          }
          else
          {
            /* Time stamp is already in place. No work needed here. */
          }

          /* Copy the event ID before the time stamp */
          event_full->id = event_q_item->event_id.sized_field;

          event_full = NULL;      /* Done with this reference */
        }

        pkt_len += frag_last - frag + 1;
        num_events++;

        /* We have processed a new event. Check if this is the last for
           this packet. */
        if((event_q_item == event_rpt_last_sec) || 
           (num_events >= event_pkg_count_sec) || (pkt_len >= evt_prep_len))
        {
          /* Set flag to terminate the packet. */
          send.terminate = TRUE;
        }

      }
      else
      {
        ERR_FATAL ("event_q_get returned NULL!", 0 ,0, 0);
      }
    }
  }

  /* Commit local copies of persistent data back to static variables. */
  event_q_item_sav = event_q_item;
  send.pkt = (void *) frag;
  send.last = (void *) frag_last;
  finished_sav = finished;       /* return value */

  return finished;
}                               /* event_drain */

/*===========================================================================

FUNCTION DIAG_CLEAR_EVENT_STALE_TIMER

DESCRIPTION
  This function clears the event staleness timer if it is set.

===========================================================================*/
static inline void
diag_clear_event_stale_timer(void)
{
  int return_val = 0;
  if (diag_event_stale_timer_pending)
  {
    return_val = osal_reset_timer(&event_stale_timer);
    diag_event_stale_timer_pending = FALSE;
    ASSERT(OSAL_SUCCESS == return_val);
  }
}
/*===========================================================================

FUNCTION DIAG_EVENT_STALE_TIMER_ENABLED

DESCRIPTION
  This function returns whether the event stale timer is currently enabled.

===========================================================================*/
static inline boolean
diag_event_stale_timer_enabled(void)
{
  return event_stale_timer_len > 0;
}
/*===========================================================================

FUNCTION DIAG_SET_EVENT_STALE_TIMER

DESCRIPTION
  This function sets the event staleness timer if it is not set.

===========================================================================*/
static inline void
diag_set_event_stale_timer()
{
  int return_val = 0;
  
  if ( ! diag_event_stale_timer_pending && diag_event_stale_timer_enabled() ) {
    diag_event_stale_timer_pending = TRUE;
    return_val = osal_set_timer(&event_stale_timer, event_stale_timer_len);
    ASSERT(OSAL_SUCCESS == return_val);
  }
} /* diag_set_event_stale_timer */

/*===========================================================================

FUNCTION DIAG_CLEAR_EVENT_STALE_TIMER_SEC

DESCRIPTION
  This function clears the secondary event staleness timer if it is set.

===========================================================================*/
static inline void
diag_clear_event_stale_timer_sec(void)
{
  int return_val = 0;
  if (diag_event_stale_timer_pending_sec)
  {
    return_val = osal_reset_timer(&event_stale_timer_sec);
    diag_event_stale_timer_pending_sec = FALSE;
    ASSERT(OSAL_SUCCESS == return_val);
  }
}
/*===========================================================================

FUNCTION DIAG_EVENT_STALE_TIMER_SEC_ENABLED

DESCRIPTION
  This function returns whether the secondary event stale timer is currently
  enabled.

===========================================================================*/
static inline boolean
diag_event_stale_timer_sec_enabled(void)
{
  return event_stale_timer_len > 0;
}
/*===========================================================================

FUNCTION DIAG_SET_EVENT_STALE_TIMER_SEC

DESCRIPTION
  This function sets the event staleness timer for the secondary channel.

===========================================================================*/
static inline void
diag_set_event_stale_timer_sec()
{
  int return_val = 0;
  
  if ( ! diag_event_stale_timer_pending_sec && diag_event_stale_timer_sec_enabled() ) {
    diag_event_stale_timer_pending_sec = TRUE;
    return_val = osal_set_timer(&event_stale_timer_sec, event_stale_timer_len);
    ASSERT(OSAL_SUCCESS == return_val);
  }
} /* diag_set_event_stale_timer_sec */

/*===========================================================================

FUNCTION EVENT_CONTROL

DESCRIPTION
  Enable/disable the event service.  Since multiple clients can configure 
  the service, a mask is used.  Only the bits in 'config' that have its
  corresponding bit in 'config_mask' set to '1' will effect the 
  configuration of the event service.
  
===========================================================================*/
void
event_control (uint8 config, uint8 config_mask)
{
  uint8 change_mask;

  /* First, clear all unused bits from config mask. */
  config_mask &= EVENT_CONFIG_VALID_MASK;

  /* Next, set the bits that are masked to be set */
  change_mask = config & config_mask;
  event_config |= change_mask;

  /* Last, clear the bits that are masked to be cleared */
  change_mask = config | ~config_mask;
  event_config &= change_mask;

  if(event_config != 0) {
    event_cycle();
  }
  else
  {
    event_stale (); /* trigger events to be sent */

    diag_clear_event_stale_timer();
  }
}                               /* event_control */
void
event_control_sec (uint8 config, uint8 config_mask)
{
  uint8 change_mask;

  /* First, clear all unused bits from config mask. */
  config_mask &= EVENT_CONFIG_VALID_MASK;

  /* Next, set the bits that are masked to be set */
  change_mask = config & config_mask;
  event_config_sec |= change_mask;

  /* Last, clear the bits that are masked to be cleared */
  change_mask = config | ~config_mask;
  event_config_sec &= change_mask;

  if(event_config_sec != 0) {
    event_cycle_sec();
  }
  else
  {
    event_stale_sec (); /* trigger events to be sent */

    diag_clear_event_stale_timer_sec();
  }
}                               /* event_control_sec */

/*=========================================================================*/
void
event_flush (void)
{
  event_flush_last = (event_store_type *) q_last_check (&event_q.q);

  if(event_flush_last == NULL)
  {
    event_control (0, EVENT_CONFIG_COMM_MASK);
  }
}
/*=========================================================================*/
void
event_flush_sec (void)
{
  event_flush_last_sec = (event_store_type *) q_last_check (&event_q_sec.q);

  if(event_flush_last_sec == NULL)
  {
    event_control_sec (0, EVENT_CONFIG_COMM_MASK);
  }
}

/*===========================================================================

FUNCTION EVENT_STALE

DESCRIPTION
  This routine triggers the event service to wake up the DIAG task and send
  whatever is queued.  This is used to flush as well as to avoid stale
  events lingering in the event queues for extended periods of time.

ASSUMPTIONS
  This funciton is only called in DIAG task context.

===========================================================================*/
void
event_stale (void)
{
  osal_sigs_t return_sigs; 
  int return_val = 0;

  osal_lock_mutex(&event_rpt_last_cs);
  if (!event_rpt_last)
  {
    event_rpt_last = (event_store_type *) q_last_check (&event_q.q);
  }
  osal_unlock_mutex(&event_rpt_last_cs);

  if( osal_ints_are_locked())
  {
    return_val = osal_set_timer(&event_notify_timer, 1);
    ASSERT(OSAL_SUCCESS == return_val);
  }
  else
  {
    return_val = osal_set_sigs(diag_task_tcb, DIAG_EVENT_DRAIN_SIG, &return_sigs);
    ASSERT(OSAL_SUCCESS == return_val);
  }

  return;
}                               /* event_stale */
void
event_stale_sec (void)
{
  osal_sigs_t return_sigs; 
  int return_val = 0;

  osal_lock_mutex(&event_rpt_last_sec_cs);
  if(!event_rpt_last_sec)
  {
    event_rpt_last_sec = (event_store_type *) q_last_check (&event_q_sec.q);

  }
  osal_unlock_mutex(&event_rpt_last_sec_cs);

  if( osal_ints_are_locked())
  {
    return_val = osal_set_timer(&event_notify_timer_sec, 1);
    ASSERT(OSAL_SUCCESS == return_val);
  }
  else
  {
    return_val = osal_set_sigs(diag_task_tcb, DIAG_EVENT_DRAIN_SEC_SIG, &return_sigs);
    ASSERT(OSAL_SUCCESS == return_val);
  }

  return;
}                               /* event_stale */


/*===========================================================================

FUNCTION EVENT_INBOUND_PKT

DESCRIPTION
  This is the packet handler for the event service configuration packet.

===========================================================================*/
PACK(void *)
event_inbound_pkt (PACK(void *) req_pkt, uint16 pkt_len)
{
  event_cfg_req_type *req = (event_cfg_req_type *) req_pkt;
  event_cfg_rsp_type *rsp = NULL;
  const unsigned int rsp_len = sizeof (event_cfg_rsp_type);
  uint8 config;
  uint8 set_mask;
  int i =0;
  boolean send_rsp = TRUE;

  (void) pkt_len; /* suppress compiler warning */

  if(req)
  {
    config = (req->enable) ? EVENT_CONFIG_COMM_MASK : 0;

    event_control (config, EVENT_CONFIG_COMM_MASK);

    /* if master processor on multi processor build, don't send response */
    #if !defined(DIAG_RSP_SEND)
    if(!diagcomm_apps_only_config)
      send_rsp = FALSE;
    #endif

    #if defined(DIAG_MP_MASTER)
    /* MP Master now responds to "set mask" requests */
    send_rsp = TRUE;
    #endif
    
   /* if diag is not connected, don't send response */
    if (!diagcomm_status(DIAGCOMM_PORT_1))
    {
      send_rsp = FALSE;
    }

    if(send_rsp)
    {
      rsp = (event_cfg_rsp_type *) diagpkt_alloc (DIAG_EVENT_REPORT_F, rsp_len);

      if(rsp)
      {
        rsp->length = 0;
      }
    }

    set_mask = (config) ? 0xFF : 0;

    /* Set all the complete bytes to set_mask. Then set the
    ** bits in the last incomplete byte, if there is one.
    */
    for(i = 0; i < EVENT_MASK_SIZE; i++)
      diag_event_mask[DIAG_STREAM_1-1][i] = set_mask;

#if defined(DIAG_MP_MASTER)
    /* Save master event mask */
    for( i = 0; i < MSTR_EVENT_MASK_SIZE; i++ )
      diag_event_mask_mstr[DIAG_STREAM_1-1]->mask[i] = set_mask;

    if( set_mask )
      diag_event_mask_mstr[DIAG_STREAM_1-1]->status = DIAG_CTRL_MASK_ALL_ENABLED;
    else
      diag_event_mask_mstr[DIAG_STREAM_1-1]->status = DIAG_CTRL_MASK_ALL_DISABLED;
#else
    if( set_mask )
      event_preset_status[diag_cur_preset_id-1] = DIAG_CTRL_MASK_ALL_ENABLED;
    else
      event_preset_status[diag_cur_preset_id-1] = DIAG_CTRL_MASK_ALL_DISABLED;
#endif
  }

  (void)diag_client_maskchange(DIAG_EVENTSVC_MASK_CHANGE); /* MAD: TODO: send an err response if this fails? */

  return((void *) rsp);

} /* event_inbound_pkt */

/*
 * Packet processing functions.
 */

/*===========================================================================

FUNCTION EVENT_MASK_GET_HANDLER
DESCRIPTION
  Handles the EVENT_MASK_GET packet. Returns the current event mask.
============================================================================*/
PACK(void *)
event_mask_get_handler (PACK(void *) req_ptr, uint16 pkt_len)
{
  event_mask_get_rsp_type *mask_get_rsp_ptr = NULL;
  dword rsp_len = FPOS(event_mask_get_rsp_type,mask);
  boolean send_rsp = TRUE;

  (void) req_ptr; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */

  /* if master processor on multi processor build, don't send response */
  #if !defined(DIAG_RSP_SEND)
  if(!diagcomm_apps_only_config)
    send_rsp = FALSE;
  #endif

  #if defined(DIAG_MP_MASTER)
    /* For "get mask" requests, Master Diag needs to respond if we're in buffering mode
       or if Modem is not up yet. This response sending logic will be done in diagpkt_process_request(). */
    send_rsp = TRUE;
  #endif
  
/* if diag is not connected, don't send response */
  if (!diagcomm_status(DIAGCOMM_PORT_1))
  {
    send_rsp = FALSE;
  }

  if(send_rsp)
  {
    rsp_len += EVENT_MASK_SIZE;

    mask_get_rsp_ptr = (event_mask_get_rsp_type *)
                       diagpkt_alloc (DIAG_EVENT_MASK_GET_F, rsp_len);
    if(mask_get_rsp_ptr == NULL)
      return NULL;

    mask_get_rsp_ptr->error_code = 0;
    mask_get_rsp_ptr->reserved = 0;
    mask_get_rsp_ptr->numbits = EVENT_LAST_ID + 1;

  memscpy (mask_get_rsp_ptr->mask,EVENT_MASK_SIZE, diag_event_mask[DIAG_STREAM_1-1], EVENT_MASK_SIZE);
  }

  return((void *) mask_get_rsp_ptr);

} /* END event_mask_get_handler */


/*===========================================================================

FUNCTION EVENT PKT GET LOCAL MASKS

DESCRIPTION
  This function will return the event mask values.
  This function queries the local Diag event mask table, as opposed to event_mask_get_handler(),
  which queries the target master event mask table

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACK(void *)
event_pkt_get_local_masks (PACK(void *) req_pkt, uint16 pkt_len)
{
  event_get_local_mask_req_type *req_ptr = (event_get_local_mask_req_type *) req_pkt;
  event_get_local_mask_rsp_type *rsp = NULL;
  diagpkt_subsys_cmd_code_type cmd_code;
  unsigned int rsp_len = 0;

  if(req_ptr == NULL) 
  {
   MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "event_pkt_get_local_masks(): Request pointer is NULL \n",1);
   return NULL;
  }

  cmd_code = diagpkt_subsys_get_cmd_code (req_ptr);

  if ((pkt_len != sizeof(event_get_local_mask_req_type)) || 
       ((cmd_code != DIAGDIAG_EVENT_MASK_RETRIEVAL_MODEM) && (cmd_code != DIAGDIAG_EVENT_MASK_RETRIEVAL_APP)
         && (cmd_code != DIAGDIAG_EVENT_MASK_RETRIEVAL_Q6) && (cmd_code != DIAGDIAG_EVENT_MASK_RETRIEVAL_RIVA)) || 
       (req_ptr->stream_id > DIAG_MAX_STREAM_ID) || (req_ptr->stream_id < DIAG_MIN_STREAM_ID))
  {
     MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "MSG_2 cmd_code %d stream id %d \n",cmd_code,req_ptr->stream_id);
     return ((void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req_ptr, pkt_len));
  }
 

  if(diagcomm_status(DIAGCOMM_PORT_1))
  {
   rsp_len = FPOS (event_get_local_mask_rsp_type, mask) + EVENT_MASK_SIZE;

   rsp = (event_get_local_mask_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, 
                                              cmd_code, rsp_len);

  if(rsp) 
   {
    rsp->status = SUCCESSFUL;
    rsp->stream_id = req_ptr->stream_id;
    rsp->numbits = EVENT_LAST_ID + 1;
    memscpy (rsp->mask, EVENT_MASK_SIZE, diag_event_mask[(req_ptr->stream_id)-1], EVENT_MASK_SIZE);
   }
  }

  return ((void *) rsp);
  
} /* END event_pkt_get_local_masks */


/*===========================================================================

FUNCTION EVENT_MASK_SET_INTERNAL_HANDLER

DESCRIPTION
  Internal function to set the event mask and save to master event mask.
  
PARAMETERS
  op_code - Operation code for this function:
               DIAG_OP_SET_EVENT_STREAM - Set event mask by Stream ID. 'id' param will represent event Stream ID to set.
               DIAG_OP_SET_EVENT_PRESET - Set event mask by Preset ID. 'id' param will represent Preset ID to set for Stream 1.
  id - Stream ID or Preset ID. (See 'op_code').
  numbits - Number of bits in 'mask_ptr' to actually set the event mask.
  mask_len - Length (in bytes) of 'mask_ptr'
  mask_ptr - Pointer to entire source event mask to set.
  
RETURN VALUE
  None
  
============================================================================*/
void event_mask_set_internal_handler( uint8 op_code, uint8 id, uint16 numbits, 
                                      unsigned int mask_len, unsigned char * mask_ptr )
{
  uint16 numbits_to_set, numbytes = 0;
  uint8 last_byte_mask = 0;
  unsigned char * event_mask_ptr = NULL;
#if defined(DIAG_MP_MASTER)
  diag_event_mask_type * event_mask_mstr_ptr = NULL;
#endif

  if( (mask_ptr == NULL) || (numbits <= 0) )
    return;
    
  if( op_code == DIAG_OP_SET_EVENT_STREAM )
  {
    /* Set by stream. Use current preset. (id = stream_id) */
    if( (id < DIAG_MIN_STREAM_ID) || (id > DIAG_MAX_STREAM_ID) )
      return;
            
    event_mask_ptr = diag_event_mask[id-1];
#if defined(DIAG_MP_MASTER)
    event_mask_mstr_ptr = diag_event_mask_mstr[id-1];
#endif
  }
  else if( op_code == DIAG_OP_SET_EVENT_PRESET )
  {
    /* Set by preset. Only for Stream 1. (id = preset_id) */    
    if( (id < DIAG_MIN_PRESET_ID) || (id > DIAG_MAX_PRESET_ID) )
      return;
      
    event_mask_ptr = diag_event_preset_mask[id-1];
#if defined(DIAG_MP_MASTER)
    event_mask_mstr_ptr = &diag_event_mask_mstr_preset[id-1];
#endif
  }
  else
  {
    return;
  }
    
  /* Adjust the specified number of mask bits using the number of bits
  ** in the packet and the number of bits in the mask array.
  */
  numbits_to_set = numbits;
  if( numbits_to_set > mask_len * 8 )
    numbits_to_set = (uint16) (mask_len * 8);
  if( numbits_to_set >= EVENT_LAST_ID )
    numbits_to_set = EVENT_LAST_ID + 1;
  numbytes = (numbits_to_set + 7)/8;

  /* Copy all the complete bytes from the input mask over. Then copy the
  ** bits in the last incomplete byte, if there is one.
  */
  memscpy( event_mask_ptr, EVENT_MASK_SIZE, mask_ptr, numbits_to_set/8 );

  last_byte_mask = 0xff;
  last_byte_mask <<= (numbits_to_set & 0x07);
  if( last_byte_mask != 0xff )
  {
    event_mask_ptr[numbytes - 1] = (event_mask_ptr[numbytes - 1] & last_byte_mask) | (mask_ptr[numbytes - 1] & ~last_byte_mask);
  }

#if defined(DIAG_MP_MASTER)
  /* Save mask changes to master event mask */
  numbits_to_set = numbits;
  if( numbits_to_set > mask_len * 8 )
    numbits_to_set = (uint16) (mask_len * 8);
  if( numbits_to_set >= MSTR_EVENT_LAST_ID )
    numbits_to_set = MSTR_EVENT_LAST_ID + 1;
  numbytes = (numbits_to_set + 7)/8;

  memscpy( event_mask_mstr_ptr->mask, MSTR_EVENT_MASK_SIZE, mask_ptr, numbits_to_set/8 );

  last_byte_mask = 0xff;
  last_byte_mask <<= (numbits_to_set & 0x07);
  if( last_byte_mask != 0xff )
  {
    event_mask_mstr_ptr->mask[numbytes - 1] = 
                                (event_mask_mstr_ptr->mask[numbytes - 1] & last_byte_mask) |
                                (mask_ptr[numbytes - 1] & ~last_byte_mask);
  }

  if( event_mask_mstr_ptr->mask_len < numbytes )
    event_mask_mstr_ptr->mask_len = numbytes;
    
  if( event_mask_mstr_ptr->numbits < numbits_to_set )
    event_mask_mstr_ptr->numbits = numbits_to_set;

  event_mask_mstr_ptr->status = DIAG_CTRL_MASK_VALID;
#else
  if( (op_code == DIAG_OP_SET_EVENT_STREAM) && (id == DIAG_STREAM_1) )
    event_preset_status[diag_cur_preset_id-1] = DIAG_CTRL_MASK_VALID;
  else if( op_code == DIAG_OP_SET_EVENT_PRESET )
    event_preset_status[id-1] = DIAG_CTRL_MASK_VALID;
#endif

} /* event_mask_set_internal_handler */


/*===========================================================================

FUNCTION EVENT_MASK_SET_HANDLER

DESCRIPTION
  Handles the EVENT_MASK_SET packet. Sets the event mask.
============================================================================*/
PACK(void *)
event_mask_set_handler (PACK(void *) req_ptr, uint16 pkt_len)
{
  event_mask_set_req_type *mask_set_req_ptr;
  event_mask_set_rsp_type *mask_set_rsp_ptr = NULL;
  dword rsp_len = FPOS(event_mask_set_rsp_type,mask);
  unsigned int mask_len = (unsigned int) pkt_len -
                          FPOS (event_mask_set_req_type, mask);
  boolean send_rsp = TRUE;
  uint16 numbits = 0;
  uint32 rsp_mask_len = 0;
  unsigned char * rsp_mask = NULL;
  
  mask_set_req_ptr = (event_mask_set_req_type *) req_ptr;

  /* if master processor on multi processor build, don't send response */
  #if !defined(DIAG_RSP_SEND)
  if(!diagcomm_apps_only_config)
    send_rsp = FALSE;
  #endif

  #if defined(DIAG_MP_MASTER)
  /* MP Master now responds to "set mask" requests */
  send_rsp = TRUE;
  #endif

  /* if diag is not connected, don't send response */
  if (!diagcomm_status(DIAGCOMM_PORT_1))
  {
    send_rsp = FALSE;
  }

  event_mask_set_internal_handler( DIAG_OP_SET_EVENT_STREAM, DIAG_STREAM_1, mask_set_req_ptr->numbits, 
                                   mask_len, (unsigned char *)mask_set_req_ptr->mask );

  if(send_rsp)
  {
    #if defined(DIAG_MP_MASTER)
      rsp_mask_len = diag_event_mask_mstr_preset[diag_cur_preset_id-1].mask_len;
      numbits = diag_event_mask_mstr_preset[diag_cur_preset_id-1].numbits;
      rsp_mask = diag_event_mask_mstr_preset[diag_cur_preset_id-1].mask;
    #else
      rsp_mask_len = EVENT_MASK_SIZE;
      numbits = EVENT_LAST_ID + 1;
      rsp_mask = diag_event_mask[DIAG_STREAM_1-1];
    #endif
        
    rsp_len += rsp_mask_len;
    mask_set_rsp_ptr = (event_mask_set_rsp_type *)
                       diagpkt_alloc (DIAG_EVENT_MASK_SET_F, rsp_len);
    if(mask_set_rsp_ptr == NULL)
      return NULL;

    mask_set_rsp_ptr->error_code = 0;
    mask_set_rsp_ptr->reserved = 0;
    mask_set_rsp_ptr->numbits = numbits;

    memscpy( mask_set_rsp_ptr->mask,EVENT_MASK_SIZE, rsp_mask, rsp_mask_len );
  }

  (void)diag_client_maskchange(DIAG_EVENTSVC_MASK_CHANGE); /* TODO: send an err response if this fails */

  return((void *) mask_set_rsp_ptr);

} /* event_mask_set_handler */


/*===========================================================================

FUNCTION event_set_all_rt_masks

DESCRIPTION
  This function will set all real time masks to the requested value.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void event_set_all_rt_masks( uint8 config, byte stream_id)
{
  uint8 set_mask;
  int stream_index, mask_index=0;

  set_mask = (config) ? 0xFF : 0;

  if(stream_id == 0)
  {
    for(stream_index= 0; stream_index < DIAG_MAX_STREAM_ID; stream_index++)
    {
      for(mask_index = 0; mask_index < EVENT_MASK_SIZE; mask_index++)
      {
        diag_event_mask[stream_index][mask_index] = set_mask;
      }

#if defined(DIAG_MP_MASTER)
      for( mask_index = 0; mask_index < MSTR_EVENT_MASK_SIZE; mask_index++ )
      {
        diag_event_mask_mstr[stream_index]->mask[mask_index] = set_mask;
      }

      if( set_mask )
        diag_event_mask_mstr[stream_index]->status = DIAG_CTRL_MASK_ALL_ENABLED;
      else
        diag_event_mask_mstr[stream_index]->status = DIAG_CTRL_MASK_ALL_DISABLED;
#else
      if( stream_index == (DIAG_STREAM_1-1) )
      {
        if( set_mask)
          event_preset_status[diag_cur_preset_id-1] = DIAG_CTRL_MASK_ALL_ENABLED;
        else
          event_preset_status[diag_cur_preset_id-1] = DIAG_CTRL_MASK_ALL_DISABLED;
      }
#endif
    }
  }
  else
  {
    for( mask_index = 0; mask_index < EVENT_MASK_SIZE; mask_index++ )
      diag_event_mask[stream_id - 1][mask_index] = set_mask;

#if defined(DIAG_MP_MASTER)
    for( mask_index = 0; mask_index < MSTR_EVENT_MASK_SIZE; mask_index++ )
      diag_event_mask_mstr[stream_id - 1]->mask[mask_index] = set_mask;

    if( set_mask )
      diag_event_mask_mstr[stream_id - 1]->status = DIAG_CTRL_MASK_ALL_ENABLED;
    else
      diag_event_mask_mstr[stream_id - 1]->status = DIAG_CTRL_MASK_ALL_DISABLED;
#else
    if( stream_id == DIAG_STREAM_1 )
    {
      if( set_mask )
        event_preset_status[diag_cur_preset_id-1] = DIAG_CTRL_MASK_ALL_ENABLED;
      else
        event_preset_status[diag_cur_preset_id-1] = DIAG_CTRL_MASK_ALL_DISABLED;
    }
#endif
  }

  (void)diag_client_maskchange(DIAG_EVENTSVC_MASK_CHANGE);

} /* event_set_all_rt_masks */


void event_ctrl_stale_timer(unsigned int timer_len)
{
  event_stale_timer_len = timer_len;
  if (event_stale_timer_len == 0)
  {
    diag_clear_event_stale_timer();
    diag_clear_event_stale_timer_sec();
  }
  else if(event_stale_timer_len > 0) {
    /* Get things rolling to drain any events we may have sitting around */
    event_cycle();
    event_cycle_sec();
  }
}

void event_ctrl_report_size(unsigned int report_size, unsigned int send_max_num, unsigned int rpt_pkt_len_siz)
{
/*Since we check for flow control ONLY before draining events, the maximum report size has a 
limitation that if maximum number of events with maximum payload size are generated, then
it should fit into (lower threshold of DSM items in pool used for outbound allocation + 1)
  number of DSM items.
*/
  event_report_pkt_size = report_size;
  event_send_max_num = send_max_num; /* Set to arbitrarily high value */
  event_report_pkt_len_size = rpt_pkt_len_siz;
}

/*===========================================================================

FUNCTION EVENT_MASK_SSM

DESCRIPTION
  This function will populate diag_event_mask_ssm array with permission masks

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
#ifdef FEATURE_SSM
void event_mask_ssm(
   /*diag_event_mask_ssm has to be passed for this field*/
   diag_mask_ssm_buffer_type* code_range, 
   uint32 size)
{
  int i=0;
  int j =0;
  int check_perm_value, mask_index;

   if ((code_range == NULL)  || (size == 0))
   {
     return;
   }
  
  for (mask_index = 0; mask_index < EVENT_MASK_SIZE; mask_index++)
    {
      #ifdef DIAG_SSM_LOG_EVENT_WHITELIST
      diag_event_mask_ssm[mask_index] = 0x0;
      /*If white listing approach is followed then we check for permission value 0 (not allowed); 
    the mask corresponding to these log codes in permission mask array are set to 1. */
      check_perm_value = 0;
      #else
      diag_event_mask_ssm[mask_index] = 0xff;
      /*If black listing approach is followed then we check for permission value 1 (not allowed); 
    the mask corresponding to these log codes in permission mask array are set to 0. */
      check_perm_value = 1;
      #endif
    }

   osal_lock_mutex(&diag_event_mask_ssm_mutex);
  for (i=0;i<size;i++)
  {  
    /*permission 1 is not allowed*/ 
    if (code_range->permission == check_perm_value)
    {

      for (j=code_range->start;j<=code_range->end; j++)
    {
      if(j>EVENT_LAST_ID) 
      {
        /*Invalid Event code*/
        break;
      }

      #ifdef DIAG_SSM_LOG_EVENT_WHITELIST
      diag_event_mask_ssm[(j)/8] |= (0x01 << ((j) & 0x07));
      #else
      diag_event_mask_ssm[(j)/8] &= (~(0x01 << ((j) & 0x07)));
      #endif
    }
    }
    code_range++;

    }
    free(diag_event_mask_range);
  diag_event_mask_range = NULL;
  num_event_code_ssm = 0;
  osal_unlock_mutex(&diag_event_mask_ssm_mutex);
}
#endif

/*===========================================================================

FUNCTION DIAGDIAG_EXT_EVENT_REPORT_CTRL

DESCRIPTION
  This procedure handles the extended event report control subsystem command.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
PACK(void *) diagdiag_ext_event_report_ctrl(PACK(void *) req_pkt,
                                            uint16 pkt_len)
{
  event_ext_rpt_ctrl_type *req = (event_ext_rpt_ctrl_type *)req_pkt;
  byte config = 0;
  dword rsp_len = 0;
  boolean send_rsp = TRUE;
  boolean send_err = FALSE;
  
  /* if master processor on multi processor build, don't send response */
  #if !defined(DIAG_RSP_SEND)
  if(!diagcomm_apps_only_config)
    send_rsp = FALSE;
  #endif

  #if defined(DIAG_MP_MASTER)
    /* MP Master now responds to "set mask" requests.
       For "get mask" requests, Master Diag needs to respond if we're in buffering mode
       or if Modem is not up yet. This response sending logic will be done in diagpkt_process_request(). */
    send_rsp = TRUE;
  #endif
          
  /* if diag is not connected, don't send response */
  if (!diagcomm_status(DIAGCOMM_PORT_1))
  {
    send_rsp = FALSE;
  }

  /* Sanity checks */
  if( (req_pkt == NULL) || (pkt_len < sizeof(event_ext_rpt_ctrl_type)) )
  {
    if( send_rsp )
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len);
    else
      return NULL;
  }

  switch( req->cmd_version )
  {
    case 1:
    {
      if( (req->id.stream_id > DIAG_MAX_STREAM_ID) ||
          (req->id.stream_id == 0 && req->subcmd != DIAG_EXT_EVENT_REPORT_SET_ALL) )
      {
        /* For command version 1, if stream_id is out of range, send Bad Param error response. */
        send_err = TRUE;
      }
      break;
    }
    case 2:
    {
      if( (req->id.preset_id > DIAG_MAX_PRESET_ID) ||
          (req->id.preset_id == 0 && req->subcmd != DIAG_EXT_EVENT_REPORT_SET_ALL) )
      {
        /* For command version 2, if preset_id is out of range, send Bad Param error response. */
        send_err = TRUE;
      }
      break;
    }
    default:
    {
      /* Send Bad Param error response for unsupported command version. */
      send_err = TRUE;
      break;
    }
  }
  
  if( send_err == TRUE )
  {
    if( send_rsp )
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len);
    else
      return NULL;
  }
    
  MSG_3(MSG_SSID_DIAG, MSG_LEGACY_LOW, "ext_event_report_ctrl subcmd=%d cmd_ver=%d id=%d", 
        req->subcmd, req->cmd_version, req->id.stream_id);
          
   if(req->subcmd == DIAG_EXT_EVENT_REPORT_GET_MASK) // Get Event Mask
   {
    event_ext_rpt_ctrl_rsp_type *rsp=NULL;
    uint16 numbits = 0;
    uint32 mask_len = 0;
    unsigned char * rsp_mask = NULL;
    
    if( send_rsp )
    {      
      rsp_len = FPOS(event_ext_rpt_ctrl_rsp_type,mask);     
        
      if( req->cmd_version == 1 )
      {
        #if defined(DIAG_MP_MASTER)
          mask_len = diag_event_mask_mstr[req->id.stream_id-1]->mask_len;
          numbits = diag_event_mask_mstr[req->id.stream_id-1]->numbits;
          rsp_mask = diag_event_mask_mstr[req->id.stream_id-1]->mask;
        #else
          mask_len = EVENT_MASK_SIZE;
          numbits = EVENT_LAST_ID + 1;
          rsp_mask = diag_event_mask[req->id.stream_id-1];
        #endif
        
        rsp_len += mask_len;
        rsp = (event_ext_rpt_ctrl_rsp_type *) diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, req->subsys_cmd_code, rsp_len);
        if(rsp == NULL)
          return NULL;
          
        rsp->header.id.stream_id = req->id.stream_id;
        memscpy( rsp->mask, mask_len, rsp_mask, mask_len );
      }
      else if( req->cmd_version == 2 )
      {
        #if defined(DIAG_MP_MASTER)
          mask_len = diag_event_mask_mstr_preset[req->id.preset_id-1].mask_len;
          numbits = diag_event_mask_mstr_preset[req->id.preset_id-1].numbits;
          rsp_mask = diag_event_mask_mstr_preset[req->id.preset_id-1].mask;
        #else
          mask_len += EVENT_MASK_SIZE;
          numbits = EVENT_LAST_ID + 1;
          rsp_mask = diag_event_preset_mask[req->id.preset_id-1];
        #endif
        
        rsp_len += mask_len;
        rsp = (event_ext_rpt_ctrl_rsp_type *) diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, req->subsys_cmd_code, rsp_len);
        if(rsp == NULL)
          return NULL;
        
        rsp->header.id.preset_id = req->id.preset_id;
        memscpy( rsp->mask,mask_len, rsp_mask, mask_len );
      }
      
      // diagpkt_subsys_alloc copies the subsys_id and subsys_cmd_code
      rsp->header.cmd_code = req->cmd_code;
      rsp->header.cmd_version = req->cmd_version;
      rsp->header.subcmd = req->subcmd;
      rsp->error_code = 0;
      rsp->numbits = numbits;
      
    }
    return (void *)rsp;
  }

  else if(req->subcmd == DIAG_EXT_EVENT_REPORT_SET_MASK) // Set Event Mask
  {
    unsigned int mask_len;
    uint16 numbits = 0;
    unsigned char * rsp_mask = NULL;
    event_ext_rpt_ctrl_rsp_type *rsp=NULL;
    event_ext_rpt_ctrl_set_mask_req_type *req_set = (event_ext_rpt_ctrl_set_mask_req_type *)req_pkt;
    
    /* Get the number of bits to set from the request. Byte offset of numbits field is 8 */
    mask_len = (unsigned int)(pkt_len - FPOS(event_ext_rpt_ctrl_set_mask_req_type, mask));
    config = 0xFF;
    
    if( req->cmd_version == 1 )
    {
      event_mask_set_internal_handler( DIAG_OP_SET_EVENT_STREAM, req_set->header.id.stream_id,  
                                       req_set->numbits_to_set, mask_len, (unsigned char *)req_set->mask );
      
#if defined(DIAG_MP_MASTER)
      /* Propagate masks to Slaves. */
      if( req_set->header.id.stream_id == DIAG_STREAM_1 )
        diagcomm_mask_update( DIAG_UPDATE_MASK_EVENTS, req_set->header.id.stream_id, diag_cur_preset_id, TRUE );
      else if( req_set->header.id.stream_id == DIAG_STREAM_2 )
        diagcomm_mask_update( DIAG_UPDATE_MASK_EVENTS, req_set->header.id.stream_id, DIAG_DCI_MASK_1, TRUE ); //ToDo: use array?
#endif
    } 
    else if( req->cmd_version == 2 )
    {
      event_mask_set_internal_handler( DIAG_OP_SET_EVENT_PRESET, req_set->header.id.preset_id,  
                                       req_set->numbits_to_set, mask_len, (unsigned char *)req_set->mask );

      event_control( config, EVENT_CONFIG_COMM_MASK );
#if defined(DIAG_MP_MASTER)
      /* Propagate masks to Slaves. */
      diagcomm_mask_update( DIAG_UPDATE_MASK_EVENTS, DIAG_STREAM_1, req_set->header.id.preset_id, TRUE );
#endif
    }

    (void)diag_client_maskchange(DIAG_EVENTSVC_MASK_CHANGE);

    if( send_rsp )
    {
      rsp_len = FPOS(event_ext_rpt_ctrl_rsp_type,mask);
      
      /* Copy back the modified mask into the response */
      if( req->cmd_version == 1 )
      {
        #if defined(DIAG_MP_MASTER)
          mask_len = diag_event_mask_mstr[req->id.stream_id-1]->mask_len;
          numbits = diag_event_mask_mstr[req->id.stream_id-1]->numbits;
          rsp_mask = diag_event_mask_mstr[req->id.stream_id-1]->mask;
        #else
          mask_len = EVENT_MASK_SIZE;
          numbits = EVENT_LAST_ID + 1;
          rsp_mask = diag_event_mask[req->id.stream_id-1];
        #endif
        
        rsp_len += mask_len;
        rsp = (event_ext_rpt_ctrl_rsp_type *)
            diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, req_set->header.subsys_cmd_code, rsp_len);
        if(rsp == NULL)
          return NULL;
        
        rsp->header.id.stream_id = req_set->header.id.stream_id;
        memscpy( rsp->mask, mask_len, rsp_mask, mask_len );
      }
      else if( req->cmd_version == 2 )
      {
        #if defined(DIAG_MP_MASTER)
          mask_len = diag_event_mask_mstr_preset[req->id.preset_id-1].mask_len;
          numbits = diag_event_mask_mstr_preset[req->id.preset_id-1].numbits;
          rsp_mask = diag_event_mask_mstr_preset[req->id.preset_id-1].mask;
        #else
          mask_len = EVENT_MASK_SIZE;
          numbits = EVENT_LAST_ID + 1;
          rsp_mask = diag_event_mask[req->id.stream_id-1];
        #endif
        
        rsp_len += mask_len;
        rsp = (event_ext_rpt_ctrl_rsp_type *)
            diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, req_set->header.subsys_cmd_code, rsp_len);
        if(rsp == NULL)
          return NULL;
          
        rsp->header.id.preset_id = req_set->header.id.preset_id;
        memscpy( rsp->mask, mask_len, rsp_mask, mask_len );
      }
      
      // diagpkt_subsys_alloc copies the subsys_id and subsys_cmd_code
      rsp->header.cmd_code = req_set->header.cmd_code;
      rsp->header.cmd_code = req->cmd_code;
      rsp->header.cmd_version = req->cmd_version;
      rsp->header.subcmd = req->subcmd;
      rsp->error_code = 0;
      rsp->numbits = numbits;
    }
    return (void *)rsp;
  }

  else if( (req->subcmd == DIAG_EXT_EVENT_REPORT_SET_ALL) && (req->cmd_version == 1) )  // Sets the entire mask for streams
  {
    int stream_index, mask_index = 0;
    event_ext_set_all_rsp_type *rsp=NULL;
    event_ext_rpt_ctrl_set_all_req_type *req_all = (event_ext_rpt_ctrl_set_all_req_type *)req_pkt;

    config = (req_all->config)? 0xFF : 0;

    if(req_all->header.id.stream_id > 0 && req_all->header.id.stream_id <= DIAG_MAX_STREAM_ID)
    {
      for(mask_index=0; mask_index < EVENT_MASK_SIZE; mask_index++)
      {
        diag_event_mask[req_all->header.id.stream_id-1][mask_index] = config;
      }

#if defined(DIAG_MP_MASTER)
      /* Save master event mask */
      for( mask_index = 0; mask_index < MSTR_EVENT_MASK_SIZE; mask_index++ )
        diag_event_mask_mstr[req_all->header.id.stream_id-1]->mask[mask_index] = config;

      if( config )
        diag_event_mask_mstr[req_all->header.id.stream_id-1]->status = DIAG_CTRL_MASK_ALL_ENABLED;
      else
        diag_event_mask_mstr[req_all->header.id.stream_id-1]->status = DIAG_CTRL_MASK_ALL_DISABLED;
        
      /* Propagate masks to Slaves. */
      if( req_all->header.id.stream_id == DIAG_STREAM_1 )
        diagcomm_mask_update( DIAG_UPDATE_MASK_EVENTS, req_all->header.id.stream_id, diag_cur_preset_id, TRUE );
      else if( req_all->header.id.stream_id == DIAG_STREAM_2 )
        diagcomm_mask_update( DIAG_UPDATE_MASK_EVENTS, req_all->header.id.stream_id, DIAG_DCI_MASK_1, TRUE ); //ToDo: create array for cur stream preset?
#else
      if( req_all->header.id.stream_id == DIAG_STREAM_1 )
      {
        if( config )
          event_preset_status[diag_cur_preset_id-1] = DIAG_CTRL_MASK_ALL_ENABLED;
        else
          event_preset_status[diag_cur_preset_id-1] = DIAG_CTRL_MASK_ALL_DISABLED;
      }
#endif
    }

    if(req_all->header.id.stream_id == DIAG_STREAM_1)
    {
      event_control (config, EVENT_CONFIG_COMM_MASK);
    }
    else if(req_all->header.id.stream_id == DIAG_STREAM_2)
    {
      event_control_sec (config, EVENT_CONFIG_COMM_MASK);
    }
    else if(!req_all->header.id.stream_id)      
    {
      // Set all the streams if stream_id = 0
      for(stream_index=0; stream_index < DIAG_MAX_STREAM_ID; stream_index++)
      {
        for(mask_index=0; mask_index < EVENT_MASK_SIZE; mask_index++)
        {
          diag_event_mask[stream_index][mask_index] = config;
        }

#if defined(DIAG_MP_MASTER)
        /* Save master event mask */
        for( mask_index = 0; mask_index < MSTR_EVENT_MASK_SIZE; mask_index++ )
          diag_event_mask_mstr[stream_index]->mask[mask_index] = config;

        if( config )
          diag_event_mask_mstr[stream_index]->status = DIAG_CTRL_MASK_ALL_ENABLED;
        else
          diag_event_mask_mstr[stream_index]->status = DIAG_CTRL_MASK_ALL_DISABLED;
#else
        if( stream_index == (DIAG_STREAM_1-1) )
        {
          if( config )
            event_preset_status[diag_cur_preset_id-1] = DIAG_CTRL_MASK_ALL_ENABLED;
          else
            event_preset_status[diag_cur_preset_id-1] = DIAG_CTRL_MASK_ALL_DISABLED;
        }
#endif
      }
      event_control(config, EVENT_CONFIG_COMM_MASK);
      event_control_sec(config, EVENT_CONFIG_COMM_MASK);
      
#if defined(DIAG_MP_MASTER)
      /* Propagate masks to Slaves. */
      diagcomm_mask_update( DIAG_UPDATE_MASK_EVENTS, DIAG_STREAM_1, diag_cur_preset_id, TRUE );
      diagcomm_mask_update( DIAG_UPDATE_MASK_EVENTS, DIAG_STREAM_2, DIAG_DCI_MASK_1, TRUE ); //ToDo: create array?
#endif
    }

    (void)diag_client_maskchange(DIAG_EVENTSVC_MASK_CHANGE);

    if(send_rsp)
    {
      rsp_len = sizeof(event_ext_set_all_rsp_type);
      rsp = (event_ext_set_all_rsp_type *)
            diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, req_all->header.subsys_cmd_code, rsp_len);
      if(rsp == NULL)
        return NULL;

      rsp->header.cmd_code = req_all->header.cmd_code;
      // diagpkt_subsys_alloc copies the subsys_id and subsys_cmd_code
      rsp->header.cmd_version = req_all->header.cmd_version;
      rsp->header.subcmd = req_all->header.subcmd;
      rsp->header.id.stream_id = req_all->header.id.stream_id;
      rsp->config = req_all->config;
      rsp->error_code = 0;
    }
    return(void *)rsp;
  }

  else if( (req->subcmd == DIAG_EXT_EVENT_REPORT_SET_ALL) && (req->cmd_version == 2) ) // Sets the entire mask for presets
  {
    uint32 mask_index = 0;
    uint8 preset_index = 0;
    event_ext_set_all_rsp_type *rsp=NULL;
    event_ext_rpt_ctrl_set_all_req_type *req_all = (event_ext_rpt_ctrl_set_all_req_type *)req_pkt;
   
#if defined(DIAG_MP_MASTER)
    uint32 mstr_mask_index = 0;
#endif

    config = (req_all->config)? 0xFF : 0;

    // Sets the whole mask for a single specified preset
    if( req_all->header.id.preset_id > 0 && req_all->header.id.preset_id <= DIAG_MAX_PRESET_ID )
    {
      for( mask_index=0; mask_index < EVENT_MASK_SIZE; mask_index++ )
      {
        diag_event_preset_mask[req_all->header.id.preset_id-1][mask_index] = config;
      }
      
      if( req_all->header.id.preset_id == diag_cur_preset_id )
      {
        event_control(config, EVENT_CONFIG_COMM_MASK);
      }
    
#if defined(DIAG_MP_MASTER)
      /* Save master event mask */
      for( mstr_mask_index = 0; mstr_mask_index < MSTR_EVENT_MASK_SIZE; mstr_mask_index++ )
        diag_event_mask_mstr_preset[req_all->header.id.preset_id-1].mask[mstr_mask_index] = config;
        
      if( config )
        diag_event_mask_mstr_preset[req_all->header.id.preset_id-1].status = DIAG_CTRL_MASK_ALL_ENABLED;
      else
        diag_event_mask_mstr_preset[req_all->header.id.preset_id-1].status = DIAG_CTRL_MASK_ALL_DISABLED;
        
      /* Propagate masks to Slaves. */
      diagcomm_mask_update( DIAG_UPDATE_MASK_EVENTS, DIAG_STREAM_1, req_all->header.id.preset_id, TRUE );
#else
      if( config )
        event_preset_status[req_all->header.id.preset_id-1] = DIAG_CTRL_MASK_ALL_ENABLED;
      else
        event_preset_status[req_all->header.id.preset_id-1] = DIAG_CTRL_MASK_ALL_DISABLED;
#endif
    }
    
    // Set the whole mask for all presets if preset_id = 0
    if( req_all->header.id.preset_id == 0 )
    {
      for( preset_index=0; preset_index < DIAG_MAX_PRESET_ID; preset_index++ )
      {
        for( mask_index=0; mask_index < EVENT_MASK_SIZE; mask_index++ )
        {
          diag_event_preset_mask[preset_index][mask_index] = config;
        }

#if defined(DIAG_MP_MASTER)
        /* Save master event mask */
        for( mstr_mask_index = 0; mstr_mask_index < MSTR_EVENT_MASK_SIZE; mstr_mask_index++ )
          diag_event_mask_mstr_preset[preset_index].mask[mstr_mask_index] = config;
        
        if( config )
          diag_event_mask_mstr_preset[preset_index].status = DIAG_CTRL_MASK_ALL_ENABLED;
        else
          diag_event_mask_mstr_preset[preset_index].status = DIAG_CTRL_MASK_ALL_DISABLED;
          
        /* Propagate masks to Slaves. */
        diagcomm_mask_update( DIAG_UPDATE_MASK_EVENTS, DIAG_STREAM_1, preset_index+1, TRUE );
#else
        if( config )
          event_preset_status[preset_index] = DIAG_CTRL_MASK_ALL_ENABLED;
        else
          event_preset_status[preset_index] = DIAG_CTRL_MASK_ALL_DISABLED;
#endif
      }
      event_control(config, EVENT_CONFIG_COMM_MASK);
    }

    (void)diag_client_maskchange(DIAG_EVENTSVC_MASK_CHANGE);
        
    if(send_rsp)
    {
      rsp_len = sizeof(event_ext_set_all_rsp_type);
      rsp = (event_ext_set_all_rsp_type *)
            diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, req_all->header.subsys_cmd_code, rsp_len);
      if(rsp == NULL)
        return NULL;
  
      rsp->header.cmd_code = req_all->header.cmd_code;
      // diagpkt_subsys_alloc copies the subsys_id and subsys_cmd_code
      rsp->header.cmd_version = req_all->header.cmd_version;
      rsp->header.subcmd = req_all->header.subcmd;
      rsp->header.id.preset_id = req_all->header.id.preset_id;
      rsp->config = req_all->config;
      rsp->error_code = 0;
    }
    return (void *)rsp;
  }
  
  else 
  {
    if(send_rsp)
    {
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len);
    }
    else
    {
      return NULL;
    }
  }

} /* diagdiag_ext_event_report_ctrl */


/*===========================================================================

FUNCTION DIAG_SWITCH_EVENT_PRESET_MASK

DESCRIPTION
  Whenever 'diag_cur_preset_id' is updated via command/response or control 
  packet to switch the current Stream 1 preset mask id, we need to call this
  function to actually switch the current preset event mask in use.
  
PARAMETERS
  None
  
RETURN VALUE
  None
  
===========================================================================*/
void diag_switch_event_preset_mask()
{
  diag_event_mask[DIAG_STREAM_1-1] = &diag_event_preset_mask[diag_cur_preset_id-1][0];
  
#if defined(DIAG_MP_MASTER)
  diag_event_mask_mstr[DIAG_STREAM_1-1] = &diag_event_mask_mstr_preset[diag_cur_preset_id-1];
  
  if( diag_event_mask_mstr[DIAG_STREAM_1-1]->status == DIAG_CTRL_MASK_ALL_ENABLED )
    event_control( 0xFF, EVENT_CONFIG_COMM_MASK );
  else if( diag_event_mask_mstr[DIAG_STREAM_1-1]->status == DIAG_CTRL_MASK_VALID )
    event_control( 0xFF, EVENT_CONFIG_COMM_MASK );
  else if( diag_event_mask_mstr[DIAG_STREAM_1-1]->status == DIAG_CTRL_MASK_ALL_DISABLED )
    event_control( 0, EVENT_CONFIG_COMM_MASK );
#else
  if( event_preset_status[diag_cur_preset_id-1] == DIAG_CTRL_MASK_ALL_ENABLED )
    event_control( 0xFF, EVENT_CONFIG_COMM_MASK );
  else if( event_preset_status[diag_cur_preset_id-1] == DIAG_CTRL_MASK_VALID )
    event_control( 0xFF, EVENT_CONFIG_COMM_MASK );
  else if( event_preset_status[diag_cur_preset_id-1] == DIAG_CTRL_MASK_ALL_DISABLED )
    event_control( 0, EVENT_CONFIG_COMM_MASK );
#endif

  (void)diag_client_maskchange(DIAG_EVENTSVC_MASK_CHANGE);
  
} /* diag_switch_event_preset_mask */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_SEND_EVENT_MASK

DESCRIPTION
  Composes a CTRL msg to send an event mask update to specified Slave Diag.

PARAMETERS
  port_type - Port type (DIAGCOMM_PORT_SIO/DIAGCOMM_PORT_SMD)
  port_num - Port num (DIAGCOMM_PORT_1, DIAGCOMM_PORT_2, ...)
  stream_id - Event stream to send
  preset_id - 
  send_now - If True, send CTRL msg immediately. 
             If False, aggregate until DSM is full before sending.
  
RETURN VALUE
  None
  
===========================================================================*/
/* On MP Master only */
#if defined(DIAG_MP_MASTER)
void diagcomm_ctrl_send_event_mask( diagcomm_port_type port_type, 
                                    diagcomm_enum_port_type port_num, 
                                    uint8 stream_id,
                                    uint8 preset_id,
                                    boolean send_now )
{
  diag_ctrl_msg_type ctrl_msg;
  boolean is_sent = FALSE;
  diag_event_mask_type * event_mask_mstr_ptr = NULL;
  
  MSG_4(MSG_SSID_DIAG, MSG_LEGACY_LOW, "send_event_mask[1] port_num=%d stream=%d preset_id=%d send=%d", 
        port_num, stream_id, preset_id, send_now);
        
  /* Sanity checks */
  ASSERT( (stream_id >= DIAG_MIN_STREAM_ID) && (stream_id <= DIAG_MAX_STREAM_ID) );
  ASSERT( (preset_id >= DIAG_MIN_PRESET_ID) && (preset_id <= DIAG_MAX_PRESET_ID) );

  /* Check if port has been opened */
  if( !diagcomm_io_ctrl_conn[port_type][port_num].connected || 
      !diagcomm_io_ctrl_conn[port_type][port_num].channel_connected )
    return;

  if( diagcomm_ctrl_mask_status(port_type, port_num, DIAG_UPDATE_MASK_EVENTS, stream_id, preset_id) )
    diagcomm_ctrl_clear_mask_status( port_type, port_num, DIAG_UPDATE_MASK_EVENTS, stream_id, preset_id );
  else
    return;

  /* Grab the right master event mask. */
  if( stream_id == DIAG_STREAM_1 )
    event_mask_mstr_ptr = &diag_event_mask_mstr_preset[preset_id-1];
  else if(stream_id == DIAG_STREAM_2 )
    event_mask_mstr_ptr = &diag_event_mask_mstr_dci[DIAG_DCI_MASK_1-1];

  /* Compose DIAG_CTRL_MSG_EVENT_MASK_V3 packet */
  ctrl_msg.cmd_type = DIAG_CTRL_MSG_EVENT_MASK_V3;

  ctrl_msg.data.ctrl_msg_event_mask_v3.stream_id = stream_id;
  ctrl_msg.data.ctrl_msg_event_mask_v3.preset_id = preset_id;
  ctrl_msg.data.ctrl_msg_event_mask_v3.status = event_mask_mstr_ptr->status;
  ctrl_msg.data.ctrl_msg_event_mask_v3.event_config = event_config;
  ctrl_msg.data.ctrl_msg_event_mask_v3.event_mask_size = event_mask_mstr_ptr->mask_len;

  MSG_4(MSG_SSID_DIAG, MSG_LEGACY_LOW, "send_event_mask[2] port_num=%d stream=%d preset_id=%d status=%d", 
        port_num, stream_id, preset_id, event_mask_mstr_ptr->status);
        
  if( (event_mask_mstr_ptr->status == DIAG_CTRL_MASK_ALL_DISABLED) ||
      (event_mask_mstr_ptr->status == DIAG_CTRL_MASK_ALL_ENABLED) )
  {
    ctrl_msg.data_len = sizeof(diag_ctrl_msg_event_mask_v3_type);
    ctrl_msg.data.ctrl_msg_event_mask_v3.event_mask_size = 0;

    (void)diagcomm_ctrl_send_mult_pkt( (PACK(void *))&ctrl_msg, 
                                       DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_event_mask_v3_type), 
                                       NULL, 0,
                                       port_type, port_num, send_now, &is_sent );
  }
  else if( event_mask_mstr_ptr->status == DIAG_CTRL_MASK_VALID )
  {
    ctrl_msg.data_len = sizeof(diag_ctrl_msg_event_mask_v3_type) + 
                        event_mask_mstr_ptr->mask_len;

    (void)diagcomm_ctrl_send_mult_pkt( (PACK(void *))&ctrl_msg, 
                                       DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_event_mask_v3_type), 
                                       (PACK(void *))&event_mask_mstr_ptr->mask[0],
                                       event_mask_mstr_ptr->mask_len,
                                       port_type, port_num, send_now, &is_sent );
  }

} /* diagcomm_ctrl_send_event_mask */
#endif /* DIAG_MP_MASTER */


/*===========================================================================

FUNCTION DIAG_CTRL_UPDATE_EVENT_MASK

DESCRIPTION
  Given a new event mask, updates the local copy of the event mask with the
  new mask.

PARAMETERS
  diag_ctrl_msg_type * ctrl_msg
  
RETURN VALUE
  None
  
===========================================================================*/
/* On MP Slave only */
#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
void diag_ctrl_update_event_mask( diag_ctrl_msg_type * ctrl_msg )
{  
  uint8 * mask_ptr = NULL;
  uint32 i, mask_len, min_mask_len = 0;
  uint8 config, stream_id, status = 0;
 
  if( (ctrl_msg == NULL) || (ctrl_msg->cmd_type != DIAG_CTRL_MSG_EVENT_MASK_V2) )
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_event_mask Sanity check failed");
    return;
  }
  
  stream_id = ctrl_msg->data.ctrl_msg_event_mask.stream_id;
  mask_len = ctrl_msg->data.ctrl_msg_event_mask.event_mask_size;
  config = ctrl_msg->data.ctrl_msg_event_mask.event_config;
  status = ctrl_msg->data.ctrl_msg_event_mask.status;
  
  MSG_3(MSG_SSID_DIAG, MSG_LEGACY_LOW, "update_event_mask stream=%d status=%d len=%d", 
        stream_id, status, mask_len);
  
  if( (stream_id > DIAG_MAX_STREAM_ID) || (stream_id < DIAG_MIN_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_event_mask: Invalid stream_id %d", stream_id);
    return;
  }

  if( status == DIAG_CTRL_MASK_ALL_DISABLED )
  {
    for( i = 0; i < EVENT_MASK_SIZE; i++ )
      diag_event_mask[stream_id - 1][i] = 0;
      
    if( stream_id == DIAG_STREAM_1 )
      event_preset_status[diag_cur_preset_id-1] = DIAG_CTRL_MASK_ALL_DISABLED;
  }
  else if( status == DIAG_CTRL_MASK_ALL_ENABLED )
  {
    for( i = 0; i < EVENT_MASK_SIZE; i++ )
      diag_event_mask[stream_id - 1][i] = 0xFF;
      
    if( stream_id == DIAG_STREAM_1 )
      event_preset_status[diag_cur_preset_id-1] = DIAG_CTRL_MASK_ALL_ENABLED;
  }
  else if( status == DIAG_CTRL_MASK_VALID )
  {
    if( ctrl_msg->data_len != (sizeof(diag_ctrl_msg_event_mask_type) + mask_len) )
    {
      MSG_2( MSG_SSID_DIAG, MSG_LEGACY_ERROR, 
             "diag_ctrl_update_event_mask: Invalid lengths %d != %d", 
             ctrl_msg->data_len, sizeof(diag_ctrl_msg_event_mask_type) + mask_len );
      return;
    }
    
    min_mask_len = MIN(mask_len, EVENT_MASK_SIZE);
    mask_ptr = ((uint8 *)ctrl_msg) + DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_event_mask_type);
    memscpy( (void*)diag_event_mask[stream_id-1],EVENT_MASK_SIZE, mask_ptr, min_mask_len );
    
    if( stream_id == DIAG_STREAM_1 )
      event_preset_status[diag_cur_preset_id-1] = DIAG_CTRL_MASK_VALID;
  }
  
  
  if( stream_id == DIAG_STREAM_1 )
  {
    if( (event_preset_status[diag_cur_preset_id-1] == DIAG_CTRL_MASK_ALL_ENABLED) ||
        (event_preset_status[diag_cur_preset_id-1] == DIAG_CTRL_MASK_VALID) )
    {
      event_control( 0xFF, EVENT_CONFIG_COMM_MASK );
    }
    else
    {
      event_control( 0, EVENT_CONFIG_COMM_MASK );
    }
  }
  else if( stream_id == DIAG_STREAM_2 )
  {
    event_control_sec( config, EVENT_CONFIG_COMM_MASK );  
  }
  
    (void)diag_client_maskchange(DIAG_EVENTSVC_MASK_CHANGE);

} /* diag_ctrl_update_event_mask */


/*===========================================================================

FUNCTION DIAG_CTRL_UPDATE_EVENT_PRESET_MASK
DESCRIPTION
  Given a new event mask, updates the local copy of the event mask with the
  new mask.

PARAMETERS
  diag_ctrl_msg_type * ctrl_msg
  
RETURN VALUE
  None
  
===========================================================================*/
void diag_ctrl_update_event_preset_mask( diag_ctrl_msg_type * ctrl_msg )
{  
  uint8 * mask_ptr = NULL;
  uint32 i, mask_len, min_mask_len = 0;
  uint8 config, stream_id, preset_id, status = 0;
  unsigned char * event_mask_ptr = NULL;
  
  if( (ctrl_msg == NULL) || (ctrl_msg->cmd_type != DIAG_CTRL_MSG_EVENT_MASK_V3) )
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_event_preset_mask Sanity check failed");
    return;
  }
    
  stream_id = ctrl_msg->data.ctrl_msg_event_mask_v3.stream_id;
  preset_id = ctrl_msg->data.ctrl_msg_event_mask_v3.preset_id;
  mask_len = ctrl_msg->data.ctrl_msg_event_mask_v3.event_mask_size;
  config = ctrl_msg->data.ctrl_msg_event_mask_v3.event_config;
  status = ctrl_msg->data.ctrl_msg_event_mask_v3.status;
  
  MSG_5(MSG_SSID_DIAG, MSG_LEGACY_LOW, "update_event_preset_mask procid=%d stream=%d preset=%d status=%d len=%d", 
        DIAG_MY_PROC_ID, stream_id, preset_id, status, mask_len);
  
  if( stream_id == DIAG_STREAM_1 )
  {
    if( (preset_id > 0) && (preset_id <= DIAG_MAX_PRESET_ID) )
      event_mask_ptr = &diag_event_preset_mask[preset_id-1][0];
    else
      return;
  }
  else if( stream_id == DIAG_STREAM_2 )
  {
    event_mask_ptr = &diag_event_dci_mask[DIAG_DCI_MASK_1-1][0];
  }
  else
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_event_preset_mask: Invalid stream_id %d", stream_id);
    return;
  }
  
  if( status == DIAG_CTRL_MASK_ALL_DISABLED )
  {
    for( i = 0; i < EVENT_MASK_SIZE; i++ )
      event_mask_ptr[i] = 0;
      
    if( stream_id == DIAG_STREAM_1 )
      event_preset_status[preset_id-1] = DIAG_CTRL_MASK_ALL_DISABLED;
  }
  else if( status == DIAG_CTRL_MASK_ALL_ENABLED )
  {
    for( i = 0; i < EVENT_MASK_SIZE; i++ )
      event_mask_ptr[i] = 0xFF;
      
    if( stream_id == DIAG_STREAM_1 )
      event_preset_status[preset_id-1] = DIAG_CTRL_MASK_ALL_ENABLED;
  }
  else if( status == DIAG_CTRL_MASK_VALID )
  {
    if( ctrl_msg->data_len != (sizeof(diag_ctrl_msg_event_mask_v3_type) + mask_len) )
    {
      MSG_2( MSG_SSID_DIAG, MSG_LEGACY_ERROR, 
             "diag_ctrl_update_event_preset_mask: Invalid lengths %d != %d", 
             ctrl_msg->data_len, sizeof(diag_ctrl_msg_event_mask_v3_type) + mask_len );
      return;
    }
    
    min_mask_len = MIN(mask_len, EVENT_MASK_SIZE);
    mask_ptr = ((uint8 *)ctrl_msg) + DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_event_mask_v3_type);
    memscpy( (void*)event_mask_ptr, EVENT_MASK_SIZE, mask_ptr, min_mask_len );
    
    if( stream_id == DIAG_STREAM_1 )
      event_preset_status[preset_id-1] = DIAG_CTRL_MASK_VALID;
  }
  
  if( stream_id == DIAG_STREAM_1 )
  {
    if( (event_preset_status[diag_cur_preset_id-1] == DIAG_CTRL_MASK_ALL_ENABLED) ||
        (event_preset_status[diag_cur_preset_id-1] == DIAG_CTRL_MASK_VALID) )
    {
      event_control( 0xFF, EVENT_CONFIG_COMM_MASK );
    }
    else
    {
      event_control( 0, EVENT_CONFIG_COMM_MASK );
    }
  }
  else if( stream_id == DIAG_STREAM_2 )
  {
    event_control_sec( config, EVENT_CONFIG_COMM_MASK );
  }
  
  (void)diag_client_maskchange(DIAG_EVENTSVC_MASK_CHANGE);
  
} /* diag_ctrl_update_event_preset_mask */

/*===========================================================================

FUNCTION DIAG_CTRL_SEND_EVENT_ID_CTRL_PKT

DESCRIPTION
  Sends last Event Id to Master Diag for build mask centralization support.
  
PARAMETERS
  None
 
RETURN VALUE
  None
  
===========================================================================*/
void diag_ctrl_send_event_id_ctrl_pkt(void)
{
  diag_ctrl_msg_type ctrl_msg;
  boolean is_sent = FALSE;
  uint32 bytes_written = 0;

  if(bt_event_id_fwd_status == DIAG_CTRL_INFO_FWD_STATUS_NOT_SENT) 
  {
		/* Compose DIAG_CTRL_EVENT_LAST_ID packet */
		ctrl_msg.cmd_type = DIAG_CTRL_BT_EVENT_ID;
		ctrl_msg.data_len = sizeof(diag_ctrl_event_last_id_type);
		ctrl_msg.data.ctrl_msg_bt_event_last_id.version = DIAG_CTRL_MSG_BT_MASK_VER;
		ctrl_msg.data.ctrl_msg_bt_event_last_id.event_last_id = EVENT_LAST_ID;

		bytes_written = diagcomm_ctrl_send_mult_pkt( (void *)&ctrl_msg, DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_event_last_id_type), 
														NULL, 0, DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1, TRUE, &is_sent );

		if(bytes_written == 0)
			 return;
		else if(is_sent == TRUE)
			 bt_event_id_fwd_status = DIAG_CTRL_INFO_FWD_STATUS_SENT;
		else /* bytes_written > 0 && is_sent == FALSE */
			 bt_event_id_fwd_status = DIAG_CTRL_INFO_FWD_STATUS_QUEUED;
  }
}
#endif /* DIAG_MP && !DIAG_MP_MASTER */


/*===========================================================================
FUNCTION EVENT_MASK_ENABLED

DESCRIPTION
  This function returns a boolean indicating if the specified event id is enabled.
  The event id is passed as an argument to the function for which the
  event masks needs to be checked.

===========================================================================*/
boolean event_mask_enabled (
  const unsigned char *mask_ptr,
  event_id_enum_type event_id
)
{
  unsigned int mask;
  boolean enabled = FALSE;
  if (event_id <= EVENT_LAST_ID)
  {
    mask = ( 1 << (event_id & 0x07) );
    if(mask_ptr[event_id/8] & mask)
    {
      enabled  = TRUE;
    }
  }
  else
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Invalid event id given %d", event_id);
  }
  return enabled;
} /* event_mask_enabled */

/*===========================================================================
FUNCTION EVENT_SSM_SET_PRE_MASKS

DESCRIPTION
    This sets the diag_event_mask_ssm mask for the event ids passed in
    the input array. These events are set during boot up and will be cleared
	when SSM permissions apply.

    EVENT_MAX_ID indicates the end of the array.

WARNING:
	Ensure EVENT_MAX_ID is present as last element in the event_ids[] array,
	else it ends up in infinite loop causing starvation.

===========================================================================*/

#ifdef FEATURE_SSM
void event_ssm_set_pre_masks(event_id_enum_type event_ids[])
{
#ifdef DIAG_SSM_LOG_EVENT_WHITELIST
    uint32 i=0;
    uint32 index = 0;
    /*set the mask for events in the array provided. Parse until EVENT_MAX_ID is
    reached */
    while(event_ids[i] != EVENT_MAX_ID)
    {
        index = event_ids[i]/ 8;
        if (index >= EVENT_MASK_SIZE)
        {
            MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Attempting to set mask for Invalid event ID %d", event_ids[i]);
            i++;
            continue;
        }

        diag_event_mask_ssm[index] |= (0x01 << ((event_ids[i]) & 0x07));
        i++;
    }
#endif
}
#endif

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         Diagnostic Task

General Description
  The diagnostic task supports the interface between the mobile software
  tasks and the diagnostic monitor. The diagnostic task responds to
  packets sent by the Diagnostic Monitor in order to support software
  debugging, data logging and displaying the temporal analyzer.

Copyright (c) 1992-2015 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diag.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/07/16   ph      Diag log packet support for ULog
06/18/15   sa      Added support for time sync feature.
07/20/15   ph      Fixed compiler warnings.
01/19/15   is      Check FTM NV item before accessing dynamic memory for buffering mode
12/17/14   ph      Mainlined the feature DIAG_C_STRING.
01/08/15   ph      Fix the merge error with dymanic heap feature.
11/25/14   ph      Mainlined the feature DIAG_CONSUMER_API.
11/18/14   ph      Removed the reference to err_set_auto_action().
10/30/14   xy      Fixed issue that modem not able to send mask centralization 
                   control packets to apps
10/28/14   ph      Addition of signal mask for DIAG_SMDL_CMD_READ_SIG in all
                   diagbuf_drain calls.
10/17/14   ph      Send Build mask centralization control packets only if apps
                   supports the feature.
10/09/14   ph      Added control packet to send the memory size used in
                   peripheral buffering.
08/26/14   is      Replace extra mem calls with dynamic memory calls 
09/01/14   ph      Added mutex around diag_internal_sigs to resolve concurrency
                   issues with feature mask update missing.
06/19/14   sa      Added support to send an event to debug diag drops.
04/01/14   rs      Added support for build mask centralization.
02/18/14   rh      Added flush internal signal to make diagbuf_flush() calls.
01/30/14   ph      Removed the usage of macro DIAG_TASK_HANDLE().
01/22/14   sr      Fixed the issue where cmd wmq is used without being
                   initialized in Diag standalone case.
08/01/14   sa      Updated setting of DIAG_SMDL_WRTIE_SIG only if items are pending
                   in the SMDL based channel (command channel as of now)
12/31/13   sa      Removed unused variables in the diag idle processing APIs.
                   Release the mutex as many times as acquired in
                   diag_idle_processing_unregister().
01/08/13   rs      Moved diagcomm_cmd_init outside of featurization.
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
10/14/13   sr      Added support for extended event, log and F3 pkts
09/20/13   tbg     Added support for Diag over STM
09/18/13   sa      Updated the return type of diag_searchlist_delete() function
09/18/13   sa      Clean up functions to free the listener node marked as free and
                   the stale list of listeners marked as free.
09/09/13   sm      Reduced duplicate code with function
                   diag_alloc_check_common(). Contains common code used by
                   diag_alloc_check(), diag_rsp_alloc_check(),
                   diag_event_alloc_check(), and diag_event_alloc_check_sec().
12/26/13   sa      Updated the diag mask change notification signal name.
12/19/13   rs      Fixed LLVM compilation warnings.
11/28/13   ph      Fixed the bug where diag_sleep_state won't be updated when diag
                   is switched to NRT mode.
09/25/13   xy      Handle sending out feature mask on control channel first before
                   sending out cmd reg tables in diag_handle_internal_sigs()
08/31/13   is      Support for peripheral buffering mode
08/29/13   sr      Featurized the ctrl_init and cmd_init for SIO
08/22/13   sr      Fixed the concurrency issue with Diag and SSM.
08/09/13   is      Clear bootup event masks explicitly when Diag is up
07/23/13   ph      Fixed fuzzing test crash in diag_task().
07/24/13   ph      Replaced the assert in diag_alloc_check with appropriate check.
06/28/13   ph      Fixed improper check found in time out mechanism in diag_block().
06/18/13   sr      Added DIAG_SMDL_WRITE_SIG to diagbuf_drain() while handling
                   DIAG_TX_SIG
05/18/13   sr      Added support for diag mask change notification for
                   registered users
05/16/13   sm      Removed call to diag_gpio_init()
06/17/13   sr      Support for whitelisting of diag cmds, events and logs
05/14/13   sr      Added mutex check around diag_idl_proc_tbl to prevent
                   concurrency issues
05/11/13   is      Move drain timer from client conext to diag context
04/30/13   sr      Removed SIO_RX pool. Cmd req/rsp will now use CMD_SIO_TX
                   and CMD_SIO_RX pool even in backwards compatibility case
04/22/13   ph      converted diag_listeners heap usage to system heap.
04/15/13   sr      Support for WCDMA to register with Diag to obtain the
                   frame num
04/09/13   sr      Fixed the compiler warnings
04/09/13   sa      Support to wake up diag for idle processing
04/09/13   sa      Support to set the diag tx signal when changed
                   to real time mode
04/03/13   sr      Replace memcpy() and memmove() with memscpy() and memsmove()
03/13/13   rh      diag_handle_sigs() now matches return condition first
02/15/13   sr      Moved to new Dog heartbeat implementation
01/03/13   sg      Command Response Channel separation
02/12/13   sr      Freed the log buffer that was allocated to SSM
01/16/13   rh      Combined DIAG_TX_WMQ_SIG and DIAG_SMDL_WRITE_SIG into one
                   handler because they performed almost the same purpose.
01/09/13   ph      Clear DIAG_TOUT_TIMER_SIG signal used after suspending
01/08/13   rh      Init diag_event_stale_timer_pending_sec to FALSE in diag_init()
12/14/12   is      Add diag_drain_timer_pending check in diag_clr_drain_timer()
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush.
12/07/12   is      Send mask updates in a loop
11/27/12   rh      Renamed diag_blocking_malloc to diag_rsp_malloc to better
                   reflect its purpose.  Gave diag_rsp_malloc its own heap.
11/15/12   is      Support for preset masks
11/08/12   sr      Support for DCI Override feature(OCH) and
                   On Board Diagnostics(OBD)- Secure Sim feature
01/10/12   sr      Filtering of logs/events based on ssm permission file
10/18/12   rh      Removed variable use_central_routing
10/11/12   rh      New signal DIAG_TX_WMQ_SIG for t-put improvements with SMDL
10/03/12   rh      Added a means to query Diag for whether it is active.
08/31/12   ra      Use accessor macros instead of accessing private task
                   info directly; accomodated changes for User space
                   task priority changes
08/24/12   ra      Support for querying Command Registration Tables
08/10/12   sa      Ensure Master Diag always opens ports for LPASS
08/10/12   sa      Mainlining DIAG_CENTRAL_ROUTING
07/12/12   ra      When Diag Blocks, keep kicking the watchdog until
                   DIAG_BLOCK_SIG triggers
07/11/12   ra      Removed Deprecated Sleep API; now using NPA
07/10/12   ra      Ensure RCINIT handshake completes before starting Diag
06/29/12   is      Support new SSM error codes
05/03/12   rs      Moved delayed response from diag buffer to AMSS heap.
03/26/12   hvm     Extended Event/Log Listener APIs added
01/16/12   tbg     Added support for Secure Services Module
01/10/12   sr      Filtering of logs/events based on ssm permission file
11/09/11   hm      Fixed compiler warnings and enable DCI traffic even when
                   QPST is not connected
11/07/11   is      Fixed compiler warnings
11/02/11   is      Remove banned function std_strlcpy()
10/25/11   is      Remove duplicate calls to diagbuf_init()
10/14/11   hm      Diag consumer API - modem side changes
10/10/11   sg      Remove depricated function strncpy on q6
09/28/11   is      Support dynamic sleep voting and non-deferrable Diag timers
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
08/09/11   is      Changed debug msg to match SIL signature for blacklisting
08/08/11   wjg     Added signal to handle smdl_event_read in diag context
06/14/11   hm      Migrating to REX opaque TCB APIs
05/27/11   sg      Check the size of the diag request packet
                   when received on slave processor to not
                   exceed the size of diag_req_inbound
05/05/11   is      Remove support for PLT registrations
04/28/11   is      Update featurizations for central routing
04/27/11   is      Resolve compiler warnings
04/18/11   is      Ensure CTRL msg sizes are constant
03/29/11   sg      Central Routing changes for Q6
03/04/11   is      Support for Diag over SMD-Lite
03/02/11   hm      Support SIO control channel on single processor
                   for dual-mask feature
03/01/11   is      Migrate to diag_strlcpy()
02/24/11   hm      Fixed compiler warnings
02/23/11   vs      Moved rdm_register calls after diag task ready for 9k
02/04/11   vs      Added call to diag_gpio_init
12/20/10   mad     Changes for dual mask diag:open SIO control port for APPS
01/24/10   is      Define chk_security_code_ptr[] when Dload not present
10/14/10   mad     Closing SIO control port on DIAG_COMM_CLOSE_SIG
10/25/10   is      Diag feature flag cleanup
10/20/10   is      Bypass RDM to set default to UART1 for 8960 Apps only bringup
09/28/10   mad     dynamically configurable drain timer and sleep vote
09/28/10   mad     closing control port on DIAG_COMM_CLOSE_SIG
09/28/10   mad     opening SIO port unconditionally, fixed compile error regarding
                   DIAGCOMM_SMD_1 and DIAGCOMM_SMD_2
09/27/10   is      New control channel and central routing features
08/10/10   sg      Close SMD Port when the SIO port on apps is closed
09/16/10   mad     Explicitly setting synch. event to signal diag clients
09/16/10   vs      Added diag task's registration for dog
09/01/10   sg      Setting txsignal in diag_idle_processing_register()
                   to wake up diag task to do idle processing when there is no traffic
07/12/10   vs      Removed mutexes from diag_searchlist_search_all
07/06/10   sg      Fixed compiler warnings
07/23/10   mad     Moved diagpkt_check_validity_request_ondevice() to this file
07/23/10   mad     Changes for dynamic on-device mask processing for windows devices:
                   Removed functions diag_ondevice_process_inbound() and
                   checkpacketvalidity()
07/20/10   sg      Added mutexes to osal_set_timer and osal_reset_timer to prevent
                   deletion of qtimer handle which is already deleted in case of
                   drain timer ,stale timer which are set in client's context
07/15/10   sg      Cleaning up of setting drain timer during diag task initialization
                   since we set the signal to drain  the buf when we get first rx packet
04/06/10   sg      Merging back DCM WM and RTOS files
03/25/10   mad     Added support for on-device logging
04/02/10   sg      Merging memory leak fix to AMSS side
03/30/10   vs      Added featurization around clk_regime calls
03/26/10   JV      Eliminating the event_pending timer and event_pending_q
03/24/10   sg      Wait on DIAG_RX_RSP_SIG when draining the logs and messages
                   in diagbuf_drain
03/23/10   sg      EVENT_STALE_TIMER  is set when the mask is zero to drain
                   the events that are already present in the diag_heap
03/18/10   sg      Diag Votes For Sleep in DIAG_RX_RSP_SIG
03/06/10   as      Added support for processing Ondevice Logging.
03/05/10   JV      Set the TX sig to start draining when we receive teh first RX pkt.
02/23/10   as      Fixed indentation and limited the scope of efs_stat fail case.
02/10/10   vs      Removed DIAG_MP for creating of drain_timer
01/20/10   JV      Added a new signal that when set, drains events
01/18/10   JV      DO not initialize the 2 SMD pools for single processor targets.
12/28/09   ps      Fixed Klocwork errors & compiler warnings.
12/10/09   sg      Added flow control mechanism for DSM_DIAG_SIO_TX_ITEM_POOL
                   at item level granularity
11/30/09   ps      Fixed compiler warnings.
11/17/09   JV      Wait on DIAG_RX_SIG when diagbuf_drain() is called while
                   processing the DIAG_DRAIN_TIMER_SIG. Extension of the previous
                   bug fix.
10/28/09   JV      Include diag_cfg.h to get the DIAG_HEAP_SIZE
10/21/09   JV      Fix for bug where we the mask passed to diagbuf_drain()
                   to wait on did not contain the DIAG_RX_SIG ORed in.
10/16/09   JV      FIxed bug in opening diagcomm_smd port.
09/22/09   vs      Use OSAL for all rex calls on qdsp6.
09/09/09   JV      Changed signature of osal mutex calls.
09/02/09   mad     merged handling of DIAG_TX_SIG and DIAG_TX_SLAVE_SIG.
                   This ensures that TX_SIG is handled, when slave traffic is
                   queued up.
09/01/09   JV      Changes to migrate from EFS1 to EFS2 APIs. These EFS2 calls
                   are synchronous, so there is no need to wait on any signal.
08/28/09   JV      Fix to enable boot-up messages. Register toolsdiag and diagdiag
                   sooner. Done in diag_init().
07/14/09   vs      diagbuf_drain is also called when drain_timer expires.
07/13/09   vs      Added DIAG_DRAIN_TIMER_SIG back in diag_handle_sigs
                    drain timer is now set only in diabuf_commit if not
08/06/09   sg      Added support for DSM Chaining in diag_get_rx_pkt().
08/05/09   JV      task.h is now included in osal.
08/05/09   JV      Removed the CUST_H featurization around the inclusion of
                   customer.h.
07/31/09   JV      Removed warnings
07/31/09   JV      Merged Q6 diag code back to mainline
07/31/09   JV      Brought back FEATURE_DIAG_NV
07/23/09   JV      Changed the signature of diag_wait() to generic types from osal
                   types so that diagi.h can be included by clients without us
                   making osal.h a public header file.
07/10/09   as      Decoupled FEATURE_DIAG_NON_STREAMING & removed support  for
                   FEATURE_DIAG_SPC_TIMEOUT
06/04/09   as      Reverted change from EFS2 to EFS1.
05/12/09   JV      Introduced the OS abstraction layer for rex.
05/11/09   as      Fixed forward traffic issue when Diag port is NULL
05/07/09   vk      chages made to move from EFS1 to EFS2 apis
05/07/09   vk      changed includes from external to internal headers
04/22/09   mad     modified usage of diagbuf_tx_sleep_threshold_set() and
                   diagbuf_tx_sleep_time_set()
04/21/09   ps        branch collapse of 6k and 7k DIAG
03/26/09   ps     Merged diag_wait with diag_handle_sigs.
03/26/09   ps      changes to fix CR 158777
03/06/09   vg      Added call to initialize the diagpkt_resp_q
                   The call is diagpkt_rsp_q_init()
03/05/09   vg      Removed the mutex release on suspend, pause, timeout
03/04/09   vg      Merged changes that mad made for forward packets on Diag
                   task
03/02/09   is      Fix featurizations to keep modem/apps procs in sync in using
                   new vs old sleep API
02/18/09   is      Featurize sleep vote filtering with SLEEP_DIAG_OKTS_SIG to
                   indicate
                   support for new sleep API
02/01/09   vg      Made trace buffer mutex consistent between event's and
                   F3 msg. (Updated msg_init to be early then event_init)
01/19/09   is      Switch to new sleep voting interface
01/12/09   as      Fixed thread synchronization in sleep vote function.
01/08/09   as      Fixed deadlock and concurrency control issues.
01/07/09   is      Do not invoke sleep and clock regime code if sleep vote is
                   not changing
10/3/08    VG      Changed PACKED to PACK() fixed issue casting uint32 to int.
                   This caused incorrect behavior with gcc compiler where int is
           defined as 32 bit vs. 16 bits on arm compiler.
08/21/08   JV      Moved the check for DIAG_COMM_CLOSE_SIG, DIAG_COMM_OPEN_FWD_SIG
                   and DIAG_COMM_OPEN_SIG from diag_handle_sigs() to diag_wait().
02/05/08   pc      Added support for dynamic threshold and sleep values
01/20/08   pc      Added DIAG_EVENT_PEND_TIMER_SIG to busy_mask in diag_wait()
12/16/07   pc      Added support for diag_dtr_enable and diag_dtr_changed_cb.
09/25/07   ms      Removing diagdsm_init() call for WinMob. Removing dsm_init().
05/17/07   jc      Proxy by tv.  Revise checking of REX signals.
01/08/07   tbg     Added support for new err_auto_action API
01/05/07   as      Featurized drian diag timer for multi processor code.
12/21/06   as      Moved RX_SIG and Drain SIG above TX Sig.
12/15/06   as      Fixed compiler warnings.
12/04/06   as      Added  support to drain diag buffer based on a timer.
06/29/06   giri    Stubbing the panic mode implementation for the core services
                   build. The stubs is defined in diag_stub.c file.
05/03/06   as      Feature FEATURE_DISABLE_DLOAD_ENTRY_ON_ERROR_FATAL disables
                   going into downloader on error fatal.
10/31/05   as      Fixed lint errors.
07/25/05   as      Added TRAMP_SLEEP_TIMER_TICK_ISR in diag panic handler.
07/05/05   sl      Added support for SPC_TIMEOUT to double the timeout value
                   on consecutive incorrect SPCs.
06/27/05   as      FEATURE_L4 turns off diag panic mode.functionality.
03/30/05   sl      Added support for SPC_UNLOCK_TTL to unlock/lock the sp_state
03/14/05   as      Moved diagpkt_init_manual_dispatch_table() call to diag_task
02/28/05   as      Replaced rex critical section with intlocks.
02/15/05   as      Added FEATURE_DIAG_DEBUG_6275. This temporary feature will
                   be used until Raven code is stabilized.
02/10/05   as      Added FEATURE_DEBUG_DIAG_HEAP to debug memory corruption.
01/27/05   as      Restored check for blocked diag heap in diag_malloc().
12/14/04   sl      Additionaly diag_timeout() resets the SIO stream ID if
                   FEATURE_DIAG_MULTI_PORTS is defined.
04/06/04   tbg     Added support for FEATURE_ERR_EXTEDED_STORE
09/23/03   gr      Added code to not iterate through the diag heap when out
                   of memory. Should improve efficiency in certain situations.
09/08/03   as      Changed diag_tx_notify to call rex timer only when
                   interrupts are locked.
09/03/03   as      Removed diag_tx_notify_timer
12/12/02   lad     Added panic mode powerdown option (NV).
12/02/02   lad     Removed inactivity timer.
11/21/02   lad     Mainlined diag heap.
09/13/02   lad     Added config save to filestream mode.
07/30/02   as      Added inactivity timer to detect external device
                   disconnect.
07/28/02   lad     Moved services initializations to diag_init().
06/21/02   lad     Added the filestream feature to capture data generated
                   prior to tool establishing connection.
                   Restructured processing of inbound data for reuse w/
                   filestream feature.
08/20/01   jal     Changes to support more Diag packets.  Handle online/
                   offline mode.  Jump to downloader added.  Added accessor
                   functions for service programming lock state.
06/27/01   lad     Changed diag_hdlc_decode to take signed length to
                   detect invalid input.
                   Fixed error in handling the RX of a packet that is too big
04/06/01   lad     TASK_ signals were replaced with DIAG_TASK_xxx signals to
                   remove dependency on task.h.
                   Chagned max_req_pkt_len in diag_task to be
                   DIAG_MAX_RX_PKT_SIZ which is defined in diagtune.h.
                   Changed instance of diag_start_task() to
                   diag_ready_start_task().
                   Changed diag_do_escaping and diag_hdlc_decode to not pass
                   ASYNC control chars as params (use constants instead).  This
                   makes the function call cheaper and optimizes the routine
                   slightly.
                   Featurized diagpkt_refresh_cache() call under
                   FEATURE_DIAG_CACHE.
02/23/01   lad     Rearchitected the diagnostics subsystem to be a service
                   rather than a module.  All coupling to targets has been
                   moved to target-specific implementations.  The only coupling
                   that remains are common services and REX.
                   Old history comments have been removed since most of them
                   are no longer applicable.

===========================================================================*/

#include "core_variation.h"
#include "comdef.h"

#include <string.h>

#include "diag_v.h"
#include "diagbuf.h"
#include "diagbuf_v.h"
#include "diagcomm_v.h"
#include "diagdsm_v.h"
#include "diagi_v.h"
#include "diagpkt.h"
#include "diagdiag_v.h"
#include "diagtarget.h"
#include "diagtune.h"
#include "diag_cfg.h"
#include "osal.h"
#include "diag_fwd_v.h"
#include "err.h"
#include "event.h"
#include "log.h"
#include "msg.h"
#include "msgi.h"   /* For diagcomm_ctrl_send_msg_mask() */
#include "eventi.h" /* For diagcomm_ctrl_send_event_mask() */
#include "crc.h"
#include "memheap.h"
#include "queue.h"
#include "diagcomm_cmd.h"
#include "diagcomm_ctrl.h"
#include "diagcomm_smd.h"
#include "diagcomm_io.h"
#include "diag_mode_v.h" /* For diagcomm_tx_mode_init() */
#include "rcevt.h"
#include "time_svc.h" /* for time_get */
#include "ULogFront.h"

#if defined(FEATURE_DIAG_DYNAMIC_HEAP)
  #include "dynamic_heap.h"
#endif

#ifdef FEATURE_SSM
  #include "ssm.h"
  #include "diag_dci_auth_v.h"
  #include "diag_dci_auth.h"
#endif /* FEATURE_SSM */

#ifdef DIAG_SIO_SUPPORT
  #include "diagcomm_io_sio.h"
#endif

#ifdef DIAG_SMDL_SUPPORT
  #include "diagcomm_io_smdl.h"
#endif

#if defined (DIAG_QDSP6_APPS_PROC)
  #include "diagstub.h"
#else
  #include "assert.h"
#endif /* DIAG_QDSP6_APPS_PROC */

#include "stringl.h" /* For strlcpy() */

#if defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING)&& !defined (IMAGE_APPS_PROC)
#include "clkregim.h"
#endif

#if defined (FEATURE_DIAG_NV)
#include "nv.h"
#include "diagnv_v.h"
#endif

#if defined (DIAG_TOOLSDIAG_INIT)
#include "toolsdiag.h"
#endif

#ifdef FEATURE_WINCE
#error code not present
#endif  /* FEATURE_WINCE */

#if ((defined(DIAG_RUNTIME_DEVMAP))||(defined(FEATURE_DIAG_SWITCH)&&(!defined(FEATURE_WINCE))))
  #include "rdevmap.h"
#endif

#include "diag_stm.h"

/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */
osal_tcb_t *diag_task_tcb = NULL;
uint8 diag_cur_preset_id = DIAG_PRESET_MASK_1;
uint8 diag_max_presets[DIAG_MAX_STREAM_ID] = {DIAG_MAX_PRESET_ID, DIAG_MAX_DCI_ID};

static unsigned int diag_drain_timer_len = DIAG_DRAIN_TIMER_LEN; /* Set default value */
static unsigned int diag_enable_sleep_voting = 1; /* by default, enable diag sleep voting. Can be disabled by a ctrl message */

/*Used to check if the mask notification was sent out to the registered users or not*/
uint8 diag_mask_notify = 0;

/* ULog handle for DIAG Errors*/
ULogHandle diag_err_ulog_handle;
#define DIAG_ERR_ULOG_SIZE (1024*2)

/* Diag Drop Event Parameters */
uint8 diag_max_per_drop_threshold = DIAG_DROP_PER_THRESHOLD_MAX; /* Threshold Percentage Drop Count */
uint8 diag_max_per_step = DIAG_DROP_PER_STEP_MAX;               /* Step Percentage Drop Count */

uint8 diag_per_count_dropped = 0;  /* Percentage Drop Count */
uint8 diag_per_count_dropped_prev = 0;  /* To store the last Percentage drop at which event was reported */

/* Extern variables from diagbuf.c */
extern uint32 drop_count_log;
extern uint32 drop_count_f3;

extern uint32 alloc_count_log;
extern uint32 alloc_count_f3;

extern uint32 diagcomm_flow_ctrl_count[];
extern boolean bCountReset; /* from diagbuf.c To track if the log/drop counts are reset */

#ifdef FEATURE_WINCE
#error code not present
#endif /* FEATURE_WINCE */

/* Timer to wake Diagnostic task so that it can set
** its Watchdog Task Report Signal.
*/
#if defined (DIAG_DOG_SUPPORT)
osal_dog_report_type diag_dog_rpt_var = 0;
#endif

#if defined(FEATURE_DIAG_DYNAMIC_HEAP)
static uint8 * dsm_diag_buffering_item_array = NULL;
#endif

#ifdef FEATURE_SSM
static uint32 diag_ssm_id;
static uint32 diag_log_mask_ssm_id;
static uint32 diag_event_mask_ssm_id;
diag_mask_ssm_buffer_type *diag_log_mask_range = NULL;
diag_mask_ssm_buffer_type *diag_event_mask_range = NULL;
uint32 num_log_code_ssm = 0;
uint32 num_event_code_ssm = 0;
extern char * diag_ssm_cmd_fb;
extern char* diag_ssm_event_fb;
extern char* diag_ssm_log_fb;
#endif /* FEATURE_SSM */

extern boolean diag_real_time_mode; /*from diagpkt.c to track diag mode */
boolean diag_services_initialized = FALSE;
/* Timer to use when a bad SPC has been received.
*/
static osal_timer_t diag_tout_timer;

osal_timer_t diag_sio_tout_timer;

/*diag_mask_change_notify_timer callback function */
void diag_mask_notify_timer_cb(uint32 params);

/*Timer used to wait for a certain amount of time before sending
  out the notifications to users about diag mask change*/
osal_timer_t diag_mask_change_notify_timer;

/* Flag will be set to TRUE if diag times out on SIO callback
** Flag will be cleared if SIO callback is called
*/
static boolean diag_flush_timeout = FALSE;

/* Diag drain timer.  If this timer expires, the current buffer will
   be sent to SIO */

static osal_timer_t diag_drain_timer;

static osal_timer_t diag_rx_rsp_notify_timer;
static osal_timer_t diag_rx_delay_rsp_notify_timer;


#ifdef FEATURE_DIAG_SWITCH
  int diag_on_single_proc;
#endif

/* Request packets are sent from the DM to the mobile.  Request packet is
** reconstructed from pieces received from SIO.
*/
static uint8 diag_req_inbound[DIAG_MAX_RX_PKT_SIZ];

/* This timer is used to notify DIAG that a client has submitted data to TX.
   Using a timer instead of rex_set_sigs() prevents a context switch in case
   the client calls from within INTLOCK(). */
static osal_timer_t diag_tx_notify_timer;
boolean diag_tx_pending = TRUE;  /* Init to TRUE until init in diag_init(). */
boolean diag_drain_timer_pending = TRUE;
boolean diag_event_stale_timer_pending = TRUE;
boolean diag_event_stale_timer_pending_sec = TRUE;

static dword diag_internal_sigs = 0; /* Internal Diag sigs */

/*Mutex for diag_internal_sigs */
static osal_mutex_arg_t diag_internal_sig_mutex;

/* Diag state variables.
*/
static diag_security_type diag_security = DIAG_SEC_LOCKED;
static diag_sp_type diag_sp = DIAG_SPC_LOCKED;


/* Connectivity state.  If non-zero, something is using DIAG. */
uint32 diag_cx_state = DIAG_CX_NONE_S;

/* Definition of a constant that is the same as an unwritten security code.
*/
static const uint8 diag_no_security_code_c[] = { 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff
};

/*-------------------------------------------------------------------
** Dynamic Memory Support (heap)
**    Definitions and data to support rex_malloc/realloc/calloc/free
**-----------------------------------------------------------------*/
/* The DIAG heap in REX. */
static mem_heap_type diag_heap;
static mem_heap_type diag_rsp_heap;
static mem_heap_type diag_event_heap;
static mem_heap_type diag_event_sec_heap;

/* An upper bound on the sizes of the biggest available blocks in the
** diag heap.
*/
#define DIAG_FREEBLOCK_CACHE_SIZE 2
static unsigned int diag_freeblock_info[DIAG_FREEBLOCK_CACHE_SIZE];
static unsigned int diag_rsp_freeblock_info[DIAG_FREEBLOCK_CACHE_SIZE];

/* An upper bound on the sizes of the biggest available blocks in the
** diag event heap.
*/
#define DIAG_EVENT_FREEBLOCK_CACHE_SIZE 2
static unsigned int diag_event_freeblock_info[DIAG_EVENT_FREEBLOCK_CACHE_SIZE];

static unsigned int diag_event_freeblock_info_sec[DIAG_EVENT_FREEBLOCK_CACHE_SIZE];


#define DIAG_MAX_FREEBLOCK_CACHE_SIZE          \
  MAX(DIAG_FREEBLOCK_CACHE_SIZE,               \
      DIAG_EVENT_FREEBLOCK_CACHE_SIZE)


/* Is the freeblock cache for the diag heap valid or not?
*/
static int diag_heap_cache_valid = 0;
static int diag_rsp_heap_cache_valid = 0;

/* Is the freeblock cache for the diag event heap valid or not?
*/
static int diag_event_heap_cache_valid = 0;
static int diag_event_sec_heap_cache_valid = 0;

/* Minimum chunk size for the diag heap.
*/
static unsigned int diag_heap_chunk_size;
static unsigned int diag_rsp_heap_chunk_size;

/* Minimum chunk size for the diag event heap.
*/
static unsigned int diag_event_heap_chunk_size;
static unsigned int diag_event_sec_heap_chunk_size;


/* The DIAG memory buffer used by the heap manager. */
static char diag_heap_buf[DIAG_HEAP_SIZE];

/* The DIAG memory buffer used by the heap manager. */
static char diag_rsp_heap_buf[DIAG_RSP_HEAP_SIZE];

/* The DIAG memory buffer used by the heap manager. */
static char diag_event_heap_buf[DIAG_EVENT_HEAP_SIZE];

static char diag_event_sec_heap_buf[DIAG_EVENT_SEC_HEAP_SIZE];

/* Idle processing table.  If fp != NULL, the function will be called when
   the DIAG task is idle (nothing else to do). */
typedef struct
{
  diag_idle_processing_ftype fp;
  void *param;
}
diag_idle_proc_tbl_type;

diag_idle_proc_tbl_type diag_idle_proc_tbl[DIAG_IDLE_PROC_CNT];

#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)

osal_thread_attr_t diag_fwd_task_attr;

unsigned long long diag_fwd_stack[DIAG_FWD_STACK_SIZE];

void diag_fwd_task_init(void);

#endif

#if defined (FEATURE_SSM)
boolean is_dci_allowed = FALSE;
#endif
/* To track the time api to use */
diag_time_api_types diag_time_api_to_use = 0;

/* Variable related to the time API configuration */
boolean diag_time_initialized = FALSE;  /* Track if the diag time API is initialized */
static int diag_time_init_begin = FALSE;  

/* Function pointer to update the time API to be used */
uint8 (*diag_time_get)(qword);

//extern void diag_time_api_change_notify(int Svc);
osal_tcb_t diag_fwd_task_tcb;

static void diag_idle_processing (void);

extern void diagpkt_init_manual_dispatch_table (void);

extern void diagpkt_rsp_q_init(void);

extern void diagpkt_delay_rsp_q_init(void);

void diag_mask_change_notify(void);
extern diag_searchlist_type diag_f3_listeners;
extern diag_searchlist_type event_listeners;
extern diag_searchlist_type log_listeners;
/* Single Listeners Clean Up Function */
void diag_single_listeners_cleanup(diag_searchlist_type *list);
/* Stale Single Listeners List Clean Up Function*/
void diag_single_listener_heap_cleanup(diag_searchlist_type *list);
static int listener_cleanup_init= FALSE;
static int listener_heap_cleanup_init  = FALSE;
/* Array of listener arrays to be freed*/
uint32* diag_f3_listeners_heap_to_delete[((DIAG_HEAP_SIZE /(DIAG_SEARCHLIST_SIZE_INCR + sizeof(diag_searchlist_node_type)))/2)];
uint32* diag_log_listeners_heap_to_delete[((DIAG_HEAP_SIZE/(DIAG_SEARCHLIST_SIZE_INCR + sizeof(diag_searchlist_node_type)))/2)];
uint32* diag_event_listeners_heap_to_delete[((DIAG_HEAP_SIZE/(DIAG_SEARCHLIST_SIZE_INCR + sizeof(diag_searchlist_node_type)))/2)];
extern uint8 msg_enabled_mask;
extern diag_tx_mode_type diag_tx_mode[DIAG_MAX_STREAM_ID];
extern void diagpkt_delayed_tag_list_init(void);


#if defined (DIAG_FEATURE_EFS2)
/* ----------------------------------------------------------------------------
  "Diagnostics Filestream Mode"

  The diagnostic task filestream mode reads a file to serve as an input
  stream in lieu of an active comm layer.  The output of the diagnostic
  service is buffered in diag's output buffer manager (diagbuf.c).
  When a connection is activated via the comm layer and a valid inbound packet
  is detected, the buffered output is flushed to the comm layer,
  thus capturing all diagnostic data generated after initialization.

  This mode continues until one of the following conditions occur:

  (1) Communication is initiated via the comm layer (external device),
      triggering the filestream mode to flush its output buffer to the
      comm layer.
  (2) The inactivity timer expires, stopping this mode and turning off all
      diag services.

  NOTE: The diag task (diag.c) is responsible for detecting these conditions
  and calling this API appropriately.

  The input file is written by the external device during the prior instance
  of the target running.  The diag task calls diag_fs_init() at startup,
  detecting the presence of the input file.  Once the entire input file
  stream is processed, it is deleted.

Noteworthy features:

  This mode is enabled only if the input file is present.

  Once the input file is processed, it is deleted.  This is done to avoid
  the performance loss of running the diagnostic task every boot cycle.
  If this were ever enabled in a production phone, for example, it would
  run the diag service every time the user turns a phone on, impacting battery
  life.  It also prevents legacy tools from failing due to lack of support
  for streaming and thie feature.
---------------------------------------------------------------------------- */
#include "fs_public.h"

/* Diag filestream type: contains file information as well as position for
   reading and max size for writing.
   ASSUMPTION: Writing is always done at the end of the file. */

//TODO: No longer needs to be global
typedef struct
{
  dword read_pos;   /* position for reading */
  dword write_pos;  /* position for writing */
  unsigned int max_size;        /* max file size */
  int handle;
  char *fname;
}
diag_fs_type;

static diag_fs_type diag_fs_config = {
  0,
  0,
  0xFFFFFFFFU,          /* Unused field */
  -1,
  ".DIAGCFG"
};

static diag_fs_type diag_fs_in = {
  0,
  0,
  0xFFFFFFFFU,          /* Unused field */
  -1,
  ".DIAGIN"
};



#if !defined (DIAG_FS_INBUF_MAX_SIZE)
#define DIAG_FS_INBUF_MAX_SIZE (16 * 1024)
#endif

static diag_fs_type diag_fs_inbuf = {
  0,
  0,
  DIAG_FS_INBUF_MAX_SIZE,
  -1,
  ".DIAGINTMP"
};

/* Prototypes for filestream mode */
static void diag_fs_init (diag_fs_type * fs);
static boolean diag_fs_inbound_mode (void *buf, unsigned int size);
static uint32 diag_fs_read (diag_fs_type * fs, void *buf, unsigned int size);
static boolean diag_fs_write (diag_fs_type * fs, void *buf,
                  unsigned int size);
static void diag_fs_process_inbound (diag_fs_type * fs, void *buf,
                     unsigned int size);
static void diag_fs_terminate (diag_fs_type * fs);
static void diag_fs_inbound_comm (diag_fs_type * fs, void *buf,
                  unsigned int size);
static void diag_fs_process_config (void *req_pkt, word pkt_len);
static void diag_fs_write_config (void);
static void diag_process_input_file (diag_fs_type * fs,void *buf,
                  unsigned int size);



#endif /* DIAG_FEATURE_EFS2 */

/*Diag calls WCDMA cb to get the frame number. The cb is stored in
  the variable below*/
diag_wcdma_wl1_frame_cb_type diag_wcdma_wl1_frame_cb = NULL;

typedef struct
{
  /* (Input) raw (HDLC encoded) stream buffer.  This is never changed
     by the packet extraction/decoding functions. */
  byte *src_ptr;

  /* (I/O) The current byte array index of src_ptr.
     When src_idx == src_size, the src_ptr buffer is complete. */
  unsigned int src_idx;

  /* (Input) This is the number of bytes to convert from src_ptr. */
  unsigned int src_size;

  /* NOTE: Since a packet can cpan mulitple source blocks, dest_idx
     must be stored to maintain the state of the packet extraction.
     For src_ptr, this is not necessary. */

  /* (Input) The extracted packet.  This is never changed
     by the packet extraction/decoding functions. */
  byte *dest_ptr;

  /* (I/O) The array index into dest_ptr of the next unwritten byte.
     Upon successful decode of a packet, this field is the length. */
  unsigned int dest_idx;

  /* This is the number of bytes that can be stored in dest_ptr. */
  unsigned int dest_size;

  /* Used internally: Are we in the middle of an escape code?
     This boolean is needed in order to be able to handle the
     case where an escape code straddles two packets. */
  boolean escaping;

}
diag_hdlc_decode_type;

/* Q and Q types for diag_cmd_req () */
typedef struct
{
  q_link_type link;

  diag_cmd_rsp rsp_func;
  void *param;
  boolean diag_mv_cmd; /*Mobile view Command*/
  unsigned int length;
  byte req[1024];             /* sized by the 'length' field */
}
diag_req_cmd_type;

static q_type diag_req_cmd_q;

extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */
extern diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];
extern diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];


/* -------------------------------------------------------------------------
** Forward Declarations
** ------------------------------------------------------------------------- */
boolean diag_check_password (void *code_ptr);

#ifdef FEATURE_DIAG_SWITCH
static boolean diag_get_rx_pkt_slave (diag_rx_pkt_type *rx_pkt);
static boolean diag_get_rx_pkt (uint8 * dest_ptr, unsigned int size,
                unsigned int *pkt_len);
#else
 #if !defined (DIAG_FRAMING)
   static boolean diag_get_rx_pkt_slave (diag_rx_pkt_type *rx_pkt);
  #else
    static boolean diag_get_rx_pkt (uint8 * dest_ptr, unsigned int size,
                unsigned int *pkt_len);
  #endif
#endif
void diag_handle_internal_sigs( void );
static osal_sigs_t diag_handle_sigs (osal_sigs_t mask);
#if ((!defined(FEATURE_DIAG_SWITCH) && defined(DIAG_FRAMING)) || (defined(FEATURE_DIAG_SWITCH))|| defined(DIAG_FEATURE_EFS2))
static boolean diag_extract_pkt (diag_hdlc_decode_type * hdlc);
static boolean diag_hdlc_decode (diag_hdlc_decode_type * hdlc);
#endif
static void diag_heap_init (void);
void diag_rx_rsp_notify( void );
void diag_rx_delay_rsp_notify( void );

uint32 diag_listeners_alloc_cnt = 0;
uint32 diag_listeners_drop_cnt = 0;
uint32 diag_listeners_free_cnt =0;
uint16 diag_active_listeners_cnt = 0;

#ifdef FEATURE_DEBUG_DIAG_HEAP
uint32 diag_malloc_sucess_cnt =0;
uint32 diag_free_cnt =0;
uint32 diag_process_rx_free_cnt = 0;
uint32 diag_searchlist_add_malloc_cnt = 0;
uint32 diag_searchlist_add_free_cnt = 0;
uint32 diag_searchlist_delete_malloc_cnt = 0;
uint32 diag_searchlist_delete_free_cnt = 0;
uint32 diag_cmd_req_cnt = 0;
uint32 diag_fs_process_inbound_free_cnt = 0;
uint32 diag_fs_process_inbound_malloc_cnt = 0;

uint32 diag_rsp_malloc_sucess_cnt =0;
uint32 diag_rsp_free_cnt =0;

uint32 diag_event_malloc_sucess_cnt =0;
uint32 diag_event_free_cnt =0;

uint32 diag_event_malloc_sec_sucess_cnt =0;
uint32 diag_event_free_sec_cnt =0;



uint32 diag_heap_used_bytes(void)
{
  return diag_heap.used_bytes;
}

uint32 diag_rsp_heap_used_bytes(void)
{
  return diag_rsp_heap.used_bytes;
}

uint32 diag_event_heap_used_bytes(void)
{
  return diag_event_heap.used_bytes;
}

uint32 diag_event_sec_heap_used_bytes(void)
{
  return diag_event_sec_heap.used_bytes;
}

#endif /* FEATURE_DEBUG_DIAG_HEAP */

static void diag_process_rx (void);


/*static*/ void diag_process_rx_dci (void);
uint32 diag_dci_cmd_tag = 0;


#include <stdlib.h>
#ifdef FEATURE_WINCE
#error code not present
#endif

static osal_mutex_arg_t diag_srch_list_mutex;
osal_mutex_arg_t diag_sleep_vote_mutex; /*used in diagpkt.c */

/*Mutex used while updating events and log arrays for SSM */
#ifdef FEATURE_SSM
osal_mutex_arg_t diag_event_mask_ssm_mutex;
osal_mutex_arg_t diag_log_mask_ssm_mutex;
#endif /*FEATURE_SSM*/

osal_mutex_arg_t diag_idle_process_mutex;

 void diag_search_list_cs_init(void)
 {
    int return_val;

    diag_srch_list_mutex.name = "MUTEX_DIAG_SRCH_LIST_CS";
    return_val = osal_init_mutex(&diag_srch_list_mutex);
    ASSERT(OSAL_SUCCESS == return_val);
 }

 void diag_sleep_vote_cs_init(void)
 {
    int return_val;

    diag_sleep_vote_mutex.name = "MUTEX_DIAG_SLEEP_VOTE_CS";
    return_val = osal_init_mutex(&diag_sleep_vote_mutex);
    ASSERT(OSAL_SUCCESS == return_val);
 }

 void diag_int_sig_cs_init(void)
 {
    int return_val;

    diag_internal_sig_mutex.name = "MUTEX_DIAG_INT_SIG_CS";
    return_val = osal_init_mutex(&diag_internal_sig_mutex);
    ASSERT(OSAL_SUCCESS == return_val);
 }

extern boolean diagpkt_rsp_send (void);
extern boolean diagcomm_sio_flow_enabled(void);
extern boolean diagpkt_delay_rsp_send (void);
/*Indicates diag task initialization started */
static int diag_task_begin = FALSE;

typedef enum
{
  DIAG_ALLOC_CHECK_OPT           = 0,
  DIAG_ALLOC_CHECK_OPT_RSP       = 1,
  DIAG_ALLOC_CHECK_OPT_EVENT     = 2,
  DIAG_ALLOC_CHECK_OPT_EVENT_SEC = 3,

  DIAG_ALLOC_CHECK_OPT_INVALID
} diag_alloc_check_opt_type;

/* Common code for alloc_check functions. */
static int diag_alloc_check_common(unsigned int num_allocs,
                                   uint32 *alloc_sizes,
                                   diag_alloc_check_opt_type opt);


/* -------------------------------------------------------------------------
** Function Implementations
** ------------------------------------------------------------------------- */

/*===========================================================================

FUNCTION DIAG_INIT_LARGE_BUFFER (internal)

DESCRIPTION
  This function allocates large memory buffer dynamically and sets it as the 
  SIO TX DSM pool.
  
  We will not request the large memory buffer if we're in FTM mode.  We will 
  determine if we're in FTM mode by querying NV_FTM_MODE_I NV item.
  
PARAMETERS
  None
  
RETURN VALUE
  None
  
===========================================================================*/
#if defined(FEATURE_DIAG_DYNAMIC_HEAP)
void diag_init_large_buffer( void )
{
  uint32 max_heap_size = 0;
  boolean buffer_status = FALSE;
  unsigned int ret_val;
  nv_cmd_type nv_cmd_buf;
  byte ftm_mode = 0;                  /* Set default to assume we're not in FTM */

  nv_cmd_buf.cmd = NV_READ_F;         /* NV operation */
  nv_cmd_buf.tcb_ptr = NULL;          /* Setting tcb_ptr to NULL makes this a blocking call. We need NV item
                                         value here to proceed further. */
  nv_cmd_buf.sigs = NULL;
  nv_cmd_buf.done_q_ptr = NULL;
  nv_cmd_buf.item = NV_FTM_MODE_I;    /* If NV_FTM_MODE_I is not set, then we're not in FTM */
  nv_cmd_buf.data_ptr = (nv_item_type*)&ftm_mode;
  
  nv_cmd(&nv_cmd_buf);
  
  if( nv_cmd_buf.status != NV_DONE_S )
  {
    /* When the phone is just flashed and boots up it is likely that FTM NV item is not set at all. We may receive NV_NOT_ACTIVE_S. 
       In such case, we'll consider this as "not" in FTM. */
    ftm_mode = 0;
  }
  
  /* If not in FTM mode, request memory from dynamic heap. */
  if( ftm_mode == 0 )
  {
      ret_val = dynamic_heap_g_querysize( DYNAMIC_MEM_CLIENT_ID_DIAG, &max_heap_size );
        
      if( ret_val == DYNAMIC_MEM_SUCCESS )
      {
        //ToDo: truncate 'max_heap_size' retrieved to be divisible by 32 and dsm item size?
        ret_val = dynamic_heap_g_init( DYNAMIC_MEM_CLIENT_ID_DIAG, DYNAMIC_MEM_ATTRIBUTE_CMA, max_heap_size, NULL, (void *)&dsm_diag_buffering_item_array );
        
        if( ret_val == DYNAMIC_MEM_SUCCESS )
        {
          if( dsm_diag_buffering_item_array != NULL )
          {
            buffer_status = diag_set_buffering_buffer( dsm_diag_buffering_item_array, max_heap_size, DIAG_STREAM_1 );

            if( buffer_status == FALSE )
              MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_set_buffering_buffer of len %d failed", max_heap_size);
          }
        }
        else
        {
          MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "dynamic_heap_g_init failed %d", ret_val);
        }
      }
      else
      {
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "dynamic_heap_g_querysize failed %d", ret_val);
      }
  }
  else
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Not requesting extra memory because we're in FTM (%)", ftm_mode);
  }
  
} /* diag_init_large_buffer */
#endif
  
/*===========================================================================
FUNCTION DIAG_CTRL_SEND_PERIPH_BUF_SIZE

DESCRIPTION
  Sends buffering pool size in bytes to apps through a control packet. This should
  be called after the memory request from HLOS is done and also whenever buffer size 
  is changed. At present it is sent once during init.

PARAMETERS
  None

RETURN VALUE
  None

===========================================================================*/
void diag_ctrl_send_periph_buf_size(void)
{
    diag_ctrl_msg_type ctrl_msg;
    boolean is_sent = FALSE;
    uint32 bytes_written = 0;
    static diag_ctrl_info_fwd_status diag_periph_buf_size_fwd_status=DIAG_CTRL_INFO_FWD_STATUS_NOT_SENT;

    if(diag_periph_buf_size_fwd_status == DIAG_CTRL_INFO_FWD_STATUS_NOT_SENT)
    {
        /* Compose DIAG_CTRL_MSG_PERIPH_BUF_SIZE packet */
        ctrl_msg.cmd_type = DIAG_CTRL_MSG_PERIPH_BUF_SIZE;
        ctrl_msg.data_len = sizeof(diag_ctrl_msg_periph_buf_size_type);
        ctrl_msg.data.ctrl_msg_mem_buf_size.version = DIAG_CTRL_MSG_PERIPH_BUF_SIZE_VER;
        ctrl_msg.data.ctrl_msg_mem_buf_size.size= DIAG_BUFFERED_DSM_CNT_TO_BYTES(DSM_POOL_ITEM_CNT(diag_tx_mode[DIAG_ID_TO_INDEX(DIAG_STREAM_1)].pool_info.pool_id));

        bytes_written = diagcomm_ctrl_send_mult_pkt( (void *)&ctrl_msg, DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_periph_buf_size_type),
                                                        NULL, 0, DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1, TRUE, &is_sent );

        if(bytes_written == 0)
             return;
        else if(is_sent == TRUE)
             diag_periph_buf_size_fwd_status = DIAG_CTRL_INFO_FWD_STATUS_SENT;
        else /* bytes_written > 0 && is_sent == FALSE */
             diag_periph_buf_size_fwd_status = DIAG_CTRL_INFO_FWD_STATUS_QUEUED;

    }

}

/*===========================================================================

FUNCTION DIAG_RX_DCI_NOTIFY

DESCRIPTION
  This callback routine is called when data has been received on DCI stream
  and is ready to be read by the diag task.

===========================================================================*/
void
diag_rx_dci_notify (void)
{
	osal_sigs_t return_sigs;
    int return_val = 0;
    return_val =osal_set_sigs(diag_task_tcb, DIAG_RX_DCI_SIG, &return_sigs);
    ASSERT(OSAL_SUCCESS == return_val);
}


/* ==================================================================
FUNCTION DIAG_HEAP_INIT
DESCRIPTION
   Initializes the heap that REX uses to dole out memory.
===================================================================== */
static void
diag_heap_init (void)
{
  mem_init_heap (&diag_heap, diag_heap_buf, sizeof (diag_heap_buf), NULL);
  #ifndef FEATURE_WINCE
  (void) mem_heap_set_int_lock (&diag_heap);
  #endif
  diag_heap_chunk_size = mem_heap_get_chunk_size (&diag_heap);
  return;
}               /* END diag_heap_init */

/* ==================================================================
FUNCTION DIAG_RSP_HEAP_INIT
DESCRIPTION
   Initializes the heap that REX uses to dole out memory.
===================================================================== */
static void
diag_rsp_heap_init (void)
{
  mem_init_heap (&diag_rsp_heap, diag_rsp_heap_buf, sizeof (diag_rsp_heap_buf), NULL);
  (void) mem_heap_set_int_lock (&diag_rsp_heap);
  diag_rsp_heap_chunk_size = mem_heap_get_chunk_size (&diag_rsp_heap);
  return;
}               /* END diag_event_heap_init */

/* ==================================================================
FUNCTION DIAG_ALLOC_CHECK_COMMON
DESCRIPTION
   Common code used by alloc_check functions.
===================================================================== */
static int
diag_alloc_check_common (unsigned int num_allocs,
                         uint32 *alloc_sizes,
                         diag_alloc_check_opt_type opt)
{
  unsigned int i;
  unsigned int j;
  unsigned int cur_req_size;
  unsigned int best_match_size;
  unsigned int best_match_index;
  unsigned int freeblocks[DIAG_MAX_FREEBLOCK_CACHE_SIZE];
           int *heap_cache_valid_ptr;
  unsigned int *freeblock_info_ptr;
  unsigned int heap_chunk_size;
  unsigned int freeblock_cache_size;


  switch (opt)
  {
    case DIAG_ALLOC_CHECK_OPT:
      heap_cache_valid_ptr = &diag_heap_cache_valid;
      freeblock_info_ptr = diag_freeblock_info;
      heap_chunk_size = diag_heap_chunk_size;
      freeblock_cache_size = DIAG_FREEBLOCK_CACHE_SIZE;
      break;
    case DIAG_ALLOC_CHECK_OPT_RSP:
      heap_cache_valid_ptr = &diag_rsp_heap_cache_valid;
      freeblock_info_ptr = diag_rsp_freeblock_info;
      heap_chunk_size = diag_rsp_heap_chunk_size;
      freeblock_cache_size = DIAG_FREEBLOCK_CACHE_SIZE;
      break;
    case DIAG_ALLOC_CHECK_OPT_EVENT:
      heap_cache_valid_ptr = &diag_event_heap_cache_valid;
      freeblock_info_ptr = diag_event_freeblock_info;
      heap_chunk_size = diag_event_heap_chunk_size;
      freeblock_cache_size = DIAG_EVENT_FREEBLOCK_CACHE_SIZE;
      break;
    case DIAG_ALLOC_CHECK_OPT_EVENT_SEC:
      heap_cache_valid_ptr = &diag_event_sec_heap_cache_valid;
      freeblock_info_ptr = diag_event_freeblock_info_sec;
      heap_chunk_size = diag_event_sec_heap_chunk_size;
      freeblock_cache_size = DIAG_EVENT_FREEBLOCK_CACHE_SIZE;
      break;
    default:
      /* Return 0 if invalid option specified. */
      return 0;
  }

  /* Return 0 if num_allocs is greater than free block cache size. */
  if (num_allocs > freeblock_cache_size)
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_alloc_check_common: num_allocs %d greater than freeblock cache", num_allocs);
    return 0;
  }

  /* Return 1 if the freeblock cache is dirty.
  */
  if (!(*heap_cache_valid_ptr))
  {
    return 1;
  }

  memscpy (freeblocks, sizeof(freeblocks), freeblock_info_ptr,
    freeblock_cache_size * sizeof (int));

  /* Match each requested size up with the closest available size that
  ** is greater than the requested size. Update the freeblock information
  ** after each match. If even one requested size cannot be matched up,
  ** then the heap does not contain enough space for all the requests.
  */
  for (i = 0; i < num_allocs; i++)
  {
    cur_req_size = ((alloc_sizes[i] + sizeof (mem_block_header_type)
      + heap_chunk_size - 1) / heap_chunk_size)
      * heap_chunk_size;
    best_match_size  = 0x7fffffff;
    best_match_index = freeblock_cache_size;
    for (j = 0; j < freeblock_cache_size; j++)
    {
      if ((freeblocks[j] >= cur_req_size) &&
          (freeblocks[j] <  best_match_size))
      {
        best_match_size  = freeblocks[j];
        best_match_index = j;
      }
    }
    if (best_match_index < freeblock_cache_size)
    {
      freeblocks[best_match_index] -= cur_req_size;
    }
    else
    {
      return 0;
    }
  }

  return 1;

} /* END diag_alloc_check_common */

/*===================================================================*/
int
diag_rsp_alloc_check (unsigned int num_allocs, uint32 *alloc_sizes )
{
  return diag_alloc_check_common(num_allocs,
                                 alloc_sizes,
                                 DIAG_ALLOC_CHECK_OPT_RSP);

} /* END diag_rsp_alloc_check */

void *
diag_rsp_malloc (uint32 num_bytes)
{
  void *buf = NULL;

  if (!diag_rsp_heap_cache_valid || diag_rsp_alloc_check (1, (uint32 *)&num_bytes))
  {
    buf = mem_malloc (&diag_rsp_heap, num_bytes);
  }

  /* If the allocation failed, update the freeblock cache if it is not
  ** currently valid.
  */
  if (buf == NULL)
  {
    if (!diag_heap_cache_valid)
    {
      mem_heap_get_freeblock_info (&diag_rsp_heap, DIAG_FREEBLOCK_CACHE_SIZE,
        diag_rsp_freeblock_info);
      diag_rsp_heap_cache_valid = 1;
    }
  }
  else
  {
    diag_rsp_heap_cache_valid = 0;

#ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_rsp_malloc_sucess_cnt++;
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "<<DIAG RESPONSE MALLOC sucess_cnt = %d, diag rsp heap used = %d ",
                    diag_rsp_malloc_sucess_cnt,diag_rsp_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */
  }
  return buf;
} /* diag_rsp_malloc */

/*===================================================================*/
void
diag_rsp_free(void *mem_ptr)
{
  mem_free (&diag_rsp_heap, mem_ptr);
  diag_rsp_heap_cache_valid = 0;
#ifdef FEATURE_DEBUG_DIAG_HEAP
  diag_rsp_free_cnt++;
  MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        ">> DIAG FREE diag_free_cnt = %d, diag heap used = %d ",
        diag_rsp_free_cnt,diag_rsp_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */
  return;
} /* END diag_rsp_free */

/*===================================================================*/
void *
diag_malloc (unsigned long num_bytes)
{
  void *buf = NULL;

  if (!diag_heap_cache_valid || diag_alloc_check (1, (uint32 *)&num_bytes))
  {
     buf = mem_malloc (&diag_heap, num_bytes);
  }

  /* If the allocation failed, update the freeblock cache if it is not
  ** currently valid.
  */

  if (buf == NULL)
  {
    if (!diag_heap_cache_valid)
    {
      mem_heap_get_freeblock_info (&diag_heap, DIAG_FREEBLOCK_CACHE_SIZE,
        diag_freeblock_info);
      diag_heap_cache_valid = 1;
    }
  }
  else
  {
    diag_heap_cache_valid = 0;

#ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_malloc_sucess_cnt++;
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "<<DIAG MALLOC diag_malloc_sucess_cnt = %d,  heap used = %d ",
                            diag_malloc_sucess_cnt, diag_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */
  }
  return buf;
} /* END diag_malloc */


int
diag_alloc_check (unsigned int num_allocs, uint32 *alloc_sizes )
{
  return diag_alloc_check_common(num_allocs,
                                 alloc_sizes,
                                 DIAG_ALLOC_CHECK_OPT);

} /* END diag_alloc_check */


/*===================================================================*/
void
diag_free(void *mem_ptr)
{
  mem_free (&diag_heap, mem_ptr);
#ifdef FEATURE_DEBUG_DIAG_HEAP
  diag_free_cnt++;
  MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        ">> DIAG FREE diag_free_cnt = %d, diag heap used = %d ",
                        diag_free_cnt,diag_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */
  diag_heap_cache_valid = 0;
  return;
} /* END diag_free */

/* ==================================================================
FUNCTION DIAG_EVENT_HEAP_INIT
DESCRIPTION
   Initializes the heap that REX uses to dole out memory.
===================================================================== */
static void
diag_event_heap_init (void)
{
  mem_init_heap (&diag_event_heap, diag_event_heap_buf, sizeof (diag_event_heap_buf), NULL);
  (void) mem_heap_set_int_lock (&diag_event_heap);
  diag_event_heap_chunk_size = mem_heap_get_chunk_size (&diag_event_heap);
  return;
} /* END diag_event_heap_init */


/*===================================================================*/
void *
diag_event_malloc (unsigned long num_bytes)
{
  void *buf = NULL;

  if (!diag_event_heap_cache_valid || diag_event_alloc_check (1, (uint32 *)&num_bytes))
  {
     buf = mem_malloc (&diag_event_heap, num_bytes);
  }

  /* If the allocation failed, update the freeblock cache if it is not
  ** currently valid.
  */

  if (buf == NULL)
  {
    if (!diag_event_heap_cache_valid)
    {
      mem_heap_get_freeblock_info (&diag_event_heap, DIAG_EVENT_FREEBLOCK_CACHE_SIZE,
        diag_event_freeblock_info);
      diag_event_heap_cache_valid = 1;
    }
  }
  else
  {
    diag_event_heap_cache_valid = 0;

    #ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_event_malloc_sucess_cnt++;
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
    "<<DIAG MALLOC diag_event_malloc_sucess_cnt = %d,  event_heap used = %d ",
                    diag_event_malloc_sucess_cnt, diag_event_heap_used_bytes());
    #endif /* FEATURE_DEBUG_DIAG_HEAP */
  }
  return buf;
} /* END diag_event_malloc */


int
diag_event_alloc_check (unsigned int num_allocs, uint32 *alloc_sizes )
{
  return diag_alloc_check_common(num_allocs,
                                 alloc_sizes,
                                 DIAG_ALLOC_CHECK_OPT_EVENT);

} /* END diag_event_alloc_check */

/*===================================================================*/
void
diag_event_free(void *mem_ptr)
{
  mem_free (&diag_event_heap, mem_ptr);
  diag_event_heap_cache_valid = 0;

  #ifdef FEATURE_DEBUG_DIAG_HEAP
  diag_event_free_cnt++;
  MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            ">> DIAG FREE diag_event_free_cnt = %d, diag event heap used = %d ",
                            diag_event_free_cnt,diag_event_heap_used_bytes());
  #endif /* FEATURE_DEBUG_DIAG_HEAP */
  return;
} /* END diag_event_free */

static void
diag_event_sec_heap_init (void)
{
  mem_init_heap (&diag_event_sec_heap, diag_event_sec_heap_buf, sizeof (diag_event_sec_heap_buf), NULL);
  (void) mem_heap_set_int_lock (&diag_event_sec_heap);
  diag_event_sec_heap_chunk_size = mem_heap_get_chunk_size (&diag_event_sec_heap);
  return;
}               /* END diag_event_sec_heap_init */


/*===================================================================*/
void *
diag_event_malloc_sec (unsigned long num_bytes)
{
  void *buf = NULL;

  if (!diag_event_sec_heap_cache_valid || diag_event_alloc_check_sec (1, (uint32 *)&num_bytes))
  {
     buf = mem_malloc (&diag_event_sec_heap, num_bytes);
  }

  /* If the allocation failed, update the freeblock cache if it is not
  ** currently valid.
  */

  if (buf == NULL)
  {
    if (!diag_event_sec_heap_cache_valid)
    {
      mem_heap_get_freeblock_info (&diag_event_sec_heap, DIAG_EVENT_FREEBLOCK_CACHE_SIZE,
        diag_event_freeblock_info_sec);
      diag_event_sec_heap_cache_valid = 1;
    }
  }
  else
  {
    diag_event_sec_heap_cache_valid = 0;

    #ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_event_malloc_sec_sucess_cnt++;
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
    " <<DIAG MALLOC diag_event_malloc_sucess_cnt = %d,  event_heap used = %d ",
            diag_event_malloc_sec_sucess_cnt, diag_event_sec_heap_used_bytes());
    #endif /* FEATURE_DEBUG_DIAG_HEAP */
  }
  return buf;
} /* END diag_event_malloc */


int
diag_event_alloc_check_sec (unsigned int num_allocs, uint32 *alloc_sizes )
{
  return diag_alloc_check_common(num_allocs,
                                 alloc_sizes,
                                 DIAG_ALLOC_CHECK_OPT_EVENT_SEC);

} /* END diag_event_alloc_check_sec */

/*===================================================================*/
void
diag_event_free_sec(void *mem_ptr)
{
  mem_free (&diag_event_sec_heap, mem_ptr);
  diag_event_sec_heap_cache_valid = 0;

  #ifdef FEATURE_DEBUG_DIAG_HEAP
  diag_event_free_sec_cnt++;
  MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        ">> DIAG FREE diag_event_free_cnt = %d, diag event heap used = %d ",
                    diag_event_free_sec_cnt,diag_event_sec_heap_used_bytes());
  #endif /* FEATURE_DEBUG_DIAG_HEAP */
  return;
} /* END diag_event_free_sec */



/*===================================================================*/
void *
diag_listeners_malloc (unsigned long num_bytes)
{
  void *buf = NULL;

  if( diag_active_listeners_cnt >= DIAG_MAX_ACTIVE_LISTENERS )
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "Limit on active listeners reached, dropping the listeners");
    diag_listeners_drop_cnt++;
  }

  else
  {
    /*memory allocated from system heap */
    buf = malloc(num_bytes);

    if (buf)
    {
        if(diag_listeners_alloc_cnt == MAX_VALUE_UINT32)
        {
            MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable diag_listeners_alloc_cnt has reached maximum value.");
        }
        else
        {
            diag_listeners_alloc_cnt++;
            MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Memory allocated for a diag listener");
            diag_active_listeners_cnt++;
        }
    }
    else
    {
        diag_listeners_drop_cnt++;
        MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR,
                    "Unable to allocate memory from system heap");
    }
  }

  return buf;
} /* END diag_listeners_malloc */

/*===================================================================*/
void
diag_listeners_free(void *mem_ptr)
{

  free (mem_ptr);
  if(diag_listeners_free_cnt == MAX_VALUE_UINT32)
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable diag_listeners_free_cnt has reached maximum value.");
  }
  else
  {
    diag_listeners_free_cnt++;
    diag_active_listeners_cnt--;
  }

  return;
} /* END diag_listeners_free */
/*===================================================================*/


/*===================================================================*/
void
diag_block (void)
{
  osal_sigs_t return_sigs = 0;
  int return_val = 0,return_val_tout;

  do
  {
    return_sigs = osal_thread_wait (diag_task_tcb, DIAG_BLOCK_SIG | DIAG_RPT_SIG|DIAG_SIO_TOUT_TIMER_SIG);
    diag_kick_watchdog();

  } while( ((return_sigs & DIAG_BLOCK_SIG) == 0) && ((return_sigs & DIAG_SIO_TOUT_TIMER_SIG) == 0));

  /*Verify whether time out has occurred and set flag to TRUE */
  if((return_sigs & DIAG_SIO_TOUT_TIMER_SIG))
  {
    diag_flush_timeout = TRUE;
    MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Diag timed out on SIO callback");
  }
  return_val_tout = osal_reset_sigs(diag_task_tcb, DIAG_SIO_TOUT_TIMER_SIG, &return_sigs);
  return_val = osal_reset_sigs(diag_task_tcb, DIAG_BLOCK_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val_tout);
  ASSERT(OSAL_SUCCESS == return_val);
  return;
} /* END diag_block */

/*===================================================================*/
void
diag_release (void)
{
  osal_sigs_t return_sigs;
  int return_val = 0,return_val_tout = 0;

  return_val = osal_set_sigs(diag_task_tcb, DIAG_BLOCK_SIG, &return_sigs);
  return_val_tout = osal_reset_timer(&diag_sio_tout_timer);
  diag_flush_timeout = FALSE;
  ASSERT(OSAL_SUCCESS == return_val);
  ASSERT(OSAL_SUCCESS == return_val_tout);

  return;
} /* END diag_release */

#ifdef FEATURE_SSM
static ssm_err_t diag_ssm_init_err = E_SSM_CLIENT_NOT_INITIALIZED;
static ssm_err_t diag_log_mask_ssm_init_err = E_SSM_CLIENT_NOT_INITIALIZED;
static ssm_err_t diag_event_mask_ssm_init_err = E_SSM_CLIENT_NOT_INITIALIZED;

/*===========================================================================

FUNCTION DIAG_SSM_CB

DESCRIPTION
  This callback is used to notify Diag asynchronosly about the state of
  initialization.  'diag_ssm_id' is only valid when this callback returns
  'E_SSM_SUCCESS'.  SSM can transition to different modes (transparent to
  Diag), and this callback will be used to notify Diag if 'diag_ssm_id'
  is valid or not.

===========================================================================*/
static void diag_ssm_cb( ssm_err_t err)
{
  diag_ssm_init_err = err;

} /* diag_ssm_cb */


/*===========================================================================

FUNCTION DIAG_LOG_MASK_SSM_CB

DESCRIPTION
  This callback is used to notify Diag asynchronosly about the state of
  initialization.  'diag_log_mask_ssm_id' is only valid when this callback returns
  'E_SSM_SUCCESS'.  SSM can transition to different modes (transparent to
  Diag), and this callback will be used to notify Diag if 'diag_log_mask_ssm_id'
  is valid or not.

  The SSM APIs are called here to read the permission file as this function is
  called during initialization and if there is a new permission file to be read.

===========================================================================*/
static void diag_log_mask_ssm_cb( ssm_err_t err)
{
  diag_log_mask_ssm_init_err = err;

/*check if the operation of reading permission was successful or not*/
/*If the operation of reading permission file was not successful, then we either revert
  back to default permissions (boot up) or to previous permissions (run time)
  */
  if(err != E_SSM_SUCCESS )
  {
    return;
  }

    /*Lock the mutex*/
   osal_lock_mutex(&diag_log_mask_ssm_mutex);

  num_log_code_ssm = 0;

  ssm_err_t diag_log_mask_ssm_get_data_size_status = ssm_get_perm_data_size (diag_log_mask_ssm_id, &num_log_code_ssm);

  if(diag_log_mask_ssm_get_data_size_status != E_SSM_SUCCESS || num_log_code_ssm == 0)
  {
    /*ERROR IN GETTING PERMISSION INFORMATION FROM SSM*/
    return;
  }

   if(diag_log_mask_range != NULL)
     free (diag_log_mask_range);

  diag_log_mask_range = (diag_mask_ssm_buffer_type *) malloc (num_log_code_ssm * sizeof(diag_mask_ssm_buffer_type));

  if(diag_log_mask_range == NULL)
  {
    /* COULD NOT ALLOCATE BUFFER TO SAVE LOG_CODE RANGES*/
    return;
  }

memset (diag_log_mask_range, 0, num_log_code_ssm * sizeof(diag_mask_ssm_buffer_type));

ssm_err_t diag_ssm_copy_log_status = ssm_copy_perm_data (diag_log_mask_ssm_id, num_log_code_ssm * sizeof(diag_mask_ssm_buffer_type), (void*)diag_log_mask_range);

  if(diag_ssm_copy_log_status != E_SSM_SUCCESS || diag_log_mask_range->end == 0)
  {
    /*ERROR IN GETTING PERMISSION INFORMATION FROM SSM*/
    return;
  }

 /*Unlock the mutex*/
 osal_unlock_mutex(&diag_log_mask_ssm_mutex);

  diag_set_internal_sigs(DIAG_INT_LOG_MASK_SSM_UPDATE_SIG);

}  /* diag_log_mask_ssm_cb */

/*===========================================================================

FUNCTION DIAG_EVENT_MASK_SSM_CB

DESCRIPTION
  This callback is used to notify Diag asynchronosly about the state of
  initialization.  'diag_ssm_id' is only valid when this callback returns
  'E_SSM_SUCCESS'.  SSM can transition to different modes (transparent to
  Diag), and this callback will be used to notify Diag if 'diag_ssm_id'
  is valid or not.

===========================================================================*/
static void diag_event_mask_ssm_cb( ssm_err_t err)
{
  diag_event_mask_ssm_init_err = err;

 /*check if the operation of reading permission was successful or not
   If the operation of reading permission file was not successful,
   then we retain the current permissions
   (either the default permissions if SSM permissions have
   not been read yet, or the last loaded set of SSM permissions)
  */

  if(err != E_SSM_SUCCESS )
  {
    return;
  }

    /*Lock the mutex*/
  osal_lock_mutex(&diag_event_mask_ssm_mutex);

  num_event_code_ssm = 0;

  ssm_err_t diag_event_mask_ssm_get_data_size_status = ssm_get_perm_data_size (diag_event_mask_ssm_id, &num_event_code_ssm);

  if(diag_event_mask_ssm_get_data_size_status != E_SSM_SUCCESS || num_event_code_ssm == 0)
  {
    /* ERROR IN GETTING PERMISSION INFORMATION FROM SSM*/
    return;
  }

    if(diag_event_mask_range != NULL)
  {
    free(diag_event_mask_range);
  }

  diag_event_mask_range = (diag_mask_ssm_buffer_type *) malloc (num_event_code_ssm * sizeof(diag_mask_ssm_buffer_type));
  if(diag_event_mask_range == NULL)
  {
    /*COULD NOT ALLOCATE BUFFER TO SAVE EVENT_CODE RANGES*/
    return;
  }
  memset (diag_event_mask_range, 0, num_event_code_ssm * sizeof(diag_mask_ssm_buffer_type));

  ssm_err_t diag_ssm_copy_event_status = ssm_copy_perm_data (diag_event_mask_ssm_id, num_event_code_ssm * sizeof(diag_mask_ssm_buffer_type), (void*)diag_event_mask_range);

  if(diag_ssm_copy_event_status != E_SSM_SUCCESS)
  {
    /* ERROR IN GETTING PERMISSION INFORMATION FROM SSM*/
    return;
  }

      /*Unlock the mutex*/
 osal_unlock_mutex(&diag_event_mask_ssm_mutex);

  diag_set_internal_sigs(DIAG_INT_EVENT_MASK_SSM_UPDATE_SIG);

} /* diag_event_mask_ssm_cb */

/*===========================================================================

FUNCTION DIAG_SSM_INIT

DESCRIPTION
  To register with the Security Services Module.  SSM may access filesystem
  during registration, so ssm_client_init_func() may return
  'E_SSM_CLIENT_INIT_IN_PROCESS' immediately.  Diag will be notified when
  initialization is complete via the diag_ssm_cb() callback, by returing
  'E_SSM_SUCCESS' or some error code.

===========================================================================*/
static void diag_ssm_init(void)
{
  /* diag_ssm_id : uint32 :: Always returns a valid client id, whether
  initialization was succesful or not. It is recommended that clients
  check the return value of this init call before using client id.

  For Diag, SSM will look for ssm_diag_<mode_num>.ssm
  */

  diag_ssm_init_err = ssm_client_init_func( "diag",
                                            DIAG_SSM_KEY_NUM,
                                            DIAG_SSM_CMD_DFLT_PERM,
                                            SSM_UINT32,
                                                              diag_ssm_cb,
                                                      &diag_ssm_id,
                                                      diag_ssm_cmd_fb,
                                                      strlen(diag_ssm_cmd_fb)
                                                    );

  if( (diag_ssm_init_err != E_SSM_SUCCESS) &&
      (diag_ssm_init_err != E_SSM_CLIENT_INIT_IN_PROCESS) )
  {
    MSG_1 (MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Diag SSM Initialization Failed %d",
                                                            diag_ssm_init_err);
  }

} /* diag_ssm_init */

/*===========================================================================
FUNCTION DIAG_LOG_MASK_SSM_INIT

DESCRIPTION
  To register with the Security Services Module.  SSM may access filesystem
  during registration, so ssm_client_init_func() may return
  'E_SSM_CLIENT_INIT_IN_PROCESS' immediately.  Diag will be notified when
  initialization is complete via the diag_ssm_cb() callback, by returing
  'E_SSM_SUCCESS' or some error code.

===========================================================================*/
static void diag_log_mask_ssm_init(void)
{
   /* diag_log_mask_ssm_id : uint32 :: Always returns a valid client id, whether
  initialization was succesful or not. It is recommended that clients
  check the return value of this init call before using client id.

  For Diag, SSM will look for diag_log_mask_ssm.ssm
  */

   /*initialize the mutex used while updating diag_log_mask_ssm array*/
  int return_val = 0;
  diag_log_mask_ssm_mutex.name = "MUTEX_DIAG_LOG_MASK_SSM";
    return_val = osal_init_mutex(&diag_log_mask_ssm_mutex);
    ASSERT(OSAL_SUCCESS == return_val);

   diag_log_mask_ssm_init_err = ssm_client_init_func( "diag_log_mask_ssm",
                                            DIAG_LOG_MASK_SSM_KEY_NUM,
                                            DIAG_SSM_LOG_EVENT_DFLT_PERM,
                                            SSM_UINT32,
                                            diag_log_mask_ssm_cb,
                                            &diag_log_mask_ssm_id,
                                            diag_ssm_log_fb,
                                            strlen(diag_ssm_log_fb)
                                            );

  if( (diag_log_mask_ssm_init_err != E_SSM_SUCCESS) &&
      (diag_log_mask_ssm_init_err != E_SSM_CLIENT_INIT_IN_PROCESS) )
  {
    MSG_1 (MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Diag log mask SSM Initialization Failed %d", diag_ssm_init_err);
  }

} /* diag_log_mask_ssm_init */

/*===========================================================================
FUNCTION DIAG_EVENT_MASK_SSM_INIT

DESCRIPTION
  To register with the Security Services Module.  SSM may access filesystem
  during registration, so ssm_client_init_func() may return
  'E_SSM_CLIENT_INIT_IN_PROCESS' immediately.  Diag will be notified when
  initialization is complete via the diag_ssm_cb() callback, by returing
  'E_SSM_SUCCESS' or some error code.

===========================================================================*/
static void diag_event_mask_ssm_init(void)
{
   /* diag_event_mask_ssm_id : uint32 :: Always returns a valid client id, whether
  initialization was succesful or not. It is recommended that clients
  check the return value of this init call before using client id.

   For Diag, SSM will look for diag_event_mask_ssm.ssm
  */

   /*initialize the mutex used while updating diag_log_mask_ssm array*/
  int return_val = 0;
  diag_event_mask_ssm_mutex.name = "MUTEX_DIAG_EVENT_MASK_SSM";
    return_val = osal_init_mutex(&diag_event_mask_ssm_mutex);
    ASSERT(OSAL_SUCCESS == return_val);

  diag_event_mask_ssm_init_err = ssm_client_init_func( "diag_event_mask_ssm",
                                            DIAG_EVENT_MASK_SSM_KEY_NUM,
                                            DIAG_SSM_LOG_EVENT_DFLT_PERM,
                                            SSM_UINT32,
                                                              diag_event_mask_ssm_cb,
                                                      &diag_event_mask_ssm_id,
                                            diag_ssm_event_fb,
                                            strlen(diag_ssm_event_fb)
                                                      );

  if( (diag_event_mask_ssm_init_err != E_SSM_SUCCESS) &&
      (diag_event_mask_ssm_init_err != E_SSM_CLIENT_INIT_IN_PROCESS) )
  {
    MSG_1 (MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "Diag event mask SSM Initialization Failed %d", diag_ssm_init_err);
  }

} /* diag_event_mask_ssm_init */

/*===========================================================================

FUNCTION DIAG_SSM_GET_ID

DESCRIPTION
  Return SSM ID associated with Diag.  'diag_ssm_id' is only considered valid
  if 'diag_ssm_init_err' is 'E_SSM_SUCCESS.

===========================================================================*/
uint32 diag_ssm_get_id( void )
{
  return diag_ssm_id;

} /* diag_ssm_get_id */

/*===========================================================================

FUNCTION DIAG_LOG_MASK_SSM_GET_ID

DESCRIPTION
  Return SSM ID associated with Diag.  'diag_ssm_id' is only considered valid
  if 'diag_ssm_init_err' is 'E_SSM_SUCCESS.

===========================================================================*/
uint32 diag_log_mask_ssm_get_id( void )
{
  return diag_log_mask_ssm_id;

} /* diag_log_mask_ssm_id */

/*===========================================================================

FUNCTION DIAG_EVENT_MASK_SSM_GET_ID

DESCRIPTION
  Return SSM ID associated with Diag.  'diag_ssm_id' is only considered valid
  if 'diag_ssm_init_err' is 'E_SSM_SUCCESS.

===========================================================================*/
uint32 diag_event_mask_ssm_get_id( void )
{
  return diag_event_mask_ssm_id;

} /* diag_log_mask_ssm_id */

/*===========================================================================

FUNCTION DIAG_SSM_GET_ERROR

DESCRIPTION
  Return SSM init error code. 'diag_ssm_id' is only considered valid
  if 'diag_ssm_init_err' is 'E_SSM_SUCCESS.

===========================================================================*/
ssm_err_t diag_ssm_get_error( void )
{
  return diag_ssm_init_err;

} /* diag_ssm_get_error */

/*===========================================================================

FUNCTION DIAG_LOG_MASK_SSM_GET_ERROR

DESCRIPTION
  Return SSM init error code. 'diag_log_mask_ssm_id' is only considered valid
  if 'diag_log_mask_ssm_init_err' is 'E_SSM_SUCCESS.

===========================================================================*/
ssm_err_t diag_log_mask_ssm_get_error( void )
{
  return diag_log_mask_ssm_init_err;

} /* diag_log_mask_ssm_get_error */

/*===========================================================================

FUNCTION DIAG_EVENT_MASK_SSM_GET_ERROR

DESCRIPTION
  Return SSM init error code. 'diag_event_mask_ssm_id' is only considered valid
  if 'diag_event_mask_ssm_init_err' is 'E_SSM_SUCCESS.

===========================================================================*/
ssm_err_t diag_event_mask_ssm_get_error( void )
{
  return diag_event_mask_ssm_init_err;

} /* diag_event_mask_ssm_get_error */

#endif /* FEATURE_SSM */


/*===========================================================================

FUNCTION DIAG_EARLY_INIT

DESCRIPTION
  This function initializes the diagnostics message,log,event services
  If the clients wanted to call the diag apis before the diag task is
  initialized then this function needs to be called earlier
===========================================================================*/
void diag_early_init(void)
{
  if( diag_services_initialized )
    return;

  #ifdef FEATURE_SSM
  diag_ssm_init();
  diag_log_mask_ssm_init();
  diag_event_mask_ssm_init();
  #endif /* FEATURE_SSM */

  diagbuf_init ();

  msg_init ();

  /* Init event service */
  event_init ();

  /* Init log service */
  log_init ();

  diag_services_initialized = TRUE;

} /* diag_early_init */

/*===========================================================================

FUNCTION DIAG_INIT

DESCRIPTION
  This function initializes the SIO descriptors for the request and
  response buffers.  Initializes the msg services.  This is called prior to
  the task being started, so no assumptions about other tasks can be made.

===========================================================================*/
void
diag_init (void)
{
  int i, return_val = 0;

  /*Retrieve the diag_tcb pointer from RCINIT API*/
  diag_task_tcb=rcinit_lookup_rextask("diag");

  diag_init_feature_mask();

#if defined (FEATURE_WINCE)
  #error code not present
#endif

  diag_sleep_init();

#if defined FEATURE_DIAG_STM
  // Initialize STM interface
  diag_stm_init();
#endif

  if(FALSE == diag_time_initialized)
  {
    (void)diag_time_init();
  }
#if defined (DIAG_DOG_SUPPORT)
     diag_dog_rpt_var = osal_dog_hb_register((osal_sigs_t)DIAG_RPT_SIG);
#endif

  if(diag_task_tcb)
  {
      return_val = osal_create_diag_timer(&diag_tout_timer, diag_task_tcb, DIAG_TOUT_TIMER_SIG);
      ASSERT(OSAL_SUCCESS == return_val);

  /* Timer used to notify DIAG that TX is pending from a client. */
  return_val = osal_create_diag_timer(&diag_tx_notify_timer, diag_task_tcb, DIAG_TX_SIG);
  ASSERT(OSAL_SUCCESS == return_val);

  /* Timer used to notify DIAG that TX is pending from a client. */
  return_val = osal_create_diag_timer(&diag_drain_timer, diag_task_tcb, DIAG_DRAIN_TIMER_SIG);
  ASSERT(OSAL_SUCCESS == return_val);

  /* Timer used to notify DIAG that RSP is pending from a client. */
  return_val = osal_create_diag_timer(&diag_rx_rsp_notify_timer, diag_task_tcb, DIAG_RX_RSP_SIG);
  ASSERT(OSAL_SUCCESS == return_val);

  /* Timer used to notify DIAG that delayed RSP is pending from a client. */
  return_val = osal_create_diag_timer(&diag_rx_delay_rsp_notify_timer, diag_task_tcb, DIAG_RX_DELAY_RSP_SIG);
  ASSERT(OSAL_SUCCESS == return_val);

  return_val = osal_create_diag_timer(&diag_sio_tout_timer, diag_task_tcb, DIAG_SIO_TOUT_TIMER_SIG);
  ASSERT(OSAL_SUCCESS == return_val);

  /*Timer used to wait for a certain amount of time before sending out the notifications to registered users about diag mask change*/
  return_val = osal_create_diag_timer_ex(&diag_mask_change_notify_timer, diag_task_tcb, diag_mask_notify_timer_cb, 0);
  ASSERT(OSAL_SUCCESS == return_val);

      /* reset the pending, enabling the mechanism. */
      diag_tx_pending = FALSE;
      diag_drain_timer_pending = FALSE;
      diag_event_stale_timer_pending = FALSE;
      diag_event_stale_timer_pending_sec = FALSE;
  }
  else
  {
    ASSERT(0);
  }
  diagcomm_io_init();

  /* Creating Diag error ULog handle */
  ULogFront_RealTimeInit(&diag_err_ulog_handle, "DiagErrors", DIAG_ERR_ULOG_SIZE, ULOG_MEMORY_LOCAL, ULOG_LOCK_OS);

  /* Initialize buffering mode parameters. */
  diag_tx_mode_init();

  /* If the code in the phone is not programmed, the phone defaults to
     unlocked security. */
  if (diag_check_password (NULL))
  {
    diag_security = DIAG_SEC_UNLOCKED;
  }
  else
  {
    diag_security = DIAG_SEC_LOCKED;
  }

  diag_search_list_cs_init();
  diag_sleep_vote_cs_init();
  diag_int_sig_cs_init();

#if defined (FEATURE_WINCE)
  #error code not present
#endif

  diag_heap_init ();
  diag_rsp_heap_init ();
  diag_event_heap_init();
  diag_event_sec_heap_init();


  /* Registering Diag item pools */
  /*if diagdsm_init() fails, for WinCE, the windiag.dll should not be loaded.
  For WinMobile, Move this call to WDG_Init(), which is called by the kernel.*/
#if !defined (FEATURE_WINCE)
  diagdsm_init();
#endif

#if defined (FEATURE_DIAG_DYNAMIC_HEAP)
  diag_init_large_buffer();
#endif

#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
  diag_fwd_task_init();
#endif

  diag_early_init();

   /*initialize the mutex used while acessing diag_idl_proc_tbl*/
  diag_idle_process_mutex.name = "MUTEX_DIAG_IDLE_PROCESS";
  return_val = osal_init_mutex(&diag_idle_process_mutex);
  ASSERT(OSAL_SUCCESS == return_val);

  for (i = 0; i < DIAG_IDLE_PROC_CNT; i++)
  {
    diag_idle_proc_tbl[i].fp = NULL;
    diag_idle_proc_tbl[i].param = NULL;
  }

#ifdef FEATURE_WINCE
#error code not present
#endif

  /* Initialized the client command queue. */
  (void) q_init (&diag_req_cmd_q);
  /* Initialize the response queue */
  (void) diagpkt_rsp_q_init();
  /* Initialize the delay response queue */
  (void) diagpkt_delay_rsp_q_init();
  /* Initialize the delayed tag list */
  diagpkt_delayed_tag_list_init();


#if defined (DIAG_TOOLSDIAG_INIT)
    toolsdiag_init ();
#endif

  diagdiag_init ();


#if !defined(DIAG_RDM_LATE_REGISTRATION)
  /* diagcomm_sio_open_cb should be called to set the signal to open
     the port when RDM is not available */
#if !defined (DIAG_RUNTIME_DEVMAP) && defined (DIAG_SIO_SUPPORT)
   diagcomm_sio_open_cb (SIO_PORT_NULL);
#endif /* !DIAG_RUNTIME_DEVMAP */

#if defined DIAG_RUNTIME_DEVMAP
  #ifdef DIAG_SIO_SUPPORT
    /* Register callbacks with RDM */
    rdm_register_open_func (RDM_DIAG_SRVC, diagcomm_sio_open_cb);
    rdm_register_close_func (RDM_DIAG_SRVC, diagcomm_sio_close_cb);
  #endif /* DIAG_SIO_SUPPORT */
#else

#ifdef FEATURE_DIAG_SWITCH
#ifdef FEATURE_WINCE
  #error code not present
#endif

if(diag_on_single_proc == DIAG_FTM_SWITCH_VAL)
{
  #ifdef DIAG_SIO_SUPPORT
    /* Register callbacks with RDM */
    rdm_register_open_func (RDM_DIAG_SRVC, diagcomm_sio_open_cb);
    rdm_register_close_func (RDM_DIAG_SRVC, diagcomm_sio_close_cb);
  #endif /* DIAG_SIO_SUPPORT */
}
#endif /* FEATURE_DIAG_SWITCH */
#endif /* DIAG_RUNTIME_DEVMAP */
#endif /* !DIAG_RDM_LATE_REGISTRATION */

#if !defined  (FEATURE_WINCE)
   MSG (MSG_SSID_DIAG, MSG_LEGACY_LOW, "DIAG task initialized");
#else
  #error code not present
#endif

#ifdef DIAGDIAG_EXT_LISTENER_TEST
  extern void diagdiag_ext_listener_test(void);
  diagdiag_ext_listener_test();
#endif

}               /* diag_init */

#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
/*===========================================================================

FUNCTION DIAG_FWD_TASK_INIT

DESCRIPTION


===========================================================================*/
void diag_fwd_task_init(void)
{
  int diag_fwd_task_pri_array[32] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

  /* Start the task */
  /* stack is of type unsigned long long' */
  diag_fwd_task_attr.stack_size = (DIAG_FWD_STACK_SIZE * sizeof(unsigned long long) );
  diag_fwd_task_attr.start_func = diag_fwd_task_start;
  diag_fwd_task_attr.arg = NULL;
  diag_fwd_task_attr.name = "diag_fwd_task";
  diag_fwd_task_attr.priority = DIAG_FWD_PRI;
  diag_fwd_task_attr.stack_address = diag_fwd_stack;
  diag_fwd_task_attr.p_tskname = "DIAG_FWD_TASK";
  diag_fwd_task_attr.suspended = FALSE;
  diag_fwd_task_attr.dog_report_val = 0;

  if ((osal_create_thread(&diag_fwd_task_tcb, &diag_fwd_task_attr, diag_fwd_task_pri_array)) != 0)
  {
    ERR_FATAL("Create diag fwd task failed!", 0, 0, 0);

  }
}
#endif
/*===========================================================================

FUNCTION DIAG_WAIT

DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal.

  To avoid deadlock situations, this function also waits on the MC task
  sigs.  Since diag doesn't really care when power goes away, the func
  immediately ack's MC.

===========================================================================*/
dword
diag_wait (dword mask)  /* Signals to wait for */
{
  return ((dword)diag_handle_sigs ((osal_sigs_t)mask));
}


/*===========================================================================

FUNCTION DIAG_RX_NOTIFY

DESCRIPTION
  This callback routine is called when data has been received and is ready
  be read by the diag task.

===========================================================================*/
void
diag_rx_notify (void)
{
  osal_sigs_t return_sigs;
  int return_val = 0;

  return_val = osal_set_sigs(diag_task_tcb, DIAG_RX_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);

} /* diag_rx_notify */


/*=========================================================================*/
void
diag_tx_notify (void)
{
  osal_sigs_t return_sigs;
  int return_val = 0;

  if (!diag_tx_pending)
  {
    diag_tx_pending = TRUE;

  /* The timer is used because some MSG macros in DMSS?AMSS are called from
     INTLOCK. If you signal DIAG directly, it will cause a context switch
     while in INTLOCK.
     By setting a timer, the signal won't occur until the next clock tick
     ISR. Therefore, no signal is set during the possible INTLOCK. */

#if defined (DIAG_QDSP6_APPS_PROC)
  return_val = osal_set_sigs(diag_task_tcb, DIAG_TX_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);
#else
  if( osal_ints_are_locked())
    {
      return_val = osal_set_timer(&diag_tx_notify_timer, 1);
      ASSERT(OSAL_SUCCESS == return_val);
    }
    else
    {
      return_val = osal_set_sigs(diag_task_tcb, DIAG_TX_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
    }
#endif /* (IMAGE_APPS_PROC) && defined (IMAGE_QDSP6_PROC) */

  }
  return;
}
/*===========================================================================

FUNCTION DIAG_RX_RSP_NOTIFY

DESCRIPTION
  This routine sets the DIAG_RX_RSP_SIG

===========================================================================*/
void diag_rx_rsp_notify( void )
{
  osal_sigs_t return_sigs;
  int return_val = 0;

  if( osal_ints_are_locked())
    {
      return_val = osal_set_timer(&diag_rx_rsp_notify_timer, 1);
      ASSERT(OSAL_SUCCESS == return_val);
    }
    else
    {
  return_val = osal_set_sigs(diag_task_tcb, DIAG_RX_RSP_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);
}
}

/*===========================================================================

FUNCTION DIAG_RX_DELAY_RSP_NOTIFY

DESCRIPTION
  This routine sets the DIAG_RX_DELAY_RSP_SIG

===========================================================================*/
void diag_rx_delay_rsp_notify( void )
{
  osal_sigs_t return_sigs;
  int return_val = 0;

  if( osal_ints_are_locked())
  {
    return_val = osal_set_timer(&diag_rx_delay_rsp_notify_timer, 1);
    ASSERT(OSAL_SUCCESS == return_val);
  }
  else
  {
    return_val = osal_set_sigs(diag_task_tcb, DIAG_RX_DELAY_RSP_SIG, &return_sigs);
    ASSERT(OSAL_SUCCESS == return_val);
  }
}

/*===========================================================================

FUNCTION DIAG_PROCESS_RX

DESCRIPTION
  This routine processes the RX queue as well as the cmd Q from the system
  monitor API, i.e. diag_cmd_req().

===========================================================================*/
static void
diag_process_rx (void)
{
  #if (defined (DIAG_FRAMING)  || defined (FEATURE_DIAG_SWITCH))
  unsigned int pkt_len = 0;
  #endif

  diag_req_cmd_type *cmd = NULL;

  static int diag_first_pkt = TRUE;
#ifndef DIAG_FRAMING
  diag_rx_pkt_type rx_pkt = {diag_req_inbound, sizeof(diag_req_inbound), 0, DIAGCOMM_PORT_1};
#endif

  /* We must be connected, so set the TX sig to start draining. */
  if (TRUE == diag_first_pkt)
  {
    diag_tx_notify();
    diag_first_pkt = FALSE;
  }

  /* Disable diag sleep so packet can be processed w/out interruption */
  diag_sleep_vote (DIAG_SLEEP_DIAG, FALSE);

  /* Normal operation: process inbound data from comm layer.  Process
     packet if found. */
/*lint -save -e{545,740} Suspicious use of & Unusual pointer cast*/
#if !defined (DIAG_FRAMING)
#ifdef FEATURE_DIAG_SWITCH
  if(diag_on_single_proc != DIAG_FTM_SWITCH_VAL)
  {
    if (diag_get_rx_pkt_slave (&rx_pkt) == TRUE)
    {
      /* If we RX good packets, we must be connected. */
      DIAG_SET_MASK (diag_cx_state, DIAG_CX_COMM_S);

      diagpkt_process_request (&diag_req_inbound, rx_pkt.pkt_len, NULL, NULL,FALSE);

      #if defined (DIAG_FEATURE_EFS2)
        diag_fs_process_config (&diag_req_inbound, rx_pkt.pkt_len);
      #endif /* DIAG_FEATURE_EFS2 */
    }
  }
  else if(diag_on_single_proc == DIAG_FTM_SWITCH_VAL)
  {
    if (diag_get_rx_pkt ((uint8 *) & diag_req_inbound,
                sizeof (diag_req_inbound), (unsigned int *)&pkt_len) == TRUE)
    {
      /* If we RX good packets, we must be connected. */
      DIAG_SET_MASK (diag_cx_state, DIAG_CX_COMM_S);

      diagpkt_process_request (&diag_req_inbound, pkt_len, NULL, NULL,FALSE);

      #if defined (DIAG_FEATURE_EFS2)
        diag_fs_process_config (&diag_req_inbound, pkt_len);
      #endif /* DIAG_FEATURE_EFS2 */
    }
  }
#else /*FEATURE_DIAG_SWITCH*/
  if (diag_get_rx_pkt_slave (&rx_pkt) == TRUE)
  {
    /* If we RX good packets, we must be connected. */
    DIAG_SET_MASK (diag_cx_state, DIAG_CX_COMM_S);

    diagpkt_process_request (&diag_req_inbound, rx_pkt.pkt_len, NULL, NULL,FALSE);

    #if defined (DIAG_FEATURE_EFS2)
      diag_fs_process_config (&diag_req_inbound, rx_pkt.pkt_len);
    #endif /* DIAG_FEATURE_EFS2 */
  }
#endif /*FEATURE_DIAG_SWITCH*/
#else
  if (diag_get_rx_pkt ((uint8 *) & diag_req_inbound,
                sizeof (diag_req_inbound), (unsigned int *)&pkt_len) == TRUE)
  {
    /* If we RX good packets, we must be connected. */
    DIAG_SET_MASK (diag_cx_state, DIAG_CX_COMM_S);

    diagpkt_process_request (&diag_req_inbound, pkt_len, NULL, NULL,FALSE);

    #if defined (DIAG_FEATURE_EFS2)
      diag_fs_process_config (&diag_req_inbound, pkt_len);
    #endif /* DIAG_FEATURE_EFS2 */
  }
#endif
/*lint -restore */

  while ((cmd = q_get (&diag_req_cmd_q)) != NULL)
  {
    diagpkt_process_request (cmd->req, (uint16) cmd->length,
                             cmd->rsp_func, cmd->param,cmd->diag_mv_cmd);

    diag_free (cmd);
#ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_process_rx_free_cnt++;
    MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
                "FREE Q_GET DiagProcessRxFree = %d, heapUsed = %d, Req_q = %d",
     diag_process_rx_free_cnt, diag_heap_used_bytes(),q_cnt (&diag_req_cmd_q));
#endif /* FEATURE_DEBUG_DIAG_HEAP */
    cmd = NULL;
  }

  /* Go back to sleep after processing Rx packet */
  diag_sleep_vote(DIAG_SLEEP_DIAG, TRUE);

} /* diag_process_rx */

/*===========================================================================

FUNCTION DIAG_PROCESS_RX_DCI

DESCRIPTION
  This routine processes the DCI RX queue.

  Packet Structure

Field   7E Length Cmd_Code Cmd_Tag  Command_Packet      7E

Bytes   1     2      1        4    (Length - (1+4))     1

===========================================================================*/
  /*static*/ void
  diag_process_rx_dci (void)
  {
#if !defined (DIAG_FRAMING)
    diag_rx_pkt_type  rx_pkt = {&diag_req_inbound[3], sizeof(diag_req_inbound), 0, DIAGCOMM_PORT_2};
    diag_dci_enc_pkt *rx_dci = NULL;
	uint16 actual_cmd_length = 0; /* Length of the decoded command (excluding tag) */
    uint16 received_length = 0; /* Actual length received */
#endif


    /* Disable diag sleep so packet can be processed w/out interruption */
    diag_sleep_vote (DIAG_SLEEP_DIAG, FALSE);

#if !defined (DIAG_FRAMING)
    if(diag_get_rx_pkt_slave(&rx_pkt) == TRUE)
    {
      /* If we RX good packets, we must be connected. */
      DIAG_SET_MASK (diag_cx_state, DIAG_CX_DCI_S);

      rx_dci = (diag_dci_enc_pkt *)(rx_pkt.dest_ptr);

      /* Check if the packet is well-formatted */
      ASSERT((rx_dci->start) == DIAG_ASYNC_CONTROL_CHAR);

      /* Check the version */
      ASSERT((rx_dci->version) == DIAG_CONSUMER_API_VER_1);

      /* Length Sanity Check. Length returned by diag_get_rx_pkt_slave is the entire
         length from 7E to 7E (see description). Length field embedded in the packet
         is from Cmd_code to Command_Packet. */
      received_length = rx_pkt.pkt_len - sizeof(byte)- sizeof(byte) - sizeof(rx_dci->cmd_len) - sizeof(byte);
      ASSERT((rx_dci->cmd_len) == (received_length));

      /* TODO: Check the command code to see if it is a valid command request */

      /* Compute the actual length of the command request */
      actual_cmd_length = rx_dci->cmd_len - (sizeof(rx_dci->cmd_req_code) + sizeof(rx_dci->cmd_tag));
      diag_dci_cmd_tag  = rx_dci->cmd_tag;
      diagpkt_process_request (rx_dci->command, actual_cmd_length, NULL, NULL,FALSE);

      /* Set the global tag to zero so it would not be mis-used */
      diag_dci_cmd_tag = 0;

    }
#endif

    /* Go back to sleep after processing Rx packet */
    diag_sleep_vote(DIAG_SLEEP_DIAG, TRUE);
  }

/*===========================================================================

FUNCTION DIAG_HANDLE_INTERNAL_SIGS

DESCRIPTION
  Handle all of the internal Diag signals. This is an extension of
  diag_handle_sigs().

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

===========================================================================*/
void diag_handle_internal_sigs( void )
{
  int return_val = 0;
  osal_sigs_t return_sigs;
  osal_sigs_t sigs_internal;

  /*Get the signal mask of diag internal signals set */
  sigs_internal = diag_get_internal_sigs();

  /*feature mask should be the first thing to be sent*/
  if( sigs_internal & DIAG_INT_FEAT_UPDATE_SIG )
  {
    diag_clear_internal_sigs( DIAG_INT_FEAT_UPDATE_SIG );
    diag_ctrl_send_feature_mask();
#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
    diagcomm_ctrl_send_num_presets();
#endif
  }
  else if( sigs_internal & DIAG_INT_CTRL_REG_SIG )
  {
    diag_clear_internal_sigs( DIAG_INT_CTRL_REG_SIG );
#if defined(DIAG_MP) &&!defined(DIAG_MP_MASTER)
    /* Signal to forward Diag registration msgs to Master via CTRL channel. */
    diagpkt_fwd_cmd_reg();
#endif
  }
  else if (sigs_internal & DIAG_INT_SEND_BT_MASK_SIG)
  {
      diag_clear_internal_sigs( DIAG_INT_SEND_BT_MASK_SIG );
      /* Order of ctrl packets for this feature: (CTRL PKT 22,CTRL PKT 23,CTRL PKT 24,CTRL PKT 25)
      If nothing else, MSG SSID Range reporting (24) needs to be sent before MSG Build-mask reporting(25)
      Build/Range related control packets should be sent after feature-mask and any existing command-registrations are sent over */
#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
            diag_ctrl_send_event_id_ctrl_pkt();
            diag_ctrl_send_log_range_ctrl_pkt();
            diag_ctrl_send_msg_ssid_range_ctrl_pkt();
            diag_ctrl_send_msg_bt_mask_ctrl_pkt();
#endif
  }
  else if (sigs_internal & DIAG_INT_FLUSH_BUF_SIG)
  {
    diag_clear_internal_sigs( DIAG_INT_FLUSH_BUF_SIG );
    diagbuf_flush_timeout(0);
  }
  else if (sigs_internal & DIAG_INT_SEND_PERIPH_BUF_SIZE_SIG)
  {
    diag_clear_internal_sigs( DIAG_INT_SEND_PERIPH_BUF_SIZE_SIG );
#if defined(DIAG_MP) &&!defined(DIAG_MP_MASTER)
    diag_ctrl_send_periph_buf_size();
#endif
  }
  else if (sigs_internal & DIAG_INT_MASK_CHANGE_NOTIFY_SIG)
  {
    diag_clear_internal_sigs( DIAG_INT_MASK_CHANGE_NOTIFY_SIG );
    /*Notify the registered users about diag mask change*/
    diag_mask_change_notify();
  }
  else if(sigs_internal & DIAG_INT_LOG_MASK_SSM_UPDATE_SIG)
  {
    diag_clear_internal_sigs( DIAG_INT_LOG_MASK_SSM_UPDATE_SIG );
    #if defined(FEATURE_SSM)
    log_mask_ssm(diag_log_mask_range,num_log_code_ssm);
    #endif
  }
  else if( sigs_internal & DIAG_INT_SET_DRAIN_TIMER_SIG )
  {
    int return_val = 0;

    diag_clear_internal_sigs( DIAG_INT_SET_DRAIN_TIMER_SIG );
    if ( !diag_drain_timer_pending && diag_drain_timer_enabled() )
    {
      diag_drain_timer_pending = TRUE;
      return_val = osal_set_timer(&diag_drain_timer, diag_drain_timer_len);
      ASSERT(OSAL_SUCCESS == return_val);
    }
  }
  else if(sigs_internal & DIAG_INT_EVENT_MASK_SSM_UPDATE_SIG)
  {
    diag_clear_internal_sigs( DIAG_INT_EVENT_MASK_SSM_UPDATE_SIG );
    #if defined(FEATURE_SSM)
    event_mask_ssm(diag_event_mask_range,num_event_code_ssm);
    #endif
  }
  else if( sigs_internal & DIAG_INT_MASK_UPDATE_SIG )
  {
#if defined(DIAG_MP_MASTER)
    boolean is_sent = FALSE;
    diagcomm_enum_port_type port_num = 0;
    uint8 stream_index = 0;
    uint8 preset_index = 0;
#endif

    MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Handle DIAG_INT_MASK_UPDATE_SIG");

    diag_clear_internal_sigs( DIAG_INT_MASK_UPDATE_SIG );
#if defined(DIAG_MP_MASTER)
    for( port_num=0; port_num < NUM_SMD_PORTS; port_num++ )
    {
      if( diagcomm_ctrl_channel_status(DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num) )
      {
        for( stream_index=0; stream_index < DIAG_MAX_STREAM_ID; stream_index++ )
        {
          for( preset_index=0; preset_index < diag_max_presets[stream_index]; preset_index++ )
          {
            /* Checks will be done inside the send mask functions to determine whether masks needs to be sent or not. */
            diagcomm_ctrl_send_event_mask( DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num, stream_index+1, preset_index+1, FALSE );
            diagcomm_ctrl_send_log_mask( DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num, stream_index+1, preset_index+1, FALSE );
            diagcomm_ctrl_send_msg_mask( DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num, stream_index+1, preset_index+1, FALSE );
          }
        }

        /* No more masks to fwd; Send the current DSM item immediately and then quit */
        diagcomm_ctrl_send_mult_pkt( NULL, 0, NULL, 0, DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num, TRUE, &is_sent );
      }
    }
#endif
  }
  /* DIAG_INT_REG_TBL_RSP_SIG is set when a Registration Table Retrieval Command is sent
     ( in diagdiag_get_reg_table() ) */
  else if ( sigs_internal & DIAG_INT_REG_TBL_RSP_SIG )
  {
      diag_clear_internal_sigs( DIAG_INT_REG_TBL_RSP_SIG );
      diagpkt_get_registration_table();
  }
  else if(sigs_internal & DIAG_INT_OPEN_DCI_CHANNEL_SIG)
  {
    diag_clear_internal_sigs( DIAG_INT_OPEN_DCI_CHANNEL_SIG );
#if defined(FEATURE_SSM)
    if (diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2].connected == FALSE)
    {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Open the DCI channel");
      diagcomm_sio_open (DIAGCOMM_PORT_2); /* Open the DCI stream */
    }
    if (diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2].connected == FALSE)
    {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Open the DCI  command channel");
      diagcomm_cmd_open (DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_2);
    }
#endif
  }
  else if(sigs_internal & DIAG_INT_CLOSE_DCI_CHANNEL_SIG)
  {
    diag_clear_internal_sigs( DIAG_INT_CLOSE_DCI_CHANNEL_SIG );
    #if defined(FEATURE_SSM)
    /*Close the dci stream only if this function returns FALSE*/
    is_dci_allowed = diag_dci_open_query();
    if (!is_dci_allowed)
    {
          MSG_1 (MSG_SSID_DIAG, MSG_LEGACY_LOW, "Close the DCI channel %d", 0 );
          diagcomm_close_dci();   /*Close DCI stream*/
    }
    #endif
  }
  else if( sigs_internal & DIAG_INT_DRAIN_DSM_BUFFER_SIG )
  {
    diag_clear_internal_sigs( DIAG_INT_DRAIN_DSM_BUFFER_SIG );
    diag_drain_dsm_buffer( DIAG_STREAM_1 );
  }
  else if( sigs_internal & DIAG_INT_DRAIN_DCI_DSM_BUFFER_SIG )
  {
    diag_clear_internal_sigs( DIAG_INT_DRAIN_DCI_DSM_BUFFER_SIG );
    diag_drain_dsm_buffer( DIAG_STREAM_2 );
  }
  else if( sigs_internal & DIAG_INT_IDLE_PROCESSING_SIG )
  {
      /* Dont do anything. This is just to wake up diag to do idle processing */
      diag_clear_internal_sigs(DIAG_INT_IDLE_PROCESSING_SIG);
  }

  if( diag_get_internal_sigs() == 0 )
  {
    /* Clear real signal if all internal signals have been handled */
    return_val = osal_reset_sigs( diag_task_tcb, DIAG_INTERNAL_SIG, &return_sigs );
    ASSERT(OSAL_SUCCESS == return_val);
  }
  else
  {
    /* There are more sigs to process */
    return_val = osal_set_sigs( diag_task_tcb, DIAG_INTERNAL_SIG, &return_sigs );
    ASSERT(OSAL_SUCCESS == return_val);
  }

} /* diag_handle_internal_sigs */

#ifdef FEATURE_DIAG_STRESS_PROFILE
uint64 diag_sigs_pcycle_total=0, diag_sigs_pcycle_iterations=0;
#endif

/*===========================================================================

FUNCTION DIAG_HANDLE_SIGS

DESCRIPTION
  Handle all of the signals that interest diag.  Watch-dog is kicked as
  needed.

RETURN VALUE
  With *sigs, signals are returned.

===========================================================================*/
static osal_sigs_t
diag_handle_sigs (osal_sigs_t mask)
{
  int return_val = 0;
  boolean found = FALSE;
  static boolean bDiagDropEventRegistered = FALSE;  /* To check if the diag drop event report is regisetered for Idle Processing */
#ifdef FEATURE_DIAG_STRESS_PROFILE
  uint64 pcycle_start, pcycle_end;
#endif
  osal_sigs_t return_sigs;
  osal_sigs_t sigs = 0;    /* Signals currently set for this task. */
  osal_sigs_t busy_mask = mask |
      DIAG_TX_SIG | DIAG_DRAIN_TIMER_SIG |DIAG_RX_RSP_SIG |DIAG_EVENT_DRAIN_SIG |
      DIAG_EVENT_TIMER_SIG |DIAG_EVENT_TIMER_SEC_SIG | DIAG_EVENT_DRAIN_SEC_SIG | DIAG_TASK_STOP_SIG | DIAG_TASK_OFFLINE_SIG | DIAG_RX_DELAY_RSP_SIG ;
  osal_sigs_t composite_mask = busy_mask | DIAG_RX_SIG |
#ifdef FEATURE_DIAG_NV
    DIAG_REFRESH_CACHE_SIG    |
#endif
    DIAG_COMM_OPEN_SIG          |
#if defined DIAG_MP
    DIAG_COMM_OPEN_SMD_SIG      |
#endif
#ifdef DIAG_SMD_SUPPORT
    DIAG_CTRL_MSG_SIG         |
#endif
    DIAG_RX_DCI_SIG           |

#ifdef DIAG_SMDL_SUPPORT
    DIAG_SMDL_READ_SIG        |
    DIAG_SMDL_CMD_READ_SIG    |
    DIAG_SMDL_DCI_CMD_READ_SIG |
    DIAG_SMDL_WRITE_SIG        |
#endif // DIAG_SMDL_SUPPORT
    DIAG_INTERNAL_SIG |
    DIAG_COMM_SIO_TX_FLUSH_SIG      |
    DIAG_RPT_SIG        |
    DIAG_COMM_CLOSE_SIG;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  while (!found)
  {
    sigs = osal_thread_wait (diag_task_tcb, composite_mask);

    /* Pet the dog before doing any work. */
    diag_kick_watchdog ();

    #ifdef FEATURE_DIAG_STRESS_PROFILE
    pcycle_start = qurt_get_core_pcycles();
    #endif

     /*Register for Diag Idle Processing only if there are drops */
    if( (FALSE == bDiagDropEventRegistered) && (drop_count_f3 > 0 || drop_count_log > 0))
    {
      diag_idle_processing_register(diag_drop_event_report, NULL);
      bDiagDropEventRegistered = TRUE;
    }


    if (sigs & mask)
    {
      found = TRUE;
    }

    else if (sigs & DIAG_TASK_STOP_SIG)
    {
      return_val = osal_reset_sigs(diag_task_tcb, DIAG_TASK_STOP_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
      diag_shutdown ();
    }

      else if (sigs & DIAG_TASK_OFFLINE_SIG)
      {
        return_val = osal_reset_sigs(diag_task_tcb, DIAG_TASK_OFFLINE_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);
        diag_offline ();
      }

      else if (sigs & DIAG_COMM_CLOSE_SIG)
      {
        return_val = osal_reset_sigs(diag_task_tcb, DIAG_COMM_CLOSE_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);

#if defined (DIAG_MP_MASTER)
  diagcomm_ctrl_close (DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_1);
        diagcomm_cmd_close (DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_1);
  diagcomm_smd_close( DIAGCOMM_PORT_1 );

  diagcomm_ctrl_close (DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_2);
        diagcomm_cmd_close (DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_2);
  diagcomm_smd_close( DIAGCOMM_PORT_2 );
#endif

#if defined (DIAG_MP) && !defined (DIAG_MP_MASTER)
        diagcomm_ctrl_close (DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1);
        diagcomm_cmd_close (DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1);
#endif /* DIAG_MP && !DIAG_MP_MASTER */

        diagcomm_close ();
      }

      else if (sigs & DIAG_COMM_OPEN_SIG)
      {
        return_val = osal_reset_sigs(diag_task_tcb, DIAG_COMM_OPEN_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);
#if defined (DIAG_MP_MASTER)
        diagcomm_ctrl_open (DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_1);
        diagcomm_cmd_open (DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_1);
        diagcomm_smd_open (DIAGCOMM_PORT_1);

        diagcomm_ctrl_open (DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_2);
        diagcomm_cmd_open (DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_2);
        diagcomm_smd_open (DIAGCOMM_PORT_2);
#endif

#if defined (DIAG_MP) && !defined (DIAG_MP_MASTER)
        diagcomm_ctrl_open (DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1);
        diagcomm_cmd_open (DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1);
#endif // DIAG_MP && !DIAG_MP_MASTER

        diagcomm_sio_open (DIAGCOMM_PORT_1);
      }

      else if (sigs & DIAG_COMM_SIO_TX_FLUSH_SIG)
      {
        return_val = osal_reset_sigs(diag_task_tcb, DIAG_COMM_SIO_TX_FLUSH_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);
        diagcomm_sio_tx_flush();
      }

#ifdef DIAG_SMD_SUPPORT
      else if (sigs & DIAG_CTRL_MSG_SIG)
      {
        /* Diag is signalled to process inbound CTRL traffic */
        return_val = osal_reset_sigs(diag_task_tcb, DIAG_CTRL_MSG_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);

        diagcomm_process_ctrl();
      }
#endif // DIAG_SMD_SUPPORT

#ifdef DIAG_SMDL_SUPPORT
    else if (sigs & DIAG_SMDL_CMD_READ_SIG)
    {
      /* Signal to process smdl_event_read */
      return_val = osal_reset_sigs(diag_task_tcb, DIAG_SMDL_CMD_READ_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);

     diagcomm_smdl_buffer_read(&diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1]);
    }

    else if (sigs & DIAG_SMDL_DCI_CMD_READ_SIG)
    {
      /* Signal to process smdl_event_read */
      return_val = osal_reset_sigs(diag_task_tcb, DIAG_SMDL_DCI_CMD_READ_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);

     diagcomm_smdl_buffer_read(&diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2]);
    }

    else if (sigs & DIAG_SMDL_READ_SIG)
    {
      /* Signal to process smdl_event_read */
      return_val = osal_reset_sigs(diag_task_tcb, DIAG_SMDL_READ_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
      int i,j;

      for (i = 0; i < NUM_SMD_PORT_TYPES; i++)
      {
       for (j = 0; j < NUM_SMD_PORTS; j++)
       {
         diagcomm_smdl_buffer_read(&diagcomm_io_conn[i][j]);
         diagcomm_smdl_buffer_read(&diagcomm_io_ctrl_conn[i][j]);
       }
      }
    }
    else if (sigs & DIAG_SMDL_WRITE_SIG)
    {
      /* Signal to process smdl_event_write */
      return_val = osal_reset_sigs(diag_task_tcb, DIAG_SMDL_WRITE_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);

      /* 'wmq_q_cnt' and 'diag_prev_tx_pending' are checked inside diagcomm_io_smdl_send() */
      diagcomm_io_smdl_send(&diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1]);
      diagcomm_io_smdl_send(&diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2]);

      /*TODO: if data and control channel are made SMDL, uncomment the respective send function */
      /*diagcomm_io_smdl_send(&diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1]);
      diagcomm_io_smdl_send(&diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2]);
      diagcomm_io_smdl_send(&diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1]);
      diagcomm_io_smdl_send(&diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2]);*/

      /* TODO: if data and control channel are made SMDL, uncomment checking the status
                of corresponding channels and setting the SMDL WRITE Signal */
      /* Check if we have data in 'wm_q' to send, but FIFO wasn't full. */
      if( /* diagcomm_io_has_tx_data(&diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1]) ||
           diagcomm_io_has_tx_data(&diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2]) ||
           diagcomm_io_has_tx_data(&diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1])      ||
           diagcomm_io_has_tx_data(&diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2])      ||*/
           diagcomm_io_has_tx_data(&diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1])  ||
           diagcomm_io_has_tx_data(&diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2]) )
      {
        return_val = osal_set_sigs(diag_task_tcb, DIAG_SMDL_WRITE_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);
      }
    }
#endif // DIAG_SMDL_SUPPORT

      else if (sigs & DIAG_EVENT_TIMER_SIG)
      {
        return_val = osal_reset_sigs(diag_task_tcb, DIAG_EVENT_TIMER_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);
        diag_event_stale_timer_pending = FALSE;
        event_stale ();
      }

      else if ((sigs & DIAG_RX_SIG) != 0)
      {
        return_val = osal_reset_sigs(diag_task_tcb, DIAG_RX_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);

        diag_process_rx ();
      }

      else if ((sigs & DIAG_RX_DCI_SIG) != 0)
      {
        return_val = osal_reset_sigs(diag_task_tcb, DIAG_RX_DCI_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);

        diag_process_rx_dci ();
      }

      else if((sigs & DIAG_RX_RSP_SIG)!= 0)
      {
        return_val = osal_reset_sigs(diag_task_tcb, DIAG_RX_RSP_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);

        if (diagcomm_status(DIAGCOMM_PORT_1))
            diagpkt_rsp_send();
        diag_sleep_vote (DIAG_SLEEP_DIAG, TRUE);
      }
      else if(sigs & DIAG_RX_DELAY_RSP_SIG)
      {
        return_val = osal_reset_sigs(diag_task_tcb, DIAG_RX_DELAY_RSP_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);

        diag_sleep_vote (DIAG_SLEEP_DIAG, FALSE);
        if (diagcomm_status(DIAGCOMM_PORT_1))
            diagpkt_delay_rsp_send();
        diag_sleep_vote(DIAG_SLEEP_DIAG, TRUE);
      }

#ifdef DIAG_SMDL_SUPPORT
    else if (sigs & DIAG_SMDL_CMD_READ_SIG)
    {
      /* Signal to process smdl_event_read */
      return_val = osal_reset_sigs(diag_task_tcb, DIAG_SMDL_CMD_READ_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);

      diagcomm_smdl_buffer_read(&diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1]);
    }
#endif
      else if (sigs & DIAG_INTERNAL_SIG)
      {
        diag_handle_internal_sigs();
      }

      else if (sigs & DIAG_EVENT_DRAIN_SIG)
      {
        return_val = osal_reset_sigs(diag_task_tcb, DIAG_EVENT_DRAIN_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);
        event_drain ( mask | DIAG_COMM_SIO_TX_FLUSH_SIG);
      }
       else if (sigs & DIAG_EVENT_TIMER_SEC_SIG)
      {
        return_val = osal_reset_sigs(diag_task_tcb, DIAG_EVENT_TIMER_SEC_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);
        diag_event_stale_timer_pending_sec = FALSE;
        event_stale_sec ();
      }
      else if (sigs & DIAG_EVENT_DRAIN_SEC_SIG)
      {
        return_val = osal_reset_sigs(diag_task_tcb, DIAG_EVENT_DRAIN_SEC_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);
        event_drain_sec ( mask | DIAG_COMM_SIO_TX_FLUSH_SIG);
      }

      else if (sigs & DIAG_DRAIN_TIMER_SIG)
      {
        diagbuf_status_enum_type status;

        return_val = osal_reset_sigs(diag_task_tcb, DIAG_DRAIN_TIMER_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);

        diag_drain_timer_pending = FALSE;

        if (diagcomm_sio_flow_enabled())
        {
          /* If the drain timer is active, we assume that Diag traffic is expected in a somewhat timely manner.
             In that case, make sure existing Diag traffic gets sent before draining more. */
          diagbuf_send_pkt(NULL, DIAG_STREAM_1 | DIAG_STREAM_2);

          diag_sleep_vote(DIAG_SLEEP_DIAG, FALSE);

          status = diagbuf_drain(DIAG_TASK_STOP_SIG | mask | DIAG_RX_SIG | DIAG_RX_RSP_SIG | DIAG_EVENT_DRAIN_SIG | DIAG_EVENT_DRAIN_SEC_SIG | DIAG_COMM_SIO_TX_FLUSH_SIG | DIAG_SMDL_CMD_READ_SIG | DIAG_SMDL_WRITE_SIG);

          /* This function will kick the watchdog for us. */
          if (status == DIAGBUF_EMPTY_S)
          {
            diag_sleep_vote(DIAG_SLEEP_DIAG, TRUE);
          }
        }
      }

      else if (sigs & DIAG_TX_SIG)
      {
        diagbuf_status_enum_type status;
        return_val = osal_reset_sigs(diag_task_tcb, DIAG_TX_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);

        /* Clear the TX notification semaphore. */
        diag_tx_pending = FALSE;
        if(diagcomm_sio_flow_enabled())
        {
          /* Turn off sleep while buffering. */
          diag_sleep_vote (DIAG_SLEEP_DIAG, FALSE);

           status = diagbuf_drain (DIAG_TASK_STOP_SIG | mask | DIAG_RX_SIG | DIAG_RX_RSP_SIG | DIAG_EVENT_DRAIN_SIG | DIAG_COMM_SIO_TX_FLUSH_SIG | DIAG_SMDL_CMD_READ_SIG| DIAG_SMDL_WRITE_SIG);

          /* This function will kick the watchdog for us. */
          if (status == DIAGBUF_EMPTY_S)
          {
            diag_sleep_vote (DIAG_SLEEP_DIAG, TRUE);
          }
        }
      }

    #ifdef FEATURE_DIAG_NV
      else if (sigs & DIAG_REFRESH_CACHE_SIG)
      {
        /* The packet processing cache may have been changed. */
        return_val = osal_reset_sigs(diag_task_tcb, DIAG_REFRESH_CACHE_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);

        diagnv_refresh_cache ();
      }
    #endif

      /* See if there is anything to do.  If empty, this is a quick operation. */
      diag_idle_processing ();

      #ifdef FEATURE_DIAG_STRESS_PROFILE
      pcycle_end = qurt_get_core_pcycles();
      diag_sigs_pcycle_iterations++;
      /* May need to add protection around diag_sigs_pcycle_total if it this
         test feature gets used more extensively */
      diag_sigs_pcycle_total += pcycle_end - pcycle_start;
      #endif

  }

  return sigs;
}                /* diag_handle_sigs */

#if ((!defined(FEATURE_DIAG_SWITCH) && !defined(DIAG_FRAMING)) || (defined(FEATURE_DIAG_SWITCH)))
/*===========================================================================

FUNCTION DIAG_GET_RX_PKT_SLAVE

DESCRIPTION
  This function will retrieve the next DM packet sent by modem to app.
  The incoming packets can be a chained. This function calls dsm_pullup_tail
  and extracts the whole packet.

DEPENDENCIES
  This procedure can only be called when a full DM packet is available on
  queue from SIO.  Further, if multiple packets are enqueued, then this will
  only process the first packet.  The remaining packets are left on the queue.

RETURN VALUE
  Returns TRUE if full packet recd, else returns FALSE.

===========================================================================*/
static boolean
diag_get_rx_pkt_slave (diag_rx_pkt_type* rx_pkt)
{
  dsm_item_type *item_ptr = NULL;
  /* TRUE if found a packet boundary in HDLC stream. */
  boolean pkt_bnd = FALSE;
  unsigned int used;
  unsigned int total_req_bytes;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  item_ptr = (dsm_item_type *) diagcomm_inbound (&used, rx_pkt->port_num);

  if( item_ptr != NULL)
  {
    total_req_bytes = dsm_length_packet(item_ptr);
    if(total_req_bytes > rx_pkt->max_size)
    {
     dsm_free_packet(&item_ptr);
     MSG_2 (MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "Received Diag Req Pkt Size is %d Max Size For Req Pkt is %d \n",
             total_req_bytes, rx_pkt->max_size);
     return FALSE;
    }


    rx_pkt->pkt_len = dsm_pullup_tail(&item_ptr,rx_pkt->dest_ptr,total_req_bytes);
    item_ptr = NULL;
    pkt_bnd = TRUE;
  }

  if(rx_pkt->port_num == DIAGCOMM_PORT_1)
  {
    if(diagcomm_io_get_rx_wmq_cnt(diagcomm_get_sio_conn(DIAGCOMM_PORT_1)) > 0)
    {
      diag_rx_notify ();
    }
  }
  else
  {
    if(diagcomm_io_get_rx_wmq_cnt(diagcomm_get_sio_conn(DIAGCOMM_PORT_2)) > 0)
    {
      diag_rx_dci_notify ();
    }
  }

  return pkt_bnd;
}       /* diag_get_rx_pkt_slave */
#endif
#if ((!defined(FEATURE_DIAG_SWITCH) && defined(DIAG_FRAMING)) || (defined(FEATURE_DIAG_SWITCH)))
/*===========================================================================

FUNCTION DIAG_GET_RX_PKT

DESCRIPTION
  This function will retrieve the next DM packet.  The incoming packets come
  in pieces.  This function will do the necessary unescaping and combine the
  pieces to form a whole packet.

DEPENDENCIES
  This procedure can only be called when a full DM packet is available on
  queue from SIO.  Furhter, if multiple packets are enqueued, then this will
  only process the first packet.  The remaining packets are left on the queue.

RETURN VALUE
  Returns TRUE if full packet recd, else returns FALSE.

===========================================================================*/
static boolean
diag_get_rx_pkt (uint8 * dest_ptr,  /* Buffer for received packet. */
         unsigned int size, /* Maximum size for request packet. */
         unsigned int *pkt_len  /* Packet length when it is constructed. */
  )
{
  static diag_hdlc_decode_type hdlc;
  static boolean initialized = FALSE;
  static dsm_item_type *item_ptr = NULL;
  unsigned int used;

  /* TRUE if found a packet boundary in HDLC stream. */
  boolean pkt_bnd = FALSE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!initialized)
  {
    initialized = TRUE;
    memset (&hdlc, 0, sizeof (hdlc));
  }

  hdlc.dest_ptr = dest_ptr;
  hdlc.dest_size = size;
  *pkt_len = 0;

  do
  {
    if (hdlc.src_ptr == NULL)
    {
      if (item_ptr == NULL)
      {
        item_ptr = (dsm_item_type *) diagcomm_inbound (&used, DIAGCOMM_PORT_1);
        if (item_ptr == NULL)
         goto exit;
      }
      hdlc.src_ptr = (byte *) (item_ptr->data_ptr);
      hdlc.src_idx = 0;
      hdlc.src_size = item_ptr->used;

    }

    pkt_bnd = diag_extract_pkt (&hdlc);

    if (pkt_bnd)
    {
      *pkt_len = hdlc.dest_idx;
      hdlc.dest_idx = 0;    /* Initialize for the next packet */
    }
    /* Free the pointer if the buffer has been completely read.*/

    if (hdlc.src_idx == hdlc.src_size)
    {
       item_ptr = dsm_free_buffer(item_ptr);
       hdlc.src_ptr = NULL;
    }

  }
  while (!pkt_bnd );

  if ((hdlc.src_ptr != NULL)||(item_ptr!=NULL))
  {
    diag_rx_notify();
  }

  else
  exit:

  if(item_ptr == NULL)
  {
   if(diagcomm_get_rx_wmq_cnt(DIAGCOMM_PORT_1) > 0)
    /* There is more data available, signal diag to continue RXing */
    diag_rx_notify ();
  }

  return pkt_bnd;

}               /* diag_get_rx_pkt */
#endif
#if ((!defined(FEATURE_DIAG_SWITCH) && defined(DIAG_FRAMING)) || (defined(FEATURE_DIAG_SWITCH))|| defined(DIAG_FEATURE_EFS2))
/*===========================================================================

FUNCTION DIAG_EXTRACT_PKT

DESCRIPTION
  This function extracts a packet from a HDLC encoded stream, and verifies
  CRC is correct.

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if a valid packet was found.

===========================================================================*/
static boolean
diag_extract_pkt (diag_hdlc_decode_type * hdlc)
{
  boolean pkt_bnd = FALSE;
  /* Cyclic Redundancy Check of incoming packet. */
  word crc;

  if (hdlc && hdlc->src_ptr && hdlc->dest_ptr)
  {

    pkt_bnd = diag_hdlc_decode (hdlc);

    if (pkt_bnd == TRUE)
    {
      /* At a packet boundary, check the CRC, reset dst_idx for the next
       * packet, and return the packet (if valid). */
      crc = crc_16_l_calc (hdlc->dest_ptr, (word) ((hdlc->dest_idx - 1) * 8));

      if (crc == CRC_16_L_OK)
      {
    hdlc->dest_idx -= DIAG_ASYNC_TAIL_SIZE;
      }
      else
      {
        MSG (MSG_SSID_DIAG, MSG_LEGACY_LOW,"Bad RX CRC");
        pkt_bnd = FALSE;
        hdlc->dest_idx = 0; /* Start new packet */
      }
    }
    else if (hdlc->dest_idx >= hdlc->dest_size)
    {
      /* The inbound packet is too large.  Therefore, we cannot recieve it.
       * By setting the dest_idx field to 0, the rest of the packet will be read
       * in, and the CRC will fail. */
      hdlc->dest_idx = 0;
    }
  }

  return pkt_bnd;
}
#endif
/*=========================================================================*/
void
diag_timeout (void)
{
  #if defined (DIAG_QDSP6_APPS_PROC)
  diag_suspend (DIAG_TOUT_TIME);
  #else

  /* First, flush the TX buffer chain. */
  diagbuf_flush_timeout(DIAG_SIO_TIMEOUT_TIMER_LEN);

  #if defined(FEATURE_REX_OPAQUE_TCB_APIS)
     /* The passed in value is ignored */
     osal_autodog_enable(0);
  #else
     osal_autodog_enable(osal_thread_self()->dog_report_val);
  #endif

  diag_suspend (DIAG_TOUT_TIME);
  osal_autodog_disable();


  diagcomm_flush_rx (NULL);
  #endif

}               /* diag_timeout */

/*=========================================================================*/
void
diag_suspend (unsigned int duration)
{
  if (duration > 0)
  {
    osal_sigs_t return_sigs;
    #if defined (DIAG_QDSP6_APPS_PROC)
    (void)osal_timed_wait(diag_task_tcb, DIAG_TOUT_TIMER_SIG, &diag_tout_timer, duration);
    (void)osal_reset_sigs(diag_task_tcb, DIAG_TOUT_TIMER_SIG, &return_sigs);
    #else
    #if defined(FEATURE_REX_OPAQUE_TCB_APIS)
     osal_autodog_enable(0);
    #else
     osal_autodog_enable(osal_thread_self()->dog_report_val);
    #endif
    (void) osal_timed_wait (diag_task_tcb, DIAG_TOUT_TIMER_SIG, &diag_tout_timer, duration);
    (void) osal_reset_sigs (diag_task_tcb, DIAG_TOUT_TIMER_SIG, &return_sigs);
    osal_autodog_disable();
    #endif
  }
}

/*=========================================================================*/
void
diag_pause (unsigned int duration, boolean enable_rx_processing)
{

  if (duration > 0)
  {
    int return_val = 0;
    return_val = osal_set_timer(&diag_tout_timer, duration);
    ASSERT(OSAL_SUCCESS == return_val);

    if (enable_rx_processing)
    {
      /* Process RX packets as well. */
      (void) diag_handle_sigs (DIAG_TOUT_TIMER_SIG);
    }
    else
    {
      /* Just process TX */
      (void) diag_wait (DIAG_TOUT_TIMER_SIG);
    }
  }
}


/*===========================================================================

FUNCTION DIAG_SET_DRAIN_TIMER

DESCRIPTION
  This function sets a Diag internal signal which will in turn set the
  diag drain timer from Diag's context.

===========================================================================*/
void
diag_set_drain_timer()
{
  if ( !diag_drain_timer_pending && diag_drain_timer_enabled() )
  {
    diag_set_internal_sigs( DIAG_INT_SET_DRAIN_TIMER_SIG );
  }

} /* diag_set_drain_timer */


/*===========================================================================

FUNCTION DIAG_DRAIN_TIMER_ENABLED

DESCRIPTION
    Returns TRUE if drain timer length set is greater than 0 indicating
    drain timer is enabled.

===========================================================================*/
boolean diag_drain_timer_enabled(void)
{
  return diag_drain_timer_len > 0;
}

/*===========================================================================

FUNCTION DIAG_CLR_DRAIN_TIMER

DESCRIPTION
  This function clears the drain timer.

===========================================================================*/
void
diag_clr_drain_timer()
{
  int return_val = 0;

  if( diag_drain_timer_pending == FALSE )
    return;

  return_val = osal_reset_timer(&diag_drain_timer);
  diag_drain_timer_pending = FALSE;
  ASSERT(OSAL_SUCCESS == return_val);

} /* diag_clr_drain_timer */


/*===========================================================================

FUNCTION DIAG_TASK

DESCRIPTION
  This function contains the diagnostic task.  It processes diag commands
  for each request packet received from the external device (tool).

  This procedure does not return.

===========================================================================*/
void
diag_task (uint32 params)
{
#if defined (FEATURE_DIAG_SWITCH) && defined (FEATURE_DIAG_NV)
  nv_stat_enum_type ftm_nvstatus = NV_FAIL_S;
  uint8 diagloc_proc_decider = 0;
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DIAG_SWITCH
 diag_on_single_proc = DIAG_FTM_SWITCH_VAL;
#endif

  void *name = "diag_task";

  /*This sets diag_task_begin to 1 and for subsequent calls we return */
  if(!osal_atomic_compare_and_set((osal_atomic_word_t*)&diag_task_begin, FALSE, TRUE))
    return;

/* Keep Lint happy */
  (void) params;

  /* The BLAST OS needs an API to be called to set the name of the task.
  Necessary for debugging. This BLAST API sets the name of teh current task. */
  osal_set_task_name(name);

  /* Notify that we are ready to start. When this function returns, RCINIT
     handshake is complete and we are started. This must come before
     diag_init() to ensure SSM task is also ready before Diag task continues.
   */
  diag_ready_start_task ();

  /* Initialize task */
  diag_init ();

#if defined(DIAG_RDM_LATE_REGISTRATION)
  #if !defined (DIAG_RUNTIME_DEVMAP) && defined (DIAG_SIO_SUPPORT)
   diagcomm_sio_open_cb (SIO_PORT_NULL);
#endif /* !DIAG_RUNTIME_DEVMAP */
#if defined (DIAG_RUNTIME_DEVMAP)
  /* Register callbacks with RDM */
  rdm_register_open_func (RDM_DIAG_SRVC, diagcomm_sio_open_cb);
  rdm_register_close_func (RDM_DIAG_SRVC, diagcomm_sio_close_cb);
#else

#ifdef FEATURE_DIAG_SWITCH
#ifdef FEATURE_WINCE
  #error code not present
#endif

if(diag_on_single_proc == DIAG_FTM_SWITCH_VAL)
{
  /* Register callbacks with RDM */
  rdm_register_open_func (RDM_DIAG_SRVC, diagcomm_sio_open_cb);
  rdm_register_close_func (RDM_DIAG_SRVC, diagcomm_sio_close_cb);
}
#endif /* FEATURE_DIAG_SWITCH */
#endif /* DIAG_RUNTIME_DEVMAP */
#endif /* defined(DIAG_RDM_LATE_REGISTRATION) */



  /* The comm layer needs to be initialized before anything else.  This is
   * needed because diagbuf_init() sets callbacks that are initialized in
   * diagcomm_init(). */

#ifdef FEATURE_DIAG_SWITCH
  diag_on_single_proc = 0;
#endif

#if defined (FEATURE_DIAG_NV) && defined (FEATURE_DIAG_SWITCH)
  ftm_nvstatus = diag_nv_read (NV_DIAG_FTM_MODE_SWITCH_I, &diagloc_proc_decider);
  if(ftm_nvstatus != NV_FAIL_S)
    diag_on_single_proc = diagloc_proc_decider;
 #endif

#ifdef FEATURE_DIAG_SWITCH
  if(diag_on_single_proc != DIAG_FTM_SWITCH_VAL)
#endif

#if defined (DIAG_MP_MASTER)
 diagcomm_cmd_init(DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_1);
 diagcomm_smd_init(DIAGCOMM_PORT_1);
 diagcomm_ctrl_init(DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_1);
 diagcomm_ctrl_init(DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_2);
 diagcomm_cmd_init(DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_2);
 diagcomm_smd_init(DIAGCOMM_PORT_2);
#endif


#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
  diagcomm_ctrl_init(DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1);
  diagcomm_cmd_init(DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1);
#elif !defined(DIAG_MP)
/*Initialize the cmd wmqs as they are used to receive data
  even if channel seperation is not supported*/
  diagcomm_cmd_wmq_init(DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1);
#endif

  diagcomm_sio_init(DIAGCOMM_PORT_1);

/*Initialize the DCI ports*/
  diagcomm_cmd_init(DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_2);
  diagcomm_sio_init(DIAGCOMM_PORT_2);

  /* Register a callback to notify us when inbound data is available. */
  diagcomm_register_inbound (diag_rx_notify);

  /* Forid sleep during init. */
  diag_sleep_vote (DIAG_SLEEP_DIAG, FALSE);

  /* Read the packet processing cache.*/
#if defined(FEATURE_DIAG_NV)
  diagnv_refresh_cache ();
#endif

#if defined(FEATURE_SSM)
/*OCH- Initialization of the DCI Override Feature */
  diag_och_ssm_init();

  /*Check if DCI is allowed or not*/
  is_dci_allowed = diag_dci_open_query();
  if (is_dci_allowed)
  {
    MSG_1 (MSG_SSID_DIAG, MSG_LEGACY_LOW, "Open the DCI channel %d", 1);
    diagcomm_cmd_open (DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_2);
    diagcomm_sio_open (DIAGCOMM_PORT_2); /* Open the DCI stream */
  }
#endif

#if defined(DIAG_FEATURE_EFS2)
  /* Process filestream mode, if applicable */
/*lint -save -e{545} Suspicious use of & */
  while (diag_fs_inbound_mode (&diag_req_inbound,
                   sizeof (diag_req_inbound)) == TRUE);
/*lint -restore */

  /* The powerup event cannot be sent until the input file has processed.
     This is the only way the event service would be enabled. */
#endif /* DIAG_FEATURE_EFS2 */

  /* Send powerup event */
  event_report (EVENT_POWERUP);

  /* Vote OK to sleep for the diagnostic task. */
  diag_sleep_vote (DIAG_SLEEP_DIAG, TRUE);

    diagpkt_init_manual_dispatch_table ();

  diagbuf_tx_sleep_parameters_set();

#ifdef FEATURE_WINCE
#error code not present
#endif

  /* Clear bootup event mask */
  event_set_all_rt_masks(0, 0);

 /*----------------------------------------------------------------------------
                           Begin task operations
----------------------------------------------------------------------------*/
  /* Get task signals */
  (void) diag_handle_sigs (0);

}               /* diag_task */

#if ((!defined(FEATURE_DIAG_SWITCH) && defined(DIAG_FRAMING)) || (defined(FEATURE_DIAG_SWITCH))|| defined(DIAG_FEATURE_EFS2))
/*===========================================================================

FUNCTION DIAG_HDLC_DECODE

DESCRIPTION
  This routine provides XOR unescaping functionality for the use of callers.
  This routine is the inverse of do_escaping.

  When DIAG_ASYNC_ESC_CHAR is found, it is discarded and the next character
  is unescaped as follows <src_byte ^ DIAG_ASYNC_ESC_MASK>.  This character is
  stored in dest_ptr.

  When an unescaped DIAG_ASYNC_CONTROL_CHAR is found, this routine will exit
  immediately and indicate that an unescaped control character was found, in
  the parameter found_control_char.

  src_ptr and dest_ptr can be the same or overlapping.

DEPENDENCIES
  When an escape character straddles two packets, escaping will indicate this
  fact.  The caller needs to pass this parameter (unscathed) to
  do_unescaping in order for un-escaping to be done correctly.

RETURN VALUE
  A boolean indicating that the control character was found.

===========================================================================*/
static boolean
diag_hdlc_decode (diag_hdlc_decode_type * hdlc)
{
  byte *src_ptr = NULL, *dest_ptr = NULL;
  unsigned int src_length = 0, dest_length = 0;

  unsigned int converted_length = 0;
  unsigned int i;
  byte src_byte;
  boolean pkt_bnd = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdlc && hdlc->src_ptr && hdlc->dest_ptr &&
      (hdlc->src_size - hdlc->src_idx > 0) &&
      (hdlc->dest_size - hdlc->dest_idx > 0))
  {

    /* Copy paramters to local variables for optimized memory access */
    src_ptr = hdlc->src_ptr;
    src_ptr = &src_ptr[hdlc->src_idx];
    src_length = hdlc->src_size - hdlc->src_idx;

    dest_ptr = hdlc->dest_ptr;
    dest_ptr = &dest_ptr[hdlc->dest_idx];
    dest_length = hdlc->dest_size - hdlc->dest_idx;

    for (i = 0; i < src_length; i++)
    {
      src_byte = src_ptr[i];

      if (hdlc->escaping)
      {
        dest_ptr[converted_length++] = src_byte ^ DIAG_ASYNC_ESC_MASK;
        hdlc->escaping = FALSE;

      }
      else if (src_byte == DIAG_ASYNC_ESC_CHAR)
      {
        /* If DIAG_ASYNC_ESC_CHAR is found at the end of src_ptr,
         ** there is nothing to un-escape.  So, no choice
         ** but to die
         */
        if (i == (src_length - 1))
        {
          hdlc->escaping = TRUE;
          i++;
          break;
        }
        else
        {
          dest_ptr[converted_length++] = src_ptr[++i] ^ DIAG_ASYNC_ESC_MASK;
        }
      }
      else if (src_byte == DIAG_ASYNC_CONTROL_CHAR)
      {
        /* Found control character.
         */
        dest_ptr[converted_length++] = src_byte;
        pkt_bnd = TRUE;
        i++;
        break;
      }
      else
      {
        dest_ptr[converted_length++] = src_byte;
      }

      if (converted_length >= dest_length)
      {
        /* If more data cannot be stored into dest_ptr,
         ** exit loop
         */
        i++;
        break;
      }
    }               /* for i=0; i<src_length; i++ */

    hdlc->src_idx += i;
    hdlc->dest_idx += converted_length;
  }

  return pkt_bnd;
}               /* diag_hdlc_decode */
#endif

/*=========================================================================*/
boolean
diag_idle_processing_register (diag_idle_processing_ftype fp, void *param)
{
  int i;
  boolean status = FALSE;   /* Default to unsuccessful */

  if (fp)
  {
    /* if not yet found in the table, look for a NULL entry. */
    for (i = 0; !status && i < DIAG_IDLE_PROC_CNT; i++)
    {
      osal_lock_mutex(&diag_idle_process_mutex);
      if (diag_idle_proc_tbl[i].fp == NULL)
      {
        /* Found an empty table entry. */
        status = TRUE;

        diag_idle_proc_tbl[i].fp = fp;
        diag_idle_proc_tbl[i].param = param;
      }
      osal_unlock_mutex(&diag_idle_process_mutex);
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "diag_idle_proc_register(): fp=%p, i=%d, status=TRUE\n", fp, i);
     diag_set_internal_sigs(DIAG_INT_IDLE_PROCESSING_SIG);
    }


  }

  return status;
}

/*=========================================================================*/
boolean
diag_idle_processing_unregister (diag_idle_processing_ftype fp, void *param)
{
  int i;
  boolean status = FALSE;   /* Default to unsuccessful */

  if (fp)
  {
    /* Match and remove from table. */
    for (i = 0; !status && i < DIAG_IDLE_PROC_CNT; i++)
    {
      osal_lock_mutex(&diag_idle_process_mutex);
      if (diag_idle_proc_tbl[i].fp == fp &&
      diag_idle_proc_tbl[i].param == param)
      {
        /* Found an empty table entry. */
        status = TRUE;

        diag_idle_proc_tbl[i].fp = NULL;
        diag_idle_proc_tbl[i].param = NULL;
      }
      osal_unlock_mutex(&diag_idle_process_mutex);
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "diag_idle_proc_unregister(): fp=%p, i=%d, status=TRUE\n", fp, i);
    }
  }

  return status;
}

/*===========================================================================

FUNCTION DIAG_IDLE_PROCESSING

DESCRIPTION
  This procedure is called when the DIAG task loop has nothing else to do,
  i,e when diag is idle.

  This routine traverses the list of registered functions and calls them
  in order of registration.

  This enables clients to regain DIAG task context from async interfaces.

===========================================================================*/
static void
diag_idle_processing (void)
{
  static boolean inprogress = FALSE;
  int i;

  if (!inprogress)
  {
    /* Prevent reentrant calls. */
    inprogress = TRUE;

    /* Call all idle processing functions that are registered. */
    for (i = 0; i < DIAG_IDLE_PROC_CNT; i++)
    {
      osal_lock_mutex(&diag_idle_process_mutex);
      if (diag_idle_proc_tbl[i].fp)
      {
    diag_idle_proc_tbl[i].fp (diag_idle_proc_tbl[i].param);
      }
      osal_unlock_mutex(&diag_idle_process_mutex);
    }

    inprogress = FALSE;
  }

  return;
}

/*=========================================================================*/
/* Initial state = OK to sleep. */
/* I am inclined to make this static, but I think it would make it impossible to find for debugging. -- Richard */
uint32 diag_ballot_mask = 0;
/*This diag_sleep_state is updated in diag_allow_sleep() and diag_forbid_sleep() */
diag_sleep_state_enum_type diag_sleep_state = DIAG_SLEEP_STATE_UNKNOWN;


void
diag_sleep_vote (diag_sleep_vote_enum_type diag_svc, boolean vote)
{
  uint32 vote_mask = 0;

  /* Assumption: the initial state is that sleep is not allowed.
     This is established in diag_init (). */

  if (diag_enable_sleep_voting && diag_svc < DIAG_SLEEP_CNT)
  {
    vote_mask = 1 << diag_svc;

    if (vote == TRUE)
    {
      /* This service is OK to sleep, 0 the bit. */
      DIAG_CLR_MASK (diag_ballot_mask, vote_mask);

      /* If not connected, always allow sleep. */
      if (!DIAG_CHECK_MASK (diag_cx_state, DIAG_CX_COMM_S) || diag_ballot_mask == 0)
      {

        osal_lock_mutex(&diag_sleep_vote_mutex);

        if (diag_sleep_state != DIAG_SLEEP_STATE_ASLEEP)
        {
          diag_allow_sleep ();
          #if defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING)&& !defined (IMAGE_APPS_PROC)
            clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_DIAG);
          #endif
        }

        osal_unlock_mutex(&diag_sleep_vote_mutex);
      }
    }
    else
    {
      /* This service is not OK to sleep, set the bit to 1. */
      DIAG_SET_MASK (diag_ballot_mask, vote_mask);

      /* Must be connected to forbid sleep. */
      if (DIAG_CHECK_MASK (diag_cx_state, DIAG_CX_COMM_S) && diag_ballot_mask != 0)
      {
         osal_lock_mutex(&diag_sleep_vote_mutex);

         if (diag_sleep_state != DIAG_SLEEP_STATE_AWAKE)
         {
           diag_forbid_sleep ();
           #if defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING)&& !defined (IMAGE_APPS_PROC)
             clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_DIAG);
           #endif
         }

         osal_unlock_mutex(&diag_sleep_vote_mutex);
      }
    }
  }
}

/*===========================================================================

FUNCTION DIAG_MASKS_ENABLED

DESCRIPTION
  This function is used to determine if Diag has been activated to process
  logs, events, and/or F3 messages.

  It calculates its return value by checking two places:
  1) All mask options which vote against sleep
  2) All connection state variables which get activated because of listeners

RETURN VALUE
  Boolean indicating the whether Diag has any logging services enabled.
  FALSE indicates that none are enabled.
  TRUE indicates that at least one feature is enabled.

===========================================================================*/
boolean
diag_masks_enabled(void)
{
  boolean enabled = FALSE;
  /* These three ballot entries are for logging masks.
     If more persistent states get added in the future, they should be added to this. */
  int mask = (1 << DIAG_SLEEP_MSG) | (1 << DIAG_SLEEP_LOG) | (1 << DIAG_SLEEP_EVENT);
  enabled = diag_ballot_mask & mask;
  /* Recheck events, because they don't currently mark the sleep ballot */
  enabled = enabled || event_config || event_config_sec;
  if(!enabled)
  {
    /* These connection state mask bits are for listeners.
       Notably, listeners do not make DIAG vote against sleep on their own. */
    uint32 cx_mask = DIAG_CX_F3_LISTEN_S | DIAG_CX_F3_LISTEN_EXT_S |
      DIAG_CX_LOG_LISTEN_S | DIAG_CX_EVENT_LISTEN_S |
      DIAG_CX_LOG_LISTEN_EXT_S | DIAG_CX_EVENT_LISTEN_EXT_S;
    /* Set it here, because, if we're here, it's false. */
    enabled = DIAG_CHECK_MASK(diag_cx_state, cx_mask);
  }
  return enabled;
}

/*=========================================================================*/
diag_security_type
diag_get_security_state (void)
{
  return diag_security;
}

/*=========================================================================*/
boolean
diag_set_security_state (diag_security_type security)
{
  MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Changing security state to 0x%x", security);

  diag_security = security;

  return TRUE;
}

/*=========================================================================*/
boolean
diag_set_sp_state (diag_sp_type sp)
{
  MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Changing service programming lock state to 0x%x", sp);

  diag_sp = sp;

  return TRUE;
}

/*=========================================================================*/
diag_sp_type
diag_get_sp_state (void)
{
  return diag_sp;
}

/*=========================================================================*/
#ifdef DIAG_CHECK_SECURITY
/* This is normally defined in dload.c */
uint8 const chk_security_code_ptr[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
#endif /* DIAG_CHECK_SECURITY */

boolean
diag_check_password (void *code_ptr)
{
#if defined FEATURE_WINCE  || defined DIAG_QDSP6_APPS_PROC
    return TRUE;
#else

  int status;
  void *ptr;

  if (code_ptr == NULL)
  {
    ptr = (void *) diag_no_security_code_c;
  }
  else
  {
    ptr = code_ptr;
  }

  status = memcmp (ptr, chk_security_code_ptr, DIAG_PASSWORD_SIZE);

  return (status == 0) ? TRUE : FALSE;
#endif /* FEATURE_WINCE || DIAG_QDSP6_APPS_PROC */
}               /* diag_check_password */

/*=========================================================================*/
static int
diag_searchlist_insert (diag_searchlist_node_type list[],
                        int count, int size,
                        diag_searchlist_node_type new_node)
{
  int i;

  if (list && count >= 0 && count < size)
  {
    for (i = count - 1; i >= 0 && list[i].key > new_node.key; i--)
    {
      /* Sort nodes from the end of the list until a match is found */
      list[i+1] = list[i];
    }

    /* 'i+1' offsets the i-- of the for loop */
    list[i+1] = new_node;

    count++;
  }

  return count;
}

/*=========================================================================*/
/* This routine performs a binary search on the given searchlist.

   If found, this routine returns the key.  If duplicate entries
   exist, this routine returns one of them.

   If not found, this routine returns -1. */
static int
diag_searchlist_search (unsigned int key,
                        diag_searchlist_node_type list[],
                        int count)
{
  int low, i, high;

  if (list)
  {
    low = (-1);
    high = count;

    while (high - low > 1)
    {
      i = (high + low) / 2;

      if (key <= list[i].key)
      {
        high = i;
      }
      else
      {
        low = i;
      }
    }

    if (key != list[high].key)
    {
      /* Force an exit of the loop since 'low' is never less than -1. */
      high = (-1);
    }
    else
    {
      /* 'high' contains the matched index */
    }
  }
  else
  {
    high = (-1);
  }

  return high;
}

/*=========================================================================*/
/* returns TRUE if the listener node is found and marked as deleted
   else return FALSE */
static boolean
diag_searchlist_remove (diag_searchlist_node_type list[],
                        int count,
                        diag_searchlist_node_type* node,
                        diag_searchlist_match match_fp)
{
  int i, result;
  boolean found = FALSE;

  result = diag_searchlist_search (node->key, list, count);

  if (result >= 0)
  {
    /* Search before and after the found entry for the first match. */
    for (i = result; i < count && list[i].key == node->key ; i++)
    {
      if (match_fp)
      {
        if (match_fp (node, &list[i]))
        {
          found = TRUE;
          break; /* Avoid incrementing 'i' */
        }
      }
      else if (node->goo == list[i].goo)
      {
        /* update the remove flag of the node to TRUE */
        node->remove_flag = TRUE;
        found = TRUE;
        break; /* Avoid incrementing 'i' */
      }
    }

    if (!found)
    {
      for (i = result - 1; i >= 0 && list[i].key == node->key; i--)
      {
        if (match_fp)
        {
          if (match_fp (node, &list[i]))
          {
            found = TRUE;
            break; /* Avoid incrementing 'i' */
          }
        }
        else if (node->goo == list[i].goo)
        {
          /* update the remove flag of the node to TRUE */
          node->remove_flag = TRUE;
          found = TRUE;
          break; /* Avoid incrementing 'i' */
        }
      }
    }

  }

  return (found);
}

/*=========================================================================*/
//NOTE: If 'notify' or 'payload' are NULL, the return value will indicate
//existance of the key, but no action is taken to deliver the node.
boolean
diag_searchlist_search_all (unsigned int key,
                            diag_searchlist_type *list,
                            void *payload,
                            diag_searchlist_notify notify)
{
  int i, count;
  int result = -1;
  uint32 seq_num = 0;
  diag_searchlist_node_type *r = NULL;
  boolean found = FALSE;

  /* !!! This function assumes the caller is not in IRQ mode since it
     is not part of the externalized API of listeners. */
  if (list && list->initialized && !os_is_in_irq_mode ())
  {
    /* Replaced rex critical sections with intlocks, since we cannot
    ** acquire a lock on critcal section when tasks are locked.
    */

    count = list->count;
    r = list->r;

    result = diag_searchlist_search (key, r, count);

    found = (result >= 0) ? TRUE : FALSE;

    /* Only notify and search for duplicates if 'notify' is specified. */
    if (found && notify && payload)
    {
       /* Increment the sequence number for each successful search. */
       seq_num = list->seq_num + 1;

      /* Search before and after the found entry for duplicate entries. */
      for (i = result; i < count; i++)
      {
        if((r[i].key == key) && (FALSE == r[i].remove_flag))
        notify (seq_num, payload, &r[i]);
      }

      for (i = result - 1; i >= 0; i--)
      {
        if((r[i].key == key) && (FALSE == r[i].remove_flag))
        notify (seq_num, payload, &r[i]);
      }

      /* Save updated sequence number */
      list->seq_num = seq_num;
      /* Clean up function to free the listeners marked to be deleted */
      diag_single_listeners_cleanup(list);
    }
  }

  return (found);
}
/*=========================================================================*/
void diag_searchlist_free_node(diag_searchlist_type *list, int index)
{
  diag_searchlist_node_type node_to_free;
  int count = 0;
  int new_count = 0;
  int free_heap_index = 0;
  boolean free_heap_found = FALSE;
  diag_searchlist_node_type *new_r = NULL; /* If allocating a new array */
  diag_searchlist_node_type *temp_r = NULL; /* If allocating a new array, pointer to the old array */
  count = list->count;
  node_to_free = list->r[index];
  for(count--; index < count; index++)
  {
    /* Unlink the node from the list */
    list->r[index] = list->r[index+1];
  }
  list->r[index].key= 0;
  list->r[index].remove_flag= 0;
  list->r[index].goo= NULL;
  list->count--;
  new_count = list->count;
  /* Free the memory allocated from listeners heap */
  diag_listeners_free(node_to_free.goo);
  /*Decrement the number of listeners to be deleted*/
  list->free_count--;
  if (new_count == 0)
  {
    /* Check if the heap block is already marked as free */
    for(free_heap_index = 0; !free_heap_found && free_heap_index < list->heap_delete_count; free_heap_index++)
    {
      if(list->listeners_list_to_delete[free_heap_index] == (uint32*)list->r)
      {
        free_heap_found = TRUE;
      }
    }
    if(FALSE == free_heap_found)
    {
    /* Add the heap pointer of the old array to the list of listeners heap to be deleted */
      list->listeners_list_to_delete[list->heap_delete_count++] = (uint32*)list->r;
      list->r = NULL;
    }
#ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_searchlist_delete_free_cnt++;
    MSG_FATAL("FREE diag_searchlist_delete_free_cnt= %d,heap used = %d ",
    diag_searchlist_delete_free_cnt,diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */
    list->size = 0;
  }
  else if (list->size - new_count >= DIAG_SEARCHLIST_SIZE_INCR)
  {
    /* Delete crossed the increment threshold.  Alloc a smaller
    list and, if successful, copy the old list. */
    new_r = (diag_searchlist_node_type *) malloc (
    ((uint32)(list->size) - DIAG_SEARCHLIST_SIZE_INCR) *
    sizeof (diag_searchlist_node_type));
    if (new_r)
    {
#ifdef FEATURE_DEBUG_DIAG_HEAP
      diag_searchlist_delete_malloc_cnt++;
      MSG_FATAL("MALLOC diag_searchlist_delete_malloc_cnt=%d,heapUsed=%d ",
      diag_searchlist_delete_malloc_cnt,diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */
            /* Only copy valid entries */
            memscpy (new_r, (((uint32)(list->size) - DIAG_SEARCHLIST_SIZE_INCR) *
            sizeof (diag_searchlist_node_type)), list->r,
                    (uint32)new_count * sizeof (diag_searchlist_node_type));
      temp_r = list->r;
      list->r = new_r;
    /* Check if the heap block is already marked as free */
      for(free_heap_index = 0; !free_heap_found && free_heap_index < list->heap_delete_count; free_heap_index++)
      {
        if(list->listeners_list_to_delete[free_heap_index] == (uint32*)temp_r)
        {
          free_heap_found = TRUE;
        }
      }
      if(FALSE == free_heap_found)
      {
        /* Add the heap pointer of the old array to the list of listeners heap to be deleted */
        list->listeners_list_to_delete[list->heap_delete_count++] = (uint32*)temp_r;
      }
#ifdef FEATURE_DEBUG_DIAG_HEAP
      diag_searchlist_delete_free_cnt++;
      MSG_FATAL("FREE diag_searchlist_delete_free_cnt= %d,heap used = %d ",
      diag_searchlist_delete_free_cnt, diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */
      list->size -= DIAG_SEARCHLIST_SIZE_INCR;
      new_r = NULL;
      temp_r = NULL;
    }
  }
}
/*===========================================================================
FUNCTION DIAG_SEARCHLIST_CLEANUP
DESCRIPTION
This routine is called during diag idle processing to clean up the listeners
marked to be deleted.
===========================================================================*/
void diag_searchlist_cleanup(void* param)
{
  diag_single_listeners_cleanup(&diag_f3_listeners);
  if (diag_f3_listeners.count == 0)
  {
    DIAG_CLR_MASK (diag_cx_state, DIAG_CX_F3_LISTEN_S);
    DIAG_CLR_MASK (msg_enabled_mask, DIAG_MSG_LISTENER);
  }
  diag_single_listeners_cleanup(&log_listeners);
  if (log_listeners.count == 0)
  {
    DIAG_CLR_MASK (diag_cx_state, DIAG_CX_LOG_LISTEN_S);
  }
  diag_single_listeners_cleanup(&event_listeners);
  if (event_listeners.count == 0)
  {
    DIAG_CLR_MASK (diag_cx_state, DIAG_CX_EVENT_LISTEN_S);
  }
}
/*===========================================================================
FUNCTION DIAG_LISTENER_HEAP_CLEANUP
DESCRIPTION
This routine is called during diag idle processing to clean up the  stale listeners
 list marked to be deleted.
==========================================================================*/
void diag_listener_heap_cleanup(void* param)
{
  diag_single_listener_heap_cleanup(&diag_f3_listeners);
  diag_single_listener_heap_cleanup(&log_listeners);
  diag_single_listener_heap_cleanup(&event_listeners);
}
/*===========================================================================
FUNCTION diag_single_listener_heap_cleanup
DESCRIPTION
  This routine is called by the listener heap clean up function during diag
  idle processing.

  The heap memory chunks (stale listeners lists) which are marked as free will be freed in this
  function from heap.

===========================================================================*/
void diag_single_listener_heap_cleanup(diag_searchlist_type *list)
{
  int i = 0;
  if(list->initialized)
  {
      osal_enter_crit_sect (&list->crit_sect);
      while( list->heap_delete_count > 0 )
      {
        if(list->listeners_list_to_delete[i] != NULL)
        {
          free((void*)list->listeners_list_to_delete[i]);
          list->listeners_list_to_delete[i] = NULL;
        }
        list->heap_delete_count--;
        i++;
      }
      osal_exit_crit_sect (&list->crit_sect);
  }
}
/*===========================================================================
FUNCTION diag_single_listeners_cleanup
DESCRIPTION
  This routine is called by the listener clean up function during diag
  idle processing.

  The nodes which are flagged to be removed will be cleaned up from the
  listeners list.

===========================================================================*/
void diag_single_listeners_cleanup(diag_searchlist_type *list)
{
  int i =0;
  diag_searchlist_node_type* node;
  if(list->initialized)
  {
    osal_enter_crit_sect (&list->crit_sect);
    /* Parse through the listeners list to check for the nodes flagged to be removed */
    while( (list->free_count > 0) && (i < list->count) )
    {
      node = &list->r[i];
      if(TRUE == node->remove_flag)
      {
        /* Call the cleanup routine to free the node from the list */
        diag_searchlist_free_node(list, i);
      }
      else
      {
          i++;
      }
    }
    osal_exit_crit_sect (&list->crit_sect);
  }
}
/*=========================================================================*/
boolean
diag_searchlist_add (diag_searchlist_type *list, unsigned int key, void *goo)
{
  int new_count = 0;
  diag_searchlist_node_type node;
  diag_searchlist_node_type *new_r = NULL; /* If allocating a new array */
  diag_searchlist_node_type *temp_r = NULL; /* If allocating a new array , pointed to the old array*/
  boolean success = FALSE;
  int free_heap_index = 0;
  boolean  free_heap_found = FALSE;

  /* Fill in the node - this is passed by value */
  node.key = key;
  node.goo = goo;
  node.remove_flag = FALSE;

  if (list && !os_is_in_irq_mode ())
  {
    if (!list->initialized)
    {

      osal_lock_mutex(&diag_srch_list_mutex);

      /* This check is done twice to avoid mutexing for every check
         of initialization and multiple initialization.
         Too bad C doesn't have constructors... */
      if (!list->initialized)
      {
        if(osal_atomic_compare_and_set((osal_atomic_word_t*)&listener_cleanup_init, FALSE, TRUE))
        {
          diag_idle_processing_register(diag_searchlist_cleanup, NULL);
        }
        if(osal_atomic_compare_and_set((osal_atomic_word_t*)&listener_heap_cleanup_init, FALSE, TRUE))
        {
          diag_idle_processing_register(diag_listener_heap_cleanup, NULL);
        }
        osal_init_crit_sect (&list->crit_sect);
        list->initialized = TRUE;
        list->seq_num = 0;
      }

      osal_unlock_mutex(&diag_srch_list_mutex);


    }

    /* Enter critical section to avoid corrupt list */
    osal_enter_crit_sect (&list->crit_sect);

    /* If the list doesn't have an array allocated, allocate one. */
    if (!list->r)
    {
      list->count = 0;
      list->size = DIAG_SEARCHLIST_SIZE_INCR;

      list->r = (diag_searchlist_node_type *) malloc (
        (uint32)(list->size) * sizeof (diag_searchlist_node_type));

      if (!list->r)
      {
        /* reset size */
        list->size = 0;
      }
    }

    if (list->r)
    {
#ifdef FEATURE_DEBUG_DIAG_HEAP
      diag_searchlist_add_malloc_cnt++;
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "MALLOC diag_searchlist_add_malloc_cnt = %d, allocated bytes = %d ",
            diag_searchlist_add_malloc_cnt,
            ((list->size) * sizeof (diag_searchlist_node_type)));
#endif /* FEATURE_DEBUG_DIAG_HEAP */

      if (list->count >= list->size)
      {
        /* There isn't enough room in this array to add.  Allocate
           a new one and, if successful, copy the old list. */
        new_r = (diag_searchlist_node_type *) malloc (
          ((uint32)(list->size) + DIAG_SEARCHLIST_SIZE_INCR) *
          sizeof (diag_searchlist_node_type));

        if (new_r)
        {
#ifdef FEATURE_DEBUG_DIAG_HEAP
          diag_searchlist_add_malloc_cnt++;
          MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "MALLOC diag_searchlist_add_malloc_cnt = %d, allocated bytes = %d ",
             diag_searchlist_add_malloc_cnt,
             (((list->size) + DIAG_SEARCHLIST_SIZE_INCR) *
                            sizeof (diag_searchlist_node_type)));
#endif /* FEATURE_DEBUG_DIAG_HEAP */

          /* Only copy valid entries */
          memscpy (new_r, (((uint32)(list->size) + DIAG_SEARCHLIST_SIZE_INCR) * sizeof (diag_searchlist_node_type)),
                   list->r, (uint32)(list->count) * sizeof (diag_searchlist_node_type));

          list->size += DIAG_SEARCHLIST_SIZE_INCR;

          temp_r = list->r;
          list->r = new_r;
          /* Check if the heap block is already marked as free */
          for(free_heap_index = 0; !free_heap_found && free_heap_index < list->heap_delete_count; free_heap_index++)
          {
            if(list->listeners_list_to_delete[free_heap_index] == (uint32*)temp_r)
            {
              free_heap_found = TRUE;
            }
          }
          if(FALSE == free_heap_found)
          {
            /* Add the heap pointer of the old array to the list of listeners heap to be deleted */
            list->listeners_list_to_delete[list->heap_delete_count++] = (uint32*)temp_r;
          }
#ifdef FEATURE_DEBUG_DIAG_HEAP
          diag_searchlist_add_free_cnt++;
          MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
                "FREE diag_process_rx_free_cnt = %d ",
                 diag_searchlist_add_free_cnt);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

          new_r = NULL;
        }
      }

      if (list->count < list->size)
      {
        new_count = diag_searchlist_insert (list->r, list->count,
                                            list->size, node);

        if (new_count > list->count)
        {
          list->count = new_count;
          success = TRUE;
        }
        else if (list->count == 0)
        {
          free_heap_found = FALSE;
        /* Check if the heap block is already marked as free */
          for(free_heap_index = 0; !free_heap_found && free_heap_index < list->heap_delete_count; free_heap_index++)
          {
            if(list->listeners_list_to_delete[free_heap_index] == (uint32*)list->r)
            {
              free_heap_found = TRUE;
            }
          }
          if(FALSE == free_heap_found)
          {
                /* Add the heap pointer of the old array to the list of listeners heap to be deleted */
            list->listeners_list_to_delete[list->heap_delete_count++] = (uint32*)list->r;
            list->r = NULL;
          }
#ifdef FEATURE_DEBUG_DIAG_HEAP
          diag_searchlist_add_free_cnt++;
          MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
                "FREE diag_process_rx_free_cnt = %d ",
                diag_searchlist_add_free_cnt);
#endif /* FEATURE_DEBUG_DIAG_HEAP */
          list->size = 0;

        }
      }
    }

    osal_exit_crit_sect (&list->crit_sect);
  }

  return success;
}

/*=========================================================================*/
boolean
diag_searchlist_delete (diag_searchlist_type *list,
                        diag_searchlist_node_type *node,
                        diag_searchlist_match match_fp)
{
  boolean success = FALSE;

  if (list && list->initialized && !os_is_in_irq_mode ())
  {
    /* Enter critical section to make sure the list doesn't change */
    osal_enter_crit_sect (&list->crit_sect);

    if (list->r)
    {
      success = diag_searchlist_remove (
        list->r, list->count, node, match_fp);

      if(success)

        {

        /* Increment the number of listeners to be deleted*/



        list->free_count++;


      }
    }

    osal_exit_crit_sect (&list->crit_sect);
  }

  return success;
}

/*=========================================================================*/
boolean
diag_cmd_req (const byte *req, unsigned int length,
              const diag_cmd_rsp rsp_cb, void *param)
{
  diag_req_cmd_type *cmd = NULL;
  boolean success = FALSE;

  if (req && length > 0)
  {
    cmd = (diag_req_cmd_type *) diag_malloc (
      FPOS (diag_req_cmd_type, req) + length);

    if (cmd)
    {
#ifdef FEATURE_DEBUG_DIAG_HEAP
      diag_cmd_req_cnt++;
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "MALLOC diag_cmd_req_cnt = %d, heap used=%d ",diag_cmd_req_cnt,
          diag_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */

      cmd->rsp_func = rsp_cb;
      cmd->param = param;
      cmd->length = length;
      cmd->diag_mv_cmd = TRUE;
      memscpy (cmd->req, length, req, length);

      q_put (&diag_req_cmd_q, q_link (cmd, &cmd->link));

#ifdef FEATURE_DEBUG_DIAG_HEAP
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "Q_PUT diag_cmd_req diag_req_cmd_q_cnt = %d heap used = %d",
        q_cnt (&diag_req_cmd_q),  diag_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */

      /* DIAG checks this queue as part of its normal RX processing. */
      diag_rx_notify ();

      success = TRUE;
    }
  }

  /* The handle is the address of the command buffer */
  return success;
}

#if defined (DIAG_FEATURE_EFS2)
/*===========================================================================

FUNCTION DIAG_FS_INIT

DESCRIPTION
  This function initializes diag filestream mode, enabling it if applicable.

===========================================================================*/
static void
diag_fs_init (diag_fs_type * fs)
{
  int fs_rsp;
  fs_off_t result;

  if (fs && fs->handle < 0)
  {
    fs_rsp = efs_open (fs->fname, O_RDONLY);

    if (fs_rsp < 0)
    {
        /***********************************************************************
           no error case as if the file does not exist then nothing is read from
           it and configured. This can be in case of a fresh boot or an erase of
           efs.
         ***********************************************************************/
    }
    else
    {
        fs->handle = fs_rsp;

        result = efs_lseek(fs->handle, 0, SEEK_CUR);
        if (result < 0)
      {
            diag_fs_terminate(fs);
      }
      else
      {
            fs->read_pos = (dword)result;
      }
    }
  }
}

/*===========================================================================

FUNCTION DIAG_PROCESS_INPUT_FILE

DESCRIPTION
  This function processes inbound data from the specified file.

RETURN VALUE
  None.

===========================================================================*/
static void diag_process_input_file (diag_fs_type * fs,void *buf,
                                     unsigned int size)
{
  MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Processing input file");

  diag_fs_process_inbound (fs, buf, size);

  /* This is unnecessary if the input file is closed. */
  if (fs->handle >= 0)
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Attempting inbound comm buffering.");
    diag_fs_inbound_comm (&diag_fs_inbuf, buf, size);
  }
}

/*===========================================================================

FUNCTION DIAG_FS_INBOUND_MODE

DESCRIPTION
  This function handles all inbound operations for the filestream mode.

===========================================================================*/
static boolean
diag_fs_inbound_mode (void *buf, unsigned int size)
{
  static boolean initialized = FALSE;
  boolean filestream_mode_active = FALSE;

  if (!initialized)
  {
    initialized = TRUE;

    /* Initialize diag input file streams. */
    diag_fs_init (&diag_fs_config);

    diag_fs_init (&diag_fs_in);
  }

  ASSERT (buf && size > 0);

  /* If filestream mode is active, process the input file, then
     detect valid comm layer connection and buffer any inbound data.
     Once the input file is complete, the buffered inbound data will
     be processed. */
  if (diag_fs_config.handle >= 0)
  {
    filestream_mode_active = TRUE;
    diag_process_input_file(&diag_fs_config, buf, size);
  }
  else if (diag_fs_in.handle >= 0)
  {
    filestream_mode_active = TRUE;
    diag_process_input_file(&diag_fs_in, buf, size);
  }

  else if (diag_fs_inbuf.handle >= 0)
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Input file complete");
    filestream_mode_active = TRUE;

    if (DIAG_CHECK_MASK (diag_cx_state, DIAG_CX_COMM_S))
    {
      /* Since we have established a connection, the temporary file must
         contain at least one good packet. */
      MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Processing buffered comm input");
      diag_fs_process_inbound (&diag_fs_inbuf, buf, size);

      if (diag_fs_inbuf.handle >= 0)
      {
        /* Some data still exists in the file, continue comm layer buffering,
           if inbound comm layer data exists.  If this state continues too long,
           the max file size of the buffer file will be reached and no more data
           will be written.  At that point, the processing of the stream can
           "catch up" to the comm layer buffering. */
        MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Attempting buffer of inbound comm");
        diag_fs_inbound_comm (&diag_fs_inbuf, buf, size);
      }
    }
    else
    {
      /* At this point, the procesing of the original input file is
         complete and we haven't yet established a connection on the comm
         layer.  If the buffer file has data, it may or may not be useful
         data.  Since the buffer emulates normal comm layer processing,
         "catch up" to the comm layer and then exit filestream mode. */
      MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Catch up: buffered comm input");

      while (diag_fs_inbuf.handle >= 0)
      {
        diag_fs_process_inbound (&diag_fs_inbuf, buf, size);
      }

      /* Input buffer processing is complete, remove the file. */
      diag_fs_terminate (&diag_fs_inbuf);
    }
  }

  return filestream_mode_active;
}

/*===========================================================================

FUNCTION DIAG_FS_TERMINATE

DESCRIPTION
  This function closes and deletes the specified file.

===========================================================================*/
static void
diag_fs_terminate (diag_fs_type * fs)
{
  int fs_rsp;

  if(fs)
  {
    if (fs->handle >= 0)
    {
      /* Clean up input file */
      fs_rsp = efs_close(fs->handle);
      if(fs_rsp == -1)
        MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"efs_close() failed");
      fs->handle = -1;
    }

    /* Remove the input file. */
      if (efs_unlink(fs->fname))
      ERR ("Input file delete fail! Stat: %d", efs_errno, 0, 0);
  }
}

/*===========================================================================

FUNCTION DIAG_FS_READ

DESCRIPTION
  This function reads inbound data from the specified filestream.

RETURN VALUE
  Number of bytes read.

===========================================================================*/
static uint32
diag_fs_read (diag_fs_type * fs, void *buf, unsigned int size)
{
  int fs_rsp;
  fs_off_t result;
  unsigned int read_count = 0;

  if (fs && buf && fs->handle >= 0 && size > 0)
  {
    result = efs_lseek (fs->handle, fs->read_pos, SEEK_SET);

    if (result >= 0)
    {
      fs_rsp = efs_read (fs->handle, buf, (fs_size_t) size);

      if (fs_rsp != 0 && fs_rsp != -1)
      {
        read_count = fs_rsp;

        result = efs_lseek (fs->handle, 0, SEEK_CUR);

        if (result >= 0)
        {
            fs->read_pos = (dword)result;
        }
        else
        {
          diag_fs_terminate (fs);
        }
      }
      else
      {
        diag_fs_terminate (fs);
      }
    }
    else
    {
      diag_fs_terminate (fs);
    }
  }

  return read_count;
}

/*===========================================================================

FUNCTION DIAG_FS_WRITE

DESCRIPTION
  This function writes data to the file specified by 'fs'.

RETURN VALUE
  Boolean indicating success.

===========================================================================*/
static boolean
diag_fs_write (diag_fs_type * fs, void *buf, unsigned int size)
{
  fs_off_t result;
  fs_ssize_t write_result;
  boolean success = FALSE;

  if (fs && fs->handle)
  {
    /* Assumption: all writes are at the end of the file. */
    result = efs_lseek (fs->handle, (fs_off_t) fs->write_pos, SEEK_SET);

    if (result >= 0)
    {

      write_result = efs_write (fs->handle, buf, (fs_size_t) size);

      if (write_result != -1 && write_result == size)
      {
        success = TRUE;
        fs->write_pos += size;
      }
      else
      {
         diag_fs_terminate (fs);
      }
    }
    else
    {
      diag_fs_terminate (fs);
    }
  }
  else
  {
    diag_fs_terminate (fs);
  }

  return success;
}

/*===========================================================================

FUNCTION DIAG_FS_PROCESS_INBOUND

DESCRIPTION
  This function processes inbound data from the specified file.

RETURN VALUE
  None.

===========================================================================*/
static void
diag_fs_process_inbound (diag_fs_type * fs, void *buf, unsigned int size)
{
  diag_hdlc_decode_type hdlc;
  unsigned int src_buf_size = 0;

  /* Length of packet, if boundary is found.
     0 is returned if no packet found. */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize HDLC structure */
  memset (&hdlc, 0, sizeof (hdlc));

  /* Allocate a buffer from the comm layer to use as a work buffer
     for the file stream input processing. */
  src_buf_size = 256;
  hdlc.src_ptr = diag_malloc (src_buf_size);

  if (hdlc.src_ptr)
  {
#ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_fs_process_inbound_malloc_cnt++;
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "MALLOC diag_fs_process_inbound_malloc_cnt = %d,heap used=%d ",
            diag_fs_process_inbound_malloc_cnt,diag_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    if (fs && fs->handle >= 0)
    {
      hdlc.dest_ptr = buf;
      hdlc.dest_size = size;

      do
      {
        /* If the src buffer has been exhausted, time to read more */
        if (hdlc.src_size == hdlc.src_idx)
        {
          hdlc.src_size = (uint32)diag_fs_read (fs, hdlc.src_ptr, src_buf_size);

          if (hdlc.src_size > 0)
          {
            hdlc.src_idx = 0;
          }
          else
          {
            diag_fs_terminate (fs);
            break;
          }
        }

        if (hdlc.src_size > hdlc.src_idx)
        {
          if (diag_extract_pkt (&hdlc) == TRUE)
          {

            /* dest_idx is the length of the decoded packet */
            diagpkt_process_request (buf, (uint16) hdlc.dest_idx, NULL, NULL,FALSE);

            /* If src_ptr is not exhausted, update read_pos before exit. */
            if (hdlc.src_size > hdlc.src_idx)
            {
              fs->read_pos -= hdlc.src_size - hdlc.src_idx;
            }
            break;
          }
        }
      }
      while (fs->handle >= 0);
    }

    diag_free (hdlc.src_ptr);
#ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_fs_process_inbound_free_cnt++;
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "FREE diag_fs_process_inbound_free_cnt = %d, heap used = %d ",
            diag_fs_process_inbound_free_cnt,diag_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */
  }
}

/*===========================================================================

FUNCTION DIAG_FS_INBOUND_COMM

DESCRIPTION
  This function buffers inbound comm layer data to the file 'fs'.

  A temporary buffer is used to decode the inbound data and check for a
  valid packet.

RETURN VALUE
  None.

===========================================================================*/
static void
diag_fs_inbound_comm (diag_fs_type * fs, void *buf, unsigned int size)
{
  int fs_rsp;
  fs_off_t result;
  struct fs_stat fs_stat_buf;
  diag_hdlc_decode_type hdlc;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize HDLC structure */
  memset (&hdlc, 0, sizeof (hdlc));

  if (fs)
  {
    if (fs->handle < 0)
    {
      if (!(efs_stat (fs->fname, &fs_stat_buf)))
      {
        /* here no error checking is done as we are only dealing with regular files
           so if the efs_stat fails we do nothing here. now if an unlink fails we
           are not in an error case as we open a new same file again which should
           overwrite the earlier file.*/
        efs_unlink (fs->fname);
      }  /* If file stat fails, then unlink the file  */

      fs_rsp = efs_open (fs->fname, O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);

      if (fs_rsp >= 0)
      {
        fs->handle = fs_rsp;
        result = efs_lseek (fs->handle, 0, SEEK_CUR);

        if (result >= 0)
        {
          fs->read_pos = (dword)result;
          fs->write_pos = (dword)result;
        }
      }
    }
    /* Initialize HDLC structure w/ working buffer. */
    hdlc.dest_ptr = buf;
    hdlc.dest_size = size;

    /* If the file is open, buffer to the file. */
    while (fs->handle >= 0)
    {

      if (hdlc.src_ptr == NULL)
      {

        /* Make sure the file is not at max size yet. */
        if (fs->write_pos < fs->max_size)
        {
          /* Get a buffer from the comm layer.  If a valid packet is found,
          declare a "connected" status and buffer the input to the file. */
          hdlc.src_ptr = diagcomm_inbound (&hdlc.src_size, DIAGCOMM_PORT_1);
          hdlc.src_idx = 0;
        }
      }

      if (hdlc.src_ptr == NULL)
      {
        break;
      }

      if (DIAG_CHECK_MASK (diag_cx_state, DIAG_CX_COMM_S))
      {
        /* We are already connected, so just buffer the data and move on. */
        (void) diag_fs_write (fs, hdlc.src_ptr, hdlc.src_size);
        diagcomm_inbound_complete (hdlc.src_ptr);
        hdlc.src_ptr = NULL;
      }
      else
      {
        /* Not connected - must look for a valid packet to connect. */
        if (hdlc.src_idx < hdlc.src_size)
        {
          if (diag_extract_pkt (&hdlc) == TRUE)
          {
            /* Valid packet means we can assume a connection. */
            DIAG_SET_MASK (diag_cx_state, DIAG_CX_COMM_S);
          }
        }
        else
        {
          /* The input buffer has been exhausted.  Free the buffer.  The
             next iteration of the loop will grab a new one. */
          if (hdlc.dest_idx > 0)
          {
            /* This indicates that a decode is in progress.  Write the
               buffer to the file in case it is part of the first
               valid packet. */
            (void) diag_fs_write (fs, hdlc.src_ptr, hdlc.src_size);
          }
          else
          {
            /* Delete any accumulated data thus far and reopen.  Since
               there is no decode in progress, and diag is not connected,
               all inbound data thus far is garbage. */
            diag_fs_terminate (fs);
          }

          /* Finished with this inbound buffer. */
          diagcomm_inbound_complete (hdlc.src_ptr);
          hdlc.src_ptr = NULL;
        }
      }
    }
  }
}

/*===========================================================================

FUNCTION DIAG_FS_PROCESS_CONFIG

DESCRIPTION
  This function checks if the given packet has potentially changed the
  existing diag config file.  If so, a new file is generated and writen
  to the file.

===========================================================================*/
static void
diag_fs_process_config (void *req_pkt, word pkt_len)
{
  (void) pkt_len; /* suppress compiler warning */

  if (req_pkt)
  {
    switch (diagpkt_get_cmd_code (req_pkt))
    {
      //TODO: For now, only event reporting is supported.
      case DIAG_EVENT_REPORT_F:
        /* Save the diagnostics service configuration for the next power cycle. */
        diag_fs_write_config ();
        break;
      default:
        break;
    }
  }
}

/*===========================================================================

FUNCTION DIAG_FS_WRITE_CONFIG

DESCRIPTION
  This function saves the diagnostic services' configurations to a file to be
  read at the next power cycle.  The config packet is stored in the form of
  a raw (HDLC encoded) input stream with commands to configure the
  appropriate services.

===========================================================================*/
word diag_do_escaping (const byte * src_ptr, byte * dest_ptr, word src_length,
               word dest_length, unsigned int *src_used);

static void
diag_fs_write_config (void)
{
  union
  {
    event_cfg_req_type pkt;
    byte buf[sizeof (event_cfg_req_type) + 3];
  }
  event_pkt;
  uint16 event_pkt_len = sizeof (event_cfg_req_type);
  unsigned int encoded_pkt_len = 0;

  byte buf[20];         //XXX
  word crc;
  diag_fs_type fs;
  int fs_rsp;

  memset (&fs, 0, sizeof (diag_fs_type));

  fs.handle = -1;
  fs.fname = ".DIAGCFG";
  fs.max_size = 0x8000;     /* 32K is more than enough - it will never get this big. */

  /* here no error checking is done as we are only dealing with regular files
     so if the unlink fails we are not in an error case as we open a new same
     file again which should overwrite the earlier file.*/
  efs_unlink (fs.fname);

  fs_rsp = efs_open (fs.fname, O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);

  if (fs_rsp >= 0)
  {
    fs.handle = fs_rsp;
  }

  if (fs.handle >= 0)
  {
    diagpkt_set_cmd_code (&event_pkt, DIAG_EVENT_REPORT_F);

    event_pkt.pkt.enable = (event_config != 0) ? TRUE : FALSE;

    crc = crc_16_l_calc ((void *) &event_pkt,
             (uint16) (sizeof (event_cfg_req_type) * 8));

    /* Write the CRC to the end of unescaped packet.
       ** The CRC will be escaped with everything else.
     */
    event_pkt.buf[event_pkt_len++] = crc & 0x00ff;
    event_pkt.buf[event_pkt_len++] = crc >> 8;

    /* Reuse event_pkt_len to now hold size of HDLC encoded packet */
    event_pkt_len = diag_do_escaping (event_pkt.buf, buf, event_pkt_len, 20,
                      &encoded_pkt_len);
    if(event_pkt_len < 20)
    {
        buf[event_pkt_len++] = DIAG_ASYNC_CONTROL_CHAR;
    }

    (void) diag_fs_write (&fs, buf, event_pkt_len);

    efs_close (fs.handle);
  }
}


/*===========================================================================

FUNCTION DIAG_DO_ESCAPING

DESCRIPTION
  This routine provides XOR escaping functionality for the use of callers.
  Both Control character and Escape character will be escaped by substituting
  the character with the following two characters:

  <DIAG_ASYNC_ESC_CHAR> <src_byte ^ DIAG_ASYNC_ESC_MASK>

DEPENDENCIES
  src_ptr & dest_ptr cannot be the same nor overlapping.

RETURN VALUE
  Returns the number of bytes that were stored into dest_ptr.

  Also, via pointers, returns src_used.

===========================================================================*/
word
diag_do_escaping (
  const byte *src_ptr,
  /* This is the pointer that contains the data that needs to be
  ** escaped
  */

  byte       *dest_ptr,
  /* This is the pointer into which the escaped data should be
  ** stored.
  */

  word        src_length,
  /* This is the number of bytes to convert.
  */

  word        dest_length,
  /* This is the number of bytes that can be stored in dest_ptr.
  */

  unsigned int *src_used
  /* This will contain the number of values which were read from
  ** src_ptr.
  */
)
{
  word i;
  byte src_byte;
  word converted_length = 0;
  word check_size;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (dest_length >= DIAG_ESC_EXPAND_SIZE);
  ASSERT (src_ptr != NULL);
  ASSERT (dest_ptr != NULL);

  if (dest_length >= (src_length << DIAG_ESC_EXPAND_SHIFT))
  {
    /* Do not need to check for size for each character
     */
    converted_length = src_length;

    for (i = src_length; i > 0; i--)
    {
      src_byte = *src_ptr++;

      if ((src_byte == DIAG_ASYNC_CONTROL_CHAR) ||
      (src_byte == DIAG_ASYNC_ESC_CHAR))
      {

        *dest_ptr++ = DIAG_ASYNC_ESC_CHAR;
        *dest_ptr++ = src_byte ^ DIAG_ASYNC_ESC_MASK;

        /* Destination became one character
        ** larger than projected.
        */
        converted_length++;
      }
      else
      {
        *dest_ptr++ = src_byte;
      }
    }               /* for i=0; i<src_length; i++ */

    *src_used = src_length;
  }             /* if dest_length >= src_length << 1 */
  else
  {
    check_size = dest_length - DIAG_ESC_EXPAND_SIZE;

    /* Do need to check for size.  Right now check size for
       ** each character.  Could further optimize, which would
       ** make code more complicated but faster.
     */

    for (i = src_length; i > 0; i--)
    {
      src_byte = *src_ptr++;

      if ((src_byte == DIAG_ASYNC_CONTROL_CHAR) ||
      (src_byte == DIAG_ASYNC_ESC_CHAR))
      {

        *dest_ptr++ = DIAG_ASYNC_ESC_CHAR;
        *dest_ptr++ = src_byte ^ DIAG_ASYNC_ESC_MASK;
        converted_length += DIAG_ESC_EXPAND_SIZE;
      }
      else
      {
        *dest_ptr++ = src_byte;
        converted_length++;
      }

      if (converted_length > check_size)
      {
        i--;
        break;
      }
    }               /* for i=0; i<src_length; i++ */

    *src_used = src_length - i;
  }             /* else if dest_length < src_length << 1 */

  return converted_length;
}               /* diag_do_escaping */
#endif /* DIAG_FEATURE_EFS2  */

/*===========================================================================

FUNCTION DIAGPKT_CHECK_VALIDITY_REQUEST_ONDEVICE

DESCRIPTION
  This procedure checks if the given request is one of the few commands
  which is allowed to be processed.

RETURN VALUE
  Boolean indicating success.

===========================================================================*/
boolean diagpkt_check_validity_request_ondevice (void * buf)
{
  uint8 packet_id;
  uint8 subsys_id;
  uint16 ss_cmd;

  if(!buf)
    return FALSE;

  packet_id = diagpkt_get_cmd_code (buf);

  if (packet_id == 0x4B)
  {
    subsys_id = diagpkt_subsys_get_id (buf);
    ss_cmd = diagpkt_subsys_get_cmd_code (buf);
    /* Packets with SSID which are allowed */
    switch (subsys_id) {
      case 0x04: /* DIAG_SUBSYS_WCDMA */
        if ((ss_cmd == 0) || (ss_cmd == 0xF))
          return TRUE;
        break;
      case 0x08: /* DIAG_SUBSYS_GSM */
        if ((ss_cmd == 0) || (ss_cmd == 0x1))
          return TRUE;
        break;
      case 0x09: /* DIAG_SUBSYS_UMTS */
      case 0x0F: /* DIAG_SUBSYS_CM */
        if (ss_cmd == 0) return TRUE;
        break;
      case 0x0C: /* DIAG_SUBSYS_OS */
        if ((ss_cmd == 2) || (ss_cmd == 0x100)) return TRUE; /* MPU and APU */
        break;
      case 0x12: /* DIAG_SUBSYS_DIAG_SERV */
        if ((ss_cmd == 0) || (ss_cmd == 0x6) || (ss_cmd == 0x7) || (ss_cmd == 0x0060) || (ss_cmd == 0x0061) || (ss_cmd == 0x0062))
          return TRUE;
        break;
      case 0x13: /* DIAG_SUBSYS_FS */
        if ((ss_cmd == 0) || (ss_cmd == 0x1))
          return TRUE;
        break;
      default:
        return FALSE;
    }
  }
  else
  {
    switch (packet_id) {
      case 0x00:    /* Version Number */
      case 0x0C:    /* CDMA status packet */
      case 0x1C:    /* Diag Version */
      case 0x1D:    /* Time Stamp */
      case 0x60:    /* Event Report Control */
      case 0x63:    /* Status snapshot */
      case 0x73:    /* Logging Configuration */
      case 0x7C:    /* Extended build ID */
      case 0x7D:    /* Extended Message configuration */
      case 0x81:    /* Event get mask */
      case 0x82:    /* Set the event mask */
        return TRUE;
      default:
        return FALSE;
    }
  }
  return FALSE;
}

void diag_control_sleep_voting(unsigned int enable_vote)
{
  diag_enable_sleep_voting = enable_vote;
}

/*===========================================================================

FUNCTION DIAG_CTRL_DRAIN_TIMER_LEN

DESCRIPTION
  This function is used to update the global drain timer length.

===========================================================================*/

void diag_ctrl_drain_timer_len(unsigned int drain_timer_len)
{
  diag_drain_timer_len = drain_timer_len;

  /* If mode is real time, set the TX signal to start draining from diagbuf
        without waiting for the drain timer to expire  */
  if(diag_real_time_mode)
    diag_tx_notify();

  if( drain_timer_len > 0 )
    diag_set_drain_timer();
  else
    diag_clr_drain_timer();
}

/*===========================================================================

FUNCTION DIAG_GET_INTERNAL_SIGS

DESCRIPTION
    Retrieves the mask of internal signals set. This function should be used
    to read diag_internal_sigs instead of accessing it directly to avoid
    concurrency issues.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  Returns the 32bit internal signal mask value.

SIDE EFFECTS

===========================================================================*/
dword diag_get_internal_sigs(void )
{
  osal_sigs_t return_sigs;

  osal_lock_mutex(&diag_internal_sig_mutex);
  return_sigs= diag_internal_sigs;
  osal_unlock_mutex(&diag_internal_sig_mutex);

  return return_sigs;

} /* diag_get_internal_sigs */
/*===========================================================================

FUNCTION DIAG_SET_INTERNAL_SIGS

DESCRIPTION
  Sets Diag internal signal mask, which also sets DIAG_INTERNAL_SIG. When
  diag_handle_sigs() handles DIAG_INTERNAL_SIG, diag_handle_internal_sig()
  is executed to handle all Diag internal sigs. This function should be used
  to update diag_internal_sigs to avoid concurrency issues.

PARAMETERS
  sigs - Diag internal signals to set.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
dword diag_set_internal_sigs( dword sigs )
{
  osal_sigs_t return_sigs;
  int return_val = 0;
  dword prev_sigs = 0;

  prev_sigs = diag_internal_sigs;
  osal_lock_mutex(&diag_internal_sig_mutex);
  diag_internal_sigs |= sigs;
  osal_unlock_mutex(&diag_internal_sig_mutex);

  return_val = osal_set_sigs( diag_task_tcb, DIAG_INTERNAL_SIG, &return_sigs );
  ASSERT(OSAL_SUCCESS == return_val);

  return prev_sigs;

} /* diag_set_internal_sigs */


/*===========================================================================

FUNCTION DIAG_CLEAR_INTERNAL_SIGS

DESCRIPTION
  Clears Diag internal sig.  If all Diag internal sigs are cleared,
  DIAG_INTERNAL_SIG is also cleared. This function should be used
  to update diag_internal_sigs to avoid concurrency issues.

PARAMETERS
  sigs - Diag internal signals to clear.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
dword diag_clear_internal_sigs( dword sigs )
{
  osal_sigs_t return_sigs;
  int return_val = 0;
  dword prev_sigs = 0;

  osal_lock_mutex(&diag_internal_sig_mutex);
  prev_sigs = diag_internal_sigs;
  sigs = ~sigs;
  diag_internal_sigs &= sigs;

  /* Clear signal if no more internal signals are set */
  if( ~(diag_internal_sigs & 0xFFFFFFFF) )
  {
    return_val = osal_reset_sigs(diag_task_tcb, DIAG_INTERNAL_SIG, &return_sigs);
    ASSERT(OSAL_SUCCESS == return_val);
  }
  osal_unlock_mutex(&diag_internal_sig_mutex);

  return prev_sigs;

} /* diag_clear_internal_sigs */


/*===========================================================================

FUNCTION DIAG_SET_CURRENT_PRESET_MASK_INDEX

DESCRIPTION

PARAMETERS
  Preset mask index to set.

DEPENDENCIES
  None

RETURN VALUE
  0 - Success
  Otherwise, error:
    1 - Preset id not supported.

SIDE EFFECTS

===========================================================================*/
uint8 diag_set_current_preset_mask_id( uint8 preset_id )
{
  uint8 max_num_presets = DIAG_MAX_PRESET_ID;
  uint8 ret_val = DIAG_SET_PRESET_ID_SUCCESS;

#if defined(DIAG_MP_MASTER)
  boolean is_sent = FALSE;
  uint8 port_num = 0;
  diag_ctrl_msg_type ctrl_msg;
#endif

  if( (preset_id < DIAG_MIN_PRESET_ID) || (preset_id > DIAG_MAX_PRESET_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,
         "diag_set_current_preset_mask_id - Preset %d not supported.", preset_id);
    return DIAG_SET_PRESET_ID_NOT_SUPPORTED;
  }

#if defined(DIAG_MP_MASTER)
  //ToDo: Change this to just keep track of the max preset as it gets sent in from Slaves?
  //ToDo: Standalone modem case?
  for( port_num=0; port_num < NUM_SMD_PORTS; port_num++ )
  {
    if( diagcomm_ctrl_channel_status(DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num) )
    {
      if( DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].feature_mask, F_DIAG_PRESET_MASKS) )
      {
        max_num_presets = MIN(max_num_presets, diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].num_presets);
      }
      else
      {
        /* A Slave that's up does not support preset masks. Set to default of 1. */
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,
              "diag_set_current_preset_mask_id - Slave %d does not support preset masks. Use default.", port_num);
        max_num_presets = 1;
        break;
      }
    }
  }
#endif

  if( max_num_presets == 0 )
  {
    /* Did not recieve preset num from a Slave, set to default. */
    diag_cur_preset_id = DIAG_PRESET_MASK_1;
    ret_val = DIAG_SET_PRESET_ID_NOT_SUPPORTED;
  }
  else if( preset_id > max_num_presets )
  {
    /* Don't change preset mask if not supported by all Slaves. */
    return DIAG_SET_PRESET_ID_NOT_SUPPORTED;
  }
  else
  {
    /* Finally, switch the current presetid. */
    diag_cur_preset_id = preset_id;

    //ToDo: what if some slave supports presets but are late?
    //ToDo: what if a slave is late and turns out it supports less presets?
  }

  diag_switch_event_preset_mask();
  diag_switch_log_preset_mask();
  diag_switch_msg_preset_mask();

#if defined(DIAG_MP_MASTER)
  /* Compose DIAG_CTRL_MSG_SET_CUR_PRESET */
  ctrl_msg.cmd_type = DIAG_CTRL_MSG_SET_CUR_PRESET;
  ctrl_msg.data_len = sizeof(diag_ctrl_msg_cur_preset_type);
  ctrl_msg.data.cur_preset.preset_id = diag_cur_preset_id;

  /* Update slaves */
  for( port_num=0; port_num < NUM_SMD_PORTS; port_num++ )
  {
    (void)diagcomm_ctrl_send_mult_pkt( (void *)&ctrl_msg, DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_cur_preset_type),
                                        NULL, 0, DIAGCOMM_PORT_SMD, (diagcomm_enum_port_type)port_num, TRUE, &is_sent );
  }
#endif

  return ret_val;

} /* diag_set_current_preset_mask_id */


/*===========================================================================

FUNCTION DIAG_GET_CURRENT_PRESET_MASK_ID

DESCRIPTION
  Retrieve the current preset mask id.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  Current preset mask index.

===========================================================================*/
uint8 diag_get_current_preset_mask_id()
{
  return diag_cur_preset_id;

} /* diag_get_current_preset_mask_id */

/*==========================================================================

FUNCTION DIAG_WCDMA_WL1_FRAME_REG

DESCRIPTION
  This API will be used by WCDMA to register/unregister with Diag. The
  purpose of doing this is to avoid calling WCDMA function directly in
  Diag code.

PARAMETERS
  Valid pointer - Registers the cb
  NULL - Unregisters the cb

DEPENDENCIES
  None

RETURN VALUE
  Current preset mask index.

===========================================================================*/
void diag_wcdma_wl1_frame_reg(diag_wcdma_wl1_frame_cb_type ptr)
{
  /*if ptr is NULL, then WCDMA deregisters with Diag*/

  diag_wcdma_wl1_frame_cb = ptr;
  return;
}

/*===========================================================================

FUNCTION DIAG_MASK_NOTIFY_TIMER_CB

DESCRIPTION
   This function is called when the timer for diag mask change notification
   has expired.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

===========================================================================*/
void diag_mask_notify_timer_cb(uint32 params)
{
  /*Reset the timer*/
  osal_reset_timer(&diag_mask_change_notify_timer);
  diag_set_internal_sigs(DIAG_INT_MASK_CHANGE_NOTIFY_SIG);
}

/*===========================================================================

FUNCTION DIAG_MASK_CHANGE_NOTIFY

DESCRIPTION
  This function notifies the registered users about diag mask change.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

===========================================================================*/
void diag_mask_change_notify(void)
{
  if (DIAG_CHECK_MASK(diag_mask_notify,DIAG_EVENT_MASK_NOTIFY_BIT))
  {
           //Notify the registered clients about event mask change
      if (RCEVT_NULL == rcevt_signal_name(DIAG_EVENT_MASK_CHANGE_NOTIFY))
      {
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Event mask change notification was unsuccessful for proc id %d", DIAG_MY_PROC_ID);
      }
      else
      {
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_MED, "Event mask change notification was successful for proc id %d", DIAG_MY_PROC_ID);
        DIAG_CLR_MASK(diag_mask_notify,DIAG_EVENT_MASK_NOTIFY_BIT);
      }

  }
   if (DIAG_CHECK_MASK(diag_mask_notify,DIAG_LOG_MASK_NOTIFY_BIT))
  {
           //Notify the registered clients about log mask change
      if (RCEVT_NULL == rcevt_signal_name(DIAG_LOG_MASK_CHANGE_NOTIFY))
      {
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Log mask change notification was unsuccessful for proc id %d", DIAG_MY_PROC_ID);
      }
      else
      {
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_MED, "Log mask change notification was successful for proc id %d", DIAG_MY_PROC_ID);
        DIAG_CLR_MASK(diag_mask_notify,DIAG_LOG_MASK_NOTIFY_BIT);
      }

  }
   if (DIAG_CHECK_MASK(diag_mask_notify,DIAG_MSG_MASK_NOTIFY_BIT))
  {
         if (RCEVT_NULL == rcevt_signal_name(DIAG_MSG_MASK_CHANGE_NOTIFY))
     {
       MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Msg mask change notification was unsuccessful for proc id %d", DIAG_MY_PROC_ID);
     }
     else
     {
       MSG_1(MSG_SSID_DIAG, MSG_LEGACY_MED, "Msg mask change notification was successful for proc id %d", DIAG_MY_PROC_ID);
        DIAG_CLR_MASK(diag_mask_notify,DIAG_MSG_MASK_NOTIFY_BIT);
     }
  }
}
/*===========================================================================
FUNCTION DIAG_DROP_EVENT_REPORT
DESCRIPTION
  This function sends an event whenever the total drop count exceeds the defined
  threshold and then further at a defined periodicity.

PARAMETERS
  None

DEPENDENCIES
  None
RETURN VALUE
  None

===========================================================================*/
void diag_drop_event_report (void *param)
{
  EVENT_DIAG_DROP_DBG_PAYLOAD_INFO_type drop_event_payload; /* Drop Event Report Payload */
/* Local variables for alloc andd drop bytes */
  uint32 diag_drop_count_log = drop_count_log;
  uint32 diag_drop_count_f3 = drop_count_f3;
  uint32 diag_alloc_count_log = alloc_count_log;
  uint32 diag_alloc_count_f3 = alloc_count_f3;
  uint64 diag_total_drop_count = 0;  /* Total number of Diag packets dropped */
  uint64 diag_total_pkt_count = 0;   /* Total number of diag packets allocated and dropped */


  /* Calculate the percentage count of diag packets dropped only if there are drops else return */
  if(diag_drop_count_log > 0 || diag_drop_count_f3 > 0)
  {
    diag_total_drop_count =  (diag_drop_count_f3 + diag_drop_count_log)*100;
    diag_total_pkt_count = (diag_drop_count_log + diag_drop_count_f3 + diag_alloc_count_log + diag_alloc_count_f3);
    diag_per_count_dropped = (diag_total_drop_count / diag_total_pkt_count);
  }
  else if(bCountReset)
  {
    bCountReset = FALSE;
    diag_per_count_dropped = 0;
    diag_per_count_dropped_prev = 0;
  }
  else
    return;

  /* Report an event when the percentage drop count exceeds the threshold set and then at every step % level*/
  if( (diag_per_count_dropped >= diag_max_per_drop_threshold) && (diag_per_count_dropped - diag_per_count_dropped_prev >= diag_max_per_step))
  {
    /* Fill the event payload*/
    drop_event_payload.alloc_f3_cnt = diag_alloc_count_f3;
    drop_event_payload.alloc_log_cnt = diag_alloc_count_log;
    drop_event_payload.drop_f3_cnt = diag_drop_count_f3;
    drop_event_payload.drop_log_cnt = diag_drop_count_log;
    drop_event_payload.drop_cnt_per = diag_per_count_dropped;
    drop_event_payload.flow_ctrl_cnt_smd = diagcomm_flow_ctrl_count[DIAGCOMM_PORT_SMD];
    drop_event_payload.flow_ctrl_cnt_sio = diagcomm_flow_ctrl_count[DIAGCOMM_PORT_SIO];
    drop_event_payload.flow_ctrl_cnt_dci = 0; /* TODO: Update this once the flow control support is added for DIAG_STREAM_2 */

    /* Since this is on guest OS, updating the pd id to zero.
       This will have to updated later to report drop event for various PDs in multi-pd */
    drop_event_payload.pd_id = DIAG_GUEST_OS_PD;
    drop_event_payload.proc_id = (uint16) DIAG_MY_PROC_ID;
    drop_event_payload.event_source = DIAG_DROP;

    /* Send the event*/
    event_report_payload(EVENT_DIAG_DROP_DEBUG ,sizeof(EVENT_DIAG_DROP_DBG_PAYLOAD_INFO_type), (void*) &drop_event_payload);

    /*Store the value of the drop percentage for next comparison */
    diag_per_count_dropped_prev = diag_per_count_dropped;
  }
  return;
}



/*===========================================================================

FUNCTION DIAG_TIME_INIT

DESCRIPTION
  This function initializes the default time API to be used with diag packets.
 
PARAMETERS
  None
 
DEPENDENCIES 
  None 
 
RETURN 
  Boolean status to indicate that the default time API to use has been set.
===========================================================================*/
boolean diag_time_init()
{
  if(!osal_atomic_compare_and_set((osal_atomic_word_t*)&diag_time_init_begin, FALSE, TRUE))
    return FALSE;
  
      /* Diag uses unprotected timestamp call on QDSP6 modem */
#if defined DIAG_IMAGE_QDSP6_PROC && defined DIAG_IMAGE_MODEM_PROC
      diag_time_get = time_tod_get_no_mutex;
#else
#if defined DIAG_GET_OPTIMIZED_TS
      diag_time_get = time_get_optimized_till_msec;
#else
      diag_time_get = time_get;
#endif
#endif

  diag_time_initialized = TRUE;
  return TRUE;

}

/*===========================================================================
FUNCTION DIAG_SET_TIME_API

DESCRIPTION
  This function sets the time API to be used with the diag packets generated on the ADSP and
  also notify the client PDs to use the time API set.
  
PARAMETERS
  Time API to use
 
DEPENDENCIES 
  None 
 
RETURN 

===========================================================================*/
void diag_set_time_api(uint8 time_api_to_use)
{
  /* Update the global variable which tracks the time API to be used*/
  diag_time_api_to_use = time_api_to_use;

  switch(diag_time_api_to_use)
  {
    case DIAG_TIME_GET:
    {
      /* Diag uses unprotected timestamp call on QDSP6 modem */
#if defined DIAG_IMAGE_QDSP6_PROC && defined DIAG_IMAGE_MODEM_PROC
      diag_time_get = time_tod_get_no_mutex;
#else
#if defined DIAG_GET_OPTIMIZED_TS
      diag_time_get = time_get_optimized_till_msec;
#else
      diag_time_get = time_get;
#endif
#endif
      /* Notify the client PDs */
      //diag_time_api_change_notify(DIAG_TIME_API_CHANGE);
    }
    break;
    case DIAG_TIME_GET_UPTIME:
    {
      /* Notify the client PDs */
      diag_time_get = time_get_uptime;
      //diag_time_api_change_notify(DIAG_TIME_API_CHANGE);
    }
    break;
    case DIAG_TIME_GET_TBD:
    {
      /* Remove this error fatal after TIME SYNC Phase 2 implementation*/
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Invalid Time API to be set : %d", diag_time_api_to_use);
    }
    break;    
    default:
    {
      break;
    }
  }  
}


/*===========================================================================

FUNCTION DIAG_GET_TIME_API_REQUEST

DESCRIPTION
  This handler returns the TIME API currently being used to update the timestamp
  in diag packets on this proc.
  
PARAMETERS
  req_pkt     Incoming command packet
  pkt_len     Length of incoming packet
 
DEPENDENCIES
  None
 
RETURN 
  Returns response packet

===========================================================================*/

PACK(void*) diag_get_time_api_request(PACK(void*) req_pkt, uint16 pkt_len)
{ 
  diag_get_time_api_req_type *req = (diag_get_time_api_req_type*)req_pkt;
  diag_get_time_api_rsp_type* rsp = NULL;
  diagpkt_subsys_cmd_code_type cmd_code;
  unsigned int rsp_len = sizeof(diag_get_time_api_rsp_type);

  if(pkt_len != sizeof(diag_get_time_api_req_type))
  {
    return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  if(req->cmd_version != 1)
  {
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));
  }

  cmd_code = diagpkt_subsys_get_cmd_code(req_pkt);
  rsp = (diag_get_time_api_rsp_type*)diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, cmd_code,rsp_len);

  if(rsp)
  {
    rsp->cmd_version = req->cmd_version;
    rsp->time_api = diag_time_api_to_use;
  }

  return (void*)rsp;
  
}
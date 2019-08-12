/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 3 G   D A T A   S E R V I C E S   M A N A G E R

                  S I O   I N T E R F A C E

GENERAL DESCRIPTION
  This software unit contains wrapper functions for interfacing to the serial
  i/o driver, including routines to open/close a serial port, to change the
  serial mode and to set the states of serial port control signals.

EXTERNALIZED FUNCTIONS
  ds3g_siolib_open()
    Opens a specified serial port, in Autodetect mode

  ds3g_siolib_close()
    Closes the currently open serial port

  ds3g_siolib_change_mode()
    Sets the mode of the currently open serial port to the specified mode

  ds3g_siolib_cd_assert()
    Asserts the Carrier Detect (CD) signal

  ds3g_siolib_cd_deassert()
    De-asserts the Carrier Detect (CD) signal

  ds3g_siolib_ri_assert()
    Asserts the Ring Indicator (RI) signal

  ds3g_siolib_ri_deassert()
    De-asserts the Ring Indicator (RI) signal

  ds3g_siolib_set_inbound_flow()
    Used for flow control. Enables/disables inbound data flow from laptop

  ds3g_siolib_dsr_assert()
    Asserts the Data Set Ready (DSR) signal

  ds3g_siolib_dsr_deassert()
    De-asserts the Data Set Ready (DSR) signal

  ds3g_siolib_is_dtr_asserted()
    Returns the state of the Data Terminal Ready (DTR) signal

  ds3g_siolib_set_flow_ctl_method()
    Configures the flow control method for the currently open serial port

  ds3g_siolib_setup_tx()
    Queues data for serial transmission

  ds3g_siolib_change_baud_rate()
    Sets the baud rate of the currently open serial port to the specified
    value

  ds3g_siolib_set_cd_state()
    Arbitrator function for setting the Carrier Detect (CD) state

  ds3g_siolib_rearm_autodetect()
    Re-arms SIO autodetect mode

  ds3g_siolib_flush_tx()
    Schedules the SIO TX buffer to be flushed.

  ds3g_siolib_allocate_port()
    Allocate port resource semaphore

  ds3g_siolib_release_port()
    Release port allocation semaphore

  ds3g_siolib_get_at_state()
    Returns AT command state for the input port.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  All the serial port operations can be performed only after ds3g_siolib_open()
  is called at least once i.e. only if a serial port is currently open.


  Copyright (c) 2001 - 2016 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3gsiolib.c_v   1.19   13 Feb 2003 17:17:20   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3gsiolib.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/18/16   skc     Fixed issue related to cd assert for DUN call.
08/17/16   skc     Fixed wrong call type used for getting port_id.
08/03/16   skc     Adding support for ESC SEQUENCE for PS Data call.
07/26/16   skc     Fixed issue with ATO command.
08/27/14   tk      Fixed issue in RING URC reporting for MT CSD call.
06/27/14   tk      Added support for Dynamic ATCoP.
07/24/13   sc      Fixed CD deassert issue.
10/29/12   sd      Fixed compiler warnings.
08/23/12   tk      Fixed ATCoP KW errors.
08/13/12   ss      Removed ds_create_efs_item_file usage. Listed the file in 
                   ds_init_efs_config_file instead.
08/07/12   sk      CD,DSR Aseerts sent only when DTR is High.
05/22/12   tk      Added check for DTR status for every port through query API.
03/21/12   msh     TCB changes due to Core changes 
02/13/12   sb      Sending URC's only when DTR for USB_CDC, A2_DUN ports is HIGH.
02/08/12   sk      Feature cleanup.
11/11/11   ss      Fixed deadlock between HS-USB and DS task.
09/27/11   nd      Fixed compiler warnings.
09/16/11   dvk     Fixed compiler warnings
08/25/11   ua      Fixed AT commands stream from multiple ports. 
08/15/11   vpk     Cleanup dsm queue as part of setting up of watermarks. 
06/28/11   ttv     Fixed issue of DS cmd buffer exhaustion during data
                   transfer.
05/26/11   dvk     Global Variables Cleanup. 
03/23/11   sn      Globals cleanup for free floating of tasks.
02/11/11   vs      DUN changes for CSFB Fusion
02/02/11   ss      Fixed an issue where FLOW_ENABLE/FLOW_DISABLE IOCTLs are
                   sent irrespective of flow being already ENABLED/DISABLED.
11/01/10   sa      Removing REX_DISABLE_PREMPTION/REX_ENABLE_PREMPTION.
10/14/10   sa      Adding debug messages before ASSERT.
08/25/10   nc      Ensured to allocate/release among HSUART1 
                   and HSUART2 ports as forwarding port.
08/22/10   vk      Use a single signal for all forwarding ports
06/25/10   vs      Added Forwarding Port manager and other misc changes 
06/04/10   sa      Set packet size to ensure that high wm level is hit before
                   dne is hit in the rx watermark for CS data.
06/03/10   sn      Added FEATURE_BT_MODEM featurization for BT specific 
                   functionality. This feature will be enabled in MODEM proc 
                   on multi-proc targets.
05/21/10   asn     Enable DS3G for DSM large item based flow control
05/10/10   kk      Added support for ^HC SMS commands. 
02/04/10   asn     Process DSM Mem events in DS Task context 
10/15/09   vs      SU Level Api Defeaturization
07/01/09   sa      Fixed crash when VT call is getting connected using SMD port.
05/05/09   ss      Fixed Compiler Warnings.
03/04/09   sa      AU level CMI modifications.
12/23/08   sa      Replaced q_init with dsm_queue_init for watermark initialization.
12/12/08   sn      Fixed off-target lint errors.
12/01/08   nc      Fixing the feature wrap issue for non QDSP6 Targets for 
                   REX_DISABLE_PREMPTION/REX_ENABLE_PREMPTION
10/31/08   sa      Fixed flushing of CONNECT result code for synchronous CSD calls.
07/16/08   ua      Added support for external client support.
                   Optimized received SIO signals usage. This is feature wrapped in
                   FEATURE_DSAT_EXT_CLIENT_SUPPORT.
06/29/08   vk      Set packet size to ensure that high wm level is hit before
                   dne is hit in the rx watermark.
05/06/08   sa      Allow unsolicted result codes to be sent only to ports which are 
                   not in ONLINE_DATA_STATE.
05/05/08   sa      Added ds3g_siolib_get_at_state() function.
04/02/08   sn      Fixed Klockwork errors.
03/08/08   ac      a) Decoupled the DSM larget item event the flow control between RM
                   watermark and the USB
                   b) Remove the reduntant flow control between RM wartermark and the USB
12/18/07   sn      Fixed lint errors.
10/26/07   ua      Added support for AT&S command.
10/08/07   ar      Added support for DS3G_SIOLIB_SMD_DATA11_PORT.
08/09/07   ar      Fix behavior for mix of auto and manual answer scenarios.
07/26/07   ar      Ensure incoming alert not active before DTR signal filtered.
05/18/07   sa      Changed DTR call back handlers to post signal to DS task
                   only if DTR is recieved on the modem port.
04/12/07   ua      Added function ds3g_siolib_get_answer_port.
02/25/07   vd      Changed MSG_MED to MSG_LOW.
02/16/07   ar      Replace ds3g_siolib_incoming_call with per-call dir indication.
02/14/07   vd      Lint error fixes.
01/03/07   vp/sq   Add ds3g_siolib_incoming_cs_call_reset function
11/28/06   ar      Add support for explicit modem port selection.
08/17/06   vp      Changes to ds3g_siolib_change_baud_rate to query RDM instead
                   of using active port.
08/03/06   rsl     Changed some of the benign asserts to ERRs.
07/24/06   ar      Add looping over all ports for alerting user of MT call.
06/28/06   sl      Removed the defaulting of modem port to USB if answer port
                   is not set in ds3g_siolib_set_modem_port()
06/13/06   sl      Add port_id to ds3g_siolib_set_flow_ctl_method() interface
05/10/06   vp      Definition of ds3g_siolib_get_modem_port()
05/01/06   vp      Changes to enable multiple data ports (shared memory data
                   ports) for multiple processor builds.
03/01/06   sl      Do not reset active port on asynch ATCOP cmd processing.
01/19/06   sl      Serializer merged with SIOLIB.
11/22/05   squ     For non auto detect mode, removed direct operation of 
                   queue inside a watermark in ds3gsiolib_change_mode
08/22/05   rsl     Added sio_close_in_progress to make sure all actions on a port
                   that is being closed will not be processed.
10/28/04   jd      Send entire dsm packet chain to SIO instead of seperating
                   into individual items (required for RmNet)
09/20/04   ar      Add port allocation/release semaphore functionality.
05/24/04   snb     Register ATCOP to SIO low/high water callbacks and change
                   low/high watermarks.
01/22/04   atp     When ds3gsiolib_change_mode(AUTODETECT) is called, added
                   call to change atcop preprocessor state to command mode.
08/08/03   rsl     Support for DSM 2.0
02/13/03   TMR     Commented out MSG_LOW from ds3g_siolib_set_inbound_flow()
01/17/02   sb      Reduced the priority of ERR message when rearming the Autodt
                   serial state. This has to do with fax switching between 
                   online-cmd/online-data modes during a fax call and should 
                   not be treated as error.
12/19/02   atp     Removed deprecated functions inbound_flow_enable/disable()
11/13/02   usb     Replaced DS_IS_FLOW_ENABLED with DS_FLOW_ALL_MASK
11/04/02   tmr     Change AT state to DS3G_SIOLIB_ONLINE_DATA_AT_STATE when
                   ds3g_siolib_change_mode() is called to change to any mode
                   except AUTODETECT.
10/15/02    ak     Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_DS_1X
10/13/02   ak      For JCDMA, use different times for RI intervals.
10/09/02   ak      When registering mem events with SIO, the many event
                   should be for FREEing an item.
09/26/02   tmr     Set call_answered to TRUE in ds3g_alert_user when 
                   auto_answer_handler is called
09/12/02   atp     Removed setting open_param fields in ds3gsiolib_ioctl().
07/11/02   ak      In setting FLOW_CONTROL, we had a MSG_HIGH.  Since this 
                   (seems) to be solid, make MSG_LOW.
07/01/02   ak      Minor fixes for first data call.
05/08/02   rsl     Added siolib related design changes.
03/05/02   smp     Added support for CD winking (&C2). Fixed bug due to which
                   Autodetect was not getting re-armed.
02/25/02   smp     In ds3g_siolib_setup_tx(), check if current serial mode is
                   Autodetect Mode, before re-arming Autodetect.
02/25/02   smp     Do not re-arm Autodetect if the current serial mode is not
                   Autodetect Mode.
02/19/02   rlp     Code review changes.
02/05/02   rlp     Added online-command mode support.
12/14/01   smp     Update ri_asserted state variable when RI signal is
                   asserted or de-asserted.
10/31/01   smp     Don't ERR_FATAL when sio_open() fails, just return a value
                   indicating that the port could not be opened. Also fixed
                   some compiler warnings.
10/04/01   smp     Added ds3g_siolib_is_rts_asserted() function.
08/28/01   smp     Fixed pointer issue in ds3g_siolib_is_dtr_asserted().
08/17/01   smp     Renamed ATCOP files/functions/data types.
06/07/01   smp     Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include <stringl/stringl.h>
#include "msg.h"
#include "dsmgr_api.h"

#define FEATURE_DSM_WM_CB


#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST 
#include "dsut.h"
#endif /* FEATURE_DATA_LTE_OFFTARGET_TEST */

#ifdef TEST_FRAMEWORK 
#error code not present
#endif

#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "rex.h"
#include "task.h"
#include "dstask.h"
#include "dstaski.h"
#include "dsat_v.h"
#include "dsati.h"
#include "ds3gsiolib.h"
#include "ds3gmgrint.h"
#include "ds3gutil.h"
#include "ds3gtimer.h"
#include "ds3grdmif.h"
#include "task.h"
#include "ds3gsiolib_ex.h"
#include "ds_3gppi_utils.h"

#ifdef FEATURE_IRDA
#include "irda_api.h"
#endif /* FEATURE_IRDA */

#ifdef FEATURE_DATA_SDIO_MUX_A2_DUN
#include "fs_sys_types.h"
#include "fs_errno.h"
#endif

#include "data_err_suppr.h"
#include "data_msg.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/*---------------------------------------------------------------------------
  Multiplexed signal value definitions. Currently muxing is used only for 
  forwarding ports
---------------------------------------------------------------------------*/
#define DS_HSUART1_PORT_1ST_SIO_RX_SIG_VAL 0x00000001
#define DS_HSUART2_PORT_1ST_SIO_RX_SIG_VAL 0x00000002
#define DS_HSUART3_PORT_1ST_SIO_RX_SIG_VAL 0x00000004

/*---------------------------------------------------------------------------
  Critical section for protecting read/write to mux'ed signal value set
---------------------------------------------------------------------------*/
static rex_crit_sect_type ds3g_siolib_rx_sig_crit_sec = {{0}};

/*---------------------------------------------------------------------------
  Convenience macros for acquiring/leaving/initing rx sig crit section
---------------------------------------------------------------------------*/
#ifdef TEST_FRAMEWORK   
  #error code not present
#endif /* TEST_FRAMEWORK */

#ifndef TEST_FRAMEWORK 
#define DS3G_SIOLIB_RX_SIG_CRIT_SECT_ENTER() \
            rex_enter_crit_sect(&ds3g_siolib_rx_sig_crit_sec)

#define DS3G_SIOLIB_RX_SIG_CRIT_SECT_LEAVE() \
            rex_leave_crit_sect(&ds3g_siolib_rx_sig_crit_sec)

#define DS3G_SIOLIB_RX_SIG_CRIT_SECT_INIT() \
            rex_init_crit_sect(&ds3g_siolib_rx_sig_crit_sec)
#endif /* TEST_FRAMEWORK */


/*---------------------------------------------------------------------------
  The time intervals, in ms, for which the RI signal will be asserted and
  de-asserted when an incoming call is received.
---------------------------------------------------------------------------*/
  #define DS3G_SIOLIB_RI_ASSERT_INTERVAL      100
  #define DS3G_SIOLIB_RI_DEASSERT_INTERVAL   5900

/*---------------------------------------------------------------------------
 Variable to store the port id of the serial port currently active, i.e
 for which data is being currently processed. 
---------------------------------------------------------------------------*/
static ds3g_siolib_port_e_type ds3g_siolib_active_port = DS3G_SIOLIB_PORT_NONE;
#ifdef FEATURE_DSAT_MDM_FUSION
static ds3g_siolib_port_e_type ds3g_siolib_proxy_port = DS3G_SIOLIB_PORT_NONE;
#endif /* FEATURE_DSAT_MDM_FUSION */
/*---------------------------------------------------------------------------
  Indicates the port currently in a data call
---------------------------------------------------------------------------*/
static ds3g_siolib_port_e_type ds3g_siolib_modem_port = DS3G_SIOLIB_PORT_NONE;

/*---------------------------------------------------------------------------
  Indicates the port that was last on a data call
---------------------------------------------------------------------------*/
static ds3g_siolib_port_e_type ds3g_siolib_last_modem_port = 
                                    DS3G_SIOLIB_PORT_NONE;


/*---------------------------------------------------------------------------
  Indicates the port pending sio_close callback
---------------------------------------------------------------------------*/
static ds3g_siolib_port_e_type   ds3g_siolib_pending_close_port = 
                                                     DS3G_SIOLIB_PORT_NONE;

/*---------------------------------------------------------------------------
  Indicate current waiting ports.
---------------------------------------------------------------------------*/
static word  ds3g_siolib_port_waiting_mask = 0;

/*---------------------------------------------------------------------------
  Indicate the unsolicited data type. 
---------------------------------------------------------------------------*/
static ds3g_siolib_tx_e_type ds3g_siolib_tx_type = DS3G_SIOLIB_UNSOLICTED_NONE;
static ds3g_siolib_port_e_type ds3g_siolib_tx_port = DS3G_SIOLIB_PORT_NONE;

/*---------------------------------------------------------------------------
  Indicate the client that is sending response 
---------------------------------------------------------------------------*/
static ds3g_siolib_client_e_type ds3g_siolib_client_type = 
                                                      DS3G_SIOLIB_CLIENT_NONE;

#define DS3G_SIOLIB_RING_TYPE_NONE   (DSAT_OK)
static dsat_result_enum_type ds3g_siolib_current_ring_type = 
                                                 DS3G_SIOLIB_RING_TYPE_NONE;

/*---------------------------------------------------------------------------
  For each port state info is maintained containing stream_id, at_state, etc.
---------------------------------------------------------------------------*/
static ds3g_siolib_state_info_type ds3g_siolib_state[DS3G_SIOLIB_PORT_MAX];

#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT
static dsat_ext_client_id_type ds3g_siolib_active_ext_client = 
                                    DSAT_EXT_CLIENT_ID_INVALID;
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT */
/*---------------------------------------------------------------------------
  For the DS over Bluetooth Serial Port Profile
---------------------------------------------------------------------------*/
#if defined(FEATURE_BT) || defined(FEATURE_BT_MODEM)
static bt_spp_status_type ds3g_siolib_bt_spp_status;
static bt_spp_state_type ds3g_siolib_bt_spp_state = BT_SPP_ST_CLOSED;
#endif /* FEATURE_BT || FEATURE_BT_MODEM */


/*---------------------------------------------------------------------------
  Lo Water, Hi Water and Don't Exceed counts for the Autodetect watermark
  used for RX data bytes from SIO.
---------------------------------------------------------------------------*/
#define DS3G_SIOLIB_FROM_SIO_LOWATER_CNT              0
#define DS3G_SIOLIB_FROM_SIO_HIWATER_CNT             60
#define DS3G_SIOLIB_FROM_SIO_DONT_EXCEED_CNT        (7*536)

/*---------------------------------------------------------------------------
  Lo Water, Hi Water and Don't Exceed counts for the Autodetect watermark
  used for TX data bytes to SIO.  Don't exceed count size is set based on
  the largest AT command response that can be queued up to send to the TE.
  This is six DSM large item data payloads, plus one extra for safety
  margin.
---------------------------------------------------------------------------*/
#define DS3G_SIOLIB_TO_SIO_LOWATER_CNT              750 
#define DS3G_SIOLIB_TO_SIO_HIWATER_CNT             2000 
#define DS3G_SIOLIB_TO_SIO_DONT_EXCEED_CNT      (7*536)

/*---------------------------------------------------------------------------
  Lo Water, Hi Water and Don't Exceed counts for the DTR event watermark
  used for DTR events from SIO.
---------------------------------------------------------------------------*/
#define DS3G_SIOLIB_DTR_EVENT_MSG_SIZE        sizeof(ds3g_siolib_port_e_type)
#define DS3G_SIOLIB_DTR_EVENT_LOWATER_CNT     (DS3G_SIOLIB_DTR_EVENT_MSG_SIZE * 0)
#define DS3G_SIOLIB_DTR_EVENT_HIWATER_CNT     (DS3G_SIOLIB_DTR_EVENT_MSG_SIZE * 49)
#define DS3G_SIOLIB_DTR_EVENT_DONT_EXCEED_CNT (DS3G_SIOLIB_DTR_EVENT_MSG_SIZE * 50)

/*---------------------------------------------------------------------------
  Latest event info indexes
---------------------------------------------------------------------------*/
#define DS3G_SIOLIB_SMALL_DSM_ITEM_INDEX         0
#define DS3G_SIOLIB_LARGE_DSM_ITEM_INDEX         1
#define DS3G_SIOLIB_MAX_DSM_ITEM_INDEX           2

/*---------------------------------------------------------------------------
  Ring timer used when ringing the serial port for an incoming call.
---------------------------------------------------------------------------*/
static rex_timer_type ds3g_siolib_ring_timer[DS3G_SIOLIB_PORTID_MAX];

/*---------------------------------------------------------------------------
  Timer used to wink Carrier Detect at the end of a call.
---------------------------------------------------------------------------*/
static rex_timer_type ds3g_siolib_dcd_wink_timer[DS3G_SIOLIB_PORTID_MAX];

/*---------------------------------------------------------------------------
  SIO port allocation semaphore flags
---------------------------------------------------------------------------*/
ds3g_siolib_port_e_type ds3g_siolib_port_allocated[DS3G_SIOLIB_PORTID_MAX] = 
                                                          {DS3G_SIOLIB_PORT_NONE};

/*---------------------------------------------------------------------------
  SIO port allocation semaphore flags
---------------------------------------------------------------------------*/
ds3g_siolib_port_e_type ds3g_siolib_last_port_allocated[DS3G_SIOLIB_PORTID_MAX] = 
                                                          {DS3G_SIOLIB_PORT_NONE};

/*---------------------------------------------------------------------------
  SIO port Singal Stauts
---------------------------------------------------------------------------*/
ds3g_siolib_port_signal_status_type ds3g_siolib_port_signal_status[DS3G_SIOLIB_PORTID_MAX + 1];

/*---------------------------------------------------------------------------
  Indicates the port on which we received an ATS0
---------------------------------------------------------------------------*/
ds3g_siolib_answer_info_type ds3g_siolib_answer_info 
                           = {0, DS3G_SIOLIB_PORT_NONE, DS3G_SIOLIB_PORT_NONE, NULL};

#ifdef FEATURE_DSAT_MDM_FUSION
/* Constant indicating the maximum number of forwarding ports that can be 
   allocated */
#define NUM_FORWARDING_PORTS_MAX 2

/* Forwarding port table which maps the physical port to the forwarding port */
typedef struct
{
  ds3g_siolib_port_e_type phys_port;
  ds3g_siolib_port_e_type forwarding_port;
} ds3g_siolib_forwarding_port_tbl_type;

static ds3g_siolib_forwarding_port_tbl_type 
            forwarding_port_tbl[NUM_FORWARDING_PORTS_MAX];

#endif /* FEATURE_DSAT_MDM_FUSION */

typedef struct ds3g_siolib_latest_event
{
  boolean                  is_posted[DS3G_SIOLIB_MAX_DSM_ITEM_INDEX];
  dsm_mem_level_enum_type  event[DS3G_SIOLIB_MAX_DSM_ITEM_INDEX];
}ds3g_siolib_latest_event_type;

static ds3g_siolib_latest_event_type latest_event_info;

/* DTR event watermark queue */
LOCAL dsm_watermark_type ds3g_siolib_dtr_event_wm;
LOCAL q_type             ds3g_siolib_dtr_event_q = {{NULL}};

/* DTR signal counters */
LOCAL uint8              ds3g_siolib_dtr_sig_cntr = 0;

/* Initializing current call type */
ds_call_type_e_type current_ds_call_type = DS_CALL_TYPE_UNSPECIFIED;

/*===========================================================================
                    Forward declarations/prototypes
===========================================================================*/

#ifdef FEATURE_DSAT_MDM_FUSION
static ds3g_siolib_status_e_type  ds3g_siolib_hsuart_open
( 
  sio_port_id_type        port_id,
  dsm_watermark_type      *rx_wm_ptr,   /* Watermark for rx data from SIO */
  dsm_watermark_type      *tx_wm_ptr    /* Watermark for tx data to SIO */
);
#endif /* FEATURE_DSAT_MDM_FUSION */

static void ds3g_siolib_mem_ctrl_event
(
  ds_cmd_type  *cmd_ptr
);

static void ds3g_siolib_handle_1st_sio_rx_sig
( 
  ds3g_siolib_port_e_type     port_id
);

static void ds3g_siolib_handle_sio_close_complete_cmd( void );

static void ds3g_siolib_process_enter_online_data_mode
(
  boolean                     flush_tx
);

static void ds3g_siolib_timer_cb
(
  unsigned long               timer_id
);

static void ds3g_siolib_wm_non_empty_cb
(
  struct dsm_watermark_type_s *wm_ptr, 
  void*                       user_data_ptr
);

static  void ds3g_siolib_dtr_changed_cb
( 
  void* cb_data 
);

#ifdef FEATURE_DSAT_MSM_FUSION
ds3g_siolib_port_e_type ds3g_siolib_get_urc_port(void);
#endif /* FEATURE_DSAT_MSM_FUSION*/

static void ds3g_siolib_set_dtr_handling_pending_flag
(
  boolean                 flag,
  ds3g_siolib_port_e_type port_id
);

static boolean ds3g_siolib_dtr_handling_pending_flag_is_set
(
  ds3g_siolib_port_e_type port_id
);


static ds3g_siolib_port_e_type ds3g_siolib_get_allocated_port_from_index
(
  ds3g_siolib_portid_e_type port_index
);

/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3G_SIOLIB_TIMER_CB

DESCRIPTION
    Callback for Wink DCD and RI timers.

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
void ds3g_siolib_timer_cb
(
  unsigned long  timer_id
)
{
  ds_cmd_type                  *cmd_ptr = NULL;
  ds3g_timer_expired_cmd_type  *timer_expired_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_timer_expired_cmd_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  DS_SIO_MSG1_HIGH("Timer Id %d expired", timer_id);
  timer_expired_cmd_ptr = cmd_ptr->cmd_payload_ptr;
  timer_expired_cmd_ptr->timer_id = timer_id;
  cmd_ptr->hdr.cmd_id = DS_CMD_TIMER_EXPIRED;
  ds_put_cmd( cmd_ptr );

  return;
} /* ds3g_siolib_timer_cb */

#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT
/*===========================================================================

FUNCTION DS3G_SIOLIB_SERVICE_EXT_CLIENTS

DESCRIPTION
  This function is a wrapper function for the actual service clients
  which is dsatclient_service_ext_clients. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_siolib_service_ext_clients(void)
{
  dsatclient_service_ext_clients();

}/* ds3g_siolib_service_ext_clients */
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT */

/*===========================================================================

FUNCTION DS3G_SIOLIB_WM_NON_EMPTY_CB

DESCRIPTION
  Callback function invoked when the Autodetect watermark item goes non-empty.
  This function looks at the callback data to determine the port for which
  the watermark has gone non-empty and then sets the corresponding signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
void  ds3g_siolib_wm_non_empty_cb
(
  struct dsm_watermark_type_s *wm_ptr, 
  void* user_data_ptr
)
{
  ds3g_siolib_port_e_type port_id =(ds3g_siolib_port_e_type)(int)user_data_ptr;
  ds3g_siolib_state_info_type *state= NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SIO_MSG1_MED("in non_empty_cb, port id %d",port_id);

  if((port_id > DS3G_SIOLIB_PORT_NONE) && (port_id < DS3G_SIOLIB_PORT_MAX))
  {
    state = &ds3g_siolib_state[port_id];
    if(state->serial_info.rx_sig)
    {
       DS_SIO_MSG0_MED("Signal was already set previously");
    }
    else
    {
       DS_SIO_MSG0_MED("Setting rx signal");
    }
    state->serial_info.rx_sig = TRUE;
    (void) rex_set_sigs( dsi_get_ds_tcb(), DS_DATA_PORT_1ST_SIO_RX_SIG );
  }
  else
  {
    DS_SIO_MSG0_ERROR("Corrupt user_data_ptr in ds3g_siolib_wm_non_empty_cb");
  }

} /* ds3g_siolib_wm_non_empty_cb() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_SETUP_WATERMARKS

DESCRIPTION
  This function initializes the watermarks used for RX and TX data
  from/to SIO. The lowater, hiwater and don't exceed counts are configured,
  and the watermark count (amount of data stored in the watermark) is set to
  zero.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3g_siolib_setup_watermarks
( 
  ds3g_siolib_port_e_type port_id 
)
{
  ds3g_siolib_serial_info_type *info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(port_id == DS3G_SIOLIB_PORT_NONE)
  {
    DATA_ERR_FATAL("input port is DS3G_SIOLIB_PORT_NONE");
    return;
  }

  info = &ds3g_siolib_state[port_id].serial_info;

  if (info->rx_wm_ptr.current_cnt != 0)
  {
     DS_SIO_MSG1_MED("Clearing %d bytes rx watermark",
           info->rx_wm_ptr.current_cnt);
     /* Clean up any remaining watermark entries  */
     dsm_empty_queue(&info->rx_wm_ptr);
  }

  /*-------------------------------------------------------------------------
    Initialize , set Don't Exceed counts and register the associated queue 
    for storing rx'ed data.
  -------------------------------------------------------------------------*/
  dsm_queue_init(&info->rx_wm_ptr,
                 DS3G_SIOLIB_FROM_SIO_DONT_EXCEED_CNT,
                 &info->rx_wm_q);
                 
  
  /*-------------------------------------------------------------------------
    Set the Lo Water and Hi Water  for the RX data
    watermark.
  -------------------------------------------------------------------------*/
  info->rx_wm_ptr.lo_watermark = DS3G_SIOLIB_FROM_SIO_LOWATER_CNT;
  info->rx_wm_ptr.hi_watermark = DS3G_SIOLIB_FROM_SIO_HIWATER_CNT;

  /*-------------------------------------------------------------------------
    Initialize the watermark internal byte counts for the RX data watermark.
  -------------------------------------------------------------------------*/
  info->rx_wm_ptr.current_cnt = 0;
#ifdef FEATURE_DSM_MEM_CHK
  info->rx_wm_ptr.highest_cnt = 0;
  info->rx_wm_ptr.total_rcvd_cnt = 0;
#endif

  /*-------------------------------------------------------------------------
    Register callback functions for the RX data watermark. The only event of
    interest is the non-empty event.
  -------------------------------------------------------------------------*/
  info->rx_wm_ptr.lowater_func_ptr = NULL;
  info->rx_wm_ptr.hiwater_func_ptr = NULL;
  info->rx_wm_ptr.each_enqueue_func_ptr = NULL;
  info->rx_wm_ptr.gone_empty_func_ptr = NULL;
  info->rx_wm_ptr.non_empty_func_ptr = ds3g_siolib_wm_non_empty_cb;
  info->rx_wm_ptr.non_empty_func_data = (void *) port_id;


  if (info->tx_wm_ptr.current_cnt != 0)
  {
     DS_SIO_MSG1_MED("Clearing %d bytes tx watermark",
           info->tx_wm_ptr.current_cnt);
     /* Clean up any remaining watermark entries  */
     dsm_empty_queue(&info->tx_wm_ptr);
  }

  /*-------------------------------------------------------------------------
    Initialize , set Don't Exceed counts and register the associated queue 
    for storing TX data.
  -------------------------------------------------------------------------*/
  dsm_queue_init(&info->tx_wm_ptr,
                 DS3G_SIOLIB_FROM_SIO_DONT_EXCEED_CNT,
                 &info->tx_wm_q);

  
/*-------------------------------------------------------------------------
    Set the Lo Water and Hi Water  counts for the TX data
    watermark.
  -------------------------------------------------------------------------*/
  info->tx_wm_ptr.lo_watermark = DS3G_SIOLIB_TO_SIO_LOWATER_CNT;
  info->tx_wm_ptr.hi_watermark = DS3G_SIOLIB_TO_SIO_HIWATER_CNT;

  /*-------------------------------------------------------------------------
    Initialize the watermark internal byte counts for the TX data watermark.
  -------------------------------------------------------------------------*/
  info->tx_wm_ptr.current_cnt = 0;
#ifdef FEATURE_DSM_MEM_CHK
  info->tx_wm_ptr.highest_cnt = 0;
  info->tx_wm_ptr.total_rcvd_cnt = 0;
#endif

  /*-------------------------------------------------------------------------
    Register callback functions for the TX data watermark.
  -------------------------------------------------------------------------*/
  info->tx_wm_ptr.lowater_func_ptr = dsat_to_sio_lowater_cb;
  info->tx_wm_ptr.hiwater_func_ptr = dsat_to_sio_hiwater_cb;
  info->tx_wm_ptr.gone_empty_func_ptr = NULL;
  info->tx_wm_ptr.non_empty_func_ptr = NULL;

} /* ds3g_siolib_setup_watermarks() */

/*===========================================================================
FUNCTION DS3G_SIOLIB_MEM_EVENT_CTRL_EV_CB

DESCRIPTION
  This handler is registered with DSM for memory use events

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds3g_siolib_mem_ctrl_ev_cb
(
  dsm_mempool_id_enum_type pool_id,            /* memory pool ID */
  dsm_mem_level_enum_type  event,              /* type of memory event */
  dsm_mem_op_enum_type     op
)
{
  ds_cmd_type                    *cmd_ptr = NULL;
  ds3g_siolib_dsm_mem_ctrl_type  *dsm_mem_ctrl_ptr = NULL;
  boolean                         post_cmd = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS3G_SIOLIB_RX_SIG_CRIT_SECT_ENTER();
  if(pool_id == DSM_DS_SMALL_ITEM_POOL)
  {
    latest_event_info.event[DS3G_SIOLIB_SMALL_DSM_ITEM_INDEX]      = event;
    if(latest_event_info.is_posted[DS3G_SIOLIB_SMALL_DSM_ITEM_INDEX] == FALSE)
    {
      latest_event_info.is_posted[DS3G_SIOLIB_SMALL_DSM_ITEM_INDEX]  = TRUE;
      post_cmd = TRUE;
    }
  }
  else if(pool_id == DSM_DS_LARGE_ITEM_POOL)
  {
    latest_event_info.event[DS3G_SIOLIB_LARGE_DSM_ITEM_INDEX]      = event;
    if(latest_event_info.is_posted[DS3G_SIOLIB_LARGE_DSM_ITEM_INDEX] == FALSE)
    {
      latest_event_info.is_posted[DS3G_SIOLIB_LARGE_DSM_ITEM_INDEX]  = TRUE;
      post_cmd = TRUE;
    }
  }
  DS3G_SIOLIB_RX_SIG_CRIT_SECT_LEAVE();

  if(post_cmd == TRUE)
  {
    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_siolib_dsm_mem_ctrl_type));
    if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {
      ASSERT(0);
      return;
    }
    DS_SIO_MSG3_HIGH( "dsm mem ev cb: free [%d], event [%d], op [%d]",
                      DSM_POOL_FREE_CNT(pool_id), event, op );
    cmd_ptr->hdr.cmd_id = DS_CMD_SIO_DSM_MEM_CTRL;
    dsm_mem_ctrl_ptr 
      = (ds3g_siolib_dsm_mem_ctrl_type*)cmd_ptr->cmd_payload_ptr;

    dsm_mem_ctrl_ptr->pool_id = pool_id;
    dsm_mem_ctrl_ptr->event   = event;
    dsm_mem_ctrl_ptr->op      = op;

    DS_SIO_MSG1_HIGH( "post DS task cmd:%d", DS_CMD_SIO_DSM_MEM_CTRL );
    ds_put_cmd( cmd_ptr ); 
  }
  return;
} /* ds3g_siolib_mem_ctrl_ev_cb */

/*===========================================================================
FUNCTION DS3G_SIOLIB_MEM_EVENT_CTRL

DESCRIPTION
  This function will call the appropriate memory event actions for the Data
  Services application.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
static void ds3g_siolib_mem_ctrl_event
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_siolib_port_e_type         port_count;
  ds3g_siolib_port_e_type         port_init = DS3G_SIOLIB_PORT_NONE;
  dsm_mem_level_enum_type         event_type = DSM_MEM_LEVEL_DNE;
  ds3g_siolib_dsm_mem_ctrl_type  *dsm_mem_ctrl_ptr = NULL;
  dsm_mempool_id_enum_type        pool_id;
  //dsm_mem_level_enum_type         event;
  dsm_mem_op_enum_type            op;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  if(cmd_ptr->hdr.cmd_id != DS_CMD_SIO_DSM_MEM_CTRL)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid cmd: %d", cmd_ptr->hdr.cmd_id);
    return;
  }

  dsm_mem_ctrl_ptr = (ds3g_siolib_dsm_mem_ctrl_type*)cmd_ptr->cmd_payload_ptr;
  pool_id = dsm_mem_ctrl_ptr->pool_id;
  //event   = dsm_mem_ctrl_ptr->event;
  op      = dsm_mem_ctrl_ptr->op;

  /*-----------------------------------------------------------------------
   Increment the enumerator to point to DS3G_SIOLIB_PORT_NONE+1
 ------------------------------------------------------------------------*/     
  port_init++;

  DS3G_SIOLIB_RX_SIG_CRIT_SECT_ENTER();
  if(pool_id == DSM_DS_SMALL_ITEM_POOL)
  {
    event_type = latest_event_info.event[DS3G_SIOLIB_SMALL_DSM_ITEM_INDEX];
    latest_event_info.is_posted[DS3G_SIOLIB_SMALL_DSM_ITEM_INDEX] = FALSE;
  }
  else if(pool_id == DSM_DS_LARGE_ITEM_POOL)
  {
    event_type = latest_event_info.event[DS3G_SIOLIB_LARGE_DSM_ITEM_INDEX];
    latest_event_info.is_posted[DS3G_SIOLIB_LARGE_DSM_ITEM_INDEX] = FALSE;
  }
  DS3G_SIOLIB_RX_SIG_CRIT_SECT_LEAVE();

  if(pool_id == DSM_DS_SMALL_ITEM_POOL)
  {
    switch (event_type)
    {
    case DSM_MEM_LEVEL_SIO_MANY:
      
      if (op == DSM_MEM_OP_FREE)
      {
        /*-----------------------------------------------------------------
          Do buffer based flow control. If it seems that we are recovering
          from a lack of items, enable inbound flow again.
        ----------------------------------------------------------------*/
        DS_SIO_MSG0_HIGH(" SMALL ITEM MANY ITEMS MARK Reached");
        for (port_count = port_init; 
              port_count < DS3G_SIOLIB_PORT_MAX; port_count++) 
        {         
          ds3g_siolib_set_port_inbound_flow(port_count, DS_FLOW_DSM_SMALL_Q_MASK,
                                     DS_FLOW_ENABLE);
        }
      }
      break;
      
    case DSM_MEM_LEVEL_SIO_FEW:
      /*-----------------------------------------------------------------
        Deassert flow control on the Um and the Rm interfaces
        ---------------------------------------------------------------*/
      DS_SIO_MSG0_HIGH(" SMALL ITEM FEW ITEMS MARK Reached");
      for (port_count = port_init; 
            port_count < DS3G_SIOLIB_PORT_MAX; port_count++) 
      {         
        ds3g_siolib_set_port_inbound_flow(port_count, DS_FLOW_DSM_SMALL_Q_MASK,
                                   DS_FLOW_DISABLE);
      }
      break;
      
    case DSM_MEM_LEVEL_DNE:
      DS_SIO_MSG0_HIGH(" SMALL ITEM DO NOT EXCEED Reached");
      break;
      
    default:
      DS_SIO_MSG0_HIGH("Do nothing");
    }
  }
  else if(pool_id == DSM_DS_LARGE_ITEM_POOL)
  {
    switch (event_type)
    {
    case DSM_MEM_LEVEL_SIO_MANY:
      /*-----------------------------------------------------------------
        Do buffer based flow control. If it seems that we are recovering
        from a lack of items, enable inbound flow again.
      ----------------------------------------------------------------*/
      DS_SIO_MSG0_HIGH(" LARGE ITEM MANY ITEMS MARK Reached");
      for (port_count = port_init; 
            port_count < DS3G_SIOLIB_PORT_MAX; port_count++) 
      {         
        ds3g_siolib_set_port_inbound_flow(port_count, DS_FLOW_DSM_LARGE_Q_MASK,
                                          DS_FLOW_ENABLE);
      }
      
      break;
      
    case DSM_MEM_LEVEL_SIO_FEW:
      /*-----------------------------------------------------------------
        Deassert flow control on the Um and the Rm interfaces
      ---------------------------------------------------------------*/
      DS_SIO_MSG0_HIGH(" LARGE ITEM FEW ITEMS MARK Reached");
      
      for (port_count = port_init; 
            port_count < DS3G_SIOLIB_PORT_MAX; port_count++) 
      {         
        ds3g_siolib_set_port_inbound_flow(port_count, DS_FLOW_DSM_LARGE_Q_MASK,
                                          DS_FLOW_DISABLE);
      }
      
      break;
      
    case DSM_MEM_LEVEL_DNE:
      DS_SIO_MSG0_HIGH(" LARGE ITEM DO NOT EXCEED Reached");
      break;
      
    default:
      DS_SIO_MSG0_HIGH("Do nothing");
    }
  }
  else 
  {
    DS_SIO_MSG0_HIGH("Do nothing");
  } 
  
} /* ds3g_siolib_mem_event_ctrl */

/*===========================================================================

FUNCTION DS3G_SIOLIB_REGISTER_CALLBACK_FUNC

DESCRIPTION
  This function registers a mode specific call back function table.

  Caller should check the return value of this function to see if the
  registration succeeded.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if a func_tbl is not registered with SIOLIB already and
  the function pointer passed in is not NULL, otherwise returns FALSE;

SIDE EFFECTS
  None
===========================================================================*/

boolean  ds3g_siolib_register_callback_func
(
  ds3g_siolib_func_tbl_type  *func_tbl_ptr        /* Handler function table */
)
{
  return ds3g_siolib_ex_register_callback_func(func_tbl_ptr, ds3g_siolib_modem_port);
} /* ds3g_siolib_register_callback_func() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_DEREGISTER_CALLBACK_FUNC

DESCRIPTION
  This function deregisters a mode specific call back function table and
  resets to NULL.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if deregister happened successfully.
  Returns FALSE if func ptr passed in does not match siolib table value.

SIDE EFFECTS
  None
===========================================================================*/

boolean  ds3g_siolib_deregister_callback_func
(
  ds3g_siolib_func_tbl_type  *func_tbl_ptr
)
{
  return ds3g_siolib_ex_deregister_callback_func(func_tbl_ptr, ds3g_siolib_modem_port);
} /* ds3g_siolib_deregister_callback_func() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_SET_CALL_COMING_UP

DESCRIPTION
  This function sets the call coming up boolean variable which determines
  if a data call is in the process of coming up.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void  ds3g_siolib_set_call_coming_up
(
    boolean busy
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds3g_siolib_ex_set_call_coming_up(ds3g_siolib_modem_port, busy);

} /* ds3g_siolib_set_call_coming_up() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_DTR_CHANGED_CB

DESCRIPTION
  Callback function registered with SIO, used to inform DS of any changes in
  the state of the DTR signal on any port

  This function simply posts a DS_DTR_EVENT_CMD to DS.

DEPENDENCIES
  None

PARAMETERS 
  Callback Data - Port Id 
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void ds3g_siolib_dtr_changed_cb( void* cb_data )
{
  ds_cmd_type                    *cmd_ptr = NULL;
  ds3g_siolib_dtr_chg_cmd_type   *dtr_changed_cmd_ptr = NULL;
  ds3g_siolib_port_e_type         port = (ds3g_siolib_port_e_type)cb_data;
  dsm_item_type                  *dsm_item_ptr = NULL;
  ds3g_siolib_portid_e_type       port_index;
  boolean                         port_is_allocated = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(port > DS3G_SIOLIB_PORT_MAX || port <= DS3G_SIOLIB_PORT_NONE)
  {
    DS_SIO_MSG0_ERROR("DTR CB on invalid port");
    return;
  }

  dsm_item_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
  if (NULL != dsm_item_ptr)
  {
    if (dsm_pushdown_tail(&dsm_item_ptr, &port,
                          sizeof(ds3g_siolib_port_e_type),
                          DSM_ITEM_POOL(dsm_item_ptr)
                         ) == sizeof(ds3g_siolib_port_e_type))
    {
      /* Add to queue and raise subtask signal */
      dsm_enqueue(&ds3g_siolib_dtr_event_wm, &dsm_item_ptr);
    }
    else
    {
      DS_SIO_MSG0_ERROR("Failed to pushdown DSM item for DTR event");
      dsm_free_packet(&dsm_item_ptr);
    }
  }
  else
  {
    DS_SIO_MSG0_ERROR("Failed to allocate DSM item for DTR event");
  }

  port_index = ds3g_siolib_get_index_from_allocated_port(port);
  DS_SIO_MSG2_MED( "ds3g_siolib_dtr_changed_cb: port_index %d port %d ", port_index, port); 
  if(port_index == DS3G_SIOLIB_PORTID_NONE)
  {
    port_is_allocated = FALSE;
  }
  else
  {
    port_is_allocated = TRUE;
  }

  DS_SIO_MSG3_MED
  ( "ds3g_siolib_dtr_changed_cb: ds3g_siolib_current_ring_type %d port_is_allocated %d "
    "ds3g_siolib_dtr_handling_pending_flag_is_set %d", 
     ds3g_siolib_current_ring_type, 
     port_is_allocated, 
     ds3g_siolib_dtr_handling_pending_flag_is_set(port) 
  ); 

  if(((DS3G_SIOLIB_RING_TYPE_NONE == ds3g_siolib_current_ring_type) &&
       !port_is_allocated) 
         || 
       ds3g_siolib_dtr_handling_pending_flag_is_set(port)) 
  {
    DS_SIO_MSG1_MED( "Invalid DTR change: Port Number %d ", port);
    return;
  }

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_siolib_dtr_chg_cmd_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  ds3g_siolib_set_dtr_handling_pending_flag(TRUE, port);
  cmd_ptr->hdr.cmd_id = DS_CMD_SIO_DTR_CHANGED;
  dtr_changed_cmd_ptr 
    = (ds3g_siolib_dtr_chg_cmd_type*)cmd_ptr->cmd_payload_ptr;

  dtr_changed_cmd_ptr->port_id = (int)port;
  ds_put_cmd( cmd_ptr );

  return;
}

#ifdef FEATURE_DSAT_MDM_FUSION
/*===========================================================================

FUNCTION DS3G_SIOLIB_HSUART_CD_CHANGED_CB

DESCRIPTION
  Callback function registered with HSUART, used to notify RmSm proxy when
  a change in the Carrier Detect (CD) signal occurs.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_siolib_hsuart_cd_changed_cb 
(
  void *user_data_ptr
)
{
  ds_cmd_type                  *cmd_ptr = NULL;
  ds3g_siolib_cd_chg_cmd_type  *cd_chg_cmd_ptr = NULL;
  sio_port_id_type              port_id = (sio_port_id_type)user_data_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SIO_MSG1_HIGH("HSUART CD changed callback for port: %d", port_id);

  if (port_id == NULL) 
  {
    DS_SIO_MSG0_ERROR("Invalid HSUART Port Id received");
    ASSERT(0);
    return;
  }

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_siolib_cd_chg_cmd_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_SIO_CD_CHANGED;
  cd_chg_cmd_ptr = (ds3g_siolib_cd_chg_cmd_type*)cmd_ptr->cmd_payload_ptr;

  cd_chg_cmd_ptr->port_id = (int)ds3g_siolib_map_sio_port(port_id);
  ds_put_cmd( cmd_ptr );

  return;
}

#endif /* FEATURE_DSAT_MDM_FUSION */


/*===========================================================================

FUNCTION DS3G_SIOLIB_ESCAPE_DETECTED_CB

DESCRIPTION
  Callback function registered with SIO, used to notify data services when
  the escape code (+++) is detected (in Rawdata Mode).

  This function simply sets the DS_SIO_ESCAPE_SIG.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3g_siolib_escape_detected_cb( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void)rex_set_sigs( dsi_get_ds_tcb(), DS_SIO_ESCAPE_SIG );

} /* ds3g_siolib_escape_detected_cb() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_DTR_EVENT_WM_NON_EMPTY_CB

DESCRIPTION
  Callback function invoked when the DTR event watermark item goes non-empty.
  This function sets the DTR event signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL void ds3g_siolib_dtr_event_wm_non_empty_cb
(
  struct dsm_watermark_type_s *wm_ptr,
  void                        *user_data_ptr
)
{
  (void) rex_set_sigs(dsi_get_ds_tcb(), DS_DTR_EVENT_SIG);
} /* ds3g_siolib_dtr_event_wm_non_empty_cb() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_SETUP_DTR_EVENT_WATERMARKS

DESCRIPTION
  This function initializes the watermarks used for receiving DTR events.
  The lowater, hiwater and don't exceed counts are configured.
  Queue to be used by WM is also initialized.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void ds3g_siolib_setup_dtr_event_watermarks(void)
{
  /*-------------------------------------------------------------------------
    Initialize, set Don't Exceed count and register the associated queue
  -------------------------------------------------------------------------*/
  dsm_queue_init(&ds3g_siolib_dtr_event_wm,
                 DS3G_SIOLIB_DTR_EVENT_DONT_EXCEED_CNT,
                 &ds3g_siolib_dtr_event_q);

  /*-------------------------------------------------------------------------
    Set the Lo Water and Hi Water  counts for the watermark.
  -------------------------------------------------------------------------*/
  ds3g_siolib_dtr_event_wm.lo_watermark = DS3G_SIOLIB_DTR_EVENT_LOWATER_CNT;
  ds3g_siolib_dtr_event_wm.hi_watermark = DS3G_SIOLIB_DTR_EVENT_HIWATER_CNT;

  /*-------------------------------------------------------------------------
    Register callback functions for the watermark flow control.
  -------------------------------------------------------------------------*/
  ds3g_siolib_dtr_event_wm.lowater_func_ptr   = NULL;
  ds3g_siolib_dtr_event_wm.hiwater_func_ptr   = NULL;
  ds3g_siolib_dtr_event_wm.non_empty_func_ptr = ds3g_siolib_dtr_event_wm_non_empty_cb;

  return;
} /* ds3g_siolib_setup_dtr_event_watermarks */

/*===========================================================================

FUNCTION DS3G_SIOLIB_HANDLE_DTR_EVENT_SIG

DESCRIPTION
  This function is to handle DTR event when SIOLIB receives a DS_DTR_EVENT_SIG signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void ds3g_siolib_handle_dtr_event_sig(void)
{
  uint8                        prev_dtr_sig_cntr;
  dsm_item_type               *item_ptr;
  ds3g_siolib_port_e_type      port_id;
  boolean                      dtr_status = FALSE;
  ds3g_siolib_err_code_e_type  error_code = DS3G_SIOLIB_ERR_NONE;

  prev_dtr_sig_cntr = ds3g_siolib_dtr_sig_cntr;

  /*-------------------------------------------------------------------------
    De-queue dsm items and process DTR event data until the watermark is empty.
  -------------------------------------------------------------------------*/
  while (NULL != (item_ptr = dsm_dequeue(&ds3g_siolib_dtr_event_wm)))
  {
    if (sizeof(ds3g_siolib_port_e_type) ==
        dsm_pullup(&item_ptr, &port_id, sizeof(ds3g_siolib_port_e_type)))
    {
      if (DS3G_SIOLIB_OK == ds3g_siolib_ex_is_dtr_asserted(&dtr_status, port_id))
      {
        DS_SIO_MSG2_MED("DTR Status = %d, Port ID = %d", dtr_status, port_id);
 
        if (TRUE == dtr_status)
        {
          if (ds3g_siolib_dtr_sig_cntr < DS3G_SIOLIB_DTR_SIG_CNTR_MAX)
          {
            ds3g_siolib_dtr_sig_cntr++;
          }
          else
          {
            error_code = DS3G_SIOLIB_ERR_DTR_SIG_CNTR_MAX;
          }
        }
        else
        {
          if (ds3g_siolib_dtr_sig_cntr > 0)
          {
            ds3g_siolib_dtr_sig_cntr--;
          }
          else
          {
            error_code = DS3G_SIOLIB_ERR_DTR_SIG_CNTR_MIN;
          }
        }
      }
      else
      {
        error_code = DS3G_SIOLIB_ERR_INVALID_DTR_STATUS;
      }
    }
    else
    {
      error_code = DS3G_SIOLIB_ERR_INVALID_DTR_EVENT;
    }

    dsm_free_packet(&item_ptr);
  }

  DS_SIO_MSG2_HIGH("DTR signal counters: prev = %d, curr = %d",
                   prev_dtr_sig_cntr, ds3g_siolib_dtr_sig_cntr);

  if (DS3G_SIOLIB_ERR_NONE == error_code)
  {
    if ((prev_dtr_sig_cntr == 0) && (ds3g_siolib_dtr_sig_cntr > 0))
    {
      DSAT_DL_CMD_HANDLER(DSAT_DL_CMD_LOAD);
    }
    else if ((prev_dtr_sig_cntr > 0) && (ds3g_siolib_dtr_sig_cntr == 0))
    {
      DSAT_DL_CMD_HANDLER(DSAT_DL_CMD_UNLOAD);
    }
  }
  else
  {
    DS_SIO_MSG1_ERROR("DTR event handling error %d", error_code);
  }

  return;
} /* ds3g_siolib_handle_dtr_event_sig() */

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION DS3G_SIOLIB_INIT

DESCRIPTION
  This function performs power-up initialization of SIOLIB.  This includes
  initialization of some state variables and watermark pointers for all the
  ports.

  This function must be called once at data services task startup.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

rex_sigs_type ds3g_siolib_init ( void )
{
  rex_sigs_type  signal_mask;                    /* Signal mask to wait on */
  ds3g_siolib_port_e_type port_id;
  ds3g_siolib_port_e_type port_init =DS3G_SIOLIB_PORT_NONE;
  ds3g_siolib_state_info_type *state = NULL;
  int temp =0; /* temporary variable for initializing the array */
#ifdef FEATURE_DSAT_MDM_FUSION
  uint8 i;
#endif

  boolean rdm_ret = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS3G_SIOLIB_RX_SIG_CRIT_SECT_INIT();

 /*-----------------------------------------------------------------------
   Increment the enumerator to point to DS3G_SIOLIB_PORT_NONE+1
 ------------------------------------------------------------------------*/
  port_init++;

  /*-------------------------------------------------------------------------
    Initialize SIOLIB state information for all ports.
  -------------------------------------------------------------------------*/
  for(port_id= port_init; port_id < DS3G_SIOLIB_PORT_MAX; 
       port_id++)
  {
    state = &ds3g_siolib_state[port_id];
    state->serial_info.stream_id = SIO_NO_STREAM_ID;
    state->serial_info.port_id = SIO_PORT_NULL;
    state->serial_info.close_in_progress = FALSE;
    state->serial_info.serial_state = DS3G_SIOLIB_NULL_SERIAL_STATE;
    state->serial_info.flow_ctl_mask = (uint32)0xFFFFFFFF;
    state->at_state = DS3G_SIOLIB_NULL_AT_STATE;
    state->call_coming_up = FALSE;
    state->func_tbl = NULL;
    state->ring_counter = 0;
    state->flow_status = DS3G_SIOLIB_FLOW_STATUS_NONE;
    state->serial_info.rx_sig = FALSE;
    state->serial_info.rx_sig_val = 0;
    state->is_dtr_high = TRUE;
#ifdef FEATURE_DSAT_MDM_FUSION
    if ((port_id == DS3G_SIOLIB_HSUART1_PORT) || 
        (port_id == DS3G_SIOLIB_HSUART2_PORT))
    {
      state->forwarding_port_state = DS3G_SIOLIB_FORWARDING_PORT_AVAILABLE_STATE;
    }
    else
    {
      state->forwarding_port_state = DS3G_SIOLIB_FORWARDING_PORT_INVALID_STATE;
    }
#endif

    state->dtr_handling_pending_flag = FALSE;

    rex_init_crit_sect(&state->port_info_crit_sec);
	
    switch(port_id)
    {
      case DS3G_SIOLIB_UART1_PORT:
        state->serial_info.rx_pending_bit = DS3G_SIOLIB_PORT_UART1_BIT;
        break;
      case DS3G_SIOLIB_USB_CDC_PORT:
        state->serial_info.rx_pending_bit = DS3G_SIOLIB_PORT_USB_CDC_BIT;
        state->is_dtr_high = FALSE;
        break;
      case DS3G_SIOLIB_BT_NA_PORT:
        state->serial_info.rx_pending_bit = DS3G_SIOLIB_PORT_BT_NA_BIT;
        break;
      case DS3G_SIOLIB_BT_SPP_PORT:
        state->serial_info.rx_pending_bit = DS3G_SIOLIB_PORT_BT_SPP_BIT;
        break;
      case DS3G_SIOLIB_IRCOMM_PORT:
        state->serial_info.rx_pending_bit = DS3G_SIOLIB_PORT_IRCOMM_BIT;
        break;
#if defined(FEATURE_MULTIPROCESSOR) && defined(FEATURE_SMD)
      case DS3G_SIOLIB_SMD_DS_PORT:
        state->serial_info.rx_pending_bit = DS3G_SIOLIB_PORT_SMD_DS_BIT;
        break;
      case DS3G_SIOLIB_SMD_DATA1_PORT:
        state->serial_info.rx_pending_bit = DS3G_SIOLIB_PORT_SMD_DATA1_BIT;
        break;
      case DS3G_SIOLIB_SMD_DATA2_PORT:
        state->serial_info.rx_pending_bit = DS3G_SIOLIB_PORT_SMD_DATA2_BIT;
        break;
      case DS3G_SIOLIB_SMD_DATA3_PORT:
        state->serial_info.rx_pending_bit = DS3G_SIOLIB_PORT_SMD_DATA3_BIT;
        break;
      case DS3G_SIOLIB_SMD_DATA4_PORT:
        state->serial_info.rx_pending_bit = DS3G_SIOLIB_PORT_SMD_DATA4_BIT;
        break;
      case DS3G_SIOLIB_SMD_DATA11_PORT:
        state->serial_info.rx_pending_bit = DS3G_SIOLIB_PORT_SMD_DATA11_BIT;
        break;
#endif /* if defined(FEATURE_MULTIPROCESSOR... */
#if defined(FEATURE_DATA_A2) && !defined(FEATURE_DATA_SDIO_MUX_A2_DUN)
      case DS3G_SIOLIB_A2_MDM_PORT:
        state->serial_info.rx_pending_bit = DS3G_SIOLIB_PORT_A2_MDM_BIT;
        state->is_dtr_high = FALSE;
        break;
#endif /*FEATURE_DATA_A2 && !FEATURE_DATA_SDIO_MUX_A2_DUN*/
#ifdef FEATURE_DATA_SDIO_MUX_A2_DUN
      case DS3G_SIOLIB_SDIO_MUX_A2_DUN_0_PORT:
        state->serial_info.rx_pending_bit = 
          DS3G_SIOLIB_PORT_SDIO_MUX_A2_DUN_0_BIT;
          state->is_dtr_high = FALSE;
        break;
#endif /*FEATURE_DATA_SDIO_MUX_A2_DUN*/
#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT
      case DS3G_SIOLIB_CLIENT_VSP_PORT:
        state->serial_info.rx_pending_bit = DS3G_SIOLIB_PORT_CLIENT_VSP_BIT;
        state->serial_info.rx_sig = FALSE;
        state->at_state = DS3G_SIOLIB_COMMAND_AT_STATE;
        break;
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT */
#if defined(FEATURE_DSAT_MDM_FUSION) || defined(FEATURE_DSAT_MSM_FUSION)
      case DS3G_SIOLIB_HSUART1_PORT:
        state->serial_info.rx_pending_bit = DS3G_SIOLIB_PORT_HSUART1_BIT;
        break;
      case DS3G_SIOLIB_HSUART2_PORT:
        state->serial_info.rx_pending_bit = DS3G_SIOLIB_PORT_HSUART2_BIT;
      case DS3G_SIOLIB_HSUART3_PORT:
        state->serial_info.rx_pending_bit = DS3G_SIOLIB_PORT_HSUART3_BIT;
#endif

#ifdef FEATURE_DATA_MUX
      case DS3G_SIOLIB_DATA_MUX_1_PORT:
        state->serial_info.rx_pending_bit = DS3G_SIOLIB_PORT_MUX_DATA1_BIT;
        break;
      case DS3G_SIOLIB_DATA_MUX_2_PORT:
        state->serial_info.rx_pending_bit = DS3G_SIOLIB_PORT_MUX_DATA2_BIT;
        break;
      case DS3G_SIOLIB_DATA_MUX_3_PORT:
        state->serial_info.rx_pending_bit = DS3G_SIOLIB_PORT_MUX_DATA3_BIT;
        break;
#endif /* FEATURE_DATA_MUX */
      default:
        break;
    }

    /*-------------------------------------------------------------------------
      Setup the Autodetect watermark items.
    -------------------------------------------------------------------------*/
    ds3g_siolib_setup_watermarks(port_id);
  }

  /*-------------------------------------------------------------------------
      Setup the DTR event watermark items.
    -------------------------------------------------------------------------*/
  ds3g_siolib_setup_dtr_event_watermarks();

  /*-------------------------------------------------------------------------
    Initialize and Register Wink DCD and RI timer expiration callbacks.
  -------------------------------------------------------------------------*/

  memset(&ds3g_siolib_ring_timer[DS3G_SIOLIB_PORTID_0],0,sizeof(rex_timer_type));
  memset(&ds3g_siolib_ring_timer[DS3G_SIOLIB_PORTID_1],0,sizeof(rex_timer_type));
  memset(&ds3g_siolib_dcd_wink_timer[DS3G_SIOLIB_PORTID_0],0,sizeof(rex_timer_type));
  memset(&ds3g_siolib_dcd_wink_timer[DS3G_SIOLIB_PORTID_1],0,sizeof(rex_timer_type));

  ds3gi_timer_register ( &ds3g_siolib_ring_timer[DS3G_SIOLIB_PORTID_0],
                               ds3g_siolib_timer_cb,
                               DS3G_TIMER_RING_TE_1,
                          DS3G_TIMER_TYPE_REX_TIMER);

  ds3gi_timer_register ( &ds3g_siolib_dcd_wink_timer[DS3G_SIOLIB_PORTID_0],
                               ds3g_siolib_timer_cb,
                               DS3G_TIMER_WINK_DCD_1,
                          DS3G_TIMER_TYPE_REX_TIMER);

  ds3gi_timer_register ( &ds3g_siolib_ring_timer[DS3G_SIOLIB_PORTID_1],
                               ds3g_siolib_timer_cb,
                               DS3G_TIMER_RING_TE_2,
                          DS3G_TIMER_TYPE_REX_TIMER );

  ds3gi_timer_register ( &ds3g_siolib_dcd_wink_timer[DS3G_SIOLIB_PORTID_1],
                               ds3g_siolib_timer_cb,
                               DS3G_TIMER_WINK_DCD_2,
                          DS3G_TIMER_TYPE_REX_TIMER);

  /*-------------------------------------------------------------------------
    Register open and close routines with RDM. These are called by RDM to
    tell SIOLIB to open or close a serial device, respectively.
  -------------------------------------------------------------------------*/
  rdm_ret = rdm_register_func_multi_dev(RDM_DATA_SRVC, ds3gi_rdm_open_cb, 
                                        ds3gi_rdm_close_cb, NULL);
  if (rdm_ret == FALSE) 
  {
    DS_SIO_MSG0_ERROR("rdm_register_func_multi_dev failed");
  }

#ifdef FEATURE_DSAT_MDM_FUSION
  for (i=0 ; i< NUM_FORWARDING_PORTS_MAX; i++) 
  {
    forwarding_port_tbl[i].phys_port = DS3G_SIOLIB_PORT_NONE;
    forwarding_port_tbl[i].forwarding_port = DS3G_SIOLIB_PORT_NONE;
  }
#endif

  /*-------------------------------------------------------------------------
    Create the signal mask containing the SIOLIB sub-task signals.
  -------------------------------------------------------------------------*/
  signal_mask = DS_DATA_PORT_1ST_SIO_RX_SIG | DS_DTR_EVENT_SIG |
                DS_SIO_RX_PKT_SIG | DS_SIO_ESCAPE_SIG;


  /*-------------------------------------------------------------------------
    Initiialize module global variables.
  -------------------------------------------------------------------------*/
  memset((void*)&ds3g_siolib_answer_info, 0x0,
         sizeof(ds3g_siolib_answer_info));
  for(temp=0; temp<(int)DS3G_SIOLIB_PORTID_MAX; temp++) 
  {
    ds3g_siolib_port_allocated[temp]=DS3G_SIOLIB_PORT_NONE;
    ds3g_siolib_last_port_allocated[temp]=DS3G_SIOLIB_PORT_NONE;
    memset((void*)&ds3g_siolib_port_signal_status[temp], 0x0,
           sizeof(ds3g_siolib_port_signal_status[temp]));
  }

  return( signal_mask );
} /* ds3g_siolib_init() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_OPEN

DESCRIPTION
  This function opens the specified serial port. Received data bytes will be
  stored in the RX watermark specified by the caller. Data bytes pending
  transmission will be stored in the TX watermark specified by the caller.

  The port is opened in Autodetect mode, and the baud rate is set according
  to the +IPR setting. Callback functions for DTR state changes and packet
  detection are registered with SIO. RS-232 control signals are initialized
  as follows:
    CD  - Initialized per the &C setting
    CTS - Asserted
    RI  - De-asserted
    DSR - Asserted

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_open
(
  sio_port_id_type      port_id,       /* Port id of the port to be opened */
  dsm_watermark_type   *rx_wm_ptr,       /* Watermark for rx data from SIO */
  dsm_watermark_type   *tx_wm_ptr          /* Watermark for tx data to SIO */
)
{
  sio_open_type           open_param;
  sio_ioctl_param_type    ioctl_param;
  ds3g_siolib_status_e_type  ret_val = DS3G_SIOLIB_OK;
  ds3g_siolib_port_e_type open_port = DS3G_SIOLIB_PORT_NONE;
#if defined(FEATURE_BT) || defined(FEATURE_BT_MODEM)
  bt_spp_open_type bso;
  char bt_svc_name[] = "Serial Port Profile";
#endif /* FEATURE_BT || FEATURE_BT_MODEM */
  ds3g_siolib_state_info_type *state;

#if defined(FEATURE_MULTIPROCESSOR) && defined(FEATURE_SMD)
  boolean call_drop_pending_xmit_data_ioctl;
#endif /* #if defined(FEATURE_MULTIPROCESSOR) && defined(FEATURE_SMD) */
#ifdef FEATURE_DATA_A2
  sio_open_param_type ds_sio_open_param;
#endif

#ifdef FEATURE_DATA_SDIO_MUX_A2_DUN
  sio_open_param_type ds_sdio_mux_a2_dun_0_sio_open_param;
#endif /*FEATURE_DATA_SDIO_MUX_A2_DUN*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&open_param,0x0,sizeof(sio_open_type));
  open_port = ds3g_siolib_map_sio_port(port_id);

  if(open_port == DS3G_SIOLIB_PORT_NONE)
  {
    DATA_ERR_FATAL("open port is DS3G_SIOLIB_PORT_NONE");
    return(DS3G_SIOLIB_NO_PORT_OPEN);
  }

#ifdef FEATURE_DSAT_MDM_FUSION
  if (open_port == DS3G_SIOLIB_HSUART1_PORT ||
      open_port == DS3G_SIOLIB_HSUART2_PORT ||
      open_port == DS3G_SIOLIB_HSUART3_PORT ) 
  {
    return ds3g_siolib_hsuart_open (port_id, rx_wm_ptr, tx_wm_ptr);
  }  
#endif /* FEATURE_DSAT_MDM_FUSION */

  state = &ds3g_siolib_state[open_port];

  /*-------------------------------------------------------------------------
    Set the port id and rx/tx watermarks to the values provided by the caller
    of this function.
  -------------------------------------------------------------------------*/
  open_param.port_id = port_id;
  open_param.rx_queue = rx_wm_ptr;
  open_param.tx_queue = tx_wm_ptr;

  /*-------------------------------------------------------------------------
    Set the remaining parameters to the appropriate values. A serial port is
    always opened in Autodetect Mode. The baud rate is set according to the
    +IPR setting.
  -------------------------------------------------------------------------*/
  open_param.stream_mode = SIO_DS_AUTODETECT_MODE;
  open_param.rx_func_ptr = NULL;
  open_param.rx_bitrate = (sio_bitrate_type)(dsat_num_item_type)dsatutil_get_val(DSAT_EXT_IPR_IDX,0,0,NUM_TYPE);
  open_param.tx_bitrate = (sio_bitrate_type)(dsat_num_item_type)dsatutil_get_val(DSAT_EXT_IPR_IDX,0,0,NUM_TYPE);
  open_param.tail_char_used = FALSE;
  open_param.rx_flow = SIO_FCTL_BEST;
  open_param.tx_flow = SIO_FCTL_BEST;

#if defined(FEATURE_DATA_A2) && !defined(FEATURE_DATA_SDIO_MUX_A2_DUN)
  memset (&ds_sio_open_param,0x0,sizeof(sio_open_param_type));
  if(open_port == DS3G_SIOLIB_A2_MDM_PORT)
  {
    ds_sio_open_param.usb_end_point_info.mode = 
      (sio_usb_mode_enum_type) SIO_USB_MODE_SPS_USB_BAM;
    open_param.open_param = &ds_sio_open_param;
    open_param.port_id = SIO_PORT_A2_MDM;
  }
  else if(open_port == DS3G_SIOLIB_USB_CDC_PORT)
  {
    ds_sio_open_param.usb_end_point_info.mode = 
      (sio_usb_mode_enum_type) SIO_USB_MODE_LEGACY;
    open_param.open_param = &ds_sio_open_param;
    open_param.port_id = port_id;
  }
  else
  {
    DS_SIO_MSG0_HIGH("open_param.open_param is set to NULL");
  }
#endif /* FEATURE_DATA_A2 && !FEATURE_DATA_SDIO_MUX_A2_DUN*/

#ifdef FEATURE_DATA_SDIO_MUX_A2_DUN
  memset (&ds_sdio_mux_a2_dun_0_sio_open_param,
          0x0,
          sizeof(sio_open_param_type));
  if(open_port == DS3G_SIOLIB_SDIO_MUX_A2_DUN_0_PORT)
  {
    ds_sdio_mux_a2_dun_0_sio_open_param.usb_end_point_info.mode = 
      (sio_usb_mode_enum_type) SIO_USB_MODE_SPS_USB_BAM;
    open_param.open_param = &ds_sdio_mux_a2_dun_0_sio_open_param;
    open_param.port_id = SIO_PORT_SDIO_MUX_A2_DUN_0;
  }
  else if(open_port == DS3G_SIOLIB_USB_CDC_PORT)
  {
    ds_sio_open_param.usb_end_point_info.mode = 
      (sio_usb_mode_enum_type) SIO_USB_MODE_LEGACY;
    open_param.open_param = &ds_sio_open_param;
    open_param.port_id = port_id;
  }
  else
  {
    DS_SIO_MSG0_HIGH("open_param.open_param is set to NULL");
  }
#endif /* FEATURE_DATA_SDIO_MUX_A2_DUN */

#if defined(FEATURE_BT) || defined(FEATURE_BT_MODEM)
  if(port_id == SIO_PORT_BT_SPP)
  {
    /*-----------------------------------------------------------------------
      When using BT_SPP port, Bluetooth specific information is needed
      in order to open a server SIO stream that waits for connection 
      requests from other Bluetooth devices (in this case, any laptops 
      equipped with Bluetooth PC card) Fill in Bluetooth specific 
      information 
    -----------------------------------------------------------------------*/
    open_param.bt_open_ptr   = &bso;
    bso.client_app           = FALSE; /* always server */
    bso.bd_addr_ptr          = NULL;  /* not used for server */
    bso.status_change_fptr   = ds3g_siolib_update_sio_conn_status;
    bso.config_change_fptr   = NULL;
    bso.modem_status_fptr    = NULL;
    bso.line_error_fptr      = NULL;
    bso.max_frame_size       = BT_SPP_MFS_AUTOMATIC;
    bso.rc_server_channel    = BT_SPP_SCN_UNSPECIFIED;
    bso.service_uuid         = BT_SD_SERVICE_CLASS_SERIAL_PORT;
    bso.service_version      = 0;
    bso.service_name_str_ptr = bt_svc_name;
    ds3g_siolib_bt_spp_status.spp_state  = BT_SPP_ST_CLOSED;
  }
#endif /* FEATURE_BT || FEATURE_BT_MODEM */

  /*-------------------------------------------------------------------------
    Open the serial port, and store the assigned stream id. If the open was
    unsuccessful, log an error.
  -------------------------------------------------------------------------*/
  if( (state->serial_info.stream_id = sio_open( &open_param )) == 
      SIO_NO_STREAM_ID )
  {
    DS_SIO_MSG0_ERROR( "Data Services couldn't open SIO stream" );
    ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  }
  else
  {
#if defined(FEATURE_BT) || defined(FEATURE_BT_MODEM)
    /*
       The SIO stream opened for BT_SPP is not considered opened
       until it reaches BT_SPP_ST_OPEN state;  if Bluetooth driver
       fails in setting up the BT_SPP port for Bluetooth connection,
       it sets the state to BT_SPP_ST_OPEN_ERROR, in which case the
       SIO stream should be closed;
    */
    if ( port_id == SIO_PORT_BT_SPP )
    {
      if ( (ds3g_siolib_bt_spp_status.spp_state == BT_SPP_ST_CLOSED) ||
           (ds3g_siolib_bt_spp_status.spp_state == BT_SPP_ST_OPENING) )
      {
        ds3g_siolib_block_task ();
      }
      if ( ds3g_siolib_bt_spp_status.spp_state != BT_SPP_ST_OPEN )
      {
        sio_close (state->serial_info.stream_id, NULL);
        state->serial_info.stream_id = SIO_NO_STREAM_ID;
        return DS3G_SIOLIB_NO_PORT_OPEN;
      }
    }
#endif /* FEATURE_BT || FEATURE_BT_MODEM */

    /*-----------------------------------------------------------------------
      Register callback functions for serial port events of interest, namely:
      changes in DTR state and packet detection.
    -----------------------------------------------------------------------*/
    switch(port_id) 
    {
      case SIO_PORT_UART_MAIN:
#ifdef FEATURE_USB_CDC_ACM
      case SIO_PORT_USB_CDC_ACM:
#endif /* FEATURE_USB_CDC_ACM */
#if defined(FEATURE_BT) || defined(FEATURE_BT_MODEM)
      case SIO_PORT_BT_NA:
      case SIO_PORT_BT_SPP:
#endif /* FEATURE_BT || FEATURE_BT_MODEM */
#ifdef FEATURE_IRDA
      case SIO_PORT_IRCOMM:
#endif /* FEATURE_IRDA */
#if defined(FEATURE_MULTIPROCESSOR) && defined(FEATURE_SMD)
      case SIO_PORT_SMD_DS:
      case SIO_PORT_SMD_DATA1:
      case SIO_PORT_SMD_DATA2:
      case SIO_PORT_SMD_DATA3:
      case SIO_PORT_SMD_DATA4:
      case SIO_PORT_SMD_DATA11:
#endif /* if defined(FEATURE_MULTI_PROCESSOR)... */
#ifdef FEATURE_DSAT_MSM_FUSION
      case SIO_PORT_DATA_MUX_2:
      case SIO_PORT_DATA_MUX_3:
      case SIO_PORT_DATA_MUX_4:
#endif

#ifdef FEATURE_DATA_MUX
      case SIO_PORT_DATA_MUX_1:
      case SIO_PORT_DATA_MUX_2:
      case SIO_PORT_DATA_MUX_3:
#endif /* FEATURE_DATA_MUX */
      case SIO_PORT_USB_CSVT: 
        DS_SIO_MSG3_MED("ds3g_siolib_open port_id : %d", port_id,0,0);
        /* Register for DTR event with DS3G_SIO port */
        ioctl_param.enable_dte_ready_event_ext.cb_func = ds3g_siolib_dtr_changed_cb;
        ioctl_param.enable_dte_ready_event_ext.cb_data = (void*)open_port;
        break;

      default:
        DATA_ERR_FATAL("Invalid Port being opened in ds3g_siolib_open");
        return(ret_val);
    }

    state->serial_info.port_id      = port_id;
    state->serial_info.serial_state =  DS3G_SIOLIB_AUTODETECT_SERIAL_STATE;
    
    sio_ioctl( state->serial_info.stream_id,
               SIO_IOCTL_ENABLE_DTR_EVENT_EXT,
               &ioctl_param );

#if defined(FEATURE_MULTIPROCESSOR) && defined(FEATURE_SMD)
    switch (port_id)
    {
      case SIO_PORT_SMD_DS:
      case SIO_PORT_SMD_DATA1:
      case SIO_PORT_SMD_DATA2:
      case SIO_PORT_SMD_DATA3:
      case SIO_PORT_SMD_DATA4:
      case SIO_PORT_SMD_DATA11:
        call_drop_pending_xmit_data_ioctl = TRUE;
        break;        

      default:
        call_drop_pending_xmit_data_ioctl = FALSE;
        break;
    }

    /*-----------------------------------------------------------------------
      Set the drop pending state for SMD ports
    -----------------------------------------------------------------------*/
    /* For shared memory ports, we assert the drop pending
    ** IOCTL which frees any data packets received if the
    ** other side of the SMD pipe is not opened
    */
    if (call_drop_pending_xmit_data_ioctl)
    {
      ioctl_param.drop_asserted = TRUE;
      sio_ioctl (state->serial_info.stream_id,
                 SIO_IOCTL_SET_DROP_PEND,
                 &ioctl_param);
    }
#endif /* #if defined(FEATURE_MULTIPROCESSOR) && defined(FEATURE_SMD) */
                 

    /*-----------------------------------------------------------------------
      Initialize the states of MT->TE serial port control signals.
    -----------------------------------------------------------------------*/
#if defined(FEATURE_HS_USB)
    ioctl_param.buffer_size = DS3G_SIOLIB_USB_BUFFER_SIZE;
    ds3g_siolib_ex_ioctl( SIO_IOCTL_SET_BUFFER_SIZE, &ioctl_param, open_port );
#endif
    ds3g_siolib_set_port_inbound_flow ( open_port, (uint32)DS_FLOW_ALL_MASK,
                                        DS_FLOW_ENABLE );
    (void)ds3g_siolib_ri_deassert(open_port);
  }

  return( ret_val );

} /* ds3g_siolib_open() */



/*===========================================================================

FUNCTION DS3G_SIOLIB_CLOSE

DESCRIPTION
  This function closes the specified serial port.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3g_siolib_close
( 
  ds3g_siolib_port_e_type port_id 
)
{
  ds3g_siolib_state_info_type *state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(port_id == DS3G_SIOLIB_PORT_NONE)
  {
    DATA_ERR_FATAL("input port is equal to DS3G_SIOLIB_PORT_NONE");
    return;
  }

  state = &ds3g_siolib_state[port_id];

  if ( state->call_coming_up || 
       ds3g_siolib_pending_close_port != DS3G_SIOLIB_PORT_NONE )
  {
    /*-----------------------------------------------------------------------
      We are busy, so don't close the port. Notify RDM that the close was
      unsuccessful. *** This logic will need to change when support for
      socket calls is added. ***
    -----------------------------------------------------------------------*/
    DS_SIO_MSG0_ERROR( "ds3g_siolib_close ignored, DS is busy" );
    return;
  } 


#if defined(FEATURE_BT) || defined(FEATURE_BT_MODEM)
  if(port_id == DS3G_SIOLIB_BT_SPP_PORT)
  {
    if (ds3g_siolib_bt_spp_status.spp_state == BT_SPP_ST_CONNECTED)
    {
      sio_ioctl( state->serial_info.stream_id, SIO_IOCTL_BT_DISCONNECT, NULL );
      if ( ds3g_siolib_bt_spp_status.spp_state != BT_SPP_ST_OPEN )
      {
        ds3g_siolib_block_task (); /* wait until server port becomes open (=disconnected) */
      }
    }
  }
#endif /* FEATURE_BT || FEATURE_BT_MODEM */

  if( state->serial_info.stream_id != SIO_NO_STREAM_ID )
  {
    ds3g_siolib_pending_close_port = port_id;
    state->serial_info.close_in_progress = TRUE;
    sio_close( state->serial_info.stream_id, ds3g_siolib_sio_close_cb );
    /* Do we need to disable the DTR event and/or reset RS-232 control
       signals here? */
  }
  else
  {
    DS_SIO_MSG1_ERROR("Call to ds3g_siolib_close on an already closed port: %d", port_id);
  }
} /* ds3g_siolib_close() */

#ifdef FEATURE_DSAT_MDM_FUSION
/*===========================================================================
FUNCTION DS3G_SIOLIB_HSUART_OPEN

DESCRIPTION
  This function opens the specified serial port. Received data bytes will be
  stored in the RX watermark specified by the caller. Data bytes pending
  transmission will be stored in the TX watermark specified by the caller.

  The port is opened in Autodetect mode, and the baud rate is set according
  to the +IPR setting. Callback functions for DTR state changes and packet
  detection are registered with SIO. RS-232 control signals are initialized
  as follows:
    CD  - Initialized per the &C setting

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_hsuart_open
(
  sio_port_id_type        port_id,
  dsm_watermark_type      *rx_wm_ptr,   /* Watermark for rx data from SIO */
  dsm_watermark_type      *tx_wm_ptr    /* Watermark for tx data to SIO */
)
{
  ds3g_siolib_port_e_type open_port;    /* Port id of the port to be opened */
  sio_open_type           open_param;
  sio_ioctl_param_type    ioctl_param;
  ds3g_siolib_status_e_type  ret_val = DS3G_SIOLIB_OK;

  ds3g_siolib_state_info_type *state;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  open_port = ds3g_siolib_map_sio_port(port_id);

  if(open_port == DS3G_SIOLIB_PORT_NONE)
  {
    DS_SIO_MSG0_ERROR("open port is DS3G_SIOLIB_PORT_NONE");
    ASSERT(0);
    return(DS3G_SIOLIB_NO_PORT_OPEN);
  }

  state = &ds3g_siolib_state[open_port];

  /*-------------------------------------------------------------------------
    Set the port id and rx/tx watermarks to the values provided by the caller
    of this function.
  -------------------------------------------------------------------------*/
  open_param.port_id = port_id;
  open_param.rx_queue = rx_wm_ptr;
  open_param.tx_queue = tx_wm_ptr;

  /*-------------------------------------------------------------------------
    Set the remaining parameters to the appropriate values. A serial port is
    always opened in Autodetect Mode. The baud rate is set according to the
    +IPR setting.
  -------------------------------------------------------------------------*/
  open_param.stream_mode = SIO_DS_AUTODETECT_MODE;
  open_param.rx_func_ptr = NULL;
  open_param.rx_bitrate = (sio_bitrate_type)(dsat_num_item_type)dsatutil_get_val(DSAT_EXT_IPR_IDX,0,0,NUM_TYPE);
  open_param.tx_bitrate = (sio_bitrate_type)(dsat_num_item_type)dsatutil_get_val(DSAT_EXT_IPR_IDX,0,0,NUM_TYPE);
  open_param.tail_char_used = FALSE;
  open_param.rx_flow = SIO_FCTL_BEST;
  open_param.tx_flow = SIO_FCTL_BEST;

  /*-------------------------------------------------------------------------
    Open the serial port, and store the assigned stream id. If the open was
    unsuccessful, log an error.
  -------------------------------------------------------------------------*/
  if( (state->serial_info.stream_id = sio_open( &open_param )) == 
      SIO_NO_STREAM_ID )
  {
    DS_SIO_MSG0_ERROR( "Data Services couldn't open SIO stream");
    ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Register callback functions for serial port events of interest, namely:
      changes in DTR state and packet detection.
    -----------------------------------------------------------------------*/
    switch(port_id) 
    {
      case SIO_PORT_DATA_MUX_2:
        ioctl_param.enable_cd_ready_event_ext.cb_func = 
          ds3g_siolib_hsuart_cd_changed_cb;
        ioctl_param.enable_cd_ready_event_ext.cb_data = 
          (void*)SIO_PORT_DATA_MUX_2;
        break;

      case SIO_PORT_DATA_MUX_3:
        ioctl_param.enable_cd_ready_event_ext.cb_func = 
          ds3g_siolib_hsuart_cd_changed_cb;
        ioctl_param.enable_cd_ready_event_ext.cb_data = 
          (void*)SIO_PORT_DATA_MUX_3;
        break;

      case SIO_PORT_DATA_MUX_4:
        ioctl_param.enable_cd_ready_event_ext.cb_func = 
                                        ds3g_siolib_hsuart_cd_changed_cb;
        ioctl_param.enable_cd_ready_event_ext.cb_data = 
          (void*)SIO_PORT_DATA_MUX_4;
        break;
        
      default:
        DS_SIO_MSG0_ERROR("Invalid Port being opened in ds3g_siolib_hsuart_open");
        ASSERT(0);
        return(ret_val);
    }

    state->serial_info.port_id      = port_id;
    state->serial_info.serial_state =  DS3G_SIOLIB_AUTODETECT_SERIAL_STATE;

    sio_ioctl( state->serial_info.stream_id,
               SIO_IOCTL_ENABLE_CD_EVENT_EXT,
               &ioctl_param );

    /*-----------------------------------------------------------------------
      Initialize the states of MT->TE serial port control signals.
    -----------------------------------------------------------------------*/

    ds3g_siolib_set_port_inbound_flow ( open_port, (uint32)DS_FLOW_ALL_MASK,
                                        DS_FLOW_ENABLE );

  }
  return( ret_val );
}/* ds3g_siolib_hsuart_open() */

#endif

/*===========================================================================

FUNCTION DS3G_SIOLIB_IOCTL

DESCRIPTION
  This function allows control of open stream for the port on which data call
  is established.

DEPENDENCIES
  No action is taken if there is no port on which data call exists.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3g_siolib_ioctl
(
  sio_ioctl_cmd_type cmd,
  sio_ioctl_param_type *param
)
{
  ds3g_siolib_ex_ioctl(cmd, param, ds3g_siolib_modem_port);
} /* ds3g_siolib_ioctl() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_IS_BAUD_VALID

DESCRIPTION
  This routine checks if the specified baud rate is valid under the
  current feature set.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the baud rate is valid under the current feature set.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean ds3g_siolib_is_baud_valid
(
  sio_bitrate_type   check_baud              /* SIO baud rate */
)
{
  return(sio_is_baud_valid ( check_baud ));
} /* ds3g_siolib_is_baud_valid() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_CHANGE_MODE
DESCRIPTION
  This function changes the mode of the currently open serial port to the
  specified mode. After the mode change, received data bytes will be stored
  in the SIO RX watermark , and the rx_func will be called each time data is
  enqueued to the watermark.

  Depending on the new serial mode, callback functions are registered for
  serial events of interest, namely:
    Autodetect Mode - Packet detection callback
    Rawdata Mode    - Escape signal detection callback
    Packet Mode     - None

DEPENDENCIES
  No action is taken if there is no port on which a data call exists.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_change_mode
(
  sio_stream_mode_type  stream_mode,                    /* New stream mode */
  dsm_watermark_type    *rx_wm_ptr,       /* Watermark for rx data from SIO */
  dsm_watermark_type    *tx_wm_ptr,         /* Watermark for tx data to SIO */
  sio_rx_func_ptr_type  rx_func_ptr   /* Called each time data bytes rx'ed */
)
{
  return ds3g_siolib_ex_change_mode(stream_mode,
                                    rx_wm_ptr,
                                    tx_wm_ptr,
                                    rx_func_ptr,
                                    ds3g_siolib_modem_port);

} /* ds3g_siolib_change_mode() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_CD_ASSERT

DESCRIPTION
  Asserts the Carrier Detect (CD) signal on the specified port.

DEPENDENCIES
  No action is taken if the specified port is not currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_cd_assert
( 
  ds3g_siolib_port_e_type port_id
)
{
  sio_ioctl_param_type  ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  ds3g_siolib_state_info_type *state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (port_id == DS3G_SIOLIB_PORT_NONE) 
  {
        DS_SIO_MSG0_ERROR("siolib_modem_port = PORT_NONE");
        return ret_val;
  }

  state = &ds3g_siolib_state[port_id];

  if( (state->serial_info.stream_id != SIO_NO_STREAM_ID) && 
      (state->serial_info.close_in_progress == FALSE) )
  {
    sio_ioctl( state->serial_info.stream_id, SIO_IOCTL_CD_ASSERT, &ioctl_param );
    DATA_MSG1(MSG_LEGACY_HIGH,"CD Asserted on port %d", port_id);
    ret_val = DS3G_SIOLIB_OK;
  }
  else
  {
    DS_SIO_MSG1_ERROR ("Asserting CD w/bad stream id %d", state->serial_info.stream_id);
  }

  return( ret_val );

} /* ds3g_siolib_cd_assert() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_CD_DEASSERT

DESCRIPTION
  De-asserts the Carrier Detect (CD) signal for the specified port.

DEPENDENCIES
  No action is taken if the specified port is not currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_cd_deassert
( 
  ds3g_siolib_port_e_type port_id
)
{
  sio_ioctl_param_type  ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  ds3g_siolib_state_info_type *state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (port_id == DS3G_SIOLIB_PORT_NONE) 
  {
        DS_SIO_MSG0_ERROR("siolib_modem_port = PORT_NONE");
        return ret_val;
  }

  state = &ds3g_siolib_state[port_id];

  if( (state->serial_info.stream_id != SIO_NO_STREAM_ID) &&
      (state->serial_info.close_in_progress == FALSE) )
  {
    sio_ioctl( state->serial_info.stream_id, SIO_IOCTL_CD_DEASSERT, &ioctl_param );
    DATA_MSG1(MSG_LEGACY_HIGH,"CD Deassert on port %d",port_id);
    ret_val = DS3G_SIOLIB_OK;
  }
  else
  {
    DS_SIO_MSG1_ERROR ("De-Asserting CD w/bad stream id %d", state->serial_info.stream_id);
  }

  return( ret_val );

} /* ds3g_siolib_cd_deassert() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_RI_ASSERT

DESCRIPTION
  Asserts the Ring Indicator (RI) signal for the specified port.

DEPENDENCIES
  No action is taken if the specified port is not currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_ri_assert
( 
  ds3g_siolib_port_e_type port_id
)
{
  sio_ioctl_param_type  ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  ds3g_siolib_port_e_type port_count;
  ds3g_siolib_port_e_type port_init= DS3G_SIOLIB_PORT_NONE;
  ds3g_siolib_state_info_type *state = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
    Increment the enumerator to point to DS3G_SIOLIB_PORT_NONE+1
  ------------------------------------------------------------------------*/    
  port_init++;

  if (port_id == DS3G_SIOLIB_PORT_NONE) 
  {
        DS_SIO_MSG0_ERROR("siolib_modem_port = PORT_NONE");
        return ret_val;
  }

  if(port_id == DS3G_SIOLIB_DATA_PORT)
  {
    for (port_count = port_init; port_count < DS3G_SIOLIB_PORT_MAX;
         port_count++) 
    {
      state = &ds3g_siolib_state[port_count];
      if( (state->serial_info.stream_id != SIO_NO_STREAM_ID) &&
          (state->serial_info.close_in_progress == FALSE) &&
          (state->at_state != DS3G_SIOLIB_ONLINE_DATA_AT_STATE))
      {
        sio_ioctl( state->serial_info.stream_id, 
                   SIO_IOCTL_RI_ASSERT, &ioctl_param );
        state->serial_info.ri_asserted = TRUE;
        ret_val = DS3G_SIOLIB_OK;
      }
    }
  }
  else
  {
    state = &ds3g_siolib_state[port_id];
    if( (state->serial_info.stream_id != SIO_NO_STREAM_ID) &&
        (state->serial_info.close_in_progress == FALSE) )
    {
      sio_ioctl( state->serial_info.stream_id, SIO_IOCTL_RI_ASSERT, &ioctl_param );
      state->serial_info.ri_asserted = TRUE;
      ret_val = DS3G_SIOLIB_OK;
    }
  }

  return( ret_val );

} /* ds3g_siolib_ri_assert() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_RI_DEASSERT

DESCRIPTION
  De-asserts the Ring Indicator (RI) signal for the specified port.

DEPENDENCIES
  No action is taken if the specified port is not currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_ri_deassert
( 
  ds3g_siolib_port_e_type port_id
)
{
  sio_ioctl_param_type  ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  ds3g_siolib_port_e_type port_count;
  ds3g_siolib_port_e_type port_init = DS3G_SIOLIB_PORT_NONE;
  ds3g_siolib_state_info_type *state = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 /*-----------------------------------------------------------------------
   Increment the enumerator to point to DS3G_SIOLIB_PORT_NONE+1
 ------------------------------------------------------------------------*/     
  port_init++;

  if (port_id == DS3G_SIOLIB_PORT_NONE) 
  {
        DS_SIO_MSG0_ERROR("siolib_modem_port = PORT_NONE");
        return ret_val;
  }

  if(port_id == DS3G_SIOLIB_DATA_PORT)
  {
    for (port_count = port_init; port_count < DS3G_SIOLIB_PORT_MAX;
         port_count++) 
    {
      state = &ds3g_siolib_state[port_count];
      if( (state->serial_info.stream_id != SIO_NO_STREAM_ID) &&
          (state->serial_info.close_in_progress == FALSE) &&
          (state->at_state != DS3G_SIOLIB_ONLINE_DATA_AT_STATE))
      {
        sio_ioctl( state->serial_info.stream_id, 
                   SIO_IOCTL_RI_DEASSERT, &ioctl_param );
        state->serial_info.ri_asserted = FALSE;
        ret_val = DS3G_SIOLIB_OK;
      }
    }
  }
  else
  {
    state = &ds3g_siolib_state[port_id];
    if( (state->serial_info.stream_id != SIO_NO_STREAM_ID) &&
        (state->serial_info.close_in_progress == FALSE) )
    {
      sio_ioctl( state->serial_info.stream_id, SIO_IOCTL_RI_DEASSERT, 
                 &ioctl_param );
      state->serial_info.ri_asserted = FALSE;
      ret_val = DS3G_SIOLIB_OK;
    }
  }

  return( ret_val );

} /* ds3g_siolib_ri_deassert() */



/*===========================================================================

FUNCTION DS3G_SIOLIB_DSR_ASSERT

DESCRIPTION
  Asserts the Data Set Ready (DSR) signal on the specified port.

DEPENDENCIES
  No action is taken if the specified port is not currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_dsr_assert
( 
  ds3g_siolib_port_e_type port_id
)
{
  sio_ioctl_param_type  ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  ds3g_siolib_state_info_type *state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((port_id == DS3G_SIOLIB_PORT_NONE) || (port_id >= DS3G_SIOLIB_PORT_MAX))
  {
    DS_SIO_MSG0_ERROR("Invalid siolib_modem_port");
    return ret_val;
  }

  state = &ds3g_siolib_state[port_id];

  if( (state->serial_info.stream_id != SIO_NO_STREAM_ID) &&
      (state->serial_info.close_in_progress == FALSE) )
  {
    sio_ioctl( state->serial_info.stream_id, 
               SIO_IOCTL_DSR_ASSERT, &ioctl_param );
    ret_val = DS3G_SIOLIB_OK;
  }
  else
  {
    DS_SIO_MSG1_ERROR ("Asserting DSR w/bad stream id %d", state->serial_info.stream_id);
  }

  return( ret_val );

} /* ds3g_siolib_dsr_assert() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_DSR_DEASSERT

DESCRIPTION
  De-asserts the Data Set Ready (DSR) signal on the specified port.

DEPENDENCIES
  No action is taken if the specified port is not currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_dsr_deassert
( 
  ds3g_siolib_port_e_type port_id
)
{
  sio_ioctl_param_type  ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  ds3g_siolib_state_info_type *state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((port_id == DS3G_SIOLIB_PORT_NONE) || (port_id >= DS3G_SIOLIB_PORT_MAX))
  {
    DS_SIO_MSG0_ERROR("Invalid siolib_modem_port");
    return ret_val;
  }

  state = &ds3g_siolib_state[port_id];

  if( (state->serial_info.stream_id != SIO_NO_STREAM_ID) &&
      (state->serial_info.close_in_progress == FALSE) )
  {
    sio_ioctl( state->serial_info.stream_id, SIO_IOCTL_DSR_DEASSERT, 
               &ioctl_param );
    ret_val = DS3G_SIOLIB_OK;
  }
  else
  {
    DS_SIO_MSG1_ERROR ("De-Asserting DSR w/bad stream id %d", state->serial_info.stream_id);
  }

  return( ret_val );

} /* ds3g_siolib_dsr_deassert() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_IS_DTR_ASSERTED

DESCRIPTION
  Checks the state of the Data Terminal Ready (DTR) signal, and returns a
  value (by means of a passed-by-reference parameter) indicating whether or
  not DTR is asserted.

DEPENDENCIES
  No action is taken if there is no port on which a data call exists.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_is_dtr_asserted
(
  boolean  *dtr_status     /* Set to TRUE if DTR asserted, or FALSE if not */
)
{
  return ds3g_siolib_ex_is_dtr_asserted(dtr_status, ds3g_siolib_modem_port);
} /* ds3g_siolib_is_dtr_asserted() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_IS_RTS_ASSERTED

DESCRIPTION
  Checks the state of the Request To Send (RTS) signal, and returns a
  value (by means of a passed-by-reference parameter) indicating whether or
  not RTS is asserted.

DEPENDENCIES
  If there is no port on which data call exists, the rts_status value 
  has no meaning.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_is_rts_asserted
(
  boolean  *rts_status     /* Set to TRUE if RTS asserted, or FALSE if not */
)
{
  return ds3g_siolib_ex_is_rts_asserted(rts_status, ds3g_siolib_modem_port);
} /* ds3g_siolib_is_rts_asserted() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_SET_FLOW_CTL_METHOD

DESCRIPTION
  Configures the flow control method for the currently open serial port.

DEPENDENCIES
  No action is taken if there is no port currently active.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_set_flow_ctl_method
(
  ds3g_siolib_port_e_type  port_id,              /* SIOLIB port id */
  sio_flow_ctl_type  tx_flow,                    /* TX flow control method */
  sio_flow_ctl_type  rx_flow                     /* RX flow control method */
)
{
  sio_ioctl_param_type  ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  ds3g_siolib_state_info_type *state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (port_id == DS3G_SIOLIB_PORT_NONE) 
  {
        DS_SIO_MSG0_ERROR("siolib_modem_port = PORT_NONE");
        return ret_val;
  }

  state = &ds3g_siolib_state[port_id];

  if( (state->serial_info.stream_id != SIO_NO_STREAM_ID) &&
      (state->serial_info.close_in_progress == FALSE) )
  {
    ioctl_param.flow_ctl.tx_flow = tx_flow;
    ioctl_param.flow_ctl.rx_flow = rx_flow;
    sio_ioctl( state->serial_info.stream_id, 
               SIO_IOCTL_SET_FLOW_CTL, &ioctl_param );
    ret_val = DS3G_SIOLIB_OK;
  }
  else
  {
    DS_SIO_MSG1_ERROR ("Setting flow ctl w/bad stream id %d", state->serial_info.stream_id);
  }

  return( ret_val );

} /* ds3g_siolib_set_flow_ctl_method() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_SETUP_TX

DESCRIPTION
  This function queues data for transmission over the serial interface.
  Additionally, Autodetect Mode is re-armed, if specified by the caller.
  Typically, the re-arm feature is used only by ATCoP.

DEPENDENCIES
  No action is taken if there is no port on which a data call exists.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_setup_tx
(
  dsm_item_type  *tx_ptr,         /* DSM item containing data to transmit */
  boolean         rearm_autodetect       /* TRUE = Re-arm Autodetect Mode */
)
{
  return ds3g_siolib_ex_setup_tx(tx_ptr,rearm_autodetect,ds3g_siolib_modem_port);
} /* ds3g_siolib_setup_tx() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_CHANGE_BAUD_RATE

DESCRIPTION
  Sets the baud rate of the currently active serial port to the specified
  value.  Change may be scheduled or immediate based on caller.

DEPENDENCIES
  No action is taken if there is no port currently active.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_change_baud_rate
(
  sio_bitrate_type  baud_rate,             /* New baud rate                */
  boolean           chg_now                /* Flag for immediate change    */
)
{
  sio_ioctl_param_type  ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  ds3g_siolib_state_info_type *state;
  rdm_device_enum_type rdm_port;
  ds3g_siolib_port_e_type     ds3g_port = DS3G_SIOLIB_PORT_NONE;
  rdm_device_enum_type rdm_port_init = RDM_FIRST_DEV;

  uint32 rdm_multi_dev_mask = rdm_get_multi_dev(RDM_DATA_SRVC);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-----------------------------------------------------------------------
        We want rdm_port_init to start at RDM_FIRST_DEV + 1.  To satisfy lint,
        we increment it here instead of incrementing in the for loop below.
  -------------------------------------------------------------------------*/
  rdm_port_init ++;
  for(rdm_port = rdm_port_init;rdm_port < RDM_DEV_MAX;rdm_port++)
  { 
    if(rdm_multi_dev_mask & (1 << (int) rdm_port))
    {
      ds3g_port = ds3g_siolib_map_rdm_port(rdm_port);
      if( ds3g_port != DS3G_SIOLIB_PORT_NONE )
      {
        state = &ds3g_siolib_state[ds3g_port];

      if( (state->serial_info.stream_id != SIO_NO_STREAM_ID) &&
          (state->serial_info.close_in_progress == FALSE) )
      {
        ioctl_param.bitrate = baud_rate;
        sio_ioctl( state->serial_info.stream_id,
                   (chg_now) ? SIO_IOCTL_CHANGE_BAUD_NOW
                             : SIO_IOCTL_CHANGE_BAUD_RATE,
                   &ioctl_param );
        ret_val = DS3G_SIOLIB_OK;
      }
      else
      {
        DS_SIO_MSG1_ERROR ("Changing baud rate w/bad stream id %d", 
                  state->serial_info.stream_id);
      }
      }
    }
  }
  return( ret_val );
 
} /* ds3g_siolib_change_baud_rate() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_SET_CD_STATE

DESCRIPTION
  This function sets the state of the Carrier Detect (CD) signal, based on
  the following:
  1) &C setting
     &C0 - CD is always ON
     &C1 - CD is ON when the remote carrier is ON
     &C2 - CD is always ON except for 1sec 'wink' when a data call is
           disconnected
  2) Remote carrier state
  3) End-of-call winking (only if &C=2)
  4) SMS message entry
     CD is ON during SMS message entry (overrides &C setting)

  This function should be called when a serial port is opened, and whenever
  any event occurs that impacts the CD state.

DEPENDENCIES
  No action is taken if there is no port on which data call exists.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_set_cd_state
(
  ds3g_siolib_dcd_event_e_type  dcd_event      /* Event that occurred, if any */
)
{
  ds3g_siolib_port_e_type port_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Check to see which client requested change in CD state.
    if client is ATCOP then the current active port should be used.
    Otherwise modem port should be used.
  -------------------------------------------------------------------------*/
  if ( ds3g_siolib_client_type == DS3G_SIOLIB_CLIENT_ATCOP )
  {
    ds3g_siolib_client_type = DS3G_SIOLIB_CLIENT_NONE;         /* Reset */
    if(ds3g_siolib_active_port != DS3G_SIOLIB_PORT_NONE)
    {
      port_id = ds3g_siolib_active_port;
    }
    else
    {
      DS_SIO_MSG0_ERROR ("NULL active port while setting CD state");
      return DS3G_SIOLIB_NO_PORT_OPEN;
    }
  }
  else
  {
    port_id = ds3g_siolib_modem_port;
  }

  return ds3g_siolib_ex_set_cd_state(dcd_event,port_id);

} /* ds3g_siolib_set_cd_state() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_REARM_AUTODETECT

DESCRIPTION
  This function re-arms SIO Autodetect Mode. Typically, the re-arm
  feature is used only by ATCoP.

DEPENDENCIES
  No action is taken if the specified port is not open, or if the current
  serial mode is not Autodetect Mode.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_rearm_autodetect
( 
  ds3g_siolib_port_e_type port_id
)
{
  sio_ioctl_param_type  ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  ds3g_siolib_state_info_type *state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (port_id == DS3G_SIOLIB_PORT_NONE) 
  {
        DS_SIO_MSG0_ERROR("siolib_modem_port = PORT_NONE");
        return ret_val;
  }

  state = &ds3g_siolib_state[port_id];

  if( (state->serial_info.stream_id != SIO_NO_STREAM_ID) &&
      (state->serial_info.close_in_progress == FALSE) )
  {
    if( state->serial_info.serial_state == DS3G_SIOLIB_AUTODETECT_SERIAL_STATE )
    {
      sio_ioctl( state->serial_info.stream_id, SIO_IOCTL_ENABLE_AUTODETECT,
                 &ioctl_param );
    }
    else
    {
      DS_SIO_MSG0_HIGH( "Autodt rearmed in non-Autodt serial mode");
    }
    ret_val = DS3G_SIOLIB_OK;
  }
  else
  {
    DS_SIO_MSG1_ERROR ("Rearming autodetect w/bad stream id %d", state->serial_info.stream_id);
  }

  ds3g_siolib_service_ports();
  return( ret_val );

} /* ds3g_siolib_rearm_autodetect() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_FLUSH_TX

DESCRIPTION
  This function is used to schedule the SIO TX queue to be flushed when
  changed to and from online-command and online-data modes.

  The flush_func_ptr_cb parameter is used to pass a callback function to
  SIO that is called when the flush operation has completed.  The callback
  should be used to notify the task which requested the flush operation
  that the TX queue is now empty.

DEPENDENCIES
  No action is taken if there is no port on which data call exists.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_flush_tx
(
  void (*flush_func_ptr_cb)(void)
)
{
  return ds3g_siolib_ex_flush_tx(flush_func_ptr_cb, ds3g_siolib_modem_port);
} /* ds3g_siolib_flush_tx() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_HANDLE_1ST_SIO_RX_SIG

DESCRIPTION
  This function processes the DS_<PORT>_1ST_SIO_RX_SIG, which indicates that 
  data bytes (i.e. AT commands) have been received from SIO, in Command state 
  or Online Command state. An ATCoP function is called to process the received
  data bytes if the ATCOP engine is free. If not the port waiting mask is set
  for the port on which data has been received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void  ds3g_siolib_handle_1st_sio_rx_sig
( 
  ds3g_siolib_port_e_type port_id
)
{
  dsat_mode_enum_type  at_mode;                              /* ATCoP mode */
  ds3g_siolib_state_info_type *state =NULL;
  boolean ret_val = FALSE;       /* return value from dsat_process_sio_command */
  dsm_item_type *dsm_item_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(port_id == DS3G_SIOLIB_PORT_NONE)
  {
    DATA_ERR_FATAL("input port equal to DS3G_SIOLIB_PORT_NONE");
    return;
  }

  state = &ds3g_siolib_state[port_id];

#ifdef FEATURE_DSAT_MDM_FUSION
  if ( port_id == ds3g_siolib_proxy_port )
  {
    DS_SIO_MSG0_HIGH("Received response from 7x30");
    (void)dsat_process_proxy_response(&state->serial_info.rx_wm_ptr);
    return;
  }
  else if( port_id == DS3G_SIOLIB_HSUART3_PORT )
  {
    DS_SIO_MSG0_HIGH("Received URC from 7x30");
    (void)dsat_process_proxy_urc(&state->serial_info.rx_wm_ptr);
    return;
  }
#endif /* FEATURE_DSAT_MDM_FUSION */

  /*-------------------------------------------------------------------------
    Set the ATCoP mode based on the current AT command processing state.
  -------------------------------------------------------------------------*/
  if( state->at_state == DS3G_SIOLIB_COMMAND_AT_STATE )
  {
    at_mode = DSAT_CMD;
  }
  else if( state->at_state == DS3G_SIOLIB_ONLINE_CMD_AT_STATE )
  {
    at_mode = DSAT_ONLINE_CMD;
  }
  else
  {
    /*-----------------------------------------------------------------------
      We are either in Null state or Online Data state. The 1ST_SIO_RX signal
      should never be set in these states, so log an error and return without
      any further processing.
    -----------------------------------------------------------------------*/
    DS_SIO_MSG0_ERROR( "1ST_SIO_RX_SIG rx'ed in invalid state" );
    return;
  }

  /*-------------------------------------------------------------------------
    Invoke the ATCoP function to load the ATCoP DLL if it's not already loaded.
    Free the RX WM data if it fails to load.
  -------------------------------------------------------------------------*/
  if (DSAT_DL_STATUS_LOADED != DSAT_DL_GET_STATUS())
  {
    DSAT_DL_CMD_HANDLER(DSAT_DL_CMD_LOAD);

    if (DSAT_DL_STATUS_LOADED != DSAT_DL_GET_STATUS())
    {
      while (NULL != (dsm_item_ptr = dsm_dequeue(&state->serial_info.rx_wm_ptr)))
      {
        dsm_free_packet(&dsm_item_ptr);
      }

      return;
    }
  }

  /*-------------------------------------------------------------------------
    Invoke an ATCoP function to process the rx'ed data bytes (AT commands).
  -------------------------------------------------------------------------*/
  
  if(ds3g_siolib_active_port == port_id || 
     ds3g_siolib_active_port == DS3G_SIOLIB_PORT_NONE)
  {
    ds3g_siolib_active_port = port_id;
    DS_SIO_MSG0_MED("calling dsat_process_sio_command");

    ret_val = dsat_process_sio_command(at_mode, 
                             &state->serial_info.rx_wm_ptr);
    if(ret_val == TRUE)
    {
      DS_SIO_MSG0_HIGH("Received abort call from dstat_process_sio_command");
    }
  }
  else
  {
    ds3g_siolib_port_waiting_mask |= state->serial_info.rx_pending_bit;
    DS_SIO_MSG2_MED("Active_port:%d, Port in waiting: %d", ds3g_siolib_active_port,
            port_id);
  }
  
} /* ds3g_siolib_handle_1st_sio_rx_sig() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_HANDLE_SIO_CLOSE_COMPLETE_CMD

DESCRIPTION
  This function processes the DS_RDM_CLSOE_COMPLETE_CMD, which indicates that
  sio_close has completed successfully and its now safe to notify RDM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void  ds3g_siolib_handle_sio_close_complete_cmd( void )
{
  ds3g_siolib_state_info_type *state;
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ds3g_siolib_pending_close_port == DS3G_SIOLIB_PORT_NONE)
  {
    DATA_ERR_FATAL("pending_close_port==DS3G_SIOLIB_PORT_NONE");
    return;
  }

  state = &ds3g_siolib_state[ds3g_siolib_pending_close_port];

  state->serial_info.stream_id = SIO_NO_STREAM_ID;
  state->serial_info.port_id   = SIO_PORT_NULL;

  /*-----------------------------------------------------------------------
    Set the serial and ATCoP states to Null, and discard any data that is
    sitting in the Autodetect watermark.
  -----------------------------------------------------------------------*/
  state->serial_info.serial_state = DS3G_SIOLIB_NULL_SERIAL_STATE;
  state->at_state = DS3G_SIOLIB_NULL_AT_STATE;

  ds3g_siolib_cleanup_autodetect_rx_watermark();

  /*-----------------------------------------------------------------------
    Clear SIOLIB callback function table.
  -----------------------------------------------------------------------*/

  if (ds3g_siolib_port_is_in_active_call(ds3g_siolib_pending_close_port))
  {
    ret_val = ds3g_siolib_ex_deregister_callback_func(state->func_tbl,
                                                      ds3g_siolib_pending_close_port);
    if(ret_val == FALSE)
    {
      DS_SIO_MSG0_ERROR("Invalid deregister call back function");
    }
  }
#if defined(FEATURE_DSAT_MDM_FUSION) || defined(FEATURE_DSAT_MSM_FUSION)
  if (!(ds3g_siolib_pending_close_port == DS3G_SIOLIB_HSUART1_PORT ||
       ds3g_siolib_pending_close_port == DS3G_SIOLIB_HSUART2_PORT ||
       ds3g_siolib_pending_close_port == DS3G_SIOLIB_HSUART3_PORT))
#endif
  {
    rdm_notify( RDM_DATA_SRVC, RDM_DONE_S );
#ifdef FEATURE_DATA_MUX
    if(ds3g_siolib_pending_close_port == (ds3g_siolib_port_e_type)dsat_get_cmux_port() )
    {
      /* Call the handler to start initializing MUX module */
      dsat_cmux_handler();
    }
#endif /* FEATURE_DATA_MUX */
  }


  ds3g_siolib_pending_close_port = DS3G_SIOLIB_PORT_NONE;
  state->serial_info.close_in_progress = FALSE;

} /* ds3g_siolib_handle_sio_close_complete_cmd() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_PERFORM_ONLINE_CMD_SWITCH

DESCRIPTION
  This function is used to complete the transition of an SIO/ATCOP mode
  change from online data to online command mode.  It is responsible for
  switching the SIO watermarks to those specified by the mode specific
  handler when a data call is established.

  This function is called directly from ds3g_siolib_process_enter_online_cmd_mode()
  when a DS3G_SIOLIB_ENTER_ONLINE_CMD_NO_TX_FLUSH command is received and no SIO TX
  flushing is required.  Since there is no flushing required in this case,
  the watermarks can be switched immediately.

  It is also called when a DS3G_SIOLIB_COMPLETE_ONLINE_CMD_SWITCH_CMD is received.
  This command is sent to SIOLIB in response to receiving a
  DS3G_SIOLIB_ENTER_ONLINE_CMD_TX_FLUSH command which requires the SIO TX queue to
  be flushed.  In this situation, the watermarks can NOT be switched until
  the TX queue has been emptied otherwise data may be lost.  When the queue
  is emptied, SIO will invoke a registered callback function that will send
  the DS3G_SIOLIB_COMPLETE_ONLINE_CMD_SWITCH_CMD to SIOLIB indicating that it 
  is now okay to switch the watermarks for online-command processing.

DEPENDENCIES
  Refer to the following companion functions for additional details:

    ds3g_enter_online_cmd_mode()
    ds3g_siolib_process_enter_online_cmd_mode()
    ds3g_siolib_signal_complete_online_cmd_switch_cb()

RETURN VALUE
  None

SIDE EFFECTS
  None.

=========================================================================*/
static void ds3g_siolib_perform_online_cmd_switch(ds3g_siolib_port_e_type port_id)
{
  ds3g_siolib_state_info_type *state = NULL;
  ds3g_siolib_status_e_type ret_val;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (port_id <= DS3G_SIOLIB_PORT_NONE ||
      port_id >= DS3G_SIOLIB_PORT_MAX )
  {
    DATA_ERR_FATAL("siolib_modem_port==SIOLIB_PORT_NONE");
    return;
  }

  state = &ds3g_siolib_state[port_id];
  ret_val = ds3g_siolib_ex_change_mode( SIO_DS_AUTODETECT_MODE, 
                           NULL,
                           NULL,
                           NULL,
                           port_id );
  if(ret_val == DS3G_SIOLIB_NO_PORT_OPEN)
  {
    DS_SIO_MSG0_HIGH("port mode change did not occur");
  }
  

  state->serial_info.serial_state =DS3G_SIOLIB_AUTODETECT_SERIAL_STATE;
  state->at_state = DS3G_SIOLIB_ONLINE_CMD_AT_STATE;
  
  DS_SIO_MSG0_HIGH( "Completed ONLINE_CMD mode switch" );
} /* ds3g_siolib_perform_online_cmd_switch() */



/*===========================================================================

FUNCTION DS3G_SIOLIB_HANDLE_SIO_ESCAPE_SIG

DESCRIPTION
  This function is to handle AT escape processing (+++ detection) when
  SIOLIB receives a DS3G_SIOLIB_ESCAPE_SIG signal.  It calls the callback
  function that was registered previously by one of the SIOLIB clients
  like the packet controller, async. controller and so on.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void ds3g_siolib_handle_sio_escape_sig ( void )
{
  ds3g_siolib_func_tbl_type *hdlr_tbl_ptr;
  ds3g_siolib_state_info_type *state;
  ds3g_siolib_port_e_type port_id = DS3G_SIOLIB_PORT_NONE;
  ds_call_type_e_type call_type = ds3g_siolib_ex_get_current_call_type();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(call_type == DS_CALL_TYPE_CKT)
  {
    port_id = ds3g_siolib_ex_get_csd_port();
  }
  else if(call_type == DS_CALL_TYPE_PKT)
  {
    port_id = ds3g_siolib_ex_get_psd_port();
  }
  else
  {
    DS_SIO_MSG0_ERROR("Invalid call type");
    return;
  }

  if (port_id <= DS3G_SIOLIB_PORT_NONE ||
      port_id >= DS3G_SIOLIB_PORT_MAX )
  {
    DATA_ERR_FATAL("siolib_modem_port == DS3G_SIOLIB_PORT_NONE");
    return;
  }

  state = &ds3g_siolib_state[port_id];

  /*-----------------------------------------------------------------------
  Point to the appropriate handler function table based on the current
  mode and call type.
  -----------------------------------------------------------------------*/
  hdlr_tbl_ptr = state->func_tbl;

  /*-----------------------------------------------------------------------
   Ensure that a handler function table is registered, with a non-null
   escape_sequence_handler.
  -----------------------------------------------------------------------*/
  if( hdlr_tbl_ptr != NULL )
  {
    if( hdlr_tbl_ptr->at_escape_sequence_handler != NULL )
    {
      if (hdlr_tbl_ptr->at_escape_sequence_handler() == DSAT_ONLINE_CMD)
      {
        DS_SIO_MSG3_HIGH( "detected '+++' - switching to online-command mode",0,0,0);

        ds3g_siolib_perform_online_cmd_switch(port_id);

        dsat_send_result_code_ex(DSAT_OK,port_id);

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
        /*------------------------------------------------------------------
        Flush any pending SMS indications on entry into online command
        mode after detecting an escape sequence.
        ------------------------------------------------------------------*/
        dsat_flush_sms_indications();
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */
      }
      else
      {
        DS_SIO_MSG0_HIGH( "Mode handler rejected '+++' escape sequence" );
      }
    } /* if esc_seq_hdlr != NULL */
    else
    {
      DS_SIO_MSG0_HIGH( "at_escape_sequence_handler() not registered" );
      dsat_send_result_code ( DSAT_ERROR );
    }
  } /* if hdlr_tbl_ptr != NULL */
  else
  {
    DS_SIO_MSG0_HIGH( "hdlr_tbl_ptr = NULL in siolib table" );
  }
} /* ds3g_siolib_handle_sio_escape_sig() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_SIGNAL_COMPLETE_ONLINE_CMD_SWITCH_CB

DESCRIPTION
  This is a callback function that is registered with SIO by SIOLIB when
  the SIO TX fifo is scheduled to be flushed during a mode change to online
  command mode.  It is invoked by SIO and is used to notify SIOLIB when
  flushing of the TX fifo has completed.

  The function sends a DS_COMPLETE_ONLINE_CMD_SWITCH_CMD command to SIOLIB.
  This command is used to by SIOLIB to indicate when it is ok
  to complete the transition from online data to online command mode.

DEPENDENCIES
  Used only during a SIO/ATCOP mode change from online data to online
  command mode.

  Refer to the following companion functions for additional details:

    ds3g_enter_online_cmd_mode()
    ds3g_siolib_process_enter_online_cmd_mode()
    ds3g_siolib_perform_online_cmd_switch()

RETURN VALUE
  None

SIDE EFFECTS
  SIOLIB will be notified that SIO TX flushing has completed.

===========================================================================*/

static void ds3g_siolib_signal_complete_online_cmd_switch_cb(void)
{
  ds_cmd_type *cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Allocate the command buffer and queue the command to DS */
  if ( (cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) != NULL )
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_COMPLETE_ONLINE_CMD_SWITCH;

    /* Notify the DS Task */
    ds_put_cmd(cmd_ptr);
  }
  else
  {
    ASSERT(0);
    return;
  }
} /* ds3g_siolib_signal_complete_online_cmd_switch_cb() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_SIGNAL_COMPLETE_ONLINE_DATA_SWITCH_CB

DESCRIPTION
  This is a callback function that is registered with SIO by SIOLIB when
  the SIO TX fifo is scheduled to be flushed during a mode change to online
  data mode.  It is invoked by SIO and is used to notify SIOLIB when
  flushing of the TX fifo has completed.

  The function sends a DS_COMPLETE_ONLINE_DATA_SWITCH_CMD command to 3G
  SIOLIB.  This command is used to by SIOLIB to indicate when it is ok
  to complete the transition from online command to online data mode.

DEPENDENCIES
  Used only during a SIO/ATCOP mode change from online command to online
  data mode.

  Refer to the following companion functions for additional details:

    ds3g_enter_online_data_mode()
    ds3g_siolib_process_enter_online_data_mode()

RETURN VALUE
  None

SIDE EFFECTS
  SIOLIB will be notified that SIO TX flushing has completed.

===========================================================================*/
static void ds3g_siolib_signal_complete_online_data_switch_cb(void)
{
  ds_cmd_type *cmd_ptr;
  ds3g_siolib_state_info_type *state;
  ds3g_siolib_port_e_type port_id;
  ds_call_type_e_type call_type = ds3g_siolib_ex_get_current_call_type();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Allocate the command buffer and queue the command to DS */
  if ( (cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) != NULL )
  {
    /* We are here as flush has been already done, so setting
       no wait for flush */
    if(DS_CALL_TYPE_CKT == call_type)
    {
      port_id = ds3g_siolib_ex_get_csd_port();
    }
    else if(DS_CALL_TYPE_PKT == call_type)
    {
      port_id = ds3g_siolib_ex_get_psd_port();
    }
    else
    {
      DS_SIO_MSG0_ERROR("Invalid call type");
      return;
    }

    state = ds3g_siolib_get_ds3g_siolib_state(port_id);
    if(NULL == state)
    {
      DS_SIO_MSG0_ERROR("sio state NULL");
      ASSERT(0);
      return;
    }

    state->serial_info.serial_state = DS3G_SIOLIB_NO_WAIT_TX_FLUSH_SERIAL_STATE;

    cmd_ptr->hdr.cmd_id = DS_CMD_COMPLETE_ONLINE_DATA_SWITCH;

    /* Notify the DS Task */
    ds_put_cmd(cmd_ptr);
  }
  else
  {
    ASSERT(0);
    return;
  }
} /* ds3g_siolib_signal_complete_online_data_switch_cb() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_PROCESS_ENTER_ONLINE_DATA_MODE

DESCRIPTION
  This function is used to process either a DS_ENTER_ONLINE_DATA_TX_FLUSH_CMD
  or DS_ENTER_ONLINE_DATA_NO_TX_FLUSH_CMD received from an external task
  request to switch the SIO/ATCOP processing mode to online data mode.

  The flush_tx parameter is used to indicate whether the SIO TX fifo should
  be flushed prior to switching to online data mode as follows:

    If flush_tx == TRUE, the SIO TX fifo will be flushed.
    If flush_tx == FALSE, the SIO TX fifo will not be flushed, any data
      currently in the queue will be lost.

  If TX flushing is required, the 
  ds3g_siolib_signal_complete_online_data_switch_cb()
  function will be registered with SIO to be invoked when TX flushing has
  completed.  The callback function is used to notify SIOLIB that it can
  now complete the mode change to online data mode.

  If no TX flushing is required, the mode change will occur immediately.

  The mode specific handler is invoked when the actual switch to online
  data mode has completed.  The handler should be used to notify the
  requesting task that online data mode processing is now active.

DEPENDENCIES
  A data call must currently be active.  The request must contain a valid
  call identifier.  Must currently be processing in online command mode.

  Refer to the following companion functions for additional details:

    ds3g_enter_online_data_mode()
    ds3g_siolib_signal_complete_online_data_switch_cb()

RETURN VALUE
  None

SIDE EFFECTS
  If the TX buffer is drained, SIO will invoke the specified callback
  function to signal SIOLIB when the fifo has been drained.

===========================================================================*/

static void  ds3g_siolib_process_enter_online_data_mode
(
  boolean       flush_tx
)
{
  ds3g_siolib_func_tbl_type  *hdlr_tbl_ptr;    /* Ptr to mode-sp. hdlr table */
  dsm_watermark_type         *from_sio_wm_ptr;
  dsm_watermark_type         *to_sio_wm_ptr;
  sio_rx_func_ptr_type        sio_rx_func_ptr;
  ds3g_siolib_state_info_type *state;
  ds3g_siolib_status_e_type ret_val; 
  ds3g_siolib_port_e_type port_id;
  ds_call_type_e_type call_type = ds3g_siolib_ex_get_current_call_type();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(call_type == DS_CALL_TYPE_CKT)
  {
    port_id = ds3g_siolib_ex_get_csd_port();
  }
  else if(call_type == DS_CALL_TYPE_PKT)
  {
    port_id = ds3g_siolib_ex_get_psd_port();
  }
  else
  {
    DS_SIO_MSG1_ERROR("invalid call_type %d", call_type);
    ASSERT(0);
    return;
  }
  
  if (port_id <= DS3G_SIOLIB_PORT_NONE ||
      port_id >= DS3G_SIOLIB_PORT_MAX )
  {
    DATA_ERR_FATAL("siolib_modem_port== DS3G_SIOLIB_PORT_NONE");
    return;
  }

  state = &ds3g_siolib_state[port_id];

  DS_SIO_MSG1_HIGH( "ONLINE_DATA mode switch, flush_tx=%d", flush_tx);

  /* Don't allow multiple TX flush requests */
  if ( state->serial_info.serial_state ==
       DS3G_SIOLIB_WAIT_TX_FLUSH_SERIAL_STATE )
  {
    DS_SIO_MSG3_HIGH("Switch to ONLINE_DATA mode rejected, TX flush already in progress",
              0,0,0);
    return;
  }

  if (state->at_state == DS3G_SIOLIB_ONLINE_CMD_AT_STATE)
  {
    /*-----------------------------------------------------------------------
      Check to see if the TX buffer needs to be flushed.  If so we schedule
      the flush with SIO.  When the buffer is emptied, SIO will call the
      registered callback function which will complete the transition to
      online command mode.
    -----------------------------------------------------------------------*/
    if (flush_tx)
    {
      DS_SIO_MSG0_HIGH( "Scheduling TX buffer flush");

      state->serial_info.serial_state = DS3G_SIOLIB_WAIT_TX_FLUSH_SERIAL_STATE;

      ret_val = ds3g_siolib_ex_flush_tx(ds3g_siolib_signal_complete_online_data_switch_cb, port_id);
      if (ret_val != DS3G_SIOLIB_OK)
      {
        DS_SIO_MSG1_ERROR ("Return value not DS3G_SIOLIB_OK returned %d", (int)(ret_val));  
      }
    }
    else
    {
      /*-----------------------------------------------------------------------
        Point to the appropriate handler function table based on the current
        mode and call type.
      -----------------------------------------------------------------------*/
      hdlr_tbl_ptr = state->func_tbl;

      /*-----------------------------------------------------------------------
        Ensure that a handler function table is registered, with a non-null
        call_ended_handler.
      -----------------------------------------------------------------------*/
      if ( hdlr_tbl_ptr != NULL )
      {
        if ( hdlr_tbl_ptr->at_return_to_online_data_handler != NULL )
        {
          if ( hdlr_tbl_ptr->at_return_to_online_data_complete_handler != NULL )
          {
            /*-----------------------------------------------------------
              Query the watermarks and SIO RX callback function from
              protocol stack. These parameters are needed for switching
              SIO between online modes.
            -----------------------------------------------------------*/
            hdlr_tbl_ptr->at_return_to_online_data_handler(
                            &from_sio_wm_ptr,
                            &to_sio_wm_ptr,
                            &sio_rx_func_ptr);

            /*-----------------------------------------------------------
              Make the switch to online-data mode.
            -----------------------------------------------------------*/
            if(call_type == DS_CALL_TYPE_CKT)
            {
              ret_val = ds3g_siolib_ex_change_mode( SIO_DS_RAWDATA_MODE, 
                                     from_sio_wm_ptr,
                                     to_sio_wm_ptr,
                                     sio_rx_func_ptr,
                                     port_id );
              state->serial_info.serial_state = DS3G_SIOLIB_RAWDATA_SERIAL_STATE;
            }
            else
            {
              //For DS_CALL_TYPE_PKT
              ret_val = ds3g_siolib_ex_change_mode( SIO_DS_PKT_MODE, 
                                     from_sio_wm_ptr,
                                     to_sio_wm_ptr,
                                     NULL,
                                     port_id );
              state->serial_info.serial_state = DS3G_SIOLIB_PACKET_SERIAL_STATE;
            }
            if(ret_val == DS3G_SIOLIB_NO_PORT_OPEN)
            {
              DS_SIO_MSG0_HIGH("port mode change did not occur");
            }

            state->at_state = DS3G_SIOLIB_ONLINE_DATA_AT_STATE;

            DS_SIO_MSG0_HIGH( "Completed ONLINE_DATA mode switch" );

            /*-----------------------------------------------------------
              Finish the switch to online data mode by performing
              protocol stack specific operations. The mode-specific
              handler should resume data transfer over the air.
            -----------------------------------------------------------*/
            hdlr_tbl_ptr->at_return_to_online_data_complete_handler();

          } /* if at_return_to_online_data_complete_handler != NULL */
          else
          {
            DS_SIO_MSG0_ERROR( "Complete mode handler not registered" );
          }
        } /* if at_return_to_online_data_handler != NULL */
        else
        {
          DS_SIO_MSG0_ERROR( "Mode handler not registered" );
        }
      } /* if hdlr_tbl_ptr != NULL */
    } /* if (flush_tx)*/
  } /* if ( (state->at_state... */
  else
  {
    if ( state->call_coming_up == FALSE )
    {
      DS_SIO_MSG1_ERROR( "Switch to ONLINE_CMD mode failed",
                state->call_coming_up);
    }
    else
    {
      DS_SIO_MSG1_ERROR( "Switch to ONLINE_CMD mode failed,invalid at_state=%d",
                state->at_state);
    }
  }
} /* ds3gi_process_enter_online_data_mode() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_PROCESS_ENTER_ONLINE_CMD_MODE

DESCRIPTION
  This function is used to process either a DS_ENTER_ONLINE_CMD_TX_FLUSH_CMD
  or DS_ENTER_ONLINE_CMD_NO_TX_FLUSH_CMD received from an external task
  request to switch the SIO/ATCOP processing mode to online command mode.

  The flush_tx parameter is used to indicate whether the SIO TX fifo should
  be flushed prior to switching to online command mode as follows:

    If flush_tx == TRUE, the SIO TX fifo will be flushed.
    If flush_tx == FALSE, the SIO TX fifo will not be flushed, any data
      currently in the queue will be lost.

  If TX flushing is required, the 
  ds3g_siolib_signal_complete_online_cmd_switch_cb()
  function will be registered with SIO to be invoked when TX flushing has
  completed.  The callback function is used to notify SIOLIB that it can
  now complete the mode change to online comand mode.

  If no TX flushing is required, the mode change will occur immediately.

  The mode specific handler is invoked when the actual switch to online
  command mode has completed.  The handler should be used to notify the
  requesting task that online command mode processing is now active.

DEPENDENCIES
  A data call must currently be active.  The request must contain a valid
  call identifier.  Must currently be processing in online data mode.

  Refer to the following companion functions for additional details:

    ds3g_enter_online_cmd_mode()
    ds3g_siolib_signal_complete_online_cmd_switch_cb()
    ds3g_siolib_perform_online_cmd_switch()

RETURN VALUE
  None

SIDE EFFECTS
  If the TX buffer is drained, SIO will invoke the specified callback
  function to signal SIOLIB when the fifo has been drained.

===========================================================================*/

static void  ds3g_siolib_process_enter_online_cmd_mode
(
  boolean       flush_tx
)
{
  ds3g_siolib_func_tbl_type  *hdlr_tbl_ptr;    /* Ptr to mode-sp. hdlr table */
  ds3g_siolib_state_info_type *state = NULL;
  ds3g_siolib_status_e_type ret_val;
  ds3g_siolib_port_e_type port_id;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  port_id = ds3g_siolib_ex_get_csd_port();

  if (port_id <= DS3G_SIOLIB_PORT_NONE ||
      port_id >= DS3G_SIOLIB_PORT_MAX )
  {
    DATA_ERR_FATAL("port_id IS INVALID");
    return;
  }

  state = &ds3g_siolib_state[port_id];

  DS_SIO_MSG1_MED( "Processing ONLINE_CMD mode switch, flush_tx=%d", flush_tx);

  if ( state->serial_info.serial_state == 
           DS3G_SIOLIB_WAIT_TX_FLUSH_SERIAL_STATE )
  {
    DS_SIO_MSG0_HIGH( "Switch to ONLINE_CMD mode rejected, TX flush already "
                      "in progress");
    return;
  }

  if ( state->at_state == DS3G_SIOLIB_ONLINE_DATA_AT_STATE )
  {
    /*-----------------------------------------------------------------------
      Check to see if the TX buffer needs to be flushed.  If so we schedule
      the flush with SIO.  When the buffer is emptied, SIO will call the
      registered callback function which will complete the transition to
      online command mode.
    -----------------------------------------------------------------------*/
    if (flush_tx)
    {
      DS_SIO_MSG0_HIGH( "Scheduling TX buffer flush");
      state->serial_info.serial_state = DS3G_SIOLIB_WAIT_TX_FLUSH_SERIAL_STATE;

      ret_val = ds3g_siolib_ex_flush_tx(ds3g_siolib_signal_complete_online_cmd_switch_cb,
                                        port_id);
      if (ret_val == DS3G_SIOLIB_NO_PORT_OPEN)
      {
        DS_SIO_MSG0_HIGH("siolib_flush_tx returned no port open");
      }
    }
    else
    {
      /*-----------------------------------------------------------------------
      Point to the appropriate handler function table based on the current
      mode and call type.
      -----------------------------------------------------------------------*/
      hdlr_tbl_ptr = state->func_tbl;

      /*-----------------------------------------------------------------------
      Ensure that a handler function table is registered, with a non-null
      call_ended_handler.
      -----------------------------------------------------------------------*/
      if ( hdlr_tbl_ptr != NULL )
      {
        if ( hdlr_tbl_ptr->return_to_online_cmd_mode_complete_handler != NULL )
        {
          /*-----------------------------------------------------------------
          Make the switch to online-command mode
          -----------------------------------------------------------------*/
          ds3g_siolib_perform_online_cmd_switch(port_id);

          /*-----------------------------------------------------------------
           Invoke the mode-specific handler function.
          -----------------------------------------------------------------*/
          hdlr_tbl_ptr->return_to_online_cmd_mode_complete_handler();
        }
        else
        {
          DS_SIO_MSG0_ERROR( "online_cmd_mode_complete mode handler not registered");
        }
      } /* if hdlr_tbl_ptr != NULL */
    } /* else (flush_tx) */
  } /* if (ds3g_siolib_state.at_state... */
  else
  {
    if ( state->call_coming_up == FALSE )
    {
      DS_SIO_MSG1_ERROR( "Switch to ONLINE_CMD mode failed", state->call_coming_up);
    }
    else
    {
      DS_SIO_MSG1_ERROR( "Switch to ONLINE_CMD mode failed,invalid at_state=%d",
                state->at_state);
    }
  }
} /* ds3g_siolib_process_enter_online_cmd_mode() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_PORT_IS_IN_ACTIVE_CALL

DESCRIPTION
  This function checks whether the given port is in an active Data call.

DEPENDENCIES
  None

RETURN VALUE
  TRUE, if port is already being used for a Data call
  FALSE, otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean ds3g_siolib_port_is_in_active_call
(
  ds3g_siolib_port_e_type port_id
)
{
  ds3g_siolib_portid_e_type port_index;

  for(port_index = DS3G_SIOLIB_PORTID_0; port_index < DS3G_SIOLIB_PORTID_MAX; port_index++)
  {
    if(ds3g_siolib_get_allocated_port_from_index(port_index) == port_id)
    {
      return TRUE;
    }
  }

  return FALSE;
}

/*===========================================================================

FUNCTION DS3G_SIOLIB_HANDLE_DTR_CHANGED_SIG

DESCRIPTION
  This function calls the callback function that was previously registered
  by one of the SIOLIB clients like the packet controller, async.
  controller and so on.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void ds3g_siolib_handle_dtr_changed_sig
(
  ds_cmd_type  *cmd_ptr
)
{
  boolean                        dtr_status = FALSE;
  ds3g_siolib_status_e_type      ret_val = DS3G_SIOLIB_OK;
  ds3g_siolib_func_tbl_type     *hdlr_tbl_ptr = NULL;
  ds3g_siolib_state_info_type   *state = NULL;
  ds3g_siolib_port_e_type        orig_modem_port_id = ds3g_siolib_modem_port;
  ds3g_siolib_dtr_chg_cmd_type  *dtr_chg_cmd_ptr = NULL;
  ds3g_siolib_port_e_type        port_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  if(cmd_ptr->hdr.cmd_id != DS_CMD_SIO_DTR_CHANGED)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "Invalid cmd: %d", cmd_ptr->hdr.cmd_id);
    return;
  }

  dtr_chg_cmd_ptr = (ds3g_siolib_dtr_chg_cmd_type*)cmd_ptr->cmd_payload_ptr;
  port_id = dtr_chg_cmd_ptr->port_id;

  if( port_id <= DS3G_SIOLIB_PORT_NONE || 
      port_id >= DS3G_SIOLIB_PORT_MAX )
  {
    DS_SIO_MSG1_ERROR( "Invalid Port ID value %d", port_id);
    return;
  }

  DS_SIO_MSG1_HIGH("DTR signal handler: using port %d", port_id);

  /* Clearing of DTR handling pending flag */
  ds3g_siolib_set_dtr_handling_pending_flag(FALSE, port_id);

  /* Check for unassigned modem port (such as for incoming call). */

  if(!ds3g_siolib_port_is_in_active_call(port_id))
  {
    DS_SIO_MSG0_HIGH("Data call is not active");

    /* If ports are being alerted, assign modem port to this one. */
    if( DS3G_SIOLIB_RING_TYPE_NONE != ds3g_siolib_current_ring_type)
    {
      ds3g_siolib_modem_port = port_id;
      DS_SIO_MSG1_HIGH("DTR signal handler: using port %d",
               ds3g_siolib_modem_port);

      /* Use mode-specific handler's answer info callback table if
       * specified port's callback table is empty. */
      if( (NULL == ds3g_siolib_state[port_id].func_tbl) &&
          (NULL != ds3g_siolib_answer_info.func_tbl_ptr) )
      {
        ds3g_siolib_state[port_id].func_tbl =
          ds3g_siolib_answer_info.func_tbl_ptr;
      }
    }
    else
    {
      /* Store the DTR status */
      ret_val = ds3g_siolib_ex_is_dtr_asserted(&dtr_status, port_id);
      if (ret_val == DS3G_SIOLIB_OK)
      {
        DS_SIO_MSG2_HIGH( "DTR Status %d, port %d", dtr_status, port_id );
        ds3g_siolib_state[port_id].is_dtr_high = dtr_status;
      }
      if(dtr_status == TRUE)
      {
         DS_SIO_MSG0_MED("CD,DSR Asserted as DTR High");
        (void)ds3g_siolib_init_cd_state( port_id );
         /* "DSR should be asserted only when &S is 0 at initialization. */
        if ( (dsat_num_item_type)dsatutil_get_val(
                                     DSAT_BASIC_N_S_IDX,0,0,NUM_TYPE) == DS3G_DSR_ALWAYS_ON )
        {
          (void)ds3g_siolib_dsr_assert(port_id);
          DS_SIO_MSG0_MED("AT&S = 0; DSR Asserted at DTR High");
        }
      }
      else
      {
         DS_SIO_MSG0_MED("CD,DSR DeAsserted as DTR Low");
         ds3g_siolib_cd_deassert(port_id);
        /* "DSR should be asserted only when &S is 0 at initialization. */
        if ( (dsat_num_item_type)dsatutil_get_val(
                                     DSAT_BASIC_N_S_IDX,0,0,NUM_TYPE) == DS3G_DSR_ALWAYS_ON )
        {
          (void)ds3g_siolib_dsr_deassert(port_id);
          DS_SIO_MSG0_MED("AT&S = 0; DSR DeAsserted at DTR Low");
        }
      }
#ifdef FEATURE_IRDA
      if (port_id == DS3G_SIOLIB_IRCOMM_PORT)
      {
        /* incase IrDA has just been disabled from the UI, the OBEX session is
           gone down so finish off by closing the IRCOMM port via RDM */
        irda_api_close_ircomm_port();
      }
#endif /* FEATURE_IRDA */
      return;
    }
  }
  
  state = &ds3g_siolib_state[port_id];
  hdlr_tbl_ptr = state->func_tbl;
  ret_val = ds3g_siolib_ex_is_dtr_asserted ( &dtr_status,port_id );

  DS_SIO_MSG1_HIGH("DTR status %d", dtr_status);

  /* Restore modem port incase it was changed above. */
  ds3g_siolib_modem_port = orig_modem_port_id;
  
  if ( hdlr_tbl_ptr == NULL ) 
  {
    if ( state->call_coming_up == TRUE )
    {
      DS_SIO_MSG0_ERROR( "ds3g_siolib_func_tbl() not registered" );
    }
    return;
  } /* if hdlr_tbl_ptr == NULL */

  if (ret_val == DS3G_SIOLIB_OK)
  {
    if (dtr_status == FALSE) 
    {
      if ((dsat_num_item_type)dsatutil_get_val(
                                       DSAT_BASIC_N_D_IDX,0,0,NUM_TYPE) == 0)
      {
        DS_SIO_MSG0_HIGH("DSAT_BASIC_N_D_IDX: Value 0");
        if(hdlr_tbl_ptr->dtr_changed_sig_ex_handler_ampd0 != NULL )
        {
          hdlr_tbl_ptr->dtr_changed_sig_ex_handler_ampd0 ( dtr_status, (void*)port_id);
        }

        else if ( hdlr_tbl_ptr->dtr_changed_sig_handler_ampd0 != NULL )
        {
          hdlr_tbl_ptr->dtr_changed_sig_handler_ampd0 ( dtr_status );
        }
        else
        {
          if ( state->call_coming_up == TRUE )
          {
            DS_SIO_MSG0_MED("AT&D=0 callback not registered");
          }
          return;
        }
      }
      else if (((dsat_num_item_type)dsatutil_get_val(
                                       DSAT_BASIC_N_D_IDX,0,0,NUM_TYPE) == 1) && 
               (state->at_state == DS3G_SIOLIB_ONLINE_DATA_AT_STATE))
      {
        DS_SIO_MSG0_HIGH("DSAT_BASIC_N_D_IDX: Value 1");

        if((hdlr_tbl_ptr->at_escape_sequence_ex_handler != NULL &&
            hdlr_tbl_ptr->at_escape_sequence_ex_handler((void*)port_id) == DSAT_ONLINE_CMD ) 
            ||
           (hdlr_tbl_ptr->at_escape_sequence_handler != NULL &&
           hdlr_tbl_ptr->at_escape_sequence_handler() == DSAT_ONLINE_CMD ))
          {
            /*---------------------------------------------------------------
            &D1 => DTR de-asserted means enter online command mode
            ---------------------------------------------------------------*/
            DS_SIO_MSG0_HIGH( "DTR de-asserted AT&D=1, enter online-command mode");
            ds3g_siolib_perform_online_cmd_switch(port_id);

            dsat_send_result_code_ex(DSAT_OK,port_id);

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
            /*---------------------------------------------------------------
            Flush any pending SMS indications on entry into online command
            mode after DTR is negated and &D=1.
            -----------------------------------------------------------------*/
            dsat_flush_sms_indications();
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */
          } /* if esc_seq_hdlr returns DSAT_ONLINE_CMD */ 

          else
          {
            if ( state->call_coming_up == TRUE )
            {
              DS_SIO_MSG3_MED ( "escape_seq_handler() not registered", 0, 0, 0 );
            }
            return;
          } /* if &D=1 */
      } /* if esc_seq_hdlr != NULL */

      else if ((dsat_num_item_type)dsatutil_get_val(
                                       DSAT_BASIC_N_D_IDX,0,0,NUM_TYPE) == 2)
      {
        DS_SIO_MSG0_HIGH("DSAT_BASIC_N_D_IDX: Value 2");
        if ( hdlr_tbl_ptr->dtr_changed_sig_ex_handler != NULL )
        {
          hdlr_tbl_ptr->dtr_changed_sig_ex_handler ( dtr_status,(void*)port_id );
        } /* if dtr_changed_sig_hdlr != NULL */
    
        else if ( hdlr_tbl_ptr->dtr_changed_sig_handler != NULL )
        {
          hdlr_tbl_ptr->dtr_changed_sig_handler ( dtr_status );
        } /* if dtr_changed_sig_hdlr != NULL */
        else
        {
          if ( state->call_coming_up == TRUE )
          {
            DS_SIO_MSG3_MED ( "dtr_changed_sig_handler() not registered", 0, 0, 0 );
          }
          return;
        } /* dtr_changed_sig_hdlr == NULL */
      } /* amp_d_val = 2 */
      else
      {
        DATA_MSG1(MSG_LEGACY_ERROR,
                  "Invalid AT&D value %d",
                  (dsat_num_item_type)dsatutil_get_val(DSAT_BASIC_N_D_IDX,
                                                       0,
                                                       0,
                                                       NUM_TYPE));
      }
    } /* dtr_status == FALSE */
  } /* Ret_Val != OK  */
  else
  {
    DS_SIO_MSG0_ERROR ( "NO PORT OPEN");
  } 
} /* ds3g_siolib_handle_dtr_changed_sig() */

#ifdef FEATURE_DSAT_MDM_FUSION
/*===========================================================================

FUNCTION DS3G_SIOLIB_HANDLE_CD_CHANGED_SIG

DESCRIPTION
  This function calls the callback function that was previously registered
  by one of the SIOLIB clients like the packet controller, async.
  controller and so on.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void ds3g_siolib_handle_cd_changed_sig
(
  ds_cmd_type  *cmd_ptr
)
{
  boolean                       cd_status = FALSE;
  ds3g_siolib_func_tbl_type    *hdlr_tbl_ptr = NULL;
  ds3g_siolib_state_info_type  *state = NULL;
  ds3g_siolib_status_e_type     ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  ds3g_siolib_cd_chg_cmd_type  *cd_chg_cmd_ptr = NULL;
  ds3g_siolib_port_e_type       port_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "NULL cmd ptr / payload ptr");
    return;
  }

  if(cmd_ptr->hdr.cmd_id != DS_CMD_SIO_CD_CHANGED)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Invalid cmd: %d", cmd_ptr->hdr.cmd_id);
    return;
  }

  cd_chg_cmd_ptr = (ds3g_siolib_cd_chg_cmd_type*)cmd_ptr->cmd_payload_ptr;
  port_id = cd_chg_cmd_ptr->port_id;

  if( (port_id == DS3G_SIOLIB_PORT_NONE) ||
      (port_id != ds3g_siolib_get_forwarding_port(ds3g_siolib_modem_port)) )
  {
    DS_SIO_MSG1_ERROR( "Invalid Port ID value %d", port_id );
    return;
  }

  state = &ds3g_siolib_state[port_id];
  hdlr_tbl_ptr = state->func_tbl;
  ret_val = ds3g_siolib_ex_is_cd_asserted ( &cd_status, port_id );

  if (ret_val == DS3G_SIOLIB_OK)
  {
    if (cd_status == FALSE) 
    {
      if ( hdlr_tbl_ptr == NULL ) 
      {
        DS_SIO_MSG1_ERROR("CD event handler function for port %d", (int)port_id);
        return;
      } /* if hdlr_tbl_ptr == NULL */
      
      if ( hdlr_tbl_ptr->cd_changed_sig_handler!= NULL )
      {
        hdlr_tbl_ptr->cd_changed_sig_handler ( cd_status );
      }
      else
      {
        DS_SIO_MSG1_ERROR("CD event handler function for port %d", (int)port_id);
        return;
      }
    } /*cd_status == FALSE */
  }
  else
  {
    DS_SIO_MSG0_ERROR("Port not open");
  }
} /* ds3g_siolib_handle_cd_changed_sig() */
#endif /* FEATURE_DSAT_MDM_FUSION */

/*===========================================================================

FUNCTION DS3G_SIOLIB_PROCESS_SIGNALS

DESCRIPTION
  This function processes SIOLIB signals. The appropriate processing is
  performed based on the signal(s) that are set in the signal mask.

  This function should be called by the top-level task dispatcher whenever
  any of the SIOLIB signals are set.

DEPENDENCIES
  SIOLIB should have been initialized prior to calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3g_siolib_process_signals
(
  rex_sigs_type    set_sigs                 /* Mask containing set signals */
)
{
  rex_sigs_type ret_val =0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    Perform the appropriate processing based on the signal(s) that are set.
  -------------------------------------------------------------------------*/
  if( (set_sigs & DS_DATA_PORT_1ST_SIO_RX_SIG) != 0 )
  {
    ds3g_siolib_port_e_type port_id;
    
    ret_val = rex_clr_sigs( dsi_get_ds_tcb(), DS_DATA_PORT_1ST_SIO_RX_SIG );
    if(!(ret_val & DS_DATA_PORT_1ST_SIO_RX_SIG))
    {
      DS_SIO_MSG0_MED("DS_DATA_PORT_1ST_SIO_RX_SIG was not active"); 
    }
    for ( port_id = DS3G_SIOLIB_UART1_PORT; 
              port_id<DS3G_SIOLIB_PORT_MAX; ++port_id )
    {
      if ( ds3g_siolib_state[port_id].serial_info.rx_sig == TRUE )
      {
        ds3g_siolib_state[port_id].serial_info.rx_sig = FALSE;
        ds3g_siolib_handle_1st_sio_rx_sig(port_id);
      }
    }
  }
  if( (set_sigs & DS_DTR_EVENT_SIG) != 0 )
  {
    DS_SIO_MSG0_MED( "Rx'ed DS_DTR_EVENT_SIG");
    ret_val = rex_clr_sigs( dsi_get_ds_tcb(), DS_DTR_EVENT_SIG );
    if(!(ret_val & DS_DTR_EVENT_SIG))
    {
      DS_SIO_MSG0_MED("DS_DTR_EVENT_SIG was not active");
    }
    ds3g_siolib_handle_dtr_event_sig();
  }
  if( (set_sigs & DS_SIO_RX_PKT_SIG) != 0 )
  {
    DS_SIO_MSG0_MED( "Rx'ed SIO_RX_PKT_SIG");
    ret_val = rex_clr_sigs( dsi_get_ds_tcb(), DS_SIO_RX_PKT_SIG );
    if(!(ret_val & DS_SIO_RX_PKT_SIG))
    {
      DS_SIO_MSG0_MED("DS_SIO_RX_PKT_SIG was not active");
    }
  }

  if( (set_sigs & DS_SIO_ESCAPE_SIG) != 0 )
  {
    DS_SIO_MSG0_MED( "Rx'ed SIO_ESCAPE_SIG" );
    ret_val = rex_clr_sigs( dsi_get_ds_tcb(), DS_SIO_ESCAPE_SIG );
    if(!(ret_val & DS_SIO_ESCAPE_SIG))
    {
      DS_SIO_MSG0_MED("DS_SIO_ESCAPE_SIG was not active");
    }
    ds3g_siolib_handle_sio_escape_sig();
  }
} /* ds3g_siolib_process_signals() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_PROCESS_CMDS

DESCRIPTION
  This function processes SIOLIB commands. A command processing function is
  called based on the type of command received.

  This function should be called by the top-level task dispatcher whenever
  any SIOLIB command is received.

DEPENDENCIES
  SIOLIB should have been initialized prior to calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3g_siolib_process_cmds
(
  ds_cmd_type  *cmd_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Invoke the appropriate function to process the command, based on the
    command id.
  -------------------------------------------------------------------------*/
  DS_SIO_MSG1_HIGH( "ds3g sio process cmd id [%d]", cmd_ptr->hdr.cmd_id );
  switch( cmd_ptr->hdr.cmd_id )
  {
    case DS_CMD_RDM_OPEN:
      ds3gi_handle_rdm_open_cmd( cmd_ptr );
      break;

    case DS_CMD_RDM_CLOSE:
      ds3gi_handle_rdm_close_cmd( cmd_ptr );
      break;

    case DS_CMD_SIO_CLOSE_COMPLETE:
      ds3g_siolib_handle_sio_close_complete_cmd(); 
      break;

    case DS_CMD_SIO_DTR_CHANGED:
      ds3g_siolib_handle_dtr_changed_sig( cmd_ptr );
      break;

#ifdef FEATURE_DSAT_MDM_FUSION
    case DS_CMD_SIO_CD_CHANGED:
      ds3g_siolib_handle_cd_changed_sig( cmd_ptr );
      break;
#endif

    case DS_CMD_SIO_DSM_MEM_CTRL:
      ds3g_siolib_mem_ctrl_event( cmd_ptr );
      break;

    case DS_CMD_ENTER_ONLINE_CMD_TX_FLUSH:
      ds3g_siolib_process_enter_online_cmd_mode( TRUE );
      break;

    case DS_CMD_COMPLETE_ONLINE_CMD_SWITCH:  /* Rx'd after a TX flush completes */
      ds3g_siolib_process_enter_online_cmd_mode( FALSE );
      break;

    case DS_CMD_ENTER_ONLINE_DATA_TX_FLUSH:
      ds3g_siolib_process_enter_online_data_mode( TRUE );
      break;

    case DS_CMD_ENTER_ONLINE_DATA_NO_TX_FLUSH:/* Enter online-cmd w/no TX flush */
    case DS_CMD_COMPLETE_ONLINE_DATA_SWITCH:  /* Rx'd after a TX flush completes */
      ds3g_siolib_process_enter_online_data_mode( FALSE );
      break;

    default:
      DS_SIO_MSG1_ERROR( "SIOLIB rx'ed unrecognized cmd: %d", cmd_ptr->hdr.cmd_id );
      break;

  } /* switch( cmd_id ) */

} /* ds3g_siolib_process_cmds() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_CLEANUP_AUTODETECT_RX_WATERMARK

DESCRIPTION
  This function cleans up the watermark used for RX data from SIO.
  Any data stored in the watermark is discarded.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3g_siolib_cleanup_autodetect_rx_watermark( void )
{
  dsm_item_type  *item_ptr;                         /* Pointer to dsm item */
  ds3g_siolib_state_info_type *state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ds3g_siolib_pending_close_port == DS3G_SIOLIB_PORT_NONE) 
  {
        DS_SIO_MSG0_ERROR("siolib_modem_port = PORT_NONE");
        return;
  }

  state = &ds3g_siolib_state[ds3g_siolib_pending_close_port];

  /*-------------------------------------------------------------------------
    De-queue dsm items and free the items (i.e. discard the data), until the
    watermark is empty.
  -------------------------------------------------------------------------*/
  while( (item_ptr = dsm_dequeue( &state->serial_info.rx_wm_ptr )) != NULL )
  {
    dsm_free_packet( &item_ptr );
  }

} /* ds3g_siolib_cleanup_autodetect_rx_watermark() */

/*===========================================================================

FUNCTION DS3GI_CONVERT_SERIAL_MODE

DESCRIPTION
  This function converts the specified SIO serial mode into the corresponding
  3G Dsmgr serial state.

DEPENDENCIES
  None

RETURN VALUE
  Returns the 3G Dsmgr serial state corresponding to the specified SIO serial
  mode.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_serial_state_e_type  ds3gi_convert_serial_mode
(
  sio_stream_mode_type  sio_serial_mode                 /* SIO serial mode */
)
{
  ds3g_siolib_serial_state_e_type  serial_state;        /* 3G Dsmgr serial state */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( sio_serial_mode == SIO_DS_AUTODETECT_MODE )
  {
    serial_state = DS3G_SIOLIB_AUTODETECT_SERIAL_STATE;
  }
  else if( sio_serial_mode == SIO_DS_RAWDATA_MODE )
  {
    serial_state = DS3G_SIOLIB_RAWDATA_SERIAL_STATE;
  }
  else if( sio_serial_mode == SIO_DS_PKT_MODE )
  {
    serial_state = DS3G_SIOLIB_PACKET_SERIAL_STATE;
  }
  else
  {
    DS_SIO_MSG0_ERROR( "Invalid serial mode, using Autodetect");
    serial_state = DS3G_SIOLIB_AUTODETECT_SERIAL_STATE;
  }

  return( serial_state );

} /* ds3gi_convert_serial_mode() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_ALERT_USER

DESCRIPTION
  This function rings the serial ports to alert the user/TE of an incoming
  data or voice call. The RI signal is toggled, and the RING result code is
  sent to the TE. Auto-answer is also performed, if requested by the caller
  (and if enabled by the S0 s-register).

  This function should be called to start alerting, and subsequently, each
  time the ring timer expires. This function may be invoked either by the
  mode specific handler (for data calls) or ATCoP (for voice calls).

  Note that the auto-answer facility is intended to be used for data calls
  only.

DEPENDENCIES
  This function does nothing if there is no serial port on a data call.

RETURN VALUE
  None

SIDE EFFECTS
  Updates 3G Dsmgr state variables (ring_counter).

===========================================================================*/
void  ds3g_siolib_alert_user
(
  dsat_result_enum_type  ring_type,      /* Ring result code to send to TE */
  boolean                auto_answer        /* TRUE => Perform auto-answer */
)
{
  boolean                      call_answered = FALSE;
  dword                        duration = 0;
  ds3g_siolib_port_e_type      port;
  ds3g_siolib_port_e_type      port_init = DS3G_SIOLIB_PORT_NONE;
  ds3g_siolib_state_info_type *state = NULL; 
  boolean                      sent_rc = FALSE;
  ds3g_siolib_status_e_type    ret_val;
  dsat_num_item_type           s0_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 /*-----------------------------------------------------------------------
   Increment the enumerator to point to DS3G_SIOLIB_PORT_NONE+1
 ------------------------------------------------------------------------*/     
  port_init++;

  /*---------------------------------------------------------------------
    Loop over all ports to send ring indication notification
  ---------------------------------------------------------------------*/
  for( port = port_init; 
       port < DS3G_SIOLIB_PORT_MAX;
       port++ ) 
  {         
    state = &ds3g_siolib_state[port];

    if( state->serial_info.serial_state != DS3G_SIOLIB_NULL_SERIAL_STATE )
    {
      state->ring_type = ring_type;
      if( state->serial_info.ri_asserted )
      {
        /*---------------------------------------------------------------------
          RI is asserted, so de-assert it for DS3GI_RI_DEASSERT_INTERVAL sec.
        ---------------------------------------------------------------------*/
        ret_val = ds3g_siolib_ri_deassert( port ); 
        if (ret_val == DS3G_SIOLIB_NO_PORT_OPEN)
        {
          DS_SIO_MSG0_ERROR("siolib_ri_deassert returned no port open");
        }
        duration = DS3G_SIOLIB_RI_DEASSERT_INTERVAL;

        /*---------------------------------------------------------------------
          Send the RING result code to the TE. The ring type is based on the
          type of call: voice, fax etc.
          Do this only once as lower routine broadcasts to all ports.
        ---------------------------------------------------------------------*/
        if( !sent_rc )
        {
          dsat_send_unsolicited_result_code(ring_type);
          sent_rc = TRUE;
        }

        /*---------------------------------------------------------------------
          If auto-answering is to be performed, then do so based on the value
          of S0. Note: It is assumed that auto-answer is only performed for
          data calls (and not for voice calls answered by the TE).
        ---------------------------------------------------------------------*/
        s0_val = (dsat_num_item_type)dsatutil_get_val(
                                         DSAT_SREG_S0_IDX,0,0,NUM_TYPE);
        
        if( auto_answer && (port == ds3g_siolib_answer_info.port) )
        {
          if( s0_val != 0 )
          {
            if ( ++state->ring_counter >= s0_val ) 
            {
              /* Use answer callback table if none specified yet. */
              if( state->func_tbl == NULL )
              {
                state->func_tbl = ds3g_siolib_answer_info.func_tbl_ptr;
              }
              
              if( (state->func_tbl != NULL) &&
                  (state->func_tbl->auto_answer_handler != NULL) )
              {
                state->func_tbl->auto_answer_handler();
                call_answered = TRUE;
              }
              else
              {
                DS_SIO_MSG0_ERROR ( "auto answer handler not registered" );
              }
            }
          } /* if S0 is non-zero */
        } /* if auto_answer */
      } /* if ri_asserted */
      else
      {
        /*---------------------------------------------------------------------
          RI is de-asserted, so assert it for DS3GI_RI_ASSERT_INTERVAL sec.
        ---------------------------------------------------------------------*/
        ret_val = ds3g_siolib_ri_assert( port );
        if (ret_val == DS3G_SIOLIB_NO_PORT_OPEN)
        {
          DS_SIO_MSG0_ERROR("siolib_ri_assert returned no port open");
        }
        duration = DS3G_SIOLIB_RI_ASSERT_INTERVAL;
      }

      if( call_answered )
      {
        /*---------------------------------------------------------------------
          Call was answered, so de-assert RI and reset the ring counter for the
          next call.
        ---------------------------------------------------------------------*/
        ret_val = ds3g_siolib_ri_deassert( port );
        if (ret_val == DS3G_SIOLIB_NO_PORT_OPEN)
        {
          DS_SIO_MSG0_ERROR("siolib_ri_deassert returned no port open");
        }
        state->ring_counter = 0;
      }
    } /* serial_state != DS3G_SIOLIB_NULL_SERIAL_STATE */
  } /* for */

  
  /*---------------------------------------------------------------------
    Call was not answered, re-start the ring timer.
  ---------------------------------------------------------------------*/
  if( !call_answered && duration )
  {
    ds3g_siolib_current_ring_type = ring_type;
    ds3gi_start_timer( DS3G_TIMER_RING_TE_1, duration );
  }
  
} /* ds3g_siolib_alert_user() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_STOP_ALERTING

DESCRIPTION
  This function stops alerting the user i.e. stops ringing the serial ports.
  The RI signal is de-asserted and the ring timer is stopped.

  This function should be called when the user answers the call or if the
  call is released in the RINGING call state.

DEPENDENCIES
  This function does nothing if there is no serial port on a data call.

RETURN VALUE
  None

SIDE EFFECTS
  Updates 3G Dsmgr state variables (resets the ring_counter to zero).

===========================================================================*/

void  ds3g_siolib_stop_alerting( void )
{
  ds3g_siolib_port_e_type      port;
  ds3g_siolib_port_e_type      port_init = DS3G_SIOLIB_PORT_NONE;
  ds3g_siolib_state_info_type *state = NULL;
  ds3g_siolib_status_e_type    ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 /*-----------------------------------------------------------------------
   Clear the timer ring type flag to indicate no alerting underway. 
 ------------------------------------------------------------------------*/     
 ds3g_siolib_current_ring_type = DS3G_SIOLIB_RING_TYPE_NONE;
  
 /*-----------------------------------------------------------------------
   Increment the enumerator to point to DS3G_SIOLIB_PORT_NONE+1
 ------------------------------------------------------------------------*/     
  port_init++;

  /*---------------------------------------------------------------------
    Loop over all ports to send ring indication notification
  ---------------------------------------------------------------------*/
  for( port = port_init; 
       port < DS3G_SIOLIB_PORT_MAX;
       port++ ) 
  {         

    state = &ds3g_siolib_state[port];

    if( state->serial_info.serial_state != DS3G_SIOLIB_NULL_SERIAL_STATE )
    {
      ret_val = ds3g_siolib_ri_deassert( port ); 
      if (ret_val == DS3G_SIOLIB_NO_PORT_OPEN)
      {
         DS_SIO_MSG0_ERROR("siolib_ri_deassert returned no port open");
      }
      state->ring_counter = 0;
    }
  }

  if( ds3gi_is_timer_running( DS3G_TIMER_RING_TE_1 ) )
  {
    ds3gi_stop_timer( DS3G_TIMER_RING_TE_1 );
  }
} /* ds3g_siolib_stop_alerting() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_SIO_CLOSE_CB

DESCRIPTION
  This function sends the SIO_CLOSE_COMPLETE_CMD to DS.

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void  ds3g_siolib_sio_close_cb( void )
{
  ds_cmd_type           *cmd_ptr;                    /* Pointer to command */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ( cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO) ) == NULL )
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_SIO_CLOSE_COMPLETE;
  ds_put_cmd( cmd_ptr );
  return;
} /* ds3g_siolib_sio_close_cb() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_ALLOCATE_PORT

DESCRIPTION
  This function attempts to allocate an SIO port for an external data
  call.  It implements a simple semaphore flag for the port resource
  to ensure only one data call per SIO port.  A port must be
  unallocated and in AUTODETECT state for it to be available.  On
  successful allocation, a valid port identifier is returned.
  Otherwise, the NONE identifier is returned.  It is the calling
  routines responsibility to honor the resource semaphore. 

  The current assumption is that only one port can host an external call
  at any given time.  This port is referred to as the modem port.

DEPENDENCIES
  None

RETURN VALUE
  DS3G_SIOLIB_PORTID_NONE on error condition, valid port identifier otherwise.

SIDE EFFECTS
  ds3g_siolib_modem_port is set if was NONE on entry and operation
  successful.

===========================================================================*/

ds3g_siolib_portid_e_type ds3g_siolib_allocate_port
(
  ds3g_siolib_port_alloc_info_s_type  *alloc_info_ptr
)
{
  ds3g_siolib_port_e_type port_id = 
    ds3g_siolib_ex_allocate_port(alloc_info_ptr,DS_CALL_TYPE_UNSPECIFIED);

  return ds3g_siolib_get_index_from_allocated_port(port_id);

} /* ds3g_siolib_allocate_port() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_RELEASE_PORT

DESCRIPTION
  This function releases a previously allocated SIO port, clearing the
  semaphore flag.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_siolib_release_port
(
  ds3g_siolib_portid_e_type port_index
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds3g_siolib_port_e_type port_id = 
    ds3g_siolib_get_allocated_port_from_index(port_index);

  ds3g_siolib_ex_release_port(port_id);

} /* ds3g_siolib_release_port() */

#if defined(FEATURE_BT) || defined(FEATURE_BT_MODEM)
/*===========================================================================

FUNCTION DS3G_SIOLIB_UPDATE_SIO_CONN_STATUS

DESCRIPTION
  Callback function handling SPP status change for BT.  Called by BT driver.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_siolib_update_sio_conn_status
(
  bt_spp_status_type* spp_status_ptr
)
{
  ds3g_siolib_state_info_type *state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(spp_status_ptr);
  state = &ds3g_siolib_state[DS3G_SIOLIB_BT_SPP_PORT];

  if(spp_status_ptr == NULL)
  {
    DS_SIO_MSG0_ERROR("Null Status ptr passed in ds3g_siolib_update_sio_conn_status");
    return;
  }

  if ( (spp_status_ptr->spp_state == BT_SPP_ST_OPENING) &&
       (state->serial_info.stream_id == SIO_NO_STREAM_ID)
     )
  {
    /* 
      If BT task has higher priority than DS task, this function would
      be called before sio_open() returns a valid stream ID.  We need to
      save the stream ID here.
    */
    state->serial_info.stream_id = spp_status_ptr->stream_id;
    ds3g_siolib_bt_spp_status = *spp_status_ptr;
  }
  else if ( spp_status_ptr->stream_id == state->serial_info.stream_id )
  {
    ds3g_siolib_bt_spp_status = *spp_status_ptr;
    switch ( spp_status_ptr->spp_state )
    {
      case BT_SPP_ST_CLOSED:
      case BT_SPP_ST_OPEN:
      case BT_SPP_ST_OPEN_ERROR:
        ds3g_siolib_release_task();
        break;
      default:
        break;
    }

    if( ds3g_siolib_bt_spp_state == BT_SPP_ST_CONNECTED &&
        (spp_status_ptr->spp_state == BT_SPP_ST_OPEN ||
         spp_status_ptr->spp_state == BT_SPP_ST_DISCONNECTING) )
      ds3g_siolib_bt_spp_dtr_changed_cb();           /* mimic DTR drop */
    ds3g_siolib_bt_spp_state = spp_status_ptr->spp_state;
  }
} /* ds3g_siolib_update_sio_conn_status() */

#endif /* FEATURE_BT || FEATURE_BT_MODEM */

/*===========================================================================

FUNCTION DS3G_SIOLIB_BLOCK_TASK

DESCRIPTION
  Suspends DS task until one of the relevant signals are set which call
  ds3g_siolib_release.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_siolib_block_task (void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) rex_wait (DS3G_SIOLIB_BLOCK_SIG);
  (void) rex_clr_sigs (dsi_get_ds_tcb(), DS3G_SIOLIB_BLOCK_SIG);
  return;
} /* ds3g_siolib_block_task() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_RELEASE_TASK

DESCRIPTION
  Unblocks the DS task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_siolib_release_task (void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) rex_set_sigs (dsi_get_ds_tcb(), DS3G_SIOLIB_BLOCK_SIG);
  return;
} /* ds3g_siolib_release_task() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_MAP_SIO_PORT

DESCRIPTION
  This port reads the SIO port passed to it and returns the corresponding
  DSMGR port (of type ds3g_siolib_port_e_type). 

DEPENDENCIES
  None
  
RETURN VALUE
  DSMGR port of type ds3g_siolib_port_e_type
  
SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_port_e_type ds3g_siolib_map_sio_port
(
  sio_port_id_type port_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(port_id) 
  {
    case SIO_PORT_UART_MAIN:
      return DS3G_SIOLIB_UART1_PORT;
#ifdef FEATURE_USB_CDC_ACM
    case SIO_PORT_USB_CDC_ACM:

#ifdef FEATURE_DATA_SDIO_MUX_A2_DUN
      if (ds3g_get_mdm_tethering_enabled()  == FALSE)
        return DS3G_SIOLIB_SDIO_MUX_A2_DUN_0_PORT;
      else if (ds3g_get_mdm_tethering_enabled()  == TRUE)
        return DS3G_SIOLIB_USB_CDC_PORT;
#elif defined (FEATURE_DATA_A2_MDM)
      if(ds3g_get_dun_acc_cfg() == 0x01)
      {
        return DS3G_SIOLIB_A2_MDM_PORT;
      }
      else
      {
        return DS3G_SIOLIB_USB_CDC_PORT;
      }
#else
      return DS3G_SIOLIB_USB_CDC_PORT;
#endif
#endif
#if defined(FEATURE_BT) || defined(FEATURE_BT_MODEM)
    case SIO_PORT_BT_NA:
      return DS3G_SIOLIB_BT_NA_PORT;
    case SIO_PORT_BT_SPP:
      return DS3G_SIOLIB_BT_SPP_PORT;
#endif /* FEATURE_BT || FEATURE_BT_MODEM */
#ifdef FEATURE_IRDA
    case SIO_PORT_IRCOMM:
      return DS3G_SIOLIB_IRCOMM_PORT;
#endif    
#if defined(FEATURE_MULTIPROCESSOR) && defined(FEATURE_SMD)
    case SIO_PORT_SMD_DS:
      return DS3G_SIOLIB_SMD_DS_PORT;
    case SIO_PORT_SMD_DATA1:
      return DS3G_SIOLIB_SMD_DATA1_PORT;
    case SIO_PORT_SMD_DATA2:
      return DS3G_SIOLIB_SMD_DATA2_PORT;
    case SIO_PORT_SMD_DATA3:
      return DS3G_SIOLIB_SMD_DATA3_PORT;
    case SIO_PORT_SMD_DATA4:
      return DS3G_SIOLIB_SMD_DATA4_PORT;
    case SIO_PORT_SMD_DATA11:
      return DS3G_SIOLIB_SMD_DATA11_PORT;
#endif /* if defined(FEATURE_MULTIPROCESSOR)... */
#if defined(FEATURE_DSAT_MDM_FUSION) || defined(FEATURE_DSAT_MSM_FUSION)
    case SIO_PORT_DATA_MUX_2:
      return DS3G_SIOLIB_HSUART1_PORT;
    case SIO_PORT_DATA_MUX_3:
      return DS3G_SIOLIB_HSUART2_PORT;
    case SIO_PORT_DATA_MUX_4:
      return DS3G_SIOLIB_HSUART3_PORT;
#endif
#if defined(FEATURE_DATA_A2_MDM) && !defined(FEATURE_DATA_SDIO_MUX_A2_DUN)
    case SIO_PORT_A2_MDM:
      return DS3G_SIOLIB_A2_MDM_PORT;
#endif /*FEATURE_DATA_A2_MDM && !FEATURE_DATA_SDIO_MUX_A2_DUN*/
#ifdef FEATURE_DATA_SDIO_MUX_A2_DUN
    case SIO_PORT_SDIO_MUX_A2_DUN_0:
      return DS3G_SIOLIB_SDIO_MUX_A2_DUN_0_PORT;
#endif /*FEATURE_DATA_SDIO_MUX_A2_DUN*/

#ifdef FEATURE_DATA_MUX
    case SIO_PORT_DATA_MUX_1:
       return DS3G_SIOLIB_DATA_MUX_1_PORT;
    case SIO_PORT_DATA_MUX_2:
       return DS3G_SIOLIB_DATA_MUX_2_PORT;
    case SIO_PORT_DATA_MUX_3:
       return DS3G_SIOLIB_DATA_MUX_3_PORT;
#endif /* FEATURE_DATA_MUX */
    default:
      return DS3G_SIOLIB_PORT_NONE;
  }
} /* ds3g_siolib_map_sio_port() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_MAP_RDM_PORT

DESCRIPTION
  This port reads the RDM port passed to it and returns the corresponding
  DSMGR port (of type ds3g_siolib_port_e_type). 

DEPENDENCIES
  None
  
RETURN VALUE
  DSMGR port of type ds3g_siolib_port_e_type
  
SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_port_e_type ds3g_siolib_map_rdm_port
(
  rdm_device_enum_type port_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(port_id) 
  {
    case RDM_UART1_DEV:
      return DS3G_SIOLIB_UART1_PORT;
#ifdef FEATURE_USB_CDC_ACM
    case RDM_USB_CDC_DEV:
      return DS3G_SIOLIB_USB_CDC_PORT;
#endif
#if defined(FEATURE_BT) || defined(FEATURE_BT_MODEM)
    case RDM_BT_NA_DEV:
      return DS3G_SIOLIB_BT_NA_PORT;
    case RDM_BT_SPP_DEV:
      return DS3G_SIOLIB_BT_SPP_PORT;
#endif /* FEATURE_BT || FEATURE_BT_MODEM */
#ifdef FEATURE_IRDA
    case RDM_IRCOMM_DEV:
      return DS3G_SIOLIB_IRCOMM_PORT;
#endif    
#if defined(FEATURE_MULTIPROCESSOR) && defined(FEATURE_SMD)
    case RDM_SMD_DS_DEV:
      return DS3G_SIOLIB_SMD_DS_PORT;
    case RDM_SMD_DATA1_DEV:
      return DS3G_SIOLIB_SMD_DATA1_PORT;
    case RDM_SMD_DATA2_DEV:
      return DS3G_SIOLIB_SMD_DATA2_PORT;
    case RDM_SMD_DATA3_DEV:
      return DS3G_SIOLIB_SMD_DATA3_PORT;
    case RDM_SMD_DATA4_DEV:
      return DS3G_SIOLIB_SMD_DATA4_PORT;
    case RDM_SMD_DATA11_DEV:
      return DS3G_SIOLIB_SMD_DATA11_PORT;
#endif /* if defined(FEATURE_MULTIPROCESSOR)... */

#ifdef FEATURE_DATA_MUX
    case RDM_MUX_DEV_1:
      return DS3G_SIOLIB_DATA_MUX_1_PORT;
    case RDM_MUX_DEV_2:
      return DS3G_SIOLIB_DATA_MUX_2_PORT;
    case RDM_MUX_DEV_3:
      return DS3G_SIOLIB_DATA_MUX_3_PORT;
#endif /* FEATURE_DATA_MUX */
    default:
      return DS3G_SIOLIB_PORT_NONE;
  }
} /* ds3g_siolib_map_rdm_port() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_MAP_DS_PORT

DESCRIPTION
  This port reads the DSMGR port passed to it and returns the corresponding
  SIO port (of type sio_port_id_type). 

DEPENDENCIES
  None
  
RETURN VALUE
  SIO port of type sio_port_id_type
  
SIDE EFFECTS
  None
  
===========================================================================*/

sio_port_id_type ds3g_siolib_map_ds_port
(
  ds3g_siolib_port_e_type port_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (port_id) 
  {
    case DS3G_SIOLIB_UART1_PORT:
      return SIO_PORT_UART_MAIN;
#ifdef FEATURE_USB_CDC_ACM
    case DS3G_SIOLIB_USB_CDC_PORT:
      return SIO_PORT_USB_CDC_ACM;
#endif
#if defined(FEATURE_BT) || defined(FEATURE_BT_MODEM)
    case DS3G_SIOLIB_BT_NA_PORT:
      return SIO_PORT_BT_NA;
    case DS3G_SIOLIB_BT_SPP_PORT:
      return SIO_PORT_BT_SPP;
#endif /* FEATURE_BT || FEATURE_BT_MODEM */
#ifdef FEATURE_IRDA
    case DS3G_SIOLIB_IRCOMM_PORT:
      return SIO_PORT_IRCOMM;
#endif
#if defined(FEATURE_MULTIPROCESSOR) && defined(FEATURE_SMD)
    case DS3G_SIOLIB_SMD_DS_PORT:
      return SIO_PORT_SMD_DS;
    case DS3G_SIOLIB_SMD_DATA1_PORT:
      return SIO_PORT_SMD_DATA1;
    case DS3G_SIOLIB_SMD_DATA2_PORT:
      return SIO_PORT_SMD_DATA2;
    case DS3G_SIOLIB_SMD_DATA3_PORT:
      return SIO_PORT_SMD_DATA3;
    case DS3G_SIOLIB_SMD_DATA4_PORT:
      return SIO_PORT_SMD_DATA4;
    case DS3G_SIOLIB_SMD_DATA11_PORT:
      return SIO_PORT_SMD_DATA11;
#endif /* if defined(FEATURE_MULTIPROCESSOR)... */
#if defined(FEATURE_DSAT_MDM_FUSION) || defined(FEATURE_DSAT_MSM_FUSION)
    case DS3G_SIOLIB_HSUART1_PORT:
      return SIO_PORT_DATA_MUX_2;
    case DS3G_SIOLIB_HSUART2_PORT:
	  return SIO_PORT_DATA_MUX_3;
    case DS3G_SIOLIB_HSUART3_PORT:
	  return SIO_PORT_DATA_MUX_4;
#endif
#if defined(FEATURE_DATA_A2_MDM) && !defined(FEATURE_DATA_SDIO_MUX_A2_DUN)
    case DS3G_SIOLIB_A2_MDM_PORT:
      return SIO_PORT_A2_MDM;
#endif
#ifdef FEATURE_DATA_SDIO_MUX_A2_DUN
    case DS3G_SIOLIB_SDIO_MUX_A2_DUN_0_PORT:
      return SIO_PORT_SDIO_MUX_A2_DUN_0;
#endif /*FEATURE_DATA_SDIO_MUX_A2_DUN*/

#ifdef FEATURE_DATA_MUX
    case DS3G_SIOLIB_DATA_MUX_1_PORT:
       return SIO_PORT_DATA_MUX_1;
    case DS3G_SIOLIB_DATA_MUX_2_PORT:
       return SIO_PORT_DATA_MUX_2;
    case DS3G_SIOLIB_DATA_MUX_3_PORT:
       return SIO_PORT_DATA_MUX_3;
#endif /* FEATURE_DATA_MUX */

    default:
      return SIO_PORT_NULL;
  }
} /* ds3g_siolib_map_ds_port() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_GET_SIO_PORT

DESCRIPTION
  This routine determines the SIO port from the specified modem port.

DEPENDENCIES
  None
  
RETURN VALUE
  SIO port of type sio_port_id_type
  
SIDE EFFECTS
  None
  
===========================================================================*/
sio_port_id_type ds3g_siolib_get_sio_port
( 
  ds3g_siolib_port_e_type modem_port_id
)
{
  if(modem_port_id != DS3G_SIOLIB_PORT_NONE)
  {
    return ds3g_siolib_map_ds_port( modem_port_id );
  }

  return SIO_PORT_NULL;
}

/*===========================================================================

FUNCTION DS3G_SIOLIB_CONVERT_DS_TO_RDM_PORT

DESCRIPTION
  This routine determines the RDM port from the specified DS modem port.

DEPENDENCIES
  None
  
RETURN VALUE
  RDM port of type rdm_device_enum_type
  
SIDE EFFECTS
  None
  
===========================================================================*/
rdm_device_enum_type ds3g_siolib_convert_ds_to_rdm_port
( 
  ds3g_siolib_port_e_type port_id
)
{

  switch(port_id) 
  {
    case DS3G_SIOLIB_UART1_PORT :
      return RDM_UART1_DEV;
#ifdef FEATURE_USB_CDC_ACM
    case DS3G_SIOLIB_USB_CDC_PORT :
      return RDM_USB_CDC_DEV;
#endif
#if defined(FEATURE_BT) || defined(FEATURE_BT_MODEM)
    case DS3G_SIOLIB_BT_NA_PORT :
      return RDM_BT_NA_DEV;
    case DS3G_SIOLIB_BT_SPP_PORT :
      return RDM_BT_SPP_DEV;
#endif /* FEATURE_BT || FEATURE_BT_MODEM */
#ifdef FEATURE_IRDA
    case DS3G_SIOLIB_IRCOMM_PORT :
      return RDM_IRCOMM_DEV;
#endif    
#if defined(FEATURE_MULTIPROCESSOR) && defined(FEATURE_SMD)
    case DS3G_SIOLIB_SMD_DS_PORT :
      return RDM_SMD_DS_DEV;
    case DS3G_SIOLIB_SMD_DATA1_PORT :
      return RDM_SMD_DATA1_DEV;
    case DS3G_SIOLIB_SMD_DATA2_PORT :
      return RDM_SMD_DATA2_DEV;
    case DS3G_SIOLIB_SMD_DATA3_PORT :
      return RDM_SMD_DATA3_DEV;
    case DS3G_SIOLIB_SMD_DATA4_PORT :
      return RDM_SMD_DATA4_DEV;
    case DS3G_SIOLIB_SMD_DATA11_PORT :
      return RDM_SMD_DATA11_DEV;
#endif /* if defined(FEATURE_MULTIPROCESSOR)... */
#ifdef FEATURE_DATA_MUX
    case DS3G_SIOLIB_DATA_MUX_1_PORT :
      return RDM_MUX_DEV_1;
    case DS3G_SIOLIB_DATA_MUX_2_PORT :
      return RDM_MUX_DEV_2;
    case DS3G_SIOLIB_DATA_MUX_3_PORT :
      return RDM_MUX_DEV_3;
#endif /* FEATURE_DATA_MUX */
    default:
      return RDM_NULL_DEV;
  }
}/* ds3g_siolib_convert_ds_to_rdm_port */

/*===========================================================================

FUNCTION DS3G_SIOLIB_INIT_CD_STATE

DESCRIPTION
  This function inits the state of the Carrier Detect (CD) signal, based on
  the following:
  1) &C setting
     &C0, &C2 - CD is always ON
     &C1 - CD is OFF

  This function should be called when a serial port is opened.

DEPENDENCIES
  None

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_init_cd_state
(
  ds3g_siolib_port_e_type port_id
)
{
  boolean         dcd_state = OFF;
  ds3g_siolib_status_e_type ret_val;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Now set the CD state appropriately, based on the &C setting and related
    state variables.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    &C2, &C0 => CD is always ON.
  -------------------------------------------------------------------------*/
  if( ((dsat_num_item_type)dsatutil_get_val(
                                     DSAT_BASIC_N_C_IDX,0,0,NUM_TYPE) == 0) || 
       ((dsat_num_item_type)dsatutil_get_val(
                                     DSAT_BASIC_N_C_IDX,0,0,NUM_TYPE) == 2))
  {
    dcd_state = ON;
  }
  /*-------------------------------------------------------------------------
    &C1 => CD is ON only if the remote carrier is ON.
  -------------------------------------------------------------------------*/
  else if( (dsat_num_item_type)dsatutil_get_val(
                                     DSAT_BASIC_N_C_IDX,0,0,NUM_TYPE) == 1 )
  {
    dcd_state = OFF;
  }

  /*-------------------------------------------------------------------------
    Set the state of the Carrier Detect (CD) signal.
  -------------------------------------------------------------------------*/
  ret_val = ( dcd_state ? ds3g_siolib_cd_assert(port_id) :
                          ds3g_siolib_cd_deassert(port_id) );

  return( ret_val );

} /* ds3g_siolib_init_cd_state() */



/*===========================================================================
FUNCTION DS3G_SIOLIB_SET_PORT_INBOUND_FLOW

DESCRIPTION
  This function controls the enabling or diabling of inbound flow ctl on a 
  specified port.
  This function was required because inbound flow ctl. can be changed
  from several places for different reasons, and we need an arbitrator
  function to handle those changes.

  Basically, the function enables inbound flow, when no caller wants it
  disabled. If even one caller wants the flow disabled, the inbound flow
  is disabled.


DEPENDENCIES
  This is the only function that should be used to toggle the inbound flow.
  This is important because the function keeps track of the state of inbound
  flow, and if the state of inbound flow is changed


RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds3g_siolib_set_port_inbound_flow
(
  ds3g_siolib_port_e_type  port_id,   /* DS3G_SIOLIB port id                */
  uint32 in_mask,          /* input mask of calling function               */
  ds3g_flow_e_type flow  /* specifies if flow is to be enabled or disabled */
)
{
  ds3g_siolib_ex_set_inbound_flow(in_mask,flow,port_id);

} /* ds3g_siolib_set_port_inbound_flow() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_SETUP_AT_TX

DESCRIPTION
  This function queues data (ATCOP responses) for transmission over the 
  serial interface.
  Additionally, Autodetect Mode is re-armed, if specified by the caller.
  Typically, the re-arm feature is used only by ATCoP.

DEPENDENCIES
  None

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_setup_at_tx
(
  dsm_item_type  *tx_ptr,         /* DSM item containing data to transmit */
  boolean         rearm_autodetect       /* TRUE = Re-arm Autodetect Mode */
)
{
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_OK;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* We will take care of unsolicted data first, The data will be sent to 
  ** the port depending on ds3g_siolib_tx_type set by ds3g_siolib_setup_tx_type.
  */
  if (ds3g_siolib_tx_type) 
  {
    ds3g_siolib_handle_unsolicited_result ( &ret_val, tx_ptr, rearm_autodetect, 
                                    ds3g_siolib_tx_type, ds3g_siolib_tx_port);
    /* reset the tx_type */
    ds3g_siolib_tx_type = DS3G_SIOLIB_UNSOLICTED_NONE;
    ds3g_siolib_tx_port = DS3G_SIOLIB_PORT_NONE;
    ds3g_siolib_client_type = DS3G_SIOLIB_CLIENT_NONE;
    return( ret_val );
  }

  /*-------------------------------------------------------------------------
    Send the input data to the underlying SIO device.  If we haven't been 
    assigned a valid stream ID by SIO, trash the input data.
  -------------------------------------------------------------------------*/
  ds3g_siolib_send_data_to_port(&ret_val, tx_ptr, rearm_autodetect, 
                                ds3g_siolib_active_port);

  /* Check to see whether the other port is available to process*/
  if (rearm_autodetect) 
  {
    ds3g_siolib_service_ports();
  }

  ds3g_siolib_client_type = DS3G_SIOLIB_CLIENT_NONE;
  return( ret_val );

} /* ds3g_siolib_setup_at_tx() */



/*===========================================================================

FUNCTION DS3G_SIOLIB_HANDLE_UNSOLICITED_RESULT

DESCRIPTION
  This function process the tx_type for unsolicited result code and sends the
  response to the specified port. In the event the specified port is 
  DS3G_SIOLIB_DATA_PORT, the response is broadcasted to all serial ports
  currently open.

DEPENDENCIES
  It depends on ds3g_siolib_tx_type set by ds3g_siolib_set_tx_type().

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
 
void ds3g_siolib_handle_unsolicited_result
(
  ds3g_siolib_status_e_type *ret_val,                      /* SIO return value            */
  dsm_item_type           *tx_ptr,          /* dsm item ptr                */
  boolean                 rearm_autodetect, /* Done with AT command line?  */ 
  ds3g_siolib_tx_e_type   tx_type,          /* unsolicited result code type*/
  ds3g_siolib_port_e_type tx_port           /* Desired port                */
)
{
 
  dsm_item_type           *tmp_ptr = NULL;
  ds3g_siolib_port_e_type      port_count;
  ds3g_siolib_port_e_type      port_init = DS3G_SIOLIB_PORT_NONE;
  ds3g_siolib_state_info_type *state = NULL;
  boolean dtr_status = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 /*-----------------------------------------------------------------------
   Increment the enumerator to point to DS3G_SIOLIB_PORT_NONE+1
 ------------------------------------------------------------------------*/     
  port_init++;
#ifdef FEATURE_DSAT_MSM_FUSION
  if(tx_port == DS3G_SIOLIB_DATA_PORT )
  {
    tx_port = ds3g_siolib_get_urc_port();
  }
#endif /* FEATURE_DSAT_MSM_FUSION */
  switch (tx_type) 
  {
  case DS3G_SIOLIB_UNSOLICTED_FAX_RESULT_CODE:
  case DS3G_SIOLIB_UNSOLICTED_DS_RESULT_CODE:
  case DS3G_SIOLIB_UNSOLICTED_AT_ERROR_CODE:
  case DS3G_SIOLIB_UNSOLICTED_CM:
  case DS3G_SIOLIB_UNSOLICTED_SMS:
  case DS3G_SIOLIB_UNSOLICTED_ACC:
    if ( tx_port == DS3G_SIOLIB_DATA_PORT && tx_ptr != NULL) 
    {
      for (port_count = port_init; 
            port_count < DS3G_SIOLIB_PORT_MAX; port_count++) 
      {
#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT
        if ( port_count == DS3G_SIOLIB_CLIENT_VSP_PORT )
        {
          dsatclient_send_urc_response(tx_ptr,ret_val,rearm_autodetect);
          continue;
        }
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT */
        state = &ds3g_siolib_state[port_count];
        ds3g_siolib_ex_is_dtr_asserted(&dtr_status, port_count);

        DS_SIO_MSG3_MED("Port:%d, DTR%d, initDTR%d",port_count,dtr_status,state->is_dtr_high);

        if( ( state->serial_info.stream_id != SIO_NO_STREAM_ID ) &&
            ( state->at_state != DS3G_SIOLIB_ONLINE_DATA_AT_STATE)&&
            ( state->serial_info.serial_state == DS3G_SIOLIB_AUTODETECT_SERIAL_STATE) &&
            ( dtr_status == TRUE) )
        {
          uint16 size = (uint16)dsm_length_packet( tx_ptr );
          if ( dsm_dup_packet( &tmp_ptr, tx_ptr, 0, size ) != size )
          {
            dsm_free_packet( &tmp_ptr );
          }
          else
          {
            ds3g_siolib_send_data_to_port(ret_val,tmp_ptr,rearm_autodetect, 
                                          port_count);
          }
        }
      }
      dsm_free_packet( &tx_ptr );
    }
    else
    {
      ds3g_siolib_send_data_to_port(ret_val,tx_ptr,rearm_autodetect,tx_port);
    }
    break;

  default:
    DS_SIO_MSG0_ERROR("WRONG UNSOLICITED RESULT CODE TYPE");
    break;
  }

  if( (DS3G_SIOLIB_UNSOLICTED_FAX_RESULT_CODE != tx_type) &&
      (rearm_autodetect || 
       (DS3G_SIOLIB_UNSOLICTED_DS_RESULT_CODE == tx_type)) ) 
  {
    /* Check to see whether the other port is available to process. */
    /* Not done for fax calls due to performance constraints.       */
    /* For Unsolicited DS codes (CONNECT, NO_CARRIER, etc.) reset the 
      active port as these codes are send in response to AT cmds */
    DS_SIO_MSG3_HIGH( "rearm_auto %d tx_type %d active_port %d", rearm_autodetect, tx_type, ds3g_siolib_active_port );
    ds3g_siolib_service_ports();
    
  }
 
} /* ds3g_siolib_handle_unsolicited_result() */



/*===========================================================================

FUNCTION DS3G_SIOLIB_SEND_DATA_TO_PORT

DESCRIPTION
  This function does tx based on specified port.

DEPENDENCIES
  Called from ds3g_siolib_setup_tx or ds3g_siolib_handle_unsolicited_result.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_siolib_send_data_to_port
(
  ds3g_siolib_status_e_type *ret_val,
  dsm_item_type  *tx_ptr,        /* DSM item ptr                  */
  boolean       rearm_autodetect,/* Done with one AT Command Line?*/
  ds3g_siolib_port_e_type port_id   /* Current active port           */
)
{

  sio_ioctl_param_type  ioctl_param;
  ds3g_siolib_state_info_type *state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ( port_id <= DS3G_SIOLIB_PORT_NONE ) || 
      ( port_id >= DS3G_SIOLIB_PORT_MAX ) )
  {
    DS_SIO_MSG1_ERROR( "input port is %d", port_id );
    ASSERT(0);
    return;
  }

#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT
  if(port_id == DS3G_SIOLIB_CLIENT_VSP_PORT)
  {
    dsatclient_send_response(tx_ptr,ret_val,rearm_autodetect);
  }
  else
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT */
  {
    state = &ds3g_siolib_state[port_id];
  
    /*-------------------------------------------------------------------------
     Send the input data to the underlying SIO device.  If we haven't been 
     assigned a valid stream ID by SIO, trash the input data.
    -------------------------------------------------------------------------*/
    if( (state->serial_info.stream_id != SIO_NO_STREAM_ID) &&
        (state->serial_info.close_in_progress == FALSE) )
    {
      if( rearm_autodetect )
      {
        if( state->serial_info.serial_state == DS3G_SIOLIB_AUTODETECT_SERIAL_STATE )
        {
          sio_ioctl( state->serial_info.stream_id, SIO_IOCTL_ENABLE_AUTODETECT,
                     &ioctl_param );
        }
        else
        {
          DS_SIO_MSG0_HIGH( "Autodt rearmed in non-Autodt serial mode" );
        }
      }
  
      sio_transmit( state->serial_info.stream_id, tx_ptr );
    }
    else
    {
      DS_SIO_MSG0_ERROR("Null stream_id in ds3g_siolib_send_data_to_port");
      dsm_free_packet ( &tx_ptr );
      if(ret_val != NULL)
      {
        *ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
      }
    }
  }

} /* ds3g_siolib_send_data_to_port() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_SERVICE_PORTS

DESCRIPTION
  This function checks the ds3g_port_waiting_mask to see which port we
  will process next. We use Round Robin Algorithm here. The first port to be 
  processed is the one next to the port just processed. This ensures fairness
  by not allowing a single port to hog the ATCOP engine.

DEPENDENCIES
  It depends on current active port which is ds3g_active_port. 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_siolib_service_ports (void)
{

  ds3g_siolib_port_e_type current_port = ds3g_siolib_active_port;
  ds3g_siolib_port_e_type max_port = DS3G_SIOLIB_PORT_MAX;
  ds3g_siolib_port_e_type port_count;
  int loop;
  ds3g_siolib_state_info_type *state = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SIO_MSG0_HIGH( "Called service ports ");

  /* Set the port_waiting_mask for the active port in case the rx watermark is
   ** still not empty.
   */
  if( ds3g_siolib_active_port != DS3G_SIOLIB_PORT_NONE )
  {
    state = &ds3g_siolib_state[ds3g_siolib_active_port];
    if(state->serial_info.rx_wm_ptr.current_cnt != 0)
    {
      ds3g_siolib_port_waiting_mask |= state->serial_info.rx_pending_bit;
    }
  }

  /* We will depend on current active port to decide which port is the next
   ** one to be processed.
   */
  if (ds3g_siolib_port_waiting_mask) 
  {
    for(loop=0; loop<2; loop++)
    {
      for(port_count = ++current_port; ((port_count < max_port) && 
                                       (port_count > DS3G_SIOLIB_PORT_NONE) &&
                                       (port_count < DS3G_SIOLIB_PORT_MAX)); port_count++) 
      {
        state = &ds3g_siolib_state[(int)port_count];
        if(ds3g_siolib_port_waiting_mask & state->serial_info.rx_pending_bit)
        {
          ds3g_siolib_port_waiting_mask &= ~state->serial_info.rx_pending_bit;
          if( state->serial_info.rx_sig)
          {
            DS_SIO_MSG0_MED("Signal was already set previously");
          }
          state->serial_info.rx_sig = TRUE;
          (void) rex_set_sigs( dsi_get_ds_tcb(),DS_DATA_PORT_1ST_SIO_RX_SIG);
          ds3g_siolib_active_port = DS3G_SIOLIB_PORT_NONE;
          return;
        }
      }
      current_port = DS3G_SIOLIB_PORT_NONE;
      max_port = (ds3g_siolib_port_e_type)(ds3g_siolib_active_port + 1);
    }
  }

  /* Check to make sure that we do not reset the active port if the rx watermark
   ** is non empty.
   */
  if( (ds3g_siolib_active_port != DS3G_SIOLIB_PORT_NONE) &&
      (dsatcmdp_processing_async_cmd == FALSE) )
  {
    state = &ds3g_siolib_state[ds3g_siolib_active_port];
    if(state->serial_info.rx_wm_ptr.current_cnt == 0)
    {
      ds3g_siolib_active_port = DS3G_SIOLIB_PORT_NONE;
    }
  }

#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT
  /* Service the clients only when current client got serviced. 
     'ds3g_siolib_ active_ext_client' is set to NULL 
     when the response is sent back or 
     when there is really no client command received. 
  */
  if (ds3g_siolib_active_ext_client == DSAT_EXT_CLIENT_ID_INVALID )
  {
    ds3g_siolib_service_ext_clients();
  }
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT */
} /* ds3g_siolib_service_ports() */

#ifdef FEATURE_DSAT_MDM_FUSION
/*===========================================================================

FUNCTION DS3G_SIOLIB_GET_PROXY_PORT

DESCRIPTION
  Returns the current proxy port

DEPENDENCIES
  None

RETURN VALUE
  DS3G_SIOLIB_ACTIVE_PORT

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_port_e_type ds3g_siolib_get_proxy_port ( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ds3g_siolib_proxy_port;
} /* ds3g_siolib_get_proxy_port() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_SET_PROXY_PORT

DESCRIPTION
  Sets the current proxy port

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_siolib_set_proxy_port
(
  ds3g_siolib_port_e_type port_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds3g_siolib_proxy_port = port_id;
} /* ds3g_siolib_set_proxy_port() */
#endif /* FEATURE_DSAT_MDM_FUSION */

#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT
/*===========================================================================

FUNCTION DS3G_SIOLIB_GET_ACTIVE_EXT_CLIENT

DESCRIPTION
  Returns the current active external client

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

dsat_ext_client_id_type ds3g_siolib_get_active_ext_client ( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ds3g_siolib_active_ext_client;
} /* ds3g_siolib_get_active_ext_client() */
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT */
/*===========================================================================

FUNCTION DS3G_SIOLIB_GET_AT_STATE

DESCRIPTION
  Returns the current AT command state.

DEPENDENCIES
  None

RETURN VALUE
  ds3g_siolib_at_state_e_type 

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_at_state_e_type ds3g_siolib_get_at_state 
( 
  ds3g_siolib_port_e_type port      /* port_id */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( ( port > DS3G_SIOLIB_DATA_PORT ) && 
     ( port < DS3G_SIOLIB_PORT_MAX ) )
  {
    return ds3g_siolib_state[port].at_state;
  }
  else
  {
    DS_SIO_MSG1_ERROR("Invalid Input port (%d)",(int)port);
    return DS3G_SIOLIB_NULL_AT_STATE;
  }
} /* ds3g_siolib_get_at_state() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_GET_MODEM_PORT

DESCRIPTION
  Returns the current modem port

DEPENDENCIES
  None

RETURN VALUE
  DS3G_SIOLIB_MODEM_PORT

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_port_e_type ds3g_siolib_get_modem_port ( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ds3g_siolib_modem_port;
} /* ds3g_siolib_get_modem_port() */

#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT
/*===========================================================================

FUNCTION DS3G_SIOLIB_SET_ACTIVE_EXT_CLIENT

DESCRIPTION
  Sets the current active external client

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_siolib_set_active_ext_client 
(  
  dsat_ext_client_id_type client_id    /* External client id */
)
{
  ds3g_siolib_active_ext_client = client_id;
} /* ds3g_siolib_set_active_ext_client() */
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT */

/*===========================================================================
FUNCTION DS3G_SIOLIB_SET_TX_TYPE

DESCRIPTION
  This function sets up the tx_type for unsolicited data.
  the tx_type is used in ds3g_siolib_setup_tx() while doing tx to SIO.
  We use tx_type to process unsolicited data such as RSSI, SMS...etc.
  Function call also specifies the port to which the unsolicited data
  has to be sent.

DEPENDENCIES
  ds3g_siolib_setup_tx() will use tx_type to distribute data.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_siolib_set_tx_type
(
  ds3g_siolib_tx_e_type     tx_type,      /* unsolicited result code type */
  ds3g_siolib_port_e_type   desired_port  /* port type                    */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds3g_siolib_tx_type  = tx_type;
  ds3g_siolib_tx_port  = desired_port;
} /* ds3g_siolib_set_tx_type() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_SET_CLIENT_TYPE

DESCRIPTION
  This function sets up the client_type for response routing.
  Mainly used when on ONLINE DATA state. While on ONLINE DATA state,
  distinction has to be made on where to route the data. If the data
  is from ATCOP, it cannot be routed to MODEM port. If any virtual port
  is active, it should be routed to that port. Active port will receive
  the data while on ONLINE COMMAND mode and COMMAND mode.

DEPENDENCIES
  ds3g_siolib_setup_tx() will use client_type and resets the state
  variable.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_siolib_set_client_type
(
  ds3g_siolib_client_e_type client_type /* client type */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds3g_siolib_client_type  = client_type;
} /* ds3g_siolib_set_client_type() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_GET_CLIENT_TYPE

DESCRIPTION
  This function gets the client_type for response routing.
  Mainly used when on ONLINE DATA state. While on ONLINE DATA state,
  distinction has to be made on where to route the data. If the data
  is from ATCOP, it cannot be routed to MODEM port. If any virtual port
  is active, it should be routed to that port. Active port will receive
  the data while on ONLINE COMMAND mode and COMMAND mode.

DEPENDENCIES
  ds3g_siolib_setup_tx() will use client_type and resets the state
  variable.

RETURN VALUE
  Client Type

SIDE EFFECTS
  None

===========================================================================*/
ds3g_siolib_client_e_type ds3g_siolib_get_client_type
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds3g_siolib_client_type;
} /* ds3g_siolib_set_client_type() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_RESET_MODEM_PORT

DESCRIPTION
  Reset the current modem port to be DS3G_SIOLIB_PORT_NONE.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_siolib_reset_modem_port ( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds3g_siolib_modem_port != DS3G_SIOLIB_PORT_NONE)
  {
    DS_SIO_MSG1_HIGH("Resetting the Modem port: %d", ds3g_siolib_modem_port);
    ds3g_siolib_last_modem_port = ds3g_siolib_modem_port;
    ds3g_siolib_modem_port = DS3G_SIOLIB_PORT_NONE;
  }
  else
  {
    DS_SIO_MSG0_MED("Resetting a NULL modem port");
  }

} /* ds3g_siolib_reset_modem_port() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_SET_MODEM_PORT

DESCRIPTION
  Sets the modem port to be the current active port or for an incoming call
  to the appropriate answer port.
  
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_siolib_set_modem_port
(
  ds3g_siolib_port_e_type port_id
)
{
  ds3g_siolib_modem_port = port_id;
} /* ds3g_siolib_set_modem_port() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_IS_EXTERNAL_PORT_FREE

DESCRIPTION
  Reports state of external modem port availablilty.  Should be called by
  mode-specific handler before alerting TE for terminating calls.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if modem port is unassigned, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean ds3g_siolib_is_external_port_free( void ) 
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  int i = 0;

  for(i=0; i< DS3G_SIOLIB_PORTID_MAX; i++)
  {
    MSG_MED( "ds3g_siolib_is_external_port_free: ds3g_siolib_port_allocated[i] %d ", 
              ds3g_siolib_port_allocated[i], 0, 0 ); 
    if(ds3g_siolib_port_allocated[i] == DS3G_SIOLIB_PORT_NONE)
    {
      return TRUE;
    }
  }

  return FALSE;
} /* ds3g_siolib_is_external_port_free() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_SET_ANSWER_PORT

DESCRIPTION
  Assigns the active port depending on the answer mode for CSData
  calls.  For auto-answer mode, the active port is preserved in
  answer_port.  This is used to assign the modem_port when auto-answer
  is invoked. For manual answer mode, the active port is assigned
  directly to the modem_port.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_siolib_set_answer_port
(
  ds3g_siolib_answer_mode_e_type answer_mode  /* Call answer mode */
)
{
  dsat_num_item_type s0_val = (dsat_num_item_type)dsatutil_get_val(
                                         DSAT_SREG_S0_IDX,0,0,NUM_TYPE);
  boolean autoanswer = ( 0 != s0_val );

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds3g_siolib_active_port == DS3G_SIOLIB_PORT_NONE)
  {
    DATA_ERR_FATAL("siolib_active_port is DS3G_SIOLIB_PORT_NONE");
    return;
  }

  /*-------------------------------------------------------------------------
    Assign port based on caller's answer mode.
  -------------------------------------------------------------------------*/
  switch (answer_mode)
  {
  case DS3G_SIOLIB_ANSWER_MODE_MANUAL:
    /*-----------------------------------------------------------------------
      Used by ATA to slam modem port to active port in
      ds3g_siolib_set_modem_port(). We do not assign modem port here in 
      case call is aborted before connect call processing has completed.
    -----------------------------------------------------------------------*/
    SET_ANSWERMODE( DS3G_SIOLIB_ANSWER_MODE_MANUAL );
    break;
    
  case DS3G_SIOLIB_ANSWER_MODE_AUTO:
    /*-----------------------------------------------------------------------
      Used by ATSO to preserve the port setting auto-answer.
    -----------------------------------------------------------------------*/
    if( autoanswer )
    {
      /* Enable autoanswer mode for current active port */
      SET_ANSWERMODE( DS3G_SIOLIB_ANSWER_MODE_AUTO );
      ds3g_siolib_answer_info.port = ds3g_siolib_active_port;
    }
    else
    {
      /* Disable autoanswer mode */
      CLEAR_ANSWERMODE( DS3G_SIOLIB_ANSWER_MODE_AUTO );
      ds3g_siolib_answer_info.port = DS3G_SIOLIB_PORT_NONE;
    }
    break;
    
  default:
    DS_SIO_MSG1_ERROR("Unsupported answer mode: %d", answer_mode);
  }
  return;
} /* ds3g_siolib_set_answer_port() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_GET_ANSWER_PORT

DESCRIPTION
  Returns the cached port for call answer when auto-answer enabled.

DEPENDENCIES
  None

RETURN VALUE
  Valid port when auto-answer enabled, DS3G_SIOLIB_PORT_NONE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_port_e_type ds3g_siolib_get_answer_port ( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return IS_ANSWERMODE( DS3G_SIOLIB_ANSWER_MODE_AUTO )?
         ds3g_siolib_answer_info.port : DS3G_SIOLIB_PORT_NONE;
} /* ds3g_siolib_get_answer_port() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_SET_ANSWER_CALLBACKS

DESCRIPTION
  Assigns the callback function table for auto answer functionality.
  Parameter may be NULL to clear the registration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_siolib_set_answer_callbacks
(
  ds3g_siolib_func_tbl_type  *func_tbl_ptr               /* Callback table */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Assign the callback table (may be NULL) */
  ds3g_siolib_answer_info.func_tbl_ptr = func_tbl_ptr;
  return;
} /* ds3g_siolib_set_answer_port() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_CHANGE_SIO_PARAMS

DESCRIPTION
  Sets the baud rate and flow control of the specified SIO port.
  Change of baud rate may be scheduled or immediate based on caller.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_siolib_change_sio_params
( 
  sio_port_id_type port_id,                      /* SIO port_id            */
  sio_flow_ctl_type tx_flow,                    /* TX flow control method */
  sio_flow_ctl_type rx_flow,                    /* RX flow control method */
  sio_bitrate_type new_baud_rate,               /* Requested baud rate    */
  boolean change_baud_now                       /* Immediate or post ack  */
) 
{

  ds3g_siolib_port_e_type port = DS3G_SIOLIB_PORT_NONE;
  sio_ioctl_param_type  ioctl_param;
  ds3g_siolib_state_info_type *state;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  port = ds3g_siolib_map_sio_port(port_id);

  if (port == DS3G_SIOLIB_PORT_NONE) 
  {
        DS_SIO_MSG0_ERROR("siolib_modem_port = PORT_NONE");
        return;
  }

  state = &ds3g_siolib_state[port];

  if( (state->serial_info.stream_id != SIO_NO_STREAM_ID) &&
      (state->serial_info.close_in_progress == FALSE) )
  {
      /* Change flow control */
    ioctl_param.flow_ctl.tx_flow = tx_flow;
    ioctl_param.flow_ctl.rx_flow = rx_flow;
    sio_ioctl( state->serial_info.stream_id, 
               SIO_IOCTL_SET_FLOW_CTL, 
               &ioctl_param );

    /* Change baud rate */
    ioctl_param.bitrate = new_baud_rate;
    sio_ioctl( state->serial_info.stream_id,
               (change_baud_now) ? SIO_IOCTL_CHANGE_BAUD_NOW
                                : SIO_IOCTL_CHANGE_BAUD_RATE,
                &ioctl_param );
  }
  else
  {
    DS_SIO_MSG1_ERROR ("Changing SIO params w/bad stream id %d", state->serial_info.stream_id);
  }
} /* ds3g_siolib_change_sio_params() */


/*===========================================================================
FUNCTION DS3G_SIOLIB_SET_INBOUND_FLOW

DESCRIPTION
  This function controls the enabling or diabling of inbound flow ctl.
  This function was required because inbound flow ctl. can be changed
  from several places for different reasons, and we need an arbitrator
  function to handle those changes.

  Basically, the function enables inbound flow, when no caller wants it
  disabled. If even one caller wants the flow disabled, the inbound flow
  is disabled.


DEPENDENCIES
  This is the only function that should be used to toggle the inbound flow.
  This is important because the function keeps track of the state of inbound
  flow, and if the state of inbound flow is changed


RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds3g_siolib_set_inbound_flow
(
  uint32 in_mask,          /* input mask of calling function                 */
  ds3g_flow_e_type flow  /* specifies if flow is to be enabled or disabled */
)
{                                     
  ds3g_siolib_port_e_type port_id = ds3g_siolib_modem_port;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( port_id == DS3G_SIOLIB_PORT_NONE )
  {
    if( ds3g_siolib_last_modem_port == DS3G_SIOLIB_PORT_NONE )
    {
      DS_SIO_MSG0_ERROR("Undefined Modem port for setting inbound flow");
      return;
    }
    else
    {
      port_id = ds3g_siolib_last_modem_port;
    }
  }

  ds3g_siolib_set_port_inbound_flow(port_id, in_mask, flow);
} /* ds3g_siolib_set_inbound_flow() */

#ifdef FEATURE_DSAT_MDM_FUSION

/*===========================================================================
FUNCTION DS3G_SIOLIB_ALLOCATE_FORWARDING_PORT

DESCRIPTION This function accepts a physical port as parameter 
            (i.e. the port on which an AT command is received).
			It allocates and returns the forwarding port to the calling 
			function, if available. Initially the forwarding port state is
			DS3G_SIOLIB_FORWARDING_PORT_AVAILABLE_STATE. If the allocation 
			of the forwarding port is successful, the state is changed to 
			DS3G_SIOLIB_FORWARDING_PORT_AT_STATE.

DEPENDENCIES

RETURN VALUE
  Forwarding Port on SUCCESS
  DS3G_SIOLIB_PORT_NONE on FAILURE

SIDE EFFECTS
  An entry is made in the forwarding port table, which maps the physical port
  to the forwarding port.
===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_allocate_forwarding_port
(
  ds3g_siolib_port_e_type phys_port
)
{
  uint8 port_id;
  uint8 j;
  ds3g_siolib_state_info_type *state;

  DS_SIO_MSG0_HIGH("Try to allocate Forwarding Port");

  if(ds3g_siolib_state[DS3G_SIOLIB_HSUART1_PORT].forwarding_port_state 
      == DS3G_SIOLIB_FORWARDING_PORT_AVAILABLE_STATE)
  {
    port_id = DS3G_SIOLIB_HSUART1_PORT;
  }
  else if(ds3g_siolib_state[DS3G_SIOLIB_HSUART2_PORT].forwarding_port_state 
      == DS3G_SIOLIB_FORWARDING_PORT_AVAILABLE_STATE)
  {
    port_id = DS3G_SIOLIB_HSUART2_PORT;
  }
  else
  {
    DS_SIO_MSG0_ERROR("None of the UART Ports available for forwarding !");
    return DS3G_SIOLIB_PORT_NONE;
  }

  state = &ds3g_siolib_state[port_id];
  state->forwarding_port_state = DS3G_SIOLIB_FORWARDING_PORT_AT_STATE;

  for (j=0; j < NUM_FORWARDING_PORTS_MAX; j++)
  {
    if ((forwarding_port_tbl[j].phys_port == DS3G_SIOLIB_PORT_NONE ) &&
        (forwarding_port_tbl[j].forwarding_port == DS3G_SIOLIB_PORT_NONE ))
    {
      forwarding_port_tbl[j].phys_port = phys_port;
      forwarding_port_tbl[j].forwarding_port = 
                                          (ds3g_siolib_port_e_type)port_id;
      return (ds3g_siolib_port_e_type)port_id;
    }
  }

  DS_SIO_MSG0_ERROR("Couldnot allocate a forwarding port");
  return DS3G_SIOLIB_PORT_NONE;
}

/*===========================================================================
FUNCTION DS3G_SIOLIB_GET_FORWARDING_PORT

DESCRIPTION This function accepts a physical port as parameter 
            (i.e. the port on which an AT command is received).
			It looks up the forwarding port table and returns the previously
			allocated forwarding port to the calling function, if available. 
DEPENDENCIES

RETURN VALUE
  Forwarding Port on SUCCESS
  DS3G_SIOLIB_PORT_NONE on FAILURE

SIDE EFFECTS

===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_get_forwarding_port
(
  ds3g_siolib_port_e_type phys_port
)
{
  uint8 j;

  DS_SIO_MSG0_HIGH("Try to fetch Forwarding Port");
  for (j=0; j < NUM_FORWARDING_PORTS_MAX; j++)
  {
    if (forwarding_port_tbl[j].phys_port == phys_port) 
    {
      return forwarding_port_tbl[j].forwarding_port;
    }
  }

  DS_SIO_MSG0_ERROR("Couldnot fetch the forwarding port");
  return DS3G_SIOLIB_PORT_NONE;
}

/*===========================================================================
FUNCTION DS3G_SIOLIB_CHANGE_FORWARDING_PORT_STATE

DESCRIPTION  This function changes the state of the forwarding port to the 
             specifed value.
DEPENDENCIES

RETURN VALUE None

SIDE EFFECTS

===========================================================================*/
void ds3g_siolib_change_forwarding_port_state
(
  ds3g_siolib_port_e_type                forwarding_port,
  ds3g_siolib_forwarding_port_state_type state
)
{
  DS_SIO_MSG1_HIGH("Change the state of forwarding port to %d",(uint32)state);

  if ((forwarding_port <=  DS3G_SIOLIB_PORT_NONE) ||
      (forwarding_port >= DS3G_SIOLIB_PORT_MAX)) 
  {
    DS_SIO_MSG0_ERROR("Invalid Forwarding Port");
    return;
  }

  ds3g_siolib_state[forwarding_port].forwarding_port_state = state;
  return;
}

/*===========================================================================
FUNCTION DS3G_SIOLIB_RELEASE_FORWARDING_PORT

DESCRIPTION This function accepts forwarding port as the parameter. 
            It releases the forwarding port and changes its state back to
			DS3G_SIOLIB_FORWARDING_PORT_AVAILABLE_STATE. 
            
DEPENDENCIES

RETURN VALUE None

SIDE EFFECTS
  The entry in forwarding port table which maps the physical port
  to the forwarding port is now removed.
===========================================================================*/
void ds3g_siolib_release_forwarding_port
(
  ds3g_siolib_port_e_type forwarding_port
)
{
  uint8 j;

  DS_SIO_MSG0_HIGH("Release the forwarding port");

  if ((forwarding_port !=  DS3G_SIOLIB_HSUART1_PORT ) &&
      (forwarding_port != DS3G_SIOLIB_HSUART2_PORT)) 
  {
    DS_SIO_MSG0_ERROR("Invalid Forwarding Port");
    return;
  }

  for (j=0; j < NUM_FORWARDING_PORTS_MAX; j++)
  {
    if (forwarding_port_tbl[j].forwarding_port == forwarding_port)
    {
      forwarding_port_tbl[j].forwarding_port = 
        DS3G_SIOLIB_PORT_NONE;
      forwarding_port_tbl[j].phys_port = 
        DS3G_SIOLIB_PORT_NONE;
    }
  }

  ds3g_siolib_state[forwarding_port].forwarding_port_state = 
    DS3G_SIOLIB_FORWARDING_PORT_AVAILABLE_STATE;
}
#endif /* FEATURE_DSAT_MDM_FUSION */

#ifdef FEATURE_DSAT_MSM_FUSION
ds3g_siolib_port_e_type ds3g_siolib_get_urc_port
(
  void
)
{
  return DS3G_SIOLIB_HSUART3_PORT;
}
#endif /* FEATURE_DSAT_MSM_FUSION */
 
#ifdef FEATURE_DSAT_MDM_FUSION
/*===========================================================================
FUNCTION       DS3G_SIOLIB_SET_DS3G_SIOLIB_PROXY_PORT

DESCRIPTION    This function sets the ds3g_siolib_proxy_port

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
void ds3g_siolib_set_ds3g_siolib_proxy_port
(
  ds3g_siolib_port_e_type proxy_port
)
{
  ds3g_siolib_proxy_port  = proxy_port;
}

/*===========================================================================
FUNCTION       DS3G_SIOLIB_GET_DS3G_SIOLIB_PROXY_PORT

DESCRIPTION    This function returns the ds3g_siolib_proxy_port.

DEPENDENCIES   None.

RETURN VALUE   ds3g_siolib_port_e_type - proxy port

SIDE EFFECTS   None

===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_get_ds3g_siolib_proxy_port 
(
  void
)
{
  return ds3g_siolib_proxy_port;
}

#endif /* FEATURE_DSAT_MDM_FUSION*/

/*===========================================================================
FUNCTION       DS3G_SIOLIB_SET_DS3G_SIOLIB_ACTIVE_PORT

DESCRIPTION    This function sets the ds3g_siolib_active_port

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
void ds3g_siolib_set_ds3g_siolib_active_port
(
  ds3g_siolib_port_e_type active_port
)
{
  ds3g_siolib_active_port  = active_port;
}

/*===========================================================================
FUNCTION       DS3G_SIOLIB_GET_DS3G_SIOLIB_ACTIVE_PORT

DESCRIPTION    This function returns the ds3g_siolib_active_port.

DEPENDENCIES   None.

RETURN VALUE   ds3g_siolib_port_e_type - active port

SIDE EFFECTS   None

===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_get_ds3g_siolib_active_port 
(
  void
)
{
  return ds3g_siolib_active_port;
}

/*===========================================================================
FUNCTION       DS3G_SIOLIB_SET_DS3G_SIOLIB_MODEM_PORT

DESCRIPTION    This function sets the ds3g_siolib_modem_port

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
void ds3g_siolib_set_ds3g_siolib_modem_port
(
  ds3g_siolib_port_e_type modem_port
)
{
  ds3g_siolib_modem_port  = modem_port;
}

/*===========================================================================
FUNCTION       DS3G_SIOLIB_GET_DS3G_SIOLIB_MODEM_PORT

DESCRIPTION    This function returns the current port in data call.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_get_ds3g_siolib_modem_port 
(
  void
)
{
  return ds3g_siolib_modem_port;
}

/*===========================================================================
FUNCTION       DS3G_SIOLIB_SET_DS3G_SIOLIB_PENDING_CLOSE_PORT

DESCRIPTION    This function sets the port pending sio_close callback

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
void ds3g_siolib_set_ds3g_siolib_pending_close_port
(
  ds3g_siolib_port_e_type pending_close_port
)
{
  ds3g_siolib_pending_close_port  = pending_close_port;
}

/*===========================================================================
FUNCTION       DS3G_SIOLIB_GET_DS3G_SIOLIB_PENDING_CLOSE_PORT

DESCRIPTION    This function returns the port pending sio_close callback

DEPENDENCIES   None.

RETURN VALUE   ds3g_siolib_port_e_type - pending close port

SIDE EFFECTS   None

===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_get_ds3g_siolib_pending_close_port 
(
  void
)
{
  return ds3g_siolib_pending_close_port;
}

/*===========================================================================
FUNCTION       DS3G_SIOLIB_SET_DS3G_SIOLIB_CURRENT_RING_TYPE

DESCRIPTION    This function sets the current ring type

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
void ds3g_siolib_set_ds3g_siolib_current_ring_type
(
  dsat_result_enum_type current_ring_type
)
{
  ds3g_siolib_current_ring_type  = current_ring_type;
}

/*===========================================================================
FUNCTION       DS3G_SIOLIB_GET_DS3G_SIOLIB_CURRENT_RING_TYPE

DESCRIPTION    This function returns the current ring type

DEPENDENCIES   None.

RETURN VALUE   dsat_result_enum_type - current ring type

SIDE EFFECTS   None

===========================================================================*/
dsat_result_enum_type ds3g_siolib_get_ds3g_siolib_current_ring_type 
(
  void
)
{
  return ds3g_siolib_current_ring_type;
}

/*===========================================================================
FUNCTION       DS3G_SIOLIB_GET_DS3G_SIOLIB_STATE

DESCRIPTION    This function returns the port state

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
ds3g_siolib_state_info_type* ds3g_siolib_get_ds3g_siolib_state 
(
  ds3g_siolib_port_e_type port_id
)
{ 
  if(( port_id >= DS3G_SIOLIB_PORT_MAX ) || ( port_id < DS3G_SIOLIB_DATA_PORT ))
  {
    DS_SIO_MSG1_ERROR("Invalid SIO port: %d",port_id);
    return  NULL;
  }
  return &ds3g_siolib_state[port_id];
}
#ifdef FEATURE_DATA_MUX
/*===========================================================================
FUNCTION       DS3G_SIOLIB_IS_CLIENT_MUX_PORT

DESCRIPTION    This function checks if the input port is a client side MUX port.

DEPENDENCIES   None.

RETURN VALUE    
  TRUE if the input port is a client side MUX port.
  FALSE otherwise.

SIDE EFFECTS   None

===========================================================================*/
boolean ds3g_siolib_is_client_mux_port
(
  ds3g_siolib_port_e_type port_id
)
{
  boolean result = FALSE;
  
  switch(port_id)
  {
    case DS3G_SIOLIB_DATA_MUX_1_PORT:
    case DS3G_SIOLIB_DATA_MUX_2_PORT:
    case DS3G_SIOLIB_DATA_MUX_3_PORT:
     result = TRUE;
     break;

    default:
     DS_SIO_MSG1_LOW("Not a Client MUX port %d",port_id);
  }

  return result;
}/* ds3g_siolib_is_client_mux_port */
#endif /* FEATURE_DATA_MUX */

static void ds3g_siolib_set_dtr_handling_pending_flag
(
  boolean                 flag,
  ds3g_siolib_port_e_type port_id
)
{
  if (port_id > DS3G_SIOLIB_PORT_MAX ||
      port_id <= DS3G_SIOLIB_PORT_NONE )
  {
    return;
  }

  rex_enter_crit_sect(&ds3g_siolib_state[port_id].port_info_crit_sec);
  ds3g_siolib_state[port_id].dtr_handling_pending_flag = flag;
  rex_leave_crit_sect(&ds3g_siolib_state[port_id].port_info_crit_sec);
}

/*===========================================================================

FUNCTION DS3G_SIOLIB_DTR_HANDLING_PENDING_FLAG_IS_SET

DESCRIPTION
  This function checks whether DTR handling is pending.

DEPENDENCIES
  None

PARAMETERS
  Port Id

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static boolean ds3g_siolib_dtr_handling_pending_flag_is_set
(
  ds3g_siolib_port_e_type port_id
)
{
  boolean retval = FALSE;

  if (port_id > DS3G_SIOLIB_PORT_MAX ||
      port_id <= DS3G_SIOLIB_PORT_NONE )
  {
    return retval;
  }

  rex_enter_crit_sect(&ds3g_siolib_state[port_id].port_info_crit_sec);

  if (ds3g_siolib_state[port_id].dtr_handling_pending_flag)
  {
    retval = TRUE;
  }
  else
  {
    retval = FALSE;
  }
  rex_leave_crit_sect(&ds3g_siolib_state[port_id].port_info_crit_sec);

  return retval;
}

/*===========================================================================

FUNCTION DS3G_SIOLIB_GET_INDEX_FROM_ALLOCATED_PORT

DESCRIPTION
  This functions return an index into the Allocated Port array

DEPENDENCIES
  None

PARAMETERS
  Port Id

RETURN VALUE
  Index into the Allocated port array.
  DS3G_SIOLIB_PORTID_NONE, if port if the not allocated.

SIDE EFFECTS
  None

===========================================================================*/
ds3g_siolib_portid_e_type ds3g_siolib_get_index_from_allocated_port
(
  ds3g_siolib_port_e_type port_id
)
{
  ds3g_siolib_portid_e_type portid;
  for(portid = DS3G_SIOLIB_PORTID_0; portid < DS3G_SIOLIB_PORTID_MAX; portid++)
  {
    DS_SIO_MSG2_MED ("ds3g_siolib_get_index_from_allocated_port: "
                     "ds3g_siolib_port_allocated[portid] %d, %d ", 
                      ds3g_siolib_port_allocated[portid], portid); 

    if(ds3g_siolib_port_allocated[portid] == port_id)
    {
      return portid;
    }
  }

  return DS3G_SIOLIB_PORTID_NONE;
}

/*===========================================================================

FUNCTION DS3G_SIOLIB_GET_INDEX_FROM_LAST_ALLOCATED_PORT

DESCRIPTION
  This functions return an index into the Last Allocated Port array.

DEPENDENCIES
  None

PARAMETERS
  Port Id

RETURN VALUE
  Index into the Last allocated port array.
  DS3G_SIOLIB_PORTID_NONE, if port is not found in
  the last allocated array.

SIDE EFFECTS
  None

===========================================================================*/
ds3g_siolib_portid_e_type ds3g_siolib_get_index_from_last_allocated_port
(
  ds3g_siolib_port_e_type port_id
)
{
 ds3g_siolib_portid_e_type portid;

 for(portid = DS3G_SIOLIB_PORTID_0; portid < DS3G_SIOLIB_PORTID_MAX; portid++)
 {
   if(ds3g_siolib_last_port_allocated[portid] == port_id)
   {      
     return portid;
   }
  }  
  return DS3G_SIOLIB_PORTID_NONE;
}

/*===========================================================================

FUNCTION DS3G_SIOLIB_GET_PORT_ALLOCATED_FROM_INDEX

DESCRIPTION
  This functions return an index into the Allocated Port array

DEPENDENCIES
  None

PARAMETERS
  Index into the Port Allocated Table

RETURN VALUE
  Port Id at the index.

SIDE EFFECTS
  None

===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_get_allocated_port_from_index
(
  ds3g_siolib_portid_e_type port_index
)
{
  if(port_index >= DS3G_SIOLIB_PORTID_MAX ||
     port_index <= DS3G_SIOLIB_PORTID_NONE)
  {
    return DS3G_SIOLIB_PORT_NONE;
  }
  
  DS_SIO_MSG2_MED( "ds3g_siolib_get_index_from_allocated_port: "
                   "ds3g_siolib_port_allocated[port_index] %d port_index %d", 
                   ds3g_siolib_port_allocated[port_index], port_index); 

  return ds3g_siolib_port_allocated[port_index];
}

/*===========================================================================

FUNCTION DS3G_SIOLIB_GET_ACTIVE PORT

DESCRIPTION
  This functions return the Active port

DEPENDENCIES
  None

PARAMETERS
 None

RETURN VALUE
  Active port

SIDE EFFECTS
  None

===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_get_active_port 
(
  void
)
{
  return ds3g_siolib_active_port;
}

/*===========================================================================

FUNCTION DS3G_SIOLIBI_RX_SIG_CRIT_SECT_ENTER

DESCRIPTION
  This functions enters ds3g_siolib_rx_sig_crit_sec critical section

DEPENDENCIES
  None

PARAMETERS
 None

RETURN VALUE
  Active port

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_siolibi_rx_sig_crit_sect_enter
(
  void 
)
{
  DS3G_SIOLIB_RX_SIG_CRIT_SECT_ENTER();
}

/*===========================================================================

FUNCTION DS3G_SIOLIBI_RX_SIG_CRIT_SECT_LEAVE

DESCRIPTION
  This functions leaves ds3g_siolib_rx_sig_crit_sec critical section

DEPENDENCIES
  None

PARAMETERS
 None

RETURN VALUE
  Active port

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_siolibi_rx_sig_crit_sect_leave
(
  void 
)
{
  DS3G_SIOLIB_RX_SIG_CRIT_SECT_LEAVE();
}


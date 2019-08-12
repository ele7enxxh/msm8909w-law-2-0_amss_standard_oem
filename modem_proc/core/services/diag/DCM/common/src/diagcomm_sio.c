/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostic Communications via SIO

General Description
  The routines in this file handle the communications layer for the
  diagnostic subsystem when communicating via SIO.

Copyright (c) 2001-2014 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                               Edit History

 $PVCSPath:  L:/src/asw/MSM5200/common/vcs/diagcomm_sio.c_v   1.10   08 May 2002 14:37:42   ldefauw  $
 $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diagcomm_sio.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/25/14   ph      Mainlined the feature DIAG_CONSUMER_API.
10/30/14   xy      Fixed issue that modem not able to send mask centralization 
                   control packets to apps  
10/09/14   ph      Added control packet to send the memory size used in
                   peripheral buffering.
07/25/14   sa      Added support to send an event to debug diag drops
07/06/14   ph      Fixed KW errors.
05/29/14   ph      Fixed compiler warning.
04/18/14   ph      Resolve delayed response before immediate response issue.
04/01/14   rs      Added support for build mask centralization.
03/20/14   is      Resolve delayed responses out of order issue
03/05/14   rh      Fixed streaming bug resulting from buffering behavior fix.
01/30/14   ph      Removed the usage of macro DIAG_TASK_HANDLE().
01/01/14   ph      Fixed inconsistent drain behavior in circular and threshold buffering.
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
11/01/13   is      Resolve buffering mode issues
09/25/13   xy      Handle sending out feature mask on control channel first before 
                   sending out cmd reg tables in diag_handle_internal_sigs()   
09/13/13   ph      Updated drain logic for TX packets of size greater than 4k
                   to send the current dsm item and then start the chain.
08/31/13   is      Support for peripheral buffering mode 
04/30/13   sr      Removed SIO_RX pool. Cmd req/rsp will now use CMD_SIO_TX 
                   and CMD_SIO_RX pool even in backwards compatibility case   
01/03/13   sg      Command Response Channel separation
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush.
11/28/12   sr      Parameterized diagcomm_status() to pass in port_id
11/15/12   is      Support for preset masks
11/08/12   sr      Support for DCI Override feature(OCH) and 
                   On Board Diagnostics(OBD)- Secure Sim feature  
10/11/12   rh      Moved defines for TX/RX Max Chain Size to diagi_v.h
08/31/12   ra      Use accessor macros instead of accessing private task         
                   info directly; accomodated changes for User space
                   task priority changes  
05/03/12   sa      Mainlining DIAG_CENTRAL_ROUTING
03/04/12   sg      set the signal to forward the regsitration tables to the apps
                   proc only when the ctrl channel is opened on both the sides
10/14/11   hm      Diag consumer API - modem side changes 
10/07/11   is      Support sending log mask per equipment id
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
04/28/11   mad     Fixed featurization for diag_fwd_task access
04/18/11   is      Ensure CTRL msg sizes are constant
03/29/11   sg      Central Routing changes for Q6
03/25/11   is      Fix compilation errors for modem-only configuration
03/04/11   is      Support for Diag over SMD-Lite
02/28/11   sg      Power saving changes 
12/20/10   mad     Changes for dual mask
12/16/10   vs      Open SDIO port based on feature
10/20/10   is      Bypass RDM to set default to UART1 for 8960 Apps only bringup  
10/14/10   is      Featurize diagcomm_send_dtr_msg() for multi-proc only
10/05/10   mad     Register for DTR only if port open is successful
09/28/10   mad     Included diaglogi.h, eventi.h to fix build error 
09/14/10   is      New control channel and central routing features
08/10/10   sg      Close SMD Port when the SIO port on apps is closed
09/13/10   is      Add flow control count and DSM chaining count to Diag Health
09/01/10   sg      Call sio_transmit() with smd stream id on Q6 to put the 
                   tx traffic into diagcomm_smd_tx_wmq.
08/06/10   sg      Featurized diagcomm_sio_close_done_cb(),diagcomm_sio_close_cb
                   to fix compiler warnings
07/23/10   mad     Turning on command-filtering before sio_open() call, for 
                   on-device diag port 
07/21/10   sg      Added chaining support for rx response packets
07/15/10   sg      Cleaning up the setting and resetting of drain timer in
                   diag_dtr_changed_cb()
06/21/10   vs      Added ASSERT to catch dropping of partially filled DSM item.
05/10/10   sg      mainlined setting and clearing of drain timer
06/07/10   is      Removed support for FEATURE_DIAG_MULTI_PORTS, FEATURE_SIO_USB_ETHR
                   FEATURE_SIO_USB_COMPOSITE
05/25/10   JV      New task on master to process slave traffic
05/06/10   as      De-coupled Diag and ondevice modules. Mainlined the code for 
                   retry count when the open port fails. Fixed the assign port
                   logic when Diag port is configured to ondevice.
05/03/10   sg      fixing corner case where we can expect triple chaining 
                   with 6k logs
04/29/10   is      Removed support for DEBUG_TX_EFS
04/20/10   sg      Set Event Drian Signal After Flow control is turned off
03/25/10   mad     Support for on-device logging for WM7 devices
03/15/10   vs      Fixed unchecked pointer access in diagcomm_outbound_alloc
03/03/10   as      Added support RDM assign port needed for Ondevice retry.
01/04/10   vs      Modifications to support diagbuf_send_pkt_single_proc
12/30/09   sg      Fixed compiler warning
12/28/09   ps      Fixed Klocwork errors & compiler warnings.
12/10/09   sg      Added flow control mechanism for DSM_DIAG_SIO_TX_ITEM_POOL
                   at item level granularity
11/30/09   ps      Fixed compiler warnings.
11/05/09   vs      Mainlined functions diagbuf_outbound_alloc and diagbuf_send
10/29/09   JV      Pass the DSM pool ID to SIO-USB
09/11/09   sg      Made changes to have dedicated pools in each Tx and RX direction
                   for single processor targets
08/11/09   mad     Signal Diag with DIAG_TX_SLAVE_SIG, when DTR changes
08/06/09   sg      Added support for DSM chaining in diagcomm_sio_inbound_pkt_notify(),
                   diagcomm_inbound()
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
08/04/09   JV      Diag does not vote for/against sleep based on dtr change events.
07/31/09   JV      Removed inclusion of task.h. Removed warnings.
07/31/09   JV      Merged Q6 diag code back to mainline
06/23/09   JV      Introduced the OS abstraction layer
05/07/09   vk      changed includes from external to internal headers
04/21/09   ps	   branch collapse of 6k and 7k DIAG
11/06/08   mad     modified diagcomm_status() to return only the port 
                   open/close status. diagcomm_status() is not affected by usb 
                   cable detach anymore.
10/03/08   vg      Updated code to use PACK() vs. PACKED
1/28/08    as      Fixed tail char setup for single and modem only builds.
12/16/07   pc      Added support for diag_dtr_enable and diag_dtr_changed_cb.
05/17/07   jc      Proxy tv.  FEATURE_SIO_USB_COMPOSITE
12/15/06   as      Fixed compiler warnings.
10/31/05   as      Fixed lint errors.
09/09/05   sl      Added support for multiple tx/rx queues for the case of
                   FEATURE_DIAG_MULTI_PORTS
05/15/05   as      Added support for FEATURE_DATA_SERIALIZER
03/28/05   sl      Added an array of watermark queues for the multi port feature
03/24/05   as      Added check in UART/USB notify functions to check if the
                   connected accessory detected by AAM is UART and USB
03/05/05   sl      Corrected BT Featurization for FEATURE_DIAG_MULTI_PORTS
02/20/05   sl      Added support for DIAG over BT via RDM under
                   FEATURE_DIAG_MULTI_PORTS
02/12/05   sl      Fixed a bug that caused the close/open port bit masks
                   not being cleared
02/09/05   abu     Added support for Diag over IrDA under FEATURE_IRDA flag  
01/25/04   sl      Added support for callback notifications from Accessory
                   Attachment Manager (AAM) for USB and UART cable attach and
                   detach under FEATURE_ACCESSORY_ATTACHMENT_MANAGER.
12/14/04   sl      Added support for DIAG to hold multiple SIO ports
                   controlled by FEATURE_DIAG_MULTI_PORTS
04/20/04   pn      Added support for DIAG over Bluetooth.
10/28/03   as      Changes to fix errors when compiling with RVCT2.0
09/29/03   dna     Get T_QUASAR to compile and work again for MSM6500
08/14/03   vsk     DSM 2.0 changes 
04/10/03    wx     Move qcdmg and to_data_mode functions from RDM to here.
05/08/02   lad     Moved dsm_simple_enqueue_isr() call above diagcomm_sio_cb
                   to enqueue data prior to potential context switch in cb.
02/27/02   dwp     Additional changes to use new DSM interface functions for
                   registering/handling memory events.
02/19/02   lad     Made changes to use new DSM interface functions for
                   registering/handling memory events.
01/21/02   as      Changed nv_sio_baud_type to nv_sio_baudrate_type
11/06/01   abp     Added diagcomm_flush_tx() to flush SIO transmit channel
10/30/01   abp     Added diagcomm_sio_close_to_open_cb().
                   In diagcomm_sio_close_done_cb() callback function ptr is
                   checked for NULL.
                   In diagcomm_open() added mechanism to close connection
                   before opening a new connection
10/09/01   ttl     Added second parameter to diagcomm_sio_tx_start_cb() and
                   diagcomm_sio_tx_stop_cb() due the DSM memory pool registration
                   changes.
09/17/01   jal     Fixed diagcomm_close() to notify RDM of completion
                   properly
05/07/01   lad     Cleaned up file.
04/17/01   dwp     Initialize diagcomm_sio_open_params.port_id to NULL. RDM
                   will instruct DIAG as to what port to open.
04/06/01   lad     Introduced types for callback function pointers.
                   Cleaned up FEATURE_RUNTIME_DEVMAP support.
                   Updated watermark settings (this may be changed to save RAM).
02/23/01   lad     Created file.

===========================================================================*/

#include "core_variation.h"
#include "comdef.h"

#include "diag_v.h"
#include "diagbuf_v.h"
#include "diagcmd.h"
#include "diagcomm_v.h"
#include "diagi_v.h"
#include "diagpkt.h"
#include "msg.h"
#include "msgi.h" /* For msg_set_all_rt_masks_adv () */
#include "diagdsmi.h"
#include "diagtune.h"
#include "diagdiag_v.h"
#include "diag_cfg.h"
#include "diag_mode_v.h" /* For diag_buffering_pool_used_cnt() */
#include "diagcomm_smd.h"
#include "diagcomm_ctrl.h"
#include "diagcomm_io.h" /* For diagcomm_port_type */
#include "diagdsm_v.h"
#include "diag_fwd_v.h"
#include "diaglogi.h" /* for log_config_mask */
#include "event.h"
#include "eventi.h" /* for event_set_all_rt_masks */

#include "dsm.h"

#if defined (FEATURE_DIAG_NV)
#include "diagnv_v.h"
#include "nv.h"
#endif

#if ((defined(DIAG_RUNTIME_DEVMAP))||(defined(FEATURE_DIAG_SWITCH)&&(!defined(FEATURE_WINCE))))
  #include "rdevmap.h"
#endif

#ifdef FEATURE_WINCE
#error code not present
#endif /* FEATURE_WINCE */

#ifdef DIAG_SIO_SUPPORT
#include "diagcomm_io_sio.h"
#endif /* DIAG_SIO_SUPPORT */

#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "assert.h"
#endif

/*lint -save -e751 */
/* Supress local typedef 'diag_config_comm_enum_type' not referenced */


/* -------------------------------------------------------------------------
** Definitions and Declarataions
** ------------------------------------------------------------------------- */
extern diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */

extern diag_tx_mode_type diag_tx_mode[DIAG_MAX_STREAM_ID];

extern uint8 diag_per_count_dropped;  /* Percentage Drop Count */

/* Variables to store drop count for DIAG Health */
extern uint32 drop_count_log;
extern uint32 drop_count_f3;

/* Variables to store allocation requests for DIAG Health */
extern uint32 alloc_count_log;
extern uint32 alloc_count_f3;
extern void diagcomm_buffer_enqueue_stream( dsm_item_type ** item_ptr,
                                            diagcomm_port_type port_type,
                                            uint8 stream_id ); /* From diagcomm_smd.c */

#ifdef FEATURE_DIAG_SWITCH
  extern int diag_on_single_proc;
#endif

/* Diag Health counter - Keeps track of # of times SIO wmq flow ctrl is triggered.*/
static uint32 diagcomm_wmq_flow_ctrl_count[NUM_SIO_PORTS] = {0, 0};

/* Diag Health counter - Keeps track of # of times DSM flow ctrl is triggered. */
uint32 diagcomm_flow_ctrl_count[NUM_SMD_PORT_TYPES] = {0, 0};

boolean diagcomm_apps_only_config = FALSE;

/* Queues around which the watermark queues are built */
static q_type diagcomm_sio_tx_q[NUM_SIO_PORTS];
static q_type diagcomm_sio_buffering_tx_q[NUM_SIO_PORTS];

/* Watermark queues used for communications with SIO */
dsm_watermark_type diagcomm_sio_tx_wmq[NUM_SIO_PORTS];

/*Intermediate Watermark queue used for buffering TX data */
dsm_watermark_type diagcomm_sio_buffering_tx_wmq[NUM_SIO_PORTS];

extern dsm_watermark_type diagcomm_cmd_tx_wmq[NUM_SMD_PORTS]; /*From diagcomm_cmd.c*/
extern dsm_watermark_type diagcomm_cmd_rx_wmq[NUM_SMD_PORTS]; /*From diagcomm_cmd.c*/

#ifdef FEATURE_WINCE
#error code not present
#endif

/* To avoid extra copies between layers in the embedded system, we pass
   pointers from SIO and wait for the caller to release them. */
static dsm_item_type *diagcomm_sio_inbound_item_ptr = NULL;
static dsm_item_type *diagcomm_sio_outbound_item_ptr[NUM_SIO_PORTS] = {NULL, NULL};

/* Configuration information for the diagnostic communications layer.
*/
static struct
{
  void (*inbound_cb) (void);

  /* !!! Note: if using a system with separate threads
     ** for RX and TX, these should be NULL.
   */
  void (*tx_start_cb) (void);
  void (*tx_stop_cb) (void);
}
diagcomm_params;

static boolean allowflow = TRUE; /* Keep track of state of flow control. */

static void diagcomm_sio_dsm_low_mem_event_cb (dsm_mempool_id_type,
                                                  dsm_mem_level_enum_type,
                                                  dsm_mem_op_enum_type);

static void diagcomm_sio_dsm_high_mem_event_cb (dsm_mempool_id_type pool_id,
                                                   dsm_mem_level_enum_type mem_level,
                                                   dsm_mem_op_enum_type mem_op);

static boolean bDiagAllowTxWmqFlow[NUM_SMD_PORTS] = {TRUE, TRUE}; /* Keep track of state of WMQ flow control. */
static void diagcomm_buffering_wmq_low_mem_event_cb( dsm_watermark_type *wm_ptr, void * data );
static void diagcomm_buffering_wmq_high_mem_event_cb( dsm_watermark_type *wm_ptr, void * data );

#if defined (DIAG_RUNTIME_DEVMAP)|| defined (FEATURE_WINCE)|| defined (FEATURE_DIAG_SWITCH)
static uint32 diagcomm_retry_count = 0;
#endif 

#define DIAGCOMM_RETRY_MAX_CNT 5

extern diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];


/*===========================================================================

FUNCTION DIAGCOMM_GET_SIO_CONN

DESCRIPTION

===========================================================================*/
diagcomm_io_conn_type * diagcomm_get_sio_conn( diagcomm_enum_port_type port_num )
{
#if defined(DIAG_MP_MASTER) || !defined(DIAG_MP)
  return &diagcomm_io_conn[DIAGCOMM_PORT_SIO][port_num];
#else
if( DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].feature_mask, F_DIAG_REQ_RSP_CHANNEL) )
  return &diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][port_num];
else 
return &diagcomm_io_conn[DIAGCOMM_PORT_SIO][port_num];
#endif

} /* diagcomm_get_sio_conn */


/*===========================================================================

FUNCTION DIAGCOMM_SIO_INBOUND_PKT_NOTIFY

DESCRIPTION
  This is the callback SIO calls when data is received.

  This function enqueues the data onto the diag's watermark.  Further, if the
  last char in the input dsm_item_type is equal to DIAG_ASYNC_CONTROL_CHAR,
  the inbound callback is called.

===========================================================================*/
void
diagcomm_sio_inbound_pkt_notify (dsm_item_type ** item_ptr)
{ 
  dsm_enqueue (&diagcomm_cmd_rx_wmq[DIAGCOMM_PORT_1], item_ptr);

  /* Call the inbound callback to tell the client
     that we have inbound data available. */
  if (diagcomm_params.inbound_cb)
  {
    diagcomm_params.inbound_cb ();
  }
} /* diagcomm_sio_inbound_notify */

/*===========================================================================

FUNCTION DIAGCOMM_SIO2_INBOUND_PKT_NOTIFY

DESCRIPTION
  This is the callback SIO calls when data is received.

  This function enqueues the data onto the diag's watermark.  Further, if the
  last char in the input dsm_item_type is equal to DIAG_ASYNC_CONTROL_CHAR,
  the inbound callback is called.

===========================================================================*/
void
diagcomm_sio2_inbound_pkt_notify (dsm_item_type ** item_ptr)
{
  dsm_enqueue (&diagcomm_cmd_rx_wmq[DIAGCOMM_PORT_2], item_ptr);

  /* Notify diag that there is inbound data available to be read */
  diag_rx_dci_notify();

} /* diagcomm_sio_inbound_notify */


/*===========================================================================

FUNCTION DIAGCOMM_INBOUND_PKT_NOTIFY

DESCRIPTION
  This is the callback SIO calls when data is received.

  This function enqueues the data onto the diag's watermark.  Further, if the
  last char in the input dsm_item_type is equal to DIAG_ASYNC_CONTROL_CHAR,
  the inbound callback is called.

===========================================================================*/
void
diagcomm_inbound_pkt_notify (dsm_item_type ** item_ptr)
{
  dsm_enqueue (&diagcomm_cmd_rx_wmq[DIAGCOMM_PORT_1], item_ptr);

  /* Call the inbound callback to tell the client
     that we have inbound data available. */
  if (diagcomm_params.inbound_cb)
  {
    diagcomm_params.inbound_cb ();
  }
  diagcomm_apps_only_config = TRUE;
  
} /* diagcomm_inbound_pkt_notify */



#if defined (DIAG_RUNTIME_DEVMAP)|| defined (FEATURE_WINCE)|| defined (FEATURE_DIAG_SWITCH)
/*===========================================================================

FUNCTION DIAGCOMM_SIO_CLOSE_CB

DESCRIPTION
  This callback is called when RDM wants diag to close its port.

===========================================================================*/
void
diagcomm_sio_close_cb ()
{
  osal_sigs_t return_sigs; 
  int return_val = 0;

  diagcomm_retry_count = 0;

  return_val = osal_set_sigs(diag_task_tcb, DIAG_COMM_CLOSE_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);
}
#endif  /* defined (DIAG_RUNTIME_DEVMAP)|| defined (FEATURE_WINCE)|| defined (FEATURE_DIAG_SWITCH)*/
/*===========================================================================

FUNCTION DIAGCOMM_SIO_CLOSE_DONE_CB

DESCRIPTION
  This callback is called when sio_close is completed.

===========================================================================*/
static void
diagcomm_sio_close_done_cb (void)
{
  diag_release ();
}

/* -------------------------------------------------------------------------
** Externalized routines (declared in diagcomm.h).
** ------------------------------------------------------------------------- */
/*===========================================================================

FUNCTION DIAGCOMM_REGISTER_INBOUND

DESCRIPTION
  This function configures the diagnostic subsystem's communcations layer.

===========================================================================*/
void
diagcomm_register_inbound (void (*inbound_pkt_cb) (void))
{
  diagcomm_params.inbound_cb = inbound_pkt_cb;
}               /* diagcomm_register_inbound */


/*===========================================================================

FUNCTION DIAGCOMM_SIO_OPEN

DESCRIPTION
  Opens the communcications stream.  Returns boolean indicating success.

===========================================================================*/
boolean
diagcomm_sio_open (diagcomm_enum_port_type port_num)
{
  ASSERT(DIAGCOMM_PORT_1 == port_num || DIAGCOMM_PORT_2 == port_num);

  diagcomm_io_conn_type * conn = &diagcomm_io_conn[DIAGCOMM_PORT_SIO][port_num];
  diagpkt_filtered_cmd_set = FALSE;

  if(DIAGCOMM_PORT_1 == port_num)
  {
    #if defined(FEATURE_DIAG_NV) && defined(DIAG_DEFAULT_BITRATE_NV)
        nv_sio_baudrate_type nv_default_baud =NV_SIO_BAUDRATE_AUTO;
        nv_stat_enum_type nv_status;
    #endif /* FEATURE_DIAG_NV && DIAG_DEFAULT_BITRATE_NV */

    if (conn->connected)
    {
      #if defined (DIAG_RUNTIME_DEVMAP) || defined (FEATURE_WINCE)
        /* As requested, we will now close. */
        diagcomm_io_close( conn );
      #else
        #ifdef FEATURE_DIAG_SWITCH
        if(diag_on_single_proc == DIAG_FTM_SWITCH_VAL)
        {
          /* As requested, we will now close. */
          diagcomm_io_close( conn );
        }
        #endif /* FEATURE_DIAG_SWITCH */
      #endif /* DIAG_RUNTIME_DEVMAP || FEATURE_WINCE */
  
      /* success or not, consider close operation successful */
      conn->connected = FALSE;
    }
  
  #if defined(FEATURE_DIAG_NV) && defined(DIAG_DEFAULT_BITRATE_NV)
    nv_status = diag_nv_read (NV_DIAG_DEFAULT_BAUDRATE_I,
                  (nv_item_type *) & nv_default_baud);
  
    switch (nv_status)
    {
      case NV_DONE_S:
        conn->open_params.sio_params.params.rx_bitrate = (sio_bitrate_type) nv_default_baud;
        conn->open_params.sio_params.params.tx_bitrate = (sio_bitrate_type) nv_default_baud;
        break;
  
      case NV_NOTACTIVE_S:
        nv_default_baud = NV_SIO_BAUDRATE_115200;
        (void) diag_nv_write (NV_DIAG_DEFAULT_BAUDRATE_I, &nv_default_baud);
        break;
  
      default:
        MSG (MSG_SSID_DIAG, MSG_LEGACY_MED, "Failed to read sio bitrate");
        break;
    } /* end switch */
  #endif /* FEATURE_DIAG_NV && DIAG_DEFAULT_BITRATE_NV */
  
    diagcomm_io_set_port( conn );

  #if defined FEATURE_WINCE
    #error code not present
#endif
  
    if( !conn->connected )
    {    
      if( diagcomm_io_open(conn) == TRUE )
      {
        conn->connected = TRUE;
      }
      else
      {
        diagpkt_filtered_cmd_set = FALSE;
        MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"diagcomm_sio_open (diagcomm_port_1) - Couldn't get stream.");
      }
    }
  
  #if defined (DIAG_RUNTIME_DEVMAP)
      if (conn->connected)
      {
        rdm_notify (RDM_DIAG_SRVC, RDM_DONE_S);
      }
      else
      {
        rdm_notify (RDM_DIAG_SRVC, RDM_NOT_ALLOWED_S);
        if (diagcomm_retry_count < DIAGCOMM_RETRY_MAX_CNT)
        {
          diagcomm_retry_count++;
          diag_suspend (DIAG_TOUT_TIME); 
  
          rdm_assign_port ( RDM_DIAG_SRVC,
                            RDM_ONDEVICE_DIAG_DEV,
                            NULL );
        }
      }
  #endif /* DIAG_RUNTIME_DEVMAP */
  
  #ifdef FEATURE_WINCE
    #error code not present
#endif
  
    diag_io_detect_connection( conn );
  
    if( diagcomm_is_ondevice_mode(conn) )
    { 
        diag_control_sleep_voting(0);
        diag_allow_sleep();
    }
  }

  else if (DIAGCOMM_PORT_2 == port_num)
  {
   conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG2;

    if( !conn->connected )
    {    
      if( diagcomm_io_open(conn) == TRUE )
      {
        conn->connected = TRUE;
      }
      else
      {
        MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,
            "diagcomm_sio_open (diagcomm_port_2) - Couldn't get stream.");
      }
    }
  }

  return conn->connected;

} /* diagcomm_sio_open */


/*===========================================================================

FUNCTION DIAGCOMM_SIO_CONN_CHANGED

DESCRIPTION

PARAMETERS
  conn - Diag I/O connection handle
  connected - True: SIO data channel has just changed to connected
              False: SIO data channel has just changed to dis-connected
  
RETURN VALUE
  None
  
===========================================================================*/
void diagcomm_sio_conn_changed( diagcomm_io_conn_type * conn, boolean connected )
{
  osal_sigs_t return_sigs;
  int return_val = 0;
  
  ASSERT( conn !=NULL );
  
  if (!connected)
  {
    conn->channel_connected = FALSE;

#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
    return_val = osal_reset_sigs(&diag_fwd_task_tcb, DIAG_TX_SLAVE_SIG, &return_sigs);
    ASSERT(OSAL_SUCCESS == return_val);
#endif

     return_val = osal_set_sigs(diag_task_tcb, DIAG_COMM_SIO_TX_FLUSH_SIG , &return_sigs);
     ASSERT(OSAL_SUCCESS == return_val);
  }
  else
  {
    conn->channel_connected = TRUE;

 #if defined (DIAG_MP) && defined (DIAG_MP_MASTER) 
    if(diagcomm_status(DIAGCOMM_PORT_1))
    {
      return_val = osal_set_sigs(&diag_fwd_task_tcb, DIAG_TX_SLAVE_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
    }
#endif
  }
} /* diagcomm_sio_conn_changed */


/*===========================================================================

FUNCTION DIAGCOMM_SIO_CTRL_CONN_CHANGED

DESCRIPTION
  Set the status of channel connected to TRUE when the ctrl port is opened on the 
  remote side and set signal to forward the registration tables to apps.
  
PARAMETERS
  conn - Diag I/O connection handle
  connected - True: SIO control channel has just changed to connected
              False: SIO control channel has just changed to dis-connected
  
RETURN VALUE
  None
  
===========================================================================*/
void diagcomm_sio_ctrl_conn_changed( diagcomm_io_conn_type * conn, boolean connected )
{
  ASSERT( conn !=NULL );
  
  if (!connected)
  {
    conn->channel_connected = FALSE;
  }
  else
  {
    conn->channel_connected = TRUE;
   
#if defined (DIAG_MP) && !defined (DIAG_MP_MASTER)
    /* Check if connection to Master proc is established first */
    if( diagcomm_ctrl_status(DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1) && 
        diagcomm_ctrl_channel_status(DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1) )
    {
        /*set feature mask signal first*/
       /* Set signal to send Diag feature flag mask to remote procs */
      (void)diag_set_internal_sigs( DIAG_INT_FEAT_UPDATE_SIG );

      /* Set signal to notify Diag that there's reg tables to be fwded */
      (void)diag_set_internal_sigs( DIAG_INT_CTRL_REG_SIG );
        
      /* Set signal to send Diag build time masks to remote procs */
      /*move it to the place where modem receives feature mask from apps*/
      //(void)diag_set_internal_sigs( DIAG_INT_SEND_BT_MASK_SIG );
      
      /*Set signal to send peripheral buffering pool size */
      (void)diag_set_internal_sigs(DIAG_INT_SEND_PERIPH_BUF_SIZE_SIG);
     
    }
#endif /* DIAG_MP && !DIAG_MP_MASTER */
  }
  
} /* diagcomm_sio_ctrl_conn_changed */


/*===========================================================================

FUNCTION DIAGCOMM_CLOSE

DESCRIPTION
  Closes the current connection, if any.

===========================================================================*/
void
diagcomm_close ( void )
{
  diagcomm_io_conn_type * conn = &diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1];
  
  if (conn->connected)
  {
    /* Flush the diagnostic subsystem's output buffering system. */
    diagbuf_flush_timeout(DIAG_SIO_TIMEOUT_TIMER_LEN);

#if defined (DIAG_RUNTIME_DEVMAP) || defined (FEATURE_WINCE)
    conn->connected = FALSE;
    diagcomm_io_close( conn );
#endif /* DIAG_RUNTIME_DEVMAP || FEATURE_WINCE */
    
    diagpkt_filtered_cmd_set = FALSE;  /* Reset filtered set */

    diag_control_sleep_voting(1);
#if defined (DIAG_RUNTIME_DEVMAP)
    rdm_notify (RDM_DIAG_SRVC, RDM_DONE_S);
#endif  /* DIAG_RUNTIME_DEVMAP */

    /* success or not, consider close operation successful */
    conn->connected = FALSE;
  }
  
#ifdef FEATURE_WINCE
  #error code not present
#endif /* FEATURE_WINCE */

} /* diagcomm_close */


/*===========================================================================

FUNCTION DIAGCOMM_CLOSE_DCI

DESCRIPTION
  Closes the current DCI connection, if any.

===========================================================================*/
void
diagcomm_close_dci (void)
{
 
  diagcomm_io_conn_type * conn = &diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2];

 /*Turn off all the masks for stream 2*/
  msg_set_all_rt_masks_adv (0, DIAG_STREAM_2, DIAG_DCI_MASK_1);

  if (conn->connected)
  {
    conn->connected = FALSE;
    diagcomm_io_close( conn );
    conn->connected = FALSE;
  }
  conn = &diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2];
   if (conn->connected)
  {
    conn->connected = FALSE;
    diagcomm_io_close( conn );
    conn->connected = FALSE;
  }

} /* diagcomm_close_dci */


/*===========================================================================

 FUNCTION diagcomm_sio_empty_wm_queue (internal)

 DESCRIPTION
   This function empties the Tx watermark queue.
   
 PARAMETERS
   port_num - port num to empty Tx wmq for.
   
 RETURN VALUE
   None

===========================================================================*/
void diagcomm_sio_empty_tx_wm_queue( diagcomm_enum_port_type port_num )
{
  if( port_num <= DIAGCOMM_PORT_NUM_LAST )
    dsm_empty_queue(&diagcomm_sio_tx_wmq[port_num]);
  else
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diagcomm_sio_empty_tx_wm_queue - Invalid port_num %d", port_num);
	
} /* diagcomm_sio_empty_wm_queue */

/*===========================================================================

 FUNCTION diagcomm_sio_tx_flush

 DESCRIPTION
   This function flushes outbound water mark queue and turns off all the masks
   to not generate any more traffic

===========================================================================*/
void diagcomm_sio_tx_flush( void )
{
 /*Turn off all message,log,event masks  */
  msg_set_all_rt_masks_adv (0, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL);
  log_config_mask(-1, FALSE, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL);
  log_config_mstr_mask(-1, FALSE, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL);
  event_set_all_rt_masks(0, 0);

  /* Clear the diag connected status */
  DIAG_CLR_MASK (diag_cx_state, DIAG_CX_FLUSH_S | DIAG_CX_COMM_S);

  /* Empty the water mark queue if there are any items pending since smd is not going to free
     them as the port is closed */
  dsm_empty_queue(&diagcomm_sio_tx_wmq[DIAGCOMM_PORT_1]);
  dsm_empty_queue(&diagcomm_cmd_tx_wmq[DIAGCOMM_PORT_1]);
  
} /* diagcomm_sio_tx_flush */


/*===========================================================================

 FUNCTION DIAGCOMM_FLUSH_RX

 DESCRIPTION
   Flushes inbound queue.  This is used when a security timeout occurs,
   though the capability is generic.

===========================================================================*/
void
diagcomm_flush_rx (void (*flush_cb) (void))
{
  dsm_empty_queue (&diagcomm_cmd_rx_wmq[DIAGCOMM_PORT_1]);

  if (flush_cb)
  {
    flush_cb ();
  }
} /* diagcomm_flush_rx */

/*===========================================================================

 FUNCTION DIAGCOMM_FLUSH_TX_CB

 DESCRIPTION
   Called by SIO to ack the successful completion of a TX flush.

===========================================================================*/
static void
diagcomm_flush_tx_cb (void)
{
  diag_release ();
  
} /* diagcomm_flush_tx_cb */

/*===========================================================================

 FUNCTION DIAGCOMM_FLUSH_TX

 DESCRIPTION
   Flushes the transmit channel on current connection.

===========================================================================*/
void
diagcomm_flush_tx (void (*flush_cb) (void), unsigned int timeoutVal)
{
  if (diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].connected)
  {
    diagcomm_io_flush_tx( &diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1], diagcomm_flush_tx_cb,timeoutVal );
  }

  if (flush_cb)
  {
    flush_cb ();
  }

} /* diagcomm_flush_tx */

/*===========================================================================

FUNCTION DIAGCOMM_STATUS

DESCRIPTION
    This function returns TRUE if the port passed in is open.

===========================================================================*/
boolean
diagcomm_status (diagcomm_enum_port_type port)
{
  return diagcomm_io_conn[DIAGCOMM_PORT_SIO][port].connected;
}


/*===========================================================================

FUNCTION DIAGCOMM_INBOUND

DESCRIPTION
  This function returns a pointer to some inbound data.  If no inbound
  data is available, NULL is returned.

===========================================================================*/
void *
diagcomm_inbound (unsigned int *stream_len, diagcomm_enum_port_type port_num)
{
  void *return_ptr = NULL;

  ASSERT (stream_len);
  ASSERT (port_num == DIAGCOMM_PORT_1 || port_num == DIAGCOMM_PORT_2);

  if (diagcomm_sio_inbound_item_ptr != NULL)
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Attempt to get more than one inbound buffer.");

    *stream_len = 0;
  }
  else
  {
    diagcomm_sio_inbound_item_ptr = diagcomm_io_dequeue_rx_wmq( diagcomm_get_sio_conn(port_num) );

    if (diagcomm_sio_inbound_item_ptr != NULL)
    {
      ASSERT (diagcomm_sio_inbound_item_ptr->data_ptr != NULL);

      *stream_len = diagcomm_sio_inbound_item_ptr->used;

      return_ptr = diagcomm_sio_inbound_item_ptr;
      diagcomm_sio_inbound_item_ptr = NULL;
    }
  }

  return return_ptr;
  
} /* diagcomm_inbound */

/*===========================================================================

FUNCTION DIAGCOMM_INBOUND_COMPLETE

DESCRIPTION
  This function tells the diag communications layer that the caller is done
  with the buffer returned by diagcomm_send() may now be released.

===========================================================================*/
void                                                      
diagcomm_inbound_complete (void *ptr)
{
  ASSERT (ptr != NULL);

  if (diagcomm_sio_inbound_item_ptr != NULL &&
      ptr == diagcomm_sio_inbound_item_ptr->data_ptr)
  {
    (void) dsm_free_packet (&diagcomm_sio_inbound_item_ptr);
    diagcomm_sio_inbound_item_ptr = NULL;
  }
} /* diagcomm_inbound_complete */


/*===========================================================================

FUNCTION DIAGCOMM_GET_RX_WMQ_CNT

DESCRIPTION
  This function returns the rx watermark queue count.

===========================================================================*/
uint32 diagcomm_get_rx_wmq_cnt(diagcomm_enum_port_type port_num)
{
  ASSERT(port_num == DIAGCOMM_PORT_1 || port_num == DIAGCOMM_PORT_2);
  return (*(diagcomm_cmd_rx_wmq[port_num].q_ptr)).cnt;
  
} /* diagcomm_get_rx_wmq_cnt */


/*===========================================================================

FUNCTION DIAGCOMM_CONNECT_STATE

DESCRIPTION

===========================================================================*/
boolean diagcomm_connect_state()
{
  return diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].channel_connected;
}

/*===========================================================================

FUNCTION DIAGCOMM_SIO_TX_DSM_LEVEL_REG (internal)

DESCRIPTION
  This function regsiteres and de-registers DSM pool level callbacks per
  stream/port id.
  
PARAMETERS
  port_num - port id to register/re-register
  reg_level - TRUE, register level callbacks.
              FALSE, de-register level callbacks.
  
RETURN VALUE
  NONE
  
DEPENDENCIES
  Update info in diag_tx_mode[] first before calling this function.
  
===========================================================================*/
void diagcomm_sio_tx_dsm_level_reg( uint8 port_num, boolean reg_level )
{
  uint8 stream_index;
  
  if( port_num > DIAGCOMM_PORT_NUM_LAST )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diagcomm_sio_tx_dsm_level_reg - invalid port_num %d", port_num);
    return;
  }
  
  stream_index = DIAG_PORT_TO_INDEX(port_num);
  
  //ToDo: Add dsm wm cb for stream 2 as well
  if( DIAG_INDEX_TO_ID(stream_index) != DIAG_STREAM_1 )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, 
          "diagcomm_sio_tx_dsm_level_reg - Only registering dsm wm callbacks for stream 1, not stream %d", 
          DIAG_INDEX_TO_ID(stream_index));
    return;
  }
  
  if( reg_level == TRUE )
  {
    /* Set the DSM flow-control parameters */
    dsm_reg_mem_event_level
    (
      diag_tx_mode[stream_index].pool_info.pool_id,
      DSM_MEM_LEVEL_MANY, 
      diag_tx_mode[stream_index].pool_info.item_cnt-diag_tx_mode[stream_index].many_flow_ctrl_dsm_mark /* DSMS avail */
    );

    dsm_reg_mem_event_level
    (
      diag_tx_mode[stream_index].pool_info.pool_id, 
      DSM_MEM_LEVEL_FEW, 
      diag_tx_mode[stream_index].pool_info.item_cnt-diag_tx_mode[stream_index].few_flow_ctrl_dsm_mark /* DSMS avail */
    );
  
    /* Register the call-backs for the MANY and FEW levels */
    dsm_reg_mem_event_cb
    (
      diag_tx_mode[stream_index].pool_info.pool_id, 
      DSM_MEM_LEVEL_MANY,      
      DSM_MEM_OP_NEW,          
      diagcomm_sio_dsm_high_mem_event_cb
    );

    dsm_reg_mem_event_cb
    (
      diag_tx_mode[stream_index].pool_info.pool_id, 
      DSM_MEM_LEVEL_FEW,     
      DSM_MEM_OP_FREE,          
      diagcomm_sio_dsm_low_mem_event_cb 
    );
  }
  else
  {
    dsm_unreg_mem_event_cb( diag_tx_mode[stream_index].pool_info.pool_id, DSM_MEM_LEVEL_MANY, DSM_MEM_OP_NEW ); 
    dsm_unreg_mem_event_cb( diag_tx_mode[stream_index].pool_info.pool_id, DSM_MEM_LEVEL_FEW, DSM_MEM_OP_FREE );
  }

} /* diagcomm_sio_tx_dsm_level_reg */


/*===========================================================================

FUNCTION DIAGCOMM_SIO_INIT

DESCRIPTION
  This function initializes the diagnostic subsystem's communications layer.

===========================================================================*/
void
diagcomm_sio_init (diagcomm_enum_port_type port_num)
{
  ASSERT(port_num == DIAGCOMM_PORT_1 || port_num == DIAGCOMM_PORT_2);
  diagcomm_io_conn_type * conn = &diagcomm_io_conn[DIAGCOMM_PORT_SIO][port_num];
  
  /* Initialize watermark structures */
  memset (&diagcomm_sio_tx_wmq[port_num], 0, sizeof (dsm_watermark_type));
  memset (&diagcomm_sio_buffering_tx_wmq[port_num], 0, sizeof (dsm_watermark_type));

  if(DIAGCOMM_PORT_1 == port_num)
  {
      diagpkt_filtered_cmd_set = FALSE;
      
      /* Init callbacks, etc */
      diagcomm_params.inbound_cb = NULL;
      diagcomm_params.tx_start_cb = NULL;
      diagcomm_params.tx_stop_cb = NULL;
    
      /* Make sure we are closed */
      diagcomm_close();
  
  #if defined(DIAG_SMDL_SUPPORT) 
    {
      osal_sigs_t return_sigs;
      (void)osal_set_sigs(diag_task_tcb, DIAG_COMM_OPEN_SIG, &return_sigs);
    }
  #endif
  }
	
    (void) q_init (&diagcomm_sio_tx_q[port_num]);
    
    dsm_queue_init
    (
      &diagcomm_sio_tx_wmq[port_num],                      /* watermark queue */
      diag_tx_mode[port_num].pool_info.item_cnt * diag_tx_mode[port_num].pool_info.item_size, /* dont_exceed_cnt */
      &diagcomm_sio_tx_q[port_num]                         /* queue */
    );
      
    (void) q_init (&diagcomm_sio_buffering_tx_q[port_num]);

    dsm_queue_init
    (
      &diagcomm_sio_buffering_tx_wmq[port_num],                      /* watermark queue */
      diag_tx_mode[port_num].pool_info.item_cnt * diag_tx_mode[port_num].pool_info.item_size, /* dont_exceed_cnt */
      &diagcomm_sio_buffering_tx_q[port_num]                         /* queue */
    );

    /* Register dsm level callbacks for SIO buffering pool. */
    diagcomm_sio_tx_dsm_level_reg( port_num, TRUE );
  
    /* Set up watermarks for WaterMark Queue.  Also, set-up functions
       which will be called when the watermarks are reached. */
  
    /* CB called to resume flow when this many bytes are used in wmq */
    dsm_set_low_wm
    (
      &diagcomm_sio_tx_wmq[port_num],
      diag_tx_mode[port_num].pool_info.item_cnt * diag_tx_mode[port_num].pool_info.item_size
    );

    /* CB called to stop flow when this many bytes are used in wmq */
    dsm_set_hi_wm
    (
      &diagcomm_sio_tx_wmq[port_num],
      diag_tx_mode[port_num].pool_info.item_cnt * diag_tx_mode[port_num].pool_info.item_size
    );
        
    dsm_set_low_wm
    (
      &diagcomm_sio_buffering_tx_wmq[port_num],
      diag_tx_mode[port_num].pool_info.item_cnt * diag_tx_mode[port_num].pool_info.item_size
    );

    /* CB called to stop flow when this many bytes are used in wmq */
    dsm_set_hi_wm
    (
      &diagcomm_sio_buffering_tx_wmq[port_num],
      diag_tx_mode[port_num].pool_info.item_cnt * diag_tx_mode[port_num].pool_info.item_size
    );

    /* Register for wmq callbacks for flow control. */
    diagcomm_sio_buffering_tx_wmq[port_num].lowater_func_ptr = diagcomm_buffering_wmq_low_mem_event_cb;
    diagcomm_sio_buffering_tx_wmq[port_num].lowater_func_data = (void*)conn;
    diagcomm_sio_buffering_tx_wmq[port_num].hiwater_func_ptr = diagcomm_buffering_wmq_high_mem_event_cb;
    diagcomm_sio_buffering_tx_wmq[port_num].hiwater_func_data = (void*)conn;

    diagcomm_io_open_init( conn );

} /* diagcomm_sio_init */


/*===========================================================================
FUNCTION DIAGCOMM_SIO_OPEN_INIT

DESCRIPTION

===========================================================================*/
#ifdef DIAG_SIO_SUPPORT
void diagcomm_sio_open_init( diagcomm_io_conn_type * conn )
{
  ASSERT( conn !=NULL );
  
  if( DIAGCOMM_PORT_SIO != conn->port_type )
    return;

  ASSERT(DIAGCOMM_PORT_1 == conn->port_num || DIAGCOMM_PORT_2 == conn->port_num);
    
  if(DIAGCOMM_PORT_1 == conn->port_num)
  {
     
    /* Default DM bit rate is 38.4Kbps unless specified in NV.
       Now, it uses 115.2 (the max) as the default. */
    conn->open_params.sio_params.params.rx_bitrate = SIO_BITRATE_115200;
    conn->open_params.sio_params.params.tx_bitrate = SIO_BITRATE_115200;
  
    /* Setting callbacks */
    conn->open_params.sio_params.params.rx_func_ptr = diagcomm_sio_inbound_pkt_notify;
    
  #if defined (DIAG_RUNTIME_DEVMAP)|| defined (FEATURE_WINCE)|| defined (FEATURE_DIAG_SWITCH)
    conn->open_params.sio_params.close_func_ptr = diagcomm_sio_close_done_cb;
  #else
    conn->open_params.sio_params.close_func_ptr = NULL;
  #endif
   
  }
  else if(DIAGCOMM_PORT_2 == conn->port_num)
  {
		/* Set callback functions */
      conn->open_params.sio_params.params.rx_func_ptr = diagcomm_sio2_inbound_pkt_notify;
      conn->open_params.sio_params.close_func_ptr = diagcomm_sio_close_done_cb;
  }

  /* Parameters common to both the streams  */
    conn->open_params.sio_params.params.stream_mode = SIO_DM_MODE;
    conn->open_params.sio_params.params.rx_queue = &diagcomm_cmd_rx_wmq[conn->port_num];
    conn->open_params.sio_params.params.tx_queue = &diagcomm_sio_tx_wmq[conn->port_num];

  /* Use whatever the flow control method is currently. */
    conn->open_params.sio_params.params.rx_flow = SIO_FCTL_OFF;
    conn->open_params.sio_params.params.tx_flow = SIO_FCTL_OFF;

  /* Pass the DSM pool ID to SMD */
    conn->open_params.sio_params.open_param.dsm_pool_param.dsm_mempool_id = DSM_DIAG_CMD_SIO_RX_ITEM_POOL;
    conn->open_params.sio_params.open_param.dsm_pool_param.mempool_set = 0;
    conn->open_params.sio_params.open_param.dsm_pool_param.usb_mode = SIO_USB_MODE_LEGACY;

  /* Normally, this layer doesn't know about the HDLC encoding of the diag
       protocol.  However, SIO is configured to look for tail character. */
  #if !defined (DIAG_FRAMING)
  #ifdef FEATURE_DIAG_SWITCH
    if(diag_on_single_proc != DIAG_FTM_SWITCH_VAL)
    {
  #endif
      conn->open_params.sio_params.params.tail_char_used = FALSE;
      conn->open_params.sio_params.params.tail_char = 0;
  #ifdef FEATURE_DIAG_SWITCH
    }
    else if(diag_on_single_proc == DIAG_FTM_SWITCH_VAL)
    {
      conn->open_params.sio_params.params.tail_char_used = TRUE;
      conn->open_params.sio_params.params.tail_char = DIAG_ASYNC_CONTROL_CHAR;
    }
  #endif
  #else
  #ifdef FEATURE_DIAG_SWITCH
    if(diag_on_single_proc == DIAG_FTM_SWITCH_VAL)
    {
      conn->open_params.sio_params.params.tail_char_used = FALSE;
      conn->open_params.sio_params.params.tail_char = 0;
    }
    else
    {
  #endif
      conn->open_params.sio_params.params.tail_char_used = TRUE;
      conn->open_params.sio_params.params.tail_char = DIAG_ASYNC_CONTROL_CHAR;
  #ifdef FEATURE_DIAG_SWITCH
    }
  #endif
  #endif

  conn->open_params.sio_params.params.open_param = &(conn->open_params.sio_params.open_param);

} /* diagcomm_sio_open_init */
#endif /* DIAG_SIO_SUPPORT */

/*lint -restore */  

/*===========================================================================

FUNCTION DIAGCOMM_OUTBOUND_ALLOC

DESCRIPTION
  This function requests a buffer from the diag communications layer.

PARAMETERS 
  count -Number of bytes to be allocated 
  is_chain- Requesting a chain if variable is TRUE 
  length - length of the data in the dsm
  priority - indicates whether this data is of high priority over others in queue
  is_cmd_rsp_type -tells whether data is cmd rsp type or other(f3/logs/events)  
===========================================================================*/
dsm_item_type *
diagcomm_outbound_alloc (unsigned int *count, boolean is_chain, boolean priority,boolean is_cmd_rsp_type)
{
  dsm_item_type *item_ptr = NULL;
  ASSERT (count);

  if (is_cmd_rsp_type)
  {
    item_ptr = dsm_new_buffer(DSM_DIAG_CMD_SIO_TX_ITEM_POOL);

    if (item_ptr)
    {
      item_ptr->priority = priority;
      *count = item_ptr->size;
      item_ptr->pkt_ptr = NULL;

    } 
    else
    {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Ran out of SIO Rx Item Pool DSM items");
      *count = 0;
    }
  } 
  else 
  {
    item_ptr = dsm_new_buffer( diag_tx_mode[DIAG_STREAM_1-1].pool_info.pool_id );

    if (item_ptr)
    {
     *count = item_ptr->size;
     item_ptr->pkt_ptr = NULL; 
    }
     
    else
    {
	  MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diagcomm_outbound_alloc - Ran out of DSM items from pool %d", 
            diag_tx_mode[DIAG_STREAM_1-1].pool_info.pool_id);
      *count = 0;
    }
  }
  
  if (item_ptr)
  {
   
    if ( diagcomm_sio_outbound_item_ptr[DIAGCOMM_PORT_1] == NULL)
    {
      diagcomm_sio_outbound_item_ptr[DIAGCOMM_PORT_1] = item_ptr;
    }

    else if (is_chain)
    {
        unsigned int chain_size = 0;
	    dsm_item_type *tmp = diagcomm_sio_outbound_item_ptr[DIAGCOMM_PORT_1];
	    
        diagbuf_incr_dsm_chained_count();
        
		while(tmp->pkt_ptr != NULL)
	   	{
	   	  chain_size += tmp->size;

          if (is_cmd_rsp_type)
          {
            ASSERT(chain_size <= (DIAG_RX_RSP_MAX_CHAINED_ITEMS_SIZ));
          }
          else
          {
            ASSERT(chain_size <= (DIAG_TX_MAX_CHAINED_ITEMS_SIZ));
          }

	   	  tmp = tmp->pkt_ptr;
	   	}

	   	tmp->pkt_ptr = item_ptr;

	}

	else 
	{
		  ASSERT(0); /* is_chain not set and requesting a buffer before previous one sent */
	}
   }
   diagbuf_incr_dsm_count();
   
  return item_ptr;
  
} /* diagcomm_outbound_alloc */

/*===========================================================================

FUNCTION DIAGCOMM_OUTBOUND_ALLOC_DCI

DESCRIPTION
 This function requests a buffer from the diag communications layer.
PARAMETERS 
  count -Number of bytes to be allocated 
  is_chain- Requesting a chain if variable is TRUE 
  length - length of the data in the dsm
  priority - indicates whether this data is of high priority over others in queue
  is_cmd_rsp_type -tells whether data is cmd rsp type or other(f3/logs/events)   
===========================================================================*/
dsm_item_type *
diagcomm_outbound_alloc_dci (unsigned int *count, boolean is_chain, boolean priority,boolean is_cmd_rsp_type)
{
  dsm_item_type *item_ptr = NULL;
  ASSERT (count);

  if (is_cmd_rsp_type)
  {
    if( DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].feature_mask, F_DIAG_REQ_RSP_CHANNEL) )
	{
      item_ptr = dsm_new_buffer (DSM_DIAG_CMD_SIO_DCI_TX_ITEM_POOL);
	}
    else
    {
      item_ptr = dsm_new_buffer (diag_tx_mode[DIAG_STREAM_2-1].pool_info.pool_id);
    }
  }
  else
  {
    item_ptr = dsm_new_buffer (diag_tx_mode[DIAG_STREAM_2-1].pool_info.pool_id);
  }

  if (item_ptr)
  {
   *count = item_ptr->size;
   item_ptr->pkt_ptr = NULL; 
  }
  else
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diagcomm_outbound_alloc_dci - Ran out of (DCI) SIO Tx Item Pool DSM items");
    *count = 0;
  }

    if (item_ptr)
    {
     *count = item_ptr->size;
     item_ptr->pkt_ptr = NULL; 
    }
     
    else
    {
        MSG (MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Ran out of DSM items on DCI channel");
        *count = 0;
    }
 
  if (item_ptr)
  {
   
    if ( diagcomm_sio_outbound_item_ptr[DIAGCOMM_PORT_2] == NULL)
    {
      diagcomm_sio_outbound_item_ptr[DIAGCOMM_PORT_2] = item_ptr;
    }

    else if (is_chain)
    {
        unsigned int chain_size = 0;
	    dsm_item_type *tmp = diagcomm_sio_outbound_item_ptr[DIAGCOMM_PORT_2];
        
        diagbuf_incr_dsm_chained_count();
        
		while(tmp->pkt_ptr != NULL)
	   	{
	   	  chain_size += tmp->size;

          if (is_cmd_rsp_type)
          {
            ASSERT(chain_size <= DIAG_RX_RSP_MAX_CHAINED_ITEMS_SIZ);
          }
          else
          {
            ASSERT(chain_size <= DIAG_TX_MAX_CHAINED_ITEMS_SIZ);
          }

	   	  tmp = tmp->pkt_ptr;
	   	}

	   	tmp->pkt_ptr = item_ptr;

	}

	else 
	{
		  ASSERT(0); /* is_chain not set and requesting a buffer before previous one sent */
	}
   }
   diagbuf_incr_dsm_count();
   
  return item_ptr;
}  
 /* diagcomm_outbound_alloc_dci */

/*===========================================================================

FUNCTION DIAGCOMM_SEND
  
DESCRIPTION
  This function returns how many items are currently in the buffering queue.
  
PARAMETERS 
  port_num - Port of buffering queue to be checked
 
RETURN VALUE
  Length of queue or 0 if no queue exists
  
===========================================================================*/
uint32 diagcomm_sio_get_buf_tx_wmq_cnt( diagcomm_enum_port_type port_num )
{
  if (port_num > DIAGCOMM_PORT_NUM_LAST )
    return 0;
  return q_cnt(diagcomm_sio_buffering_tx_wmq[port_num].q_ptr);
}

/*===========================================================================

FUNCTION DIAGCOMM_SEND
  
DESCRIPTION
  Sends the given data stream if the connection is open.
  
PARAMETERS
  ptr - Pointer to DSM packet to send
  length - Length of data in DSM packet
  is_cmd_rsp_type - True is DSM contains response packet (has higher priority). 
                    Else False.
  port_num - Port number
  
RETURN VALUE
  None
  
===========================================================================*/
void
diagcomm_send (dsm_item_type * ptr, unsigned int length, boolean is_cmd_rsp_type, diagcomm_enum_port_type port_num)
{
 diagcomm_io_tx_params_type tx_params;
 boolean is_buffered = FALSE;
 diagcomm_io_conn_type * conn = &diagcomm_io_conn[DIAGCOMM_PORT_SIO][port_num];
 diagcomm_io_conn_type * cmd_conn = &diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][port_num];
 diagcomm_io_conn_type * ctrl_conn = &diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][port_num];
 uint8 stream_index;
 uint8 stream_id;
 
 ASSERT (ptr);

 if( port_num > DIAGCOMM_PORT_NUM_LAST )
 {
   MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diagcomm_send: Invalid port_num (%d)", port_num);
   return;
 }
 
 stream_index = DIAG_PORT_TO_INDEX(port_num);
 stream_id = DIAG_INDEX_TO_ID(stream_index);
 
 if (diagcomm_sio_outbound_item_ptr[port_num] == NULL)    
 {
   /* Corrupt state.*/
   MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"sio_oubound_ptr is NULL. Nothing to send");
 }
 else
 {      
    if (length > 0)
    {   
       /* If we're in buffering modes, enqueue the DSM instead of sending to SIO immediately. */
       if( (diag_tx_mode[stream_index].mode == DIAG_TX_MODE_BUFFERED_THRESH) && (is_cmd_rsp_type == FALSE) )
       {
         is_buffered = TRUE;
         diagcomm_buffer_enqueue_stream( &diagcomm_sio_outbound_item_ptr[port_num], DIAGCOMM_PORT_SIO, stream_id );
       }
       else if( (diag_tx_mode[stream_index].mode == DIAG_TX_MODE_BUFFERED_CIR) && (is_cmd_rsp_type == FALSE) )
       {
         is_buffered = TRUE;
         diagcomm_buffer_enqueue_stream( &diagcomm_sio_outbound_item_ptr[port_num], DIAGCOMM_PORT_SIO, stream_id );
       }
       else if( (diag_tx_mode[stream_index].mode == DIAG_TX_MODE_STREAMING) && (is_cmd_rsp_type == FALSE) )
       {
        /* Queue the DSM to the end of the buffering WM queue if that queue is not empty. Else just send it right away. */
        if(diagcomm_sio_get_buf_tx_wmq_cnt(DIAGCOMM_PORT_1) > 0 )
         {
           is_buffered = TRUE;
           diagcomm_buffer_enqueue_stream( &diagcomm_sio_outbound_item_ptr[port_num], DIAGCOMM_PORT_SIO, stream_id );
         }
       }
       
       if( is_buffered == FALSE )
       {
         tx_params.dsm_ptr = diagcomm_sio_outbound_item_ptr[port_num];
         #if !defined(DIAG_MP_MASTER)
         if ((is_cmd_rsp_type))
         {
          if( DIAG_IS_INT_FEATURE_BIT_SET(ctrl_conn->feature_mask, F_DIAG_REQ_RSP_CHANNEL) )
            diagcomm_io_transmit( cmd_conn, &tx_params );
          else
            diagcomm_io_transmit( conn, &tx_params );
         }
         else
         #endif
         diagcomm_io_transmit( conn, &tx_params );
       }
       else
       {
         /* If data is buffered, check if ready to drain. */
         if( diag_drain_pool_check(stream_id) )
         {           
           if( stream_id == DIAG_STREAM_1 )
             diag_set_internal_sigs( DIAG_INT_DRAIN_DSM_BUFFER_SIG );
           else if( stream_id == DIAG_STREAM_2 )
             diag_set_internal_sigs( DIAG_INT_DRAIN_DCI_DSM_BUFFER_SIG );
         }
       }
    }
    else
    {
      if (diagcomm_sio_outbound_item_ptr[port_num]->used > 0) 
       ASSERT(0);

      /* If empty, just release it */
      (void) dsm_free_packet (&diagcomm_sio_outbound_item_ptr[port_num]);
    }

    diagcomm_sio_outbound_item_ptr[port_num] = NULL;
 }
} /* diagcomm_send */


/*===========================================================================
FUNCTION diagcomm_sio_flow_enabled

DESCRIPTION
   This function tells whether the flow to 
   DSM_DIAG_SIO_TX_ITEM_POOL/DSM_DIAG_BUFFERING_ITEM_POOL is enabled or not.
===========================================================================*/
boolean diagcomm_sio_flow_enabled(void)
{
  return allowflow;
}

/*===========================================================================
FUNCTION diagcomm_sio_dsm_high_mem_event_cb

DESCRIPTION
   This function gets triggered and disables flow, when the high item count 
   for DSM_DIAG_SIO_TX_ITEM_POOL/DSM_DIAG_BUFFERING_ITEM_POOL is reached.
   (DSM_DIAG_SIO_TX_ITEM_POOL/DSM_DIAG_BUFFERING_ITEM_POOL is used by sio, 
   to receive data.)
   
===========================================================================*/
static void diagcomm_sio_dsm_high_mem_event_cb (dsm_mempool_id_type pool_id,
                                                dsm_mem_level_enum_type mem_level,
                                                dsm_mem_op_enum_type mem_op)
{
   uint32 flow_cnt = 0;
   
   /* check to be defensive */
   if(pool_id != diag_tx_mode[DIAG_ID_TO_INDEX(DIAG_STREAM_1)].pool_info.pool_id || 
      mem_level != DSM_MEM_LEVEL_MANY ||
      mem_op != DSM_MEM_OP_NEW)
      return;

   if (TRUE == allowflow)
   {
      allowflow = FALSE;
      flow_cnt = diagcomm_incr_flow_ctrl_count(DIAGCOMM_PORT_SIO);
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "diagcomm_sio_dsm_high_mem_event_cb: disabled flow (cnt=%d)", flow_cnt);
   }
   
} /* diagcomm_sio_dsm_high_mem_event_cb */


/*===========================================================================
FUNCTION diagcomm_sio_dsm_low_mem_event_cb

DESCRIPTION
   This function gets triggered and enables flow, when the low item count 
   for DSM_DIAG_SIO_TX_ITEM_POOL/DSM_DIAG_BUFFERING_ITEM_POOL is reached.
   (DSM_DIAG_SIO_TX_ITEM_POOL/DSM_DIAG_BUFFERING_ITEM_POOL is used by sio, 
   to receive data.)
   
===========================================================================*/
static void diagcomm_sio_dsm_low_mem_event_cb (dsm_mempool_id_type pool_id,
                                               dsm_mem_level_enum_type mem_level,
                                               dsm_mem_op_enum_type mem_op)
{
  osal_sigs_t return_sigs;
  int return_val = 0;

   /* check to be defensive */
   if(pool_id != diag_tx_mode[DIAG_ID_TO_INDEX(DIAG_STREAM_1)].pool_info.pool_id || 
      mem_level != DSM_MEM_LEVEL_FEW ||
      mem_op != DSM_MEM_OP_FREE)
      return;
   
   if (FALSE == allowflow)
   {
      /* Set flow control to allow flow */
      allowflow = TRUE;
      diag_tx_notify();

      return_val = osal_set_sigs(diag_task_tcb, DIAG_EVENT_DRAIN_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);

      MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"diagcomm_sio_dsm_low_mem_event_cb: enabled flow");
   }

} /* diagcomm_sio_dsm_low_mem_event_cb */


/*===========================================================================
FUNCTION diagcomm_buffering_tx_wmq_flow_enabled

DESCRIPTION
   This function tells whether the flow to diagcomm_sio_buffering_tx_wmq is enabled
   or not.
   
PARAMETERS
    port_num - Port num (DIAGCOMM_PORT_1, DIAGCOMM_PORT_2, ...)
    
RETURN VALUE
  TRUE - Flow to SIO Buffering Tx wmq is enabled
  FALSE - Otherwise 
  
===========================================================================*/
boolean diagcomm_buffering_tx_wmq_flow_enabled( diagcomm_enum_port_type port_num )
{
  return bDiagAllowTxWmqFlow[port_num];
 
} /* diagcomm_buffering_tx_wmq_flow_enabled */


/*===========================================================================
FUNCTION diagcomm_buffering_wmq_high_mem_event_cb

DESCRIPTION
   This function gets triggered and disables flow, when the hi_watermark byte 
   count for diagcomm_sio_buffering_tx_wmq is reached.
   
   diagcomm_sio_buffering_tx_wmq is used by diag to queue Tx data in buffering mode.
===========================================================================*/
static void diagcomm_buffering_wmq_high_mem_event_cb( dsm_watermark_type *wm_ptr, void * data )
{
  uint32 flow_cnt = 0;
  diagcomm_io_conn_type * conn = (diagcomm_io_conn_type *)data;

  if( data == NULL )
    return;
      
  if( bDiagAllowTxWmqFlow[conn->port_num] == TRUE )
  {
    bDiagAllowTxWmqFlow[conn->port_num] = FALSE;
    flow_cnt = diagcomm_incr_wmq_flow_ctrl_count(conn->port_num);
    
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, 
          "diagcomm_buffering_wmq_high_mem_event_cb: disabled flow (port=%d cnt=%d)", conn->port_num, flow_cnt);
  }
  
} /* diagcomm_buffering_wmq_high_mem_event_cb */


/*===========================================================================
FUNCTION diagcomm_buffering_wmq_low_mem_event_cb

DESCRIPTION
   This function gets triggered and enables flow, when the lo_watermark byte 
   count for diagcomm_sio_buffering_tx_wmq is reached.
   
   diagcomm_sio_buffering_tx_wmq is used by diag to queue Tx data in buffering mode.
===========================================================================*/
static void diagcomm_buffering_wmq_low_mem_event_cb( dsm_watermark_type *wm_ptr, void * data )
{
  uint8 stream_index;
  uint8 stream_id;
  diagcomm_io_conn_type * conn = (diagcomm_io_conn_type *)data;
  
  if( data == NULL )
    return;
    
  stream_index = DIAG_PORT_TO_INDEX( conn->port_num );
  stream_id = DIAG_INDEX_TO_ID(stream_index);
  
  if( bDiagAllowTxWmqFlow[conn->port_num] == FALSE )
  {
    bDiagAllowTxWmqFlow[conn->port_num] = TRUE;

    if( (diag_tx_mode[stream_index].mode == DIAG_TX_MODE_STREAMING) || diag_tx_mode[stream_index].drain )
    {      
      if( stream_id == DIAG_STREAM_1 )
        diag_set_internal_sigs( DIAG_INT_DRAIN_DSM_BUFFER_SIG );
      else if( stream_id == DIAG_STREAM_2 )
        diag_set_internal_sigs( DIAG_INT_DRAIN_DCI_DSM_BUFFER_SIG );
    }
    MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "diagcomm_buffering_wmq_low_mem_event_cb: enabled flow");
  }
  
} /* diagcomm_buffering_wmq_low_mem_event_cb */


/*===========================================================================
FUNCTION DIAGCOMM_RESET_FLOW_CTRL_COUNT

DESCRIPTION
    This function resets the internal variable diagcomm_flow_ctrl_count, which keeps
    track of the # of times flow control is triggered.
    
===========================================================================*/
void diagcomm_reset_flow_ctrl_count( void )
{
  diagcomm_flow_ctrl_count[DIAGCOMM_PORT_SMD] = 0;
  diagcomm_flow_ctrl_count[DIAGCOMM_PORT_SIO] = 0;
   
}  /* diagcomm_reset_flow_ctrl_count */


/*===========================================================================
FUNCTION DIAGCOMM_GET_FLOW_CTRL_COUNT

DESCRIPTION
    This function returns the internal variable diagcomm_flow_ctrl_count, which keeps
    track of the # of times flow control is triggered.
    
===========================================================================*/
uint32 diagcomm_get_flow_ctrl_count( diagcomm_port_type port_type )
{
  return diagcomm_flow_ctrl_count[port_type];
  
} /* diagcomm_get_flow_ctrl_count */


/*===========================================================================
FUNCTION DIAGCOMM_INCR_FLOW_CTRL_COUNT

DESCRIPTION
  This function increments the internal variable diagcomm_flow_ctrl_count, which keeps
  track of the # of times flow control is triggered.
    
PARAMETERS
  port_type
  
RETURN VALUE
  The current flow control count.
  
===========================================================================*/
uint32 diagcomm_incr_flow_ctrl_count( diagcomm_port_type port_type )
{
  EVENT_DIAG_DROP_DBG_PAYLOAD_INFO_type flow_ctrl_event_payload;
  
  if( diagcomm_flow_ctrl_count[port_type] == MAX_VALUE_UINT32 ) 
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "diagcomm_flow_ctrl_count has reached max value");
  }
  else
  {
    diagcomm_flow_ctrl_count[port_type]++;
  }

  flow_ctrl_event_payload.alloc_f3_cnt = alloc_count_f3;
  flow_ctrl_event_payload.alloc_log_cnt = alloc_count_log;
  flow_ctrl_event_payload.drop_f3_cnt = drop_count_f3;
  flow_ctrl_event_payload.drop_log_cnt = drop_count_log;
  flow_ctrl_event_payload.drop_cnt_per = diag_per_count_dropped;
  flow_ctrl_event_payload.flow_ctrl_cnt_sio = diagcomm_flow_ctrl_count[DIAGCOMM_PORT_SIO];
  flow_ctrl_event_payload.flow_ctrl_cnt_smd  = diagcomm_flow_ctrl_count[DIAGCOMM_PORT_SMD];
  flow_ctrl_event_payload.flow_ctrl_cnt_dci = 0; /* TODO: Update this when flow control is added for the DCI Channel */
  flow_ctrl_event_payload.pd_id = DIAG_GUEST_OS_PD;
  flow_ctrl_event_payload.proc_id = (uint16) DIAG_MY_PROC_ID;
  flow_ctrl_event_payload.event_source = DIAG_FLOW_CTRL;

  event_report_payload(EVENT_DIAG_DROP_DEBUG, sizeof(EVENT_DIAG_DROP_DBG_PAYLOAD_INFO_type), &flow_ctrl_event_payload);
  
  return diagcomm_flow_ctrl_count[port_type];

} /* diagcomm_incr_flow_ctrl_count */


/*===========================================================================
FUNCTION DIAGCOMM_INCR_WMQ_FLOW_CTRL_COUNT

DESCRIPTION
  This function increments the internal variable diagcomm_wmq_flow_ctrl_count, which keeps
  track of the # of times flow control is triggered.
    
PARAMETERS
  port_num
  
RETURN VALUE
  The current wmq flow control count.
  
===========================================================================*/
uint32 diagcomm_incr_wmq_flow_ctrl_count( diagcomm_enum_port_type port_num )
{
  if( diagcomm_wmq_flow_ctrl_count[port_num] == MAX_VALUE_UINT32 ) 
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "diagcomm_incr_wmq_flow_ctrl_count has reached max value");
  } 
  else 
  {
    diagcomm_wmq_flow_ctrl_count[port_num]++;
  }
  
  return diagcomm_wmq_flow_ctrl_count[port_num];
  
} /* diagcomm_incr_wmq_flow_ctrl_count */


#ifdef FEATURE_WINCE
#error code not present
#endif /* FEATURE_WINCE */



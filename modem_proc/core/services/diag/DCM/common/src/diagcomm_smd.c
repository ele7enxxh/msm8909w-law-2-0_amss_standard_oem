/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostic Communications via SMD

General Description
  The routines in this file handle the communications layer for the
  diagnostic subsystem when communicating via SMD.

Copyright (c) 2001-2014 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                               Edit History

 $PVCSPath:  L:/src/asw/MSM5200/common/vcs/diagcomm_sio.c_v   1.10   08 May 2002 14:37:42   ldefauw  $
 $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diagcomm_smd.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/15/15   ph      Corrected the exit condition to dequeue the DSM items from 
                   buffering queue.
01/30/14   ph      Removed the usage of macro DIAG_TASK_HANDLE().
01/01/14   ph      Fixed inconsistent drain behavior in circular and threshold buffering.
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
05/31/13   is      Support for peripheral buffering mode
01/03/13   sg      Command Response Channel separation
08/31/12   ra      Use accessor macros instead of accessing private task         
                   info directly; accomodated changes for User space
                   task priority changes   
05/25/12   sa      Replaced DIAG_REQ_FWD with DIAG_MP_MASTER
05/10/12   sa      Removed DIAG_MP_SLAVE_LAST
05/03/12   sa      Mainlining DIAG_CENTRAL_ROUTING
10/07/11   is      Support sending log mask per equipment id
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
04/28/11   is      Add missing sleep vote
04/28/11   is      Fix compilation warnings
03/29/11   sg      Central Routing changes for Q6
03/25/11   is      Fix compilation errors for modem-only configuration
03/04/11   is      Support for Diag over SMD-Lite
12/10/10   mad     cleanup: Got rid of init_done in diagcomm_smd_open() 
12/08/10   mad     do not open SMD port if already open: do not reset 
                   diagcomm_smd_connected
10/20/10   is      Resolve compilation issue in Apps only Diag
09/28/10   mad     Fixed compile error regarding  DIAGCOMM_SMD_1 and DIAGCOMM_SMD_2
09/14/10   is      New control channel and central routing features 
08/10/10   sg      Close SMD Port when the SIO port on apps is closed
09/13/10   is      Add flow control count and DSM chaining count to Diag Health
08/25/10   sg      Fixed compiler warnings
05/25/10   JV      New task on master to process slave traffic
05/13/10   JV      Merge functionality in diagcomm_smd.c and diagcomm_smd2.c
                   in a single file
10/29/09   JV      Specify DSM_DIAG_SIO_TX_ITEM_POOL as pool ID for slave last
                   processor.
10/29/09   JV      Pass the DSM pool ID to SMD
09/23/09   JV      To initialize the smd_tx_wmq, use the sizes and counts from the
                   SIO_TX pool. This is the DSM pool it uses.
09/23/09   JV      Do not initialize the SMD_RX and SMD_TX pools on the slave
                   last.
08/11/09   mad     flow-control is now independent of port open/close status
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/31/09   JV      Merged Q6 diag code back to mainline
07/23/09   JV      Removed inclusion of task.h
06/23/09   JV      Included diagdsm_v.h instead of diagdsm.h. Featurized
                   inclusion of customer.h and target.h for wm.
05/28/09   mad     Mainlined item-level flow-control for SMD_TX pool
05/12/09   JV      Introduced the OS abstraction layer.
05/07/09   vk      changed includes from external to internal headers
04/10/09   mad     modified flow-control for SMD_TX pool, added SMD2 channel
03/20/09   mad     Changed DSM_MEM_LEVEL_MANY for SMD_TX flow-control,to keep
                   up with change in MANY level defined in diagdsmi_wce.h
02/19/09   mad     Inbound traffic from SMD is now processed in diag 
                   task context, by handling DIAG_TX_SLAVE_SIG signal.
10/30/08   mad     Introduced flow-control for DSM_DIAG_SMD_TX_ITEM_POOL, at
                   item-level granularity. Took out watermark Q method of 
                   flow-control.
08/14/08   sj      Added flow control function for higher threshold 
05/17/07   jc      Proxy by tv.  Revise low water mark to 5 * DSM_DIAG_SMD_TX_ITEM_SIZ
12/15/06   as      Fixed compiler warnings.
11/21/06   as      Modified code to use diag internal features.
01/04/06   ptm     Change sio port name for SMD.
07/20/05   pjb     Changes to support DSM 3
06/15/05   cr      Fix CR#: 66743 - Surf crashes when diag is changed from 
                   USB to UART1. Don't dequeue dsm_item or call sio_transmit
                   unless diagcomm is connected.
06/01/05   as      Added data transfer mode and transfer flow type to sio
                   open params under FEATURE_SMD. 
05/17/05   as      Added Dual processor Diag support.
04/14/05   adm     Added FEATURE SMD check along with FEATURE_SMEM_DS 
03/04/05    cr     Delete calls to dsm_reg_mem_event_cb because that was
                   completely breaking outbound flow control by leaving null
                   function pointer in diagcomm start/stop callback pointers
                   leading to DSM item exhausting and QXDM disconnect.
02/17/05   as      Removed async character check for APPS processor.
11/22/04    cr     Only compile diagcomm_smd for modem side, multi-processor 
                   build.
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
04/17/01   dwp     Initialize diagcomm_smd_open_params.port_id to NULL. RDM
                   will instruct DIAG as to what port to open.
04/06/01   lad     Introduced types for callback function pointers.
                   Cleaned up FEATURE_RUNTIME_DEVMAP support.
                   Updated watermark settings (this may be changed to save RAM).
02/23/01   lad     Created file.

===========================================================================*/

#include "core_variation.h"
#include "comdef.h"
#ifndef FEATURE_WINCE
#endif 

#include "diag_v.h"
#include "diagbuf_v.h"
#include "diagcmd.h"
#include "diagcomm_smd.h"
#include "diagcomm_v.h"
#include "diagi_v.h"
#include "diagpkt.h"
#include "diagtune.h"
#include "msg.h"
#include "diag_cfg.h"   /* For DIAGBUF_COMMIT_THRESHOLD */
#include "diagdiag_v.h" /* For DIAGDIAG_TX_MODE_CONFIG_APP */
#include "eventi.h"
#include "diagcomm_io.h"
#include "diagdsm_v.h"
#include "diag_mode_v.h" /* For diag_buffering_pool_used_cnt() */
#include "diagdsmi.h" /* for def. of DSMI_DIAG_SMD_TX_ITEM_FEW_MARK etc */
#include "diag_fwd_v.h"

#if defined (FEATURE_DIAG_NV)
#include "diagnv_v.h"
#include "nv.h"
#endif

#include "dsm.h"
#include "osal.h"

#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "assert.h"
#endif

/* -------------------------------------------------------------------------
** Definitions and Declarataions
** ------------------------------------------------------------------------- */
extern volatile diag_tx_mode_type diag_tx_mode[DIAG_MAX_STREAM_ID]; /* From diag_mode.c */

extern diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];      /* From diagcomm_cfg.c */
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_cfg.c */
extern diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_cfg.c */


/* Queues around which the watermark queues are built */
#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
static q_type diagcomm_smd_rx_q[NUM_SMD_PORTS];
#endif
static q_type diagcomm_smd_tx_q[NUM_SMD_PORTS];

/* Watermark queues used for communications with SIO */
dsm_watermark_type diagcomm_smd_rx_wmq[NUM_SMD_PORTS];
dsm_watermark_type diagcomm_smd_tx_wmq[NUM_SMD_PORTS];
extern dsm_watermark_type diagcomm_sio_buffering_tx_wmq[NUM_SIO_PORTS];
extern uint32 diagcomm_sio_get_buf_tx_wmq_cnt( diagcomm_enum_port_type port_num );

/*-------------------------------------------------------------------------
                   Local function prototype
 ------------------------------------------------------------------------- */
#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
/* Call-backs for flow control at pool's item-level granularity.
Flow-control using watermark Qs work with the number of bytes,
 and that does not work reliably, because a pool might run out of items,
 but still have enough bytes left. */
static void diagcomm_smd_dsm_low_mem_event_cb (dsm_mempool_id_type,
                                               dsm_mem_level_enum_type,
                                               dsm_mem_op_enum_type);
static void diagcomm_smd_dsm_high_mem_event_cb (dsm_mempool_id_type pool_id,
                                                dsm_mem_level_enum_type mem_level,
                                                dsm_mem_op_enum_type mem_op);
#endif


/*===========================================================================

FUNCTION DIAG_FWD_NOTIFY

DESCRIPTION

PARAMETERS
  None
  
RETURN VALUE
  None
  
===========================================================================*/
#if defined(DIAG_MP_MASTER)
void diag_fwd_notify( void )
{
  osal_sigs_t return_sigs;
  int return_val = 0;
  uint32 dsm_cnt = 0;
  
  if( diag_tx_mode.mode == DIAG_TX_MODE_BUFFERED_THRESH )
  {
    dsm_cnt = diag_buffering_pool_used_cnt( DIAG_STREAM_1 );
	
	if( dsm_cnt >= diag_tx_mode.buffered_many_mark )
	  diag_tx_mode.drain = TRUE;
	else if( diag_tx_mode.drain && (dsm_cnt <= diag_tx_mode.buffered_few_mark) )
	  diag_tx_mode.drain = FALSE;
  }
  
  if( (diag_tx_mode.mode == DIAG_TX_MODE_STREAMING) || diag_tx_mode.drain )
  {	
    if( diagcomm_sio_tx_wmq_flow_enabled() == TRUE )
	{
      /* Set signal to diag task to process traffic from slave */
      return_val = osal_set_sigs(&diag_fwd_task_tcb, DIAG_TX_SLAVE_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
	}
  }

} /* diag_fwd_notify */
#endif /* DIAG_MP_MASTER */


/*===========================================================================

FUNCTION DIAGCOMM_BUFFER_ENQUEUE_STREAM

DESCRIPTION

PARAMETERS
  dsm_item_type
  diagcomm_port_type
  stream_id
  
RETURN VALUE
  None
  
===========================================================================*/
void diagcomm_buffer_enqueue_stream( dsm_item_type ** item_ptr, 
									 diagcomm_port_type port_type,
								     uint8 stream_id )
{
  dsm_item_type * dsm_dequeue_item_ptr = NULL;
  dsm_item_type * dsm_chained_ptr = NULL;
  uint32 dsm_cnt = 0;
  uint32 dsm_dropped_cnt = 0;
  uint32 bytes_dropped_cnt = 0;
  uint32 dsm_alloc_cnt = 0;
  uint32 bytes_alloc_cnt = 0;
  uint8 stream_index = 0;
  diagcomm_enum_port_type port_num;
  
  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diagcomm_buffer_enqueue_stream - Invalid stream_id (%d)", stream_id);
    return;
  }
  
  port_num = DIAG_ID_TO_PORT(stream_id);
  stream_index = DIAG_ID_TO_INDEX(stream_id);
  
  /* Dequeue and count dropped DSMs only in Buffered Cir mode. */
  if( diag_tx_mode[stream_index].mode == DIAG_TX_MODE_BUFFERED_CIR )
  {
    /*Count of the dsm items in buffering queue*/
    dsm_cnt = diagcomm_sio_get_buf_tx_wmq_cnt( port_num );
    diag_tx_mode[stream_index].cur_dsm_cnt[port_num] = dsm_cnt; //Save dsm count read

    while( DIAG_BUFFERED_DSM_CNT_TO_BYTES(dsm_cnt) >= diag_tx_mode[stream_index].buffered_many_bytes_mark )
	{
	  if( port_type == DIAGCOMM_PORT_SMD )
	    dsm_dequeue_item_ptr = diagcomm_io_dequeue_rx_wmq( &diagcomm_io_conn[port_type][DIAGCOMM_PORT_1] );
	  else if( port_type == DIAGCOMM_PORT_SIO )
        dsm_dequeue_item_ptr = diagcomm_dequeue_buffering_tx_wmq( (uint8) DIAGCOMM_PORT_1);
		
	  ASSERT( dsm_dequeue_item_ptr != NULL );
	  
	  //bytes_dropped_cnt += dsm_length_packet( dsm_dequeue_item_ptr );
	  dsm_chained_ptr = dsm_dequeue_item_ptr;
	  do
      {  
	    dsm_dropped_cnt += 1;
		bytes_dropped_cnt += dsm_chained_ptr->used;
		
		// Point to next DSM in the chain 
        dsm_chained_ptr = dsm_chained_ptr->pkt_ptr;
		
        // Continue to count chained DSMs 
	  } while( dsm_chained_ptr != NULL );
      //} while( (dsm_chained_ptr != NULL) && (dsm_chained_ptr->data_ptr != NULL) && (dsm_chained_ptr->used > 0) );
	  
	  // Update dropped count health statistics 
	  if( (diag_tx_mode[stream_index].dsm_dropped_cnt + dsm_dropped_cnt) >= MAX_VALUE_UINT32 )
	    diag_tx_mode[stream_index].dsm_dropped_cnt = MAX_VALUE_UINT32;
	  else
	    diag_tx_mode[stream_index].dsm_dropped_cnt += dsm_dropped_cnt;
	  
	  if( (diag_tx_mode[stream_index].byte_dropped_cnt + bytes_dropped_cnt) >= MAX_VALUE_UINT32 )
	    diag_tx_mode[stream_index].byte_dropped_cnt = MAX_VALUE_UINT32;
	  else
        diag_tx_mode[stream_index].byte_dropped_cnt += bytes_dropped_cnt;
	  
	  // Free the dequeued DSM item, including it's chained items 
	  dsm_free_packet( &dsm_dequeue_item_ptr );
	  /*Count of the dsm items in buffering queue*/
      dsm_cnt = diagcomm_sio_get_buf_tx_wmq_cnt(port_num);	  
      diag_tx_mode[stream_index].cur_dsm_cnt[port_num] = dsm_cnt;
	}
  }

  /* We don't drop/dequeue DSMs in Buffered Threshold mode. Just count alloc
     counts and enqueue. */
  if( (diag_tx_mode[stream_index].mode == DIAG_TX_MODE_BUFFERED_THRESH) || 
      (diag_tx_mode[stream_index].mode == DIAG_TX_MODE_BUFFERED_CIR) )
  {
	//bytes_alloc_cnt += dsm_length_packet( *item_ptr );
	dsm_chained_ptr = *item_ptr;
	do
	{
	   dsm_alloc_cnt += 1;
	   bytes_alloc_cnt += dsm_chained_ptr->used;
	   
	   // Point to next DSM in the chain 
       dsm_chained_ptr = dsm_chained_ptr->pkt_ptr;
	   
	  // Continue to count chained DSMs 
	} while( dsm_chained_ptr != NULL );
	
	// Update alloc count health statistics 
	if( (diag_tx_mode[stream_index].dsm_alloc_cnt + dsm_alloc_cnt) >= MAX_VALUE_UINT32 )
	  diag_tx_mode[stream_index].dsm_alloc_cnt = MAX_VALUE_UINT32;
	else
	  diag_tx_mode[stream_index].dsm_alloc_cnt += dsm_alloc_cnt;
	  
	if( (diag_tx_mode[stream_index].byte_alloc_cnt + bytes_alloc_cnt) >= MAX_VALUE_UINT32 )
	  diag_tx_mode[stream_index].byte_alloc_cnt = MAX_VALUE_UINT32;
	else
	  diag_tx_mode[stream_index].byte_alloc_cnt += bytes_alloc_cnt;
	
	if( port_type == DIAGCOMM_PORT_SMD )
	  dsm_enqueue (diagcomm_io_conn[port_type][DIAGCOMM_PORT_1].open_params.sio_params.params.rx_queue, item_ptr);
	else if( port_type == DIAGCOMM_PORT_SIO )
      dsm_enqueue (&diagcomm_sio_buffering_tx_wmq[DIAGCOMM_PORT_1], item_ptr);
  }
  else
  {
    /* Streaming mode; Just enqueue it. */
	if( port_type == DIAGCOMM_PORT_SMD )
      dsm_enqueue (diagcomm_io_conn[port_type][DIAGCOMM_PORT_1].open_params.sio_params.params.rx_queue, item_ptr);
	else if( port_type == DIAGCOMM_PORT_SIO )
      dsm_enqueue (&diagcomm_sio_buffering_tx_wmq[DIAGCOMM_PORT_1], item_ptr);
  }

} /* diagcomm_buffer_enqueue_stream */


/*===========================================================================

FUNCTION DIAGCOMM_SMD_INBOUND_PKT_NOTIFY

DESCRIPTION
  This is the callback SIO calls when data is received.

  This function enqueues the data onto the diag's watermark.  Further, if the
  last char in the input dsm_item_type is equal to DIAG_ASYNC_CONTROL_CHAR,
  the inbound callback is called.

===========================================================================*/
#if defined(DIAG_SIO_SUPPORT)
void
diagcomm_smd_inbound_pkt_notify (dsm_item_type ** item_ptr)
{
#if defined(DIAG_MP_MASTER) 
  if( diag_tx_mode.mode == DIAG_TX_MODE_BUFFERED_CIR )
  {
    diagcomm_buffer_enqueue_stream( item_ptr, DIAGCOMM_PORT_SMD, DIAG_STREAM_1 );
  }
  else
  {	
    diagcomm_buffer_enqueue_stream( item_ptr, DIAGCOMM_PORT_SMD, DIAG_STREAM_1 );
    diag_fwd_notify();
  }
#endif /* DIAG_MP_MASTER */

} /* diagcomm_smd_inbound_pkt_notify */
#endif /* DIAG_SIO_SUPPORT */

 
/*===========================================================================

FUNCTION DIAGCOMM_SMD2_INBOUND_PKT_NOTIFY

DESCRIPTION
  This is the callback SIO calls when data is received.

  This function enqueues the data onto the diag's watermark.  Further, if the
  last char in the input dsm_item_type is equal to DIAG_ASYNC_CONTROL_CHAR,
  the inbound callback is called.

===========================================================================*/
#if defined(DIAG_SIO_SUPPORT) && !defined(DIAG_BUFFERING_SUPPORT)
void
diagcomm_smd2_inbound_pkt_notify (dsm_item_type ** item_ptr)
{
  osal_sigs_t return_sigs;
  int return_val = 0;
  
  /* dsm item originated on APPS proc */
  dsm_enqueue (diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_2].open_params.sio_params.params.rx_queue, item_ptr);

  /* Set signal to diag task to process slave traffic */
  return_val = osal_set_sigs(&diag_fwd_task_tcb, DIAG_TX_SLAVE_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);
  
} /* diagcomm_smd2_inbound_pkt_notify */
 #endif /* DIAG_SIO_SUPPORT && !DIAG_BUFFERING_SUPPORT */
 

void diagcomm_smd_close_done_cb(void)
{
  diag_release ();
  
} /* diagcomm_smd_close_done_cb */

/* -------------------------------------------------------------------------
** Externalized routines (declared in diagcomm.h).
** ------------------------------------------------------------------------- */


/*===========================================================================

FUNCTION DIAGCOMM_SMD_OPEN

DESCRIPTION
  Opens the communcications stream.  Returns boolean indicating success.

===========================================================================*/
boolean
diagcomm_smd_open (diagcomm_enum_port_type port_num)
{
  diagcomm_io_conn_type * conn = &diagcomm_io_conn[DIAGCOMM_PORT_SMD][port_num];
  
  if( conn->connected )
  {
    return conn->connected;
  }
  
  diagcomm_io_set_port( conn );

  if( !conn->connected )
  {
    if( diagcomm_io_open(conn) == TRUE )
    {
      conn->connected = TRUE;
      diag_io_detect_connection( conn );
    }
    else
    {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "diagcomm_smd_open - Couldn't get stream.");
    }
  }
  /* Register DTR callbacks to detect connection change. */
  diag_io_detect_connection( conn );
  
  return conn->connected;
} /* diagcomm_smd_open */

/*===========================================================================

FUNCTION DIAGCOMM_SMD_CLOSE

DESCRIPTION
  Closes the current connection, if any.

===========================================================================*/
void
diagcomm_smd_close ( diagcomm_enum_port_type port_num )
{
  diagcomm_io_conn_type * conn = &diagcomm_io_conn[DIAGCOMM_PORT_SMD][port_num];
  
  if (DIAGCOMM_PORT_1 == conn->port_num)
  {
    #if (!defined (DIAG_MP_MASTER) && !defined (DIAG_QDSP6_APPS_PROC))
	return;
	#endif
  }

  else if (DIAGCOMM_PORT_2 == conn->port_num)
  {
    #if !defined (DIAG_MP_MASTER)
	return;
	#endif
  }
  /* Got a command to close the connection. */
  if( conn->connected )
  {
    diagcomm_io_close( conn );
  }

  /* success or not, consider close operation successful  */
  conn->connected = FALSE;

} /* diagcomm_smd_close */


/*===========================================================================

FUNCTION DIAGCOMM_SMD_CONN_CHANGED

DESCRIPTION

===========================================================================*/
void diagcomm_smd_conn_changed( diagcomm_io_conn_type * conn, boolean connected )
{
  /* Nothing needs to be done when SMD conn changed on Slave for now. */
#if defined(DIAG_MP_MASTER)
  uint8 preset_index;

  if( connected )
  {
    conn->channel_connected = TRUE;
    
    /* If connection just opened, Master needs to update all masks on Slave */
    if( DIAGCOMM_IO_CTRL == conn->channel_type )
    {
      /* Send feature flag mask to remote procs */
      diag_set_internal_sigs( DIAG_INT_FEAT_UPDATE_SIG );
      
      for( preset_index=0; preset_index < DIAG_MAX_PRESET_ID; preset_index++ )
      {
        DIAG_SET_MASK(conn->mask_preset_update[preset_index], DIAG_UPDATE_MASK_F3);
        DIAG_SET_MASK(conn->mask_preset_update[preset_index], DIAG_UPDATE_MASK_LOGS);
        DIAG_SET_MASK(conn->mask_preset_update[preset_index], DIAG_UPDATE_MASK_EVENTS);
      }
      DIAG_SET_MASK(conn->mask_dci_update[DIAG_DCI_MASK_1-1], DIAG_UPDATE_MASK_F3);
      DIAG_SET_MASK(conn->mask_dci_update[DIAG_DCI_MASK_1-1], DIAG_UPDATE_MASK_LOGS);
      DIAG_SET_MASK(conn->mask_dci_update[DIAG_DCI_MASK_1-1], DIAG_UPDATE_MASK_EVENTS);
      
      diag_set_internal_sigs( DIAG_INT_MASK_UPDATE_SIG );
    }
  }
  else
  {
    conn->channel_connected = FALSE;
    
    for( preset_index=0; preset_index < DIAG_MAX_PRESET_ID; preset_index++ )
    {
      conn->mask_preset_update[preset_index] = DIAG_UPDATE_MASK_NONE;
    }
    conn->mask_dci_update[DIAG_DCI_MASK_1-1] = DIAG_UPDATE_MASK_NONE;
  }
#endif

  if( connected )
    conn->channel_connected = TRUE;
  else
    conn->channel_connected = FALSE;
	
} /* diagcomm_smd_conn_changed */


/*===========================================================================

FUNCTION DIAGCOMM_SMD_STATUS

DESCRIPTION
  This function returns TRUE if the SMD communications layer has been 
  opened locally and on the remote end.

PARAMETERS
  port_num - Data SMD channel to check for connection status
  
RETURN VALUE
  True - The Data SMD communications layer is opened locally and on the 
         remote end.
  False - Otherwise.

===========================================================================*/
boolean
diagcomm_smd_status ( diagcomm_enum_port_type port_num )
{
  return (diagcomm_io_conn[DIAGCOMM_PORT_SMD][port_num].connected && 
          diagcomm_io_conn[DIAGCOMM_PORT_SMD][port_num].channel_connected);
  
} /* diagcomm_smd_status */



/*===========================================================================

FUNCTION DIAGCOMM_SMD_PROCESS_SLAVE_TX

DESCRIPTION
  This function sends APP diag traffic which came from SMEM to SIO.

===========================================================================*/
void diagcomm_smd_process_slave_tx(diagcomm_enum_port_type port_num)
{
  static dsm_item_type *diagcomm_app_item_ptr = NULL;
  diagcomm_io_tx_params_type tx_params;
  
  if (DIAGCOMM_PORT_2 == port_num)
  {
    #if defined (DIAG_MP_MASTER)
	{}
	#else
	return;
	#endif
  }
   
   if (diagcomm_status(DIAGCOMM_PORT_1))
   {
     diagcomm_app_item_ptr = diagcomm_io_dequeue_rx_wmq( &diagcomm_io_conn[DIAGCOMM_PORT_SMD][port_num] );
     if(diagcomm_app_item_ptr!=NULL)
     {
       tx_params.dsm_ptr = diagcomm_app_item_ptr;
       diagcomm_io_transmit( &diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1], &tx_params );
     }
   }
} /* diagcomm_smd_process_slave_tx */


/*===========================================================================

FUNCTION DIAGCOMM_SMD_GET_RX_WMQ_CNT

DESCRIPTION
  This function returns forward RX watermark queue count.

===========================================================================*/
uint32 diagcomm_smd_get_rx_wmq_cnt(diagcomm_enum_port_type port_num)
{
  return diagcomm_io_get_rx_wmq_cnt( &diagcomm_io_conn[DIAGCOMM_PORT_SMD][port_num] );
  
} /* diagcomm_smd_get_rx_wmq_cnt */



/*===========================================================================

FUNCTION DIAGCOMM_SMD_INIT

DESCRIPTION
  This function initializes the diagnostic subsystem's communcations layer.

===========================================================================*/
void
diagcomm_smd_init (diagcomm_enum_port_type port_num)
{
  diagcomm_io_conn_type * conn = &diagcomm_io_conn[DIAGCOMM_PORT_SMD][port_num];
  
  /* Make sure we are closed */
  diagcomm_smd_close( port_num );

  /* Initialize watermark structures */
  memset (&diagcomm_smd_rx_wmq[conn->port_num], 0, sizeof (dsm_watermark_type));
  memset (&diagcomm_smd_tx_wmq[conn->port_num], 0, sizeof (dsm_watermark_type));

#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
  if (DIAGCOMM_PORT_1 == conn->port_num)
  {
  /* Adding flow control at pool's item-level granularity */
  /* Register the levels */
  dsm_reg_mem_event_level
  (
    DSM_DIAG_SMD_TX_ITEM_POOL,                                /* dsm_mempool_id_type pool_id */
    DSM_MEM_LEVEL_MANY,                                       /* dsm_mem_level_enum_type level */
    DSMI_DIAG_SMD_TX_ITEM_CNT-DSMI_DIAG_SMD_TX_ITEM_MANY_MARK /* uint32 avail_item_count */
  );
  dsm_reg_mem_event_level
  (
    DSM_DIAG_SMD_TX_ITEM_POOL,                               /* dsm_mempool_id_type pool_id */
    DSM_MEM_LEVEL_FEW,                                       /* dsm_mem_level_enum_type level */
    DSMI_DIAG_SMD_TX_ITEM_CNT-DSMI_DIAG_SMD_TX_ITEM_FEW_MARK /* uint32 avail_item_count */
  );

  /* Register the call-backs for the MANY and FEW levels */
  dsm_reg_mem_event_cb
  (
   DSM_DIAG_SMD_TX_ITEM_POOL,                                 /* dsm_mempool_id_type      pool_id*/
   DSM_MEM_LEVEL_MANY,                                        /* dsm_mem_level_enum_type  level */
   DSM_MEM_OP_NEW,                                            /* dsm_mem_op_enum_type     mem_op */
   diagcomm_smd_dsm_high_mem_event_cb                         /* pointer to call-back function */
  );
  dsm_reg_mem_event_cb
  (
   DSM_DIAG_SMD_TX_ITEM_POOL,                                 /* dsm_mempool_id_type      pool_id*/
   DSM_MEM_LEVEL_FEW,                                         /* dsm_mem_level_enum_type  level */
   DSM_MEM_OP_FREE,                                           /* dsm_mem_op_enum_type     mem_op */
   diagcomm_smd_dsm_low_mem_event_cb                          /* pointer to call-back function */
  );


  /* Set up Queue for receiving data from SIO. */
  dsm_queue_init
  (
    &diagcomm_smd_rx_wmq[conn->port_num],                     /* Watermark queue structure */
    DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ,      /* Do not exceed count */
    &diagcomm_smd_rx_q[conn->port_num]                        /* Queue to hold the items */
  );

  /* Set up watermarks for WaterMark Queue.  Also, set-up
     functions which will be called when the watermarks are
     reached. */
  /* Watermarks are not to be used, but the queue is; use inert values. */
    diagcomm_smd_rx_wmq[conn->port_num].dont_exceed_cnt = DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ;
 
    dsm_set_low_wm(&diagcomm_smd_rx_wmq[conn->port_num],
     (DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ));
    dsm_set_hi_wm(&diagcomm_smd_rx_wmq[conn->port_num],
     (DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ));


  /* Set up Queue for sending data via SIO. */
  dsm_queue_init
  (
    &diagcomm_smd_tx_wmq[conn->port_num],                     /* Watermark queue structure */
    DSM_DIAG_SMD_RX_ITEM_CNT * DSM_DIAG_SMD_RX_ITEM_SIZ,      /* Do not exceed count */
    &diagcomm_smd_tx_q[conn->port_num]                        /* Queue to hold the items */
  );

  /* Set up watermarks for WaterMark Queue. */
  /* Watermarks are not to be used, but the queue is; use inert values. */
    dsm_set_low_wm(&diagcomm_smd_tx_wmq[conn->port_num],
     (DSM_DIAG_SMD_RX_ITEM_CNT * DSM_DIAG_SMD_RX_ITEM_SIZ));
    dsm_set_hi_wm(&diagcomm_smd_tx_wmq[conn->port_num],
     (DSM_DIAG_SMD_RX_ITEM_CNT * DSM_DIAG_SMD_RX_ITEM_SIZ));
  }
  else if (DIAGCOMM_PORT_2 == conn->port_num)
  {
    /* Set up Queue for receiving data from Q6. */
    dsm_queue_init
    (
      &diagcomm_smd_rx_wmq[conn->port_num],                   /* Watermark queue structure */
      DSM_DIAG_SMD_RX_ITEM_CNT * DSM_DIAG_SMD_RX_ITEM_SIZ,    /* Do not exceed count */
      &diagcomm_smd_rx_q[conn->port_num]                      /* Queue to hold the items */
    );

    /* Set up watermarks for WaterMark Queue.  Also, set-up
     functions which will be called when the watermarks are
     reached. */
    /* Watermarks are not to be used, but the queue is; use inert values. */
    diagcomm_smd_rx_wmq[conn->port_num].lo_watermark = DSM_DIAG_SMD_RX_ITEM_CNT * DSM_DIAG_SMD_RX_ITEM_SIZ;
    diagcomm_smd_rx_wmq[conn->port_num].hi_watermark = DSM_DIAG_SMD_RX_ITEM_CNT * DSM_DIAG_SMD_RX_ITEM_SIZ;
    diagcomm_smd_rx_wmq[conn->port_num].dont_exceed_cnt = DSM_DIAG_SMD_RX_ITEM_CNT * DSM_DIAG_SMD_RX_ITEM_SIZ;
 
    dsm_set_low_wm(&diagcomm_smd_rx_wmq[conn->port_num],
     (DSM_DIAG_SMD_RX_ITEM_CNT * DSM_DIAG_SMD_RX_ITEM_SIZ));

    dsm_set_hi_wm(&diagcomm_smd_rx_wmq[conn->port_num],
     (DSM_DIAG_SMD_RX_ITEM_CNT * DSM_DIAG_SMD_RX_ITEM_SIZ));

    /* Set up Queue for sending data via SIO. */
    dsm_queue_init
    (
      &diagcomm_smd_tx_wmq[conn->port_num],                   /* Watermark queue structure */
      DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ,    /* Do not exceed count */
      &diagcomm_smd_tx_q[conn->port_num]                      /* Queue to hold the items */
    );

    /* Set up watermarks for WaterMark Queue.  Also, set-up functions
     which will be called when the watermarks are reached. */

    /* Watermarks are not to be used, but the queue is; use inert values. */
    dsm_set_low_wm(&diagcomm_smd_tx_wmq[conn->port_num],
     (DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ));
    dsm_set_hi_wm(&diagcomm_smd_tx_wmq[conn->port_num],
     (DSM_DIAG_SMD_TX_ITEM_CNT * DSM_DIAG_SMD_TX_ITEM_SIZ));
  }
#else

  /* Set up Queue for sending data via SIO. */
  dsm_queue_init
  (
    &diagcomm_smd_tx_wmq[conn->port_num],                     /* Watermark queue structure */
    DSM_DIAG_SIO_TX_ITEM_CNT * DSM_DIAG_SIO_TX_ITEM_SIZ,      /* Do not exceed count */
    &diagcomm_smd_tx_q[conn->port_num]                        /* Queue to hold the items */
  );

  /* Set up watermarks for WaterMark Queue. */
  /* Watermarks are not to be used, but the queue is; use inert values. */
  dsm_set_low_wm(&diagcomm_smd_tx_wmq[conn->port_num],
     (DSM_DIAG_SIO_TX_ITEM_CNT * DSM_DIAG_SIO_TX_ITEM_SIZ));
  dsm_set_hi_wm(&diagcomm_smd_tx_wmq[conn->port_num],
     (DSM_DIAG_SIO_TX_ITEM_CNT * DSM_DIAG_SIO_TX_ITEM_SIZ));
#endif

  diagcomm_io_open_init( conn );

} /* diagcomm_smd_init */


/*===========================================================================
FUNCTION DIAGCOMM_SMD_OPEN_INIT

DESCRIPTION

===========================================================================*/
#ifdef DIAG_SIO_SUPPORT
void diagcomm_smd_open_init( diagcomm_io_conn_type * conn )
{
  if( DIAGCOMM_PORT_SMD != conn->port_type )
    return;
    
  /* Now prepare parameters for opening a port */
  conn->open_params.sio_params.params.stream_mode = SIO_DM_MODE;
  conn->open_params.sio_params.params.tx_queue = &(diagcomm_smd_tx_wmq[conn->port_num]);

  /* Only use one queue/pool to support buffering mode. */
  conn->open_params.sio_params.params.rx_queue = &diagcomm_smd_rx_wmq[DIAGCOMM_PORT_1];
  
  /* Normally, this layer doesn't know about the HDLC encoding of the diag
     protocol.  However, SIO is configured to look for tail character. */
  conn->open_params.sio_params.params.tail_char_used = FALSE;
  conn->open_params.sio_params.params.tail_char = 0;

  /* Use whatever the flow control method is currently. */
  conn->open_params.sio_params.params.rx_flow = SIO_CTSRFR_FCTL;
  conn->open_params.sio_params.params.tx_flow = SIO_FCTL_OFF;

  /* Default DM bit rate is 38.4Kbps unless specified in NV.
     Now, it uses 115.2 (the max) as the default. */
  conn->open_params.sio_params.params.rx_bitrate = SIO_BITRATE_115200;
  conn->open_params.sio_params.params.tx_bitrate = SIO_BITRATE_115200;

  /* Setting callbacks */
  if( DIAGCOMM_PORT_1 == conn->port_num )
  {
    if( conn->open_params.sio_params.params.port_id == SIO_PORT_SMD_DIAG_APPS )
      conn->open_params.sio_params.params.rx_func_ptr = diagcomm_inbound_pkt_notify;
    else
      conn->open_params.sio_params.params.rx_func_ptr = diagcomm_smd_inbound_pkt_notify;
  }
  else
  {
    conn->open_params.sio_params.params.rx_func_ptr = diagcomm_smd_inbound_pkt_notify;
  }
  
  conn->open_params.sio_params.close_func_ptr = diagcomm_smd_close_done_cb;
  
  /* Passing the pool ID to SMD */
#if defined(DIAG_MP_MASTER)
  conn->open_params.sio_params.open_param.dsm_pool_param.dsm_mempool_id = DSM_DIAG_SMD_TX_ITEM_POOL;
#else
  conn->open_params.sio_params.open_param.dsm_pool_param.dsm_mempool_id = DSM_DIAG_SIO_TX_ITEM_POOL;
#endif
  
  conn->open_params.sio_params.open_param.dsm_pool_param.mempool_set = 0;
  conn->open_params.sio_params.open_param.dsm_pool_param.usb_mode = SIO_USB_MODE_LEGACY;

  conn->open_params.sio_params.params.open_param = &(conn->open_params.sio_params.open_param);
  
} /* diagcomm_smd_open_init */
#endif /* DIAG_SIO_SUPPORT */


#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
static boolean bAllowFlow = TRUE; /* Keep track of state of flow control. */

/*===========================================================================
FUNCTION diagcomm_smd_flow_enabled

DESCRIPTION
   This function tells whether the flow to DSM_DIAG_SMD_TX_ITEM_POOL is enabled 
   or not.
   
===========================================================================*/
boolean diagcomm_smd_flow_enabled(void)
{
  return bAllowFlow; 
  
} /* diagcomm_smd_flow_enabled */


/*===========================================================================
FUNCTION diagcomm_smd_dsm_high_mem_event_cb

DESCRIPTION
   This function gets triggered and disables flow, when the high item count 
   for DSM_DIAG_SMD_TX_ITEM_POOL is reached.
   (DSM_DIAG_SMD_TX_ITEM_POOL is used by smd, to receive data.)
   
===========================================================================*/
static void diagcomm_smd_dsm_high_mem_event_cb (dsm_mempool_id_type pool_id,
                                                dsm_mem_level_enum_type mem_level,
                                                dsm_mem_op_enum_type mem_op)
{
   uint32 flow_cnt = 0;
   
   /* Check to be defensive */
   if(pool_id != DSM_DIAG_SMD_TX_ITEM_POOL || 
      mem_level != DSM_MEM_LEVEL_MANY ||
      mem_op != DSM_MEM_OP_NEW)
      return;
	
   if (TRUE == bAllowFlow)
   {
      diagcomm_io_disable_flow( &diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1] );
      diagcomm_io_disable_flow( &diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_2] );

      bAllowFlow = FALSE;
      flow_cnt = diagcomm_incr_flow_ctrl_count(DIAGCOMM_PORT_SMD);
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "diagcomm_smd_dsm_high_mem_event_cb: disabled flow (cnt=%d)", flow_cnt);
   }

} /* diagcomm_smd_dsm_high_mem_event_cb */


/*===========================================================================
FUNCTION diagcomm_smd_dsm_low_mem_event_cb

DESCRIPTION
   This function gets triggered and enables flow, when the low item count 
   for DSM_DIAG_SMD_TX_ITEM_POOL is reached.
   (DSM_DIAG_SMD_TX_ITEM_POOL is used by smd, to receive data.)
   
===========================================================================*/
static void diagcomm_smd_dsm_low_mem_event_cb (dsm_mempool_id_type pool_id,
                                               dsm_mem_level_enum_type mem_level,
                                               dsm_mem_op_enum_type mem_op)
{
   osal_sigs_t return_sigs;
   int return_val = 0;
  
   /* check to be defensive */
   if(pool_id != DSM_DIAG_SMD_TX_ITEM_POOL || 
      mem_level != DSM_MEM_LEVEL_FEW ||
      mem_op != DSM_MEM_OP_FREE)
      return;
	
   if (FALSE == bAllowFlow)
   {
      diagcomm_io_enable_flow( &diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1] );
      diagcomm_io_enable_flow( &diagcomm_io_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_2] );
      
      bAllowFlow = TRUE;
	  	  
	  diag_tx_notify();
      return_val = osal_set_sigs(diag_task_tcb, DIAG_EVENT_DRAIN_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
	  
      MSG(MSG_SSID_DIAG,MSG_LEGACY_LOW, "diagcomm_smd_dsm_low_mem_event_cb: enabled flow");
   }

} /* diagcomm_smd_dsm_low_mem_event_cb */


/*===========================================================================
FUNCTION DIAGCOMM_SMD_SEND

DESCRIPTION
  Sends a packet over SMD channel
  
PARAMETERS
  req_pkt - packet to send
  pkt_len - length of 'req_pkt'
  port_num - channel to send pkt on
  
RETURN VALUE
  None
  
===========================================================================*/
void diagcomm_smd_send( void *req_pkt, uint16 pkt_len, 
                        diagcomm_enum_port_type port_num )
{
  diagcomm_io_tx_params_type tx_params;
  dsm_item_type *app_req_item_ptr = NULL;
  diagcomm_io_conn_type * conn = NULL;
  dsm_mempool_id_type pool_id;
  uint16 write_len = 0;
  
    ASSERT(req_pkt != NULL);

  if( DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][port_num].feature_mask, F_DIAG_REQ_RSP_CHANNEL) )
  {
    conn = &diagcomm_io_cmd_conn[DIAGCOMM_PORT_SMD][port_num];
	pool_id = DSM_DIAG_CMD_SMD_TX_ITEM_POOL;
  }
  else
  {
	conn = &diagcomm_io_conn[DIAGCOMM_PORT_SMD][port_num]; 
	pool_id = DSM_DIAG_SMD_RX_ITEM_POOL;
  }

  if( diagcomm_status() && diagcomm_io_status(conn) )
  {
	write_len = dsm_pushdown_tail(&app_req_item_ptr, req_pkt, pkt_len, pool_id); 
	
#if defined(DIAG_DSM_DEBUG)	
	ASSERT( write_len == pkt_len );
#endif
	
    if( write_len == pkt_len )
    { 
      tx_params.dsm_ptr = app_req_item_ptr;
      diagcomm_io_transmit( conn, &tx_params );
      diag_sleep_vote(DIAG_SLEEP_DIAG, TRUE);
    }
    else
    {
      dsm_free_packet(&app_req_item_ptr);
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "diagcomm_smd_send - Ran out of DSM items on pool %d", pool_id);
    }
  }
} /* diagcomm_smd_send */
#endif /* DIAG_MP && DIAG_MP_MASTER*/


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostic Communications for Diag Control Channel

General Description

Copyright (c) 2010-2014 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diagcomm_ctrl.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/14   rs      Added support for build mask centralization.  
01/30/14   ph      Removed the usage of macro DIAG_TASK_HANDLE().
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
12/20/13   rs      Ported LLVM warning fix. 
08/28/13   sr      Featurized the registration of low and high cb for flow 
                   control on ctrl channel are registered                     
08/02/13   rh      Added flow control logic for control channel pools
06/03/13   ph      Fixed a corner case where diagpkt_table_fwd_status was not updated.
04/03/13   sr      Replace memcpy() and memmove() with memscpy() and memsmove() 
01/03/13   sg      Command Response Channel separation
02/08/13   sr      Fixed compiler  and Klockwork warnings 
11/15/12   is      Support for preset masks
08/31/12   ra      Use accessor macros instead of accessing private task         
                   info directly; accomodated changes for User space
                   task priority changes
08/29/12   sa      Support for handling large control packets
                   in a chained DSM item
08/15/12   vk/ra   Modification for LogOnDemand algorithm change
08/10/12   rh      Ensure Master Diag always opens ports for LPASS
08/10/12   rh      Removed DIAG_MP_SLAVE_LAST
08/10/12   rh      Mainlining DIAG_CENTRAL_ROUTING
07/19/12   rh      Set Diag control channel size to 8kB 
03/02/12   sg      Added function to return the channel connected status
                   for the ctrl port, call diagcomm_io_detection() while
                   opening the ctrl channel to register with dtr changes
10/07/11   is      Support sending log mask per equipment id
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
08/11/11   wjg     Added function to handle processing DIAG_CTRL_MSG_SIG
04/27/11   is      Resolve compiler warnings
04/18/11   is      Ensure CTRL msg sizes are constant
03/29/11   sg      Central Routing changes for Q6
03/04/11   is      Support for Diag over SMD-Lite
03/02/11   hm      Support SIO control channel on single processor for
                   dual-mask feature 
12/20/10   mad     Changes for dual mask: Open SIO control port in MASTER DIAG
12/08/10   mad     do not open CTRL port if already open: do not reset 
                   diagcomm_ctrl_conn
10/14/10   mad     Diag now blocks until control port finishes closing
10/20/10   is      Resolve compilation issue in Apps only Diag
09/28/10   mad     Closing control port in diagcomm_ctrl_close
09/14/10   is      New control channel and central routing features 

===========================================================================*/

#include "diag_v.h"
#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "assert.h" /* For assert */
#endif

#include "diagbuf.h"         // For diagbuf_drain()
#include "diagcomm_ctrl.h"
#include "diagcomm_smd.h"
#include "diagdsmi.h"        // For DSMI_DIAG_SMD_TX_ITEM_CNT, etc.
#include "diagi_v.h"         // For DIAG_TX_SLAVE_SIG, diagpkt_process_ctrl_msg()
#include "diagpkt.h"         // For DIAGPKT_DISPATCH_TABLE_REGISTER_SLAVE()
#include "msg.h"             // For MSG_1, MSG_2 etc.
#include "osal.h"            // For osal* functions
#include "diagdsm_v.h"
#include "feature.h"         // For DIAG_INT_FEATURE_MASK_LEN
#include "stringl.h"

#ifdef DIAG_SIO_SUPPORT
  #include "diagcomm_io_sio.h" // For diagcomm_ctrl_open_cb
  #include "smd.h"
#endif

extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */

/* Queues around which the watermark queues are built */
static q_type diagcomm_ctrl_rx_q[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];
static q_type diagcomm_ctrl_tx_q[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];


/* Keep track of state of flow control on control channel. */
static boolean ctrl_tx_allow_flow = TRUE;

/* Watermark queues used for communications with SIO */
dsm_watermark_type diagcomm_ctrl_rx_wmq[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];
dsm_watermark_type diagcomm_ctrl_tx_wmq[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS];

/*-------------------------------------------------------------------------
                   Local function prototype
 ------------------------------------------------------------------------- */
#ifdef DIAG_SIO_SUPPORT 
static void diagcomm_ctrl_close_done_cb (void);
#endif /* DIAG_SIO_SUPPORT */

#ifdef DIAG_SMD_SUPPORT
void diagcomm_process_inbound_ctrl_msg( diagcomm_port_type port_type, diagcomm_enum_port_type ctrl_channel );
#endif /* DIAG_SMD_SUPPORT */

/*===========================================================================

FUNCTION DIAGCOMM_CTRL_SMD_INBOUND_PKT_NOTIFY

DESCRIPTION
  This is the callback SIO calls when data is received from DIAGCOMM_PORT_1 
  channel.

  This function enqueues the recieved data onto Diag's DIAGCOMM_PORT_1
  Rx watermark queue and sets the DIAG_CTRL_MSG_SIG signal to notify
  Diag to process the recieved ctrl data.

===========================================================================*/
#ifdef DIAG_SIO_SUPPORT
void
diagcomm_ctrl_smd_inbound_pkt_notify( dsm_item_type ** item_ptr )
{
  osal_sigs_t return_sigs;
  int return_val = 0;

  dsm_enqueue( diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_1].open_params.sio_params.params.rx_queue, item_ptr );

  /* Set signal to diag task to process ctrl traffic from slave */
  return_val = osal_set_sigs( diag_task_tcb, DIAG_CTRL_MSG_SIG, &return_sigs );
  ASSERT(OSAL_SUCCESS == return_val);

 } /* diagcomm_ctrl_smd_inbound_pkt_notify */
#endif /* DIAG_SIO_SUPPORT */

 
/*===========================================================================

FUNCTION DIAGCOMM_CTRL2_SMD_INBOUND_PKT_NOTIFY

DESCRIPTION
  This is the callback SIO calls when data is received from DIAGCOMM_PORT_2 
  channel.

  This function enqueues the recieved data onto Diag's DIAGCOMM_PORT_2
  Rx watermark queue and sets the DIAG_CTRL_MSG_SIG signal to notify
  Diag to process the recieved ctrl data.
  
===========================================================================*/
#ifdef DIAG_SIO_SUPPORT
void
diagcomm_ctrl2_smd_inbound_pkt_notify( dsm_item_type ** item_ptr )
{
  osal_sigs_t return_sigs;
  int return_val = 0;

  dsm_enqueue( diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SMD][DIAGCOMM_PORT_2].open_params.sio_params.params.rx_queue, item_ptr );

  /* Set signal to diag task to process ctrl traffic from slave */
  return_val = osal_set_sigs( diag_task_tcb, DIAG_CTRL_MSG_SIG, &return_sigs );
  ASSERT(OSAL_SUCCESS == return_val);

 } /* diagcomm_ctrl2_smd_inbound_pkt_notify */
 #endif /* DIAG_SIO_SUPPORT */
 
 
 /*===========================================================================

FUNCTION DIAGCOMM_CTRL_SIO_INBOUND_PKT_NOTIFY

DESCRIPTION
  This is the callback SIO calls when data is received from DIAGCOMM_PORT_1 
  channel.

  This function enqueues the recieved data onto Diag's DIAGCOMM_PORT_1
  Rx watermark queue and sets the DIAG_CTRL_MSG_SIG signal to notify
  Diag to process the recieved ctrl data.

===========================================================================*/
#ifdef DIAG_SIO_SUPPORT
void
diagcomm_ctrl_sio_inbound_pkt_notify( dsm_item_type ** item_ptr )
{
  osal_sigs_t return_sigs;
  int return_val = 0;

  dsm_enqueue( diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].open_params.sio_params.params.rx_queue, item_ptr );

  /* Set signal to diag task to process ctrl traffic from slave */
  return_val = osal_set_sigs( diag_task_tcb, DIAG_CTRL_MSG_SIG, &return_sigs );
  ASSERT(OSAL_SUCCESS == return_val);

 } /* diagcomm_ctrl_sio_inbound_pkt_notify */
#endif /* DIAG_SIO_SUPPORT */
  
 
/*===========================================================================

FUNCTION DIAGCOMM_CTRL2_SIO_INBOUND_PKT_NOTIFY

DESCRIPTION
  This is the callback SIO calls when data is received from DIAGCOMM_PORT_2 
  channel.

  This function enqueues the recieved data onto Diag's DIAGCOMM_PORT_2
  Rx watermark queue and sets the DIAG_CTRL_MSG_SIG signal to notify
  Diag to process the recieved ctrl data.
  
===========================================================================*/
#ifdef DIAG_SIO_SUPPORT
void
diagcomm_ctrl2_sio_inbound_pkt_notify( dsm_item_type ** item_ptr )
{
  osal_sigs_t return_sigs;
  int return_val = 0;

  dsm_enqueue( diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2].open_params.sio_params.params.rx_queue, item_ptr );

  /* Set signal to diag task to process ctrl traffic from slave */
  return_val = osal_set_sigs( diag_task_tcb, DIAG_CTRL_MSG_SIG, &return_sigs );
  ASSERT(OSAL_SUCCESS == return_val);

 } /* diagcomm_ctrl2_sio_inbound_pkt_notify */
#endif /* DIAG_SIO_SUPPORT */

#ifdef DIAG_SMD_SUPPORT
/*===========================================================================

FUNCTION DIAGCOMM_PROCESS_CTRL

DESCRIPTION
  Process control traffic when DIAG_CTRL_MSG_SIG received.
  
PARAMETERS
  None
  
RETURN VALUE
  None
  
===========================================================================*/
void
diagcomm_process_ctrl( void )
{
#if defined (DIAG_MP_MASTER) 
  diagcomm_process_inbound_ctrl_msg (DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_1);
  diagcomm_process_inbound_ctrl_msg (DIAGCOMM_PORT_SMD, DIAGCOMM_PORT_2);
#else
  diagcomm_process_inbound_ctrl_msg (DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1);
#endif // DIAG_MP_MASTER || DIAG_QDSP6_APPS_PROC
}

/*===========================================================================

FUNCTION DIAGCOMM_PROCESS_INBOUND_CTRL_MSG

DESCRIPTION
  Process inbound traffic recieved via control channel.
  
PARAMETERS
  port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel
  
RETURN VALUE
  None
  
===========================================================================*/
void
diagcomm_process_inbound_ctrl_msg( diagcomm_port_type port_type, 
                                   diagcomm_enum_port_type ctrl_channel )
{
  dsm_item_type *dsm_item_head_ptr = NULL;
  dsm_item_type *dsm_item_ptr = NULL;
  osal_sigs_t return_sigs;
  int return_val = 0;
  diagcomm_io_tx_params_type tx_params;
  diagcomm_io_conn_type * conn = &diagcomm_io_ctrl_conn[port_type][ctrl_channel];
  

//Total bytes in a DSM Chained Packet.
  uint32 total_req_bytes = 0;
//Number of bytes pulled up from the DSM Chained Packet.
  uint32 ctrl_pkt_len = 0;
  
//Buffer to copy the entire DSM packet (if chained)
  uint8 diag_ctrl_pkt_inbound[DIAG_CTRL_PKT_RX_MAX_CHAIN_SIZ] = {0};

   
  /* Retrieve CTRL Rx data from wmq queue if there's any */
  dsm_item_ptr = diagcomm_io_dequeue_rx_wmq( conn );
  /* Keep track of head of DSM item to free later */
  dsm_item_head_ptr = dsm_item_ptr;
  
  if( dsm_item_ptr != NULL )
  {
    /* data_ptr from DSM shouldn't be NULL */
    ASSERT( dsm_item_ptr->data_ptr != NULL );
  }
  else
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"diagcomm_process_inbound_ctrl_msg: Recvd NULL DSM");
    return;
  }
 
  /* DIAG_CTRL_MSG_DIAGDATA msgs are from a different pool and not intermixed with other ctrl msgs */
  if( diagpkt_ctrl_get_msg_type( (void*)dsm_item_head_ptr->data_ptr ) == DIAG_CTRL_MSG_DIAGDATA )
  {
    if( diagcomm_io_status(&diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1]) )
    {
       tx_params.dsm_ptr = dsm_item_head_ptr;
       diagcomm_io_transmit( &diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1], &tx_params );
    }
    else
    {
      (void) dsm_free_packet( &dsm_item_head_ptr );
    }
  }
  else
  {
  
    /* Check if its a chained DSM Packet. */
    if(dsm_item_ptr->pkt_ptr != NULL)
    {

      /* Get the total number of bytes in the dsm item chain. */
      total_req_bytes = dsm_length_packet(dsm_item_head_ptr);

      
      if(total_req_bytes > DIAG_CTRL_PKT_RX_MAX_CHAIN_SIZ)
      {
        dsm_free_packet(&dsm_item_head_ptr);
        MSG_2( MSG_SSID_DIAG, MSG_LEGACY_ERROR, 
             "Received Diag Ctrl Pkt Size is %d Max Size For Ctrl Pkt is %d \n", 
              total_req_bytes, sizeof(diag_ctrl_pkt_inbound));
        return ;
      }
      
      /* Pull up the total_req_bytes from the dsm item chain */
      ctrl_pkt_len = dsm_pullup_tail(&dsm_item_head_ptr, diag_ctrl_pkt_inbound, total_req_bytes);

      /* Send the entire DSM item chain  for processing */
      diagpkt_process_ctrl_msg( diag_ctrl_pkt_inbound, ctrl_pkt_len, port_type, ctrl_channel );
	}
	else
    {	
        diagpkt_process_ctrl_msg( dsm_item_ptr->data_ptr, dsm_item_ptr->used, port_type, ctrl_channel );
		/* Free DSM item when the entire chained DSM has finished processing */
		(void) dsm_free_packet( &dsm_item_head_ptr );
    }
  }
  
  
  /* Signal again if there's more CTRL Rx traffic to process */
  if( diagcomm_io_get_rx_wmq_cnt( conn ) > 0 )
  {
    return_val = osal_set_sigs( diag_task_tcb, DIAG_CTRL_MSG_SIG, &return_sigs );
    ASSERT( OSAL_SUCCESS == return_val );
  }
  
 } /* diagcomm_process_inbound_ctrl_msg */ 
#endif /* DIAG_SMD_SUPPORT */

/*===========================================================================

FUNCTION DIAGCOMM_CTRL_STATUS

DESCRIPTION
  This function returns TRUE if the communications layer has an open port.

PARAMETERS
  port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel
  
RETURN VALUE
  True if the logical control stream is connected.
  False otherwise.
  
===========================================================================*/
boolean
diagcomm_ctrl_status ( diagcomm_port_type port_type, 
                       diagcomm_enum_port_type ctrl_channel ) 
{
  return (diagcomm_io_ctrl_conn[port_type][ctrl_channel].connected &&
          diagcomm_io_ctrl_conn[port_type][ctrl_channel].channel_connected);
  
} /* diagcomm_ctrl_status */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_CHANNEL_STATUS

DESCRIPTION
  This function returns TRUE if the communications layer has an open port
  and the port is opened on the remote processor.

PARAMETERS
  port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel
  
RETURN VALUE
  True if the logical control stream is connected.
  False otherwise.
  
===========================================================================*/
boolean
diagcomm_ctrl_channel_status( diagcomm_port_type port_type,
                              diagcomm_enum_port_type ctrl_channel )
{
  return (diagcomm_io_ctrl_conn[port_type][ctrl_channel].connected && 
          diagcomm_io_ctrl_conn[port_type][ctrl_channel].channel_connected);
} /* diagcomm_ctrl_channel_status */


#if defined(DIAG_MP_MASTER)
/*===========================================================================

FUNCTION DIAGCOMM_CTRL_MASK_STATUS

DESCRIPTION
  This function checks internal bit-masks to determine if msg/event/logs have
  been updated and needs to be propagated to Slaves. Bit-masks are checked per
  mask type (msg/event/log) and per port/channel.


PARAMETERS
  port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel
  mask - One of the DIAG_UPDATE_MASK_* bit masks to check
  stream_id - Stream id mask to check
  preset_id - Preset id mask to check
  
RETURN VALUE
  TRUE - Mask is set
  FALSE - Otherwise
  
===========================================================================*/
boolean diagcomm_ctrl_mask_status( diagcomm_port_type port_type, 
                                   diagcomm_enum_port_type ctrl_channel,
                                   dword mask, 
                                   uint8 stream_id,
                                   uint8 preset_id )
{    
  ASSERT( (stream_id >= DIAG_MIN_STREAM_ID) && (stream_id <= DIAG_MAX_STREAM_ID) );
  ASSERT( (preset_id >= DIAG_MIN_PRESET_ID) && (preset_id <= DIAG_MAX_PRESET_ID) );

  if( stream_id == DIAG_STREAM_1 )
  {
    MSG_5(MSG_SSID_DIAG, MSG_LEGACY_LOW, "diagcomm_ctrl_mask_status port_num=%d stream=%d preset_id=%d cur_mask=%d mask=%d", 
         ctrl_channel, stream_id, preset_id, diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_preset_update[preset_id-1], mask);
    
    return DIAG_CHECK_MASK(diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_preset_update[preset_id-1], mask);
  }
  else if( stream_id == DIAG_STREAM_2 )
  {
    MSG_5(MSG_SSID_DIAG, MSG_LEGACY_LOW, "diagcomm_ctrl_mask_status port_num=%d stream=%d preset_id=%d cur_mask=%d mask=%d", 
          ctrl_channel, stream_id, preset_id, diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_dci_update[DIAG_DCI_MASK_1-1], mask);
    
    return DIAG_CHECK_MASK(diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_dci_update[DIAG_DCI_MASK_1-1], mask);
  }
  else
    return FALSE;
    
} /* diagcomm_ctrl_mask_status */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_CLEAR_MASK_STATUS

DESCRIPTION

PARAMETERS
  port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel
  mask
  stream_id
  preset_id
  
RETURN VALUE

===========================================================================*/
dword diagcomm_ctrl_clear_mask_status( diagcomm_port_type port_type, 
                                       diagcomm_enum_port_type ctrl_channel,
                                       dword mask,
                                       uint8 stream_id,
                                       uint8 preset_id )
{
  ASSERT( (stream_id >= DIAG_MIN_STREAM_ID) && (stream_id <= DIAG_MAX_STREAM_ID) );
  ASSERT( (preset_id >= DIAG_MIN_PRESET_ID) && (preset_id <= DIAG_MAX_PRESET_ID) );
    
  if( stream_id == DIAG_STREAM_1 )
  {
    DIAG_CLR_MASK(diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_preset_update[preset_id-1], mask);
    
    MSG_5(MSG_SSID_DIAG, MSG_LEGACY_LOW, "clear_mask_status port_num=%d stream=%d preset_id=%d clear=%d ret=%d", 
        ctrl_channel, stream_id, preset_id, mask, diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_preset_update[preset_id-1]);
    
    return diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_preset_update[preset_id-1];
  }
  else if( stream_id == DIAG_STREAM_2 )
  {
    DIAG_CLR_MASK(diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_dci_update[DIAG_DCI_MASK_1-1], mask);
    
    MSG_5(MSG_SSID_DIAG, MSG_LEGACY_LOW, "clear_mask_status port_num=%d stream=%d preset_id=%d clear=%d ret=%d", 
        ctrl_channel, stream_id, preset_id, mask, diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_dci_update[DIAG_DCI_MASK_1-1]);
    
    return diagcomm_io_ctrl_conn[port_type][ctrl_channel].mask_dci_update[DIAG_DCI_MASK_1-1];
  }
  else
    return FALSE;
  
} /* diagcomm_ctrl_clear_mask_status */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_MSG_MASK_STATUS

DESCRIPTION

PARAMETERS

RETURN VALUE

===========================================================================*/
boolean diagcomm_ctrl_msg_mask_status( diagcomm_port_type port_type, 
                                       diagcomm_enum_port_type ctrl_channel,
                                       uint8 stream_id,
                                       uint8 preset_id,
                                       uint8 index )
{
  ASSERT( (stream_id >= DIAG_MIN_STREAM_ID) && (stream_id <= DIAG_MAX_STREAM_ID) );
  ASSERT( (preset_id >= DIAG_MIN_PRESET_ID) && (preset_id <= DIAG_MAX_PRESET_ID) );
  
  if( stream_id == DIAG_STREAM_1 )
    return diagcomm_io_ctrl_conn[port_type][ctrl_channel].msg_mask_preset_update[preset_id-1][index];
#if defined(DIAG_STREAM_2_MASK_CHECK)
  else if( stream_id == DIAG_STREAM_2 )
    return diagcomm_io_ctrl_conn[port_type][ctrl_channel].msg_mask_dci_update[DIAG_DCI_MASK_1-1][index];
#endif
  else
    return FALSE;
  
} /* diagcomm_ctrl_msg_mask_status */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_CLEAR_MSG_MASK_STATUS

DESCRIPTION

PARAMETERS
  port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel
  stream_id - Mask stream ID
  
RETURN VALUE
  None
  
===========================================================================*/
void diagcomm_ctrl_clear_msg_mask_status( diagcomm_port_type port_type, 
                                          diagcomm_enum_port_type ctrl_channel,
                                          uint8 stream_id,
                                          uint8 preset_id )
{
  uint8 index = 0;
  
  ASSERT( (stream_id >= DIAG_MIN_STREAM_ID) && (stream_id <= DIAG_MAX_STREAM_ID) );
  ASSERT( (preset_id >= DIAG_MIN_PRESET_ID) && (preset_id <= DIAG_MAX_PRESET_ID) );
  
  for( index=0; index < MSG_MASK_TBL_CNT; index++ )
  {
    if( stream_id == DIAG_STREAM_1 )
      diagcomm_io_ctrl_conn[port_type][ctrl_channel].msg_mask_preset_update[preset_id-1][index] = FALSE;
#if defined(DIAG_STREAM_2_MASK_CHECK)
    else if( stream_id == DIAG_STREAM_2 )
      diagcomm_io_ctrl_conn[port_type][ctrl_channel].msg_mask_dci_update[DIAG_DCI_MASK_1-1][index] = FALSE;
#endif
  }
  
} /* diagcomm_ctrl_clear_msg_mask_status */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_LOG_MASK_STATUS

DESCRIPTION

PARAMETERS
  port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel
  stream_id - Mask stream ID
  equip_id - Log equipment id
  
RETURN VALUE

===========================================================================*/
boolean diagcomm_ctrl_log_mask_status( diagcomm_port_type port_type, 
                                       diagcomm_enum_port_type ctrl_channel,
                                       uint8 stream_id,
                                       uint8 equip_id )
{
  return diagcomm_io_ctrl_conn[port_type][ctrl_channel].log_mask_update[stream_id-1][equip_id];
  
} /* diagcomm_ctrl_log_mask_status */

#endif /* DIAG_MP_MASTER */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_PORT_ID

DESCRIPTION
  This function returns the SMD/SMDL port ID 
  
PARAMETERS
  port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel
  
RETURN VALUE
  Port ID of the control channel
  
===========================================================================*/
uint16
diagcomm_ctrl_port_id( diagcomm_port_type port_type,
                       diagcomm_enum_port_type ctrl_channel )
{
#if defined(DIAG_SIO_SUPPORT)
  return diagcomm_io_ctrl_conn[port_type][ctrl_channel].open_params.sio_params.params.port_id;
#elif defined(DIAG_SMDL_SUPPORT)
  return diagcomm_io_ctrl_conn[port_type][ctrl_channel].open_params.smdl_params.edge;
#else
  return 0;
#endif

} /* diagcomm_ctrl_port_id */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_OPEN

DESCRIPTION
  Opens the communcications stream.

PARAMETERS
  ctrl_port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel to open
  
RETURN VALUE
  True if control port opened successfully.
  False otherwise.
  
===========================================================================*/
boolean
diagcomm_ctrl_open( diagcomm_port_type port_type, diagcomm_enum_port_type ctrl_channel )
{
  diagcomm_io_conn_type * conn = &diagcomm_io_ctrl_conn[port_type][ctrl_channel];
  
#if defined(DIAG_MP_MASTER)
  uint8 preset_index;
#endif

  if (conn->connected)
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
      
#if defined(DIAG_MP_MASTER)
      // Master has opened CTRL port to Slave; Need to send mask updates to Slave
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
#endif
    }
    else
    {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"diagcomm_ctrl_open - Couldn't get stream.");
    }
  }
  
  return conn->connected;

} /* diagcomm_ctrl_open */


/*===========================================================================

FUNCTION DIAGCOMM_NEW_DSM

DESCRIPTION
  This function requests a new DSM buffer from a specified DSM pool.

PARAMETERS
  pool_id - DSM pool to allocate new DSM buffer from
  priority - If True, sets the DSM priority to high.
             If False, DSM is set to default priority.
  
RETURN VALUE
  Pointer to the new DSM item
  
===========================================================================*/
dsm_item_type *
diagcomm_new_dsm( dsm_mempool_id_type pool_id, boolean priority )
{
  dsm_item_type *item_ptr = NULL;
  
  item_ptr = dsm_new_buffer(pool_id);
  if( item_ptr ) 
  {  
    item_ptr->pkt_ptr = NULL;
    
    if( priority )
    {
      item_ptr->priority = DSM_HIGHEST;
    }
  }
  else
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"diagcomm_new_dsm - Ran out of DSM items from pool %d", pool_id);
  }
  
  return item_ptr;
  
} /* diagcomm_new_dsm */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_SEND_MULT_PKT

DESCRIPTION
  This function aggregates multiple ctrl packets to be sent in one DSM and
  transmits it via 'ctrl_channel' when it's full. Client may also opt to send
  the DSM immediately by setting 'send_now' to True.

PARAMETERS
  ctrl_pkt - The data packet to aggregate into the DSM
  ctrl_pktlen - The length of 'ctrl_pkt'
  user_pkt - Additional data packet to aggregate into the DSM.
             Used for additional variable length data appended to 'ctrl_pkt'.
  user_pktlen - The length of 'user_pktlen'
  port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel to transmit the DSM to
  send_now - If True, aggregate 'ctrl_pkt' into the DSM (if any) and transmit the DSM
               immediately.
             If False, aggregate 'ctrl_pkt' into the DSM and transmit the DSM only
               when full.
  is_sent [out] - Returns True if a DSM item was sent. False otherwise.
  
RETURN VALUE
  The number of bytes newly aggregated into the current DSM
  
 WARNING
  This function may only be called by the diag task.  It is not re-entrant!
  
===========================================================================*/
uint32
diagcomm_ctrl_send_mult_pkt( PACK(void *) ctrl_pkt, 
                             uint32 ctrl_pktlen, 
                             PACK(const void *) user_pkt, 
                             uint32 user_pktlen,
                             diagcomm_port_type port_type,
                             diagcomm_enum_port_type ctrl_channel,
                             boolean send_now,
                             boolean * is_sent )
{
  uint32 added_pkt_len = 0;
  uint8 * cur_data_ptr = NULL;
  uint8 * diag_send_ctrl_buf_pointer = NULL;
  boolean retry = FALSE;
  diagcomm_io_tx_params_type tx_params;
  /* Persistent data. Indicates dsm item chain to be sent  */
  static boolean send_cur_dsm = FALSE;
  
  /* Persistent data. This makes this function not re-entrant! */
  static dsm_item_type * dsm_item_ptr_sav = NULL;

/* Buffer to copy the control packet to a DSM Chain */
  uint8 diag_send_ctrl_buf[DIAG_CTRL_PKT_TX_MAX_CHAIN_SIZ] = {0};
  uint32 writelen = 0;
  
  /* Sanity check */
  if(( !is_sent) || (ctrl_pktlen > DIAG_CTRL_PKT_TX_MAX_CHAIN_SIZ))
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"diagcomm_ctrl_send_mult_pkt - Sanity check failed");
    return added_pkt_len;
  }
  *is_sent = FALSE;
  
  if( !ctrl_pkt || (ctrl_pktlen <= 0))
  {
    if( send_now && (dsm_item_ptr_sav != NULL) )
    {
      /* Just send immediately if there's no pkt to write */
      tx_params.dsm_ptr = dsm_item_ptr_sav;
      diagcomm_io_transmit( &diagcomm_io_ctrl_conn[port_type][ctrl_channel], &tx_params );
      *is_sent = TRUE;
      dsm_item_ptr_sav = NULL;
    }
    else
    {
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_LOW,"diagcomm_ctrl_send_mult_pkt - No pkt to write 0x%x %d", ctrl_pkt, ctrl_pktlen);
    }
    return added_pkt_len;
  }

  if( !diagcomm_sio_ctrl_flow_enabled() )
  {
    return added_pkt_len;
  }
  do
  {
    retry = FALSE;
    if(send_cur_dsm)
    {
        if(dsm_item_ptr_sav)
        {
            send_cur_dsm = FALSE;
            /* Send the chained DSM immediately */
            tx_params.dsm_ptr = dsm_item_ptr_sav;
            diagcomm_io_transmit( &diagcomm_io_ctrl_conn[port_type][ctrl_channel], &tx_params);
            *is_sent = TRUE;
            dsm_item_ptr_sav = NULL;
        }
    }

    if( dsm_item_ptr_sav == NULL )
    {
      #if defined(DIAG_MP_MASTER)
        /* Master sends to Slaves with SMD pools */
        dsm_item_ptr_sav = diagcomm_new_dsm( DSM_DIAG_SMD_CTRL_TX_ITEM_POOL, FALSE );
      #else
        /* Slave sends to Master with SIO pools */
        dsm_item_ptr_sav = diagcomm_new_dsm( DSM_DIAG_SIO_CTRL_TX_ITEM_POOL, FALSE );
      #endif
    }
  
    if( dsm_item_ptr_sav )
    {
      
    /* Chain DSM items only if the control packet size is 
    greater than the DSM item size */
    if((ctrl_pktlen +  user_pktlen) > dsm_item_ptr_sav->size)
    {
        /* if the previously allocated  DSM item is already filled by control packets, 
    transmit it and allocate a new DSM Item for this control packet */
      if(dsm_item_ptr_sav->used > 0)
      {
        /* Send the current DSM */
           tx_params.dsm_ptr = dsm_item_ptr_sav;
           diagcomm_io_transmit( &diagcomm_io_ctrl_conn[port_type][ctrl_channel], &tx_params );
           *is_sent = TRUE;
           dsm_item_ptr_sav = NULL;
        
          /* Retry writing this control pkt into new DSM item */
           retry = TRUE;
      }
    
      if(dsm_item_ptr_sav)
      {

        diag_send_ctrl_buf_pointer = (uint8*)ctrl_pkt;
		added_pkt_len = ctrl_pktlen;

        /* if user pkt if not NULL, Copy the ctrl data and user data in a buffer */
        if(user_pkt != NULL && user_pktlen > 0)
        {
          diag_send_ctrl_buf_pointer = diag_send_ctrl_buf;
          memscpy(diag_send_ctrl_buf, sizeof(diag_send_ctrl_buf), (const void*)ctrl_pkt, ctrl_pktlen);
        
          /* Append the user data */
		  memscpy(&diag_send_ctrl_buf[ctrl_pktlen],sizeof(diag_send_ctrl_buf)-ctrl_pktlen ,(const void*)user_pkt, user_pktlen);
		  added_pkt_len += user_pktlen;
        }
            
          /*Copy the entire control packet in a chained DSM item */
#if defined(DIAG_MP_MASTER)
        writelen = dsm_pushdown_tail(&dsm_item_ptr_sav, diag_send_ctrl_buf_pointer, (ctrl_pktlen + user_pktlen),
                          DSM_DIAG_SMD_CTRL_TX_ITEM_POOL);
#else
        writelen = dsm_pushdown_tail(&dsm_item_ptr_sav, diag_send_ctrl_buf_pointer, (ctrl_pktlen + user_pktlen),
                          DSM_DIAG_SIO_CTRL_TX_ITEM_POOL);
#endif

        /*Transmit if the entire control packet is copied in the DSM Chain */
        if(writelen == (ctrl_pktlen + user_pktlen))
        {
            send_cur_dsm = TRUE;
        }

        if( send_now )
        {
          /* Send the current DSM immediately */
          tx_params.dsm_ptr = dsm_item_ptr_sav;
          diagcomm_io_transmit( &diagcomm_io_ctrl_conn[port_type][ctrl_channel], &tx_params);
          *is_sent = TRUE;
          send_cur_dsm = FALSE;
          dsm_item_ptr_sav = NULL;
        }
      }         
    }
    else if( (dsm_item_ptr_sav->used + ctrl_pktlen + user_pktlen) < dsm_item_ptr_sav->size ) 
    {
        /* Append ctrl_pkt to DSM */
        cur_data_ptr = dsm_item_ptr_sav->data_ptr + dsm_item_ptr_sav->used;
        memscpy( cur_data_ptr, (dsm_item_ptr_sav->size - dsm_item_ptr_sav->used), (void *)ctrl_pkt, ctrl_pktlen );
        dsm_item_ptr_sav->used += (uint16)ctrl_pktlen;
        added_pkt_len = ctrl_pktlen;
        
        /* Append user_pkt to DSM, if any */
        if( (user_pkt != NULL) && (user_pktlen > 0) )
        {
          cur_data_ptr = dsm_item_ptr_sav->data_ptr + dsm_item_ptr_sav->used;
          memscpy( cur_data_ptr, (dsm_item_ptr_sav->size - dsm_item_ptr_sav->used),
                  (void *)user_pkt, user_pktlen );
          dsm_item_ptr_sav->used += (uint16)user_pktlen;
          added_pkt_len += user_pktlen;
        }
        
        if( send_now )
        {
			
          /* Send the current DSM immediately */
          tx_params.dsm_ptr = dsm_item_ptr_sav;
          diagcomm_io_transmit( &diagcomm_io_ctrl_conn[port_type][ctrl_channel], &tx_params);
          *is_sent = TRUE;
          dsm_item_ptr_sav = NULL;
        }
      }
      else
      {
        /* Send the current DSM since it's too full for additional pkts */
        send_cur_dsm = TRUE;
        
        /* Retry writing pkt into new DSM item */
        retry = TRUE;
      }
    }
  }	while( retry );

  return added_pkt_len;
  
} /* diagcomm_ctrl_send_mult_pkt */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_CLOSE

DESCRIPTION
  Closes the current control channel connection, if any.

PARAMETERS
  *close_func_ptr - Pointer to function to execute
  port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel to close
 
RETURN VALUE
  None
  
===========================================================================*/
void
diagcomm_ctrl_close( diagcomm_port_type port_type, diagcomm_enum_port_type ctrl_channel )
{
  diagcomm_io_conn_type * conn = &diagcomm_io_ctrl_conn[port_type][ctrl_channel];
  
  /* Got a command to close the connection. */
  if( conn->connected )
  {
    diagcomm_io_close( conn );
  }

  /* success or not, consider close operation successful  */
  conn->connected = FALSE;

} /* diagcomm_ctrl_close */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_CLOSE_DONE_CB

DESCRIPTION

PARAMETERS
 
RETURN VALUE
  None
  
===========================================================================*/
#ifdef DIAG_SIO_SUPPORT 
static void diagcomm_ctrl_close_done_cb (void)
{
  diag_release ();
  
} /* diagcomm_ctrl_close_done_cb */
#endif /* DIAG_SIO_SUPPORT */

#if defined(DIAG_MP) && !defined (DIAG_MP_MASTER)
static boolean ctrl_allow_flow = TRUE; /* Keep track of state of flow control. */
static uint32 diag_ctrl_tx_flow_ctrl_count = 0;
/*===========================================================================
FUNCTION diagcomm_sio_dsm_high_mem_event_cb

DESCRIPTION
   This function gets triggered and disables flow, when the high item count 
   for DSM_DIAG_SIO_CTRL_TX_ITEM_POOL is reached.
   (DSM_DIAG_SIO_CTRL_TX_ITEM_POOL is used by sio, to receive data.)
   
===========================================================================*/
static void diagcomm_sio_ctrl_dsm_high_mem_event_cb (dsm_mempool_id_type pool_id,
                                                   dsm_mem_level_enum_type mem_level,
                                                   dsm_mem_op_enum_type mem_op)
{
   /* check to be defensive */
   if(pool_id != DSM_DIAG_SIO_CTRL_RX_ITEM_POOL || 
      mem_level != DSM_MEM_LEVEL_MANY ||
      mem_op != DSM_MEM_OP_NEW)
      return;

   if (TRUE == ctrl_allow_flow)
   {
      diagcomm_io_disable_flow( &diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1] );
      diagcomm_io_disable_flow( &diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2] );

      ctrl_allow_flow = FALSE;
      diagcomm_incr_flow_ctrl_count(DIAGCOMM_PORT_SIO);
      MSG(MSG_SSID_DIAG,MSG_LEGACY_LOW, "diagcomm_smd_dsm_high_mem_event_cb: disabled flow");
   }
} /* diagcomm_sio_dsm_high_mem_event_cb */


/*===========================================================================
FUNCTION diagcomm_sio_dsm_low_mem_event_cb

DESCRIPTION
   This function gets triggered and enables flow, when the low item count 
   for DSM_DIAG_SIO_CTRL_RX_ITEM_POOL is reached.
   (DSM_DIAG_SIO_CTRL_RX_ITEM_POOL is used by sio, to receive data.)
   
===========================================================================*/
static void diagcomm_sio_ctrl_dsm_low_mem_event_cb (dsm_mempool_id_type pool_id,
                                                  dsm_mem_level_enum_type mem_level,
                                                  dsm_mem_op_enum_type mem_op)
{
      /* check to be defensive */
   if(pool_id != DSM_DIAG_SIO_CTRL_RX_ITEM_POOL || 
      mem_level != DSM_MEM_LEVEL_FEW ||
      mem_op != DSM_MEM_OP_FREE)
      return;
   
   if (FALSE == ctrl_allow_flow)
   {
      diagcomm_io_enable_flow( &diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1] );
      diagcomm_io_enable_flow( &diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_2] );

      ctrl_allow_flow = TRUE;
      MSG(MSG_SSID_DIAG,MSG_LEGACY_LOW, "diagcomm_smd_dsm_low_mem_event_cb: enabled flow");
   }

} /* diagcomm_sio_dsm_low_mem_event_cb */

/*===========================================================================
FUNCTION DIAG_CTRL_INCR_TX_FLOW_CTRL_COUNT

DESCRIPTION
    This function increments the internal variable diag_ctrl_tx_flow_ctrl_count,
	 which keeps track of the # of times flow control is triggered.
    
===========================================================================*/
static void diag_ctrl_incr_tx_flow_ctrl_count(void)
{
  if( diag_ctrl_tx_flow_ctrl_count == MAX_VALUE_UINT32 ) 
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "diag_ctrl_tx_flow_ctrl_count has reached max value");
  } 
  else 
  {
    diag_ctrl_tx_flow_ctrl_count += 1;
  }
  
} /* diag_ctrl_incr_tx_flow_ctrl_count */

/*===========================================================================
FUNCTION diagcomm_sio_ctrl_tx_dsm_high_mem_event_cb

DESCRIPTION
   This function gets triggered and disables flow, when the high item count 
   for DSM_DIAG_SIO_CTRL_TX_ITEM_POOL is reached.
   (DSM_DIAG_SIO_CTRL_TX_ITEM_POOL is used by sio, to send data.)
   
===========================================================================*/
static void diagcomm_sio_ctrl_tx_dsm_high_mem_event_cb (dsm_mempool_id_type pool_id,
                                                   dsm_mem_level_enum_type mem_level,
                                                   dsm_mem_op_enum_type mem_op)
{
   /* check to be defensive */
   if((pool_id != DSM_DIAG_SIO_CTRL_TX_ITEM_POOL) || 
      (mem_level != DSM_MEM_LEVEL_MANY) ||
      (mem_op != DSM_MEM_OP_NEW))
		 return;

   if (TRUE == ctrl_tx_allow_flow)
   {
		 diag_ctrl_incr_tx_flow_ctrl_count();
		 ctrl_tx_allow_flow = FALSE;
		 MSG(MSG_SSID_DIAG,MSG_LEGACY_LOW, "diagcomm_sio_ctrl_tx_dsm_high_mem_event_cb: disabled flow");
   }
} /* diagcomm_sio_ctrl_tx_dsm_high_mem_event_cb */


/*===========================================================================
FUNCTION diagcomm_sio_ctrl_tx_dsm_low_mem_event_cb

DESCRIPTION
   This function gets triggered and enables flow, when the low item count 
   for DSM_DIAG_SIO_CTRL_TX_ITEM_POOL is reached.
   (DSM_DIAG_SIO_CTRL_TX_ITEM_POOL is used by sio, to send data.)
   
===========================================================================*/
static void diagcomm_sio_ctrl_tx_dsm_low_mem_event_cb (dsm_mempool_id_type pool_id,
                                                  dsm_mem_level_enum_type mem_level,
                                                  dsm_mem_op_enum_type mem_op)
{
      /* check to be defensive */
   if((pool_id != DSM_DIAG_SIO_CTRL_TX_ITEM_POOL) || 
      (mem_level != DSM_MEM_LEVEL_FEW) ||
      (mem_op != DSM_MEM_OP_FREE))
      return;
   
   if (FALSE == ctrl_tx_allow_flow)
   {
		 ctrl_tx_allow_flow = TRUE;

		 /* Make sure all the updates are sent to APPS */
		 (void)diag_set_internal_sigs( DIAG_INT_FEAT_UPDATE_SIG );
		 (void)diag_set_internal_sigs( DIAG_INT_CTRL_REG_SIG );
		 (void)diag_set_internal_sigs( DIAG_INT_SEND_BT_MASK_SIG );

		  MSG(MSG_SSID_DIAG,MSG_LEGACY_LOW, "diagcomm_sio_ctrl_tx_dsm_low_mem_event_cb: enabled flow");
   }

} /* diagcomm_sio_ctrl_tx_dsm_low_mem_event_cb */
#endif

/*===========================================================================

FUNCTION DIAGCOMM_CTRL_INIT

DESCRIPTION
  This function initializes the diagnostic subsystem's communcations layer.

PARAMETERS
  port_type - Control channel port type (SIO/SMD)
  ctrl_channel - Control channel
  
RETURN VALUE
  None
  
===========================================================================*/
void
diagcomm_ctrl_init( diagcomm_port_type port_type, diagcomm_enum_port_type port_num )
{  
  diagcomm_io_conn_type * conn = &diagcomm_io_ctrl_conn[port_type][port_num];
  
    /* Make sure we are closed */
  diagcomm_ctrl_close( port_type, port_num );

  /* Initialize watermark structures */
  memset( &diagcomm_ctrl_rx_wmq[port_type][port_num], 0, sizeof (dsm_watermark_type) );
  memset( &diagcomm_ctrl_tx_wmq[port_type][port_num], 0, sizeof (dsm_watermark_type) );
  
#if defined(DIAG_MP) && defined(DIAG_MP_MASTER)
  if( (conn->port_type == DIAGCOMM_PORT_SMD) && (DIAGCOMM_PORT_1 == port_num) )
  {
    /* Set up Queue for receiving data from SIO. */
    dsm_queue_init
    (
      &diagcomm_ctrl_rx_wmq[port_type][port_num],                /* Watermark queue structure */
      DSM_DIAG_SMD_CTRL_RX_ITEM_CNT * DSM_DIAG_SMD_CTRL_RX_ITEM_SIZ,       /* Do not exceed count */
      &diagcomm_ctrl_rx_q[port_type][port_num]                   /* Queue to hold the items */
    );

    /* Set up watermarks for WaterMark Queue. Also, set-up
       functions which will be called when the watermarks are reached. */
    /* Watermarks are not to be used, but the queue is; use inert values. */
    diagcomm_ctrl_rx_wmq[port_type][port_num].dont_exceed_cnt = DSM_DIAG_SMD_CTRL_RX_ITEM_CNT * DSM_DIAG_SMD_CTRL_RX_ITEM_SIZ;
 
    dsm_set_low_wm
    (
      &diagcomm_ctrl_rx_wmq[port_type][port_num],
      (DSM_DIAG_SMD_CTRL_RX_ITEM_CNT * DSM_DIAG_SMD_CTRL_RX_ITEM_SIZ));
      dsm_set_hi_wm(&diagcomm_ctrl_rx_wmq[port_type][port_num],
      (DSM_DIAG_SMD_CTRL_RX_ITEM_CNT * DSM_DIAG_SMD_CTRL_RX_ITEM_SIZ)
    );

    /* Set up Queue for sending data via SIO. */
    dsm_queue_init
    (
      &diagcomm_ctrl_tx_wmq[port_type][port_num],                /* Watermark queue structure */
      DSM_DIAG_SMD_CTRL_TX_ITEM_CNT * DSM_DIAG_SMD_CTRL_TX_ITEM_SIZ,       /* Do not exceed count */
      &diagcomm_ctrl_tx_q[port_type][port_num]                   /* Queue to hold the items */
    );

    /* Set up watermarks for WaterMark Queue. */
    /* Watermarks are not to be used, but the queue is; use inert values. */
    dsm_set_low_wm
    (
      &diagcomm_ctrl_tx_wmq[port_type][port_num],
      (DSM_DIAG_SMD_CTRL_TX_ITEM_CNT * DSM_DIAG_SMD_CTRL_TX_ITEM_SIZ));
      dsm_set_hi_wm(&diagcomm_ctrl_tx_wmq[port_type][port_num],
      (DSM_DIAG_SMD_CTRL_TX_ITEM_CNT * DSM_DIAG_SMD_CTRL_TX_ITEM_SIZ)
    );
  }
  else if( (DIAGCOMM_PORT_SMD == conn->port_type) && (DIAGCOMM_PORT_2 == port_num) )
  {
    /* Set up Queue for receiving data from Q6. */
    dsm_queue_init
    (
      &diagcomm_ctrl_rx_wmq[port_type][port_num],                /* Watermark queue structure */
      DSM_DIAG_SMD_CTRL_RX_ITEM_CNT * DSM_DIAG_SMD_CTRL_RX_ITEM_SIZ,       /* Do not exceed count */
      &diagcomm_ctrl_rx_q[port_type][port_num]                   /* Queue to hold the items */
    );

    /* Set up watermarks for WaterMark Queue.  Also, set-up
     functions which will be called when the watermarks are reached. */
    /* Watermarks are not to be used, but the queue is; use inert values. */
    diagcomm_ctrl_rx_wmq[port_type][port_num].lo_watermark = DSM_DIAG_SMD_CTRL_RX_ITEM_CNT * DSM_DIAG_SMD_CTRL_RX_ITEM_SIZ;
    diagcomm_ctrl_rx_wmq[port_type][port_num].hi_watermark = DSM_DIAG_SMD_CTRL_RX_ITEM_CNT * DSM_DIAG_SMD_CTRL_RX_ITEM_SIZ;
    diagcomm_ctrl_rx_wmq[port_type][port_num].dont_exceed_cnt = DSM_DIAG_SMD_CTRL_RX_ITEM_CNT * DSM_DIAG_SMD_CTRL_RX_ITEM_SIZ;
 
    dsm_set_low_wm
    (
      &diagcomm_ctrl_rx_wmq[port_type][port_num],
      (DSM_DIAG_SMD_CTRL_RX_ITEM_CNT * DSM_DIAG_SMD_CTRL_RX_ITEM_SIZ)
    );

    dsm_set_hi_wm
    (
      &diagcomm_ctrl_rx_wmq[port_type][port_num],
      (DSM_DIAG_SMD_CTRL_RX_ITEM_CNT * DSM_DIAG_SMD_CTRL_RX_ITEM_SIZ)
    );

    /* Set up Queue for sending data via SIO. */
    dsm_queue_init
    (
      &diagcomm_ctrl_tx_wmq[port_type][port_num],                /* Watermark queue structure */
      DSM_DIAG_SMD_CTRL_TX_ITEM_CNT * DSM_DIAG_SMD_CTRL_TX_ITEM_SIZ,       /* Do not exceed count */
      &diagcomm_ctrl_tx_q[port_type][port_num]                   /* Queue to hold the items */
    );

    /* Set up watermarks for WaterMark Queue.  Also, set-up functions
       which will be called when the watermarks are reached. */

    /* Watermarks are not to be used, but the queue is; use inert values. */
    dsm_set_low_wm
    (
      &diagcomm_ctrl_tx_wmq[port_type][port_num],
      (DSM_DIAG_SMD_CTRL_TX_ITEM_CNT * DSM_DIAG_SMD_CTRL_TX_ITEM_SIZ));
      dsm_set_hi_wm(&diagcomm_ctrl_tx_wmq[port_type][port_num],
      (DSM_DIAG_SMD_CTRL_TX_ITEM_CNT * DSM_DIAG_SMD_CTRL_TX_ITEM_SIZ)
    );
  }
#else

  if( port_type == DIAGCOMM_PORT_SIO )
  {
    /* Set up Queue for receiving data from SIO. */
    dsm_queue_init
    (
      &diagcomm_ctrl_rx_wmq[port_type][port_num],                /* Watermark queue structure */
      DSM_DIAG_SIO_CTRL_RX_ITEM_CNT * DSM_DIAG_SIO_CTRL_RX_ITEM_SIZ,       /* Do not exceed count */
      &diagcomm_ctrl_rx_q[port_type][port_num]                   /* Queue to hold the items */
    );

    /* Set up watermarks for WaterMark Queue. Also, set-up
       functions which will be called when the watermarks are reached. */
       
    /* Watermarks are not to be used, but the queue is; use inert values. */
    diagcomm_ctrl_rx_wmq[port_type][port_num].dont_exceed_cnt = DSM_DIAG_SIO_CTRL_RX_ITEM_CNT * DSM_DIAG_SIO_CTRL_RX_ITEM_SIZ;

    dsm_set_low_wm
    (
      &diagcomm_ctrl_rx_wmq[port_type][port_num],
      (DSM_DIAG_SIO_CTRL_RX_ITEM_CNT * DSM_DIAG_SIO_CTRL_RX_ITEM_SIZ));
      dsm_set_hi_wm(&diagcomm_ctrl_rx_wmq[port_type][port_num],
      (DSM_DIAG_SIO_CTRL_RX_ITEM_CNT * DSM_DIAG_SIO_CTRL_RX_ITEM_SIZ)
    );
            
    /* Set up Queue for sending data via SIO. */
    dsm_queue_init
    (
      &diagcomm_ctrl_tx_wmq[port_type][port_num],                /* Watermark queue structure */
		DSM_DIAG_SIO_CTRL_TX_ITEM_CNT * DSM_DIAG_SIO_CTRL_TX_ITEM_SIZ,       /* Do not exceed count */
      &diagcomm_ctrl_tx_q[port_type][port_num]                   /* Queue to hold the items */
    );

    /* Set up watermarks for WaterMark Queue. */
    /* Watermarks are not to be used, but the queue is; use inert values. */
    dsm_set_low_wm
    (
      &diagcomm_ctrl_tx_wmq[port_type][port_num],
      (DSM_DIAG_SIO_CTRL_TX_ITEM_CNT * DSM_DIAG_SIO_CTRL_TX_ITEM_SIZ));
      dsm_set_hi_wm(&diagcomm_ctrl_tx_wmq[port_type][port_num],
      (DSM_DIAG_SIO_CTRL_TX_ITEM_CNT * DSM_DIAG_SIO_CTRL_TX_ITEM_SIZ)
    );
  }
#endif /* DIAG_MP_MASTER && DIAG_MP */

#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)

  /* Set the DSM flow-control parameters */
    dsm_reg_mem_event_level
    (
      DSM_DIAG_SIO_CTRL_RX_ITEM_POOL, /* dsm_mempool_id_type pool_id */
      DSM_MEM_LEVEL_MANY,        /*dsm_mem_level_enum_type level */
      DSMI_DIAG_SIO_CTRL_RX_ITEM_CNT-DSMI_DIAG_SIO_CTRL_RX_ITEM_MANY_MARK /* uint32 avail_item_count */
    );
    dsm_reg_mem_event_level
    (
      DSM_DIAG_SIO_CTRL_RX_ITEM_POOL, /* dsm_mempool_id_type pool_id */
      DSM_MEM_LEVEL_FEW,        /*dsm_mem_level_enum_type level */
      DSMI_DIAG_SIO_CTRL_RX_ITEM_CNT-DSMI_DIAG_SIO_CTRL_RX_ITEM_FEW_MARK/* uint32 avail_item_count */
    );
  
   /* Register the call-backs for the MANY and FEW levels */
    dsm_reg_mem_event_cb
    (
      DSM_DIAG_SIO_CTRL_RX_ITEM_POOL, /* dsm_mempool_id_type      pool_id*/
      DSM_MEM_LEVEL_MANY,        /*dsm_mem_level_enum_type  level */
      DSM_MEM_OP_NEW,             /* dsm_mem_op_enum_type     mem_op */
      diagcomm_sio_ctrl_dsm_high_mem_event_cb  /* pointer to call-back function */
    );
    dsm_reg_mem_event_cb
    (
      DSM_DIAG_SIO_CTRL_RX_ITEM_POOL, /* dsm_mempool_id_type      pool_id*/
      DSM_MEM_LEVEL_FEW,        /*dsm_mem_level_enum_type  level */
      DSM_MEM_OP_FREE,             /* dsm_mem_op_enum_type     mem_op */
      diagcomm_sio_ctrl_dsm_low_mem_event_cb  /* pointer to call-back function */
    );

  /* Set the DSM flow-control parameters */
    dsm_reg_mem_event_level
    (
      DSM_DIAG_SIO_CTRL_TX_ITEM_POOL, /* dsm_mempool_id_type pool_id */
      DSM_MEM_LEVEL_MANY,        /*dsm_mem_level_enum_type level */
      DSMI_DIAG_SIO_CTRL_TX_ITEM_CNT-DSMI_DIAG_SIO_CTRL_TX_ITEM_MANY_MARK /* uint32 avail_item_count */
    );
    dsm_reg_mem_event_level
    (
      DSM_DIAG_SIO_CTRL_TX_ITEM_POOL, /* dsm_mempool_id_type pool_id */
      DSM_MEM_LEVEL_FEW,        /*dsm_mem_level_enum_type level */
      DSMI_DIAG_SIO_CTRL_TX_ITEM_CNT-DSMI_DIAG_SIO_CTRL_TX_ITEM_FEW_MARK/* uint32 avail_item_count */
    );
  
   /* Register the call-backs for the MANY and FEW levels */
    dsm_reg_mem_event_cb
    (
      DSM_DIAG_SIO_CTRL_TX_ITEM_POOL, /* dsm_mempool_id_type      pool_id*/
      DSM_MEM_LEVEL_MANY,        /*dsm_mem_level_enum_type  level */
      DSM_MEM_OP_NEW,             /* dsm_mem_op_enum_type     mem_op */
      diagcomm_sio_ctrl_tx_dsm_high_mem_event_cb  /* pointer to call-back function */
    );
    dsm_reg_mem_event_cb
    (
      DSM_DIAG_SIO_CTRL_TX_ITEM_POOL, /* dsm_mempool_id_type      pool_id*/
      DSM_MEM_LEVEL_FEW,        /*dsm_mem_level_enum_type  level */
      DSM_MEM_OP_FREE,             /* dsm_mem_op_enum_type     mem_op */
      diagcomm_sio_ctrl_tx_dsm_low_mem_event_cb  /* pointer to call-back function */
    );
#endif /* !DIAG_MP_MASTER && DIAG_MP */

  diagcomm_io_open_init( conn );
  
} /* diagcomm_ctrl_init */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_OPEN_INIT

DESCRIPTION

PARAMETERS
  
RETURN VALUE
  None
  
===========================================================================*/
#ifdef DIAG_SIO_SUPPORT
void diagcomm_ctrl_open_init( diagcomm_io_conn_type * conn )
{
  /* Now prepare parameters for opening a port */
  conn->open_params.sio_params.params.stream_mode = SIO_DM_MODE;
  conn->open_params.sio_params.params.rx_queue = &(diagcomm_ctrl_rx_wmq[conn->port_type][conn->port_num]);
  conn->open_params.sio_params.params.tx_queue = &(diagcomm_ctrl_tx_wmq[conn->port_type][conn->port_num]);
  conn->open_params.sio_params.open_param.dsm_pool_param.smd_fifo_size = SMD_STANDARD_FIFO;

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
  if( DIAGCOMM_PORT_SMD == conn->port_type )
  {
    if( DIAGCOMM_PORT_1 == conn->port_num )
    {
      conn->open_params.sio_params.params.rx_func_ptr = diagcomm_ctrl_smd_inbound_pkt_notify;
    }
    else if( DIAGCOMM_PORT_2 == conn->port_num )
    {
      conn->open_params.sio_params.params.rx_func_ptr = diagcomm_ctrl2_smd_inbound_pkt_notify;
    }
  }
  else if( DIAGCOMM_PORT_SIO == conn->port_type )
  {
    if( DIAGCOMM_PORT_1 == conn->port_num )
    {
      conn->open_params.sio_params.params.rx_func_ptr = diagcomm_ctrl_sio_inbound_pkt_notify;
    }
    else if( DIAGCOMM_PORT_2 == conn->port_num )
    {
      conn->open_params.sio_params.params.rx_func_ptr = diagcomm_ctrl2_sio_inbound_pkt_notify;
    }
  }

  conn->open_params.sio_params.close_func_ptr = diagcomm_ctrl_close_done_cb;
  
  /* Passing the pool ID to SMD */
  if( DIAGCOMM_PORT_SIO == conn->port_type )
  {
#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
    conn->open_params.sio_params.open_param.dsm_pool_param.dsm_mempool_id = DSM_DIAG_SIO_CTRL_RX_ITEM_POOL;  
#endif
  }
  else
  {
#if !defined (DIAG_MP_MASTER) || !defined (DIAG_MP)
    conn->open_params.sio_params.open_param.dsm_pool_param.dsm_mempool_id = DSM_DIAG_SIO_CTRL_RX_ITEM_POOL;
#else
    conn->open_params.sio_params.open_param.dsm_pool_param.dsm_mempool_id = DSM_DIAG_SMD_CTRL_RX_ITEM_POOL;
#endif
  }

  conn->open_params.sio_params.open_param.dsm_pool_param.mempool_set = 0;
  conn->open_params.sio_params.open_param.dsm_pool_param.usb_mode = SIO_USB_MODE_LEGACY;
  conn->open_params.sio_params.params.open_param = &(conn->open_params.sio_params.open_param);
  
} /* diagcomm_ctrl_open_init */
#endif /* DIAG_SIO_SUPPORT */

/*===========================================================================

FUNCTION DIAGCOMM_CTRL_SEND_NUM_PRESETS

DESCRIPTION
   
PARAMETERS
  None

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
===========================================================================*/
void diagcomm_ctrl_send_num_presets( void )
{
#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
  diag_ctrl_msg_type ctrl_msg;
  boolean is_sent = FALSE;
  
  /* Compose DIAG_CTRL_MSG_NUM_PRESETS */
  ctrl_msg.cmd_type = DIAG_CTRL_MSG_NUM_PRESETS;
  ctrl_msg.data_len = sizeof(diag_ctrl_msg_num_presets_type);
  ctrl_msg.data.ctrl_msg_num_presets.num_presets = DIAG_MAX_PRESET_ID;
  
  (void)diagcomm_ctrl_send_mult_pkt( (void *)&ctrl_msg, DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_num_presets_type), 
                                      NULL, 0, DIAGCOMM_PORT_SIO, DIAGCOMM_PORT_1, TRUE, &is_sent );
#endif  
  
} /* diagcomm_ctrl_send_num_presets */

/*===========================================================================
FUNCTION diagcomm_sio_ctrl_flow_enabled

DESCRIPTION
   This function tells whether the flow through DSM_DIAG_SIO_CTRL_TX_ITEM_POOL is enabled 
   or not.

RETURN
   TRUE if flow is enabled.
   FALSE if flow is disabled.
   
===========================================================================*/
boolean diagcomm_sio_ctrl_flow_enabled(void)
{
  return ctrl_tx_allow_flow; 
  
} /* diagcomm_sio_ctrl_flow_enabled */




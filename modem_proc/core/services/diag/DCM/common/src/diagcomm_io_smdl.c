/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diag I/O Wrapper APIs for Interprocessor Communication and I/O 

General Description

Copyright (c) 2011-2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diagcomm_io_smdl.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/17/14   ph      Mainlined the feature DIAG_C_STRING.
06/03/14   rh      Changes to support 8KB cmd/rsp
01/30/14   ph      Removed the usage of macro DIAG_TASK_HANDLE().
08/31/13   is      Support for peripheral buffering mode
07/08/13   rh      Don't clear the SMDL handle either, for a remote close 
05/15/13   rh      No longer call diagcomm_io_close when notified of an SMDL 
                   remote close event
04/30/13   sr      Removed SIO_RX pool. Cmd req/rsp will now use CMD_SIO_TX 
                   and CMD_SIO_RX pool even in backwards compatibility case   
01/03/13   sg      Command Response Channel separation
10/11/12   rh      Added support for t-put improvements with SMDL
08/31/12   ra      Use accessor macros instead of accessing private task         
                   info directly; accomodated changes for User space
                   task priority changes  
05/10/12   sa      Removed DIAG_MP_SLAVE_LAST
09/28/11   is      Check if there's more data to read in diagcomm_smdl_buffer_read()
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
08/08/11   wjg     Modified to bring smdl_read into diag context 
03/04/11   is      Support for Diag over SMD-Lite

===========================================================================*/
#include "core_variation.h"

#ifdef DIAG_SMDL_SUPPORT 

#include "diagcomm_io_smdl.h"
#include "diagcomm_io.h"         /* For diagcomm_io_conn_type */
#include "smd_lite.h"
#include "osal.h"
#include "diag.h"                /* For DIAG_COMM_OPEN_SMD_SIG */
#include "diagdsm_v.h"           
#include "diag_v.h"              /* For diag_rx_notify() */
#include "diagi_v.h"             /* For DIAG_SMDL_READ_SIG */
#include "diag_fwd_v.h"          /* For diag_fwd_task_tcb */
#include "assert.h"              /* For ASSERT() */
#include "diagtarget.h"          /* For diag_tcb */
#include "diagpkt.h"             /* For DIAG_MAX_TX_PKT_SIZ */

extern diag_tx_mode_type diag_tx_mode[DIAG_MAX_STREAM_ID];

extern dsm_watermark_type diagcomm_smd_rx_wmq[NUM_SMD_PORTS];  /* From diagcomm_smd.c */
extern dsm_watermark_type diagcomm_smd_tx_wmq[NUM_SMD_PORTS];  /* From diagcomm_smd.c */

extern dsm_watermark_type diagcomm_ctrl_rx_wmq[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_ctrl.c */
extern dsm_watermark_type diagcomm_ctrl_tx_wmq[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_ctrl.c */

extern dsm_watermark_type diagcomm_sio_rx_wmq[NUM_SIO_PORTS];  /* From diagcomm_sio.c */
extern dsm_watermark_type diagcomm_sio_tx_wmq[NUM_SIO_PORTS];  /* From diagcomm_sio.c */

extern dsm_watermark_type diagcomm_cmd_rx_wmq[NUM_SMD_PORTS];  /* From diagcomm_cmd.c */
extern dsm_watermark_type diagcomm_cmd_tx_wmq[NUM_SMD_PORTS];  /* From diagcomm_cmd.c */

extern diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_cfg.c */
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_cfg.c */
extern diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_cfg.c */

uint32 diag_smdl_read_buf[DIAG_MAX_RX_PKT_SIZ/4];
uint32 diag_tx_pkt_conn_data[DIAG_TX_MAX_CHAIN_SIZ/4]; /* i.e. DIAG_MAX_TX_PKT_SIZ(0x1820)*2 to support 6k log pkts */
uint32 diag_tx_pkt_conn_ctrl[DIAG_TX_MAX_CHAIN_SIZ/4];
void diagcomm_smdl_buffer_read(diagcomm_io_conn_type *conn);

/*===========================================================================
FUNCTION DIAGCOMM_IO_SMDL_NOTIFY

DESCRIPTION
  This is the callback function registered with SMDL for any notification of
  OPEN/CLOSE/READ/WRITE events.

PARAMETERS
  port - SMDL handle
  event - SMDL event type
  data - Data returned from SMDL for the specific event notification
  
RETURN VALUE
  None
  
===========================================================================*/
static void diagcomm_io_smdl_notify_cb( smdl_handle_type port,
                                        smdl_event_type event,
                                        void *data )
{
  diagcomm_io_conn_type * conn = (diagcomm_io_conn_type *)data;
  
  switch( event )
  {
    case SMDL_EVENT_OPEN:
    {
      /* Received when the remote processor opens the port */
      conn->channel_connected = TRUE;
      diagcomm_io_conn_changed( conn, conn->channel_connected );
      break;
    }
    
    case SMDL_EVENT_REMOTE_CLOSE:
    {
      /* Received when the remote processor closes the port */
      conn->channel_connected = FALSE;
      diagcomm_io_conn_changed( conn, conn->channel_connected );
      /* This is not a local close event, so we don't need to do anything else. */
      break;
    }
    
	case SMDL_EVENT_CLOSE_COMPLETE:
	{
	  /* Received when SMD Lite has finished closing the port after the client
         calls smdl_close(). */
      conn->connected = FALSE;
      diag_release();	  
	  break;
	}
	
    case SMDL_EVENT_READ:
    {
      /* Received when data is available to be read from the FIFO */
      osal_sigs_t return_sigs;
      int return_val = 0;
      
      /* Check if previous data not read */
      if( conn->data_pending == FALSE )
      {
        /* Set data pending flag for connection */
        conn->data_pending = TRUE;
        
        if( conn->channel_type == DIAGCOMM_IO_CMD )
        {
        #if defined (DIAG_MP_MASTER)
         /* Responses recieved from peripherals available to be read. */
         return_val = osal_set_sigs(&diag_fwd_task_tcb, DIAG_SMDL_CMD_READ_SIG, &return_sigs);
         ASSERT(OSAL_SUCCESS == return_val);
        #else
          if( conn->port_num == DIAGCOMM_PORT_1 )
          {
            return_val = osal_set_sigs(diag_task_tcb, DIAG_SMDL_CMD_READ_SIG, &return_sigs);
            ASSERT(OSAL_SUCCESS == return_val);
          }
          else if( conn->port_num == DIAGCOMM_PORT_2 )
          {
            return_val = osal_set_sigs(diag_task_tcb, DIAG_SMDL_DCI_CMD_READ_SIG, &return_sigs);
            ASSERT(OSAL_SUCCESS == return_val);
          }
        #endif
        }
        else
        {
          /* Set signal to read smdl buffer */ /* TODO: Internal? */
          return_val = osal_set_sigs(diag_task_tcb, DIAG_SMDL_READ_SIG, &return_sigs);
          ASSERT(OSAL_SUCCESS == return_val);
        }
      }
      else
      {
        // Did not read last message
      }
      
      break;
    }
    
    case SMDL_EVENT_WRITE:
    {
      /* Received when previous smdl_write() failed and SMDL is ready to write again.
         Reason for failure may have been pkt to be written is > than outgoing FIFO size,
         and needs to be continued */
      osal_sigs_t return_sigs;
      int return_val = 0;
       
      /* Check if previous pkt not sent completely */
      /* Set signal to write the remaining pkt */ 
      return_val = osal_set_sigs(diag_task_tcb, DIAG_SMDL_WRITE_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
	  
      break;
    }
    
    default:
      break;
  } /* end switch */
  
} /* diagcomm_io_smdl_notify_cb */


/*===========================================================================
FUNCTION DIAGCOMM_SMDL_OPEN_INIT

DESCRIPTION

PARAMETERS
  conn - Diag I/O connection handle
  
RETURN VALUE
  None
  
===========================================================================*/
void diagcomm_smdl_open_init( diagcomm_io_conn_type * conn )
{
  uint8 stream_index;
  
  ASSERT( conn != NULL );
  
  if( conn->port_num > DIAGCOMM_PORT_NUM_LAST )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diagcomm_smdl_open_init - Invalid port_num %d", conn->port_num);
    return;
  }
  
  stream_index = DIAG_PORT_TO_INDEX(conn->port_num);
/*
  struct
  {
    const char       *name;
    smd_channel_type  edge;
    uint32            flags;
    uint32            fifo_size;
    smdl_callback_t   notify;
    void             *cb_data;
    dsm_watermark_type       *rx_queue;
    dsm_watermark_type       *tx_queue;
    dsm_mempool_id_enum_type  dsm_mempool_id;
  } smdl_params;
  
  typedef enum
  {
    SMD_CHANNEL_TYPE_FIRST,
    SMD_APPS_MODEM = SMD_CHANNEL_TYPE_FIRST,
    SMD_APPS_QDSP,
    SMD_MODEM_QDSP,
    SMD_APPS_DSPS,
    SMD_MODEM_DSPS,
    SMD_QDSP_DSPS,
    SMD_APPS_RIVA,
    SMD_MODEM_RIVA,
    SMD_QDSP_RIVA,
    SMD_DSPS_RIVA,
    SMD_CHANNEL_TYPE_LAST,
    SMD_INVALID_CHANNEL_TYPE
  } smd_channel_type;
*/
  
  conn->open_params.smdl_params.flags = SMDL_OPEN_FLAGS_MODE_PACKET;
  conn->open_params.smdl_params.fifo_size = SMD_STANDARD_FIFO;
  conn->open_params.smdl_params.notify = diagcomm_io_smdl_notify_cb;
  conn->open_params.smdl_params.cb_data = conn;
  
  
  
  /* Set watermark queues */
  if( DIAGCOMM_IO_DATA == conn->channel_type )
  {
    if( DIAGCOMM_PORT_SMD == conn->port_type )
    {
      conn->open_params.smdl_params.rx_queue = &diagcomm_smd_rx_wmq[conn->port_num];
      conn->open_params.smdl_params.tx_queue = &diagcomm_smd_tx_wmq[conn->port_num];
    }
    else if( DIAGCOMM_PORT_SIO == conn->port_type )
    {
      conn->open_params.smdl_params.rx_queue = &diagcomm_cmd_rx_wmq[conn->port_num];
      conn->open_params.smdl_params.tx_queue = &diagcomm_sio_tx_wmq[conn->port_num];
    }
  }
  else if( DIAGCOMM_IO_CTRL == conn->channel_type )
  {
    conn->open_params.smdl_params.rx_queue = &(diagcomm_ctrl_rx_wmq[conn->port_type][conn->port_num]);
    conn->open_params.smdl_params.tx_queue = &(diagcomm_ctrl_tx_wmq[conn->port_type][conn->port_num]);
  }
  else if( DIAGCOMM_IO_CMD == conn->channel_type )
  {
    conn->open_params.smdl_params.rx_queue = &diagcomm_cmd_rx_wmq[conn->port_num]; 
    conn->open_params.smdl_params.tx_queue = &diagcomm_cmd_tx_wmq[conn->port_num];
  }
  
  if( (DIAGCOMM_IO_DATA == conn->channel_type) || (DIAGCOMM_IO_CTRL == conn->channel_type) ) 
  {
  /* Set DSM pool ID */
  if( DIAGCOMM_PORT_SIO == conn->port_type )
  {
    conn->open_params.smdl_params.dsm_mempool_id = DSM_DIAG_CMD_SIO_RX_ITEM_POOL; 
  }
  else if( DIAGCOMM_PORT_SMD == conn->port_type )
  {
#if defined (DIAG_MP_MASTER)
	conn->open_params.smdl_params.dsm_mempool_id = DSM_DIAG_SMD_TX_ITEM_POOL;
#else
	conn->open_params.smdl_params.dsm_mempool_id = diag_tx_mode[stream_index].pool_info.pool_id;
#endif
  }
  }
  else if( DIAGCOMM_IO_CMD == conn->channel_type )
  {
   #ifndef DIAG_MP_MASTER
    if(conn->port_num == DIAGCOMM_PORT_1)
     conn->open_params.smdl_params.dsm_mempool_id = DSM_DIAG_CMD_SIO_RX_ITEM_POOL;
    else if( conn->port_num == DIAGCOMM_PORT_2)
     conn->open_params.smdl_params.dsm_mempool_id = DSM_DIAG_CMD_SIO_DCI_RX_ITEM_POOL;
    #else 
    if(conn->port_num == DIAGCOMM_PORT_1)
     conn->open_params.smdl_params.dsm_mempool_id = DSM_DIAG_CMD_SMD_RX_ITEM_POOL;
    else if( conn->port_num == DIAGCOMM_PORT_2)
     conn->open_params.smdl_params.dsm_mempool_id = DSM_DIAG_CMD_SMD_DCI_RX_ITEM_POOL;
    #endif 
  }

} /* diagcomm_smdl_open_init */


/*===========================================================================

FUNCTION DIAG_SMDL_BUFFER_READ

DESCRIPTION
  This function reads data from a smdl connection. It is called when data 
  is available to be read from a smdl FIFO.

PARAMETERS
  None
  
RETURN VALUE
  None
  
===========================================================================*/
void
diagcomm_smdl_buffer_read(diagcomm_io_conn_type *conn)
{
  uint32 read_len = 0;
  uint32 in_fifo = 0;
  int32 smdl_readv_ret = 0;
  int32 dsm_return_val = 0;
  dsm_item_type *dsm_item_ptr = NULL;
  osal_sigs_t return_sigs;
  int return_val = 0;
  if( conn && conn->io_type == DIAGCOMM_IO_TYPE_SMDL )
      {
        /* Check for data to be read */
        if (conn->data_pending == TRUE)
        {
      if(smdl_rx_peek_ext(conn->handle.smdl_handle, &read_len, &in_fifo) != 0)
      {
        MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diagcomm_smdl_buffer_read() called smdl_rx_peek_ext() on invalid port");
      }
      if (in_fifo == 0)
      {
        conn->data_pending = FALSE;
        return; //Nothing to read
      }

      conn->diag_rx_iovec.buffer = diag_smdl_read_buf;
      conn->diag_rx_iovec.length = sizeof(diag_smdl_read_buf);
      conn->diag_rx_iovec.next = NULL;

      smdl_readv_ret = smdl_readv(conn->handle.smdl_handle, &(conn->diag_rx_iovec), SMDL_READ_FLAG_LONG);
      if( smdl_readv_ret == read_len )
      {
        conn->data_pending = FALSE; /* We read complete packet */
      }
      else if( smdl_readv_ret == SMDL_CONTINUE || smdl_readv_ret == 0 )
      {
        conn->data_pending = TRUE; /* SMDL has more data in FIFO, come back and read more */
      }
      else
      {
        conn->data_pending = FALSE; /* this should never happen */
      }

      if( smdl_readv_ret > 0 && smdl_readv_ret == read_len && conn->data_pending == FALSE ) /* If a complete packet is read */
          {
        dsm_return_val = dsm_pushdown_tail(&dsm_item_ptr, conn->diag_rx_iovec.buffer, smdl_readv_ret,
                                           conn->open_params.smdl_params.dsm_mempool_id);
        if(  dsm_return_val == smdl_readv_ret )
            { 
              dsm_enqueue( conn->open_params.smdl_params.rx_queue, &dsm_item_ptr );

          if( (DIAGCOMM_IO_CMD == conn->channel_type) && (DIAGCOMM_PORT_SIO == conn->port_type) )
          {
            if( conn->port_num == DIAGCOMM_PORT_1 )
            {
              diag_rx_notify();
            } 
            else if(  conn->port_num == DIAGCOMM_PORT_2 )
            {
              diag_rx_dci_notify();
            }
          }
          else if( DIAGCOMM_IO_CTRL == conn->channel_type )
              {
                /* Set signal to diag task to process ctrl traffic from slave */
                return_val = osal_set_sigs( diag_task_tcb, DIAG_CTRL_MSG_SIG, &return_sigs );
                ASSERT(OSAL_SUCCESS == return_val);
              }
              else if( (DIAGCOMM_IO_DATA == conn->channel_type) && (DIAGCOMM_PORT_SMD == conn->port_type) )
              {
                /* Set signal to diag task to process traffic from slave */
                return_val = osal_set_sigs( &diag_fwd_task_tcb, DIAG_TX_SLAVE_SIG, &return_sigs );
                ASSERT(OSAL_SUCCESS == return_val);
              }
              else if( (DIAGCOMM_IO_DATA == conn->channel_type) && (DIAGCOMM_PORT_SIO == conn->port_type) )
              {
                diag_rx_notify();
              }
            }
            else
            {
          MSG_3(MSG_SSID_DIAG, MSG_LEGACY_ERROR,
                "diagcomm_smdl_buffer_read - Ran out of DSM items, Could not read from remote processor. Channel = %d, port_type = %d, num %d",
                conn->channel_type, conn->port_type, conn->port_num);

              dsm_free_packet( &dsm_item_ptr );
            }
      } // end of if (smdl_readv_ret > 0)
    
      /* Check if there's more data to be read */
      if(smdl_rx_peek_ext(conn->handle.smdl_handle, &read_len, &in_fifo) != 0)
      {
        MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diagcomm_smdl_buffer_read() called smdl_rx_peek_ext() on invalid port");
    }
      if( in_fifo > 0 )
      {
        conn->data_pending = TRUE;
        if( smdl_readv_ret != SMDL_CONTINUE && smdl_readv_ret != 0 )
  {
    if( (DIAGCOMM_IO_CTRL == conn->channel_type) || (DIAGCOMM_IO_DATA == conn->channel_type) )
    {
      return_val = osal_set_sigs(diag_task_tcb, DIAG_SMDL_READ_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
    }
    else if( (DIAGCOMM_IO_CMD == conn->channel_type) && (DIAGCOMM_PORT_SIO == conn->port_type) )
    {
      if( conn->port_num == DIAGCOMM_PORT_1 )
      {
        return_val = osal_set_sigs(diag_task_tcb, DIAG_SMDL_CMD_READ_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);
      }
      else if(  conn->port_num == DIAGCOMM_PORT_2 )
      {
        return_val = osal_set_sigs(diag_task_tcb, DIAG_SMDL_DCI_CMD_READ_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);
      }
          } // else if( (DIAGCOMM_IO_CMD == conn->channel_type)
        } // end of if( smdl_readv_ret != SMDL_CONTINUE )
      } // end of if( smdl_rx_peek(conn->handle.smdl_handle) > 0 )
      else
      {
        conn->data_pending = FALSE;
     }
    } // end of if( conn->data_pending == TRUE )
  else
  {
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "No data pending conn->data_pending = %d", conn->data_pending);
  }
  } /* end of if( conn->io_type == DIAGCOMM_IO_TYPE_SMDL ) */
  
} /* diagcomm_smdl_buffer_read */


/*===========================================================================

FUNCTION DIAGCOMM_IO_SMDL_SEND

DESCRIPTION
  This function writes data to a smdl connection. It is called when data 
  is already enqueued in the Tx WMQ of the data/ctrl channel.

PARAMETERS
  conn - Diag I/O connection handle
  
RETURN VALUE
  None
  
===========================================================================*/
void diagcomm_io_smdl_send ( diagcomm_io_conn_type * conn )
{
  uint32 total_pkt_length = 0;
  int32 smdl_writev_ret=0;  
  dsm_item_type * diag_tx_dsm_ptr = NULL;

  /* Sanity check for conn */
  ASSERT( conn != NULL );
  if( conn->io_type != DIAGCOMM_IO_TYPE_SMDL )
  {
    return;
  }
    
  /* Check if Tx WMQ is empty & prev tx was complete */
  if ( dsm_is_wm_empty(conn->open_params.smdl_params.tx_queue) && !conn->diag_prev_tx_pending )
  {
    return;
  }

  /* dequeue new dsm item and update iovec_ptr only when previous Tx was complete */
  if( ! conn->diag_prev_tx_pending )
  {  
    diag_tx_dsm_ptr = diagcomm_io_dequeue_tx_wmq(conn);
    ASSERT ( diag_tx_dsm_ptr != NULL );
    
    /* dsm_length_packet() provides length of entire packet if DSMs are chained */
    total_pkt_length = dsm_length_packet(diag_tx_dsm_ptr);

    conn->diag_tx_iovec.length = total_pkt_length;
    conn->diag_tx_iovec.next = NULL;

    if (conn->channel_type == DIAGCOMM_IO_DATA )
    {
      /* Check if pkt fits into the diag_tx_pkt before dsm_pullup_tail() */
      ASSERT( total_pkt_length <= sizeof(diag_tx_pkt_conn_data));
      dsm_pullup_tail(&diag_tx_dsm_ptr, &diag_tx_pkt_conn_data, total_pkt_length);
      conn->diag_tx_iovec.buffer = &diag_tx_pkt_conn_data;
    }
    if (conn->channel_type == DIAGCOMM_IO_CMD )
    {
      /* Check if pkt fits into the diag_tx_pkt before dsm_pullup_tail() */
      ASSERT( total_pkt_length <= sizeof(diag_tx_pkt_conn_data));
      dsm_pullup_tail(&diag_tx_dsm_ptr, &diag_tx_pkt_conn_data, total_pkt_length);
      conn->diag_tx_iovec.buffer = &diag_tx_pkt_conn_data;
    }
    if (conn->channel_type == DIAGCOMM_IO_CTRL )
    {
      /* Check if pkt fits into the diag_tx_pkt before dsm_pullup_tail() */
      ASSERT( total_pkt_length <= sizeof(diag_tx_pkt_conn_ctrl));
      dsm_pullup_tail(&diag_tx_dsm_ptr, &diag_tx_pkt_conn_ctrl, total_pkt_length);
      conn->diag_tx_iovec.buffer = &diag_tx_pkt_conn_ctrl;
    }
    
    /* free incoming dsm_ptr since it is already queued */
    (void)dsm_free_packet( &(diag_tx_dsm_ptr) );
  }

  /* call smdl_writev() */
  /* for log_pkt_len > smd fifo_sz, wait for SMDL_EVENT_WRITE cb to call us again with same iovec ptr */
  smdl_writev_ret = smdl_writev( conn->handle.smdl_handle, &(conn->diag_tx_iovec), SMDL_WRITE_FLAG_LONG );
  if( smdl_writev_ret == conn->diag_tx_iovec.length )
    conn->diag_prev_tx_pending = FALSE; /* Packet successfully sent */
  else if( smdl_writev_ret == SMDL_CONTINUE || smdl_writev_ret == 0 )
    conn->diag_prev_tx_pending = TRUE;  /* FIFO is full; come back later */
  else
    conn->diag_prev_tx_pending = FALSE; /* If we get here, something has gone wrong. */
    
}

#endif /*DIAG_SMDL_SUPPORT*/

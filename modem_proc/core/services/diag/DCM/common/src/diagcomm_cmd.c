/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostic Communications for Diag Command/Response Channel

General Description

Copyright (c) 2013-2014 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diagcomm_cmd.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/22/14   sr      Fixed the issue where cmd wmq is used without being 
                   initialized in Diag standalone case 
08/08/13   is      Command/response separation

===========================================================================*/

#include "diagcomm_cmd.h"
#include "diagdsm_v.h"       /* For DSM_DIAG_CMD_RX_ITEM_SIZ, etc. */
#include "msg.h"             /* For MSG_1, etc. */
#include "assert.h"          /* For ASSERT() */
#include "diagcomm_v.h"

extern diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_cfg.c */
extern diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_cfg.c */

static q_type diagcomm_cmd_rx_q[NUM_SMD_PORTS];
static q_type diagcomm_cmd_tx_q[NUM_SMD_PORTS];

dsm_watermark_type diagcomm_cmd_rx_wmq[NUM_SMD_PORTS];
dsm_watermark_type diagcomm_cmd_tx_wmq[NUM_SMD_PORTS];

/*===========================================================================

FUNCTION DIAGCOMM_CMD_INIT

DESCRIPTION
  This function initializes the diagnostic subsystem's communcations layer
  for the command/response channel.

PARAMETERS
  port_type - Cmd channel port type
  port_num - Cmd channel to initilize
  
RETURN VALUE
  None
  
===========================================================================*/
void
diagcomm_cmd_init( diagcomm_port_type port_type, diagcomm_enum_port_type port_num )
{  
  diagcomm_io_conn_type * conn = NULL;
  
  ASSERT( (port_type == DIAGCOMM_PORT_SMD) || (port_type == DIAGCOMM_PORT_SIO) );
  ASSERT( port_num <= DIAGCOMM_PORT_NUM_LAST );
      
  /* Make sure we are closed */
  diagcomm_cmd_close( port_type, port_num );
   
  /*Initialize the wmq*/
  diagcomm_cmd_wmq_init(port_type,port_num);
  
  conn = &diagcomm_io_cmd_conn[port_type][port_num];  
  diagcomm_io_open_init( conn );
  
} /* diagcomm_cmd_init */

/*===========================================================================

FUNCTION DIAGCOMM_CMD_WMQ_INIT

DESCRIPTION
  Initializes the cmd watermark queues

PARAMETERS
  port_type - Cmd channel port type
  port_num - Cmd channel to initilize
  
RETURN VALUE
  None
  
===========================================================================*/
void
diagcomm_cmd_wmq_init( diagcomm_port_type port_type, diagcomm_enum_port_type port_num )
{
  diagcomm_io_conn_type * conn = NULL;

  ASSERT( (port_type == DIAGCOMM_PORT_SMD) || (port_type == DIAGCOMM_PORT_SIO) );
  ASSERT( port_num <= DIAGCOMM_PORT_NUM_LAST );

  conn = &diagcomm_io_cmd_conn[port_type][port_num]; 
  
  if( conn->port_num == DIAGCOMM_PORT_1 )
  {
      memset (&diagcomm_cmd_rx_wmq[port_num], 0, sizeof (dsm_watermark_type));
      memset (&diagcomm_cmd_tx_wmq[port_num], 0, sizeof (dsm_watermark_type));
  #ifndef DIAG_MP_MASTER
      dsm_queue_init
      (
        &diagcomm_cmd_rx_wmq[conn->port_num],                     
        DSM_DIAG_CMD_SIO_RX_ITEM_CNT * DSM_DIAG_CMD_SIO_RX_ITEM_SIZ,      
        &diagcomm_cmd_rx_q[conn->port_num]                        
      );
      
      diagcomm_cmd_rx_wmq[conn->port_num].dont_exceed_cnt = DSM_DIAG_CMD_SIO_RX_ITEM_CNT * DSM_DIAG_CMD_SIO_RX_ITEM_SIZ;
      
      dsm_queue_init
      (
        &diagcomm_cmd_tx_wmq[conn->port_num],                     
        DSM_DIAG_CMD_SIO_TX_ITEM_CNT * DSM_DIAG_CMD_SIO_TX_ITEM_SIZ,      
        &diagcomm_cmd_tx_q[conn->port_num]                        
      );
      
      diagcomm_cmd_tx_wmq[conn->port_num].dont_exceed_cnt = DSM_DIAG_CMD_SIO_TX_ITEM_CNT * DSM_DIAG_CMD_SIO_TX_ITEM_SIZ;
  #else
      dsm_queue_init
      (
        &diagcomm_cmd_rx_wmq[conn->port_num],                     
        DSM_DIAG_CMD_SMD_RX_ITEM_CNT * DSM_DIAG_CMD_SMD_RX_ITEM_SIZ,      
        &diagcomm_cmd_rx_q[conn->port_num]                        
      );
      
      diagcomm_cmd_rx_wmq[conn->port_num].dont_exceed_cnt = DSM_DIAG_CMD_SMD_RX_ITEM_CNT * DSM_DIAG_CMD_SMD_RX_ITEM_SIZ;
      
      dsm_queue_init
      (
        &diagcomm_cmd_tx_wmq[conn->port_num],                     
        DSM_DIAG_CMD_SMD_TX_ITEM_CNT * DSM_DIAG_CMD_SMD_TX_ITEM_SIZ,      
        &diagcomm_cmd_tx_q[conn->port_num]                        
      );
      
      diagcomm_cmd_tx_wmq[conn->port_num].dont_exceed_cnt = DSM_DIAG_CMD_SMD_TX_ITEM_CNT * DSM_DIAG_CMD_SMD_TX_ITEM_SIZ;
      #endif
  }
  else if( conn->port_num == DIAGCOMM_PORT_2 )
  {
      memset (&diagcomm_cmd_rx_wmq[port_num], 0, sizeof (dsm_watermark_type));
      memset (&diagcomm_cmd_tx_wmq[port_num], 0, sizeof (dsm_watermark_type));
      #ifndef DIAG_MP_MASTER
      dsm_queue_init
      (
        &diagcomm_cmd_rx_wmq[conn->port_num],                     
        DSM_DIAG_CMD_SIO_DCI_RX_ITEM_CNT * DSM_DIAG_CMD_SIO_DCI_RX_ITEM_SIZ,      
        &diagcomm_cmd_rx_q[conn->port_num]                        
      );
      
      diagcomm_cmd_rx_wmq[conn->port_num].dont_exceed_cnt = DSM_DIAG_CMD_SIO_DCI_RX_ITEM_CNT * DSM_DIAG_CMD_SIO_DCI_RX_ITEM_SIZ;
      
      dsm_queue_init
      (
        &diagcomm_cmd_tx_wmq[conn->port_num],                     
        DSM_DIAG_CMD_SIO_DCI_TX_ITEM_CNT * DSM_DIAG_CMD_SIO_DCI_TX_ITEM_SIZ,      
        &diagcomm_cmd_tx_q[conn->port_num]                        
      );
      
      diagcomm_cmd_tx_wmq[conn->port_num].dont_exceed_cnt = DSM_DIAG_CMD_SIO_DCI_TX_ITEM_CNT * DSM_DIAG_CMD_SIO_DCI_TX_ITEM_SIZ;
      #else
      dsm_queue_init
      (
        &diagcomm_cmd_rx_wmq[conn->port_num],                     
        DSM_DIAG_CMD_SMD_DCI_RX_ITEM_CNT * DSM_DIAG_CMD_SMD_DCI_RX_ITEM_SIZ,      
        &diagcomm_cmd_rx_q[conn->port_num]                        
      );
      
      diagcomm_cmd_rx_wmq[conn->port_num].dont_exceed_cnt = DSM_DIAG_CMD_SMD_DCI_RX_ITEM_CNT * DSM_DIAG_CMD_SMD_DCI_RX_ITEM_SIZ;
      
      dsm_queue_init
      (
        &diagcomm_cmd_tx_wmq[conn->port_num],                     
        DSM_DIAG_CMD_SMD_DCI_TX_ITEM_CNT * DSM_DIAG_CMD_SMD_DCI_TX_ITEM_SIZ,      
        &diagcomm_cmd_tx_q[conn->port_num]                        
      );
      
      diagcomm_cmd_tx_wmq[conn->port_num].dont_exceed_cnt = DSM_DIAG_CMD_SMD_DCI_TX_ITEM_CNT * DSM_DIAG_CMD_SMD_DCI_TX_ITEM_SIZ;
      #endif
  }
} 
/*===========================================================================

FUNCTION DIAGCOMM_CMD_OPEN

DESCRIPTION
  Opens the communcications stream for the command/response channel.

PARAMETERS
  port_type - Cmd channel port type
  channel - Cmd channel to open
  
RETURN VALUE
  True if control port opened successfully.
  False otherwise.
  
===========================================================================*/
boolean 
diagcomm_cmd_open( diagcomm_port_type port_type, diagcomm_enum_port_type channel )
{
  diagcomm_io_conn_type * conn = NULL;
  
  ASSERT( (port_type == DIAGCOMM_PORT_SMD) || (port_type == DIAGCOMM_PORT_SIO) );
  ASSERT( channel <= DIAGCOMM_PORT_NUM_LAST );
  
  conn = &diagcomm_io_cmd_conn[port_type][channel];
  
  if( conn->connected )
  {
    return conn->connected;
  }
  
  if( !conn->connected )
  {
    if( diagcomm_io_open(conn) == TRUE )
    {
      conn->connected = TRUE;
    }
    else
    {
	  MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diagcomm_cmd_open - Could not open stream (%d, %d)", port_type, channel);
    }
  }
  
  return conn->connected;
  
} /* diagcomm_cmd_open */


/*===========================================================================

FUNCTION DIAGCOMM_CMD_CLOSE

DESCRIPTION
  Closes the communcications stream for the command/response channel, if any.

PARAMETERS
  port_type - Cmd channel port type
  channel - Cmd channel to close
 
RETURN VALUE
  None
  
===========================================================================*/
void 
diagcomm_cmd_close( diagcomm_port_type port_type, diagcomm_enum_port_type channel )
{
  diagcomm_io_conn_type * conn = NULL;
  
  ASSERT( (port_type == DIAGCOMM_PORT_SMD) || (port_type == DIAGCOMM_PORT_SIO) );
  ASSERT( channel <= DIAGCOMM_PORT_NUM_LAST );
  
  conn = &diagcomm_io_cmd_conn[port_type][channel];
  
  /* Got a command to close the connection. */
  if( conn->connected )
  {
    diagcomm_io_close( conn );
    
  }

  /* Success or not, consider close operation successful */
  conn->connected = FALSE;
  
} /* diagcomm_cmd_close */


/*===========================================================================

FUNCTION DIAGCOMM_CMD_GET_RX_WMQ_CNT

DESCRIPTION
  This function returns CMD RX watermark queue count.

===========================================================================*/
uint32 diagcomm_cmd_get_rx_wmq_cnt(diagcomm_port_type port_type, diagcomm_enum_port_type port_num)
{
  ASSERT( (port_type == DIAGCOMM_PORT_SMD) || (port_type == DIAGCOMM_PORT_SIO) );
  ASSERT( port_num <= DIAGCOMM_PORT_NUM_LAST );
  
  return diagcomm_io_get_rx_wmq_cnt( &diagcomm_io_cmd_conn[port_type][port_num] );
  
} /* diagcomm_cmd_get_rx_wmq_cnt */

/*===========================================================================

FUNCTION DIAGCOMM_CMD_STATUS

DESCRIPTION
    This function returns TRUE if the port passed in is open.

===========================================================================*/
boolean
diagcomm_cmd_status (diagcomm_enum_port_type port)
{
  return diagcomm_io_cmd_conn[DIAGCOMM_PORT_SIO][port].connected;
}
/*===========================================================================

FUNCTION DIAGCOMM_CMD_PROCESS_SLAVE_TX

DESCRIPTION


===========================================================================*/
void diagcomm_cmd_process_slave_tx(diagcomm_enum_port_type port_num)
{
  static dsm_item_type *diag_cmd_item_ptr = NULL;
  diagcomm_io_tx_params_type tx_params;
     
  ASSERT( port_num <= DIAGCOMM_PORT_NUM_LAST );
  
  if( diagcomm_cmd_status(DIAGCOMM_PORT_1) )
  {
    diag_cmd_item_ptr = diagcomm_io_dequeue_rx_wmq( &diagcomm_io_cmd_conn[DIAGCOMM_PORT_SMD][port_num] );
    if( diag_cmd_item_ptr != NULL )
    {
      tx_params.dsm_ptr = diag_cmd_item_ptr;
      diagcomm_io_transmit( &diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1], &tx_params );
    }
  }
  
} /* diagcomm_cmd_process_slave_tx */



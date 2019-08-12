/*==========================================================================

              Diag I/O Wrapper APIs for Interprocessor Communication and I/O 

General Description
  
Copyright (c) 2011-2014 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diagcomm_io_sio.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/30/14   ph      Removed the usage of macro DIAG_TASK_HANDLE().
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
01/03/13   sg      Command Response Channel separation
11/27/12   is      Resolve issue with channel_connected status not updated
                   on control channel connection
08/31/12   ra      Use accessor macros instead of accessing private task         
                   info directly; accomodated changes for User space
                   task priority changes  
06/15/12   sa      Removed function diagcomm_ctrl_open_cb
06/15/12   sa      Ensure Master Diag always opens ports for LPASS
05/03/12   sa      Mainlining DIAG_CENTRAL_ROUTING
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
05/03/11   hm      Featurised so apps does not open sio control channel 
04/18/11   is      Ensure CTRL msg sizes are constant
03/29/11   sg      Central Routing changes for Q6
03/04/11   is      Support for Diag over SMD-Lite

===========================================================================*/
#include "core_variation.h"
#ifndef FEATURE_WINCE
#endif

#ifdef DIAG_SIO_SUPPORT

#include "diagcomm_io_sio.h"
#include "sio.h"              /* For sio_ioctl_param_type, etc */

#include "msg.h"              /* For MSG_N() macros */
#include "osal.h"
#include "diagcomm_v.h"
#include "diag_fwd_v.h"
#include "diagi_v.h"          /* For DIAG_COMM_SIO_TX_FLUSH_SIG */
#include "diag.h"             /* For DIAG_COMM_OPEN_SIG */
#include "assert.h"           /* For ASSERT() */
#include "diagcomm_ctrl.h"    /* For diagcomm_ctrl_send_mult_pkt */


extern boolean diagpkt_filtered_cmd_set; /* Defined in diagpkt.c */
extern diagcomm_io_conn_type diagcomm_io_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_SMD_PORT_TYPES][NUM_SMD_PORTS]; /* From diagcomm_io.c */


/*===========================================================================

FUNCTION DIAGCOMM_SEND_DTR_MSG

DESCRIPTION
  Sends DTR msg to slave procs via control channel.

PARAMETERS
  dte_ready_asserted - DTR state
  
RETURN VALUE
  None
  
===========================================================================*/
#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
static void diagcomm_send_dtr_msg( boolean dte_ready_asserted )
{
  diag_ctrl_msg_type dtr_msg;
  boolean is_sent = FALSE;
  
  /* Compose DIAG_CTRL_MSG_REG packet */
  dtr_msg.cmd_type = DIAG_CTRL_MSG_DTR;
  dtr_msg.data_len = sizeof(diag_ctrl_msg_dtr_type);
  dtr_msg.data.ctrl_msg_dtr.version = DIAG_CTRL_MSG_DTR_VER;
  dtr_msg.data.ctrl_msg_dtr.dte_ready_asserted = dte_ready_asserted;
  
  /* Send msg immediately */
  (void) diagcomm_ctrl_send_mult_pkt( (PACK(void *))&dtr_msg, 
                                      DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_dtr_type), 
                                      NULL, 0,
                                      DIAGCOMM_PORT_SMD, 
                                      DIAGCOMM_PORT_1, TRUE, &is_sent );
  
} /* diagcomm_send_dtr_msg */
#endif // DIAG_MP && DIAG_MP_MASTER


/*===========================================================================

FUNCTION DIAG_DTR_CHANGED_CB

DESCRIPTION
  Invoked when DTR changes. This is intended to flip a variable that causes
  Diag to vote/not vote for sleep based on connectivity status.

PARAMETERS
  cb_data - Diag I/O connection handle
  
RETURN VALUE
  None
  
===========================================================================*/
void diag_dtr_changed_cb( void * cb_data )
{
  sio_ioctl_param_type param;
  boolean dte_ready_asserted;
      
  diagcomm_io_conn_type * conn = (diagcomm_io_conn_type *)cb_data; 

  ASSERT( conn != NULL );
  ASSERT( conn->io_type == DIAGCOMM_IO_TYPE_SIO );

  param.dte_ready_asserted = &dte_ready_asserted;
  diagcomm_io_ioctl( conn, (uint32)SIO_IOCTL_DTE_READY_ASSERTED, (void*)&param );
  
  
  if( conn->port_type == DIAGCOMM_PORT_SIO )
  {
    if( conn->channel_type == DIAGCOMM_IO_CTRL )
  {
	  sio_control_ioctl( conn->handle.sio_handle, SIO_IOCTL_DTE_READY_ASSERTED, &param );
      diagcomm_sio_ctrl_conn_changed( conn, dte_ready_asserted );
    }
    else if( conn->channel_type == DIAGCOMM_IO_DATA )
    {
	  sio_ioctl( conn->handle.sio_handle, SIO_IOCTL_DTE_READY_ASSERTED, &param );
      diagcomm_sio_conn_changed( conn, dte_ready_asserted );
    }
  }
  else if( conn->port_type == DIAGCOMM_PORT_SMD )
  {
    if( conn->channel_type == DIAGCOMM_IO_CTRL )
    {
      sio_control_ioctl( conn->handle.sio_handle, SIO_IOCTL_DTE_READY_ASSERTED, &param );
    }
    else if( conn->channel_type == DIAGCOMM_IO_DATA )
    {
      sio_ioctl( conn->handle.sio_handle, SIO_IOCTL_DTE_READY_ASSERTED, &param );
    }
  	diagcomm_smd_conn_changed( conn, dte_ready_asserted );
  }
  
  MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"diag_dtr_changed_cb: port=%d, channel=%d, \
        status=%d", conn->port_type, conn->channel_type, conn->channel_connected);
  
}  /* diag_dtr_changed_cb */


/*===========================================================================

FUNCTION DIAG_DTR_ENABLE

DESCRIPTION
  Invoked when Diag initializes. This registers a callback with SIO that is
  intended to report when there are DTR changes.

PARAMETERS
  None
  
RETURN VALUE
  None
  
===========================================================================*/
void diag_dtr_enable( diagcomm_io_conn_type * conn )
{
  sio_ioctl_param_type ioctl_param;

  ASSERT( conn != NULL );
  ASSERT( conn->io_type == DIAGCOMM_IO_TYPE_SIO );
  
  /* Register DTR callback for SMD and SIO ports */
  ioctl_param.enable_dte_ready_event_ext.cb_func = diag_dtr_changed_cb;
  
  //Set up call back data - since we are interested in knowing SIO stream_id when 
  //call back comes we are passing port info to SIO for MPSS  
  ioctl_param.enable_dte_ready_event_ext.cb_data = (void *)conn;
  
  if ( conn->channel_type == DIAGCOMM_IO_CTRL )
  {
    // Set DTR using sio_control_ioctl for control channel
    sio_control_ioctl ( conn->handle.sio_handle,
                        (uint32)SIO_IOCTL_ENABLE_DTR_EVENT_EXT,
                     (void*)&ioctl_param );
  }
  else if ( conn->channel_type == DIAGCOMM_IO_DATA )
  {
    // Set DTR using sio_ioctl for data channel
    sio_ioctl ( conn->handle.sio_handle,
                (uint32)SIO_IOCTL_ENABLE_DTR_EVENT_EXT,
                (void*)&ioctl_param );
  }

} /* diag_dtr_enable */


/*===========================================================================
FUNCTION DIAGCOMM_SMD_SIO_SET_PORT

DESCRIPTION

PARAMETERS
  conn - Diag I/O connection handle
  
RETURN VALUE
  None
  
===========================================================================*/
void diagcomm_smd_sio_set_port( diagcomm_io_conn_type * conn )
{

#if defined (DIAG_MP_MASTER)     // Apps processor 
    if (DIAGCOMM_PORT_1 == conn->port_num)
      conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG;
    else if (DIAGCOMM_PORT_2 == conn->port_num)
      conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG_QDSP;
#else
	#if defined(DIAG_QDSP6_APPS_PROC) //LPASS
	  conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG_APPS;
	#else
	  conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG;
	#endif
#endif 
 
} /* diagcomm_smd_sio_set_port */


/*===========================================================================
FUNCTION DIAGCOMM_CTRL_SIO_SET_PORT

DESCRIPTION

PARAMETERS
  conn - Diag I/O connection handle
  
RETURN VALUE
  None
  
===========================================================================*/
void diagcomm_ctrl_sio_set_port( diagcomm_io_conn_type * conn )
{ 
  if( DIAGCOMM_PORT_SMD == conn->port_type )
  {
    #if defined (DIAG_MP_MASTER)     // Apps processor 
      if (DIAGCOMM_PORT_1 == conn->port_num)
        conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG;
      else if (DIAGCOMM_PORT_2 == conn->port_num)
        conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG_QDSP;
    #else
	  #if defined(DIAG_QDSP6_APPS_PROC) //LPASS
	    conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG_APPS;
	  #else
	    conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG;
 	  #endif
    #endif 	
  }
  else if( DIAGCOMM_PORT_SIO == conn->port_type )
  {
    #if !defined (DIAG_RUNTIME_DEVMAP)
      conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG;
    #endif

    #if defined(DIAG_SIO_USB)
      conn->open_params.sio_params.params.port_id = SIO_PORT_USB_DIAG;
    #endif 
	
	#if defined(DIAG_QDSP6_APPS_PROC) //LPASS
	    conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG_APPS;
	#elif !defined(DIAG_MP_MASTER)
	    conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG;
 	#endif
	
  }
  
} /* diagcomm_ctrl_sio_set_port */


/*===========================================================================
FUNCTION DIAGCOMM_SIO_SET_PORT

DESCRIPTION

PARAMETERS
  conn - Diag I/O connection handle
  
RETURN VALUE
  None
  
===========================================================================*/
void diagcomm_sio_set_port( diagcomm_io_conn_type * conn )
{

#if !defined (DIAG_RUNTIME_DEVMAP)
  conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG;
#endif

#if defined (DIAG_SIO_USB)
  conn->open_params.sio_params.params.port_id = SIO_PORT_USB_DIAG;
#endif 

#if defined (DIAG_ON_SDIO_PORT)
  conn->open_params.sio_params.params.port_id = SIO_PORT_SDIOC_DIAG;  
#endif

#if defined (DIAG_SIO_UART)
  conn->open_params.sio_params.params.port_id = SIO_PORT_UART_MAIN;  
#endif


#if defined (DIAG_MP_MASTER)
    #if defined (DIAG_SIO_USB)
	  conn->open_params.sio_params.params.port_id = SIO_PORT_USB_DIAG;
	#endif
#elif defined(DIAG_QDSP6_APPS_PROC) //LPASS
	  conn->open_params.sio_params.params.port_id = SIO_PORT_SMD_DIAG_APPS;
#endif


  if( conn->open_params.sio_params.params.port_id == SIO_PORT_ONDEVICE_DIAG )
  {
    diagpkt_filtered_cmd_set = TRUE; /* Port is Ondevice */
  }
    
} /* diagcomm_sio_set_port */


/*===========================================================================

FUNCTION DIAGCOMM_SIO_OPEN_CB

DESCRIPTION
  This callback is called when RDM wants diag to open a port on the SIO channel

PARAMETERS
  port_id - SIO port ID to open
  
RETURN VALUE
  None
  
===========================================================================*/
void
diagcomm_sio_open_cb (sio_port_id_type port_id)
{
  osal_sigs_t return_sigs;
  int return_val = 0;
  
  /* Update SIO port id */
  diagcomm_io_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].open_params.sio_params.params.port_id = port_id;
  
  /* Update ctrl channel SIO port id */
  if( (port_id == SIO_PORT_ONDEVICE_DIAG) || (port_id == SIO_PORT_ONDEVICE_DIAG_USB_AUTO) )
  {
    diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].open_params.sio_params.params.port_id = port_id;
  }
  else
  {
    diagcomm_io_ctrl_conn[DIAGCOMM_PORT_SIO][DIAGCOMM_PORT_1].open_params.sio_params.params.port_id = SIO_PORT_NULL;
  }
  
  return_val = osal_set_sigs(diag_task_tcb, DIAG_COMM_OPEN_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);
  
} /* diagcomm_sio_open_cb */


/*===========================================================================

FUNCTION DIAGCOMM_CTRL_OPEN_CB

DESCRIPTION
  This is called when we want to open a port on the control channel

PARAMETERS
  port_id - SIO port ID to open
  
RETURN VALUE
  None
  
===========================================================================*/
void
diagcomm_ctrl_open_cb( sio_port_id_type port_id )
{
  osal_sigs_t return_sigs;
  int return_val = 0;
  
  return_val = osal_set_sigs( diag_task_tcb, DIAG_COMM_OPEN_SMD_SIG, &return_sigs );
  ASSERT(OSAL_SUCCESS == return_val);
  
} /* diagcomm_ctrl_open_cb */

#endif /* DIAG_SIO_SUPPORT */

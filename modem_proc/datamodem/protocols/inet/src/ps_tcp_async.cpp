/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             T C P ASYNC

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2011 - 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_tcpsubr.c_v   1.0   08 Aug 2002 11:19:52   akhare  $
  $Header $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---    ---------------------------------------------------------- 
05/07/13   xc      Fix compiler errors 
09/21/12   ss      Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR and 
                   evaluating ASSERT requirement.
03/28/12   dvk     Removed dependency on cs header files
10/25/11   dvk     Fixed KW error.
08/29/11   dvk     Fixed KW error.
02/28/11   ttv     Fixed KW error.
02/23/11   ttv     Fixed compilation warnings.
08/27/10   sn      Fixed compilation errors.
07/26/10   ps      Created the module.
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#ifdef FEATURE_DATA_IS707_ASYNC
#include "dsm.h"
#include "ps_pkt_info.h"
#include "ps_iface.h"

#include "ps_ip.h"
#include "memory.h"
#include "msg.h"
#include "err.h"
#include "ps_socket_defs.h"
#include "ps_tcp_hdr.h"
#include "pstimer.h"
#include "ps_tcp_asynci.h"
#include "dsstcp_async.h"
#include "ps_metai_info.h"
#include "ds_Sock_Def.h"
#include "ds_Utils_CCritSect.h"

static Async  *Async_handle = NULL;

/*===========================================================================
  
					   PUBLIC MEMBER FUNCTIONS
  
===========================================================================*/

Async::Async
(
 ps_async_func_tbl_type *func_table
)
{
  ps_async_func_tbl.s_upcall = func_table->s_upcall;
  ps_async_func_tbl.r_upcall = func_table->r_upcall;
  ps_async_func_tbl.t_upcall = func_table->t_upcall;

  critSectPtr = NULL;
  readerPtr_Recvq = NULL;
  writerPtr_Sendq = NULL;
  pcb_handle = 0;
   
}


int32 Async::CreateInstance
(
  ps_async_func_tbl_type *func_table,
  ps_iface_type *iface_ptr
)
{
  ds::ErrorType           res;
  ds::Sock::ProtocolType  protocol = Protocol::QDS_TCP;
  IReader                 *readerPtr_Sendq;
  IWriter                 *writerPtr_Recvq;
  int16                   ps_error_no;
  int16                   ret_val;
  ps_tx_meta_info_type    *meta_info_ptr; /* Metainfo about packet             */
 /*------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------
      Async is a singleton class, thus only one handle can exist at max
    -------------------------------------------------------------------------*/
 
  if ( Async_handle != NULL ) 
  {
    return (int32)Async_handle;
  }	
  
  Async_handle = new Async( func_table );


  if(AEE_SUCCESS != DS_Utils_CreateInstance( NULL,
                                             AEECLSID_CCritSect,
                                             (void **) &( Async_handle->critSectPtr)))
  {
    LOG_MSG_ERROR_0 ("Cannot create crit sect");
    ASSERT (0);
  }

  
  /*-------------------------------------------------------------------------
       Creating sendQ
    --------------------------------------------------------------------------*/
  res = SocketIOFactory::GetReaderAndWriter( 
        protocol, &readerPtr_Sendq, &(Async_handle->writerPtr_Sendq), 
        Async_handle->critSectPtr );

  if (res != AEE_SUCCESS)
  {
    LOG_MSG_ERROR_0 ("Cannot create send bytestream");
    ASSERT (0);
  }

  /*-------------------------------------------------------------------------
       Creating ReciveQ
     --------------------------------------------------------------------------*/
  
  res = SocketIOFactory::GetReaderAndWriter( 
        protocol, &(Async_handle->readerPtr_Recvq),&writerPtr_Recvq, 
        Async_handle->critSectPtr );

  if (res != AEE_SUCCESS)
  {
    LOG_MSG_ERROR_0 ("Cannot create bytestream");
    ASSERT (0);
  }

  /*------------------------------------------------------------------------
       Register Reader event listener on recieve queue   
    ------------------------------------------------------------------------*/
  Async_handle->readerPtr_Recvq->RegisterReaderListener( Async_handle );

  /*-------------------------------------------------------------------------- 
          Register Writer event listener on Send queue   
   -------------------------------------------------------------------------*/
  Async_handle->writerPtr_Sendq->RegisterWriterListener( Async_handle );

  /*---------------------------------------------------------------------------
       Create TCB for this instance.
    --------------------------------------------------------------------------*/
  ret_val = dsstcp_create_async_socket( 0XA7070000,
                                        &(Async_handle->pcb_handle),
                                        readerPtr_Sendq,
                                        writerPtr_Recvq,
                                        &ps_error_no );

  if(DSS_ERROR == ret_val)
  {
    MSG_ERROR("Create TCB failed , Error %d",ps_error_no,0,0);
	goto bail;
  }

  PS_TX_META_INFO_AND_RT_META_INFO_GET(meta_info_ptr);
  
  if( (meta_info_ptr != NULL) && (meta_info_ptr->rt_meta_info_ptr != NULL) )
  {
    PS_TX_META_SET_ROUTING_CACHE(meta_info_ptr, iface_ptr);
	
  	dsstcp_set_route_meta_info( Async_handle->pcb_handle, 0XA7070000, 
		                        meta_info_ptr->rt_meta_info_ptr);
  }
  else
  {
    MSG_ERROR("Metainfo: No mem", 0, 0, 0);
    ASSERT( 0 );
  }
  
  return (int32)Async_handle;

  bail:
 /*-------------------------------------------------------------------------
    Free reader and writer that are allocated for SendQ
  -------------------------------------------------------------------------*/
  if( 0 != readerPtr_Sendq)
  {
    readerPtr_Sendq->ReleaseReader();
  
  }

  if( 0 != Async_handle->writerPtr_Sendq )
  {
    Async_handle->writerPtr_Sendq->ReleaseWriter();
  
  }
  
 /*-------------------------------------------------------------------------
    Free reader and writer that are allocated for RecieveQ
  -------------------------------------------------------------------------*/ 
  if( 0 != Async_handle->readerPtr_Recvq )
  {
    Async_handle->readerPtr_Recvq->ReleaseReader();
  
  }

  if( 0 != writerPtr_Recvq)
  {
    writerPtr_Recvq->ReleaseWriter();
  
  }

  return DSS_ERROR;
}
  

void Async::DataAvailable
( 
  void
)
throw()
{
  int           cnt = 0;
  dsm_item_type *dsmItemPtr;
  int16         psErrno;
/*-------------------------------------------------------------------------*/

  cnt = ( this->readerPtr_Recvq )->Read (&dsmItemPtr, NULL, NULL, &psErrno);
   
  if( psErrno == DS_ENOERR)
  {
    ps_async_func_tbl.r_upcall( dsmItemPtr, cnt );
  }
  else
  {
    MSG_ERROR(" Read Operation Failed",0, 0, 0);         
  }
}

void Async::BufferAvailable
(
  void
)
{
  ps_async_func_tbl.t_upcall();
}

void Async::EmptyQueue
(
 void
)
{
  MSG_LOW("No Action",0,0,0); 
}

void Async::Async_reset_tcp 
(
 boolean silent_close_flag
)
{
  dsstcp_close( pcb_handle, 0XA7070000, silent_close_flag );
}

void Async::Async_tcp_output
( 
  int32 pkt_length
)
{
  dsstcp_async_write(pcb_handle, 0XA7070000, pkt_length, NULL, NULL);
}

void Async::Async_tcp_open
(
  struct  connection *ps707_async_connection,
  word mode,
  uint16  rwindow,
  uint16  mss
)
{
  struct ps_sockaddr_in6  dest_addr;
  struct ps_sockaddr_in6  src_addr;
  ps_route_scope_type     route_scope;
  uint8                   i;
/*------------------------------------------------------------------------*/

  dest_addr.ps_sin6_addr = ps707_async_connection->remote.address;
  dest_addr.ps_sin6_port = ps707_async_connection->remote.port;
  dest_addr.ps_sin6_flowinfo = 0;

  src_addr.ps_sin6_addr = ps707_async_connection->local.address;
  src_addr.ps_sin6_port = ps707_async_connection->local.port;


  for(i=0; i<PS_MAX_ROUTE_SCOPE_INDEX; i++)
  {
    route_scope.if_map[i] = 0xFFFFFFFFUL;
  }	
  
  dsstcp_set_route_scope( pcb_handle, 0XA7070000, &route_scope);
  	
  dsstcp_bind( pcb_handle, 0XA7070000, &src_addr);

  if ( mode == TCP_ACTIVE)
  {
    dsstcp_connect( pcb_handle, 0XA7070000, &dest_addr );
  }
  else
  {
    dsstcp_listen( pcb_handle, 0XA7070000, 1, rwindow);
  }

}

int32 Async::Async_send_data
(
  dsm_item_type   *new_item_ptr
)
{
  int32          retVal = -1;
  int16          psErrno;
  /*------------------------------------------------------------------------*/
  
  if( new_item_ptr == NULL)
  {
    dsm_free_packet( &new_item_ptr);
    return -1;
  }

  /*-------------------------------------------------------------------------
	 Write data to the data pipe
    -------------------------------------------------------------------------*/
  retVal = (this->writerPtr_Sendq)->Write( &new_item_ptr,
								           dsm_length_packet(new_item_ptr),
								           0,
								           0,
								           &psErrno );
  if (-1 == retVal)
  {
    return psErrno;
  } 
  else
  {
    dsstcp_async_write(pcb_handle, 0XA7070000,retVal, NULL,NULL);
  }

 return retVal;
  
}

struct ps_in6_addr Async::Async_get_remote_address
(
 void
)
{
  return(dsstcp_async_get_remote_address(pcb_handle));
 
}

struct ps_in6_addr Async::Async_get_local_address
(
 void
)
{
  return(dsstcp_async_get_local_address(pcb_handle));
 
}

uint32 Async::Async_outstanding_data
(
  void
)
{
  return(dsstcp_async_outstanding_data(pcb_handle));
}

int32 Async::Async_get_tcb_handle
(
 void
)
{
  if( Async_handle == NULL )
  {
    return -1;
  }
  else
  {
    return (Async_handle->pcb_handle);
  }
}

int32 Async::Async_get_async_handle
(
 void
)
{
  if( Async_handle == NULL )
  {
    return -1;
  }
  else  
  {
    return (int32)Async_handle;
  }
}

void Async::Async_state_change_upcall
(
  async_tcp_state_async_type state
)
{
  ps_async_func_tbl.s_upcall( state );
};

/*===========================================================================
FUNCTION PS_TCP_ASYNC_CREATE_HANDLE()

DESCRIPTION
  This function will setup TCB and will perform necessary initializations.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_tcp_async_create_handle
( 
  ps_async_func_tbl_type *func_table,
  ps_iface_type *iface_ptr
)
{
  return ( Async::CreateInstance(func_table,iface_ptr) );
    
}    

/*===========================================================================
FUNCTION PS_TCP_ASYNC_SEND_DATA()

DESCRIPTION
  This function will queue the passed data for TXing on the SendQ.

DEPENDENCIES
  None

RETURN VALUE
  No of bytes written on the SendQ: if Succeeded. 
  -1							 : if failed to write or Queue is Full.

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_tcp_async_send_data
(
  int32           client_handle,
  dsm_item_type   *new_item_ptr
  
)
{
  Async   *Async_hdl;
  Async   *Temp_handle;
 /*--------------------------------------------------------------------------*/

  Async_hdl = (Async*)(client_handle);
  Temp_handle = (Async *)Async::Async_get_async_handle();

  if ( Temp_handle == Async_hdl )
  {
    return ( Async_hdl->Async_send_data(new_item_ptr) );
  }
  else
  {
    MSG_ERROR( " Wrong Handle passed ",0,0,0);
	return -1;
  }
}

/*===========================================================================
FUNCTION PS_TCP_ASYNC_RECIEVE_DATA()

DESCRIPTION
  This function will notify the reader for the availibility
  of data on the RecieveQ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_tcp_async_recieve_data
(
 int32 client_handle
)
{
  Async  *Async_hdl;
  Async  *Temp_handle;
 /*--------------------------------------------------------------------------*/
  Async_hdl = (Async*)(client_handle);
  Temp_handle = (Async *)Async::Async_get_async_handle();

  if ( Temp_handle == Async_hdl )
  {
    Async_hdl->DataAvailable();
  }
  else
  {
    MSG_ERROR( " Wrong Handle passed ",0,0,0);
  }
}

/*===========================================================================
FUNCTION PS_TCP_ASYNC_OUTSTANDING_DATA()

DESCRIPTION
  This is a utility function to return the net outstanding data on SenQ, 
  this includes un-acknowledged seq# as well.

DEPENDENCIES
  None

RETURN VALUE
  Net outstandng data on SendQ.

SIDE EFFECTS
  None
===========================================================================*/
uint32 ps_tcp_async_outstanding_data
(
 void
)
{
  Async  *Async_hdl;
  int32  temp_hdl;
  /*--------------------------------------------------------------------------*/
  
  temp_hdl = Async::Async_get_async_handle();
  
  if( temp_hdl == -1 )
  {
    return 0;
  }
  else
  {
    Async_hdl = (Async *)temp_hdl;
    return (Async_hdl->Async_outstanding_data()); 
  }
}

/*===========================================================================
FUNCTION PS_TCP_ASYNC_CLOSE_TCP()

DESCRIPTION
  This is an API exposed to Async module to close the associated TCP connection.
  This really means "I have no more data to send". It only closes the  
  connection in one direction, and we can continue to receive data indefinitely.
  
DEPENDENCIES
  None

RETURN VALUE
  0 : If close action succeed.
 -1: For failure. 

SIDE EFFECTS
  None
===========================================================================*/
void ps_tcp_async_close_tcp
( 
  int32 client_handle
)
{
  Async  *Async_hdl;
  Async  *Temp_handle;
 /*--------------------------------------------------------------------------*/

  Async_hdl = (Async*)(client_handle);
 
  Temp_handle = (Async *)Async::Async_get_async_handle();

  if ( Temp_handle == Async_hdl )
  {
    Async_hdl->Async_reset_tcp(FALSE);
  }
  else
  {
    MSG_ERROR( " Wrong Handle passed ",0,0,0);
  }
}

/*===========================================================================
FUNCTION PS_TCP_ASYNC_RESET_TCP()

DESCRIPTION
  This is an API exposed to async module to shutdown the associated 
  TCP connection. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  ps_tcp_async_reset_tcp 
( 
  int32 client_handle
)
{
  Async  *Async_hdl;
  Async  *Temp_handle;
 /*--------------------------------------------------------------------------*/

  Async_hdl = (Async*)(client_handle);

  Temp_handle = (Async *)Async::Async_get_async_handle();

  if ( Temp_handle == Async_hdl )
  {
    Async_hdl->Async_reset_tcp(TRUE);
  }
  else
  {
    MSG_ERROR( " Wrong Handle passed ",0,0,0);
  }
}

/*===========================================================================
FUNCTION PS_TCP_ASYNC_TCP_OUTPUT()

DESCRIPTION
 This is an API exposed to Async module to send a TCP segment on the
 associated connection. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  ps_tcp_async_tcp_output 
( 
  int32 client_handle
)
{
  Async  *Async_hdl;
  Async  *Temp_handle;
 /*--------------------------------------------------------------------------*/
  Async_hdl = (Async*)(client_handle);

  Temp_handle = (Async *)Async::Async_get_async_handle();

  if ( Temp_handle == Async_hdl )
  {
    Async_hdl->Async_tcp_output(0);
  }
  else
  {
    MSG_ERROR( " Wrong Handle passed ",0,0,0);
  }
} 

/*===========================================================================
FUNCTION PS_TCP_ASYNC_TCP_OPEN

DESCRIPTION
  This is an API exposed to Async module and will Open a TCP connection 
  using the specified parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  ps_tcp_async_tcp_open
(
  int32   client_handle,
  struct  connection *ps707_async_connection,
  word    mode,
  uint16  rwindow,
  uint16  mss
)
{
  Async  *Async_hdl;
  Async  *Temp_handle;
 /*--------------------------------------------------------------------------*/
  Async_hdl = (Async*)(client_handle);  

  Temp_handle = (Async *)Async::Async_get_async_handle();

  if ( Temp_handle == Async_hdl )
  {
    Async_hdl->Async_tcp_open( ps707_async_connection,mode,
		                      rwindow,mss );
  }
  else
  {
    MSG_ERROR( " Wrong Handle passed ",0,0,0);
  }
}

/*===========================================================================
FUNCTION PS_TCP_ASYNC_STATE_CHANGE_UPCALL()

DESCRIPTION
  This function is an wraper and will be invoked by the TCP input 
  routine when the TCP state changes.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_tcp_async_state_change_upcall 
(
 async_tcp_state_async_type state
)
{
   Async *Async_hdl;
   int32 temp_hdl;
  /*-------------------------------------------------------------------------*/
 
  temp_hdl = Async::Async_get_async_handle();
  
  if( temp_hdl == -1)
  {
    MSG_ERROR("Async Handler not initialized %d", temp_hdl, 0, 0);
  }
  else
  {
    Async_hdl = (Async *)temp_hdl;
    Async_hdl->Async_state_change_upcall(state);
  }
}

/*===========================================================================
FUNCTION PS_TCP_ASYNC_GET_REMOTE_ADDRESS

DESCRIPTION
  This is an API exposed to Async module and will Open a TCP connection 
  using the specified parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
struct ps_in6_addr ps_tcp_async_get_remote_address
(
 void
)
{
  Async *Async_hdl;
  int32 temp_hdl;
  struct ps_in6_addr addr = {{{0},}};
  /*--------------------------------------------------------------------------*/
  
  temp_hdl = Async::Async_get_async_handle();

  if( temp_hdl != -1 )
  { 
    Async_hdl = (Async *)temp_hdl;
    addr = Async_hdl->Async_get_remote_address();
  }
  
  return addr;
}

/*===========================================================================
FUNCTION PS_TCP_ASYNC_GET_LOCAL_ADDRESS

DESCRIPTION
  This is an API exposed to Async module and will Open a TCP connection 
  using the specified parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
struct ps_in6_addr ps_tcp_async_get_local_address
(
  void
)
{
  Async *Async_hdl;
  int32 temp_hdl;
  struct ps_in6_addr addr = {{{0},}};
 /*--------------------------------------------------------------------------*/
 
  temp_hdl = Async::Async_get_async_handle();
  
  if(temp_hdl != -1 )
  { 
    Async_hdl = (Async *)temp_hdl;
    addr = Async_hdl->Async_get_local_address();
  }
  
  return addr;
}

/*===========================================================================
FUNCTION PS_TCP_ASYNC_GET_LOCAL_ADDRESS

DESCRIPTION
  This is an API exposed to Async module and will Open a TCP connection 
  using the specified parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void* ps_tcp_async_get_tcb_handle
(
 void
)
{  
  int32 temp_hdl;
/*--------------------------------------------------------------------------*/

  temp_hdl = Async::Async_get_tcb_handle();

  if( temp_hdl == -1)
  {
    return NULL;
  }

  return (void*)temp_hdl;
}

#endif /* FEATURE_DATA_IS707_ASYNC */
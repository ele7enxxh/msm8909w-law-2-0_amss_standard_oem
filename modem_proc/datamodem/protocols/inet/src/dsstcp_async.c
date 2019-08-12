/*===========================================================================

        D A T A  S E R V I C E S  S O C K E T  T C P   F I L E

DESCRIPTION

 The Data Services sockets TCP interface file. This contains all the
 functions used by the sockets library to access TCP socket function calls.

EXTERNALIZED FUNCTIONS
  dsstcp_socket()
    TCP specific socket() function.  Sets socket state to INIT.

Copyright (c) 1998-2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
	
/*===========================================================================
	
					  EDIT HISTORY FOR FILE
	
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
	
  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsstcp.c_v	 1.9   13 Feb 2003 12:20:46   ssinghai	$
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/dsstcp_async.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
	
	
when		who    what, where, why
--------	---    ----------------------------------------------------------
02/23/11   ttv     Fixed compilation warnings.
10/01/10       sa      Added appropriate log messages before ASSERT(0).
07/26/10       ps      created the module

===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR THE MODULE

===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"
	
#ifdef FEATURE_DATA_IS707_ASYNC
#include "ps_route_scope.h"
#include "dsstcp_async.h"
#include "ps_tcp_hdr.h"
#include "dsstcpi.h"
#include "ps_tcp_async.h"
#include "ps_in.h"

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION DSSTCPI_ASYNC_STATE_CHANGE_UPCALL()

DESCRIPTION
  This function will be invoked by the TCP input routine when the TCP state
  changes when in sockets mode. This function will use the old and the new
  TCP states to determine the processing requirements.  For sockets,
  this function currently does not actually use these values, but rather
  retains them to support backward compatibility to existing TCP code.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void dsstcpi_async_state_change_upcall
(
  struct tcb *tcb_ptr,         /* pointer to TCP control block which has a
                                  list head pointer to payload data        */
  tcp_state_enum_type old_state,  /* previous TCP state                    */
  tcp_state_enum_type new_state   /* new TCP state                         */
)
{
   
  if( NULL == tcb_ptr )
  {
	   MSG_ERROR( "NULL tcb_ptr passed", 0, 0, 0 );
	   ASSERT( 0 );
	   return;
  }
  
  MSG_MED("TCP socket=%d oldstate=%d newstate=%d",
		        (int)tcb_ptr->sockfd, (int) old_state, (int) new_state);
  
  /*-------------------------------------------------------------------------
    Do processing for relevant TCP state changes only.  Note, this block
    will generate a number of Lint errors, as the state enum is defined
    within the tcb structure in tcp.h.  This was maintained so as to reduce
    risk of changing any legacy tcp code.
  ---------------------------------------------------------------------------*/
   
  switch(tcb_ptr->state)
  {
    case TCP_ESTABLISHED:
      ps_tcp_async_state_change_upcall( ASYNC_TCP_ESTABLISHED );
	     break;
	  
   case TCP_CLOSE_WAIT:
	    ps_tcp_async_state_change_upcall( ASYNC_TCP_CLOSE_WAIT );
	    break;
	  
   case TCP_CLOSED:
	    ps_tcp_async_state_change_upcall( ASYNC_TCP_CLOSED );
	    break;

   default:
   	 MSG_ERROR("Bad state passed",0,0,0);
     break;
  }

}/* dsstcpi_async_state_change_upcall */

/*===========================================================================
FUNCTION DSSTCP_SOCKET()

DESCRIPTION
  This function is the protocol-specific socket function.  For TCP, it
  just sets the socket state to INIT.

  This function is called from the context of the application task.

DEPENDENCIES
  None

RETURN VALUE
  On success, returns DSS_SUCCESS
  On error, return DSS_ERROR and places the error condition value in *errno.

  Errno Values
  ------------
  DS_EPROTOTYPE       specified protocol invalid for socket type
  DS_EMFILE           too many descriptors open.  A socket is already open or
                      has not closed compeletely.

SIDE EFFECTS
  None.
===========================================================================*/
int16 dsstcp_create_async_socket
(
  int32      sockfd,
  int32   *  pcb_handle_ptr,
  void    *  reader_ptr,
  void    *  writer_ptr,
  int16   *  errno_ptr
)
{
  struct tcb *  tcb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check arguments.
  -------------------------------------------------------------------------*/
  if (errno_ptr == NULL)
  {
    LOG_MSG_ERROR_0(" dsstcp_create_async_socket(): errno_ptr is NULL ");
    ASSERT(0);
    goto bail;
  }

  if (reader_ptr == NULL || writer_ptr == NULL ||
      pcb_handle_ptr == NULL)
  {
    *errno_ptr = DS_EFAULT;
    LOG_MSG_ERROR_0(" dsstcp_create_async_socket(): Invalid argument passed ");
    ASSERT(0);
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Create TCP control block
  -------------------------------------------------------------------------*/
#define PS_ASYNC_TEMP_SCB 0xDEAD0000

  if ( (tcb_ptr = dsstcpi_create_tcb((void *)PS_ASYNC_TEMP_SCB, sockfd)) == NULL)
  {
    MSG_HIGH(" Could not create a TCB", 0, 0, 0);
    *errno_ptr = DS_EMFILE;
    goto bail;
  }
  
  tcb_ptr->scb_ptr  = NULL;
  tcb_ptr->s_upcall = dsstcpi_async_state_change_upcall;
  tcb_ptr->reader   = reader_ptr;
  tcb_ptr->writer   = writer_ptr;
  *pcb_handle_ptr   = (int32) tcb_ptr;

  return DSS_SUCCESS;

bail:
  return DSS_ERROR;
}  /* dsstcp_create_async_socket*/

/*===========================================================================
FUNCTION DSSTCP_ASYNC_WRITE()

DESCRIPTION
  Sends dsm memory item out to TCP.
  This function is called from the context of the PS task.

DEPENDENCIES
  None.

RETURN VALUE
  Uses return values from send_tcp().  On error, the function returns -1.
  Otherwise, it returns the number of bytes appended to TCP send queue.

SIDE EFFECTS
  None.

===========================================================================*/
void dsstcp_async_write
(
  int32                     pcb_handle,
  int32                     sockfd,
  int32                     pkt_length,
  struct ps_sockaddr_in6  * ta_ptr,
  ps_pkt_meta_info_type   * pkt_meta_info_ptr
)
{
  struct tcb  * tcb_ptr = (struct tcb *) pcb_handle;
  (void) ta_ptr;
  (void) pkt_meta_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == tcb_ptr)
  {
    MSG_ERROR( "NULL TCB", 0, 0, 0);
    ASSERT(0);
    return;
  }

  if (tcb_ptr->sockfd != sockfd)
  {
    MSG_HIGH("Sockfd %d != TCB's sockfd %d", sockfd, tcb_ptr->sockfd, 0);
    return;
  }

  tcb_ptr->sndcnt += pkt_length;

  switch(tcb_ptr->state)
  {
   	case TCP_CLOSED:
	   case TCP_FINWAIT1:
	   case TCP_FINWAIT2:
	   case TCP_CLOSING:
	   case TCP_LAST_ACK:
	   case TCP_TIME_WAIT:
	   {
	     break;
	   }
	
  	 case TCP_LISTEN:
	   {
     	/* --------------------------------------------------------------
	        If any remote address is 0, then it is a passive connection (LtoM).
	        We stay in LISTEN state till we receive TCP packets from the IWF.
	        Earlier the remote address was set to 0 in psmgr.c, so we enter
	        the if when we are in passive mode and don't have dest. port # yet
   	  ----------------------------------------------------------------*/
   	  if( PS_IN6_ARE_ADDR_EQUAL( &tcb_ptr->conn.remote.address, &ps_in6addr_any)
                                 || (tcb_ptr->conn.remote.port == 0) )
		    {
		      /* Save data for later */
		      break;
		    }

      /* Change state from passive to active */
    		tcb_ptr->flags.active = 1;
    		tcp_send_syn(tcb_ptr);
    		tcp_setstate(tcb_ptr,TCP_SYN_SENT);
   	}
  
	   /* Note fall-thru */
	   case TCP_SYN_SENT:
	   case TCP_SYN_RECEIVED:
	   case TCP_ESTABLISHED:
	   case TCP_CLOSE_WAIT:
	     tcp_output(tcb_ptr);
	     break;
	  
  }

  /*-------------------------------------------------------------------------
    Start persist timer only if the peer's window size is 0 and there is
    more data to be sent
  -------------------------------------------------------------------------*/
  if (0 == tcb_ptr->snd.wnd && 0 < tcb_ptr->sndcnt &&
      tcb_ptr->persist_timer != PS_TIMER_INVALID_HANDLE &&
      ps_timer_is_running(tcb_ptr->timer) == FALSE &&
      ps_timer_is_running(tcb_ptr->persist_timer) == FALSE)
  {
    tcp_start_persist_timer(tcb_ptr);
  }
  else
  {
    tcp_output(tcb_ptr);
  }

  return;
} /* dsstcp_async_write */

/*===========================================================================
FUNCTION DSSTCP_CLEANUP_ALL_RESEQ()

DESCRIPTION
  This function cleans up the resequencing queue of all the TCBs. This
  fucntion should be called to free up memory potentially consumed by the
  segments lying in the resequencing queues.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

struct ps_in6_addr dsstcp_async_get_remote_address
(
  int32 pcb_handle
)
{
  struct tcb * tcb_ptr = (struct tcb *) pcb_handle;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return(tcb_ptr->conn.remote.address);

} /* dsstcp_async_get_remote_address*/

/*===========================================================================
FUNCTION DSSTCP_CLEANUP_ALL_RESEQ()

DESCRIPTION
  This function cleans up the resequencing queue of all the TCBs. This
  fucntion should be called to free up memory potentially consumed by the
  segments lying in the resequencing queues.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

struct ps_in6_addr dsstcp_async_get_local_address
(
  int32 pcb_handle
)
{
  struct tcb * tcb_ptr = (struct tcb *) pcb_handle;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return(tcb_ptr->conn.local.address);
  
} /* dsstcp_async_get_local_address */

/*===========================================================================
FUNCTION DSSTCP_CLEANUP_ALL_RESEQ()

DESCRIPTION
  This function cleans up the resequencing queue of all the TCBs. This
  fucntion should be called to free up memory potentially consumed by the
  segments lying in the resequencing queues.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 dsstcp_async_outstanding_data 
(
  int32 pcb_handle
)
{
  struct tcb * tcb_ptr = (struct tcb *) pcb_handle;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ( tcb_ptr->sndcnt -           
          (tcb_ptr->snd.nxt - tcb_ptr->snd.una) );
  
} /* dsstcp_async_outstanding_data*/

#endif /* FEATURE_DATA_IS707_ASYNC */
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               H D R   B R O A D C A S T   P R O T O C O L    T A S K

GENERAL DESCRIPTION
  The HDR Broadcast Protocol task runs the following protocols:
  - Broadcast Content Mac Protocol
  - Broadcast Security Protocol
  - Broadcast Framing Protocol
      
EXTERNALIZED FUNCTIONS
  hdrbc_task
    Entry point to the HDR Broadcast Protocol task.
    

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2003 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbctask.c#1 $
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/04/13   sat     Removed unused parameters in F3s.
08/08/13   rmv     Featurized tmc APIs with ifndef FEATURE_MODEM_RCINIT
03/01/13   dsp     Moved watchdog timer code to watchdog heart-beat model.
07/09/12   dsp     Removed unnecessary F3s. 
03/22/12   arm     Fixed featurization bug for RCINIT.  
03/20/12   cnx     Resolved TCB compiler errors for Dime. 
03/05/12   arm     Fixed RCINIT bugs found during integration.
10/10/11   arm     Added support for RCINIT.
12/03/10   kss     Removed QSR_* macros from last check-in.
12/01/10   kss     Fixed compiler warning. 
09/16/10   pxu     Memory heap changes. 
09/15/10   ljl     Supported dynamic wdog.
03/11/10   rkc     Replaced deprecated task_ functions w/ tmc_task_ functions.
12/04/09   wsh     Fixed incorrectly stopping dog timer 
09/30/09   wsh     Fixed KW warnings
03/06/09   pmk     Clean up the resources before sending task_stop
12/10/08   smd     Replaced assert.h with amssassert.h
01/30/08   etv     Used ps_mem_init and ps_hdlc_lib_init for HDLC init.
07/24/06   mpa     Featurize ps mem pool initialization
07/23/06   mpa     Initialize hdlc lib here (ps_mem pool)
03/29/06   kss     Add support for BCMCS sleep optimizations.
09/30/05   mpa     Moved flow enable/disable cmds to hdrbcframe.
                   Route bc task cmds to appropriate protocols.
09/01/05   kss     Removed include of "hdrbcmaci.h".
08/28/05   dna     (mpa for) Added hdrbctask_queue_ind().
02/10/05   etv     Fixed the F3 message indicating reach of Queue limit.
02/02/05   etv     Fixed coding style to match convention.
01/25/05   etv     Cleaned lint errors. 
11/04/04   kss     BC packet scheduling is now run in HDR srch task.
08/17/04   kss     Call hdrbcmac_run_scheduler() to trigger scheduling.
08/16/04   kss     Moved flow status tracking to hdrbcmac.
07/08/04   kss     Remove include of hdrbcslot.h
07/06/04   kss     Removed logging; added access functions for logging info.
06/30/04   kss     Use dsm_items in data_ready function.
06/07/04   kss     Added flow enable/disable command processing.
04/25/04   kss     Ported from QC Corp.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "amssassert.h"
#include "task.h"
#ifndef FEATURE_MODEM_RCINIT
#include "tmc.h"
#endif /* FEATURE_MODEM_RCINIT */
#include "hdrdebug.h"
#include "err.h"
#include "dog.h"
#include "dog_hb_rex.h"
#include "dsm.h"
#include "queue.h"
#include "hdrbuf.h"
#include "modem_mem.h"

#ifdef FEATURE_HDR_BCMCS
#include "hdrbctask.h"
#include "hdrbctest.h"
#include "hdrlogi.h"
#include "hdrbclog.h"
#include "hdrbccp.h"
#include "hdrlog.h"
#include "hdrbcmac.h"
#include "hdrbcsec.h"
#include "hdrbcframe.h"

#ifdef FEATURE_DATA_ON_APPS
#include "ps_mem.h"
#endif /* FEATURE_DATA_ON_APPS */

#ifdef FEATURE_MODEM_RCINIT
#include "rcinit.h"
#define HDRBC_RCEVT_PREFIX "HDRBC:"
#define HDRBC_RCEVT_READY HDRBC_RCEVT_PREFIX "ready"
#define HDRDEC_RCEVT_PREFIX "HDRDEC:"
#define HDRDEC_RCEVT_READY HDRDEC_RCEVT_PREFIX "ready"
#endif


/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
rex_tcb_type *hdrbc_tcb_ptr;

#define HDRBCTASK_MACPKTS_Q_LIM 32

dog_report_type   hdrbc_dog_rpt_var = 0;
  /* Dog rpt */

/* Queue of pending commands, indications, and messages. */

LOCAL q_type hdrbctask_macpkts_q;


/*---------------------------------------------------------------------------
 Task event handling.
---------------------------------------------------------------------------*/

/* Queue of pending commands, indications, and messages. */

q_type hdrbctask_event_q;




/* Buffer type definition. */
typedef struct
{
  hdrbuf_hdr_type hdr;                       /* Header for this item       */
  union
  {
    hdrbctask_cmd_union_type       cmd;  /* Cmds for prot's in rx task */
    hdrbuf_ind_type ind; /* All tasks use the same structure for indications */
    dsm_item_type                 *item_ptr; /* Msg is a DSM item (chain)  */
  } item;
} hdrbctask_buf_type;


/* Number of buffers available. */
#define HDRBCTASK_EVENT_BUFS_NUM  8

/* Statically allocated buffers. */
hdrbctask_buf_type hdrbctask_event_pool[ HDRBCTASK_EVENT_BUFS_NUM ];


 /* EJECT */
/*=============================================================================
                           FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================

FUNCTION HDRBCTASK_PROCESS_WDOG

DESCRIPTION
  This procedure reports wdog and starts wdog timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbctask_process_wdog( void )
{

  (void) rex_clr_sigs( HDRBC_TASK_PTR, HDRBC_RPT_TIMER_SIG );
  dog_hb_report(DOG_HDRBC_RPT); 

} /* hdrbctask_process_wdog */

/*===========================================================================

FUNCTION HDRBCTASK_WAIT

DESCRIPTION
  This procedure waits for specified signals to be set.  This routine also
  checks any queues that are being waited on.  If the queue has an item, 
  this routine sets the signal without waiting.  This allows for the queue
  signal to be cleared after each item is removed.  If a queue signal is set,
  this routine checks the queue and clears the signal if there is nothing
  on it.  This centralizes the queue checking.

DEPENDENCIES
  None.

RETURN VALUE
  For non-queue signals, a signal bit is set in the return mask if the signal 
  was in the requested mask and is also set in the REX TCB.
  
  For queue signals, a signal bit is set in the return mask if the signal was 
  in the requested mask and the queue associated with the signal has an item
  on it.

SIDE EFFECTS
  None.

===========================================================================*/
rex_sigs_type hdrbctask_wait
(
  rex_sigs_type  requested_mask         /* Mask of REX signals to wait for */
)
{
  rex_sigs_type  rex_signals_mask; /* Signal mask returned by rex_get_sigs */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
   Loop until at least one requested signal is set
  -------------------------------------------------------------------------*/
  do {

    /* Actually wait for one of the requested signals to be set */
    rex_signals_mask = rex_wait( requested_mask | HDRBC_RPT_TIMER_SIG);

    /*-----------------------------------------------------------------------
     Watchdog report timer signal. Kick watchdog and reset timer.
    -----------------------------------------------------------------------*/
    if ( (rex_signals_mask & HDRBC_RPT_TIMER_SIG) != 0 ) 
    {
      hdrbctask_process_wdog();
    }

  } while ( (requested_mask & rex_signals_mask) == 0 );

  /*-------------------------------------------------------------------------
   Kick watchdog one last time on the way out.
  -------------------------------------------------------------------------*/
  (void) rex_clr_sigs( HDRBC_TASK_PTR, HDRBC_RPT_TIMER_SIG );
  dog_hb_report( DOG_HDRBC_RPT );

  /*-------------------------------------------------------------------------
   Assemble return mask.  Only return signals that were requested.
  -------------------------------------------------------------------------*/
  return ( rex_signals_mask & requested_mask );

}/* hdrbctask_wait */


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCTASK_INIT

DESCRIPTION
  This procedure does any task specific initialization.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbctask_init ( void )
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize queues */
  ( void ) q_init( &hdrbctask_macpkts_q );

  /* Initialize queues */
  ( void ) q_init( &hdrbctask_event_q );

  /* Init for BC protocols. */
  hdrbcframe_init();
  hdrbcsec_init();
#ifndef FEATURE_MODEM_RCINIT
  hdrbcmac_init();
#endif /* FEATURE_MODEM_RCINIT */
  hdrbctest_init();
  hdrbclog_clear_mcs_chan_metrics();

#ifndef FEATURE_HDR_BCMCS_NO_SLEEP
  /* Enable BCMCS sleep optimizations */
  hdrbcmac_enable_bcmcs_sleep();
#endif  /* FEATURE_HDR_BCMCS_NO_SLEEP */

#ifdef FEATURE_DATA_ON_APPS
  /* The HDLC library needs to also reside on the Modem processor because of 
     BCMCS (the only client on the modem side - all other clients are on the
     apps processor).  

     For now, we'll do the modem hdlc initialization here, but if we add 
     more clients to the HDLC library on the modem processor, then the 
     ps_mem_pool_init should happen in some other common task. */
  ps_mem_init();
  ps_hdlc_lib_init();
#endif /* FEATURE_DATA_ON_APPS */

}/* hdrbctask_init */


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCTASK_QUEUE_CMD

DESCRIPTION
  This function queues a command on the hdrbctask_event_q and sets the signal.
  The command data is copied into the command buffer before it is queued.  
  This function is called in the task context of whoever is giving the command.

DEPENDENCIES
  This routine must be multi-thread safe, as two different tasks may give
  commands to protocols in the RX task at the same time.

  cmd_size must not be larger than hdrbctask_protocol_cmd_type, because 
  this is the data type the command gets copied into.

PARAMETERS
  protocol_name - Who the command is for
  input_cmd_ptr - Pointer to a union of all protocols in HDRRX subtask
  cmd_size - Size of the command being given

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbctask_queue_cmd
( 
  hdrhai_protocol_name_enum_type protocol_name,
  void                        *input_cmd_ptr, 
  uint32                       cmd_size 
)
{
  hdrbctask_buf_type               *buf_ptr;         /* buffer for command */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  HDR_MSG_PROT_1 (MSG_LEGACY_MED,  "hdrbctask_queue_cmd received %d",
                  protocol_name);

  if ( input_cmd_ptr == NULL )
  {
    ERR( "No command to queue!", 0, 0, 0 );
    return;
  }

  ASSERT (cmd_size <= sizeof(hdrbctask_cmd_union_type) );

  /* Get a buffer from the heap */
  if ( ( buf_ptr = ( hdrbctask_buf_type * ) modem_mem_alloc( 
    sizeof( hdrbctask_buf_type ), MODEM_MEM_CLIENT_HDR_CRIT ) ) == NULL )
  {
    ERR( "No memory left in heap", 0, 0, 0 );
    return;
  }

  /* Fill in the buffer.  Make sure all fields are filled in. */
  buf_ptr->hdr.protocol_name = protocol_name;
  buf_ptr->hdr.type_of_item = HDRBUF_COMMAND;
  (void) memcpy( &buf_ptr->item.cmd, input_cmd_ptr, cmd_size );

  /* Queue the buffer and set a signal */
  ( void ) q_link( buf_ptr, &buf_ptr->hdr.link );
  q_put( &hdrbctask_event_q, &buf_ptr->hdr.link );
  ( void ) rex_set_sigs( HDRBC_TASK_PTR, HDRBC_EVENT_Q_SIG );

} /* hdrbctask_queue_cmd */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCTASK_QUEUE_IND

DESCRIPTION
  This function queues an indication on the hdrbctask_event_q and sets the
  signal.  If there is any data associated with the indication, it is copied
  into the buffer before it is queued.  This function is called in the task
  context of whoever is giving the indication.

DEPENDENCIES
  This routine must be multi-thread safe, as two different tasks may give
  an indication to protocols in the BC task at the same time.

  ind_data_size must not be larger than hdrind_ind_data_union_type, because
  this is the data type the command gets copied into.

PARAMETERS
  protocol_name - Who the indication is for
  ind_name - Global enumeration of indications
  input_ind_ptr - Pointer to the indication data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrbctask_queue_ind
( 
  hdrhai_protocol_name_enum_type protocol_name,
  hdrind_ind_name_enum_type ind_name,
  void *input_ind_data_ptr
)
{
  hdrbctask_buf_type *buf_ptr;    /* Buffer for indication */
  uint32 ind_data_size;           /* Size of data with indication */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_3 (MSG_LEGACY_LOW,  "hdrbc_queue_ind (%d, %d, %lx)",
                  protocol_name, ind_name, input_ind_data_ptr );

  /* Get a buffer from the heap */
  if ( ( buf_ptr = ( hdrbctask_buf_type * ) modem_mem_alloc( 
    sizeof( hdrbctask_buf_type ), MODEM_MEM_CLIENT_HDR_CRIT ) ) == NULL )
  {
    ERR( "No memory left in heap", 0, 0, 0 );
    return;
  }
  /* Fill in the buffer.  Make sure all fields are filled in. */
  buf_ptr->hdr.protocol_name = protocol_name;
  buf_ptr->hdr.type_of_item = HDRBUF_INDICATION;
  buf_ptr->item.ind.ind_name = ind_name;
  if ( input_ind_data_ptr != NULL )
  {
    ind_data_size = hdrind_get_ind_data_size (ind_name);

    ASSERT (ind_data_size <= sizeof(hdrind_ind_data_union_type) );
    (void) memcpy( &buf_ptr->item.ind.ind_data, input_ind_data_ptr,
                   ind_data_size );
  }

  /* Queue the buffer and set a signal */
  ( void ) q_link( buf_ptr, &buf_ptr->hdr.link );
  q_put( &hdrbctask_event_q, &buf_ptr->hdr.link );
  ( void ) rex_set_sigs( HDRBC_TASK_PTR, HDRBC_EVENT_Q_SIG );

} /* hdrbctask_queue_ind() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCTASK_QUEUE_MSG

DESCRIPTION
  This function queues a message on the hdrbctask_event_q and sets the signal.
  The queued buffer simply holds a pointer to the DSM item chain that is
  the message.  This function is called in the task context of whoever is 
  giving the message, which is always the task in which SLP runs.

DEPENDENCIES
  None

PARAMETERS
  protocol_name - Who the message is for
  protocol_instance - Instance of the protocol the message is for
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbctask_queue_msg
( 
  hdrhai_protocol_instance_enum_type protocol_instance,
  dsm_item_type               *item_ptr 
)
{
   hdrbctask_buf_type *buf_ptr;       /* Buffer for msg (w/ptr to DSM item */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get a buffer from the heap */
  if ( ( buf_ptr = ( hdrbctask_buf_type * ) modem_mem_alloc( 
    sizeof( hdrbctask_buf_type ), MODEM_MEM_CLIENT_HDR_CRIT ) ) == NULL )
  {
    ERR( "No memory left in heap", 0, 0, 0 );
    return;
  }

  /* Fill in the buffer.  Make sure all fields are filled in. */
  buf_ptr->hdr.protocol_instance = protocol_instance;
  buf_ptr->hdr.type_of_item  = HDRBUF_MESSAGE;
  buf_ptr->item.item_ptr     = item_ptr;

  /* Queue the buffer and set a signal */
  ( void ) q_link( buf_ptr, &buf_ptr->hdr.link );
  q_put( &hdrbctask_event_q, &buf_ptr->hdr.link );
  ( void ) rex_set_sigs( HDRBC_TASK_PTR, HDRBC_EVENT_Q_SIG );

}/* hdrbctask_queue_msg */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRBCTASK_PROTOCOL_CMD

DESCRIPTION
  All asynchronous commands for protocols that run in the BC task context 
  are routed through here. This function dispatches the command to the 
  protocol it is for.  Synchronous commands are processed in the context of
  the protocol issuing the command, so they are not queued and thus are not
  routed through here.
  
DEPENDENCIES
  None

PARAMETERS
  protocol_name - Name of the protocol the command is for.
  hdrbc_cmd_ptr - Command for a protocol in BC task 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbctask_protocol_cmd
( 
  hdrhai_protocol_name_enum_type protocol_name,
  hdrbctask_cmd_union_type *hdrbctask_cmd_union_ptr 
)
{

  switch ( protocol_name )
  {

    case HDRHAI_BROADCAST_MAC_PROTOCOL:
      hdrbcmac_process_cmd( &hdrbctask_cmd_union_ptr->bcmac);
    break;

    case HDRHAI_BROADCAST_FRAMING_PROTOCOL:
      hdrbcframe_process_cmd( &hdrbctask_cmd_union_ptr->bcframe);
    break;

    default:
      ERR("Unknown protocol (%d)",protocol_name ,0 ,0);
    break;
  }

} /* hdrbctask_protocol_cmd() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRBCTASK_PROTOCOL_IND

DESCRIPTION
  All indications for protocols that run in the HDRBC task context are routed
  through here. This function dispatches the indication to the protocol it
  is for.

DEPENDENCIES
  None

PARAMETERS
  protocol_name - Who the indication is for
  hdrmc_ind_ptr - Received indication for a protocol in BC task to be routed

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdrbctask_protocol_ind
(
  hdrhai_protocol_name_enum_type protocol_name,
  hdrbuf_ind_type * hdrbc_ind_ptr
)
{
  switch ( protocol_name )
  {
    case HDRHAI_BROADCAST_MAC_PROTOCOL:
      hdrbcmac_process_ind( hdrbc_ind_ptr->ind_name,
                            &hdrbc_ind_ptr->ind_data );
      break;

    default:
      ERR( "IND for unknown protocol %d", protocol_name, 0, 0 );
      break;
  }
} /* hdrbctask_protocol_ind() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRBCTASK_PROTOCOL_MSG

DESCRIPTION
  All messages for protocols that run in the BC task context are routed 
  through here. This function dispatches the message to the protocol it 
  is for.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Message for a protocol in BC task

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbctask_protocol_msg 
( 
  dsm_item_type               *item_ptr 
  /*lint -esym(715,item_ptr)
   * Ignores the unused argument(item_ptr) lint error #715 */
)
{

}/* hdrbctask_protocol_msg */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRBCTASK_PROTOCOL_ITEM

DESCRIPTION
  This function routes the item to the protocol it is for, according to
  whether it is a command, indication, or message

DEPENDENCIES
  None

PARAMETERS
  hdrbc_buf_ptr - Buffer containing cmd/msg/ind for a protocol in BC task

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbctask_protocol_item
( 
  hdrbctask_buf_type *hdrbc_buf_ptr 
)
{
  switch ( hdrbc_buf_ptr->hdr.type_of_item )
  {
    case HDRBUF_COMMAND: /* The item is a command */
    {
      hdrbctask_protocol_cmd( hdrbc_buf_ptr->hdr.protocol_name,
                              &hdrbc_buf_ptr->item.cmd );
      break;
    }

    case HDRBUF_INDICATION: /* The item is an indication */
      hdrbctask_protocol_ind( hdrbc_buf_ptr->hdr.protocol_name,
                              &hdrbc_buf_ptr->item.ind );
      break;

    case HDRBUF_MESSAGE: /* The item is a message (DSM item) */
    {
      hdrbctask_protocol_msg( hdrbc_buf_ptr->item.item_ptr );
      break;
    }

    default:
    {
      ERR( "BUF for unknown item %d", hdrbc_buf_ptr->hdr.type_of_item, 0, 0 );
      break;
    }
  }

}/* hdrbctask_protocol_item */


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCTASK_QUEUE_MAC_PKT

DESCRIPTION
  Places a mac packet on the Broadcast packet queue to be processed in 
  hdrbc task context.

DEPENDENCIES

PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrbctask_queue_mac_pkt
( 
   dsm_item_type* item_ptr
)
{
   
  int mac_q_cnt = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (item_ptr == NULL)
  {
    HDR_MSG_PROT ( MSG_LEGACY_FATAL, "Attempt to Queue Null Pkt to BCProtocol");
    return;
  }

  mac_q_cnt = q_cnt(&hdrbctask_macpkts_q);
  
  if ( mac_q_cnt >= HDRBCTASK_MACPKTS_Q_LIM)
  {
     HDR_MSG_PROT_1 ( MSG_LEGACY_ERROR,
                      "Mac packet queue limit reached %d", 
                      mac_q_cnt );
     dsm_free_packet(&item_ptr);
     return;
  }
   
  /* Queue the item and set a signal */
  ( void ) q_link( item_ptr, &(item_ptr->link) );
  q_put( &hdrbctask_macpkts_q, &(item_ptr->link) );
  ( void ) rex_set_sigs( HDRBC_TASK_PTR, HDRBC_MAC_PKTS_SIG );

} /* hdrbctask_queue_macpkt */  


 /* EJECT */
/*===========================================================================

FUNCTION HDRBCASTPROT_SUBTASK

DESCRIPTION
  The "active" subtask for hdrbc. Subtask is entered when HDR mode is active.


DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbctask_subtask ( void )
{
  rex_sigs_type   rex_signals_mask;   /* Task signal mask.     */
  rex_sigs_type   rex_wait_mask;   /* Task wait mask.     */
  boolean exit_hdr_mode = FALSE;     /* Exit flag */
  hdrbctask_buf_type* hdrbctask_buf_ptr;
  dsm_item_type* item_ptr;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT (MSG_LEGACY_HIGH, "Entering HDRBC subtask...");


  rex_wait_mask =                ( HDRBC_EXIT_HDR_SIG        |
                                   TASK_OFFLINE_SIG          |
                                   HDRBC_EVENT_Q_SIG         |
                                   HDRBC_MAC_PKTS_SIG        |
                                   TASK_STOP_SIG       
                                   );


  while (!exit_hdr_mode)
  {
    rex_signals_mask = hdrbctask_wait( rex_wait_mask );

    /*-----------------------------------------------------------------------
     Powerdown command signal. Clear signal, send ack to MC, and proceed.
    -----------------------------------------------------------------------*/
    if ( (rex_signals_mask & TASK_STOP_SIG) != 0 ) 
    {
       HDR_MSG_PROT (MSG_LEGACY_MED,  "TASK_STOP_SIG received");
       (void) rex_clr_sigs( HDRBC_TASK_PTR, TASK_STOP_SIG );

#ifndef FEATURE_MODEM_RCINIT
       tmc_task_stop();
#endif /* FEATURE_MODEM_RCINIT */
    }

    /*-----------------------------------------------------------------------
     Offline command signal. Clear signal, send ack to MC, and proceed.
    -----------------------------------------------------------------------*/
    if ( (rex_signals_mask & TASK_OFFLINE_SIG) != 0 ) 
    {
      HDR_MSG_PROT (MSG_LEGACY_MED,  "TASK_OFFLINE_SIG received");
      (void) rex_clr_sigs( HDRBC_TASK_PTR, TASK_OFFLINE_SIG );

#ifndef FEATURE_MODEM_RCINIT
      tmc_task_offline();
#endif /* FEATURE_MODEM_RCINIT */
    }

    /*-----------------------------------------------------------------------
          HDRBC_MAC_PKTS signal
    -----------------------------------------------------------------------*/
    if ( ( rex_signals_mask & HDRBC_MAC_PKTS_SIG ) != 0 )
    {
      (void) rex_clr_sigs( HDRBC_TASK_PTR, HDRBC_MAC_PKTS_SIG );

      /* Get packets of the queue and process them */
      while( q_cnt( &hdrbctask_macpkts_q ) != 0 )
      {
        item_ptr = (dsm_item_type*) q_get( &hdrbctask_macpkts_q );
      
        hdrbcmac_process_pkt(item_ptr);

      }
    }

    /*-----------------------------------------------------------------------
     Event queue signal. Clear signal and handle all events on event queue.
    -----------------------------------------------------------------------*/
    if ( ( rex_signals_mask & HDRBC_EVENT_Q_SIG ) != 0 )
    { 
      (void) rex_clr_sigs( HDRBC_TASK_PTR, HDRBC_EVENT_Q_SIG );

      while( q_cnt( &hdrbctask_event_q ) != 0 )
      {
        hdrbctask_buf_ptr = (hdrbctask_buf_type*) q_get( &hdrbctask_event_q );
        ASSERT(hdrbctask_buf_ptr);
        hdrbctask_protocol_item( hdrbctask_buf_ptr );
        modem_mem_free( hdrbctask_buf_ptr, MODEM_MEM_CLIENT_HDR_CRIT );
      }
    }


    /*-----------------------------------------------------------------------
     Exit HDR signal. Clear signal, and set flag to exit.
    -----------------------------------------------------------------------*/
    if ( (rex_signals_mask & HDRBC_EXIT_HDR_SIG) != 0 ) 
    {
       HDR_MSG_PROT (MSG_LEGACY_MED,  "HDRBC_EXIT_HDR_SIG received");
       (void) rex_clr_sigs( HDRBC_TASK_PTR, HDRBC_EXIT_HDR_SIG );
       exit_hdr_mode = TRUE;
    }
  }

}/* hdrbctask_subtask */


 /* EJECT */
/*===========================================================================

FUNCTION HDRBC_TASK

DESCRIPTION
  This function is the entry point to the HDR Broadcast task.  
  It contains the task event processing loop, which executes while waiting 
  to enter HDR mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbc_task
( 
  dword           dummy              /* Required for REX, ignore */
  /*lint -esym(715,dummy)
   * ignores the unused argument(dummy) lint error #715 */
)
{
  rex_sigs_type   rex_signals_mask;  /* Task signal mask.     */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
   Perform task initialization.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_MODEM_RCINIT
  rcinit_handshake_startup();

  /* Run time synch with other tasks is done here if needed, following APIs
  ** used: rcinit_lookup(),  rcevt_wait_name()
  */
  rcevt_wait_name(HDRDEC_RCEVT_READY);
  rcevt_signal_name(HDRBC_RCEVT_READY);

  /* RCINIT calls init function defined in task scons file in rcinit context
     All tcb related intializations should be done after rcinit starts our
     task */

  /* Store task tcb pointer */
  hdrbc_tcb_ptr = rex_self();

  hdrbc_dog_rpt_var = dog_hb_register_rex( (rex_sigs_type) HDRBC_RPT_TIMER_SIG );
   /* Register with dog HB */

    hdrbcmac_init();

#else /*  FEATURE_MODEM_RCINIT */

   /* Store task tcb pointer */
  hdrbc_tcb_ptr = rex_self();

  hdrbctask_init();

  /*-------------------------------------------------------------------------
   Wait for the start signal from the Main Control task.
  -------------------------------------------------------------------------*/
  tmc_task_start();
#endif /*  FEATURE_MODEM_RCINIT */

  /*-------------------------------------------------------------------------
   Send the first watchdog report and set the timer for report interval.
  -------------------------------------------------------------------------*/
  hdrbctask_process_wdog( );


  /*-------------------------------------------------------------------------
   Enter the task loop to handle task signals.
  -------------------------------------------------------------------------*/
  for(;;) 
  {
    rex_signals_mask = hdrbctask_wait( HDRBC_ENTER_HDR_SIG |
                                   TASK_OFFLINE_SIG    |
                                   TASK_STOP_SIG       );

    /*-----------------------------------------------------------------------
     Powerdown command signal. Clear signal, send ack to MC, and proceed.
    -----------------------------------------------------------------------*/
    if ( (rex_signals_mask & TASK_STOP_SIG) != 0 ) 
    {
      HDR_MSG_PROT (MSG_LEGACY_MED,  "TASK_STOP_SIG received");
      (void) rex_clr_sigs( HDRBC_TASK_PTR, TASK_STOP_SIG );

#ifndef FEATURE_MODEM_RCINIT
      tmc_task_stop();
#endif /* FEATURE_MODEM_RCINIT */
    }

    /*-----------------------------------------------------------------------
     Offline command signal. Clear signal, send ack to MC, and proceed.
    -----------------------------------------------------------------------*/
    if ( (rex_signals_mask & TASK_OFFLINE_SIG) != 0 ) 
    {
      HDR_MSG_PROT (MSG_LEGACY_MED,  "TASK_OFFLINE_SIG received");
      (void) rex_clr_sigs( HDRBC_TASK_PTR, TASK_OFFLINE_SIG );

#ifndef FEATURE_MODEM_RCINIT
      tmc_task_offline();
#endif /* FEATURE_MODEM_RCINIT */
    }

    /*-----------------------------------------------------------------------
     Enter HDR signal. Clear signal, and enter subtask.
    -----------------------------------------------------------------------*/
    if ( (rex_signals_mask & HDRBC_ENTER_HDR_SIG) != 0 ) 
    {
      HDR_MSG_PROT (MSG_LEGACY_MED,  "HDRBC_ENTER_HDR_SIG received");
      (void) rex_clr_sigs( HDRBC_TASK_PTR, HDRBC_ENTER_HDR_SIG );

      /* Enable periodic logging of broadcast metrics */
      hdrbclog_enable_periodic_logging(HDRBCLOG_LOGGING_PERIOD);

      /* Enter HDR mode processing subtask */
      hdrbctask_subtask();

      /* Disable periodic logging of broadcast metrics */
      hdrbclog_disable_periodic_logging();
    }

  }/* for (;;) */
     
}/* hdrbc_task */

#endif /* FEATURE_HDR_BCMCS */

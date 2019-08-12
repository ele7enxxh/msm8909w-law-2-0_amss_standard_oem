/*===========================================================================
         COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

         EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/gsmsg.c_v   1.17   14 Mar 2002 16:31:26   amcintos  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/src/gsmsg.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

04/01/03   ATM     Removed compiler warning
03/13/02   ATM     Additional modification per code review
02/20/02   ATM     Merge GS_GSM and GS functions together
01/09/02   CD      Merged changes introduced in branched revision 1.11.1.0
12/17/01   JC      Replaced FEATURE_RR with FEATURE_GSM.
04/19/01   TS      Updated gs_send_message() to send appropriate command_type to 
                   call manager.
06/22/01   TS      Fixed problem when FEATURE_CM_INCLUDED not defined.

7/13/01    CD      Replaced GS_QUEUE_MMI by GS_QUEUE_CM in gs_send_message()
                   Replaced MS_MN_CM by MS_MN_CM in gs_send_message()

08/14/01   AB      Renamed connection manager reference to CNM from CM.

8/15/01    AB      Renamed all MMI references to CM.
10/11/01   jc      Update message sizing for GSDI message routing.

11/9/01    CD      Added FEATURE_GWSMS for UASMS
11/21/01   TS      Changed handling of GSM lower layer messages to use 
                   gsm_send_message() function.
09/18/02   AB      Added gs_count_message() to get how many messages/item 
                   are enqueued.
05/14/05   kvk     Added gs_clear_messages() to clear the messages in the queue
                   whichout calling callback.
07/04/05   kvk     Resolved static buffer overrun problem.
08/18/11   gps     Return gs_status failure if message length is larger than the
                   maximum queue size.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"  // Catch-all for FEATURES
#include "gs_v.h"        // This header file
#include "msg.h"       // For MSG_ERROR()
#include "sys_stru_v.h"  // For GET_IMH_LEN
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif                       
#include <stringl/stringl.h>
#include "ms.h"
#include "nasutils.h"
/*===========================================================================
                        TYPE DEFINITIONS FOR EVENTS & LOG PACKETS
===========================================================================*/
/* Defined this to map gs_queue_id to REX parameters */
typedef struct
{
    rex_tcb_type      *tcb_ptr;         /* thread to signal after enqueue of message      */
    rex_sigs_type      cmd_q_sig;       /* signal to use after enqueue of message         */
    q_type            *rcv_q_ptr;       /* queue to place message on                      */
    q_type            *rcv_free_q_ptr;  /* if NULL, then use dynamic allocation           */
    unsigned int       buffer_size;     /* number of bytes for gs_alloc to allocate       */
    byte               high_watermark;  /* Error will be emitted if queue exceeds this    */
    gs_put_handler_ptr enqueue_func_ptr;/* This function handles sending a single message */
    gs_get_handler_ptr proc_func_ptr;   /* This function handles reading a single message */
    int                queue_max_size;  /* For Statistics */
    unsigned int       messages_sent;   /* For Statistics */
} queue_info_T;

/* Generic CMD structure */
/* GS assumes first 4 bytes of "cmd" is the IMH_T */
/* primarily so length can be determined */
/* alternatively so msgset can be determined for logging */
typedef struct
{
    q_link_type      link;
    byte            *cmd;
}  cmd_type;

/*===========================================================================
                         DATA DECLARATIONS
===========================================================================*/
queue_info_T GS_TaskList[GS_MAX_QUEUE];

/* ACCESS SHORTCUTS */
#define QID_TO_TCB(gs_queue_id)     GS_TaskList[gs_queue_id].tcb_ptr
#define QID_TO_SIG(gs_queue_id)     GS_TaskList[gs_queue_id].cmd_q_sig
#define QID_TO_QPTR(gs_queue_id)    GS_TaskList[gs_queue_id].rcv_q_ptr
#define QID_TO_FREE_Q(gs_queue_id)  GS_TaskList[gs_queue_id].rcv_free_q_ptr
#define QID_TO_SIZE(gs_queue_id)    GS_TaskList[gs_queue_id].buffer_size
#define QID_TO_WMARK(gs_queue_id)   GS_TaskList[gs_queue_id].high_watermark
#define QID_TO_PUTFUNC(gs_queue_id) GS_TaskList[gs_queue_id].enqueue_func_ptr
#define QID_TO_GETFUNC(gs_queue_id) GS_TaskList[gs_queue_id].proc_func_ptr
#define QID_TO_MAX_STAT(id)         GS_TaskList[id].queue_max_size
#define QID_TO_MSG_STAT(id)         GS_TaskList[id].messages_sent


/*===========================================================================
                         FUNCTION DECLARATIONS
===========================================================================*/
/*===========================================================================

FUNCTION  gs_send_message

DESCRIPTION
  This module writes a message to an inter task communications link.  If the
  write cannot be completed immediately, the module either waits until it can
  be completed, or returns without completing, dependant on the wait
  parameter.

RETURN VALUE
  status -- GS_SUCCESS if sent, GS_FAILURE otherwise

===========================================================================*/
gs_status_T gs_send_message
(
gs_queue_id_T                       gs_queue_id,
void                               *msg_p,
boolean                             wait
)
{
    gs_status_T   gs_status = GS_FAILURE;
    word          message_length;
    void         *cmd_ptr;
    int           cnt;

    if ( NULL == msg_p )
    {   /* Sanity Check */
        MSG_ERROR_1("msg_p was NULL: Q=%d",gs_queue_id);
        return gs_status;
    }
    if (gs_queue_id >= GS_MAX_QUEUE)
    {   /* Sanity Check */
        MSG_ERROR_1("Bad QID: Q=%d",gs_queue_id);
        return gs_status;
    }
    
    /* 1. Gather Statistics & Log Message */
    //    Increase Message Count by one
    ++QID_TO_MSG_STAT(gs_queue_id); 
    //    Get message length
    GET_IMH_LEN(message_length, msg_p); 
    //    Add in header size
    message_length += sizeof(IMH_T);
    //    Log using CFA -- filtered by message set
    if(((IMH_T*)msg_p)->message_set != MS_TDSRRC_RR)
    {
        gs_log_message( (IMH_T*) msg_p, gs_queue_id, message_length );
    }


    /* -----------------------------
    ** 2a. Enqueue the message 
    **  w/handler if one exists
    ** ----------------------------- */
    if (  QID_TO_PUTFUNC(gs_queue_id) )
    {   /* Message Handler function exists */
        return QID_TO_PUTFUNC(gs_queue_id)( msg_p );
    }
    /* -----------------------------
    ** 2b. Otherwise use default routine
    ** make sure that the recipient has registered
    ** a queue to receive this message.
    ** ----------------------------- */
    else if ( NULL == QID_TO_QPTR(gs_queue_id) )
    {   /* Sanity Check */
        MSG_ERROR_1("Queue %d not registered with GS",gs_queue_id);
        return gs_status;
    }

    if(QID_TO_SIZE(gs_queue_id) <  (message_length + sizeof(q_link_type)))
    {
        MSG_ERROR_1("Queue %d is not having sufficiant size",gs_queue_id);
        return gs_status;
    }

    /* 3. Get memory for the MESSAGE */
    if ( NULL == QID_TO_FREE_Q(gs_queue_id) )
    {   /* 3a. Allocate command buffer buffer dynamically*/
#ifndef FEATURE_MODEM_HEAP
        cmd_ptr = gs_alloc( QID_TO_SIZE(gs_queue_id) );
        if(NULL == cmd_ptr)
        {
          MSG_ERROR_1("gs_alloc failure, QID %d",gs_queue_id);
        }
#else
        cmd_ptr = modem_mem_calloc(1,QID_TO_SIZE(gs_queue_id), MODEM_MEM_CLIENT_GSLIB);
        if(NULL == cmd_ptr)
        {
          MSG_ERROR_1("modem_mem_calloc failure, QID %d",gs_queue_id);
        }
#endif
    }
    else
    {   /* 3b. Get buffer from Free Queue */
        cmd_ptr = q_get( QID_TO_FREE_Q(gs_queue_id) );
        if ( NULL == cmd_ptr)
        {
            MSG_ERROR_1("No Buffers left, Q %d full",gs_queue_id);
        }
    }
    
    if ( cmd_ptr )
    {   /* 4. Copy data to command buffer */
        memscpy(&((cmd_type*)cmd_ptr)->cmd, message_length, (byte*)msg_p, message_length);

        /* 5. Place message on destination task queue */
        (void) q_link( cmd_ptr, &((cmd_type*)cmd_ptr)->link );
        q_put(QID_TO_QPTR(gs_queue_id), &((cmd_type*)cmd_ptr)->link );

        /* 6. Gather Statistics */
        cnt = q_cnt(QID_TO_QPTR(gs_queue_id));
        if ( cnt > QID_TO_MAX_STAT(gs_queue_id) )
        {
            QID_TO_MAX_STAT(gs_queue_id) = cnt; // Hold the maximum reached limit
        }

        /* 7. Check if max desired queue size is exceeded
           -since we're just reading, INTLOCK is not critical */
        if ( cnt > QID_TO_WMARK(gs_queue_id) )
        {
            MSG_ERROR_2("Queue %d exceeds %d msgs!",gs_queue_id,QID_TO_WMARK(gs_queue_id));
        }

        /* 8. Set the command queue signal for receive task */
        (void)rex_set_sigs( QID_TO_TCB(gs_queue_id), QID_TO_SIG(gs_queue_id) ); 

        gs_status = GS_SUCCESS;
    }

    return gs_status;

}


/*===========================================================================

FUNCTION  gs_read_messages

DESCRIPTION
  This function drains the named queue, calls the named proc_func_ptr function
  then removes the message.

RETURN VALUE
  None

===========================================================================*/
void gs_read_messages
(
gs_queue_id_T                       gs_queue_id
)
{
    cmd_type *cmd_ptr;

    /* Clear Message signal */
    (void)rex_clr_sigs( QID_TO_TCB(gs_queue_id), QID_TO_SIG(gs_queue_id) ); 

    /* Drain Message Queue */
    while ((cmd_ptr = (cmd_type*) q_get( QID_TO_QPTR(gs_queue_id) )) != NULL)
    {
        /* -----------------------------
        ** Process the message w/handler
        ** ----------------------------- */
        if ( QID_TO_GETFUNC(gs_queue_id) )
        {   /* Message Handler function exists */
            QID_TO_GETFUNC(gs_queue_id)( cmd_ptr );
        }
        else
        {
            MSG_ERROR_1("Queue %d: no process handler!",gs_queue_id);
        }

        if ( NULL == QID_TO_FREE_Q(gs_queue_id) )
        {   /* Deallocate Command Buffer */
#ifndef FEATURE_MODEM_HEAP
            gs_free( cmd_ptr );
#else
            modem_mem_free(cmd_ptr, MODEM_MEM_CLIENT_GSLIB);
#endif

        }
        else
        {   /* Return buffer to Free Queue */
            q_put( QID_TO_FREE_Q(gs_queue_id), &((cmd_type*)cmd_ptr)->link );
        }
    }   

} /* end gs_read_messages() */

/*===========================================================================

FUNCTION  gs_count_message()

DESCRIPTION
  This module return number of messages that are currently available 
  in the queue.

RETURN VALUE
  cnt -- number of messages in the queue.

===========================================================================*/
word gs_count_message (gs_queue_id_T gs_queue_id)
{
    word cnt;

    if ( gs_queue_id >= GS_MAX_QUEUE )
    {   /* Sanity Check */
        MSG_ERROR_1("Bad QID: Q=%d",gs_queue_id);
        cnt = 0; 
    }
    else
    {
        cnt = (word) q_cnt(QID_TO_QPTR(gs_queue_id));
    }

    return cnt;
}


/*===========================================================================

FUNCTION  gs_clear_messages

DESCRIPTION
  This function drains the named queue and removes the message.

RETURN VALUE
  None

===========================================================================*/
void gs_clear_messages (gs_queue_id_T gs_queue_id)
{
  cmd_type *cmd_ptr;

  /* Clear Message signal */
  (void)rex_clr_sigs( QID_TO_TCB(gs_queue_id), QID_TO_SIG(gs_queue_id) ); 

  /* Drain Message Queue */
  while ((cmd_ptr = (cmd_type*) q_get( QID_TO_QPTR(gs_queue_id) )) != NULL)
  {
    if ( NULL == QID_TO_FREE_Q(gs_queue_id) )
    {   /* Deallocate Command Buffer */
#ifndef FEATURE_MODEM_HEAP
       gs_free( cmd_ptr );
#else
       modem_mem_free(cmd_ptr, MODEM_MEM_CLIENT_GSLIB);
#endif
    }
    else
    {   /* Return buffer to Free Queue */
       q_put( QID_TO_FREE_Q(gs_queue_id), &((cmd_type*)cmd_ptr)->link );
    }
  }   
}



/*===========================================================================

FUNCTION  gs_init

DESCRIPTION
  This module initializes the queue Id, the rex tcb, tcb signal as well as
  the receive and free queues.  This is used for static msg passing.

RETURN VALUE
  status -- GS_SUCCESS if sent, GS_FAILURE otherwise

===========================================================================*/
gs_status_T gs_init
(
gs_queue_id_T      gs_queue_id,
rex_tcb_type      *tcb_ptr,
rex_sigs_type      cmd_q_sig,
q_type            *rcv_q_ptr,
q_type            *rcv_free_q_ptr,
unsigned int       buffer_size,
byte               high_watermark,
gs_get_handler_ptr process_routine
)
{
    gs_status_T gs_status = GS_FAILURE;

    if (gs_queue_id <  GS_MAX_QUEUE)
    {
        // GENERAL PARAMETERS
        QID_TO_TCB(gs_queue_id)      = tcb_ptr;
        QID_TO_SIG(gs_queue_id)      = cmd_q_sig;
        QID_TO_QPTR(gs_queue_id)     = rcv_q_ptr;

        // SEND-SPECIFIC PARAMETERS
        QID_TO_FREE_Q(gs_queue_id)   = rcv_free_q_ptr;
        QID_TO_SIZE(gs_queue_id)     = buffer_size;
        QID_TO_PUTFUNC(gs_queue_id)  = NULL;          // Don't use specialized SEND routine

        // RECEIVE-SPECIFIC PARAMETERS
        QID_TO_GETFUNC(gs_queue_id)  = process_routine;

        // Statistics
        QID_TO_WMARK(gs_queue_id)    = high_watermark;
        QID_TO_MAX_STAT(gs_queue_id) = 0;
        QID_TO_MSG_STAT(gs_queue_id) = 0;

        gs_status = GS_SUCCESS;
    }

    return gs_status;
}

// Simpler registration function
// for layers that are only providing
// gs_send_message() for other layers
gs_status_T gs_init_for_send_only
(
   gs_queue_id_T      gs_queue_id,
   gs_put_handler_ptr enqueue_routine
)
{
    gs_status_T gs_status = GS_FAILURE;

    if (gs_queue_id <  GS_MAX_QUEUE)
    {
        // SEND-SPECIFIC PARAMETERS
        QID_TO_PUTFUNC(gs_queue_id)  = enqueue_routine;
        gs_status = GS_SUCCESS;
    }

    return gs_status;
}



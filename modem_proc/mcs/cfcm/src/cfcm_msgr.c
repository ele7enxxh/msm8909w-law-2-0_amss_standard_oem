/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

            C O M M O N        F L O W        C O N T R O L        M A N A G E R        M O D U L E

                            Common Flow-control Manager(CFCM)  Module Source File

GENERAL DESCRIPTION

  This file provides CFCM solution file


EXTERNALIZED FUNCTIONS


REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS



  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=


==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cfcm/src/cfcm_msgr.c#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/07/14   rj      initial version
============================================================================*/

/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/
#include "mcs_variation.h"
#include <IxErrno.h>
#include <msgr_rex.h>

#include "cfcm_msgr.h"
#include "cfcmi.h"
#include "cfcm_client.h"
#include "cfcm_monitor.h"
#include "cfcm_cpu_monitor.h"


/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

============================================================================*/

/*=============================================================================

                         INTERNAL VARIABLES

=============================================================================*/

/* CFCM message type */
typedef struct
{
  /* qlink for the REX Q associated to this task */
  q_link_type            qlink;

  /* MSGR message within this packet */
  cfcm_msgr_rcv_msg_u     msg;

} cfcm_msgr_rcv_msg_type_s;


/*----------------------------------------------------------------------------
  CFCM MSGR global data structure
----------------------------------------------------------------------------*/

typedef struct 
{

/* Buffer for the received messages */
q_type msgr_rcv_q;

/* CFCM MSGR client id */
msgr_client_t            msgr_client_id;

/* CFCM MSGR queue id */
msgr_id_t                msgr_queue_id;


}cfcm_msgr_type_s;

/* Global CFCM task state variables */
static cfcm_msgr_type_s cfcm_msgr;



/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/

/*=======================================================================
FUNCTION CFCM_MSGR_REGISTER_MSG

DESCRIPTION
  To register a single msg/umid for CFCM MSGR's client

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
static void cfcm_msgr_register_msg (
  msgr_umid_type umid /*!< UMID of the message to Register */
)
{
  errno_enum_type  estatus;

  /*-----------------------------------------------------------------------*/

  estatus = msgr_register( MSGR_MCS_CFCM,
                           &cfcm_msgr.msgr_client_id,
                           MSGR_ID_REX,
                           umid );
  CFCM_ASSERT( E_FAILURE != estatus);

  /*-----------------------------------------------------------------------*/

} /* cfcm_msgr_register_msg */


/*=======================================================================
FUNCTION CFCM_MSGR_DEREGISTER_MSG

DESCRIPTION
  To de-register a single msg/umid for CFCM MSGR's client

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
static void cfcm_msgr_deregister_msg (
  msgr_umid_type umid /*!< UMID of the message to deregister */
)
{
  errno_enum_type  estatus;

  /*-----------------------------------------------------------------------*/

  estatus = msgr_deregister( MSGR_MCS_CFCM,
                             &cfcm_msgr.msgr_client_id,
                             umid );
  CFCM_ASSERT( E_FAILURE != estatus);

  /*-----------------------------------------------------------------------*/

} /* cfcm_msgr_deregister_msg */


/*=======================================================================
FUNCTION CFCM_MSGR_REGISTER_MSGS

DESCRIPTION
  To register for all related UMIDs for CFCM MSGR's client

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
static void cfcm_msgr_register_msgs ( void )
{
  /*-----------------------------------------------------------------------*/

  cfcm_msgr_register_msg( MCS_CFCM_CLIENT_REGISTER_REQ );
  cfcm_msgr_register_msg( MCS_CFCM_CLIENT_DEREGISTER_REQ );
  cfcm_msgr_register_msg( MCS_CFCM_MONITOR_IND );

  /*-----------------------------------------------------------------------*/

  return;
} /* cfcm_msgr_register_msgs */


/*=======================================================================
FUNCTION CFCM_MSGR_DEREGISTER_MSGS

DESCRIPTION
  To de-register for all related UMIDs for CFCM MSGR's client

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
static void cfcm_msgr_deregister_msgs ( void )
{
  /*-----------------------------------------------------------------------*/

  cfcm_msgr_deregister_msg( MCS_CFCM_CLIENT_REGISTER_REQ );
  cfcm_msgr_deregister_msg( MCS_CFCM_CLIENT_DEREGISTER_REQ );
  cfcm_msgr_deregister_msg( MCS_CFCM_MONITOR_IND );

  /*-----------------------------------------------------------------------*/

  return;
} /* cfcm_msgr_deregister_msgs */


/*=======================================================================
FUNCTION CFCM_MSGR_INIT

DESCRIPTION
  To initialize MSGR client & setup associated queue(s)

DEPENDENCIES
  None

RETURN VALUE
  errno_enum_type

SIDE EFFECTS
  None
========================================================================*/
errno_enum_type cfcm_msgr_init ( void )
{
  errno_enum_type retval = E_SUCCESS;

  /*-----------------------------------------------------------------------*/

  /* Initialize the queues */
  (void) q_init( &cfcm_msgr.msgr_rcv_q );

  /* Create the MSGR client */
  retval = msgr_client_create( &cfcm_msgr.msgr_client_id );

  if( E_SUCCESS == retval )
  {
    /* Add the CFCM REX Q with the MSGR client */
    retval = msgr_client_add_rex_q ( &cfcm_msgr.msgr_client_id,
                                     &cfcm_tcb,
                                     CFCM_MSGR_Q_SIG,
                                     &cfcm_msgr.msgr_rcv_q,
                                     MSGR_NO_QUEUE,
                                     (uint16)sizeof( q_link_type ),
                                     MSGR_NO_OFFSET,
                                     (uint16)sizeof( cfcm_msgr_rcv_msg_type_s ),
                                     &cfcm_msgr.msgr_queue_id );
  }

  /* Register for all necessary umids */
  if ( E_SUCCESS == retval )
  {
    /* Register for all necessary umids */
    cfcm_msgr_register_msgs(); 
  }

  /*-----------------------------------------------------------------------*/

  return retval;
} /* cfcm_msgr_init */


/*=======================================================================
FUNCTION CFCM_MSGR_DEINIT

DESCRIPTION
  To de-initialize MSGR client & associated queue(s)

DEPENDENCIES
  None

RETURN VALUE
  errno_enum_type

SIDE EFFECTS
  None
========================================================================*/
errno_enum_type cfcm_msgr_deinit ( void )
{
  errno_enum_type  retval;

  /*-----------------------------------------------------------------------*/

  /* Deregister for all umids */
  cfcm_msgr_deregister_msgs();

  /* Clear REX signals */
  (void) rex_clr_sigs( &cfcm_tcb, CFCM_MSGR_Q_SIG );

  /* Delete client with the MSGR */
  retval = msgr_client_delete( &cfcm_msgr.msgr_client_id );

  /*-----------------------------------------------------------------------*/

  return retval;
} /* cfcm_msgr_deinit */


/*=======================================================================
FUNCTION CFCM_MSGR_SEND_MSG

DESCRIPTION
  Send a message via MSGR

DEPENDENCIES
  None

RETURN VALUE
  errno_enum_type

SIDE EFFECTS
  None
========================================================================*/
errno_enum_type cfcm_msgr_send_msg (
  msgr_hdr_struct_type *msg_hdr_ptr,  /*!< Pointer to MSG Header  */
  msgr_umid_type        umid,         /*!< UMID of the message    */
  int32                 size          /*!< Size of the message    */
)
{
  errno_enum_type  retval;

  /*-----------------------------------------------------------------------*/

  /* Initialize the message header */
  msgr_init_hdr( msg_hdr_ptr, MSGR_MCS_CFCM, umid );

  /* Send the message */
  retval = msgr_send( msg_hdr_ptr, size );

  /*-----------------------------------------------------------------------*/

  return retval;
} /* cfcm_msgr_send_msg */

/*=======================================================================
FUNCTION CFCM_MSGR_PROCESS_RCVD_MSG

DESCRIPTION
  Process CFCM's received MSGR message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
static void cfcm_msgr_process_rcvd_msg (
  void *rcv_msgr_msg_ptr /*!< Pointer to the rcvd msgr msg */
)
{
  msgr_hdr_struct_type *msg_ptr;
  errno_enum_type       ret_status;

  /*-----------------------------------------------------------------------*/

  CFCM_ASSERT( NULL != rcv_msgr_msg_ptr );

  msg_ptr = (msgr_hdr_struct_type*) rcv_msgr_msg_ptr;

  switch ( msg_ptr->id )
  {
    case MCS_CFCM_CLIENT_REGISTER_REQ:
      /* Handle Register a Client Request */
      cfcm_client_proc_reg((cfcm_reg_req_msg_type_s*) rcv_msgr_msg_ptr);
      break;

    case MCS_CFCM_CLIENT_DEREGISTER_REQ:
      /* Handle De-Register a Client Request */
      cfcm_client_proc_dereg((cfcm_dereg_req_msg_type_s*) rcv_msgr_msg_ptr);
      break;

    case MCS_CFCM_MONITOR_IND:
      /* Handle Monitor indication */
      cfcm_monitor_proc_update((cfcm_monitor_ind_msg_s*)rcv_msgr_msg_ptr);
      break;

    case MCS_CFCM_LOOPBACK_SPR:
      /* Handle Loopback indication */
      ret_status = msgr_loopback_reply(
               (msgr_spr_loopback_struct_type*) rcv_msgr_msg_ptr, MSGR_MCS_CFCM);
      CFCM_ASSERT(ret_status == E_SUCCESS);
      break;

    default:
      break;
  } /* end switch */

} /* cfcm_msgr_process_rcvd_msg */

/*=======================================================================
FUNCTION CFCM_MSGR_PROCESS_SIG

DESCRIPTION
  Process the CFCM's MSGR REX signal & associated queue

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
void cfcm_msgr_process_sig ( void )
{
  cfcm_msgr_rcv_msg_type_s *rcv_msg_ptr;

  /*-----------------------------------------------------------------------*/

  /* There is a chance of getting more than one message on the
     same signal. Hence do a while loop to process all messages */

  /* Get the received message off the queue and process it */
  while ( ( rcv_msg_ptr = (cfcm_msgr_rcv_msg_type_s*)q_get(&cfcm_msgr.msgr_rcv_q) ) != NULL )
  {
    //@TODO: DO we need to add WDOG at this point??? as done in LMTSMGR
    /* Check if we need to pet dog */
	  //cfcm_task_check_dog();
	
    /* Now process the received message */
    cfcm_msgr_process_rcvd_msg( (void*) &(rcv_msg_ptr->msg) );

    /* Once the processing is over, free the buffer */
    msgr_rex_free_msg_buf( &(rcv_msg_ptr->qlink) );
  } /* End while loop */

  /*-----------------------------------------------------------------------*/

  return;
} /* cfcm_msgr_process_sig */



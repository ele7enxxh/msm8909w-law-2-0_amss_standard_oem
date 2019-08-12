/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          H D R  D A T A  S E R V I C E S  T A S K  H A N D L E R

GENERAL DESCRIPTION
  This module is the task level handler for HDR data service modules. The
  module handles queuing of events (messages, commands & indications) for the
  ds task and de-queuing and calling the appropriate processing functions
  from the ds task.

EXTERNALIZED FUNCTIONS

  hdrds_queue_msg() - Queues a message to be processed in the ds task context
  hdrds_queue_cmd() - Queues a command to be processed in the ds task context
  hdrds_queue_ind() - Queues a indication to be processed in the ds task
                      context

INITIALIZATION AND SEQUENCING REQUIREMENTS
  hdrds_init() should be called.

Copyright (c) 2001-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/src/hdrds.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/26/13   sc      Replace all the memcpy and memmove with memscpy and 
                   memsmove as its safer.
11/18/11   vpk     HDR SU API cleanup
05/12/11   op      Migrated to MSG 2.0 macros
02/08/11    ls     Global variable cleanup
03/10/06   spn     Increased the hdds buffers from 5 to 20.
10/13/03   vas     Changed message level from Msg Fatal to Med.
09/10/03   mpa     Converted F3 messages to use MSG2.0 SSID
08/28/03   vas     Code cleanup
02/03/03   ht      Based on asw/COMMON/vcs/hdrds.c_v   1.6
                   Add changes to support HDR 3GData call.
03/01/02   hcg     Set initial value for hdrds_ps_q_lim.  Remove #include for
                   hdrdsanauth.h.
12/09/01   vr      Made hdrds_ps_q_lim a variable so that we can have 
                   different limits for relay and net. model calls
12/09/01   vr      Added flag hdrds_ps_q_lim_exceeded for FL flow control
12/01/01   vas     Added support for HDR authentication state module
11/09/01   rao     Added DSM REUSE Optimization support.
10/17/01   vas     Changes to use the ds command queue instead of a signal
07/30/01   vas     Created module.

===========================================================================*/

/* <EJECT> */

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "err.h"
#include "amssassert.h"
#include "task.h"
#include "data_msg.h"

#include "hdrcom_api.h"
#include "hdrds.h"
#include "dsm.h"

#include "hdrpac.h"
#include "data_err_suppr.h"
#include <stringl/stringl.h>

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* HDRDS runs in the ds task */

/* Enumeration for type of Event - command, indication or message */
typedef enum
{
  HDRDS_COMMAND,    /* The item is a command */
  HDRDS_INDICATION, /* The item is an indication */
  HDRDS_MESSAGE    /* The item is a message (DSM item) */
} hdrds_event_enum_type;

/* This structure is common to all events (msgs, ind, cmd) being queued */
typedef struct
{
  q_link_type link;            /* Queue link */
  q_type *done_q_ptr;          /* Queue to place buffer on when done */
  hdrcom_hai_protocol_name_enum_type protocol_name; /* Who the command is for */
  hdrds_event_enum_type event; /* What type of item is this header for */
} hdrds_event_hdr_type;

/* Union of commands for the various protocols */
typedef union
{
  hdrpac_cmd_type pac;  /* PAC Command  structure                   */
} hdrds_cmd_union_type;

/* Any event for HDR DS that is queued on the main DS command queue has a
   pointer to a struct of this type
*/
typedef struct
{
  hdrds_event_hdr_type hdr;   /* Header - tells whether event being queued
                              ** is command, indication or message */
  union
  {
    hdrds_cmd_union_type cmd; /* Commands use this structure */
    hdrcom_buf_ind_type ind;      /* Indications use this structure */
    dsm_item_type *msg_ptr;
  } item; 
} hdrds_event_type;

/* Constant describing the number of buffers on event queue */
#define HDRDS_EVENT_BUFS_NUM 20 

static struct
{
  q_type event_free_q; /* Free Queue for cmd, ind, and msg buffers */

  /* Statically allocated space for free queue buffers */
  hdrds_event_type event_pool[HDRDS_EVENT_BUFS_NUM];
} hdrds;

#if defined(FEATURE_HDR_DSMREUSE_OPT)
/*---------------------------------------------------------------------------
  Item stack used in DSM REUSE optimization   
---------------------------------------------------------------------------*/
static hdrds_dsmreuse_stack_type hdrds_dsmreuse_stack;
#endif /* (FEATURE_HDR_DSMREUSE_OPT) */

/*---------------------------------------------------------------------------
  For Net-mdl tput issue
  Flag to help flow control FL based on number of items in PS PPP RX queue
---------------------------------------------------------------------------*/
static uint16  hdrds_ps_q_lim = HDRDS_NETMDL_PS_Q_LIM;
static boolean hdrds_ps_q_lim_exceeded = FALSE;

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION      GET_HDRDS_PS_Q_LIM_EXCEEDED_PTR

DESCRIPTION   Accessor function for hdrds_ps_q_lim_exceeded
  
DEPENDENCIES  None

RETURN VALUE  Address of hdrds_ps_q_lim_exceeded

SIDE EFFECTS  None
===========================================================================*/
boolean* get_hdrds_ps_q_lim_exceeded_ptr 
(
  void
)
{
  return &hdrds_ps_q_lim_exceeded;
} /* get_hdrds_ps_q_lim_exceeded_ptr */

/*===========================================================================
FUNCTION      GET_HDRDS_PS_Q_LIM_PTR

DESCRIPTION   Accessor function for hdrds_ps_q_lim
  
DEPENDENCIES  None

RETURN VALUE  Address of hdrds_ps_q_lim

SIDE EFFECTS  None
===========================================================================*/
uint16* get_hdrds_ps_q_lim_ptr 
(
  void
)
{
  return &hdrds_ps_q_lim;
} /* get_hdrds_ps_q_lim */

/*===========================================================================
FUNCTION HDRDS_INIT

DESCRIPTION
  This function initializes the HDR DS module..

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrds_init( void )
{
  int i;  /* index counter */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize queues. */
  ( void ) q_init( &hdrds.event_free_q );

  /* Fill hdrds.event_free_q. */
  for ( i = 0; i < HDRDS_EVENT_BUFS_NUM; i++ )
  {
    hdrds.event_pool[i].hdr.done_q_ptr = &hdrds.event_free_q;
    q_put( &hdrds.event_free_q,
    q_link( &hdrds.event_pool[i], &hdrds.event_pool[i].hdr.link ) );
  }

} /* hdrds_init() */

/* <EJECT> */

/*===========================================================================
FUNCTION HDRDS_QUEUE_MSG

DESCRIPTION
  This function queues a message on the hdrds task event queue and sends a 
  command to the main ds task command handler. Along with the message, the
  stream that identifies the instance of the protocol for which the message 
  is meant, is stored.  This function is called in the task context of 
  whoever is queuing the received message.
  
DEPENDENCIES
  None.

PARAMETERS
  protocol_name - Who the message is for
  item_ptr      - Pointer to the message to be queued

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrds_queue_msg
(
  hdrcom_hai_protocol_name_enum_type protocol_name,
  dsm_item_type *item_ptr
)
{

  ds_cmd_type  *cmd_ptr;     /* Pointer to command buffer               */
  hdrds_event_type *hdrds_event_type_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 
  cmd_ptr = ds_allocate_cmd_buf(sizeof(hdrds_event_type));

  /** Get command buffer to do task switch */
  if( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) )
  {
    ASSERT(0);
    return;
  }

  hdrds_event_type_ptr = (hdrds_event_type* ) cmd_ptr->cmd_payload_ptr;
  
  /* Fill in the fields identifying this message */
  hdrds_event_type_ptr->hdr.event = HDRDS_MESSAGE;
  hdrds_event_type_ptr->hdr.protocol_name = protocol_name;
  hdrds_event_type_ptr->item.msg_ptr = item_ptr;


  DATA_HDR_MSG0(MSG_LEGACY_MED, "3G hdrds_queue_msg");
  
    /*---------------------------------------------------------------------
      Fill in the command parameters and send the command.
    ---------------------------------------------------------------------*/
  cmd_ptr->hdr.cmd_id = DS_CMD_HDR_EVENT;
  /*lint +e740 */
  ds_put_cmd( cmd_ptr );

} /* hdrds_queue_msg() */

/* <EJECT> */

/*===========================================================================
FUNCTION HDRDS_QUEUE_IND

DESCRIPTION
  This function queues an indication on the hdrds task event queue and sends
  a command to the main ds task command handler. If there is any data 
  associated with the indication, it is copied into the buffer before it is 
  queued.  This function is called in the task context of whoever is giving 
  the indication. 
  
DEPENDENCIES
  None.

PARAMETERS
  protocol_name - Who the indication is for
  ind_name - Global enumeration of indications
  ind_data_ptr - Pointer to the indication data

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrds_queue_ind
(
  hdrcom_hai_protocol_name_enum_type protocol_name,
  hdrcom_ind_name_enum_type ind_name,
  void *ind_data_ptr
)
{

  uint32 ind_data_size;        /* Size of data with indication             */
  hdrds_event_type *event_ptr; /* Pointer to event being queued            */
  ds_cmd_type  *ds_cmd_ptr;       /* Pointer to command buffer               */
  DATA_MDM_ARG_NOT_CONST(ind_data_ptr);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_cmd_ptr = ds_allocate_cmd_buf(sizeof(hdrds_event_type));

  /** Get command buffer to do task switch */
  if( (NULL == ds_cmd_ptr) || (NULL == ds_cmd_ptr->cmd_payload_ptr) )
  {
    ASSERT(0);
    return;
  }
  event_ptr = (hdrds_event_type *) ds_cmd_ptr->cmd_payload_ptr;

  /* Fill in the fields identifying this message */
  event_ptr->hdr.event = HDRDS_INDICATION;
  event_ptr->hdr.protocol_name = protocol_name;
  event_ptr->item.ind.ind_name = ind_name;
  if ( ind_data_ptr != NULL )
  {
    ind_data_size = hdrcom_ind_get_ind_data_size (ind_name);

    ASSERT (ind_data_size <= sizeof(hdrcom_ind_data_union_type) );
    if ( ind_data_size <= sizeof(hdrcom_ind_data_union_type) )
    {
      (void) memscpy( &event_ptr->item.ind.ind_data,
      	              sizeof(hdrcom_ind_data_union_type), 
      	              ind_data_ptr, 
                      ind_data_size );
    }
  }
    /*---------------------------------------------------------------------
      Fill in the command parameters and send the command.
    ---------------------------------------------------------------------*/
    ds_cmd_ptr->hdr.cmd_id = DS_CMD_HDR_EVENT;

   /*lint +e740 */
    ds_put_cmd( ds_cmd_ptr );
  

} /* hdrds_queue_ind() */

/*===========================================================================
FUNCTION HDRDS_QUEUE_IND_EXT

DESCRIPTION
  This function is a wrapper to hdrds_queue_ind with the indication info (third
  argument) restricted to hdrcom_ind_data_union_type. hdrds_queue_ind_ext should 
  be in the APP Side and will be called from Modem side

DEPENDENCIES
  None.

PARAMETERS
  protocol_name - Who the indication is for
  ind_name - Global enumeration of indications
  ind_data_ptr - Pointer to the indication data

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void hdrds_queue_ind_ext
(
  hdrcom_hai_protocol_name_enum_type protocol_name,
  hdrcom_ind_name_enum_type ind_name,
  hdrcom_ind_data_union_type ind_data /*~ PARAM ind_data DISC ind_name */
)
{
  DATA_MDM_ARG_NOT_USED(protocol_name);
  hdrds_queue_ind( HDRHAI_PACKET_APPLICATION_CONTROLLER, ind_name, (void*)&ind_data );
}/*hdrds_queue_ind_ext*/

/*===========================================================================
FUNCTION HDRDS_QUEUE_CMD

DESCRIPTION
  This function queues an indication on the hdrds task event queue and sends
  a command to the main ds task command handler. The command data is copied 
  into the command buffer before it is queued. This function is called in the
  task context of whoever is giving the command.
  
DEPENDENCIES
  None.

PARAMETERS
  protocol_name - Who the command is for
  cmd_ptr - Pointer to a the command
  cmd_size - Size of the command being given

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrds_queue_cmd
(
  hdrcom_hai_protocol_name_enum_type protocol_name,
  void *cmd_ptr,
  uint32 cmd_size
)
{

  hdrds_event_type *event_ptr; /* Pointer to event being queued            */
  ds_cmd_type  *ds_cmd_ptr;     /* Pointer to command buffer               */
  DATA_MDM_ARG_NOT_CONST(cmd_ptr);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( cmd_ptr == NULL )
  {
    DATA_HDR_MSG0(MSG_LEGACY_ERROR, "No command to queue!");
    return;
  }

  ASSERT (cmd_size <= sizeof(hdrds_cmd_union_type) );
  DATA_HDR_MSG0(MSG_LEGACY_MED, "3G hdrds_queue_cmd");
  
  ds_cmd_ptr = ds_allocate_cmd_buf(sizeof(hdrds_event_type));
  /** Get command buffer to do task switch */
  if( (NULL == ds_cmd_ptr) || (NULL == ds_cmd_ptr->cmd_payload_ptr) )
  {
    ASSERT(0);
    return;
  }
  event_ptr = (hdrds_event_type*) ds_cmd_ptr->cmd_payload_ptr;


  /* Fill in the fields identifying this message */
  event_ptr->hdr.event = HDRDS_COMMAND;
  event_ptr->hdr.protocol_name = protocol_name;
  if (cmd_size <= sizeof(hdrds_cmd_union_type) )
  {
    (void) memscpy( &event_ptr->item.cmd,
    	            sizeof(hdrds_cmd_union_type),
    	            cmd_ptr, 
                    cmd_size );
  }


    /*---------------------------------------------------------------------
      Fill in the command parameters and send the command.
    ---------------------------------------------------------------------*/
    ds_cmd_ptr->hdr.cmd_id = DS_CMD_HDR_EVENT;

    /*lint +e740 */
    ds_put_cmd( ds_cmd_ptr );
  

} /* hdrds_queue_cmd() */

/* <EJECT> */

/*===========================================================================
FUNCTION HDRDS_PROCESS_MSG

DESCRIPTION
  This function determines the protocol for which the message is meant and 
  routes it appropriately.
  
DEPENDENCIES
  None.

PARAMETERS
  protocol_name - Who the message is for
  item_ptr      - Pointer to the message to be queued

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrds_process_msg
(
  hdrcom_hai_protocol_name_enum_type protocol_name,
  dsm_item_type *item_ptr
)
{
  DATA_MDM_ARG_NOT_CONST(item_ptr);
  switch ( protocol_name )
  {
     default:
      DATA_HDR_MSG1(MSG_LEGACY_ERROR, "MSG for unknown protocol %d",
                    protocol_name);
      break;
  }
} /* hdrds_process_msg() */

/* <EJECT> */

/*===========================================================================
FUNCTION HDRDS_PROCESS_IND

DESCRIPTION
  This function determines the protocol for which the indication is meant and
  routes it appropriately.
  
DEPENDENCIES
  None.

PARAMETERS
  protocol_name - Who the indication is for
  ind_name - Global enumeration of indications
  ind_data_ptr - Indication data

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrds_process_ind
(
  hdrcom_hai_protocol_name_enum_type protocol_name,
  hdrcom_ind_name_enum_type ind_name,
  hdrcom_ind_data_union_type *ind_data_ptr
)
{
  DATA_HDR_MSG1(MSG_LEGACY_ERROR, "IND for protocol %d",
                    protocol_name);
  switch ( protocol_name )
  {
    case HDRHAI_PACKET_APPLICATION_CONTROLLER:
      hdrpac_process_ind( ind_name, ind_data_ptr );
      break;

    case HDRHAI_FLOW_CONTROL_PROTOCOL:
      /* Currently FCP does not process any indications */
      
      break;

    default:
      break;
  }
} /* hdrds_process_ind() */

/* <EJECT> */

/*===========================================================================
FUNCTION HDRDS_PROCESS_CMD

DESCRIPTION
  This function determines the protocol for which the command is meant and 
  routes it appropriately.
  
DEPENDENCIES
  None.

PARAMETERS
  protocol_name - Which protocol the command is for
  cmd_name  - Actual command

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrds_process_cmd
(
  hdrcom_hai_protocol_name_enum_type protocol_name,
  hdrds_cmd_union_type cmd 
)
{
  switch( protocol_name )
  {
    case HDRHAI_PACKET_APPLICATION_CONTROLLER:
    hdrpac_process_cmd( &cmd.pac );
    break;
    

  default:
      DATA_HDR_MSG1(MSG_LEGACY_ERROR, "CMD for unknown protocol %d",
                    protocol_name);
      break;
  }
} /* hdrds_process_cmd() */

/* <EJECT> */

/*===========================================================================
FUNCTION HDRDS_PROCESS_HDR_EVENT_CMD

DESCRIPTION
  The function is called from the ds task whenever there is a event
  (message, indication or command) for a HDR DS protocol. The function
  determines the particular event and routes it to the appropriate
  protocol processing function.
  
DEPENDENCIES
  None.

PARAMETERS
  ds_cmd_ptr - Pointer to event being processed 

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrds_process_hdr_event_cmd
(
  ds_cmd_type  *ds_cmd_ptr
)
{
  hdrds_event_type* event_ptr; /* Pointer to event being processed */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(ds_cmd_ptr);
  ASSERT( ds_cmd_ptr->hdr.cmd_id == DS_CMD_HDR_EVENT);
  /*lint -save -e826 */
  /*lint -save -e740 */
  event_ptr = (hdrds_event_type* ) ds_cmd_ptr->cmd_payload_ptr;
  /*lint -restore */

  /* Determine event */
  if( event_ptr->hdr.event == HDRDS_COMMAND )
  {
    hdrds_process_cmd( event_ptr->hdr.protocol_name,
                       event_ptr->item.cmd );
  }
  else if( event_ptr->hdr.event == HDRDS_INDICATION )
  {
    hdrds_process_ind( event_ptr->hdr.protocol_name,
                       event_ptr->item.ind.ind_name,
                       &event_ptr->item.ind.ind_data ); 
  }
  else if( event_ptr->hdr.event == HDRDS_MESSAGE )
  {
    hdrds_process_msg( event_ptr->hdr.protocol_name,
                       event_ptr->item.msg_ptr );
  }
  else
  {
    DATA_HDR_MSG0(MSG_LEGACY_ERROR, "Unrecognized event");
  }

} /* hdrds_process_event() */

/* <EJECT> */

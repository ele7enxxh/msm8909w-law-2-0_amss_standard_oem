/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 HIT DIAG Handler
GENERAL DESCRIPTION
  This file contains functions associated with handling hit packets from diag.

EXTERNALIZED FUNCTIONS
  hitdiag_handler
  
REGIONAL FUNCTIONS
  hitdiagcmd_signal

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2006, 2007 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/hit/src/hitdiag.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     -------------------------------------------------------
01/15/2013   smd     Featurized hit cmd and hit diag.
05/07/2007   vh      Changed dynamic memory allocation to static one
03/28/2007   vh      Created

==========================================================================*/

/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/
#include "hdr_variation.h"
#include "customer.h"

#ifdef FEATURE_HIT
#ifdef FEATURE_HIT_DIAG_AND_CMD

#include "comdef.h"
#include "diagpkt.h"
#include "hitcmd.h"
#include "hitdiag.h"

/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

hitcmd_ret_type hitdiag_cmd_handler(hitcmd_buf_type*);

extern hitcmd_ret_type hitapp_cmd_handler(hitcmd_buf_type*);
  /* command handler that will queue cmd to hitapp-task-queue */

static hitcmd_handler_info hitdiag_cmd_handler_table[]=
{
  {0x0000, hitdiag_cmd_handler}, /* handler to execute hitdiag commands*/
  {0xffff, hitapp_cmd_handler},     /* handler to execute all other commands */
};


/*
   Indicates the mode of operation of the HIT framework
   TRUE: Queue commands without exiecuting them
   FALSE: execute command as soon as possible
*/
static boolean queue_mode = FALSE;

/*
  This is the paramter structure definition for the hitdiagcmd_signal
*/
typedef PACKED struct
{
  uint16 testapp_id;
}hitdiag_queue_param_type;

/*===========================================================================

FUNCTION hitdiag_queue

DESCRIPTION
  This procedure allows the selection of the mode of operation of the Hit 
  Framework. The Framework can operate in BLOCKING or NONBLOCKING mode.
  This function also allows to signal a specific task that belongs to test 
  application registered to the HIT framework.

DEPENDENCIES
  None

PARAMETERS
  cmd_buf_ptr

RETURN VALUE
  status 

SIDE EFFECTS
  None

===========================================================================*/
hitcmd_ret_type hitdiag_queue
( 
  hitdiag_queue_param_type* param_ptr
)
{
  hitcmd_ret_type status = HITCMD_ERROR_NONE;
    /* status of the command */

  hitcmd_handler_type handler;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Set the mode (BLOCKING/NON-BLOCKING 
     Or else signal a given test application */
  switch( param_ptr->testapp_id )
  {
  case HIT_DIAG:
  {
    queue_mode = FALSE;
    MSG( MSG_SSID_HIT, MSG_LEGACY_HIGH, \
         "hitdiag_queue: mode set to BLOCKING" );
    break;
  }
  case HIT_QUEUE_MODE_ENABLE:
  {
    queue_mode = TRUE;
    MSG( MSG_SSID_HIT, MSG_LEGACY_HIGH, \
         "hitdiag_queue: mode set to NON-BLOCKING" );
    break;
  }
  default:
  {
    handler = hitcmd_handler(param_ptr->testapp_id, hitdiag_cmd_handler_table);

    if (handler)
    {
      status = handler(NULL); /* signal task */
      MSG_1( MSG_SSID_HIT, MSG_LEGACY_HIGH, \
           "hitdiag_queue: testapp=%d queue triggered", param_ptr->testapp_id);
    }
    else
    {
      status = HITCMD_ERROR_PARAM;
    }
    break;
  }
  }/* switch( param_ptr->testapp_id ) */

  return status;

} /* hitdiag_queue */

/*===========================================================================

FUNCTION hitdiag_cmd_handler

DESCRIPTION
  This procedure executes hit diag commands

DEPENDENCIES
  None

PARAMETERS
  cmd_buf_ptr

RETURN VALUE
  status 

SIDE EFFECTS
  None

===========================================================================*/
hitcmd_ret_type hitdiag_cmd_handler
(
  hitcmd_buf_type *cmd_buf_ptr
    /* pointer to command buffer */
)
{
  hitcmd_ret_type status = HITCMD_ERROR_NONE;

  switch ( cmd_buf_ptr->cmd_id )
  {
  case 0: /* hit_queue command */
    status = hitdiag_queue( 
               (hitdiag_queue_param_type*)(cmd_buf_ptr->param) );
    break;
  default:
      status = HITCMD_ERROR_CMD_ID;
  }

  return status;

} /* hitdiag_cmd_handler */

/* struct for immediate response packet used by hitdiag_handler */
PACKED typedef struct 
{
diagpkt_subsys_header_v2_type hdr;
hitcmd_ret_type status;
} hitdiag_imm_rsp_type;


/*=============================================================================

FUNCTION HITDIAG_HANDLER

DESCRIPTION
  The hitdiag_handler method gets the packet from diag. An immediate response  
  is generated using diag API. The immediate response generated contains a  
  delayed response id which is to be used by the different test application 
  tasks for sending delayed responses when they finish processing the command. 

  The hit diag handler maintains a vector table of the interface functions to  
  the different test application tasks. The hit handler indexes the table using 
  the interface id stored in the incoming packet and calls the function. The  
  interface functions just put the command in their respective queues and  
  returns a status of their operation. The status returned is stored in the 
  immediate response structure and is returned to diag. 

DEPENDENCIES
  None

PARAMETERS
  packet_ptr
  pkt_len

RETURN VALUE
  immediate response pointer

SIDE EFFECTS
  None

=============================================================================*/
PACKED void* hitdiag_handler
(
  PACKED void* packet_ptr,
    /* incoming diag cmd packet */

  uint16 pkt_len
    /* length of packet */
)
{ 
  hitcmd_buf_type *cmd_buf_ptr = NULL;
    /* pointer to store command pkt */

  diagpkt_subsys_delayed_rsp_id_type delayed_rsp_id;
    /* variable to store delayed response id assigned by diag */

  diagpkt_subsys_rsp_cnt rsp_cnt = 0x8000; 
    /* response count of a command, initialized to "immediate response" */

  hitcmd_ret_type status = HITCMD_ERROR_NONE;

  hitdiag_imm_rsp_type *imm_rsp_ptr = NULL;

  hitcmd_handler_type handler;
    /* commnd handler */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_buf_ptr = (hitcmd_buf_type *) packet_ptr;
  imm_rsp_ptr = (hitdiag_imm_rsp_type *) 
    diagpkt_subsys_alloc_v2( cmd_buf_ptr->subsys_id, 
                                         cmd_buf_ptr->testapp_id, 
                                         sizeof( hitdiag_imm_rsp_type ) );

  if ( imm_rsp_ptr )
  {
    /* Get delayed response id from diag and allocate memory for command */
    delayed_rsp_id = diagpkt_subsys_get_delayed_rsp_id( imm_rsp_ptr );
    cmd_buf_ptr = hitcmd_alloc( packet_ptr, pkt_len, delayed_rsp_id );

    if (cmd_buf_ptr == NULL)
    {
      rsp_cnt = 0;
      status = HITCMD_ERROR_MEM_ALLOC;
    } /* if ( cmd_buf_ptr == NULL ) */
    else if ( !hitcmd_refresh(cmd_buf_ptr) )
    {
      handler = hitcmd_handler(cmd_buf_ptr->testapp_id, hitdiag_cmd_handler_table);

      if (handler)
      {
        status = handler(cmd_buf_ptr);
      }
      else
      {
        status = HITCMD_ERROR_TESTAPP_ID;
      }

      if(cmd_buf_ptr->testapp_id==HIT_DIAG || status!=HITCMD_ERROR_NONE)
      {
        rsp_cnt = 0;
        hitcmd_free(cmd_buf_ptr);
      }
      else if( !queue_mode )
      {
        handler(NULL);
      }
    } /* else if ( !hit_refresh_cmd(cmd_buf_ptr) ) */

    diagpkt_subsys_set_status( imm_rsp_ptr, status );
      /* set the status */
    diagpkt_subsys_set_rsp_cnt( imm_rsp_ptr, rsp_cnt );
      /* set the response cnt */
  }
  else
  {
    MSG( MSG_SSID_HIT, MSG_LEGACY_FATAL, \
         "hitdiag_handler: diagpkt_subsys_alloc_v2 failed" );
  }

  return imm_rsp_ptr;

} /* end hitdiag_handler() */

#endif /* FEATURE_HIT_DIAG_AND_CMD */
#endif /* FEATURE_HIT */

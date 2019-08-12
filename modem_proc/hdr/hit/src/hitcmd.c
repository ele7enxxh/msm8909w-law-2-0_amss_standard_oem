/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        HIT COMMAND SERVICE UTILITY FUNCTIONS

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS
    hitcmd_alloc
    hitcmd_free
    hitcmd_param_ptr
    hitcmd_send_status
    hitcmd_send_rsp_pkt
    hitcmd_refresh
    
REGIONAL FUNCTIONS
    None

INITIALIZATION AND SEQUENCING REQUIREMENTS
    None
    
    Copyright (c) 2006 - 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/hit/src/hitcmd.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     -------------------------------------------------------
01/15/2013   smd     Featurized hit cmd and hit diag.
01/15/2014   vko     Fixed compiler warnings
12/19/2013   sat     Changed memcpy() to assignment or memscpy() (safer version).
05/07/2007   vh      Changed dynamic memory allocation to static one
03/28/2007   vh      Created

===========================================================================*/

/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/
#include "hdr_variation.h"
#include "customer.h"

#ifdef FEATURE_MEMCPY_REMOVAL
#include "stringl.h"
#endif /* FEATURE_MEMCPY_REMOVAL */

#include "amssassert.h"

#ifdef FEATURE_HIT
#ifdef FEATURE_HIT_DIAG_AND_CMD

#include "comdef.h"
#include "diagpkt.h"
#include "hitcmd.h"
#include "hdrdebug.h"

/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

/* HITCMD_MAX_ELEMENTS is the maximun number of commands alive */
#define HITCMD_ARRAY_MAX   25

/* Header size: this is the size of the following fields in hit_cmd_buf_type
   code, subsys_id, testapp_id, cmd_id, delay */
#define HITCMD_HDR_SIZE  (sizeof(diagpkt_subsys_header_type) + \
                           sizeof(uint8) + sizeof(uint32))

/* Structure to represent the delayed response packet */
PACKED typedef struct 
{
  diagpkt_subsys_header_v2_type hdr;
  uint32 time_stamp;
  hitcmd_ret_type status;
} hitcmd_delayed_rsp_type;

/*==========================================================================

                     GLOBAL VARIABLES FOR MODULE

==========================================================================*/

static hitcmd_buf_type hitcmd_array[HITCMD_ARRAY_MAX];
  /* The command queue */

/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/

/*===========================================================================

FUNCTION hitcmd_init

DESCRIPTION
  Initializes the hitcmd_array

DEPENDENCIES
  None

RETURN VALUE
  hit_cmd_buf_type*  

SIDE EFFECTS
  None

===========================================================================*/
void hitcmd_init( void )
{
  uint8 i;

  for (i=0; i<HITCMD_ARRAY_MAX; i++)
  {
    hitcmd_array[i].rsp_id = 0;
  }

} /* hitcmd_init */


/*===========================================================================

FUNCTION hitcmd_alloc

DESCRIPTION
  Allocates memory for a command buffer

DEPENDENCIES
  None

PARAMETERS
  packet
  packet_len
  rsp_id

RETURN VALUE
  hitcmd_buf_type *

SIDE EFFECTS
  None

===========================================================================*/
hitcmd_buf_type* hitcmd_alloc
(
  PACKED uint8 *packet,
    /* incomming diag cmd packet */

  uint16 packet_len,
    /* lenght of the incoming packet */

  diagpkt_subsys_delayed_rsp_id_type rsp_id
    /* response id for the command - assigned by diag */
)
{
  hitcmd_buf_type *cmd_buf_ptr = NULL;
    /* pointer to store command pkt */

  uint8 i;
    /* loop index */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(packet==NULL || packet_len<HITCMD_HDR_SIZE)
  {
      MSG( MSG_SSID_HIT, MSG_LEGACY_FATAL, "hitcmd_alloc: bad parameters" );
      return NULL;
  }

  /* Find a available cmd buf chuck in the array */
  for (i=0; i<HITCMD_ARRAY_MAX; i++)
  {
    if (hitcmd_array[i].rsp_id == 0)
  {
      cmd_buf_ptr = &(hitcmd_array[i]);
      break;
    }
  }

  if (cmd_buf_ptr==NULL)
  {
    MSG( MSG_SSID_HIT, MSG_LEGACY_HIGH, "hitcmd_alloc: cmd queue full" );
    return NULL;
  } /* ( cmd_buf_ptr == NULL ) */

  cmd_buf_ptr->param_len = packet_len - HITCMD_HDR_SIZE;

  if ( cmd_buf_ptr->param_len > HITCMD_PARAM_MAX_BYTES )
  {
    MSG_1( MSG_SSID_HIT, MSG_LEGACY_FATAL, \
         "hitcmd_alloc: PARAM_MAX_BYTES not sufficient, required %d", \
           cmd_buf_ptr->param_len );
      return NULL;
  }

  /* copy the content of incoming packet to the cmd_buf */
#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy( (void*)cmd_buf_ptr, packet_len, (void*)packet, packet_len );
#else
  memcpy( (void*)cmd_buf_ptr, (void*)packet, packet_len );
#endif /* FEATURE_MEMCPY_REMOVAL */

  /* inititalize the response id and the response count (to zeroth reponse) */
  cmd_buf_ptr->rsp_id = rsp_id;
  cmd_buf_ptr->rsp_cnt = 0x8000;

  MSG_4( MSG_SSID_HIT, MSG_LEGACY_MED, \
       "hitcmd_alloc: testapp_id=%d, cmd_id=%d, param_len=%d, rsp_id=", \
        cmd_buf_ptr->testapp_id, cmd_buf_ptr->cmd_id, \
        cmd_buf_ptr->param_len, cmd_buf_ptr->rsp_id);

  return cmd_buf_ptr;

} /* hitcmd_alloc */


/*===========================================================================

FUNCTION hitcmd_free

DESCRIPTION
  Deallocates the comamnd buffer

DEPENDENCIES
  None

PARAMETERS
  cmd_buf_ptr
  
RETURN VALUE
  Void 

SIDE EFFECTS
  None

===========================================================================*/
void hitcmd_free
(
  hitcmd_buf_type *cmd_buf_ptr
    /* command buffer to be de-allocated */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( cmd_buf_ptr )
  {
    cmd_buf_ptr->rsp_id = 0; /* Now this buffer is available for new cmds */
  } /* end if ( cmd_buf_ptr ) */

} /* end hitcmd_free() */


/*=============================================================================

FUNCTION hitcmd_param_ptr

DESCRIPTION
  Returns the param_ptr

DEPENDENCIES
  None

PARAMETERS
  cmd_buf_ptr
  
RETURN VALUE
  param_ptr 

SIDE EFFECTS
  None

=============================================================================*/
void *hitcmd_param_ptr
(
  hitcmd_buf_type *cmd_buf_ptr
    /* pointer to the command buffer */
)
{
  if (cmd_buf_ptr)
  {
    return (void*)cmd_buf_ptr->param;
  }
  else
  {
    return NULL;
  }
} /* hitcmd_param_ptr */


/*=============================================================================

FUNCTION hitcmd_send_rsp_pkt

DESCRIPTION
  This procedure may be used by a command implementation to send diag responses
  to the host. A time stamp is added.
  A command may send multiple responses. It may indicate is a  response is the 
  final one. In case of a final response the command buffer is deallocated 
  after sending the response.
  This routine may be used to send a user defined response pkt.
  To send a uint16 status use hitcmd_send_status

DEPENDENCIES
  None

PARAMETERS
  cmd_buf_ptr
  pkt
  pkt len
  final_rsp
  
RETURN VALUE
  status: TRUE: The response was succesfully sent
          FALSE: Failed to send a response due to bad pointers.

SIDE EFFECTS
  None

===========================================================================*/
boolean hitcmd_send_rsp_pkt
(
  hitcmd_buf_type *cmd_buf_ptr,
    /* pointer to the command buffer */

  void *pkt,
    /* the pkt or reponse to be sent */

  uint16 pkt_len,
    /* pkt length in bytes */

  boolean final_rsp
    /* indicates if this is the final reponse for the command */
)
{
  hitcmd_delayed_rsp_type* delayed_rsp_ptr;
    /* pointer to store the delayed response */

  diagpkt_subsys_rsp_cnt rsp_cnt;
    /* variable to keep track of how many commands have been sent */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( cmd_buf_ptr==NULL || cmd_buf_ptr->rsp_id==0 )
  {
    MSG(MSG_SSID_HIT, MSG_LEGACY_FATAL, "hitcmd_send_rsp_pkt: Bad parameters");
    return FALSE;
  }

  /* Allocate a response packet */
  delayed_rsp_ptr = (hitcmd_delayed_rsp_type*) diagpkt_subsys_alloc_v2_delay(
    cmd_buf_ptr->subsys_id, 
    cmd_buf_ptr->testapp_id,
    cmd_buf_ptr->rsp_id,
    (sizeof(hitcmd_delayed_rsp_type)-sizeof(hitcmd_ret_type)+pkt_len));

  if ( delayed_rsp_ptr == NULL )
  {
    MSG( MSG_SSID_HIT, MSG_LEGACY_FATAL, \
         "hitcmd_send_rsp_pkt: memory allocation failure" );
    return FALSE;
  }

  /* copy user pkt to the rsp pkt */
#ifdef FEATURE_MEMCPY_REMOVAL
  ASSERT(pkt_len <= sizeof(delayed_rsp_ptr->status));
  /* copy user pkt to the rsp pkt */
  memscpy((void*)&(delayed_rsp_ptr->status), pkt_len, pkt, pkt_len);
#else
  memcpy((void*)&(delayed_rsp_ptr->status), pkt, pkt_len);
#endif /* FEATURE_MEMCPY_REMOVAL */

  /* update response count */
  rsp_cnt = ++(cmd_buf_ptr->rsp_cnt);

  MSG_2( MSG_SSID_HIT, MSG_LEGACY_MED, \
       "hitcmd_send_rsp_pkt: testapp_id=%d, cmd_id=%d", \
       cmd_buf_ptr->testapp_id, cmd_buf_ptr->cmd_id);

  /* If this is the final reponse */
  if ( final_rsp == TRUE )
  {
    rsp_cnt &= (~0x8000);
    hitcmd_free( cmd_buf_ptr );
    MSG( MSG_SSID_HIT, MSG_LEGACY_MED, "hitcmd_send_rsp_pkt: final resp");
  }

  /* Send the response */
  diagpkt_subsys_set_rsp_cnt( delayed_rsp_ptr, rsp_cnt );
  diagpkt_delay_commit( delayed_rsp_ptr );    

  return TRUE;
} /* hitcmd_send_rsp_pkt */


/*=============================================================================

FUNCTION hitcmd_send_status

DESCRIPTION
  This procedure may be used by a command implementation to send diag responses
  to the host. A time stamp is automatically added to the response.
  A command may send multiple responses. It may indicate is a  response is the 
  final one. In case of a final response the command buffer is deallocated 
  after sending the response.
  This routine may be used to send uint16 status

DEPENDENCIES
  None

PARAMETERS
  cmd_buf_ptr
  status
  final_rsp
  
RETURN VALUE
  status 

SIDE EFFECTS
  None

===========================================================================*/
boolean hitcmd_send_status
(
  hitcmd_buf_type *cmd_buf_ptr,
    /* pointer to the command buffer */

  hitcmd_ret_type status,
    /* the status or reponse to be sent */

  boolean final_rsp
    /* indicates if this is the final reponse for the command */
)
{
  return  hitcmd_send_rsp_pkt( cmd_buf_ptr, \
                               (void*)&status, sizeof(hitcmd_ret_type), \
                               final_rsp);
} /* hitcmd_send_status */

/*===========================================================================

FUNCTION hitcmd_refresh

DESCRIPTION
  Refreshes the comamnd buffer with the new rsp_id. This is a workaraound for 
  a limitation that ASIA has. ASIA will stop polling for response 10 mins
  after a command was issued. To workaround this for commands that may be 
  alive for more time - the host may resend the command with the 'delay' field 
  of the command populated with the response id of the original command, ORed
  with 0xffff0000.
  This routine searches for a command with the response id to be refreshed and 
  replaces old rsp_id with the rsp_id of the new cmd. Note that the new cmd 
  need not have the same parameters as the old command.

  The HIT framework is designed to be used with ASIA, though other tools may 
  be used.

DEPENDENCIES
  None

PARAMETERS
  new_cmd_ptr
  
RETURN VALUE
  Void 

SIDE EFFECTS
  None

===========================================================================*/
boolean hitcmd_refresh
(
    hitcmd_buf_type *new_cmd_ptr
      /* incomming diag cmd packet */
)
{
  hitcmd_buf_type *cmd_ptr;
  diagpkt_subsys_delayed_rsp_id_type rsp_id;
  uint8 i;

  if( new_cmd_ptr==NULL || new_cmd_ptr->rsp_id==0 )
  {
    MSG( MSG_SSID_HIT,  MSG_LEGACY_MED, "hitcmd_refresh: bad parameters" );
    return FALSE;
  }
  else if( new_cmd_ptr->delay<=0xffff0000 )
  {
    return FALSE;
  }

  rsp_id=(diagpkt_subsys_delayed_rsp_id_type)((new_cmd_ptr->delay)&0x0000ffff);

  for (i=0; i<HITCMD_ARRAY_MAX; i++)
  {    
    cmd_ptr = &(hitcmd_array[i]);

    /* if the all the ids match then refresh the matched cmd */
    if ( (cmd_ptr->rsp_id  == rsp_id) && 
         (cmd_ptr->testapp_id == new_cmd_ptr->testapp_id) &&
         (cmd_ptr->cmd_id == new_cmd_ptr->cmd_id)
        )
    {

      MSG_3( MSG_SSID_HIT, MSG_LEGACY_MED, \
             "hitcmd_refresh: testapp_id=%d, cmd_id=%d rsp_id=%d", \
             cmd_ptr->testapp_id, cmd_ptr->cmd_id, cmd_ptr->rsp_id);

      /* copy the new rsp_id into the cmd buffer */      
      cmd_ptr->rsp_id = new_cmd_ptr->rsp_id;

      /* free the old cmd_buf by sending a final response */
      new_cmd_ptr->rsp_id = rsp_id;
      new_cmd_ptr->rsp_cnt = cmd_ptr->rsp_cnt;
      hitcmd_send_status (new_cmd_ptr, HITCMD_REFRESHED, TRUE);

      /* set the rsp_cnt to first response */
      cmd_ptr->rsp_cnt = 0x8000;

      return TRUE;
    }
  }

  return FALSE;

} /* hitcmd_refresh */

/*===========================================================================

FUNCTION hitcmd_handler

DESCRIPTION
  This procedure may be used by the test application task to process/execute 
  a command that is in its command-queue.
  It uses the testapp_id argument and matches it with the one of the 
  elements in the handler_table, if a match is found it returns the hanlder 
  If a match is not found it returns the default handler which should be at
  the end of the table with testapp_id = 0xffff

DEPENDENCIES
  None

PARAMETERS
  testapp_id
  handler_table
  
RETURN VALUE
  handler 

SIDE EFFECTS
  None

===========================================================================*/
hitcmd_handler_type hitcmd_handler
(
  diagpkt_subsys_cmd_code_type testapp_id,
    /* testapp_id which handler belongs to */

  hitcmd_handler_info *handler_table
    /* pointer to the command table */
)
{
  while (handler_table->testapp_id < 0xffff && \
         testapp_id != handler_table->testapp_id)
  {
    handler_table++;
  }

  return handler_table->handler;
} /* hitcmd_handler() */

#endif /* FEATURE_HIT_DIAG_AND_CMD */
#endif /* FEATURE_HIT */


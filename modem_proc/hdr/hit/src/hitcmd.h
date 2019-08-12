#ifndef _HITCMD_H
#define _HITCMD_H

/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  HIT COMMAND SERVICE UTILITY HEADER FILE

GENERAL DESCRIPTION
 This file contains types and declarations associated with the HIT command 
 processing. 

EXTERNALIZED FUNCTIONS
    hitcmd_alloc
    hitcmd_param_ptr
    hitcmd_send_rsp_pkt
    hitcmd_send_status
    hitcmd_refresh
    
REGIONAL FUNCTIONS
    None
    
    Copyright (c) 2006, 2007 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/hit/src/hitcmd.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

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
#include "comdef.h"
#ifdef FEATURE_HIT_DIAG_AND_CMD
#include "diagpkt.h"
#include "msg.h"
#include "msgcfg.h"

/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

/* HIT ERROR ENUM */
enum hitcmd_status_enum
{
  HITCMD_ERROR_NONE          = 0x0000,
  HITCMD_ERROR_MEM_ALLOC     = 0xf001,
  HITCMD_ERROR_QUEUE         = 0xf002,
  HITCMD_ERROR_TESTAPP_ID    = 0xf003,
  HITCMD_ERROR_CMD_ID        = 0xf004,
  HITCMD_ERROR_PARAM         = 0xf005,
  HITCMD_REFRESHED           = 0xf006,
  HITCMD_REPLACED            = 0xf007,
};


/* All hit api functions return this type */
typedef uint16 hitcmd_ret_type;

/* HITCMD_PARAM_MAX_BYTES is defined as the size of the largest command 
   parameter in any HIT application.
   This may have to be updated if a new command api is added */
#define HITCMD_PARAM_MAX_BYTES 150

/* Structure to store all the info required for processing a command */
typedef PACKED struct
{
  diagpkt_cmd_code_type code;
    /* hit command code = 128 */

  diagpkt_subsys_id_type subsys_id;
    /* hit subsys id = 57 */

  diagpkt_subsys_cmd_code_type testapp_id;
    /* test app id to which command belongs to */

  uint8 cmd_id;
    /* cmd id within the test app */

  uint32 delay;
    /* cmd delay */

  uint8 param[HITCMD_PARAM_MAX_BYTES];
    /* pointer to the command param struct */

  uint16 param_len;
    /* Lenght of the param structure in bytes. 
       Can be used to confirm if the cmd param
       sent by the host is the same size as 
       defined in the test application.*/

  diagpkt_subsys_delayed_rsp_id_type rsp_id;
    /* response id for the command assigned by diag */

  diagpkt_subsys_rsp_cnt rsp_cnt;
    /* variable to keep track of how many commands have been sent */

} hitcmd_buf_type;

/* function type for testapp command handlers registered to task */
typedef hitcmd_ret_type (*hitcmd_handler_type)(hitcmd_buf_type*);

typedef struct
{
  diagpkt_subsys_cmd_code_type testapp_id;
    /* testapp_id which handler belongs to */
  hitcmd_handler_type handler;
    /* function pointer to handler */
} hitcmd_handler_info;

/* Macro to find the number of elements in an array */
#define SIZEOFARR( arr ) sizeof( arr ) / sizeof( arr[0] )

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
void hitcmd_init( void );

/*===========================================================================

FUNCTION hitcmd_alloc

DESCRIPTION
  Allocates memory for a command buffer

DEPENDENCIES
  None

RETURN VALUE
  hit_cmd_buf_type*  

SIDE EFFECTS
  None

===========================================================================*/
hitcmd_buf_type* hitcmd_alloc
(
  PACKED uint8 *packet,
    /* incomming diag cmd packet */

  uint16 packet_len,
    /* length of the incoming packet */

  diagpkt_subsys_delayed_rsp_id_type rsp_id
    /* response id for the command - assigned by diag */
);

/*===========================================================================

FUNCTION hitcmd_free

DESCRIPTION
  Deallocates the comamnd buffer

DEPENDENCIES
  None

RETURN VALUE
  void 

SIDE EFFECTS
  None

===========================================================================*/
void hitcmd_free
(
  hitcmd_buf_type *cmd_buf_ptr
    /* command buffer to be de-allocated */
);

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
);

/*=============================================================================

FUNCTION hitcmd_send_rsp_pkt

DESCRIPTION
  This procedure may be used by a command implementation to send diag responses
  to the host. A time stamp is NOT added.
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
  status 

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
    /* pkt length */

  boolean final_rsp
    /* indicates if this is the final reponse for the command */
);

/*===========================================================================

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

RETURN VALUE
  hit_ret_type 

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
);

/*===========================================================================

FUNCTION hitcmd_refresh

DESCRIPTION
  Refreshes the comamnd buffer

DEPENDENCIES
  None

PARAMETERS
  packet,
  packet_len,
  rsp_id
  
RETURN VALUE
  Void 

SIDE EFFECTS
  None

===========================================================================*/
boolean hitcmd_refresh
(
    hitcmd_buf_type *new_cmd_ptr
      /* incomming diag cmd packet */
);

/*===========================================================================

FUNCTION hitcmd_process

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
);
#endif /* FEATURE_HIT_DIAG_AND_CMD */
#endif /* _HITCMD_H */

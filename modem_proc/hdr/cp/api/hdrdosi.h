#ifndef HDRDOSI_H
#define HDRDOSI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       H D R   D A T A   O V E R   S I G N A L I N G  P R O T O C O L  
                                    
                   I N T E R N A L   H E A D E R   F I L E       


GENERAL DESCRIPTION
  This file contains the private definitions and interfaces for the HDR 
  Data Over Signaling protocol.

EXTERNALIZED FUNCTIONS
  hdrdos_process_msg
    Process messages received for the protocol.
    
  hdrdos_process_cmd
    Process command received for the protocol.

  hdrdos_process_timer
    Process timer expiration event for the protocol.
         
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.
  
  Copyright (c) 2004,2005 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================
                           
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrdosi.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/17/06   pba     Added support for DOS over EMPA.
05/09/05   ar/pba  Integrated Data Over Signaling protocol code
08/02/04   dna     Created module

===========================================================================*/

/* EJECT */
/*===========================================================================

                          INCLUDE FILES FOR MODULE
                               
===========================================================================*/
#include "comdef.h"
#include "dsm.h"
#include "hdrhai.h"

/* EJECT */
/*===========================================================================
                   
                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions and declarations for constants, macros,
types, variables and other items needed by this module.

===========================================================================*/
/* Command type */
/* 
   Note: The cmd enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/
 
typedef enum
{
  HDRDOS_SEND_DOS_MSG_CMD=0              /* Send MO DOS message */
} hdrdos_cmd_name_enum_type;

typedef struct                           /* HDRDOS_SEND_DOS_MSG_CMD */
{
  dsm_item_type *msg_ptr;
  uint8 link_flow_number;
  byte route;
  boolean ack_req;
} hdrdos_send_dos_msg_params_type;

typedef union                            /* Common command fields */ 
{                                        
  hdrdos_send_dos_msg_params_type        dos_msg;
} hdrdos_cmd_params_union_type;

typedef struct
{
  hdrdos_cmd_name_enum_type name;        /* Name of the command */
  hdrhai_protocol_name_enum_type sender; /* Protocol giving command, for 
                                          * debug */
  hdrdos_cmd_params_union_type params;   /* Common command fields */
} hdrdos_cmd_type;

/* <EJECT> */
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/


/* EJECT */
/*===========================================================================

FUNCTION HDRDOS_PROCESS_MSG

DESCRIPTION
  This function processes incoming messages for this protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  msg_ptr - pointer to receive message buffer.
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void hdrdos_process_msg 
( 
  dsm_item_type *msg_ptr,                  
  hdrhai_protocol_instance_enum_type protocol_instance
);

/* EJECT */
/*===========================================================================

FUNCTION HDRDOS_PROCESS_CMD

DESCRIPTION
  This function processes commands given to DOS protocol.
   
DEPENDENCIES
  None

PARAMETERS
  data        - user data
  status      - message transmission status

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void hdrdos_process_cmd
(
  hdrdos_cmd_type *dos_cmd_ptr
);

/*===========================================================================
FUNCTION HDRDOS_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for the Data Over 
  Signalling protocol.
  
DEPENDENCIES
  None

PARAMETERS
  timer_id - Timer ID of the expired timer

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrdos_process_timer
(
  uint32 timer_id
);

#endif /* HDRDOSI_H */


#ifndef _HITDIAG_H
#define _HITDIAG_H

/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  HIT   DIAG  HEADER    FILE

GENERAL DESCRIPTION
 This file contains types and declarations associated with the HIT diag 
 processing. 

EXTERNALIZED FUNCTIONS
    hitdiag_init
    
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
01/15/2013   smd     Featurized hit cmd and hit diag.
05/07/2007   vh      Changed dynamic memory allocation to static one
03/28/2007   vh      Created

==========================================================================*/

#ifdef FEATURE_HIT_DIAG_AND_CMD
/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

/* Testapp ids */
typedef enum
{
  HIT_DIAG                = 0x0000,
  HIT_HDR                 = 0x0001,
    /* Add new testapplication above this line */
  HIT_QUEUE_MODE_ENABLE   = 0xFFFF
} hitdiag_testapp_enum_type;

/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/

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
);
#endif /* FEATURE_HIT_DIAG_AND_CMD */
#endif /* _HITDIAG_H */

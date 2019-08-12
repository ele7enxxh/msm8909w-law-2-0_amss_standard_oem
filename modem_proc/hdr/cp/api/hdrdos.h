#ifndef HDRDOS_H
#define HDRDOS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       H D R   D A T A   O V E R   S I G N A L I N G  P R O T O C O L  
                                    
                   E X T E R N A L   H E A D E R   F I L E       


GENERAL DESCRIPTION
  This file contains the public definitions and interfaces for the HDR 
  Data Over Signaling protocol.

EXTERNALIZED FUNCTIONS
  hdrdos_send_dos_msg
    Send a Data Over Signaling Message
  hdrdos_msg_cb
    Callback function that queues incoming messages for this protocol.
  hdrdos_init
    Initializes the HDRDOS protocol.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.
  
  Copyright (c) 2004,2005 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================
                           
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrdos.h#1 $
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/26/11   smd     SU API cleanup 
12/01/09   suren   Added QChat optimization changes.
11/17/06   pba     Added support for DOS over EMPA.
06/05/06   pba     Added support to store session data in EFS.
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
#include "hdrind.h"

/* EJECT */
/*===========================================================================
                   
                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions and declarations for constants, macros,
types, variables and other items needed by this module.

===========================================================================*/

/* Message ID values */

#define HDRDOS_DOSMSG_ROUTE_MASK  0x0100
#define HDRDOS_DOSMSG_MSGSEQ_MASK 0x00FF

/* List of DOS protocol events that would cause the DOS ACK timer to be started
 * or stopped */
typedef enum
{
  HDRDOS_RECEIVED_NEW_DOSMSG,
  HDRDOS_RECEIVED_DOS_ACK,
  HDRDOS_DOS_TIMER_EXPIRED
}hdrdos_dos_event_type;

/* This enum lists the route instances that are supported
   by the DOS protocol */
typedef enum
{
  HDRDOS_ROUTE_A,
  HDRDOS_ROUTE_B
} hdrdos_route_enum_type;

typedef struct 
{
  /* Message sequence of the most recent DOS message V(S) */
  uint8 tx_msg_seq;
  /* Message sequence of last received DOS message V(R) */
  uint8 rx_msg_seq;

}hdrdos_msg_seqnum_type;

typedef struct
{
  hdrdos_msg_seqnum_type route_a;
    /* Receive and Transmit Message sequence numbers for Route A */

  hdrdos_msg_seqnum_type route_b;
    /* Receive and Transmit Message sequence numbers for Route B */

}hdrdos_config_struct_type;

/*===========================================================================

FUNCTION HDRDOS_SEND_DOS_MSG

DESCRIPTION
  This function runs in the context of the HDRMC task.  It builds a Data
  Over Signaling message and queues it to HMP for transmit.

DEPENDENCIES
  None

PARAMETERS
  msg_ptr   - A pointer to the DSM item containing the HigherLayerPacket
              to be sent as part of the DataOverSignaling message.
  link_flow_number  - The RLP Flow with which this DataOverSignaling message is 
              associated.
  ack_req   - TRUE if an ACK is required, FALSE otherwise           

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hdrdos_send_dos_msg
( 
  dsm_item_type * msg_ptr,
  uint8 link_flow_number,
  hdrdos_route_enum_type route,
  boolean ack_req
);

/*===========================================================================

FUNCTION HDRDOS_MSG_CB

DESCRIPTION
  This callback function queues incoming messages for this protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  msg_ptr - pointer to receive message buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void hdrdos_msg_cb 
( 
  dsm_item_type *msg_ptr
);

/*===========================================================================

FUNCTION hdrdos_powerup_init

DESCRIPTION
  This function initializes the HDRDOS protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void hdrdos_powerup_init 
( 
  void
);

/*===========================================================================

FUNCTION HDRDOS_GET_INCONFIG_HANDLE

DESCRIPTION
  This function returns a pointer to inconfig instance of DOS

DEPENDENCIES
  None

PARAMETERS
  None
                     
RETURN VALUE
  Pointer to inconfig instance of RUP

SIDE EFFECTS
  None

===========================================================================*/
extern void* hdrdos_get_inconfig_handle(void);

/*===========================================================================

FUNCTION HDRDOS_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  This function initializes Data Over Signaling Protocol's config parameters
  to default. 

DEPENDENCIES
  None

PARAMETERS
  current_config - Pointer to Data Over Signaling Protocol's current config
                   structure in SCMDB
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrdos_set_config_to_default
( 
  hdrdos_config_struct_type *current_config 
);

/*===========================================================================

FUNCTION HDRDOS_PROCESS_IND

DESCRIPTION
  This function processes indications given to DOS

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for DOS to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrdos_process_ind 
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
);

#endif /* HDRDOS_H */


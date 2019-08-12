#ifndef HDRCSNA_H
#define HDRCSNA_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  H D R   C I R C U I T   S E R V I C E S   

              N O T I F I C A T I O N   A P P L I C A T I O N
                                  
                   E X T E R N A L   H E A D E R   F I L E       


GENERAL DESCRIPTION
  This file contains the public definitions and interfaces for the HDR 
  Circuit Services Notification protocol.

EXTERNALIZED FUNCTIONS
  hdrcsna_process_msg
    Process messages received for the protocol.
    
  hdrcsna_process_cmd
    Process command received for the protocol.

  hdrcsna_process_timer
    Process timer expiration event for the protocol.    
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.
  
  Copyright (c) 2006 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================
                           
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrcsna.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/18/09   wsh     Updated file to include private header hdrmc_v.h
04/06/07   yll     Start quick repeat timer after message transmission
                   outcome callback.
01/17/07   yll     Changes for HOMER interface.
01/08/07   yll     Created module

===========================================================================*/

/* EJECT */
/*===========================================================================

                          INCLUDE FILES FOR MODULE
                               
===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "dsm.h"
#include "hdrhai.h"

#ifdef FEATURE_CMI
#include "hdrmc_v.h"
#endif

#include "hdrerrno.h"

#ifdef FEATURE_HDR_CSNA
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
  HDRCSNA_SEND_3G1X_SERVICES_MSG_CMD = 0, /* Send 3G1X Services message */
  HDRCSNA_UPDATE_CSNA_CONFIG_CMD     = 1  /* Update CSNA config info */
} hdrcsna_cmd_name_enum_type;

typedef struct                            /* HDRCSNA_SEND_3G1X_MSG_CMD */
{
  uint8     chan;
  uint8     p_rev;
  uint16    payload_len;
  uint8     *payload;
} hdrcsna_send_3g1x_msg_params_type;

typedef union                             /* Common command fields */
{
  hdrcsna_send_3g1x_msg_params_type msg_3g1x;
  hdrmc_csna_msg_config_type        config_attrib;

} hdrcsna_cmd_params_union_type;

typedef struct
{
  hdrcsna_cmd_name_enum_type      name;   /* Name of the command */
  hdrhai_protocol_name_enum_type  sender; /* Protocol giving command, for
                                           * debug */
  hdrcsna_cmd_params_union_type   params; /* Common command fields */
} hdrcsna_cmd_type;
#endif /* FEATURE_HDR_CSNA */


/* CSNA 3G1XServices message TX status indication type */
typedef struct
{
  uint8               tx_msg_seq;
  hdrerrno_enum_type  status;
} hdrcsna_tx_msg_status_ind_type;

/* 1X Search parameters retrieved from 3G1XParameters message */
typedef struct
{
  boolean   t_add_included;
  uint8     t_add;
  boolean   pilot_inc_included;
  uint8     pilot_inc;
} hdrcsna_1x_search_params_type;

#ifdef FEATURE_HDR_CSNA
/*===========================================================================

FUNCTION HDRCSNA_MSG_CB

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
extern void hdrcsna_msg_cb 
( 
  dsm_item_type *msg_ptr
);


/*===========================================================================

FUNCTION HDRCSNA_POWERUP_INIT

DESCRIPTION
  This function initializes the HDRCSNA protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void hdrcsna_powerup_init ( void );


/* EJECT */
/*===========================================================================

FUNCTION HDRCSNA_PROCESS_MSG

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
extern void hdrcsna_process_msg 
( 
  dsm_item_type *msg_ptr,                  
  hdrhai_protocol_instance_enum_type protocol_instance
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRCSNA_PROCESS_CMD

DESCRIPTION
  This function processes commands given to CSNA.

DEPENDENCIES
  None

PARAMETERS
  csna_cmd_ptr - Pointer to the command for CSNA to process  

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void hdrcsna_process_cmd
(
  hdrcsna_cmd_type *cmd_ptr
);


/*===========================================================================
FUNCTION HDRCSNA_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for CSNA.
  
DEPENDENCIES
  None

PARAMETERS
  timer_id - Timer ID of the expired timer

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrcsna_process_timer
(
  uint32 timer_id
);


/* EJECT */
/*===========================================================================
FUNCTION HDRCSNA_TUNNEL_MSG

DESCRIPTION
  This function commands CSNA protocol to tunnel message on DO channel

DEPENDENCIES
  None

PARAMETERS
  sender        - Protocol name who send this command
  dest_protocol - Protocol name who should receive this command
  chan          - 3G1X Logical Channel
  p_rev         - 1X Protocol Revision
  payload_len   - PDU length
  payload       - PDU data
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void hdrcsna_tunnel_msg
( 
  hdrhai_protocol_name_enum_type  sender,
  sys_sys_id_type_e_type          dest_protocol,
  uint8                           chan,
  uint8                           p_rev,
  uint16                          payload_len,
  uint8                           *payload
);

/* EJECT */
/*===========================================================================
FUNCTION HDRCSNA_UPDATE_MSG_CONFIG

DESCRIPTION
  This function commands CSNA protocol to update the CSNA attribute 
  configurations.

DEPENDENCIES
  None

PARAMETERS
  sender        - Protocol name who send this command
    
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void hdrcsna_update_msg_config
( 
  hdrhai_protocol_name_enum_type  sender,
  hdrmc_csna_msg_config_type      *csna_config_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRCSNA_CONFIGURE

DESCRIPTION
  This function conditionally commands HDRCSNA to configure 

DEPENDENCIES
  hdrcsna_configure shall only be called after the AT Initiated stream is 
  complete, or there is already a valid stream configuration.
  HDRMC task context

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  TRUE - the configure command is accepted
  FALSE - the configure command is rejected

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrcsna_configure
(
  hdrhai_protocol_name_enum_type sender 
);
#endif /* FEATURE_HDR_CSNA */


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCSNA_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRCSNA.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Indication to process, is a global enum for all
                    possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrcsna_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void * ind_data_ptr
);


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCSNA_GET_1X_SRCH_PARAMS

DESCRIPTION
  This function retrieves the 1X Search parameters from 3G1XParameters 
  message.

DEPENDENCIES
  The 1x Search parameters are retrieved only when 3G1XParameters 
  message is valid

PARAMETERS
  params - pointer to the 1x Search parameter variable whose values to
           be filled in.
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
extern void hdrcsna_get_1x_srch_params
(
  hdrcsna_1x_search_params_type   *params
);

#endif /* HDRCSNA_H */


#ifndef HDRFMACI_H
#define HDRFMACI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         H D R   F O R W A R D   T R A F F I C   C H A N N E L   M A C   
       
                               P R O T O C O L  
                                    
                   I N T E R N A L   H E A D E R   F I L E       


GENERAL DESCRIPTION
  This file contains the private definitions and interfaces for the HDR 
  Forward Traffic Channel Media Access Control (MAC) protocol.

EXTERNALIZED FUNCTIONS
  hdrfmac_process_cmd
    Process commands received for the protocol.
    
  hdrfmac_process_sig
    Process signals received for the protocol.
   
  hdrfmac_process_msg
    Process messages received for the protocol.
    
  hdrfmac_msg_cb
    Callback function that queues received messages for the protocol.
         
  hdrfmacsm_change_state
    Change the protocol state.
    
  hdrfmacsm_get_state
    Get the current protocol state.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol.  The procotol must be activated before Control Channel 
  packets can be processed.

  Copyright (c) 2000, 2001, 2002 by Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2003, 2004, 2005 by Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2006, 2007, 2008 by Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2009, 2010, 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================
                           
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/hdrfmaci.h_v   1.12   05 Jun 2002 09:57:48   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/mac/api/hdrfmaci.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/25/15   ljl     Fixed compiler warning.  
12/17/14   vke     Added API to handle the flow control command from DS
08/23/12   smd     Added updates for Device only DTX and FlexCONN.
06/20/12   smd     Added Device only DTX and FlexCONN.
03/01/12   rmg     Made changes to process set_fl_active_cb() in HDRRX task.
03/22/11   arm     Added support for thermal pa backoff feature. 
06/21/10   cnx     Fixed compiler warnings.
05/22/09   etv     Added support for Q6 message interface
09/10/07   etv     Removed hdrfmaci_tcam_type & moved MC TCAM type to hdrfmac.h
08/30/07   etv     Added Multi-carrier TCAM params type.
05/05/06   etv     Removed unused enum type definition
05/04/06   etv     Trigger AGC Tx attrib updates and wait for flag reset.
09/06/05   etv     Created Library for enhanced subtype.
05/13/05   etv     Added support for Rev-A.
06/03/02   mpa     Mainlined FEATURE_HDR_7_4_DDR.
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
08/24/01   kss     Moved config declarations to hdrfmacc.h
06/01/01   kss     Fixed FTCMAC packet bits.
11/03/00   dna     Added HDR event logging
03/30/00   st      Created.

===========================================================================*/

/* EJECT */
/*===========================================================================

                          INCLUDE FILES FOR MODULE
                               
===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "dsm.h"
#include "rex.h"

#include "hdrbit.h"
#include "hdrmac.h"
#include "hdrfmac.h"
#ifdef FEATURE_HDR_THERMAL_BACKOFF
#include "ds707_ccm.h"
#endif /* FEATURE_HDR_THERMAL_BACKOFF */

/* EJECT */
/*===========================================================================
                   
                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions and declarations for constants, macros,
types, variables and other items needed by this module.

===========================================================================*/

/* The number of consecutive non-null slots needed for restart */
#define HDRFMAC_DRC_SLOTS_NEEDED_FOR_RESTART  16

/* The DRC supervision timer, in slots (240 ms) */
#define HDRFMAC_DRC_SUPERVISION_SLOTS         144 

#define HDRFMAC_DRC_LOCK_BACKOFF_20_DB_V   2560 /* Default value, Rel 0. */
#define HDRFMAC_DRC_LOCK_BACKOFF_6_DB_V     768 /* Used for Rev A. */


/*---------------------------------------------------------------------------
                              PROTOCOL COMMANDS
                              
 The following codes are for use with the 'cmd' field of the protocol command
 header type defined below, to define the operation requested by the command.                              
---------------------------------------------------------------------------*/
/*
   Note: The cmd enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/
typedef enum
{
  HDRFMAC_DEACTIVATE_CMD=0,
  HDRFMAC_ACTIVATE_CMD=1,
  HDRFMAC_SUPERVISION_CMD=2
#ifdef FEATURE_HDR_THERMAL_BACKOFF
  ,HDRFMAC_THERMAL_DIV_CMD=3
#endif /* FEATURE_HDR_THERMAL_BACKOFF */

#if defined(FEATURE_HDR_FL_DATA_ADAPTIVE_RXD) || \
    defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)
  ,HDRFMAC_SET_FL_ACTIVE_CB_CMD=4
#endif /* FEATURE_HDR_FL_DATA_ADAPTIVE_RXD || FEATURE_HDR_FLEX_CONN||
          FEATURE_HDR_DEVICE_ONLY_DTX */

#ifdef FEATURE_HDR_HANDLE_FC_IN_HDRTX
  ,HDRFMAC_FLOW_CONTROL_CMD=5
#endif /* FEATURE_HDR_HANDLE_FC_IN_HDRTX */
} 
hdrfmac_cmd_enum_type;


/*--------------------------------------------------------------------------
                           COMMAND HEADER TYPE

 This header type is contained in every protocol command. It contains info
 common to all commands.
--------------------------------------------------------------------------*/

typedef struct                      
{                                      
  hdrfmac_cmd_enum_type  cmd;       /* Specifies which command to perform */
  hdrhai_protocol_name_enum_type sender; /* Who sent the command */
}
hdrfmac_cmd_hdr_type;


/*--------------------------------------------------------------------------
                             ACTIVATE COMMAND

 This command is given to activate the protocol.
--------------------------------------------------------------------------*/

typedef struct                         
{                                      
  hdrfmac_cmd_hdr_type   hdr;                 /* Common header */    
}
hdrfmac_act_cmd_type;


/*--------------------------------------------------------------------------
                           DEACTIVATE COMMAND

 This command is given to activate the protocol.
--------------------------------------------------------------------------*/

typedef struct                         
{                                      
  hdrfmac_cmd_hdr_type   hdr;                 /* Common header */    
}
hdrfmac_deact_cmd_type;


/*--------------------------------------------------------------------------
                        LINK STATUS UPDATE COMMAND

 This command is given to cause the protocol to perform supervision given
 updated information from the network.
--------------------------------------------------------------------------*/

typedef struct                         
{                                      
  hdrfmac_cmd_hdr_type   hdr;                 
    /* Common header */     

  qword                  chan_valid_bits_63_to_0;   
    /* QuickConfig.ChannelValid bits 63 to 0 */

  qword                  chan_valid_bits_127_to_64; 
    /* QuickConfig.ChannelValid bits 127 to 64 */

  uint8                  link_id;          
    /* Link ID */
}
hdrfmac_sup_cmd_type;

#ifdef FEATURE_HDR_THERMAL_BACKOFF
typedef struct                         
{                                      
  hdrfmac_cmd_hdr_type   hdr;                 
    /* Common header */     

  ds707_ccm_flow_control_cmd_enum_type thermal_div_cmd;
   /* Thermal flow control command received */
}
hdrfmac_thermal_div_cmd_type;
#endif /*FEATURE_HDR_THERMAL_BACKOFF */

/*--------------------------------------------------------------------------
                           SET DRC LENGTH COMMAND

 This command is given to update the length of the DRC transmission on the
 reverse link to a value received from the network.
--------------------------------------------------------------------------*/

typedef struct                         
{                                      
  hdrfmac_cmd_hdr_type   hdr;       /* Common header */
  uint8                  drc_len;   /* TrafficChannelAssignment.DRCLength */
}
hdrfmac_drc_cmd_type;


/*--------------------------------------------------------------------------
                           SET RAB LENGTH COMMAND

 This command is given to update the length of the RAB transmission on the
 forward link to a value received from the network.
--------------------------------------------------------------------------*/

typedef struct                         
{                                      
  hdrfmac_cmd_hdr_type   hdr;       /* Common header */     
  uint8                  rab_len;   /* TrafficChannelAssignment.RABLength */     
}
hdrfmac_rab_cmd_type;


#if defined(FEATURE_HDR_FL_DATA_ADAPTIVE_RXD) || \
    defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)
/*--------------------------------------------------------------------------
                           SET FL ACTIVE CB COMMAND

 This command is given to set FL active callback.
--------------------------------------------------------------------------*/

typedef struct                         
{                                      
  hdrfmac_cmd_hdr_type        hdr;  /* Common header */    
  hdrfmac_fl_active_cb_type   cb;   /* FL active callback pointer */
  hdrfmac_fl_active_cb_client_type client_id; /* client id */
  uint32                      payload; /* payload of this command */
}
hdrfmac_set_active_cb_cmd_type;
#endif /* FEATURE_HDR_FL_DATA_ADAPTIVE_RXD ||FEATURE_HDR_FLEX_CONN||
          FEATURE_HDR_DEVICE_ONLY_DTX*/

#ifdef FEATURE_HDR_HANDLE_FC_IN_HDRTX
/*--------------------------------------------------------------------------
                           FLOW CONTROL COMMAND

 This command is given to handle the flow control from DS
--------------------------------------------------------------------------*/

typedef struct                         
{                                      
  hdrfmac_cmd_hdr_type        hdr;  /* Common header */    
  ds707_ccm_flow_control_cmd_enum_type cmd; /* Flow control cmd from DS */ 
 }
hdrfmac_flow_control_cmd_type;
#endif /* FEATURE_HDR_HANDLE_FC_IN_HDRTX */

/*--------------------------------------------------------------------------
                        UNION OF PROTOCOL COMMANDS
                      
 This union type includes all command types, as well as the common header
 type, to define a type that can contain any of the command types.
--------------------------------------------------------------------------*/

typedef union                          
{                                      
  hdrfmac_cmd_hdr_type   hdr;
  hdrfmac_act_cmd_type   act;
  hdrfmac_deact_cmd_type deact;
  hdrfmac_sup_cmd_type   supv;
#ifdef FEATURE_HDR_THERMAL_BACKOFF
  hdrfmac_thermal_div_cmd_type thermal_div; 
#endif /* FEATURE_HDR_THERMAL_BACKOFF */

#if defined(FEATURE_HDR_FL_DATA_ADAPTIVE_RXD) || \
    defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)
  hdrfmac_set_active_cb_cmd_type set_active_cb;
#endif /* FEATURE_HDR_FL_DATA_ADAPTIVE_RXD ||FEATURE_HDR_FLEX_CONN||FEATURE_HDR_DEVICE_ONLY_DTX*/

#ifdef FEATURE_HDR_HANDLE_FC_IN_HDRTX
  hdrfmac_flow_control_cmd_type flow_contol;
#endif /* FEATURE_HDR_HANDLE_FC_IN_HDRTX */

} 
hdrfmac_cmd_union_type;


/*---------------------------------------------------------------------------
                              PROTOCOL STATES
                              
 The protocol can operate in the following states.
---------------------------------------------------------------------------*/

/* Protocol states. */

/*
   Note: The state enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/

typedef enum
{
  HDRFMAC_INACTIVE_STATE=0,
  HDRFMAC_VAR_RATE_STATE=1,
  HDRFMAC_FIX_RATE_STATE=2
} 
hdrfmac_state_enum_type;

/* Number of protocol states. */

#define HDRFMAC_NUM_STATES   3


/*---------------------------------------------------------------------------
                             PROTOCOL DATA UNIT

 The Forward Traffic Channel packet is 1003 bits in length.  The packet 
 contains a single Security Layer packet that can be up to 1000 bits in 
 length.  The last three bits correspond to the MAC Layer packet Format 
 (FMT) bits, which indicate the authentication and encryption status of the
 Security Layer packet, and whether the size is included at the beginning of
 each Connection Layer packet within the Security Layer packet.
---------------------------------------------------------------------------*/

/* ------------ Forward Traffic Channel packet field lengths ------------- */

/* Packet tail length. */

#define HDRFMAC_PKT_TAIL_LEN   3


/* ----- Forward Traffic Channel packet "header" (footer) macros --------- */

/* Connection Layer packet size included bit. A non-zero value indicates
   that the Connection Layer packet includes the packet length. */

#define HDRFMAC_PKT_IS_SIZE( ptr )\
          ( ( ( *( (uint8*)(ptr) ) & 0x80 ) != 0 ) ? TRUE : FALSE )

/* Mac Valid packet bit. If this bit is not set, the packet is dropped. */
#define HDRFMAC_PKT_IS_MAC_VALID( ptr )\
          ( ( ( *( (uint8*)(ptr) ) & 0x40 ) != 0 ) ? TRUE : FALSE )


/*---------------------------------------------------------------------------
                            PROTOCOL MESSAGE HEADER                      
---------------------------------------------------------------------------*/

/* Message ID field type. */

typedef uint8 hdrfmac_msg_id_type;

/* Transaction ID field type. */

typedef uint8 hdrfmac_msg_trans_id_type;

/* Get message ID field. */

#define HDRFMAC_GET_MSG_ID( dsm_ptr, val )\
          HDRBIT_UNPACK8( dsm_ptr, &val, 0, 8 )

/* Get transaction ID field. */

#define HDRFMAC_GET_TRANS_ID( dsm_ptr, val )\
          HDRBIT_UNPACK8( dsm_ptr, &val, 8, 8 )

/*---------------------------------------------------------------------------
                          FIXED MODE REQUEST MESSAGE                      
---------------------------------------------------------------------------*/

/* Message ID value. */

#define HDRFMAC_FIX_MODE_REQ_MSG_ID  0x00

/* Message priority. */

#define HDRFMAC_FIX_MODE_REQ_MSG_PRI   40

/* Message size. */

#define HDRFMAC_FIX_MODE_REQ_MSG_LEN    4


/*---------------------------------------------------------------------------
                         FIXED MODE RESPONSE MESSAGE                      
---------------------------------------------------------------------------*/

/* Message ID value. */

#define HDRFMAC_FIX_MODE_RSP_MSG_ID  0x01

/* Message priority. */

#define HDRFMAC_FIX_MODE_RSP_MSG_PRI   40


/*---------------------------------------------------------------------------
                        CONFIGURATION REQUEST MESSAGE                      
---------------------------------------------------------------------------*/

/* Message ID value. */

#define HDRFMAC_CFG_REQ_MSG_ID       0x50

/* Message priority. */

#define HDRFMAC_CFG_REQ_MSG_PRI        40

/*---------------------------------------------------------------------------
                       CONFIGURATION RESPONSE MESSAGE                      
---------------------------------------------------------------------------*/

/* Message ID value. */

#define HDRFMAC_CFG_RSP_MSG_ID       0x51

/* Message priority. */

#define HDRFMAC_CFG_RSP_MSG_PRI        40

/* Message size. */

#define HDRFMAC_CFG_RSP_MSG_LEN         4


/* EJECT */
/*===========================================================================

FUNCTION HDRFMAC_PROCESS_CMD

DESCRIPTION
  This function processes the commands for this protocol that were queued
  on the task command queue from a different task context.
  
DEPENDENCIES
  None.

PARAMETERS
  cmd_ptr - pointer to received command buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void hdrfmac_process_cmd 
( 
  hdrfmac_cmd_union_type *cmd_ptr
);


/* EJECT */
/*===========================================================================

FUNCTION HDRFMAC_PROCESS_SIG

DESCRIPTION
  This function handles the task signals assigned to this protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  signal - task signal to be processed.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void hdrfmac_process_sig 
( 
  rex_sigs_type signal
);


/* EJECT */
/*===========================================================================

FUNCTION HDRFMAC_PROCESS_MSG

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
extern void hdrfmac_process_msg 
( 
  dsm_item_type *msg_ptr,                  
  hdrhai_protocol_instance_enum_type protocol_instance
);

/* EJECT */
/*===========================================================================

FUNCTION HDRFMACSM_CHANGE_STATE

DESCRIPTION
  This function manages the protocol state transitions.  For valid state
  transitions, the current state is exited by performing any required exit
  processing, then the new state is entered by perfroming any required entry
  processing.
  
DEPENDENCIES
  None.

PARAMETERS
  new_state - new state to enter.
  
RETURN VALUE
  None.

SIDE EFFECTS
  Protocol state is changed, unless the state transition is not permitted.
  
===========================================================================*/
extern void hdrfmacsm_change_state 
( 
  hdrfmac_state_enum_type new_state                  
);


/* EJECT */
/*===========================================================================

FUNCTION HDRFMACSM_GET_STATE

DESCRIPTION
  This function returns the current state of the protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  Current protocol state.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern hdrfmac_state_enum_type hdrfmacsm_get_state ( void );


/*===========================================================================

FUNCTION HDRFMAC_CONTROL_FLOW                              EXTERNAL

DESCRIPTION
  This interface processes flow control command given by DS.
  Runs in DS task context.
  
DEPENDENCIES
  None.

PARAMETERS
  cmd - Memory Flow control command

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/

void hdrfmac_control_flow (uint32 cmd);

#endif /* HDRFMACI_H */


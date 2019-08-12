#ifndef HDRBCFRAME_H
#define HDRBCFRAME_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                H D R   B R O A D C A S T   F R A M I N G
       
                             P R O T O C O L
       
                  E X T E R N A L   H E A D E R   F I L E       

GENERAL DESCRIPTION
  This file contains the external interface of the HDR Broadcast Framing
  Protocol.
  
EXTERNALIZED FUNCTIONS
  hdrbcframe_init
    Initialize the protocol.
    
  hdrbcframe_activate
    Activate the protocol.

  hdrbcframe_deactivate
    Deactivate the protocol.
    
  hdrbcframe_is_active
    Determine if the protocol is active.
    
  hdrbcframe_put_packet
    Process a framing packet.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol. 

  Copyright (c) 2002 by Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2003 by Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2004 by Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2005 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/inc/hdrbcframe.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/01/09   suren   Added QChat optimization changes.
09/30/05   mpa     Added command processing mechanism.
                   Moved flow enable / disable cmds from hdrbctask.
02/02/05   etv     Fixed coding style to match convention.
08/16/04   kss     Changed API to take callback directly.
07/19/04   kss     Use dsm_item pointer in hdrbcframe_put_pkt().
04/24/04   kss     Ported from QC Corp.

===========================================================================*/

/* EJECT */
/*===========================================================================

                         INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "hdrlogi.h"
#include "dsm.h"
#include "hdrbcovhd.h"

#define HDRBCFRAME_MIN_PROT_ID_LEN             1

/* Returns TRUE if the given byte is odd and FALSE otherwise */
#define HDRBCFRAME_IS_BYTE_ODD( byte )    (((byte) & 0x01U)? TRUE: FALSE)

typedef struct 
{
  uint32          crc_value;
  uint32          crc_seed;
  dsm_item_type * unframed_pkt;
  uint16          min_length;
}hdrbcframe_segment_deframer;

/* HDLC Deframer Status */
typedef struct 
{
  void* handle;
}hdrbcframe_hdlc_deframer;

/* Status is maintained for each broadcast channel open */
typedef struct
{
  uint32 flow_id;
    /* Flow id for reference */

  void (*data_cb)(dsm_item_type *ip_pkt);

  int ref_count;
    /* 0 is inactive, >0 active for a particular channel */

  boolean                fcs_included;
  hdrbcovhd_framing_type deframer_type;

  union 
  {
    hdrbcframe_segment_deframer seg_def;
    hdrbcframe_hdlc_deframer    hdlc_def;
  }deframer;
    /* Deframer info/status */

  hdrlogbc_framing_metrics_type chan_metrics;
    /* Storage for metrics for logging, etc. */

} hdrbcframe_type;

typedef enum
{
  HDRBCFRAME_FLOW_ENABLE_CMD  = 0,
  HDRBCFRAME_FLOW_DISABLE_CMD = 1
} hdrbcframe_cmd_name_enum_type;

typedef struct
{      
  uint32 flow_id;
  void (*data_cb)(dsm_item_type * data_pkt);
} hdrbcframe_enable_flow_params_type;

typedef struct
{      
  uint32 flow_id;
} hdrbcframe_disable_flow_params_type;


typedef union
{                       
  hdrbcframe_enable_flow_params_type  enable_flow;  
  hdrbcframe_disable_flow_params_type disable_flow;  
} hdrbcframe_cmd_params_union_type;

typedef struct
{
  hdrbcframe_cmd_name_enum_type name;         /* Name of the command */
  hdrbcframe_cmd_params_union_type params;    /* command parameters  */
  hdrhai_protocol_name_enum_type sender;      /* Protocol giving cmd */
} hdrbcframe_cmd_union_type;

/*===========================================================================

FUNCTION HDRBCFRAME_INIT

DESCRIPTION
  This function initializes the protocol prior to use.  
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcframe_init( void );


/*===========================================================================

FUNCTION HDRBCFRAME_PUT_PKT

DESCRIPTION
  The function processes Broadcast Framing layer packets.
  
DEPENDENCIES
  None.

PARAMETERS
  data     - A pointer to the packet data
  length   - length of the data
  chan_idx - the channel the data arrived on.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcframe_put_pkt
( 
  dsm_item_type* data, 
  uint16 length,
  int chan_idx 
);

/*===========================================================================

FUNCTION HDRBCFRAME_PROCESS_CMD

DESCRIPTION
  This function processes the commands for this protocol that were queued
  on the task command queue from a different task context.

DEPENDENCIES
  None.

PARAMETERS
  cmd_ptr - address of command structure.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcframe_process_cmd
(
  hdrbcframe_cmd_union_type *cmd_ptr
);

/*===========================================================================

FUNCTION HDRBCFRAME_GET_CHANNEL_METRICS

DESCRIPTION
  This function copies security protocol metrics into a provided structure
  (for logging, etc.)
  
DEPENDENCIES
  None.

PARAMETERS
  chan_idx - The channel to get metrics from.
  chan_metrics - A structure to copy the data into.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcframe_get_channel_metrics
(
  uint8 chan_idx,
  hdrlogbc_framing_metrics_type *chan_metrics
);


/*===========================================================================

FUNCTION HDRBCFRAME_CLEAR_CHANNEL_METRICS

DESCRIPTION
  This function clears the channel metrics for the specified channel. The
  channel should be active.
  
DEPENDENCIES
  None.

PARAMETERS
  chan_idx - The channel metrics to clear.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcframe_clear_channel_metrics
(
  uint8 chan_idx 
);

/*===========================================================================

FUNCTION HDRBCFRAME_FLOW_ENABLE

DESCRIPTION
  This command enables a broadcast flow. The command will be queued and
  handled by the broadcast task context.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The Broadcast flow ID to enable
  data_cb - A callback function for deframed broadcast data. 
  sender  - Protocol sending the command
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcframe_flow_enable
(
  uint32 flow_id,
  void (*data_cb)(dsm_item_type *ip_pkt),
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRBCFRAME_FLOW_DISABLE

DESCRIPTION
  Disables a broadcast flow.  The command will be queued and
  handled by the broadcast task context.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The flow ID to disable.
  sender  - Protocol sending the command
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrbcframe_flow_disable
(
  uint32 flow_id,
  hdrhai_protocol_name_enum_type sender
);

#endif /* HDRBCFRAME_H */



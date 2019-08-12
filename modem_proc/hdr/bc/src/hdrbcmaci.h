#ifndef HDRBCMACI_H
#define HDRBCMACI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     H D R   B R O A D C A S T   M A C
       
                             P R O T O C O L
       
                  I N T E R N A L   H E A D E R   F I L E       

GENERAL DESCRIPTION
  This file contains internal interface of the HDR Broadcast MAC
  Protocol.
  
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol. 

  Copyright (c) 2004 - 2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbcmaci.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/15/09   kss     Use new PACK() macro for Q6 compatibility.
08/06/09   wsh     Support CMI API changes in sys.h 
01/30/09   etv     Merged the following changes to main branch
                   Added support for 8/16 Multiplexes
09/30/05   mpa     Removed hdrbcmac_set_overhead_message().
09/01/05   kss     Removed hdrbcmac_process_ind().
08/28/05   dna     (mpa for) Use bcovhd interface to retrieve ovhd info.
08/19/04   kss     More cleanup.
08/16/04   kss     Added support for tracking flow status; some cleanup.
07/08/04   kss     Created.

===========================================================================*/

/* EJECT */
/*===========================================================================

                         INCLUDE FILES FOR MODULE
                     
===========================================================================*/

#include "hdr_variation.h"
#include "hdrbcovhd.h"
#include "hdrlogi.h"    
#include "hdrind.h"

        
/*===========================================================================
                   
                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions and declarations for constants, macros,
types, variables and other items needed by this module.

===========================================================================*/

/* Max supported simultaneous logical channels */                                           
#define HDRBCMAC_INTERLACE_COUNT  HDRBCOVHD_MAX_INTERLACES
#define HDRBCMAC_MULTIPLEX_COUNT  HDRBCOVHD_MAX_SUPPORTED_MULTIPLEXES

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#else
#define HDRBCMACI_BCMCS_MAX_FLOWS SYS_BCMCS_10_MAX_FLOWS
#endif /* FEATURE_HDR_BCMCS_2p0 */


/* Structure for CRC pass/fail metrics on a per-interlace/multiplex
   pair basis */
typedef PACK(struct)
{
  uint32 crc_pass;
  uint32 crc_fail;
} hdrbcmac_multiplex_metrics;

/* Structure for overall pkt metrics, maintained by BC MAC */
typedef  struct
{
  hdrbcmac_multiplex_metrics 
      multiplex[ HDRBCMAC_INTERLACE_COUNT ][ HDRBCMAC_MULTIPLEX_COUNT ];

  uint32 ecb_out_of_sync;
  uint32 decoder_ref_count_error;
} hdrbcmac_static_metrics;

typedef struct
{
  rex_crit_sect_type crit_sect; /* Critical Section */
  boolean is_valid;
  hdrbcovhd_bc_msg_head_type bc_msg_head;
  uint8  num_flows;
  uint32 bcmcs_flow_id[HDRBCMACI_BCMCS_MAX_FLOWS];
  hdrbcovhd_logical_channel_info_type log_chan_info[HDRBCMACI_BCMCS_MAX_FLOWS];
  hdrbcovhd_soft_combine_info_type    soft_combine_info[HDRBCMACI_BCMCS_MAX_FLOWS];
} hdrbcmac_ovhd_info_type;
                            

/*=============================================================================
                           FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================

FUNCTION HDRBCMAC_CHANNEL_ENABLE

DESCRIPTION
  The function enables a new flow in the HDR Broadcast MAC Protocol.
  A flow_id is taken as a parameter to open a new channel.  The return value
  is a handle (or index) for the assigned logical channel.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The desired flow.
  
RETURN VALUE
  channel index to use to refer to this channel.
  -1 if flow cannot be enabled.

SIDE EFFECTS
  None.

===========================================================================*/
extern int hdrbcmac_channel_enable
( 
  uint32 flow_id
);

/*===========================================================================

FUNCTION HDRBCMAC_CHANNEL_DISABLE

DESCRIPTION
  The function disables a flow in the HDR Broadcast MAC Protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  flow_id - The flow to disable.
  
RETURN VALUE
  The channel index of the logical channel being disabled (-1 if no channel
  is being disabled)

SIDE EFFECTS
  None.

===========================================================================*/
extern int hdrbcmac_channel_disable
( 
  uint32 flow_id 
);


/*===========================================================================

FUNCTION HDRBCMAC_RESET

DESCRIPTION
  Resets this protocol layer.  The layer will require a new channel map
  inorder to begin processing physical layer packets.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrbcmac_reset(void );


/*===========================================================================

FUNCTION HDRBCMAC_CLEAR_CHANNEL_METRICS

DESCRIPTION
  Clears logical channel metrics for a particular channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrbcmac_clear_channel_metrics
(
  uint8 chan_idx 
    /* Index of channel to clear */
);

/*===========================================================================

FUNCTION HDRBCMAC_GET_CHANNEL_METRICS

DESCRIPTION
  Retrieves logical channel metrics for a particular channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrbcmac_get_channel_metrics
(
  uint8 chan_idx,
    /* Index of channel to retrieve metrics for */
  hdrlogbc_mac_metrics_type *chan_metrics
    /* Destination for metric data */
);


/*===========================================================================

FUNCTION HDRBCMAC_GET_MULTIPLEX_COUNT

DESCRIPTION
  Return the current multiplex count (4, 8 or 16).

DEPENDENCIES
  None.

RETURN VALUE
  The current multiplex count.

SIDE EFFECTS
  None.

===========================================================================*/
extern int hdrbcmac_get_multiplex_count( void );


/*===========================================================================

FUNCTION HDRBCMAC_IS_PHYS_CHAN_ACTIVE

DESCRIPTION
  Returns whether a physical channel (a specific interlace-multiplex pair) 
  is currently active.  (Active=being monitored for broadcast packets)

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - Interlace/multiplex pair channel is active.
  FALSE - Interlace/multiplex pair channel is not active.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrbcmac_is_phys_chan_active
( 
  uint8 intr, 
  uint8 mux
);


/*===========================================================================

FUNCTION HDRBCMAC_GET_METRICS_PTR

DESCRIPTION
  Returns a pointer to the metrics maintained by the MAC.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to structure containing metrics maintained by MAC.

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrbcmac_static_metrics * hdrbcmac_get_metrics_ptr( void );


/*===========================================================================

FUNCTION HDRBCMAC_GET_CHANNEL_COUNT

DESCRIPTION
  This function returns the number of active logical channels.
  
DEPENDENCIES
  None.

PARAMETERS
  None.  
  
RETURN VALUE
  Number of active broadcast channels.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint8 hdrbcmac_get_channel_count( void );


/*===========================================================================

FUNCTION HDRBCMAC_GET_FLOW_ID

DESCRIPTION
  This function returns the flow id associated with a logical channel.
  
DEPENDENCIES
  None.

PARAMETERS
  chan_idx - Logical channel
  flow_id  - Pointer to uint32 to place the flow_id value.
  
RETURN VALUE
  TRUE - Logical channel is active and has a flow_id associated with it.
  FALSE - Logical channel is not active.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrbcmac_get_flow_id
(
  uint8 chan_idx, 
  uint32* flow_id
);


/*===========================================================================

FUNCTION HDRBCMAC_GET_OVHD_INFO_PTR

DESCRIPTION
  This function externalizes a pointer to the current overhead information
  stored by HDRBCMAC.

DEPENDENCIES
  This function should only be called from within HDRBC task context.

PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern const hdrbcmac_ovhd_info_type * hdrbcmac_get_ovhd_info_ptr( void );

#endif /* HDRBCMACI_H */



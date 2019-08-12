#ifndef HDRBCMAC_H
#define HDRBCMAC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     H D R   B R O A D C A S T   M A C
       
                             P R O T O C O L
       
                  E X T E R N A L   H E A D E R   F I L E       

GENERAL DESCRIPTION
  This file contains the external interface of the HDR Broadcast MAC
  Protocol.
  
EXTERNALIZED FUNCTIONS
  hdrbcmac_init
    Initialize the protocol.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol. 

  Copyright (c) 2004, 2005 by Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2006 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/api/hdrbcmac.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
03/20/06   kss     Added hdrbcmac_enable_bcmcs_sleep() and 
                   hdrbcmac_is_ok_to_sleep() for sleep optimizations.
10/28/05   dna     Add pilot pn as parameter to hdrbcmac_update_ovhd_info.
09/30/05   mpa     Added command processing capability.
09/01/05   kss     Added hdrbcmac_process_ind().
08/28/05   dna     (mpa for) Use bcovhd interface to retrieve ovhd info.
02/02/05   etv     Fixed coding style to match convention.
09/19/04   etv     Renamed hdrbcmac_clear_metrics() to
                   hdrbcmac_clear_phy_chan_metrics().
09/17/04   kss     Added hdrbcmac_run_scheduler(), moved several function
                   prototypes to hdrbcslot.h. 
08/19/04   kss     Some comment clean-up, added hdrbcmac_clear_metrics().
08/18/04   kss     Implemented BCMAC suspend/resume for hybrid operation
08/02/04   kss     Restored time parameter to hdrbcmac_queue_pkt().
07/08/04   kss     Split into this file and hdrbcmaci.h
07/06/04   kss     Added functions for logging, extern metrics structures.
07/02/04   kss     Removed time parameter from hdrbcmac_queue_pkt().
07/02/04   mpa     Renamed hdrbcast to hdrbcovhd
06/07/04   kss     Added prototype for hdrbcmac_init().
04/24/04   kss     Ported from QC Corp.

===========================================================================*/

/* EJECT */
/*===========================================================================

                         INCLUDE FILES FOR MODULE
                     
===========================================================================*/


#include "hdr_variation.h"
#include "dsm.h"    
#include "hdrind.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

        
/*===========================================================================
                   
                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions and declarations for constants, macros,
types, variables and other items needed by this module.

===========================================================================*/

typedef enum
{
  HDRBCMAC_UPDATE_OVHD = 0
} hdrbcmac_cmd_name_enum_type;

typedef struct
{
  hdrbcmac_cmd_name_enum_type name;         /* Name of the command */
  hdrhai_protocol_name_enum_type sender;    /* Protocol giving cmd */
  sys_pilot_pn_type         pilot_pn;
} hdrbcmac_cmd_union_type;

/* Max supported simultaneous logical channels */                                           
#define HDRBCMAC_MAX_LOGICAL_CHANNELS   10


/*===========================================================================

FUNCTION HDRBCMAC_CLEAR_PHY_CHAN_METRICS

DESCRIPTION
  Clears metrics that do not relate to specific channels (interlace-
  multiplex crc counts, for example).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrbcmac_clear_phy_chan_metrics(void);

/*===========================================================================

FUNCTION HDRBCMAC_PROCESS_PKT

DESCRIPTION
  Processes a Broadcast MAC packet by placing it in the ECB, and decoding
  if necessary.
  
  Runs in HDR Broadcast task context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrbcmac_process_pkt
(
  dsm_item_type* item_ptr 
    /* Mac packet to be processed */
);


/*===========================================================================

FUNCTION HDRBCMAC_QUEUE_PKT

DESCRIPTION
  Places a received Broadast MAC packet in the Broadcast queue for 
  processing by the BC task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrbcmac_queue_pkt
(
  dsm_item_type* bc_pkt_ptr,
  int16          slot_time
);


/*===========================================================================

FUNCTION HDRBCMAC_INIT

DESCRIPTION
  Initializes HDRBC MAC layer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrbcmac_init(void );


/*===========================================================================

FUNCTION HDRBCMAC_RUN_SCHEDULER

DESCRIPTION
  Updates DSP with broadcast info for next frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrbcmac_run_scheduler( void );


/*===========================================================================

FUNCTION HDRBCMAC_WAS_BCC_PACKET_SCHEDULED

DESCRIPTION
  Query function that can be used to track punctured packets (getting a FTC
  or CC packet when a broadcast packet was scheduled).  
  
  Returns whether there was scheduled to be a broadcast packet at the 
  indicated slot time.
  
DEPENDENCIES
  None.

RETURN VALUE
  Returns whether there was scheduled to be a broadcast packet at the 
  indicated slot time.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrbcmac_was_bcc_packet_scheduled
(
  uint16 str
);

/*===========================================================================

FUNCTION HDRBCMAC_UPDATE_DECODE_INFO

DESCRIPTION
  Called when a packet is received, in order to update tracking of 
  broadcast packets.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrbcmac_update_decode_info
(
  uint16  str,
  boolean bcc_flag,
  boolean crc_flag
);

/*===========================================================================

FUNCTION HDRBCMAC_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRBCMAC.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Indication to process is a global enum for all possible 
             indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrbcmac_ind_cb
( 
  hdrind_ind_name_enum_type ind_name, 
  void *ind_data 
);

/*===========================================================================

FUNCTION HDRBCMAC_PROCESS_IND

DESCRIPTION
  This function processes indications given to BCMAC.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for BCMAC to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrbcmac_process_ind
( 
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr 
);

/*===========================================================================

FUNCTION HDRBCMAC_PROCESS_CMD

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
extern void hdrbcmac_process_cmd
(
  hdrbcmac_cmd_union_type *cmd_ptr
);

/*===========================================================================

FUNCTION HDRBCMAC_PROCESS_UPDATE_OVHD_INFO

DESCRIPTION
  This command updates the Broadcast MAC related Overhead info.
  The command will be queued and handled by the broadcast task context.
  
DEPENDENCIES
  None.

PARAMETERS
  sender - Protocol sending the command
   
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrbcmac_update_ovhd_info 
(
  hdrhai_protocol_name_enum_type sender,
  sys_pilot_pn_type         pilot_pn
);


/*===========================================================================

FUNCTION HDRBCMAC_ENABLE_BCMCS_SLEEP

DESCRIPTION
  This function enables support for BCMCS sleep.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrbcmac_enable_bcmcs_sleep( void );


/*===========================================================================

FUNCTION HDRBCMAC_DISABLE_BCMCS_SLEEP

DESCRIPTION
  This function disables support for BCMCS sleep.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrbcmac_disable_bcmcs_sleep( void );


/*===========================================================================

FUNCTION HDRBCMAC_IS_OK_TO_SLEEP

DESCRIPTION
  This function reports on whether HDR BCMCS MAC is ok to 
  sleep.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  Boolean - Whether HDR BC MAC is ok to sleep immediately.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrbcmac_is_ok_to_sleep( void );

#endif /* HDRBCMAC_H */


#ifndef HDRBCSLOT_H
#define HDRBCSLOT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              H D R   B R O A D C A S T   M A C  P R O T O C O L       
              
                B R O A D C A S T   S L O T   S C H E D U L E R 
                
                   E X T E R N A L   H E A D E R   F I L E  

GENERAL DESCRIPTION
  This module contains the external interface to the HDR Broadcast MAC
  Protocol slot scheduling algorithm.
    
EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2004 - 2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
  

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbcslot.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
01/30/09   etv     Merged the following changes to main branch
                   Added support for 8/16 Multiplexes
03/03/06   kss     Added hdrbcslot_is_ready_to_sleep() and 
                   hdrbcslot_track_packet_arrival().
10/28/05   dna     Add pilot pn as parameter to hdrbcslot_update_phy_ch_info.
08/28/05   dna     (mpa for) Use new hdrbcovhd interface to retrieve data.
02/02/05   etv     Fixed the coding style to match the convention.
11/18/04   kss     Added includes of sys.h and hdrovhd.h.
11/04/04   kss     Removed hdrbcslot_start/stop functions (no longer needed).
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
09/17/04   kss     Moved several function prototypes from hdrbcmac.h to here.
08/02/04   kss     Removed used fields in metric structure.
07/19/04   kss     Removed unused externs.
07/14/04   kss     Removed old debug enums.
07/08/04   kss     Moved soft combine and slot_enable interface to hdrbcmac.h
07/06/04   kss     Added soft combine enable/disable interface.
06/14/04   ss      (mpa for) Updated hdrbcslot_bcc_phys_ch_info_type
04/24/04   kss     Ported from QC Corp.

===========================================================================*/

/* EJECT */
/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/


/*===========================================================================
  
                     INCLUDE FILES FOR MODULE
                        
===========================================================================*/
#include "hdr_variation.h"
#include "customer.h"
#include "mccdma.h"

#ifdef FEATURE_CMI
#include "mccdma_v.h"
#endif

#include "qw.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

#include "hdrhai.h"
#include "hdrbcovhd.h"
#include "hdrbcmaci.h"

/*===========================================================================

                           DATA DECLARATIONS

This section contains public type definitions, MACROS, constants, and
variable declarations
===========================================================================*/

typedef struct
{
  uint32 dspif_dec_phy_pkts;
  uint32 dspif_dec_phy_pkts_bef_fctl;
  uint32 hdrrx_punctured_pkts;
}hdrbcslot_bcast_metrics_type;

extern hdrbcslot_bcast_metrics_type hdrbcslot_bcast_metrics;


typedef struct 
{

  /* If this intr, mux pair is being Used or Not.          
     meaning is it mapped to any logical channels  
     currently being broadcast */
  boolean available;   
                       
  /* This contains the index of the logical channel ( within the
     overhead message) to which this intr,mux is mapped to. */
  uint8 log_chan_index;

  /* If this intr, mux pair belongs to any of the logical channels that we are 
     interested in. Note that a intr, mux should be available ( mapped to a 
     logical channel ) in order to be interested. This information is derived 
     from the InterestList. Note that a BCMCS Flow should be available and 
     interested to be decoded. */
  boolean interested;
  
  /* DRC that will be used (by DSP) to demodulate the packet. */
  byte drc;

  /* SlotLength associated with this broadcast packet
     Note that when slot_red field is set (to 1) slot_length will not
     be what is normally associated with this DRC  */
  byte slot_length;    

  /* Broadcast burst length. This should always be a multipe of slot_length. */
  byte burst_length;   

  /* If we have reduced the no. of slots for this packet, than what is 
     normally associated with this DRC ( rate) */
  boolean slot_red;    
  
  /* Contains the number of mac packets per physical layer packet. This 
     depends on the bcast data rate. */
  byte num_mac_pkts;

  uint8 soft_comb_pn_count;

  /* If a sector can be soft combined  its corresponding entry in the
     array is set to 1 otherwise set to 0. Sector ordering matches
      NeighborList ordering in SectorParam message. */
  sys_pilot_pn_type soft_comb_pn_list[HDRHAI_MAX_SP_NEIGHBORS];
}
hdrbcslot_bcc_phys_ch_info_type;


/* This structure contains information about a broadcast packet that will
   be decoded in a particular slot. This will be used by the hdrrx task
   to label the bcc decoder packets, with appropriate time stamp, logical
   channel packet number etc */
typedef struct
{
  boolean valid;
  uint8   intr;
  uint8   mux;
  uint16  hstr; /* half slot time reference */
  qword   packet_num;
}hdrbcslot_bcc_decode_info_type;


//DA Ummm, why are we externalizing so much data?  Some of these variables
// Are being accessed from other tasks, too.
extern hdrbcslot_bcc_phys_ch_info_type 
  hdrbcslot_bcc_phys_ch_table 
    [HDRBCMAC_INTERLACE_COUNT][HDRBCMAC_MULTIPLEX_COUNT];

extern hdrbcslot_bcc_decode_info_type hdrbcslot_bcc_decode_info[];

extern boolean hdrbcslot_intlock_on_dsm_free;

extern boolean hdrbcslot_bcc_ovhd_info_updated;


 /* EJECT */
/*=============================================================================
 
FUNCTION  HDRBCSLOT_DEC_INFO_GET_NUM_MAC_PKTS

DESCRIPTION
   Retrieves the number of mac pkts associated with a phy layer packet from
   the dec_info array at index == slot_index

DEPENDENCIES
  None.
 
ARGUMENTS
  slot_index - index of dec_info array.
   
RETURN VALUE:
  Number of MAC packes associated with a phy layer packet
   
SIDE EFFECTS:
  None.

=============================================================================*/
extern uint8 hdrbcslot_dec_info_get_num_mac_pkts(uint16 slot_index);


/*==========================================================================
 
FUNCTION  HDRBCSLOT_UPDATE_DSP_BROADCAST_INFO

DESCRIPTION
  Updates DSP with broadcast info for next frame.

DEPENDENCIES
  None.

PARAMETERS
  ovhd_info_ptr - Pointer to overhead info retrieved from hdrbcovhd.c

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
extern void hdrbcslot_update_dsp_broadcast_info
(
  const hdrbcmac_ovhd_info_type * const ovhd_info_ptr
);


/*===========================================================================

FUNCTION HDRBCSLOT_DISABLE_SOFT_COMBINING

DESCRIPTION
  Disables soft combining in broadcast.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrbcslot_disable_soft_combining( void );


/*===========================================================================

FUNCTION HDRBCSLOT_ENABLE_SOFT_COMBINING

DESCRIPTION
  Re-enables soft combining in broadcast (previous disabled by 
  hdrbcslot_disable_soft_combining).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrbcslot_enable_soft_combining( void );


 /* EJECT */
/*========================================================================
 *
FUNCTION     HDRBCSLOT_UPDATE_CURRENT_PN

DESCRIPTION
  Updates the current PN (the one we are receiving overhead from).

DEPENDENCIES
  None.

PARAMETERS
  pilot_pn - The current PN.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
extern void hdrbcslot_update_current_pn
( 
  sys_pilot_pn_type         pilot_pn
);


/*========================================================================
 *
FUNCTION     HDRBCSLOT_UPDATE_PHY_CH_INFO

DESCRIPTION
   Overhead message organizes most of the information on a per Logical 
   channel basis Except, its burst_length info is organized on int, mux basis.
   This function essentially extracts  information from the Overhead
   message and stores it in a separate structure hdrbcslot_bcc_phy_ch_table, 
   which organizes information on a int,mux basis. This organization is 
   important to update the interface with the DSP.

DEPENDENCIES
  None.

PARAMETERS
  ovhd_info_ptr - Pointer to overhead info retrieved from hdrbcovhd.c

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
extern void hdrbcslot_update_phy_ch_info
( 
  const hdrbcmac_ovhd_info_type *ovhd_info_ptr
);


/*===========================================================================

FUNCTION  HDRBCSLOT_CONFIG_SLEEP

DESCRIPTION
  Determines whether sleep can be supported with the current flow
  configuration, and if so, returns the sleep cycle supported.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Returns the sleep cycle supported by the current configuration.
  0 means sleep is not supported.
  
SIDE EFFECTS
  Ready to sleep flags are reset (to not ok to sleep for active interlaces).

============================================================================*/
uint16 hdrbcslot_config_sleep( void );


/*===========================================================================

FUNCTION  HDRBCSLOT_RESET_SLEEP_FLAGS

DESCRIPTION
  Resets BCMCS-releted internal ok-to-sleep type flags to false (for
  active interlaces).
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

============================================================================*/
void hdrbcslot_reset_sleep_flags( void );


/*===========================================================================

FUNCTION  HDRBCSLOT_TRACK_PACKET_ARRIVAL

DESCRIPTION
  Tracks the arrival of scheduled packets. This is needed to determine when 
  the AT can sleep (while still monitoring BCMCS flows).

DEPENDENCIES
  None.

PARAMETERS
  pkt_hstr - The 16-bit HSTR of the packet just received.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================*/
void hdrbcslot_track_packet_arrival
(
  uint16 pkt_hstr
);


/*===========================================================================

FUNCTION  HDRBCSLOT_IS_READY_TO_SLEEP

DESCRIPTION
  Returns whether the BCMCS air link layer is ready to sleep.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean - TRUE if BCMCS air link is ready to sleep.

SIDE EFFECTS
  None.

============================================================================*/
extern boolean hdrbcslot_is_ready_to_sleep( void );

#endif /* HDRBCSLOT_H */


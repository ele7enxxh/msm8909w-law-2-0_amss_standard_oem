#ifndef HDRBCOVHD_H
#define HDRBCOVHD_H

/*===========================================================================

       B R O A D C A S T     P R O T O C O L       S U I T E
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the Broadcast Overhead Protocol 

Copyright (c) 2004 - 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/inc/hdrbcovhd.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/09   wsh     Support CMI API changes in sys.h 
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
01/30/09   etv     Merged the following changes to main branch
                   Fixed Klocwork errors.
                   Added support for 8/16 Multiplexes
01/27/09   wsh     Merged BCMCS 2.0 onto mainbranch
11/10/08   smd     Replaced errno.h with IxErrno.h  
12/12/07   sju     Renamed hdr channel record type structure
07/13/07   sju     Changed channel record type to hdr channel record type 
04/07/06   dna     Add support for programs with FlowDiscriminatorCount == 0
04/11/06   mpa     Added hdrbcovhd_msg_is_up_to_date().
04/05/06   mpa     Added hdrbcovhd_get_flow_format().
02/13/06   mpa     Added hdrbcovhd_get_program_id_lsbs().
09/19/05   mpa     Added hdrbcovhd_is_channel_in_additional_cdma_channel_list()
08/28/05   dna     (mpa for) bcovhd redesign and caching. 
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
10/29/04   mpa     Set Max # of supported bcmcs flows in ovhd to 10
07/22/04   mpa     (ss) Store link on which the ovhd msg was received.
07/21/04   kss     Added accessor function for includeFCS field.
07/14/04   kss     Moved BC rate definitions to hdrbcslot.
07/02/04   mpa     Code cleanup.  Moved all flow related ops to hdrbccp.
                   Only keep broadcast overhead related functions.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

             INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "comdef.h"
#include "IxErrno.h"
#include "dsm.h"
#include "hdrhai.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif


/*===========================================================================

                           CONSTANTS

===========================================================================*/

#define HDRBCOVHD_MAX_MULTIPLEXES 16
#define HDRBCOVHD_MAX_INTERLACES  4

#ifdef FEATURE_HDR_BCMCS_2P0
#define HDRBCOVHD_MAX_SUPPORTED_MULTIPLEXES HDRBCOVHD_MAX_MULTIPLEXES
#define HDRBCOVHD_BCMCS_MAX_FLOWS SYS_BCMCS_20_MAX_FLOWS
#else
#define HDRBCOVHD_MAX_SUPPORTED_MULTIPLEXES 4
#define HDRBCOVHD_BCMCS_MAX_FLOWS SYS_BCMCS_10_MAX_FLOWS
#endif /* FEATURE_HDR_BCMCS_2P0 */

#define HDRBCOVHD_MAX_PHYSICAL_CHANNEL_COUNT 64

#define HDRBCOVHD_MAX_CDMA_CHANNELS 7

/*===========================================================================

                      TYPE DECLARATIONS

===========================================================================*/

typedef enum
{
   HDRBCOVHD_MULTIPLEX_COUNT_4  = 4,
   HDRBCOVHD_MULTIPLEX_COUNT_8  = 8,
   HDRBCOVHD_MULTIPLEX_COUNT_16 = 16,
   HDRBCOVHD_MULTIPLEX_COUNT_INVALID = 0xFF
}hdrbcovhd_multiplex_count_enum_type;

/* I dont want to use the value 0 in the enum */
typedef enum
{
   HDRBCOVHD_FRAMING_INVALID = 0,
   HDRBCOVHD_FRAMING_SEGMENT = 10,
   HDRBCOVHD_FRAMING_HDLC    = 11
}hdrbcovhd_framing_type;


typedef enum
{
   HDRBCOVHD_FLOW_FORMAT_0 = 0,
   HDRBCOVHD_FLOW_FORMAT_1 = 1
}hdrbcovhd_flow_format_enum_type;

/*  
    0 =>  1 
    1 =>  2 
    2 =>  3 
    3 =>  4 
    4 =>  6 
    5 =>  8 
    6 =>  9 
    7 =>  12
    8 =>  16
    9 =>  20
    10 => 24
    11 => 32
    12 => 36
    13 => 48
    14 => 60
    15 => 64
*/

typedef enum
{
   HDRBCOVHD_BURST_LENGTH_1 = 1,
   HDRBCOVHD_BURST_LENGTH_2 = 2,
   HDRBCOVHD_BURST_LENGTH_3 = 3,
   HDRBCOVHD_BURST_LENGTH_4 = 4,
   HDRBCOVHD_BURST_LENGTH_6 = 6,
   HDRBCOVHD_BURST_LENGTH_8 = 8,
   HDRBCOVHD_BURST_LENGTH_9 = 9,
   HDRBCOVHD_BURST_LENGTH_12 = 12,
   HDRBCOVHD_BURST_LENGTH_16 = 16,
   HDRBCOVHD_BURST_LENGTH_20 = 20,
   HDRBCOVHD_BURST_LENGTH_24 = 24,
   HDRBCOVHD_BURST_LENGTH_32 = 32,
   HDRBCOVHD_BURST_LENGTH_36 = 36,
   HDRBCOVHD_BURST_LENGTH_48 = 48,
   HDRBCOVHD_BURST_LENGTH_60 = 60,
   HDRBCOVHD_BURST_LENGTH_64 = 64,
   HDRBCOVHD_BURST_LENGTH_INVALID = 0xFF
}hdrbcovhd_burst_length_enum_type;


/*-------------------------------------------------
 Logical Channel Info valid for both Format 0 & 1
-------------------------------------------------*/
/* Information about a quantum physical channel */
typedef struct
{
  uint8 intr; 
  uint8 mux;
} hdrbcovhd_phy_chan_type;

/* Information for the Logical channel that carries a BCMCS FLOW.
   Two logical channels cannot contain the same phy_chan.

   To restate it from overhead message point of view: If a BCMCS flow 
   contains a phy_chan as that of another flow, then both flows must
   be associated with the same logical channel. Since there is no logical
   channel ID only way to verify this is to compare all the fields of this 
   structure. */
typedef struct
{
  uint8  phys_chan_count; 
  uint8  data_rate;
  uint8  outer_code;
  uint8  mac_pkts_per_ecb_row;
  hdrbcovhd_phy_chan_type phys_chan[HDRBCOVHD_MAX_PHYSICAL_CHANNEL_COUNT];
} hdrbcovhd_logical_channel_info_type;

typedef struct
{
  uint8  soft_comb_pn_count;
  sys_pilot_pn_type soft_comb_pn_list[HDRHAI_MAX_SP_NEIGHBORS];
} hdrbcovhd_soft_combine_info_type;


/*-----------------------------------------
  Broadcast Overhead structure. 
-----------------------------------------*/

/* The flow offset structure contains the bit offsets to various
 * places in the cached message to ease the extraction of fields.
 */
typedef struct
{
  uint32 bcmcs_flow_id;
  uint16 program_offset; /* Only used if bcmcs_flow_format is 1 */
  uint16 flow_offset;
  uint16 logical_channel_offset;
  uint16 soft_combine_offset;
  uint16 additional_cdma_channel_offset;

  /* By unpacking and storing the soft combine info we use an extra 4K bytes
   * of memory, but avoid re-extracting the logical channel on most handoffs.
   */
  hdrbcovhd_soft_combine_info_type soft_comb_pn_info;
} hdrbcovhd_flow_offsets_type;

/* The structure contains the offsets for "program only" entries i.e.
 * programs listed as available on other channels with no discriminators.
 */
typedef struct
{
  uint32 bcmcs_flow_id; /* Desired FlowID matching BOM "program only" entry */
  uint16 program_offset;        /* Program with FlowDiscriminatorCount == 0 */
  uint16 additional_cdma_channel_offset;     /* AdditionalCDMAChannelCount1 */
} hdrbcovhd_program_only_offsets_type;

typedef struct
{
  uint16 broadcast_overhead_signature;
  uint16 sector_signature;
  uint8 neighbor_count;
  hdrbcovhd_multiplex_count_enum_type multiplex_count;
  hdrbcovhd_framing_type framing_type; /* 11-HDLC 10-Segment based */
  boolean include_fcs;
  hdrbcovhd_flow_format_enum_type bcmcs_flow_format;
  boolean interlace_included[HDRBCOVHD_MAX_INTERLACES];
  boolean same_burst_lengths[HDRBCOVHD_MAX_INTERLACES];
  uint16 total_burst_length[HDRBCOVHD_MAX_INTERLACES];
  hdrbcovhd_burst_length_enum_type 
    burst_length[HDRBCOVHD_MAX_INTERLACES][HDRBCOVHD_MAX_SUPPORTED_MULTIPLEXES];
  uint8 bcmcs_flow_id_length;
  uint8 flow_count; /* About 100 bytes to this point depending on compiler */
  uint8 program_count;
} hdrbcovhd_bc_msg_head_type;

/* A Format 1 flow_id must include the FlowDiscriminatorLength to allow
 * the ProgramIDLSBs to be located within the FlowID if the BOM doesn't
 * contain the FlowDiscriminators for a given program.
 */ 
typedef struct
{
  sys_bcmcs_flow_id_type flow_id;
  uint8 flow_discrim_len; 
} hdrbcovhd_flow_id_type;

typedef struct
{
  hdrbcovhd_bc_msg_head_type head;
  uint8 num_desired_flows; /* How many desired flows were found in msg */
  hdrbcovhd_flow_offsets_type desired_flow[HDRBCOVHD_BCMCS_MAX_FLOWS];
  /* How many desired flows are programs with no flow discriminators */
  uint8 num_desired_programs; 
  hdrbcovhd_program_only_offsets_type desired_program[HDRBCOVHD_BCMCS_MAX_FLOWS];
} hdrbcovhd_bc_msg_type;

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=============================================================================

FUNCTION HDRBCOVHD_GET_FRAMING_TYPE

DESCRIPTION
  Accessor function to query the broadcast framing type.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  HDRBCOVHD_FRAMING_INVALID if hdrbcovhd has no BroadcastOverhead message
  HDRBCOVHD_FRAMING_SEGMENT or HDRBCOVHD_FRAMING_HDLC otherwise, depending
  on the specified framing type in the BroadcastOverhead message

SIDE EFFECTS
  None

=============================================================================*/
extern hdrbcovhd_framing_type hdrbcovhd_get_framing_type( void );

/*=============================================================================

FUNCTION HDRBCOVHD_IS_FCS_INCLUDED

DESCRIPTION
  Accessor function to query the broadcast "include fcs" field, which 
  indicates whether an FCS is used in framing.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if the IncludeFCS field of the BroadcastOverhead message is set to one,
  FALSE otherwise, including if hdrbcovhd has no BroadcastOverhead message

SIDE EFFECTS
  None

=============================================================================*/
extern boolean hdrbcovhd_is_fcs_included( void );

/*=============================================================================

FUNCTION HDRBCOVHD_GET_FLOW_ID_LEN

DESCRIPTION
  Accessor function to query the FlowIDLength

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  zero if hdrbcovhd has no BroadcastOverhead message
  2, 3, or 4 is the number of octets used for the FlowID

SIDE EFFECTS
  None

=============================================================================*/
extern uint8 hdrbcovhd_get_flow_id_len( void );

/*===========================================================================
FUNCTION HDRBCOVHD_IS_FLOW_AVAILABLE

DESCRIPTION
  Tells if a flow is available in the most recentely recevied 
  BroadcastOverhead message.
  
DEPENDENCIES
  
PARAMETERS
  flow_id - The FlowID of "desired flow" to look for.  This must be a flow
            that has been requested by an application.

RETURN VALUE
  TRUE  if the desired FlowID passed in is listed as being available.
  FALSE if the FlowID is not a desired flow, or if the flow is desired but
        not available or if hdrbcovhd has no BroadcastOverhead message
  
SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrbcovhd_is_flow_available
(
  uint32 flow_id
);

/*===========================================================================
FUNCTION HDRBCOVHD_BASIC_INFO_HAS_CHANGED

DESCRIPTION
  Determines if the structure of the BCMCS physical channel (interlaces,
  multiplexes and burst lengths) has changed. 
  
  Specifically, it compares these values:
  
  multiplex_count
  framing_type
  include_fcs
  bcmcs_flow_format
  interlace_included[HDRBCOVHD_MAX_INTERLACES]
  same_burst_lengths[HDRBCOVHD_MAX_INTERLACES]
  total_burst_length[HDRBCOVHD_MAX_INTERLACES]
  burst_length[HDRBCOVHD_MAX_INTERLACES][HDRBCOVHD_MAX_MULTIPLEXES]
  bcmcs_flow_id_length
  
  
DEPENDENCIES
  
PARAMETERS
  bc_msg_head_ptr - Pointer to an older version of the BroadcastOverhead 
                    message head, to be compared with the latest version.

RETURN VALUE
  TRUE if the information has changed
  FALSE otherwise
  
SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrbcovhd_basic_info_has_changed
(
  const hdrbcovhd_bc_msg_head_type  *bc_msg_head_ptr
);

/*===========================================================================
FUNCTION HDRBCOVHD_GET_BC_MSG_HEAD

DESCRIPTION
  Returns the head of the BroadcstOverhead information, up to the flow
  specific information.
  
DEPENDENCIES
  
PARAMETERS
  bc_msg_head_ptr - Location to return BroadcastOverhead message info into.          

RETURN VALUE
  E_SUCCESS if all data was extracted successfully
  E_FAILURE if hdrbcovhd has no BroadcastOverhead message
  
  If E_SUCCESS is returned, the buffer at address bc_msg_msg_head_ptr is filled 
  in.  Otherwise it is unchanged.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern errno_enum_type hdrbcovhd_get_bc_msg_head
(
  hdrbcovhd_bc_msg_head_type  *bc_msg_head_ptr
);

/*===========================================================================
FUNCTION HDRBCOVHD_FLOW_INFO_HAS_CHANGED

DESCRIPTION
  Determines if the structure of the BCMCS logical channel for a given
  desired flow has changed.
  
  Specifically, this function compares these fields:
  
  phys_chan_count
  data_rate
  outer_code
  mac_pkts_per_ecb_row
  phys_chan[HDRBCOVHD_MAX_PHYSICAL_CHANNEL_COUNT] 
  (i.e. the list of interlace/multiplex pairs assigned to the flow)
  
  
DEPENDENCIES
  
PARAMETERS
  bcmcs_flow_id         - FlowID of the flow to compare
  logical_chan_info_ptr - Pointer to older logical channel info for the given 
                          FlowID, to be compared with the latest.

RETURN VALUE
  TRUE if the information has changed or if the FlowID is not a desired flow
  FALSE otherwise
  
SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrbcovhd_flow_info_has_changed
(
  uint32                               bcmcs_flow_id,
  const hdrbcovhd_logical_channel_info_type  *logical_chan_info_ptr
);

/*===========================================================================
FUNCTION HDRBCOVHD_GET_LOGICAL_CHAN_INFO

DESCRIPTION
  Returns the flow IDs, logical channel and soft combine information for all
  desired flows.  We want to give the all three lists here so it is all 
  within the same critical section.  Otherwise, since they are parallel
  arrays, it would be bad to pull one from a different message than the others.
  
DEPENDENCIES
  
PARAMETERS
  max_bcmcs_flow_ids    - Size of the flow ID array pointer passed in
  num_flows_ptr         - Number of desired flows
  bcmcs_flow_id_ptr     - Array to store desired FlowIDs into.
  logical_chan_info_ptr - Location to return logical channel information
                          for desired flows.
  soft_combine_info_ptr - Location to return soft combine information for
                          desired flows.

RETURN VALUE
  E_SUCCESS if all data was extracted successfully
  E_FAILURE if hdrbcovhd has no BroadcastOverhead message
  
  If E_SUCCESS is returned, the buffers pointed to by flow_id_ptr, 
  logical_chan_info_ptr and soft_combine_info_ptr are updated.
  Otherwise they are unchanged.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern errno_enum_type hdrbcovhd_get_logical_chan_info
(
  uint8                                max_bcmcs_flow_ids,
  uint8                                *num_flows_ptr,
  uint32                               *bcmcs_flow_id_ptr,
  hdrbcovhd_logical_channel_info_type  *logical_chan_info_ptr,
  hdrbcovhd_soft_combine_info_type     *soft_combine_info_ptr
);

/*===========================================================================
FUNCTION HDRBCOVHD_FLOWS_HAVE_SAME_LOGICAL_CHANNEL

DESCRIPTION
  Determines if the two FlowIDs given have the same logical channel
  
DEPENDENCIES
  
PARAMETERS
  bcmcs_flow_id1 - FlowID to compare
  bcmcs_flow_id2 - FlowID to compare

RETURN VALUE
  TRUE if the two flows are desired flows that both reference the same
  logcial channel information
  FALSE if either flow is not a desired flow or if the two flows do not
  share the same logical channel information
  
SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrbcovhd_flows_have_same_logical_channel
(
  uint32 bcmcs_flow_id1,
  uint32 bcmcs_flow_id2
);

/*===========================================================================
FUNCTION HDRBCOVHD_GET_PHYS_CHAN_COUNT

DESCRIPTION
  Returns the phys_chan_count for the given FlowID
  
DEPENDENCIES
  
PARAMETERS
  flow_id - The FlowID of "desired flow" to look for.  This must be a flow
            that has been requested by an application.
  phys_chan_count_ptr - Location to return phys_chan_count into.          

RETURN VALUE
  E_SUCCESS if all data was extracted successfully
  E_FAILURE if hdrbcovhd has no BroadcastOverhead message
  E_NOT_AVAILABLE if the FlowID was not a desired available flow 
  E_NO_DATA if there was problem unpacking the data
  
  If E_SUCCESS is returned, phys_chan_count_ptr is set to the phys_chan_count 
  of the given FlowID. Otherwise phys_chan_count_ptr is unchanged.
  desired.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern errno_enum_type hdrbcovhd_get_phys_chan_count
(
  uint32 flow_id,
  uint8  *phys_chan_count_ptr
);

/*===========================================================================
FUNCTION HDRBCOVHD_GET_FLOW_REG_INFO

DESCRIPTION
  Tells if a flow specified to be registered for paging and/or dynamic
  broadcast.
  
DEPENDENCIES
  None.
  
PARAMETERS
  flow_id - The FlowID of "desired flow" to look for.  This must be a flow
            that has been requested by an application.
  reg_for_paging_ptr     - Variable to return reg_for_paging value in
  reg_for_dynamic_bc_ptr - Variable to return reg_for_dynamic_bc value in          

RETURN VALUE
  E_SUCCESS if all data was extracted successfully
  E_FAILURE if hdrbcovhd has no BroadcastOverhead message
  E_NOT_AVAILABLE if the FlowID was not a desired available flow 
  E_NO_DATA if there was problem unpacking the data
  
  If E_SUCCESS is returned, the values for reg_for_paging and
  reg_for_dynamic_bc for the given FlowID are returned through the pointers
  provided.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern errno_enum_type hdrbcovhd_get_flow_reg_info
(
  uint32 flow_id,
  boolean *reg_for_paging_ptr,
  boolean *reg_for_dynamic_bc_ptr
);

/*===========================================================================
FUNCTION HDRBCOVHD_GET_PROGRAM_ID_LSBS

DESCRIPTION
  Returns the ProgramIDLSBs for the given flow (format 1)
  
DEPENDENCIES
  None.
  
PARAMETERS
  flow_id - The FlowID of "desired flow" to look for.  This must be a flow
            that has been requested by an application.
  program_id_lsbs_ptr - Place to return ProgramIDLSBs info          

RETURN VALUE
  E_SUCCESS if all data was extracted successfully
  E_FAILURE if hdrbcovhd has no BroadcastOverhead message
  E_NOT_AVAILABLE if the FlowID was not a desired available flow 
  E_NO_DATA if there was problem unpacking the data
  
  If E_SUCCESS is returned, the ProgramIDLSBs value for the given FlowID 
  is returned through the pointer provided.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern errno_enum_type hdrbcovhd_get_program_id_lsbs
(
  uint32 flow_id,
  uint32 *program_id_lsbs_ptr
);

/*===========================================================================
FUNCTION HDRBCOVHD_GET_ADDITIONAL_CDMA_CHANNEL_LIST

DESCRIPTION
  Returns the AdditionalCDMAChannel fields for the given flow
  
DEPENDENCIES
  
PARAMETERS
  flow_id - The FlowID of "desired flow" to look for.  This must be a flow
            that has been requested by an application.
  add_chan_count_ptr - Place to return AdditionalChannelCount into
  add_chan_ptr - Pointer to array to put AdditionalChannel records into.
  add_chan_max - Maximum number of AdditionalRecords to return.

RETURN VALUE
  E_SUCCESS if all the channels were extracted successfully
  E_FAILURE if hdrbcovhd has no BroadcastOverhead message
  E_NOT_AVAILABLE if the FlowID was not a desired available flow 
  E_NO_DATA if there was a problem extracting values
  E_DATA_TOO_LARGE if there were more channels than add_chan_max
  
  If E_SUCCESS or E_DATA_TOO_LARGE is returned, add_chan_count_ptr is set 
  to the number of valid additional channels that were extracted and 
  add_chan_ptr array is the list of channels.
  Otherwise, add_chan_count_ptr is set to zero and add_chan_ptr array
  may have been overwritten with inconsistent data.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern errno_enum_type hdrbcovhd_get_additional_cdma_channel_list
(
  uint32 flow_id,
  uint8  *add_chan_count_ptr,
  sys_hdr_channel_record_s_type *add_chan_ptr,
  uint8 add_chan_max
);

/*===========================================================================
FUNCTION HDRBCOVHD_IS_CHANNEL_IN_ADDITIONAL_CDMA_CHANNEL_LIST

DESCRIPTION
  Checks to see if a channel is available in the additional cdma channel list
  for a particular flow id.
  
DEPENDENCIES
  
PARAMETERS
  flow_id - The FlowID of "desired flow" to look for.  This must be a flow
            that has been requested by an application.
  channel - The channel to look up in a flow's additional CDMA channel list.

RETURN VALUE
  TRUE  - Channel was found in the channel list.
  FALSE - Channel was not found or there was an error in unpacking the
          channel info record.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrbcovhd_is_channel_in_additional_cdma_channel_list
(
  uint32 flow_id,
  sys_channel_type channel
);

/*=============================================================================
FUNCTION HDRBCOVHD_CREATE_BCMCS_FLOW_ID

DESCRIPTION
  This function creates a FlowID as per 3GPP2 C.S0054 Ver 1.0 Section 2.4.4.3.1
  for a BroadcastOverhead message using Format 1.
  
  +----------------+-----------+-------------------+---------------+
  | FlowDisLen (3) | Zeros (n) | ProgramIDLSBs (x) | FlowDis (0-7) |
  +----------------+-----------+-------------------+---------------+
  
  ProgramIDLSBs is of length (x) bits where (x) is 
   
  MIN ((BCMCSFliowIDLength - 3 - FlowDiscriminatorLength), ProgramIDLSBLength)
   
  If required there are (n) pad bits.                                                                  
                    
DEPENDENCIES
  None

PARAMETERS
  flow_id_length_in_bits     - Length of the Flow ID to be created in bits
  flow_discriminator_length  - Length of flow discriminator in bits
  flow_discriminator         - Flow discriminator
  program_id_lsbs            - Program ID LSBs

RETURN VALUE
  The flow ID that was created

SIDE EFFECTS
  None
=============================================================================*/
extern sys_bcmcs_flow_id_type hdrbcovhd_create_bcmcs_flow_id
(
  uint16 flow_id_length_in_bits,
  uint8  flow_discriminator_length,
  uint8  flow_discriminator,
  uint32 program_id_lsbs
);

/*=============================================================================

FUNCTION HDRBCOVHD_GET_FLOW_FORMAT

DESCRIPTION
  Accessor function to query the flow format

DEPENDENCIES
  None

PARAMETERS
  flow_format_ptr - returns the flow format

RETURN VALUE
  E_SUCCESS - hdr ovhd has a Broadcast Overhead message
  E_FAILURE - no broadcast overhead message

SIDE EFFECTS
  None

=============================================================================*/
extern errno_enum_type hdrbcovhd_get_flow_format
( 
  uint8 *flow_format_ptr 
);
/*=============================================================================

FUNCTION HDRBCOVHD_MSG_IS_UP_TO_DATE

DESCRIPTION
  Returns whether or not the bc ovhd message is up to date.

DEPENDENCIES
  None

PARAMETERS
  bc_item_ptr - Pointer to the bc item pointer

RETURN VALUE
  TRUE  - bc ovhd message is up to date
  FALSE - otherwise

SIDE EFFECTS
  None

=============================================================================*/
extern boolean hdrbcovhd_msg_is_up_to_date
(
  dsm_item_type *bc_item_ptr
);

#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */

#endif /* HDRBCOVHD_H */

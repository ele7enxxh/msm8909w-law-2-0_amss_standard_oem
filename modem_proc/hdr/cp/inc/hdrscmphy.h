#ifndef HDRSCMPHY_H
#define HDRSCMPHY_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

                 P H Y S I C A L  L A Y E R  P R O T O C O L

                            H E A D E R  F I L E

GENERAL DESCRIPTION
  This module provides supporting API for SCM module to handle AT-init Session
    
EXTERNALIZED FUNCTIONS
    
  hdrscmphy_is_attrib_simple 

  hdrscmphy_get_attrib_offset_and_size

  hdrscmphy_get_attrib_verno_index

  hdrscmphy_prop_max_transmit_power
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

  
  Copyright (c) 2007 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/17/13   arm     Supported NV enable/disable for network dtx attributes. 
12/10/08   smd     Replaced assert.h with amssassert.h
09/03/08   etv     Added signaling support for DTX feature.
09/15/07   etv     Fixed compiler warnings.
08/15/07   etv     Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/

#include "dsm.h"
#include "comdef.h"
#include "err.h"
#include "amssassert.h"

#include "hdrdebug.h"
#include "hdrdsm.h"
#include "hdrscp.h"
#include "hdrhai.h"
#include "hdrtrace.h"
#include "hdrbit.h"
#include "hdrutil.h"
#include "hdrhmp.h"

#include "hdrscm.h"
#include "hdrscmdb.h"


/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/* EJECT */ 
/*===========================================================================
 
                        DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
/* Maximum Number of Additional Reverse CDMA channels that can be assigned to
   the access terminal */
#define HDRSCMPHY_MAX_NUM_ADDITIONAL_RL_CDMA_CHANNELS 15

/* Maximum number of sectors for which the TxInitAdjust is transmitted to AT */
#define HDRSCMPHY_MAX_NUM_SECTORS                     7

/* Maximum number of simple attributes */
#define HDRSCMPHY_MAX_NUM_SIMPLE_ATTRIBS              7

/* Maximum number of complex attributes */
#define HDRSCMPHY_MAX_NUM_COMPLEX_ATTRIBS             2

/* Total number of attributes in ST-3 Physical layer protocol */
#define HDRSCMPHY_MAX_NUM_CONFIG_ATTRIBS                       \
        (HDRSCMPHY_MAX_NUM_SIMPLE_ATTRIBS + HDRSCMPHY_MAX_NUM_COMPLEX_ATTRIBS)

/* Maximum number of GAUPable attributes */
#define HDRSCMPHY_MAX_NUM_GAUPABLE_ATTRIBS            4

/* Enumeration of ST-3 Physical layer protocol attributes */
typedef enum
{
  /* Complex Attributes */

  HDRSCMPHY_ADDITIONAL_CARRIERS_INIT_TX_POWER = 0x01, 
    /* AdditionalCarriersInitTxPower */

  HDRSCMPHY_FIRST_COMPLEX_ATTRIB = HDRSCMPHY_ADDITIONAL_CARRIERS_INIT_TX_POWER,
    /* First Complex attribute */

  HDRSCMPHY_REVERSE_PILOT_TRANSMIT_SLOTS = 0x02,
    /* ReversePilotTransmitSlots */

  HDRSCMPHY_LAST_COMPLEX_ATTRIB = HDRSCMPHY_REVERSE_PILOT_TRANSMIT_SLOTS,
    /* Last Complex attribute */

  /* Simple Attributes */
  HDRSCMPHY_DTX_SUPPORT = 0xF9,
    /* DTX Support */

  HDRSCMPHY_FIRST_SIMPLE_ATTRIB = HDRSCMPHY_DTX_SUPPORT,
    /* First Simple attribute */

  HDRSCMPHY_MAC_INDEX_BITS = 0xFA,
    /* MACIndexBits */

  HDRSCMPHY_DTX_RRI_MODE = 0xFB,
    /* DTXRRIMode */

  HDRSCMPHY_DTX_MODE = 0xFC,
    /* DTXMode */

  HDRSCMPHY_MAX_TRANSMIT_POWER = 0xFD,
    /* MaxTransmitPower */

  HDRSCMPHY_MAX_RL_TX_POWER_DIFF = 0xFE,
    /* MaxRLTxPwrDiff */

  HDRSCMPHY_MULTI_CARRIER_CAPABILITY = 0xFF,
    /* MultiCarrierCapability */

  HDRSCMPHY_LAST_SIMPLE_ATTRIB = HDRSCMPHY_MULTI_CARRIER_CAPABILITY,

} hdrscmphy_attrib_id_enum_type;

/* Array for Version numbers of configuration attributes. The attribute
   version numbers are arranged in the same order as the attributes in 
   hdrscmphy_attrib_id_enum_type. It has version numbers for complex
   attributes followed by simple attributes. */
typedef uint8 hdrscmphy_verno_type[HDRSCMPHY_MAX_NUM_CONFIG_ATTRIBS];

#define HDRSCMPHY_ADDITIONAL_CARRIERS_INIT_TX_POWER_VERNO_OFFSET    0
#define HDRSCMPHY_DTX_RRI_MODE_VERNO_OFFSET                         4
#define HDRSCMPHY_DTX_MODE_VERNO_OFFSET                             5
#define HDRSCMPHY_MAX_RL_TX_POWER_DIFF_VERNO_OFFSET                 7 

/* Sector information type for sectors for which the TxInitAdjust is 
  transmitted to AT */
typedef struct
{
  uint16 pilot_pn_offset;
    /* Pilot PN sequence offset in units of 64 chips */

  uint8 relative_tx_init_adjust;
    /* TxInitAdjust for the assigned reverse CDMA channel relative to that 
       of the reference CDMA channel expressed in 2's complement number
       in steps of 1dB. */

} hdrscmphy_tx_init_adjust_sector_info_type;

/* Additional Reverse CDMA channels type */
typedef struct
{

  uint32 reverse_cdma_channel;
    /* Channel record of the reverse CDMA channel assigned to the AT */

  uint8 num_sectors;
    /* Number of sectors for which the TxInitAdjust is transmitted to AT */

  hdrscmphy_tx_init_adjust_sector_info_type sectors[HDRSCMPHY_MAX_NUM_SECTORS];
    /* Sector information for sectors for which the TxInitAdjust is transmitted
       to AT */

} hdrscmphy_additional_rl_cdma_channels_type;

/* Additional Carriers' Init Tx Power type */
typedef struct
{

  uint8 additional_carriers_assigned;
    /* Are any additional reverse CDMA channels assigned to AT? 
       If this is false, ignore rest of the fields in the struct. */

  uint32 reference_cdma_channel;
    /* Active Reverse CDMA channel that AT shall use to determine the 
       initial transmit power for the additional reverse CDMA channels */

  uint8 num_additional_rl_channels;
    /* Number of additional Reverse CDMA channels. */

  hdrscmphy_additional_rl_cdma_channels_type 
    additional_rl_chans[HDRSCMPHY_MAX_NUM_ADDITIONAL_RL_CDMA_CHANNELS];
    /* Additional Reverse CDMA channels. */
  
} hdrscmphy_additional_carriers_init_tx_pwr_type;

typedef struct
{
  uint8 enabled;
    /* Enable/Disable Reverse Link Pilot Transmissions for slots 
       specified by duration and period */

  uint8 duration;
    /* Rev-Link Pilot Transmission interval in units of sub-frames
       {000, 001, 010, 011} => {4, 8, 12, 16} sub-frames */

  uint8 period;
    /* Rev-Link Pilot Tranmission starts at time T (in frames) where T mod 
       period = 0. {0000, 0001, 0010, 0011, 0100} => {16, 20, 24, 28, 32} */

} hdrscmphy_reverse_pilot_transmit_slots_type;

typedef struct
{

  /* Complex Attributes */

  hdrscmphy_additional_carriers_init_tx_pwr_type init_tx_pwr;
    /* Additional Carriers Init Tx Power */

  hdrscmphy_reverse_pilot_transmit_slots_type rl_pilot_transmit_slots;
    /* Reverse Link Pilot Transmit slots */

  /* Simple Attributes.
     The order of simple attributes in the struct must be the same as the
     order generated by HDRSCMPHY_SIMPLE_ATTRIB_ID_TO_IND. Please make
     appropriate corrections when adding a new attribute  */

  uint8 dtx_support;
    /* Does AT support DTXMode? Capability Announcement attribute 
       Attribute ID: 0xF9; Valid Range {0x00, 0x01}
       0x00 - AT does not support DTX Mode
       0x01 - AT supports DTX Mode */

  /* uint8 drx_support; */
    /* Does AT support DRX Mode? Capability Announcement attribute.
       Attribute ID: 0xF9; Valid Range (0x00, 0x01)
       0x00 - AT does not support DRX Mode
       0x01 - AT supports DRX Mode */

  uint8 mac_index_bits;
    /* Enum specifying the number of bits in MAC index (FL & RL)
       Attribute ID: 0xFA; Valid Range {0x00, 0x01} 
       0x00 - AT supports 7-bit Preamble MAC index and 7-bit RL MAC index
       0x01 - AT supports an 8-bit Preamble MAC index and 9-bit RL MAC index */

  uint8 dtx_rri_mode;
    /* Enum specifying Discontinuous Transmission mode of RRI channel. 
       Attribute ID: 0xFB; Valid Range {0x00, 0x01} */

  uint8 dtx_mode;
    /* Enum specifying Discontinuous Tramission Mode for DRC, DSC and
       ACK Channel. Attribute ID: 0xFC; Valid Range {0x00, 0x01} */

  uint8 max_transmit_power;
    /* Enum specifying Maximum Transmit Power in 25mW increments starting
       from 20mW (0x00). Attribute ID: 0xFD; Valid Range: {0x00-0xFF} */

  uint8 max_rl_tx_pwr_diff;
    /* Enum specifying Maximum Transmit power differential between adjacent 
       carriers. Attribute ID: 0xFE; 
       Valid Range: {0x00-0x06} - {15, 18, 21, 24, 27, 30, INF}dB */

  uint8 multi_carrier_capability;
    /* Does the AT support Feedback Multiplexing Mode? 
       Attribute ID: 0xFF; Valid Range: {0x00 - 0x02}
       0x00 - AT supports No Feedback Multiplexing mode 
       0x01 - AT supports Basic Feedback Multiplexing Mode 
       0x02 - AT supports Enhanced Feedback Multiplexing Mode */

} hdrscmphy_config_struct_type;


/* EJECT */ 
/*===========================================================================

FUNCTION HDRSCMPHY_ATTRIB_IS_SUPPORTED

DESCRIPTION
  Is this attribute supported?

DEPENDENCIES
  None

PARAMETERS 
  attrib_id: Attribute ID in question.
  subtype:   Subtype of the protocol that received this attribute.

RETURN VALUE
  TRUE - If the attribute is supported.
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrscmphy_attrib_is_supported
(
  uint16 const attrib_id,
  uint16 const subtype
);

/* EJECT */ 
/*===========================================================================

FUNCTION HDRSCMPHY_PROCESS_COMPLEX_ATTRIB

DESCRIPTION
  This is a generic interface to process all complex attributes given below.

    0x01 - AdditionalCarriersInitTxPower
    0x02 - ReversePilotTransmitSlots 

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.
  subtype  - Subtype negotiated Physical layer protocol.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrscmphy_process_complex_attrib
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  uint16                        const          subtype
);

/* EJECT */ 
/*===========================================================================

FUNCTION HDRSCMPHY_ATTRIB_IS_SIMPLE

DESCRIPTION
  To find if the attribute given by the attrib_id is simple. 
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id - Attribute ID 

ASSUMPTIONS
  Assumes that the subtype is 0x0003
  Assumes that the attribute ID is valid for the given subtype.

RETURN VALUE
  TRUE - if the attribute given by 'attrib_id' is simple.
  FALSE - otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrscmphy_attrib_is_simple
(
  uint16 const attrib_id,
  uint16 const subtype
);

/* EJECT */ 
/*===========================================================================

FUNCTION HDRSCMPHY_SIMPLE_ATTRIB_IS_VALID

DESCRIPTION
  This function is used by SCM to query if the given 'attrib_value' is a 
  valid value for the attribute specified by 'attrib_id'.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id    - Attribute ID of the simple attribute.
  attrib_value - Value of the simple attribute. 
  subtype      - Subtype negotiated for FMAC.

ASSUMPTIONS
  1. Subtype is HDRSCP_SUB3_PHYS_LAYER. (Please note that any message for
     other subtypes will be dropped at attrib_is_supported check)
  2. Attribute ID given by "attrib_id" is a valid simple attribute for the 
     given subtype.

RETURN VALUE
  E_SUCCESS          - If the attrib_value is valid.
  E_OUT_OF_RANGE     - Otherwise.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern hdrerrno_enum_type hdrscmphy_simple_attrib_is_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value,
  uint16 const subtype
);

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMPHY_ATTRIB_IS_GAUPABLE

DESCRIPTION
  This function is used by SCM module to find out if an attribute is GAUPable.

DEPENDENCIES
  None.

PARAMETERS
  attrib_id - Attribute ID.
  subtype - Subtype of Physical layer protocol
  
RETURN VALUE
  E_SUCCESS       - If the attribute is GAUPable.
  E_NOT_ALLOWED   - If AN is not allowed to GAUP this attribute.

SIDE EFFECTS
  None.
===========================================================================*/
extern hdrerrno_enum_type hdrscmphy_attrib_is_gaupable
(
  uint16 const attrib_id,
  uint16 const subtype
);

/* EJECT */ 
/*===========================================================================

FUNCTION HDRSCMPHY_GET_ATTRIB_OFFSET_AND_SIZE

DESCRIPTION
  To get the attribute's byte offset into the config struct and its size.

DEPENDENCIES
  None

PARAMETERS
  attrib_id     - Attribute Id.
  attrib_offset - Address where the Byte Offset from the start of the struct 
                  to the attribute is stored.
  attrib_size   - Address where the size of the attribute is stored.
  
RETURN VALUE
  E_SUCCESS - If the given attrib ID is valid.
  E_DATA_INVALID - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern hdrerrno_enum_type hdrscmphy_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
);

/* EJECT */ 
/*===========================================================================

FUNCTION HDRSCMPHY_GET_ATTRIB_VERNO_INDEX

DESCRIPTION
  Returns the index of the given attribute in the version number array.

DEPENDENCIES
  None

PARAMETERS
  attrib_id     - Attribute Id.
  
RETURN VALUE
  E_SUCCESS - If the given attribute ID is valid.

SIDE EFFECTS
  None

===========================================================================*/
extern hdrerrno_enum_type hdrscmphy_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *index,
  uint16 subtype
);

/* EJECT */ 
/*===========================================================================

FUNCTION HDRSCMPHY_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  Set the configuration attributes to its default values.

DEPENDENCIES
  None

PARAMETERS
  cfg - Pointer to the config structure.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrscmphy_set_config_to_default
(
  hdrscmphy_config_struct_type *cfg
);

/* EJECT */ 
/*===========================================================================
FUNCTION HDRSCMPHY_CONFIG_IS_NEEDED                                 EXTERNAL

DESCRIPTION
  This function is used to find out if an AT-initiated Configuration is 
  needed by Phy ST-3

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE - If AT-init config is required.
  FALSE - Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrscmphy_config_is_needed( void );

/* EJECT */ 
/*===========================================================================
FUNCTION HDRSCMPHY_CONFIGURE                                        EXTERNAL

DESCRIPTION
  This function does the AT-init configuration of the following PHY attributes 
    1. MaxTransmitPower 
    2. DTXSupport

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE - If the configuration succeeded.
  FALSE - Otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrscmphy_configure( void );

/* EJECT */ 
/*===========================================================================

FUNCTION HDRSCMPHY_GET_PROPOSED_ATTRIB_VALUE

DESCRIPTION
  This function is used by SCM to get the attribute value proposed by AT in 
  the recent transaction.

DEPENDENCIES
  None.

PARAMETERS
  trans_id      - Transaction ID.
  a_aip         - Array of attribute information pointers where the info about
                  each of the attributes in the transaction specified by 
                  trans_id are stored.

RETURN VALUE
  E_SUCCESS - If the given transaction ID is valid.
  E_FAILURE - If a valid transaction specified by 'trans_id' is not the recent.

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrscmphy_get_proposed_attrib_value
(
  uint16 const trans_id,
  hdrscm_accepted_attrib_type *a_aip,
  uint16 const subtype
);

/* EJECT */ 
/*===========================================================================

FUNCTION HDRSCMPHY_FAIL_CURR_CONFIG_NEG

DESCRIPTION
  This function fails the current config negotiation

DEPENDENCIES
  None

PARAMETERS
  close_reason - Session Close reason

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrscmphy_fail_curr_config_neg
(
  hdrhai_session_close_reason_enum_type close_reason
);

#ifdef FEATURE_HDR_REVB_DTX_MODE
/*===========================================================================

FUNCTION HDRSCMPHY_SET_NET_DTX_MODE

DESCRIPTION
   This function is used to read network based dtx mode from NV.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscmphy_set_net_dtx_mode( boolean is_net_dtx_enabled_by_nv );
#endif /* FEATURE_HDR_REVB_DTX_MODE */


#endif /* HDRSCMPHY_H */

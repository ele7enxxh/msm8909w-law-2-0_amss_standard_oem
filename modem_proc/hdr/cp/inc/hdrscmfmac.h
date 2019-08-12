#ifndef HDRSCMFMAC_H
#define HDRSCMFMAC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        H D R   S E S S I O N  C O N F I G U R A T I O N  M A N A G E R 

           F O R W A R D   C H A N N E L   M A C   P R O T O C O L
                                    
                            H E A D E R   F I L E       

GENERAL DESCRIPTION
  This module contains the configuration interface of the HDR Reverse Channel 
  Medium Access Control (MAC) Protocol.

EXTERNALIZED FUNCTIONS

  hdrscmfmac_process_complex_attrib 
    Processes all FMAC complex attributes 
    
  hdrscmfmac_is_attrib_simple 
    Returns true if an attribute is simple

  hdrscmfmac_is_simple_attrib_valid
    Verifies the validity of the simple attribute

  hdrscmfmac_get_attrib_offset_and_size
    Returns attribute size and offset into the config structure.

  hdrscmfmac_get_attrib_verno_index
    Returns the index of an attribute in the version number array. 

INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2010 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/inc/hdrscmfmac.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/21/10   cnx     Added support for GAUP LoadInformationSupported attribute
06/07/10   wsh     Added support for Load Info Msg 
06/20/07   cc      Added support for DOrB FTC MAC.
01/03/07   etv     Added support to handle unknown attributes.
08/16/06   etv     Change interface get_simple_attr_size to take attrib_id.
06/08/06   etv     Send session close if AN GAUPs a non-GAUPable attribute.
12/07/05   etv     Added a function to get the simple attrib value size.
11/21/05   etv     Added header tags for perforce to fill in
11/02/05   etv     Changed SCM design to propogate subtype info.
10/12/05   etv     Fixed some L1 featurization bugs.
09/09/05   etv     Added function to set default values.
09/06/05   etv     Added edit history block and minor code review fixes.
08/22/05   etv     Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "hdr_variation.h"
#include "customer.h"  /* to make sure custhdr.h is included */
#include "dsm.h"
#include "comdef.h"
#include "err.h"

#include "hdrdsm.h"
#include "hdrscp.h"
#include "hdrhai.h"
#include "hdrtrace.h"

#include "hdrscm.h"

 /* EJECT */
/*===========================================================================
                  DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables and
other items needed by this module.

===========================================================================*/

/* DRC Lock configuration attribute data type. */
typedef struct
{
  uint8 period;
  uint8 length;
} hdrscmfmac_drc_lock_struct_type;

/* Handoff Delays configuration attribute data type. */
typedef struct
{
  uint8 softer_handoff_delay;
  uint8 soft_handoff_delay;
} hdrscmfmac_handoff_delays_struct_type;

/* Array of offsets for drc N. There is no offset for DRC 0; So the array
 * size is only 14 excluding DRC-0. Note: INDEX 0 HAS OFFSET FOR DRC 1 */
typedef uint8 
  hdrscmfmac_drc_translation_offset_type[ HDRHAI_MAX_NUM_REL_A_DRCS-1 ];

#ifdef FEATURE_HDR_REVB

/* ATSupportedPacketFormats configuration attribute data type */
typedef struct
{
  uint8 max_optional_data_rate;
  uint8 max_optional_payload_size;
  uint8 short_pkt_enabled_thresh;
} hdrscmfmac_at_supported_packet_formats_struct_type;

/* Array of mappings from Tentative DRC to TX DRC. There is no offset for DRC 0; So the array
 * size is only 26 excluding DRC-0 & DRC-F (N/A). Note: INDEX 0 HAS OFFSET FOR DRC 1 */
typedef uint8
  hdrscmfmac_tentative_drc_to_tx_drc_map_struct_type[HDRHAI_MAX_NUM_REL_B_DRCS-2];

/* ExtendedSpans configuration attribute data type*/
typedef struct
{
  boolean extended_spans_enabled;
  uint8 per_target_span;
} hdrscmfmac_extended_spans_struct_type;

#endif /* FEATURE_HDR_REVB */

/* Structure for all configuration attribute data */
typedef struct
{ 
  hdrscmfmac_handoff_delays_struct_type  handoff_delays;
    /* ID: 0x00; Complex - Minumum Interruption that AT should expect for
     * soft/softer handoff.
     * units: 8-slots; Valid Range: None. Default: 0x01(Softer), 0x10(Soft) */ 

  hdrscmfmac_drc_lock_struct_type        drc_lock;
    /* ID: 0x01; Simple - DRC lock period and length.
     *
     * Lock Length: Number of times the DRCLock bit is repeated. 
     * Def: units: None; Valid: (0x00-0x03)~(4-32)times; Default: 0x01(8)
     *
     * Lock Period: Time interval between 2 consecutive DRCLock bit xmissions.
     * Units: None; Valid:(0-1)~(8-16)slots. Default: 1 (16 slots)
     * +++ Note: This field is applicable only for Default Subtype +++ */

  /*----------------------------------------------------------------------
                   Applicable only for Enhanced FMAC Sub-type 
   ---------------------------------------------------------------------*/

  hdrscmfmac_drc_translation_offset_type  drc_translation_offset;
    /* ID: 0x01; Complex - Offset used for computing transmitted DRC value from
     * tentative DRC value.
     * drcN: units: None; Valid Range: (0-N); Default: 0 */

#ifdef FEATURE_HDR_REVC
  boolean                                 load_info_supported;
    /* ID: 0xe9; Simple */
#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_HDR_REVB
  /*----------------------------------------------------------------------
                   Applicable only for MC FMAC Sub-type 
   ---------------------------------------------------------------------*/
  hdrscmfmac_at_supported_packet_formats_struct_type at_supported_packet_formats;
    /* ID: 0x01; Complex */

  hdrscmfmac_tentative_drc_to_tx_drc_map_struct_type tentative_drc_to_tx_drc_map;
    /* ID: 0x02; Complex */

  hdrscmfmac_extended_spans_struct_type extended_spans;
    /* ID: 0x03; Complex */

  uint8                                   serving_sector_lock_across_sub_active_sets;
    /* ID: 0xEF; Can AT select a different serving sector on each of the Sub-Active Sets.
     * units: none.  Valid values: 0x00, 0x01.  Default value: 0x00 */

  uint8                                   support_gaup_delta_ack_channel_gain_mup;
    /* ID: 0xF0; if GAUP'ing DeltaACKChannelGainMUP is supported by AT.
     * units: none.  Valid values: 0x00, 0x01.  Default Value: 0x00 */

  uint8                                   mup_preamble_base;
    /* ID: 0xF1; Base MAC Index for MUP.
     * units: none.  Valid values: 0x06~0x3b, 0x48~0x7b. Default Value: 0x42 */

  uint8                                   support_gaup_mup_preamble_base;
    /* ID: 0xF2; if GAUP'ing MUPPreambleBase is supported by AT.
     * units: none.  Valid values: 0x00, 0x01.  Default Value: 0x00 */

#endif /* FEATURE_HDR_REVB */

  uint8                                   drc_boost_length;
    /* ID: 0xF3; Number of slots starting with the slot in which the DRC cover
     * is changed for which DRCChannelGain is boosted.
     * units: 2-slots. Valid Range: (0x00-0x20) Default Value: 0x04(8 slots) */

  uint8                                   drc_chan_gain_boost;
    /* ID: 0xF4; Increase in DRCChannelGain relative to DRCChannelGainBase
     * starting in slot where DRC cover is changed.
     * units: dB; Valid Range: (0x00-0x06)dB; Default: 0x00dB */

  uint8                                   dsc_boost_length;
    /* ID: 0xF5; Number of slots in units of 8-slots starting with the slot in
     * which the DSC is changed for which DSC Channel Gain is boosted. 
     * units: 8-slots; Valid Range: (0x00 - 0x40); Default: 0x10(128 slots) */

  uint8                                   dsc_chan_gain_boost;
    /* ID: 0xF6; Increase in DSCChannelGain in dB relative to DSCChannelGainBase
     * starting in slot where DSC is changed.
     * units: dB; Valid Range: (0x00-0x06)dB; Default value: 0x00dB */

  uint8                                   min_cont_span;
    /* ID: 0xF7; Simple - Minimum Continuation Span in sub-pkts.
     * units: Sub Packets; Valid Range: (0x01-0x10); Default: 0x04 sub-pkts */
  
  uint8                                   drc_supervision_timer;
    /* ID: 0xF8; Simple
     * DRC supervision timer = (drc_supervision_timer x 10)+240 ms
     * units: none; Valid Range: (0x01-0xFF); Default: 0x00 */

  uint8                                   su_mux_pkt_enabled;
    /* ID: 0xF9; Simple - Is SU mux pkt enabled?
     * units: none; Valid Range: (0x00-0x01); Default: 0x00 (Disabled) */

  uint8                                   short_pkt_enabled_thresh;
    /* ID: 0xFA; Simple - Short packet enabled threshold
     * units: none; Valid: (0x00-0x03)~(1024-4096)bits; Default: 0x01(2048) */

  uint8                                   delta_mup_ack_chan_gain;
    /* ID: 0xFB; Simple - MUP Ack channel gain delta in half-dB
     * units: half-dB; Valid Range: (0x00 - 0x12); Default: 0x0C(6 dB) */

  uint8                                   dsc_length;
    /* ID: 0xFC; Simple - Length of a single DSC transmission.
     * units: 8-slots; Valid Range: (0x01-0x20); Default: 0x08(64 slots) */

  uint8                                   mup_enabled;
    /* ID: 0xFD; Simple - Is MU pkt enabled? 
     * units: None; Valid Range: (0x00-0x01); Default: 0x00(Disabled) */

  uint8                                   drc_lock_length;
    /* ID: 0xFE; Lock Length: Number of times the DRCLock bit is repeated. 
     * units: None; Valid: (0x00-0x03)~(8-64)times; Default: 0x01(16) */

  uint8                                  null_rate_drc_38_4_enable;
    /* ID: 0xFD; Simple - Is DRC 1 enabled for NULL rate? 
     * units:None; Valid Range (0x00-0x01); Default: 0x00
     * +++ Note: This field is applicable only for Default Subtype +++ */

  uint8                                  drc_gating;
    /* ID: 0xFF; Simple - Continuous(0x00)/Discontinuous(0x01) DRC transmission
     * units: None; Valid Range: (0x00-0x01); Default: 0x00 */

} hdrscmfmac_config_struct_type;


typedef enum
{

  HDRSCMFMAC_ATTRIB_HANDOFF_DELAYS = 0x00,

  HDRSCMFMAC_ATTRIB_DRC_LOCK = 0x01,

  HDRSCMFMAC_ATTRIB_DRC_TRANSLATION_OFFSET = 0x01,

#ifdef FEATURE_HDR_REVB
  HDRSCMFMAC_ATTRIB_AT_SUPPORTED_PACKET_FORMATS = 0x01,

  HDRSCMFMAC_ATTRIB_TENTATIVE_DRC_TO_TX_DRC_MAP = 0x02,

  HDRSCMFMAC_ATTRIB_EXTENDED_SPANS = 0x03,

#ifdef FEATURE_HDR_REVC
  HDRSCMFMAC_ATTRIB_LOAD_INFO_SUPPORTED = 0xE9,
#endif /* FEATURE_HDR_REVC */

  HDRSCMFMAC_ATTRIB_SERVING_SECTOR_LOCK_ACROSS_SUB_ACTIVE_SETS = 0xEF,

  HDRSCMFMAC_ATTRIB_SUPPORT_GAUP_DELTA_ACK_CHANNEL_GAIN_MUP = 0xF0,

  HDRSCMFMAC_ATTRIB_MUP_PREAMBLE_BASE = 0xF1,

  HDRSCMFMAC_ATTRIB_SUPPORT_GAUP_MUP_PREAMBLE_BASE = 0xF2,
#endif /* FEATURE_HDR_REVB */

  HDRSCMFMAC_ATTRIB_DRC_BOOST_LENGTH = 0xF3,

  HDRSCMFMAC_ATTRIB_DRC_CHAN_GAIN_BOOST = 0xF4,

  HDRSCMFMAC_ATTRIB_DSC_BOOST_LENGTH = 0xF5,

  HDRSCMFMAC_ATTRIB_DSC_CHAN_GAIN_BOOST = 0xF6,

  HDRSCMFMAC_ATTRIB_MIN_CONTINUATION_SPAN = 0xF7,

  HDRSCMFMAC_ATTRIB_DRC_SUPERVISION_TIMER = 0xF8,

  HDRSCMFMAC_ATTRIB_SU_MUX_PACKETS_ENABLED = 0xF9,

  HDRSCMFMAC_ATTRIB_SHORT_PACKETS_ENABLED_THRESH = 0xFA,

  HDRSCMFMAC_ATTRIB_DELTA_ACK_CHAN_GAIN_MUP = 0xFB,

  HDRSCMFMAC_ATTRIB_DSC_LENGTH = 0xFC,

  HDRSCMFMAC_ATTRIB_MUP_ENABLED = 0xFD,

  HDRSCMFMAC_ATTRIB_DRC_LOCK_LENGTH = 0xFE,

  HDRSCMFMAC_ATTRIB_NULL_RATE_384_ENABLE = 0xFD,

  HDRSCMFMAC_ATTRIB_DRC_GATING = 0xFF

} hdrscmfmac_attrib_id_enum_type;

#ifdef FEATURE_HDR_REVB
/*
 * Rev. B Attribs Verno's are arranged after the pre-Rev. B attributes
 * as the diagram below
 *
 *  -------------------------------------------------------------------
 * | Pre Rev. B attribs (17) | Rev. B Complex (4) | Rev B. Simple (14) |
 *  -------------------------------------------------------------------
*/
#define HDRSCMFMAC_MAX_NUM_PRE_REL_B_CONFIG_ATTRIBS         17
#ifdef FEATURE_HDR_REVC
#define HDRSCMFMAC_SUBTYPE2_MAX_NUM_SIMPLE_CONFIG_ATTRIBS   15
#else /* !FEATURE_HDR_REVC */
#define HDRSCMFMAC_SUBTYPE2_MAX_NUM_SIMPLE_CONFIG_ATTRIBS   14
#endif /* !FEATURE_HDR_REVC */
#define HDRSCMFMAC_SUBTYPE2_MAX_NUM_COMPLEX_CONFIG_ATTRIBS  4
#define HDRSCMFMAC_SUBTYPE2_MAX_NUM_CONFIG_ATTRIBS          \
  (HDRSCMFMAC_SUBTYPE2_MAX_NUM_SIMPLE_CONFIG_ATTRIBS +      \
   HDRSCMFMAC_SUBTYPE2_MAX_NUM_COMPLEX_CONFIG_ATTRIBS)
#define HDRSCMFMAC_MAX_NUM_CONFIG_ATTRIBS                   \
  (HDRSCMFMAC_MAX_NUM_PRE_REL_B_CONFIG_ATTRIBS +            \
   HDRSCMFMAC_SUBTYPE2_MAX_NUM_CONFIG_ATTRIBS)
#else /* FEATURE_HDR_REVB */
#define HDRSCMFMAC_MAX_NUM_CONFIG_ATTRIBS   17
#endif /* FEATURE_HDR_REVB*/

typedef uint8 hdrscmfmac_verno_type[HDRSCMFMAC_MAX_NUM_CONFIG_ATTRIBS];


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_PROCESS_COMPLEX_ATTRIB

DESCRIPTION
  This is a generic interface to process all complex attributes given below.

    0x00 - Handoff Delays Attribute
    0x01 - DRC Translation Offset Attribute ( Enhanced FMAC )
    0x01 - DRC Lock Attribute ( Default FMAC )

  Note: Attribute ID 0x01 is shared between default FMAC and Enhanced FMAC.
  
DEPENDENCIES

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.
  subtype  - Subtype negitiated for FMAC

RETURN VALUE
  TRUE  - If a valid complex attribute was parsed successfully.
  FALSE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmfmac_process_complex_attrib
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  uint16                        const          subtype
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_IS_ATTRIB_SIMPLE

DESCRIPTION
  This function is used by SCM to query if an attribute is simple, given the
  attribute ID.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id - Attribute ID 
  subtype   - Subtype negotiated for FMAC.

RETURN VALUE
  TRUE - if the attribute given by 'attrib_id' is simple.
  FALSE - otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrscmfmac_is_attrib_simple
(
  uint16 const attrib_id,
  uint16 const subtype
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_IS_SIMPLE_ATTRIB_VALID

DESCRIPTION
  This function is used by SCM to query if the given 'attrib_value' is a 
  valid value for the attribute specified by 'attrib_id'.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id    - Attribute ID of the simple attribute.
  attrib_value - Value of the simple attribute. 
  subtype      - Subtype negotiated for FMAC.

RETURN VALUE
  E_SUCCESS          - If the attrib_value is valid.
  E_OUT_OF_RANGE     - Otherwise.

SIDE EFFECTS
  None.
  
===========================================================================*/
hdrerrno_enum_type hdrscmfmac_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value,
  uint16 const subtype

);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_GET_SIMPLE_ATTRIB_SIZE

DESCRIPTION
  This function is used by SCM to query the simple attrib size for negotiated 
  subtype of FMAC.
  
DEPENDENCIES
  None.

PARAMETERS
  subtype - Subtype negotiated for FMAC.

RETURN VALUE
  Size of the value ID in bytes.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern uint8 hdrscmfmac_get_simple_attrib_size
(
  uint16 const attrib_id,
  uint16 const subtype
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_GET_ATTRIB_OFFSET_AND_SIZE

DESCRIPTION
  This function computes the byte offset of the attribute specified by the 
  attrib_id in the struct hdrscmfmac_config_struct_type and stores it in 
  out-param attrib_offset. It also stores the size of the attribute in 
  attrib_size.

DEPENDENCIES
  None

PARAMETERS
  attrib_id     - Attribute Id.
  attrib_offset - Address where the Byte Offset from the start of the struct 
                  to the attribute is stored.
  attrib_size   - Address where the size of the attribute is stored.
  subtype       - Subtype of FMAC.
  
RETURN VALUE
  E_SUCCESS if the given attrib ID is valid.
  E_DATA_INVALID if the attrib ID is not valid

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrscmfmac_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_GET_ATTRIB_VERNO_INDEX

DESCRIPTION
  Returns the index of the given attribute in the version number array.

DEPENDENCIES
  None

PARAMETERS
  attrib_id     - Attribute Id.
  index         - Address where the index is stored.
  subtype       - Subtype of FMAC.
  
RETURN VALUE
  E_SUCCESS - If the given attribute ID is valid.
  

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrscmfmac_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *index,
  uint16 subtype
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  Set the configuration attributes to its default values.

DEPENDENCIES
  None

PARAMETERS
  hdrfmac_cfg - Pointer to the config structure.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void hdrscmfmac_set_config_to_default
(
  hdrscmfmac_config_struct_type *hdrfmac_cfg
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_GET_PROP_ATTRIB_VALUE

DESCRIPTION
  The function is used by SCM to get the proposed complex attribute value.

DEPENDENCIES
  None

PARAMETERS
  trans_id - Transaction ID of the Configuration message.
  a_aip    - Accepted attribute information pointer.
  subtype  - Subtype of the protocol for which the message is destined.
  
RETURN VALUE
  E_SUCCESS - If the subtype of the procotol supports AT-init negotiation 
              and if there was a valid outstanding transaction.
  E_FAILURE - If the trans_id does not match to any outstanding transaction.
  E_NOT_SUPPORTED - If the subtype does not support AT-init Neg.

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrscmfmac_get_prop_attrib_value
(
  uint16 const trans_id,
  hdrscm_accepted_attrib_type *a_aip,
  uint16 const subtype
);


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSCMFMAC_FAIL_CURR_CONFIG_NEG

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
void hdrscmfmac_fail_curr_config_neg
(
  hdrhai_session_close_reason_enum_type close_reason
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_IS_ATTRIB_GAUPABLE

DESCRIPTION
  This function is used by SCM module to find out if an attribute is GAUPable.

DEPENDENCIES
  None.

PARAMETERS
  attrib_id - Attribute ID.
  subtype - Protocol subtype.
  
RETURN VALUE
  E_SUCCESS       - If the attribute is GAUPable.
  E_NOT_ALLOWED   - If AN is not allowed to GAUP this attribute.

SIDE EFFECTS
  None.
===========================================================================*/
hdrerrno_enum_type hdrscmfmac_is_attrib_gaupable
(
  uint16 const attrib_id,
  uint16 const subtype
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_ATTRIB_IS_SUPPORTED

DESCRIPTION
  Is this a known attribute?

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
boolean hdrscmfmac_attrib_is_supported
(
  uint16 const attrib_id,
  uint16 const subtype
);


 /* EJECT */
/*===========================================================================
FUNCTION HDRSCMFMAC_CONFIG_IS_NEEDED                                 EXTERNAL

DESCRIPTION
  This function is used to find out if an AT-initiated Configuration is 
  needed by FMAC2. 

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
boolean hdrscmfmac_config_is_needed( void );


 /* EJECT */
/*===========================================================================
FUNCTION HDRSCMFMAC_CONFIGURE                                        EXTERNAL

DESCRIPTION
  This function does the AT-init configuration of FMAC2 attributes. 

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
boolean hdrscmfmac_configure( void );


#ifdef FEATURE_HDR_REVC
/*===========================================================================
 
FUNCTION HDRSCMFMAC_GAUP_LOAD_INFO_SUPPORTED

DESCRIPTION
  This function updates LoadInformationSupported attribute. 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS       - If a GAUP Update was sent successfully.
  E_NOT_AVAILABLE - If we didn't have a free Rsp timer or free msg buffer
                    to send this GAUP update.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmfmac_gaup_load_info_supported( void );


/*===========================================================================
 
FUNCTION HDRSCMFMAC_SET_LOAD_INFO_ATTRIB_REJECTED

DESCRIPTION
  This function resets the indicator which shows whether
  LoadInformationSupported attribute is rejected by the current
  serving sector. 

DEPENDENCIES
  None.

PARAMETERS
  load_info_attrib_rejected - TRUE, if LoadInformationSupported attribute
                              proposed has been rejected by the current serving
                              sector. FALSE, if not.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrscmfmac_set_load_info_attrib_rejected
(
  boolean load_info_attrib_rejected 
);

/*===========================================================================
 
FUNCTION HDRSCMFMAC_IS_LOAD_INFO_ATTRIB_REJECTED

DESCRIPTION
  This function tells whether LoadInformationSupported attribute has been
  rejected by the current serving sector. 

DEPENDENCIES
  None.

PARAMETERS
  None.
 
RETURN VALUE
  TRUE  - LoadInformationSupported attribute request has been
          rejected by the current serving sector.
  FALSE - LoadInformationSupported attribute request has not
          been rejected by the current serving sector yet.

SIDE EFFECTS
  None.
 
===========================================================================*/
boolean hdrscmfmac_is_load_info_attrib_rejected( void );


/*=========================================================================== 

FUNCTION HDRSCMFMAC_RESET_LOAD_INFO_GAUP_CONDITIONS

DESCRIPTION
  Reset the variables which control the updating of
  LoadInformationSupported attribute.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void hdrscmfmac_reset_load_info_gaup_conditions( void );

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_GET_GAUPED_ATTRIB_VALUE

DESCRIPTION
  This function is used by SCM to get the attribute value proposed by
  AT as GAUP update.

DEPENDENCIES
  None.

PARAMETERS
  trans_id      - Transaction ID.
  a_aip         - Array of Accepted attribute information where the info about
                  each of the attributes in the transaction specified by 
                  trans_id are stored.
  attrib_count  - Number of attributes GAUPed in this transaction.
  subtype       - Subtype negotiated for FMAC.
 
RETURN VALUE
  E_SUCCESS       - If the given transaction ID is valid.
  E_NOT_SUPPORTED - If the current subtype is not allowed to receive any GAUP
                    update response message. Currently FMAC only GAUPs
                    LoadInfoSupported and only when RevC is enabled.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmfmac_get_gauped_attrib_value
(
  uint8 const trans_id,
  dsm_item_type **aai_dsm,
  uint8 *const p_attrib_count,
  uint16 const subtype
);
#endif /* FEATURE_HDR_REVC */

#endif /* HDRSCMFMAC_H */

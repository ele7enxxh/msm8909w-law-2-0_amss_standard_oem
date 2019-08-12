#ifndef HDRSCMAMAC_H
#define HDRSCMAMAC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        H D R   S E S S I O N  C O N F I G U R A T I O N  M A N A G E R 

           A C C E S S   C H A N N E L   M A C   P R O T O C O L
                                    
                            H E A D E R   F I L E       

GENERAL DESCRIPTION
  This module contains the configuration interface of the HDR Access Channel
  (MAC) Protocol.

EXTERNALIZED FUNCTIONS

  hdrscmamac_is_attrib_simple 
    Returns true if an attribute is simple

  hdrscmamac_is_simple_attrib_valid
    Verifies the validity of the simple attribute

  hdrscmamac_get_attrib_offset_and_size
    Returns attribute size and offset into the config structure.

  hdrscmamac_get_attrib_verno_index
    Returns the index of an attribute in the version number array. 

INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2001-2008 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrscmamac.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/22/08   pba     Klockwork fixes.
08/28/07   kss     Added hdrscmamac_config_is_needed().
04/27/07   kss     Added PNRandomizationDelaySupported.
01/03/07   etv     Added support to handle unknown attributes.
06/08/06   etv     Send session close if AN GAUPs a non-GAUPable attribute.
11/27/05   etv     Created Module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/
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
#define HDRSCMAMAC_AR_MSG_PRIO            40
#define HDRSCMAMAC_NUM_COMPLEX_ATTRIBS    2 
#define HDRSCMAMAC_NUM_SIMPLE_ATTRIBS     8
#define HDRSCMAMAC_NUM_CONFIG_ATTRIBS     (HDRSCMAMAC_NUM_SIMPLE_ATTRIBS + \
                                           HDRSCMAMAC_NUM_COMPLEX_ATTRIBS)

#define HDRSCMAMAC_NUM_GAUPABLE_ATTRIBS   3

#define HDRSCMAMAC_PN_DELAY_SUPPORT_VERNO_OFFSET      0
#define HDRSCMAMAC_AT_CLASS_OVERRIDE_VERNO_OFFSET     1
#define HDRSCMAMAC_TERM_ACCESS_RATE_MAX_VERNO_OFFSET  6
#define HDRSCMAMAC_AP_OVERRIDE_VERNO_OFFSET           7

/* Enum defining the IDs of all configuration attributes of AMAC */
typedef enum
{
  /* Simple attributes (Only in Enhanced subtype) */
  HDRSCMAMAC_PN_RANDOMIZATION_DELAY_SUPPORTED_ATTRIB = 0xF8,
  HDRSCMAMAC_FIRST_SIMPLE_ATTRIB                     = 
    HDRSCMAMAC_PN_RANDOMIZATION_DELAY_SUPPORTED_ATTRIB,
  HDRSCMAMAC_AT_CLASS_OVERRIDE_ATTRIB                = 0xF9,
  HDRSCMAMAC_DATAOFFSET_38K4_ATTRIB                  = 0xFA,
  HDRSCMAMAC_DATAOFFSET_19K2_ATTRIB                  = 0xFB,
  HDRSCMAMAC_DATAOFFSET_9K6_ATTRIB                   = 0xFC,
  HDRSCMAMAC_DATAOFFSET_NOM_ATTRIB                   = 0xFD,
  HDRSCMAMAC_TERM_ACCESS_RATE_MAX_ATTRIB             = 0xFE,
  HDRSCMAMAC_APERSISTENCE_OVERRIDE_ATTRIB            = 0xFF,
  HDRSCMAMAC_LAST_SIMPLE_ATTRIB                      = 
    HDRSCMAMAC_APERSISTENCE_OVERRIDE_ATTRIB,

  /* Complex attributes */
  HDRSCMAMAC_INITIAL_CONFIG_ATTRIB            = 0x00,

  /* Only in Default subtype */
  HDRSCMAMAC_POWER_PARAMS_ATTRIB              = 0x01
} hdrscmamac_attrib_id_enum_type;

/* Enum defining the available modes for APersistenceOverride values */
typedef enum
{
  HDRSCMAMAC_AP_OVERRIDE_NORMAL_MODE,
  HDRSCMAMAC_AP_OVERRIDE_AGGRESSIVE_MODE,
  HDRSCMAMAC_NUM_AP_OVERRIDE_MODES
} hdrscmamac_ap_override_mode_enum_type;

/* Initial Configuration Attribute type */
typedef struct
{

  uint8 probe_seq_max;
    /* Maximum number of probe sequneces for a single access attempt.
       Range: (1-15); Default: 3; */

  uint8 probe_backoff;
    /* Inter-probe backoff. The upper limit of the backoff range in units of
       AccessCycleDuration that the AT uses between probes. Default: 4 */

  uint8 probe_seq_backoff;
    /* Inter-probe sequence backoff. The upper limit of the backoff range in 
       units of AccessCycleDuration that the AT uses between probe sequences.
       Default: 8 */

} hdrscmamac_initial_config_attrib_type;

/* Power Parameters Attribute - Only for Default subtype */
typedef struct
{
  uint8 data_offset_nom;
    /* Nominal offset of data channel power to pilot channel power expressed
       as a 2s complement value in units of 0.5 dB. */

  uint8 data_offset_9k6;
    /* Ratio of access channel power at 9600 bps to the nominal access channel 
       power at 9600 bps, expressed as a 2's complement in units of 0.25 dB. */

} hdrscmamac_power_params_attrib_type;

/* Struct encapsulating all configuration attributes for AMAC */
typedef struct
{
  uint8 pn_delay_supported;
    /* PNRandomizationDelaySupported: 1 = Supported, 0 = Not supported. 
       AttribID: 0xF8; Range: (0x00-0x01); Default: 0x00 */

  uint8 at_class_override;
    /* AccessTerminalClassOverride: Array index of APersistence in AP message.
       AttribID: 0xF9; Range: {0xFF, (0x00-0x03)}; Default: 0xFF; */

  uint8 data_offset_38k4;
    /* DataOffset38k4: Ratio of access channel power at 38400bps to the nominal
       access channel power at 38400 bps expressed as a 2s complement number.
       AttribID: 0xFA; Range: {(0x00-0x07), (0xF8-0xFF)}; Default: 0x00;
       Units: 0.25dB */

  uint8 data_offset_19k2;
    /* DataOffset19k2: Ratio of access channel power at 19200bps to the nominal
       access channel power at 19200 bps expressed as a 2s complement number.
       AttribID: 0xFB; Range: {(0x00-0x07), (0xF8-0xFF)}; Default: 0x00;
       Units: 0.25dB */

  uint8 data_offset_9k6;
    /* DataOffset9k6: Ratio of access channel power at 9600 bps to the nominal 
       access channel power at 9600 bps expressed as a 2s complement number.
       AttribId: 0xFC; Range: {(0x00-0x07), (0xF8-0xFF)}; Default: 0x00;
       Units: 0.25dB */

  uint8 data_offset_nom;
    /* DataOffsetNom: Nominal offset of the access data channel power to the 
       pilot channel power. AttribID: 0xFD; Range: {(0x00-0x07), (0xF8-0xFF)}; 
       Default: 0x00; Units: 0.25dB */

  uint8 term_acc_rate_max;
    /* Terminal Access Rate Max: Maximum rate at which an AT is allowed to xmit
       access probes. AttribID: 0xFE; Range: {0x00-0x02}; Default: 0x00 */

  uint8 ap_override;
    /* Apersistence Override - Persistence probability. AttribID: 0xFF; 
       Range: {0xFF, 0x3F, (0x00-0x3E)}; Default: 0xFF */

  hdrscmamac_initial_config_attrib_type init_config;
    /* InitialConfiguration Attribute. AttribID: 0x00; */

  hdrscmamac_power_params_attrib_type power_params;
    /* PowerParameters attribute. AttribID: 0x01; */

} hdrscmamac_config_struct_type;


/* Version number array for configuration attributes */
typedef uint8 hdrscmamac_verno_type[HDRSCMAMAC_NUM_CONFIG_ATTRIBS];


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_PROCESS_COMPLEX_ATTRIB

DESCRIPTION
  This is a generic interface to process all complex attributes given below.


DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - The pointer points to the accepted attribute information
             struct that gets filled in after the complex attribute is parsed.

RETURN VALUE
  E_SUCCESS      - If a valid complex attribute was parsed successfully.
  E_NO_DATA      - If the config request message does not have enough data.
  E_OUT_OF_RANGE - If the config request message does not have any valid data.
  E_DATA_INVALID - If the given attrib_id is not valid.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmamac_process_complex_attrib
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  uint16 subtype
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_IS_ATTRIB_SIMPLE

DESCRIPTION
  This function is used by SCM to query if an attribute is simple, given the
  attribute ID.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id - Attribute ID 
  subtype   - Subtype negotiated for AMAC 

RETURN VALUE
  TRUE - if the attribute given by 'attrib_id' is simple.
  FALSE - otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrscmamac_is_attrib_simple
(
  uint16 const attrib_id,
  uint16 const subtype
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_IS_SIMPLE_ATTRIB_VALID

DESCRIPTION
  This function is used by SCM to query if the given 'attrib_value' is a 
  valid value for the attribute specified by 'attrib_id'.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id    - Attribute ID of the simple attribute.
  attrib_value - Value of the simple attribute. 
  subtype      - Subtype Negotiated

RETURN VALUE
  E_SUCCESS          - If the attrib_value is valid.
  E_OUT_OF_RANGE     - Otherwise.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern hdrerrno_enum_type hdrscmamac_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value,
  uint16 const subtype
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_GET_ATTRIB_OFFSET_AND_SIZE

DESCRIPTION
  This function computes the byte offset of the attribute specified by the 
  attrib_id in the struct hdrscmamac_config_struct_type and stores it in 
  out-param attrib_offset. It also stores the size of the attribute in 
  attrib_size.

DEPENDENCIES
  None

PARAMETERS
  attrib_id     - Attribute Id.
  attrib_offset - Address where the Byte Offset from the start of the struct 
                  to the attribute is stored.
  attrib_size   - Address where the size of the attribute is stored.
  
RETURN VALUE
  E_SUCCESS if the given attrib ID is valid.
  E_DATA_INVALID if the attrib ID is not valid

SIDE EFFECTS
  None

===========================================================================*/
extern hdrerrno_enum_type hdrscmamac_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_GET_ATTRIB_VERNO_INDEX

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
extern hdrerrno_enum_type hdrscmamac_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *index,
  uint16 subtype
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  Set the configuration attributes to its default values.

DEPENDENCIES
  None

PARAMETERS
  hdrcmac_cfg - Pointer to the config structure.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void hdrscmamac_set_config_to_default
(
  hdrscmamac_config_struct_type *hdramac_cfg
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC0_PROP_GAUP_RATE_PARAMS

DESCRIPTION
  This routine is used by Enhanced AMAC to send GAUP update request for 
  APersistenceOverride attribute.

DEPENDENCIES
  None.

PARAMETERS
  mode - ApersistenceOverride Mode 
  p_trans_id  - Address where the transaction ID is stored to be returned back.
  
RETURN VALUE
  E_SUCCESS - If a GAUP Update was sent successfully.
  E_NOT_AVAILABLE - If we didn't have a free Rsp timer to send this GAUP update.

SIDE EFFECTS
  None.
===========================================================================*/
hdrerrno_enum_type hdrscmamac_gaup_ap_override
(
  hdrscmamac_ap_override_mode_enum_type ap_mode,
  uint8 *p_trans_id
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_IS_ATTRIB_GAUPABLE

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
hdrerrno_enum_type hdrscmamac_is_attrib_gaupable
(
  uint16 const attrib_id,
  uint16 const subtype
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_ATTRIB_IS_SUPPORTED

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
boolean hdrscmamac_attrib_is_supported
(
  uint16 const attrib_id,
  uint16 const subtype
);


/*===========================================================================

FUNCTION HDRSCMAMAC_PROP_PN_DELAY_SUPPORTED

DESCRIPTION
  This function is used to propose PNDelaySupported attribute as 
  part of AT-init session configuration.

DEPENDENCIES
  None

PARAMETERS
  None.
   
RETURN VALUE
  TRUE - If the a Rsp timer was available for use.
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrscmamac_prop_pn_delay_supported( void );

/*===========================================================================

FUNCTION HDRSCMAMAC_FAIL_CURR_CONFIG_NEG

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
extern void hdrscmamac_fail_curr_config_neg
(
  hdrhai_session_close_reason_enum_type close_reason
);

/* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMAC_GET_PROP_ATTRIB_VALUE

DESCRIPTION
  The function is used by SCM to check the proposed attribute value.

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
extern hdrerrno_enum_type hdrscmamac_get_prop_attrib_value
(
  uint16 const trans_id,
  hdrscm_accepted_attrib_type *a_aip,
  uint16 const subtype
);

/* EJECT */
/*===========================================================================
FUNCTION HDRSCMAMAC_CONFIG_IS_NEEDED                                EXTERNAL

DESCRIPTION
  This function is used to find out if an AT-initiated Configuration is 
  needed by Enh. ACMAC.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE  - If AT-init config is required.
  FALSE - Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean hdrscmamac_config_is_needed( void );

#endif /* HDRSCMAMAC_H */

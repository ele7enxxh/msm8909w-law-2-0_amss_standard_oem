#ifndef HDRSCMAMP_H
#define HDRSCMAMP_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    H D R   S E S S I O N  C O N F I G U R A T I O N  M A N A G E R 

         A D D R E S S   M A N A G E M E N T   P R O T O C O L
                                    
                        H E A D E R   F I L E       

GENERAL DESCRIPTION
  This module contains the configuration interface of the Address Manangement
  Protocol (AMP).

EXTERNALIZED FUNCTIONS

  hdrscmamp_process_complex_attrib 
    Processes all AMP complex attributes 
    
  hdrscmamp_is_attrib_simple 
    Returns true if an attribute is simple

  hdrscmamp_is_simple_attrib_valid
    Verifies the validity of the simple attribute

  hdrscmamp_get_attrib_offset_and_size
    Returns attribute size and offset into the config structure.

  hdrscmamp_get_attrib_verno_index
    Returns the index of an attribute in the version number array. 

INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2006 - 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrscmamp.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/11   cnx     Support Optimized Handoff phase 3.
01/03/07   etv     Added support to handle unknown attributes.
09/14/06   hal     Added GAUP support for MaxNoMonitorDistance
07/31/06   pba     Added SupportSecondaryColorCode attribute.
02/23/06   hal     Created module

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "hdr_variation.h"
#include "hdrscm.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#define HDRSCMAMP_MAX_NUM_SIMPLE_ATTRIBS       0x06
#define HDRSCMAMP_MAX_NUM_CONFIGURABLE_ATTRIBS 0x06
#define HDRSCMAMP_MAX_SIMPLE_ATTRIB_ID         0xFF

#define HDRSCMAMP_MAX_NO_MONITOR_DISTANCE_ATTRIB_SIZE                   16

#define HDRSCMAMP_MAX_NO_MONITOR_DISTANCE_ATTRIB_DEFAULT              0x0000
#define HDRSCMAMP_HARDWARE_SEPARABLE_FROM_SESSION_ATTRIB_DEFAULT      0x00
#define HDRSCMAMP_SUPPORT_GAUP_MAX_NO_MONITOR_DISTANCE_ATTRIB_DEFAULT 0x00
#define HDRSCMAMP_REDUCED_SUBNET_MASK_OFFSET_ATTRIB_DEFAULT           0x00
#define HDRSCMAMP_SUPPORT_SECONDARY_COLORCODES_ATTRIB_DEFAULT         0x00
#define HDRSCMAMP_INTER_RAT_MOBILITY_ENABLED_ATTRIB_DEFAULT           0x00

typedef enum
{
  HDRSCMAMP_INTER_RAT_MOBILITY_ENABLED_ATTRIB_ID = 0xFA,
  HDRSCMAMP_FIRST_ATTRIB_ID = 
	HDRSCMAMP_INTER_RAT_MOBILITY_ENABLED_ATTRIB_ID,
  HDRSCMAMP_SUPPORT_SECONDARY_COLORCODES_ATTRIB_ID = 0xFB,
  HDRSCMAMP_FIRST_NON_IRAT_ATTRIB_ID = 
	HDRSCMAMP_SUPPORT_SECONDARY_COLORCODES_ATTRIB_ID,
  HDRSCMAMP_REDUCED_SUBNET_MASK_OFFSET_ATTRIB_ID = 0xFC,
  HDRSCMAMP_SUPPORT_GAUP_MAX_NO_MONITOR_DISTANCE_ATTRIB_ID = 0xFD,
  HDRSCMAMP_HARDWARE_SEPARABLE_FROM_SESSION_ATTRIB_ID = 0xFE,
  HDRSCMAMP_MAX_NO_MONITOR_DISTANCE_ATTRIB_ID = 0xFF,
  HDRSCMAMP_LAST_ATTRIB_ID = 
    HDRSCMAMP_MAX_NO_MONITOR_DISTANCE_ATTRIB_ID
} hdrscmamp_attrib_id_enum_type;

/* AMP Configurable Parameters Type */
typedef struct
{
  /* MaxNoMonitorDistance simple attribute */
  uint16 max_no_monitor_distance;

  /* HardwareSeparableFromSession simple attribute */
  uint8 hardware_separable_from_session;

  /* SupportGAUPMaxNoMonitorDistance simple attribute */
  uint8 support_gaup_max_no_monitor_distance;

  /* ReducedSubnetMask simple attribute */
  uint8 reduced_subnet_mask_offset;

  /* SupportSecondaryColorCodes simple attibute */
  uint8 support_secondary_colorcodes;

#ifdef FEATURE_LTE_TO_HDR_OH
  /* InterRATMobilityEnabled simple attibute */
  uint8 inter_rat_mobility_enabled;
#endif /* FEATURE_LTE_TO_HDR_OH */

} hdrscmamp_config_params_type;

typedef uint8 hdrscmamp_verno_type[HDRSCMAMP_MAX_NUM_CONFIGURABLE_ATTRIBS];

/*==========================================================================

                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRSCMAMP_PROCESS_COMPLEX_ATTRIB

DESCRIPTION
  This is a generic interface to process all complex attributes given below.

  < No complex attributes listed in AMP >
  
DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  E_SUCCESS - If a valid complex attribute was parsed successfully.
  E_FAILURE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrscmamp_process_complex_attrib
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  uint16                        const          subtype
);

/*===========================================================================

FUNCTION HDRSCMAMP_IS_ATTRIB_SIMPLE

DESCRIPTION
  This function is used by SCM to query if an attribute is simple, given the
  attribute ID.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id - Attribute ID 

RETURN VALUE
  TRUE - if the attribute given by 'attrib_id' is simple.
  FALSE - otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrscmamp_is_attrib_simple
(
  uint16 const attrib_id,
  uint16 const subtype
);

/*===========================================================================

FUNCTION HDRSCMAMP_IS_SIMPLE_ATTRIB_VALID

DESCRIPTION
  This function is used by SCM to query if the given 'attrib_value' is a 
  valid value for the attribute specified by 'attrib_id'.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id    - Attribute ID of the simple attribute.
  attrib_value - Value of the simple attribute. 

RETURN VALUE
  E_SUCCESS          - If the attrib_value is valid.
  E_OUT_OF_RANGE     - Otherwise.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern hdrerrno_enum_type hdrscmamp_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value,
  uint16 const subtype
);

/*===========================================================================

FUNCTION HDRSCMAMP_GET_ATTRIB_OFFSET_AND_SIZE

DESCRIPTION
  This function computes the byte offset of the attribute specified by the 
  attrib_id in the struct hdrscmamp_config_params_type and stores it in 
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
extern hdrerrno_enum_type hdrscmamp_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
);

/*===========================================================================

FUNCTION HDRSCMAMP_GET_ATTRIB_VERNO_INDEX

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
extern hdrerrno_enum_type hdrscmamp_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *index,
  uint16 subtype
);

/*===========================================================================

FUNCTION HDRSCMAMP_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  Set the configuration attributes to its default values.

DEPENDENCIES
  None

PARAMETERS
  amp_cfg_ptr - Pointer to the config structure.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrscmamp_set_config_to_default
( 
  hdrscmamp_config_params_type *amp_cfg_ptr
);

/*===========================================================================

FUNCTION HDRSCMAMP_IS_ATTRIB_GAUPABLE

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
extern hdrerrno_enum_type hdrscmamp_is_attrib_gaupable
(
  uint16 const attrib_id,
  uint16 const subtype
);

/* EJECT */
/*===========================================================================

FUNCTION HDRSCMAMP_ATTRIB_IS_SUPPORTED

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
extern boolean hdrscmamp_attrib_is_supported
(
  uint16 const attrib_id,
  uint16 const subtype
);

#endif /* HDRSCMAMP_H */


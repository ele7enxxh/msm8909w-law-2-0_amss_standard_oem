#ifndef HDRSCMCMAC_H
#define HDRSCMCMAC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        H D R   S E S S I O N  C O N F I G U R A T I O N  M A N A G E R 

           C O N T R O L   C H A N N E L   M A C   P R O T O C O L
                                    
                            H E A D E R   F I L E       

GENERAL DESCRIPTION
  This module contains the configuration interface of the HDR Control Channel
  (MAC) Protocol.

EXTERNALIZED FUNCTIONS

  hdrscmcmac_is_attrib_simple 
    Returns true if an attribute is simple

  hdrscmcmac_is_simple_attrib_valid
    Verifies the validity of the simple attribute

  hdrscmcmac_get_attrib_offset_and_size
    Returns attribute size and offset into the config structure.

  hdrscmcmac_get_attrib_verno_index
    Returns the index of an attribute in the version number array. 

INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/inc/hdrscmcmac.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/03/07   etv     Added support to handle unknown attributes.
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

/* Struct encapsulating all configuration attributes for CMAC */
typedef struct
{

  uint8 sht_pkt_mac_index;
    /* Preamble MAC index for short packet xmission on the control channel */ 

} hdrscmcmac_config_struct_type;


/* Version number array for configuration attributes */
typedef uint8 hdrscmcmac_verno_type[1];


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMCMAC_IS_ATTRIB_SIMPLE

DESCRIPTION
  This function is used by SCM to query if an attribute is simple, given the
  attribute ID.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id - Attribute ID 
  subtype   - Subtype negotiated for CMAC 

RETURN VALUE
  TRUE - if the attribute given by 'attrib_id' is simple.
  FALSE - otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrscmcmac_is_attrib_simple
(
  uint16 const attrib_id,
  uint16 const subtype
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMCMAC_IS_SIMPLE_ATTRIB_VALID

DESCRIPTION
  This function is used by SCM to query if the given 'attrib_value' is a 
  valid value for the attribute specified by 'attrib_id'.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id    - Attribute ID of the simple attribute.
  attrib_value - Value of the simple attribute. 
  subtype      - Subtype Negotiated for CMAC

RETURN VALUE
  E_SUCCESS          - If the attrib_value is valid.
  E_OUT_OF_RANGE     - Otherwise.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern hdrerrno_enum_type hdrscmcmac_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value,
  uint16 const subtype
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMCMAC_GET_ATTRIB_OFFSET_AND_SIZE

DESCRIPTION
  This function computes the byte offset of the attribute specified by the 
  attrib_id in the struct hdrscmcmac_config_struct_type and stores it in 
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
extern hdrerrno_enum_type hdrscmcmac_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMCMAC_GET_ATTRIB_VERNO_INDEX

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
extern hdrerrno_enum_type hdrscmcmac_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *index,
  uint16 subtype
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMCMAC_SET_CONFIG_TO_DEFAULT

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
void hdrscmcmac_set_config_to_default
(
  hdrscmcmac_config_struct_type *hdrcmac_cfg
);

/* EJECT */
/*===========================================================================

FUNCTION HDRSCMCMAC_ATTRIB_IS_SUPPORTED

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
boolean hdrscmcmac_attrib_is_supported
(
  uint16 const attrib_id,
  uint16 const subtype
);

#endif /* HDRSCMCMAC_H */

#ifndef HDRSCMOVHD_H
#define HDRSCMOVHD_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

           O V E R H E A D  M E S S A G E S   P R O T O C O L

                        H E A D E R  F I L E
       
GENERAL DESCRIPTION
  This module provides supporting API for SCM module to process
  ConfigurationRequest messages directed to Overhead Messages protocol.
    
EXTERNALIZED FUNCTIONS
    
  hdrscmovhd_is_attrib_simple 
    Returns true if an attribute is simple

  hdrscmovhd_is_simple_attrib_valid
    Verifies the validity of the simple attribute

  hdrscmovhd_get_attrib_offset_and_size
    Returns attribute size and offset into the config structure.

  hdrscmovhd_get_attrib_verno_index
    Returns the index of an attribute in the version number array. 
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

  
  Copyright (c) 2005 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/29/11   pba     EV-DO Rev C overhead caching support.
12/23/08   pba     Support for Quick Idle Protocol
01/03/07   etv     Added support to handle unknown attributes.
06/21/06   etv     Created hdrscmovhd module to support OMP config attribs.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "dsm.h"
#include "comdef.h"
#include "err.h"

#include "hdrdebug.h"
#include "hdrdsm.h"
#include "hdrscp.h"
#include "hdrhai.h"
#include "hdrtrace.h"

#include "hdrscmovhd.h"
#include "hdrscmdb.h"
#include "hdrscm.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
/* Number of configuration attributes in overhead mesesages protocol  */
#define HDRSCMOVHD_NUM_CONFIG_ATTRIBS                 4

typedef enum
{
  HDRSCMOVHD_FIRST_ATTRIB_ID = 0xFC,
  /* IsSupervisionDisablingWhileIdlewithCacheSupported */
  HDRSCMOVHD_IS_SUPERVISION_DISABLING_SUPPORTED = 
      HDRSCMOVHD_FIRST_ATTRIB_ID,
  /* ShortTermCachePeriod */
  HDRSCMOVHD_SHORTTERM_CACHE_PERIOD_ATTRIB_ID = 0xFD,
  /* RouteUpdateTriggerCodeListSize */
  HDRSCMOVHD_RUT_CODE_LIST_SIZE_ATTRIB_ID = 0xFE,
  /* LongTermOverheadCachePeriod */
  HDRSCMOVHD_LONGTERM_CACHE_PERIOD_ATTRIB_ID = 0xFF,
  HDRSCMOVHD_LAST_ATTRIB_ID = HDRSCMOVHD_LONGTERM_CACHE_PERIOD_ATTRIB_ID

} hdrscmovhd_attrib_id_enum_type;


/*===========================================================================
 
                        DECLARATIONS FOR MODULE

  This section contains macros, types, and other items needed by this module.

===========================================================================*/

/* Struct encapsulating all the configuration attributes for Overhead Messages 
   protocol */
typedef struct
{
  uint16 longterm_ovhd_cache_period;
    /* LongTermOverheadCachePeriod; Default Value: 0xFF; Valid Range: 0x00-0xFF */

  uint16 shortterm_ovhd_cache_period;
    /* ShortTermOverheadCachePeriod; Default Value: 0x00; Valid Range: 0x00-0xFF */

  uint8 rut_code_list_size; 
    /* RouteUpdateTriggerCodeListSize; Default: 0x00; Valid Range: 0x00-0x05 */

  uint8 is_supervision_disabling_supported;
    /* IsSupervisionDisablingWhileIdlewithCacheSupported; Default: 0x00; 
       Valid Range: 0x00-0x01 */
    
} hdrscmovhd_config_struct_type;

/* Version number array for the configuration attributes */
typedef uint8 hdrscmovhd_verno_type[HDRSCMOVHD_NUM_CONFIG_ATTRIBS];

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMOVHD_IS_ATTRIB_SIMPLE

DESCRIPTION
  This function is used by SCM to query if an attribute is simple, given the
  attribute ID.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id - Attribute ID 
  subtype   - Subtype negotiated for OVHD

RETURN VALUE
  TRUE - if the attribute given by 'attrib_id' is simple.
  FALSE - otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrscmovhd_is_attrib_simple
(
  uint16 const attrib_id,
  uint16 const subtype
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMOVHD_IS_SIMPLE_ATTRIB_VALID

DESCRIPTION
  This function is used by SCM to query if the given 'attrib_value' is a 
  valid value for the attribute specified by 'attrib_id'.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id    - Attribute ID of the simple attribute.
  attrib_value - Value of the simple attribute. 
  subtype      - Subtype negotiated for CMAC

RETURN VALUE
  E_SUCCESS          - If the attrib_value is valid.
  E_OUT_OF_RANGE     - Otherwise.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern hdrerrno_enum_type hdrscmovhd_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value,
  uint16 const subtype

);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMOVHD_GET_ATTRIB_OFFSET_AND_SIZE

DESCRIPTION
  This function computes the byte offset of the attribute specified by the 
  attrib_id in the struct hdrscmovhd_config_struct_type and stores it in 
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
extern hdrerrno_enum_type hdrscmovhd_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMOVHD_GET_ATTRIB_VERNO_INDEX

DESCRIPTION
  Returns the index of the given attribute in the version number array.

DEPENDENCIES
  None

PARAMETERS
  attrib_id     - Attribute Id.
  
RETURN VALUE
  E_SUCCESS - If the given attribute ID is valid.
  E_DATA_INVALID - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern hdrerrno_enum_type hdrscmovhd_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *index,
  uint16 subtype
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMOVHD_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  Set the configuration attributes to its default values.

DEPENDENCIES
  None

PARAMETERS
  hdrovhd_cfg - Pointer to the config structure.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrscmovhd_set_config_to_default
(
  hdrscmovhd_config_struct_type *hdrovhd_cfg
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMOVHD_ATTRIB_IS_SUPPORTED

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
boolean hdrscmovhd_attrib_is_supported
(
  uint16 const attrib_id,
  uint16 const subtype
);

#endif /* HDRSCMOVHD_H */

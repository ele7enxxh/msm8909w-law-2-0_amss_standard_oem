#ifndef HDRSCMALMP_H
#define HDRSCMALMP_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    H D R   S E S S I O N  C O N F I G U R A T I O N  M A N A G E R 

         A I R   L I N K   M A N A G E M E N T   P R O T O C O L
                                    
                        H E A D E R   F I L E       

GENERAL DESCRIPTION
  This module contains the configuration interface of the Air Link 
  Management Protocol (ALMP).

EXTERNALIZED FUNCTIONS

  hdrscmalmp_process_complex_attrib 
    Processes all ALMP complex attributes 
    
  hdrscmalmp_is_attrib_simple 
    Returns true if an attribute is simple

  hdrscmalmp_is_simple_attrib_valid
    Verifies the validity of the simple attribute

  hdrscmalmp_get_attrib_offset_and_size
    Returns attribute size and offset into the config structure.

  hdrscmalmp_get_attrib_verno_index
    Returns the index of an attribute in the version number array. 

INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2005 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrscmalmp.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/03/07   etv     Added support to handle unknown attributes.
11/21/05   etv     Added header tags for perforce to fill in
11/02/05   etv     Changed SCM design to propogate subtype info.
10/18/05   hal     Created module

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "hdrscm.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#define HDRSCMALMP_MAX_NUM_SIMPLE_ATTRIBS       0x01
#define HDRSCMALMP_MAX_NUM_CONFIGURABLE_ATTRIBS 0x01
#define HDRSCMALMP_MAX_SIMPLE_ATTRIB_ID         0x00ff

#define HDRSCMALMP_CONN_FAILURE_REPORTING_ENABLED_ATTRIB_ID      0xff
#define HDRSCMALMP_CONN_FAILURE_REPORTING_ENABLED_ATTRIB_DEFAULT 0x00

/* ALMP Configurable Parameters Type */
typedef struct
{
  /* ConnectionFailureReportingEnabled simple attribute */
  uint8 conn_failure_reporting_enabled;

} hdrscmalmp_config_params_type;

/* There is only one configurable attribute in ALMP */
typedef uint8 hdrscmalmp_verno_type[HDRSCMALMP_MAX_NUM_CONFIGURABLE_ATTRIBS];

/*==========================================================================

                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRSCMALMP_PROCESS_COMPLEX_ATTRIB

DESCRIPTION
  This is a generic interface to process all complex attributes given below.

  < No complex attributes listed in ALMP >
  
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
extern hdrerrno_enum_type hdrscmalmp_process_complex_attrib
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  uint16                        const          subtype
);

/*===========================================================================

FUNCTION HDRSCMALMP_IS_ATTRIB_SIMPLE

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
extern boolean hdrscmalmp_is_attrib_simple
(
  uint16 const attrib_id,
  uint16 const subtype
);

/*===========================================================================

FUNCTION HDRSCMALMP_IS_SIMPLE_ATTRIB_VALID

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
extern hdrerrno_enum_type hdrscmalmp_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value,
  uint16 const subtype
);

/*===========================================================================

FUNCTION HDRSCMALMP_GET_ATTRIB_OFFSET_AND_SIZE

DESCRIPTION
  This function computes the byte offset of the attribute specified by the 
  attrib_id in the struct hdrscmalmp_config_params_type and stores it in 
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
extern hdrerrno_enum_type hdrscmalmp_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
);

/*===========================================================================

FUNCTION HDRSCMALMP_GET_ATTRIB_VERNO_INDEX

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
extern hdrerrno_enum_type hdrscmalmp_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *index,
  uint16 subtype
);

/*===========================================================================

FUNCTION HDRSCMALMP_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  Set the configuration attributes to its default values.

DEPENDENCIES
  None

PARAMETERS
  hdralmp_cfg - Pointer to the config structure.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrscmalmp_set_config_to_default
( 
  hdrscmalmp_config_params_type *almp_cfg_ptr
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMALMP_ATTRIB_IS_SUPPORTED

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
boolean hdrscmalmp_attrib_is_supported
(
  uint16 const attrib_id,
  uint16 const subtype
);

#endif /* HDRSCMALMP_H */

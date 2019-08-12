
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

             B R O A D C A S T   C O N T R O L  P R O T O C O L
       
GENERAL DESCRIPTION
  This module provides supporting API for SCM module to process
  ConfigurationRequest messages and AttributeUpdateRequest messages
  directed to BroadCast Control protocol. 
    
EXTERNALIZED FUNCTIONS
  hdrscmbccp_is_attrib_simple 
    Returns true if an attribute is simple

  hdrscmbccp_is_simple_attrib_valid
    Verifies the validity of the simple attribute

  hdrscmbccp_get_attrib_offset_and_size
    Returns attribute size and offset into the config structure.

  hdrscmbccp_get_attrib_verno_index
    Returns the index of an attribute in the version number array. 
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

  
  Copyright (c) 2005 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/inc/hdrscmbccp.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------
01/03/07   etv     Added support to handle unknown attributes.
11/21/05   etv     Added header tags for perforce to fill in
11/02/05   etv     Changed SCM design to propogate subtype info.
10/21/05   pba     Added Broadcast protocol to SCM
09/26/05   pba     Created module.

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

#include "hdrscmdb.h"
#include "hdrscm.h"

/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/

#define HDRSCMBCCP_MAX_NUM_CONFIG_ATTRIBS 5
                                                                     
typedef struct
{
  uint8 bcast_overhead_period;
  uint8 paging_reg_period;
  uint8 dyn_bc_reg_period;
  uint8 monitor_period;
  uint8 num_monitor_period;
  /* _mpa_ hdrbcsec attributes */
} hdrscmbccp_config_params_type;

typedef uint8 hdrscmbccp_verno_type[HDRSCMBCCP_MAX_NUM_CONFIG_ATTRIBS];

/*===========================================================================

                         FUNCTION DECLERATIONS
                    
===========================================================================*/

/*===========================================================================

FUNCTION HDRSCMBCCP_IS_ATTRIB_SIMPLE 

DESCRIPTION
  This function is used by SCM to query if an attribute is simple, given the
  attribute ID.

DEPENDENCIES
  None.

PARAMETERS
  attrib_id - Attribute ID 
  subtype   - Subtype negotiated.

RETURN VALUE
  TRUE - if the attribute given by 'attrib_id' is simple.
  FALSE - otherwise.
  
SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrscmbccp_is_attrib_simple
( 
  uint16 const attrib_id,
  uint16 const subtype
);

/*===========================================================================

FUNCTION HDRSCMBCCP_IS_SIMPLE_ATTRIB_VALID 

DESCRIPTION
  This function is used by SCM to query if the given 'attrib_value' is a 
  valid value for the attribute specified by 'attrib_id'.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id    - Attribute ID of the simple attribute.
  attrib_value - Value of the simple attribute. 
  subtype      - Subtype negotiated

RETURN VALUE
  E_SUCCESS          - If the attrib_value is valid.
  E_OUT_OF_RANGE     - Otherwise.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern hdrerrno_enum_type hdrscmbccp_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value,
  uint16 const subtype
);

/*===========================================================================

FUNCTION HDRSCMBCCP_GET_ATTRIB_OFFSET_AND_SIZE 

DESCRIPTION
  This function computes the byte offset of the attribute specified by the 
  attrib_id in the struct hdrscmbccp_config_params_type and stores it in 
  out-param attrib_offset. It also stores the size of the attribute in 
  attrib_size.

DEPENDENCIES
  None.

PARAMETERS
  attrib_id     - Attribute Id.
  attrib_offset - Address where the Byte Offset from the start of the struct 
                  to the attribute is stored.
  attrib_size   - Address where the size of the attribute is stored.
  subtype       - Subtype negotiated

RETURN VALUE
  E_SUCCESS if the given attrib ID is valid.
  E_DATA_INVALID if the attrib ID is not valid
  
SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrscmbccp_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
);

/*===========================================================================

FUNCTION HDRSCMBCCP_GET_ATTRIB_VERNO_INDEX 

DESCRIPTION
  Returns the index of the given attribute in the version number array.

DEPENDENCIES
  None

PARAMETERS
  attrib_id     - Attribute Id.
  subtype       - Subtype negotiated
  
RETURN VALUE
  E_SUCCESS - If the given attribute ID is valid.

SIDE EFFECTS
  None

===========================================================================*/
extern hdrerrno_enum_type hdrscmbccp_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *index,
  uint16 subtype
);

/*=============================================================================
FUNCTION HDRSCMBCCP_SET_CONFIG_PARAMS_DEFAULT

DESCRIPTION
  This function initializes the Broadcast Protocol's configurable parameters 
  to their default value
  
DEPENDENCIES
  None.

PARAMETERS
  current_config - Pointer to Broadcast protocol's current configuration
                   structure in SCMDB.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
extern void hdrscmbccp_set_config_params_default 
( 
  hdrscmbccp_config_params_type *current_config
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMBCCP_ATTRIB_IS_SUPPORTED

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
boolean hdrscmbccp_attrib_is_supported
(
  uint16 const attrib_id,
  uint16 const subtype
);


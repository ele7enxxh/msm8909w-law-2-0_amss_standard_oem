#ifndef TRM_XLATE_H
#define TRM_XLATE_H
/*===========================================================================

                   T R M    S T R U C T   H E A D E R    F I L E

DESCRIPTION
   This file contains the declaration of TRM data structure.

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/trm/src/trm_xlate.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/12/14   mn      Support for a unified IRAT mechanism which works across all 
                    TRM modes (CR: 705286).
04/11/14   mn      Refactoring TRM

===========================================================================*/

#include "customer.h"
#include "trmi.h"

/*===========================================================================

                              HEADER CONSTANTS

  Numeric values for the identifiers in this section should not be changed.

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*===========================================================================

                              FUNCTIONS

===========================================================================*/

/*============================================================================

Member Function trm_get_group_from_chain

Description
  Helper function :This method is used to find out the chain group that needs
  to be assigned using the chain that has been requested.

Dependencies
  None

Return Value
  None

Side Effects
  None

============================================================================*/
trm_group trm_get_group_from_chain( trm_chain_enum chain );

/*============================================================================

Member Function trm_get_chain_from_group

Description
  Helper function to convert group to chain.

Dependencies
  None

Return Value
  None

Side Effects
  None

============================================================================*/
trm_chain_enum trm_get_chain_from_group( trm_group group );

/*============================================================================

Member Function trm_chain_enum_to_index

Description
  Convert chain to index.

Dependencies
  None

Return Value
  None

SIDE EFFECTS
  None

============================================================================*/
uint32 trm_chain_enum_to_index( trm_chain_enum  chain );

/*============================================================================

Member Function trm_index_to_chain_enum

Description
  Convert chain to index.

Dependencies
  None

Return Value
  None

SIDE EFFECTS
  None

============================================================================*/
trm_chain_enum trm_index_to_chain_enum( trm_chain_offset_type  index );

/*============================================================================

Member Function TRM_MAP_CHAIN_TYPE_TO_CHAIN

Description
  Gets the chain offset for corresponding chain enum type. This is
  needed so that we wouldnt need to re-work logging and have different
  parsers for NikeL and Dime.

Dependencies 
  None. 


Return Value
  Chain Offset.

Side Effects
  None.

============================================================================*/
uint32 trm_map_chain_type_to_chain( const trm_chain_enum chain );

/*============================================================================
FUNCTION TRM_GET_GRANT_EVENT_FROM_CHAIN

DESCRIPTION
  Converts the trm_chain_enum to trm_grant_event_enum_t

DEPENDENCIES
  None

RETURN VALUE
  Grant event corresponding to the input chain.
  TRM_DENIED when conversion is not possible.

SIDE EFFECTS
  None
============================================================================*/
trm_grant_event_enum_t trm_get_grant_event_from_chain( trm_chain_enum chain );

/*============================================================================

FUNCTION TRM_GET_CHAIN_FOR_DEVICE

DESCRIPTION
  Returns the trm chain number for corresponding rf device.

DEPENDENCIES
  None

RETURN VALUE
  rfm_device_enum_type

SIDE EFFECTS
  None

============================================================================*/
trm_chain_enum trm_get_chain_for_device
(
  rfm_device_enum_type device
);

/*============================================================================

FUNCTION TRM_GET_DEVICE_FOR_CHAIN

DESCRIPTION
  Returns the rf device corresponding to a trm chain number.
  
DEPENDENCIES
  None

RETURN VALUE
  rfm_device_enum_type

SIDE EFFECTS
  None

============================================================================*/
rfm_device_enum_type trm_get_device_for_chain
(
  uint32  chain_info   /* Chain number for which RF device is required */
);

/*============================================================================
FUNCTION TRM_GET_CHAIN_FROM_CHAIN_OFFSET

DESCRIPTION
  Helper function to get the chain enum type from the offset.
  
DEPENDENCIES
  NONE

RETURN VALUE
  Chain Enum Type for the chain offset.

SIDE EFFECTS
  None

============================================================================*/
trm_chain_enum trm_get_chain_from_chain_offset(trm_chain_offset_type chainoffset);

/*============================================================================
FUNCTION TRM_GET_GRANT_EVENT_FROM_RF_DEVICE

DESCRIPTION
  Converts the rfm_device_enum_type to trm_grant_event_enum_t

DEPENDENCIES
  None

RETURN VALUE
  Grant event corresponding to the input chain.
  TRM_DENIED when conversion is not possible.

SIDE EFFECTS
  None
============================================================================*/
trm_grant_event_enum_t trm_get_grant_event_from_rf_device( rfm_device_enum_type chain );

/*============================================================================

FUNCTION TRM_GET_SECONDARY_CLIENT

DESCRIPTION
  This is an internal API that can be used to get the tech's secondary
  client id

DEPENDENCIES
  None

RETURN VALUE
  secondary client's TRM id

SIDE EFFECTS
  None

============================================================================*/
trm_client_enum_t trm_get_secondary_client(trm_client_enum_t c_id);

/*============================================================================

FUNCTION TRM_GET_RF_DEVICE_FROM_GROUP

DESCRIPTION
  This is an internal API that can be used to convert a group to an RF device 

DEPENDENCIES
  None

RETURN VALUE
  RF device. RFM_INVALID device when group is INVALID.

SIDE EFFECTS
  None

============================================================================*/
rfm_device_enum_type trm_get_rf_device_from_group(trm_group c_id);

/*============================================================================

FUNCTION TRM_GET_CHAIN_FROM_RF_DEVICE

DESCRIPTION
  Translates rfm_device_enum_type to trm_chain_enum. 

DEPENDENCIES
  None

RETURN VALUE
  A valid chain if mapping is possible.
  TRM_CHAIN_MAX if rf device is INVALID.

SIDE EFFECTS
  None

============================================================================*/
trm_chain_enum trm_get_chain_from_rf_device(rfm_device_enum_type rf_device);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TRM_XLATE_H */
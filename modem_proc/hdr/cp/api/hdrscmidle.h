#ifndef HDRSCMIDLE_H
#define HDRSCMIDLE_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

        I D L E  S T A T E   P R O T O C O L  H E A D E R  F I L E

       
GENERAL DESCRIPTION
  This module provides supporting API for SCM module to handle AT-init Session
  Negotiation for SupportedCDMAChannel Attribute of RouteUpdateProtocol. 
    
EXTERNALIZED FUNCTIONS
    
  hdrscmidle_is_attrib_simple 

  hdrscmidle_get_attrib_offset_and_size

  hdrscmidle_gaup_slotted_mode_attrib
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

  
  Copyright (c) 2005,2006 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/23/11   smd     SU API cleanup.
04/24/08   pba     AT initiated GAUP support for SCI7.
05/22/07   sju     Added support for phase2 of AT-init GAUP of SlottedMode attribute. 
01/17/07   etv     Included the pre-compiler directive for the header file.
01/04/07   dna/sju Fixed GAUPing SlottedMode attribute for QChat App.
05/15/06   etv     Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/

#include "hdr_variation.h"
#include "dsm.h"
#include "comdef.h"
#include "err.h"

#include "hdridlec.h"
#include "hdrscm.h"
#include "hdrscmdb.h"
#include "hdrcp_api.h"


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMIDLE_IS_ATTRIB_SIMPLE

DESCRIPTION
  To find if the attribute given by the attrib_id is simple. 
  
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
extern boolean hdrscmidle_is_attrib_simple
(
  uint16 const attrib_id,
  uint16 const subtype
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMIDLE_GAUP_SLOTTED_MODE_ATTRIB

DESCRIPTION
  This function is used go GAUP slotted Mode attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  sm_override -  What type of override to start:
                 HDRSCMIDLE_SM_OVERRIDE_NONE
                 HDRSCMIDLE_SM_OVERRIDE_426_MSECS
                 HDRSCMIDLE_SM_OVERRIDE_213_MSECS

  p_trans_id - Address where the transaction ID is stored to be returned back.

RETURN VALUE
  E_SUCCESS - If a GAUP Update was queued to HDRMC successfully.
  E_NOT_AVAILABLE - If we didn't have a free Rsp timer to send this GAUP update.
  E_FAILURE - If attempting to turn off override when no stored config is 
              available i.e. no override was done previously.
              AT couldn't retrieve its current slotted mode config.
              sm_override out of range.

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrscmidle_gaup_slotted_mode_attrib
(
  hdrscmidle_sm_override_enum_type sm_override,
  uint8 *p_trans_id
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMIDLE_GET_GAUPED_COMPLEX_ATTRIB_VALUE

DESCRIPTION
  This function is used by SCM to get the complex attribute value proposed by
  AT as GAUP update.

DEPENDENCIES
  None.

PARAMETERS
  trans_id      - Transaction ID.
  a_aip         - Array of attribute information pointers where the info about
                  each of the attributes in the transaction specified by 
                  trans_id are stored.
  attrib_count  - Number of attributes GAUPed in this transaction.
 
RETURN VALUE
  E_SUCCESS       - If the given transaction ID is valid.
  E_NOT_AVAILABLE - If a valid transaction specified by 'trans_id' is not 
                    available.

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrscmidle_get_gauped_complex_attrib_value
(
  uint8 const trans_id,
  dsm_item_type **aai_dsm,
  uint8 *const attrib_count,
  uint16 subtype
);

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMIDLE_GET_ATTRIB_OFFSET_AND_SIZE

DESCRIPTION
  To get the attribute's byte offset into the config struct and its size.

DEPENDENCIES
  None

PARAMETERS
  attrib_id     - Attribute Id.
  attrib_offset - Address where the Byte Offset from the start of the struct 
                  to the attribute is stored.
  attrib_size   - Address where the size of the attribute is stored.
  subtype       - subtype.
  
RETURN VALUE
  E_SUCCESS - If the given attrib ID is valid.
  E_DATA_INVALID - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern hdrerrno_enum_type hdrscmidle_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
);

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMIDLE_INIT_saved_scmidle_INFO

DESCRIPTION
  This function is used to initiate saved slottedmode information. 
  
DEPENDENCIES
  None.

PARAMETERS
  saved_scmidle_info_ptr - pointer to the saved sm info

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrscmidle_init_scmidle_info ( hdridlec_saved_scmidle_info_type *saved_scmidle_info_ptr );

/*===========================================================================

FUNCTION HDRSCMIDLE_SET_saved_scmidle_INFO_VALID

DESCRIPTION
  This function is used to set if hdrscmidle.saved_scmidle_info.saved_slotted_mode  
  is valid.  
  
DEPENDENCIES
  None.

PARAMETERS
  saved_scmidle_info_is_valid - if saved slottedmode info is valid 

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrscmidle_set_scmidle_info_valid ( boolean saved_scmidle_info_is_valid );

/*===========================================================================

FUNCTION HDRSCMIDLE_saved_scmidle_INFO_IS_VALID

DESCRIPTION
  This function returns if hdrscmidle.saved_scmidle_info.saved_slotted_mode  
  is valid.  
  
DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrscmidle_saved_scmidle_info_is_valid ( void );


#ifdef FEATURE_HDR_REVC
/*===========================================================================
 
FUNCTION HDRSCMIDLE_GAUP_MULTI_AT_PAGE_SUPPORTED

DESCRIPTION
  This function sends MultiATPageMsgSupported attribute update request. 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS       - Update request was sent successfully.
  E_NOT_AVAILABLE - If we didn't have a free Rsp timer or free msg buffer
                    to send this GAUP update.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmidle_gaup_multi_at_page_supported( void );

/*=========================================================================== 

FUNCTION HDRSCMIDLE_RESET_MULTI_AT_PAGE_GAUP_CONDITIONS

DESCRIPTION
  Reset the variables which determine the conditions of triggering
  MultiATPageMessageSupported attribute update request.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void hdrscmidle_reset_multi_at_page_gaup_conditions( void );

/*=========================================================================== 

FUNCTION HDRSCMIDLE_IS_MULTI_AT_PAGE_GAUP

DESCRIPTION
  This function checks whether a MultiATPageMessageSupported update
  request/response based on the GAUP transcation ID.

DEPENDENCIES
  None

PARAMETERS
  trans_id - GAUP transcation ID.

RETURN VALUE
  TRUE - a MultiATPageMessageSupported update request/response
  FALSE - not a MultiATPageMessageSupported update request/response

SIDE EFFECTS
  None
 
===========================================================================*/
boolean hdrscmidle_is_multi_at_page_gaup
(   
  uint8 const trans_id
);
#endif /* FEATURE_HDR_REVC*/
#endif /* HDRSCMIDLE_H */

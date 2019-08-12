
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

           C O N T R O L  C H A N N E L  M A C  P R O T O C O L

       
GENERAL DESCRIPTION
  This module provides supporting API for SCM module to process
  ConfigurationRequest messages directed to Control channel MAC protocol. 
    
EXTERNALIZED FUNCTIONS
  hdrscmcmac_process_complex_attrib 
    Processes all CMAC complex attributes 
    
  hdrscmcmac_is_attrib_simple 
    Returns true if an attribute is simple

  hdrscmcmac_is_simple_attrib_valid
    Verifies the validity of the simple attribute

  hdrscmcmac_get_attrib_offset_and_size
    Returns attribute size and offset into the config structure.

  hdrscmcmac_get_attrib_verno_index
    Returns the index of an attribute in the version number array. 
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

  
  Copyright (c) 2005 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrscmcmac.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/03/07   etv     Added support to handle unknown attributes.
01/31/06   etv     Removed ASSERTs for graceful degradation.
11/27/05   etv     Created Module.

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

#include "hdrscmcmac.h"
#include "hdrscmdb.h"
#include "hdrscm.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

#define HDRSCMCMAC_CC_SHT_PKT_MAC_INDEX_ATTRIB_ID       0x00

/*------------------------------------------------------------------
              Default Values for Configuration Attributes
-------------------------------------------------------------------*/

#define HDRSCMCMAC_DEFAULT_CC_SHT_PKT_MAC_INDEX         0x00


/*===========================================================================
 
                  DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/


/*------------------------------------------------------------------------
                            GLOBAL VARIABLES
 -----------------------------------------------------------------------*/

/*===========================================================================

                         LOCAL FUNCTION DEFINITIONS
                    
===========================================================================*/


 /* EJECT */
/*============================================================================

                        EXTERNAL FUNCTION DEFINITIONS

=============================================================================*/


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
boolean hdrscmcmac_is_attrib_simple
(
  uint16 const attrib_id,
  uint16 const subtype
)
{
  boolean is_simple = FALSE;
    /* Is the attribute simple? */
/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* There are no configuration attributes in Default CCMAC and we should
     never get here if the negotiated subtype if default */
  if (subtype == HDRSCP_DEFAULT_CCMAC)
  {
    /* By returning FALSE for this scenario, this would be treated as an 
       unknown attribute by SCM. */
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                   "Err: Got CReq for attrib 0x%x for Def CMAC", attrib_id);
  }
  else
  {
    is_simple = (attrib_id == HDRSCMCMAC_CC_SHT_PKT_MAC_INDEX_ATTRIB_ID);
  }

  return is_simple;

} /* hdrscmcmac_is_attrib_simple */


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
  subtype      - Subtype negotiated for CMAC

RETURN VALUE
  E_SUCCESS          - If the attrib_value is valid.
  E_OUT_OF_RANGE     - Otherwise.

SIDE EFFECTS
  None.
  
===========================================================================*/
hdrerrno_enum_type hdrscmcmac_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value,
  uint16 const subtype

)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error code */

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* There are no configuration attributes in Default CCMAC and we should
     never get here if the negotiated subtype if default */
  if (subtype == HDRSCP_DEFAULT_CCMAC)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                   "Err: Got CReq for attrib 0x%x for Def CMAC", attrib_id);
    err_no = E_DATA_INVALID;
  }
  else
  {
    err_no = hdrscmcmac_is_attrib_simple(attrib_id, subtype)? 
      E_SUCCESS: E_OUT_OF_RANGE;

    if (err_no == E_SUCCESS)
    {
      err_no = ( (attrib_value == 0x00) ||
                 ((attrib_value >= 0x06) && (attrib_value <= 0x3F)) ||
                 ((attrib_value >= 0x48) && (attrib_value <= 0x7F))
               )? E_SUCCESS: E_OUT_OF_RANGE;
    }
  }

  return err_no;

} /* hdrscmcmac_is_simple_attrib_valid */


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
hdrerrno_enum_type hdrscmcmac_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (attrib_id)
  {
    case HDRSCMCMAC_CC_SHT_PKT_MAC_INDEX_ATTRIB_ID:
      *attrib_offset = FPOS(hdrscmcmac_config_struct_type, sht_pkt_mac_index);
      *attrib_size = 0x01;
      break;

    default:
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,"Invalid attribute for CMAC %d", attrib_id);
      err_no = E_DATA_INVALID;
  }

  return err_no;

} /* hdrscmcmac_get_attrib_offset_and_size */


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
hdrerrno_enum_type hdrscmcmac_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *index,
  uint16 subtype
)
{
    hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (attrib_id)
  {
    case HDRSCMCMAC_CC_SHT_PKT_MAC_INDEX_ATTRIB_ID:
      *index = 0;
      break;

    default:
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,"Invalid attribute for CMAC %d", attrib_id);
      err_no = E_DATA_INVALID;
  }

  return err_no;

} /* hdrscmcmac_get_attrib_verno_index */


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
)
{

  /* Set Short packet MAC index to its default value */
  hdrcmac_cfg->sht_pkt_mac_index = HDRSCMCMAC_DEFAULT_CC_SHT_PKT_MAC_INDEX;
  
} /* hdrscmcmac_set_config_to_default */


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
)
{
  boolean known = FALSE;

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(subtype)
  {
    case HDRSCP_ENH_CCMAC:
      if (attrib_id == HDRSCMCMAC_CC_SHT_PKT_MAC_INDEX_ATTRIB_ID)
      {
        known = TRUE;
      }
      else
      {
        HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                       "Enh CMAC attribute: 0x%x not supported", attrib_id);
      }
      break;

    case HDRSCP_DEFAULT_CCMAC:
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "CMAC attribute: 0x%x not supported", attrib_id);
      break;

    default:
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "CMAC subtype 0x%x not supported", subtype);
  }

  return known; 

} /* hdrscmcmac_attrib_is_supported */


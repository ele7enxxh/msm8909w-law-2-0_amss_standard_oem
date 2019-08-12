

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

           O V E R H E A D  M E S S A G E S   P R O T O C O L

       
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
12/23/08   pba     Support for Quick Idle Protocol
10/17/07   pba     Updates based on code review comments.
01/03/07   etv     Added support to handle unknown attributes
                   Reject Rev-A default subtype attributes when Phy is not ST2
10/18/06   pba     Defined maximum RUTC list size in hdrhai.h 
09/20/06   hal     Fixed maximum RUTC List size
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
/*------------------------------------------------------------------
              Default Values for Configuration Attributes
-------------------------------------------------------------------*/
/* LongTermOverheadCachePeriod */
#define HDRSCMOVHD_DEFAULT_LONGTERM_OVHD_CACHE_PERIOD   0x00

/* ShortTermOverheadCachePeriod */
#define HDRSCMOVHD_DEFAULT_SHORTTERM_OVHD_CACHE_PERIOD  0x00

/* RouteUpdateTriggerCodeListSize */
#define HDRSCMOVHD_DEFAULT_RUT_CODE_LIST_SIZE       0x00

/* IsSupervisionDisablingSupported */
#define HDRSCMOVHD_DEFAULT_IS_SUPERVISION_DISABLING_SUPPORTED   0x00

/*------------------------------------------------------------------
              Valid Ranges for Configuration Attributes
-------------------------------------------------------------------*/
/* Max (Long or Short Term) OverheadCachePeriod */
#define HDRSCMOVHD_CACHE_PERIOD_MAX                 0xFF

/* Max RouteUpdateTriggerCodeListSize */
#define HDRSCMOVHD_RUT_CODE_LIST_SIZE_MAX           HDRHAI_RUTC_LIST_SIZE_MAX

#define HDRSCMOVHD_SUPERVISION_DISABLING_SUPPORTED    0x01

/* Index in the version number array for LTOverheadCachePeriod */
#define HDRSCMOVHD_LONGTERM_CACHE_PERIOD_VERNO_INDEX    0

/* Index in the version number array for RouteUpdateTriggerCodeListSize */
#define HDRSCMOVHD_RUT_CODE_LIST_SIZE_VERNO_INDEX   1

/* Index in the version number array for STOverheadCachePeriod */
#define HDRSCMOVHD_SHORTTERM_CACHE_PERIOD_VERNO_INDEX   2

/* Index in the version number array for IsSupervisionDisablingSupported */
#define HDRSCMOVHD_IS_SUPERVISION_DISABLING_SUPPORTED_VERNO_INDEX   3

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
boolean hdrscmovhd_is_attrib_simple
(
  uint16 const attrib_id,
  uint16 const subtype
)
{
  boolean is_simple = FALSE;
    /* Is the attribute simple? */

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (attrib_id)
  {
    /* Only simple attributes of overhead messages protocol */
    case HDRSCMOVHD_LONGTERM_CACHE_PERIOD_ATTRIB_ID:
    case HDRSCMOVHD_SHORTTERM_CACHE_PERIOD_ATTRIB_ID:
    case HDRSCMOVHD_RUT_CODE_LIST_SIZE_ATTRIB_ID:
    case HDRSCMOVHD_IS_SUPERVISION_DISABLING_SUPPORTED:
      is_simple = TRUE;
      break;

    default:
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
                     "Invalid attrib 0x%x for OVHD sub %d",
                     attrib_id,
                     subtype);

  } /* switch */

  return is_simple;

} /* hdrscmovhd_is_attrib_simple */


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
  E_OUT_OF_RANGE     - If the attrib_value is invalid.
  E_DATA_INVALID     - If the attribute ID is invalid.

SIDE EFFECTS
  None.
  
===========================================================================*/
hdrerrno_enum_type hdrscmovhd_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value,
  uint16 const subtype

)
{
  hdrerrno_enum_type err_no = E_OUT_OF_RANGE;
    /* Error code */

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (attrib_id)
  {
    case HDRSCMOVHD_LONGTERM_CACHE_PERIOD_ATTRIB_ID:
    case HDRSCMOVHD_SHORTTERM_CACHE_PERIOD_ATTRIB_ID:
      err_no = ((attrib_value <= HDRSCMOVHD_CACHE_PERIOD_MAX)? 
                E_SUCCESS: E_OUT_OF_RANGE);
      break;

    case HDRSCMOVHD_RUT_CODE_LIST_SIZE_ATTRIB_ID:
      err_no = ((attrib_value <= HDRSCMOVHD_RUT_CODE_LIST_SIZE_MAX)? 
                E_SUCCESS: E_OUT_OF_RANGE);
      break;

    case HDRSCMOVHD_IS_SUPERVISION_DISABLING_SUPPORTED:
        err_no = ((attrib_value <= HDRSCMOVHD_SUPERVISION_DISABLING_SUPPORTED)? 
                  E_SUCCESS: E_OUT_OF_RANGE);
        break;

    default:
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
                     "Invalid attrib 0x%x for OVHD sub %d",
                     attrib_id,
                     subtype);

      err_no = E_DATA_INVALID;

  } /* switch */

  return err_no;

} /* hdrscmovhd_is_simple_attrib_valid */


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
hdrerrno_enum_type hdrscmovhd_get_attrib_offset_and_size
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
    case HDRSCMOVHD_LONGTERM_CACHE_PERIOD_ATTRIB_ID:
      *attrib_offset = FPOS(hdrscmovhd_config_struct_type, longterm_ovhd_cache_period);
      *attrib_size = sizeof(uint8);
      break;

    case HDRSCMOVHD_SHORTTERM_CACHE_PERIOD_ATTRIB_ID:
      *attrib_offset = FPOS(hdrscmovhd_config_struct_type, shortterm_ovhd_cache_period);
      *attrib_size = sizeof(uint8);
      break;

    case HDRSCMOVHD_RUT_CODE_LIST_SIZE_ATTRIB_ID:
      *attrib_offset = FPOS(hdrscmovhd_config_struct_type, rut_code_list_size);
      *attrib_size = sizeof(uint8);
      break;

    case HDRSCMOVHD_IS_SUPERVISION_DISABLING_SUPPORTED:
      *attrib_offset = FPOS(hdrscmovhd_config_struct_type, is_supervision_disabling_supported);
      *attrib_size = sizeof(uint8);
      break;

    default:
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,"Invalid attribute for OVHD %d", attrib_id);
      err_no = E_DATA_INVALID;
  }

  return err_no;

} /* hdrscmovhd_get_attrib_offset_and_size */


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
hdrerrno_enum_type hdrscmovhd_get_attrib_verno_index
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
    case HDRSCMOVHD_LONGTERM_CACHE_PERIOD_ATTRIB_ID:
      *index = HDRSCMOVHD_LONGTERM_CACHE_PERIOD_VERNO_INDEX;
      break;
  
    case HDRSCMOVHD_SHORTTERM_CACHE_PERIOD_ATTRIB_ID:
      *index = HDRSCMOVHD_SHORTTERM_CACHE_PERIOD_VERNO_INDEX;
      break;

    case HDRSCMOVHD_RUT_CODE_LIST_SIZE_ATTRIB_ID:
      *index = HDRSCMOVHD_RUT_CODE_LIST_SIZE_VERNO_INDEX;
      break;

    case HDRSCMOVHD_IS_SUPERVISION_DISABLING_SUPPORTED:
      *index = HDRSCMOVHD_IS_SUPERVISION_DISABLING_SUPPORTED_VERNO_INDEX;
      break;

    default:
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR,
                     "Invalid attribute for OVHD %d", attrib_id);
      err_no = E_DATA_INVALID;
  }

  return err_no;

} /* hdrscmovhd_get_attrib_verno_index */


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
void hdrscmovhd_set_config_to_default
(
  hdrscmovhd_config_struct_type *hdrovhd_cfg
)
{

  /* Set LongTermOverheadCachePeriod to its default value */  
  hdrovhd_cfg->longterm_ovhd_cache_period =
      HDRSCMOVHD_DEFAULT_LONGTERM_OVHD_CACHE_PERIOD;

  /* Set ShortTermOverheadCachePeriod to its default value */  
  hdrovhd_cfg->shortterm_ovhd_cache_period = 
      HDRSCMOVHD_DEFAULT_SHORTTERM_OVHD_CACHE_PERIOD;

  /* Set RouteUpdateTriggerCodeListSize to its default value */
  hdrovhd_cfg->rut_code_list_size = HDRSCMOVHD_DEFAULT_RUT_CODE_LIST_SIZE;

  /* Set IsSupervisionDisablingSupported to its default value */
  hdrovhd_cfg->is_supervision_disabling_supported = 
      HDRSCMOVHD_DEFAULT_IS_SUPERVISION_DISABLING_SUPPORTED;

} /* hdrscmovhd_set_config_to_default */


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
)
{
  boolean is_supported = FALSE;

  if (!hdrscp_phone_is_set_to_rev0_mode())
  {
    is_supported =  ((attrib_id >= HDRSCMOVHD_FIRST_ATTRIB_ID) && 
                     (attrib_id <= HDRSCMOVHD_LAST_ATTRIB_ID));
  }

  return is_supported;
         
} /* hdrscmovhd_attrib_is_supported */

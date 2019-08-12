/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R
         
         A D D R E S S   M A N A G E M E N T   P R O T O C O L
         

GENERAL DESCRIPTION
  This module provides the supporting API to the SCM module to process any
  ConfigurationRequest or AttributeUpdateRequest (if supported) messages
  destined for the Address Manangement Protocol (AMP).


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

Copyright (c) 2006 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
                        
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrscmamp.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/14/12   cnx     Accept InterRATMobilityEnabled attr when IRAT NV is set.
11/08/11   cnx     Support Optimized Handoff phase 3.
12/10/08   smd     Replaced assert.h with amssassert.h
10/17/07   pba     Updates based on code review comments.
08/22/07   etv     Fixed high lint warnings.
01/03/07   etv     Added support to handle unknown attributes.
                   Reject Rev-A default subtype attributes when Phy is not ST2
09/14/06   hal     Added GAUP support for MaxNoMonitorDistance
07/31/06   pba     Added SupportSecondaryColorCode attribute.
02/23/06   hal     Created module

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "hdrscmamp.h"
#include "hdrscmdb.h"
#include "hdrscm.h"
#include "hdrscp.h"

#include "dsm.h"
#include "hdrdebug.h"
#include "hdrbit.h"
#include "err.h"
#include "amssassert.h"

#include "hdrerrno.h"

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* A entry in the range table of valid simple attributes {min, max} */
typedef struct
{
  uint8 min;
  uint8 max;

} hdrscmamp_simple_attrib_range_type;

/* Table of valid ranges {min, max} for each simple attribute */
static hdrscmamp_simple_attrib_range_type const 
  valid_ranges[HDRSCMAMP_MAX_NUM_SIMPLE_ATTRIBS] =
{
  {0xff, 0xff}, /* MaxNoMonitorDistance ID: 0xff - treated as a complex 
                   attribute because of a 16 bit value; this table entry 
                   is used only to maintain the validity of the 
                   successive indices                                   */
  {0x00, 0x01}, /* HardwareSeparableFromSession ID: 0xfe                */
  {0x00, 0x01}, /* SupportGAUPMaxNoMonitorDistance ID: 0xfd             */
  {0x00, 0x10}, /* ReducedSubnetMaskOffset ID: 0xfc                     */
  {0x00, 0x01}, /* SupportSecondaryColorCodes ID: 0xfb                  */
  {0x00, 0x01}  /* InterRATMobilityEnabled ID: 0xfa                     */
                /* Future attributes listed in descending order of ID;
                   YYY ID: 0xf9                                         */
};

/*==========================================================================

                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

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
hdrerrno_enum_type hdrscmamp_is_attrib_gaupable
(
  uint16 const attrib_id,
  uint16 const subtype
)
{
  hdrerrno_enum_type err_no = E_NOT_ALLOWED;

  /* Negotiated value of SupportGAUPMaxNoMonitorDistance */
  uint8 support_gaup_max_no_monitor_distance;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*//*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( attrib_id )
  {
    case HDRSCMAMP_MAX_NO_MONITOR_DISTANCE_ATTRIB_ID:
      /* Retreive attrib from SCM DB */
      (void) hdrscmdb_get_inuse_attrib( 
              HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, 
              HDRSCMAMP_SUPPORT_GAUP_MAX_NO_MONITOR_DISTANCE_ATTRIB_ID,
              &support_gaup_max_no_monitor_distance,
              subtype );
    
      /* If support for GAUP has been negotiated then process the message */
      if( support_gaup_max_no_monitor_distance == 0x01 )
      {
        err_no = E_SUCCESS;
      }
      break;

    default:
      break;
  }

  /* If attrib is GAUPable then err_no will have been updated to 
     E_SUCCESS; otherwise err_no defaults to E_NOT_ALLOWED */
  return err_no;

} /* hdrscmamp_is_attrib_gaupable() */

/*===========================================================================

FUNCTION HDRSCMAMP_PROCESS_MAX_NO_MONITOR_DISTANCE

DESCRIPTION
  This is a generic interface to process the MaxNoMonitorDistance simple 
  attribute. This attribute needs special handling because of its 16 bit
  value field.
  
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
static hdrerrno_enum_type hdrscmamp_process_max_no_monitor_distance
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{
  /* Offset to the current byte being processed from the start of DSM chain */
  uint16 attrib_offset;

  /* 16 bit value of the attribute */    
  uint16 value;

  /* Status code */
  hdrerrno_enum_type err = E_FAILURE;

  /* Negotiated value of SupportGAUPMaxNoMonitorDistance */
  uint8 support_gaup_max_no_monitor_distance; 
    
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Retreive attrib from SCM DB */
  (void) hdrscmdb_get_inuse_attrib( 
          HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, 
          HDRSCMAMP_SUPPORT_GAUP_MAX_NO_MONITOR_DISTANCE_ATTRIB_ID,
          &support_gaup_max_no_monitor_distance,
          HDRSCP_DEFAULT_AMP );

  /* If the config req is a GAUP and support for GAUP has not 
     been negotiated then drop the message */
  if( p_aip->is_gaup && support_gaup_max_no_monitor_distance != 0x01 )
  {
    err = E_FAILURE;
    return err;
  }
    
  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  while( (attrib_offset + 
          HDRSCMAMP_MAX_NO_MONITOR_DISTANCE_ATTRIB_SIZE) <= p_aip->attrib_len )
  {
    err = HDRBIT_UNPACK16( item_ptr, 
                           &value, 
                           p_aip->attrib_offset + attrib_offset, 
                           HDRSCMAMP_MAX_NO_MONITOR_DISTANCE_ATTRIB_SIZE );

    if( err == E_SUCCESS )
    {
      a_aip->complex_val_ptr = NULL;
      a_aip->value_id = value;
      a_aip->sattr_val_size = HDRSCMAMP_MAX_NO_MONITOR_DISTANCE_ATTRIB_SIZE;

      a_aip->is_accepted = TRUE;
      break;
    }
    
    
    attrib_offset += HDRSCMAMP_MAX_NO_MONITOR_DISTANCE_ATTRIB_SIZE;
  }

  return err;

} /* hdrscmamp_process_max_no_monitor_distance() */

/*===========================================================================

FUNCTION HDRSCMAMP_PROCESS_COMPLEX_ATTRIB

DESCRIPTION
  This is a generic interface to process all complex attributes given below.

  < No complex attributes listed in AMP; however, MaxNoMonitorDistance has
    a value field of 16 bits and must be handled independently from the
    rest of the simple attributes >
  
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
hdrerrno_enum_type hdrscmamp_process_complex_attrib
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  uint16                        const          subtype
)
{
  /* Status code */
  hdrerrno_enum_type err = E_SUCCESS;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( p_aip->attrib_id )
  {
    case HDRSCMAMP_MAX_NO_MONITOR_DISTANCE_ATTRIB_ID:
      err = hdrscmamp_process_max_no_monitor_distance( item_ptr, p_aip, a_aip );
      break;

    default:
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, "Invalid attrib ID %d ignored", 
                      p_aip->attrib_id );
      err = E_FAILURE;
      break;
  }
  
  return err;

} /* hdrscmamp_process_complex_attrib() */

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
boolean hdrscmamp_is_attrib_simple
(
  uint16 const attrib_id,
  uint16 const subtype
)
{
  boolean attrib_is_simple;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( attrib_id )
  {
    case HDRSCMAMP_HARDWARE_SEPARABLE_FROM_SESSION_ATTRIB_ID:
    case HDRSCMAMP_SUPPORT_GAUP_MAX_NO_MONITOR_DISTANCE_ATTRIB_ID:
    case HDRSCMAMP_REDUCED_SUBNET_MASK_OFFSET_ATTRIB_ID:
    case HDRSCMAMP_SUPPORT_SECONDARY_COLORCODES_ATTRIB_ID:
#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRSCMAMP_INTER_RAT_MOBILITY_ENABLED_ATTRIB_ID:
#endif /* FEATURE_LTE_TO_HDR_OH */
      attrib_is_simple = TRUE;
      break;

    /* MaxNoMonitorDistance needs to be handled as a complex attribute due
       to a value size of 16 bits */
    case HDRSCMAMP_MAX_NO_MONITOR_DISTANCE_ATTRIB_ID:
      attrib_is_simple = FALSE;
      break;

    default:
      attrib_is_simple = FALSE;
      break;
  }

  return attrib_is_simple;

} /* hdrscmamp_is_attrib_simple() */

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
hdrerrno_enum_type hdrscmamp_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value,
  uint16 const subtype
)
{

  hdrerrno_enum_type err_code = E_OUT_OF_RANGE;

  /* Calculate the index into the range table by taking the
     difference of the MAX simple attrib ID and the current
     attrib ID. Example:
       index = MAX_ATTRIB_ID (0xff) - CURRENT_ID (0xfe) = 0x01 */
  int index = (uint16) ( HDRSCMAMP_MAX_SIMPLE_ATTRIB_ID - attrib_id );

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Error */
  if( index >= HDRSCMAMP_MAX_NUM_SIMPLE_ATTRIBS )
  {
    return E_OUT_OF_RANGE;
  }

  switch( attrib_id )
  {
    case HDRSCMAMP_HARDWARE_SEPARABLE_FROM_SESSION_ATTRIB_ID:
    case HDRSCMAMP_SUPPORT_GAUP_MAX_NO_MONITOR_DISTANCE_ATTRIB_ID:
    case HDRSCMAMP_REDUCED_SUBNET_MASK_OFFSET_ATTRIB_ID:
    case HDRSCMAMP_SUPPORT_SECONDARY_COLORCODES_ATTRIB_ID:
#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRSCMAMP_INTER_RAT_MOBILITY_ENABLED_ATTRIB_ID:
#endif /* FEATURE_LTE_TO_HDR_OH */
      if( attrib_value >= valid_ranges[index].min &&
          attrib_value <= valid_ranges[index].max )
      {
        err_code = E_SUCCESS;
      }

      break;

    /* This case should never be reached; MaxNoMonitorDistance is handled
       by the process_complex_attrib() function */
    case HDRSCMAMP_MAX_NO_MONITOR_DISTANCE_ATTRIB_ID:
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
        "Parsing MaxNoMonitorDistance (id: 0x%x) as simple attrib", 
        HDRSCMAMP_MAX_NO_MONITOR_DISTANCE_ATTRIB_ID );
      break;

    default:
      err_code = E_OUT_OF_RANGE;
      break;
  }

  return err_code;

} /* hdrscmamp_is_simple_attrib_valid() */

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
hdrerrno_enum_type hdrscmamp_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
)
{

  hdrerrno_enum_type err_code = E_SUCCESS;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( attrib_id )
  {
    case HDRSCMAMP_MAX_NO_MONITOR_DISTANCE_ATTRIB_ID:
      *attrib_offset = FPOS( hdrscmamp_config_params_type, 
                             max_no_monitor_distance );

      *attrib_size   = FSIZ( hdrscmamp_config_params_type,
                             max_no_monitor_distance );
      break;

    case HDRSCMAMP_HARDWARE_SEPARABLE_FROM_SESSION_ATTRIB_ID:
      *attrib_offset = FPOS( hdrscmamp_config_params_type, 
                             hardware_separable_from_session );

      *attrib_size   = FSIZ( hdrscmamp_config_params_type,
                             hardware_separable_from_session );
      break;

    case HDRSCMAMP_SUPPORT_GAUP_MAX_NO_MONITOR_DISTANCE_ATTRIB_ID:
      *attrib_offset = FPOS( hdrscmamp_config_params_type, 
                             support_gaup_max_no_monitor_distance );

      *attrib_size   = FSIZ( hdrscmamp_config_params_type,
                             support_gaup_max_no_monitor_distance );
      break;

    case HDRSCMAMP_REDUCED_SUBNET_MASK_OFFSET_ATTRIB_ID:
      *attrib_offset = FPOS( hdrscmamp_config_params_type, 
                             reduced_subnet_mask_offset );

      *attrib_size   = FSIZ( hdrscmamp_config_params_type,
                             reduced_subnet_mask_offset );
      break;

    case HDRSCMAMP_SUPPORT_SECONDARY_COLORCODES_ATTRIB_ID:
      *attrib_offset = FPOS( hdrscmamp_config_params_type, 
                             support_secondary_colorcodes );

      *attrib_size   = FSIZ( hdrscmamp_config_params_type,
                             support_secondary_colorcodes );
      break;

#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRSCMAMP_INTER_RAT_MOBILITY_ENABLED_ATTRIB_ID:
      *attrib_offset = FPOS( hdrscmamp_config_params_type, 
                             inter_rat_mobility_enabled );

      *attrib_size   = FSIZ( hdrscmamp_config_params_type,
                             inter_rat_mobility_enabled );
      break;
#endif /* FEATURE_LTE_TO_HDR_OH */

    default:
      err_code = E_DATA_INVALID;
      break;
  }

  return err_code;

} /* hdrscmamp_get_attrib_offset_and_size() */

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
hdrerrno_enum_type hdrscmamp_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *index,
  uint16 subtype
)
{

  hdrerrno_enum_type err_code = E_FAILURE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( attrib_id )
  {
    case HDRSCMAMP_MAX_NO_MONITOR_DISTANCE_ATTRIB_ID:
    case HDRSCMAMP_HARDWARE_SEPARABLE_FROM_SESSION_ATTRIB_ID:
    case HDRSCMAMP_SUPPORT_GAUP_MAX_NO_MONITOR_DISTANCE_ATTRIB_ID:
    case HDRSCMAMP_REDUCED_SUBNET_MASK_OFFSET_ATTRIB_ID:
    case HDRSCMAMP_SUPPORT_SECONDARY_COLORCODES_ATTRIB_ID:
#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRSCMAMP_INTER_RAT_MOBILITY_ENABLED_ATTRIB_ID:
#endif /* FEATURE_LTE_TO_HDR_OH */
      *index = (uint16) ( HDRSCMAMP_MAX_SIMPLE_ATTRIB_ID - attrib_id );

      err_code = E_SUCCESS;
      break;

    default:

      break;
  }

  return err_code;

} /* hdrscmamp_get_attrib_verno_index() */

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
void hdrscmamp_set_config_to_default
( 
  hdrscmamp_config_params_type *amp_cfg_ptr
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set attribute defaults */

  amp_cfg_ptr->max_no_monitor_distance =
    HDRSCMAMP_MAX_NO_MONITOR_DISTANCE_ATTRIB_DEFAULT;;

  amp_cfg_ptr->hardware_separable_from_session = 
    HDRSCMAMP_HARDWARE_SEPARABLE_FROM_SESSION_ATTRIB_DEFAULT;

  amp_cfg_ptr->support_gaup_max_no_monitor_distance =
    HDRSCMAMP_SUPPORT_GAUP_MAX_NO_MONITOR_DISTANCE_ATTRIB_DEFAULT;

  amp_cfg_ptr->reduced_subnet_mask_offset = 
    HDRSCMAMP_REDUCED_SUBNET_MASK_OFFSET_ATTRIB_DEFAULT;

  amp_cfg_ptr->support_secondary_colorcodes = 
    HDRSCMAMP_SUPPORT_SECONDARY_COLORCODES_ATTRIB_DEFAULT;

#ifdef FEATURE_LTE_TO_HDR_OH
  amp_cfg_ptr->inter_rat_mobility_enabled = 
    HDRSCMAMP_INTER_RAT_MOBILITY_ENABLED_ATTRIB_DEFAULT;
#endif /* FEATURE_LTE_TO_HDR_OH */

} /* hdrscmamp_set_config_to_default() */


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
boolean hdrscmamp_attrib_is_supported
(
  uint16 const attrib_id,
  uint16 const subtype
)
{

  boolean is_supported = FALSE;

  if ( hdrscp_phone_is_set_to_rev0_mode() )
  {
    is_supported = FALSE;
  }
#ifdef FEATURE_LTE_TO_HDR_OH
  else if ( hdrscp_is_irat_enabled() )
  {
    is_supported =  ((attrib_id >= HDRSCMAMP_FIRST_ATTRIB_ID) && 
                     (attrib_id <= HDRSCMAMP_LAST_ATTRIB_ID));
  }
#endif /* FEATURE_LTE_TO_HDR_OH */
  else
  {
    /* If irat subtypes are not negotiated, INTER_RAT_MOBILITY_ENABLED
     * attribute is not supported. */
    is_supported =  ((attrib_id >= HDRSCMAMP_FIRST_NON_IRAT_ATTRIB_ID) && 
                     (attrib_id <= HDRSCMAMP_LAST_ATTRIB_ID));
  }

  return is_supported;

} /* hdrscmamp_attrib_is_supported */

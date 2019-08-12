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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrscmbccp.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/29/07   sju     Ignored BCMCS attributes if default subtype was negotiated.
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

#include "hdrscmbccp.h"
#include "hdrscmdb.h"
#include "hdrscm.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/* Attribute IDs and Default Values */
#define HDRSCMBCCP_OVHD_PERIOD_ATTRIB_ID       0xFF
#define HDRSCMBCCP_PAGING_REG_PERIOD_ATTRIB_ID 0xFE
#define HDRSCMBCCP_DYN_BC_REG_PERIOD_ATTRIB_ID 0xFD
#define HDRSCMBCCP_MONITOR_PERIOD_ATTRIB_ID    0xFC
#define HDRSCMBCCP_NUM_PERIODS_ATTRIB_ID       0xFB

#define HDRSCMBCCP_DEFAULT_OVHD_PERIOD       0x07
#define HDRSCMBCCP_DEFAULT_PAGING_REG_PERIOD 0x00
#define HDRSCMBCCP_DEFAULT_DYN_BC_REG_PERIOD 0x00
#define HDRSCMBCCP_DEFAULT_MONITOR_PERIOD    0x00
#define HDRSCMBCCP_DEFAULT_NUM_PERIODS       0x02

#define HDRSCMBCCP_MIN_REG_PERIOD 0x05
#define HDRSCMBCCP_MAX_REG_PERIOD 0x15

#define HDRSCMBCCP_MIN_MONITOR_PERIOD 0x00
#define HDRSCMBCCP_MAX_MONITOR_PERIOD 0x0F

#define HDRSCMBCCP_MIN_NUM_PERIOD 0x01
#define HDRSCMBCCP_MAX_NUM_PERIOD 0x04

/* Simple attribute data size in bytes */
#define HDRSCMBCCP_SIMPLE_ATTRIB_DATA_SIZE 1

/*===========================================================================
 
                  DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/


/*===========================================================================

                         LOCAL FUNCTION DEFINITIONS
                    
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
boolean hdrscmbccp_is_attrib_simple
( 
  uint16 const attrib_id,
  uint16 const subtype
)
{

  return ((attrib_id >= 0xFB && attrib_id <= 0xFF)? TRUE : FALSE);

} /* hdrscmbccp_is_attrib_simple() */

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
hdrerrno_enum_type hdrscmbccp_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value,
  uint16 const subtype
)
{
  hdrerrno_enum_type err_no = E_FAILURE;
/*-------------------------------------------------------------------------*/

  switch( attrib_id )
  {
    case HDRSCMBCCP_OVHD_PERIOD_ATTRIB_ID:
      if (attrib_value != 0x00)
      {
        err_no = E_SUCCESS;
      }
      break;

    case HDRSCMBCCP_PAGING_REG_PERIOD_ATTRIB_ID:
    case HDRSCMBCCP_DYN_BC_REG_PERIOD_ATTRIB_ID:
      if ( ( attrib_value == 0x00 ) ||
           ( (attrib_value >= HDRSCMBCCP_MIN_REG_PERIOD) && 
             (attrib_value <= HDRSCMBCCP_MAX_REG_PERIOD) ) 
         )
      {  
        err_no = E_SUCCESS;
      }   
      break;

    case HDRSCMBCCP_MONITOR_PERIOD_ATTRIB_ID:
      if ( /* (attrib_val >= HDRSCMBCCP_MIN_MONITOR_PERIOD) && always true */
           (attrib_value <= HDRSCMBCCP_MAX_MONITOR_PERIOD) )
      {
        err_no = E_SUCCESS;
      }
      break;

    case HDRSCMBCCP_NUM_PERIODS_ATTRIB_ID:
      if ( (attrib_value >= HDRSCMBCCP_MIN_NUM_PERIOD) && 
           (attrib_value <= HDRSCMBCCP_MAX_NUM_PERIOD)
         )
      {
        err_no = E_SUCCESS;
      }
      break;

    default:
      ERR("hdrscmbccp_is_simple_attrib_valid unknown attrib_id %d", 
           attrib_id, 0, 0);
      break;
  }

  return err_no;

} /* hdrscmbccp_is_simple_attrib_valid() */

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
hdrerrno_enum_type hdrscmbccp_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Status of get attrib offset and size */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  switch( attrib_id )
  {
    case HDRSCMBCCP_OVHD_PERIOD_ATTRIB_ID:
      *attrib_offset = FPOS( hdrscmbccp_config_params_type, 
                             bcast_overhead_period );
      *attrib_size = HDRSCMBCCP_SIMPLE_ATTRIB_DATA_SIZE;
      break;

    case HDRSCMBCCP_PAGING_REG_PERIOD_ATTRIB_ID:
      *attrib_offset = FPOS( hdrscmbccp_config_params_type, 
                             paging_reg_period );
      *attrib_size = HDRSCMBCCP_SIMPLE_ATTRIB_DATA_SIZE;
      break;

    case HDRSCMBCCP_DYN_BC_REG_PERIOD_ATTRIB_ID:
      *attrib_offset = FPOS( hdrscmbccp_config_params_type, 
                             dyn_bc_reg_period );
      *attrib_size = HDRSCMBCCP_SIMPLE_ATTRIB_DATA_SIZE;
      break;

    case HDRSCMBCCP_MONITOR_PERIOD_ATTRIB_ID:
      *attrib_offset = FPOS( hdrscmbccp_config_params_type, monitor_period );
      *attrib_size = HDRSCMBCCP_SIMPLE_ATTRIB_DATA_SIZE;
      break;

    case HDRSCMBCCP_NUM_PERIODS_ATTRIB_ID:
      *attrib_offset = FPOS( hdrscmbccp_config_params_type, 
                             num_monitor_period );
      *attrib_size = HDRSCMBCCP_SIMPLE_ATTRIB_DATA_SIZE;
      break;

    default:
      ERR("hdrscmbccp_is_simple_attrib_valid unknown attrib_id %d", 
           attrib_id, 0, 0);
      err_no = E_DATA_INVALID;
      break;
  }

  return err_no;
} /* hdrscmbccp_get_attrib_offset_and_size() */

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
hdrerrno_enum_type hdrscmbccp_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *index,
  uint16 subtype
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Status of get attrib offset and size */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  if (attrib_id >= 0xFB && attrib_id <= 0xFF)
  {
    /* Attribute version numbers are stored in array indexed by the
     * attribute id */
    *index = 0xFF - attrib_id;
  }
  else
  {
    ERR("hdrscmbccp_get_attrib_verno_index unknown attrib_id %d", 
         attrib_id, 0, 0);
    err_no = E_DATA_INVALID;
  }

  return err_no;

} /* hdrscmbccp_get_attrib_verno_index() */

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
void hdrscmbccp_set_config_params_default 
( 
  hdrscmbccp_config_params_type *current_config
)
{
  
  current_config->monitor_period = HDRSCMBCCP_DEFAULT_MONITOR_PERIOD;
  current_config->dyn_bc_reg_period = HDRSCMBCCP_DEFAULT_DYN_BC_REG_PERIOD;
  current_config->num_monitor_period = HDRSCMBCCP_DEFAULT_NUM_PERIODS;
  current_config->bcast_overhead_period = HDRSCMBCCP_DEFAULT_OVHD_PERIOD;
  current_config->paging_reg_period = HDRSCMBCCP_DEFAULT_PAGING_REG_PERIOD;

} /* hdrscmbccp_set_config_params_default() */


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
)
{

  boolean known = FALSE;

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(subtype)
  {
    case HDRSCP_DEFAULT_BCMCS:
      HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                     "No Configuration attributes are define for Default BCMCS");
      break;

    case HDRSCP_GENERIC_BCMCS:
      known = ((attrib_id >= HDRSCMBCCP_NUM_PERIODS_ATTRIB_ID) &&
               (attrib_id <= HDRSCMBCCP_OVHD_PERIOD_ATTRIB_ID));
      break;

    default:
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                     "BCMCS Subtype 0x%x not supported", subtype);
      break;
  }

  if (!known)
  {
    HDR_MSG_PROT_2(MSG_LEGACY_ERROR, "BCMCS subtype %d: attribute 0x%x not supported", 
                   subtype, attrib_id);
                   
  }

  return known;

} /* hdrscmbccp_attrib_is_supported */

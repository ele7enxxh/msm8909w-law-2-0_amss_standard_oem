#ifndef HDRSCMRMAC_H
#define HDRSCMRMAC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

           R E V E R S E  C H A N N E L  M A C  P R O T O C O L
                  
                            H E A D E R   F I L E       

       
GENERAL DESCRIPTION
  This module provides supporting API for SCM module to process
  ConfigurationRequest messages and AttributeUpdateRequest messages directed
  to Reverse Traffic channel MAC protocol. 
    
EXTERNALIZED FUNCTIONS
  hdrscmrmac_process_complex_attrib 
    Processes all FMAC complex attributes 
    
  hdrscmrmac_is_attrib_simple 
    Returns true if an attribute is simple

  hdrscmrmac_is_simple_attrib_valid
    Verifies the validity of the simple attribute

  hdrscmrmac_get_attrib_offset_and_size
    Returns attribute size and offset into the config structure.

  hdrscmrmac_get_attrib_verno_index
    Returns the index of an attribute in the version number array. 
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

  
  Copyright (c) 2005 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrscmrmac.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/29/13   smd     Disable RevB code if FEATURE_HDR_REVB_SUPPORTED is not defined.
06/20/07   cc      Added DOrB RTC MAC support.
02/17/07   wsh     Added value_id/attrib_id checking processing ConfigResp
01/03/07   etv     Added support to handle unknown attributes.
07/11/06   etv     Fixed bug by passing dsm_item_typ** instead of dsm_item_typ*
06/08/06   etv     Send session close if AN GAUPs a non-GAUPable attribute.
04/18/06   etv     Enhanced SCM to support indefinite num of attribs in CReq.
01/31/06   etv     Added support for AT-init Neg. for RMAC3.
11/21/05   etv     Added header tags for perforce to fill in
11/04/05   etv     Changed set_default interface to take subtype.
11/02/05   etv     Changed SCM design to propogate subtype info.
08/22/05   etv     Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/

#include "hdr_variation.h"
#include "dsm.h"
#include "comdef.h"
#include "err.h"

#include "hdrdebug.h"
#include "hdrdsm.h"
#include "hdrscp.h"
#include "hdrhai.h"
#include "hdrtrace.h"
#include "hdrbit.h"

#ifdef FEATURE_HDR_REVB_SUPPORTED
#include "hdrscmrmac4.h"
#endif /* FEATURE_HDR_REVB_SUPPORTED */
#include "hdrscmrmac3.h"
#include "hdrscmrmac0.h"
#include "hdrscm.h"
#include "hdrscmdb.h"

 /* EJECT */
/*===========================================================================
 
                  DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/* Union defining the configuration attributes for RTCMAC. It includes both
   Subtype-0/1 and Subtype-3 */
typedef union
{
#ifdef FEATURE_HDR_REVB_SUPPORTED
  hdrscmrmac4_config_struct_type sub4;
#endif /* FEATURE_HDR_REVB_SUPPORTED */
  hdrscmrmac3_config_struct_type sub3;
  hdrscmrmac0_config_struct_type sub_0_1;

} hdrscmrmac_config_attrib_union_type;

/* Union defining the version number for ST-0/1 and ST-3 Configuration 
   attributes */
typedef union
{
  hdrscmrmac0_verno_struct_type sub_0_1;
  hdrscmrmac3_verno_type sub3;
#ifdef FEATURE_HDR_REVB_SUPPORTED
  hdrscmrmac4_verno_type sub4;
#endif /* FEATURE_HDR_REVB_SUPPORTED */
} hdrscmrmac_verno_union_type;


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC_PROCESS_COMPLEX_ATTRIB

DESCRIPTION
  This is a generic interface to process the configuration request messages of
  all complex attributes of Reverse channel MAC protocol.

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.
  subtype  - Subtype negotiated for RMAC

RETURN VALUE
  E_SUCCESS      - If a valid complex attribute was parsed successfully.
  E_NO_DATA      - If the config request message does not have enough data.
  E_OUT_OF_RANGE - If the config request message does not have any valid data.
  E_DATA_INVALID - If the given attrib_id is not valid.

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrscmrmac_process_complex_attrib
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  uint16                        const          subtype
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC_IS_ATTRIB_SIMPLE

DESCRIPTION
  This function is used by SCM to query if an attribute is simple, given the
  attribute ID.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id - Attribute ID 
  subtype   - Subtype negotiated for RMAC.

RETURN VALUE
  TRUE - if the attribute given by 'attrib_id' is simple.
  FALSE - otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrscmrmac_is_attrib_simple
(
  uint16 const attrib_id,
  uint16 const subtype
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC_IS_SIMPLE_ATTRIB_VALID

DESCRIPTION
  This function is used by SCM to query if the given 'attrib_value' is a 
  valid value for the attribute specified by 'attrib_id'.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id    - Attribute ID of the simple attribute.
  attrib_value - Value of the simple attribute. 
  subtype   - Subtype negotiated for RMAC.

RETURN VALUE
  E_SUCCESS          - If the attrib_value is valid.
  E_OUT_OF_RANGE     - If the attrib_value is not valid.
  E_DATA_INVALID     - If the attribute specified by attrib_id is not simple.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern hdrerrno_enum_type hdrscmrmac_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value,
  uint16 const subtype
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC_GET_ATTRIB_OFFSET_AND_SIZE

DESCRIPTION
  This function computes the byte offset of the attribute specified by the 
  attrib_id in the struct hdrscmrmac3_config_struct_type if the subtype in 
  config is 3 or hdrscmrmac0_config_struct_type if the subtype in config 
  is either 0 or 1 and stores it in out-param attrib_offset. It also stores 
  the size of the attribute in attrib_size.

DEPENDENCIES
  None

PARAMETERS
  attrib_id     - Attribute Id.
  attrib_offset - Address where the Byte Offset from the start of the struct 
                  to the attribute is stored.
  attrib_size   - Address where the size of the attribute is stored.
  subtype       - Subtype of RMAC
  
RETURN VALUE
  E_SUCCESS - If the given attrib ID is valid.
  E_DATA_INVALID - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern hdrerrno_enum_type hdrscmrmac_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC_GET_ATTRIB_VERNO_INDEX

DESCRIPTION
  Returns the index of the given attribute in the version number array.

DEPENDENCIES
  None

PARAMETERS
  attrib_id     - Attribute Id.
  pindex        - Address where the index is stored.
  subtype       - Subtype negotiated for RMAC.
  
RETURN VALUE
  E_SUCCESS - If the given attribute ID is valid.
  

SIDE EFFECTS
  None

===========================================================================*/
extern hdrerrno_enum_type hdrscmrmac_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *pindex,
  uint16 subtype
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  Set the ST-0/1 RMAC configuration attributes to its default values.

DEPENDENCIES
  None

PARAMETERS
  p_config - Pointer to hdrscmrmac_config_attrib_union_type
  subtype  - Subtype negotiated for RMAC.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void hdrscmrmac_set_config_to_default
(
  hdrscmrmac_config_attrib_union_type *p_config,
  uint16 subtype
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC_GET_ATTRIB_ID_SIZE

DESCRIPTION
  The attribute ID size in RMAC is subtype-specific. This function is used by
  SCM to get the attribute ID size of the current/config subtype.

DEPENDENCIES
  None.

PARAMETERS
  subtype   - Subtype negotiated for RMAC.

RETURN VALUE
  attrib_id_size - Size of the attribute ID for the current/config subtype.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 hdrscmrmac_get_attrib_id_size( uint16 subtype );


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC_PROP_GAUP_RATE_PARAMS

DESCRIPTION
  This routine is used by ST-1 RMAC to send GAUP update request for Rate 
  Parameters attribute.

DEPENDENCIES
  None.

PARAMETERS
  rp_mode    - Rate Parameters Mode. Valid values are, 
                1. HDRSCMRMAC0_RATE_PARAMS_VT_MODE
                2. HDRSCMRMAC0_RATE_PARAMS_NORMAL_MODE
  p_trans_id - Address where the transaction ID is stored to be returned back.
 
RETURN VALUE
  E_SUCCESS       - AT has sent the AttributeUpdate Message and transaction ID 
                    is sent back.
  E_NOT_SUPPORTED - If the current subtype is not ST-1.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmrmac_prop_gaup_rate_params
(
  hdrscmrmac0_rate_params_mode_enum_type rp_mode,
  uint8 *p_trans_id
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC_GET_GAUPED_COMPLEX_ATTRIB_VALUE

DESCRIPTION
  This function is used by SCM to get the complex attribute value proposed by
  AT as GAUP update.

DEPENDENCIES
  None.

PARAMETERS
  trans_id      - Transaction ID.
  a_a_aip       - Array of Accepted attribute information pointer where the 
                  info about each of the attributes in the transaction 
                  specified by trans_id are stored.
  attrib_cap    - Maximum number of attributes that could be stored in the 
                  array.
  attrib_count  - Number of attributes GAUPed in this transaction.
  subtype   - Subtype negotiated for RMAC.
 
RETURN VALUE
  E_SUCCESS       - If the given transaction ID is valid.
  E_NOT_SUPPORTED - If the current subtype is not allowed to receive any GAUP
                    update response message. RMAC ST-1 is the only subtype
                    allowed to propose rate params.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmrmac_get_gauped_complex_attrib_value
(
  uint8 const trans_id,
  dsm_item_type **aai_dsm,
  uint8 *const attrib_count,
  uint16 const subtype
);

/* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC_GET_PROP_ATTRIB_VALUE

DESCRIPTION
  The function is used by SCM to get the proposed complex attribute value.

DEPENDENCIES
  None

PARAMETERS
  trans_id - Transaction ID of the Configuration message.
  a_aip    - Accepted attribute information pointer.
  subtype  - Subtype of the protocol for which the message is destined.
  
RETURN VALUE
  E_SUCCESS - If the subtype of the procotol supports AT-init negotiation 
              and if there was a valid outstanding transaction.
  E_FAILURE - If the trans_id does not match to any outstanding transaction.
  E_NOT_SUPPORTED - If the subtype does not support AT-init Neg.

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrscmrmac_get_prop_attrib_value
(
  uint16 const trans_id,
  hdrscm_accepted_attrib_type *a_aip,
  uint16 const subtype
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSCMRMAC_FAIL_CURR_CONFIG_NEG

DESCRIPTION
  This function fails the current config negotiation

DEPENDENCIES
  None

PARAMETERS
  close_reason - Session Close reason

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscmrmac_fail_curr_config_neg
(
  hdrhai_session_close_reason_enum_type close_reason
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC_IS_ATTRIB_GAUPABLE

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
hdrerrno_enum_type hdrscmrmac_is_attrib_gaupable
(
  uint16 const attrib_id,
  uint16 const subtype
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC_ATTRIB_IS_SUPPORTED

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
boolean hdrscmrmac_attrib_is_supported
(
  uint16 const attrib_id,
  uint16 const subtype
);

#endif /* HDRSCMRMAC_H */

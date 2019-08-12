#ifndef HDRSCMRMAC0_H
#define HDRSCMRMAC0_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

          S U B T Y P E - 0  A N D  S U B T Y P E - 1  R E V E R S E  

                  C H A N N E L  M A C  P R O T O C O L  

                          H E A D E R  F I L E

       
GENERAL DESCRIPTION
  This module provides supporting API for SCM module to process
  ConfigurationRequest messages and AttributeUpdateRequest messages directed
  to Subtype-0 and Subtype-1 Reverse Traffic channel MAC protocol. 
    
EXTERNALIZED FUNCTIONS
  hdrscmrmac0_process_complex_attrib 
    Processes all FMAC complex attributes 
    
  hdrscmrmac0_is_attrib_simple 
    Returns true if an attribute is simple

  hdrscmrmac0_is_simple_attrib_valid
    Verifies the validity of the simple attribute

  hdrscmrmac0_get_attrib_offset_and_size
    Returns attribute size and offset into the config structure.

  hdrscmrmac0_get_attrib_verno_index
    Returns the index of an attribute in the version number array. 
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

  
  Copyright (c) 2005 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrscmrmac0.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/28/11   smd     SU API cleanup.
09/21/07   wsh     Fixed RMAC1 rejecting AN GAUPing RateParameter 
01/03/07   etv     Added support to handle unknown attributes.
07/11/06   etv     Fixed bug by passing dsm_item_typ** instead of dsm_item_typ*
04/18/06   etv     Enhanced SCM to support indefinite num of attribs in CReq.
11/21/05   etv     Added header tags for perforce to fill in
10/26/05   dna     Integration updates to GAUP support for RMAC1 rate params
08/22/05   etv     Created module.

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
#include "hdrbit.h"

#include "hdrscm.h"
#include "hdrscmdb.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

#define HDRSCMRMAC0_NUM_RATE_PARAM_TRANS            8
#define HDRSCMRMAC0_NUM_DATA_OFFSET_RATES           6

 /* EJECT */
/*===========================================================================
 
                  DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/* ST-0/1 Attribute ID enum type */
typedef enum
{
  HDRSCMRMAC0_ATTRIB_POWER_PARAMS = 0x00,
  HDRSCMRMAC0_ATTRIB_RATE_PARAMS  = 0x01,
  HDRSCMRMAC0_NUM_ATTRIBUTES
} hdrscmrmac0_attrib_id_enum_type;

/* ST-0/1 RMAC Version Number type */
typedef uint8 hdrscmrmac0_ver_no_type[HDRSCMRMAC0_NUM_ATTRIBUTES];

/* Power Parameters configuration data type. */
typedef struct
{
  uint8 data_offset_rate[HDRSCMRMAC0_NUM_DATA_OFFSET_RATES]; 
    /*Index 0 refers to nom offset*/

  uint8 rpc_step;
    /* Reverse Power Control Step */

} hdrscmrmac0_power_params_struct_type;


/* Rate Parameters configuration data type. */
typedef struct
{
  uint8 transition[HDRSCMRMAC0_NUM_RATE_PARAM_TRANS];

} hdrscmrmac0_rate_params_struct_type;

typedef struct
{
  hdrscmrmac0_power_params_struct_type power_params;
    /* PowerParameters Attribute. AttribId: 0x00 */

  hdrscmrmac0_rate_params_struct_type rate_params;
    /* RateParameters Attribuet. AttribId: 0x01 */

} hdrscmrmac0_config_struct_type;

typedef struct
{
  uint8 power_params;
    /* Version number for power parameters */

  uint8 rate_params;
    /* Version number for rate parameters */

} hdrscmrmac0_verno_struct_type;


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC0_PROCESS_COMPLEX_ATTRIB

DESCRIPTION
  This is a generic interface to process the configuration request messages of
  all complex attributes of Subtype-0 and Subtype-1 Reverse channel MAC 
  protocol.

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  E_SUCCESS      - If a valid complex attribute was parsed successfully.
  E_NO_DATA      - If the config request message does not have enough data.
  E_OUT_OF_RANGE - If the config request message does not have any valid data.
  E_DATA_INVALID - If the given attrib_id is not valid.

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrscmrmac0_process_complex_attrib
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC0_IS_ATTRIB_SIMPLE

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
extern boolean hdrscmrmac0_is_attrib_simple
(
  uint16 const attrib_id
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC0_IS_SIMPLE_ATTRIB_VALID

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
  E_OUT_OF_RANGE     - If the attrib_value is not valid.
  E_DATA_INVALID     - If the attribute specified by attrib_id is not simple.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern hdrerrno_enum_type hdrscmrmac0_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value 
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC0_GET_ATTRIB_OFFSET_AND_SIZE

DESCRIPTION
  This function computes the byte offset of the attribute specified by the 
  attrib_id in the struct hdrscmrmac0_config_struct_type and stores it in 
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
  E_SUCCESS - If the given attrib ID is valid.
  E_DATA_INVALID - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern hdrerrno_enum_type hdrscmrmac0_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC0_GET_ATTRIB_VERNO_INDEX

DESCRIPTION
  Returns the index of the given attribute in the version number array.

DEPENDENCIES
  None

PARAMETERS
  attrib_id     - Attribute Id.
  pindex        - Address where the index is stored.
  
RETURN VALUE
  E_SUCCESS - If the given attribute ID is valid.
  

SIDE EFFECTS
  None

===========================================================================*/
extern hdrerrno_enum_type hdrscmrmac0_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *pindex
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC0_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  Set the ST-0/1 RMAC configuration attributes to its default values.

DEPENDENCIES
  None

PARAMETERS
  p_config - Pointer to hdrscmrmac0_config_struct_type
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrscmrmac0_set_config_to_default
(
  hdrscmrmac0_config_struct_type *p_config
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC0_PROP_GAUP_RATE_PARAMS

DESCRIPTION
  This routine is used by ST-1 RMAC to send GAUP update request for Rate 
  Parameters attribute.

DEPENDENCIES
  Since SCMRMAC0 is not notified whether the GAUP update is accepted or not, it
  doesn't know the current Mode of RateParameters. It is the responsibility of
  the PacketApp to check for the following scenarios. 

  The 'IV' is an invalid scenario, 'U' is a useless scenario and 'V's are valid 
  scenarios.

                      Requested RP Mode
                     
                         | Normal |   VT  |
                  -----------------------------
       Current    Normal |   U    |   V   |
         RP       -----------------------------
        Mode        VT   |   V    |   IV  |


  The PacketApp should also make sure that this function is not called while it 
  is waiting on two AttribUpdateRequests already. The effect of calling this 
  function again could be an Error Fatal owing to insufficient Response timers.

ASSUMPTIONS
  When a VT mode is requested, the current mode could only be Normal. If this
  assumption is broken, the current configuration saved in rp_normal would be 
  lost and the configuration would stay VT until session close!

PARAMETERS
  rp_mode - Rate Parameters Mode. Could only be one of the following two values
            HDRSCMRMAC0_RATE_PARAMS_VT_MODE
            HDRSCMRMAC0_RATE_PARAMS_NORMAL_MODE
  p_trans_id - Address where the transaction ID is stored to be returned back.
  
RETURN VALUE
  E_SUCCESS - If a GAUP Update was sent successfully.
  E_NOT_AVAILABLE - If we didn't have a free Rsp timer to send this GAUP update.

SIDE EFFECTS
  Error Fatal if there aren't enough response timers to proceed.

===========================================================================*/
extern hdrerrno_enum_type hdrscmrmac0_prop_gaup_rate_params
(
  hdrscmrmac0_rate_params_mode_enum_type rp_mode, uint8 *p_trans_id
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC0_GET_GAUPED_COMPLEX_ATTRIB_VALUE

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
  attrib_cap    - Maximum number of attributes that could be stored in the 
                  array.
  attrib_count  - Number of attributes GAUPed in this transaction.
 
RETURN VALUE
  E_SUCCESS       - If the given transaction ID is valid.
  E_NOT_AVAILABLE - If a valid transaction specified by 'trans_id' is not 
                    available.

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrscmrmac0_get_gauped_complex_attrib_value
(
  uint8 const trans_id,
  dsm_item_type **aai_dsm,
  uint8 *const attrib_count
);


/*===========================================================================

FUNCTION HDRSCMRMAC0_READ_VT_RP_FROM_NV                                

DESCRIPTION
  This function is used to read custom VT Rate parameters from NV.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrscmrmac0_read_vt_rp_from_nv (void);

/* EJECT */
/*===========================================================================

FUNCTION HDRSCMRMAC0_ATTRIB_IS_SUPPORTED

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
extern boolean hdrscmrmac0_attrib_is_supported
(
  uint16 const attrib_id
);

/*===========================================================================

FUNCTION HDRSCMRMAC0_IS_ATTRIB_GAUPABLE

DESCRIPTION
  This function is used by SCM module to find out if an attribute is GAUPable.

DEPENDENCIES
  None.

PARAMETERS
  attrib_id - Attribute ID.
  
RETURN VALUE
  E_SUCCESS       - If the attribute is GAUPable.
  E_NOT_ALLOWED   - If AN is not allowed to GAUP this attribute.

SIDE EFFECTS
  None.
===========================================================================*/
hdrerrno_enum_type hdrscmrmac0_is_attrib_gaupable
(
  uint16 const attrib_id
);

#endif /* HDRSCMRMAC0_H */

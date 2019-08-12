#ifndef HDRSCMGMMCDP_H
#define HDRSCMGMMCDP_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        H D R   S E S S I O N  C O N F I G U R A T I O N  M A N A G E R 

          G E N E R I C  M U L T I - M O D E  C A P A B I L I T Y 

                    D I S C O V E R Y  P R O T O C O L

                          H E A D E R   F I L E       


GENERAL DESCRIPTION
  This module contains the configuration interface of the HDR Session 
  Configuration Manager Generic Multi-Mode Capability Discovery protocol.


EXTERNALIZED FUNCTIONS

  hdrscmgmmcdp_is_attrib_simple 
    Returns true if an attribute is simple

  hdrscmgmmcdp_get_attrib_offset_and_size
    Returns attribute size and offset into the config structure.

  hdrscmgmmcdp_get_attrib_verno_index
    Returns the index of an attribute in the version number array. 

INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2005-2010 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrscmgmmcdp.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/02/11   cnx     Removed hdrscmgmmcdp_powerup_init().
11/22/10   cnx     Added hdrscmgmmscd_is_simple_attrib_valid(). 
07/31/09   wsh     Do not propose GMMCDP attributes if not needed
09/03/08   etv     Fixed bug with RxDiversity Attribute proposal.
09/18/07   wsh     Report correct DiversityComb and SimultaneousCommonChannelReceive
                   value in Multi-Mode Discovery protocol
02/17/07   wsh     Added value_id/attrib_id checking processing ConfigResp
01/03/07   etv     Added support to handle unknown attributes.
02/02/06   etv     Defined enum Multi-rx Capability type.
11/27/05   etv     Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "dsm.h"
#include "comdef.h"
#include "err.h"

#include "hdrdsm.h"
#include "hdrscp.h"
#include "hdrhai.h"
#include "hdrtrace.h"

#include "hdrscm.h"

 /* EJECT */
/*===========================================================================
                  DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables and
other items needed by this module.

===========================================================================*/

/* Bitmask used to inidcate which GMMCDP attribute need to be negotiated */
#define HDRSCMGMMCDP_CONFIG_NOT_NEEDED          0
#define HDRSCMGMMCDP_SIM_COMMON_CHAN_RCV_NEEDED 1
#define HDRSCMGMMCDP_DIVERSITY_COMB_NEEDED      2

#define HDRSCMGMMCDP_NUM_SUPPORTED_ATTRIBS          2

/* Multiple Rx Chain Capability Type */
typedef enum
{
  HDRSCMGMMCDP_SINGLE_RX_CHAIN_FOR_COMMON_CHAN = 0x00,
  /* according to standard, value 0x01 means AT has dedicated tunner */
  HDRSCMGMMCDP_MULTI_RX_CHAIN_FOR_COMMON_CHAN = 0x01, 
  HDRSCMGMMCDP_MULTI_RX_CHAIN_FOR_DEDICATED_CHAN_SENS_5MHZ = 0x02,
  HDRSCMGMMCDP_MULTI_RX_CHAIN_FOR_DEDICATED_CHAN_SENS_10MHZ = 0x03,
  HDRSCMGMMCDP_MULTI_RX_CHAIN_FOR_DEDICATED_CHAN_SENS_15MHZ = 0x04,
  HDRSCMGMMCDP_MULTI_RX_CHAIN_FOR_DEDICATED_CHAN_SENS_20MHZ = 0x05,
  HDRSCMGMMCDP_MULTI_RX_CHAIN_FOR_DEDICATED_CHAN_SENS_5MHZ_MULTIBAND = 0x06,
  HDRSCMGMMCDP_MULTI_RX_CHAIN_FOR_DEDICATED_CHAN_SENS_10MHZ_MULTIBAND = 0x07,
  HDRSCMGMMCDP_MULTI_RX_CHAIN_FOR_DEDICATED_CHAN_SENS_15MHZ_MULTIBAND = 0x08,
  HDRSCMGMMCDP_MULTI_RX_CHAIN_FOR_DEDICATED_CHAN_SENS_20MHZ_MULTIBAND = 0x09
} hdrscmgmmcdp_multi_rx_capability_type;

/* Diversity Combining Capability Type */
typedef enum
{
  HDRSCMGMMCDP_DIVERSITY_COMB_DISABLED = 0x00,
  HDRSCMGMMCDP_DIVERSITY_COMB_ENABLED = 0x01
} hdrscmgmmcdp_diversity_comb_type;

/* Commands supported by the Generic Multi-mode Capability Discovery protocol.
   Note: The cmd enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value */
typedef enum
{
  HDRSCMGMMCDP_CONFIGURE = 0    /* Start AT-initiated Session Negotiation */

} hdrscmgmmcdp_cmd_enum_type;

typedef struct
{
  uint8 sim_comm_chan_rcv;
    /* SimultaneousCommonChannelReceive AttribId: 0xFD; Default 0x00; 
       Valid Range: {0x00 ~ 0x09); AT proposes: 0x01 */

  uint8 diversity_comb_enable;
    /* ReceiverDiversity AttribId: 0xFA; Default 0x01
       Valid Range: {0x00, 0x01}; 
       AT proposes 0x00 only if Diversity is not enabled */


} hdrscmgmmcdp_config_struct_type;

typedef uint8 hdrscmgmmcdp_verno_type[HDRSCMGMMCDP_NUM_SUPPORTED_ATTRIBS];

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMGMMCDP_IS_ATTRIB_SIMPLE

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
extern boolean hdrscmgmmcdp_is_attrib_simple
(
  uint16 const attrib_id,
  uint16 const subtype
);


/*===========================================================================

FUNCTION HDRSCMGMMCDP_IS_SIMPLE_ATTRIB_VALID

DESCRIPTION
  This function is used by SCM to query if the given 'attrib_value' is a 
  valid value for the attribute specified by 'attrib_id'.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id    - Attribute ID of the simple attribute.
  attrib_value - Value of the simple attribute. 
  subtype      - Subtype of the protocol.

RETURN VALUE
  E_SUCCESS          - If the attrib_value is valid.
  E_OUT_OF_RANGE     - Otherwise.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern hdrerrno_enum_type hdrscmgmmscd_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value,
  uint16 const subtype
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMGMMCDP_CONFIGURE

DESCRIPTION
  This function is used by SCP to command Generic Multi-mode Capability
  Discovery protocol to start its AT-init Session Negotiation. At present
  the only attribute that this protocol proposes is 

  SimultaneousCommonChannelReceive (0xFD) Value - 0x01
  
DEPENDENCIES
  None.

PARAMETERS
  sender - Protocol that commands GMMCDP to configure

RETURN VALUE
  TRUE - If the config message was packed and sent to HMP for delivery.
  FALSE - Otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrscmgmmcdp_configure
(
  hdrhai_protocol_name_enum_type sender
);


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSCMGMMCDP_FAIL_CURR_CONFIG_NEG

DESCRIPTION
  This function fails the current config negotiation

DEPENDENCIES
  None

PARAMETERS
  close_reason - Session Close reason
  attrib_id    - Attribute ID

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscmgmmcdp_fail_curr_config_neg
(
  hdrhai_session_close_reason_enum_type close_reason
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMGMMCDP_GET_ATTRIB_OFFSET_AND_SIZE

DESCRIPTION
  This function computes the byte offset of the attribute specified by the 
  attrib_id in the struct hdrscmgmmcdp_config_struct_type and stores it in 
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
extern hdrerrno_enum_type hdrscmgmmcdp_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMGMMCDP_GET_ATTRIB_VERNO_INDEX

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
extern hdrerrno_enum_type hdrscmgmmcdp_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *index,
  uint16 subtype
);

/* EJECT */
/*===========================================================================

FUNCTION HDRSCMGMMCDP_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  Set the configuration attributes to its default values.

DEPENDENCIES
  None

PARAMETERS
  hdrgmmcdp_cfg - Pointer to the config structure.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void hdrscmgmmcdp_set_config_to_default
(
  hdrscmgmmcdp_config_struct_type *hdrgmmcdp_cfg
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMGMMCDP_ATTRIB_IS_SUPPORTED

DESCRIPTION
  Is this attribute supported?

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
boolean hdrscmgmmcdp_attrib_is_supported
(
  uint16 const attrib_id,
  uint16 const subtype
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSCMGMMCDP_GET_PROP_ATTRIB_VALUE

DESCRIPTION
  This function checks if the ConfigResp for a simple attribute is valid

DEPENDENCIES
  None

PARAMETERS
  trans_id - The transaction_id AT proposed
  a_aip - pointer to caller allocated buffer for storing ConfigResp
  subtype  - The protocol subtype

RETURN VALUE
  TRUE - If value is valid
  FALSE - If AN responded with a invalid value

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrscmgmmcdp_get_prop_attrib_value
(
  uint16 const trans_id,
  hdrscm_accepted_attrib_type *const a_aip,
  uint16 const subtype
);

/*===========================================================================

FUNCTION HDRSCMGMMCDP_RF_SETTING_CHANGED

DESCRIPTION
  This function checks if the SimCommRecv or DiversityComb setting
  based on NV_ITEM, etc, has changed from the InUse value.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - If RF setting has changed from InUse value
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscmgmmcdp_rf_setting_changed( void );

#endif /* HDRSCMGMMCDP_H */

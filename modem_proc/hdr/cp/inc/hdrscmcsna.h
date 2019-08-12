#ifndef HDRSCMCSNA_H
#define HDRSCMCSNA_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

                      C I R C U I T   S E R V I C E S

              N O T I F I C A T I O N   A P P L I C A T I O N

                        H E A D E R  F I L E
       
GENERAL DESCRIPTION
  This module provides supporting API for SCM module to process
  ConfigurationRequest messages directed to Circuit Services Notification
  Application (CSNA)

EXTERNALIZED FUNCTIONS

  hdrscmcsna_process_complex_attrib
    Processes all CSNA complex attributes

  hdrscmcsna_is_attrib_simple
    Returns true if an attribute is simple

  hdrscmcsna_is_simple_attrib_valid
    Verifies the validity of the simple attribute

  hdrscmcsna_get_attrib_offset_and_size
    Returns attribute size and offset into the config structure.

  hdrscmcsna_get_attrib_verno_index
    Returns the index of an attribute in the version number array.

INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2006 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/inc/hdrscmcsna.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/18/09   wsh     Updated file to include private header hdrmc_v.h
02/17/07   wsh     Added value_id/attrib_id checking processing ConfigResp
02/05/07   etv     Added support to handle unknown attributes.
01/17/07   yll     Changes for HOMER interface.
01/08/07   yll     Created hdrscmcsna module to support CSNA config attribs.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"

#ifdef FEATURE_CMI
#include "hdrmc_v.h"
#endif


/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
// YLL - consider moving them to hdrscmcsna.c module to avoid homer include */

/* Number of configuration attributes in CSNA protocol  */
#define HDRSCMCSNA_NUM_CONFIG_ATTRIBS                 11

/* Enum defining the IDs of all configuration attributes of CSNA */
typedef enum
{
  /* Simple attributes */

  /* IdleTunnelExponent */
  HDRSCMCSNA_IDLE_TUNNEL_EXPONENT_ID        = 0xFF,

  /* RAND */
  HDRSCMCSNA_RAND_ID                        = 0xFE,

  HDRSCMCSNA_LAST_SIMPLE_ATTRIB_ID = 
    HDRSCMCSNA_RAND_ID,

  /* MOB_P_REV */
  HDRSCMCSNA_MOB_P_REV_ID                   = 0xFD,

  HDRSCMCSNA_FIRST_SIMPLE_ATTRIB_ID = 
    HDRSCMCSNA_MOB_P_REV_ID,

  /* Complex Attributes */

  /* AllowedReverseLinkMessages */
  HDRSCMCSNA_ALLOWED_REV_LINK_MESSAGES_ID   = 0x01,

  HDRSCMCSNA_FIRST_COMPLEX_ATTRIB_ID = 
    HDRSCMCSNA_ALLOWED_REV_LINK_MESSAGES_ID,

  /* AllowedForwardLinkMessages */
  HDRSCMCSNA_ALLOWED_FOR_LINK_MESSAGES_ID   = 0x02,

  HDRSCMCSNA_LAST_COMPLEX_ATTRIB_ID = 
    HDRSCMCSNA_ALLOWED_FOR_LINK_MESSAGES_ID,

  /* Rest of the attributes not supported at the moment. */

  /* ServiceOptionIdle */
  HDRSCMCSNA_SERVICE_OPTION_IDLE_ID         = 0x03,

  /* ServiceOptionConnected */
  HDRSCMCSNA_SERVICE_OPTION_CONNECTED_ID    = 0x04,

  /* BurstTypeIdle */
  HDRSCMCSNA_BURST_TYPE_IDLE_ID             = 0x05,

  /* BurstTypeConnected */
  HDRSCMCSNA_BURST_TYPE_CONNECTED_ID        = 0x06,

  /* OrderCodeReverse */
  HDRSCMCSNA_ORDER_CODE_REVERSE_ID          = 0x07,

  /* OrderCodeForward */
  HDRSCMCSNA_ORDER_CODE_FORWARD_ID          = 0x08,

} hdrscmcsna_attrib_id_enum_type;

#define HDRSCMCSNA_MAX_MSG_ID_COUNT           5

#define HDRSCMCSNA_MAX_SRV_OPT_COUNT          256

/* TSB58 has 10 defined burst types plus other proprietary burst types,
 * use 10 here to save memory
 */
#define HDRSCMCSNA_MAX_BURST_TYPE_COUNT       10
#define HDRSCMCSNA_MAX_BCAST_ADDR_COUNT       4
#define HDRSCMCSNA_MAX_BCAST_ADDR_LEN         16

#define HDRSCMCSNA_MAX_ORDER_CODE_COUNT       64

/* Allowed Forward/Reverse Link Messages Attribute type */
typedef struct
{
  /* CSCH Message IDs */
  uint8   csch_msg_id_count;
  uint8   csch_msg_id[HDRSCMCSNA_MAX_MSG_ID_COUNT];

  /* DSCH Message IDs */
  uint8   dsch_msg_id_count;
  uint8   dsch_msg_id[HDRSCMCSNA_MAX_MSG_ID_COUNT];

} hdrscmcsna_allowed_msgs_attrib_type;

/* Service Option Attribute type */
typedef struct
{
  uint8   so_count;
  uint16  so[HDRSCMCSNA_MAX_SRV_OPT_COUNT];

} hdrscmcsna_srv_opt_attrib_type;

/* Burst Type Attribute type */
typedef struct
{
  uint8   burst_type_count;

  /* This structure consumes over 1K memory!!! 
   * No variables are defined since it's not supported yet */
  struct
  {
    uint8 burst_type;

    uint8 addr_count;   /* Broadcast Address Count: 2 bits */

    struct
    {
      uint8 len;        /* Broadcast Address length */
      uint8 addr[HDRSCMCSNA_MAX_BCAST_ADDR_LEN];  /* Broadcast Address */

    } bc_addr[HDRSCMCSNA_MAX_BCAST_ADDR_COUNT];

  } burst[HDRSCMCSNA_MAX_BURST_TYPE_COUNT];

} hdrscmcsna_burst_type_attrib_type;

/* Order Code Attribute type */
typedef struct
{
  uint8   order_count;

  struct
  {
    uint8 order_code;
    boolean ord_qual_incl;  /* Order Qualificatiion Code Included */
    uint8 ord_qual;         /* Order Qualification Code */
  } order[HDRSCMCSNA_MAX_ORDER_CODE_COUNT];

} hdrscmcsna_order_code_attrib_type;


/*===========================================================================
 
                        DECLARATIONS FOR MODULE

  This section contains macros, types, and other items needed by this module.

===========================================================================*/

/* Struct encapsulating all the configuration attributes for CSNA */
typedef struct
{
  /* IdleTunnelExponent Attribute */
//  uint16                              idle_tunnel_exp;

  /* RAND */
  uint32                              rand;

  /* MOB_P_REV */
  uint8                               mob_p_rev;

  /* AllowedReverseLinkMessages Attribute */
  hdrscmcsna_allowed_msgs_attrib_type rev_msgs;

  /* AllowedForwardLinkMessages Attribute */
  hdrscmcsna_allowed_msgs_attrib_type fwd_msgs;

  /* ServiceOptionIdle Attribute */
//  hdrscmcsna_srv_opt_attrib_type      so_idle;

  /* ServiceOptionConnected Attribute */
//  hdrscmcsna_srv_opt_attrib_type      so_conn;

  /* BurstTypeIdle Attribute */
//  hdrscmcsna_burst_type_attrib_type   burst_idle;

  /* BurstTypeConnected Attribute */
//  hdrscmcsna_burst_type_attrib_type   burst_conn;

  /* OrderCodeReverse Attribute */
//  hdrscmcsna_order_code_attrib_type   order_rev;

  /* OrderCodeForward Attribute */
//  hdrscmcsna_order_code_attrib_type   order_fwd;

} hdrscmcsna_config_struct_type;

/* Version number array for the configuration attributes */
typedef uint8 hdrscmcsna_verno_type[HDRSCMCSNA_NUM_CONFIG_ATTRIBS];



 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_POWERUP_INIT

DESCRIPTION
  This function initializes the HDRSCMCSNA variables.
  It also requests the supported AllowedForward/ReverseLinkMessages and 
  MOB_P_REV attribute values from CM/HOMER.

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrscmcsna_powerup_init(void);

#ifdef FEATURE_HDR_CSNA
 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_GET_AT_SUPPORTED_ATTRIB_FROM_HOMER

DESCRIPTION
  This function udpates the supported AllowedForward/ReverseLinkMessages and 
  MOB_P_REV attribute values from CM/HOMER. If this message configuration
  information is received while CSNA config is pending, we will start the
  CSNA configuration here.

DEPENDENCIES
  None.

PARAMETERS
  config_ptr - Pointer to the AT supported CSNA attributes from CM/HOMER.
  
RETURN VALUE
  None.
    
SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrscmcsna_get_at_supported_attrib_from_homer
(
  hdrmc_csna_msg_config_type  *config_ptr
);
#endif /* FEATURE_HDR_CSNA */

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_PROCESS_COMPLEX_ATTRIB

DESCRIPTION
  This is a generic interface to process the configuration request messages of
  all complex attributes of CSNA.

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.
  subtype  - Negotiated subtype of CSNA

RETURN VALUE
  E_SUCCESS      - If a valid complex attribute was parsed successfully.
  E_NO_DATA      - If the config request message does not have enough data.
  E_OUT_OF_RANGE - If the config request message does not have any valid data.
  E_DATA_INVALID - If the given attrib_id is not valid.
  E_NOT_ALLOWED  - If the Message ID Count is not allowed

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrscmcsna_process_complex_attrib
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  uint16                        const          subtype
);

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_IS_ATTRIB_SIMPLE

DESCRIPTION
  This function is used by SCM to query if an attribute is simple, given the
  attribute ID.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id - Attribute ID 
  subtype   - Subtype negotiated for AMAC 

RETURN VALUE
  TRUE - if the attribute given by 'attrib_id' is simple.
  FALSE - otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrscmcsna_is_attrib_simple
(
  uint16 const attrib_id,
  uint16 const subtype
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_IS_SIMPLE_ATTRIB_VALID

DESCRIPTION
  This function is used by SCM to query if the given 'attrib_value' is a 
  valid value for the attribute specified by 'attrib_id'.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id    - Attribute ID of the simple attribute.
  attrib_value - Value of the simple attribute. 
  subtype      - Subtype Negotiated

RETURN VALUE
  E_SUCCESS          - If the attrib_value is valid.
  E_OUT_OF_RANGE     - Otherwise.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern hdrerrno_enum_type hdrscmcsna_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value,
  uint16 const subtype
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_GET_SIMPLE_ATTRIB_SIZE

DESCRIPTION
  This function is used by SCM to get the size of a simple attribute value
  in bits. 
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id    - Attribute ID of the simple attribute.
  attrib_value - Value of the simple attribute. 
  subtype      - Subtype Negotiated

RETURN VALUE
  E_SUCCESS          - If the attrib_value is valid.
  E_OUT_OF_RANGE     - Otherwise.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern uint8 hdrscmcsna_get_simple_attrib_size
(
  uint16 const attrib_id,
  uint16 const subtype
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_GET_ATTRIB_OFFSET_AND_SIZE

DESCRIPTION
  This function computes the byte offset of the attribute specified by the 
  attrib_id in the struct hdrscmcsna_config_struct_type and stores it in 
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
extern hdrerrno_enum_type hdrscmcsna_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_GET_ATTRIB_VERNO_INDEX

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
extern hdrerrno_enum_type hdrscmcsna_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *index,
  uint16 subtype
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_GET_RAND

DESCRIPTION
  This function gets the Rand value

DEPENDENCIES
  None

PARAMETERS
  None.
  
RETURN VALUE
  Returns the Rand value

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 hdrscmcsna_get_rand ( void );


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  Set the configuration attributes to its default values.

DEPENDENCIES
  None

PARAMETERS
  hdramac_cfg - Pointer to the config structure.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrscmcsna_set_config_to_default
(
  hdrscmcsna_config_struct_type *hdrcsna_cfg
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_PROCESS_CONFIG

DESCRIPTION
  This function is to propose AllowedForwardLinkMessages attribute 
  as part of AT-init session configuration.

DEPENDENCIES
  None

PARAMETERS
  None.
   
RETURN VALUE
  TRUE - If the a Rsp timer was available for use.
  FALSE - otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrscmcsna_process_config( void );



/* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_GET_PROP_ATTRIB_VALUE

DESCRIPTION
  This function is used to get the proposed complex attribute value

DEPENDENCIES
  None

PARAMETERS
  trans_id - Transaction ID
  a_aip - Attribute Information pointer 
     
RETURN VALUE
  E_SUCCESS - If the transaction ID is valid.
  E_FAILURE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern hdrerrno_enum_type hdrscmcsna_get_prop_attrib_value
(
  uint16 const trans_id,
  hdrscm_accepted_attrib_type *a_aip,
  uint16 const subtype
);


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSCMCSNA_FAIL_CURR_CONFIG_NEG

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
extern void hdrscmcsna_fail_curr_config_neg
(
  hdrhai_session_close_reason_enum_type close_reason
);


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSCMCSNA_PROCESS_ATTRIB_CHANGED

DESCRIPTION
  This function udpates the CM with the attribute values.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrscmcsna_process_attrib_changed (void);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_IS_ATTRIB_GAUPABLE

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
extern hdrerrno_enum_type hdrscmcsna_is_attrib_gaupable
(
  uint16 const attrib_id,
  uint16 const subtype
);


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_ATTRIB_IS_SUPPORTED

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
extern boolean hdrscmcsna_attrib_is_supported
(
  uint16 const attrib_id,
  uint16 const subtype
);

#endif /* HDRSCMCSNA_H */

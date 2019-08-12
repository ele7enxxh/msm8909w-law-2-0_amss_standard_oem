/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

        R O U T E  U P D A T E  P R O T O C O L  H E A D E R  F I L E

       
GENERAL DESCRIPTION
  This module provides supporting API for SCM module to handle AT-init Session
  Negotiation for SupportedCDMAChannel Attribute of RouteUpdateProtocol. 
    
EXTERNALIZED FUNCTIONS
    
  hdrscmrup_is_attrib_simple 

  hdrscmrup_is_simple_attrib_valid

  hdrscmrup_get_attrib_offset_and_size

  hdrscmrup_get_attrib_verno_index

  hdrscmrup_prop_supp_cdma_chan_attrib
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

  
  Copyright (c) 2005 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/13/07   sju     Added support for MC RUP negotiation
02/17/07   wsh     Added value_id/attrib_id checking processing ConfigResp
12/07/05   etv     Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/

#include "dsm.h"
#include "comdef.h"
#include "err.h"

#include "hdrrupc.h"
#include "hdrscm.h"
#include "hdrscmdb.h"


/*===========================================================================

                    DATA DECLARATIONS

===========================================================================*/
#define HDRSCMRUP_SUCC_BAND_CLASS_CNT_SIZE          8
#define HDRSCMRUP_SUCC_BAND_CLASS_SIZE              5
#define HDRSCMRUP_SUCC_BAND_SUB_CLASS_CNT_SIZE      8
#define HDRSCMRUP_SUCC_BAND_SUB_CLASS_SIZE          8
#define HDRSCMRUP_SUCC_INTER_FLEX_DUPLEX_TAG_SIZE   8
#define HDRSCMRUP_SUCC_INTRA_FLEX_DUPLEX_FLAG_SIZE  1

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRUP_IS_ATTRIB_SIMPLE

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
extern boolean hdrscmrup_is_attrib_simple
(
  uint16 const attrib_id,
  uint16 const subtype
);


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRUP_GET_ATTRIB_OFFSET_AND_SIZE

DESCRIPTION
  To get the attribute's byte offset into the config struct and its size.

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
extern hdrerrno_enum_type hdrscmrup_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
);

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRUP_GET_PROPOSED_ATTRIB_VALUE

DESCRIPTION
  This function is used by SCM to get the complex attribute value proposed by
  AT in the recent transaction.

DEPENDENCIES
  None.

PARAMETERS
  trans_id      - Transaction ID.
  a_aip         - Array of attribute information pointers where the info about
                  each of the attributes in the transaction specified by 
                  trans_id are stored.

RETURN VALUE
  E_SUCCESS - If the given transaction ID is valid.
  E_FAILURE - If a valid transaction specified by 'trans_id' is not the recent.

SIDE EFFECTS
  None.

===========================================================================*/
extern hdrerrno_enum_type hdrscmrup_get_proposed_attrib_value
(
  uint16 const trans_id,
  hdrscm_accepted_attrib_type *a_aip,
  uint16 const subtype
);


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSCMRUP_FAIL_CURR_CONFIG_NEG

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
extern void hdrscmrup_fail_curr_config_neg
(
  hdrhai_session_close_reason_enum_type close_reason
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSCMRUP_START_CONFIG

DESCRIPTION
  This function is used to start RUP configuration during AT Init phase 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrscmrup_start_config(void);

/*===========================================================================

FUNCTION HDRSCMRUP_GET_SIMPLE_ATTRIB_SIZE

DESCRIPTION
  This function is used by SCM to get the size of a simple attribute value
  in bits. 
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id    - Attribute ID of the simple attribute.
  subtype      - Subtype Negotiated

RETURN VALUE
  attrib_size  - size of simple attribute 

SIDE EFFECTS
  None.
  
===========================================================================*/
extern uint8 hdrscmrup_get_simple_attrib_size
(
  uint16 const attrib_id,
  uint16 const subtype
);

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

                      C I R C U I T   S E R V I C E S   

              N O T I F I C A T I O N   A P P L I C A T I O N

       
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

  
  Copyright (c) 2006, 2007 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrscmcsna.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $                    
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/29/13   sat     Reduced F3 messages (or/and) ASSERTs
10/27/09   pba     Klockwork fixes.
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
08/22/07   etv     Fixed high lint warnings.
06/14/07   wsh     Moved value_id comparision to SCM instead of individual 
                   modules when handling get_prop_attrib()
05/24/07   yll     Check the MOB_P_REV value received from CM. Not include 
                   MOB_P_REV in AT's ConfigRequest if it's default of 6.
03/23/07   yll     Send HOMER the CSNA config information during power up.                   
03/09/07   yll     Clean up the HOMER interface.
02/17/07   wsh     Added value_id/attrib_id checking processing ConfigResp
02/05/07   etv     Added support to handle unknown attributes.
01/17/07   yll     Changes for HOMER interface.
01/08/07   yll     Created hdrscmcsna module to support CSNA config attribs.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "hdr_variation.h"
#include "dsm.h"
#include "comdef.h"
#include "err.h"
#include "cm.h"

#ifdef FEATURE_CMI
#include "cm_v.h"
#endif

#include "cmxll.h"
#include "homer.h"
#include "hdrdebug.h"
#include "hdrdsm.h"
#include "hdrbit.h"
#include "hdrutil.h"
#include "hdrhai.h"
#include "hdrtrace.h"
#include "hdrstream.h"
#include "hdrhmp.h"

#include "hdrscmdb.h"
#include "hdrscm.h"
#include "hdrscmcsna.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
/* For complex attributes */
#define HDRSCMCSNA_ATTRIB_ID_SIZE                   8
#define HDRSCMCSNA_VALUE_ID_SIZE                    8

/* Allowed Reverse/Forward Link Messages Attribute */
#define HDRSCMCSNA_ALLOWED_MSGS_ATTRIB_MIN_SIZE     8
#define HDRSCMCSNA_MSG_ID_COUNT_SIZE                8
#define HDRSCMCSNA_CSCH_MSG_RESERVED_SIZE           2
#define HDRSCMCSNA_CSCH_MSG_ID_SIZE                 6
#define HDRSCMCSNA_DSCH_MSG_ID_SIZE                 8

/* Service Option Idle / Connected Attribute */
#define HDRSCMCSNA_SRV_OPT_ATTRIB_MIN_SIZE          8
#define HDRSCMCSNA_SRV_OPT_COUNT_SIZE               8
#define HDRSCMCSNA_SRV_OPT_SIZE                     16

/* Burst Type Idle / Connected Attribute */
#define HDRSCMCSNA_BURST_TYPE_ATTRIB_MIN_SIZE       8
#define HDRSCMCSNA_BURST_MIN_SIZE                   8
#define HDRSCMCSNA_BCAST_ADDR_MIN_SIZE              4

#define HDRSCMCSNA_BURST_TYPE_COUNT_SIZE            8
#define HDRSCMCSNA_BURST_TYPE_SIZE                  6
#define HDRSCMCSNA_BCAST_ADDR_COUNT_SIZE            2
#define HDRSCMCSNA_BCAST_ADDR_LEN_SIZE              4
#define HDRSCMCSNA_BCAST_ADDR_SIZE                  8

/* Order Code Forward/Reverse Attribute */
#define HDRSCMCSNA_ORDER_CODE_ATTRIB_MIN_SIZE       8
#define HDRSCMCSNA_ORDER_CODE_MIN_SIZE              8

#define HDRSCMCSNA_ORDER_CODE_COUNT_SIZE            8
#define HDRSCMCSNA_ORDER_RESERVED_SIZE              1
#define HDRSCMCSNA_ORDER_CODE_SIZE                  6
#define HDRSCMCSNA_ORDER_QUAL_INCL_SIZE             1
#define HDRSCMCSNA_ORDER_QUAL_CODE_SIZE             8

/*------------------------------------------------------------------
              Default Values for Configuration Attributes
-------------------------------------------------------------------*/
#define HDRSCMCSNA_DEFAULT_MSG_ID_COUNT             0x00
#define HDRSCMCSNA_DEFAULT_SRV_OPT_COUNT            0x00
#define HDRSCMCSNA_DEFAULT_BURST_TYPE_COUNT         0x00
#define HDRSCMCSNA_DEFAULT_BCAST_ADDR_COUNT         0x00
#define HDRSCMCSNA_DEFAULT_ORDER_CODE_COUNT         0x00

/*------------------------------------------------------------------
              Valid Ranges for Configuration Attributes
-------------------------------------------------------------------*/

/*===========================================================================
 
                  DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
/* Finds if the given attribute Id is of a simple attribute */
#define HDRSCMCSNA_IS_ATTRIB_SIMPLE(attrib_id)  \
        ((((attrib_id) & 0x00F0) == 0x00F0)? TRUE: FALSE)

/* Get the attribute's index in Table from the attribute ID.
 * Simple attribute, index = 0x0F - attrib_id.
 * Complex attribute, index = attrib_id + 2.
 */
#define HDRSCMCSNA_SIMPLE_ATTRIB_ID_TO_IND(attrib_id) \
        (0xFF - (attrib_id))

#define HDRSCMCSNA_COMPLEX_ATTRIB_ID_TO_IND(attrib_id) \
        ((attrib_id) + 2)

#define HDRSCMCSNA_ATTRIB_ID_TO_IND(attrid_id) \
        (HDRSCMCSNA_IS_ATTRIB_SIMPLE(attrib_id) ? \
         HDRSCMCSNA_SIMPLE_ATTRIB_ID_TO_IND(attrib_id) : \
         HDRSCMCSNA_COMPLEX_ATTRIB_ID_TO_IND(attrib_id))
        
/*------------------------------------------------------------------------
                  LOOKUP TABLE FOR SIMPLE ATTRIBUTES
 -----------------------------------------------------------------------*/
/* Some simple attribute, for example, IdleTunnelExponent, has 
 * discontinuous value arranges, i.e. 0x0000-0x00FE, 0xFFFF. This
 * defines the number of discontinuous value ranges for the
 * simple attribute. */
#define HDRSCMCSNA_SIMPLE_ATTRIB_NUM_RANGES     2

/* This enum lists all the simple attributes, index to the
 * lookup table. */
typedef enum
{
  HDRSCMCSNA_IDLE_TUN_EXP_IND  = 0,
  HDRSCMCSNA_RAND_IND          = 1,
  HDRSCMCSNA_MOB_P_REV_IND     = 2,

  HDRSCMCSNA_NUM_SIMPLE_ATTRIBS = HDRSCMCSNA_MOB_P_REV_IND + 1

} hdrscmcsna_simple_attrib_ind_type;

/* This structure defines everything needed for a simple attribute */
typedef struct
{
  uint8   index;        
    /* Index to the simple attribute look up table, also the index
     * to hdrscmcsna_config_struct_type */
  uint16  attrib_id;    
  uint8   value_size;   /* 8: 8 bit, 16: 16 bits, 32: 32 bits */
  uint8   num_ranges;
  struct range_type
  {
    uint32  min;  /* Start of value range */
    uint32  max;  /* End of value range */
  } range[HDRSCMCSNA_SIMPLE_ATTRIB_NUM_RANGES];

  uint32  default_val;

} hdrscmcsna_simple_attrib_type;

static hdrscmcsna_simple_attrib_type 
hdrscmcsna_simple_attribs[HDRSCMCSNA_NUM_SIMPLE_ATTRIBS] =
{
  {
    HDRSCMCSNA_IDLE_TUN_EXP_IND,
    HDRSCMCSNA_IDLE_TUNNEL_EXPONENT_ID,
    16, /* 16 bits */
    2,  /* Number of value ranges */
    {
      {0x00000000, 0x000000FE},
      {0x0000FFFF, 0x0000FFFF}
    },
    0x00000000  /* default */
  },
  {
    HDRSCMCSNA_RAND_IND,
    HDRSCMCSNA_RAND_ID,
    32, /* 32 bits */
    1,  /* Number of value range */
    {
      {0x00000000, 0xFFFFFFFF},
      {0xFFFFFFFF, 0xFFFFFFFF}  /* No meaning */
    },
    0x00000000  /* default */
  },
  {
    HDRSCMCSNA_MOB_P_REV_IND,
    HDRSCMCSNA_MOB_P_REV_ID,
    8,  /* 8 bits */
    1,  /* Number of value range */
    {
      {0x00000006, 0x0000000B},
      {0x0000000B, 0x0000000B}  /* No meaning */
    },
    0x00000006
  }
};

/*------------------------------------------------------------------------
                  GAUPABLE ATTRIBUTES
 -----------------------------------------------------------------------*/

/* Total Gaupable attributes */
#define HDRSCMCSNA_NUM_GAUPABLE_ATTRIBS                3

static uint8 const 
  hdrscmcsna_gaup_attrib_ids[HDRSCMCSNA_NUM_GAUPABLE_ATTRIBS] = 
{
  /* Simple Attributes */
  0xFE,   /* 0.  RAND */

  /* Complex Attributes */
  0x01,   /* 1.  AllowedReverseLinkMessages */

  0x07    /* 2.  OrderCodeReverse */
};


/*------------------------------------------------------------------------
                            GLOBAL VARIABLES
 -----------------------------------------------------------------------*/

static struct
{
  /* Attribute AT-supported config info is received from CM. */
  boolean                             config_info_received;

  /* AT supported config attribute values to be proposed in 
   * AT initiated session negotiation */
  hdrscmcsna_config_struct_type       at_supported;

  /* Current in-use configuration */
  hdrscmcsna_config_struct_type       inuse;

  /* In-config instance */
  hdrscmcsna_config_struct_type       inconfig;

  /* Transaction ID for AT-init configuration */
  uint8                               trans_id;
  
  uint8                               allow_fwd_value_id;
  uint8                               allow_rev_value_id;  
} hdrscmcsna;



/*===========================================================================

                         LOCAL FUNCTION DEFINITIONS
                    
===========================================================================*/

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_LOG_MSG_ID

DESCRIPTION
  This function prints out the message IDs in Allowed Forward/Reverse 
  Link Messages attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_ptr - Pointer to the AllowedFor/RevMessages attributes
  is_forward_link - If it's the Forward or Reverse Link message attributes

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrscmcsna_log_msg_id
(
  hdrscmcsna_allowed_msgs_attrib_type   *attrib_ptr,
  boolean                               is_forward_link
)
{
  int     i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (is_forward_link)
  {
    HDR_MSG_PROT(MSG_LEGACY_MED, "--AllowedForwardLinkMessages--");
  }
  else
  {
    HDR_MSG_PROT(MSG_LEGACY_MED, "--AllowedReverseLinkMessages--");
  }

  ASSERT((attrib_ptr->csch_msg_id_count <= HDRSCMCSNA_MAX_MSG_ID_COUNT) &&
         (attrib_ptr->dsch_msg_id_count <= HDRSCMCSNA_MAX_MSG_ID_COUNT));

  for (i = 0; i < attrib_ptr->csch_msg_id_count; i++)
  {
    HDR_MSG_PROT_2(MSG_LEGACY_MED, 
                   "  csch_msg_id[%d] = 0x%x", i, attrib_ptr->csch_msg_id[i]);
  }

  for (i = 0; i < attrib_ptr->dsch_msg_id_count; i++)
  {
    HDR_MSG_PROT_2(MSG_LEGACY_MED, 
                   "  dsch_msg_id[%d] = 0x%x", i, attrib_ptr->dsch_msg_id[i]);
  }

} /* hdrscmcsna_log_msg_id */

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_IS_MSG_ID_SUPPORTED

DESCRIPTION
  This function checks whether the message ID received in Allowed 
  Forward/Reverse Link Messages is supported.
  
DEPENDENCIES
  None.

PARAMETERS
  uint8 messag_id - The message ID to be checked.
  boolean is_forward_link - If this is a forward link message.
  boolean is_csch_msg     - If this is a CSCH message or DSCH message

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.
  
===========================================================================*/
boolean hdrscmcsna_is_msg_id_supported
(
  uint8   message_id,
  boolean is_forward_link,
  boolean is_csch_msg
)
{
  boolean is_supported = FALSE;
  uint8   *msg_id_ptr;
  uint8   msg_id_count;
  int     i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_3(MSG_LEGACY_MED, 
                 "hdrscmcsna_is_msg_id_supported-id 0x%x flink %d is_csch %d",
                 message_id, is_forward_link, is_csch_msg);

  if (is_forward_link)
  {
    if (is_csch_msg)
    {
      msg_id_ptr = &hdrscmcsna.at_supported.fwd_msgs.csch_msg_id[0];
      msg_id_count = hdrscmcsna.at_supported.fwd_msgs.csch_msg_id_count;
    }
    else
    {
      msg_id_ptr = &hdrscmcsna.at_supported.fwd_msgs.dsch_msg_id[0];
      msg_id_count = hdrscmcsna.at_supported.fwd_msgs.dsch_msg_id_count;
    }
  }
  else
  {
    if (is_csch_msg)
    {
      msg_id_ptr = &hdrscmcsna.at_supported.rev_msgs.csch_msg_id[0];
      msg_id_count = hdrscmcsna.at_supported.rev_msgs.csch_msg_id_count;
    }
    else
    {
      msg_id_ptr = &hdrscmcsna.at_supported.rev_msgs.dsch_msg_id[0];
      msg_id_count = hdrscmcsna.at_supported.rev_msgs.dsch_msg_id_count;
    }
  }

  for (i = 0; i < msg_id_count; i++, msg_id_ptr++)
  {
    if (message_id == *msg_id_ptr)
    {
      is_supported = TRUE;
      break;
    }
  }

  if (!is_supported)
  {
    HDR_MSG_PROT_3(MSG_LEGACY_ERROR, 
                   "For/Rev %d CSCH/DSCH %d Message ID 0x%x not supported", 
                   is_forward_link, is_csch_msg, message_id);
  }
  else
  {
    HDR_MSG_PROT_1(MSG_LEGACY_MED, "Message ID 0x%x supported", message_id);
  }

  return is_supported;

} /* hdrscmcsna_is_message_id_supported */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_IS_ALLOWED_REV_LINK_MSGS_VALID

DESCRIPTION
  This function checks whether the received Allowed Forward/Reverse Link 
  Messages are supported.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id    - Attribute ID.
  attrib_ptr   - Pointer to the Allowed Message attribute being checked 

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.
  
===========================================================================*/
boolean hdrscmcsna_is_allowed_msgs_valid
(
  uint16  attrib_id,
  hdrscmcsna_allowed_msgs_attrib_type  *attrib_ptr
)
{
  boolean is_valid = TRUE;
  int     i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT(MSG_LEGACY_MED, 
          "hdrscmcsna_is_allowed_msgs_valid. Message ID passed in: -");

  switch (attrib_id)
  {
    case HDRSCMCSNA_ALLOWED_REV_LINK_MESSAGES_ID:

      /* If AN doesn't allow AT to send even Origination message, CSNA
       * session can't continue. But what should AT do? */
      if ( attrib_ptr->csch_msg_id_count == 0 )
      {
        HDR_MSG_PROT(MSG_LEGACY_ERROR, "AN doesn't allow ORM!!!");
      }
      else
      {
        hdrscmcsna_log_msg_id(attrib_ptr, FALSE);
      }

      /* In other cases, even if AN sends the AllowedReverseLinkMessages
       * that AT doesn't support, AT can just ignore these messages. */

      HDR_MSG_PROT(MSG_LEGACY_MED, 
                   "AT accepts all AllowedReveresLinkMessages");
      break;

    case HDRSCMCSNA_ALLOWED_FOR_LINK_MESSAGES_ID:

      hdrscmcsna_log_msg_id(attrib_ptr, TRUE);

      /* Verify that message IDs in the AllowedForwardLinkMessage can 
       * be supported by AT */
      for (i = 0; i < attrib_ptr->csch_msg_id_count; i++)
      {
        if (!hdrscmcsna_is_msg_id_supported(attrib_ptr->csch_msg_id[i], 
                                            TRUE, TRUE))
        {            
          is_valid = FALSE;
          break;
        }
      }

      for (i = 0; is_valid && (i < attrib_ptr->dsch_msg_id_count); i++)
      {
        if (!hdrscmcsna_is_msg_id_supported(attrib_ptr->dsch_msg_id[i], 
                                            TRUE, FALSE))
        {            
          is_valid = FALSE;
          break;
        }
      }

      break;

    default:
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "Unsupported attrib_id %d", attrib_id);
      break;
  }

  return is_valid;

} /* hdrscmcsna_is_allowed_msgs_valid */

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_PROCESS_ALLOWED_MSGS_ATTRIB

DESCRIPTION
  This is a generic interface to process the AllowedReverseLinkMessages
  Attribute.

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.
  is_forward_link - If this is a AllowedForwardLinkMessage or 
                    AllowedReverseLinkMessage

RETURN VALUE
  E_SUCCESS      - If a valid complex attribute was parsed successfully.
  E_NO_DATA      - If the config request message does not have enough data.
  E_NOT_ALLOWED  - If the Message ID Count is not allowed

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmcsna_process_allowed_msgs_attrib
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  boolean                                      is_forward_link
)
{
  uint16 attrib_offset;
    /* Offset to the current byte being processed from the start of DSM chain */
  uint8 value_id;
    /* Value ID of the complex attribute */
  hdrscmcsna_allowed_msgs_attrib_type   *attrib_ptr;
    /* Pointer of the parsed AllowedReverse/ForwardMessages attribute */
  hdrerrno_enum_type err_no = E_NO_DATA;
    /* Status Code */
  int   i;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  if (is_forward_link)
  {
    attrib_ptr = &hdrscmcsna.inconfig.fwd_msgs;
  }
  else
  {
    attrib_ptr = &hdrscmcsna.inconfig.rev_msgs;
  }

  /* Initialize the attribute offset to start of first Value ID */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
     set of this Attribute Value */
  while ( (attrib_offset + p_aip->value_id_size + 
           HDRSCMCSNA_ALLOWED_MSGS_ATTRIB_MIN_SIZE ) <= p_aip->attrib_len )
  {
    /*----------------------------------------------------------------------
       There is at least one more attribute value in this attribute Record
     *--------------------------------------------------------------------*/
    /* Unpack the Value ID into a uint8 */
    err_no = HDRBIT_UNPACK8( item_ptr,
                              &value_id,
                              (p_aip->attrib_offset + attrib_offset),
                              p_aip->value_id_size );
    attrib_offset += p_aip->value_id_size ;

    /* Unpack R-CSCH Mesage ID Count */
    err_no |= HDRBIT_UNPACK8( item_ptr,
                              &attrib_ptr->csch_msg_id_count,
                              (p_aip->attrib_offset + attrib_offset),
                              HDRSCMCSNA_MSG_ID_COUNT_SIZE );
    attrib_offset += HDRSCMCSNA_MSG_ID_COUNT_SIZE;

    if ( attrib_ptr->csch_msg_id_count > HDRSCMCSNA_MAX_MSG_ID_COUNT )
    {
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
                     "CSNA: AllowedRevLinkMsg cschMsgIDCount %d > max %d", 
                     attrib_ptr->csch_msg_id_count,
                     HDRSCMCSNA_MAX_MSG_ID_COUNT);
      err_no = E_NOT_ALLOWED;
      break;
    }

    for ( i = 0; i < attrib_ptr->csch_msg_id_count; i++ )
    {
      /* Skip the reserved field */
      attrib_offset += HDRSCMCSNA_CSCH_MSG_RESERVED_SIZE;

      /* Unpack CSCH Message ID field */
      err_no |= HDRBIT_UNPACK8( item_ptr,
                                &attrib_ptr->csch_msg_id[i],
                                (p_aip->attrib_offset + attrib_offset),
                                HDRSCMCSNA_CSCH_MSG_ID_SIZE );
      attrib_offset += HDRSCMCSNA_CSCH_MSG_ID_SIZE;
    }

    /* Unpack DSCH Message ID Count */
    err_no |= HDRBIT_UNPACK8( item_ptr,
                              &attrib_ptr->dsch_msg_id_count,
                              (p_aip->attrib_offset + attrib_offset),
                              HDRSCMCSNA_MSG_ID_COUNT_SIZE );
    attrib_offset += HDRSCMCSNA_MSG_ID_COUNT_SIZE;

    if ( attrib_ptr->dsch_msg_id_count > HDRSCMCSNA_MAX_MSG_ID_COUNT )
    {
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
                     "CSNA: AllowedRevLinkMsg dschMsgIDCount %d > max %d", 
                     attrib_ptr->dsch_msg_id_count,
                     HDRSCMCSNA_MAX_MSG_ID_COUNT);
      err_no = E_NOT_ALLOWED;
      break;
    }

    for ( i = 0; i < attrib_ptr->dsch_msg_id_count; i++ )
    {
      /* Unpack DSCH Message ID field */
      err_no |= HDRBIT_UNPACK8( item_ptr,
                                &attrib_ptr->dsch_msg_id[i],
                                (p_aip->attrib_offset + attrib_offset),
                                HDRSCMCSNA_DSCH_MSG_ID_SIZE );
      attrib_offset += HDRSCMCSNA_DSCH_MSG_ID_SIZE;
    }

    if ( err_no != E_SUCCESS )
    {
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                     "CSNA: AllowedRev/ForLinkMsg attrib parse failed %d",
                     err_no);
      break;
    }

    /* Parsing done! Check if the parsed attribute is valid */
    if ( hdrscmcsna_is_allowed_msgs_valid(p_aip->attrib_id, attrib_ptr))
    {
      /* Update the accepted attribute information */
      a_aip->is_accepted = TRUE;
      a_aip->value_id = value_id;
      a_aip->complex_val_ptr = (void*) attrib_ptr;
      a_aip->complex_val_size = sizeof(hdrscmcsna_allowed_msgs_attrib_type);
      break;
    }

    /* Otherwise, try next set of values if there is one.*/

  } /* while */

  return err_no;

} /* hdrscmcsna_process_allowed_msgs_attrib */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_PACK_ALLOWED_MSGS_ATTRIB

DESCRIPTION
  This function is to pack AllowedForward/ReverseLinkMessages attribute.

DEPENDENCIES
  None

PARAMETERS
  None.
   
RETURN VALUE
  E_SUCCESS      - If the attribute was packed successfully.
  
SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrscmcsna_pack_allowed_msgs_attrib
( 
  dsm_item_type *send_msg_ptr,
  uint16        *a_offset,
  boolean       is_forward_link
)
{
  uint8   value_id = 0;
    /* Value ID */

  uint16  attrib_offset = *a_offset;

  int     i;

  hdrscmcsna_allowed_msgs_attrib_type   *attrib_ptr;
  uint8 attrib_len;
  uint8 attrib_id;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( is_forward_link )
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                 "CSNA: AT Pack AllowedForwardLinkMessages");

    /* Get supported AllowedForwardMessages attribute from CM */
    attrib_ptr = &hdrscmcsna.at_supported.fwd_msgs;

    /* Attribute length is in octets */
    attrib_len = 
      (attrib_ptr->csch_msg_id_count + attrib_ptr->dsch_msg_id_count + 4);
    attrib_id = HDRSCMCSNA_ALLOWED_FOR_LINK_MESSAGES_ID;
    value_id = ++hdrscmcsna.allow_fwd_value_id;
  }
  else
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                 "CSNA: AT Pack AllowedReverseLinkMessages");

    /* Get supported AllowedReverseMessages attribute from CM */
    attrib_ptr = &hdrscmcsna.at_supported.rev_msgs;

    attrib_len = 
      (attrib_ptr->csch_msg_id_count + attrib_ptr->dsch_msg_id_count + 4);
    attrib_id = HDRSCMCSNA_ALLOWED_REV_LINK_MESSAGES_ID;
    value_id = ++hdrscmcsna.allow_rev_value_id;
  }

  /* Pack the length */
  HDRBIT_PACK8(send_msg_ptr, attrib_len,
               attrib_offset, HDRSCM_LENGTH_SIZE);

  /* Shift the offset to start of AttributeID field */
  attrib_offset += HDRSCM_LENGTH_SIZE;

  HDRBIT_PACK8(send_msg_ptr, attrib_id, attrib_offset, 
               HDRSCMCSNA_ATTRIB_ID_SIZE);

  /* Shift the offset to start of Value ID field */
  attrib_offset += HDRSCMCSNA_ATTRIB_ID_SIZE;

  /* Pack Value ID */
  HDRBIT_PACK8(send_msg_ptr, value_id, attrib_offset, 
               HDRSCMCSNA_VALUE_ID_SIZE);

  attrib_offset += HDRSCMCSNA_VALUE_ID_SIZE;

  /* Pack f/r-cschMessageIDCount */
  HDRBIT_PACK8(send_msg_ptr, attrib_ptr->csch_msg_id_count,
               attrib_offset, HDRSCMCSNA_MSG_ID_COUNT_SIZE);

  attrib_offset += HDRSCMCSNA_MSG_ID_COUNT_SIZE;

  for ( i = 0; i < attrib_ptr->csch_msg_id_count; i++ )
  {
    /* Pack Reserved field */
    HDRBIT_PACK8(send_msg_ptr, 0,
                 attrib_offset, HDRSCMCSNA_CSCH_MSG_RESERVED_SIZE);

    attrib_offset += HDRSCMCSNA_CSCH_MSG_RESERVED_SIZE;

    /* Pack r-cschMessageIDs */
    HDRBIT_PACK8(send_msg_ptr, attrib_ptr->csch_msg_id[i],
                 attrib_offset, HDRSCMCSNA_CSCH_MSG_ID_SIZE);

    attrib_offset += HDRSCMCSNA_CSCH_MSG_ID_SIZE;
  }

  /* Pack f/r-dschMessageIDCount */
  HDRBIT_PACK8(send_msg_ptr, attrib_ptr->dsch_msg_id_count,
               attrib_offset, HDRSCMCSNA_MSG_ID_COUNT_SIZE);

  attrib_offset += HDRSCMCSNA_MSG_ID_COUNT_SIZE;

  /* Pack f/r-dschMessageIDs */
  for ( i = 0; i < attrib_ptr->dsch_msg_id_count; i++ )
  {
    HDRBIT_PACK8(send_msg_ptr, attrib_ptr->dsch_msg_id[i],
                 attrib_offset, HDRSCMCSNA_DSCH_MSG_ID_SIZE);

    attrib_offset += HDRSCMCSNA_DSCH_MSG_ID_SIZE;
  }

  /* Set the a_offset to the new location */
  *a_offset = attrib_offset;

  /* Log the packed allowed message IDs */
  hdrscmcsna_log_msg_id(attrib_ptr, is_forward_link);

  return E_SUCCESS;

} /* hdrscmcsna_pack_allowed_msgs_attrib */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_PACK_MOB_P_REV_ATTRIB

DESCRIPTION
  This function is to pack MOB_P_REV attribute.

DEPENDENCIES
  None

PARAMETERS
  None.
   
RETURN VALUE
  E_SUCCESS      - If the attribute was packed successfully.
  
SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrscmcsna_pack_mob_p_rev_attrib
( 
  dsm_item_type *send_msg_ptr,
  uint16        *a_offset
)
{
  uint16  attrib_offset = *a_offset;
  uint8   index;
  uint8   attrib_len;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT(MSG_LEGACY_HIGH, "Pack MOB_P_REV attrib");

  index = HDRSCMCSNA_SIMPLE_ATTRIB_ID_TO_IND(HDRSCMCSNA_MOB_P_REV_ID);

  /* Pack the length */
  attrib_len = ( HDRSCMCSNA_ATTRIB_ID_SIZE + 
                 hdrscmcsna_simple_attribs[index].value_size ) / 8;

  HDRBIT_PACK8(send_msg_ptr, attrib_len,
               attrib_offset, HDRSCM_LENGTH_SIZE);

  attrib_offset += HDRSCM_LENGTH_SIZE;

  /* Attrib ID field */
  HDRBIT_PACK8( send_msg_ptr, HDRSCMCSNA_MOB_P_REV_ID,
                attrib_offset, HDRSCMCSNA_ATTRIB_ID_SIZE);

  attrib_offset += HDRSCMCSNA_ATTRIB_ID_SIZE;

  /* Pack MOB_P_REV Attrib value - Put 7 here for now.
   * Need to get the value from 1x side */
  HDRBIT_PACK8(send_msg_ptr, hdrscmcsna.at_supported.mob_p_rev, attrib_offset,
               hdrscmcsna_simple_attribs[index].value_size);

  attrib_offset += hdrscmcsna_simple_attribs[index].value_size;

  *a_offset = attrib_offset;

  HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "CSNA: AT proposed MOB_P_REV %d",
                 hdrscmcsna.at_supported.mob_p_rev );

  return E_SUCCESS;

} /* hdrscmcsna_pack_mob_p_rev_attrib */



 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_ALLOWED_MSGS_CONFIG_IS_NEEDED

DESCRIPTION
  This functions compares the AT supported AllowedForward/ReverseMessages 
  attribute values against the inuse values to determine whether 
  AT-initiated configuration is needed or not

DEPENDENCIES
  None

PARAMETERS
  boolean is_forward_link - Forward or Reverse Link Messages.
   
RETURN VALUE
  TRUE - The supported values are different from inuse ones so AT needs to 
         initiate session negotiation with AN.
  No   - The supported values are the same as inuse ones so AT doesn't 
         need to initiate the session negotiation with AN.
  
SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscmcsna_allowed_msgs_config_is_needed
(
  boolean is_forward_link
)
{
  boolean                               is_config_needed = FALSE;
  hdrscmcsna_allowed_msgs_attrib_type   *sup_ptr;
  hdrscmcsna_allowed_msgs_attrib_type   *inuse_ptr;
  int                                   i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (is_forward_link)
  {
    sup_ptr = &hdrscmcsna.at_supported.fwd_msgs;
    inuse_ptr = &hdrscmcsna.inuse.fwd_msgs;
  }
  else
  {
    sup_ptr = &hdrscmcsna.at_supported.rev_msgs;
    inuse_ptr = &hdrscmcsna.inuse.rev_msgs;
  }

  /* If message Id counts doesn't match, need to do AT-init config */
  if ((sup_ptr->csch_msg_id_count != inuse_ptr->csch_msg_id_count) ||
      (sup_ptr->dsch_msg_id_count != inuse_ptr->dsch_msg_id_count)   )
  {
    is_config_needed = TRUE;
  }
  else
  {
    /* Find out if all the supported CSCH Message IDs are 
     * in the inuse instance. */
    for (i = 0; i < inuse_ptr->csch_msg_id_count; i++)
    {
      if (!hdrscmcsna_is_msg_id_supported(inuse_ptr->csch_msg_id[i], 
                                          is_forward_link, TRUE))
      {
        is_config_needed = TRUE;
        break;
      }
    }

    for (i = 0; !is_config_needed && (i < inuse_ptr->dsch_msg_id_count); i++)
    {
      if (!hdrscmcsna_is_msg_id_supported(inuse_ptr->dsch_msg_id[i],
                                          is_forward_link, FALSE))
      {
        is_config_needed = TRUE;
        break;
      }
    }
  }

  HDR_MSG_PROT_2(MSG_LEGACY_HIGH, 
                 "CSNA: Allowed For/Rev %d Message config needed? %d",
                 is_forward_link, is_config_needed);

  return is_config_needed;

} /* hdrscmcsna_allowed_msgs_config_is_needed */



 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_ALLOWED_MSGS_CONFIG_IS_NEEDED

DESCRIPTION
  This functions compares the AT's MOB_P_REV attribute values 
  against the inuse value to determine whether AT-initiated 
  configuration is needed or not. If MOB_P_REV is of default value
  6, no need to negotiate this attribute.

DEPENDENCIES
  None

PARAMETERS
  None
   
RETURN VALUE
  TRUE - The supported values are different from inuse one so AT needs to 
         initiate session negotiation with AN.
  No   - The supported values are the same as inuse one so AT doesn't 
         need to initiate the session negotiation with AN.
  
SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscmcsna_mob_p_rev_config_is_needed ( void )
{

  return ((hdrscmcsna.at_supported.mob_p_rev != hdrscmcsna.inuse.mob_p_rev)
          &&
          (hdrscmcsna.at_supported.mob_p_rev != 
           hdrscmcsna_simple_attribs[HDRSCMCSNA_MOB_P_REV_IND].default_val)
         );

} /* hdrscmcsna_mob_p_rev_config_is_needed */



 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_CONFIG_IS_NEEDED

DESCRIPTION
  This functions compares the AT supported attribute values against the 
  default values to determine whether AT-initiated configuration is 
  needed or not

DEPENDENCIES
  None

PARAMETERS
  None.
   
RETURN VALUE
  TRUE - The supported values are different from default ones so AT needs to 
         initiate session negotiation with AN.
  No   - The supported values are the same as default ones so AT doesn't 
         need to initiate the session negotiation with AN.
  
SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscmcsna_config_is_needed(void)
{

  return ( hdrscmcsna_allowed_msgs_config_is_needed(FALSE) ||
           hdrscmcsna_allowed_msgs_config_is_needed(TRUE)  ||
           hdrscmcsna_mob_p_rev_config_is_needed() );

} /* hdrscmcsna_config_is_needed */


 /* EJECT */
#ifdef FEATURE_HDR_CSNA
/*===========================================================================

FUNCTION HDRSCMCSNA_GET_AT_SUP_MSGS_FROM_HOMER

DESCRIPTION
  This function udpates the supported AllowedForward/ReverseLinkMessages
  received from CM/HOMER. 

DEPENDENCIES
  None.

PARAMETERS
  src_msg_id_count - Message ID count from source
  src_msg_config   - Message ID and config status from source
  dest_msg_id_count - Destination message ID count to be updated
  dest_msg_id      - Message ID entry from destination
  
RETURN VALUE
  None.
    
SIDE EFFECTS
  None.

===========================================================================*/
static void hdrscmcsna_get_at_sup_msgs_from_homer
(
  int     src_msg_id_count,
  homer_csna_msg_config_s_type  src_msg_config[],
  uint8   *dest_msg_id_count,
  uint8   dest_msg_id[]
)
{
  int     i, j ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( src_msg_id_count <= HDRSCMCSNA_MAX_MSG_ID_COUNT );

  for ( i = 0, j = 0; i < src_msg_id_count; i++ )
  {
    if (( src_msg_config[i].csna_msg_status ==
          HOMER_CSNA_MSG_CONFIG_STATUS_SUPPORTED ) ||
        ( src_msg_config[i].csna_msg_status ==
          HOMER_CSNA_MSG_CONFIG_STATUS_SUPPORTED_AND_CONFIGURED ))
    {
      dest_msg_id[j] = src_msg_config[i].msg_id;
      j++;
    }
  }

  *dest_msg_id_count = j;

} /* hdrscmcsna_get_at_sup_msgs_from_homer */


/*===========================================================================

FUNCTION HDRSCMCSNA_UPDATE_CONFIGED_MSGS_TO_HOMER

DESCRIPTION
  This function udpates the configured AllowedForward/ReverseLinkMessages
  to CM/HOMER.

DEPENDENCIES
  None.

PARAMETERS
  src_msg_id_count - Message ID count from source
  src_msg_id       - Message ID from source
  dest_msg_id_count - Destination message ID count to be updated
  dest_msg_config  - Message ID and config status entry from destination
  
RETURN VALUE
  None.
    
SIDE EFFECTS
  None.

===========================================================================*/
static void hdrscmcsna_update_configed_msgs_to_homer
(
  uint8   src_msg_id_count,
  uint8   src_msg_id[],
  int     *dest_msg_id_count,
  homer_csna_msg_config_s_type  dest_msg_config[]
)
{
  int     i ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( src_msg_id_count <= HDRSCMCSNA_MAX_MSG_ID_COUNT );

  *dest_msg_id_count = src_msg_id_count;

  for ( i = 0; i < src_msg_id_count; i++ )
  {
    dest_msg_config[i].msg_id = src_msg_id[i];
    dest_msg_config[i].csna_msg_status = 
      HOMER_CSNA_MSG_CONFIG_STATUS_SUPPORTED_AND_CONFIGURED;
  }

} /* hdrscmcsna_update_configed_msgs_to_homer */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSCMCSNA_UPDATE_INUSE_ATTRIB_TO_HOMER

DESCRIPTION
  This function udpates the CM/HOMER with the inuse attribute values.

DEPENDENCIES
  CSNA inuse instance must have been initialized

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscmcsna_update_inuse_attrib_to_homer (void)
{
  cm_mc_rpt_type  *cmd_ptr;    /* Pointer to CM/HOMER command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT(MSG_LEGACY_MED, "Currently AT Inuse instance --");

  hdrscmcsna_log_msg_id(&hdrscmcsna.inuse.fwd_msgs, TRUE);
  hdrscmcsna_log_msg_id(&hdrscmcsna.inuse.rev_msgs, FALSE);
  HDR_MSG_PROT_2(MSG_LEGACY_MED, "RAND 0x%x, MOB_P_REV 0x%x", 
                 hdrscmcsna.inuse.rand, hdrscmcsna.inuse.mob_p_rev);

  /* Update CM with CSNA message config value */
  cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal();

  cmd_ptr->update_csna_msg_config.hdr.cmd = CM_UPDATE_CSNA_MSG_CONFIG_F;

  /* Forward link messages updated */
  cmd_ptr->update_csna_msg_config.fwd_link_msg_update_present = TRUE;

  hdrscmcsna_update_configed_msgs_to_homer(
    hdrscmcsna.inuse.fwd_msgs.csch_msg_id_count,
    hdrscmcsna.inuse.fwd_msgs.csch_msg_id,
    &cmd_ptr->update_csna_msg_config.num_valid_cch_fwd_link_msg,
    cmd_ptr->update_csna_msg_config.cch_fwd_link_msg);

  hdrscmcsna_update_configed_msgs_to_homer(
    hdrscmcsna.inuse.fwd_msgs.dsch_msg_id_count,
    hdrscmcsna.inuse.fwd_msgs.dsch_msg_id,
    &cmd_ptr->update_csna_msg_config.num_valid_ded_fwd_link_msg,
    cmd_ptr->update_csna_msg_config.ded_fwd_link_msg);

  /* Reverse link message updated */
  cmd_ptr->update_csna_msg_config.rev_link_msg_update_present = TRUE;

  hdrscmcsna_update_configed_msgs_to_homer(
    hdrscmcsna.inuse.rev_msgs.csch_msg_id_count,
    hdrscmcsna.inuse.rev_msgs.csch_msg_id,
    &cmd_ptr->update_csna_msg_config.num_valid_cch_rev_link_msg,
    cmd_ptr->update_csna_msg_config.cch_rev_link_msg);

  hdrscmcsna_update_configed_msgs_to_homer(
    hdrscmcsna.inuse.rev_msgs.dsch_msg_id_count,
    hdrscmcsna.inuse.rev_msgs.dsch_msg_id,
    &cmd_ptr->update_csna_msg_config.num_valid_ded_rev_link_msg,
    cmd_ptr->update_csna_msg_config.ded_rev_link_msg);

  cmd_ptr->hdr.task_ptr = NULL;

  HDR_MSG_PROT(MSG_LEGACY_MED, "CSNA: Update msg config to homer" );

  cm_mc_rpt(cmd_ptr);

} /* hdrscmcsna_update_inuse_attrib_to_homer */


 /* EJECT */
/*============================================================================

                        EXTERNAL FUNCTION DEFINITIONS

=============================================================================*/


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
void hdrscmcsna_powerup_init(void)
{
  cm_mc_rpt_type  *cmd_ptr;    /* Pointer to CM/HOMER command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset( &hdrscmcsna, 0, sizeof(hdrscmcsna) );

  /* Get a copy of the in-use instance of CSNA attributes */
  hdrscmdb_get_inuse_config(HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL, 
                            &hdrscmcsna.inuse);

  /* HOMER relies on CSNA to get the AllowedFwd/RevMessageID 
   * attributes to process the payload received from 3G1XServices
   * messages. CSNA will update HOMER when a session is negotiated
   * or changed. But if AT powers up and reuses the existing 
   * session, CSNA should also update HOMER with the inuse information.
   */
  hdrscmcsna_update_inuse_attrib_to_homer();


  /* CSNA relies on HOMER to obtain the AT supported capabilities,
   * including the AllowedForward/ReverseLinkMessages. This is to
   * be used in AT init session negotiation.
   */
  HDR_MSG_PROT(MSG_LEGACY_HIGH, "CSNA: request AT-sup attribs from HOMER");

  /* We haven't received config from CM/HOMER yet */
  hdrscmcsna.config_info_received = FALSE;

  cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal();
  cmd_ptr->get_csna_msg_config.hdr.cmd = CM_GET_CSNA_MSG_CONFIG_F;
  cmd_ptr->hdr.task_ptr = NULL;

  cm_mc_rpt(cmd_ptr);

} /* hdrscmcsna_powerup_init */

  
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
void hdrscmcsna_get_at_supported_attrib_from_homer
(
  hdrmc_csna_msg_config_type  *config_ptr
)
{
  hdrscmcsna_simple_attrib_type *p_rev;
  int i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT(MSG_LEGACY_MED, "CSNA: Update AT Supported attributes");
  
  if ( hdrscmcsna.config_info_received )
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, "Unexpected Update Msg Config from HOMER");
  }
  else
  {
    hdrscmcsna.config_info_received = TRUE;
  }

  /* Check if the MOB_P_REV received from HOMER/1x is valid */
  p_rev = &hdrscmcsna_simple_attribs[HDRSCMCSNA_MOB_P_REV_IND];

  for ( i = 0; i < p_rev->num_ranges; i++ )
  {
    if ((p_rev->range[i].min <= (uint32) config_ptr->p_rev) &&
        ((uint32) config_ptr->p_rev <= p_rev->range[i].max)   )
    {
      break;
    }
  }
  if ( i == p_rev->num_ranges )
  {
    ERR_FATAL("CSNA: Recvd invalid MOB_P_REV %d from CM", 0,0,0);
    return;
  }

  hdrscmcsna.at_supported.mob_p_rev = config_ptr->p_rev;

  /* Get the AllowedFwd/RevMessages from HOMER */
  hdrscmcsna_get_at_sup_msgs_from_homer(
    config_ptr->num_valid_cch_fwd_csna_msg,
    config_ptr->cch_fwd_csna_msg,
    &hdrscmcsna.at_supported.fwd_msgs.csch_msg_id_count,
    hdrscmcsna.at_supported.fwd_msgs.csch_msg_id );

  hdrscmcsna_get_at_sup_msgs_from_homer(
    config_ptr->num_valid_ded_fwd_csna_msg,
    config_ptr->ded_fwd_csna_msg,
    &hdrscmcsna.at_supported.fwd_msgs.dsch_msg_id_count,
    hdrscmcsna.at_supported.fwd_msgs.dsch_msg_id );

  hdrscmcsna_get_at_sup_msgs_from_homer(
    config_ptr->num_valid_cch_rev_csna_msg,
    config_ptr->cch_rev_csna_msg,
    &hdrscmcsna.at_supported.rev_msgs.csch_msg_id_count,
    hdrscmcsna.at_supported.rev_msgs.csch_msg_id );

  hdrscmcsna_get_at_sup_msgs_from_homer(
    config_ptr->num_valid_ded_rev_csna_msg,
    config_ptr->ded_rev_csna_msg,
    &hdrscmcsna.at_supported.rev_msgs.dsch_msg_id_count,
    hdrscmcsna.at_supported.rev_msgs.dsch_msg_id );

  hdrscmcsna_log_msg_id(&hdrscmcsna.at_supported.rev_msgs, FALSE);
  hdrscmcsna_log_msg_id(&hdrscmcsna.at_supported.fwd_msgs, TRUE);
  HDR_MSG_PROT_1(MSG_LEGACY_MED, 
                 "MOB_P_REV=0x%x", hdrscmcsna.at_supported.mob_p_rev);

} /* hdrscmcsna_get_at_supported_attrib_from_homer */
  
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
hdrerrno_enum_type hdrscmcsna_process_complex_attrib
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  uint16                        const          subtype
)
{

  hdrerrno_enum_type err_no = E_DATA_INVALID;
    /* Error Code */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (p_aip->attrib_id)
  {
    /* AllowedReverseLinkMessages attribute */
    case HDRSCMCSNA_ALLOWED_REV_LINK_MESSAGES_ID:

      HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                   "CSNA: Process AllowedReverseLinkMessages attrib");
      err_no = hdrscmcsna_process_allowed_msgs_attrib(item_ptr, p_aip, 
                                                      a_aip, FALSE);
      break;

    /* AllowedForwardLinkMessages attribute */
    case HDRSCMCSNA_ALLOWED_FOR_LINK_MESSAGES_ID:

      HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                   "Process AllowedForwardLinkMessages attrib");
      err_no = hdrscmcsna_process_allowed_msgs_attrib(item_ptr, p_aip, 
                                                      a_aip, TRUE);
      break;

    default:

      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "CSNA: Invalid attrib ID %d ignored",
                     p_aip->attrib_id);
      err_no = E_DATA_INVALID;
      break;
  }

  return err_no;
  
} /* hdrscmcsna_process_complex_attrib */


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
boolean hdrscmcsna_is_attrib_simple
(
  uint16 const attrib_id,
  uint16 const subtype
)
{

   return HDRSCMCSNA_IS_ATTRIB_SIMPLE(attrib_id);

} /* hdrscmcsna_is_attrib_simple */


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
hdrerrno_enum_type hdrscmcsna_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value,
  uint16 const subtype
)
{
  hdrerrno_enum_type err_no = E_OUT_OF_RANGE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  HDR_MSG_PROT(MSG_LEGACY_MED, "hdrscmcsna_is_simple_attrib_valid()...");

  switch (attrib_id)
  {
    case HDRSCMCSNA_RAND_ID:

      HDR_MSG_PROT(MSG_LEGACY_MED, "AT accepts any RAND value");
      err_no = E_SUCCESS;
      break;

    case HDRSCMCSNA_IDLE_TUNNEL_EXPONENT_ID:
    case HDRSCMCSNA_MOB_P_REV_ID:
    default:

      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                     "Attrib 0x%x not supported!!", attrib_id);
      err_no = E_NOT_ALLOWED;
      break;
  }

  return err_no;

} /* hdrscmcsna_is_simple_attrib_valid */


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
uint8 hdrscmcsna_get_simple_attrib_size
(
  uint16 const attrib_id,
  uint16 const subtype
)
{
  uint8 ind;
    /* Index of simple attribute in look up table */
  uint8 attrib_size;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  ind = HDRSCMCSNA_SIMPLE_ATTRIB_ID_TO_IND(attrib_id);

  if ( HDRSCMCSNA_IS_ATTRIB_SIMPLE(attrib_id) && (ind < HDRSCMCSNA_NUM_SIMPLE_ATTRIBS))
  {
    attrib_size = hdrscmcsna_simple_attribs[ind].value_size;
  }
  else
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                   "CSNA: Not a simple attrib %d", attrib_id);
    attrib_size = 0;
  }

  HDR_MSG_PROT_2(MSG_LEGACY_MED, "Simple attrib 0x%x size=%d",
                 attrib_id, attrib_size);

  return attrib_size;

} /* hdrscmcsna_get_simple_attrib_size */


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
hdrerrno_enum_type hdrscmcsna_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  HDR_MSG_PROT(MSG_LEGACY_MED, "hdrscmcsna_get_attrib_offset_and_size()");

  /* Is the attribute simple? */
//  if (HDRSCMCSNA_IS_ATTRIB_SIMPLE(attrib_id))
//  {
//    /* Get the index */
//    attrib_index = HDRSCMCSNA_SIMPLE_ATTRIB_ID_TO_IND(attrib_id);

    /* The attributes are arranged in the order of index in the config 
       struct.
     */  
//    *attrib_offset = attrib_index;

//    *attrib_size = hdrscmcsna_simple_attribs[attrib_index].value_size;
//  }
//  else
//  {
    /* The attribute is probably complex... */
    switch(attrib_id)
    {
      /* RAND */
      case HDRSCMCSNA_RAND_ID:

        *attrib_offset = FPOS(hdrscmcsna_config_struct_type, rand);
        *attrib_size = FSIZ(hdrscmcsna_config_struct_type, rand);
        break;

      /* MOB_P_REV */
      case HDRSCMCSNA_MOB_P_REV_ID:

        *attrib_offset = FPOS(hdrscmcsna_config_struct_type, mob_p_rev);
        *attrib_size = FSIZ(hdrscmcsna_config_struct_type, mob_p_rev);
        break;

      /* AllowedReverseLinkMessages */
      case HDRSCMCSNA_ALLOWED_REV_LINK_MESSAGES_ID:

        *attrib_offset = FPOS(hdrscmcsna_config_struct_type, rev_msgs);
        *attrib_size = sizeof(hdrscmcsna_allowed_msgs_attrib_type);
        break;

      /* AllowedForwardLinkMessages */
      case HDRSCMCSNA_ALLOWED_FOR_LINK_MESSAGES_ID:

        *attrib_offset = FPOS(hdrscmcsna_config_struct_type, fwd_msgs);
        *attrib_size = sizeof(hdrscmcsna_allowed_msgs_attrib_type);
        break;

      default:

        HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                       "CSNA: Invalid attrib ID: 0x%x; Ignored", attrib_id);
        err_no = E_DATA_INVALID;
        break;
    }

  return err_no;

} /* hdrscmcsna_get_attrib_offset_and_size */


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
hdrerrno_enum_type hdrscmcsna_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *index,
  uint16 subtype
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  HDR_MSG_PROT(MSG_LEGACY_MED, "hdrscmcsna_get_attrib_verno_index()");

  /* The attributes are arranged in the order of index in the Version number
     array */
  *index = HDRSCMCSNA_ATTRIB_ID_TO_IND(attrib_id);

  return err_no;

} /* hdrscmcsna_get_attrib_verno_index */


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
uint32 hdrscmcsna_get_rand ( void )
{
  return hdrscmcsna.inuse.rand;

} /* hdrscmcsna_get_rand */



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
void hdrscmcsna_set_config_to_default
(
  hdrscmcsna_config_struct_type *hdrcsna_cfg
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
  HDR_MSG_PROT(MSG_LEGACY_MED, "hdrscmcsna_set_config_to_default()");

  /* Set all simple attributes to default values */
  hdrcsna_cfg->rand = hdrscmcsna_simple_attribs[HDRSCMCSNA_RAND_IND].default_val;
  hdrcsna_cfg->mob_p_rev = 
    hdrscmcsna_simple_attribs[HDRSCMCSNA_MOB_P_REV_IND].default_val;

  /* Allowed Reverse/Forward Link Messages */
  hdrcsna_cfg->rev_msgs.csch_msg_id_count = HDRSCMCSNA_DEFAULT_MSG_ID_COUNT;
  hdrcsna_cfg->rev_msgs.dsch_msg_id_count = HDRSCMCSNA_DEFAULT_MSG_ID_COUNT;
  hdrcsna_cfg->fwd_msgs.csch_msg_id_count = HDRSCMCSNA_DEFAULT_MSG_ID_COUNT;
  hdrcsna_cfg->fwd_msgs.dsch_msg_id_count = HDRSCMCSNA_DEFAULT_MSG_ID_COUNT;

} /* hdrscmcsna_set_config_to_default */


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_PROCESS_CONFIG

DESCRIPTION
  This function is to start CSNA configuration as part of AT-init 
  session configuration.

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
boolean hdrscmcsna_process_config( void )
{
  uint8 timer_id;
      /* Reserved timer Id */

  dsm_item_type * send_msg_ptr = NULL;
    /* Pointer to Config req. message to send */

  uint16 attrib_offset = 0;
    /* Attribute field offset from start of DSM item */

  boolean status = TRUE;
    /* Return code */

  hdrhai_stream_enum_type stream_id;
    /* Stream associated with this applicaiton */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT(MSG_LEGACY_HIGH, "CSNA: process config");

  /* 0x6: HDRHAI_3G1X_CSN_APP negotiation */
  if ( !hdrstream_get_negotiated_stream( HDRHAI_3G1X_CSN_APP, 
                                         &stream_id) )
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR, "CSNA: No stream for AT-init config");
    status = FALSE;
  }
  /* We should have recieved the configuration information
   * from CM/HOMER. If not, error fatal. */
  else if ( !hdrscmcsna.config_info_received )
  {
    status = FALSE;
    ERR_FATAL("CSNA: AT should have received msg config from CM", 0, 0, 0);
  }
  else
  {
    /* Check if AT-initiated configuration is needed */
    if ( hdrscmcsna_config_is_needed() )
    {
      /* Reserve a Response timer for this Config Request message  */
      if ( (hdrscm_reserve_rsp_timer(HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL,
                                     &timer_id, hdrscmcsna.trans_id, FALSE))
           != E_SUCCESS)
      {
        /* No free timer was available. */
        HDR_MSG_PROT(MSG_LEGACY_ERROR, "CSNA: No free timer available");
        status = FALSE;
      }
      else
      {
        /* Get a new DSM buffer */
        send_msg_ptr = hdrutil_new_msg_buffer();
    
        /* Pack AttribUpdateRequest message's Message ID */
        HDRBIT_PACK8(send_msg_ptr, HDRSCM_CONFIG_REQ_MSG,
                     attrib_offset, HDRSCM_MESSAGE_ID_SIZE );
    
        /* Shift the offset to start of Transaction ID */
        attrib_offset += HDRSCM_MESSAGE_ID_SIZE;
    
        /* Pack the message's Transaction ID */
        HDRBIT_PACK8(send_msg_ptr, hdrscmcsna.trans_id, attrib_offset, 
                     HDRSCM_TRANS_ID_SIZE);
    
        /* Shift the offset to start of Attribute Record's Length */
        attrib_offset += HDRSCM_TRANS_ID_SIZE;
    
        /* Pack the AllowedReverseLinkMessages Attribute */
        if ( hdrscmcsna_allowed_msgs_config_is_needed(FALSE) )
        {
          (void) hdrscmcsna_pack_allowed_msgs_attrib(
                    send_msg_ptr, &attrib_offset, FALSE);
        }
    
        /* Pack the AllowedForwardLinkMessages Attribute */
        if ( hdrscmcsna_allowed_msgs_config_is_needed(TRUE) )
        {
          (void) hdrscmcsna_pack_allowed_msgs_attrib(
                    send_msg_ptr, &attrib_offset, TRUE);
        }
    
        /* Pack the MOB_P_REV attribute */
        if ( hdrscmcsna_mob_p_rev_config_is_needed() )
        {
          (void) hdrscmcsna_pack_mob_p_rev_attrib(
                    send_msg_ptr, &attrib_offset);
        }
    
        /* Now sends out the messages */
        hdrhmp_app_send_msg(HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL,
                            stream_id,
                            HDRHAI_RTC,
                            TRUE,
                            HDRSCM_CONFIG_REQ_MSG_PRI,
                            HDRHAI_IN_CONFIG, /* Instance of protocol sending message */
                            send_msg_ptr,
                            hdrscm_tx_outcome_cb,
                            (void*) ((dword) timer_id) );
    
        /* Increment the transaction ID for next transaction */ 
        ++hdrscmcsna.trans_id;
      } /* hdrscm_reserve_rsp_timer */
    } /* hdrscmcsna_config_is_needed */
    else
    {
      HDR_MSG_PROT(MSG_LEGACY_HIGH, "CSNA: AT-init config is not needed");
    }
  }

  return status;

} /* hdrscmcsna_process_config */

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
void hdrscmcsna_fail_curr_config_neg
(
  hdrhai_session_close_reason_enum_type close_reason
)
{

  hdrind_ind_data_union_type ind_data;

  /* Allocate out of the heap because it is possible for this indication to
  be queued from a different task context. Otherwise, the payload may be
  corrupted which could cause a null pointer de-reference. */
  static hdrscp_scp_fail_ind_type session_fail_data;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  HDR_MSG_PROT(MSG_LEGACY_MED, "hdrscmcsna_fail_curr_config_neg()...");

  /* fill the indication data */
  session_fail_data.reason = close_reason;

  ind_data.scp_fail_ptr = (void *)&session_fail_data;

  hdrind_give_ind(HDRIND_CSNA_PROT_NEG_FAILED,(void *)&ind_data);

} /* hdrscmcsna_fail_curr_config_neg */



/* <EJECT> */
/*===========================================================================

FUNCTION HDRSCMCSNA_PROCESS_ATTRIB_CHANGED

DESCRIPTION
  This function processes the attribute changed event.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscmcsna_process_attrib_changed (void)
{

  HDR_MSG_PROT(MSG_LEGACY_MED, "hdrscmcsna_process_attrib_changed()...");

  /* Get a copy of the inuse instance */
  hdrscmdb_get_inuse_config(HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL, 
                            &hdrscmcsna.inuse);

  #ifdef FEATURE_HDR_CSNA
  /* Update HOMER with the latest CSNA attribute values */
  hdrscmcsna_update_inuse_attrib_to_homer();
  #endif /* FEATURE_HDR_CSNA */

} /* hdrscmcsna_process_attrib_changed */



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
hdrerrno_enum_type hdrscmcsna_is_attrib_gaupable
(
  uint16 const attrib_id,
  uint16 const subtype
)
{
  uint8 gaup_ind = 0;
    /* Loop index */

  hdrerrno_enum_type err_no = E_NOT_ALLOWED;
    /* Error code */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  HDR_MSG_PROT_1(MSG_LEGACY_MED, 
                 "hdrscmcsna is attrib (0x%x) gaupable", attrib_id);

  /* Loop around the GAUPable attribute ID LUT to check if the given 
     attrib_id is one of them */
  for ( gaup_ind = 0; (gaup_ind < HDRSCMCSNA_NUM_GAUPABLE_ATTRIBS);
        ++gaup_ind )
  {
    if (attrib_id == hdrscmcsna_gaup_attrib_ids[gaup_ind])
    {
      /* attrib_id is GAUPable */
      err_no = E_SUCCESS;
      break;
    }
  }

  return err_no;

} /* hdrscmcsna_is_attrib_gaupable */


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
boolean hdrscmcsna_attrib_is_supported
(
  uint16 const attrib_id,
  uint16 const subtype
)
{
  boolean known = FALSE;

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  known = (((attrib_id >= HDRSCMCSNA_FIRST_SIMPLE_ATTRIB_ID) &&
            (attrib_id <= HDRSCMCSNA_LAST_SIMPLE_ATTRIB_ID)) ||
           ((attrib_id >= HDRSCMCSNA_FIRST_COMPLEX_ATTRIB_ID) &&
            (attrib_id <= HDRSCMCSNA_LAST_COMPLEX_ATTRIB_ID)));

  return known;

} /* hdrscmcsna_attrib_is_supported */

/* EJECT */
/*===========================================================================

FUNCTION HDRSCMCSNA_GET_PROP_ATTRIB_VALUE

DESCRIPTION
  This function is used to get the proposed attribute value

DEPENDENCIES
  None

PARAMETERS
  trans_id - Transaction ID
  a_aip - Attribute Information pointer
  subtype - protocol subtype, not used
     
RETURN VALUE
  E_SUCCESS - If the transaction ID is valid.
  E_FAILURE - Trans ID, Attrib ID mismatch
  E_NOT_ALLOWED - ValueID/Value not acceptable, send SessionClose

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrscmcsna_get_prop_attrib_value
(
  uint16 const trans_id,
  hdrscm_accepted_attrib_type *a_aip,
  uint16 const subtype
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT(MSG_LEGACY_MED, "hdrscmcsna_get_prop_attrib_value()");

  /* Make sure Attribute Information pointer is not NULL */
  ASSERT(a_aip != NULL);

  if (trans_id == (uint8)(hdrscmcsna.trans_id-1))
  {    
    switch ( a_aip->attrib_id )
    {
    case HDRSCMCSNA_ALLOWED_REV_LINK_MESSAGES_ID:
      a_aip->value_id = hdrscmcsna.allow_rev_value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmcsna.at_supported.rev_msgs);
      a_aip->complex_val_size = sizeof(hdrscmcsna.at_supported.rev_msgs);
      a_aip->is_accepted = TRUE;
      break;

    case HDRSCMCSNA_ALLOWED_FOR_LINK_MESSAGES_ID:
      a_aip->value_id = hdrscmcsna.allow_fwd_value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmcsna.at_supported.fwd_msgs);
      a_aip->complex_val_size = sizeof(hdrscmcsna.at_supported.fwd_msgs);
      a_aip->is_accepted = TRUE;
      break;

    case HDRSCMCSNA_MOB_P_REV_ID:
      a_aip->value_id = hdrscmcsna.at_supported.mob_p_rev;
      a_aip->complex_val_ptr = NULL;
      a_aip->is_accepted = TRUE;
      break;

    default:
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
          "CSNA: unsupported attrib_id %d", a_aip->attrib_id);
        /* attribute_id mismatch, ignore this attribute record */
      err_no = E_FAILURE;
      break;
    }
  }
  else
  {
    HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
         "CSNA: trans_id %d mismatched proposed trans_id %d", 
         trans_id, (uint8)(hdrscmcsna.trans_id-1));
     /* trans_id mismatch, ignore this attribute record */
    err_no = E_FAILURE;
  }

  return err_no;
} /* hdrscmcsna_get_prop_attrib_value */

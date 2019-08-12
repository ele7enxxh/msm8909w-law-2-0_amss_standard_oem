/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

                    R O U T E  U P D A T E  P R O T O C O L

       
GENERAL DESCRIPTION
  This module provides supporting API for SCM module to handle AT-init Session
  Negotiation for SupportedCDMAChannel Attribute of RouteUpdateProtocol. 
    
EXTERNALIZED FUNCTIONS
    
  hdrscmrup_is_attrib_simple 

  hdrscmrup_get_attrib_offset_and_size

  hdrscmrup_get_attrib_verno_index

  hdrscmrup_prop_supp_cdma_chan_attrib
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

  
  Copyright (c) 2005 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/16/16   vko     Reduce excessive F3
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
07/27/11   cnx     Merged Optimized Handoff changes.
02/19/10   mbs     Do not ignore AT config timeout for SupportedCDMAChannels
12/10/08   smd     Replaced assert.h with amssassert.h
10/09/07   sju     Updated comments
07/13/07   sju     Added support for MC RUP negotiation
06/14/07   wsh     Moved value_id comparision to SCM instead of individual 
                   modules when handling get_prop_attrib()
02/17/07   wsh     Added value_id/attrib_id checking processing ConfigResp
01/25/07   hal     Added support for ConfigResponse message timeouts
05/18/06   etv     Removed ASSERTS for graceful degradation.
04/07/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
11/28/05   etv     Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/

#include "hdr_variation.h"
#include "dsm.h"
#include "comdef.h"
#include "err.h"
#include "amssassert.h"

#include "hdrdebug.h"
#include "hdrdsm.h"
#include "hdrscp.h"
#include "hdrhai.h"
#include "hdrtrace.h"
#include "hdrbit.h"
#include "hdrutil.h"
#include "hdrhmp.h"

#include "hdrrupc.h"
#include "hdrscm.h"
#include "hdrscmdb.h"
#include "hdrscmrup.h"


/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/*---------------------------------------------------------------------------
  Sizes of the complex attributes and its fields in bits 
----------------------------------------------------------------------------*/




 /* EJECT */
/*===========================================================================
 
                  DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
#define HDRSCMRUP_LENGTH_SIZE                       8
#define HDRSCMRUP_ATTRIB_ID_SIZE                    8
#define HDRSCMRUP_VALUE_ID_SIZE                     8

#define HDRSCMRUP_DEFAULT_SUCC_INTER_FLEX_DUPLEX_TAG   0
#define HDRSCMRUP_DEFAULT_SUCC_INTRA_FLEX_DUPLEX_FLAG  0

#define HDRSCMRUP_8_BITS_ATTRIB_VALUE_SIZE          8
#define HDRSCMRUP_16_BITS_ATTRIB_VALUE_SIZE         16

#define HDRSCMRUP_MAX_NUM_FL_SUPPORTED_VALUE_SIZE       8
#define HDRSCMRUP_MAX_NUM_RL_SUPPORTED_VALUE_SIZE       8
#define HDRSCMRUP_MAX_FL_BDWIDTH_NO_JAMMER_VALUE_SIZE   16 
#define HDRSCMRUP_MAX_FL_BDWIDTH_W_JAMMER_VALUE_SIZE    16 
#define HDRSCMRUP_MAX_RL_BDWIDTH_VALUE_SIZE             16
#define HDRSCMRUP_MAX_NUM_SUB_ASETS_VALUE_SIZE          8
#define HDRSCMRUP_FWD_FEEDBACK_MUX_SUPPORTED_VALUE_SIZE 8

#define HDRSCMRUP_NUM_AT_INIT_SIMPLE_ATTRIBUTES         7

/* Finds if the given attribute Id is of a simple attribute */
#define HDRSCMRUP_IS_ATTRIB_SIMPLE(attrib_id)  \
        ((((attrib_id) & 0x00F0) == 0x00F0)? TRUE: FALSE)

#define HDRSCMRUP_SIMPLE_ATTRIB_ID_TO_IND(attrib_id) \
        (0xFF - (attrib_id))

/* This function checks if AT init configuration is needed */
typedef boolean (*hdrscmrup_config_is_needed) (void);

typedef struct
{
  uint8 attribute_id;
  uint16 desired_value;
  hdrscmrup_config_is_needed config_is_needed;
} hdrscmrup_at_init_simple_attrib_pack_info_type;

static hdrscmrup_at_init_simple_attrib_pack_info_type 
hdrscmrup_at_init_simple_attrib[HDRSCMRUP_NUM_AT_INIT_SIMPLE_ATTRIBUTES] = 
{
  {
    HDRRUPC_MAX_NUMBER_FL_SUPPORTED_ATTRIB_ID,
    HDRRUPC_DESIRED_MAX_NUM_FL_SUPPORTED,
    hdrrupc_max_num_fl_supported_attrib_config_is_needed
  },
  {
    HDRRUPC_MAX_NUMBER_RL_SUPPORTED_ATTRIB_ID,
    HDRRUPC_DESIRED_MAX_NUM_RL_SUPPORTED,
    hdrrupc_max_num_rl_supported_attrib_config_is_needed
  },
  {
    HDRRUPC_MAX_FL_BANDWIDTH_NO_JAMMER_ATTRIB_ID,
    HDRRUPC_DESIRED_MAX_FL_BANDWIDTH_NO_JAMMER,
    hdrrupc_max_fl_bdwidth_no_jammer_attrib_config_is_needed
  },
  {
    HDRRUPC_MAX_FL_BANDWIDTH_W_JAMMER_ATTRIB_ID,
    HDRRUPC_DESIRED_MAX_FL_BANDWIDTH_W_JAMMER,
    hdrrupc_max_fl_bdwidth_w_jammer_attrib_config_is_needed
  },
  {
    HDRRUPC_MAX_RL_BANDWIDTH_ATTRIB_ID,
    HDRRUPC_DESIRED_MAX_RL_BANDWIDTH,
    hdrrupc_max_rl_bdwidth_attrib_config_is_needed
  },
  {
    HDRRUPC_MAX_NUMBER_SUB_ASETS_ATTRIB_ID,
    HDRRUPC_DESIRED_MAX_NUM_SUB_ASETS,
    hdrrupc_max_num_sub_asets_attrib_config_is_needed
  },
  {
    HDRRUPC_FWD_FEEDBACK_MUX_SUPPORTED_ATTRIB_ID,
    HDRRUPC_DESIRED_FWD_FEEDBACK_MUX_SUPPORTED,
    hdrrupc_fwd_feedback_mux_supported_attrib_config_is_needed
  }
};

typedef struct
{
  /* SupportedCDMAChannels */
  /* revB: new fields: InterFlexDuplexTag, IntraFlexDuplexFlag */
  hdrrupc_supp_cdma_chan_attrib_type succ;

  /* revB - simple attribute*/
  /* MaxNumberofFLSupported*/
  uint8 max_num_fl_supported_attrib;

  /* MaxNumberofRLSupported*/
  uint8 max_num_rl_supported_attrib;

  /* MaxForwardLinkBandwidthnoJammer*/
  uint16 max_fl_bdwidth_no_jammer_attrib;

  /* MaxForwardLinkBandwidthWithJammer*/
  uint16 max_fl_bdwidth_w_jammer_attrib;

  /* MaxReverseLinkBandwith*/
  uint16 max_rl_bdwidth_attrib;

  /* MaxNumberOfSub-ActiveSets*/
  uint8 max_num_sub_asets_attrib;

  /* ForwardFeedbackMultiplexingSupported*/
  uint8 fwd_fb_mux_supp_attrib;

  /* revB - complex attribute*/
  /* SupportedDRXPatterns - Not Planned*/

} hdrscmrup_at_init_attrib_type;
/*------------------------------------------------------------------------
                            GLOBAL VARIABLES
 -----------------------------------------------------------------------*/

static struct
{
  uint8 trans_id;
    /* Transaction ID for AT-init Sesssion Negotiation */

  hdrrupc_supp_cdma_chan_attrib_type succ;
    /* Proposed instance of supported CDMA channel attribute */

  uint8 supp_cdma_chnl_value_id;
    /* value_id we used during AT proposal */
} hdrscmrup;


 /* EJECT */
/*------------------------------------------------------------------------
                              LOOKUP TABLES
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
boolean hdrscmrup_is_attrib_simple
(
  uint16 const attrib_id,
  uint16 const subtype
)
{
  return HDRSCMRUP_IS_ATTRIB_SIMPLE(attrib_id);
}


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
hdrerrno_enum_type hdrscmrup_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  switch( attrib_id )
  {
    case HDRRUPC_SUPPORTED_CDMA_CHANNELS_ATTRIB_ID:
      *attrib_offset = FPOS( hdrrupc_config_params_type, 
                             succ );
      *attrib_size   = FSIZ( hdrrupc_config_params_type,
                             succ );
      break;

    case HDRRUPC_MAX_NUMBER_FL_SUPPORTED_ATTRIB_ID:
      *attrib_offset = FPOS( hdrrupc_config_params_type, 
                             max_num_fl_supported_attrib );
      *attrib_size   = FSIZ( hdrrupc_config_params_type,
                             max_num_fl_supported_attrib );
      break;

    case HDRRUPC_MAX_NUMBER_RL_SUPPORTED_ATTRIB_ID:
      *attrib_offset = FPOS( hdrrupc_config_params_type, 
                             max_num_rl_supported_attrib );
      *attrib_size   = FSIZ( hdrrupc_config_params_type,
                             max_num_rl_supported_attrib );
      break;

    case HDRRUPC_MAX_FL_BANDWIDTH_NO_JAMMER_ATTRIB_ID:
      *attrib_offset = FPOS( hdrrupc_config_params_type, 
                             max_fl_bdwidth_no_jammer_attrib);
      *attrib_size   = FSIZ( hdrrupc_config_params_type,
                             max_fl_bdwidth_no_jammer_attrib);
      break;

    case HDRRUPC_MAX_FL_BANDWIDTH_W_JAMMER_ATTRIB_ID:
      *attrib_offset = FPOS( hdrrupc_config_params_type, 
                             max_fl_bdwidth_w_jammer_attrib);
      *attrib_size   = FSIZ( hdrrupc_config_params_type,
                             max_fl_bdwidth_w_jammer_attrib);
      break;

    case HDRRUPC_MAX_RL_BANDWIDTH_ATTRIB_ID:
      *attrib_offset = FPOS( hdrrupc_config_params_type, 
                             max_rl_bdwidth_attrib);
      *attrib_size   = FSIZ( hdrrupc_config_params_type,
                             max_rl_bdwidth_attrib);
      break;

    case HDRRUPC_MAX_NUMBER_SUB_ASETS_ATTRIB_ID:
      *attrib_offset = FPOS( hdrrupc_config_params_type, 
                             max_num_sub_asets_attrib );
      *attrib_size   = FSIZ( hdrrupc_config_params_type,
                             max_num_sub_asets_attrib );
      break;

    case HDRRUPC_FWD_FEEDBACK_MUX_SUPPORTED_ATTRIB_ID:
      *attrib_offset = FPOS( hdrrupc_config_params_type, 
                             fwd_fb_mux_supp_attrib );
      *attrib_size   = FSIZ( hdrrupc_config_params_type,
                             fwd_fb_mux_supp_attrib );
      break;

    default:
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                     "RUP: Invalid attrib ID: 0x%x; Ignored", attrib_id);
      err_no = E_DATA_INVALID;
      break;
  }

  return err_no;

} /* hdrscmrup_get_attrib_offset_and_size */

 /* EJECT */
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
uint8 hdrscmrup_get_simple_attrib_size
(
  uint16 const attrib_id,
  uint16 const subtype
)
{
  uint8 attrib_size;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* subtype is not used for now, because for the same attribute, all subtypes
   * have same size
   */
  if ( HDRSCMRUP_IS_ATTRIB_SIMPLE(attrib_id) )
  {
    switch(attrib_id)
    {
      case HDRRUPC_MAX_FL_BANDWIDTH_NO_JAMMER_ATTRIB_ID:
      case HDRRUPC_MAX_FL_BANDWIDTH_W_JAMMER_ATTRIB_ID:
      case HDRRUPC_MAX_RL_BANDWIDTH_ATTRIB_ID:
      case HDRRUPC_ROUTE_UPDATE_RADIUS_ADD_ATTRIB_ID:
        attrib_size = HDRSCMRUP_16_BITS_ATTRIB_VALUE_SIZE;
        break;

      default:
        attrib_size = HDRSCMRUP_8_BITS_ATTRIB_VALUE_SIZE;
        break;
    }
  }
  else
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                   "RUP: Not a simple attrib %d", attrib_id);
    attrib_size = 0;
  }

  HDR_MSG_PROT_2(MSG_LEGACY_MED, "Simple attrib 0x%x size=%d",
                 attrib_id, attrib_size);

  return attrib_size;

} /* hdrscmrup_get_simple_attrib_size */

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRUP_GET_PROPOSED_ATTRIB_VALUE

DESCRIPTION
  This function is used by SCM to get the attribute value proposed by
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
hdrerrno_enum_type hdrscmrup_get_proposed_attrib_value
(
  uint16 const trans_id,
  hdrscm_accepted_attrib_type *a_aip,
  uint16 const subtype
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ((subtype == HDRSCP_DEFAULT_RUP) && 
      (a_aip->attrib_id  != HDRRUPC_SUPPORTED_CDMA_CHANNELS_ATTRIB_ID))
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "Default RUP: unsupported attrib_id %d", 
                   a_aip->attrib_id);
      /* attribute_id mismatch, ignore this attribute record */
    err_no = E_FAILURE;
  }
  else if (trans_id == (uint8)(hdrscmrup.trans_id-1))
  {
    switch(a_aip->attrib_id)
    {
      case HDRRUPC_SUPPORTED_CDMA_CHANNELS_ATTRIB_ID:
      a_aip->value_id = hdrscmrup.supp_cdma_chnl_value_id;
      a_aip->complex_val_ptr = (void*) &(hdrscmrup.succ);
      a_aip->complex_val_size = sizeof(hdrscmrup.succ);
      a_aip->is_accepted = TRUE;
        break;

      case HDRRUPC_MAX_NUMBER_FL_SUPPORTED_ATTRIB_ID:
        a_aip->value_id = HDRRUPC_DESIRED_MAX_NUM_FL_SUPPORTED;
        a_aip->is_accepted = TRUE;
        break;

      case HDRRUPC_MAX_NUMBER_RL_SUPPORTED_ATTRIB_ID:
        a_aip->value_id = HDRRUPC_DESIRED_MAX_NUM_RL_SUPPORTED;
        a_aip->is_accepted = TRUE;
        break;

      case HDRRUPC_MAX_FL_BANDWIDTH_NO_JAMMER_ATTRIB_ID:
        a_aip->value_id = HDRRUPC_DESIRED_MAX_FL_BANDWIDTH_NO_JAMMER;
        a_aip->is_accepted = TRUE;
        break;

      case HDRRUPC_MAX_FL_BANDWIDTH_W_JAMMER_ATTRIB_ID:
        a_aip->value_id = HDRRUPC_DESIRED_MAX_FL_BANDWIDTH_W_JAMMER;
        a_aip->is_accepted = TRUE;
        break;

      case HDRRUPC_MAX_RL_BANDWIDTH_ATTRIB_ID:
        a_aip->value_id = HDRRUPC_DESIRED_MAX_RL_BANDWIDTH;
        a_aip->is_accepted = TRUE;
        break;

      case HDRRUPC_MAX_NUMBER_SUB_ASETS_ATTRIB_ID:
        a_aip->value_id = HDRRUPC_DESIRED_MAX_NUM_SUB_ASETS;
        a_aip->is_accepted = TRUE;
        break;

      case HDRRUPC_FWD_FEEDBACK_MUX_SUPPORTED_ATTRIB_ID:
        a_aip->value_id = HDRRUPC_DESIRED_FWD_FEEDBACK_MUX_SUPPORTED;
        a_aip->is_accepted = TRUE;
      break;

    default:
        HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "RUP: unsupported attrib_id %d", 
                       a_aip->attrib_id);
          /* attribute_id mismatch, ignore this attribute record */
        err_no = E_FAILURE;
      break;
    }
  }
  else
  {
    HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
        "RUP: trans_id %d mismatched proposed trans_id %d",
         trans_id, (uint8)(hdrscmrup.trans_id-1));
    err_no = E_FAILURE;
  }
  return err_no;
} /* hdrscmrup_get_proposed_attrib_value */


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
void hdrscmrup_fail_curr_config_neg
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

  /* fill the indication data */
  session_fail_data.type = HDRHAI_ROUTE_UPDATE_PROTOCOL;
  session_fail_data.subtype = 
    hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL);
  session_fail_data.attribute_id = HDRRUPC_SUPPORTED_CDMA_CHANNELS_ATTRIB_ID;
  session_fail_data.reason = close_reason;

  ind_data.scp_fail_ptr = (void *)&session_fail_data;

  hdrind_give_ind(HDRIND_RUP_PROT_NEG_FAILED,(void *)&ind_data);

} /* hdrscmrup_fail_curr_config_neg */

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRUP_PACK_SIMPLE_ATTRIBUTES

DESCRIPTION
  This function is used to pack simple attributes.

DEPENDENCIES
  None

PARAMETERS
 send_msg_ptr - pointer to the DSM item where the packed data is kept 
 attrib_offset_ptr - number of bits to skip before packing next field .
   
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
static void hdrscmrup_pack_simple_attributes
( 
  dsm_item_type *send_msg_ptr,
  uint16        *attrib_offset_ptr
)
{
  uint16  attrib_offset = *attrib_offset_ptr;
  uint8   attrib_value_size;
  uint8   attrib_len;
  uint8   index;

  uint16 subtype = hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  HDR_MSG_PROT(MSG_LEGACY_LOW, "Pack RUP Simple Attributes ");

  for (index = 0; index < HDRSCMRUP_NUM_AT_INIT_SIMPLE_ATTRIBUTES; index++)
  {
    if (hdrscmrup_at_init_simple_attrib[index].config_is_needed())
    {
      attrib_value_size = 
        hdrscmrup_get_simple_attrib_size(hdrscmrup_at_init_simple_attrib[index].attribute_id, subtype);

      /* Pack the length */
      attrib_len = ( HDRSCMRUP_ATTRIB_ID_SIZE + attrib_value_size) / 8;

      HDRBIT_PACK8(send_msg_ptr, attrib_len,
                   attrib_offset, HDRSCM_LENGTH_SIZE);

      attrib_offset += HDRSCM_LENGTH_SIZE;

      /* Attrib ID field */
      HDRBIT_PACK8( send_msg_ptr, hdrscmrup_at_init_simple_attrib[index].attribute_id,
                    attrib_offset, HDRSCMRUP_ATTRIB_ID_SIZE);

      attrib_offset += HDRSCMRUP_ATTRIB_ID_SIZE;

      /* Value */
      if (attrib_value_size == 8 )
      {
        HDRBIT_PACK8(send_msg_ptr, (uint8) hdrscmrup_at_init_simple_attrib[index].desired_value,
                      attrib_offset, attrib_value_size);
      }
      else
      {
        HDRBIT_PACK16(send_msg_ptr, hdrscmrup_at_init_simple_attrib[index].desired_value,
                      attrib_offset, attrib_value_size);
      }
      attrib_offset += attrib_value_size;

      HDR_MSG_PROT_2(MSG_LEGACY_MED, "RUP: AT proposed attrib_id 0x%x value %d",
                     hdrscmrup_at_init_simple_attrib[index].attribute_id,
                     hdrscmrup_at_init_simple_attrib[index].desired_value );
    }
  }

  *attrib_offset_ptr = attrib_offset;

} /* hdrscmrup_pack_simple_attributes */

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMRUP_PACK_SUPP_CDMA_CHAN_ATTRIB

DESCRIPTION
  This routine is used to by RUP to propose Supported CDMA Channel Attribute
  during initial AT-init Session Configuration.

DEPENDENCIES
  None.

PARAMETERS
 send_msg_ptr - pointer to the DSM item where the packed data is kept 
 attrib_offset_ptr - number of bits to skip before packing next field .
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrscmrup_pack_supp_cdma_chan_attrib
(
  dsm_item_type *send_msg_ptr,
  uint16        *attrib_offset_ptr
)
{
  hdrrupc_supp_cdma_chan_attrib_type succ;
  uint8 length;

  uint16  attrib_offset = *attrib_offset_ptr;

  hdrrupc_band_class_rec_type *band_class_rec;
    /* Convenience pointer */

  uint8 band_class = 0;
    /* Band class - Loop index */

  uint8 band_sub_class = 0;
    /* Band Subclass - Loop index */

  uint8 inter_flex_duplex_tag = HDRSCMRUP_DEFAULT_SUCC_INTRA_FLEX_DUPLEX_FLAG;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  length = hdrrupc_build_supp_cdma_chan_attrib( &succ );

  /* Copy to static memory */
#ifdef FEATURE_MEMCPY_REMOVAL
  hdrscmrup.succ = succ;
#else
  memcpy( &hdrscmrup.succ, &succ,
          sizeof(hdrrupc_supp_cdma_chan_attrib_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

  HDR_MSG_PROT(MSG_LEGACY_HIGH, "AT-init Proposal of SUCC ");


  /* Pack the attribute Length */
  HDRBIT_PACK8(send_msg_ptr, length, attrib_offset, 
               HDRSCMRUP_LENGTH_SIZE );

  HDR_MSG_PROT_2(MSG_LEGACY_LOW, "SUCC: offset %d Length %d",
                 attrib_offset, length);

  /* Shift the offset to start of Attrib ID */
  attrib_offset += HDRSCMRUP_LENGTH_SIZE;

  /* Pack the attribute ID of SupportedCDMAChannels Attribute */
  HDRBIT_PACK8(send_msg_ptr, HDRRUPC_SUPPORTED_CDMA_CHANNELS_ATTRIB_ID, attrib_offset, 
               HDRSCMRUP_ATTRIB_ID_SIZE);

  HDR_MSG_PROT_2(MSG_LEGACY_LOW, "SUCC: offset %d AttribID %d",
                 attrib_offset, HDRRUPC_SUPPORTED_CDMA_CHANNELS_ATTRIB_ID);

  /* Shift the offset to start of value id size */
  attrib_offset += HDRSCMRUP_ATTRIB_ID_SIZE;

  /* Pack the value ID */
  HDRBIT_PACK8( send_msg_ptr, succ.value_id, attrib_offset, 
                HDRSCMRUP_VALUE_ID_SIZE );
  hdrscmrup.supp_cdma_chnl_value_id = succ.value_id;

  HDR_MSG_PROT_2(MSG_LEGACY_LOW, "SUCC: offset %d Value ID %d",
                 attrib_offset, succ.value_id);

  /* Shift the offset to start of band class count field */
  attrib_offset += HDRSCMRUP_VALUE_ID_SIZE;

  /* Packt Band Class Count */
  HDRBIT_PACK8( send_msg_ptr, succ.band_class_cnt, attrib_offset, 
                HDRSCMRUP_SUCC_BAND_CLASS_CNT_SIZE );

  HDR_MSG_PROT_2(MSG_LEGACY_LOW, "SUCC: offset %d BndClCnt %d",
                 attrib_offset, succ.band_class_cnt);

  /* Shift the offset to start of first band class field */
  attrib_offset += HDRSCMRUP_SUCC_BAND_CLASS_CNT_SIZE;
  
  for( band_class = 0; 
       (band_class < succ.band_class_cnt); ++band_class )
  {
    /* Store the record to the convenience pointer */
    band_class_rec = &succ.band_class_list[band_class];

    /* Pack Band class */
    HDRBIT_PACK8( send_msg_ptr, band_class_rec->band_class, attrib_offset, 
                  HDRSCMRUP_SUCC_BAND_CLASS_SIZE );

    HDR_MSG_PROT_2(MSG_LEGACY_LOW, "SUCC: offset %d BndClass %d",
                   attrib_offset, band_class_rec->band_class);

    /* Shift the offset to start of Band Sub class count */
    attrib_offset += HDRSCMRUP_SUCC_BAND_CLASS_SIZE;

    if (hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_MC_RUP
#ifdef FEATURE_LTE_TO_HDR_OH
        || (hdrscp_get_config_subtype(HDRHAI_ROUTE_UPDATE_PROTOCOL) == HDRSCP_IRAT_MC_RUP)
#endif /* FEATURE_LTE_TO_HDR_OH */        
       )
    {
      /* Pack InterFlexDuplexTag */
      /* make inter_flex_duplex_tage different in each band class*/
      HDRBIT_PACK8( send_msg_ptr, inter_flex_duplex_tag++ , attrib_offset, 
                    HDRSCMRUP_SUCC_INTER_FLEX_DUPLEX_TAG_SIZE );
      attrib_offset += HDRSCMRUP_SUCC_INTER_FLEX_DUPLEX_TAG_SIZE;

      /* Pack IntraFlexDuplexFlag */
      HDRBIT_PACK8( send_msg_ptr, HDRSCMRUP_DEFAULT_SUCC_INTRA_FLEX_DUPLEX_FLAG , attrib_offset, 
                    HDRSCMRUP_SUCC_INTRA_FLEX_DUPLEX_FLAG_SIZE );
      attrib_offset += HDRSCMRUP_SUCC_INTRA_FLEX_DUPLEX_FLAG_SIZE;
    }
  
    /* Pack the Sub Class count */
    HDRBIT_PACK8( send_msg_ptr, band_class_rec->band_sub_class_cnt, 
                  attrib_offset, HDRSCMRUP_SUCC_BAND_SUB_CLASS_CNT_SIZE );

    HDR_MSG_PROT_2(MSG_LEGACY_LOW, "SUCC: offset %d BndSubClCnt %d",
                   attrib_offset, band_class_rec->band_sub_class_cnt);
  
    /* Shift the offset to start of Band Sub Class */
    attrib_offset += HDRSCMRUP_SUCC_BAND_SUB_CLASS_CNT_SIZE;

    for( band_sub_class = 0; 
         band_sub_class < band_class_rec->band_sub_class_cnt; band_sub_class++ )
    {
      /* Pack the Band Subclass field */
      HDRBIT_PACK8( send_msg_ptr, band_class_rec->band_sub_class[band_sub_class], 
                    attrib_offset, HDRSCMRUP_SUCC_BAND_SUB_CLASS_SIZE );

      HDR_MSG_PROT_2(MSG_LEGACY_LOW, "SUCC: offset %d BndSubClass %d",
                     attrib_offset, band_class_rec->band_sub_class[band_sub_class]);

      /* Shift the offset to start of next Band Sub Class field */
      attrib_offset += HDRSCMRUP_SUCC_BAND_SUB_CLASS_SIZE;
    }
  }

} /* hdrscmrup_pack_supp_cdma_chan_attrib */

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
boolean hdrscmrup_start_config(void)
{
  uint8 timer_id;
    /* Reserved timer Id */

  dsm_item_type * send_msg_ptr = NULL;
    /* Pointer to gaup req. message to send */

  uint16 attrib_offset = 0;
    /* Attribute field offset from start of DSM item */

  boolean status = TRUE;
    /* Error Code */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reserve a Response timer for this Config Request message  */
  if ( (hdrscm_reserve_rsp_timer(HDRHAI_ROUTE_UPDATE_PROTOCOL, &timer_id, 
                           hdrscmrup.trans_id, FALSE)) != E_SUCCESS)
  {
    /* No free timer was available. */
    HDR_MSG_PROT(MSG_LEGACY_ERROR, "RUP: No free timer available");
    return FALSE;
  }

  HDR_MSG_PROT(MSG_LEGACY_HIGH, "RUP: AT-init configuration");

  /* Get a new DSM buffer */
  send_msg_ptr = hdrutil_new_msg_buffer();

  /* Pack AttribUpdateRequest message's Message ID */
  HDRBIT_PACK8(send_msg_ptr, HDRSCM_CONFIG_REQ_MSG,
               attrib_offset, HDRSCM_MESSAGE_ID_SIZE );

  /* Shift the offset to start of Transaction ID */
  attrib_offset += HDRSCM_MESSAGE_ID_SIZE;

  /* Pack the message's Transaction ID */
  HDRBIT_PACK8(send_msg_ptr, hdrscmrup.trans_id, attrib_offset, 
               HDRSCM_TRANS_ID_SIZE);

  /* Shift the offset to start of Attribute Record's Length */
  attrib_offset += HDRSCM_TRANS_ID_SIZE;

  hdrscmrup_pack_simple_attributes(send_msg_ptr, &attrib_offset);


  if (hdrrupc_succ_attrib_config_is_needed())
  {
    hdrscmrup_pack_supp_cdma_chan_attrib(send_msg_ptr, &attrib_offset);
  }

  /* Send AttribUpdateRequest msg on RTC. All config messages are reliable. */
  hdrhmp_send_msg(HDRHAI_ROUTE_UPDATE_PROTOCOL,
                  HDRHAI_RTC,
                  TRUE,
                  HDRSCM_CONFIG_REQ_MSG_PRI,
                  HDRHAI_IN_CONFIG,
                  send_msg_ptr,
                  hdrscm_tx_outcome_cb,
                  (void*) ((dword) timer_id) );

  /* Increment the transaction ID for next transaction */ 
  ++hdrscmrup.trans_id;

  return status;
}

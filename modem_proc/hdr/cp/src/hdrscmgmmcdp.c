/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

          G E N E R I C  M U L T I - M O D E  C A P A B I L I T Y 

                    D I S C O V E R Y  P R O T O C O L

       
GENERAL DESCRIPTION
  This module provides supporting API for SCM module to propose configuration 
  attributes of Generic Multi-Mode Capability Discovery protocol. 
    
EXTERNALIZED FUNCTIONS
  hdrscmgmmcdp_is_attrib_simple 
    Returns true if an attribute is simple

  hdrscmgmmcdp_get_attrib_offset_and_size
    Returns attribute size and offset into the config structure.

  hdrscmgmmcdp_get_attrib_verno_index
    Returns the index of an attribute in the version number array. 
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

  
  Copyright (c) 2005 - 2010 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrscmgmmcdp.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/02/11   cnx     Removed hdrscmgmmcdp_powerup_init().
11/22/10   cnx     Added hdrscmgmmscd_is_simple_attrib_valid().  
06/21/10   cnx     Fixed compiler warnings.
07/31/09   wsh     Do not propose GMMCDP attributes if not needed
10/17/08   pba     Fixed bug in attrib is supported function.
09/03/08   etv     Fixed bug with RxDiversity Attribute proposal.
09/18/07   wsh     Report correct DiversityComb and SimultaneousCommonChannelReceive
                   value in Multi-Mode Discovery protocol
06/14/07   wsh     Moved value_id comparision to SCM instead of individual 
                   modules when handling get_prop_attrib()
02/17/07   wsh     Added value_id/attrib_id checking processing ConfigResp
01/03/07   etv     Added support to handle unknown attributes.
10/02/06   pba     Propose GMMCDP in AT_INIT phase only if we do not already 
                   have the desired values.
05/18/06   etv     Removed ASSERTs for graceful degradation.
04/07/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
04/02/06   dna     Fixed size calculation of MMCDP attribute
02/08/06   etv     Changed Message level from error to Medium.
02/02/06   etv     Propose SimCommonChanRcv only if AT supports dual receive 
                   chain.
11/27/05   etv     Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "dsm.h"
#include "comdef.h"
#include "err.h"

#include "hdrdebug.h"
#include "hdrdsm.h"
#include "hdrscps.h"
#include "hdrscp.h"
#include "hdrhai.h"
#include "hdrtrace.h"
#include "hdrutil.h"
#include "hdrhmp.h"
#include "hdrbit.h"

#include "hdrscmgmmcdp.h"
#include "hdrscmdb.h"
#include "hdrscm.h"
#include "hdrlmac.h"
#include "hdrsmp.h"
#include "hdrstream.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
#define HDRSCMGMMCDP_SIM_COMMON_CHAN_RCV_ATTRIB_ID      0xFD
#define HDRSCMGMMCDP_DIVERSITY_COMB_ATTRIB_ID           0xFA

#define HDRSCMGMMCDP_CONFIG_REQ_MSG_PRIO                40

#define HDRSCMGMMCDP_ATTRIB_ID_SIZE                     8
#define HDRSCMGMMCDP_ATTRIB_VALUE_SIZE                  8
#define HDRSCMGMMCDP_ATTRIB_LENGTH_SIZE                 8

/*===========================================================================
 
                  DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/


LOCAL struct
{
  uint8 trans_id;
    /* Transaction ID for sending out Config request message */

  uint16 subtype;
    /* Protocol Subtype */

} hdrscmgmmcdp = { 0, 0 };

/*===========================================================================

                         LOCAL FUNCTION DEFINITIONS
                    
===========================================================================*/

 /* EJECT */
/*============================================================================

                        EXTERNAL FUNCTION DEFINITIONS

=============================================================================*/

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
boolean hdrscmgmmcdp_is_attrib_simple
(
  uint16 const attrib_id,
  uint16 const subtype
)
{
  /* Currently we only support the following simple attributes. We shouldn't 
     get here for anyother attribute ID 
     1. SimultaneousCommonChannelReceive
     2. ReceiveDiversity */

  return ((attrib_id == HDRSCMGMMCDP_SIM_COMMON_CHAN_RCV_ATTRIB_ID) ||
          (attrib_id == HDRSCMGMMCDP_DIVERSITY_COMB_ATTRIB_ID));

} /* hdrscmgmmcdp_is_attrib_simple */

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
hdrerrno_enum_type hdrscmgmmscd_is_simple_attrib_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value,
  uint16 const subtype
)
{
  hdrerrno_enum_type err_code = E_OUT_OF_RANGE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Currently there is no attribute proposed by AN. */
  HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                  "Received invalid simple attribute (id: 0x%x)", attrib_id );

  return err_code;

} /* hdrscmgmmscd_is_simple_attrib_valid() */

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
hdrerrno_enum_type hdrscmgmmcdp_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
)
{
  hdrerrno_enum_type err_no = E_DATA_INVALID;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure we don't get here for any other attribute IDs */
  if (attrib_id == HDRSCMGMMCDP_SIM_COMMON_CHAN_RCV_ATTRIB_ID)
  {
    err_no = E_SUCCESS;

    *attrib_offset = FPOS(hdrscmgmmcdp_config_struct_type,
                          sim_comm_chan_rcv);
    *attrib_size = sizeof(uint8);
  }
  else if (attrib_id == HDRSCMGMMCDP_DIVERSITY_COMB_ATTRIB_ID)
  {
    err_no = E_SUCCESS;

    *attrib_offset = FPOS(hdrscmgmmcdp_config_struct_type, 
                          diversity_comb_enable);
    *attrib_size = sizeof(uint8);
  }

  return err_no;

} /* hdrscmgmmcdp_get_attrib_offset_and_size */


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
  E_DATA_INVALID - otherwise

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrscmgmmcdp_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *index,
  uint16 subtype
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch (attrib_id)
  {
    /* SimultaneousCommonChannelReceive */
    case HDRSCMGMMCDP_SIM_COMMON_CHAN_RCV_ATTRIB_ID:
      *index = 0;
      break;

    /* ReceiveDiversity */
    case HDRSCMGMMCDP_DIVERSITY_COMB_ATTRIB_ID:
      *index = 1;
      break;

    default:
      err_no = E_DATA_INVALID;
  }

  return err_no;

} /* hdrscmgmmcdp_get_attrib_verno_index */


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
)
{
  hdrgmmcdp_cfg->sim_comm_chan_rcv = (uint8) HDRSCMGMMCDP_SINGLE_RX_CHAIN_FOR_COMMON_CHAN;
  hdrgmmcdp_cfg->diversity_comb_enable = (uint8) HDRSCMGMMCDP_DIVERSITY_COMB_ENABLED;

} /* hdrscmgmmcdp_set_config_to_default */

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMGMMCDP_CONFIG_IS_NEEDED

DESCRIPTION
  This function determines whether GMMCDP requires any configuration.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Bitmask indicating which attributes are needed to be (re)negotiated.

SIDE EFFECTS
  None.

===========================================================================*/
static uint8 hdrscmgmmcdp_config_is_needed( void )
{
  uint16 subtype;
    /* Negotiated subtype */
  hdrscmgmmcdp_multi_rx_capability_type sim_comm_chan_rcv;
    /* Desired SimultaneousCommonChannelReceive Attrib */
  hdrscmgmmcdp_diversity_comb_type diversity_comb;
    /* Desired Diversity Combining Enabled*/
  hdrscmgmmcdp_multi_rx_capability_type sim_comm_chan_rcv_cur;
    /* Current SimultaneousCommonChannelReceive Attrib */
  hdrscmgmmcdp_diversity_comb_type diversity_comb_cur;
    /* Current Diversity Combining Enabled*/
  uint8 config_needed = HDRSCMGMMCDP_CONFIG_NOT_NEEDED;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  subtype = hdrscp_get_config_subtype(HDRHAI_GENERIC_MM_CAP_DISC_PROTOCOL);

  if(subtype != HDRSCP_GENERIC_MMCDP)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                   "GMMCDP: AT init Neg not allowed for subtype %d",
                   subtype);
    return config_needed;
  }

  /* Check if AT have dual receiver chain and diversity combining */
  if (hdrlmac_has_multi_rx_capability
        (HDRSCMGMMCDP_MULTI_RX_CHAIN_FOR_COMMON_CHAN) )
  {
    sim_comm_chan_rcv = 
      HDRSCMGMMCDP_MULTI_RX_CHAIN_FOR_COMMON_CHAN;
  }
  else
  {
    sim_comm_chan_rcv =
      HDRSCMGMMCDP_SINGLE_RX_CHAIN_FOR_COMMON_CHAN;
  }

  if (hdrlmac_diversity_comb_enabled())
  {
    diversity_comb = HDRSCMGMMCDP_DIVERSITY_COMB_ENABLED;
  }
  else
  {
    diversity_comb = HDRSCMGMMCDP_DIVERSITY_COMB_DISABLED;
  }

  (void) hdrscmdb_get_inuse_attrib(HDRHAI_GENERIC_MM_CAP_DISC_PROTOCOL,
                                   HDRSCMGMMCDP_SIM_COMMON_CHAN_RCV_ATTRIB_ID,
                                   (void*) &sim_comm_chan_rcv_cur,
                                   HDRSCP_GENERIC_MMCDP);

  (void) hdrscmdb_get_inuse_attrib(HDRHAI_GENERIC_MM_CAP_DISC_PROTOCOL,
                                   HDRSCMGMMCDP_DIVERSITY_COMB_ATTRIB_ID,
                                   (void*) &diversity_comb_cur,
                                   HDRSCP_GENERIC_MMCDP);

  if (sim_comm_chan_rcv_cur != sim_comm_chan_rcv) 
  {
    config_needed |= HDRSCMGMMCDP_SIM_COMMON_CHAN_RCV_NEEDED;
  }

  if (diversity_comb_cur != diversity_comb)
  {
    config_needed |= HDRSCMGMMCDP_DIVERSITY_COMB_NEEDED;
  }

  HDR_MSG_PROT_1(MSG_LEGACY_MED, "GMMCDP config_needed %b",
                 config_needed);
                  
  return config_needed;
} /* hdrscmgmmcdp_config_is_needed() */

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
boolean hdrscmgmmcdp_configure
(
  hdrhai_protocol_name_enum_type sender
)
{
  dsm_item_type * send_msg_ptr = NULL;
    /* Pointer to Config req. message to send */

  uint8 attrib_offset = 0;
    /* Attribute field offset from start of DSM item */

  uint8 timer_id;
    /* ID of the reserved timer */

  uint8 config_needed;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Trace the command */
  HDRTRACE_CMD(HDRHAI_GENERIC_MM_CAP_DISC_PROTOCOL, 
               HDRSCMGMMCDP_CONFIGURE,
               sender, 
               0); 

  config_needed = hdrscmgmmcdp_config_is_needed();

  if (config_needed == HDRSCMGMMCDP_CONFIG_NOT_NEEDED)
  {
    return FALSE;
  }

  HDR_MSG_PROT( MSG_LEGACY_HIGH, "GMMCDP Config is required" );

  /* Reserve a Response timer for this GAUP update */
  if ( (hdrscm_reserve_rsp_timer(
                  HDRHAI_GENERIC_MM_CAP_DISC_PROTOCOL, &timer_id, 
                  hdrscmgmmcdp.trans_id, FALSE)) != E_SUCCESS)
  {
    /* No free timer was available. */
    return FALSE;
  }
  
  /* Get a new DSM buffer */
  send_msg_ptr = hdrutil_new_msg_buffer();

  /* Pack ConfigurationRequest message's Message ID */
  HDRBIT_PACK8(
    send_msg_ptr, HDRSCM_CONFIG_REQ_MSG, attrib_offset, HDRSCM_MESSAGE_ID_SIZE);

  /* Shift the offset to start of Transaction ID */
  attrib_offset += HDRSCM_MESSAGE_ID_SIZE;

  /* Pack the message's Transaction ID */
  HDRBIT_PACK8(send_msg_ptr, hdrscmgmmcdp.trans_id, attrib_offset, 
               HDRSCM_TRANS_ID_SIZE);

  /* Shift the offset to start of Attribute Record's Length */
  attrib_offset += HDRSCM_TRANS_ID_SIZE;

  /*-----------------------------------------------------------------------
     Pack SimultaneousCommonChannelReceive Attribute record if necessary
  -----------------------------------------------------------------------*/

  /* if SimuCommonReceiver setting is different from InUse */
  if (config_needed & HDRSCMGMMCDP_SIM_COMMON_CHAN_RCV_NEEDED)
  {
    /* Pack the Attribute Record */
  
    /* Pack the Length field */
    HDRBIT_PACK8(send_msg_ptr, 
                 ((HDRSCMGMMCDP_ATTRIB_ID_SIZE + HDRSCMGMMCDP_ATTRIB_VALUE_SIZE)/8), 
                 attrib_offset, 
                 HDRSCMGMMCDP_ATTRIB_LENGTH_SIZE);
  
    /* Shift the offset to start of Attribute ID field */
    attrib_offset += HDRSCMGMMCDP_ATTRIB_LENGTH_SIZE;
  
    /* Pack the Attribute ID of SimultaneousCommonChannelReceive attribute */
    HDRBIT_PACK8(send_msg_ptr, HDRSCMGMMCDP_SIM_COMMON_CHAN_RCV_ATTRIB_ID, 
                 attrib_offset, HDRSCMGMMCDP_ATTRIB_ID_SIZE);
  
    /* Shift the offset to start of Attribute Value field */
    attrib_offset += HDRSCMGMMCDP_ATTRIB_ID_SIZE;
    
    if ( hdrlmac_has_multi_rx_capability
           (HDRSCMGMMCDP_MULTI_RX_CHAIN_FOR_COMMON_CHAN) )
    {
      /* Pack the value of attribute SimultaneousCommonChannelReceive Attrib */
      HDRBIT_PACK8(send_msg_ptr, 
                   (uint8) HDRSCMGMMCDP_MULTI_RX_CHAIN_FOR_COMMON_CHAN, 
                   attrib_offset,HDRSCMGMMCDP_ATTRIB_VALUE_SIZE);
    }
    else
    {
      /* Pack the value of attribute SimultaneousCommonChannelReceive Attrib */
      HDRBIT_PACK8(send_msg_ptr, 
                   (uint8) HDRSCMGMMCDP_SINGLE_RX_CHAIN_FOR_COMMON_CHAN, 
                   attrib_offset,HDRSCMGMMCDP_ATTRIB_VALUE_SIZE);
    }
  
    attrib_offset += HDRSCMGMMCDP_ATTRIB_VALUE_SIZE;
  }

  /*-----------------------------------------------------------------------
            Pack ReceiveDiversity Attribute record if necessary
  -----------------------------------------------------------------------*/

  /* if DiversityComb setting is different from InUse */
  if (config_needed & HDRSCMGMMCDP_DIVERSITY_COMB_NEEDED)
  {
    /* Pack the Length field */
    HDRBIT_PACK8(send_msg_ptr, 
                 ((HDRSCMGMMCDP_ATTRIB_ID_SIZE + HDRSCMGMMCDP_ATTRIB_VALUE_SIZE)/8), 
                 attrib_offset, 
                 HDRSCMGMMCDP_ATTRIB_LENGTH_SIZE);
  
    /* Shift the offset to start of Attribute ID field */
    attrib_offset += HDRSCMGMMCDP_ATTRIB_LENGTH_SIZE;

    /* Pack the Attribute ID of SimultaneousCommonChannelReceive attribute */
    HDRBIT_PACK8(send_msg_ptr, HDRSCMGMMCDP_DIVERSITY_COMB_ATTRIB_ID, 
                 attrib_offset, HDRSCMGMMCDP_ATTRIB_ID_SIZE);
  
    /* Shift the offset to start of Attribute Value field */
    attrib_offset += HDRSCMGMMCDP_ATTRIB_ID_SIZE;
    
    if ( hdrlmac_diversity_comb_enabled() )
    {
      /* Pack the value of attribute SimultaneousCommonChannelReceive Attrib */
      HDRBIT_PACK8(send_msg_ptr, 
                   (uint8) HDRSCMGMMCDP_DIVERSITY_COMB_ENABLED,
                   attrib_offset,HDRSCMGMMCDP_ATTRIB_VALUE_SIZE);
    }
    else
    {
      /* Pack the value of attribute SimultaneousCommonChannelReceive Attrib */
      HDRBIT_PACK8(send_msg_ptr, 
                   (uint8) HDRSCMGMMCDP_DIVERSITY_COMB_DISABLED,
                   attrib_offset,HDRSCMGMMCDP_ATTRIB_VALUE_SIZE);
    }

    attrib_offset += HDRSCMGMMCDP_ATTRIB_VALUE_SIZE;
  }

  /* Send ConfigRequest msg on RTC. All config messages are reliable. */
  hdrhmp_send_msg(HDRHAI_GENERIC_MM_CAP_DISC_PROTOCOL,
                  HDRHAI_RTC,
                  TRUE,
                  HDRSCMGMMCDP_CONFIG_REQ_MSG_PRIO,
                  HDRHAI_IN_CONFIG,
                  send_msg_ptr,
                  hdrscm_tx_outcome_cb,
                  (void*) ((dword) timer_id) );

  /* Increment the transaction ID */
  ++hdrscmgmmcdp.trans_id;

  return TRUE;

} /* hdrscmgmmcdp_configure */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSCMGMMCDP_FAIL_CURR_CONFIG_NEG

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
void hdrscmgmmcdp_fail_curr_config_neg
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
  session_fail_data.type = HDRHAI_GENERIC_MM_CAP_DISC_PROTOCOL;
  session_fail_data.subtype = 
    hdrscp_get_config_subtype(HDRHAI_GENERIC_MM_CAP_DISC_PROTOCOL);
  session_fail_data.attribute_id_length = HDRSCMGMMCDP_ATTRIB_ID_SIZE;
  session_fail_data.attribute_id = HDRSCMGMMCDP_SIM_COMMON_CHAN_RCV_ATTRIB_ID;
  session_fail_data.reason = close_reason;

  ind_data.scp_fail_ptr = (void *)&session_fail_data;

  hdrind_give_ind(HDRIND_GMMCD_PROT_NEG_FAILED,(void *)&ind_data);

} /* hdrscmgmmcdp_fail_curr_config_neg */


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
)
{
  if ( (subtype == HDRSCP_GENERIC_MMCDP) &&
       ((attrib_id == HDRSCMGMMCDP_SIM_COMMON_CHAN_RCV_ATTRIB_ID) ||
        (attrib_id == HDRSCMGMMCDP_DIVERSITY_COMB_ATTRIB_ID)) )
  {
    return TRUE;
  }
  else 
  {
    return FALSE;
  }

} /* hdrscmalmp_attrib_is_supported */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSCMGMMCDP_GET_PROP_ATTRIB_VALUE

DESCRIPTION
  This function gets the value of the attribute proposed by AT during AT-init
  negotiation.

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
)
{ 
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT(MSG_LEGACY_MED, "hdrscmgmmcdp_get_prop_attrib_value()");

  /* Make sure Attribute Information pointer is not NULL */
  ASSERT(a_aip != NULL);

  if (trans_id == (uint8)(hdrscmgmmcdp.trans_id-1))
  {
    switch ( a_aip->attrib_id )
    {
    case HDRSCMGMMCDP_SIM_COMMON_CHAN_RCV_ATTRIB_ID:
      a_aip->value_id = (uint8) HDRSCMGMMCDP_MULTI_RX_CHAIN_FOR_COMMON_CHAN;
      a_aip->is_accepted = TRUE;
      break;

    case HDRSCMGMMCDP_DIVERSITY_COMB_ATTRIB_ID:
      a_aip->value_id = (uint8) HDRSCMGMMCDP_DIVERSITY_COMB_DISABLED;
      a_aip->is_accepted = TRUE;
      break;

    default:
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
          "GMMCDP: unsupported attrib_id %d", a_aip->attrib_id);
        /* attribute_id mismatch, ignore this attribute record */
      err_no = E_FAILURE;
      break;
    }
  }
  else
  {
    HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
         "GMMCDP: trans_id %d mismatched proposed trans_id %d", 
         trans_id, (uint8)(hdrscmgmmcdp.trans_id-1));
     /* trans_id mismatch, ignore this attribute record */
    err_no = E_FAILURE;
  }

  return err_no;
} /* hdrscmgmmcdp_get_prop_attrib_value */

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
boolean hdrscmgmmcdp_rf_setting_changed( void )
{ 
  hdrscmgmmcdp_multi_rx_capability_type sim_comm_chan_rcv;
    /* InUse copy of SimultaneousCommonChannelReceive Attrib */
  hdrscmgmmcdp_diversity_comb_type diversity_comb;
    /* InUse copy of Diversity Combining Enabled*/
  hdrscmgmmcdp_multi_rx_capability_type sim_comm_chan_rcv_ps;
    /* SimultaneousCommonChannelReceive Attrib based on current phone setting */
  hdrscmgmmcdp_diversity_comb_type diversity_comb_ps;
    /* Diversity Combining Attrib based on current phone setting*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(hdrscp_get_config_subtype(HDRHAI_GENERIC_MM_CAP_DISC_PROTOCOL) != 
     HDRSCP_GENERIC_MMCDP)
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                 "GMMCDP not negotiated, skip RF change check");
    return FALSE;
  }

  (void) hdrscmdb_get_inuse_attrib(HDRHAI_GENERIC_MM_CAP_DISC_PROTOCOL,
                                   HDRSCMGMMCDP_SIM_COMMON_CHAN_RCV_ATTRIB_ID,
                                   (void*) &sim_comm_chan_rcv,
                                   HDRSCP_GENERIC_MMCDP);

  (void) hdrscmdb_get_inuse_attrib(HDRHAI_GENERIC_MM_CAP_DISC_PROTOCOL,
                                   HDRSCMGMMCDP_DIVERSITY_COMB_ATTRIB_ID,
                                   (void*) &diversity_comb,
                                   HDRSCP_GENERIC_MMCDP);

  if (hdrlmac_has_multi_rx_capability
        (HDRSCMGMMCDP_MULTI_RX_CHAIN_FOR_COMMON_CHAN))
  {
    sim_comm_chan_rcv_ps = 
      HDRSCMGMMCDP_MULTI_RX_CHAIN_FOR_COMMON_CHAN;
  }
  else
  {
    sim_comm_chan_rcv_ps = HDRSCMGMMCDP_SINGLE_RX_CHAIN_FOR_COMMON_CHAN;
  }

  if (hdrlmac_diversity_comb_enabled())
  {
    diversity_comb_ps = HDRSCMGMMCDP_DIVERSITY_COMB_ENABLED;
  }
  else
  {
    diversity_comb_ps = HDRSCMGMMCDP_DIVERSITY_COMB_DISABLED;
  }

  if ((sim_comm_chan_rcv != sim_comm_chan_rcv_ps) ||
      (diversity_comb != diversity_comb_ps))
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "GMMCDP RF Setting has changed since last negotiation" );
    return TRUE;
  }
  return FALSE;

} /* hdrscmgmmcdp_rf_setting_changed */


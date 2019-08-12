/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

                    I D L E  S T A T E  P R O T O C O L

       
GENERAL DESCRIPTION
  This module provides supporting API for SCM module to handle AT-init Session
  Negotiation for SlottedMode Attribute of IdleStateProtocol
    
EXTERNALIZED FUNCTIONS
    
  hdrscmidle_is_attrib_simple 

  hdrscmidle_get_attrib_offset_and_size

  hdrscmidle_gaup_slotted_mode_attrib
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

  
  Copyright (c) 2005 - 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/25/12   wsh     Fixed MultiATPage GAUP not working
04/03/12   wsh     Added support for MultiATPage Msg
09/24/11   cnx     Changed subtype checking for Optimized Handoff.
02/17/09   pba     Do not accpet or send GAUP of slot cycle < 6 when 
                   SmallSlotCycleAllowed attribute is set to 0.
12/23/08   pba     Support for Quick Idle Protocol
12/10/08   smd     Replaced assert.h with amssassert.h
04/24/08   pba     AT initiated GAUP support for SCI7.
08/22/07   etv     Fixed high lint warnings.
05/22/07   sju     Added support for phase2 of AT-init GAUP of SlottedMode attribute. 
03/06/07   sju     Added support to allow overriding SCI multiple times.
01/04/07   dna/sju Fixed GAUPing SlottedMode attribute for QChat App.
05/15/06   etv     Created module.

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

#include "hdrscm.h"
#include "hdrscmdb.h"
#include "hdrscmidle.h"
#include "hdrscmrup.h"
#include "hdridle.h"
#include "hdridlec.h"


/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
/* Message priority for GAUP request message */
#define HDRSCMIDLE_ATTRIB_UPDATE_REQ_MSG_PRIO                   40

/* Attribute ID of the slotted Mode attribute */
#define HDRSCMIDLE_SLOTTED_MODE_ATTRIB_ID                       0x01

/* Size of the attribute ID in bits for Enh. Idle */
#define HDRSCMIDLE_ATTRIB_ID_SIZE                               8
/* Size of the value ID in bits for Enh. Idle */
#define HDRSCMIDLE_VALUE_ID_SIZE                                8
/* Size of slot cycle in bits */
#define HDRSCMIDLE_SM_SLOT_CYCLE_SIZE                           5
/* Size of Wake Count in bits */
#define HDRSCMIDLE_SM_WAKE_COUNT_SIZE                           4
/* Size of Reserved field in bits */
#define HDRSCMIDLE_SM_RESERVED_SIZE                             1 

/* Length of slotted mode attribute in octets */
#define HDRSCMIDLE_SLOTTED_MODE_ATTRIB_SIZE                     5

#ifdef FEATURE_HDR_REVC
#define HDRSCMIDLE_MULTI_AT_PAGE_SUPPORTED_ATTRIB_ID            0xFB

#define HDRSCMIDLE_MULTI_AT_PAGE_UNSUPPORTED                    0x00
#define HDRSCMIDLE_MULTI_AT_PAGE_SUPPORTED                      0x01

#define HDRSCMIDLE_MULTI_AT_PAGE_LEN_SIZE                       0x02
#endif /* FEATURE_HDR_REVC */

 /* EJECT */
/*===========================================================================
            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* A max of only 2 outstanding AttribUpdateRequest transactions are allowed at
   any instance. */
#define HDRSCMIDLE_NUM_CONCURRENT_GAUPS       2

#define HDRSCMIDLE_ATTRIB_REQ_SLOTTED_MODE    0x01
#ifdef FEATURE_HDR_REVC
#define HDRSCMIDLE_ATTRIB_REQ_MULTI_AT_PAGE   0x02
#endif /* FEATURE_HDR_REVC */

/* This struct encapsulates an InConfig AttribUpdateRequest Transaction. */
typedef struct
{
  uint8 trans_id;
    /* Transaction ID of this transaction */

  uint8 attrib_req;
    /* Attributes that have been requested in one transation */

  hdridlec_slotted_mode_type slotted_mode;

} hdrscmidle_sm_inconfig_trans_struct_type;

/*------------------------------------------------------------------------
                            GLOBAL VARIABLES
 -----------------------------------------------------------------------*/

static struct
{
  /* Transaction ID for next AT-init Sesssion Negotiation */
  uint8 trans_id;

  /* Array of InConfig transaction instances */
  hdrscmidle_sm_inconfig_trans_struct_type 
    in_config[HDRSCMIDLE_NUM_CONCURRENT_GAUPS];

  hdridlec_saved_scmidle_info_type saved_scmidle_info;

} hdrscmidle;

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

FUNCTION HDRSCMIDLE_IS_ATTRIB_SIMPLE

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
boolean hdrscmidle_is_attrib_simple
(
  uint16 const attrib_id,
  uint16 const subtype
)
{
  /* There are no simple attributes in Enh IDLE under SCM */
  return FALSE;

} /* hdrscmidle_is_attrib_simple */


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMIDLE_GAUP_SLOTTED_MODE_ATTRIB

DESCRIPTION
  This function is used go GAUP slotted Mode attribute.
  
DEPENDENCIES
  None.

PARAMETERS
  sm_override -  What type of override to start:
                 HDRSCMIDLE_SM_OVERRIDE_NONE
                 HDRSCMIDLE_SM_OVERRIDE_426_MSECS
                 HDRSCMIDLE_SM_OVERRIDE_213_MSECS

  p_trans_id - Address where the transaction ID is stored to be returned back.

RETURN VALUE
  E_SUCCESS - If a GAUP Update was queued to HDRMC successfully.
  E_NOT_AVAILABLE - If we didn't have a free Rsp timer to send this GAUP update.
  E_FAILURE - If attempting to turn off override when no stored config is 
              available i.e. no override was done previously.
              AT couldn't retrieve its current slotted mode config.
              sm_override out of range.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmidle_gaup_slotted_mode_attrib
(
  hdrscmidle_sm_override_enum_type sm_override,
  uint8 *p_trans_id
)
{

  dsm_item_type * send_msg_ptr = NULL;
    /* Pointer to gaup req. message to send */

  uint8 attrib_offset = 0;
    /* Attribute field offset from start of DSM item */

  uint8 timer_id;
    /* ID of the reserved timer */

  uint8 value_id = 0;
    /* Value Id of complex attrib Rate Param */

  uint8 gaup_ind = 0;
    /* Index into Inconfig GAUP transaction Array */

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

  hdridlec_slotted_mode_type current_slotted_mode; /* Current SM value */
  hdridlec_slotted_mode_type slotted_mode; /* Attribute to propose */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!hdridlec_is_small_slotcycle_gaup_allowed() && 
      (sm_override == HDRSCMIDLE_SM_OVERRIDE_213_MSECS))
  {
    return E_FAILURE;
  }

  /* Reserve a Response timer for this GAUP update */
  if ( (err_no = hdrscm_reserve_rsp_timer( HDRHAI_IDLE_STATE_PROTOCOL, &timer_id, 
                                  hdrscmidle.trans_id, TRUE)) != E_SUCCESS)
  {
    /* No free timer was available. Forward the error code to Apps! */
    return err_no;
  }

  if (sm_override >= HDRSCMIDLE_SM_OVERRIDE_INVALID)
  {
    return E_FAILURE;
  }

  if ((sm_override == HDRSCMIDLE_SM_OVERRIDE_NONE) &&
      ( hdrscmidle.saved_scmidle_info.saved_slotted_mode_is_valid == FALSE))
  {
    return E_FAILURE;
  }

  if ((sm_override != HDRSCMIDLE_SM_OVERRIDE_NONE) &&
      ( hdrscmidle.saved_scmidle_info.saved_slotted_mode_is_valid == FALSE))
  {
    if (!hdridlec_get_slotted_mode_attrib(&current_slotted_mode))
    {
      return E_FAILURE;
    }

    /* Save original SCI to be used later when we want to restore the original values */
    hdrscmidle.saved_scmidle_info.saved_slotted_mode = current_slotted_mode;
    hdrscmidle_set_scmidle_info_valid(TRUE);
  }

  if (sm_override == HDRSCMIDLE_SM_OVERRIDE_NONE)
  {
    ASSERT(hdrscmidle.saved_scmidle_info.saved_slotted_mode_is_valid == TRUE);
    slotted_mode = hdrscmidle.saved_scmidle_info.saved_slotted_mode;
  }
  else 
  {
    slotted_mode.wake_count1 = 0;
    slotted_mode.wake_count2 = 0;
    if (sm_override == HDRSCMIDLE_SM_OVERRIDE_1280_MSECS)
    {
      slotted_mode.slot_cycle1 = 7;
      slotted_mode.slot_cycle2 = 7;
      slotted_mode.slot_cycle3 = 7;
    }
    else if (sm_override == HDRSCMIDLE_SM_OVERRIDE_426_MSECS)
    {
      slotted_mode.slot_cycle1 = 6;
      slotted_mode.slot_cycle2 = 6;
      slotted_mode.slot_cycle3 = 6;
    }
    else
    {
      ASSERT(sm_override == HDRSCMIDLE_SM_OVERRIDE_213_MSECS);
      slotted_mode.slot_cycle1 = 5;
      slotted_mode.slot_cycle2 = 5;
      slotted_mode.slot_cycle3 = 5;
    }
  }

  /* Index of current Transaction in the in_config array */
  gaup_ind = hdrscmidle.trans_id % HDRSCMIDLE_NUM_CONCURRENT_GAUPS;

  /* Fill the transaction details: SlottedMode and transaction ID */
  hdrscmidle.in_config[gaup_ind].trans_id = hdrscmidle.trans_id;
  hdrscmidle.in_config[gaup_ind].slotted_mode = slotted_mode;

  /* Clear previous record */
  hdrscmidle.in_config[gaup_ind].attrib_req &= 0;
  /* Indicate this is a slotted mode attribute update request */
  hdrscmidle.in_config[gaup_ind].attrib_req |= HDRSCMIDLE_ATTRIB_REQ_SLOTTED_MODE;

  /* Assign an ID for the current transaction and increment the global variable
     for next transaction */
  *p_trans_id = hdrscmidle.trans_id++;

  /* Get a new DSM buffer */
  send_msg_ptr = hdrutil_new_msg_buffer();

  /* Pack AttribUpdateRequest message's Message ID */
  HDRBIT_PACK8(send_msg_ptr, HDRSCM_ATTRIB_UPDATE_REQ_MSG,
               attrib_offset, HDRSCM_MESSAGE_ID_SIZE );

  /* Shift the offset to start of Transaction ID */
  attrib_offset += HDRSCM_MESSAGE_ID_SIZE;

  /* Pack the message's Transaction ID */
  HDRBIT_PACK8(send_msg_ptr, *(p_trans_id), attrib_offset, 
               HDRSCM_TRANS_ID_SIZE);

  /* Shift the offset to start of Attribute Record's Length */
  attrib_offset += HDRSCM_TRANS_ID_SIZE;

  /* Pack the Length field of the attribute Record */
  HDRBIT_PACK8(send_msg_ptr, HDRSCMIDLE_SLOTTED_MODE_ATTRIB_SIZE,
               attrib_offset, HDRSCM_LENGTH_SIZE);

  /* Shift the offset to start of Attribute ID field */
  attrib_offset += HDRSCM_LENGTH_SIZE;

  /* Pack the Attribute ID of the SlottedMode attribute */
  HDRBIT_PACK8(send_msg_ptr, HDRSCMIDLE_SLOTTED_MODE_ATTRIB_ID, attrib_offset,
               HDRSCMIDLE_ATTRIB_ID_SIZE);

  /* Shift the offset to start of Value ID */
  attrib_offset += HDRSCMIDLE_ATTRIB_ID_SIZE;

  /* Pack the value ID */
  HDRBIT_PACK8(send_msg_ptr, value_id, attrib_offset,
               HDRSCMIDLE_VALUE_ID_SIZE);

  /* Shift the offset to start of SlotCycle1 field */
  attrib_offset += HDRSCMIDLE_VALUE_ID_SIZE;

  /* Pack the SlotCycle1 field */
  HDRBIT_PACK8(send_msg_ptr, slotted_mode.slot_cycle1,
               attrib_offset, HDRSCMIDLE_SM_SLOT_CYCLE_SIZE);

  /* Shift the offset to start of SlotCycle2 field */
  attrib_offset += HDRSCMIDLE_SM_SLOT_CYCLE_SIZE;

  /* Pack the SlotCycle2 field */
  HDRBIT_PACK8(send_msg_ptr, slotted_mode.slot_cycle2,
               attrib_offset, HDRSCMIDLE_SM_SLOT_CYCLE_SIZE);

  /* Shift the offset to start of SlotCycle3 field */
  attrib_offset += HDRSCMIDLE_SM_SLOT_CYCLE_SIZE;

  /* Pack the SlotCycle3 field */
  HDRBIT_PACK8(send_msg_ptr, slotted_mode.slot_cycle3,
               attrib_offset, HDRSCMIDLE_SM_SLOT_CYCLE_SIZE);

  /* Shift the offset to start of WakeCount1 field */
  attrib_offset += HDRSCMIDLE_SM_SLOT_CYCLE_SIZE;

  /* Pack the WakeCount1 field */
  HDRBIT_PACK8(send_msg_ptr, slotted_mode.wake_count1,
               attrib_offset, HDRSCMIDLE_SM_WAKE_COUNT_SIZE);

  /* Shift the offset to start of WakeCount2 field */
  attrib_offset += HDRSCMIDLE_SM_WAKE_COUNT_SIZE;

  /* Pack the WakeCount2 field */
  HDRBIT_PACK8(send_msg_ptr, slotted_mode.wake_count2,
               attrib_offset, HDRSCMIDLE_SM_WAKE_COUNT_SIZE);

  /* Shift the offset to start of Reserved field */
  attrib_offset += HDRSCMIDLE_SM_WAKE_COUNT_SIZE;

  /* Pack the reserved field */
  HDRBIT_PACK8(send_msg_ptr,
               0,
               attrib_offset,
               HDRSCMIDLE_SM_RESERVED_SIZE);

  /* Send AttribUpdateRequest msg on RTC. All config messages are reliable. */
  hdrhmp_send_msg(HDRHAI_IDLE_STATE_PROTOCOL,
                  HDRHAI_RTC,
                  TRUE,
                  HDRSCMIDLE_ATTRIB_UPDATE_REQ_MSG_PRIO,
                  HDRHAI_IN_USE,
                  send_msg_ptr,
                  hdrscm_tx_outcome_cb,
                  (void*) ((dword) timer_id) );

  return err_no;

} /* hdrscmidle_gaup_slotted_mode_attrib */

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMIDLE_GET_GAUPED_COMPLEX_ATTRIB_VALUE

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
  attrib_count  - Number of attributes GAUPed in this transaction.
 
RETURN VALUE
  E_SUCCESS       - If the given transaction ID is valid.
  E_NOT_AVAILABLE - If a valid transaction specified by 'trans_id' is not 
                    available.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmidle_get_gauped_complex_attrib_value
(
  uint8 const trans_id,
  dsm_item_type **aai_dsm,
  uint8 *const attrib_count,
  uint16 subtype
)
{
  uint8 gaup_ind = (trans_id % HDRSCMIDLE_NUM_CONCURRENT_GAUPS);
    /* Index into InConfig array */

  hdrerrno_enum_type err_no = E_NOT_AVAILABLE;
    /* Error Code. A valid transaction not available! */

  hdrscm_accepted_attrib_type gauped_attribs;
    /* Accepted Attribute information type for the GAUPed attribute */

  hdrscm_accepted_attrib_type *a_aip = &gauped_attribs;
    /* Pointer to the GAUPed attribute */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdrscmidle.in_config[gaup_ind].trans_id == trans_id)
  {
    /* Found a valid transaction; Set error code to success */
    err_no = E_SUCCESS;

    /* It is possible that one attribute update request contains several
       attributes. In IDLE state, there are two attributes GAUPable - SlottedMode
       and MultiATPageMessageSupported. The update requests for the two
       attributes will not be triggered at the same time so that one attribute
       request only contains one attribute at most. When there are multiple
       attributes requested in the same update request, the following code must be
       changed */
    if ( hdrscmidle.in_config[gaup_ind].attrib_req & HDRSCMIDLE_ATTRIB_REQ_SLOTTED_MODE )
    {
      /* Populate the attribute information pointer array's first index with the
         InConfig instance of SlottedMode attribute GAUPed in this 
         transaction */
      a_aip->complex_val_ptr = &hdrscmidle.in_config[gaup_ind].slotted_mode;
      a_aip->attrib_id = HDRSCMIDLE_SLOTTED_MODE_ATTRIB_ID;
      a_aip->is_accepted = TRUE;
      a_aip->complex_val_size = sizeof(hdridlec_slotted_mode_type);
    }
#ifdef FEATURE_HDR_REVC
    else if ( hdrscmidle.in_config[gaup_ind].attrib_req & HDRSCMIDLE_ATTRIB_REQ_MULTI_AT_PAGE )
    {
      a_aip->attrib_id = HDRSCMIDLE_MULTI_AT_PAGE_SUPPORTED_ATTRIB_ID;
      /* MultiATPageMessageSupported is supported */
      a_aip->value_id = HDRSCMIDLE_MULTI_AT_PAGE_SUPPORTED; 
      a_aip->is_accepted = TRUE;
    }
#endif /* FEATURE_HDR_REVC */

    *attrib_count = 1;

   (void) dsm_pushdown(aai_dsm, 
                       &gauped_attribs, 
                       sizeof(hdrscm_accepted_attrib_type), 
                       DSM_DS_SMALL_ITEM_POOL);
  }

  return err_no;

} /* hdrscmidle_get_gauped_complex_attrib_value */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMIDLE_GET_ATTRIB_OFFSET_AND_SIZE

DESCRIPTION
  To get the attribute's byte offset into the config struct and its size.

DEPENDENCIES
  None

PARAMETERS
  attrib_id     - Attribute Id.
  attrib_offset - Address where the Byte Offset from the start of the struct 
                  to the attribute is stored.
  attrib_size   - Address where the size of the attribute is stored.
  subtype       - subtype.
  
RETURN VALUE
  E_SUCCESS - If the given attrib ID is valid.
  E_DATA_INVALID - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrscmidle_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
)
{
  hdrerrno_enum_type err_no = E_DATA_INVALID;
    /* error code */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the subtype is Enhanced or Quick Idle and the attribute ID is for slotted Mode */
  if ( ( subtype != HDRSCP_DEFAULT_IDLE ) &&
       ( attrib_id == HDRSCMIDLE_SLOTTED_MODE_ATTRIB_ID ) )
  {
    /* Set the error code to success */
    err_no = E_SUCCESS;

    /* Return the attribute offset and size */
    *attrib_offset = FPOS(hdridlec_config_params_type, slotted_mode);
    *attrib_size = sizeof(hdridlec_slotted_mode_type);
  }

#ifdef FEATURE_HDR_REVC
  else if ((subtype != HDRSCP_DEFAULT_IDLE) &&
           (attrib_id == HDRSCMIDLE_MULTI_AT_PAGE_SUPPORTED_ATTRIB_ID))
  {
    /* Set the error code to success */
    err_no = E_SUCCESS;

    /* Return the attribute offset and size */
    *attrib_offset = FPOS(hdridlec_config_params_type, multi_page_supported);
    *attrib_size = sizeof(boolean);
  }
#endif /* FEATURE_HDR_REVC */

  return err_no;

} /* hdrscmidle_get_attrib_offset_and_size */

/*===========================================================================

FUNCTION HDRSCMIDLE_INIT_SCMIDLE_INFO

DESCRIPTION
  This function is used to initiate scmidle information. 
  
DEPENDENCIES
  None.

PARAMETERS
  saved_scmidle_info_ptr - pointer to the saved sm info

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void hdrscmidle_init_scmidle_info ( hdridlec_saved_scmidle_info_type *saved_scmidle_info_ptr )
{
  hdrscmidle.saved_scmidle_info = *saved_scmidle_info_ptr;
} /* hdrscmidle_init_scmidle_info*/

/*===========================================================================

FUNCTION HDRSCMIDLE_saved_scmidle_INFO_IS_VALID

DESCRIPTION
  This function returns if hdrscmidle.saved_scmidle_info.saved_slotted_mode  
  is valid.  
  
DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrscmidle_saved_scmidle_info_is_valid ( )
{
  return hdrscmidle.saved_scmidle_info.saved_slotted_mode_is_valid;
} /* hdrscmidle_save_sm_info_is_valid */

/*===========================================================================

FUNCTION HDRSCMIDLE_SET_saved_scmidle_INFO_VALID

DESCRIPTION
  This function is used to set if hdrscmidle.saved_scmidle_info.saved_slotted_mode  
  is valid.  
  
DEPENDENCIES
  None.

PARAMETERS
  saved_scmidle_info_is_valid - if saved slottedmode info is valid 

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void hdrscmidle_set_scmidle_info_valid ( boolean saved_scmidle_info_is_valid )
{
  hdrscmidle.saved_scmidle_info.saved_slotted_mode_is_valid = saved_scmidle_info_is_valid;

  /* Update scmdb*/
  hdrscmdb_set_idle_scmidle_info(&hdrscmidle.saved_scmidle_info);

} /* hdrscmidle_set_saved_scmidle_info_valid */


#ifdef FEATURE_HDR_REVC
/*===========================================================================
 
FUNCTION HDRSCMIDLE_GAUP_MULTI_AT_PAGE_SUPPORTED

DESCRIPTION
  This function sends MultiATPageMsgSupported attribute update request. 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS       - Update request was sent successfully.
  E_NOT_AVAILABLE - If we didn't have a free Rsp timer or free msg buffer
                    to send this GAUP update.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmidle_gaup_multi_at_page_supported( void )
{
  uint8 timer_id;
    /* Reserved timer Id */

  dsm_item_type * send_msg_ptr = NULL;
    /* Pointer to message buffer */

  uint8 gaup_ind = 0;
    /* Index into Inconfig GAUP transaction Array */

  uint8 tran_id = 0;
    /* transcation ID */

  uint8 attrib_offset = 0;
    /* Attribute field offset from start of DSM item */

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                "AT-init MultiATPageMsgSupported(0xFB) update req" );

  /* Reserve a Response timer for this Config Request message */
  if ( (err_no = hdrscm_reserve_rsp_timer(HDRHAI_IDLE_STATE_PROTOCOL, &timer_id, 
                           hdrscmidle.trans_id, FALSE)) != E_SUCCESS )
  {
    /* No free timer was available */
    HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                  "Don't have a free Rsp timer for MultiPageSupp update req" );
    return err_no;
  }

  /* Get a new DSM buffer */
  send_msg_ptr = hdrutil_new_msg_buffer();    
  if (send_msg_ptr == NULL)
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, 
                  "Can't alloc msg buffer for MultiPageSupp update req" );
    return E_NOT_AVAILABLE;
  }

  /* Index of current transaction in the in_config array */
  gaup_ind = hdrscmidle.trans_id % HDRSCMIDLE_NUM_CONCURRENT_GAUPS;

  /* Fill the transaction details */
  hdrscmidle.in_config[gaup_ind].trans_id = hdrscmidle.trans_id;
  /* Clear previous record */
  hdrscmidle.in_config[gaup_ind].attrib_req &= 0;
  /* Indicate this is a MultiATPageMsgSupported attribute update request */
  hdrscmidle.in_config[gaup_ind].attrib_req |= HDRSCMIDLE_ATTRIB_REQ_MULTI_AT_PAGE;

  /* Assign an ID for the current transaction and increment the global variable
     for next transaction */
  tran_id = hdrscmidle.trans_id++;

  /* Pack AttribUpdateRequest message's Message ID */
  HDRBIT_PACK8( send_msg_ptr, HDRSCM_ATTRIB_UPDATE_REQ_MSG,
                attrib_offset, HDRSCM_MESSAGE_ID_SIZE );
  attrib_offset += HDRSCM_MESSAGE_ID_SIZE;

  /* Pack the message's Transaction ID */
  HDRBIT_PACK8( send_msg_ptr, tran_id, attrib_offset, HDRSCM_TRANS_ID_SIZE );
  attrib_offset += HDRSCM_TRANS_ID_SIZE;

  /* Length */
  HDRBIT_PACK8( send_msg_ptr, HDRSCMIDLE_MULTI_AT_PAGE_LEN_SIZE,
                attrib_offset, HDRSCM_LENGTH_SIZE );
  attrib_offset += HDRSCM_LENGTH_SIZE;

  /* AttributeID */
  HDRBIT_PACK8( send_msg_ptr, HDRSCMIDLE_MULTI_AT_PAGE_SUPPORTED_ATTRIB_ID,
                attrib_offset, HDRSCMIDLE_ATTRIB_ID_SIZE );
  attrib_offset += HDRSCMIDLE_ATTRIB_ID_SIZE;

  /* ValueIDs */
  HDRBIT_PACK8( send_msg_ptr, HDRSCMIDLE_MULTI_AT_PAGE_SUPPORTED,
                attrib_offset, HDRSCMIDLE_VALUE_ID_SIZE );
  attrib_offset += HDRSCMIDLE_VALUE_ID_SIZE;

  /* Send AttributeUpdateRequest msg on RTC */
  hdrhmp_send_msg( HDRHAI_IDLE_STATE_PROTOCOL,
                   HDRHAI_RTC,
                   TRUE,
                   HDRSCMIDLE_ATTRIB_UPDATE_REQ_MSG_PRIO,
                   HDRHAI_IN_USE,
                   send_msg_ptr,
                   hdrscm_tx_outcome_cb,
                   (void*) ((dword) timer_id) );

  HDR_MSG_PROT_2( MSG_LEGACY_MED, "GAUP MultiATPage T:%d G:%d",
                  tran_id, gaup_ind );

  return err_no;
} /* hdrscmidle_gaup_multi_at_page_supported */


/*=========================================================================== 

FUNCTION HDRSCMIDLE_RESET_MULTI_AT_PAGE_GAUP_CONDITIONS

DESCRIPTION
  Reset the variables which determine the conditions of triggering
  MultiATPageMessageSupported attribute update request.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void hdrscmidle_reset_multi_at_page_gaup_conditions( void )
{
  HDR_MSG_PROT( MSG_LEGACY_MED, 
                "Reset MultiATPageMsgSupported GAUP trigger conditions" );

  hdridle_set_multi_at_page_attrib_rejected( FALSE );

  hdridle_set_multi_at_page_msg_rcvd( FALSE );

} /* hdrscmidle_reset_multi_at_page_gaup_conditions */

/*=========================================================================== 

FUNCTION HDRSCMIDLE_IS_MULTI_AT_PAGE_GAUP

DESCRIPTION
  This function checks whether a MultiATPageMessageSupported update
  request/response based on the GAUP transcation ID.

DEPENDENCIES
  None

PARAMETERS
  trans_id - GAUP transcation ID.

RETURN VALUE
  TRUE - a MultiATPageMessageSupported update request/response
  FALSE - not a MultiATPageMessageSupported update request/response

SIDE EFFECTS
  None
 
===========================================================================*/
boolean hdrscmidle_is_multi_at_page_gaup
(   
  uint8 const trans_id
)
{
  uint8 gaup_ind = (trans_id % HDRSCMIDLE_NUM_CONCURRENT_GAUPS);

  if ( hdrscmidle.in_config[gaup_ind].attrib_req & 
	   HDRSCMIDLE_ATTRIB_REQ_MULTI_AT_PAGE )
  {
    return TRUE;
  }

  return FALSE;

} /* hdrscmidle_is_multi_at_page_gaup */
#endif /* FEATURE_HDR_REVC */

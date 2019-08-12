#ifndef DS_TFT_BLDR_H
#define DS_TFT_BLDR_H

/*===========================================================================

                        DYNAMIC QOS MANAGER INTERNAL

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2004-2005 by Qualcomm Technologies Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $Header:
  $DateTime: 2016/12/13 08:00:03 $
  $Author:

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/28/05   az/msr  Initial version

===========================================================================*/


/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_HDR_QOS

#include "ds707_pkt_mgri.h"
#include "ps_iface_defs.h"


/*===========================================================================

                      DATA DECLARATIONS

===========================================================================*/


/* 3GPP2 class number */
#define DS_TFT_BLDR_3GPP2_CLASS_NUM         231
#define DS_TFT_BLDR_3GPP2_CLASS_NUM_POS     16
#define DS_TFT_BLDR_3GPP2_CLASS_NUM_LENGTH  8

/* 3GPP2 c-type */
#define DS_TFT_BLDR_3GPP2_C_TYPE            1
#define DS_TFT_BLDR_3GPP2_C_TYPE_POS        24
#define DS_TFT_BLDR_3GPP2_C_TYPE_LENGTH     8



/* PF type field and its length in bits */
#define DS_TFT_BLDR_PF_TYPE_0         0
#define DS_TFT_BLDR_PF_TYPE_LENGTH    8


/* precedence for TX filter */                                  
#define DS_TFT_BLDR_PRECEDENCE_FOR_TX_FLTR     0xFF

  
/* The Non-Specific bit.  This bit indicates the type of FLOW_ID-to-A10 
   connection mapping requested for the associated TFT. When set, it 
   indicates that the FLOW_ID-to-A10 connection mapping will be dictated
   by the RAN in A11 signaling. 
*/
#define DS_TFT_BLDR_NS_1         1


/* SR_ID */
#define DS_TFT_BLDR_UNUSED_SR_ID  0


/* Persistency bit.
   The persistency bit is set to '1' to indicate a request from the MS to 
   keep the TFT even if the A10 connection is not established or 
   disconnected at the PDSN.
*/
#define DS_TFT_BLDR_PERSISTENCY  1


/* Transaction id position and length */
#define DS_TFT_BLDR_TS_ID_POSITION   32
#define DS_TFT_BLDR_TS_ID_LENGTH     32

/*Maximum number of packet filters allowed*/
/*X.S0011-004 B.1.1.1.1 defines 8 bits for number of packet filters*/
#define DS_TFT_BLDR_MAX_PACKET_FILTERS 255
                                        
                
/* Information element types (Refer B.1.1.1 of IS-835D.4)
   This is sent to PDSN in 3GPP2 object.
*/
typedef enum
{
  DS_TFT_BLDR_IE_TFT_IPV4               =  0,
  DS_TFT_BLDR_IE_TFT_IPV4_ERR           =  1,
  DS_TFT_BLDR_IE_TFT_IPV6               =  2,
  DS_TFT_BLDR_IE_TFT_IPV6_ERR           =  3,

  DS_TFT_BLDR_IE_HEADER_REMOVAL         =  4,
  DS_TFT_BLDR_IE_HEADER_REMOVAL_ERR     =  5,

  DS_TFT_BLDR_IE_CHANNEL_TREATMENT      =  6,
  DS_TFT_BLDR_IE_CHANNEL_TREATMENT_ERR  =  7

} ds_tft_bldr_tft_ie_type;



/* TFT operation code (Refer section B.1.1.1 IS-835D.4) */
typedef enum
{
  DS_TFT_BLDR_OPCODE_SPARE           =  0,
  DS_TFT_BLDR_OPCODE_CREATE_TFT      =  1,
  DS_TFT_BLDR_OPCODE_DELETE_TFT      =  2,
  DS_TFT_BLDR_OPCODE_ADD_FLTRS     =  3,
  DS_TFT_BLDR_OPCODE_REPLACE_FLTRS =  4,
  DS_TFT_BLDR_OPCODE_DELETE_FLTRS  =  5,
  DS_TFT_BLDR_OPCODE_QOS_CHECK          =  6,
  DS_TFT_BLDR_OPCODE_RESERVED1          =  7,
  DS_TFT_BLDR_OPCODE_RESERVED2          =  8,
  DS_TFT_BLDR_OPCODE_INITIATE_FLOW_REQUEST  = 128,
  DS_TFT_BLDR_OPCODE_QOS_CHECK_CONFIRM  =  129, 
  DS_TFT_BLDR_OPCODE_INITIATE_DELETE_FLTRS  = 130,
  DS_TFT_BLDR_OPCODE_INITIATE_REPLACE_FLTRS = 131,
  DS_TFT_BLDR_OPCODE_MAX
} ds_tft_bldr_tft_opcode_type;



/* Direction 'D' bit (Refer section B.1.1.1 IS-835D.4) */
typedef enum
{
  DS_TFT_BLDR_D_FWD           =  0,
  DS_TFT_BLDR_D_REV           =  1,
  DS_TFT_BLDR_D_FWD_AND_REV   =  2,

} ds_tft_bldr_direction_type;


/* Packet filter component type identifiers
  (Refer section B.1.1.1 IS-835D.4) */
typedef enum
{
  DS_TFT_BLDR_FLTR_TYPE_IPV4_SRC_ADDR                    = 16,
  DS_TFT_BLDR_FLTR_TYPE_IPV6_SRC_ADDR                    = 32,

  DS_TFT_BLDR_FLTR_TYPE_IPV4_DSTN_ADDR                   = 17,
  DS_TFT_BLDR_FLTR_TYPE_IPV6_DSTN_ADDR                   = 33,

  DS_TFT_BLDR_FLTR_TYPE_PROTOCOL_HEADER                  = 48,

  DS_TFT_BLDR_FLTR_TYPE_SINGLE_DSTN_PORT                 = 64,
  DS_TFT_BLDR_FLTR_TYPE_DSTN_PORT_RANGE                  = 65,

  DS_TFT_BLDR_FLTR_TYPE_SINGLE_SRC_PORT                  = 80,
  DS_TFT_BLDR_FLTR_TYPE_SRC_PORT_RANGE                   = 81,

  DS_TFT_BLDR_FLTR_TYPE_SECURITY_PARAM_INDEX             = 96,
  DS_TFT_BLDR_FLTR_TYPE_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS = 112,
  DS_TFT_BLDR_FLTR_TYPE_FLOW_LABEL                       = 128,
  DS_TFT_BLDR_FLTR_TYPE_TYPE2_ROUTING_HEADER             = 129,
  DS_TFT_BLDR_FLTR_TYPE_HOME_ADDR_OPTION                 = 130

} ds_tft_bldr_fltr_component_type;

/* RESULT CODE */
typedef enum
{
  DS_TFT_BLDR_SUCCESS                                   =  0,
  DS_TFT_BLDR_UE_INITIATED_QOS_NOT_AUTHORIZED           =  1,
  DS_TFT_BLDR_RESULT_INVALID                                  
} ds_tft_bldr_result_code_type;

/*Parameters for ResvErr Message*/

/*TFT error codes. Define the error code according to X.S0011-004 B.3.1 
  table 13.*/
typedef enum
{
  DS_TFT_BLDR_NONE_ERR                            = 0,
  DS_TFT_BLDR_PACKET_FILTER_ADD_FAILURE           = 1,
  DS_TFT_BLDR_PACKET_FILTER_UNAVAILABLE           = 2,
  DS_TFT_BLDR_UNSUCCESSFUL_TFT_PROCESSING         = 3, 
  DS_TFT_BLDR_CHANNEL_NOT_AVAILABLE               = 4,
  DS_TFT_BLDR_EVALUATION_PRECEDENCE_CONTENTION    = 5, 
  DS_TFT_BLDR_TREATMENT_NOT_SUPPORTED             = 6,
  DS_TFT_BLDR_PACKET_FILTER_REPLACE_FAILURE       = 7,
  DS_TFT_BLDR_PERSISTENCY_LIMIT_REACHED           = 8,
  DS_TFT_BLDR_PERSISTENCY_NOT_ALLOWED             = 9,
  DS_TFT_BLDR_UNAUTHORIZED_TFT                    = 10,
  DS_TFT_BLDR_MAX_NUM_PACKET_FILTERS_REACHED      = 11,
  DS_TFT_BLDR_NONEXISTENT_TFT                     = 12,
  DS_TFT_BLDR_NW_UNSUCCESSFUL_TFT_PROCESSING      = 128, 
  DS_TFT_BLDR_NW_INVALID_IP_ADDRESS               = 129,
  DS_TFT_BLDR_FLOW_PROFILE_IDS_NOT_SUPPORTED      = 130,
  DS_TFT_BLDR_INVALID_PDN_ID                      = 131,
  DS_TFT_BLDR_NW_QOS_CHECK_NEEDS_TO_BE_PERFORMED  = 132,
  DS_TFT_BLDR_NW_INITIATED_QOS_IN_PROGRESS        = 133,
  DS_TFT_BLDR_NW_EVALUATION_PRECEDENCE_CONTENTION = 134 
} ds_tft_bldr_tft_error_code;

/*Unpacking error code. The error code is defined for internal only.*/
typedef enum
{
  DS_TFT_BLDR_NULL_PTR            = 0,
  DS_TFT_BLDR_FAILED_MEM_ALLOC    = 1,
  DS_TFT_BLDR_INVALID_C_TYPE      = 2,
  DS_TFT_BLDR_INVALID_CLASS_NUM   = 3,
  DS_TFT_BLDR_INVALID_LEN         = 4,
  DS_TFT_BLDR_INVALID_TS_ID       = 5,
  DS_TFT_BLDR_INVALID_IE_TYPE     = 6,
  DS_TFT_BLDR_INVALID_DIRECTION   = 7,
  DS_TFT_BLDR_INVALID_NS          = 8,
  DS_TFT_BLDR_INVALID_SR_ID       = 9,
  DS_TFT_BLDR_INVALID_P           = 10,
  DS_TFT_BLDR_INVALID_OP_CODE     = 11,
  DS_TFT_BLDR_INVALID_NUM_FILTERS = 12,
  DS_TFT_BLDR_INVALID_FLOW_ID     = 13,
  DS_TFT_BLDR_PARSING_ERROR       = 14,
  DS_TFT_BLDR_UNRECOGNIZED_ELEM   = 15,
  DS_TFT_BLDR_COULDNT_GET_PDN     = 16,
  DS_TFT_BLDR_INVALID_IP_ADDR     = 17
} ds_tft_bldr_unpacking_enum_error_code;

typedef uint16 ds_tft_bldr_unpacking_error_code;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/



/*===========================================================================
FUNCTION   ds_tft_bldr_create_3gpp2_obj

DESCRIPTION
  Creates and returns the 3GPP2 object. This 3GPP2 object is embedded in the
  Resv message sent to the PDSN. PDSN uses this object to install, add or
  delete filters.

DEPENDENCIES
  None

RETURN VALUE
  -1 on any failure to construct the object
  0 on successful construction of 3GPP2 object

SIDE EFFECTS
  None.
===========================================================================*/

int8 ds_tft_bldr_create_3gpp2_obj(

  /* A single ds flow pointer or a list of flow pointers for whom a
     bundled create RSVP message is to be sent */
  ds707_flow_type ** ds_flow_ptr_list,


  /* length of ds707_flow_type array of pointers. i.e. number of flows */
  uint8 flow_cnt,


  /* kind of 3GPP2 obj.. whether for create or add or delete etc. */
  ds_tft_bldr_tft_opcode_type tft_opcode,


  /* transaction ID for this create TFT RSVP message */
  uint32 transaction_id,


  /* Information element types (Refer B.1.1.1 of IS-835D.4) */
  ds_tft_bldr_tft_ie_type ie_type,


  /* ps_iface pointer. This is used to get TX filters */
  ps_iface_type * ps_iface_ptr,


  /* buffer space in which to pack the 3GPP2 object. This is an OUTPUT param */
  uint8 * dqos_3gpp2_obj,


  /* length of packed dqos_3gpp2_obj. This is an OUTPUT param */
  uint16 * len_of_3gpp2_obj

  );




/*===========================================================================
FUNCTION  ds_tft_bldr_get_ts_id

DESCRIPTION
  Returns the transaction id.

DEPENDENCIES
  None

RETURN VALUE
  transaction id

SIDE EFFECTS
  None.
===========================================================================*/

uint32 ds_tft_bldr_get_ts_id(

  uint8 * dqos_3gpp2_obj

  );

#ifdef FEATURE_EHRPD
#ifdef FEATURE_NIQ_EHRPD

/*===========================================================================
FUNCTION      UNPACK_3GPP2_OBJ

DESCRIPTION   Simulates a network initiated QOS 

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
ds707_nw_init_3gpp2_obj * ds_tft_bldr_unpack_3gpp2_obj
(
  uint8                       * dqos_3gpp2_obj,
  ds_tft_bldr_tft_error_code  * error_ptr,
  ps_iface_type               * iface_ptr
);

/*===========================================================================
FUNCTION      free_test_memory

DESCRIPTION   

DEPENDENCIES  

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_tft_bldr_free_3gpp2_memory
(
  ds707_nw_init_3gpp2_obj * obj_ptr
);

/*===========================================================================
FUNCTION      ds_tft_bldr_pack_resv_err

DESCRIPTION   

DEPENDENCIES  

RETURN VALUE   

SIDE EFFECTS  None.
===========================================================================*/
int8 ds_tft_bldr_pack_resv_err
(
  uint32                        transaction_id,
  ds_tft_bldr_tft_error_code    error_code,
  ps_iface_type               * ps_iface_ptr,
  ds_tft_bldr_tft_ie_type       ie_type,
  uint8                       * dqos_3gpp2_obj,
  uint16                      * len_of_3gpp2_obj
);

#endif /*FEATURE_NIQ_EHRPD*/

/*===========================================================================
FUNCTION      DS_TFT_BLDR_SAVE_CONFIRMED_PROFILE

DESCRIPTION   This is utility to parse and save the confirmed profiles

DEPENDENCIES  None

RETURN VALUE  NONE

SIDE EFFECTS  Confirmed profiles are stored.
===========================================================================*/
boolean ds_tft_bldr_save_confirmed_profile
(
  uint8 * dqos_3gpp2_obj, 
  ds707_flow_type  **ds_flow, 
  uint8 num_flows
);


#endif /* FEATURE_EHRPD */

#endif /* #ifdef FEATURE_HDR_QOS */

#endif /* DS_TFT_BLDR_H */

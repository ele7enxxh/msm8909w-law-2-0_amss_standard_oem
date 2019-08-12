#ifndef PS_EAP_SIM_AKA_PB_H
#define PS_EAP_SIM_AKA_PB_H

/*===========================================================================

        E A P  S U B S C R I B E R  I D E N T I T Y  M O D U L E
                            
        A U T H E N T I C A T I O N   K E Y   A G R E E M E N T

                       P A R S E R  B U I L D E R

                
                   
DESCRIPTION
  This file contains Parser Builder processing functions shared by both 
  EAP-SIM and EAP-AKA implementations. 
     
    
Copyright (c) 2006-2013 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_eap_sim_aka_pb.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------

===========================================================================*/



/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"

#ifdef FEATURE_DATA_PS_EAP
#include "comdef.h"
#include "dsm.h"

#include "secutil.h"

#include "event.h"
#include "ps_eap.h"
#include "ps_eap_sim_aka.h"

/*===========================================================================

                              INTERNAL DATA 

===========================================================================*/

#define EAP_SQN_LEN   6    /*48       bits 3G TS 33.105.v.3.0(2000-03)*/
#define EAP_AMF_LEN   2    /*16       bits 3G TS 33.105.v.3.0(2000-03)*/
#define EAP_MACS_LEN  8    /*64       bits 3G TS 33.105.v.3.0(2000-03)*/
#define EAP_RES_LEN   16    /*32<->128 bits 3G TS 33.105.v.3.0(2000-03)*/
#define EAP_CK_LEN    16    /*128      bits 3G TS 33.105.v.3.0(2000-03)*/
#define EAP_IK_LEN    16    /*128      bits 3G TS 33.105.v.3.0(2000-03)*/

#define EAP_CK_PRIME_LEN    16    /*128 bits 3G TS 33.105.v.3.0(2000-03)*/
#define EAP_IK_PRIME_LEN    16    /*128 bits 3G TS 33.105.v.3.0(2000-03)*/

/*---------------------------------------------------------------------------
 RAND Len - RFC 4186/4187
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_MAX_RAND_LEN           16

/*---------------------------------------------------------------------------
 MAC Len - RFC 4186/4187
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_MAX_MAC_LEN            16

/*---------------------------------------------------------------------------
 The size of the header for an AT_MAC TLV according to EAP SIM/AKA RFCs
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_MAC_HEADER_SIZE         4

/*---------------------------------------------------------------------------
 Maximum Len of msg specific data used to compute MAC - arbitrary
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_MAX_MAC_DATA_LEN       600

/*---------------------------------------------------------------------------
 IV Len - RFC 4186/4187
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_MAX_IV_LEN             16

/*---------------------------------------------------------------------------
 Nonce_S Len - RFC 4186/4187
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_MAX_NONCE_S_LEN        16

/*---------------------------------------------------------------------------
 Reauthentication ID Len - arbitrary
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_MAX_REAUTH_ID_LEN     EAP_SIM_AKA_MAX_ID_LEN

/*---------------------------------------------------------------------------
 Pseudonym Len - arbitrary
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_MAX_PSEUDONYM_LEN     EAP_SIM_AKA_MAX_ID_LEN

/*---------------------------------------------------------------------------
 Maximum Len of encr data field
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_MAX_ENCR_DATA_LEN     EAP_SIM_AKA_MAX_REAUTH_ID_LEN + \
                                          EAP_SIM_AKA_MAX_PSEUDONYM_LEN

/*---------------------------------------------------------------------------
 the maximum length of the value field of a TLV - arbitrary
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_DATA_MAX_LEN          EAP_SIM_AKA_MAX_ENCR_DATA_LEN

/*---------------------------------------------------------------------------
  the length expressed in the len field of a TLV packet is actually equal to
  the total number of bytes devided by 4 - According to RFC 4186/4187
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_MULTIPLE_LEN            4

/*---------------------------------------------------------------------------
 Mask used to extract the S bit of a notification field
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_S_BIT_PRESENT      0x8000

/*---------------------------------------------------------------------------
 Mask used to extract the P bit of a notification field
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_P_BIT_PRESENT      0x4000

/*---------------------------------------------------------------------------
 The maximum length of data - arbitrary
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_MAX_DATA  255

/*---------------------------------------------------------------------------
 According to EAP SIM/AKA RFCs the intermediate key MK has a length of 20
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_MAX_MK         20

/*---------------------------------------------------------------------------
   According to EAP AKA PRIME RFCs the intermediate key MK has a 
   length of 20 
---------------------------------------------------------------------------*/
#define EAP_AKA_PRIME_MAX_MK      208

/*---------------------------------------------------------------------------
 The maximum length of the authentication key - RFC
---------------------------------------------------------------------------*/
#define EAP_AKA_PRIME_MAX_K_AUT   32

/*---------------------------------------------------------------------------
 The maximum length of the authentication key - RFC
---------------------------------------------------------------------------*/
#define EAP_AKA_PRIME_MAX_K_RE    32

/*---------------------------------------------------------------------------
 The maximum length of the encryption key - RFC 4186/4187
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_MAX_K_ENCR     16

/*---------------------------------------------------------------------------
 The maximum length of the authentication key - RFC 4186/4187
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_MAX_K_AUT      16

/*---------------------------------------------------------------------------
 The maximum length of the master session key - RFC 4186/4187
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_MAX_MSK        64 

/*---------------------------------------------------------------------------
 The maximum length of the computed key values
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_MAX_KEY_LEN   EAP_SIM_AKA_MAX_MK + \
                                  EAP_SIM_AKA_MAX_K_ENCR + \
                                  EAP_SIM_AKA_MAX_K_AUT  + \
                                  EAP_SIM_AKA_MAX_MSK    + \
                                  EAP_SIM_AKA_MAX_EMSK

/*---------------------------------------------------------------------------
 The maximum length of the computed key values for AKA PRIME
---------------------------------------------------------------------------*/
#define EAP_AKA_PRIME_MAX_KEY_LEN       EAP_AKA_PRIME_MAX_MK 
/*---------------------------------------------------------------------------
 The maximum length of the stored key values
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_MAX_ST_KEY_LEN   EAP_SIM_AKA_MAX_MK + \
                                     EAP_SIM_AKA_MAX_K_ENCR + \
                                     EAP_SIM_AKA_MAX_K_AUT 
                                  
/*---------------------------------------------------------------------------
 The maximum length of the stored key values for AKA_PRIME
---------------------------------------------------------------------------*/
#define EAP_AKA_PRIME_MAX_ST_KEY_LEN   EAP_AKA_PRIME_MAX_MK    + \
                                       EAP_SIM_AKA_MAX_K_ENCR  + \
                                       EAP_AKA_PRIME_MAX_K_AUT + \
                                       EAP_AKA_PRIME_MAX_K_RE


/*---------------------------------------------------------------------------
 The maximum length of the computed result
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_MAX_SRES       64 

/*---------------------------------------------------------------------------
 The maximum length of the KC ( encryption key )
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_MAX_KC         64

/*---------------------------------------------------------------------------
 The maximum length of the KC ( encryption key )
---------------------------------------------------------------------------*/
#define EAP_AKA_PRIME_MAX_KC       32

/*---------------------------------------------------------------------------
 EAP SIM Packet Type - RFC 4186
---------------------------------------------------------------------------*/
#define EAP_SIM_TYPE               18

/*---------------------------------------------------------------------------
 EAP AKA Packet Type - RFC 4187
---------------------------------------------------------------------------*/
#define EAP_AKA_TYPE               23

/*---------------------------------------------------------------------------
 EAP AKA_PRIME Packet Type - RFC 
---------------------------------------------------------------------------*/
#define EAP_AKA_PRIME_TYPE         50

/*---------------------------------------------------------------------------
 According to RFC 4186
---------------------------------------------------------------------------*/
#define EAP_SIM_MAX_RAND_NO         3

/*---------------------------------------------------------------------------
 According to RFC 4186
---------------------------------------------------------------------------*/
#define EAP_SIM_MIN_RAND_NO         2

/*---------------------------------------------------------------------------
 Maximum length of checkcode - According to RFC 4187
---------------------------------------------------------------------------*/
#define EAP_AKA_MAX_CHECKCODE      20

/*---------------------------------------------------------------------------
 According to RFC kdf-10
---------------------------------------------------------------------------*/
#define EAP_AKA_MAX_BIDDING         4

/*---------------------------------------------------------------------------
 Maximum length of checkcode - According to RFC
---------------------------------------------------------------------------*/
#define EAP_AKA_PRIME_MAX_CHECKCODE    32

/*---------------------------------------------------------------------------
   size of EAP-AKA' re-auth
---------------------------------------------------------------------------*/
#define EAP_AKA_PR_REAUTH_STR_LEN     16

/*---------------------------------------------------------------------------
  The maximum length of EAP-AKA PRIME's sdata
---------------------------------------------------------------------------*/
#define EAP_AKA_PRIME_REAUTH_SDATA_MAX_LEN EAP_SIM_AKA_IME_MAX_ID_LEN + \
                                    EAP_AKA_PR_REAUTH_STR_LEN  + \
                                    EAP_SIM_AKA_MAX_NONCE_S_LEN + \
                                    sizeof(uint16)


/*===========================================================================

                            DATA TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  Parser/Builder syncrhonous functions result enumeration
---------------------------------------------------------------------------*/
typedef enum
{
  EAP_SIM_AKA_FAILURE = 0,
  EAP_AKA_PRIME_FAILURE = EAP_SIM_AKA_FAILURE,
  EAP_SIM_AKA_SUCCESS   = 1, 
  EAP_AKA_PRIME_SUCCESS = EAP_SIM_AKA_SUCCESS 
} eap_sim_aka_result_enum_type;

/*---------------------------------------------------------------------------
  Enumeration of possible result returned by the message helper functions
---------------------------------------------------------------------------*/
typedef enum
{
   EAP_SIM_AKA_MSGH_ERROR             = 0,
   EAP_AKA_PRIME_MSGH_ERROR             = EAP_SIM_AKA_MSGH_ERROR,
   EAP_SIM_AKA_MSGH_SUCCESS_DROP_PKT  = 1,
   EAP_AKA_PRIME_MSGH_SUCCESS_DROP_PKT  = EAP_SIM_AKA_MSGH_SUCCESS_DROP_PKT,
   EAP_SIM_AKA_MSGH_SUCCESS_KEEP_PKT    = 2,
   EAP_AKA_PRIME_MSGH_SUCCESS_KEEP_PKT  = EAP_SIM_AKA_MSGH_SUCCESS_KEEP_PKT
} eap_sim_aka_msgh_res_type;

/*---------------------------------------------------------------------------
  Enumeration of possible result returned by the attribute helper functions
---------------------------------------------------------------------------*/
typedef enum
{
  EAP_SIM_AKA_ATTRH_ERROR      = 0,
  EAP_AKA_PRIME_ATTRH_ERROR      = EAP_SIM_AKA_ATTRH_ERROR,
  EAP_SIM_AKA_ATTRH_SUCCESS    = 1,
  EAP_AKA_PRIME_ATTRH_SUCCESS    = EAP_SIM_AKA_ATTRH_SUCCESS,
  EAP_SIM_AKA_ATTRH_NOTPRESENT   = 2,
  EAP_AKA_PRIME_ATTRH_NOTPRESENT = EAP_SIM_AKA_ATTRH_NOTPRESENT
} eap_sim_aka_attrh_res_type;

/*===========================================================================
                   Shared EAP-SIM and EAP-AKA Attributes
===========================================================================*/
/*---------------------------------------------------------------------------
  Enumeration of possible attributes
---------------------------------------------------------------------------*/
typedef enum
{
  AT_NONSKIPPABLE_RANGE_LOWER = 0,
  AT_RAND                     = 1,
  AT_AUTN                     = 2,
  AT_RES                      = 3,
  AT_AUTS                     = 4,
  AT_PADDING                  = 6,
  AT_NONCE_MT                 = 7,
  AT_PERMANENT_ID_REQ         = 10,
  AT_MAC                      = 11,
  AT_NOTIFICATION             = 12,
  AT_ANY_ID_REQ               = 13,
  AT_IDENTITY                 = 14,
  AT_VERSION_LIST             = 15,
  AT_SELECTED_VERSION         = 16,
  AT_FULLAUTH_ID_REQ          = 17,
  AT_COUNTER                  = 19,
  AT_COUNTER_TOO_SMALL        = 20,
  AT_NONCE_S                  = 21,
  AT_CLIENT_ERROR_CODE        = 22,
  AT_KDF_INPUT                = 23,
  AT_KDF                      = 24,
  AT_IV                       = 129,
  AT_ENCR_DATA                = 130,
  AT_NEXT_PSEUDONYM           = 132,
  AT_NEXT_REAUTH_ID           = 133,
  AT_CHECKCODE                = 134,
  AT_RESULT_IND               = 135,
  AT_BIDDING                  = 136,
  AT_SKIPPABLE_RANGE_LOWER    = 128,
  AT_SKIPPABLE_RANGE_UPPER    = 255
}eap_sim_aka_at_type;

/*---------------------------------------------------------------------------
  Enumeration of possible messages
---------------------------------------------------------------------------*/
typedef enum
{
   SIM_AKA_INVALID_MSG  = 0,
   AKA_CHALLENGE        = 1,
   AKA_PRIME_CHALLENGE    = AKA_CHALLENGE,
   AKA_AUTH_REJECT      = 2,
   AKA_PRIME_AUTH_REJECT  = AKA_AUTH_REJECT,
   AKA_SYNC_FAILURE     = 4,
   AKA_PRIME_SYNC_FAILURE = AKA_SYNC_FAILURE,
   AKA_IDENTITY         = 5,
   AKA_PRIME_IDENTITY     = AKA_IDENTITY,
   SIM_START            = 10,
   SIM_CHALLENGE        = 11,
   SIM_AKA_NOTIFICATION = 12,
   AKA_PRIME_NOTIFICATION = SIM_AKA_NOTIFICATION,
   SIM_AKA_REAUTH       = 13,
   AKA_PRIME_REAUTH       = SIM_AKA_REAUTH,
   SIM_AKA_CLIENT_ERROR   = 14,
   AKA_PRIME_CLIENT_ERROR = SIM_AKA_CLIENT_ERROR
}eap_sim_aka_msg_type;

/*---------------------------------------------------------------------------
  AT_ANY_ID_REQ, AT_PERMANENT_ID_REQ, AT_FULLAUTH_ID_REQ
---------------------------------------------------------------------------*/
typedef enum
{
   EAP_SIM_AKA_ID_REQ_ANY     = 0,
   EAP_AKA_PRIME_ID_REQ_ANY   = EAP_SIM_AKA_ID_REQ_ANY,
   EAP_SIM_AKA_ID_REQ_FULL    = 1,
   EAP_AKA_PRIME_ID_REQ_FULL  = EAP_SIM_AKA_ID_REQ_FULL,
   EAP_SIM_AKA_ID_REQ_PERM    = 2,
   EAP_AKA_PRIME_ID_REQ_PERM  = EAP_SIM_AKA_ID_REQ_PERM
}eap_sim_aka_identity_enum_type;

typedef struct 
{
  boolean                        present;
  eap_sim_aka_identity_enum_type id_req;
} eap_sim_aka_identity_req_type;


/*---------------------------------------------------------------------------
  AT_RAND intermediate structure
---------------------------------------------------------------------------*/
typedef struct 
{
  uint8          rand_len;
  uint8          eap_rand[EAP_SIM_AKA_MAX_RAND_LEN];
}eap_sim_aka_rand_type;

/*---------------------------------------------------------------------------
  AT_MAC
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean        present;
  uint8          mac_len;
  uint8          mac[EAP_SIM_AKA_MAX_MAC_LEN];
  uint16         offset;
  dsm_item_type* data_to_compute_mac;
  dsm_item_type* mac_header;
  uint8          k_aut[EAP_AKA_PRIME_MAX_K_AUT];
} eap_sim_aka_mac_type;

/*---------------------------------------------------------------------------
  AT_IV
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean        present;
  uint8          iv_len;
  uint8          iv[EAP_SIM_AKA_MAX_IV_LEN];
} eap_sim_aka_iv_type;

/*---------------------------------------------------------------------------
  AT_ENCR_DATA
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean        present;
  uint16         encr_data_len;
  uint8          encr_data[EAP_SIM_AKA_MAX_ENCR_DATA_LEN];
} eap_sim_aka_encr_type;

/*---------------------------------------------------------------------------
  AT_RESULT_IND
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean        present;
} eap_sim_aka_result_ind_type;

/*---------------------------------------------------------------------------
  AT_NEXT_PSEUDONYM
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean        present;
  uint16         pseudonym_len;
  uint8          pseudonym[EAP_SIM_AKA_MAX_PSEUDONYM_LEN];
} eap_sim_aka_pseudonym_type;

/*---------------------------------------------------------------------------
  AT_NEXT_REAUTH_ID
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean        present;
  uint16         next_reauth_id_len;
  uint8          next_reauth_id [EAP_SIM_AKA_MAX_REAUTH_ID_LEN];
} eap_sim_aka_next_reauth_id_type;

/*---------------------------------------------------------------------------
  AT_NONCE_S
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean        present;
  uint8          nonce_s_len;
  uint8          nonce_s[EAP_SIM_AKA_MAX_NONCE_S_LEN];
} eap_sim_aka_nonce_s_type;

/*---------------------------------------------------------------------------
  AT_NOTIFICATION
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean        present;
  uint16         notification;
  boolean        p_bit_present;
  boolean        s_bit_present;
} eap_sim_aka_notification_type;

/*---------------------------------------------------------------------------
  AT_COUNTER
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean        present;
  uint16         counter;
} eap_sim_aka_counter_type;

/*---------------------------------------------------------------------------
  AT_IDENTITY
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean              present;
  uint8                id_len;
  uint8                identity[EAP_SIM_AKA_MAX_ID_LEN];
} eap_sim_aka_identity_resp_type;

/*---------------------------------------------------------------------------
  AT_COUNTER_TOO_SMALL
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean        present;
} eap_sim_aka_cnt_small_type;

/*---------------------------------------------------------------------------
  AT_CLIENT_ERROR_CODE
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
 Enumeration of the possible errors according to the EAP-SIM RFC
---------------------------------------------------------------------------*/
typedef enum
{
   EAP_CLIENT_ERR_UNABLE_TO_PROCESS   = 0,
   EAP_CLIENT_ERR_UNSUPORTED_VER      = 1,
   EAP_CLIENT_ERR_CHALNO_TOO_SMALL    = 2,
   EAP_CLIENT_ERR_RAND_NOT_FRESH      = 3
}eap_sim_aka_clienterr_enum_type;

typedef struct 
{
  boolean                           present;
  uint16                            error_code;
} eap_sim_aka_clienterr_type;

/*-------------------------------------------------------------------------
 AT_PADDING
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean                           present;
  uint8                             length;
} eap_sim_aka_padding_type;

/*---------------------------------------------------------------------------
  AT_CHECKCODE
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean        present;
  uint16         checkcode_len;
  uint8          checkcode[EAP_AKA_PRIME_MAX_CHECKCODE];
} eap_sim_aka_checkcode_type;

/*---------------------------------------------------------------------------
  TLV structure
---------------------------------------------------------------------------*/
typedef struct
{
  uint16   len;
  uint8    data[EAP_SIM_AKA_DATA_MAX_LEN];
  uint16   offset;
} eap_sim_aka_len_val_type;

typedef struct
{
  eap_sim_aka_at_type        type;
  eap_sim_aka_len_val_type   len_val;
} eap_sim_aka_type_len_val_type;

/*---------------------------------------------------------------------------
  Data structure for SIM-Notification response packet
---------------------------------------------------------------------------*/
typedef struct 
{
  eap_sim_aka_mac_type         mac;
  eap_sim_aka_iv_type          iv;
  eap_sim_aka_encr_type        encr_data;
}eap_sim_aka_resp_notification_type;

/*---------------------------------------------------------------------------
  Data structure for SIM-Client Error response packet
---------------------------------------------------------------------------*/
typedef struct 
{
  eap_sim_aka_clienterr_type    cl_error;
}eap_sim_aka_resp_clienterr_type;

/*---------------------------------------------------------------------------
  Data structure for SIM-Re-Authentication response packet
---------------------------------------------------------------------------*/
typedef struct 
{
  eap_sim_aka_mac_type         mac;
  eap_sim_aka_result_ind_type  result_ind;
  eap_sim_aka_iv_type          iv;
  eap_sim_aka_encr_type        encr_data;
  eap_sim_aka_checkcode_type   checkcode;
}eap_sim_aka_resp_reauth_type;

/*---------------------------------------------------------------------------
  The encrypted data field can also contain other attributes as follows
  The encrypted field fast-re-auth
---------------------------------------------------------------------------*/
typedef struct 
{
  eap_sim_aka_counter_type     counter;
  eap_sim_aka_cnt_small_type   counter_small;   
}eap_sim_aka_resp_reauth_encr_type;

/*---------------------------------------------------------------------------
  Data structure for SIM-Notification request packet
---------------------------------------------------------------------------*/
typedef struct 
{
   eap_sim_aka_notification_type  notification;
   eap_sim_aka_mac_type           mac;
   eap_sim_aka_iv_type            iv;
   eap_sim_aka_encr_type          encr;
} eap_sim_aka_req_notification_type;

/*---------------------------------------------------------------------------
  Data structure for SIM-Re-Authenticatio request packet
---------------------------------------------------------------------------*/
typedef struct 
{
   eap_sim_aka_mac_type          mac;
   eap_sim_aka_result_ind_type   result_ind;
   eap_sim_aka_iv_type           iv;
   eap_sim_aka_encr_type         encr_data;
   eap_sim_aka_checkcode_type    checkcode;
} eap_sim_aka_req_reauth_type;

/*---------------------------------------------------------------------------
   The encrypted data field can also contain other attributes as follows:
   For the encrypted field of a challenge request
---------------------------------------------------------------------------*/
typedef struct 
{
   eap_sim_aka_pseudonym_type       pseudonym;
   eap_sim_aka_next_reauth_id_type  next_reauth_id;
   eap_sim_aka_padding_type         padding;
} eap_sim_aka_req_challenge_encr_type;

/*---------------------------------------------------------------------------
   For the encrypted field of a re-authentication request
---------------------------------------------------------------------------*/
typedef struct 
{
   eap_sim_aka_counter_type         counter;
   eap_sim_aka_nonce_s_type         nonce_s;
   eap_sim_aka_next_reauth_id_type  next_reauth_id;
   eap_sim_aka_padding_type         padding;
} eap_sim_aka_req_reauth_encr_type;

/*---------------------------------------------------------------------------
   For the encrypted field of a notification request for
   fast-reauthentication replay protection
---------------------------------------------------------------------------*/
typedef struct 
{
   eap_sim_aka_counter_type         counter;
   eap_sim_aka_padding_type         padding;
} eap_sim_aka_req_notification_encr_type;

/*===========================================================================

                     CALLBACK FUNCTION PROTOTYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  FWD Declaration 
---------------------------------------------------------------------------*/
struct eap_sim_aka_pb_ctx_S;

/*---------------------------------------------------------------------------
  Input message helper
---------------------------------------------------------------------------*/
typedef eap_sim_aka_msgh_res_type (*input_msgh_hdlr_type)
(
  struct eap_sim_aka_pb_ctx_S* context, 
  void*                        request_msg
);

/*---------------------------------------------------------------------------
  Output message helper
---------------------------------------------------------------------------*/
typedef eap_sim_aka_msgh_res_type (*output_msgh_hdlr_type)
(
  struct eap_sim_aka_pb_ctx_S*   context, 
  void*                        resp_msg
);

/*---------------------------------------------------------------------------
  Input attribute helper
---------------------------------------------------------------------------*/
typedef eap_sim_aka_attrh_res_type (*input_attrh_hdlr_type)
(
  void*                      attribute,
  eap_sim_aka_len_val_type*  len_val
);

/*--------------------------------------------------------------------------
  Output attribute helper
---------------------------------------------------------------------------*/
typedef eap_sim_aka_attrh_res_type (*output_attrh_hdlr_type)
(
  void*                           attribute,
  eap_sim_aka_type_len_val_type*  type_len_val,
  struct eap_sim_aka_pb_ctx_S*    pb_ctx
);

/*---------------------------------------------------------------------------
  Type:input handler function pair
---------------------------------------------------------------------------*/
typedef struct 
{
  eap_sim_aka_msg_type   type;
  input_msgh_hdlr_type   msgh_hdlr;
}eap_sim_aka_in_msgh_pair_type;

/*---------------------------------------------------------------------------
  Type:output handler function pair
---------------------------------------------------------------------------*/
typedef struct 
{
  eap_sim_aka_msg_type    type;
  output_msgh_hdlr_type   msgh_hdlr;
}eap_sim_aka_out_msgh_pair_type;

/*---------------------------------------------------------------------------
  Attribute:input handler function pair
---------------------------------------------------------------------------*/
typedef struct 
{
  eap_sim_aka_at_type     type;
  input_attrh_hdlr_type   attrh_hdlr;
}eap_sim_aka_in_attrh_pair_type;

/*---------------------------------------------------------------------------
  Attribute:output handler function pair
---------------------------------------------------------------------------*/
typedef struct 
{
  eap_sim_aka_at_type      type;
  output_attrh_hdlr_type   attrh_hdlr;
}eap_sim_aka_out_attrh_pair_type;

/*---------------------------------------------------------------------------
  Parser Internal State
---------------------------------------------------------------------------*/
typedef enum
{
  EAP_SIM_AKA_PARSER_ST_OPENED = 0,
  EAP_SIM_AKA_PARSER_ST_CLOSED = 1
}eap_sim_aka_parser_state_enum_type;

/*---------------------------------------------------------------------------
  Attribute type check possible results
---------------------------------------------------------------------------*/
typedef enum
{
  EAP_SIM_AKA_TYPE_ERROR       = -1,
  EAP_SIM_AKA_TYPE_UNSUPPORTED = 0,
  EAP_SIM_AKA_TYPE_SUPPORTED               = 1,
  EAP_SIM_AKA_TYPE_SKIPPABLE_UNSUPPORTED   = 2
}eap_sim_aka_pb_type_check_res;

/*---------------------------------------------------------------------------
  Result type for parsing input msgs
---------------------------------------------------------------------------*/
typedef enum
{
  EAP_SIM_AKA_PB_ERROR        = -1,
  EAP_SIM_AKA_PB_CONTINUE     =  0,
  EAP_SIM_AKA_PB_NO_MORE_ATTR =  1
}eap_sim_aka_pb_res;

/*---------------------------------------------------------------------------
  Parser/Builder context
---------------------------------------------------------------------------*/
typedef struct eap_sim_aka_pb_ctx_S
{
  dsm_item_type*                       next_tx_packet;
  dsm_item_type*                       next_rx_packet;
  eap_sim_aka_parser_state_enum_type   state;
  eap_sim_aka_in_msgh_pair_type*       in_msg_helper;
  uint8                                in_msg_count;
  eap_sim_aka_in_attrh_pair_type*      in_attr_helper;
  uint8                                in_attr_count;
  eap_sim_aka_out_msgh_pair_type*      out_msg_helper;
  uint8                                out_msg_count;
  eap_sim_aka_out_attrh_pair_type*     out_attr_helper;
  uint8                                out_attr_count;
  uint16                               offset;
  eap_sim_aka_clienterr_enum_type      client_error_type;
}eap_sim_aka_pb_ctx;


/*===========================================================================

                     EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION EAP_SIM_AKA_PB_GET_NEXT_TLV

DESCRIPTION
  Retrieves the next TLV from a packet. If no TLV was previously requested 
  retrieves the first one. 

DEPENDENCIES
  None

PARAMETERS
  len_val - the length and value fields of the TLV
  pb_ctx - the current context
  
RETURN VALUE
  returns the type of the TLV 

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_pb_res      
eap_sim_aka_pb_get_next_tlv
( 
  eap_sim_aka_len_val_type*   len_val, 
  eap_sim_aka_pb_ctx*         pb_ctx,
  eap_sim_aka_at_type*        type
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_PB_PUT_NEXT_TLV

DESCRIPTION
  Pushes a new TLV in the outgoing packet.

DEPENDENCIES
  None

PARAMETERS
  type_len_val - the type/length and value fields of the TLV
  pb_ctx - the current context
  
RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void 
eap_sim_aka_pb_put_next_tlv
(
  eap_sim_aka_type_len_val_type*    type_len_val,  
  eap_sim_aka_pb_ctx*   pb_ctx
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_PB_PROCESS_PKT

DESCRIPTION
  Parses the incoming EAP packets and converts them to a structure easy to 
  understand by the SIM or AKA Core
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx - the current context
  input_msg - the incoming message
  req - the expected req structure
  
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_result_enum_type   
eap_sim_aka_pb_process_pkt  
( 
 eap_sim_aka_pb_ctx*     pb_ctx,  
 dsm_item_type**         input_msg,  
 void*                   req
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_PB_BUILD_PKT

DESCRIPTION
  Builds an EAP message based on an input structure provided by the SIM or 
  AKA Core
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx - the current context
  resp - the resp message structure
  
RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_result_enum_type
eap_sim_aka_pb_build_pkt
( 
  eap_sim_aka_pb_ctx*     pb_ctx, 
  void*                   resp,
  uint8                   sub_type
);  

/*===========================================================================
FUNCTION EAP_SIM_AKA_PB_GET_LAST_RX_PKT

DESCRIPTION
  Retrieves to the caller the cached received packet. It is needed by the 
  SIM/AKA Core for MAC validation.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx - the current context
  msg - the output value
  
RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void 
eap_sim_aka_pb_get_last_rx_pkt
(  
  eap_sim_aka_pb_ctx*  pb_ctx,
  dsm_item_type**      msg
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_PB_DROP_LAST_RX_PKT

DESCRIPTION
  The SIM-AKA Parser will keep cached the received packet until it is being 
  explicitly told to realease it through a call of this function.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx - the current context
    
RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void 
eap_sim_aka_pb_drop_last_rx_pkt 
(
  eap_sim_aka_pb_ctx*  pb_ctx
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_PB_GET_LAST_TX_PKT

DESCRIPTION
  Retrieves to the caller the cached transmitted packet. It is needed by the 
  SIM/AKA Core for MAC computation.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx - the current context
  msg - the output value
  
RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void 
eap_sim_aka_pb_get_last_tx_pkt
(  
  eap_sim_aka_pb_ctx*  pb_ctx,
  dsm_item_type**      msg
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_PB_DROP_LAST_TX_PKT

DESCRIPTION
  The SIM/AKA Parser will keep cached the transmitted packet until it is being 
  explicitly told realease it through a call of this function
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx - the current context
    
RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void 
eap_sim_aka_pb_drop_last_tx_pkt 
(
  eap_sim_aka_pb_ctx*  pb_ctx
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_PB_PROCESS_ATTRIBUTE

DESCRIPTION
  Function for processing call to the output attribute helper. Called by the
  output message helper.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx - the current context
  data_to_send - data to send to the attr helper
  type - the type of the output attribute
    
RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_sim_aka_pb_process_attribute
(
  eap_sim_aka_pb_ctx*  pb_ctx, 
  void*                data_to_send,
  eap_sim_aka_at_type  type
);

/*===========================================================================
                 Common SIM-AKA Message Input Helpers
===========================================================================*/

/*===========================================================================
FUNCTION EAP_SIM_AKA_IN_MSGH_NOTIFICATION

DESCRIPTION
  Helper function for parsing AKA-Notification input message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  req_notification:- pointer to a eap_sim_aka_req_notification_type structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_sim_aka_in_msgh_notification
(
  eap_sim_aka_pb_ctx                 *pb_ctx, 
  eap_sim_aka_req_notification_type  *req_notification
);


/*===========================================================================
FUNCTION EAP_SIM_AKA_IN_MSGH_REAUTH

DESCRIPTION
  Helper function for parsing AKA-Reauth input message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  req_notification:- pointer to a eap_sim_aka_req_reauth_type structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_sim_aka_in_msgh_reauth
(
  eap_sim_aka_pb_ctx           *pb_ctx, 
  eap_sim_aka_req_reauth_type  *req_reauth
);


/*===========================================================================
                 Common SIM_AKA Message Output Helpers
===========================================================================*/

/*===========================================================================
FUNCTION EAP_SIM_AKA_OUT_MSGH_NOTIFICATION

DESCRIPTION
  Helper function for parsing AKA-Notification output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  resp_notif:- pointer to a eap_sim_aka_resp_notification_type structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_sim_aka_out_msgh_notification
(
  eap_sim_aka_pb_ctx                  *pb_ctx, 
  eap_sim_aka_resp_notification_type  *resp_notif
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_OUT_MSGH_REAUTH

DESCRIPTION
  Helper function for parsing AKA-REAUTH output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  resp_reauth:- pointer to a eap_sim_aka_resp_reauth_type structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_sim_aka_out_msgh_reauth
(
  eap_sim_aka_pb_ctx              *pb_ctx, 
  eap_sim_aka_resp_reauth_type    *resp_reauth
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_OUT_MSGH_CLIENT_ERROR

DESCRIPTION
  Helper function for parsing AKA-Client Error output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  resp_cl_err:- pointer to a eap_sim_aka_resp_client_error structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_sim_aka_out_msgh_client_error
(
  eap_sim_aka_pb_ctx                  *pb_ctx, 
  eap_sim_aka_resp_clienterr_type     *resp_cl_error
);

/*===========================================================================
                 Common SIM-AKA Attribute Input Helpers
===========================================================================*/

/*===========================================================================
FUNCTION EAP_SIM_IN_HLP_AT_ANY_ID_REQ	

DESCRIPTION
  Helper function for parsing AT_ANY_ID_REQ	input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:   - the length and value extracted from the EAP SIM packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_aka_in_hlp_at_any_id_req
(
  void*                        attribute,
  eap_sim_aka_len_val_type*    len_val
);

/*===========================================================================
FUNCTION EAP_SIM_IN_HLP_AT_FULLAUTH_ID_REQ	

DESCRIPTION
  Helper function for parsing AT_FULLAUTH_ID_REQ	input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:   - the length and value extracted from the EAP SIM packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_aka_in_hlp_at_full_id
(
  void*                       attribute,
  eap_sim_aka_len_val_type*   len_val
);
	
/*===========================================================================
FUNCTION EAP_SIM_IN_HLP_AT_PERMANENT_ID_REQ

DESCRIPTION
  Helper function for parsing AT_PERMANENT_ID_REQ	input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:   - the length and value extracted from the EAP SIM packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_aka_in_hlp_at_perm_id
(
  void*                       attribute,
  eap_sim_aka_len_val_type*   len_val
);

/*===========================================================================
FUNCTION EAP_SIM_IN_HLP_AT_MAC 

DESCRIPTION
  Helper function for parsing AT_MAC input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:   - the length and value extracted from the EAP SIM packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_aka_in_hlp_at_mac
(
  void*                      attribute,
  eap_sim_aka_len_val_type*  len_val
);

/*===========================================================================
FUNCTION EAP_SIM_IN_HLP_AT_RESULT_IND	

DESCRIPTION
  Helper function for parsing AT_RESULT_IND	input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:   - the length and value extracted from the EAP SIM packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_aka_in_hlp_at_result_ind
(
  void*                      attribute,
  eap_sim_aka_len_val_type*  len_val
);

/*===========================================================================
FUNCTION EAP_SIM_IN_HLP_AT_IV	

DESCRIPTION
  Helper function for parsing AT_IV	input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:   - the length and value extracted from the EAP SIM packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_aka_in_hlp_sim_at_iv
(
  void*                      attribute,
  eap_sim_aka_len_val_type*  len_val
);

/*===========================================================================
FUNCTION EAP_SIM_IN_HLP_AT_ENCR_DATA 

DESCRIPTION
  Helper function for parsing AT_ENCR_DATA input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:   - the length and value extracted from the EAP SIM packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_aka_in_hlp_at_encrdata
(
  void*                       attribute,
  eap_sim_aka_len_val_type*   len_val
);

/*===========================================================================
FUNCTION EAP_SIM_IN_HLP_AT_PADDING 

DESCRIPTION
  Helper function for parsing AT_PADDING input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:   - the length and value extracted from the EAP SIM packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_aka_in_hlp_at_padding
(
  void*                       attribute,
  eap_sim_aka_len_val_type*   len_val
);
	
/*===========================================================================
FUNCTION EAP_SIM_IN_HLP_AT_NEXT_REAUTH_ID

DESCRIPTION
  Helper function for parsing AT_NEXT_REAUTH_ID input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:   - the length and value extracted from the EAP SIM packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_aka_in_hlp_at_reauth_id
(
  void*                       attribute,
  eap_sim_aka_len_val_type*   len_val
);
	
/*===========================================================================
FUNCTION EAP_SIM_IN_HLP_AT_NEXT_PSEUDONYM

DESCRIPTION
  Helper function for parsing AT_NEXT_PSEUDONYM input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:   - the length and value extracted from the EAP SIM packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_aka_in_hlp_at_pseudonym
(
  void*                       attribute,
  eap_sim_aka_len_val_type*   len_val
);

/*===========================================================================
FUNCTION EAP_SIM_IN_HLP_AT_NOTIFICATION	

DESCRIPTION
  Helper function for parsing AT_NOTIFICATION	input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:   - the length and value extracted from the EAP SIM packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_aka_in_hlp_at_notification
(
  void*                       attribute,
  eap_sim_aka_len_val_type*   len_val
);
 
/*===========================================================================
FUNCTION EAP_SIM_IN_HLP_AT_COUNTER 

DESCRIPTION
  Helper function for parsing AT_COUNTER input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:   - the length and value extracted from the EAP SIM packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_aka_in_hlp_at_counter
(
  void*                       attribute,
  eap_sim_aka_len_val_type*   len_val
);

/*===========================================================================
FUNCTION EAP_SIM_IN_HLP_AT_NONCE_S	

DESCRIPTION
  Helper function for parsing AT_NONCE_S input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:   - the length and value extracted from the EAP SIM packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_aka_in_hlp_at_nonce_s
(
  void*                      attribute,
  eap_sim_aka_len_val_type*  len_val
);

/*===========================================================================
               Common SIM-AKA Attribute Output Helpers
===========================================================================*/

/*===========================================================================
FUNCTION EAP_SIM_AKA_OUT_HLP_AT_IDENTITY 

DESCRIPTION
  Helper function for building AT_IDENTITY output attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute:   - pointer to the attr type structure
  type_len_val:- the type, length and value of the attr needed to be added to 
                 the outgoing EAP SIM packet
  pb_ctx:      - the PB context
  
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_aka_out_hlp_at_identity
(
  void*                            attribute,
  eap_sim_aka_type_len_val_type*               type_len_val,
  eap_sim_aka_pb_ctx*  pb_ctx
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_OUT_HLP_AT_MAC 

DESCRIPTION
  Helper function for building AT_MAC output attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute:   - pointer to the attr type structure
  type_len_val:- the type, length and value of the attr needed to be added to 
                 the outgoing EAP SIM packet
  pb_ctx:      - the PB context
  
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_aka_out_hlp_at_mac
(
  void*                            attribute,
  eap_sim_aka_type_len_val_type*               type_len_val,
  eap_sim_aka_pb_ctx*  pb_ctx
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_OUT_HLP_AT_RESULT_IND 

DESCRIPTION
  Helper function for building AT_RESULT_IND output attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute:   - pointer to the attr type structure
  type_len_val:- the type, length and value of the attr needed to be added to 
                 the outgoing EAP SIM packet
  pb_ctx:      - the PB context
  
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_aka_out_hlp_at_result_ind
(
  void*                            attribute,
  eap_sim_aka_type_len_val_type*               type_len_val,
  eap_sim_aka_pb_ctx*  pb_ctx
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_OUT_HLP_AT_IV 

DESCRIPTION
  Helper function for building AT_IV output attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute:   - pointer to the attr type structure
  type_len_val:- the type, length and value of the attr needed to be added to 
                 the outgoing EAP SIM packet
  pb_ctx:      - the PB context
  
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_aka_out_hlp_at_iv
(
  void*                            attribute,
  eap_sim_aka_type_len_val_type*               type_len_val,
  eap_sim_aka_pb_ctx*  pb_ctx
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_OUT_HLP_AT_ENCR_DATA 

DESCRIPTION
  Helper function for building AT_ENCR_DATA output attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute:   - pointer to the attr type structure
  type_len_val:- the type, length and value of the attr needed to be added to 
                 the outgoing EAP SIM packet
  pb_ctx:      - the PB context
  
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_aka_out_hlp_at_encrdata
(
  void*                            attribute,
  eap_sim_aka_type_len_val_type*               type_len_val,
  eap_sim_aka_pb_ctx*  pb_ctx
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_OUT_HLP_AT_COUNTER 

DESCRIPTION
  Helper function for building AT_COUNTER output attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute:   - pointer to the attr type structure
  type_len_val:- the type, length and value of the attr needed to be added to 
                 the outgoing EAP SIM packet
  pb_ctx:      - the PB context
  
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_aka_out_hlp_at_counter
(
  void*                            attribute,
  eap_sim_aka_type_len_val_type*               type_len_val,
  eap_sim_aka_pb_ctx*  pb_ctx
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_OUT_HLP_AT_COUNTER_TOO_SMALL 

DESCRIPTION
  Helper function for building AT_COUNTER_TOO_SMALL output attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute:   - pointer to the attr type structure
  type_len_val:- the type, length and value of the attr needed to be added to 
                 the outgoing EAP SIM packet
  pb_ctx:      - the PB context
  
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_aka_out_hlp_at_counter_small
(
  void*                            attribute,
  eap_sim_aka_type_len_val_type*               type_len_val,
  eap_sim_aka_pb_ctx*  pb_ctx
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_OUT_HLP_AT_PADDING  

DESCRIPTION
  Helper function for building AT_PADDING output attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute:   - pointer to the attr type structure
  type_len_val:- the type, length and value of the attr needed to be added to 
                 the outgoing EAP SIM packet
  pb_ctx:      - the PB context
  
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_aka_out_hlp_at_padding
(
  void*                            attribute,
  eap_sim_aka_type_len_val_type*               type_len_val,
  eap_sim_aka_pb_ctx*  pb_ctx
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_OUT_HLP_AT_CLIENT_ERROR_CODE 

DESCRIPTION
  Helper function for building AT_CLIENT_ERROR_CODE output attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute:   - pointer to the attr type structure
  type_len_val:- the type, length and value of the attr needed to be added to 
                 the outgoing EAP SIM packet
  pb_ctx:      - the PB context
  
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_aka_out_hlp_at_client_error
(
  void*                            attribute,
  eap_sim_aka_type_len_val_type*               type_len_val,
  eap_sim_aka_pb_ctx*  pb_ctx
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_PB_IS_VALID_TYPE

DESCRIPTION
  Checks if the attribute type is known by this implementation.
  
DEPENDENCIES
  None

PARAMETERS
  type: - the type 
  
RETURN VALUE
  Supported, unsupported or error in req. 

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_pb_type_check_res
eap_sim_aka_pb_is_valid_type
(
  uint8 type,
  eap_sim_aka_pb_ctx* pb_ctx
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_RETRIEVE_IN_MGH

DESCRIPTION
  Retrieves the corresponding input message helper handler for a given 
  msg type.
  
DEPENDENCIES
  None

PARAMETERS
  type:   - the type 
  pb_ctx: - the PB context
  
RETURN VALUE
  The corresponding message handler or NULL if not found.

SIDE EFFECTS
  
===========================================================================*/
input_msgh_hdlr_type
eap_sim_aka_pb_retrieve_in_msgh
(
  uint8 type,
  eap_sim_aka_pb_ctx* pb_ctx
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_RETRIEVE_OUT_MGH

DESCRIPTION
  Retrieves the corresponding output message helper handler for a given 
  msg type.
  
DEPENDENCIES
  None

PARAMETERS
  type:   - the type 
  pb_ctx: - the PB context
  
RETURN VALUE
  The corresponding message handler or NULL if not found.

SIDE EFFECTS
  
===========================================================================*/
output_msgh_hdlr_type
eap_sim_aka_pb_retrieve_out_msgh
(
  uint8 type,
  eap_sim_aka_pb_ctx* pb_ctx
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_RETRIEVE_IN_ATTRH

DESCRIPTION
  Retrieves the corresponding input attribute hlp handler for a given 
  attribute type.
  
DEPENDENCIES
  None

PARAMETERS
  type:   - the type 
  pb_ctx: - the PB context
  
RETURN VALUE
  The corresponding attribute handler or NULL if not found.

SIDE EFFECTS
  
===========================================================================*/
input_attrh_hdlr_type
eap_sim_aka_pb_retrieve_in_attrh
(
  uint8 type,
  eap_sim_aka_pb_ctx* pb_ctx
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_RETRIEVE_OUT_ATTRH

DESCRIPTION
  Retrieves the corresponding output attribute hlpr handler for a given 
  attribute type.
  
DEPENDENCIES
  None

PARAMETERS
  type:   - the type 
  pb_ctx: - the PB context
  
RETURN VALUE
  The corresponding attribute handler or NULL if not found.

SIDE EFFECTS
  
===========================================================================*/
output_attrh_hdlr_type
eap_sim_aka_pb_retrieve_out_attrh
(
  uint8 type,
  eap_sim_aka_pb_ctx* pb_ctx
);



/*===========================================================================
               DIAG EVENTS FOR SIM AKA DATA DEFINITION
===========================================================================*/

/*-----------------------------------------------------------------------
  This macro prints the ID.
------------------------------------------------------------------------*/
#define EAP_SIM_AKA_MSG_SPRINTF_STR(_mask, _id, _len, _max_len) \
{ \
    _id[((_len+1) < _max_len )?(_len+1):(_max_len-1)] = '\0';\
\
    MSG_SPRINTF_1(MSG_SSID_DFLT, _mask, "ID=`%s\'", _id); \
}
        

/*---------------------------------------------------------------------------
  Macro for reporting an event
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_EVENT_REPORT( _event_payload, _event_type )             \
        do                                                                  \
        {                                                                   \
          _event_payload.event_type     = _event_type ;                     \
          event_report_payload( EVENT_EAP_SIM_AKA,                          \
                                sizeof(_event_payload),                     \
                                &_event_payload);                           \
        } while (0)

/*---------------------------------------------------------------------------
   Enumeration of EAP SIM/AKA Events
---------------------------------------------------------------------------*/
typedef enum 
{
  EAP_SIM_START_EV             = 0,
  EAP_SIM_AKA_CHALL_EV         = 1,
  EAP_AKA_PRIME_CHALL_EV       = EAP_SIM_AKA_CHALL_EV,
  EAP_SIM_AKA_REAUTH_EV        = 2,
  EAP_AKA_PRIME_REAUTH_EV      = EAP_SIM_AKA_REAUTH_EV,
  EAP_SIM_AKA_NOTIFY_EV        = 3,
  EAP_AKA_PRIME_NOTIFY_EV      = EAP_SIM_AKA_NOTIFY_EV,
  EAP_SIM_AKA_CLERR_EV         = 4,
  EAP_AKA_PRIME_CLERR_EV       = EAP_SIM_AKA_CLERR_EV,
  EAP_AKA_ID_EV                = 5,
  EAP_AKA_PRIME_ID_EV          = EAP_AKA_ID_EV,
  EAP_AKA_SYNC_FAIL_EV         = 6,
  EAP_AKA_PRIME_SYNC_FAIL_EV   = EAP_AKA_SYNC_FAIL_EV,
  EAP_AKA_AUTH_REJECT_EV       = 7,
  EAP_AKA_PRIME_AUTH_REJECT_EV = EAP_AKA_AUTH_REJECT_EV,
  EAP_SIM_AKA_SUCCESS_EV       = 8,
  EAP_AKA_PRIME_SUCCESS_EV     = EAP_SIM_AKA_SUCCESS_EV,
  EAP_SIM_AKA_FAILURE_EV       = 9,
  EAP_AKA_PRIME_FAILURE_EV     = EAP_SIM_AKA_FAILURE_EV,
  EAP_SIM_AKA_CNT_TOO_SMALL_EV = 10,
  EAP_AKA_PRIME_CNT_TOO_SMALL_EV = EAP_SIM_AKA_CNT_TOO_SMALL_EV,
  EAP_AKA_PRIME_KDF_NEGOTIATE_EV = 11
} eap_sim_akai_event_enum_type;

/*---------------------------------------------------------------------------
   Enumeration of EAP SIM/AKA Events
---------------------------------------------------------------------------*/
typedef enum 
{
  EAP_SIM_AKA_REQ_EV  = 0,
  EAP_AKA_PRIME_REQ_EV  = EAP_SIM_AKA_REQ_EV,
  EAP_SIM_AKA_RESP_EV   = 1,
  EAP_AKA_PRIME_RESP_EV = EAP_SIM_AKA_RESP_EV
} eap_sim_akai_ev_req_enum_type;


/*--------------------------------------------------------------------------
  EV Identity Type Enumeration
---------------------------------------------------------------------------*/
typedef enum
{
  EAP_SIM_AKA_EV_ID_PERMANET = 0,
  EAP_SIM_AKA_EV_ID_FULL     = 1,
  EAP_SIM_AKA_EV_ID_ANY      = 2
}eap_sim_aka_ev_identity_enum_type;

/*--------------------------------------------------------------------------
  Failure reason type enumeration
---------------------------------------------------------------------------*/
typedef enum
{
  EAP_SIM_AKA_FAIL_INVALID_MAC    = 0,
  EAP_SIM_AKA_FAIL_INVALID_STATE  = 1,
  EAP_SIM_AKA_FAIL_INVALID_ID_REQ = 2,
  EAP_SIM_AKA_FAIL_ERR_NOTIFY     = 3,
  EAP_SIM_AKA_FAIL_INVALID_FMT    = 4,
  EAP_SIM_AKA_FAIL_INTERNAL_ERR   = 5,
  EAP_SIM_FAIL_INVALID_VER        = 6
}eap_sim_aka_failure_reason_enum_type;

/*---------------------------------------------------------------------------
  Definition of the payload of a EAP SIM/AKA event
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_EV_MAX_ID_LEN 8

typedef struct 
{
  eap_sim_akai_event_enum_type          event_type; //1+1+1+1+8+2 = 14
  eap_sim_aka_failure_reason_enum_type  failure_reason;
  eap_sim_akai_ev_req_enum_type         req_resp;
  eap_sim_aka_ev_identity_enum_type     id_type;
  uint8                                 identity[EAP_SIM_AKA_EV_MAX_ID_LEN];
  uint16                                notification;
} eap_sim_akai_event_payload_type;


#endif /* FEATURE_DATA_PS_EAP */
#endif /* PS_EAP_SIM_AKA_PB_H */

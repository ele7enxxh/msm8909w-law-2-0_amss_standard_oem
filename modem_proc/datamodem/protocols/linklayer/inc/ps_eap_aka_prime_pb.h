#ifndef PS_EAP_AKA_PRIME_PB_H
#define PS_EAP_AKA_PRIME_PB_H


/*===========================================================================

        E A P   A U T H E N T I C A T I O N   K E Y   A G R E E M E N T
        
                       P R I M E P A R S E R  B U I L D E R
                            
                
                   
DESCRIPTION
  This file contains EAP-AKA_PRIME PB processing functions.
     
    
  Copyright (c) 2009-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_eap_aka_prime_pb.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $


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
#include "ps_eap_sim_aka_pb.h"
#include "ps_eap_aka_pb.h"

/*===========================================================================

                              INTERNAL DATA 

===========================================================================*/
/* According to RFC  - currently only one version of AKA_PRIME protocol*/
#define EAP_AKA_PRIME_MAX_NO_VERS       1

/* Maximum number of possible input messages - According to RFC        */
#define EAP_AKA_PRIME_MAX_IN_MSG        4

/* Maximum number of possible output messages -According to RFC        */
#define EAP_AKA_PRIME_MAX_OUT_MSG       7

/* Maximum number of possible input attributes - According to RFC      */
#define EAP_AKA_PRIME_MAX_IN_ATTR      18

/* Maximum number of possible output attributes - According to RFC     */
#define EAP_AKA_PRIME_MAX_OUT_ATTR     13

/* Maximum length of auts - According to RFC  */
#define EAP_AKA_PRIME_MAX_AUTS         14

/* Maximum length of res - According to RFC   */
#define EAP_AKA_PRIME_MAX_RES          16

/* Maximum length of kdf_input - According to RFC  */
/* TODO-SAP: Verify if this is correct */
#define EAP_AKA_PRIME_MAX_KDF_INPUT    255

/* Maximum number of AT_KDF attributes - According to RFC  */
#define EAP_AKA_PRIME_MAX_NUM_KDF      32

/* Maximum length of IK - According to RFC    */
#define EAP_AKA_PRIME_IK_LEN           16

/* Maximum length of CK - According to RFC    */
#define EAP_AKA_PRIME_CK_LEN           16

/* Maximum length of RESULT KEY for PRF PRIME ALGO - According to RFC  */
#define EAP_AKA_PRIME_MAX_RES_KEY_LEN  224

/*===========================================================================

                              DATA TYPES

===========================================================================*/

/*===========================================================================
                       Specific EAP-AKA_PRIME Attributes
===========================================================================*/

/*---------------------------------------------------------------------------
  AT_KDF_INPUT
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean                      present;
  uint16                       kdf_input_length;
  uint8                        kdf_input[EAP_AKA_PRIME_MAX_KDF_INPUT];
} eap_aka_prime_kdf_input_type;

/*---------------------------------------------------------------------------
  AT_KDF

  dup_kdf - Indicates a list of all duplicates present in the current
                EAP-REQ/AKA' CHALLENGE packet.

  dup_count - Indicates the number of duplicates present in the
              current EAP-REQ/AKA' CHALLENGE packet.  

---------------------------------------------------------------------------*/
typedef struct 
{
  uint16                       kdf_count;
  uint16                       kdf[EAP_AKA_PRIME_MAX_NUM_KDF];
  uint16                       dup_kdf_count;
  uint16                       dup_kdf[EAP_AKA_PRIME_MAX_NUM_KDF];
} eap_aka_prime_kdf_type;

/*===========================================================================
                         EAP-AKA_PRIME Request Packets
===========================================================================*/

/*---------------------------------------------------------------------------
  Data structure for AKA_PRIME-Challenge request packet
---------------------------------------------------------------------------*/
typedef struct 
{ 
   eap_aka_autn_type            autn;
   eap_aka_rand_type            eap_rand;
   eap_sim_aka_checkcode_type   checkcode;
   eap_sim_aka_mac_type         mac;
   eap_sim_aka_result_ind_type  result_ind;
   eap_sim_aka_iv_type          iv;
   eap_sim_aka_encr_type        encr_data;
   eap_aka_prime_kdf_input_type kdf_input;
   eap_aka_prime_kdf_type       kdf;
} eap_aka_prime_req_challenge_type;

/*===========================================================================
                         EAP-AKA_PRIME Response Packets
===========================================================================*/

/*---------------------------------------------------------------------------
  Data structure for AKA_PRIME-Challenge response packet
---------------------------------------------------------------------------*/
typedef struct 
{
  eap_sim_aka_mac_type         mac;
  eap_aka_res_type             res;
  eap_sim_aka_checkcode_type   checkcode;
  eap_sim_aka_result_ind_type  result_ind;
  eap_sim_aka_iv_type          iv;
  eap_sim_aka_encr_type        encr_data;
  eap_aka_prime_kdf_type       kdf;
}eap_aka_prime_resp_challenge_type;

/*---------------------------------------------------------------------------
  Data structure for AKA_PRIME-Syncro Failure  response packet
---------------------------------------------------------------------------*/
typedef struct 
{
  eap_aka_auts_type         auts;
  eap_aka_prime_kdf_type    kdf;
  boolean                   is_at_kdf_in_sync_fail_required;
}eap_aka_prime_resp_syncfail_type;

/*---------------------------------------------------------------------------
  EAP-AKA_PRIME Request Packet
---------------------------------------------------------------------------*/
typedef union
{
  eap_aka_req_identity_type          aka_prime_identity;
  eap_aka_prime_req_challenge_type   aka_prime_challenge;
  eap_sim_aka_req_notification_type  aka_prime_notifcation;
  eap_sim_aka_req_reauth_type        aka_prime_reauth;
} eap_aka_prime_req_msg_union_type;

typedef struct 
{
  eap_sim_aka_msg_type                msg_type;
  eap_aka_prime_req_msg_union_type    msg_union;
}eap_aka_prime_req_msg_type;

/*---------------------------------------------------------------------------
  EAP-AKA_PRIME Response Packet
---------------------------------------------------------------------------*/
typedef union
{
  eap_aka_resp_identity_type          aka_prime_identity;
  eap_aka_prime_resp_challenge_type   aka_prime_challenge;
  eap_aka_prime_resp_syncfail_type    aka_prime_syncfail;
  eap_sim_aka_resp_notification_type  aka_prime_notifcation;
  eap_sim_aka_resp_reauth_type        aka_prime_reauth;
  eap_sim_aka_resp_clienterr_type     aka_prime_clienterr;
  eap_aka_resp_authreject_type  aka_prime_authreject;
}eap_aka_prime_resp_msg_union_type;

typedef struct 
{
  eap_sim_aka_msg_type                 msg_type;
  eap_aka_prime_resp_msg_union_type    msg_union;
}eap_aka_prime_resp_msg_type;

/*===========================================================================

                     EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
                           Internal methods
===========================================================================*/

/*===========================================================================
                         Message Input Helpers
===========================================================================*/

/*===========================================================================
FUNCTION EAP_AKA_PRIME_IN_MSG_HLP_AKA_PRIME_IDENTIYT

DESCRIPTION
  Helper function for parsing AKA_PRIME-Identity	input message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  request_msg:- pointer to a AKA_PRIME request structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_prime_in_msgh_aka_prime_identity
(
  eap_sim_aka_pb_ctx  *pb_ctx, 
  void                *request_msg
);

/*===========================================================================
FUNCTION EAP_AKA_PRIME_IN_MSG_HLP_AKA_PRIME_CHALLENGE

DESCRIPTION
  Helper function for parsing AKA_PRIME-Challenge	input message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  request_msg:- pointer to a AKA_PRIME request structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_prime_in_msgh_aka_prime_challenge
(
  eap_sim_aka_pb_ctx  *pb_ctx, 
  void                *request_msg
);

/*===========================================================================
FUNCTION EAP_AKA_PRIME_IN_MSG_HLP_AKA_PRIME_NOTIFICATION

DESCRIPTION
  Helper function for parsing AKA_PRIME-Notification input message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  request_msg:- pointer to a AKA_PRIME request structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_prime_in_msgh_aka_prime_notification
(
  eap_sim_aka_pb_ctx  *pb_ctx, 
  void                *request_msg
);

/*===========================================================================
FUNCTION EAP_AKA_PRIME_IN_MSG_HLP_AKA_PRIME_REAUTH

DESCRIPTION
  Helper function for parsing AKA_PRIME-Re-authentication	input message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  request_msg:- pointer to a AKA_PRIME request structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_prime_in_msgh_aka_prime_reauth
(
  eap_sim_aka_pb_ctx  *pb_ctx, 
  void                *request_msg
);


/*===========================================================================
                         Message Output Helpers
===========================================================================*/

/*===========================================================================
FUNCTION EAP_AKA_PRIME_OUT_MSG_HLP_AKA_PRIME_IDENTITY

DESCRIPTION
  Helper function for building AKA_PRIME-identity output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  response_msg:- pointer to a AKA_PRIME response structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_prime_out_msgh_aka_prime_identity
(
  eap_sim_aka_pb_ctx  *pb_ctx, 
  void                *response_msg
);

/*===========================================================================
FUNCTION EAP_AKA_PRIME_OUT_MSG_HLP_AKA_PRIME_CHALLENGE

DESCRIPTION
  Helper function for building AKA_PRIME-Challenge output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  response_msg:- pointer to a AKA_PRIME response structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_prime_out_msgh_aka_prime_challenge
(
  eap_sim_aka_pb_ctx  *pb_ctx, 
  void                *response_msg
);

/*===========================================================================
FUNCTION EAP_AKA_PRIME_OUT_MSG_HLP_AKA_PRIME_NOTIFICATION

DESCRIPTION
  Helper function for building AKA_PRIME-Notification output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  response_msg:- pointer to a AKA_PRIME response structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_prime_out_msgh_aka_prime_notification
(
  eap_sim_aka_pb_ctx  *pb_ctx, 
  void                *response_msg
);

/*===========================================================================
FUNCTION EAP_AKA_PRIME_OUT_MSG_HLP_AKA_PRIME_REAUTH

DESCRIPTION
  Helper function for building AKA_PRIME-Re-authentication output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  response_msg:- pointer to a AKA_PRIME response structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_prime_out_msgh_aka_prime_reauth
(
  eap_sim_aka_pb_ctx  *pb_ctx, 
  void                *response_msg
);

/*===========================================================================
FUNCTION EAP_AKA_PRIME_OUT_MSG_HLP_AKA_PRIME_CLIENT_ERROR

DESCRIPTION
  Helper function for building AKA_PRIME-Client-Error output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  response_msg:- pointer to a AKA_PRIME response structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_prime_out_msgh_aka_prime_client_error
(
  eap_sim_aka_pb_ctx  *pb_ctx, 
  void                *response_msg
);

/*===========================================================================
FUNCTION EAP_AKA_PRIME_OUT_MSG_HLP_AKA_PRIME_SYNCRO_FAILURE

DESCRIPTION
  Helper function for building AKA_PRIME-Syncro-Failure output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  response_msg:- pointer to a AKA_PRIME response structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_prime_out_msgh_aka_prime_syncro_failure
(
  eap_sim_aka_pb_ctx  *pb_ctx, 
  void                *response_msg
);

/*===========================================================================
FUNCTION EAP_AKA_PRIME_OUT_MSG_HLP_AKA_PRIME_AUTH_REJECT

DESCRIPTION
  Helper function for building AKA_PRIME-Auth-Reject output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  response_msg:- pointer to a AKA_PRIME response structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_prime_out_msgh_aka_prime_auth_reject
(
  eap_sim_aka_pb_ctx  *pb_ctx, 
  void                *response_msg
);


/*===========================================================================
                         Attribute Input Helpers
===========================================================================*/

/*===========================================================================
FUNCTION EAP_AKA_PRIME_IN_HLP_AT_AUTN

DESCRIPTION
  Helper function for parsing AT_AUTN input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:- the length and value extracted from the EAP AKA_PRIME packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_aka_prime_in_hlp_at_autn
(
  void                     *attribute,
  eap_sim_aka_len_val_type *len_val
);
	
/*===========================================================================
FUNCTION EAP_AKA_PRIME_IN_HLP_AT_RAND

DESCRIPTION
  Helper function for parsing AT_RAND	input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:- the length and value extracted from the EAP AKA_PRIME packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_aka_prime_in_hlp_at_rand
(
  void                      *attribute,
  eap_sim_aka_len_val_type  *len_val
);

/*===========================================================================
FUNCTION EAP_AKA_PRIME_IN_HLP_AT_CHECKCODE

DESCRIPTION
  Helper function for parsing AT_CHECKCODE	input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:- the length and value extracted from the EAP AKA_PRIME packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_aka_prime_in_hlp_at_checkcode
(
  void                      *attribute,
  eap_sim_aka_len_val_type  *len_val
);

/*===========================================================================
FUNCTION EAP_AKA_PRIME_IN_HLP_AT_KDF_INPUT

DESCRIPTION
  Helper function for parsing AT_KDF_INPUT	input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:- the length and value extracted from the EAP AKA_PRIME packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_aka_prime_in_hlp_at_kdf_input
(
  void                      *attribute,
  eap_sim_aka_len_val_type  *len_val
);

/*===========================================================================
FUNCTION EAP_AKA_PRIME_IN_HLP_AT_KDF

DESCRIPTION
  Helper function for parsing AT_KDF	input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:- the length and value extracted from the EAP AKA_PRIME packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_aka_prime_in_hlp_at_kdf
(
  void                      *attribute,
  eap_sim_aka_len_val_type  *len_val
);


/*===========================================================================
                         Attribute Output Helpers
===========================================================================*/

/*===========================================================================
FUNCTION EAP_AKA_PRIME_OUT_HLP_AT_RES

DESCRIPTION
  Helper function for building AT_RES output attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute:   - pointer to the attr type structure
  type_len_val:- the type, length and value of the attr needed to be added to 
                 the outgoing EAP AKA_PRIME packet
  pb_ctx:      - the PB context
  
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_aka_prime_out_hlp_at_res
(
  void                          *attribute,
  eap_sim_aka_type_len_val_type *type_len_val,
  eap_sim_aka_pb_ctx            *pb_ctx
);

/*===========================================================================
FUNCTION EAP_AKA_PRIME_OUT_HLP_AT_AUTS

DESCRIPTION
  Helper function for building AT_AUTS output attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute:   - pointer to the attr type structure
  type_len_val:- the type, length and value of the attr needed to be added to 
                 the outgoing EAP AKA_PRIME packet
  pb_ctx:      - the PB context
  
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_aka_prime_out_hlp_at_auts
(
  void                           *attribute,
  eap_sim_aka_type_len_val_type  *type_len_val,
  eap_sim_aka_pb_ctx             *pb_ctx
);


/*===========================================================================
FUNCTION EAP_AKA_PRIME_OUT_HLP_AT_CHECKCODE

DESCRIPTION
  Helper function for building AT_CHECKCODE output attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute:   - pointer to the attr type structure
  type_len_val:- the type, length and value of the attr needed to be added to 
                 the outgoing EAP AKA_PRIME packet
  pb_ctx:      - the PB context
  
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_aka_prime_out_hlp_at_checkcode
(
  void                           *attribute,
  eap_sim_aka_type_len_val_type  *type_len_val,
  eap_sim_aka_pb_ctx             *pb_ctx
);

/*===========================================================================
FUNCTION EAP_AKA_PRIME_OUT_HLP_AT_KDF

DESCRIPTION
  Helper function for building AT_KDF output attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute:   - pointer to the attr type structure
  type_len_val:- the type, length and value of the attr needed to be added to 
                 the outgoing EAP AKA_PRIME packet
  pb_ctx:      - the PB context
  
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_aka_prime_out_hlp_at_kdf
(
  void                           *attribute,
  eap_sim_aka_type_len_val_type  *type_len_val,
  eap_sim_aka_pb_ctx             *pb_ctx
);

#endif /* FEATURE_DATA_PS_EAP */
#endif /* PS_EAP_AKA_PRIME_PB_H */

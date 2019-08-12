#ifndef PS_EAP_AKA_PB_H
#define PS_EAP_AKA_PB_H


/*===========================================================================

        E A P   A U T H E N T I C A T I O N   K E Y   A G R E E M E N T
        
                       P A R S E R  B U I L D E R
                            
                
                   
DESCRIPTION
  This file contains EAP-AKA PB processing functions.
     
    
  Copyright (c) 2006-2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_eap_aka_pb.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/31/09    mga    Merged from eHRPD branch
===========================================================================*/



/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"

#ifdef FEATURE_DATA_PS_EAP

#include "comdef.h"
#include "ps_eap_sim_aka_pb.h"

/*===========================================================================

                              INTERNAL DATA 

===========================================================================*/
/* According to RFC 4187 - currently only one version of AKA protocol*/
#define EAP_AKA_MAX_NO_VERS       1

/* Maximum number of possible input messages - According to RFC 4187 */
#define EAP_AKA_MAX_IN_MSG        4

/* Maximum number of possible output messages -According to RFC 4187 */
#define EAP_AKA_MAX_OUT_MSG       7

/* Maximum number of possible input attributes - According to RFC 4187 */
#define EAP_AKA_MAX_IN_ATTR      17

/* Maximum number of possible output attributes - According to RFC 4187 */
#define EAP_AKA_MAX_OUT_ATTR     12

/* Maximum length of autn - According to RFC 4187 */
#define EAP_AKA_MAX_AUTN         16

/* Maximum length of auts - According to RFC 4187 */
#define EAP_AKA_MAX_AUTS         14

/* Maximum length of res - According to RFC 4187 */
#define EAP_AKA_MAX_RES          16

/* Maximum length of IK - According to RFC 4187 */
#define EAP_AKA_IK_LEN           16

/* Maximum length of CK - According to RFC 4187 */
#define EAP_AKA_CK_LEN           16
/*===========================================================================

                              DATA TYPES

===========================================================================*/

/*===========================================================================
                       Specific EAP-AKA Attributes
===========================================================================*/

/*---------------------------------------------------------------------------
  AT_AUTN
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean        present;
  uint16         autn_len;
  uint8          autn[EAP_AKA_MAX_AUTN];
} eap_aka_autn_type;

/*---------------------------------------------------------------------------
  AT_AUTS
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean        present;
  uint16         auts_len;
  uint8          auts[EAP_AKA_MAX_AUTS];
} eap_aka_auts_type;

/*---------------------------------------------------------------------------
  AT_RES
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean        present;
  uint16         res_len;
  uint8          res[EAP_AKA_MAX_RES];
} eap_aka_res_type;

/*---------------------------------------------------------------------------
  AT_RAND
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean                present;
  eap_sim_aka_rand_type  eap_rand;
} eap_aka_rand_type;

/*---------------------------------------------------------------------------
  AT_BIDDING
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean                           present;
  boolean                           bidding_val;
} eap_aka_bidding_type;


/*===========================================================================
                         EAP-AKA Request Packets
===========================================================================*/

/*---------------------------------------------------------------------------
  Data structure for AKA-Identity request packet
---------------------------------------------------------------------------*/
typedef struct 
{
   eap_sim_aka_identity_req_type  id_req;
} eap_aka_req_identity_type;

/*---------------------------------------------------------------------------
  Data structure for AKA-Challenge request packet
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
   eap_aka_bidding_type         bidding;
} eap_aka_req_challenge_type;

/*===========================================================================
                         EAP-AKA Response Packets
===========================================================================*/
/*---------------------------------------------------------------------------
  Data structure for AKA-Start response packet
---------------------------------------------------------------------------*/
typedef struct 
{
  eap_sim_aka_identity_resp_type  identity;
}eap_aka_resp_identity_type;

/*---------------------------------------------------------------------------
  Data structure for AKA-Challenge response packet
---------------------------------------------------------------------------*/
typedef struct 
{
  eap_sim_aka_mac_type         mac;
  eap_aka_res_type             res;
  eap_sim_aka_checkcode_type   checkcode;
  eap_sim_aka_result_ind_type  result_ind;
  eap_sim_aka_iv_type          iv;
  eap_sim_aka_encr_type        encr_data;
}eap_aka_resp_challenge_type;

/*---------------------------------------------------------------------------
  Data structure for AKA-Syncro Failure  response packet
---------------------------------------------------------------------------*/
typedef struct 
{
  eap_aka_auts_type         auts;
}eap_aka_resp_syncfail_type;

/*---------------------------------------------------------------------------
  Data structure for AKA-Authentication Reject  response packet
---------------------------------------------------------------------------*/
typedef struct 
{
  /* structure defined just for completness - Auth Reject Pkt does not 
     contain attributes */
  uint8 empty;
}eap_aka_resp_authreject_type;

/*---------------------------------------------------------------------------
  EAP-AKA Request Packet
---------------------------------------------------------------------------*/
typedef union
{
  eap_aka_req_identity_type          aka_identity;
  eap_aka_req_challenge_type         aka_challenge;
  eap_sim_aka_req_notification_type  aka_notifcation;
  eap_sim_aka_req_reauth_type        aka_reauth;
} eap_aka_req_msg_union_type;

typedef struct 
{
  eap_sim_aka_msg_type          msg_type;
  eap_aka_req_msg_union_type    msg_union;
}eap_aka_req_msg_type;

/*---------------------------------------------------------------------------
  EAP-AKA Response Packet
---------------------------------------------------------------------------*/
typedef union
{
  eap_aka_resp_identity_type          aka_identity;
  eap_aka_resp_challenge_type         aka_challenge;
  eap_aka_resp_syncfail_type          aka_syncfail;
  eap_sim_aka_resp_notification_type  aka_notifcation;
  eap_sim_aka_resp_reauth_type        aka_reauth;
  eap_sim_aka_resp_clienterr_type     aka_clienterr;
  eap_aka_resp_authreject_type        aka_authreject;
}eap_aka_resp_msg_union_type;

typedef struct 
{
  eap_sim_aka_msg_type           msg_type;
  eap_aka_resp_msg_union_type    msg_union;
}eap_aka_resp_msg_type;

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
FUNCTION EAP_AKA_IN_MSG_HLP_AKA_IDENTIYT

DESCRIPTION
  Helper function for parsing AKA-Identity	input message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  request_msg:- pointer to a AKA request structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_in_msgh_aka_identity
(
  eap_sim_aka_pb_ctx*  pb_ctx, 
  void*                request_msg
);

/*===========================================================================
FUNCTION EAP_AKA_IN_MSG_HLP_AKA_CHALLENGE

DESCRIPTION
  Helper function for parsing AKA-Challenge	input message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  request_msg:- pointer to a AKA request structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_in_msgh_aka_challenge
(
  eap_sim_aka_pb_ctx*  pb_ctx, 
  void*                request_msg
);

/*===========================================================================
FUNCTION EAP_AKA_IN_MSG_HLP_AKA_NOTIFICATION

DESCRIPTION
  Helper function for parsing AKA-Notification input message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  request_msg:- pointer to a AKA request structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_in_msgh_aka_notification
(
  eap_sim_aka_pb_ctx*  pb_ctx, 
  void*                request_msg
);

/*===========================================================================
FUNCTION EAP_AKA_IN_MSG_HLP_AKA_REAUTH

DESCRIPTION
  Helper function for parsing AKA-Re-authentication	input message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  request_msg:- pointer to a AKA request structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_in_msgh_aka_reauth
(
  eap_sim_aka_pb_ctx*  pb_ctx, 
  void*                request_msg
);

/*===========================================================================
                 Common AKA Message Input Helpers
===========================================================================*/
/*===========================================================================
FUNCTION EAP_AKA_IN_MSGH_IDENTITY

DESCRIPTION
  Helper function for parsing AKA-Identity input message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  id_req:- pointer to eap_sim_aka_identity_resp_type 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_in_msgh_identity
(
  eap_sim_aka_pb_ctx             *pb_ctx, 
  eap_sim_aka_identity_req_type  *id_req
);

/*===========================================================================
                 Common AKA Message Output Helpers
===========================================================================*/

/*===========================================================================
FUNCTION EAP_AKA_OUT_MSGH_IDENTITY

DESCRIPTION
  Helper function for parsing AKA-Identity output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  resp_id:- pointer to a eap_sim_aka_resp_identity_type structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_out_msgh_identity
(
  eap_sim_aka_pb_ctx              *pb_ctx, 
  eap_sim_aka_identity_resp_type  *resp_id
);

/*===========================================================================
FUNCTION EAP_AKA_OUT_MSGH_CHALLENGE

DESCRIPTION
  Helper function for parsing AKA-Challenge output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  resp_id:- pointer to a eap_sim_aka_resp_challenge_type structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_out_msgh_challenge
(
  eap_sim_aka_pb_ctx          *pb_ctx, 
  eap_aka_resp_challenge_type *resp_chal
);

/*===========================================================================
FUNCTION EAP_AKA_OUT_MSGH_SYNC_FAIL

DESCRIPTION
  Helper function for parsing AKA-Syncro failure output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  resp_cl_err:- pointer to a eap_aka_resp_syncfail_type structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_out_msgh_syncfail
(
  eap_sim_aka_pb_ctx             *pb_ctx, 
  eap_aka_resp_syncfail_type     *resp_syncfail
);


/*===========================================================================
                         Message Output Helpers
===========================================================================*/

/*===========================================================================
FUNCTION EAP_AKA_OUT_MSG_HLP_AKA_IDENTITY

DESCRIPTION
  Helper function for building AKA-identity output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  response_msg:- pointer to a AKA response structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_out_msgh_aka_identity
(
  eap_sim_aka_pb_ctx*  pb_ctx, 
  void*                response_msg
);

/*===========================================================================
FUNCTION EAP_AKA_OUT_MSG_HLP_AKA_CHALLENGE

DESCRIPTION
  Helper function for building AKA-Challenge output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  response_msg:- pointer to a AKA response structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_out_msgh_aka_challenge
(
  eap_sim_aka_pb_ctx*  pb_ctx, 
  void*                response_msg
);

/*===========================================================================
FUNCTION EAP_AKA_OUT_MSG_HLP_AKA_NOTIFICATION

DESCRIPTION
  Helper function for building AKA-Notification output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  response_msg:- pointer to a AKA response structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_out_msgh_aka_notification
(
  eap_sim_aka_pb_ctx*  pb_ctx, 
  void*                response_msg
);

/*===========================================================================
FUNCTION EAP_AKA_OUT_MSG_HLP_AKA_REAUTH

DESCRIPTION
  Helper function for building AKA-Re-authentication output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  response_msg:- pointer to a AKA response structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_out_msgh_aka_reauth
(
  eap_sim_aka_pb_ctx*  pb_ctx, 
  void*                response_msg
);

/*===========================================================================
FUNCTION EAP_AKA_OUT_MSG_HLP_AKA_CLIENT_ERROR

DESCRIPTION
  Helper function for building AKA-Client-Error output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  response_msg:- pointer to a AKA response structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_out_msgh_aka_client_error
(
  eap_sim_aka_pb_ctx*  pb_ctx, 
  void*                response_msg
);

/*===========================================================================
FUNCTION EAP_AKA_OUT_MSG_HLP_AKA_SYNCRO_FAILURE

DESCRIPTION
  Helper function for building AKA-Syncro-Failure output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  response_msg:- pointer to a AKA response structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_out_msgh_aka_syncro_failure
(
  eap_sim_aka_pb_ctx*  pb_ctx, 
  void*                response_msg
);

/*===========================================================================
FUNCTION EAP_AKA_OUT_MSG_HLP_AKA_AUTH_REJECT

DESCRIPTION
  Helper function for building AKA-Auth-Reject output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  response_msg:- pointer to a AKA response structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_aka_out_msgh_aka_auth_reject
(
  eap_sim_aka_pb_ctx*  pb_ctx, 
  void*                response_msg
);


/*===========================================================================
                         Attribute Input Helpers
===========================================================================*/

/*===========================================================================
FUNCTION EAP_AKA_IN_HLP_AT_AUTN

DESCRIPTION
  Helper function for parsing AT_AUTN input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:- the length and value extracted from the EAP AKA packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_aka_in_hlp_at_autn
(
  void*                      attribute,
  eap_sim_aka_len_val_type*  len_val
);
	
/*===========================================================================
FUNCTION EAP_AKA_IN_HLP_AT_RAND

DESCRIPTION
  Helper function for parsing AT_RAND	input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:- the length and value extracted from the EAP AKA packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_aka_in_hlp_at_rand
(
  void*                      attribute,
  eap_sim_aka_len_val_type*  len_val
);

/*===========================================================================
FUNCTION EAP_AKA_IN_HLP_AT_CHECKCODE

DESCRIPTION
  Helper function for parsing AT_CHECKCODE	input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:- the length and value extracted from the EAP AKA packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_aka_in_hlp_at_checkcode
(
  void*              attribute,
  eap_sim_aka_len_val_type*      len_val
);

/*===========================================================================
FUNCTION EAP_AKA_IN_HLP_AT_BIDDING

DESCRIPTION
  Helper function for parsing AT_BIDDING input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:- the length and value extracted from the EAP AKA packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_aka_in_hlp_at_bidding
(
  void*                       attribute,
  eap_sim_aka_len_val_type*   len_val
);


/*===========================================================================
                         Attribute Output Helpers
===========================================================================*/

/*===========================================================================
FUNCTION EAP_AKA_OUT_HLP_AT_RES

DESCRIPTION
  Helper function for building AT_RES output attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute:   - pointer to the attr type structure
  type_len_val:- the type, length and value of the attr needed to be added to 
                 the outgoing EAP AKA packet
  pb_ctx:      - the PB context
  
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_aka_out_hlp_at_res
(
  void*                attribute,
  eap_sim_aka_type_len_val_type*   type_len_val,
  eap_sim_aka_pb_ctx*  pb_ctx
);

/*===========================================================================
FUNCTION EAP_AKA_OUT_HLP_AT_AUTS

DESCRIPTION
  Helper function for building AT_AUTS output attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute:   - pointer to the attr type structure
  type_len_val:- the type, length and value of the attr needed to be added to 
                 the outgoing EAP AKA packet
  pb_ctx:      - the PB context
  
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_aka_out_hlp_at_auts
(
  void*                attribute,
  eap_sim_aka_type_len_val_type*   type_len_val,
  eap_sim_aka_pb_ctx*  pb_ctx
);


/*===========================================================================
FUNCTION EAP_AKA_OUT_HLP_AT_CHECKCODE

DESCRIPTION
  Helper function for building AT_CHECKCODE output attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute:   - pointer to the attr type structure
  type_len_val:- the type, length and value of the attr needed to be added to 
                 the outgoing EAP AKA packet
  pb_ctx:      - the PB context
  
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_aka_out_hlp_at_checkcode
(
  void*                attribute,
  eap_sim_aka_type_len_val_type*   type_len_val,
  eap_sim_aka_pb_ctx*  pb_ctx
);

#endif /* FEATURE_DATA_PS_EAP */
#endif /* PS_EAP_AKA_PB_H */

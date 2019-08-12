#ifndef PS_EAP_SIM_PB_H
#define PS_EAP_SIM_PB_H


/*===========================================================================

        E A P  S U B S C R I B E R  I D E N T I T Y  M O D U L E
        
                       P A R S E R  B U I L D E R
                            
                
                   
DESCRIPTION
  This file contains EAP-SIM PB processing functions.
     
    
  Copyright (c) 2006-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_eap_sim_pb.h#1 $ 
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

/*===========================================================================

                              INTERNAL DATA 

===========================================================================*/
/* According to RFC 4186 - currently only one version of SIM protocol*/
#define EAP_SIM_MAX_NO_VERS       1

/* According to RFC 4186 */
#define EAP_SIM_MAX_NONCE_MT_LEN  16

/* Maximum number of possible input messages - According to RFC 4186 */
#define EAP_SIM_MAX_IN_MSG        4

/* Maximum number of possible output messages -According to RFC 4186 */
#define EAP_SIM_MAX_OUT_MSG       5

/* Maximum number of possible input attributes - According to RFC 4186 */
#define EAP_SIM_MAX_IN_ATTR        15

/* Maximum number of possible output attributes - According to RFC 4186 */
#define EAP_SIM_MAX_OUT_ATTR       11
/*===========================================================================

                              DATA TYPES

===========================================================================*/

/*===========================================================================
                       Specific EAP-SIM Attributes
===========================================================================*/

/*---------------------------------------------------------------------------
  AT_VERSION_LIST
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean        present;
  uint16         vers_no;
  uint16         vers[EAP_SIM_MAX_NO_VERS];
} eap_sim_ver_list_type;

/*---------------------------------------------------------------------------
  AT_RAND
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean        present;
  uint8          no_of_rand;
  eap_sim_aka_rand_type      eap_rand[EAP_SIM_MAX_RAND_NO];
  eap_sim_aka_clienterr_enum_type      client_error_type;
} eap_sim_rand_type;

/*---------------------------------------------------------------------------
  AT_SELECTED_VERSION
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean        present;
  uint16         version;
} eap_sim_selected_ver_type;

/*---------------------------------------------------------------------------
  AT_NONCE_MT
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean        present;
  uint8          nonce_mt_len;
  uint8          nonce_mt[EAP_SIM_MAX_NONCE_MT_LEN];
} eap_sim_nonce_mt_type;


/*===========================================================================
                         EAP-SIM Request Packets
===========================================================================*/

/*---------------------------------------------------------------------------
  Data structure for SIM-Start request packet
---------------------------------------------------------------------------*/
typedef struct 
{
   eap_sim_aka_identity_req_type  id_req;
   eap_sim_ver_list_type          ver_list;
} eap_sim_req_start_type;

/*---------------------------------------------------------------------------
  Data structure for SIM-Challenge request packet
---------------------------------------------------------------------------*/
typedef struct 
{
   eap_sim_rand_type            eap_rand;
   eap_sim_aka_mac_type         mac;
   eap_sim_aka_result_ind_type  result_ind;
   eap_sim_aka_iv_type          iv;
   eap_sim_aka_encr_type        encr_data;
} eap_sim_req_challenge_type;

/*===========================================================================
                         EAP-SIM Response Packets
===========================================================================*/
/*---------------------------------------------------------------------------
  Data structure for SIM-Start response packet
---------------------------------------------------------------------------*/
typedef struct 
{
  eap_sim_aka_identity_resp_type  identity;
  eap_sim_selected_ver_type       selected_ver;
  eap_sim_nonce_mt_type           nonce_mt;
}eap_sim_resp_start_type;

/*---------------------------------------------------------------------------
  Data structure for SIM-Challenge response packet
---------------------------------------------------------------------------*/
typedef struct 
{
  eap_sim_aka_mac_type         mac;
  eap_sim_aka_result_ind_type  result_ind;
  eap_sim_aka_iv_type          iv;
  eap_sim_aka_encr_type        encr_data;
}eap_sim_resp_challenge_type;

/*---------------------------------------------------------------------------
  EAP-SIM Request Packet
---------------------------------------------------------------------------*/
typedef union
{
  eap_sim_req_start_type             sim_start;
  eap_sim_req_challenge_type         sim_challenge;
  eap_sim_aka_req_notification_type  sim_notifcation;
  eap_sim_aka_req_reauth_type        sim_reauth;
} eap_sim_req_msg_union_type;

typedef struct 
{
  eap_sim_aka_msg_type          msg_type;
  eap_sim_req_msg_union_type    msg_union;
}eap_sim_req_msg_type;

/*---------------------------------------------------------------------------
  EAP-SIM Response Packet
---------------------------------------------------------------------------*/
typedef union
{
  eap_sim_resp_start_type             sim_start;
  eap_sim_resp_challenge_type         sim_challenge;
  eap_sim_aka_resp_notification_type  sim_notifcation;
  eap_sim_aka_resp_reauth_type        sim_reauth;
  eap_sim_aka_resp_clienterr_type     sim_clienterr;
} eap_sim_resp_msg_union_type;

typedef struct 
{
  eap_sim_aka_msg_type           msg_type;
  eap_sim_resp_msg_union_type    msg_union;
}eap_sim_resp_msg_type;

/*===========================================================================

                     EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
                         Message Input Helpers
===========================================================================*/

/*===========================================================================
FUNCTION EAP_SIM_IN_MSG_HLP_SIM_START

DESCRIPTION
  Helper function for parsing SIM-Start	input message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  request_msg:- pointer to a SIM request structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_sim_in_msgh_sim_start
(
  eap_sim_aka_pb_ctx*  pb_ctx, 
  void*                request_msg
);

/*===========================================================================
FUNCTION EAP_SIM_IN_MSG_HLP_SIM_CHALLENGE

DESCRIPTION
  Helper function for parsing SIM-Challenge	input message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  request_msg:- pointer to a SIM request structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_sim_in_msgh_sim_challenge
(
  eap_sim_aka_pb_ctx*  pb_ctx, 
  void*                request_msg
);

/*===========================================================================
FUNCTION EAP_SIM_IN_MSG_HLP_SIM_NOTIFICATION

DESCRIPTION
  Helper function for parsing SIM-Notification input message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  request_msg:- pointer to a SIM request structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_sim_in_msgh_sim_notification
(
  eap_sim_aka_pb_ctx*  pb_ctx, 
  void*                request_msg
);

/*===========================================================================
FUNCTION EAP_SIM_IN_MSG_HLP_SIM_REAUTH

DESCRIPTION
  Helper function for parsing SIM-Re-authentication	input message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  request_msg:- pointer to a SIM request structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_sim_in_msgh_sim_reauth
(
  eap_sim_aka_pb_ctx*  pb_ctx, 
  void*                request_msg
);


/*===========================================================================
                         Message Output Helpers
===========================================================================*/

/*===========================================================================
FUNCTION EAP_SIM_OUT_MSG_HLP_SIM_START

DESCRIPTION
  Helper function for building SIM-Start output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  response_msg:- pointer to a SIM response structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_sim_out_msgh_sim_start
(
  eap_sim_aka_pb_ctx*  pb_ctx, 
  void*                response_msg
);

/*===========================================================================
FUNCTION EAP_SIM_OUT_MSG_HLP_SIM_CHALLENGE

DESCRIPTION
  Helper function for building SIM-Challenge output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  response_msg:- pointer to a SIM response structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_sim_out_msgh_sim_challenge
(
  eap_sim_aka_pb_ctx*  pb_ctx, 
  void*                response_msg
);

/*===========================================================================
FUNCTION EAP_SIM_OUT_MSG_HLP_SIM_NOTIFICATION

DESCRIPTION
  Helper function for building SIM-Notification output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  response_msg:- pointer to a SIM response structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_sim_out_msgh_sim_notification
(
  eap_sim_aka_pb_ctx*  pb_ctx, 
  void*                response_msg
);

/*===========================================================================
FUNCTION EAP_SIM_OUT_MSG_HLP_SIM_REAUTH

DESCRIPTION
  Helper function for building SIM-Re-authentication output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  response_msg:- pointer to a SIM response structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_sim_out_msgh_sim_reauth
(
  eap_sim_aka_pb_ctx*  pb_ctx, 
  void*                response_msg
);

/*===========================================================================
FUNCTION EAP_SIM_OUT_MSG_HLP_SIM_CLIENT_ERROR

DESCRIPTION
  Helper function for building SIM-Client-Error output message.
  
DEPENDENCIES
  None

PARAMETERS
  pb_ctx: - pointer to the current context
  response_msg:- pointer to a SIM response structure 
    
RETURN VALUE
  Success or failure, Drop or Keep packet

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_msgh_res_type
eap_sim_out_msgh_sim_client_error
(
  eap_sim_aka_pb_ctx*  pb_ctx, 
  void*                response_msg
);


/*===========================================================================
                         Attribute Input Helpers
===========================================================================*/

/*===========================================================================
FUNCTION EAP_SIM_IN_HLP_AT_VERLIST

DESCRIPTION
  Helper function for parsing AT_VERSION_LIST input attribute.
  
DEPENDENCIES
  None

PARAMETERS
  attribute: - pointer to the attr type structure
  len_val:- the length and value extracted from the EAP SIM packet
    
RETURN VALUE
  Success or failure

SIDE EFFECTS
  
===========================================================================*/
eap_sim_aka_attrh_res_type
eap_sim_in_hlp_at_verlist
(
  void*                       attribute,
  eap_sim_aka_len_val_type*   len_val
);
	
/*===========================================================================
FUNCTION EAP_SIM_IN_HLP_AT_RAND

DESCRIPTION
  Helper function for parsing AT_RAND	input attribute.
  
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
eap_sim_in_hlp_at_rand
(
  void*                       attribute,
  eap_sim_aka_len_val_type*   len_val
);

/*===========================================================================
                         Attribute Output Helpers
===========================================================================*/

/*===========================================================================
FUNCTION EAP_SIM_OUT_HLP_AT_SELECTED_VERSION 

DESCRIPTION
  Helper function for building AT_SELECTED_VERSION output attribute.
  
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
eap_sim_out_hlp_at_selected
(
  void*                attribute,
  eap_sim_aka_type_len_val_type*   type_len_val,
  eap_sim_aka_pb_ctx*  pb_ctx
);


/*===========================================================================
FUNCTION EAP_SIM_OUT_HLP_AT_NONCE_MT 

DESCRIPTION
  Helper function for building AT_NONCE_MT output attribute.
  
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
eap_sim_out_hlp_at_nonce_mt
(
  void*                attribute,
  eap_sim_aka_type_len_val_type*   type_len_val,
  eap_sim_aka_pb_ctx*  pb_ctx
);

#endif /* FEATURE_DATA_PS_EAP */
#endif /* PS_EAP_SIM_PB_H */

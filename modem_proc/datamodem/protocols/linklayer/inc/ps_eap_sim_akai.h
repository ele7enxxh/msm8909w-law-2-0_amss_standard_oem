#ifndef PS_EAP_SIM_AKAI_H
#define PS_EAP_SIM_AKAI_H


/*===========================================================================

       E A P   S U B S C R I B E R   I D E N T I T Y   M O D U L E
        A U T H E N T I C A T I O N   K E Y   A G R E E M E N T
                          I N T E R N A L S
                
                   
DESCRIPTION
  This file contains common EAP-SIM/AKA processing functions.
     
    
  Copyright (c) 2006-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_eap_sim_akai.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
08/12/14    fn     Added option to configure realm inheritance for reauth id 
07/26/13    cx     Replace memcpy and memmove with memscpy and memsmove
10/26/12    fn     Added debug logging functions, activate by setting 
                    feature EAP_SIM_AKA_KEY_DEBUG_ENABLED to 1
11/09/11    scb     Fix to add AT_KDF values to the Sync Fail response
08/26/11    op     Added changes to obtain supplementary SIM/AKA auth info 
08/31/09    mga    Merged from eHRPD branch
===========================================================================*/



/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"

#ifdef FEATURE_DATA_PS_EAP

#include "comdef.h"
#include "ps_eap.h"

#include "ps_eap_sim_pb.h"
#include "ps_eap_aka_pb.h"
#include "ps_eap_aka_prime_pb.h"
#include "ps_eap_sim_aka_pb.h"

#include "ps_eap_sim_aka_ime.h"

/*===========================================================================

               EAP SIM DATA DECLARATIONS AND DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  EAP SIM/AKA/AKA' Per RFC section 5.1 counter should be intiialize to 1 
   on full authentication
---------------------------------------------------------------------------*/
#define EAP_SIM_AKA_AT_COUNTER_INIT_VALUE 1

/*---------------------------------------------------------------------------
  EAP SIM/AKA internal states
---------------------------------------------------------------------------*/
typedef enum
{
  EAP_SIM_AKA_INIT_ST                = 0,
  EAP_SIM_AKA_IDENTITY_RESOLUTION_ST = 1,
  EAP_SIM_AKA_CHALLENGE_ST           = 2,
  EAP_SIM_AKA_REAUTH_ST              = 3,
  EAP_SIM_AKA_RESULT_ST              = 4,
  EAP_SIM_AKA_CLOSING_ST             = 5,
  EAP_SIM_AKA_MAX_ST
}eap_sim_aka_istates_enum_type;

/*---------------------------------------------------------------------------
  EAP SIM/AKA entity context
---------------------------------------------------------------------------*/
typedef struct
{
  /* EAP handle coressponding to an instance - helps to uniquely idenitfy it*/
  eap_handle_type                eap_handle; 

  /* Parser-Builder context for an instance*/
  eap_sim_aka_pb_ctx             pb_ctx;

  /* Identity Management context for an instance*/
  eap_sim_aka_ime_client_ctx     ime_ctx;

  /* Current state for SIM/AKA FSM*/
  eap_sim_aka_istates_enum_type  current_state; 

  /* Flag signaling presence of result ind flag in challenge/re-auth pkt
   - used within the FSM to determine if it is necesary to wait for 
   notification pkt */
  boolean                        result_ind_present;

  /* Flag signaling if fast-reauthentication is currently used for the
     EAP authentication exchange */
  boolean                        reauth_exchange;

  /* Result of the authentication */  
  eap_result_enum_type           auth_result;

  /* Asynchronous reference counter - used to prevent a SIM/AKA instance from
  closing in case async calls to IME/UME did not return */
  uint16                         async_ref_counter;

  /* Re-auth counter - stored in SFS */
  uint16                         old_counter;

  /* Current Identity used in SIM/AKA conversation - used to extract current 
  realm*/
  eap_sim_aka_ime_id_type        current_id;

  /* IV used for encryption */
  uint8                          send_iv[EAP_SIM_AKA_MAX_IV_LEN];
  /*-----------------------------------------------------------------------
    Keying material 
  -------------------------------------------------------------------------*/

  /* Intermediary master key - saved for re-auth*/
  uint8                          mk[EAP_AKA_PRIME_MAX_MK];

  /*Computed/restored Encryption key */
  uint8                          k_encr[EAP_SIM_AKA_MAX_K_ENCR];

  /*Computed/restored authentication key */
  uint8                          k_aut[EAP_AKA_PRIME_MAX_K_AUT];

  /*Computed Master Session key */
  uint8                          msk[EAP_SIM_AKA_MAX_MSK];

  /*Computed Extended Master Session key */
  uint8                          emsk[EAP_SIM_AKA_MAX_EMSK];

  /*-----------------------------------------------------------------------
    Keying material for AKA_PRIME
  -------------------------------------------------------------------------*/

  /*Computed/restored Encryption key */
  uint8                          k_re_prime[EAP_AKA_PRIME_MAX_K_RE];

  /*-----------------------------------------------------------------------
    SIM/AKA authentication results
  -------------------------------------------------------------------------*/

  /* Computed sres and its length from the SIM/AKA authentication*/
  uint8                          sres[EAP_SIM_AKA_MAX_SRES];
  uint16                         sres_len;

  /*Computed encryption key and its length from the SIM authentication
   in case of AKA save both the encryption key and integrity key */
  uint8                          kc[EAP_SIM_AKA_MAX_KC];
  uint16                         kc_len;
  
  /*Computed encryption key and its length from the AKA_PRIME 
    authentication save both the encryption key and integrity key */
  uint8                          kc_prime[EAP_AKA_PRIME_MAX_KC];
  uint16                         kc_prime_len;

  /*-----------------------------------------------------------------------
    Received data
  -------------------------------------------------------------------------*/
  /* Received MAC in an incoming pkt and its len - stored for validation*/
  uint8                          rx_mac[EAP_SIM_AKA_MAX_MAC_LEN];
  uint16                         rx_mac_len;

  /*Offset of the MAC attribute in the received pkt*/
  uint16                         rx_mac_offset;
  
  /* Flag signaling the presence of encrypted data inside the received pkt
   bot AT_ENCR_DATA and AT_IV need to be present for this to be turned on*/
  boolean                        rx_encr_data_present;
 
  /* Received encrypted data and its IV */
  uint8                          rx_encr_data[EAP_SIM_AKA_MAX_ENCR_DATA_LEN];
  uint16                         rx_encr_data_len;
  uint8                          rx_iv[EAP_SIM_AKA_MAX_IV_LEN];
  uint16                         rx_iv_len;
  
  /* Flag signaling the presence of a re-auth id in the packet - it will 
  trigger storing of the TEKs upon successful indication*/
  boolean                        reauth_id_present;
  
   /*-----------------------------------------------------------------------
    SIM specific data 
  -------------------------------------------------------------------------*/
  /* Specific only to SIM - the negotiated version*/
  uint16                         version;

  /* Specific only to SIM - list of received versions*/
  dsm_item_type*                 ver_list;

  /* Specific only to SIM - client nonce*/
  uint8                          nonce_mt[EAP_SIM_MAX_NONCE_MT_LEN];

  eap_sim_aka_task_srv_req_type task_srv_fct;
  uint32                        min1;
  uint16                        min2;

  /* KDF_INPUT attribute length and value */
  uint16                        kdf_input_length;
  uint8                         kdf_input[EAP_AKA_PRIME_MAX_KDF_INPUT];

  /* KDF attribute length and value stored for negotiation purposes */
  uint16                        kdf_count;
  uint16                        kdf[EAP_AKA_PRIME_MAX_NUM_KDF];

  uint8                         conc_sqn[EAP_SQN_LEN];

  /* KDF attribute actual KDF count retrieved after negotiation */
  uint16                        actual_kdf_count;

  /*  The fast re-authentication identity is transmitted as a complete NAI,
   including a realm portion if a realm is required but some spec require
   peer to inherit realm if realm is not sent as part of the reauth id
   (RFC 4186 4.2.1.7 /4187 4.1.1.7) */
  boolean                       reauth_realm_inherit;

}eap_sim_aka_ctx;

/*---------------------------------------------------------------------------
  EAP_SIM_AKA_EVT_ENUM
    Enumeration of the various events that may be input on a SIM/AKA state
---------------------------------------------------------------------------*/
typedef enum
{
  EAP_SIM_AKA_MIN_EVT           = -1,
  EAP_SIM_AKA_START_EVT         =  0, /* AKA_IDENTITY or SIM_START pkt received */
  EAP_SIM_AKA_CHALLENGE_EVT     =  1, /* AKA or SIM Challenge pkt received */
  EAP_SIM_AKA_NOTIFICATION_EVT  =  2, /* Notification Pkt received */
  EAP_SIM_AKA_REAUTH_EVT        =  3, /* Re-Auth Pkt Received */
  EAP_SIM_AKA_MAX_EVT
} eap_sim_aka_evt_enum;


/*---------------------------------------------------------------------------
  Prototype of a state event handler.
---------------------------------------------------------------------------*/
typedef boolean (*eap_sim_aka_st_fnc_type)( eap_sim_aka_ctx*  sim_aka_inst,
                                            void*             user_data);


/*---------------------------------------------------------------------------
   This describes an entry in the SIM/AKA State Machine.
---------------------------------------------------------------------------*/
typedef struct
{

  eap_sim_aka_st_fnc_type  state_fnc_tbl[EAP_SIM_AKA_MAX_EVT];

} eap_sim_aka_fsm_entry_type;

/*---------------------------------------------------------------------------
   Prototype of a function registered to retrieve a nonce
---------------------------------------------------------------------------*/
typedef boolean (*eap_sim_aka_get_nonce_type)(uint8* eap_rand, uint16 eap_rand_size);

/*---------------------------------------------------------------------------
   Prototype of a function registered to retrieve an IV
---------------------------------------------------------------------------*/
typedef boolean (*eap_sim_aka_get_iv_type)(uint8* eap_rand , uint16 eap_rand_size);

/*===========================================================================

                     EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION EAP_SIM_AKA_INIT

DESCRIPTION
  Initializes the SIM-AKA common modules - UME, IME and CRY

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
    
===========================================================================*/
void
eap_sim_aka_init
(
 void
);

/*===========================================================================
FUNCTION EAP_SIM_AKA_REGISTER_IV_FCT

DESCRIPTION
  Registeres the function used to retrieve the IV by the SIM/AKA Core.
  Used as pointer in order to be easily stubbed.

DEPENDENCIES
  None

PARAMETERS
  fct_ptr - pointer to a function that can retrieve an IV
  
RETURN VALUE
  None

SIDE EFFECTS
    
===========================================================================*/
void
eap_sim_aka_register_iv_fct
(
 eap_sim_aka_get_iv_type  fct_ptr
);

/*===========================================================================
FUNCTION  EAP_SIM_XMIT_PKT

DESCRIPTION
  Called by the core or the internal managers in order to transmit packets 
  over the network by the underlying EAP.

DEPENDENCIES
  None

PARAMETERS
  userdata - data used to identify the current eap-sim session
  msg_ptr  - data to send
  
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void 
eap_sim_aka_xmit_pkt
(
  eap_handle_type  eap_handle, 
  dsm_item_type    **msg_ptr
);


/*===========================================================================
FUNCTION  EAP_SIM_AUTH_COMPLETE_IND

DESCRIPTION
  Called by EAP-SIM/AKA Core or Error Manager in order to notify authetication 
  result and PMK.

DEPENDENCIES
  None

PARAMETERS
  eap_handle: 			    current eap handle
  result_ind_user_data: data used to identify the current eap-sim session
  result: 					    the authetication result
  pre_master_key: 			Computed PMK of the calling EAP method
  pre_master_key_len: 	PMK length
  
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void 
eap_sim_aka_auth_complete_ind
( 
  eap_sim_aka_ctx*      sim_aka_inst,
  eap_result_enum_type  result,
  uint8                *pre_master_key,
  uint16                pre_master_key_len
);


/*===========================================================================
FUNCTION EAP_SIM_AKA_PB_BUILD_AND_SEND_PKT

DESCRIPTION
  Function for building and sending out EAP packets of a specific
  type   
DEPENDENCIES
  None

PARAMETERS
  eap_handle  - handle of current eap instance
  pb_ctx   - the current parser builder instance.
  response - response structure to populate with data
    
RETURN VALUE
  None

SIDE EFFECTS
  The last transmitted packet is obtained. The last tx and rx messages
  are dropped as a side effect before the new msg is transmitted out.
===========================================================================*/

void
eap_sim_aka_pb_build_and_send_pkt
(
  eap_handle_type      eap_handle,
  eap_sim_aka_pb_ctx  *pb_ctx,
  void                *response,
  eap_sim_aka_msg_type msg_type
);


/*===========================================================================
                             Notification Manager
===========================================================================*/

/*===========================================================================
FUNCTION EAP_SIM_AKA_NM_PROCESS_PKT

DESCRIPTION
   This function is used by the EAP-SIM/AKA Core when a notification packet 
   is received.
   This call will have impact on the current core's state. The notification 
   manager should also be informed if the core is expecting a notification 
   of success at this moment. 

DEPENDENCIES
  None

PARAMETERS
  sim_aka_inst: - the current eap sim/aka instance 
  notification_req: the value of the notification attribute as received from 
                    the parser
  
  
RETURN VALUE
  True if success.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean 
eap_sim_aka_nm_process_pkt 
(
  eap_sim_aka_ctx*    sim_aka_inst,
  eap_sim_aka_req_notification_type* notification_req  
);


/*===========================================================================
                      Fast Re-authentication Manager
===========================================================================*/

/*===========================================================================
FUNCTION EAP_SIM_AKA_FRM_PROCESS_PKT

DESCRIPTION
  This function can be used by the SIM/AKA Core Implementation in order to 
  process an fast re-authentication request received from the server. 

DEPENDENCIES
  None

PARAMETERS
  sim_aka_inst - the sim/aka instance treating this pkt
  reauth_req   - the content of the re-authentication message
  
RETURN VALUE
  True if success.

SIDE EFFECTS
  None
===========================================================================*/
boolean 
eap_sim_aka_frm_process_pkt
(
  eap_sim_aka_ctx*               sim_aka_inst,
  eap_sim_aka_req_reauth_type*   reauth_req
);

/*===========================================================================
                             Error Manager
===========================================================================*/

/*===========================================================================
FUNCTION EAP_SIM_AKA_EM_PROCESS_ERROR

DESCRIPTION
  This function can be used by any entity in the SIM/AKA Implementation in 
  order to notify the authentication server that an error was encountered and 
  the authentication process is being failed by the client.  

DEPENDENCIES
  None

PARAMETERS
   cl_err: the client error code 
   sim_aka_inst: - the sim/aka instance treating this pkt
  
RETURN VALUE
  True if success.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean 
eap_sim_aka_em_process_error
(
  eap_sim_aka_ctx*                  sim_aka_inst,
  eap_sim_aka_clienterr_enum_type       cl_err,
  eap_sim_aka_failure_reason_enum_type  reason
);



/*===========================================================================
  Output functions for the common EAP-SIM/AKA messages
===========================================================================*/
/*===========================================================================
FUNCTION EAP_SIM_AKAI_SEND_OUT_REAUTH_RESPONSE

DESCRIPTION
  Function called by the FRM to send out a reauth response packet

DEPENDENCIES
  None

PARAMETERS
  sim_aka_inst: - the current eap sim/aka instance
  counter_too_small - if case
  counter - the counter extracted from the reauth package
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void
eap_sim_akai_send_out_reauth_response
(
  eap_sim_aka_ctx*  sim_aka_inst,
  boolean           counter_small_present,
  uint16            counter
);

/*===========================================================================
FUNCTION EAP_SIMI_SEND_OUT_NOTIFICATION_RESPONSE

DESCRIPTION
  Function called by the NM to send out a notification response packet

DEPENDENCIES
  None

PARAMETERS
  sim_aka_inst - the current eap sim/aka instance
  mac_present - will MAC be present
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void
eap_sim_akai_send_out_notification_response
(
  boolean           mac_present,
  eap_sim_aka_ctx*  sim_aka_inst,
  boolean           counter_present
);

/*=========================================================================
  Functions for processing encrypted fields
===========================================================================*/
/*===========================================================================
FUNCTION  EAP_SIM_AKAI_DECIPHER_CHALLENGE_DATA

DESCRIPTION
  Called by EAP-SIM/AKA Core to decipher the encrypted fields of a 
  SIM/AKA Challenge

DEPENDENCIES
  None

PARAMETERS
  sim_aka_inst - the current eap sim/aka instance
  
RETURN VALUE
  TRUE if successful

SIDE EFFECTS
  None
  
===========================================================================*/
boolean
eap_sim_akai_decipher_challenge_data
(
  eap_sim_aka_ctx*  sim_aka_inst
);

/*===========================================================================
FUNCTION EAP_SIM_AKAI_UNPACK_ENCR_CHALLENGE

DESCRIPTION
  Function called by the AKA Core to unpack the encr data field

DEPENDENCIES
  None

PARAMETERS
  encr_msg  - the encrypted message as it was received
  encr_str  - output parameter
  pb_parent_ctx - parser builder context of the calling instance
  
RETURN VALUE
  TRUE if successful

SIDE EFFECTS
  None
  
===========================================================================*/
boolean 
eap_sim_akai_unpack_encr_challenge
( 
  dsm_item_type*                        encr_msg,
  eap_sim_aka_req_challenge_encr_type*  encr_str,
  const eap_sim_aka_pb_ctx*                   pb_parent_ctx
);

/*===========================================================================
FUNCTION EAP_SIM_AKAI_UNPACK_ENCR_NOTIFICATION

DESCRIPTION
  Function called by the AKA Core to unpack the encr data field

DEPENDENCIES
  None

PARAMETERS
  encr_msg  - the encrypted message as it was received
  encr_str  - output parameter
  pb_parent_ctx - parser builder context of the calling instance
  
RETURN VALUE
  TRUE if successful

SIDE EFFECTS
  None
  
===========================================================================*/
boolean 
eap_sim_akai_unpack_encr_notification
( 
  dsm_item_type*                           encr_msg,
  eap_sim_aka_req_notification_encr_type*  encr_str,
  const eap_sim_aka_pb_ctx*                pb_parent_ctx
);

/*===========================================================================
FUNCTION EAP_SIM_AKAI_UNPACK_ENCR_REAUTH

DESCRIPTION
  Function called by the FRM to unpack the encr data field

DEPENDENCIES
  None

PARAMETERS
  encr_msg  - the encrypted message as it was received
  encr_str  - output parameter
  pb_parent_ctx - parser builder context of the calling instance
  
RETURN VALUE
  TRUE if successful

SIDE EFFECTS
  None
  
===========================================================================*/
boolean
eap_sim_akai_unpack_encr_reauth
( 
  dsm_item_type*                     encr_msg,
  eap_sim_aka_req_reauth_encr_type*  encr_str,
  const eap_sim_aka_pb_ctx*                pb_parent_ctx
);

/*===========================================================================
FUNCTION  EAP_SIMI_VALIDATE_NOTIFICATION_MAC

DESCRIPTION
  Called by the Notification Manager to validate the MAC for a request.

DEPENDENCIES
  None

PARAMETERS
  sim_aka_inst - pointer to the current SIM/AKA instance
  
RETURN VALUE
  TRUE if successful

SIDE EFFECTS
  None
  
===========================================================================*/
boolean
eap_sim_akai_validate_notification_mac
(
  eap_sim_aka_ctx*   sim_aka_inst
);

/*===========================================================================
FUNCTION EAP_SIM_AKAI_COMPUTE_REAUTH_KEYS

DESCRIPTION
  Called by the Fast Re-auth manager to compute the MSK/EMSK

DEPENDENCIES
  None

PARAMETERS
  sim_aka_inst - pointer to the current SIM/AKA instance
  id           - the identity
  nonce_s      - the nonce value received from the server 
    
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void
eap_sim_akai_compute_reauth_keys
(
  eap_sim_aka_ctx*          sim_aka_inst,
  uint8*                    nonce_s
);

/*===========================================================================
FUNCTION EAP_AKA_PRIMEI_COMPUTE_REAUTH_KEYS

DESCRIPTION
  Called by the Fast Re-auth manager to compute the TEKs and MSK/EMSK

DEPENDENCIES
  None

PARAMETERS
  sim_aka_inst - pointer to the current SIM/AKA instance
  nonce_s      - the nonce value received from the server 
    
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void
eap_sim_aka_primei_compute_reauth_keys
(
  eap_sim_aka_ctx*          sim_aka_inst,
  uint8*                    nonce_s
);

/*===========================================================================
  Utility functions
===========================================================================*/
/*===========================================================================
FUNCTION  EAP_SIM_AKA_CONVERT_IDENTIY

DESCRIPTION
  Called by EAP-SIM/AKA Core to convert an identity of eap_sim_aka to an IME 
  identity

DEPENDENCIES
  None

PARAMETERS
  sim_aka_id - the id to be converted
  
RETURN VALUE
  the resulting IME id

SIDE EFFECTS
  None
  
===========================================================================*/
eap_sim_aka_ime_id_enum_type 
eap_sim_aka_convert_identity
( 
  eap_sim_aka_identity_enum_type sim_aka_id
);

/*===========================================================================
FUNCTION  EAP_SIM_AKAI_ADD_REALM

DESCRIPTION
  Utility function used to add the current realm to a given id ( used for
  pseudonym - as this type of id needs to inherit the current realm )

DEPENDENCIES
  None

PARAMETERS
  id          - pointer to the new id 
  current_id  - pointer to the current id 
  
RETURN VALUE
  the resulting IME id

SIDE EFFECTS
  None
  
===========================================================================*/
void
eap_sim_akai_add_realm
(
  eap_sim_aka_ime_id_type*  id,
  eap_sim_aka_ime_id_type*  current_id
);

/*===========================================================================
FUNCTION  EAP_SIM_AKA_RESULT_IND

DESCRIPTION
  Called by the core to do the unified processing of sim/aka result ind 

DEPENDENCIES
  None

PARAMETERS
  sim_aka_inst - current sim/aka instance 
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void
eap_sim_aka_result_ind
(
  eap_sim_aka_ctx*  sim_aka_inst, 
  eap_result_enum_type  result
);

/*===========================================================================
FUNCTION  EAP_SIM_AKA_SUPP_RESULT_IND

DESCRIPTION
  Called by the core to do the unified processing of SIM/AKA supplementary
  info

DEPENDENCIES
  None

PARAMETERS
  sim_aka_inst - current sim/aka instance
  result       - the authentication result
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void
eap_sim_aka_supp_result_ind
(
  eap_sim_aka_ctx*      sim_aka_inst, 
  eap_result_enum_type  result
);

#ifdef EAP_SIM_AKA_KEY_DEBUG_ENABLED
/*===========================================================================
FUNCTION PRINT_QUOT_VALUES

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS
  None.
RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void 
print_quot_values
(
  uint8  quot,
  uint8* val_ptr
);
#endif /* EAP_SIM_AKA_KEY_DEBUG_ENABLED */

#ifdef EAP_SIM_AKA_KEY_DEBUG_ENABLED
/*===========================================================================
FUNCTION PRINT_MOD_VALUES

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS
  None.
RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void 
print_mod_values
(
  uint8  mod,
  uint8* val_ptr
);
#endif /* EAP_SIM_AKA_KEY_DEBUG_ENABLED */

#ifdef EAP_SIM_AKA_KEY_DEBUG_ENABLED
/*===========================================================================
FUNCTION PRINT_BYTE_ARRAY

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS
  None.
RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void 
print_byte_array
(
  uint16  len,
  uint8*  byte_ptr
);
#endif /* EAP_SIM_AKA_KEY_DEBUG_ENABLED */

#endif /* FEATURE_DATA_PS_EAP */
#endif /* PS_EAP_SIM_AKAI_H */

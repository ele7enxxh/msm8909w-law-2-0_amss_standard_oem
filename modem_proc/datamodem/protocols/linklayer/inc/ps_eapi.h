
#ifndef PS_EAPI_H
#define PS_EAPI_H


/*===========================================================================


   E X T E N S I B L E   A U T H E N T I C A T I O N   P R O T O C O L (EAP)
                    I N T E R N A L   H E A D E R   F I L E
                   
DESCRIPTION


 This is the EAP internal header file and should not be included by any
 module other than EAP modules.
 
 External header file is ps_eap.h
 

REFERENCES
 
 RFC 3748 : Primary RFC describing EAP
 
 RFC 2716 : PPP EAP TLS Authentication Protocol
 

OVERVIEW

 Extensible Authentication Protocol is defined in RFC 3748. The primary use
 of this implementation is in the WLAN security functionality as part of 
 WPA/802.11i support. To that end, this EAP implementation attempts to 
 provide the functionality relevant to the WLAN security as a first objective
 and be fully RFC compliant as a secondary, less important goal.
 
REQUIREMENTS

  - RFC 3748 compliant
  - Support multiple EAP instances per EAP client; e.g. a single 802.1x 
    instance may create two or more EAP instances.
  - Support both peer and authenticator roles in the future (currently, only
    peer is supported)
    
THEORY OF OPERATION

 The EAP services are expected to be run in PS task context. It is upto the
 caller to ensure serialization.
 
 - EAP services are initialized at powerup. 
 - Client creates an instance of EAP and configures it prior to
   authentication; an example of a client is 802.1x
 - As an EAP packet is received, the corresponding method is `brought up'; 
   e.g. eap_tls_input() activates EAP TLS
   
LAYERING

 The layering structure is based on the recommendation of the RFC.
 
 The EAP layer is responsible for receiving packets from the lower layer
 (e.g. 802.1x client) and determining whether the packet should be forwarded
 to the peer or authenticator.
 It also handles duplicate detection and retransmission. Since EAP is a lock-
 step protocol, this layer only caches the last packet transmitted and the
 corresponding Identifier.
 
 The Peer layer holds the state machine to conform to the RFC mandated 
 behavior. Note that the state machine has not been proposed by the RFC.
 This implementation relieves methods of passing down the appropriate type
 in outgoing packets; peer layer enforces the method sequences as per RFC.
 
 EAP methods:
 Identity, Notification and Nak are special method types provided internally
 by the EAP implementation. 
 Other EAP methods including MD5 and TLS are supported through the external 
 interfaces.
 
 
         +-+-+-+-+-+-+-+-+-+-+-+-+  
         |           |           |  
         | EAP method| EAP method|  
         | Type = X  | Type = Y  |  
         |           |           |  
         +-+-+-+-+-+-+-+-+-+-+-+-+  
         |                       |  
         |     EAP Peer layer    |  
         |                       |  
         +-+-+-+-+-+-+-+-+-+-+-+-+  
         |                       |  
         |     EAP layer         |  
         |                       |  
         +-+-+-+-+-+-+-+-+-+-+-+-+  
         
 EAP external:
 Provides methods to initialize EAP services

 

Copyright (c) 2005-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_eapi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/26/12    fn      Added support for multiple client id cache
10/26/12    fn      Move AT_KDF_is_require NV item to ps_utils.h
05/09/12    op      Added support for the additional meta_identity
10/06/11    scb     Number of EAP instances increased by 1 for ATCOP
08/26/11    op      Added changes to obtain supplementary SIM/AKA auth info
02/24/11    ash     Cleaning up global variables for Q6 free floating effort.
08/31/09    mga     Merged from eHRPD branch
03/06/09    scb     Added changes for memory reduction initiative.
10/21/08    scb     Added function to obtain the reauth id from SFS/EFS
08/25/06    lti     Added function for setting the resp required flag
08/25/06    lti     Added result cback registration
04/14/06    lti     Moved eap_get_identity and eap_get_identifier to ps_eap.h
                    Moved eap_code_enum_type to ps_eap.h
02/27/06    lyr     WLAN 3.0 support for additional auth schemes
03/29/05    lyr     Created module.

===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"

#include "customer.h"
#ifdef FEATURE_DATA_PS_EAP
#include "dsm.h"

#include "ps_utils.h"
#include "ps_svc.h"

#include "ps_eap.h"


/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Maximum pre-master key size - 256 bits (32 bytes)
---------------------------------------------------------------------------*/
#define EAP_MAX_PRE_MASTER_KEY_SIZE 256

/*---------------------------------------------------------------------------
  Maximum permissible EAP instances
---------------------------------------------------------------------------*/
#define EAP_MAX_INSTANCES 16

/*---------------------------------------------------------------------------
  Lower layer info block - this information is used by the EAP layer to 
  communicate with the lower layer
---------------------------------------------------------------------------*/
typedef struct
{
  eap_trp_tx_cback_type          trp_tx_f_ptr;        /* Transport tx fn ptr   */
  void                          *trp_tx_user_data;    /* User data for tx fn   */
  eap_result_ind_cback_type      result_ind_f_ptr;    /* auth completed cback  */
  void                          *result_ind_user_data;/* auth cback user data  */
  eap_client_ind_cb_type         eap_client_start_ind_cb;/*ID stage success cb */
  void                          *start_ind_user_data; /* start_ind cback user
                                                         data  */
  eap_supp_result_ind_cback_type supp_result_ind_f_ptr;/* supp auth comp cback */
  void                          *supp_result_ind_user_data;/* supp auth cback 
                                                              user data  */
} eap_trp_info_type;


/*---------------------------------------------------------------------------
  Structure to hold all EAP layer init info
---------------------------------------------------------------------------*/
typedef struct
{
  eap_handle_type     eap_handle;        /* Handle to parent EAP instance  */
  eap_trp_info_type   eap_trp_info;      /* Lower layer (e.g. 802.1x) info */
  boolean             raw_mode;          /* pass-through?                  */
  eap_role_enum_type  eap_role;          /* Peer or Server mode            */
} eap_eap_info_type;


/*---------------------------------------------------------------------------
  Structure to hold all Peer layer init info
---------------------------------------------------------------------------*/
typedef struct
{
  eap_handle_type    eap_handle;         /* handle to parent EAP instance  */
  uint32             eap_auth_prot_mask; /* Supported auth protocol mask   */
  eap_identity_type  eap_identity;       /* EAP peer identity              */
  boolean            raw_mode;           /* pass-through?                  */
  boolean            authenticator_rsp_required; /* Wait for server result?*/
} eap_peer_info_type;

/*---------------------------------------------------------------------------
  Structure to hold all Server layer init info
---------------------------------------------------------------------------*/
typedef struct
{
  eap_handle_type    eap_handle;         /* handle to parent EAP instance  */
  uint32             eap_auth_prot_mask; /* Supported auth protocol mask   */
  eap_identity_type  eap_identity;       /* EAP peer identity              */
  boolean            raw_mode;           /* pass-through?                  */
  eap_client_timer_cback_type timer_f_ptr;     /*Client timer call back    */
  void                       *timer_user_data; /*user data for timer cback */
} eap_server_info_type;

/*---------------------------------------------------------------------------
  The following define the handles for the EAP layer, Peer layer, Server layer
  and methods  
---------------------------------------------------------------------------*/
typedef sint15 eap_eap_handle_type;

typedef sint15 eap_peer_handle_type;

typedef sint15 eap_server_handle_type;

/*---------------------------------------------------------------------------
  Control block for the EAP instance - this has information about the Peer
  layer, EAP layer and the corresponding lower layer for each instance
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                     is_assigned;         /* Is instance in use?  */

  eap_eap_handle_type         eap_lyr_handle;      /* EAP layer handle     */

  eap_peer_handle_type        peer_lyr_handle;     /* Peer layer handle    */

  eap_server_handle_type      server_lyr_handle;   /* Server layer handle  */

  /* EAP simply sets this and gets this field when requested by client     */
  void                       *auth_mgr;            /* Auth mgr info        */

  /*Meta info - method specific info*/
  void                       *eap_instance_meta_info_ptr; 

  /*Support eap extended method on this instance*/
  boolean                     support_eap_ext;
} eap_cb_type;

/*---------------------------------------------------------------------------
  Prototype for the method-specific supplementary result callback function,
  which each method has to register at power-up
---------------------------------------------------------------------------*/
typedef void (*eap_method_supp_result_cback_type)
(
 eap_handle_type       handle,
 eap_result_enum_type  result
);

/*---------------------------------------------------------------------------
  Enumberation for EAP client spec settings
---------------------------------------------------------------------------*/
typedef enum{
 EAP_CLIENT_MIN_SETTING = 0,
 EAP_CLIENT_REAUTH_REALM_SETTING,
 EAP_CLIENT_MAX_SETTING = 31 /*Limited by size of uint32*/
}eap_client_enable_spec;

/*---------------------------------------------------------------------------
  EAP alternative identities used by methods 
---------------------------------------------------------------------------*/
/** Re-authentication ID. */
extern eap_identity_type* eap_reauth_id[EAP_MAX_INSTANCES];
/** Pseudonym ID. */
extern eap_identity_type* eap_pseudonym_id[EAP_MAX_INSTANCES];

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  EAP layer API
---------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION EAP_EAP_INIT

DESCRIPTION
  Initializes the EAP layer structures.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Initialize the EAP layer control blocks.

===========================================================================*/
void eap_eap_init
(
  void
);


/*===========================================================================

FUNCTION EAP_EAP_CREATE_INSTANCE

DESCRIPTION
  Initializes and returns handle to EAP layer instance.

DEPENDENCIES
  EAP layer must have been initialized

RETURN VALUE
  Handle to EAP layer.

SIDE EFFECTS
  None.

===========================================================================*/
eap_eap_handle_type eap_eap_create_instance
(  
  eap_eap_info_type *eap_lyr_info
);


/*===========================================================================

FUNCTION EAP_EAP_DELETE_INSTANCE

DESCRIPTION
   Shuts down the EAP layer instance.

DEPENDENCIES
  EAP layer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void eap_eap_delete_instance
(  
  eap_eap_handle_type eap_lyr_handle
);


/*===========================================================================

FUNCTION EAP_EAP_INPUT

DESCRIPTION
  Rx function for EAP layer. Parse the EAP header and calls the peer/auth.
  Layer's input function if necessary.

DEPENDENCIES
  EAP layer instance must have been created

RETURN VALUE
  None

SIDE EFFECTS
  Calls peer/auth. RX function.

===========================================================================*/
void eap_eap_input
( 
  eap_eap_handle_type   eap_lyr_handle,
  dsm_item_type   **sdu
);


/*===========================================================================
FUNCTION EAP_EAP_XMIT_PKT

DESCRIPTION
  Tx function for the EAP layer. Adds EAP header and send the packet on
  the associated link layer.

DEPENDENCIES
  EAP layer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  Sends the packet on associated link.

===========================================================================*/
void eap_eap_xmit_pkt
(
   eap_handle_type      eap_handle,
   eap_code_enum_type   code,
   dsm_item_type      **sdu
); 

/*===========================================================================
FUNCTION EAP_EAP_REXMIT_REQ_PKT

DESCRIPTION
  Re-transmits a EAP request packet

PARAMETERS
  eap_handle - EAP layer handle

DEPENDENCIES
  EAP layer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void eap_eap_rexmit_req_pkt
(
   eap_handle_type      eap_handle
);

/*===========================================================================

FUNCTION EAP_EAP_AUTH_COMPLETE_IND

DESCRIPTION
  This function is called by the peer layer to notify EAP layer that the
  authentication result indication AS WELL as an EAP Success/Failure have
  been received.

DEPENDENCIES
  EAP layer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  Calls client's authentication completed callback.

===========================================================================*/
void eap_eap_auth_complete_ind 
(
  eap_handle_type       eap_handle,
  eap_result_enum_type  result,
  uint8                *pre_master_key,
  uint16                pre_master_key_len
);

/*===========================================================================

FUNCTION EAP_EAP_SUPP_AUTH_COMPLETE_IND

DESCRIPTION
  This function is called by the peer layer to notify EAP layer of the
  supplementary authentication info.

DEPENDENCIES
  EAP layer instance must have been created.
 
  NOTE: If supp_result_ind_f_ptr is registered, the EAP clients should not
  delete the EAP instance when auth complete ind is called

RETURN VALUE
  None.

SIDE EFFECTS
  Calls client's supp authentication completed callback.

===========================================================================*/
void eap_eap_supp_auth_complete_ind
(
  eap_handle_type                  eap_handle,
  eap_result_enum_type             result,
  eap_sim_aka_supp_auth_mask_type  supp_auth_info_mask,
  uint16                           supp_auth_info_size,
  void                            *supp_auth_info_ptr
);

/*===========================================================================

FUNCTION EAP_EAP_SUPP_RESULT_F_IND_PTR_IS_VALID

DESCRIPTION
  This function is called to verify if the client has registered for the
  supplementary result indication

DEPENDENCIES
  None.

RETURN VALUE
  True:  supp_result_ind_f_ptr is valid
  False: supp_result_ind_f_ptr is invalid

SIDE EFFECTS
  None.

===========================================================================*/
boolean eap_eap_supp_result_f_ind_ptr_is_valid
(
  eap_handle_type eap_handle
);

/*===========================================================================

FUNCTION EAP_EAP_GET_IDENTIFIER

DESCRIPTION
  This function is called by a method to request the identifier of the last EAP
  packet

DEPENDENCIES
  EAP instance must have been created

RETURN VALUE
  Last EAP pkt identifier.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 eap_eap_get_identifier
(
  eap_handle_type eap_handle
);

/*---------------------------------------------------------------------------
  Peer layer API
---------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION EAP_PEER_INIT

DESCRIPTION
  Initializes the Peer layer control blocks.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Initialize the Peer layer control blocks.

===========================================================================*/
void eap_peer_init
(
  void
);

/*===========================================================================
FUNCTION EAP_PEER_SET_AUTHENTICATOR_RSP_REQUIRED

DESCRIPTION
  Offers support for dynamically setting the authenticator_rsp_required flag

DEPENDENCIES

  
RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void 
eap_peer_set_authenticator_rsp_required
(
  eap_peer_handle_type     peer_lyr_handle,
  boolean                  authenticator_rsp_required 
);

/*===========================================================================

FUNCTION EAP_PEER_METHOD_INPUT_CBACK_REG

DESCRIPTION
  Registers the method-specific input function with Peer
  
  NOTE: This function should only be called if the method specifies that
  it supports the Peer role

DEPENDENCIES
  This must be done once at powerup as all EAP instances will use the same
  input function for a given method.
  

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void eap_peer_method_input_cback_reg
(
  eap_method_input_cback_type cback,
  eap_method_enum_type        method
);


/*===========================================================================

FUNCTION EAP_PEER_METHOD_CLOSE_CBACK_REG

DESCRIPTION
  Registers the method-specific close function with Peer
  
  NOTE: This function should only be called if the method specifies that
  it supports the Peer role

DEPENDENCIES
  This must be done once at powerup as all EAP instances will use the same
  close function for a given method.
  

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void eap_peer_method_close_cback_reg
(
  eap_method_close_cback_type cback,
  eap_method_enum_type        method
);

/*===========================================================================

FUNCTION EAP_PEER_METHOD_RESULT_CBACK_REG

DESCRIPTION
  Registers the method-specific result function with Peer
  
  NOTE: This function should only be called if the method specifies that
  it supports the Peer role

DEPENDENCIES
  This must be done once at powerup as all EAP instances will use the same
  result function for a given method.
  

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void eap_peer_method_result_cback_reg
(
  eap_method_result_cback_type cback,
  eap_method_enum_type        method
);

/*===========================================================================

FUNCTION EAP_PEER_METHOD_SUPP_RESULT_CBACK_REG

DESCRIPTION
  Registers the method-specific supplementary result function with Peer
  
  NOTE: This function should only be called if the method specifies that
  it supports the Peer role

DEPENDENCIES
  This must be done once at powerup as all EAP instances will use the same
  close function for a given method.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void eap_peer_method_supp_result_cback_reg
(
  eap_method_supp_result_cback_type cback,
  eap_method_enum_type              method
);

/*===========================================================================

FUNCTION EAP_METHOD_SUPP_RESULT_CBACK_REG

DESCRIPTION
  Registers the method-specific supplementary result function with EAP
  services

DEPENDENCIES
  This must be done once at powerup as all EAP instances will use the same
  result function for a given method.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void eap_method_supp_result_cback_reg
(
  eap_method_supp_result_cback_type cback,
  eap_method_enum_type              method,
  eap_role_enum_type                role
);

/*===========================================================================

FUNCTION EAP_PEER_CREATE_INSTANCE

DESCRIPTION
  Initializes Peer layer instance and returns handle to this instance.

DEPENDENCIES
  Peer must have been initialized

RETURN VALUE
  Handle to Peer instance.

SIDE EFFECTS
  None.

===========================================================================*/
eap_peer_handle_type eap_peer_create_instance
(  
  eap_peer_info_type *peer_lyr_info
);


/*===========================================================================

FUNCTION EAP_PEER_DELETE_INSTANCE

DESCRIPTION
   Shuts down the Peer layer instance.

DEPENDENCIES
  EAP Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void eap_peer_delete_instance
(  
  eap_peer_handle_type peer_lyr_handle
);


/*===========================================================================

FUNCTION EAP_PEER_INPUT

DESCRIPTION
  Rx function for EAP Peer layer. Parse the Peer layer header and calls the
  method's input function if necessary.

DEPENDENCIES
  EAP peer instance must have been created

RETURN VALUE
  None

SIDE EFFECTS
  Calls EAP method's RX function.

===========================================================================*/
void eap_peer_input
( 
  eap_handle_type        eap_lyr_handle,
  eap_code_enum_type   code,
  dsm_item_type   **sdu
);


/*===========================================================================
FUNCTION EAP_PEER_XMIT_PKT

DESCRIPTION
  Tx function for the Peer layer. Adds peer header and send the packet to the
  EAP layer.

DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void eap_peer_xmit_pkt
(
   eap_peer_handle_type   peer_lyr_handle,
   dsm_item_type        **sdu
); 


/*===========================================================================

FUNCTION EAP_PEER_AUTH_COMPLETE_IND

DESCRIPTION
  This function is called by the authentication protocol to notify Peer of
  the result of the authentication process.
  
  The Peer layer still waits for either a Success/Failure packet or a timeout
  before declaring an authentication indication to EAP layer (and therefore
  lower layer)

DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void eap_peer_auth_complete_ind 
(
  eap_peer_handle_type     peer_lyr_handle,
  eap_result_enum_type     result,
  uint8                   *pre_master_key,
  uint16                   pre_master_key_len
);

/*===========================================================================

FUNCTION EAP_PEER_SUPP_AUTH_COMPLETE_IND

DESCRIPTION
  This function is called by the authentication protocol to notify Peer of
  the supplementary info of the authentication process.

DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void eap_peer_supp_auth_complete_ind 
(
  eap_peer_handle_type             peer_lyr_handle,
  eap_result_enum_type             result,
  eap_sim_aka_supp_auth_mask_type  supp_auth_info_mask,
  uint16                           supp_auth_info_size,
  void                            *supp_auth_info_ptr
);

/*===========================================================================

FUNCTION EAP_SUPP_AUTH_COMPLETE_IND

DESCRIPTION
  This function is called by the authentication protocol to notify EAP of the
  supplementary information of the authentication process.

DEPENDENCIES
  EAP instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  Calls client's supplementary authentication completed callback.

===========================================================================*/
void eap_supp_auth_complete_ind 
(
  eap_handle_type                  eap_handle,
  eap_result_enum_type             result,
  eap_sim_aka_supp_auth_mask_type  supp_auth_info_mask,
  uint16                           supp_auth_info_size,
  void                            *supp_auth_info_ptr
);

/*===========================================================================

FUNCTION EAP_PEER_TRANSACTION_CLOSING

DESCRIPTION
  This function is called by SIM and AKA modules to move the peer to
  CLOSING state whereupon it can process only SUCCESS, FAILURE and
  Notification packets.

DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void eap_peer_transaction_closing
(
  eap_peer_handle_type  peer_lyr_handle
);

/*===========================================================================

FUNCTION EAP_PEER_GET_IDENTITY

DESCRIPTION
  This function is called by the Identity method to retrieve the identity
  of the peer

DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void eap_peer_get_identity
(
  eap_peer_handle_type  peer_lyr_handle,
  eap_identity_type*    identity
);


/*===========================================================================

FUNCTION EAP_PEER_GET_AUTH_PROT

DESCRIPTION
  This function is called by the Nak method to identify the supported
  authentication protocols

DEPENDENCIES
  Peer instance must have been created

RETURN VALUE
  Number of protocols supported
  0 : no protocols supported
  
  List of protocols supported is provided in buf

SIDE EFFECTS
  None.

===========================================================================*/
uint16 eap_peer_get_auth_prot
(
  eap_peer_handle_type  peer_lyr_handle,
  uint8           *buf
);

/*===========================================================================

FUNCTION EAP_PEER_GET_REAUTH_ID

DESCRIPTION
  Calls the UME layer function with the appropriate mode depending upon
  the method specified in the eap_handle
  
DEPENDENCIES
  None.

RETURN VALUE
  The reauthentication id along with the length if present, -1 otherwise.
  
SIDE EFFECTS
  None.

===========================================================================*/
int eap_peer_get_reauth_id
(
  eap_handle_type                        eap_handle,
  uint8*                                 data,
  uint16*                                max_data_len
);

/*===========================================================================

FUNCTION EAP_PEER_GET_PSEUDONYM_ID

DESCRIPTION
  Calls the UME layer function with the appropriate mode depending upon
  the method specified in the eap_handle
  
DEPENDENCIES
  None.

RETURN VALUE
  The pseudonym id along with the length if present, -1 otherwise.
  
SIDE EFFECTS
  None.

===========================================================================*/
int eap_peer_get_pseudonym_id
(
  eap_handle_type                        eap_handle,
  uint8*                                 data,
  uint16*                                max_data_len
);

/*---------------------------------------------------------------------------
  Server layer API
---------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION EAP_SERVER_INIT

DESCRIPTION
  Initializes the Server layer control blocks.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void eap_server_init
(
  void
);

/*===========================================================================
FUNCTION EAP_SERVER_CREATE_INSTANCE

DESCRIPTION
  Initializes Server layer instance and returns handle to this instance.

DEPENDENCIES
  Peer must have been initialized

RETURN VALUE
  Handle to Peer instance.

SIDE EFFECTS
  None.
===========================================================================*/
eap_server_handle_type eap_server_create_instance
(  
  eap_server_info_type *server_lyr_info
);

/*=============z==============================================================
FUNCTION EAP_SERVER_XMIT_PKT

DESCRIPTION
  Tx function for the Server layer. Adds Server header and send the packet to 
  the EAP layer.

DEPENDENCIES
  Server instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void eap_server_xmit_pkt
(
   eap_peer_handle_type   peer_lyr_handle,
   dsm_item_type        **sdu
); 

/*===========================================================================
FUNCTION EAP_SERVER_INPUT

DESCRIPTION
  Rx function for EAP Server layer. Parse the Server layer header and call the
  method's input function if necessary.

DEPENDENCIES
  EAP Server instance must have been created

RETURN VALUE
  None

SIDE EFFECTS
  Calls EAP method's RX function.
===========================================================================*/
void eap_server_input
( 
  eap_handle_type      eap_handle,
  eap_code_enum_type   code,
  dsm_item_type      **sdu
);

/*===========================================================================
FUNCTION EAP_SERVER_IDENTITY_REQ

DESCRIPTION
  Function to send Identity request packet to the peer

DEPENDENCIES
  None.

PARAMETERS
  eap_handle - EAP handle

RETURN VALUE
  0 - Success
 -1 - Failure

SIDE EFFECTS
  None
===========================================================================*/
int32 eap_server_identity_req
(
  eap_handle_type eap_handle
);

/*===========================================================================
FUNCTION EAP_SERVER_METHOD_INPUT_CBACK_REG

DESCRIPTION
  Registers the method-specific input function with Authenticator
  
  NOTE: This function should only be called if the method specifies that
  it supports the Authenticator role

DEPENDENCIES
  This must be done once at powerup as all EAP instances will use the same
  input function for a given method.
  

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void eap_server_method_input_cback_reg
(
  eap_method_input_cback_type cback,
  eap_method_enum_type        method
);

/*===========================================================================
FUNCTION EAP_SERVER_METHOD_RESULT_CBACK_REG

DESCRIPTION
  Registers the method-specific result function with Server
  NOTE: This function should only be called if the method specifies that
  it supports the Server role

DEPENDENCIES
  This must be done once at powerup as all EAP instances will use the same
  result function for a given method.
  
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void eap_server_method_result_cback_reg
(
  eap_method_result_cback_type  cback,
  eap_method_enum_type          method
);

/*===========================================================================
FUNCTION EAP_SERVER_METHOD_SUPP_RESULT_CBACK_REG

DESCRIPTION
  Registers the method-specific supplementary result function with Server

  NOTE: This function should only be called if the method specifies that
  it supports the Server role

DEPENDENCIES
  This must be done once at powerup as all EAP instances will use the same
  result function for a given method.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void eap_server_method_supp_result_cback_reg
(
  eap_method_supp_result_cback_type  cback,
  eap_method_enum_type               method
);

/*===========================================================================
FUNCTION EAP_SERVER_METHOD_CLOSE_CBACK_REG

DESCRIPTION
  Registers the method-specific close function with Server
  
  NOTE: This function should only be called if the method specifies that
  it supports the Server role

DEPENDENCIES
  This must be done once at powerup as all EAP instances will use the same
  close function for a given method.
  

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void eap_server_method_close_cback_reg
(
  eap_method_close_cback_type cback,
  eap_method_enum_type        method
);

/*===========================================================================
FUNCTION EAP_SERVER_AUTH_COMPLETE_IND

DESCRIPTION
  This function is called by the authentication protocol to notify Server of
  the result of the authentication process.
  The Server layer sends out Success/Failure packet based on the result & 
  also notifies the lower layer (eg: LAN 1.x)

DEPENDENCIES
  Server instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void eap_server_auth_complete_ind 
(
  eap_server_handle_type   server_lyr_handle,
  eap_result_enum_type     result,
  uint8                   *pre_master_key,
  uint16                   pre_master_key_len
);

/*===========================================================================
FUNCTION EAP_SERVER_SUPP_AUTH_COMPLETE_IND

DESCRIPTION
  This function is called by the authentication protocol to notify Server of
  the supplementary info of the authentication process.

DEPENDENCIES
  Server instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void eap_server_supp_auth_complete_ind 
(
  eap_server_handle_type           server_lyr_handle,
  eap_result_enum_type             result,
  eap_sim_aka_supp_auth_mask_type  supp_auth_info_mask,
  uint16                           supp_auth_info_size,
  void                            *supp_auth_info_ptr
);

/*---------------------------------------------------------------------------
  EAP Internal Method APIs
---------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION EAP_IDENTITY_INPUT

DESCRIPTION
  Rx function for EAP Identity method. 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Synchronously queues packet back to Peer layer if necessary.

===========================================================================*/
void eap_identity_input
( 
  eap_handle_type   eap_handle,
  dsm_item_type   **sdu
);


/*===========================================================================

FUNCTION EAP_NOTIFICATION_INPUT

DESCRIPTION
  Rx function for EAP Notification method. 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Synchronously queues packet back to Peer layer if necessary.

===========================================================================*/
void eap_notification_input
( 
  eap_handle_type   eap_handle,
  dsm_item_type   **sdu
);


/*===========================================================================

FUNCTION EAP_NAK_INPUT

DESCRIPTION
  Rx function for EAP NAK method. 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Synchronously queues packet back to Peer layer if necessary.

===========================================================================*/
void eap_nak_input
( 
  eap_handle_type   eap_handle,
  dsm_item_type   **sdu
);


/*---------------------------------------------------------------------------
  EAP Manager internal APIs
---------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION EAP_MD5_INPUT

DESCRIPTION
  Function for EAP MD5 method. 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Synchronously queues packet back to Peer layer if necessary.

===========================================================================*/
void eap_md5_input
( 
  eap_handle_type   eap_handle, 
  dsm_item_type   **sdu 
);

/*===========================================================================

FUNCTION EAP_GET_AUTH_PROT

DESCRIPTION
  This function is called by the Nak method to identify the supported
  authentication protocols

DEPENDENCIES
  EAP instance must have been created

RETURN VALUE
  Number of protocols supported
  0 : no protocols supported
  
  List of protocols supported is provided in buf

SIDE EFFECTS
  None.

===========================================================================*/
uint16 eap_get_auth_prot
(
  eap_handle_type  eap_handle,
  uint8           *buf
);


/*===========================================================================

FUNCTION EAP_GET_EAP_LYR_HANDLE

DESCRIPTION
  This function is called to query the EAP layer handle

DEPENDENCIES
  EAP instance must have been created

RETURN VALUE
  EAP layer handle

SIDE EFFECTS
  None.

===========================================================================*/
eap_eap_handle_type eap_get_eap_lyr_handle
(
  eap_handle_type eap_handle
);


/*===========================================================================

FUNCTION EAP_GET_PEER_LYR_HANDLE

DESCRIPTION
  This function is called to query the Peer layer handle

DEPENDENCIES
  EAP instance must have been created

RETURN VALUE
  EAP peer layer handle

SIDE EFFECTS
  None.

===========================================================================*/
eap_peer_handle_type eap_get_peer_lyr_handle
(
  eap_handle_type eap_handle
);

/*===========================================================================

FUNCTION EAP_AUTH_TRANSACTION_CLOSING

DESCRIPTION
  This function is called by the authentication protocol to notify 
  EAP's peer layer to perform appropriate state transitions, once
  indicating that the EAP auth transaction is closing.

DEPENDENCIES
  EAP instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void eap_auth_transaction_closing 
(
  eap_handle_type          eap_handle
);

/*===========================================================================
FUNCTION EAP_GET_ROLE

DESCRIPTION
  This function is called to identify whether EAP is in Server or Peer mode

DEPENDENCIES
  EAP instance must have been created

PARAMETERS
  eap_lyr_handle - EAP layer handle

RETURN VALUE
  EAP_PEER_ROLE          - EAP Peer Mode
  EAP_AUTHENTICATOR_ROLE - EAP Server Mode

SIDE EFFECTS
  None.
===========================================================================*/
eap_role_enum_type eap_get_role 
(
  eap_eap_handle_type  eap_lyr_handle
);

/*===========================================================================
FUNCTION EAP_GET_SERVER_LYR_HANDLE

DESCRIPTION
  This function is called to get the Server layer handle

DEPENDENCIES
  EAP instance must have been created

RETURN VALUE
  EAP Server layer handle or -1 (Failure)

SIDE EFFECTS
  None.
===========================================================================*/
eap_server_handle_type eap_get_server_lyr_handle
(
  eap_handle_type eap_handle
);

/*===========================================================================

FUNCTION EAP_GET_META_IDENTITY

DESCRIPTION
  This function is called to request the meta_identity provisioned by the
  EAP client

DEPENDENCIES
  EAP instance must have been created

RETURN VALUE
  Client provisioned meta identity.

SIDE EFFECTS
  None.

===========================================================================*/
eap_identity_type eap_get_meta_identity
(
  eap_handle_type eap_handle
);

/*===========================================================================
FUNCTION EAP_CLIENT_START_IND

DESCRIPTION
  This function EAP to indicate that the EAP identity stage
  is complete and that the backend authnetication server can start the
  authentication stage. This will call the corresponding client's callback.

DEPENDENCIES
  EAP instance must have been created

PARAMETERS
  eap_handle - EAP instance handle

RETURN VALUE
  0  - Success
  -1 - Error

SIDE EFFECTS
  None.
===========================================================================*/
void eap_client_start_ind 
(
  eap_handle_type eap_handle
);

/*===========================================================================
FUNCTION EAP_SERVER_DELETE_INSTANCE

DESCRIPTION
  Shuts down the Server layer instance.

DEPENDENCIES
  EAP Server instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void eap_server_delete_instance
(  
  eap_server_handle_type server_lyr_handle
);

/*===========================================================================
FUNCTION EAP_GET_CB_PTR_BY_INDEX

DESCRIPTION
  Access function to get EAP control block from eap_cb array.
 
DEPENDENCIES
  None

RETURN VALUE
  EAP control block

SIDE EFFECTS
  None.

===========================================================================*/
eap_cb_type* eap_get_cb_ptr_by_handle
(
  eap_handle_type handle
);

/*===========================================================================

FUNCTION EAP_IDENTITY_IS_VALID

DESCRIPTION
  This function is called by a method to verify the validity of the EAP
  udentity such as reauth_id and pseudonym id of eap_identity_type

DEPENDENCIES
  identity ptr has been allocated

RETURN VALUE
  True:  identity is valid
  False: identity is invalid

SIDE EFFECTS
  None.

===========================================================================*/
boolean eap_identity_is_valid
(
  eap_identity_type* id_ptr
);

#endif /* FEATURE_DATA_PS_EAP */
#endif  /* PS_EAP_H */

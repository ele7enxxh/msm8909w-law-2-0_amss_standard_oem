/*===========================================================================

                      L I N K L A Y E R _ S T U B S . C
                                
DESCRIPTION

EXTERNALIZED FUNCTIONS

Copyright (c) 2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/linklayer_stubs.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/31/09    pp     Initial release.
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"         /* Customer Specific Features                */

#ifdef FEATURE_DATA_PS_EAP
  #define _NO_EAP_STUBS
#endif /* FEATURE_DATA_PS_EAP */

#ifndef _NO_EAP_STUBS

#include "ps_eap.h"
#include "ps_eap_aka.h"
#include "ds_Utils_DebugMsg.h"

#include "msg.h"

#define LINKLAYER_STUBS_EXT_PERROR_NO_FEAT(api, feat)                     \
  LOG_MSG_ERROR_0(#api": "#feat" feature(s) turned off."                  \
           "Returning error.")

#define EAP_PERROR(api)                                                   \
  LINKLAYER_STUBS_EXT_PERROR_NO_FEAT(api, DATA_PS_EAP)

#define LLC_PERROR(api)                                                   \
  LINKLAYER_STUBS_EXT_PERROR_NO_FEAT(api, DATA_PS_LLC)

#define IPV6_PERROR(api)                                                  \
  NETIFACE_STUBS_EXT_PERROR_NO_FEAT(api, DATA_PS_IPV6)


/*===========================================================================

                          EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION EAP_METHOD_INPUT_CBACK_REG

DESCRIPTION
  Registers the method-specific input function with EAP services

DEPENDENCIES
  This must be done once at powerup as all EAP instances will use the same
  input function for a given method.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void eap_method_input_cback_reg
(
  eap_method_input_cback_type cback,
  eap_method_enum_type        method,
  eap_role_enum_type          role
)
{
  EAP_PERROR(eap_method_input_cback_reg);
  return;

} /* eap_method_input_cback_reg() */


/*===========================================================================

FUNCTION EAP_METHOD_CLOSE_CBACK_REG

DESCRIPTION
  Registers the method-specific close function with EAP services

DEPENDENCIES
  This must be done once at powerup as all EAP instances will use the same
  close function for a given method.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void eap_method_close_cback_reg
(
  eap_method_close_cback_type cback,
  eap_method_enum_type        method,
  eap_role_enum_type          role
)
{
  EAP_PERROR(eap_method_close_cback_reg);
  return;

} /* eap_method_close_cback_reg() */

/*===========================================================================

FUNCTION EAP_METHOD_RESULT_CBACK_REG

DESCRIPTION
  Registers the method-specific result function with EAP services

DEPENDENCIES
  This must be done once at powerup as all EAP instances will use the same
  result function for a given method.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void eap_method_result_cback_reg
(
  eap_method_result_cback_type cback,
  eap_method_enum_type        method,
  eap_role_enum_type          role
)
{
  EAP_PERROR(eap_method_result_cback_reg);
  return;

} /* eap_method_result_cback_reg() */

/*===========================================================================

FUNCTION EAP_CREATE_INSTANCE

DESCRIPTION
  Initializes EAP control block and returns handle to EAP instance.

DEPENDENCIES
  EAP services must have been initialized

RETURN VALUE
  Handle to EAP control block.

SIDE EFFECTS
  Allocates an EAP instance to the specified client.

===========================================================================*/
eap_handle_type eap_create_instance
(  
  eap_info_type *eap_info
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  EAP_PERROR(eap_create_instance);
  return -1;

} /* eap_create_instance()  */


/*===========================================================================

FUNCTION EAP_DELETE_INSTANCE

DESCRIPTION
   Shuts down the EAP instance.

DEPENDENCIES
  EAP instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  This will cause any subsequent packets sent from an EAP Method to be 
  discarded by the EAP layer.

===========================================================================*/
void eap_delete_instance
(  
  eap_handle_type eap_handle
)
{
  EAP_PERROR(eap_create_instance);
  return;

} /* eap_delete_instance()  */


/*===========================================================================

FUNCTION EAP_INPUT

DESCRIPTION
  Rx function for EAP protocol. Parse the EAP header and calls the upper
  Layer's input function if necessary.

DEPENDENCIES
  EAP instance must have been created

RETURN VALUE
  None

SIDE EFFECTS
  Calls auth protocols RX function.

===========================================================================*/
void eap_input
( 
  eap_handle_type   eap_handle,
  dsm_item_type   **sdu
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  EAP_PERROR(eap_input);
  return;

} /* eap_input()  */


/*===========================================================================
FUNCTION EAP_XMIT_PKT

DESCRIPTION
  Tx function for the EAP protocol. Adds EAP header and send the packet on
  the associated link layer.

DEPENDENCIES
  EAP instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  Sends the packet on associated link.

===========================================================================*/
void eap_xmit_pkt
(
   eap_handle_type   eap_handle,
   dsm_item_type   **sdu
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  EAP_PERROR(eap_xmit_pkt);
  return;

} /* eap_xmit_pkt() */


/*===========================================================================

FUNCTION EAP_AUTH_COMPLETE_IND

DESCRIPTION
  This function is called by the authentication protocol to notify EAP of the
  result of the authentication process.

DEPENDENCIES
  EAP instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  Calls client's authentication completed callback.

===========================================================================*/
void eap_auth_complete_ind 
(
  eap_handle_type          eap_handle,
  eap_result_enum_type     result,
  uint8                   *pre_master_key,
  uint16                   pre_master_key_len
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  EAP_PERROR(eap_auth_complete_ind);
  return;

} /* eap_auth_complete_ind()  */

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
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  EAP_PERROR(eap_auth_transaction_closing);
  return;
}

/*===========================================================================
FUNCTION EAP_SET_AUTHENTICATOR_RSP_REQUIRED

DESCRIPTION
  Offers support for dynamically setting the authenticator_rsp_required flag

DEPENDENCIES

  
RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void 
eap_set_authenticator_rsp_required
(
  eap_handle_type          eap_handle,
  boolean                  authenticator_rsp_required 
)
{
  EAP_PERROR(eap_set_authenticator_rsp_required);
  return;

}/* eap_set_authenticator_rsp_required() */

/*===========================================================================

FUNCTION EAP_GET_IDENTITY

DESCRIPTION
  This function is called by a method to request the identity of the EAP
  instance

DEPENDENCIES
  EAP instance must have been created

RETURN VALUE
  EAP identity.

SIDE EFFECTS
  None.

===========================================================================*/
eap_identity_type eap_get_identity
(
  eap_handle_type eap_handle
)
{
  eap_identity_type null_identity;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  EAP_PERROR(eap_get_identity);
  null_identity.len = 0;
  return null_identity;

} /* eap_get_identity() */

/*===========================================================================

FUNCTION EAP_HANDLE_IS_VALID

DESCRIPTION
  This function is called by a method to verify the validity of the EAP
  instance

DEPENDENCIES
  EAP instance should have been created

RETURN VALUE
  True:  Handle is valid
  False: Handle is invalid

SIDE EFFECTS
  None.

===========================================================================*/
boolean eap_handle_is_valid
(
  eap_handle_type eap_handle
)
{
  EAP_PERROR(eap_handle_is_valid);
  return FALSE;

} /* eap_handle_is_valid()  */


/*===========================================================================

FUNCTION EAP_SET_AUTH_MGR

DESCRIPTION
  This function set the authenticator manager for a particular upper method.
  
  Each EAP instance can have a separate auth manager irrespective of the
  method in use.
  
  EAP is just a conduit for this auth mgr information - an auth mgr is stored
  per EAP instance and not associated with any method. Caller is expected to
  store any such information in the auth_mgr entity.

DEPENDENCIES
  EAP instance should have been created

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void eap_set_auth_mgr
(
  eap_handle_type       eap_handle,
  void                 *auth_mgr
)
{
  EAP_PERROR(eap_set_auth_mgr);
  return;

} /* eap_set_auth_mgr() */


/*===========================================================================

FUNCTION EAP_SET_AUTH_MGR

DESCRIPTION
  This function gets the authenticator manager for a particular upper method.

DEPENDENCIES
  EAP instance should have been created

RETURN VALUE
  A pointer to the authenticator manager. Note here that the returned pointer
  is abstract and it is up to the caller to cast to the concrete manager.

SIDE EFFECTS
  None.

===========================================================================*/
void* eap_get_auth_mgr
(
  eap_handle_type   eap_handle
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  EAP_PERROR(eap_get_auth_mgr);
  return NULL;

} /* eap_get_auth_mgr() */

/*===========================================================================

FUNCTION EAP_SET_META_INFO

DESCRIPTION
  This function set the meta info for a particular upper method.
  
  Each EAP instance can have a separate meta info irrespective of the
  method in use.
  
DEPENDENCIES
  EAP instance should have been created

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void eap_set_meta_info
(
  eap_handle_type       eap_handle,
  void                 *meta_info_ptr
)
{
  EAP_PERROR(eap_set_meta_info);
  return;

} /* eap_set_auth_mgr() */


/*===========================================================================

FUNCTION EAP_GET_META_INFO

DESCRIPTION
  This function gets the meta info for a particular upper method.

DEPENDENCIES
  EAP instance should have been created

RETURN VALUE
  A pointer to the meta info. Note here that the returned pointer
  is abstract and it is up to the caller to cast to the concrete manager.

SIDE EFFECTS
  None.

===========================================================================*/
void* eap_get_meta_info
(
  eap_handle_type   eap_handle
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  EAP_PERROR(eap_get_meta_info);
  return NULL;

} /* eap_get_meta_info() */

/*===========================================================================

FUNCTION EAP_GET_IDENTIFIER

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
uint8 eap_get_identifier
(
  eap_handle_type eap_handle
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  EAP_PERROR(eap_get_identifier);
  return 0;

} /* eap_get_identifier() */

/*===========================================================================
FUNCTION EAP_AKA_POWERUP_INIT

DESCRIPTION
  Initialize the EAP-AKA software entity.

DEPENDENCIES
  None

PARAMETERS
  None.
RETURN VALUE
  None

SIDE EFFECTS
  Register for PEAP packets over EAP transport.
  
===========================================================================*/
void 
eap_aka_powerup_init
(
  void
)
{
  EAP_PERROR(eap_aka_powerup_init);
  return;

}

/*===========================================================================
FUNCTION EAP_AKA_INIT

DESCRIPTION
  Initialize the EAP-AKA software entity.

DEPENDENCIES
  None

PARAMETERS
  None.
RETURN VALUE
  None

SIDE EFFECTS
  Register for PEAP packets over EAP transport.
  
===========================================================================*/
void 
eap_aka_init
(
  void
)
{
  EAP_PERROR(eap_aka_init);
  return;

}/* eap_aka_init() */

/*===========================================================================
FUNCTION EAP_SIM_INIT

DESCRIPTION
  Initialize the EAP-SIM software entity.

DEPENDENCIES
  None

PARAMETERS
  None.
  
RETURN VALUE
  None

SIDE EFFECTS
  Register for EAP-SIM packets over EAP transport.
  
===========================================================================*/
void 
eap_sim_init
(
  void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  EAP_PERROR(eap_sim_init);
  return;

}/* eap_sim_init() */

/*===========================================================================

FUNCTION EAP_GET_REAUTH_ID

DESCRIPTION
  This function is called by a method to request the re-authentication 
  identifier of the last EAP packet

DEPENDENCIES
  EAP instance must have been created

RETURN VALUE
  Last EAP reauth identifier if present -1 otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
int eap_get_reauth_id
(
  eap_handle_type                        eap_handle,
  uint8*                                 data,
  uint16*                                max_data_len
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  EAP_PERROR(eap_get_reauth_id);
  return -1;

}

/*===========================================================================

FUNCTION EAP_GET_PSEUDONYM_ID

DESCRIPTION
  This function is called by a method to request the pseudonym
  identifier of the last EAP packet

DEPENDENCIES
  EAP instance must have been created

RETURN VALUE
  Last EAP pseudonym identifier if present -1 otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
int eap_get_pseudonym_id
(
  eap_handle_type                        eap_handle,
  uint8*                                 data,
  uint16*                                max_data_len
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  EAP_PERROR(eap_get_pseudonym_id);
  return -1;
}

#endif /* _NO_EAP_STUBS  */


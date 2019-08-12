/*===========================================================================

                     P S _ E A P _ I N T _ M E T H O D S . C

DESCRIPTION
  This software unit holds the PS_EAP internal methods, viz. Identity, Nak
  and Notification.

  The functions externalized by this module are to be called ONLY by the EAP
  modules.


EXTERNALIZED FUNCTIONS
  eap_svc_init()
    This function is called at powerup to initialize the eap services

  eap_method_input_cback_reg()
    This is called to register the input functions for EAP external methods

  eap_create_instance()
    This is called to create an instance of EAP to be used by the client

  eap_delete_instance()
    This returns the instance to the free pool to be reassigned to another
    client

  eap_input()
    This function is called by an EAP client to provide an incoming EAP
    packet

  eap_xmit_pkt()
    This is called by an EAP method to send a packet over the client

  eap_auth_complete_ind()
    This indicates to the EAP module that the authentication method has
    received a success indication



 Copyright (c) 2005-2013 Qualcomm Technologies Incorporated. 
 All Rights Reserved.
 Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_eapi_methods.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/27/13    cx     Replace memcpy and memmove with memscpy and memsmove
03/18/13    fn     Added reauth/pseudo changes for multi-sim support
10/26/12    fn     Added changes for memory optimization 
08/17/12    fn     Cleanup ASSERT()and dsm_pushdown 
05/09/12    op     Added support for the additional meta_identity
12/06/11    scb    Added fix to remove ASSERT() for malformed packets.
10/19/10    sa     Made changes for graceful exit in case of dsm exhaustion.
09/08/08    scb    Added fix for CR 154887.
02/27/06    lti    Added EAP-MD5 support
03/30/05    lyr    Created module

===========================================================================*/

/*===========================================================================

                                INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"         /* Customer Specific Features                */
#include <stringl/stringl.h>

#ifdef FEATURE_DATA_PS_EAP
#include "ps_eap.h"
#include "ps_eap_sim_aka.h"

#include "ps_eapi.h"

#include "ps_eapi_methods.h"

#include "dsm.h"
#include "ps_system_heap.h"
#include "msg.h"

#include "secapi.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                    LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#define EAP_MD5_DIGEST_LEN  16
#define EAP_MD5_MAX_CHALLENGE_LEN    255
#define EAP_MD5_PKT_MAX_LEN          EAPI_METHODS_PASSWORD_MAX_CHAR + \
                                     EAP_MD5_MAX_CHALLENGE_LEN + 2


typedef enum
{
  EAP_MD5_STATUS_SUCCESS,
  EAP_MD5_STATUS_FAILURE
}EAP_MD5_STATUS;

/*===========================================================================

                          FORWARD DECLARATIONS FOR MODULE

===========================================================================*/


/*===========================================================================

                          EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


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
)
{
  dsm_item_type      *tx_pkt;
  eap_identity_type   identity;
  eap_identity_type   meta_identity;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Identity method does not need to process received packet - free it
    immediately

    NOTE: Identity Request MAY be NULL, i.e. after the EAP and Peer layers
    have stripped the headers, the packet may have been freed already
  -------------------------------------------------------------------------*/
  if( sdu != NULL )
  {
    dsm_free_packet( sdu );
  }

  /*-------------------------------------------------------------------------
    Prepare a new packet for Identity response
  -------------------------------------------------------------------------*/
  tx_pkt = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );

  if( NULL == tx_pkt )
  {
    LOG_MSG_INFO1_0("Could not assign a free DSM item" );
    return;
  }

  /*-------------------------------------------------------------------------
    Get the EAP identity
  -------------------------------------------------------------------------*/
  identity = eap_get_identity( eap_handle );

  /*-------------------------------------------------------------------------
    If the client has provisioned the meta_identity, then the meta_identity
    will be used as the EAP identity only if reauth or pseudonym are not
    present.
  -------------------------------------------------------------------------*/
	if ( FALSE == eap_identity_is_valid(eap_reauth_id[eap_handle]) &&
			 FALSE == eap_identity_is_valid(eap_pseudonym_id[eap_handle]) )
  {
    meta_identity = eap_get_meta_identity( eap_handle );

    if ( 0 != meta_identity.len )
    {
      identity = meta_identity;
    }
    else
    {
      LOG_MSG_INFO2_0("meta_identity not filled out in EAP meta_info");
    }
  }

  LOG_MSG_INFO2_1("Length of EAP identity returned = %d", identity.len);

  /*-------------------------------------------------------------------------
    Per RFC:

    Identity response should contain the Identity of the Peer if available;

    Otherwise, identity response should be zero bytes in length
  -------------------------------------------------------------------------*/

  if( 0 < identity.len )
  {
    /*-----------------------------------------------------------------------
      Send the actual identity since it is non-zero
    -----------------------------------------------------------------------*/
    (void) dsm_pushdown_packed( &tx_pkt,
                                identity.name,
                                identity.len,
                                DSM_DS_SMALL_ITEM_POOL );
  }

  /*-------------------------------------------------------------------------
    Pass down to EAP
  -------------------------------------------------------------------------*/
  eap_xmit_pkt( eap_handle,
                &tx_pkt);

} /* eap_identity_input() */


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
)
{
  dsm_item_type *tx_pkt;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Notification Request contains a message to be displayed;

    Ignore it for now, but record that a notification has been received
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO2_0("EAP Notification message received by Peer" );
  dsm_free_packet( sdu );

  /*-------------------------------------------------------------------------
    Prepare a new packet for Notification response
  -------------------------------------------------------------------------*/
  tx_pkt = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );

  if( NULL == tx_pkt )
  {
    LOG_MSG_INFO1_0("Could not assign a free DSM item" );

    return;
  }

  /*-------------------------------------------------------------------------
    Per RFC:

    Notification response should have zero data bytes
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Pass down to EAP
  -------------------------------------------------------------------------*/
  eap_xmit_pkt( eap_handle,
                &tx_pkt);

} /* eap_notification_input() */


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
)
{
  dsm_item_type *tx_pkt;
  uint8          auth_prot_buf[EAP_MAX_METHOD];
  uint16         auth_prot_buf_len;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Nak method does not need to process received packet - free it immediately

    NOTE: Request may be NULL after EAP and Peer layer headers have been
    stripped off
  -------------------------------------------------------------------------*/
  if( sdu != NULL )
  {
    dsm_free_packet( sdu );
  }

  /*-------------------------------------------------------------------------
    Prepare a new packet for Nak response
  -------------------------------------------------------------------------*/
  tx_pkt = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );

  if( NULL == tx_pkt )
  {
    LOG_MSG_INFO1_0("Could not assign a free DSM item" );

    return;
  }

  /*-------------------------------------------------------------------------
    Per RFC:

    Nak response MUST contain a list of supported authentication protocols;
    Otherwise, value 0 if no proposed alternative
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Get the supported auth protocol list
  -------------------------------------------------------------------------*/
  auth_prot_buf_len = eap_get_auth_prot( eap_handle,
                                         auth_prot_buf );

  if( 0 == auth_prot_buf_len )
  {
    /*-----------------------------------------------------------------------
      Prepare to send 0 as the Nak response
    -----------------------------------------------------------------------*/
    memset( auth_prot_buf,
            0,
            ((uint8)EAP_MAX_METHOD)* sizeof(uint8) );

    auth_prot_buf_len = 1;
  }

  (void) dsm_pushdown_packed( &tx_pkt,
                              auth_prot_buf,
                              auth_prot_buf_len,
                              DSM_DS_SMALL_ITEM_POOL );

  /*-------------------------------------------------------------------------
    Pass down to EAP
  -------------------------------------------------------------------------*/
  eap_xmit_pkt( eap_handle,
                &tx_pkt);


} /* eap_nak_input()  */

/*===========================================================================

FUNCTION EAP_COMPUTE_MD5

DESCRIPTION
    This function creates an MD5 digest.

DEPENDENCIES


RETURN VALUE
    EAP_MD5_STATUS_SUCCESS if succeeded, EAP_MD5_STATUS_FAILURE if
    failed.

SIDE EFFECTS


===========================================================================*/
EAP_MD5_STATUS eap_compute_md5
(
    unsigned char            *data,
    uint16                    data_length,
    unsigned char            *digest
)
{
  secapi_algo_enum_type       secapi_algo;
  secapi_handle_type          hash_handle   = NULL;
  secapi_hsh_param_data_type  hash_param;
  secerrno_enum_type          secapi_status;
  EAP_MD5_STATUS              status        = EAP_MD5_STATUS_FAILURE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Set secapi algo
  -------------------------------------------------------------------------*/
  secapi_algo = SECAPI_MD5;

  /*-------------------------------------------------------------------------
    Create secapi instance
  -------------------------------------------------------------------------*/
  secapi_status = secapi_new(&hash_handle, secapi_algo);
  if ( secapi_status )
  {
    LOG_MSG_ERROR_0("Failed to create hash handle.");
    return status;
  }


  /*-------------------------------------------------------------------------
    Set the execution platform to ARM
  -------------------------------------------------------------------------*/
  hash_param.common.exec_platform = SECAPI_EXEC_PLATFORM_ARM;
  secapi_status = secapi_hsh_set_param( hash_handle,
                                        secapi_algo,
                                        SECAPI_EXEC_PLATFORM,
                                        &hash_param );

  if ( E_SUCCESS != secapi_status )
  {
    LOG_MSG_ERROR_0("Unable to use ARM platform" );
    return status;
  };

  /*-------------------------------------------------------------------------
    Set the mode to "atomic"
    The default processor is accelerator prefered
  -------------------------------------------------------------------------*/
  hash_param.mode = SECAPI_HSH_INPUT_MODE_ATOMIC;
  secapi_status = secapi_hsh_set_param( hash_handle,
                                        secapi_algo,
                                        SECAPI_HSH_INPUT_MODE,
                                        &hash_param );

  /*-------------------------------------------------------------------------
    Compute md5
  -------------------------------------------------------------------------*/
  if ( E_SUCCESS == secapi_status )
  {
    secapi_status = secapi_hsh_create_digest( hash_handle,
                                  			      secapi_algo,
                                              data,
                                     		      data_length,
                                     		      digest);
  }

  if ( E_SUCCESS == secapi_status )
    status = EAP_MD5_STATUS_SUCCESS;
  else
    LOG_MSG_ERROR_0("HMAC encryption failed.");

  /*-------------------------------------------------------------------------
    Clean up
  -------------------------------------------------------------------------*/
  secapi_status = secapi_delete(&hash_handle);
  if ( E_SUCCESS != secapi_status )
  {
    LOG_MSG_ERROR_0("Unable to use ARM platform" );
    status = EAP_MD5_STATUS_FAILURE;
  }

  return status;
}

/*===========================================================================

FUNCTION EAP_MD5_INPUT

DESCRIPTION
  Input function for EAP MD5 method.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void eap_md5_input
(
  eap_handle_type   eap_handle,
  dsm_item_type   **sdu
)
{
  unsigned char                   output[EAP_MD5_DIGEST_LEN];

  uint8           	              identifier;
  uint8 			                    challenge_len = 0;

  unsigned char* 	                md5_digest		= NULL;
  dsm_item_type*  	              eap_md5_pkt   = NULL;

  unsigned char* 	                data			    = NULL;
  uint16		                      data_len		  = 0;
  uint16 			                    offset 			  = 0;

  uint8                           result_len    = EAP_MD5_DIGEST_LEN;
  EAP_MD5_STATUS	                secapi_status = EAP_MD5_STATUS_FAILURE;
  eapi_methods_auth_mgr_type*     auth_mgr      = NULL;
  eapi_methods_credentials_type   eap_credentials;
  uint16                          desired_len;
  uint16                          return_len;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Sanity check
  -------------------------------------------------------------------------*/
  if (( NULL == sdu ) || ( NULL == *sdu ))
  {
    LOG_MSG_ERROR_0("EAP MD5 Fatal Error: Invalid dsm buffer sent on md5 input");
    return;
  }

  if ( EAP_INVALID_HANDLE == eap_handle )
  {
    LOG_MSG_ERROR_0("EAP MD5 Fatal Error: Invalid eap handle sent on md5 input");
    dsm_free_packet(sdu);
    return;
  }
  /*-------------------------------------------------------------------------
    retrieving the authentication manager of the eap instance
  -------------------------------------------------------------------------*/
  auth_mgr   = eap_get_auth_mgr(eap_handle);
  if ( NULL == auth_mgr )
  {
    LOG_MSG_ERROR_0("EAP MD5 Fatal Error: Invalid authentication manager" );
    dsm_free_packet(sdu);
    return;
  }

  if ( NULL == auth_mgr->eapi_methods_get_credentials_f_ptr )
  {
    LOG_MSG_ERROR_0("EAP MD5 Fatal Error: Null auth manager callback" );
    dsm_free_packet(sdu);
    return;
  }

  /*-------------------------------------------------------------------------
    retrieving the the credentials for MD5
  -------------------------------------------------------------------------*/
  if ( FALSE == auth_mgr->eapi_methods_get_credentials_f_ptr(
                                           EAP_MD5_METHOD, &eap_credentials))
  {
    LOG_MSG_ERROR_0("EAP MD5 Error: Can not retrieve credentials" );
    dsm_free_packet(sdu);
    return;
  }

  /*-------------------------------------------------------------------------
    Check Password Length in order to prevent memory overwrite
  -------------------------------------------------------------------------*/
  if ( EAPI_METHODS_PASSWORD_MAX_CHAR < eap_credentials.md5.pass.len )
  {
    LOG_MSG_ERROR_0("EAP MD5 Fatal Error : Password Too long");
    dsm_free_packet(sdu);
    return;
  }

  /*-------------------------------------------------------------------------
    Check User ID Length in order to prevent memory overwrite
  -------------------------------------------------------------------------*/
  if ( EAP_IDENTITY_MAX_CHAR < eap_credentials.md5.user_id.len )
  {
    LOG_MSG_ERROR_0("EAP MD5 Fatal Error : User ID Too long");
    dsm_free_packet(sdu);
    return;
  }

  /*-------------------------------------------------------------------------
    Unpack received packet
  -------------------------------------------------------------------------*/
  if ( sizeof(challenge_len) != dsm_pullup( sdu, &(challenge_len),
                                            sizeof(challenge_len)))
  {
    LOG_MSG_ERROR_0("EAP MD5 Error :Can not retrieve challenge len from pkt");
    dsm_free_packet(sdu);
    return;
  }

  /*-------------------------------------------------------------------------
    Preparing and allocating input buffer
  -------------------------------------------------------------------------*/
  data_len   = sizeof(identifier) + eap_credentials.md5.pass.len +
               challenge_len;

  data  = (unsigned char*) ps_system_heap_mem_alloc(EAP_MD5_PKT_MAX_LEN);
  if ( data == NULL )
  {
    LOG_MSG_ERROR_0("EAP MD5 Error :Can not allocate heap memory");
    dsm_free_packet(sdu);
    return;
  }

  memset( data, 0, data_len);
  /*-------------------------------------------------------------------------
    retrieving the identifier of the last EAP packet
  -------------------------------------------------------------------------*/
  identifier = eap_get_identifier(eap_handle);

  /*-------------------------------------------------------------------------
    According to RFC 3748 MD5 Challenge Type is analogous to PPP CHAP - RFC
    1994 and according to RFC 1994 the response value is the one way hash
    calculated over a stream of octets consisting of the Identifier, followed
    by the "secret" followed by the Challenge value.
  -------------------------------------------------------------------------*/
  memscpy( data + offset, 
           EAP_MD5_PKT_MAX_LEN - offset, 
           &(identifier), 
           sizeof(identifier) );
  offset += sizeof(identifier);

  memscpy( data + offset,
           EAP_MD5_PKT_MAX_LEN - offset, 
           eap_credentials.md5.pass.password ,
           eap_credentials.md5.pass.len );
  offset += eap_credentials.md5.pass.len;

  return_len = dsm_pullup( sdu, data + offset, challenge_len);

  if ( return_len != challenge_len )
  {
    LOG_MSG_ERROR_0("EAP MD5 Error :Can not extract challenge");
    dsm_free_packet(sdu);
    PS_SYSTEM_HEAP_MEM_FREE(data);
    return;
  }

  dsm_free_packet( sdu );

  /*-------------------------------------------------------------------------
    Create MD5 digest from data above
  -------------------------------------------------------------------------*/
  md5_digest    = output;
  secapi_status = eap_compute_md5( data, data_len, md5_digest );
  PS_SYSTEM_HEAP_MEM_FREE(data);

  if ( EAP_MD5_STATUS_SUCCESS != secapi_status )
  {
    LOG_MSG_ERROR_0("EAP MD5 Error :Unable to create MD5 digest");
    return;
  };

  /*-------------------------------------------------------------------------
    Prepare a new packet for MD5 response
  -------------------------------------------------------------------------*/
  desired_len = eap_credentials.md5.user_id.len + EAP_MD5_DIGEST_LEN +
                sizeof(result_len);

  return_len  = dsm_pushdown_packed(&eap_md5_pkt, eap_credentials.md5.user_id.name,
                                    eap_credentials.md5.user_id.len,
                                    DSM_DS_POOL_SIZE(eap_credentials.md5.user_id.len));

  return_len += dsm_pushdown_packed(&eap_md5_pkt, md5_digest,
	                                  EAP_MD5_DIGEST_LEN, DSM_DS_SMALL_ITEM_POOL );

  return_len += dsm_pushdown_packed(&eap_md5_pkt, &(result_len),
                                    sizeof(result_len),
                                    DSM_DS_POOL_SIZE(sizeof(result_len)) );

  if ( desired_len != return_len )
  {
    LOG_MSG_ERROR_0("Unable to push deisred len for MD5 response packet" );
    dsm_free_packet(&eap_md5_pkt);
    return;
  }
  /*-------------------------------------------------------------------------
    Pass down to EAP
  -------------------------------------------------------------------------*/
  eap_xmit_pkt( eap_handle,&eap_md5_pkt );

  /*-------------------------------------------------------------------------
    Notify EAP of completion
  -------------------------------------------------------------------------*/
  eap_auth_complete_ind( eap_handle, EAP_RESULT_SUCCESS, NULL,0 );

} /* eap_md5_input */

#endif /* FEATURE_DATA_PS_EAP */

/*===========================================================================

                           P S _ E A P _ M G R . C
                                
DESCRIPTION
  This software unit holds the PS_EAP external APIs to pass down to the EAP
  layer or the Peer layer in the EAP system.


EXTERNALIZED FUNCTIONS
  eap_svc_init()
    This function is called at powerup to initialize the eap services
    
  eap_method_input_cback_reg()
    This is called to register the input functions for EAP external methods
    
  eap_method_close_cback_reg()
    This is called to register the close functions for EAP external methods    
    
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
  
  eap_set_auth_mgr()
    This sets the auth mgr to the requested value
    
  eap_get_auth_mgr()
    This gets the auth mgr stored in the EAP control block
  
  eap_set_meta_info()
    This sets the method meta info to the requested value
    
  eap_get_meta_info()
    This gets the method meta info stored in the EAP control block
  


Copyright (c) 2005-2014 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_eap_mgr.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/12/14    fn      Added option to configure realm inheritance for reauth id 
05/21/13    fn     Added function to check validity of identity 
11/09/12    fn     Added ume powerup init in eap_svc_init 
10/26/12    fn     Added changes to ensure single init of sublayerm modules
10/26/12    fn     Moved NV item file path creation to ps_utils init
08/17/12    fn     Cleanup ASSERT()
02/03/12    scb    Removed ASSERTs to prevent phone from reboot in case of
                   an error
08/26/11    op     Added changes to obtain supplementary SIM/AKA auth info
08/18/11    fn     Removed assert on meta_info, eap_delete_instance,
                   eap_input, and eap_get_identifier
06/27/11    scb    Added support for start_ind_user_data in the start_ind_cb
04/04/11    mga    Changes to ensure eap_create_instance returns INVALID 
                   HANDLE if it is unable to allocate memory. 
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
08/11/10    jee    Added retransmission functionality for EAP server
06/02/10   jee/mga Modifications to support EAP server functionality
12/10/09    jee    Changes for the MMGSDI re-arch
09/29/09    jee    Fixed klocwork errors
08/31/09    mga    Merged from eHRPD branch
03/06/09    scb    Added changes for memory reduction initiative.
01/08/09    scb    Resolved MoB LINT errors
11/28/07    lti    Added changes for supporting EAP Ext
05/13/07    lti    Added meta info for upper EAP methods
11/27/06    lti    Removed assert on auth complete ind for invalid instance
08/25/06    lti    Added function for setting the resp required flag
08/25/06    lti    Added result cback registration
02/27/06    lyr    WLAN 3.0 additional authentication schemes support
03/30/05    lyr    Created module

===========================================================================*/

/*===========================================================================

                                INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"         /* Customer Specific Features                */

#ifdef FEATURE_DATA_PS_EAP
#include "ps_eap.h"
#include "ps_eap_init.h"

#include "ps_eapi.h"
#include "ps_eap_ext.h"

#include "dsm.h"

#include "msg.h"
#include "ps_crit_sect.h"
#include "ps_eap_sim_aka.h"
#include "ps_eap_sim_aka_ume.h"
#include "ps_eap_client_config.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                    LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================
                              DATA DEFINITIONS
===========================================================================*/

/*---------------------------------------------------------------------------
  This forms the pool of EAP control blocks which can be assigned using
  eap_create_instance()
---------------------------------------------------------------------------*/
static eap_cb_type eap_cb[EAP_MAX_INSTANCES];

/*---------------------------------------------------------------------------
  Critical section to ensure that only one task can create or delete an
  instance at a time.
---------------------------------------------------------------------------*/
static ps_crit_sect_type eap_crit_sect;

/*---------------------------------------------------------------------------
  To ensure that eap_svc_init() has been called atleast once during the 
  lifetime of the phone to init necessary data structs.
---------------------------------------------------------------------------*/
static boolean eap_svc_inited = FALSE ;


/*===========================================================================

                          FORWARD DECLARATIONS FOR MODULE

===========================================================================*/


/*===========================================================================

                          EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION EAP_SVC_INIT

DESCRIPTION
  Initializes the EAP control blocks.This function should be called
 only once during powerup. eap_svc_init should not be called after
 powerup initialization.

DEPENDENCIES
  GSDI task must be spawned for MMGSDI API calls

RETURN VALUE
  None

SIDE EFFECTS
  Initialize the EAP control blocks and registers MMGSDI session

===========================================================================*/
void eap_svc_init
(
  void
)
{
  int eap_cb_cnt;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Initialize the EAP control blocks
  -------------------------------------------------------------------------*/
  for( eap_cb_cnt = 0; eap_cb_cnt < EAP_MAX_INSTANCES; eap_cb_cnt++ )
  {
    eap_cb[eap_cb_cnt].eap_lyr_handle = -1;

    eap_cb[eap_cb_cnt].peer_lyr_handle = -1;

    eap_cb[eap_cb_cnt].server_lyr_handle = -1;

    eap_cb[eap_cb_cnt].is_assigned = FALSE;

    eap_cb[eap_cb_cnt].support_eap_ext = FALSE;

  }

  if( TRUE != eap_svc_inited )
  {
    /*-------------------------------------------------------------------------
      Initialize locks
    -------------------------------------------------------------------------*/
    PS_INIT_CRIT_SECTION( &eap_crit_sect );

    /*-------------------------------------------------------------------------
      Initialize EAP layer
    -------------------------------------------------------------------------*/
    eap_eap_init();

    /*-------------------------------------------------------------------------
     Initialize Peer layer
    -------------------------------------------------------------------------*/
    eap_peer_init();

    /*-------------------------------------------------------------------------
      Initialize Server layer
    -------------------------------------------------------------------------*/
    eap_server_init();

    /*-------------------------------------------------------------------------
      Initialize Extended EAP layer
    -------------------------------------------------------------------------*/
    eap_ext_init();

    /*-------------------------------------------------------------------------
      Initialize UME Module
    -------------------------------------------------------------------------*/
    eap_sim_aka_ume_powerup_init();
  }

  /*-------------------------------------------------------------------------
    Set eap_svc_inited to TRUE
  -------------------------------------------------------------------------*/
  eap_svc_inited = TRUE;

}  /* eap_svc_init()  */


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
  LOG_MSG_INFO2_2("eap_method_input_cback_reg method:%d, role:%d",
                  method, role);
  /*-------------------------------------------------------------------------
    Check which role the method supports and register callbacks for 
    peer/authenticator/both.
  -------------------------------------------------------------------------*/
  if( EAP_PEER_ROLE == role )
  {
    eap_peer_method_input_cback_reg( cback,
                                     method );
  }

  else if( EAP_AUTHENTICATOR_ROLE == role )
  {
    eap_server_method_input_cback_reg( cback,
                                       method );
  }

  else
  {
    LOG_MSG_ERROR_1( "EAP Role %d not supported; invalid cback reg", role );
    return;
  }

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
  /*-------------------------------------------------------------------------
    Check which role the method supports and register callbacks for 
    peer/authenticator/both.
  -------------------------------------------------------------------------*/
  if( EAP_PEER_ROLE == role )
  {
    eap_peer_method_close_cback_reg( cback,
                                     method );
  }

  else  if( EAP_AUTHENTICATOR_ROLE == role )
  {
    eap_server_method_close_cback_reg( cback,
                                     method );
  }

  else
  {
    LOG_MSG_ERROR_1( "EAP Role %d not supported; invalid cback reg", role );
    return;
  }

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
  /*-------------------------------------------------------------------------
    Check which role the method supports and register callbacks for 
    peer/authenticator/both.
    
    NOTE: Only Peer is currently supported
  -------------------------------------------------------------------------*/
  if( EAP_PEER_ROLE == role )
  {
    eap_peer_method_result_cback_reg( cback,
                                     method );
  }

  else  if( EAP_AUTHENTICATOR_ROLE == role )
  {
    eap_server_method_result_cback_reg( cback,
                                        method );
  }

  else
  {
    LOG_MSG_ERROR_0( "Only EAP Peer Role supported; invalid cback reg" );
    return;
  }

} /* eap_method_result_cback_reg() */

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
)
{
  /*-------------------------------------------------------------------------
    Check which role the method supports and register callbacks for 
    peer/authenticator/both.
    
    NOTE: Only Peer is currently supported
  -------------------------------------------------------------------------*/
  if( EAP_PEER_ROLE == role )
  {
    eap_peer_method_supp_result_cback_reg( cback,
                                           method );
  }

  else  if( EAP_AUTHENTICATOR_ROLE == role )
  {
    eap_server_method_supp_result_cback_reg( cback,
                                             method );
  }

  else
  {
    LOG_MSG_INFO1_0( "Only EAP Peer Role supported; invalid cback reg" );
  }

} /* eap_method_supp_result_cback_reg() */

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
  int                  eap_cb_cnt;
  eap_handle_type      eap_handle;
  eap_eap_info_type    eap_eap_info;
  eap_peer_info_type   eap_peer_info;
  eap_server_info_type eap_server_info;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    IMPORTANT NOTE:
    
    The lock should not be released until the EAP layer and Peer/Server layer
    have been created.
    Both depend upon EAP mgr to provide mutual exclusion
  -------------------------------------------------------------------------*/
  if( FALSE == eap_svc_inited )
  {
    /*-----------------------------------------------------------------------
      Could not find a free instance
    -----------------------------------------------------------------------*/
    LOG_MSG_ERROR_0("Could not create EAP instance; EAP service uninitialized" );
    return -1;
  }

  /*-------------------------------------------------------------------------
    Initialize to invalid value
  -------------------------------------------------------------------------*/
  eap_handle = -1;


  /*-------------------------------------------------------------------------
    Obtain the lock to access EAP control blocks
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( &eap_crit_sect );

  /*-------------------------------------------------------------------------
    Find a free EAP control block to operate on
  -------------------------------------------------------------------------*/
  for( eap_cb_cnt = 0; eap_cb_cnt < EAP_MAX_INSTANCES; eap_cb_cnt++ )
  {
    if( FALSE == eap_cb[eap_cb_cnt].is_assigned )
    {
      eap_handle = eap_cb_cnt;
      break;
    }
  }

  if( -1 == eap_handle )
  {
    /*-----------------------------------------------------------------------
      Could not find a free instance
    -----------------------------------------------------------------------*/
    LOG_MSG_ERROR_0("Could not assign EAP instance" );
    return -1;
  }

  /*-------------------------------------------------------------------------
    Verify the values passed in
  -------------------------------------------------------------------------*/
  if ( NULL == eap_info )
  {
    return -1;
  }

  if ( NULL == eap_info->result_ind_f_ptr || 
       NULL == eap_info->trp_tx_f_ptr )
  {
    LOG_MSG_ERROR_0 ("Invalid EAP info pointers");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Create corresponding EAP layer
  -------------------------------------------------------------------------*/
  eap_eap_info.eap_handle = eap_handle;
  eap_eap_info.eap_trp_info.result_ind_f_ptr = eap_info->result_ind_f_ptr;
  eap_eap_info.eap_trp_info.result_ind_user_data = 
    eap_info->result_ind_user_data;
  eap_eap_info.eap_trp_info.supp_result_ind_f_ptr =
    eap_info->supp_result_ind_f_ptr;
  eap_eap_info.eap_trp_info.supp_result_ind_user_data =
    eap_info->supp_result_ind_user_data;
  eap_eap_info.eap_trp_info.trp_tx_f_ptr = eap_info->trp_tx_f_ptr;
  eap_eap_info.eap_trp_info.trp_tx_user_data = eap_info->trp_tx_user_data;
  eap_eap_info.eap_trp_info.eap_client_start_ind_cb = 
                                          eap_info->eap_client_start_ind_cb;
  eap_eap_info.eap_trp_info.start_ind_user_data = 
    eap_info->start_ind_user_data;
  eap_eap_info.raw_mode = eap_info->raw_mode;
  eap_eap_info.eap_role = eap_info->eap_role;

  eap_cb[eap_handle].eap_lyr_handle = eap_eap_create_instance(&eap_eap_info);
  
  if( -1 == eap_cb[eap_handle].eap_lyr_handle )
  {
    /*-----------------------------------------------------------------------
      Could not find a free EAP layer
    -----------------------------------------------------------------------*/
    LOG_MSG_ERROR_0("Could not assign EAP layer instance" );
    return -1;
  }

  LOG_MSG_INFO1_1("EAP role = %d", eap_eap_info.eap_role );

  /*-------------------------------------------------------------------------
    Create corresponding Peer or Server layer based on EAP role
  -------------------------------------------------------------------------*/
  if (EAP_PEER_ROLE == eap_eap_info.eap_role)
  {
    eap_peer_info.eap_handle = eap_handle;
    eap_peer_info.eap_auth_prot_mask = eap_info->eap_auth_prot_mask;
    eap_peer_info.eap_identity = eap_info->user_id;
    
    eap_peer_info.raw_mode = eap_info->raw_mode;
    eap_peer_info.authenticator_rsp_required = 
      eap_info->authenticator_rsp_required;
    
    eap_cb[eap_handle].peer_lyr_handle = 
      eap_peer_create_instance(&eap_peer_info);
  
    if( -1 == eap_cb[eap_handle].peer_lyr_handle )
    {
      /*-----------------------------------------------------------------------
        Could not find a free EAP layer
      -----------------------------------------------------------------------*/
      LOG_MSG_ERROR_0("Could not assign EAP peer layer instance" );
      eap_eap_delete_instance( eap_cb[eap_handle].eap_lyr_handle );
      return -1;
    }
  }

  else if (EAP_AUTHENTICATOR_ROLE == eap_eap_info.eap_role)
  {
    eap_server_info.eap_handle = eap_handle;
    eap_server_info.eap_auth_prot_mask = eap_info->eap_auth_prot_mask;
    eap_server_info.eap_identity = eap_info->user_id;
    
    eap_server_info.raw_mode = eap_info->raw_mode;
    eap_server_info.timer_f_ptr = eap_info->timer_f_ptr;
    eap_server_info.timer_user_data = eap_info->timer_user_data;

    eap_cb[eap_handle].server_lyr_handle = 
      eap_server_create_instance(&eap_server_info);
  
    if( -1 == eap_cb[eap_handle].server_lyr_handle )
    {
      /*-----------------------------------------------------------------------
        Could not find a free EAP layer
      -----------------------------------------------------------------------*/
      LOG_MSG_ERROR_0("Could not assign EAP Server layer instance" );
      eap_eap_delete_instance( eap_cb[eap_handle].eap_lyr_handle );
      return -1;
    }
  }

  else
  {
    LOG_MSG_ERROR_1("Invalid EAp role %d", eap_eap_info.eap_role );
    eap_eap_delete_instance( eap_cb[eap_handle].eap_lyr_handle );
    return -1;
  }

  /*-------------------------------------------------------------------------
    Create the EAP Ext instance if necesary
  -------------------------------------------------------------------------*/ 
  if ( eap_cb[eap_handle].support_eap_ext ) 
  {
    LOG_MSG_ERROR_0("Instance not clean -> support_eap_ext is TRUE");
    eap_cb[eap_handle].support_eap_ext = FALSE;
  }

  if ( eap_info->support_eap_ext ) 
  {
    if( FALSE == eap_ext_create_inst(eap_handle))
    {
      LOG_MSG_ERROR_0("eap_ext_create_inst failed" );
      eap_eap_delete_instance( eap_cb[eap_handle].eap_lyr_handle );
      return -1;
    }
    eap_cb[eap_handle].support_eap_ext = TRUE;
  }

  /*-------------------------------------------------------------------------
    Reserve the EAP instance since everything was successful
  -------------------------------------------------------------------------*/  
  eap_cb[eap_handle].is_assigned = TRUE;

  /*-------------------------------------------------------------------------
    Release lock
  -------------------------------------------------------------------------*/
  PS_LEAVE_CRIT_SECTION( &eap_crit_sect );

  /*-------------------------------------------------------------------------
    Return a handle to the EAP if all required steps were successful
  -------------------------------------------------------------------------*/
  return eap_handle;

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
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_ERROR_0("Invalid EAP instance" );
    return;
  }
  LOG_MSG_INFO1_1("EAP delete instance - EAP handle: %d", eap_handle );
  

  /*-------------------------------------------------------------------------
    Acquire lock to release the EAP instance
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( &eap_crit_sect );

  /*-------------------------------------------------------------------------
    TODO: Indicate to methods that EAP is no longer in service
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Delete the EAP layer
  -------------------------------------------------------------------------*/
  eap_eap_delete_instance( eap_cb[eap_handle].eap_lyr_handle );

  /*-------------------------------------------------------------------------
    Delete the Peer layer
  -------------------------------------------------------------------------*/
  eap_peer_delete_instance( eap_cb[eap_handle].peer_lyr_handle );

  /*-------------------------------------------------------------------------
    Delete the Server layer
  -------------------------------------------------------------------------*/
  eap_server_delete_instance( eap_cb[eap_handle].server_lyr_handle );

  /*-------------------------------------------------------------------------
    If eap extended is supported delete instance
  -------------------------------------------------------------------------*/
  if ( eap_cb[eap_handle].support_eap_ext ) 
  {
    eap_ext_delete_instance( eap_handle );
  }

  /*-------------------------------------------------------------------------
    Delete EAP client config
  -------------------------------------------------------------------------*/
  eap_client_delete_config(eap_handle);

  /*-------------------------------------------------------------------------
    Free the EAP instance
  -------------------------------------------------------------------------*/
  eap_cb[eap_handle].is_assigned = FALSE;

  /*-------------------------------------------------------------------------
    Reset the EAP lyr, Server & Peer lyr handles
  -------------------------------------------------------------------------*/
  eap_cb[eap_handle].eap_lyr_handle    = -1;
  eap_cb[eap_handle].peer_lyr_handle   = -1;
  eap_cb[eap_handle].server_lyr_handle = -1;

  /*-------------------------------------------------------------------------
    Release the EAP lock
  -------------------------------------------------------------------------*/
  PS_LEAVE_CRIT_SECTION( &eap_crit_sect );

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

  LOG_MSG_INFO2_1("eap_input(): called , handle %d", eap_handle );

  /*-------------------------------------------------------------------------
    Pass it to the EAP layer for decoding if handle is valid
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_ERROR_0("eap_input(): Invalid EAP instance" );
    /*-----------------------------------------------------------------------
      Silently discard
    -----------------------------------------------------------------------*/
    dsm_free_packet(sdu);
    return;
  }

  eap_eap_input( eap_cb[eap_handle].eap_lyr_handle, sdu );

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
  eap_role_enum_type role;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  LOG_MSG_INFO2_1("eap_xmit_pkt(): called, eap_handle %d", eap_handle );

  /*-------------------------------------------------------------------------
    If handle is valid, determine whether packet is destined for the peer
    or the authenticator and forward to the appropriate layer
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_ERROR_0("eap_xmit_pkt(): Invalid EAP instance used to Xmit pkt");
    /*-----------------------------------------------------------------------
      Silently discard
    -----------------------------------------------------------------------*/
    dsm_free_packet(sdu);
    return;
  }

  /*-------------------------------------------------------------------------
    Based one the role pass the packet to either server or peer
  -------------------------------------------------------------------------*/
  role = eap_get_role(eap_cb[eap_handle].eap_lyr_handle);
  if( EAP_INVALID_ROLE == role )
  {
    LOG_MSG_ERROR_0("eap_xmit_pkt(): Invalid EAP layer instance");
    dsm_free_packet(sdu);
    return;
  }

  if (EAP_PEER_ROLE == role)
  {
    eap_peer_xmit_pkt(eap_cb[eap_handle].peer_lyr_handle,
                      sdu);
  }

  else
  {
    eap_server_xmit_pkt(eap_cb[eap_handle].server_lyr_handle,
                        sdu);
  }

} /* eap_xmit_pkt() */


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

  /*-------------------------------------------------------------------------
    If handle is valid, pass the indication on to the peer 
    for further processing
    
    NOTE: Only the peer role is currently supported
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_INFO1_0("Invalid EAP instance" );
    return;
  }

  eap_peer_transaction_closing( eap_cb[eap_handle].peer_lyr_handle );

} /* eap_auth_transaction_closing()  */

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
  eap_role_enum_type role;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    If handle is valid, pass the indication on to the peer or authenticator
    for further processing
    
    NOTE: Only the peer role is currently supported
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_INFO1_0("Invalid EAP instance" );
    return;
  }

  role = eap_get_role(eap_cb[eap_handle].eap_lyr_handle);
  if( EAP_INVALID_ROLE == role )
  {
    LOG_MSG_ERROR_0("eap_xmit_pkt(): Invalid EAP layer instance");
    return;
  }

  if (EAP_PEER_ROLE == eap_get_role(eap_cb[eap_handle].eap_lyr_handle))
  {
    eap_peer_auth_complete_ind( eap_cb[eap_handle].peer_lyr_handle,
                                result,
                                pre_master_key,
                                pre_master_key_len );
  }

  else
  {
    eap_server_auth_complete_ind (eap_cb[eap_handle].server_lyr_handle,
                                  result,
                                  pre_master_key,
                                  pre_master_key_len);
  }

} /* eap_auth_complete_ind()  */

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
)
{
  eap_role_enum_type role;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    If handle is valid, pass the indication on to the peer or authenticator
    for further processing
    
    NOTE: Only the peer role is currently supported
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_ERROR_0("Invalid EAP instance" );
    return;
  }

  role = eap_get_role(eap_cb[eap_handle].eap_lyr_handle);
  if( EAP_INVALID_ROLE == role )
  {
    LOG_MSG_ERROR_0("Invalid EAP role");
    return;
  }

  if (EAP_PEER_ROLE == role)
  {
    eap_peer_supp_auth_complete_ind( eap_cb[eap_handle].peer_lyr_handle,
                                     result,
                                     supp_auth_info_mask,
                                     supp_auth_info_size,
                                     supp_auth_info_ptr );
  }

  else if( EAP_AUTHENTICATOR_ROLE == role )
  {
    eap_server_supp_auth_complete_ind( eap_cb[eap_handle].server_lyr_handle,
                                       result,
                                       supp_auth_info_mask,
                                       supp_auth_info_size,
                                       supp_auth_info_ptr );
  }
  
  else
  {
    LOG_MSG_ERROR_1("EAP Role - %d, not supported", role);
    return;
  }

} /* eap_supp_auth_complete_ind()  */

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
  /*-------------------------------------------------------------------------
    If handle is valid, pass the indication on to the peer or authenticator
    for further processing
    
    NOTE: Only the peer role is currently supported
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_ERROR_0("Invalid EAP instance" );

    return;
  }
  LOG_MSG_INFO1_1("Setting authenticator_rsp_required to: %d",
                  authenticator_rsp_required);

  eap_peer_set_authenticator_rsp_required( 
                             eap_cb[eap_handle].peer_lyr_handle,
                             authenticator_rsp_required );

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
  eap_identity_type eap_identity;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   memset(eap_identity.name, 0, EAP_IDENTITY_MAX_CHAR);
   eap_identity.len = 0;
  /*-------------------------------------------------------------------------
    If handle is valid, return the identity of the EAP instance
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_ERROR_0("Invalid EAP instance" );
    eap_identity.len = 0;
    return eap_identity;
  }

  /*If new client return reauth Id*/
  if(((eap_sim_aka_meta_info_type*)eap_cb[
       eap_handle].eap_instance_meta_info_ptr)->new_client == TRUE)
  {
    if((eap_get_reauth_id(
        eap_handle, eap_identity.name, &eap_identity.len))!= -1)
    {
      return eap_identity;
    }   
    if((eap_get_pseudonym_id(eap_handle, eap_identity.name, 
                             &eap_identity.len))!= -1)
    {
      return eap_identity;
    }

  }

  eap_peer_get_identity( eap_cb[eap_handle].peer_lyr_handle,
                         &eap_identity);

  return eap_identity;

} /* eap_get_identity() */


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
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /*-------------------------------------------------------------------------
    If handle is valid, retrieve the authentication protocols supported
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_ERROR_0("Invalid EAP instance" );
    return 0;
  }

  /*-------------------------------------------------------------------------
    For now, only Peer is supported
  -------------------------------------------------------------------------*/
  return eap_peer_get_auth_prot( eap_cb[eap_handle].peer_lyr_handle,
                                 buf);

} /* eap_get_auth_prot()  */


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
)
{
  /*-------------------------------------------------------------------------
    If handle is valid, return the EAP layer handle
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_ERROR_0("Invalid EAP instance" );
    return -1;
  }

  return eap_cb[eap_handle].eap_lyr_handle;

} /* eap_get_eap_lyr_handle() */


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
)
{
  /*-------------------------------------------------------------------------
    If handle is valid, return Peer layer handle
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_ERROR_0("Invalid EAP instance" );
    return -1;
  }

  return eap_cb[eap_handle].peer_lyr_handle;


} /* eap_get_peer_lyr_handle()  */

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
)
{
  /*-------------------------------------------------------------------------
    If handle is valid, return Peer layer handle
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_ERROR_0("Invalid EAP instance" );
    return -1;
  }

  return eap_cb[eap_handle].server_lyr_handle;

} /* eap_get_peer_lyr_handle()  */

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
  return ( (-1 < eap_handle ) && 
           (EAP_MAX_INSTANCES > eap_handle) &&
           (eap_cb[eap_handle].is_assigned == TRUE) )
  ? TRUE : FALSE;

} /* eap_handle_is_valid()  */

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
	eap_identity_type * id_ptr
)
{
	return ( (NULL != id_ptr) &&  
           (0 < id_ptr->len) &&
           (EAP_IDENTITY_MAX_CHAR >= id_ptr->len) ) 
    ? TRUE : FALSE; 
}

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
  /*-------------------------------------------------------------------------
    If handle is valid, store the provided auth_mgr pointer
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_ERROR_0("Invalid EAP instance" );
    return;
  }

  /*-------------------------------------------------------------------------
    Obtain the lock to access EAP control blocks
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( &eap_crit_sect );

  eap_cb[eap_handle].auth_mgr = auth_mgr;

  /*-------------------------------------------------------------------------
    Release lock
  -------------------------------------------------------------------------*/
  PS_LEAVE_CRIT_SECTION( &eap_crit_sect );

} /* eap_set_auth_mgr() */


/*===========================================================================

FUNCTION EAP_GET_AUTH_MGR

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
  void *auth_mgr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    If handle is valid, return the stored auth_mgr pointer
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_ERROR_0("Invalid EAP instance" );

    return NULL;
  }

  /*-------------------------------------------------------------------------
    Obtain the lock to access EAP control blocks
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( &eap_crit_sect );

  auth_mgr = eap_cb[eap_handle].auth_mgr;

  /*-------------------------------------------------------------------------
    Release lock
  -------------------------------------------------------------------------*/
  PS_LEAVE_CRIT_SECTION( &eap_crit_sect );

  return auth_mgr;

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
  /*-------------------------------------------------------------------------
    If handle is valid, store the provided auth_mgr pointer
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_ERROR_0("Invalid EAP instance" );
    return;
  }

  /*-------------------------------------------------------------------------
    Obtain the lock to access EAP control blocks
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( &eap_crit_sect );

  eap_cb[eap_handle].eap_instance_meta_info_ptr = meta_info_ptr;

  /*-------------------------------------------------------------------------
    Release lock
  -------------------------------------------------------------------------*/
  PS_LEAVE_CRIT_SECTION( &eap_crit_sect );

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
  void *meta_info_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    If handle is valid, return the stored auth_mgr pointer
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_INFO1_0("Invalid EAP instance" );
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Obtain the lock to access EAP control blocks
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( &eap_crit_sect );

  meta_info_ptr = eap_cb[eap_handle].eap_instance_meta_info_ptr;

  /*-------------------------------------------------------------------------
    Release lock
  -------------------------------------------------------------------------*/
  PS_LEAVE_CRIT_SECTION( &eap_crit_sect );

  return meta_info_ptr;

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
  uint8 no_identifier = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    If handle is valid, return the identifier of the last EAP pkt
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_ERROR_0("Invalid EAP instance" );

    return no_identifier;
  }

  return eap_eap_get_identifier(eap_handle);

} /* eap_get_identifier() */

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
)
{
  if (TRUE != eap_handle_is_valid(handle))
  {
    LOG_MSG_ERROR_0 ("eap_get_cb_ptr_by_handle: Invalid handle");
    return NULL;
  }
  return &eap_cb[handle];
}

/*===========================================================================

FUNCTION EAP_SET_AUTH_CLIENT_ID

DESCRIPTION
  This function set the client EAP expects with a given version for EAP
  to do client specific configurations. 
  
  Each EAP instance can have a separate client/version combination
  irrespective of the method in use.
  
DEPENDENCIES
  EAP instance should have been created and this function should be
  call only during config time and not during EAP transactions for the
  client specific changes to take affect properly

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void eap_set_auth_client_id
(
  eap_handle_type       eap_handle,
  eap_client_enum_type  eap_client_type,
  uint32                eap_client_enable_spec_mask
)
{
  /*-------------------------------------------------------------------------
    If handle is valid, store the provided auth_mgr pointer
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_ERROR_0("Invalid EAP instance" );
    return;
  }

  /*-------------------------------------------------------------------------
    Initialize the client base configurations
  -------------------------------------------------------------------------*/
  eap_client_config_init(eap_handle, eap_client_type, eap_client_enable_spec_mask);

} /* eap_set_auth_client_id() */
#endif /* FEATURE_DATA_PS_EAP */

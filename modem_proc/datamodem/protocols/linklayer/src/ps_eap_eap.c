/*===========================================================================

                           P S _ E A P _ E A P . C

DESCRIPTION
  This software unit holds the EAP layer implementation of the EAP system.

  The functions exposed by this module are to be called ONLY by the EAP
  functions.


EXTERNALIZED FUNCTIONS


Copyright (c) 2005-2013 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_eap_eap.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/27/13    cx     Replace memcpy and memmove with memscpy and memsmove
10/26/12    fn     Added changes for memory optimization
08/17/12    fn     Cleanup ASSERT() and dsm_pushdown 
05/09/12    op     Added support for the additional meta_identity
03/12/12    scb    Added fix to solve QMISEC errors in eap_eap_input()
12/06/11    scb    Added fix to remove ASSERT() for malformed packets.
08/26/11    op     Added changes to obtain supplementary SIM/AKA auth info
08/18/11    fn     Added test framework feature header files
06/27/11    scb    Added support for start_ind_user_data in the start_ind_cb
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
10/19/10    sa     Made changes for graceful exit in case of dsm exhaustion.
07/30/10    jee    Added retransmission functionality for EAP server
06/02/10   jee/mga Modifications to support EAP server functionality
08/31/09    mga    Merged from eHRPD branch
06/24/09    am     Renamed hton/ntoh macros with ps_ prefix.
03/06/09    scb    Added changes for memory reduction initiative.
01/08/09    scb    Resolved MoB LINT errors
11/10/08    scb    Fixed QTF compilation warnings.
10/21/08    scb    Added function to obtain the reauth id from SFS/EFS
04/14/06    lti    Moved eap_eap_frm_header_type to ps_eap.h
02/27/06    lyr    WLAN 3.0 support for additional auth schemes
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
#include "ps_eapi.h"
#include "ps_eap_sim_aka.h"

#include "dsm.h"
#include "ps_in.h"
#include "msg.h"
#include "secapi.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_system_heap.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================

                    LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/


/*---------------------------------------------------------------------------
  EAP layer information block - this includes information required to
  communicate with client (e.g. 802.1x) as well as information required to
  communicate with Peer layer; it also handles retransmission/duplicate
  detection information
---------------------------------------------------------------------------*/
typedef struct
{

  eap_handle_type         eap_handle;             /* EAP instance handle   */

  boolean                 raw_mode;               /* pass-through?         */

  eap_role_enum_type      eap_role;               /* Peer or Server mode   */

  eap_trp_info_type       trp_info;                /* Info about trp layer  */

  /*-------------------------------------------------------------------------
    This is the identifier of the last request from the authenticator.
    If another packet is received with the same identifier, peer is required
    to retransmit the exact same response as before.
  -------------------------------------------------------------------------*/
  sint15                  last_req_rx_identifier;

  /*-------------------------------------------------------------------------
    This points to the last fully assembled response packet that was sent
    by the peer. This simply needs to be re-sent using the trp layer function
    when a previously processed identifier is detected
  -------------------------------------------------------------------------*/
  dsm_item_type          *last_tx_rsp_pkt;

  /*-------------------------------------------------------------------------
    This points to the last fully assembled request packet that was sent
    by the authenticator. This simply needs to be re-sent using the
    trp layer function when retransmission is required.
    Used only by Authneticator ROLE.
  -------------------------------------------------------------------------*/
  dsm_item_type          *last_tx_req_pkt;

  /*-------------------------------------------------------------------------
    This is the identifier of the last packet sent from Server.
    A random value is choosen initially & it is incremented for each new 
    packet sent. Used only by Authneticator ROLE.
  -------------------------------------------------------------------------*/
  uint8                  last_req_tx_identifier;

} eap_eap_lyr_cb_type;


/*---------------------------------------------------------------------------
  Macro to check if the EAP layer handle is valid
---------------------------------------------------------------------------*/
#define EAP_EAP_HANDLE_IS_VALID( eap_lyr_handle )    \
  ( (eap_lyr_handle != -1) &&                        \
    (eap_lyr_handle < EAP_MAX_INSTANCES) &&          \
    (eap_eap_lyr_cb[eap_lyr_handle] != NULL)         \
    ? TRUE : FALSE )

#define EAP_EAP_INVALID_IDENTIFIER -1

/*---------------------------------------------------------------------------
  EAP layer header
---------------------------------------------------------------------------*/
typedef  eap_frm_hdr_type eap_eap_frm_hdr_type;
/*===========================================================================
                              DATA DEFINITIONS
===========================================================================*/

/*---------------------------------------------------------------------------
  This forms the pool of EAP layer control blocks which can be assigned using
  eap_eap_create_instance()
---------------------------------------------------------------------------*/
static eap_eap_lyr_cb_type* eap_eap_lyr_cb[EAP_MAX_INSTANCES];

/*===========================================================================

                          FORWARD DECLARATIONS FOR MODULE

===========================================================================*/


/*===========================================================================

                          EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


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
)
{
  int eap_lyr_cb_cnt;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Initialize the EAP layer
  -------------------------------------------------------------------------*/
  for(eap_lyr_cb_cnt=0; eap_lyr_cb_cnt<EAP_MAX_INSTANCES; eap_lyr_cb_cnt++ )
  {
    eap_eap_lyr_cb[eap_lyr_cb_cnt] = NULL;
  }

} /* eap_eap_init() */


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
)
{
  eap_eap_handle_type eap_lyr_handle;
  int                 eap_lyr_cb_cnt;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Initialize handle to invalid value
  -------------------------------------------------------------------------*/
  eap_lyr_handle = EAP_INVALID_HANDLE;

  /*-------------------------------------------------------------------------
    Find a free EAP layer control block and reserve it
  -------------------------------------------------------------------------*/
  for( eap_lyr_cb_cnt=0; eap_lyr_cb_cnt<EAP_MAX_INSTANCES; eap_lyr_cb_cnt++ )
  {
    if( NULL == eap_eap_lyr_cb[eap_lyr_cb_cnt] )
    {
      eap_lyr_handle = eap_lyr_cb_cnt;

      eap_eap_lyr_cb[eap_lyr_cb_cnt] = (eap_eap_lyr_cb_type *)
          ps_system_heap_mem_alloc(sizeof(eap_eap_lyr_cb_type));

      if( eap_eap_lyr_cb[eap_lyr_cb_cnt] == NULL )
      {
        LOG_MSG_ERROR_0("Could not allocate EAP layer instance" );
        return EAP_INVALID_HANDLE;
      }
      break;
    }
  }

  if( EAP_INVALID_HANDLE == eap_lyr_handle )
  {
    /*-----------------------------------------------------------------------
      Could not find a free instance
    -----------------------------------------------------------------------*/
    LOG_MSG_ERROR_0("Could not assign EAP layer instance" );
    return EAP_INVALID_HANDLE;
  }

  /*-------------------------------------------------------------------------
    Initialize the EAP layer instance
  -------------------------------------------------------------------------*/
  eap_eap_lyr_cb[eap_lyr_handle]->eap_handle = eap_lyr_info->eap_handle;

  eap_eap_lyr_cb[eap_lyr_handle]->last_req_rx_identifier =
    EAP_EAP_INVALID_IDENTIFIER;

  /*-------------------------------------------------------------------------
    Initialize the identifier used by EAP server to send out packets to a 
    random value. It will be incremented for each new packet sent out
  -------------------------------------------------------------------------*/
  if (E_SUCCESS != secapi_get_random (SECAPI_PSEUDO_RANDOM, 
                     &eap_eap_lyr_cb[eap_lyr_handle]->last_req_tx_identifier,
                     sizeof (uint8)))
  {
     LOG_MSG_ERROR_0("Random value generation failed, using zero" );
     eap_eap_lyr_cb[eap_lyr_handle]->last_req_tx_identifier = 0;
  }

  eap_eap_lyr_cb[eap_lyr_handle]->last_tx_rsp_pkt = NULL;
  eap_eap_lyr_cb[eap_lyr_handle]->last_tx_req_pkt = NULL;

  eap_eap_lyr_cb[eap_lyr_handle]->trp_info = eap_lyr_info->eap_trp_info;

  eap_eap_lyr_cb[eap_lyr_handle]->raw_mode = eap_lyr_info->raw_mode;
  eap_eap_lyr_cb[eap_lyr_handle]->eap_role = eap_lyr_info->eap_role;

  /*-------------------------------------------------------------------------
    Return a handle if all required steps were successful
  -------------------------------------------------------------------------*/
  return eap_lyr_handle;

} /* eap_eap_create_instance()  */


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
)
{
  /*-------------------------------------------------------------------------
    Free the EAP instance
  -------------------------------------------------------------------------*/
  if( TRUE == EAP_EAP_HANDLE_IS_VALID( eap_lyr_handle ) )
  {
    if( eap_eap_lyr_cb[eap_lyr_handle]->last_tx_rsp_pkt != NULL )
    {
      dsm_free_packet( &eap_eap_lyr_cb[eap_lyr_handle]->last_tx_rsp_pkt );
      eap_eap_lyr_cb[eap_lyr_handle]->last_tx_rsp_pkt = NULL;
    }

    if( eap_eap_lyr_cb[eap_lyr_handle]->last_tx_req_pkt != NULL )
    {
      dsm_free_packet( &eap_eap_lyr_cb[eap_lyr_handle]->last_tx_req_pkt );
      eap_eap_lyr_cb[eap_lyr_handle]->last_tx_req_pkt = NULL;
    }

    eap_eap_lyr_cb[eap_lyr_handle]->eap_handle = EAP_INVALID_HANDLE;

    PS_SYSTEM_HEAP_MEM_FREE(eap_eap_lyr_cb[eap_lyr_handle]);
  }

} /* eap_eap_delete_instance()  */


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
)
{
  eap_eap_frm_hdr_type eap_frm_hdr;
  uint32               bytes_read = 0;
  uint16               rx_bytes;
  uint16               payload_length;
  uint32               packet_len = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( NULL == sdu )
  {
    LOG_MSG_ERROR_0("Invalid input");
    return;
  }

  /*-------------------------------------------------------------------------
    Check for handle validity and retrieve EAP layer handle
  -------------------------------------------------------------------------*/
  if( FALSE == EAP_EAP_HANDLE_IS_VALID( eap_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid EAP layer instance" );

    dsm_free_packet( sdu );
    return;
  }

  /*-------------------------------------------------------------------------
    If in raw mode, pass the packet straight to peer:
    for now, raw mode assumes PEER role; otherwise, we need to peek and
    determine where it is going
  -------------------------------------------------------------------------*/
  if( TRUE == eap_eap_lyr_cb[eap_lyr_handle]->raw_mode )
  {
    /*-----------------------------------------------------------------------
       Pass the MIN code so that Peer should not even process it except in
       raw mode
    -----------------------------------------------------------------------*/
    LOG_MSG_INFO2_0("eap_eap_input(): processing pkt in RAW mode");
    if (EAP_PEER_ROLE == eap_eap_lyr_cb[eap_lyr_handle]->eap_role)
    {
      eap_peer_input(eap_eap_lyr_cb[eap_lyr_handle]->eap_handle,
                     EAP_MIN_CODE,
                     sdu);
      return;
    }
    else if (EAP_AUTHENTICATOR_ROLE == eap_eap_lyr_cb[eap_lyr_handle]->eap_role)
    {
      eap_server_input(eap_eap_lyr_cb[eap_lyr_handle]->eap_handle,
                       EAP_MIN_CODE,
                       sdu);
      return;
    }
  }

  /*-------------------------------------------------------------------------
    Retrieve the header from the DSM item
  -------------------------------------------------------------------------*/
  bytes_read = dsm_pullup( sdu,
                           (void *)&eap_frm_hdr,
                           sizeof(eap_eap_frm_hdr_type) );

  /*-----------------------------------------------------------------------
    Verify bytes_read for overflows.
  -------------------------------------------------------------------------*/

  if( bytes_read > EAP_DEFAULT_MTU_SIZE )
  {
    LOG_MSG_ERROR_1("eap_eap_input(): packet_len %d exceeds the"
                  " Minimum EAP MTU size without header addition"
                    " in the absence of fragmentation support", bytes_read);
    dsm_free_packet(sdu);
    return;
  }

  if( sizeof(eap_eap_frm_hdr_type) > bytes_read )
  {
    LOG_MSG_ERROR_0("eap_eap_input(): Malformed packet received" );
    dsm_free_packet(sdu);
    return;
  }

  /*-------------------------------------------------------------------------
    Convert the values to host byte order for processing

    Length is the only value of concern (uint16)
  -------------------------------------------------------------------------*/

  eap_frm_hdr.length = ps_ntohs( eap_frm_hdr.length );

  /*-----------------------------------------------------------------------
    Verify contents of eap_frm_hdr length for overflows
  -------------------------------------------------------------------------*/

  if( eap_frm_hdr.length < sizeof(eap_eap_frm_hdr_type) )
  {
    LOG_MSG_ERROR_1("eap_eap_input(): eap_frm_hdr.length %d is lower than the"
                  " eap frame header length. Malformed packet received.",
                    eap_frm_hdr.length);
    dsm_free_packet(sdu);
    return;
  }

  /*-------------------------------------------------------------------------
    Per RFC :

    If Length > number of bytes received, silently discard packet

    If Length < number of bytes received, ignore additional bytes as padding

    Note: length includes EAP header: adjust for this
  -------------------------------------------------------------------------*/

  payload_length = eap_frm_hdr.length - sizeof(eap_eap_frm_hdr_type);

  packet_len = dsm_length_packet(*sdu);

  /*-----------------------------------------------------------------------
    Verify packet length for overflows
  -------------------------------------------------------------------------*/

  if( packet_len > EAP_DEFAULT_MTU_SIZE || 
      payload_length > EAP_DEFAULT_MTU_SIZE )
  {
    LOG_MSG_ERROR_2("eap_eap_input(): packet_len %d or payload length %d"
                  " exceeds the Minimum EAP MTU size without header addition"
                  " in the absence of fragmentation support",
                    packet_len, payload_length);
    dsm_free_packet(sdu);
    return;
  }

  rx_bytes = (uint16)packet_len;

  if( payload_length > rx_bytes )
  {
    LOG_MSG_ERROR_2("eap_eap_input(): Ignoring malformed EAP packet :"
                  " payload_length %d rx_bytes %d",
                    payload_length, rx_bytes );

    dsm_free_packet( sdu );
    return;
  }
  else if( payload_length < rx_bytes )
  {
    uint16 bytes_removed;

    /*-----------------------------------------------------------------------
      The following assumes that pullup will remove bytes even if a NULL
      buf is passed in
    -----------------------------------------------------------------------*/

    LOG_MSG_INFO2_0("eap_eap_input(): Removing data link padding from"
                    " EAP packet" );
    bytes_removed = dsm_pullup_tail( sdu,
                                     NULL,
                                     (rx_bytes - payload_length) );

    if( (rx_bytes - payload_length) != bytes_removed)
    {
      LOG_MSG_ERROR_2("dsm_pullup returned %d, exp %d",
                      bytes_removed, rx_bytes - payload_length);
      dsm_free_packet( sdu );
      return;
    }
  }

  /*-------------------------------------------------------------------------
    Ensure the EAP code is appropriate for the current EAP role
  -------------------------------------------------------------------------*/
  if (EAP_PEER_ROLE  == eap_get_role(eap_lyr_handle))
  {
    if (!((uint8)EAP_REQUEST_CODE == eap_frm_hdr.code) &&
        !((uint8)EAP_SUCCESS_CODE == eap_frm_hdr.code) &&
        !((uint8)EAP_FAILURE_CODE == eap_frm_hdr.code))
    {    
      LOG_MSG_INFO2_2("eap_eap_input():PEER Role Unexpected EAP CODE %d rcvd,"
                      " id %d. Freeing pkt",
                      eap_frm_hdr.code, eap_frm_hdr.identifier);
      dsm_free_packet( sdu );
      return;
    }
  }

  else if (EAP_AUTHENTICATOR_ROLE == eap_get_role(eap_lyr_handle))
  {
    if (!((uint8)EAP_RESPONSE_CODE == eap_frm_hdr.code))
    {    
      LOG_MSG_INFO2_2("eap_eap_input():Server Role Unexpected EAP CODE %d rcvd,"
                      " id %d. Freeing pkt",
                      eap_frm_hdr.code, eap_frm_hdr.identifier);
      dsm_free_packet( sdu );
      return;
    }
  }

  /*-------------------------------------------------------------------------
    We have a good EAP packet to deal with at this point - process the EAP
    header
  -------------------------------------------------------------------------*/
  switch (eap_frm_hdr.code)
  {
    case EAP_REQUEST_CODE:
      LOG_MSG_INFO2_1("eap_eap_input(): EAP_REQUEST rcvd, id %d",
                      eap_frm_hdr.identifier);

      /*---------------------------------------------------------------------
        Retransmit if required
      ---------------------------------------------------------------------*/
      if( eap_eap_lyr_cb[eap_lyr_handle]->last_req_rx_identifier ==
          (sint15)eap_frm_hdr.identifier )
      {
        LOG_MSG_INFO2_0("eap_eap_input(): EAP_REQUEST is duplicate");
        if( NULL != eap_eap_lyr_cb[eap_lyr_handle]->last_tx_rsp_pkt )
        {
          /*-----------------------------------------------------------------
            EAP Method has already processed this Request : re-transmit
          -----------------------------------------------------------------*/
          dsm_item_type  *tx_pkt = NULL;
          uint32          dup_bytes = 0;
          uint16          last_tx_rsp_pkt_len = 0;
          LOG_MSG_INFO2_0("eap_eap_input(): retransmitting response to "
                          "EAP_REQUEST");

          /*-----------------------------------------------------------------
            Duplicate packet and send packet directly to link layer
          -----------------------------------------------------------------*/
          last_tx_rsp_pkt_len = (uint16)dsm_length_packet(
                       eap_eap_lyr_cb[eap_lyr_handle]->last_tx_rsp_pkt);
          dup_bytes = dsm_dup_packet( &tx_pkt,
                         eap_eap_lyr_cb[eap_lyr_handle]->last_tx_rsp_pkt,
                         0,
                         last_tx_rsp_pkt_len);

          if( dup_bytes != last_tx_rsp_pkt_len)
          {
            LOG_MSG_ERROR_0("dsm_dup_packet failed");
            dsm_free_packet( sdu );
            return;
          }

          eap_eap_lyr_cb[eap_lyr_handle]->trp_info.
            trp_tx_f_ptr( eap_eap_lyr_cb[eap_lyr_handle]->trp_info.
                          trp_tx_user_data,
                          &tx_pkt );

          /*-----------------------------------------------------------------
            No need to send the packet to peer - free and bail out
          -----------------------------------------------------------------*/
        }
        else
        {
          /*-----------------------------------------------------------------
            Previous packet is already pending with EAP Method: silently
            discard
          -----------------------------------------------------------------*/
          LOG_MSG_ERROR_0("Authenticator re-txmit before EAP method response" );
        }
        dsm_free_packet( sdu );
        break;
      }

      /*---------------------------------------------------------------------
        New Request from Authenticator

        Store the identifier of the request for retransmission purposes
      ---------------------------------------------------------------------*/
      eap_eap_lyr_cb[eap_lyr_handle]->last_req_rx_identifier =
        (sint15)eap_frm_hdr.identifier;

      /*---------------------------------------------------------------------
        This means the server received our last response if any - free it
        if necessary
      ---------------------------------------------------------------------*/
      if( NULL != eap_eap_lyr_cb[eap_lyr_handle]->last_tx_rsp_pkt )
      {
        dsm_free_packet( &eap_eap_lyr_cb[eap_lyr_handle]->last_tx_rsp_pkt );
        eap_eap_lyr_cb[eap_lyr_handle]->last_tx_rsp_pkt  = NULL;
      }

      eap_peer_input( eap_eap_lyr_cb[eap_lyr_handle]->eap_handle,
                      (eap_code_enum_type) eap_frm_hdr.code,
                      sdu);
      break;

    case EAP_SUCCESS_CODE:
    case EAP_FAILURE_CODE:
      LOG_MSG_INFO2_2("eap_eap_input(): EAP_SUCCESS/EAP_FAILURE (code %d) rcvd,"
                      "id %d", eap_frm_hdr.code, eap_frm_hdr.identifier);

      /*---------------------------------------------------------------------
        This means the server received our last response if any - free it
        if necessary
      ---------------------------------------------------------------------*/
      if( NULL != eap_eap_lyr_cb[eap_lyr_handle]->last_tx_rsp_pkt )
      {
        LOG_MSG_INFO2_0("eap_eap_input(): freeing last_tx_rsp_pkt");
        dsm_free_packet( &eap_eap_lyr_cb[eap_lyr_handle]->last_tx_rsp_pkt );
      }

      /*---------------------------------------------------------------------
        Success/failure should be passed up directly to Peer
      ---------------------------------------------------------------------*/
      eap_peer_input( eap_eap_lyr_cb[eap_lyr_handle]->eap_handle,
                      (eap_code_enum_type) eap_frm_hdr.code,
                      sdu);
      break;

    case EAP_RESPONSE_CODE:
      LOG_MSG_INFO2_1( "eap_eap_input(): EAP_RESPONSE rcvd, id %d. ",
                       eap_frm_hdr.identifier);

      if( eap_eap_lyr_cb[eap_lyr_handle]->last_req_tx_identifier !=
          eap_frm_hdr.identifier )
      {
        LOG_MSG_ERROR_1("eap_eap_input(): EAP_RESPONSE does not match last "
                      "outstanding request,id:%d Freeing packet",
                        eap_frm_hdr.identifier);
        dsm_free_packet( sdu );
        break;
      }

      /*---------------------------------------------------------------------
        New Response from Peer. Increment the last request identifier so that
        we can identify a duplicate packet also 
      ---------------------------------------------------------------------*/
      eap_eap_lyr_cb[eap_lyr_handle]->last_req_tx_identifier++;

      /*---------------------------------------------------------------------
        This means the peer received our last request - free it
        if necessary
      ---------------------------------------------------------------------*/
      if( NULL != eap_eap_lyr_cb[eap_lyr_handle]->last_tx_req_pkt )
      {
        dsm_free_packet( &eap_eap_lyr_cb[eap_lyr_handle]->last_tx_req_pkt );
        eap_eap_lyr_cb[eap_lyr_handle]->last_tx_req_pkt  = NULL;
      }

      eap_server_input(eap_eap_lyr_cb[eap_lyr_handle]->eap_handle,
                      (eap_code_enum_type) eap_frm_hdr.code,
                       sdu);
      break;

    default:
      LOG_MSG_ERROR_2("eap_eap_input(): EAP pkt with code %d rcvd, id %d"
	              " and Ignoring EAP packet not destined for Peer",
		      eap_frm_hdr.code, eap_frm_hdr.identifier);
      dsm_free_packet( sdu );
      break;

  }

} /* eap_eap_input()  */


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
)
{
  eap_eap_handle_type  eap_lyr_handle;
  eap_eap_frm_hdr_type eap_frm_hdr;
  uint16               write_bytes;
  uint16               dup_bytes;
  uint16               sdu_length;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( NULL == sdu )
  {
    LOG_MSG_ERROR_0("eap_eap_xmit_pkt: Invalid input" );
    return;
  }

  /*-------------------------------------------------------------------------
    Check for handle validity and retrieve EAP layer handle
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_ERROR_0("Invalid EAP instance" );
    dsm_free_packet( sdu );
    return;
  }

  eap_lyr_handle = eap_get_eap_lyr_handle(eap_handle);

  if( FALSE == EAP_EAP_HANDLE_IS_VALID( eap_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid EAP layer instance" );
    dsm_free_packet( sdu );
    return;
  }

  /*-------------------------------------------------------------------------
    If in raw mode, pass the packet straight down to client
  -------------------------------------------------------------------------*/
  if( TRUE == eap_eap_lyr_cb[eap_lyr_handle]->raw_mode )
  {
    /*-----------------------------------------------------------------------
      Pass the packet to the lower layers
    -----------------------------------------------------------------------*/
    eap_eap_lyr_cb[eap_lyr_handle]->trp_info.
      trp_tx_f_ptr( eap_eap_lyr_cb[eap_lyr_handle]->trp_info.trp_tx_user_data,
                    sdu );

    return;
  }

  /*-------------------------------------------------------------------------
    Check the EAP code based on whehter EAP is acting as Server or Peer. 
      Peer   - Response packets
      Server - Request, Success or Failure packets
    Also fill in the identifier value based on Server or Peer
      Peer   - Value same as the request packet from Server
      Server - Init to Random value, incremented for each new packet sent out
  -------------------------------------------------------------------------*/
  if (EAP_PEER_ROLE == eap_eap_lyr_cb[eap_lyr_handle]->eap_role)
  {
    if (EAP_RESPONSE_CODE != code)
    {
      LOG_MSG_ERROR_1 ("Invalid EAP code %d", code );
      dsm_free_packet( sdu );
      return;
    }

    if ( 0 > eap_eap_lyr_cb[eap_lyr_handle]->last_req_rx_identifier )
    {
      LOG_MSG_ERROR_0("Invalid received request identifier");
      dsm_free_packet( sdu );
      return;
    }

    eap_frm_hdr.identifier 
        = (uint8) eap_eap_lyr_cb[eap_lyr_handle]->last_req_rx_identifier;
  }

  else if (EAP_AUTHENTICATOR_ROLE == eap_eap_lyr_cb[eap_lyr_handle]->eap_role)
  {
    if (!(EAP_REQUEST_CODE == code) && !(EAP_SUCCESS_CODE == code)
        && !(EAP_FAILURE_CODE == code))
    {
      LOG_MSG_ERROR_1 ("Invalid EAP code %d", code );
      dsm_free_packet( sdu );
      return;
    }

    eap_frm_hdr.identifier 
                   = eap_eap_lyr_cb[eap_lyr_handle]->last_req_tx_identifier;
  }

  else
  {
    LOG_MSG_ERROR_1 ("Invalid EAP role %d",
                     eap_eap_lyr_cb[eap_lyr_handle]->eap_role );
    dsm_free_packet( sdu );
    return;
  }

  /*-------------------------------------------------------------------------
    Fill in the remianing headers of the EAP packet

    Per RFC:
    Length: Length of data + EAP header
  -------------------------------------------------------------------------*/
  sdu_length = (uint16)dsm_length_packet(*sdu);
  eap_frm_hdr.code = (uint8)code;

  eap_frm_hdr.length = sdu_length + sizeof(eap_eap_frm_hdr_type);

  /*-------------------------------------------------------------------------
    Convert to network-byte ordering before forming the packet:

    Length is the only value of concern (uint16)
  -------------------------------------------------------------------------*/
  eap_frm_hdr.length = ps_htons( eap_frm_hdr.length );

  write_bytes = dsm_pushdown_packed(sdu,
                                    (void *)&eap_frm_hdr,
                                    sizeof(eap_eap_frm_hdr_type),
                                    DSM_DS_SMALL_ITEM_POOL );

  if( sizeof(eap_eap_frm_hdr_type) != write_bytes )
  {
    LOG_MSG_INFO1_0("Failed to pushdown EAP header" );

    dsm_free_packet( sdu );
    return;
  }

  sdu_length = (uint16)dsm_length_packet(*sdu);
  /*-------------------------------------------------------------------------
    Make sure there is no previously stored packet. Copy the packet before 
    sending it out.
  -------------------------------------------------------------------------*/
  if (EAP_PEER_ROLE == eap_eap_lyr_cb[eap_lyr_handle]->eap_role)
  {
    if (NULL != eap_eap_lyr_cb[eap_lyr_handle]->last_tx_rsp_pkt)
    {
      LOG_MSG_ERROR_0("Last response packet not freed" );
      dsm_free_packet (&(eap_eap_lyr_cb[eap_lyr_handle]->last_tx_rsp_pkt));
      eap_eap_lyr_cb[eap_lyr_handle]->last_tx_rsp_pkt = NULL;
      dsm_free_packet( sdu );
      return;
    }

    dup_bytes = dsm_dup_packet(
                            &eap_eap_lyr_cb[eap_lyr_handle]->last_tx_rsp_pkt,
                            *sdu,
                             0,
                             sdu_length );

    if (sdu_length != dup_bytes)
    {
      LOG_MSG_INFO1_0("Failed to duplicate EAP header" );
      dsm_free_packet(&(eap_eap_lyr_cb[eap_lyr_handle]->last_tx_rsp_pkt));
      eap_eap_lyr_cb[eap_lyr_handle]->last_tx_rsp_pkt = NULL;
      dsm_free_packet( sdu );
      return;
    }
  }

  else if (EAP_AUTHENTICATOR_ROLE == eap_eap_lyr_cb[eap_lyr_handle]->eap_role)
  {
    if (NULL != eap_eap_lyr_cb[eap_lyr_handle]->last_tx_req_pkt)
    {
      LOG_MSG_ERROR_0("Last request packet not freed" );
      dsm_free_packet (&(eap_eap_lyr_cb[eap_lyr_handle]->last_tx_req_pkt));
      eap_eap_lyr_cb[eap_lyr_handle]->last_tx_req_pkt = NULL;
      dsm_free_packet( sdu );
      return;
    }

    dup_bytes = dsm_dup_packet(
                            &eap_eap_lyr_cb[eap_lyr_handle]->last_tx_req_pkt,
                            *sdu,
                             0,
                             sdu_length );

    if (sdu_length != dup_bytes)
    {
      LOG_MSG_INFO1_0("Failed to duplicate EAP header" );
      dsm_free_packet( sdu );
      return;
    }
  }

  else
  {
    LOG_MSG_ERROR_1 ("Invalid EAP role %d",
                     eap_eap_lyr_cb[eap_lyr_handle]->eap_role );
    dsm_free_packet(&(eap_eap_lyr_cb[eap_lyr_handle]->last_tx_req_pkt));
    eap_eap_lyr_cb[eap_lyr_handle]->last_tx_req_pkt = NULL;
    dsm_free_packet( sdu );
    return;
  }

  /*-------------------------------------------------------------------------
    Pass the packet to the lower layers
  -------------------------------------------------------------------------*/
  eap_eap_lyr_cb[eap_lyr_handle]->trp_info.
    trp_tx_f_ptr( eap_eap_lyr_cb[eap_lyr_handle]->trp_info.trp_tx_user_data,
                  sdu );

} /* eap_eap_xmit_pkt() */

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
)
{
  eap_eap_handle_type    eap_lyr_handle;
  dsm_item_type         *sdu = NULL;
  uint16                 req_pkt_length;
  uint16                 dup_bytes;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check for handle validity and retrieve EAP layer handle
  -------------------------------------------------------------------------*/
  eap_lyr_handle = eap_get_eap_lyr_handle(eap_handle);
  if( FALSE == EAP_EAP_HANDLE_IS_VALID( eap_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid EAP layer instance" );
    return;
  }

  if (NULL == eap_eap_lyr_cb[eap_lyr_handle]->last_tx_req_pkt)
  {
    LOG_MSG_ERROR_0("Cannot retransmit, Last transmitted request packet"
                    " is not stored");
    return;
  }

  /*-------------------------------------------------------------------------
    Duplicate the last EAP request packet
  -------------------------------------------------------------------------*/
  req_pkt_length = (uint16)dsm_length_packet(
                           eap_eap_lyr_cb[eap_lyr_handle]->last_tx_req_pkt);

  dup_bytes = dsm_dup_packet(&sdu,
                             eap_eap_lyr_cb[eap_lyr_handle]->last_tx_req_pkt,
                             0,
                             req_pkt_length );

  LOG_MSG_INFO1_2("eap_eap_rexmit_req_pkt(): req_pkt_length = %d, dup_bytes = %d",
                  req_pkt_length, dup_bytes);
  if ( (0 == req_pkt_length) || (req_pkt_length != dup_bytes) )
  {
    LOG_MSG_ERROR_0("Cannot retransmit, failed to duplicate EAP req packet");
    dsm_free_packet(&sdu);
    return;
  }

  /*-------------------------------------------------------------------------
    Pass the packet to the lower layers for re-transmission
  -------------------------------------------------------------------------*/
  eap_eap_lyr_cb[eap_lyr_handle]->trp_info.
    trp_tx_f_ptr(eap_eap_lyr_cb[eap_lyr_handle]->trp_info.trp_tx_user_data,
                 &sdu);

} /* eap_eap_rexmit_req_pkt() */

/*===========================================================================

FUNCTION EAP_EAP_AUTH_COMPLETE_IND

DESCRIPTION
  This function is called by the peer layer to notify EAP layer that the
  authentication result indication AS WELL as an EAP Success/Failure have
  been received.

DEPENDENCIES
  EAP layer instance must have been created

  NOTE: No further EAP processing should be done: some EAP clients are
  known to delete instance when auth complete ind is called!!

RETURN VALUE
  None.

SIDE EFFECTS
  Calls client's authentication completed callback.

===========================================================================*/
void eap_eap_auth_complete_ind
(
  eap_handle_type          eap_handle,
  eap_result_enum_type     result,
  uint8                   *pre_master_key,
  uint16                   pre_master_key_len
)
{
  eap_eap_handle_type eap_lyr_handle;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check for handle validity and retrieve the EAP layer handle
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_INFO1_0("Invalid EAP instance" );

    return;
  }

  eap_lyr_handle = eap_get_eap_lyr_handle(eap_handle);

  if( FALSE == EAP_EAP_HANDLE_IS_VALID( eap_lyr_handle ) )
  {
    LOG_MSG_INFO1_0("Invalid EAP layer instance" );

    return;
  }

  /*-------------------------------------------------------------------------
      Reset the EAP layer for next authentication sequence
  -------------------------------------------------------------------------*/
  eap_eap_lyr_cb[eap_lyr_handle]->last_req_rx_identifier =
    EAP_EAP_INVALID_IDENTIFIER;

  if (E_SUCCESS != secapi_get_random (SECAPI_PSEUDO_RANDOM, 
                     &eap_eap_lyr_cb[eap_lyr_handle]->last_req_tx_identifier,
                     sizeof (uint8)))
  {
     LOG_MSG_ERROR_0("Random value generation failed, using zero" );
     eap_eap_lyr_cb[eap_lyr_handle]->last_req_tx_identifier = 0;
  }

  if( NULL != eap_eap_lyr_cb[eap_lyr_handle]->last_tx_rsp_pkt )
  {
    dsm_free_packet( &eap_eap_lyr_cb[eap_lyr_handle]->last_tx_rsp_pkt );
    eap_eap_lyr_cb[eap_lyr_handle]->last_tx_rsp_pkt = NULL;
  }

  if( NULL != eap_eap_lyr_cb[eap_lyr_handle]->last_tx_req_pkt )
  {
    dsm_free_packet( &eap_eap_lyr_cb[eap_lyr_handle]->last_tx_req_pkt );
    eap_eap_lyr_cb[eap_lyr_handle]->last_tx_req_pkt = NULL;
  }

  /*-------------------------------------------------------------------------
    Inform the client (e.g. 802.1x) of the authentication result
  -------------------------------------------------------------------------*/
  eap_eap_lyr_cb[eap_lyr_handle]->trp_info.
    result_ind_f_ptr(eap_handle,
                eap_eap_lyr_cb[eap_lyr_handle]->trp_info.result_ind_user_data,
                     result,
                     pre_master_key,
                     pre_master_key_len);

} /* eap_eap_auth_complete_ind()  */

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
)
{
  eap_eap_handle_type eap_lyr_handle;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  LOG_MSG_INFO1_0("eap_eap_supp_auth_complete_ind(): called" );

  /*-------------------------------------------------------------------------
    Check for handle validity and retrieve the EAP layer handle
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_ERROR_0("Invalid EAP instance" );
    return;
  }

  eap_lyr_handle = eap_get_eap_lyr_handle(eap_handle);

  if( FALSE == EAP_EAP_HANDLE_IS_VALID( eap_lyr_handle ) )
  {
    LOG_MSG_ERROR_0("Invalid EAP layer instance" );
    return;
  }

  if( FALSE == eap_eap_supp_result_f_ind_ptr_is_valid(eap_handle) )
  {
    LOG_MSG_ERROR_0("Client not registered for supplementary result" );
    return;
  }

  /*-------------------------------------------------------------------------
    Inform the client of the supplementary authentication info
  -------------------------------------------------------------------------*/
  eap_eap_lyr_cb[eap_lyr_handle]->trp_info. 
    supp_result_ind_f_ptr(
         eap_handle,
         eap_eap_lyr_cb[eap_lyr_handle]->trp_info.supp_result_ind_user_data,
         result,
         supp_auth_info_mask,
         supp_auth_info_size,
         supp_auth_info_ptr );

} /* eap_eap_supp_auth_complete_ind() */

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
)
{
  eap_eap_handle_type eap_lyr_handle;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Check for handle validity and retrieve the EAP layer handle
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_INFO1_0("Invalid EAP instance" );
    return FALSE;
  }

  eap_lyr_handle = eap_get_eap_lyr_handle(eap_handle);

  if( FALSE == EAP_EAP_HANDLE_IS_VALID( eap_lyr_handle ) )
  {
    LOG_MSG_INFO1_0("Invalid EAP layer instance" );
    return FALSE;
  }

  if( NULL != eap_eap_lyr_cb[eap_lyr_handle]->trp_info.supp_result_ind_f_ptr )
  {
    return TRUE;
  }
  else
  {  
    return FALSE;
  }

} /* eap_eap_supp_result_f_ind_ptr_is_valid() */

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
)
{
  uint8 no_identifier = 0;
  eap_eap_handle_type eap_lyr_handle;

  /*-------------------------------------------------------------------------
  Check for handle validity and retrieve the identifier of the last EAP pkt
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_INFO1_0("Invalid EAP instance" );
    return no_identifier;
  }

  eap_lyr_handle = eap_get_eap_lyr_handle(eap_handle);

  if( FALSE == EAP_EAP_HANDLE_IS_VALID( eap_lyr_handle ) )
  {
    LOG_MSG_INFO1_0("Invalid EAP layer instance" );
    return no_identifier;
  }

  if ( 0 > eap_eap_lyr_cb[eap_lyr_handle]->last_req_rx_identifier )
  {
    LOG_MSG_ERROR_0("Invalid received request identifier");
    return no_identifier;
  }

  return (uint8)eap_eap_lyr_cb[eap_lyr_handle]->last_req_rx_identifier;

} /* eap_eap_get_identifier() */

/*===========================================================================

FUNCTION EAP_GET_REAUTH_ID

DESCRIPTION
  This function is called by a method to request the re-authentication 
  identifier of the last EAP packet

DEPENDENCIES
  EAP instance must have been created

RETURN VALUE
  Last EAP pkt identifier.

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

  eap_eap_handle_type eap_lyr_handle;
  int                 result = -1;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check for handle validity and retrieve the EAP layer handle
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_INFO1_0("Invalid EAP instance" );

    return result;
  }

  eap_lyr_handle = eap_get_eap_lyr_handle(eap_handle);

  if( FALSE == EAP_EAP_HANDLE_IS_VALID( eap_lyr_handle ) )
  {
    LOG_MSG_INFO1_0("Invalid EAP layer instance" );
    return result;
  }

  /*-------------------------------------------------------------------------
    Inform the client (e.g. 802.1x) of the authentication result
  -------------------------------------------------------------------------*/
  result = eap_peer_get_reauth_id( 
           eap_eap_lyr_cb[eap_lyr_handle]->eap_handle, data, max_data_len);
  LOG_MSG_INFO2_1("eap_get_reauth_id(): Obtain the reauth id : %d", result);

  return result;

}


/*===========================================================================

FUNCTION EAP_GET_PSEUDONYM_ID

DESCRIPTION
  This function is called by a method to request the pseudonym
  identifier of the last EAP packet

DEPENDENCIES
  EAP instance must have been created

RETURN VALUE
  Last EAP pkt pseudonym identifier.

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

  eap_eap_handle_type eap_lyr_handle;
  int                 result = -1;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check for handle validity and retrieve the EAP layer handle
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_INFO1_0("Invalid EAP instance" );

    return result;
  }

  eap_lyr_handle = eap_get_eap_lyr_handle(eap_handle);

  if( FALSE == EAP_EAP_HANDLE_IS_VALID( eap_lyr_handle ) )
  {
    LOG_MSG_INFO1_0("Invalid EAP layer instance" );
    return result;
  }

  /*-------------------------------------------------------------------------
    Inform the client (e.g. 802.1x) of the authentication result
  -------------------------------------------------------------------------*/
  result = eap_peer_get_pseudonym_id( 
           eap_eap_lyr_cb[eap_lyr_handle]->eap_handle, data, max_data_len);
  
  LOG_MSG_INFO2_1("eap_get_pseudonym_id(): Obtain the pseudonym id : %d", result);
  return result;

} /*eap_get_pseudonym_id*/

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
  eap_handle_type  eap_handle
)
{
  eap_identity_type           eap_identity;
  eap_sim_aka_meta_info_type *meta_info_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  memset(&eap_identity, 0, sizeof(eap_identity));

  /*-------------------------------------------------------------------------
    If handle is valid, return the identity of the EAP instance
  -------------------------------------------------------------------------*/
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_ERROR_0("Invalid EAP instance" );
    return eap_identity;
  }

  meta_info_ptr = (eap_sim_aka_meta_info_type*) eap_get_meta_info(eap_handle);

  if ( NULL == meta_info_ptr )
  {
    LOG_MSG_ERROR_0("Invalid meta_info_ptr" );
    return eap_identity;
  }

  eap_identity.len = meta_info_ptr->eap_meta_identity.len;
  memscpy(eap_identity.name,
         EAP_IDENTITY_MAX_CHAR,
         meta_info_ptr->eap_meta_identity.name,
         meta_info_ptr->eap_meta_identity.len);

  return eap_identity;

} /*eap_get_meta_identity*/

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
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( FALSE == EAP_EAP_HANDLE_IS_VALID(eap_lyr_handle) )
  {
    LOG_MSG_ERROR_0("Invalid EAP layer instance" );
    return EAP_INVALID_ROLE;
  }

  return eap_eap_lyr_cb[eap_lyr_handle]->eap_role;
}

/*===========================================================================
FUNCTION EAP_START_IND

DESCRIPTION
  This function is called by the lower layer (ex: LAN 1.x) to start the 
  identity phase by the EAP server

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
int eap_start_ind 
(
  eap_handle_type eap_handle
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( FALSE == eap_handle_is_valid(eap_handle) )
  {
    LOG_MSG_INFO1_0("eap_start_ind: Invalid EAP instance" );
    return -1;
  }

  return eap_server_identity_req(eap_handle);
}/*eap_start_ind */


/*===========================================================================
FUNCTION EAP_CLIENT_START_IND

DESCRIPTION
  This function is to indicate that the EAP identity stage is complete and  
  that the backend authnetication server can start the authentication stage. 
  This will call the corresponding client's callback.

DEPENDENCIES
  EAP instance must have been created

PARAMETERS
  eap_handle - EAP instance handle

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void eap_client_start_ind 
(
  eap_handle_type eap_handle
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  eap_eap_handle_type eap_lyr_handle;

  /*-------------------------------------------------------------------------
    Get the eap_eap_lyr handle
  -------------------------------------------------------------------------*/
  eap_lyr_handle = eap_get_eap_lyr_handle(eap_handle);

  if( FALSE == EAP_EAP_HANDLE_IS_VALID(eap_lyr_handle) )
  {
    LOG_MSG_ERROR_0("Invalid EAP instance");
    return ;
  }

  LOG_MSG_INFO2_1("In eap_client_start_ind eap_lyr_handle %d", eap_lyr_handle);
    
  if( eap_eap_lyr_cb[eap_lyr_handle]->
      trp_info.eap_client_start_ind_cb == NULL)
  {
    LOG_MSG_ERROR_0("eap_client_start_ind_cb is NULL");
  }

  else
  {
    eap_eap_lyr_cb[eap_lyr_handle]->trp_info.eap_client_start_ind_cb(
      eap_handle, 
      eap_eap_lyr_cb[eap_lyr_handle]->trp_info.start_ind_user_data  );
  }

  return;
}/*eap_client_start_ind */

#endif /* FEATURE_DATA_PS_EAP */

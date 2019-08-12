/*===========================================================================

                           P S _ E A P _ E X T . C

DESCRIPTION
  This software unit holds the EAP Extended Method implementation of the 
  EAP system.

  The functions exposed by this module are to be called ONLY by the EAP
  functions.


EXTERNALIZED FUNCTIONS



 Copyright (c)2005-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_eap_ext.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/27/13    cx     Replace memcpy and memmove with memscpy and memsmove
10/26/12    fn     Added changes for memory optimization
08/17/12    fn     Cleanup ASSERT()
10/19/10    sa     Made changes for graceful exit in case of dsm exhaustion.
06/02/10  jee/mga  Modifications to support EAP server functionality
11/10/08    scb    Fixed QTF compilation warnings.
04/11/08    scb    Klocwork fixes
04/24/05    lti    Created module

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
#include "ps_eap_ext.h"

#include "dsm.h"
#include "ps_in.h"
#include "ps_system_heap.h"
#include "msg.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                    LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

eap_ext_type* eap_ext_cb[EAP_EXT_MAX_INST];

static boolean eap_ext_already_init = FALSE;
/*===========================================================================
  Internal EAP extended utility functions
===========================================================================*/
/*Clean an EAP extended instance*/
void eap_exti_clean( uint8 index );

/*Find an EAP instance*/
uint8 eap_exti_find_inst( eap_handle_type eap_handle );

/*Set vendor information*/
boolean eap_exti_set_vendor_info( uint8 index );

/*===========================================================================
  Callbacks to be registered with the EAP layer
===========================================================================*/
/* called by the lower EAP layer to input an EAP Ext packet*/
static void  eap_exti_input_cb( eap_handle_type    eap_handle,
                                dsm_item_type    **msg_ptr);

/*Closes EAP-EXT as an upper EAP authentication method. EAP-EXT at this 
  point needs to release the internal managers at this point. 
  Register this callback with EAP at initialization.*/
static void eap_exti_close_cb( eap_handle_type  eap_handle );

/*  Called by the underlying EAP to notify the EAP-EXT of a received EAP 
   Result packet. */
static void eap_exti_result_ind_cb( eap_handle_type       eap_handle, 
                                    eap_result_enum_type  result);

/*===========================================================================
FUNCTION EAP_EXT_INIT

DESCRIPTION
  Initialize the EAP-EXT software entity.

DEPENDENCIES
  None

PARAMETERS
  None.
RETURN VALUE
  None

SIDE EFFECTS
  Register for EAP_EXT packets over EAP transport.
  
===========================================================================*/
void 
eap_ext_init
(
  void
)
{
  uint8   index;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( eap_ext_already_init == FALSE )
  {
    for ( index = 0; index < EAP_EXT_MAX_INST ; index++ ) 
    {
      eap_ext_cb[index] = NULL;
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      Allow Callbacks to be re-register even if EXT was initialized 
    -----------------------------------------------------------------------*/
     LOG_MSG_INFO2_0("EAP EXT layer has already been initialized");
  }

  eap_method_input_cback_reg( eap_exti_input_cb, 
                              EAP_EXT_METHOD, 
                              EAP_PEER_ROLE);

  eap_method_close_cback_reg( eap_exti_close_cb, 
                              EAP_EXT_METHOD, 
                              EAP_PEER_ROLE);

  eap_method_result_cback_reg( eap_exti_result_ind_cb, 
                               EAP_EXT_METHOD, 
                               EAP_PEER_ROLE);

  eap_method_input_cback_reg( eap_exti_input_cb, 
                              EAP_EXT_METHOD, 
                              EAP_AUTHENTICATOR_ROLE);

  eap_method_close_cback_reg( eap_exti_close_cb, 
                              EAP_EXT_METHOD, 
                              EAP_AUTHENTICATOR_ROLE);

  eap_method_result_cback_reg( eap_exti_result_ind_cb, 
                               EAP_EXT_METHOD, 
                               EAP_AUTHENTICATOR_ROLE);

  eap_ext_already_init = TRUE;

}/* eap_ext_init */

/*===========================================================================
FUNCTION EAP_EXT_CREATE_INST

DESCRIPTION
  Creates an eap extended instance.
   
DEPENDENCIES
  None

PARAMETERS
  eap_handle - handle of the current EAP instance

RETURN VALUE
  TRUE if successful

SIDE EFFECTS
  None
  
===========================================================================*/
boolean
eap_ext_create_inst
(
  eap_handle_type    eap_handle
)
{
  uint8 index;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("Creating an EAP Extended Instance, EAP handle %d",
                  eap_handle);

  /*-------------------------------------------------------------------------
    Find a free EAP control block to operate on
  -------------------------------------------------------------------------*/
  for( index = 0; index < EAP_EXT_MAX_INST; index++ )
  {
    if ( NULL == eap_ext_cb[index] )
    {
      eap_ext_cb[index] = (eap_ext_type*)
                            ps_system_heap_mem_alloc(sizeof(eap_ext_type));
      if( eap_ext_cb[index] == NULL )
      {
        return FALSE;
      }
      eap_ext_cb[index]->eap_handle      = eap_handle;
      eap_ext_cb[index]->vendor_info_set = FALSE;
      eap_ext_cb[index]->eap_role        = EAP_PEER_ROLE;
      break;
    }
  }

  if ( EAP_EXT_MAX_INST <= index ) 
  {
    /*-----------------------------------------------------------------------
      Could not find a free instance
    -----------------------------------------------------------------------*/
    LOG_MSG_INFO1_0("Could not assign EAP Extended instance" );
    return FALSE;
  }

  return TRUE;

}/*eap_ext_create_inst*/

/*===========================================================================
FUNCTION EAP_EXT_DELETE_INSTANCE

DESCRIPTION
  Deletes an eap extended instance.
   
DEPENDENCIES
  None

PARAMETERS
  eap_handle - handle of the current EAP instance
  

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void
eap_ext_delete_instance
(
  eap_handle_type    eap_handle
)
{
  uint8 index;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("Deleting an EAP Extended Instance, EAP handle %d",
                  eap_handle);

  /*-----------------------------------------------------------------------
    Sanity check
  -------------------------------------------------------------------------*/
  if ( EAP_INVALID_HANDLE == eap_handle ) 
  {
    return;
  }

  /*-----------------------------------------------------------------------
    Find corresponding eap extended instance
  -------------------------------------------------------------------------*/
  index = eap_exti_find_inst(eap_handle);
  if ( EAP_EXT_MAX_INST <= index ) 
  {
    LOG_MSG_ERROR_0("No eap extended instance was found for this eap instance");
    return;
  }

  if ( NULL == eap_ext_cb[index] )
  {
    LOG_MSG_ERROR_0("No eap extended instance was allocated for this"
                    "eap instance");
    return;
  }

  eap_exti_clean(index);

}/*eap_ext_delete_inst*/

/*===========================================================================
FUNCTION EAP_EXT_INPUT_CB

DESCRIPTION

   EAP Header: 
   
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |    Code       |  Identifier   |         Length                |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Type      |
   +-+-+-+-+-+-+-+-+

   EAP Extended Header
                   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                   |            VENDOR ID                          |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                       Vendor Type                             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

   Only the vendor id is being stripped, the vendor type is being passed
   to the registered vendor handler.
   
DEPENDENCIES
  None

PARAMETERS
  eap_handle - handle of the current EAP instance
  msg_ptr - the message received from the EAP server without EAP header

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
static void  
eap_exti_input_cb
(
  eap_handle_type    eap_handle,
  dsm_item_type    **msg_ptr
)
{
  uint8   index;
  uint8   vendor_id[EAP_EXT_VENDOR_ID_LEN];
  uint16  bytes_read;
  uint8   vendor_index;
  uint32   vendor_type = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("Input cb called for EAP Ext, EAP handle %d", eap_handle);

  /*-----------------------------------------------------------------------
    Sanity check
  -------------------------------------------------------------------------*/
  if ((EAP_INVALID_HANDLE == eap_handle ) || ( NULL == msg_ptr)) 
  {
    dsm_free_packet(msg_ptr);
    return;
  }

  /*-----------------------------------------------------------------------
    Find corresponding eap extended instance
  -------------------------------------------------------------------------*/
  index = eap_exti_find_inst(eap_handle);
  if (EAP_EXT_MAX_INST <= index) 
  {
    LOG_MSG_ERROR_0("No eap extended instance was found for this eap instance");
    dsm_free_packet(msg_ptr);
    return;
  }

  if ( NULL == eap_ext_cb[index] )
  {
    LOG_MSG_ERROR_0("No eap extended instance was found");
    dsm_free_packet(msg_ptr);
    return;
  }
  /*-----------------------------------------------------------------------
    Check if the vendor information is set
  -------------------------------------------------------------------------*/
  if (!eap_ext_cb[index]->vendor_info_set)
  {
    if (!eap_exti_set_vendor_info(index))
    {
      LOG_MSG_ERROR_0("EAP Ext Input: Cannot set vendor information");
      dsm_free_packet(msg_ptr);
      return;
    }
  }

  /*-----------------------------------------------------------------------
    Extract vendor id from EAP header
  -------------------------------------------------------------------------*/
  bytes_read = dsm_pullup( msg_ptr, vendor_id, EAP_EXT_VENDOR_ID_LEN);

  if (EAP_EXT_VENDOR_ID_LEN != bytes_read)
  {
    LOG_MSG_ERROR_0("EAP Ext Input: Malformed packet received" );
    dsm_free_packet(msg_ptr);
    return;
  }


  /*-------------------------------------------------------------------------
    Check if it is Expanded NAK packet. Handle similar to Legacy NAK packet 
    if it is an Expanded NAK. Right now we consider Expanded NAK as failure.
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |     Code=2    |  Identifier   |           Length=28           |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |   Type=254    |       Vendor-Id = 0 (IETF)                    |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                     Vendor-Type = 3 (Nak)                     |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |   Type=254    |                0 (IETF)                       |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                                0 (No alternative)             |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  -------------------------------------------------------------------------*/
  if ((0 == vendor_id[0]) && (0 == vendor_id[1]) && (0 == vendor_id[2]))
  {
    /*-----------------------------------------------------------------------
      Extract vendor type from EAP header
    -----------------------------------------------------------------------*/
    bytes_read = dsm_pullup(msg_ptr, &vendor_type, sizeof(vendor_type));
  
    if (sizeof(vendor_type) != bytes_read)
    {
      LOG_MSG_ERROR_0("EAP Ext Input: Malformed packet received" );
      dsm_free_packet(msg_ptr);
      return;
    }

    /*-----------------------------------------------------------------------
      Convert the values to host byte order for processing
    -----------------------------------------------------------------------*/
    vendor_type = ps_ntohl(vendor_type);

    /*-----------------------------------------------------------------------
      Similar to Legacy NAK, currently other methods specified in Expanded 
      NAK are not supported. eap_exti_result_ind_cb is overloaded to indicate 
      a failure if an expanded NAK is received.
    -----------------------------------------------------------------------*/
    if (EAP_EXT_NAK_TYPE == vendor_type)
    {
      LOG_MSG_INFO1_0("Expanded NAK packet received. Indicating failure to "
                      "backend Vendor");
      dsm_free_packet(msg_ptr);
      eap_exti_result_ind_cb (eap_handle, EAP_RESULT_FAILURE);
      return;
    }

    else
    {
      LOG_MSG_ERROR_0("EAP Ext Input: Invalid packet received" );
      dsm_free_packet(msg_ptr);
      return;
    }
  }

  /*-------------------------------------------------------------------------
    Find matching vendor id and inject packet
  -------------------------------------------------------------------------*/
  for (vendor_index = 0; 
       vendor_index < eap_ext_cb[index]->vendor_id_info.vendor_id_count;
       vendor_index++) 
  {
    if (0 == memcmp(vendor_id, eap_ext_cb[index]->vendor_id_info.
                    vendor_handler_tbl[vendor_index].vendor_id, 
                    EAP_EXT_VENDOR_ID_LEN)) 
    {
      LOG_MSG_INFO1_3("EAP Ext: Input packet for Vendor Id: %x%x%x",
                    vendor_id[0], vendor_id[1], vendor_id[2]);

      if (NULL != eap_ext_cb[index]->vendor_id_info.
         vendor_handler_tbl[vendor_index].vendor_id_input_handler_ptr) 
      {
        eap_ext_cb[index]->vendor_id_info.vendor_handler_tbl[vendor_index].
          vendor_id_input_handler_ptr( eap_handle, msg_ptr );
      }

      break;
    }
  }

  if (vendor_index >= eap_ext_cb[index]->vendor_id_info.vendor_id_count) 
  {
    if (EAP_PEER_ROLE == eap_ext_cb[index]->eap_role)
    {
      LOG_MSG_ERROR_2("EAP Ext: Cannot find Vendor Id %d registered with "
                      "instance %d - dropping packet & nacking",
                      vendor_id, index);
      dsm_free_packet(msg_ptr);

      /*?? Send extended nack */
    /*---------------------------------------------------------------------
      RFC 3748 does not specify error handling for Vendor ID,
      Section 5.7: expanded nacks are only used for
      unsupport authentication method type for Expanded type 254
    ---------------------------------------------------------------------*/
    }

    else if (EAP_AUTHENTICATOR_ROLE == eap_ext_cb[index]->eap_role)
    {
      LOG_MSG_ERROR_2("EAP Ext: Cannot find Vendor Id %d registered with "
                      "instance %d", vendor_id, index);
      dsm_free_packet(msg_ptr);
    }
  }

  return;
}/*eap_ext_input_cb*/

/*===========================================================================
FUNCTION EAP_EXT_CLOSE_CB

DESCRIPTION
  Closes EAP-EXT as an upper EAP authentication method. EAP-EXT at this 
  point needs to release the internal managers at this point. 
  Register this callback with EAP at initialization.

DEPENDENCIES
  None

PARAMETERS
  eap_handle - handle of the current EAP instance 
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
static void 
eap_exti_close_cb
(
  eap_handle_type  eap_handle
)
{
  uint8 vendor_index;
  uint8 index;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("Close cb called for EAP Ext, EAP handle %d", eap_handle);

  /*-----------------------------------------------------------------------
    Sanity check
  -------------------------------------------------------------------------*/
  if ( EAP_INVALID_HANDLE == eap_handle ) 
  {
    LOG_MSG_ERROR_0("EAP Ext Create: Invalid input parameters");
    return;
  }

  /*-----------------------------------------------------------------------
    Find corresponding eap extended instance
  -------------------------------------------------------------------------*/
  index = eap_exti_find_inst(eap_handle);
  if ( EAP_EXT_MAX_INST <= index )  
  {
    LOG_MSG_ERROR_0("No eap extended instance was found for this eap instance");
    return;
  }

  if ( NULL == eap_ext_cb[index] )
  {
    LOG_MSG_ERROR_0("No eap extended instance was found");
    return;
  }

  /*-----------------------------------------------------------------------
    Check if the vendor information is set
  -------------------------------------------------------------------------*/
  if ( !eap_ext_cb[index]->vendor_info_set )
  {
    LOG_MSG_INFO1_0("EAP Ext Close: No vendor information found on close "
                    "- ignore");
  }

  /*-----------------------------------------------------------------------
    Close all vendor handlers for this instance
  -------------------------------------------------------------------------*/
  for ( vendor_index = 0; 
        vendor_index < eap_ext_cb[index]->vendor_id_info.vendor_id_count;
        vendor_index++ ) 
  {
      LOG_MSG_INFO1_3("EAP Ext: Closing Vendor Id: %x%x%x",
                      eap_ext_cb[index]->vendor_id_info.vendor_handler_tbl[vendor_index].vendor_id[0],
                      eap_ext_cb[index]->vendor_id_info.vendor_handler_tbl[vendor_index].vendor_id[1],
                      eap_ext_cb[index]->vendor_id_info. vendor_handler_tbl[vendor_index].vendor_id[2]);

      if ( NULL != eap_ext_cb[index]->vendor_id_info.
         vendor_handler_tbl[vendor_index].vendor_id_close_handler_ptr ) 
      {
        eap_ext_cb[index]->vendor_id_info.vendor_handler_tbl[vendor_index].
          vendor_id_close_handler_ptr( eap_handle );

       /*-----------------------------------------------------------------------
         Safety check after making vendor close callback
       -------------------------------------------------------------------------*/
        if (eap_ext_cb[index] == NULL)
        {
          LOG_MSG_ERROR_0("EAP EXT instance %d is invalid after vendor close cb");
          return;
        }
      }
  }

}/*eap_ext_close_cb*/

/*===========================================================================
FUNCTION  EAP_EXT_RESULT_IND_CB

DESCRIPTION
  Called by the underlying EAP to notify the EAP-EXT of a received EAP Success. 

DEPENDENCIES
  None

PARAMETERS
  eap_handle: 			    current eap handle
  result: 					    the authetication result
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
static void 
eap_exti_result_ind_cb
( 
  eap_handle_type       eap_handle, 
  eap_result_enum_type  result
)
{
  uint8 vendor_index;
  uint8 index;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("Result cb called for EAP Ext, EAP handle %d", eap_handle);

  /*-----------------------------------------------------------------------
    Sanity check
  -------------------------------------------------------------------------*/
  if (EAP_INVALID_HANDLE == eap_handle) 
  {
    return;
  }

  /*-----------------------------------------------------------------------
    Find corresponding eap extended instance
  -------------------------------------------------------------------------*/
  index = eap_exti_find_inst(eap_handle);
  if ( EAP_EXT_MAX_INST <= index )  
  {
    LOG_MSG_ERROR_0("No eap extended instance was found for this eap instance");
    return;
  }

  if ( NULL == eap_ext_cb[index] )
  {
    LOG_MSG_ERROR_0("No eap extended instance was found");
    return;
  }

  /*-----------------------------------------------------------------------
    Check if the vendor information is set
  -------------------------------------------------------------------------*/
  if (!eap_ext_cb[index]->vendor_info_set)
  {
    LOG_MSG_ERROR_0("EAP Ext Result: No vendor info found on result - ignore");
    return;
  }

  /*-----------------------------------------------------------------------
    Close all vendor handlers for this instance
  -------------------------------------------------------------------------*/
  for (vendor_index = 0; 
       vendor_index < eap_ext_cb[index]->vendor_id_info.vendor_id_count;
       vendor_index++) 
  {
      LOG_MSG_INFO1_3("EAP Ext: Notifing result Vendor Id: %x%x%x", 
                    eap_ext_cb[index]->vendor_id_info.
                      vendor_handler_tbl[vendor_index].vendor_id[0], 
                    eap_ext_cb[index]->vendor_id_info.
                      vendor_handler_tbl[vendor_index].vendor_id[1], 
                    eap_ext_cb[index]->vendor_id_info.
                      vendor_handler_tbl[vendor_index].vendor_id[2]);   

      if (NULL != eap_ext_cb[index]->vendor_id_info.vendor_handler_tbl[
                                vendor_index].vendor_id_result_handler_ptr)
      {
        eap_ext_cb[index]->vendor_id_info.vendor_handler_tbl[vendor_index].
          vendor_id_result_handler_ptr( eap_handle, result );
      }
  }
}/*eap_ext_result_ind_cb*/

/*===========================================================================
FUNCTION EAP_EXT_XMIT_PKT

DESCRIPTION
  Tx function for the EAP protocol. Adds EAP Ext header and send the packet 
  to the lower EAP layer.

DEPENDENCIES
  EAP instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS
  Sends the packet on associated link.

===========================================================================*/
void eap_ext_xmit_pkt
(
   eap_handle_type   eap_handle,
   dsm_item_type   **msg_ptr,
   uint8            *vendor_id
)
{
  uint8 vendor_index;
  uint16 bytes_wrote;
  uint8 index;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("Xmit packet called for EAP Ext, EAP handle %d", eap_handle);

  /*-----------------------------------------------------------------------
    Sanity check
  -------------------------------------------------------------------------*/
  if (( NULL == vendor_id ) || ( NULL == msg_ptr ))
  {
    dsm_free_packet(msg_ptr);
    return;
  }

  /*-----------------------------------------------------------------------
    Find corresponding eap extended instance
  -------------------------------------------------------------------------*/
  index = eap_exti_find_inst(eap_handle);
  if ( EAP_EXT_MAX_INST <= index ) 
  {
    LOG_MSG_ERROR_0("No eap extended instance was found for this eap instance");
    dsm_free_packet(msg_ptr);
    return;
  }

  if ( NULL == eap_ext_cb[index] )
  {
    LOG_MSG_ERROR_0("No eap extended instance was found");
    dsm_free_packet(msg_ptr);
    return;
  }
  /*-----------------------------------------------------------------------
    Check if the vendor information is set
  -------------------------------------------------------------------------*/
  if ( !eap_ext_cb[index]->vendor_info_set )
  {
    if (!eap_exti_set_vendor_info(index))
    {
      LOG_MSG_ERROR_0("EAP Ext Input: Cannot set vendor information");
      dsm_free_packet(msg_ptr);
      return;
    }
  }

  /*-----------------------------------------------------------------------
    Find matching vendor id and inject packet
  -------------------------------------------------------------------------*/
  for ( vendor_index = 0; 
        vendor_index < eap_ext_cb[index]->vendor_id_info.vendor_id_count;
        vendor_index++ ) 
  {
    if ( 0 == memcmp( vendor_id, eap_ext_cb[index]->vendor_id_info.
                      vendor_handler_tbl[vendor_index].vendor_id,
                      EAP_EXT_VENDOR_ID_LEN )) 
    {
      break;
    }
  }

  if ( vendor_index > eap_ext_cb[index]->vendor_id_info.vendor_id_count) 
  {
    LOG_MSG_ERROR_0("EAP Ext Xmit: Vendor ID not registered with this instance" );
    dsm_free_packet(msg_ptr);
    return;
  }

  LOG_MSG_INFO2_3("Eap Ext Xmit for Vendor Id %x %x %x",  
                eap_ext_cb[index]->vendor_id_info.
                  vendor_handler_tbl[vendor_index].vendor_id[0], 
                eap_ext_cb[index]->vendor_id_info.
                  vendor_handler_tbl[vendor_index].vendor_id[1], 
                eap_ext_cb[index]->vendor_id_info.
                  vendor_handler_tbl[vendor_index].vendor_id[2]);

  /*-----------------------------------------------------------------------
    Push vendor id header on top of the outgoing packet
  -------------------------------------------------------------------------*/
  bytes_wrote = dsm_pushdown_packed( msg_ptr, 
                                     eap_ext_cb[index]->vendor_id_info.
                           vendor_handler_tbl[vendor_index].vendor_id, 
                                     EAP_EXT_VENDOR_ID_LEN,
                                     DSM_DS_SMALL_ITEM_POOL);

  if( EAP_EXT_VENDOR_ID_LEN != bytes_wrote )
  {
    LOG_MSG_ERROR_0("EAP Ext Xmit: Failed to pushdown vendor header" );
    dsm_free_packet(msg_ptr);
    return;
  }

  eap_xmit_pkt(eap_handle, msg_ptr);
}/*eap_ext_xmit_pkt*/

/*===========================================================================
FUNCTION EAP_EXT_AUTH_COMPLETE_IND

DESCRIPTION
  This function is called by the authentication protocol to notify EAP of the
  result of the authentication process. The extended method will act as 
  pass-through and send the result to the lower EAP layer.

DEPENDENCIES
  EAP instance must have been created

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void eap_ext_auth_complete_ind 
(
  eap_handle_type          handle,
  eap_result_enum_type     result,
  uint8                   *pre_master_key,
  uint16                   pre_master_key_len
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_1("Auth complete ind called for EAP Ext, EAP handle %d",
                  handle);

  eap_auth_complete_ind(handle, result,pre_master_key,pre_master_key_len );
}/*eap_ext_auth_complete_ind*/

/*=========================================================================
                          INTERNAL METHODS
===========================================================================*/

/*===========================================================================
FUNCTION EAP_EXTI_CLEAN

DESCRIPTION
  Cleans the EAP EXT control block for this instance

DEPENDENCIES
  None

PARAMETERS
  index - the instance to be cleaned
  
RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void 
eap_exti_clean
(
  uint8  index
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("Clean CB for instance %d", index);

  /*-----------------------------------------------------------------------
    Sanity check
  -------------------------------------------------------------------------*/
  if ( EAP_EXT_MAX_INST <= index ) 
  {
    return;
  }

  if (eap_ext_cb[index] == NULL)
  {
    LOG_MSG_ERROR_1("EAP EXT: Error Invalid instance: %d", index);
    return;
  }

  PS_SYSTEM_HEAP_MEM_FREE(eap_ext_cb[index]);
}/*eap_exti_clean*/

/*===========================================================================
FUNCTION EAP_EXTI_FIND_INST

DESCRIPTION
  Finds the EAP Extended instance corresponding to the eap handle.

DEPENDENCIES
  None

PARAMETERS
  eap_handle - the handle of the matching eap instance
  
RETURN VALUE
  index of the eap extended instance

SIDE EFFECTS
  
===========================================================================*/
uint8
eap_exti_find_inst
(
  eap_handle_type  eap_handle
)
{
  uint8 index;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("Find instance for EAP handle %d", eap_handle);

  for( index = 0; index < EAP_EXT_MAX_INST; index++ )
  {
    if ((eap_ext_cb[index] != NULL ) &&
        ( eap_handle == eap_ext_cb[index]->eap_handle ))
    {
      LOG_MSG_INFO2_2("Found instance %d for EAP handle %d",
                      index, eap_handle);
      break;
    }
  }

  return index;
}/*eap_exti_find_inst*/

/*===========================================================================
FUNCTION EAP_EXTI_SET_VENDOR_INFO

DESCRIPTION
  Retrieves the meta info from the eap handle and sets the vendor info for 
  the current instance.

DEPENDENCIES
  None

PARAMETERS
  index - the index of the current eap extended instance
  
RETURN VALUE
  TRUE if successful

SIDE EFFECTS
  None
===========================================================================*/
boolean
eap_exti_set_vendor_info
(
  uint8 index
)
{
  eap_ext_vendor_id_meta_info_type*    meta_info = NULL;
  uint8                                vendor_index;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Sanity check
  -----------------------------------------------------------------------*/
  LOG_MSG_INFO1_0("EAP Ext: Setting Vendor Information");

  if ( EAP_EXT_MAX_INST <= index )  
  {
    LOG_MSG_ERROR_0("EAP Ext Set Vendor Info: Invalid Input Parameters");
    return FALSE;
  }

  if ( NULL == eap_ext_cb[index] )
  {
    LOG_MSG_ERROR_0("EAP Ext Set Vendor Info: Invalid Instance");
    return FALSE;
  }

  /*-----------------------------------------------------------------------
    Retrieve meta info from the eap instance - the vendor id information 
    is being passed as meta info
  -----------------------------------------------------------------------*/
  meta_info = eap_get_meta_info( eap_ext_cb[index]->eap_handle );
  if ( NULL == meta_info ) 
  {
    LOG_MSG_ERROR_0("EAP Ext Set Vendor Info: NULL meta information");
    return FALSE;
  }

  /*-----------------------------------------------------------------------
    Fill the vendor if info for this instance
  -----------------------------------------------------------------------*/
  eap_ext_cb[index]->vendor_id_info.vendor_id_count = 
    ( meta_info->vendor_id_count > EAP_EXT_MAX_VENDOR_ID_PER_INSTANCE )?
      EAP_EXT_MAX_VENDOR_ID_PER_INSTANCE : meta_info->vendor_id_count;

  for ( vendor_index = 0; vendor_index < 
                          eap_ext_cb[index]->vendor_id_info.vendor_id_count;
        vendor_index++ ) 
  {
    memscpy( eap_ext_cb[index]->vendor_id_info.vendor_handler_tbl[vendor_index].
               vendor_id , 
             EAP_EXT_VENDOR_ID_LEN,
             meta_info->vendor_handler_tbl[vendor_index].vendor_id,
             EAP_EXT_VENDOR_ID_LEN);

    eap_ext_cb[index]->vendor_id_info.vendor_handler_tbl[vendor_index].
      vendor_id_close_handler_ptr  = 
      meta_info->vendor_handler_tbl[vendor_index].vendor_id_close_handler_ptr;

    eap_ext_cb[index]->vendor_id_info.vendor_handler_tbl[vendor_index].
      vendor_id_input_handler_ptr  =
      meta_info->vendor_handler_tbl[vendor_index].vendor_id_input_handler_ptr;

    eap_ext_cb[index]->vendor_id_info.vendor_handler_tbl[vendor_index].
      vendor_id_result_handler_ptr =
      meta_info->vendor_handler_tbl[vendor_index].vendor_id_result_handler_ptr;
  }

  eap_ext_cb[index]->vendor_info_set = TRUE;
  eap_ext_cb[index]->eap_role = eap_get_role(eap_ext_cb[index]->eap_handle);

  return TRUE;
}/*eap_exti_set_vendor_info*/

#endif /* FEATURE_DATA_PS_EAP */

#ifndef PS_EAP_SIMI_H
#define PS_EAP_SIMI_H


/*===========================================================================

        E A P  S U B S C R I B E R  I D E N T I T Y  M O D U L E
                            
                            F O R  W L A N
                
                   
DESCRIPTION
  This file contains EAP-SIM processing functions.
     
    
Copyright (c) 2006-2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_eap_simi.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/26/11    op     Added changes to obtain supplementary SIM/AKA auth info 
12/19/08    pp     Created module as part of Common Modem Interface:
                   Public/Private API split.
===========================================================================*/



/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"

#ifdef FEATURE_DATA_PS_EAP

#include "comdef.h"
#include "ps_eap.h"

#include "ps_eap_sim_aka.h"
#include "ps_eap_sim_akai.h"
/*===========================================================================

               EAP SIM DATA DECLARATIONS AND DEFINITIONS

===========================================================================*/

/*===========================================================================

                     INTERNAL FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION EAP_SIM_REGISTER_NONCE_FCT

DESCRIPTION
  Registeres the function used to retrieve the nonce by the SIM Core.
  Used as pointer in order to be easily stubbed.

DEPENDENCIES
  None

PARAMETERS
  fct_ptr - pointer to a function that can retrieve a nonce
  
RETURN VALUE
  None

SIDE EFFECTS
    
===========================================================================*/
void
eap_sim_register_nonce_fct
(
 eap_sim_aka_get_nonce_type  fct_ptr
);

/*===========================================================================
FUNCTION EAP_SIM_INPUT_CB

DESCRIPTION
 
                   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                   |  Subtype      |         Reserved              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |              TLV              |           TLV...
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

   Structure of a TLV(type/len/value)
   
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |    Type       |         Length                |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Data ....              
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

   
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
void  
eap_sim_input_cb
(
  eap_handle_type    eap_handle,
  dsm_item_type    **msg_ptr
);

/*===========================================================================
FUNCTION EAP_SIM_CLOSE_CB

DESCRIPTION
  Closes EAP-SIM as an upper EAP authentication method. EAP-SIM at this 
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
void 
eap_sim_close_cb
(
  eap_handle_type  eap_handle
);

/*===========================================================================
FUNCTION  EAP_SIM_RESULT_IND_CB

DESCRIPTION
  Called by the underlying EAP to notify the EAP-SIM of the final result of 
  the authentication. This is a new feature that will be added by the EAP in
  order to enable SIM and AKA to be aware of the result of the authentication.
  SIM and AKA need this info to enable them to save the temporary identities 
  obtained during the transaction.

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
void 
eap_sim_result_ind_cb
( 
  eap_handle_type       eap_handle, 
  eap_result_enum_type  result
);

/*===========================================================================
FUNCTION  EAP_SIM_SUPP_RESULT_IND_CB

DESCRIPTION
  Called by the underlying EAP to notify the EAP-SIM of the supplementary info
  of the authentication.

DEPENDENCIES
  None

PARAMETERS
  eap_handle:     current eap handle
  result:         the authetication result
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void 
eap_sim_supp_result_ind_cb
( 
  eap_handle_type       eap_handle, 
  eap_result_enum_type  result
);

#endif /* FEATURE_DATA_PS_EAP */
#endif /* PS_EAP_SIM_H */

#ifndef PS_EAP_AKAI_H
#define PS_EAP_AKAI_H


/*===========================================================================

        E A P  A U T H E N T I C A T I O N   K E Y   A G R E E M E N T
                            
                            F O R  W L A N
                
                   
DESCRIPTION
  This file contains EAP-AKA processing functions.
     
    
Copyright (c) 2008-2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_eap_akai.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/26/11    op     Added changes to obtain supplementary SIM/AKA auth info 
12/19/08    pp     Created module.as part of Common Mode Interface: 
                   Public/Private split.
===========================================================================*/



/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"

#ifdef FEATURE_DATA_PS_EAP

#include "comdef.h"
#include "ps_eap.h"

/*===========================================================================

                     INTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION EAP_AKA_INPUT_CB

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
eap_aka_input_cb
(
  eap_handle_type    eap_handle,
  dsm_item_type    **msg_ptr
);

/*===========================================================================
FUNCTION EAP_AKA_CLOSE_CB

DESCRIPTION
  Closes EAP-AKA as an upper EAP authentication method. EAP-AKA at this 
  point needs to release the internal managers. 
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
eap_aka_close_cb
(
  eap_handle_type  eap_handle
);

/*===========================================================================
FUNCTION  EAP_AKA_RESULT_IND_CB

DESCRIPTION
  Called by the underlying EAP to notify the EAP-AKA of the final result of 
  the authentication. This is a new feature that will be added by the EAP in
  order to enable AKA to be aware of the result of the authentication.
  EAP AKA needs this info to save the temporary identities obtained during 
  the transaction.

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
eap_aka_result_ind_cb
( 
  eap_handle_type       eap_handle, 
  eap_result_enum_type  result
);

/*===========================================================================
FUNCTION  EAP_AKA_SUPP_RESULT_IND_CB

DESCRIPTION
  Called by the underlying EAP to notify the EAP-AKA of the supplementary info
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
eap_aka_supp_result_ind_cb
( 
  eap_handle_type       eap_handle, 
  eap_result_enum_type  result
);

#endif /* FEATURE_DATA_PS_EAP */
#endif /* PS_EAP_AKA_H */

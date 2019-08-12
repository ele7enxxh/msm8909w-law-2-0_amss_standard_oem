#ifndef PS_EAP_AKA_PRIME_H
#define PS_EAP_AKA_PRIME_H


/*===========================================================================

        E A P  A U T H E N T I C A T I O N   K E Y   A G R E E M E N T

                                P R I M E         
                   
DESCRIPTION
  This file contains EAP-AKA_PRIME' processing functions.
     
    
  Copyright (c) 2009-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE


  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/api/ps_eap_aka_prime.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $

===========================================================================*/



/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "ps_eap.h"

/*===========================================================================

                     EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION EAP_AKA_PRIME_INIT

DESCRIPTION
  Initialize the EAP-AKA_PRIME software entity.

DEPENDENCIES
  None

PARAMETERS
  None.
  
RETURN VALUE
  None

SIDE EFFECTS
  Register for EAP-AKA_PRIME packets over EAP transport.
  
===========================================================================*/
void 
eap_aka_prime_init
(
  void
);

/*===========================================================================
FUNCTION EAP_AKA_PRIME_INPUT_CB

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
eap_aka_prime_input_cb
(
  eap_handle_type    eap_handle,
  dsm_item_type    **msg_ptr
);

/*===========================================================================
FUNCTION EAP_AKA_PRIME_CLOSE_CB

DESCRIPTION
  Closes EAP-AKA_PRIME as an upper EAP authentication method. 
  EAP-AKA_PRIME at this point needs to release the internal managers. 
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
eap_aka_prime_close_cb
(
  eap_handle_type  eap_handle
);

/*===========================================================================
FUNCTION  EAP_AKA_PRIME_RESULT_IND_CB

DESCRIPTION
  Called by the underlying EAP to notify the EAP-AKA_PRIME of the final result of 
  the authentication. This is a new feature that will be added by the EAP in
  order to enable AKA_PRIME to be aware of the result of the authentication.
  EAP AKA_PRIME needs this info to save the temporary identities obtained during 
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
eap_aka_prime_result_ind_cb
( 
  eap_handle_type       eap_handle, 
  eap_result_enum_type  result
);

/*===========================================================================
FUNCTION  EAP_AKA_PRIME_SUPP_RESULT_IND_CB

DESCRIPTION
  Called by the underlying EAP to notify the EAP-AKA_PRIME of the supplementary
  info of the authentication.

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
eap_aka_prime_supp_result_ind_cb
( 
  eap_handle_type       eap_handle, 
  eap_result_enum_type  result
);

#endif /* PS_EAP_AKA_PRIME_H */

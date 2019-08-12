/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC ACCESS FUNCTION(S) DEFINITION FOR INTERNAL SIGNALING
***
***
*** DESCRIPTION
***
***  This file contains the definitions of the access function
***  GPRS_MAC_MAC_ACC_FUNC() used by MAC for the purpose of sending
***  messages to itself.
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/inc/gmacmac.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 06/26/01   tlx     Created

*****************************************************************************/

#ifndef INC_GMACMAC_H
#define INC_GMACMAC_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif


#include "gmacmacsig.h"

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      gprs_mac_mac_acc_func
===
===  DESCRIPTION
===  MAC access function. Used by MAC to send messages to itself.
===
===  The function makes a copy of the message structure and places it
===  in its own message queue.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gprs_mac_mac_acc_func(mac_int_sig_t *common_msg_ptr, gas_id_t gas_id);

#endif /* INC_GMACMAC_H */
/*** EOF: don't remove! ***/

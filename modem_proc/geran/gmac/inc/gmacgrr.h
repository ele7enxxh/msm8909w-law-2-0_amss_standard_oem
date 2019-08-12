/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC ACCESS FUNCTION(S) DEFINITION FOR INTERFACING TO GRR
***
***
*** DESCRIPTION
***
***  This file contains the definition of the access function  
***  GPRS_MAC_GRR_ACC_FUNC() used by GRR for the purpose of sending 
***  messages to MAC.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/inc/gmacgrr.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 06/26/01   tlx     Created

*****************************************************************************/

#ifndef INC_GMACGRR_H
#define INC_GMACGRR_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#include "customer.h"

#include "gmacgrrsig.h"

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      mac_get_cmd_buf_grr_mac()
===
===  DESCRIPTION
===  Allocate memory for message from GRR to MAC 
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
grr_mac_sig_t *mac_get_cmd_buf_grr_mac(void);

/*===========================================================================
===
===  FUNCTION      mac_put_cmd_grr_mac()
===
===  DESCRIPTION
===  Add MAC message to MAC signal queue. 
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
void mac_put_cmd_grr_mac(gas_id_t gas_id, grr_mac_sig_t *sig_ptr);

#endif /* INC_GMACGRR_H */
/*** EOF: don't remove! ***/

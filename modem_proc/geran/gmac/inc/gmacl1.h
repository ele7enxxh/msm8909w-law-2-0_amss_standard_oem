/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC ACCESS FUNCTION(S) DEFINITION FOR INTERFACING TO LAYER 1
***
***
*** DESCRIPTION
***
***  This file contains definitions of signals and data structures used by
***  access functions GPRS_MAC_l1_ACC_FUNC() for the prepose of sending
***  messges from L1 to MAC
***
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/inc/gmacl1.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 06/26/01   tlx     Created

*****************************************************************************/

#ifndef INC_GMACL1_H
#define INC_GMACL1_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#include "customer.h"

#include "gmacl1sig.h"
#include "gmacl1sig_g.h"

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      void gprs_mac_l1_acc_func ()

===
===  DESCRIPTION
===    MAC access function. Used by L1 to send messages to MAC.
===
===    The function makes a copy of the message structure and places it
===    in MAC's message queue.
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
void gprs_mac_l1_acc_func(l1_mac_sig_t *common_msg_ptr, uint8 *pacch_data,
                          gas_id_t gas_id
                          );

#endif /* INC_GMACL1_H */
/*** EOF: don't remove! ***/

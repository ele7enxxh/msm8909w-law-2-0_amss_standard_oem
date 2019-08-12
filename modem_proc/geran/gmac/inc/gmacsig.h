/*****************************************************************************
***
*** TITLE
***
***  GPRS common signals
***
***
*** DESCRIPTION
***
***  Defines the union of all signals sent to MAC
***
***  Defines the task id's for between MAC and the tasks it interfaces too.
***
***  Defines the Union that holds all the signal struct definitions for
***  signals between MAC/GRR/RR and L1
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/inc/gmacsig.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 06/27/01   TLX     Created
***
*****************************************************************************/

#ifndef INC_GMACSIGS_H
#define INC_GMACSIGS_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif


#include "gmacl1sig.h"
#include "gmacl1sig_g.h"
#include "gmacrlc.h"
#include "gmacgrrsig.h"
#include "gmacmacsig.h"

#include "grlcmac.h"
#include "gmacsigl1.h"
#include "gmacsigl1_g.h"
#include "mac_grr.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/
typedef union
{
  l1_mac_sig_t l1_mac_sig;
  grr_mac_sig_t grr_mac_sig;
  mac_int_sig_t mac_int_sig;
  rlc_mac_msg_t rlc_mac_sig;
} mac_signals_u;


#endif /* INC_GMACSIGS_H */
/*** EOF: don't remove! ***/

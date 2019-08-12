/*****************************************************************************
***
*** TITLE
***
***  GPRS PORTABLE LAYER ONE INTERFACE TO RLC
***
***
*** DESCRIPTION
***
***  This file defines the access function to allow RLC UL to send
***  primitive to PL1
***
***  pl1_send_ph_data_req().
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/gl1rlc.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 03/27/01    hv     created
*** 06/01/07    cs     Modify the sig code to be unique
***
*****************************************************************************/

#ifndef INC_GL1RLC_H
#define INC_GL1RLC_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif


#include "gprsdef.h"
#include "gprsdef_g.h"
#include "gprs_pdu.h"

#include "grlcmaccmn.h"
#include "gl1rlccmn.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

#define RLC_PH_DATA_REQ_SIG   0x0400    /* Used by RLC to inform layer 1 of a
                                     * RLC_PH_DATA_REQ
                                     */

/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/

/*===========================================================================
===
===  MACRO         xxx
===
===  DESCRIPTION
===
===    xxx.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/




/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/



/*===========================================================================
===
===  FUNCTION      PL1_SEND_PH_DATA_REQ()
===
===  DESCRIPTION
===
===    Access function to allow RLC to send PH_DATA_REQ primitive to PL1.
===    RLC sends this primitive to indicate to PL1 there are data blocks in
===    the RLC UL/PL1 FIFO to be processed. This only occurs at the beginning
===    of a TBF. Thereafter, PL1 polls the FIFO for more data. This is the only
===    primitive to PL1 and it contains no other paramters. As such a message
===    is not required, hence this function only sets a dedicated signal mask
===    in PL1 space, if the mask is not already set.
===
===
===  PARAMETERS
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void pl1_send_ph_data_req ( gas_id_t gas_id );


#endif /* INC_GL1RLC_H */

/*** EOF: don't remove! ***/

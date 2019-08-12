/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC TASK
***
***
*** DESCRIPTION
***
***  Handles the initialisation of the MAC task and the signal reception
***  handler.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/inc/gmactask.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 06/27/01   tlx     Created
***
*****************************************************************************/

#ifndef INC_GMACTASK_H
#define INC_GMACTASK_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


#include "gprsdef.h"
#include "gprsdef_g.h"
#include "rex.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      gmac_multislot_class_init()
===
===  DESCRIPTION
===  Multislot class parameters to be initialised depending on the
===  current multislot class supported.
===
===  NOTE: This function only supports multislot classes 1 to 12 and shall
===        cause a fatal error if any other value set.
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
void gmac_multislot_class_init(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION    gmac_init_task_data()
===
===  DESCRIPTION
===  Initialises Task data.  Used for Lower layer resets.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_task_data( gas_id_t gas_id );


/*===========================================================================
===
===  FUNCTION    gmac_reset_clear_task_queue()
===
===  DESCRIPTION
===  Used for Lower layer resets.  Clears all msgs from the task queue.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_reset_clear_task_queue( gas_id_t gas_id );
#ifdef FEATURE_GSM_EDTM
/*===========================================================================
===
===  FUNCTION      gmac_edtm_enabled_init()
===
===  DESCRIPTION
===  Initilialise the edtm_enabled flag from the RR public store.
===
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
void gmac_edtm_enabled_init(gas_id_t gas_id);
#endif /*FEATURE_GSM_EDTM*/

#if PERLUTF
#error code not present
#endif /*PERLUTF*/

#endif /* INC_GMACTASK_H */
/*** EOF: don't remove! ***/

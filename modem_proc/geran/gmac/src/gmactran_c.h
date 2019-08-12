/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC TRANSFER MODE MODULE
***
***
*** DESCRIPTION
***
***  This module administrates the signal reception and the associated actions
***  for transfer mode states.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/src/gmactran_c.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 05/12/01   sb      Created
***
*****************************************************************************/

#ifndef INC_GMACTRANSFERC_H
#define INC_GMACTRANSFERC_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#include "customer.h"

/*****************************************************************************
***
***     Public Function Prototypes
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_concurrent_data_ind 
===
===  DESCRIPTION
===  Handles received logical channel messages during uplink dynamic
===  packet transfer           
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_tdynamic_concurrent_data_ind(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_transfer_concurrent_dynamic_dl_reassign_data_ind 
===
===  DESCRIPTION
===  Handles received logical channel messages during uplink dynamic
===  packet transfer           
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_tdynamic_concurrent_dl_reassign_data_ind(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_concurrent_realloc_data_ind 
===
===  DESCRIPTION
===  Handles received logical channel messages during uplink dynamic
===  packet transfer           
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===  
===========================================================================*/
void gmac_tdynamic_concurrent_realloc_data_ind(gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      gmac_tdynamic_concurrent_realloc_dl_reassign_data_ind 
===
===  DESCRIPTION
===  Handles received logical channel messages during uplink dynamic
===  packet transfer           
===
===  DEPENDENCIES
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header
===  
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_tdynamic_concurrent_realloc_dl_reassign_data_ind(gas_id_t gas_id);

#endif /* INC_GMACTRANSFERC_H */
/*** EOF: don't remove! ***/

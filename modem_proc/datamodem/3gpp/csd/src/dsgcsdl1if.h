#ifndef DSGCSDL1IF_H
#define DSGCSDL1IF_H
/*===========================================================================

           G S M   P R O T O C O L   S T A C K   L A Y E R   1
                             I N T E R F A C E      
                            H E A D E R   F I L E

DESCRIPTION
   This header file contains the function prototypes for the GSM 
   protocol stack interface to GSM (OTA) Layer 1. 
  
Copyright (c) 2003 - 2004 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsgcsdl1if.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/11   TTV     Support for Q6 free floating.
05/30/04   AR      Lint corrections.
03/17/03   TMR     Created

===========================================================================*/


/*===========================================================================
Header files
===========================================================================*/
#include "dsucsd.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/




/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION  DS_GCSD_GET_DS_GCSD_L1_RX_BUF_CRIT_SECT_PTR

DESCRIPTION
   Returns a pointer to the variable ds_gcsd_l1_rx_buf_crit_sect. 
         
DEPENDENCIES
   None 

RETURN VALUE
   rex_crit_sect_type* - pointer to ds_gcsd_l1_rx_buf_crit_sect

SIDE EFFECTS
   None

===========================================================================*/
rex_crit_sect_type* 
ds_gcsd_get_l1_rx_buf_crit_sect_ptr(void);

/*===========================================================================

FUNCTION  DS_GCSD_GET_DS_GCSD_L1_TX_BUF_CRIT_SECT_PTR

DESCRIPTION
   Returns a pointer to the variable ds_gcsd_l1_tx_buf_crit_sect. 
         
DEPENDENCIES
   None 

RETURN VALUE
   rex_crit_sect_type* - pointer to ds_gcsd_l1_tx_buf_crit_sect
    
SIDE EFFECTS
   None

===========================================================================*/
rex_crit_sect_type* 
ds_gcsd_get_l1_tx_buf_crit_sect_ptr(void);

#endif /* DSGCSDL1IF_H */

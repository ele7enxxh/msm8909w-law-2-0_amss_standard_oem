#ifndef DSGCSDL1IF_API_H
#define DSGCSDL1IF_API_H
/*===========================================================================
  
              G C S D   P R O T O C O L   S T A C K    
              E X T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This file contains external function prototypes for GCSD transport protocol.  
  
  Copyright (c) 2003 - 2011 by Qualcomm Technologies Incorporated. All Rights Reserved.
  Qualcomm Confidential and Proprietary.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/dsgcsdl1if_api.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/08/11   nd      First revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "l1_ds_v.h"



/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION GCSD_OTA_RX_CALLBACK

DESCRIPTION
  This callback function is called by L1 every 4th frame (after every 
  data message has been received).

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gcsd_ota_rx_callback
(
   gcsd_buffer_ptr      curr_buf, 
   gcsd_buffer_ptr      *next_buf, 
   gcsd_receive_status  status
);

/*===========================================================================

FUNCTION GCSD_OTA_TX_CALLBACK

DESCRIPTION
  This callback function is called by L1 every 4th frame (before 
  every data message is sent).

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gcsd_ota_tx_callback
(
   uint8    **curr_buf, 
   uint8    *prev_buf, 
   boolean  tx_enabled
);
#endif /* DSGCSDL1IF_API_H */

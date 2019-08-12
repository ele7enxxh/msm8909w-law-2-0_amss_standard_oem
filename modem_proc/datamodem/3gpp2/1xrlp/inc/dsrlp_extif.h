#ifndef DSRLP_EXTIF_H
#define DSRLP_EXTIF_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  R A D I O   L I N K   P R O T O C O L  E X T E R N A L  I N T E R F A C E 

GENERAL DESCRIPTION
  This file contains externalized RLP interface functions to be used by
  other modules. These modules could be other modules within data services
  or outside data services.  


 Copyright (c) 2003 - 2004 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/inc/dsrlp_extif.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
07/05/04   gr         Initial version
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    PUBLIC FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================

FUNCTION       DSRLP_EXTIF_GET_NAK_CNT

DESCRIPTION    This function checks if RLP is in Established state and 
               populates the nak_count_ptr. It returns TRUE if the NAK
               Q is not empty. Otherwise it returns FALSE. 
               
DEPENDENCIES   NONE

RETURN VALUE   TRUE if NAK Q is non-empty

SIDE EFFECTS   
===========================================================================*/
boolean dsrlp_extif_get_nak_cnt
(
  uint32 *nak_count_ptr
);

/*===========================================================================

FUNCTION       DSRLP_EXTIF_GET_IDLE_TX_FRAME_CNT

DESCRIPTION    This function checks if RLP is in Established state and 
               populates the idle_tx_fr_cnt_ptr. It returns TRUE if RLP is
               the Established state. Otherwise it returns FALSE. 
               
DEPENDENCIES   NONE

RETURN VALUE   TRUE if RLP is in the ESTABLISHED state

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlp_extif_get_idle_tx_frame_cnt
(
  uint32 *idle_tx_fr_cnt_ptr /* ptr to tx idle frame count to be populated */
);

/*===========================================================================

FUNCTION       DSRLP_EXTIF_GET_RX_IDLE_FRAME_CNT

DESCRIPTION    This function checks if RLP is in Established state and 
               populates the idle_rx_fr_cnt_ptr. It returns TRUE if RLP is
               the Established state. Otherwise it returns FALSE. 
               
DEPENDENCIES   NONE

RETURN VALUE   TRUE if RLP is in the ESTABLISHED state

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlp_extif_get_idle_rx_frame_cnt
(
  uint32 *idle_rx_fr_cnt_ptr /* ptr to rx idle frame count to be populated */
);
#endif /* DSRLP_EXTIF_H */

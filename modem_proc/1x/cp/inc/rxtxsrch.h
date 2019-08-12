#ifndef RXTXSRCH_H
#define RXTXSRCH_H
/*===========================================================================

                   R X T X    S R C H    H E A D E R    F I L E

DESCRIPTION
  This file contains the interface between the rxtx (layer 2) task and the
  Search task.

  Copyright (c) 2004 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/rxtxsrch.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
06/11/04   yll     Created file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/



/*===========================================================================

FUNCTION RXTXSRCH_REG_RX_MSG_NOTIFIER

DESCRIPTION
  This registers a function to notify the received message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxtxsrch_reg_rx_msg_notifier
(
  void ( *rx_msg_notifier )( void )
);

/*===========================================================================

FUNCTION RXTXSRCH_REG_TX_MSG_NOTIFIER

DESCRIPTION
  This registers a function to notify the sent message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxtxsrch_reg_tx_msg_notifier
(
  void ( *tx_msg_notifier )( uint16 retry_count )
);



#endif /* RXTXSRCH_H */

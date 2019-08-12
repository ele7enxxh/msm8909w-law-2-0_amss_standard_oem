

#ifndef DS3G_EXT_H
#define DS3G_EXT_H
/*===========================================================================

                              D S 3 G   E X T H

                            H E A D E R   F I L E

DESCRIPTION
  This is the external header file for lower layers to use DS defined API and
  structures

  Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.  
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ds3g_ext.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/10/15   ss     Initial version.

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC ENUM/STRUCT DECLARATIONS

===========================================================================*/

typedef enum
{
  DS3G_TX_POWER_BIN_1,                        /* x <= 0dBm */ 
  DS3G_TX_POWER_BIN_2,                        /* 0dBm < x <= 8dBm */ 
  DS3G_TX_POWER_BIN_3,                        /* 8dBm < x <= 15dBm */ 
  DS3G_TX_POWER_BIN_4,                        /* 15dBm < x <= 20dBm */ 
  DS3G_TX_POWER_BIN_5,                        /* x > 20dBm */ 
  DS3G_TX_POWER_BIN_MAX                       /*  MAX */ 
}ds3g_tx_power_bin_e_type;


#endif /* DS3G_EXT_H */

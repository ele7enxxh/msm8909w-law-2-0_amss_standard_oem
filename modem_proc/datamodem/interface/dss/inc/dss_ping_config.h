#ifndef DSS_PING_CONFIG_H
#define DSS_PING_CONFIG_H
/*===========================================================================

         D A T A   S E R V I C E S   P I N G   H E A D E R  F I L E

DESCRIPTION

 This header file contains shared variables declarations for functions
 related to the ping API.


Copyright (c) 2006-2007 by Qualcomm Technologies, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#define   DSS_PING_COOKIE                           0xEC0EC0ECUL
#define   DSS_PING_INFINITE_NUM_PINGS               0xFF
#define   DSS_PING_MAX_PING_DATA_BYTES              256
#define   DSS_PING_MIN_PING_DATA_BYTES              4
#define   DSS_PING_DEFAULT_TIMES_TO_PING            4
#define   DSS_PING_DEFAULT_PING_DATA_BYTES          64
#define   DSS_PING_DEFAULT_TTL                      255

#define   DSS_PING_DEFAULT_PING_RETRANSMIT_TIME     2000   /* ms */
#define   DSS_PING_MIN_PING_RETRANSMIT_TIME          200   /* ms */
#define   DSS_PING_MAX_PING_RETRANSMIT_TIME         5000   /* ms */

#define   DSS_PING_DEFAULT_PING_RESPONSE_TIME_OUT   10000  /* ms */
#define   DSS_PING_MIN_PING_RESPONSE_TIME_OUT       5000   /* ms */
#define   DSS_PING_MAX_PING_RESPONSE_TIME_OUT       10000  /* ms */


#endif /* DSS_PING_CONFIG_H */

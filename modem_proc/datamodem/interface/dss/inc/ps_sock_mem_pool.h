#ifndef PS_SOCK_MEM_POOL_H
#define PS_SOCK_MEM_POOL_H
/*===========================================================================

          S O C K E T S  M E M O R Y  P O O L  H E A D E R  F I L E

DESCRIPTION
 The Protocol Services Socket Memory Pool Header File. Contains declarations
 of functions specifice to sockets memory initialization.

EXTERNAL FUNCTIONS
  PS_SOCK_MEM_POOL_INIT()
    Initialized memory for sockets module

Copyright (c) 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/inc/ps_sock_mem_pool.h#1 $
  $DateTime: 2016/12/13 08:00:03 $

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"


#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_SOCK_MEM_POOL_INIT()

DESCRIPTION
  Initializes dssocki module

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_sock_mem_pool_init
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* PS_SOCK_MEM_POOL_H */

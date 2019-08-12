#ifndef PS_INET_EVENT_MGR_H
#define PS_INET_EVENT_MGR_H
/*===========================================================================

          I N E T   M E M O R Y  P O O L  H E A D E R  F I L E

DESCRIPTION
  The inet module memory pool management header file.

EXTERNAL FUNCTIONS
  PS_INET_MEM_POOL_INIT()
    Initialized memory for inet module

Copyright (c) 2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/ps_inet_event_mgr.h#1 $
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
FUNCTION PS_INET_MEM_POOL_INIT()

DESCRIPTION
  Initializes inet memories.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_inet_event_mgr_init
(
  void
);

#ifdef __cplusplus
}
#endif
#endif /* PS_INET_EVENT_MGR_H */

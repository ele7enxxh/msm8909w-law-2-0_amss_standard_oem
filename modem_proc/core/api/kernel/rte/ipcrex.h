#ifndef IPCREX_H
#define IPCREX_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           S E C U R I T Y   S E R V I C E S   T A S K
                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

   ipc_new()        - Allocate a new message

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/




/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/kernel/rte/ipcrex.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/13   et      Created module.
04/06/05   et      Added rex.h

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "rex.h"
#include "ipctypes.h"

#include "pq.h"
#include "ipcdefi.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
              DEFINITIONS AND CONSTANTS
===========================================================================*/

/*===========================================================================
              DEFINITIONS AND TYPES DECLARATIONS
===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif
/*---------------------------------------------------------------------------
  External function declarations.
---------------------------------------------------------------------------*/
void ipc_post_msg
(
   ipc_internal_msg_type* msg_ptr,
   rex_tcb_type* tcb_ptr
);

void rex_register_receive_signal
(
   rex_sigs_type signal
);

/**
This function preserves the signal bits for any pending IPC transactions.
This is employed during REX signal processing in situations when the IPC
signal bits need to maintained when the user calls rex_clr_sigs().

@param[in]     p_ipc_info  Pointer to a valid IPC node for which contains the
                           pending IPC signals.

@param[in|out] p_clr_sigs  Pointer to signal bits to clear which were passed in
                           by the user.

@return
   0 on success
   Non zero otherwise.
*/
int rte_rex_clr_ipc_info(rex_ipc_info_type *p_ipc_info, rex_sigs_type *p_clr_sigs);

rex_ipc_info_type *rte_rex_get_ipc_info(rex_tcb_type *p_tcb);
#define IPC_NODE_INFO(p_tcb) rte_rex_get_ipc_info((rex_tcb_type*)p_tcb)

#ifdef __cplusplus
}
#endif

#endif /* IPCREX_H */


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           S E C U R I T Y   S E R V I C E S   T A S K
                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

   ipc_post_msg()        - Allocate a new message

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/




/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/kernel/rte/src/ipcrex.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/20/06    gr     Replaced errno.h with IxErrno.h to avoid conflicts with
                   system files.
10-May-04  rwh     Feature-ized with FEATURE_REX_IPC
03/10/13   et      Created module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_REX_IPC

#include "pq.h"
#include "ipcdefi.h"
#include "ipcrex.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/* Critical section for ipc_info */
extern rex_crit_sect_type ipc_info_crit_sect;

/*===========================================================================
              DEFINITIONS AND CONSTANTS
===========================================================================*/


/*===========================================================================
              DEFINITIONS AND TYPES DECLARATIONS
===========================================================================*/



/*---------------------------------------------------------------------------
  External function declarations.
---------------------------------------------------------------------------*/
void ipc_post_msg
(
   ipc_internal_msg_type* msg_ptr,
   rex_tcb_type* tcb_ptr
)
{
   /* Post message to the receiver
   **/
   rex_ipc_info_type *p_ipc_info_type = rte_rex_get_ipc_info(tcb_ptr);
   /* Protecting access to tcb_ptr ipc_info */
   #ifdef ARCH_QDSP6
   rex_enter_crit_sect(&ipc_info_crit_sect);
   #else
   INTLOCK();
   #endif /* #ifdef ARCH_QDSP6 */
   
   {
      /* Add the message in the priority queue */
      pq_insert_msg( msg_ptr, p_ipc_info_type->mq_ptr );
   
      /* Signal the receiver that a message has arrived
      ** only when it expects this particular message **/
      if ( (p_ipc_info_type->msg_wanted == IPC_MSG_ANY)
           ||
           (p_ipc_info_type->msg_wanted == msg_ptr->msg_id) )
      {
         p_ipc_info_type->msg_wanted = IPC_MSG_ANY;
         (void)rex_set_sigs( tcb_ptr, p_ipc_info_type->receive_signal );
      }
   }

   #ifdef ARCH_QDSP6
   rex_leave_crit_sect(&ipc_info_crit_sect);
   #else
   INTFREE();
   #endif /* #ifdef ARCH_QDSP6 */
}

void rex_register_receive_signal
(
   rex_sigs_type signal
)
{
   rex_ipc_info_type *info_ptr = rte_rex_get_ipc_info(rex_self());
   info_ptr->receive_signal = signal;
}

#endif /* FEATURE_REX_IPC */


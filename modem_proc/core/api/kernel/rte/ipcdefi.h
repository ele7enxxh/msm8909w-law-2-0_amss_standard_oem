#ifndef IPCDEFI_H
#define IPCDEFI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           S E C U R I T Y   S E R V I C E S   T A S K
                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/




/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/kernel/rte/ipcdefi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/13   et      Created module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
              DEFINITIONS AND CONSTANTS
===========================================================================*/
#define IPC_MSG_OVERHEAD offsetof(ipc_internal_msg_type,msg_id)

#define IPC_MSG_EXT_TO_INT(msg)                                      \
    ((ipc_internal_msg_type*) (((uint8*)(msg)) - IPC_MSG_OVERHEAD))

#define IPC_MSG_INT_TO_EXT(msg)                                      \
    ((union ipc_msg_type*) (((uint8*)(msg)) + IPC_MSG_OVERHEAD))


/*===========================================================================
              DEFINITIONS AND TYPES DECLARATIONS
===========================================================================*/
typedef struct ipc_internal_msg_type
{
   /*----------Internal Data-------*/
   void*  next_ptr;
   uint32 owner;
   uint32 sender;
   uint32 receiver;
   uint32 dummy; /* This is added to take care of 8byte allignment in qdsp6 */
   /*----------External Data-------*/
   /*The next two fields _MUST_ always be last and in this exact order */
   uint32 msg_id;
   char   data[1];
} ipc_internal_msg_type;

#endif /* IPCDEFI_H */

#ifndef IPC_DISPATCH_H
#define IPC_DISPATCH_H

/**
   @file ipc_dispatch.h

   @brief 
   Generic IPC Dispatcher Header File.

   Externalized Functions:
   ipc_dispatch_msg,
   ipc_dispatch_init,
   ipc_dispatch_register,
   ipc_dispatch_deregister.

   Initialization and sequencing requirements:
   None.
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          H E A D E R   F O R
               G E N E R I C  I P C  D I S P A T C H E R

Copyright (c) 2006-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/ipc_dispatch.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/11/11   leo    (Tech Pubs) Edited Doxygen comments and markup.
12/07/10   nk     Doxygenated 
10/25/06   vdr    Start using KxIPC
08/11/06   gr     Changed ipc_dispatch_func_type definition for backward
                  compatibility.
07/29/06   gr     Initial Revision.

===========================================================================*/


/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include "IxErrno.h"
#include "ixipc.h"

/*===========================================================================

                     PUBLIC DATA DECLARATIONS

===========================================================================*/
/** @addtogroup SMECOMUTIL_IPC 
@{
*/

/**
   Defines the function pointer for the IPC dispatch of the type 
   ipc_dispatch_func_type. 
*/
typedef void (*ipc_dispatch_func_type) (ixipc_msg_type **ipc_msg_ptr);

/**
   Initializes the IPC dispatch module.

   @return
   None.

   @dependencies 
   None.
*/
void ipc_dispatch_init (void);

/**
   Registers a dispatch function for a specified service ID.

   @param[in] func IPC dispatch function type.
   @param[in] service_id Unsigned integer specifying the service ID.

   @return
   Returns E_SUCCESS on success or an appropriate error code on failure.

   @dependencies 
   None.
*/
IxErrnoType ipc_dispatch_register (ipc_dispatch_func_type func,
unsigned int service_id);

/**
   De registers dispatch function.

   @param[in] func IPC dispatch function type.

   @return
   Returns E_SUCCESS on success or an appropriate error code on failure.

   @dependencies 
   None.
*/
IxErrnoType ipc_dispatch_deregister (ipc_dispatch_func_type func);

/**
   Dispatches an IPC message.

   The appropriate handler function is called depending on the service ID of the
   IPC message retrieved.

   @return
   Returns E_FAILURE if no handler is found or the status of the handler
   function if the handler is found. 

   @sideeffects 
   None.

   @dependencies 
   None.
*/
void ipc_dispatch_msg ( void );

/** @} */  /* end_addtogroup SMECOMUTIL_IPC */

#ifdef __cplusplus
}/* extern "C" */
#endif

#endif /* IPC_DISPATCH_H */

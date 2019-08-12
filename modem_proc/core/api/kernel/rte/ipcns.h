#ifndef IPCNS_H
#define IPCNS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           S E C U R I T Y   S E R V I C E S   T A S K
                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* ********************************************************************* */
/*                This file is documented with doc++.                    */
/*             Please do not change the syntax of comments.              */
/* ********************************************************************* */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/kernel/rte/ipcns.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/20/06    gr     Replaced errno.h with IxErrno.h to avoid conflicts with
                   system files.
03/10/13   et      Created module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "IxErrno.h"

#include "ipctypes.h"

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
              DEFINITIONS AND CONSTANTS
===========================================================================*/

/*===========================================================================
              DEFINITIONS AND TYPES DECLARATIONS
===========================================================================*/

/*---------------------------------------------------------------------------
  External function declarations.
---------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION IPCNS_SERVICE_REGISTER()

DESCRIPTION                                                                */
  /** Register a thread to provide a specific service.
   
@param service_id   ID of the service that will be provided
@param rex_tcb_ptr  Pointer to the thread tcb

@return {\bf None.}
*/
/*=========================================================================*/
boolean ipcns_service_register
(
   ipc_node_id_type service_id,
   void*            tcb_ptr
);


/*===========================================================================
FUNCTION IPCNS_SERVICE_LOCATE()

DESCRIPTION                                                                */
  /** Find a specific service.
      Maybe be a blocking or non-blocking call.
   
@param service_id   ID of the service to locate
@param block        The call is blocking if this is TRUE

@return {\bf The service ID located.}
*/
/*=========================================================================*/
ipc_node_id_type ipcns_service_locate
(
   ipc_node_id_type  service_id,
   boolean           block
);


/*===========================================================================
=============================================================================
=============================================================================
===                                                                       ===
===           I N T E R N A L    F U N C T I O N S   O N L Y              ===
===                                                                       ===
=============================================================================
=============================================================================
===========================================================================*/
/*===========================================================================
FUNCTION IPC_NODE_REGISTER

DESCRIPTION
 
PARAMETERS
  thread_ptr:  the thread to register

RETURN VALUE
  TRUE  - Registration was successful
  FALSE - Registration failed
 
SIDE EFFECTS
  Notifies potential clients.
===========================================================================*/
boolean ipcns_node_register
(
   void*    tcb_ptr
);


/*===========================================================================
FUNCTION IPC_NODE_DEREGISTER()

DESCRIPTION
  Remove a thread from the name server table.

PARAMETERS
  thread_ptr:  the thread to deregister

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.
===========================================================================*/
void ipcns_node_deregister
(
   void*    tcb_ptr
);


/*===========================================================================
FUNCTION IPCNS_GET_NODE()

DESCRIPTION                                                                */
  /** Get the internal information for a specific node.
   
@param node_id   ID of the node required

@return {\bf A pointer to the OS dependent structure describing that node.}
*/
/*=========================================================================*/
void* ipcns_node_get
(
   ipc_node_id_type node_id
);

#ifdef __cplusplus
}
#endif
#endif /* IPCNS_H */

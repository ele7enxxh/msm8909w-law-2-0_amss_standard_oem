#ifndef IXIPC_H
#define IXIPC_H

/**
   @file ixipc.h

   @brief 
   An abstraction layer for ipc.h for inter process communication.

   This module provides a wrapper layer that routes calls to RTE IPC.
   For more information on what the functions do, see ipc.h.

   Externalized Functions:
   ixipc_sig_wait

   Initialization and sequencing requirements:
   None.

*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                    H E A D E R   F O R
                  I X I P C  M O D U L E

Copyright (c) 2008-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                 EDIT HISTORY FOR MODULE

$Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/ixipc.h#1 $

when         who     what, where, why
--------     ---     --------------------------------------------------------
01/14/11     leo     (Tech Pubs) Edited Doxygen comments and markup.
12/11/10     nk      Doxygenated.
09/30/10     nk      Remove __IPC_DEBUG_MODE__ as not used anymore.
02/11/10     wa      Remove ipc.h include
01/20/10     wa      CMI Compliance

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include "rex.h"
#include "assert.h"
#include "ipcsvc.h"
#include "ipcns.h"
#include "rte_ipc.h"

/** @addtogroup SMECOMUTIL_IPC 
    @{
*/

/**
   Defines IPC signal types.
*/
typedef enum ixipc_sig_type_e
{
  IXIPC_REX_SIG_TYPE,        /**< IPC Real-time Executive (REX) OS signal. */
  IXIPC_KX_SIG_TYPE,         /**< IPC Quartz/Kernel (KX) signal.           */
IXIPC_REX_KX_SIG_TYPE      /**< IPC REX and KX signal type.                  */
} ixipc_sig_type;

/** @} */ /* end_addtogroup SMECOMUTIL_IPC */

/** @addtogroup SMECOMUTIL_IPC 
    @{
*/

/** 
   @brief
     Dummy definition for C functions.
*/
union ipc_msg_type {
uint32 dummy;            /**< Dummy implementation. */
};

/**
  @brief Type definition that abstracts the IPC message type.
*/
typedef union ipc_msg_type ixipc_msg_type;

/**
 Type definition that abstracts ixipc_node_id_type().
*/
typedef ipc_node_id_type ixipc_node_id_type;

/**
 Type definition that abstracts ixipc_errno_type().       
*/
typedef errno_enum_type ixipc_errno_type;

/** @} */ /* end_addtogroup SMECOMUTIL_IPC */

/** @addtogroup SMECOMUTIL_IPC 
    @{
*/

/**
  Suspends a task until one or more of the signals in the mask are set for
   the current task.

   @param[in] p_sigs Pointer to REX signals.
   @param[in, out] ipc_signals Pointer to IPC signals.

   @return
   Returns the signals that were set for the task.

   @dependencies
   None.

   @sideeffects
   Causes a task swap if the signals for the task are not already set at
   the time of the call.
*/
ixipc_sig_type ixipc_sig_wait (rex_sigs_type  *p_sigs, uint32 *ipc_signals);

/** @} */ /* end_addtogroup SMECOMUTIL_IPC */

/** @addtogroup SMECOMUTIL_IPC 
    @{
*/

/**
   Creates a new IPC message. This is an abstraction of the system call
   __ixipc_create().

   @param[in] mt  IPC message ID.
   @param[in] ptr Pointer to the message.
   @param[in] p   IPC message pool.
*/
#define ixipc_create(mt, ptr, p) \
__ixipc_create(sizeof(mt), (ipc_msg_id_type)mt##_id, ptr, p)

/**
   Creates a new IPC message of variable length.

   @param[in] mt  IPC message ID.
   @param[in] len Length of the message.
   @param[in] ptr Pointer to the message.
   @param[in] p   IPC message pool   
*/
#define ixipc_create_var(mt, len, ptr, p) \
__ixipc_create(len, (ipc_msg_id_type)mt##_id, ptr, p)

/**
   Blocking call to send an IPC message and wait for the response.
   The response pointer will be NULL if the call fails.

   @param[in] s_ptr  Pointer to the source message. 
   @param[in] node   Node containing the service ID for the service that sends
                    or receives IPC signals.
   @param[in] msg_id IPC message ID.
   @param[in] r_ptr  Pointer to the response message.
*/
#define ixipc_send_and_recv(s_ptr, node, msg_id, r_ptr) \
__ixipc_send_and_recv((ixipc_msg_type *) s_ptr, node, msg_id, r_ptr)

/**
   Sends an IPC message. This is an abstraction of the system call ipc_send().

   @param[in] msg_ptr Pointer to the message.
   @param[in] node_id Node ID.
*/
#define ixipc_send(msg_ptr, node_id) \
ipc_send((union ipc_msg_type**)(msg_ptr), (node_id))

/**
   Returns the sender of an IPC message.

   @param[in] msg_ptr Pointer to the message.
*/
#define ixipc_sender(msg_ptr) ipc_sender((union ipc_msg_type*)((void*)msg_ptr))

/**
   Macro to locate an IPC service. Calls ipcns_service_locate().

   @param[in] service_id Service ID.
*/
#define ixipc_service_locate(service_id)\
ipcns_service_locate ((service_id), TRUE)

/** @} */ /* end_addtogroup SMECOMUTIL_IPC */

/** @addtogroup SMECOMUTIL_IPC 
    @{
*/

/**
   Creates a new IPC message.

   @param[in] size IPC message size.
   @param[in] msg_id  Message ID.
   @param[in] pool_id Pool ID.

   @return
   Pointer to the allocated IPC message in memory. The pointer is of the type
   ixipc_msg_type.

   @dependencies
   None.
*/
ixipc_msg_type* ixipc_new (uint32 size, ipc_msg_id_type msg_id,
ipc_msg_pool_type pool_id);

/**
   Receives an IPC message.

   @param[in] msg_id IPC message ID.

   @return
   Pointer to the IPC message. The pointer is of the type ixipc_msg_type.

   @dependencies
   None.
*/
ixipc_msg_type* ixipc_receive (ipc_msg_id_type msg_id);

/**
   IPC call that waits for a signal.

   @param[in] ipc_signal IPC signal variable.

   @return
   TRUE --  Signal is acquired.\n
   FALSE -- Signal is not acquired.

   @dependencies
   None.
*/
boolean ixipc_message_waiting (rex_sigs_type ipc_signal);

/**
   Intializes the IPC.

   @param[in] node_id     Node ID.
   @param[in] receive_sig Signal used to receive IPC.

   @return
   None.

   @dependencies
   None.
*/
void ixipc_init (ixipc_node_id_type node_id, uint32 receive_sig);

/**
   Intializes the client.

   @param[in] mem_name Pointer to the memory name.

   @return
   None.

   @dependencies
   None.
*/
void ixipc_init_client (const char* mem_name);

/**
   Gets the service ID. Calls ipc_get_my_service_id().

   @return
   Returns the callee service ID.

   @dependencies
   None.
*/
uint32 ixipc_get_my_service_id (void);

/**
   Gets the message length.

   @param[in] msg_ptr Pointer to the message.

   @return
   Returns 0.

   @dependencies
   None.
*/
ixipc_node_id_type ixipc_length (ixipc_msg_type* msg_ptr);

/**
   Deregisters and deletes the service node, thus terminating the IPC
   service and freeing all memory.

   @return
   None.

   @dependencies
   None.
*/
void ixipc_terminate (void);

/**
   Allocates memory for IPC messages.

   @param[in] size Message size. 
   @param[in] id   Message ID.
   @param[in] ptr  Pointer to the message.
   @param[in] pool Message pool type.

   @return
   None.

   @dependencies
   None.
*/
void __ixipc_create (uint32 size, ipc_msg_id_type id,
ixipc_msg_type **ptr, ipc_msg_pool_type  pool);

/**
   Sends and receives IPC messages. Calls __ipc_send_and_recv().

   @param[in] request     Pointer to the message request type. 
   @param[in] destination Destination node ID to which the IPC messages are
                         posted or sent.
   @param[in] response_id Message response ID type.
   @param[in] response    Pointer to the message response type.

   @return
   None.

   @dependencies
   None.
*/
void __ixipc_send_and_recv (ixipc_msg_type *request, uint32 destination,
ipc_msg_id_type response_id, ixipc_msg_type **response);

/**
   Frees the memory allocated for IPC messages. Calls ipc_delete().

   @param[in] msg_ptr Pointer to the message.

   @return
    None.

   @dependencies
   None.
*/
void ixipc_delete (ixipc_msg_type **msg_ptr);

/**
   Calls native initialization calls. 

   @param[in] client_name Client name.

   @return
   None.

   @dependencies
   None.
*/
void ixinit_init_system (const char *client_name);

/** @} */  /* end_addtogroup SMECOMUTIL_IPC */

#ifdef __cplusplus
}
#endif

#endif /* IXIPC_H */

/** @addtogroup MC_RTM
 * @{
 * 
 * @file DrApiIpcMsg.h
 * @brief  Secure Driver IPC API
 *
 * Copyright (c) 2013 TRUSTONIC LIMITED
 * All rights reserved
 * 
 * The present software is the confidential and proprietary information of
 * TRUSTONIC LIMITED. You shall not disclose the present software and shall
 * use it only in accordance with the terms of the license agreement you
 * entered into with TRUSTONIC LIMITED. This software may be subject to
 * export or import laws in certain countries.
 */

#ifndef RTMIPCMSG_H_
#define RTMIPCMSG_H_

#include "DrApiCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
/** Possible message types/event types of the system. */
typedef enum {
    MSG_NULL = 0,  // Used for initializing state machines
    /***************/
    MSG_RQ                  = 1,  /**< Request; client -> server;  */
    MSG_RS                  = 2,  /**< Response; server -> client */
    MSG_RD                  = 3,  /**< Ready; server -> IPCH */
    MSG_NOT                 = 4,  /**< Notification; client -> IPCH; */
    MSG_CLOSE_TRUSTLET      = 5,  /**< Close Trustlet; MSH -> IPCH; IPCH -> all servers */
    MSG_CLOSE_TRUSTLET_ACK  = 6,  /**< Close Trustlet Ack; servers -> IPCH */
    MSG_MAP                 = 7,  /**< Map; Driver <-> IPCH; */
    MSG_ERR_NOT             = 8,  /**< Error Notification;  EXCH/SIQH -> IPCH; */
    MSG_CLOSE_DRIVER        = 9,  /**< Close Driver; MSH -> IPCH; IPCH -> Driver */
    MSG_CLOSE_DRIVER_ACK    = 10, /**< Close Driver Ack; Driver -> IPCH; IPCH -> MSH */
    MSG_GET_DRIVER_VERSION  = 11, /**< GetDriverVersion; client <-> IPCH */
    MSG_GET_DRAPI_VERSION   = 12, /**< GetDrApiVersion; Driver <-> IPCH */
    MSG_SET_NOTIFICATION_HANDLER  = 13, /**< Set (change) the SIQ handler thread; Driver <-> IPCH */
    MSG_GET_REGISTRY_ENTRY  = 14, /**< Get registry entry; Driver <-> IPCH */
    MSG_DRV_NOT             = 15, /**< Notification Driver -> Trustlet */
    MSG_SET_FASTCALL_HANDLER = 16, /**< Fastcall handler installation <-> Trustlet */
    MSG_GET_CLIENT_ROOT_AND_SP_ID   = 17,  /**<   Driver <-> IPCH **/
} message_t;


//------------------------------------------------------------------------------
/** Waits with infinite timeout for IPC.
 *
 * @param ipcPartner IPC partner to signal.
 * @param pMr0 IPC register 0.
 * @param pMr1 IPC register 1.
 * @param pMr2 IPC register 2.
 *
 * @returns DRAPI_OK in case of success
 * @returns any combination of DRAPI_ERROR_CREATE(Driver specific error code, MTK error code) 
 */
drApiResult_t drApiIpcWaitForMessage(
    threadid_t        *pIpcPartner,
    uint32_t          *pMr0,
    uint32_t          *pMr1,
    uint32_t          *pMr2
);

//------------------------------------------------------------------------------
/**
 * Send ready message or answer to IPCH and waits for a client request
 *
 * @param ipcPeer Destination to send message to.
 * @param ipcMsg IPC message.
 * @param ipcData Additional IPC data.
 *
 * @returns DRAPI_OK in case of success
 * @returns any combination of DRAPI_ERROR_CREATE(Driver specific error code, MTK error code)
 */
drApiResult_t drApiIpcCallToIPCH(
    threadid_t       *pIpcPeer,
    message_t        *pIpcMsg,
    uint32_t         *pIpcData
);

//------------------------------------------------------------------------------
/** Sets signal.
 * The signal (SIGNAL) is used by a thread to inform another thread about an event.
 * The signal operation is asynchronous, which means that the operation will return
 * immediately without blocking the user. Function uses auto-clear signals, meaning that
 * the signal is cleared automatically when the receiver receives it.
 *
 * It is up to the destination of the signal to pick up and process the information.
 *
 * @param receiver Thread to set the signal for.
 *
 * @returns DRAPI_OK in case of success
 * @returns any combination of DRAPI_ERROR_CREATE(Driver specific error code, MTK error code)
 */
drApiResult_t drApiIpcSignal(
    const threadid_t  receiver
);

//------------------------------------------------------------------------------
/** Signal wait operation.
 * A thread uses the sigWait operation to check if a signal has occurred. If no signal is
 * pending the thread will block until a signal arrives.
 *
 * @returns DRAPI_OK in case of success
 * @returns any combination of DRAPI_ERROR_CREATE(Driver specific error code, MTK error code)
 */
drApiResult_t drApiIpcSigWait( void );

//------------------------------------------------------------------------------
/** Notify NWd driver
 *
 * @returns DRAPI_OK in case of success
 * @returns any combination of DRAPI_ERROR_CREATE(Driver specific error code, MTK error code)
 */
drApiResult_t drApiNotify( void );


//------------------------------------------------------------------------------
/**
 * Makes control syscall with given parameters
 *
 * @param controlid control id
 * @param param1    parameter 1
 * @param param2    parameter 2
 * @param param3    parameter 3
 * @param param4    parameter 4
 * @param *data     set by control syscall
 *
 * @returns DRAPI_OK in case of success
 * @returns any combination of DRAPI_ERROR_CREATE(Driver specific error code, MTK error code)
 */
drApiResult_t drApiSyscallControl(
    uint32_t controlid,
    uint32_t param1,
    uint32_t param2,
    uint32_t param3,
    uint32_t param4,
    uint32_t *data
);


//------------------------------------------------------------------------------
/** Reads OEM data starting from given offset
 *
 * @param offset  data offset
 * @param data    set by control syscall
 *
 * @returns DRAPI_OK in case of success
 * @returns any combination of DRAPI_ERROR_CREATE(Driver specific error code, MTK error code)
 */
drApiResult_t drApiReadOemData(
        const uint32_t  offset,
        uint32_t        *data
);

//------------------------------------------------------------------------------
/** Sends notification to client
 *
 * @param client Client's thread id
 * *
 * @return MTK return code
 */
drApiResult_t drApiNotifyClient(
    const threadid_t  client
);

//------------------------------------------------------------------------------
/** Returns ID of Root and Service Provider ID of the specified client
 *
 * @param rootId ID of Root 
 * @param spId Service Provider ID
 * @param client Thread ID
 * 
 * @returns DRAPI_OK in case of success
 * @returns any combination of DRAPI_ERROR_CREATE(Driver specific error code, MTK error code)
 */
drApiResult_t drApiGetClientRootAndSpId(
    uint32_t        *rootId,
    uint32_t        *spId,
    const threadid_t  client
);


//------------------------------------------------------------------------------
/**
 * Handles unknown messages
 * This function has to be called by Driver if it receives a message it doesn’t recognize 
 *
 * @param ipcPeer Sender of message.
 * @param ipcMsg IPC message.
 * @param ipcData Additional IPC data.
 *
 * @returns DRAPI_OK in case of success
 * @returns any combination of DRAPI_ERROR_CREATE(Driver specific error code, MTK error code)
 */
drApiResult_t drApiIpcUnknownMessage(
    threadid_t       *pIpcPeer,
    message_t        *pIpcMsg,
    uint32_t         *pIpcData
);


#ifdef __cplusplus
}
#endif

#endif /** RTMIPCMSG_H_ */

/** @} */

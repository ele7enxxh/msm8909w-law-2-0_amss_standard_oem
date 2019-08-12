/**
 * @addtogroup TLAPI_COM
 * @{
 * The MobiCore OS provides Trustlets with a set of functions for inter-world communication.
 * Communication is based on world-shared memory buffers and notifications without message payload.
 * <BR>
 * In MobiCore OS, message-formatting is application specific.
 * Messages are interchanged on world shared memory buffers that the
 * TrustletConnector (TLC) specifies in mcOpenSession() and mcMap().
 * The MobiCore driver and MobiCore OS set up this shared memory buffer
 * between TLC and Trustlet and forward notifications between the two.
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
#ifndef TLAPICOM_H
#define TLAPICOM_H

#include "TlApi/TlApiCommon.h"
#include "TlApi/TlApiError.h"

/** Wait for a notification of the NWd.
 * Trustlets use the tlApiWaitNotification() to wait for a notification by their TrustletConnector.
 * Calling tlApiWaitNotification() will block the Trustlet until a notification dedicated to the Trustlet session arrives.
 * Depending on the underlying hardware platform a wait timeout may be provided. If the waiting timeout is not supported by
 * the platform E_TLAPI_INVALID_TIMEOUT is returned.
 *
 * @param timeout Time in milliseconds to wait (TLAPI_NO_TIMEOUT => direct return, TLAPI_INFINITE_TIMEOUT => wait infinitely
 * @return TLAPI_OK if notification has been received.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiWaitNotification(uint32_t timeout);

/** Notify the TrustletConnector about changes in the TCI message buffer.
 * Trustlets must use the tlApiNotify() function to inform the MobiCore TEE that the session channel
 * contains information to be processed by its TrustletConnector. Trustlets must not make assumptions
 * at what point in time the information will be processed by the TrustletConnector. It is up to MobiCore TEE
 * to decide when to inform the TrustletConnector. tlApiNotify() returns when the notification is processed and
 * the Trustlet can continue.
 *
 * @return TLAPI_OK if notification has been issued successfully.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiNotify(void);

#endif // TLAPICOM_H
/** @} */

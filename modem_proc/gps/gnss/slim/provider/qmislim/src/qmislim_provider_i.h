#ifndef SLIM_QMISLIM_PROVIDER_I_H
#define SLIM_QMISLIM_PROVIDER_I_H

/**
  @file
  @brief Private declarations for QMI-SLIM SLIM service provider implementation.
  @ingroup slim_QSProvider

  Copyright (c) 2014-2015 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.

 */
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/provider/qmislim/src/qmislim_provider_i.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "qmislim_common_api.h"
#include "qmislim_provider_api.h"
#include "qmislim_qmi_client_api.h"
#include "qmislim_qmi_monitor_api.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/**
  @brief Timed event type

  @sa slim_QSProvider_GetNextTimerEvent
  @ingroup slim_QSProvider
 */
#define SLIM_QS_PROVIDER_EVENT_TIMEOUT ((uint32)1)

/**
  Defines offset in bytes between beginning of slim_QSProviderT and
  slim_ServiceProviderInterfaceT.
  @ingroup slim_QSProvider
 */
#define SLIM_QS_PROVIDER_P2M(member) SLIM_QS_P2M_OFFSET(slim_QSProviderT,member)
/**
  @brief Offset for virtual function table for QMI-SLIM Provider
  @ingroup slim_QSProvider
 */
#define SLIM_QS_PROVIDER_P2M_VTBL    SLIM_QS_PROVIDER_P2M(p_Vtbl)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @brief Initializes SLIM provider object

  @ingroup slim_QSProvider
 */
boolean slim_QSProvider_Init
(
  slim_QSProviderT *pz_Object,
  /**< [in] Provider object.  */
  slim_QSNodeT e_ThisNode,
  /**< [in] This node type */
  slim_QSNodeT e_PeerNode
  /**< [in] Peer node type */
);
/**
  @brief Destroys provider object

  @ingroup slim_QSProvider
 */
boolean slim_QSProvider_Deinit
(
  slim_QSProviderT *pz_Object
  /**< [in] Provider object */
);
/**
  @brief Attempts peer connect if not connected

  @ingroup slim_QSProvider
 */
boolean slim_QSProvider_TryConnect
(
  slim_QSProviderT *pz_Object
  /**< [in] Provider object */
);
/**
  @brief Checks for pending timed operations

  The provider may have pending timed operations. If there are any, this
  method allows controller to query next event time and hint.

  @retval TRUE If there is a pending event
  @retval FALSE Error or there is no pending event
  @sa slim_QSProvider_ProcessTimerEvent
  @ingroup slim_QSProvider
 */
boolean slim_QSProvider_GetNextTimerEvent
(
  slim_QSProviderT *pz_Object,
  /**< [in] Provider object */
  uint64 *pt_EventTimeMs,
  /**< [out] Time until next event */
  uint32 *pq_EventType
  /**< [out] Event identifier */
);
/**
  @brief Handles timed event

  The controller invokes this method when the timer has been armed after call
  to @ref slim_QSProvider_GetNextTimerEvent and time has elapsed.

  @sa slim_QSProvider_GetNextTimerEvent
  @ingroup slim_QSProvider
 */
boolean slim_QSProvider_ProcessTimerEvent
(
  slim_QSProviderT *pz_Object,
  /**< [in] Provider object */
  uint32 q_EventType
  /**< [in] Event identifier */
);

/**
  @brief Computes object reference from SLIM interface
  @ingroup slim_QSProvider
 */
static INLINE slim_QSProviderT *slim_QSProviderIf2Obj
(
  slim_ServiceProviderInterfaceType *p_Iface
  /**< [in] SLIM Core Service Provider interface (or NULL) */
)
{
  return slim_QSMember2Container ((void*) p_Iface, SLIM_QS_PROVIDER_P2M_VTBL);
}
/**
  @brief Static cast from implementation object to SLIM interface
  @ingroup slim_QSProvider
 */
static INLINE slim_ServiceProviderInterfaceType *slim_QSProviderObj2If
(
  slim_QSProviderT *p_Object
  /**< [in] Provider object */
)
{
  return slim_QSContainer2Member (p_Object, SLIM_QS_PROVIDER_P2M_VTBL);
}

#endif /* SLIM_QMISLIM_PROVIDER_I_H */


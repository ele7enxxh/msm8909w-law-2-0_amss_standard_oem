#ifndef QMISLIM_TARGET_H
#define QMISLIM_TARGET_H

/**
 @file
 @brief Target-specific definitions for MPSS
 @ingroup slim_QS

 This file contains definitions for QMI-SLIM MPSS integration.
*/

/*----------------------------------------------------------------------------
  Copyright (c) 2014-2015 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.
 * -------------------------------------------------------------------------*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/qmislim/mpss/inc/qmislim_target.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_processor.h"
#include "qmi_client.h"
#include "qmi_csi.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
  @addtogroup slim_QS
  @{
*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/*! @brief Enable DM logging for monitor */
#define FEATURE_LOCTECH_QMISLIM_MONITOR_LOG 1
/*! @brief Enable DM logging for service */
#define FEATURE_LOCTECH_QMISLIM_SERVICE_LOG 1
/*! @brief Enable DM logging for client */
#define FEATURE_LOCTECH_QMISLIM_CLIENT_LOG 1
/**
 @brief Produces high-32 bits from a pointer value.
 @param[in] x Pointer
 @return 0 on modem, high-32 bits on 64-bit architectures.
 @sa SLIM_QS_LOW
 */
#define SLIM_QS_HIGH(x) ((uint32)0)
/**
 @brief Produces low-32 bits from a pointer value.
 @param[in] x Pointer
 @return low-32 bits from a pointer value.
 @sa SLIM_QS_HIGH
 */
#define SLIM_QS_LOW(x)  ((uint32)(uintptr_t)(x))

/* DIAG logging integration.

   QMI-SLIM uses 4-level logging macros which are mapped into 4-levels with
   DIAG logging levels: LOW, MEDIUM, HIGH and ERROR.

   When mapping to OS-specific logs, appropriate mapping shall be used.
 */
/**
  @brief Logs QMI-SLIM message without parameters.

  @param[in] Level Message logging level.
  @param[in] pStr  Message text.
*/
#define SLIM_QS_MSG0(Level, pStr)    SLIM_MSG0(Level, "[QMISLIM] " pStr)
/**
  @brief Logs QMI-SLIM message with a single parameter.

  @param[in] Level Message logging level.
  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
*/
#define SLIM_QS_MSG1(Level, pStr, qA)  SLIM_MSG1(Level, "[QMISLIM] " pStr, qA)
/**
  @brief Logs QMI-SLIM message with two parameters.

  @param[in] Level Message logging level.
  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
  @param[in] qB    Parameter casted to uint32.
*/
#define SLIM_QS_MSG2(Level, pStr, qA, qB) \
  SLIM_MSG2(Level, "[QMISLIM] " pStr, qA, qB)
/**
  @brief Logs QMI-SLIM message with three parameters.

  @param[in] Level Message logging level.
  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
  @param[in] qB    Parameter casted to uint32.
  @param[in] qC    Parameter casted to uint32.
*/
#define SLIM_QS_MSG3(Level, pStr, qA, qB, qC) \
  SLIM_MSG(Level, "[QMISLIM] " pStr, qA, qB, qC)

/**
  @brief Logs QMI-SLIM message without parameters with low priority.

  @param[in] pStr Message text.
*/
#define SLIM_QS_MSG0L(pStr) SLIM_QS_MSG0(MSG_LEGACY_LOW, pStr)
/**
  @brief Logs QMI-SLIM message without parameters with medium priority.

  @param[in] pStr Message text.
*/
#define SLIM_QS_MSG0M(pStr) SLIM_QS_MSG0(MSG_LEGACY_MED, pStr)
/**
  @brief Logs QMI-SLIM message without parameters with high priority.

  @param[in] pStr Message text.
*/
#define SLIM_QS_MSG0H(pStr) SLIM_QS_MSG0(MSG_LEGACY_HIGH, pStr)
/**
  @brief Logs QMI-SLIM message without parameters with error priority.

  @param[in] pStr Message text.
*/
#define SLIM_QS_MSG0E(pStr) SLIM_QS_MSG0(MSG_LEGACY_ERROR, pStr)
/**
  @brief Logs QMI-SLIM message without parameters with fatal priority.

  @param[in] pStr Message text.
*/
#define SLIM_QS_MSG0F(pStr) SLIM_QS_MSG0(MSG_LEGACY_FATAL, pStr)
/**
  @brief Logs QMI-SLIM message with a single parameter with low priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
*/
#define SLIM_QS_MSG1L(pStr ,qA) SLIM_QS_MSG1(MSG_LEGACY_LOW, pStr, qA)
/**
  @brief Logs QMI-SLIM message with a single parameter with medium priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
*/
#define SLIM_QS_MSG1M(pStr, qA) SLIM_QS_MSG1(MSG_LEGACY_MED, pStr, qA)
/**
  @brief Logs QMI-SLIM message with a single parameter with high priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
*/
#define SLIM_QS_MSG1H(pStr, qA) SLIM_QS_MSG1(MSG_LEGACY_HIGH, pStr, qA)
/**
  @brief Logs QMI-SLIM message with a single parameter with error priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
*/
#define SLIM_QS_MSG1E(pStr, qA) SLIM_QS_MSG1(MSG_LEGACY_ERROR, pStr, qA)
/**
  @brief Logs QMI-SLIM message with a single parameter with fatal priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
*/
#define SLIM_QS_MSG1F(pStr, qA) SLIM_QS_MSG1(MSG_LEGACY_FATAL, pStr, qA)
/**
  @brief Logs QMI-SLIM message with two parameters with low priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
  @param[in] qB    Parameter casted to uint32.
*/
#define SLIM_QS_MSG2L(pStr, qA, qB) SLIM_QS_MSG2(MSG_LEGACY_LOW, pStr, qA, qB)
/**
  @brief Logs QMI-SLIM message with two parameters with medium priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
  @param[in] qB    Parameter casted to uint32.
*/
#define SLIM_QS_MSG2M(pStr, qA, qB) SLIM_QS_MSG2(MSG_LEGACY_MED, pStr, qA, qB)
/**
  @brief Logs QMI-SLIM message with two parameters with high priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
  @param[in] qB    Parameter casted to uint32.
*/
#define SLIM_QS_MSG2H(pStr, qA, qB) SLIM_QS_MSG2(MSG_LEGACY_HIGH, pStr, qA, qB)
/**
  @brief Logs QMI-SLIM message with two parameters with error priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
  @param[in] qB    Parameter casted to uint32.
*/
#define SLIM_QS_MSG2E(pStr, qA, qB) \
  SLIM_QS_MSG2(MSG_LEGACY_ERROR, pStr, qA, qB)

/**
  @brief Logs QMI-SLIM message with three parameters with low priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
  @param[in] qB    Parameter casted to uint32.
  @param[in] qC    Parameter casted to uint32.
*/
#define SLIM_QS_MSG3L(pStr, qA, qB, qC) \
  SLIM_QS_MSG3(MSG_LEGACY_LOW, pStr, qA, qB, qC)
/**
  @brief Logs QMI-SLIM message with three parameters with medium priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
  @param[in] qB    Parameter casted to uint32.
  @param[in] qC    Parameter casted to uint32.
*/
#define SLIM_QS_MSG3M(pStr, qA, qB, qC) \
  SLIM_QS_MSG3(MSG_LEGACY_MED, pStr, qA, qB, qC)
/**
  @brief Logs QMI-SLIM message with three parameters with high priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
  @param[in] qB    Parameter casted to uint32.
  @param[in] qC    Parameter casted to uint32.
*/
#define SLIM_QS_MSG3H(pStr, qA, qB, qC) \
  SLIM_QS_MSG3(MSG_LEGACY_HIGH, pStr, qA, qB, qC)
/**
  @brief Logs QMI-SLIM message with three parameters with error priority.

  @param[in] pStr  Message text.
  @param[in] qA    Parameter casted to uint32.
  @param[in] qB    Parameter casted to uint32.
  @param[in] qC    Parameter casted to uint32.
*/
#define SLIM_QS_MSG3E(pStr, qA, qB, qC) \
  SLIM_QS_MSG3(MSG_LEGACY_ERROR, pStr, qA, qB, qC)

/*
  Execution context for logging. These macros are used for marking logging
  execution context scope on APSS platform. On MPSS the macros are not doing
  anything.
 */
/**
  @brief Execution context push macro

  The macro associates a name with a callback thread. The macro shall be
  accompanied with #SLIM_QS_CB_CTX_POP.

  On modem macro does nothing.

  @param[in] pName Thread context name.
  @sa SLIM_QS_CB_CTX_POP
*/
#define SLIM_QS_CB_CTX_PUSH(pName) do {} while (0)
/**
  @brief Execution context pop macro

  Marks end of callback execution context.

  On modem macro does nothing.

  @sa SLIM_QS_CB_CTX_POP
*/
#define SLIM_QS_CB_CTX_POP() do {} while(0)

/* Target configuration constants */
/** MPSS service provider always uses MP node identifier */
#define SLIM_QS_THIS_NODE         eSLIM_QS_NODE_MP

/**
  @brief Signal used by QMI-SLIM QCSI service components

  QMI-SLIM maps signal into @ref os_SignalMsgType IPC message with
  @ref IPC_SLIM_QS_TO_TASK_QCSI_SIGNAL_MSG identifier.

  @sa slim_QSCciOsParamsInit
  @ingroup slim_QSQmi
 */
#define SLIM_QS_QCSI_SIGNAL ((uint32)0x00000100)

/* QMI stack integration */

/**
  @brief QCSI parameters initialization

  Prepares OS parameters for usage with QCSI framework. This operation
  is required on MP and not required on AP.

  @ingroup slim_QSQmi
  @sa slim_QSQmi
 */
#define SLIM_QS_OS_PARAMS_INIT(OsParams) \
  slim_QSCsiOsParamsInit(&(OsParams))

/**
  @brief Register QCSI parameters for IPC messaging
  @ingroup slim_QSQmi
  @sa slim_QSQmi
 */
#define SLIM_QS_OS_PARAMS_ENABLE(OsParams) \
  os_IpcMapSignals((OsParams).sig, IPC_SLIM_QS_TO_TASK_QCSI_SIGNAL_MSG)

/**
  @brief Unregister QCSI parameters from IPC messaging
  @ingroup slim_QSQmi
  @sa slim_QSQmi
 */
#define SLIM_QS_OS_PARAMS_DISABLE(OsParams) \
  os_IpcUnmapSignals((OsParams).sig)

/**
  @brief Translates QMI IPC message into QCSI OS parameters

  On REX the IPC message shall contain signal mask.

  On Linux/Windows IPC message shall contain @a qcsi_os_params contents.

  @ingroup slim_QSQmi
  @sa slim_QSQmi
 */
#define SLIM_QS_OS_PARAMS_UNPACK_IPC(IpcMsg,OsParams) \
  slim_QSCsiUnpackIpc(&(IpcMsg), &(OsParams))

/**
  @brief Initializes QCCI OS parameters

  @param[in] OsParams Parameters to initialize.

  @ingroup slim_QSQmi
  @sa slim_QSQmi
 */
#define SLIM_QS_QCCI_PARAMS_INIT(OsParams) \
  slim_QSCciOsParamsInit(&(OsParams))


/**
  @brief Maximum number of QMI-SLIM provider objects (total)

  @ingroup slim_QSAlloc
 */
#define SLIM_QS_ALLOCATOR_PROVIDER_COUNT        1
/**
  @brief Maximum number of total opened connections

  Maximum number of total opened connections shall be a multiplication of
  total number of providers per total number of supported clients.

  @ingroup slim_QSAlloc
 */
#define SLIM_QS_ALLOCATOR_PROVIDER_CONN_COUNT   SLIM_CLIENT_MAX_COUNT
/**
  @brief Maximum number of opened transactions

  Each SLIM-Core request opens a transaction if the operation requires
  a response.

  @ingroup slim_QSAlloc
 */
#define SLIM_QS_ALLOCATOR_PROVIDER_TX_COUNT     (SLIM_CLIENT_MAX_COUNT * 5)
/**
  @brief Maximum number of opened QMI-SLIM transactions

  @ingroup slim_QSAlloc
 */
#define SLIM_QS_ALLOCATOR_QMI_CLT_TX_COUNT      (6)
/**
  @brief Maximum number of QMI-SLIM service client connections

  @ingroup slim_QSAlloc
 */
#define SLIM_QS_ALLOCATOR_QMI_SVC_CLT_COUNT     2

/**
  @brief Maximum number of test connections

  @ingroup slim_QSAlloc
 */
#define SLIM_QS_TEST_CONNECTION_COUNT 0u
/**
  @brief Maximum number of AP connections

  @ingroup slim_QSAlloc
 */
#define SLIM_QS_AP_CONNECTION_COUNT   1u
/**
  @brief Maximum number of MP connections

  @ingroup slim_QSAlloc
 */
#define SLIM_QS_MP_CONNECTION_COUNT   0u

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
  @brief Mutex type
 */
typedef os_MutexBlockType slim_QSMutexT;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @brief Initializes mutex

  @ingroup slim_QS
 */
static INLINE boolean slim_QSMutexInit
(
  slim_QSMutexT *pz_Mutex
  /**< [in] Mutex object */
)
{
  return os_MutexInit(pz_Mutex, MUTEX_DATA_ONLY_CONTEXT);
}

/**
  @brief Destroys mutex

  @ingroup slim_QS
 */
static INLINE boolean slim_QSMutexDeinit
(
  slim_QSMutexT *pz_Mutex
  /**< [in] Mutex object */
)
{
  return NULL == pz_Mutex ? FALSE : TRUE;
}

/**
  @brief Acquires mutex

  @ingroup slim_QS
 */
static INLINE void slim_QSMutexLock
(
  slim_QSMutexT *pz_Mutex
  /**< [in] Mutex object */
)
{
  os_MutexLock(pz_Mutex);
}

/**
  @brief Releases mutex

  @ingroup slim_QS
 */
static INLINE void slim_QSMutexUnlock
(
  slim_QSMutexT *pz_Mutex
  /**< [in] Mutex object */
)
{
  os_MutexUnlock(pz_Mutex);
}

/**
  @brief REX-specific QCSI parameter initialization

  @ingroup slim_QS
 */
static INLINE void slim_QSCsiOsParamsInit
(
  qmi_csi_os_params *pz_OsParams
  /**< [in] QCSI OS parameters */
)
{
  if (NULL != pz_OsParams)
  {
    pz_OsParams->tcb = os_Self();
    pz_OsParams->sig = SLIM_QS_QCSI_SIGNAL;
  }
}

/**
  @brief REX-specific QCCI parameter initialization

  @ingroup slim_QS
 */
static INLINE void slim_QSCciOsParamsInit
(
  qmi_client_os_params *pz_OsParams
  /**< [in] QCCI OS parameters */
)
{
  if (NULL != pz_OsParams)
  {
    pz_OsParams->tcb = os_Self();
    pz_OsParams->sig = C_OS_FLAG_QMI_CCI_EVENT;
  }
}

/**
  @brief REX-specific QCSI poll preparation

  @ingroup slim_QS
 */
static INLINE void slim_QSCsiUnpackIpc
(
  const os_IpcMsgType *pz_IpcMsgIn,
  /**< [in] IPC message with QCSI event */
  qmi_csi_os_params *pz_OsParamsOut
  /**< [out] QCSI OS parameters extracted from event message */
)
{
  if (NULL != pz_OsParamsOut && NULL != pz_IpcMsgIn)
  {
    os_SignalMsgType z_Data;
    memscpy ((void*) &z_Data, sizeof(z_Data),
      (void*) pz_IpcMsgIn->p_Data, sizeof(z_Data));
    pz_OsParamsOut->tcb = os_Self ();
    pz_OsParamsOut->sig = z_Data.q_SignalId;
  }
}

/** @} */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* QMISLIM_TARGET_H */

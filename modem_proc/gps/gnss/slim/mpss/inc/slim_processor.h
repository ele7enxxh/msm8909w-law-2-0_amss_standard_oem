#ifndef SLIM_PROCESSOR_H
#define SLIM_PROCESSOR_H
/**
@file
@brief SLIM processor-specific types and declarations.

  The content of this file is dependent on the host processor. For proper
  operation the following items shall be defined:
  - #SLIM_MSG_MED0
  - #SLIM_MSG_MED1
  - #SLIM_MSG_MED2
  - #SLIM_MSG_MED
  - #SLIM_MSG_HIGH0
  - #SLIM_MSG_HIGH1
  - #SLIM_MSG_HIGH2
  - #SLIM_MSG_HIGH
  - #SLIM_MSG_ERROR0
  - #SLIM_MSG_ERROR1
  - #SLIM_MSG_ERROR2
  - #SLIM_MSG_ERROR
  - #SLIM_CLIENT_MAX_COUNT
  - #SLIM_TASK_THREAD_ID
  - #SLIM_HANDLE_L
  - #slim_MutexStructType
  - #slim_ClientEnumType
  - #slim_ProviderEnumType

  QMI-SLIM requires the following definitions:
  - #SLIM_MSG0
  - #SLIM_MSG1
  - #SLIM_MSG2
  - #SLIM_MSG

@dependencies
- MSG_SSID_SLIM -- When defined, SLIM SSID is used for diagnostics logging.
- FEATURE_CGPS_LBS_TASK -- If MSG_SSID_SLIM is undefined, MSG_SSID_GPSSM is used
                           for logging if this variable is defined; otherwise
                           MSG_SSID_GNSS_LOCMW is used.
                           Sets task identifier to THREAD_ID_LBS
- FEATURE_CGPS_LOC_MIDDLEWARE -- SLIM task id is THREAD_ID_LOC_MIDDLEWARE

@ingroup slim_MPSS
*/
/*============================================================================
               Copyright (c) 2014-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/mpss/inc/slim_processor.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "aries_os_api.h"
#include "cgps_api.h"
#include "gps_types.h"
#include "msg.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @addtogroup slim_MPSS
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/* F3 logging */

/**
@def SLIM_MSG0
@brief Logs message
@param[in] LEVEL Message logging level
@param[in] str   Message to log

@def SLIM_MSG1
@brief Logs message with single argument
@param[in] LEVEL Message logging level
@param[in] str   Formatting string
@param[in] a     Argument.

@def SLIM_MSG2
@brief Logs message with two arguments
@param[in] LEVEL Message logging level
@param[in] str   Formatting string
@param[in] a     Argument.
@param[in] b     Argument.

@def SLIM_MSG
@brief Logs message with three arguments
@param[in] LEVEL Message logging level
@param[in] str   Formatting string
@param[in] a     Argument.
@param[in] b     Argument.
@param[in] c     Argument.

@def SLIM_MSG_MED0
@brief Logs medium priority message
@param[in] str Message to log.

@def SLIM_MSG_MED1
@brief Logs medium priority message with single argument.
@param[in] str Formatting string
@param[in] a     Argument.

@def SLIM_MSG_MED2
@brief Logs medium priority message with two arguments.
@param[in] str Formatting string
@param[in] a     Argument.
@param[in] b     Argument.

@def SLIM_MSG_MED
@brief Logs medium priority message with three arguments.
@param[in] str Formatting string
@param[in] a     Argument.
@param[in] b     Argument.
@param[in] c     Argument.

@def SLIM_MSG_HIGH0
@brief Logs high priority message
@param[in] str Message to log.

@def SLIM_MSG_HIGH1
@brief Logs high priority message with single argument.
@param[in] str Formatting string
@param[in] a     Argument.

@def SLIM_MSG_HIGH2
@brief Logs high priority message with two arguments.
@param[in] str Formatting string
@param[in] a     Argument.
@param[in] b     Argument.

@def SLIM_MSG_HIGH
@brief Logs high priority message with three arguments.
@param[in] str Formatting string
@param[in] a     Argument.
@param[in] b     Argument.
@param[in] c     Argument.

@def SLIM_MSG_ERROR0
@brief Logs error priority message
@param[in] str Message to log.

@def SLIM_MSG_ERROR1
@brief Logs error priority message with single argument.
@param[in] str Formatting string
@param[in] a     Argument.

@def SLIM_MSG_ERROR2
@brief Logs error priority message with two arguments.
@param[in] str Formatting string
@param[in] a     Argument.
@param[in] b     Argument.

@def SLIM_MSG_ERROR
@brief Logs error priority message with three arguments.
@param[in] str Formatting string
@param[in] a     Argument.
@param[in] b     Argument.
@param[in] c     Argument.

@def SLIM_TASK_THREAD_ID
@brief SLIM task identifier for IPC message routing.

Depending on the target build, SLIM can be run as a part of LOC_MW or LBS task.
@sa THREAD_ID_LOC_MIDDLEWARE THREAD_ID_LBS

*/

#ifdef MSG_SSID_SLIM
#define SLIM_MSG0(LEVEL, str)           MSG(MSG_SSID_SLIM, LEVEL, str)
#define SLIM_MSG1(LEVEL, str, a)        MSG_1(MSG_SSID_SLIM, LEVEL, str, a)
#define SLIM_MSG2(LEVEL, str, a, b)     MSG_2(MSG_SSID_SLIM, LEVEL, str, a, b)
#define SLIM_MSG(LEVEL, str, a, b, c)   MSG_3(MSG_SSID_SLIM, LEVEL, str, a, b, c)
#elif !defined(FEATURE_CGPS_LBS_TASK)
#define SLIM_MSG0(LEVEL, str)           MSG(MSG_SSID_GNSS_LOCMW, LEVEL, str)
#define SLIM_MSG1(LEVEL, str, a)        MSG_1(MSG_SSID_GNSS_LOCMW, LEVEL, str, a)
#define SLIM_MSG2(LEVEL, str, a, b)     MSG_2(MSG_SSID_GNSS_LOCMW, LEVEL, str, a, b)
#define SLIM_MSG(LEVEL, str, a, b, c)   MSG_3(MSG_SSID_GNSS_LOCMW, LEVEL, str, a, b, c)
#else
#define SLIM_MSG0(LEVEL, str)           MSG(MSG_SSID_GPSSM, LEVEL, str)
#define SLIM_MSG1(LEVEL, str, a)        MSG_1(MSG_SSID_GPSSM, LEVEL, str, a)
#define SLIM_MSG2(LEVEL, str, a, b)     MSG_2(MSG_SSID_GPSSM, LEVEL, str, a, b)
#define SLIM_MSG(LEVEL, str, a, b, c)   MSG_3(MSG_SSID_GPSSM, LEVEL, str, a, b, c)
#endif

#define SLIM_MSG_MED0(str)              SLIM_MSG0(MSG_LEGACY_MED, str)
#define SLIM_MSG_MED1(str, a)           SLIM_MSG1(MSG_LEGACY_MED, str, a)
#define SLIM_MSG_MED2(str, a, b)        SLIM_MSG2(MSG_LEGACY_MED, str, a, b)
#define SLIM_MSG_MED(str, a, b, c)      SLIM_MSG(MSG_LEGACY_MED, str, a, b, c)
#define SLIM_MSG_HIGH0(str)             SLIM_MSG0(MSG_LEGACY_HIGH, str)
#define SLIM_MSG_HIGH1(str, a)          SLIM_MSG1(MSG_LEGACY_HIGH, str, a)
#define SLIM_MSG_HIGH2(str, a, b)       SLIM_MSG2(MSG_LEGACY_HIGH, str, a, b)
#define SLIM_MSG_HIGH(str, a, b, c)     SLIM_MSG(MSG_LEGACY_HIGH, str, a, b, c)
#define SLIM_MSG_ERROR0(str)            SLIM_MSG0(MSG_LEGACY_ERROR, str)
#define SLIM_MSG_ERROR1(str, a)         SLIM_MSG1(MSG_LEGACY_ERROR, str, a)
#define SLIM_MSG_ERROR2(str, a, b)      SLIM_MSG2(MSG_LEGACY_ERROR, str, a, b)
#define SLIM_MSG_ERROR(str, a, b, c)    SLIM_MSG(MSG_LEGACY_ERROR, str, a, b, c)

/*!
@brief Maximum SLIM client count

SLIM has a hard limit of number of supported clients. The total number shall
be maximum of simultaneously opened client connections including:
- local connections (SAML, SAP)
- remote connections through QMI-SLIM (SLIM-AP)
*/
#define SLIM_CLIENT_MAX_COUNT SLIM_CLIENT_LAST

/* Define task thread id for IPC creation and sending.*/
#if defined FEATURE_CGPS_LOC_MIDDLEWARE
#define SLIM_TASK_THREAD_ID THREAD_ID_LOC_MIDDLEWARE
#elif defined FEATURE_CGPS_LBS_TASK
#define SLIM_TASK_THREAD_ID THREAD_ID_LBS
#else /* !FEATURE_CGPS_LOC_MIDDLEWARE */
#define SLIM_TASK_THREAD_ID THREAD_ID_ERROR
#endif /* !FEATURE_CGPS_LOC_MIDDLEWARE */
/**
@brief Convert pointer to DIAG argument

This macro handles proper handle conversion from pointer type to uint32. It is
used for logging.

@param[in] pHandle Client handle.
*/
#define SLIM_HANDLE_L(pHandle)  ((uint32)(uintptr_t)(pHandle))

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/**
@brief SLIM mutex in modem

SLIM requires locking in case when multiple tasks or threads are used. The lock
type must support recursion.
*/
typedef os_MutexBlockType slim_MutexStructType;

/**
@brief Enum for SLIM clients in modem.

The enumeration has no mandatory item, however #SLIM_CLIENT_LAST is used for
#SLIM_PROVIDER_COUNT definition.

@sa SLIM_PROVIDER_COUNT
*/
typedef enum
{
  SLIM_CLIENT_SAP  = 1,         /**< Client id for SAP */
  SLIM_CLIENT_GEOFENCING,       /**< Client id for Geofencing */
  SLIM_CLIENT_SAMLITE,          /**< Client id for SAMLite */
  SLIM_CLIENT_QMISLIM_AP,       /**< AP connections from QMI-SLIM */
  SLIM_CLIENT_LAST = SLIM_CLIENT_QMISLIM_AP, /**< Last client handle enum
                                                value in use */
  //! @cond
  SLIM_CLIENT_MAX  = 2147483647 /* To force a 32 bit signed enum.
                                   Do not change or use */
  //! @endcond
} slim_ClientEnumType;

/**
@brief Enum for SLIM Service Providers

Values should correspond to array indexes in providers array in provider
registry.

The first provider must have a value of 0, and the following elements are
mandatory:
- SLIM_PROVIDER_NONE -- Shall be equal to -1
- SLIM_PROVIDER_COUNT -- Shall be equal to total number of providers (last + 1)
*/
typedef enum
{
  SLIM_PROVIDER_NONE = -1, /**< Invalid */
  SLIM_PROVIDER_SSC = 0,   /**< SSC - Sensor core */
  SLIM_PROVIDER_SAMLITE,   /**< SAMLite */
  SLIM_PROVIDER_QMILOC,    /**< QMI_LOC via LocMW task */
  SLIM_PROVIDER_QMIPDS,    /**< QMI_PDS via LBS task */
  SLIM_PROVIDER_PE,        /**< PE */
  SLIM_PROVIDER_QMISLIM_APSS, /**< QMI-SLIM AP provider */
  SLIM_PROVIDER_COUNT,       /**< Required value for SLIM */
  //! @cond
  SLIM_PROVIDER_MAX     = 2147483647 /* To force a 32 bit signed enum.
                                        Do not change or use */
  //! @endcond
} slim_ProviderEnumType;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

//! @}

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SLIM_PROCESSOR_H */

#ifndef SLIM_QMISLIM_QMI_CLIENT_I_H
#define SLIM_QMISLIM_QMI_CLIENT_I_H

/**
  @file
  @brief QMI-SLIM QMI Client internal API
  @ingroup slim_QSQmiClient

  Copyright (c) 2014 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.

 */
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/qmislim/qmiclient/src/qmislim_qmi_client_i.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "qmislim_common_api.h"
#include "qmislim_qmi_client_api.h"

#include "sensor_location_interface_manager_v01.h"
#include "qmi_client.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/**
  @brief Default value for QMI request execution timeout

  @sa slim_QSQmiClientT
  @ingroup slim_QSQmiClient
 */
#define SLIM_QS_DEFAULT_TX_TIMEOUT_MS        ((uint64)1000)
/**
  @brief Default value for QMI reconnect attempt delay

  @sa slim_QSQmiClientT
  @ingroup slim_QSQmiClient
 */
#define SLIM_QS_DEFAULT_RECONNECT_TIMEOUT_MS ((uint64)3000)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
  @brief Type for QMI client release callback function

  @sa qmi_client_release_cb
  @ingroup slim_QSQmiClient
 */
typedef void slim_QSQmiClientReleaseCbFnT
(
  void                          *p_ReleaseCbData
  /**< [in] QMI client object */
);

/*----------------------------------------------------------------------------
 * Class Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

#endif /* SLIM_QMISLIM_QMI_CLIENT_I_H */


#ifndef SLIM_QMISLIM_QMI_SVC_I_H
#define SLIM_QMISLIM_QMI_SVC_I_H

/**
  @file
  @brief QMI-SLIM QMI Service internal API
  @ingroup slim_QSQmiSvc

  Copyright (c) 2014 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.

 */
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/qmislim/qmiservice/src/qmislim_qmi_service_i.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "qmislim_qmi_service_api.h"

#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "qmi_client.h"

#include "sensor_location_interface_manager_v01.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/**
  @brief Initializes QMI-SLIM service connection object.

  @retval TRUE On success.
  @retval FALSE On error.

  @ingroup slim_QSQmiSvc
 */
boolean slim_QSQmiSvcClt_Init
(
  slim_QSQmiSvcCltT     *pz_Object,
  /**< [in] Connection object */
  slim_QSQmiSvcT        *pz_Parent,
  /**< [in] Container service object */
  uint32                 q_Id,
  /**< [in] Connection identifier */
  qmi_client_handle      z_ClientHandle
  /**< [in] QMI connection handle */
);
/**
  @brief Destroys QMI-SLIM service connection object.

  @retval TRUE On success.
  @retval FALSE On error.

  @ingroup slim_QSQmiSvc
 */
boolean slim_QSQmiSvcClt_Deinit
(
  slim_QSQmiSvcCltT     *pz_Object
  /**< [in] Connection object */
);
/**
  @brief Processes event from QMI stack and forwards it to SLIM Core as
         appropriate.

  @retval TRUE On success.
  @retval FALSE On error.

  @ingroup slim_QSQmiSvc
 */
boolean slim_QSQmiSvcClt_ProcessReq
(
  slim_QSQmiSvcCltT     *pz_Object,
  /**< [in] Connection object */
  qmi_req_handle         z_ReqHandle,
  /**< [in] QMI request handle */
  uint32                 q_MsgId,
  /**< [in] QMI message identifier */
  const void            *p_Data,
  /**< [in] QMI message data */
  uint32                 q_DataLen
  /**< [in] QMI message size in bytes */
);
/**
  @brief Processes event from SLIM core or provider and forwards it to client.

  @retval TRUE On success.
  @retval FALSE On error.

  @ingroup slim_QSQmiSvc
 */
boolean slim_QSQmiSvcClt_HandleSlimEvent
(
  slim_QSQmiSvcCltT              *pz_Object,
  /**< [in] Connection object */
  const slimMessageHeaderStructT *pz_Hdr,
  /**< [in] SLIM event header */
  const slim_QSSlimMessageDataT  *pz_Data
  /**< [in] SLIM event payload if present */
);

#endif /* SLIM_QMISLIM_QMI_SVC_I_H */


#ifndef _QMI_CORE_ERROR_H_
#define _QMI_CORE_ERROR_H_
/*---------------------------------------------------------------------------
   QMI_CORE_ERROR.H
---------------------------------------------------------------------------*/
/*!
  @file
    qmi_core_error.h

  @brief
    This file contains the Error codes for the core server framework.

  ---------------------------------------------------------------------------
  Copyright (c) 2011 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
---------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$$

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 

===========================================================================*/
/* For each object 20 error codes are reserved, so that
when core server returns an error we can look at the error number
and figure out the error belongs to which object. */
#define ERROR_BAND 20

/* The qmi_csi_error ends at 8 and so we reserve
   error codes 8-19 for qcsi infrastructure */
#define ERROR_START 20

typedef enum qmi_core_error {
    /* Container error 20 - 39 */
    QMI_CORE_LIST_ERROR = ERROR_START,
    /* Task error 40 - 59*/
    QMI_CORE_TASK_ERROR = QMI_CORE_LIST_ERROR + ERROR_BAND,
    /* Dispatch error 60 - 79 */
    QMI_CORE_DISPATCH_ERROR = QMI_CORE_TASK_ERROR + ERROR_BAND,
    /* Indication error 80 - 99 */
    QMI_CORE_INDICATION_ERROR = QMI_CORE_DISPATCH_ERROR + ERROR_BAND,
    /* Core server error 100 - 119 */
    QMI_CORE_SERVER_ERROR = QMI_CORE_INDICATION_ERROR + ERROR_BAND
}qmi_core_error_type;


#endif 

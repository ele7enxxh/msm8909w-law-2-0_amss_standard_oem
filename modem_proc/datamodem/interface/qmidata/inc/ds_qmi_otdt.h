#ifndef DS_QMI_OTDT_H
#define DS_QMI_OTDT_H
/*===========================================================================

                    DS_QMI_OTDT.H

DESCRIPTION
  Header for DS QMI OTDT.
  This service allows to control On-target data test application.

EXTERNALIZED FUNCTIONS
  ds_qmi_otdt_send_ind()
    Sends indication of the test result back to the client.

Copyright (c) 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/inc/ds_qmi_otdt.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/02/12    aa     Created module
===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "qmi_core_server.h"
#include "on_target_data_test_v01.h"

/*===========================================================================

                                 DEFINITIONS

===========================================================================*/


/*===========================================================================
FUNCTION  ds_qmi_otdt_init
===========================================================================*/
/*!
@brief
  Initialization function for DS QMI OTDT.
*/
/*=========================================================================*/
qmi_core_server_error_type 
ds_qmi_otdt_init
(
  void
);

/*===========================================================================
FUNCTION  ds_qmi_otdt_send_ind
===========================================================================*/
/*!
@brief
  This function sends indication of the test result back to the client.

@param[in]  handle                Handle to the service, got in TEST_START
                                  message.
@param[in]  response              Test status to be sent to the client

@retval    QMI_CORE_SERVER_NO_ERR on Success
@retval    QMI_CORE_SERVER_...    Error, see error codes defined
in qmi_core_server.h
*/
/*=========================================================================*/
qmi_core_server_error_type
ds_qmi_otdt_send_ind
(
  void* handle,
  otdt_test_status_ind_msg_v01* response
);

#ifdef __cplusplus
}
#endif


#endif /* DS_QMI_OTDT_H */







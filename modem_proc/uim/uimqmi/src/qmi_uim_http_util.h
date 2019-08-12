#ifndef QMI_UIM_HTTP_UTIL_H
#define QMI_UIM_HTTP_UTIL_H

/*===========================================================================

                         Q M I _ U I M _ H T T P_ U T I L . H

DESCRIPTION

 This header file defines data types and functions used internally 
 by QMI UIM HTTP service

Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/src/qmi_uim_http_util.h#1 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/19/16    ar     Fix to support bigger profile (> 65535) download
05/27/16    ks     MACRO for memory freeing
04/13/16    ks     Initial Version of QMI UIM HTTP

===========================================================================*/

/*=============================================================================

                   I N C L U D E S

=============================================================================*/
#include "comdef.h"
#include "qmi_csi.h"
#include "qmi_sap.h"
#include "queue.h"
#include "qmi_uim_http.h"
/*=============================================================================

                   D E F I N E S

=============================================================================*/
#define QMI_UIM_HTTP_MAX_NUM_SERVICE      5

#define QMI_UIM_HTTP_FREE(ptr)                                     \
  if(NULL != ptr)                                              \
  {                                                            \
    modem_mem_free((void*)(ptr),MODEM_MEM_CLIENT_UIM);         \
    ptr = NULL;                                                \
  }

/*=============================================================================

      S T R U C T    A N D    C A L L B A C K   D A T A    T Y P E S

=============================================================================*/
/*===========================================================================
   STRUCTURE:      QMI_UIM_HTTP_CMD_TYPE
=============================================================================*/
typedef struct {
  q_link_type                                  link;
  qmi_uim_http_transaction_req_data_type       cmd_data;
}qmi_uim_http_cmd_type;

/*===========================================================================
   STRUCTURE:      QMI_UIM_HTTP_CURR_TRANSACTION_INFO_TYPE
=============================================================================*/
typedef struct
{
  boolean                                   service_in_use;
  uint8                                     transaction_id;
  qmi_uim_http_service_registry_id_type     service_id;
  qmi_uim_http_transaction_callback_type   *rsp_cb_ptr;
  qmi_uim_http_transaction_resp_data_type   rsp_data;
  uint32                                    received_bytes;
} qmi_uim_http_curr_transaction_info_type;

/*===========================================================================
   STRUCTURE:      QMI_UIM_HTTP_CLIENT_INFO_TYPE
=============================================================================*/
typedef struct
{
  boolean                                   in_use;
  qmi_client_handle                         client_handle;
  qmi_uim_http_curr_transaction_info_type   curr_transaction[QMI_UIM_HTTP_MAX_NUM_SERVICE];
} qmi_uim_http_client_info_type;

/*===========================================================================
   STRUCTURE:      QMI_UIM_HTTP_SERVICE_INFO_TYPE
=============================================================================*/
typedef struct
{
  boolean                                in_use;
  qmi_uim_http_service_registry_id_type  service_id;
} qmi_uim_http_service_info_type;

/*===========================================================================
   STRUCTURE:      QMI_UIM_HTTP_GLOBALS_TYPE
=============================================================================*/
typedef struct {
  qmi_csi_service_handle                   service_handle;
  qmi_sap_client_handle                    sap_handle;
  q_type                                   cmd_q;
  boolean                                  service_enable;
  /* There can be only a single client for this QMI UIM HTTP service */
  qmi_uim_http_client_info_type            client_registry;
  qmi_uim_http_service_info_type           service_registry[QMI_UIM_HTTP_MAX_NUM_SERVICE];
  rex_crit_sect_type                       crit_sect;
}qmi_uim_http_globals_type;

#endif /* QMI_UIM_HTTP_UTIL_H */
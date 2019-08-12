/***********************************************************************
 * fds_i.h
 *
 * Internal APIs for FDS
 * Copyright (C) 2013 QUALCOMM Technologies, Inc.
 *
 * Internal APIs requried by Flash Driver Service implemenation module.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fds/src/fds_i.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-08-07   mj    Type cast QMI_ERR_INVALID_HANDLE_V01 enum entry.
2013-12-12   rp    Allow disabling SCRUB feature at runtime.
2013-10-10   dks   Create. Moved fds/src/fds.h here.

===========================================================================*/

#ifndef __FDS_I_H__
#define __FDS_I_H__

#include "qmi_csi.h"
#include "flash_driver_service_v01.h"
#include "flash_driver_service_impl_v01.h"
#include "comdef.h"

/* Signal used by service to register the task with the framework to be
   informed of client requests. When the client does connect, disconnect,
   or process request the framework sets this signal on the service task. */
#define  FDS_HANDLE_EVENT 0x0001

/**************************************************************************//**
 * @par Name:
 *   fds_lock_init - Initialize the critical section for service module.
 *
 * @par Synopsis:
 *   @code
 *     #include "fds_i.h"
 *     fds_lock_init ();
 *   @endcode
 *
 *****************************************************************************/
void fds_lock_init (void);

/**************************************************************************//**
 * @par Name:
 *   fds_lock - Enter critical section.
 *
 * @par Synopsis:
 *   @code
 *     #include "fds_i.h"
 *     fds_lock ();
 *   @endcode
 *
 *****************************************************************************/
void fds_lock (void);

/**************************************************************************//**
 * @par Name:
 *   fds_unlock - Leave critical section.
 *
 * @par Synopsis:
 *   @code
 *     #include "fds_i.h"
 *     fds_unlock ();
 *   @endcode
 *
 *****************************************************************************/
void fds_unlock (void);


/**************************************************************************//**
 * @par Name:
 *   fds_init - Initialize the service module.
 *
 * @par Synopsis:
 *   @code
 *     #include "fds_i.h"
 *     fds_init ();
 *   @endcode
 *
 * @par Description:
 *   Initializes static objects and register callbacks with QCSI and QMUX.
 *   Initialized by the service task at startup.
 *
 *****************************************************************************/
void fds_init (void);


/**************************************************************************//**
 * @par Name:
 *  fds_handle_event - Handler called when service task receives the
 *   handle event signal registered with the QCSI framework at initialization.
 *
 * @par Synopsis:
 *   @code
 *     #include "fds_i.h"
 *     fds_handle_event ();
 *   @endcode
 *
 * @par Description:
 *   Calls the qmi_csi_handle_event function on the QCSI framework to get
 *   appropriate callback invoked based on client action.
 *
 *****************************************************************************/
void fds_handle_event (void);


/* User friendly mappings for IDL generated names to remove version dependency
   from names. */
#define fds_scrub_start_req_msg          fds_scrub_start_req_msg_v01
#define fds_scrub_start_resp_msg         fds_scrub_start_resp_msg_v01

#define fds_scrub_status                 fds_scrub_status_v01
#define FDS_SCRUB_STATUS_MIN_ENUM_VAL    FDS_SCRUB_STATUS_MIN_ENUM_VAL_V01
#define FDS_SCRUB_PENDING                FDS_SCRUB_PENDING_V01
#define FDS_SCRUB_COMPLETE               FDS_SCRUB_COMPLETE_V01
#define FDS_SCRUB_STATUS_MAX_ENUM_VAL    FDS_SCRUB_STATUS_MAX_ENUM_VAL_V01

#define fds_scrub_get_status_req_msg     fds_scrub_get_status_req_msg_v01
#define fds_scrub_get_status_resp_msg    fds_scrub_get_status_resp_msg_v01

#define fds_scrub_get_sys_info_req_msg   fds_scrub_get_sys_info_req_msg_v01
#define fds_scrub_get_sys_info_resp_msg  fds_scrub_get_sys_info_resp_msg_v01

#define fds_handle_fota_update_req_msg   fds_handle_fota_update_req_msg_v01
#define fds_handle_fota_update_resp_msg  fds_handle_fota_update_resp_msg_v01

#define fds_indication_register_req_msg  fds_indication_register_req_msg_v01
#define fds_indication_register_resp_msg fds_indication_register_resp_msg_v01

#define fds_scrub_apps_start_scrub_ind_msg \
                                         fds_scrub_apps_start_scrub_ind_msg_v01

#define FDS_GET_SUPPORTED_MSGS_REQ       QMI_FDS_GET_SUPPORTED_MSGS_REQ_V01
#define FDS_GET_SUPPORTED_MSGS_RESP      QMI_FDS_GET_SUPPORTED_MSGS_RESP_V01
#define FDS_GET_SUPPORTED_FIELDS_REQ     QMI_FDS_GET_SUPPORTED_FIELDS_REQ_V01
#define FDS_GET_SUPPORTED_FIELDS_RESP    QMI_FDS_GET_SUPPORTED_FIELDS_RESP_V01

#define FDS_SCRUB_START_REQ              QMI_FDS_SCRUB_START_REQ_V01
#define FDS_SCRUB_START_RESP             QMI_FDS_SCRUB_START_RESP_V01

#define FDS_SCRUB_GET_STATUS_REQ         QMI_FDS_SCRUB_GET_STATUS_REQ_V01
#define FDS_SCRUB_GET_STATUS_RESP        QMI_FDS_SCRUB_GET_STATUS_RESP_V01

#define FDS_SCRUB_GET_SYS_INFO_REQ       QMI_FDS_SCRUB_GET_SYS_INFO_REQ_V01
#define FDS_SCRUB_GET_SYS_INFO_RESP      QMI_FDS_SCRUB_GET_SYS_INFO_RESP_V01

#define FDS_HANDLE_FOTA_UPDATE_REQ       QMI_FDS_HANDLE_FOTA_UPDATE_REQ_V01
#define FDS_HANDLE_FOTA_UPDATE_RESP      QMI_FDS_HANDLE_FOTA_UPDATE_RESP_V01

#define FDS_INDICATION_REGISTER_REQ      QMI_FDS_INDICATION_REGISTER_REQ_V01
#define FDS_INDICATION_REGISTER_RESP     QMI_FDS_INDICATION_REGISTER_RESP_V01

#define FDS_SCRUB_APPS_START_SCRUB_IND   QMI_FDS_SCRUB_APPS_START_SCRUB_IND_V01


#define fds_get_service_object()         fds_get_service_object_v01()
#define fds_get_service_impl()           fds_get_service_impl_v01()

#define FDS_ERR_NONE                     QMI_ERR_NONE_V01
#define FDS_ERR_MALFORMED_MSG            QMI_ERR_MALFORMED_MSG_V01
#define FDS_ERR_NO_PERMISSION            QMI_ERR_NO_PERMISSION_V01
#define FDS_ERR_TOO_SOON                 QMI_ERR_TOO_SOON_V01
#define FDS_ERR_TIME_NOT_ACQUIRED        QMI_ERR_TIME_NOT_ACQUIRED_V01
#define FDS_ERR_OP_IN_PROGRESS           QMI_ERR_OP_IN_PROGRESS_V01
#define FDS_ERR_INVALID_HANDLE           (fds_scrub_status_v01) \
                                         QMI_ERR_INVALID_HANDLE_V01
#define FDS_ERR_INTERNAL                 QMI_ERR_INTERNAL_V01
#define FDS_RESULT_FAILURE               QMI_RESULT_FAILURE_V01
#define FDS_RESULT_SUCCESS               QMI_RESULT_SUCCESS_V01
#define FDS_ERR_SCRUB_DISABLED           QMI_ERR_DISABLED_V01

#endif /* not __FDS_I_H__ */

/***********************************************************************
 * fs_mfse_std_interface.h
 *
 * Maps modem_filesystem_external_v01.h structure names into EFS compatible
 * names.
 * Copyright (C) 2011, 2013 Qualcomm Technologies, Inc.
 *
 * The modem_filesystem_external_v01.h is a header file containing structures
 * and APIs generated from its IDL file modem_filesystem_external_v01.idl.
 * The IDL file is maintained by the MPSS API Change Control Board (CCB) and
 * follows naming conventions that are standard for all IDLs. This does not
 * match the naming conventions we have in the rest of the files in EFS. Hence
 * typedef all the IDL names to EFS standard conventions.
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fs_mfse/src/fs_mfse_std_interface.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-04-03   dks   Create.

===========================================================================*/

#ifndef __FS_MFSE_STD_INTERFACE_H__
#define __FS_MFSE_STD_INTERFACE_H__

#include "fs_config_i.h"

#include "modem_filesystem_external_v01.h"
#include "modem_filesystem_external_impl_v01.h"

#define fs_mfse_errno_type mfse_errno_type_v01

#define FS_MFSE_ERRNO_TYPE_MIN_ENUM_VAL MFSE_ERRNO_TYPE_MIN_ENUM_VAL_V01
#define FS_MFSE_ENOERR                  MFSE_ENOERR_V01
#define FS_MFSE_EPERM                   MFSE_EPERM_V01
#define FS_MFSE_ENOENT                  MFSE_ENOENT_V01
#define FS_MFSE_EEXIST                  MFSE_EEXIST_V01
#define FS_MFSE_EBADF                   MFSE_EBADF_V01
#define FS_MFSE_ENOMEM                  MFSE_ENOMEM_V01
#define FS_MFSE_EACCES                  MFSE_EACCES_V01
#define FS_MFSE_EBUSY                   MFSE_EBUSY_V01
#define FS_MFSE_EXDEV                   MFSE_EXDEV_V01
#define FS_MFSE_ENODEV                  MFSE_ENODEV_V01
#define FS_MFSE_ENOTDIR                 MFSE_ENOTDIR_V01
#define FS_MFSE_EISDIR                  MFSE_EISDIR_V01
#define FS_MFSE_EINVAL                  MFSE_EINVAL_V01
#define FS_MFSE_EMFILE                  MFSE_EMFILE_V01
#define FS_MFSE_ETXTBSY                 MFSE_ETXTBSY_V01
#define FS_MFSE_ENOSPC                  MFSE_ENOSPC_V01
#define FS_MFSE_ESPIPE                  MFSE_ESPIPE_V01
#define FS_MFSE_ENAMETOOLONG            MFSE_ENAMETOOLONG_V01
#define FS_MFSE_ENOTEMPTY               MFSE_ENOTEMPTY_V01
#define FS_MFSE_ELOOP                   MFSE_ELOOP_V01
#define FS_MFSE_EILSEQ                  MFSE_EILSEQ_V01
#define FS_MFSE_ESTALE                  MFSE_ESTALE_V01
#define FS_MFSE_EDQUOT                  MFSE_EDQUOT_V01
#define FS_MFSE_ENOCARD                 MFSE_ENOCARD_V01
#define FS_MFSE_EBADFMT                 MFSE_EBADFMT_V01
#define FS_MFSE_ENOTITM                 MFSE_ENOTITM_V01
#define FS_MFSE_EROLLBACK               MFSE_EROLLBACK_V01
#define FS_MFSE_FS_ERANGE               MFSE_FS_ERANGE_V01
#define FS_MFSE_EEOF                    MFSE_EEOF_V01
#define FS_MFSE_EUNKNOWN_SFAT           MFSE_EUNKNOWN_SFAT_V01
#define FS_MFSE_EUNKNOWN_HFAT           MFSE_EUNKNOWN_HFAT_V01
#define FS_MFSE_ENOTHINGTOSYNC          MFSE_ENOTHINGTOSYNC_V01
#define FS_MFSE_ERRNO_TYPE_MAX_ENUM_VAL MFSE_ERRNO_TYPE_MAX_ENUM_VAL_V01

#define fs_mfse_filesystem_id              mfse_filesystem_id_v01
#define FS_MFSE_FILESYSTEM_ID_MIN_ENUM_VAL MFSE_FILESYSTEM_ID_MIN_ENUM_VAL_V01
#define FS_MFSE_EFS2                       MFSE_EFS2_V01
#define FS_MFSE_FILESYSTEM_ID_MAX_ENUM_VAL MFSE_FILESYSTEM_ID_MAX_ENUM_VAL_V01

#define fs_mfse_sync_status                mfse_sync_status_v01
#define FS_MFSE_SYNC_STATUS_MIN_ENUM_VAL   MFSE_SYNC_STATUS_MIN_ENUM_VAL_V01
#define FS_MFSE_SYNC_PENDING               MFSE_SYNC_PENDING_V01
#define FS_MFSE_SYNC_COMPLETE              MFSE_SYNC_COMPLETE_V01
#define FS_MFSE_SYNC_STATUS_MAX_ENUM_VAL   MFSE_SYNC_STATUS_MAX_ENUM_VAL_V01


#define fs_mfse_sync_no_wait_req_msg    mfse_sync_no_wait_req_msg_v01
#define fs_mfse_sync_no_wait_resp_msg   mfse_sync_no_wait_resp_msg_v01

#define fs_mfse_sync_get_status_req_msg       \
        mfse_sync_get_status_req_msg_v01
#define fs_mfse_sync_get_status_resp_msg      \
        mfse_sync_get_status_resp_msg_v01

#define FS_MFSE_GET_SUPPORTED_MSGS_REQ        \
        QMI_MFSE_GET_SUPPORTED_MSGS_REQ_V01
#define FS_MFSE_GET_SUPPORTED_MSGS_RESP       \
        QMI_MFSE_GET_SUPPORTED_MSGS_RESP_V01
#define FS_MFSE_GET_SUPPORTED_FIELDS_REQ      \
        QMI_MFSE_GET_SUPPORTED_FIELDS_REQ_V01
#define FS_MFSE_GET_SUPPORTED_FIELDS_RESP     \
        QMI_MFSE_GET_SUPPORTED_FIELDS_RESP_V01
#define FS_MFSE_SYNC_NO_WAIT_REQ      QMI_MFSE_SYNC_NO_WAIT_REQ_V01
#define FS_MFSE_SYNC_NO_WAIT_RESP     QMI_MFSE_SYNC_NO_WAIT_RESP_V01
#define FS_MFSE_SYNC_GET_STATUS_REQ   QMI_MFSE_SYNC_GET_STATUS_REQ_V01
#define FS_MFSE_SYNC_GET_STATUS_RESP  QMI_MFSE_SYNC_GET_STATUS_RESP_V01

#define fs_mfse_get_service_object()   mfse_get_service_object_v01()
#define fs_mfse_get_service_impl()     mfse_get_service_impl_v01()

#endif /* __FS_MFSE_STD_INTERFACE_H__ */

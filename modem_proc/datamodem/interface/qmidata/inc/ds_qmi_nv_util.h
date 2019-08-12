#ifndef QMI_NV_UTIL_H
#define QMI_NV_UTIL_H
/*==============================================================================

        Q M I  -   N O N - V O L A T I L E   S T O R A G E

GENERAL DESCRIPTION
  Simple APIs to access QMI-specific NV settings.  Supports both new-style
  QMI specific NV items and old-style NV items, though it is recommended that
  both exisiting and new code accessing old-style items use the legacy NV API
  when possible.

EXTERNALIZED FUNCTIONS
  qmi_nv_read
  qmi_nv_write
  qmi_nv_is_new_indexed
  qmi_nv_file_read
  qmi_nv_file_write

INITIALIZATION AND SEQUENCING REQUIREMENTS
  EFS must be initialized

Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/inc/ds_qmi_nv_util.h#1 $
$DateTime: 2016/12/13 08:00:03 $ 
$Author: mplcsds1 $
$Change: 11985188 $

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
12/09/10 mj   Ported module to QMI

==============================================================================*/

#include "fs_public.h"
#include "nv_items.h"

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Constants and Macros
==============================================================================*/

/* Maximum size of an NV item - applications with requirements for more
 * should use NV file APIs */
#define QMI_NV_ITEM_SIZE_MAX (200)

/*==============================================================================
  Typedefs
==============================================================================*/

/* Enum for new-style QMI NV items (used in API calls) */
typedef enum qmi_nv_item_e {
  QMI_NVI_ITEM_START = NV_MAX_I, /* reserved for internal use only */

  /**************************************************************************\
  **             ---- Instructions for adding new NV items ----             **
  **  1. Add new QMI_NV_ITEM_* enum value at the end of this list           **
  **  2. Add a comment giving the QXDM ID (or 'pending' if none yet) and    **
  **     data type of the item                                              **
  **  3. Update any existing comments with 'pending' as the QXDM ID         **
  **  4. Add the filename of the new NV item to the qmi_nv_filenames list   **
  **     located in modem\datacommon\qmi\src\ds_qmi_nv_util.c               **
  **  5. If the new item is to be indexed (i.e. an array of NV items), then **
  **     update the table qmi_nv_indexed_items in ds_qmi_nv_util.c          **
  **  6. Add the directory and filename on a new line at the end of the     **
  **     file located  at tools\uqcn_tool\efs_files\qmi.conf                **
  **  7. Send the new NV item info to Tools so it can be added to QXDM      **
  **                                                                        **
  **  A note about indexed/arrayed items: their data structure MUST begin   **
  **  with a uint8 field used to store the index.  This is to maintain      **
  **  consistency with the legacy NV API.                                   **
  \**************************************************************************/

  /* < NV_MAX_I + 1>                        <QXDM ID> |<data type> */
  QMI_NV_ITEM_DEFAULT_NAI,                 /* pending | char[] */
  QMI_NV_ITEM_LAST_OTASP_STATE,            /* pending | boolean */
  QMI_NV_ITEM_USE_OTASP_ACT_STATE,         /* pending | boolean */
  QMI_NV_ITEM_ALLOW_INVALID_SPC,           /* pending | boolean */
  QMI_NV_ITEM_USE_MAX_DEFINED_DATA_RATES,  /* pending | boolean */

  /* < NV_MAX_I + 6 >                       <QXDM ID> |<data type> */
  QMI_NV_ITEM_RESTRICT_IMSI_ACCESS,        /* pending | boolean */
  QMI_NV_ITEM_MANUAL_DNS_IPV4_PRI,         /* pending | qmi_nv_dns_addr_type */
  QMI_NV_ITEM_MANUAL_DNS_IPV4_SEC,         /* pending | qmi_nv_dns_addr_type */
  QMI_NV_ITEM_AUTOCONNECT_PAUSE_SETTING,   /* pending | qmi_nv_autoconnect_pause_type */
  QMI_NV_ITEM_AUTOCONNECT_ROAM_SETTING,    /* pending | qmi_nv_autoconnect_roam_setting_type */

  /* < NV_MAX_I + 11 >                      <QXDM ID> |<data type> */
  QMI_NV_ITEM_LAST_RMNET_MIP_ERR_CODE,     /* pending | uint8 */
  QMI_NV_ITEM_RESTRICT_AUTOCONNECT,        /* pending | boolean */
  QMI_NV_ITEM_SYNC_RESET_MODEM_STATE,      /* pending | boolean */
  QMI_NV_ITEM_MANUAL_DNS_IPV6_PRI,         /* pending | qmi_nv_ipv6_dns_addr_type */
  QMI_NV_ITEM_MANUAL_DNS_IPV6_SEC,         /* pending | qmi_nv_ipv6_dns_addr_type */
  
  /* < NV_MAX_I + 16 >                      <QXDM ID> |<data type> */
  QMI_NV_ITEM_RMNET_GLOBAL_CONFIG,         /* pending | rmnet_efs_global_config_s_type */
  QMI_NV_ITEM_QMAP_CMD_RESP_TIME,          /* pending | uint32 */
  QMI_NV_ITEM_RMNET_AUTOCONNECT,           /* pending | rmnet_efs_autoconnect_s_type */
  
  /* Add new items here */

  /* below items reserved for internal use only */
  QMI_NVI_ITEM_END, 
  QMI_NVI_ITEM_BIG = 0x7FFFFFFF
} qmi_nv_item_e_type;

/* Enum for new-style QMI NV items (used in API calls) */
typedef enum qmi_nv_file_e {
  QMI_NVI_FILE_START  =  0, /* reserved for internal use only */
  /* <1>                                               <data type> */
  QMI_NV_FILE_USER_DATA,                             /* uint8[] */
  QMI_NV_FILE_ERI,                                   /* uint8[] */
  QMI_NV_FILE_ROAM_IND_LIST,                         /* uint8[] */

  /* Add new items here */

  /* below items reserved for internal use only */
  QMI_NVI_FILE_END, 
  QMI_NVI_FILE_BIG = 0x7FFFFFFF
} qmi_nv_file_e_type;

/* 
** Status enum returned by qmi_nv_read & qmi_nv_write. Applications wishing to
** compare against NV status enum settings can use a uint32 instead of
** qmi_nv_status_e_type, for example:
**   uint32 retval = qmi_nv_write(...);
**   if (retval == NV_DONE_S) ...
** This behavior is supported but not recommended for new code.
*/
typedef enum qmi_nv_status_e {

  /* The values of the following enums are equal in value and (mostly) meaning
   * of the listed nv_stat_enum_type */

  QMI_NV_STATUS_OK = 0,
    /* Request completed successfully.  Equivalent to NV_DONE_S */

  QMI_NV_STATUS_EFS_ERR = 4,
    /* Internal EFS error.  Equivalent to NV_FAIL_S */

  QMI_NV_STATUS_NOT_ACTIVE = 5,
    /* NV item file doesn't exist.  Equivalent to NV_NOTACTIVE_S */

  QMI_NV_STATUS_BAD_ITEM = 6,
    /* NV item number not in allowed range: must be < NV_MAX_I for old items, 
     * or > QMI_NVI_ITEM_START and < QMI_NVI_ITEM_END for new.
     * Somewhat equivalent to NV_BADPARM_S */

  /* The following enums are not included in nv_stat_enum_type */

  QMI_NV_STATUS_BAD_SIZE = 128,
    /* On read: data size doesn't match current item size
     * On write: data size is > QMI_NV_ITEM_SIZE_MAX
     * Not equivalent to any nv_stat_enum_type value */

  QMI_NV_STATUS_NULL_PTR = 129,
    /* NULL passed as data pointer field to write or read function */

  QMI_NV_STATUS_PATH_TOO_LONG = 130,
    /* specified item path too large */

  QMI_NVI_STATUS_BIG = 0x7FFFFFFF 
    /* Reserved (force enum to 32-bit width) */
} qmi_nv_status_e_type;

/*==============================================================================
  Variables
==============================================================================*/

/*==============================================================================
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*==========================================================================

  FUNCTION qmi_nv_read

  DESCRIPTION
    This function reads an NV item from EFS.

    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    item      [in] NV item to read: can be nv_items_enum_type or 
                   qmi_nv_item_e_type
    index     [in] index of the NV item to read, or 0 if no index
                   (if unsure, use 0)
    data_ptr [out] location to store NV item contents
    data_size [in] size of buffer at data_ptr
    
  RETURN VALUE
    QMI_NV_STATUS_OK on success
    QMI_NV_STATUS_BAD_ITEM if item is not in old NV or new QMI NV range
    QMI_NV_STATUS_NOT_ACTIVE if item doesn't exist
    QMI_NV_STATUS_BAD_SIZE if item size doesn't match data_size
    QMI_NV_STATUS_EFS_ERR on EFS error
    QMI_NV_STATUS_NULL_PTR if data_ptr is null

  SIDE EFFECTS
    Waits on fs_global_lock() - see description & dependencies

===========================================================================*/
qmi_nv_status_e_type qmi_nv_read(
  qmi_nv_item_e_type item,
  uint32             ep_id,
  uint8              mux_id,
  void *             data_ptr,
  fs_size_t          data_size
  );

/*==========================================================================

  FUNCTION qmi_nv_read_ex

  DESCRIPTION
    This function reads an NV item from EFS.

    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    item      [in] NV item to read: can be nv_items_enum_type or 
                   qmi_nv_item_e_type
    index     [in] index of the NV item to read, or 0 if no index
                   (if unsure, use 0)
    data_ptr [out] location to store NV item contents
    data_size [in] size of buffer at data_ptr
    ps_subs_id[in] ps_subscription ID
    
  RETURN VALUE
    QMI_NV_STATUS_OK on success
    QMI_NV_STATUS_BAD_ITEM if item is not in old NV or new QMI NV range
    QMI_NV_STATUS_NOT_ACTIVE if item doesn't exist
    QMI_NV_STATUS_BAD_SIZE if item size doesn't match data_size
    QMI_NV_STATUS_EFS_ERR on EFS error
    QMI_NV_STATUS_NULL_PTR if data_ptr is null

  SIDE EFFECTS
    Waits on fs_global_lock() - see description & dependencies

===========================================================================*/
qmi_nv_status_e_type qmi_nv_read_ex(
  qmi_nv_item_e_type item,
  uint32             ep_id,
  uint8              mux_id,
  void *             data_ptr,
  fs_size_t          data_size,
  uint32             ps_subs_id
  );

/*==========================================================================

  FUNCTION qmi_nv_write

  DESCRIPTION
    This function writes an NV item to EFS.  Enforces a maximum data_size of
    QMI_NV_ITEM_SIZE_MAX.  Applications wishing to save larger files should use
    EFS regular file APIs directly.

    If the NV item exists and has a different file size than data_size,
    it will be deleted and recreated with length equal to data_size.

    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    item      [in] NV item to write: can be nv_items_enum_type or 
                   qmi_nv_item_e_type
    index     [in] index of the NV item to write, or 0 if no index
    data_ptr  [in] location to store NV item contents
    data_size [in] number of bytes to write, 
                   must be <= QMI_NV_ITEM_SIZE_MAX
    
  RETURN VALUE
    QMI_NV_STATUS_OK on success
    QMI_NV_STATUS_BAD_ITEM if item is not in old NV or new QMI NV range
    QMI_NV_STATUS_BAD_SIZE if data_size > QMI_NV_ITEM_SIZE_MAX
    QMI_NV_STATUS_EFS_ERR on EFS error
    QMI_NV_STATUS_NULL_PTR if data_ptr is null

  SIDE EFFECTS
    Waits on fs_global_lock() - see description & dependencies

===========================================================================*/
qmi_nv_status_e_type qmi_nv_write(
  qmi_nv_item_e_type item,
  uint32             ep_id,
  uint8              mux_id,
  void *             data_ptr,
  fs_size_t          data_size
  );

/*==========================================================================

  FUNCTION qmi_nv_write_ex

  DESCRIPTION
    This function writes an NV item to EFS.  Enforces a maximum data_size of
    QMI_NV_ITEM_SIZE_MAX.  Applications wishing to save larger files should use
    EFS regular file APIs directly.

    If the NV item exists and has a different file size than data_size,
    it will be deleted and recreated with length equal to data_size.

    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    item      [in] NV item to write: can be nv_items_enum_type or 
                   qmi_nv_item_e_type
    index     [in] index of the NV item to write, or 0 if no index
    data_ptr  [in] location to store NV item contents
    data_size [in] number of bytes to write, 
                   must be <= QMI_NV_ITEM_SIZE_MAX
    ps_subs_id [in] subscription id on which NV should be read.
    
  RETURN VALUE
    QMI_NV_STATUS_OK on success
    QMI_NV_STATUS_BAD_ITEM if item is not in old NV or new QMI NV range
    QMI_NV_STATUS_BAD_SIZE if data_size > QMI_NV_ITEM_SIZE_MAX
    QMI_NV_STATUS_EFS_ERR on EFS error
    QMI_NV_STATUS_NULL_PTR if data_ptr is null

  SIDE EFFECTS
    Waits on fs_global_lock() - see description & dependencies

===========================================================================*/
qmi_nv_status_e_type qmi_nv_write_ex(
  qmi_nv_item_e_type item,
  uint32             ep_id,
  uint8              mux_id,
  void *             data_ptr,
  fs_size_t          data_size,
  uint32             ps_subs_id
  );


/*==========================================================================

  FUNCTION qmi_nv_is_indexed

  DESCRIPTION
    Returns whether the QMI NV item is indexed (i.e. is an array of NV items).

  DEPENDENCIES
    None.

  PARAMETERS
    item  [in] NV item to check: can be nv_items_enum_type or qmi_nv_item_e_type
    
  RETURN VALUE
    TRUE if item is new-style AND it is indexed, otherwise FALSE

  SIDE EFFECTS
    None.

===========================================================================*/
boolean qmi_nv_is_indexed(
  qmi_nv_item_e_type item
  );

/*==========================================================================

  FUNCTION qmi_nv_file_read

  DESCRIPTION
    This function reads from a regular file in EFS. Up to data_size bytes
    will be read and placed in data_ptr. On successful return status,
    file_size will be set to the size of the entire file. This value can
    be greater than data_size, indicating that not all of the file was
    read.

    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    file      [in]  NV file to read: qmi_nv_file_e_type
    data_ptr  [out] location to store file contents
    data_size [in]  size of buffer at data_ptr
    file_size [out] size of the entire file
    
  RETURN VALUE
    QMI_NV_STATUS_OK on success
    QMI_NV_STATUS_BAD_ITEM if file is not in QMI NV file range
    QMI_NV_STATUS_NOT_ACTIVE if file doesn't exist
    QMI_NV_STATUS_EFS_ERR on EFS error
    QMI_NV_STATUS_NULL_PTR if data_ptr or file_size are null

  SIDE EFFECTS
    Waits on fs_global_lock() - see description & dependencies

===========================================================================*/
qmi_nv_status_e_type qmi_nv_file_read(
  qmi_nv_file_e_type item,
  void *             data_ptr,
  fs_size_t          data_size,
  fs_size_t *        file_size
  );

/*==========================================================================

  FUNCTION qmi_nv_file_read_ex

  DESCRIPTION
    This function reads from a regular file in EFS. Up to data_size bytes
    will be read and placed in data_ptr. On successful return status,
    file_size will be set to the size of the entire file. This value can
    be greater than data_size, indicating that not all of the file was
    read.

    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    file      [in]  NV file to read: qmi_nv_file_e_type
    data_ptr  [out] location to store file contents
    data_size [in]  size of buffer at data_ptr
    file_size [out] size of the entire file
    ps_subs_id [in] subscription id on which the NV file is to be read.
    
  RETURN VALUE
    QMI_NV_STATUS_OK on success
    QMI_NV_STATUS_BAD_ITEM if file is not in QMI NV file range
    QMI_NV_STATUS_NOT_ACTIVE if file doesn't exist
    QMI_NV_STATUS_EFS_ERR on EFS error
    QMI_NV_STATUS_NULL_PTR if data_ptr or file_size are null

  SIDE EFFECTS
    Waits on fs_global_lock() - see description & dependencies

===========================================================================*/
qmi_nv_status_e_type qmi_nv_file_read_ex(
  qmi_nv_file_e_type item,
  void *             data_ptr,
  fs_size_t          data_size,
  fs_size_t *        file_size,
  uint32             ps_subs_id
  );

/*==========================================================================

  FUNCTION qmi_nv_file_write

  DESCRIPTION
    This function writes an NV file to EFS.  Enforces a maximum data_size
    of QMI_NV_FILE_SIZE_MAX.  Applications wishing to save larger files 
    should use EFS regular file APIs directly.

    If the NV file exists and has a different file size than data_size,
    it will be deleted and recreated with length equal to data_size.

    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    item      [in] NV file to write: can be qmi_nv_file_e_type
    data_ptr  [in] location to store NV item contents
    data_size [in] number of bytes to write
    
  RETURN VALUE
    QMI_NV_STATUS_OK on success
    QMI_NV_STATUS_BAD_ITEM if file is not in QMI NV file range
    QMI_NV_STATUS_BAD_SIZE if data_size > EFS supported value
    QMI_NV_STATUS_EFS_ERR on EFS error
    QMI_NV_STATUS_NULL_PTR if data_ptr is null

  SIDE EFFECTS
    Waits on fs_global_lock() - see description & dependencies

===========================================================================*/
qmi_nv_status_e_type qmi_nv_file_write(
  qmi_nv_file_e_type file,
  void *             data_ptr,
  fs_size_t          data_size
  );

/*==========================================================================

  FUNCTION qmi_nv_file_write_ex

  DESCRIPTION
    This function writes an NV file to EFS.  Enforces a maximum data_size
    of QMI_NV_FILE_SIZE_MAX.  Applications wishing to save larger files 
    should use EFS regular file APIs directly.

    If the NV file exists and has a different file size than data_size,
    it will be deleted and recreated with length equal to data_size.

    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    item      [in] NV file to write: can be qmi_nv_file_e_type
    data_ptr  [in] location to store NV item contents
    data_size [in] number of bytes to write
    ps_subs_id [in] ps subscription id
    
  RETURN VALUE
    QMI_NV_STATUS_OK on success
    QMI_NV_STATUS_BAD_ITEM if file is not in QMI NV file range
    QMI_NV_STATUS_BAD_SIZE if data_size > EFS supported value
    QMI_NV_STATUS_EFS_ERR on EFS error
    QMI_NV_STATUS_NULL_PTR if data_ptr is null

  SIDE EFFECTS
    Waits on fs_global_lock() - see description & dependencies

===========================================================================*/
qmi_nv_status_e_type qmi_nv_file_write_ex(
  qmi_nv_file_e_type file,
  void *             data_ptr,
  fs_size_t          data_size,
  uint32             ps_subs_id
  );

/*==========================================================================

  FUNCTION qmi_nv_init_efs_conf_file

  DESCRIPTION
    This function initializes EFS NV conf file. This is needed for QXDM
    and QPST NV/EFS utility to properly display and backup the NV value.

    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    None
    
  RETURN VALUE
    QMI_NV_STATUS_OK on success
    QMI_NV_STATUS_EFS_ERR on EFS error

  SIDE EFFECTS
    Waits on fs_global_lock() - see description & dependencies

===========================================================================*/
qmi_nv_status_e_type qmi_nv_init_efs_conf_file
(
  void
);

#endif /* QMI_NV_UTIL_H */


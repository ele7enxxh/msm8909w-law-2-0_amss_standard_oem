/***********************************************************************
 * fs_qmi_service.h
 *
 * Header file for the Remote efs_get efs_put service Module functions.
 * Copyright (C) 2011-2012 QUALCOMM Technologies, Inc.
 *
 * Header file for service module that processes efs_put and efs_get requests
 * from the client module running on the remote processor. The framework
 * invokes appropriate callback registered at init based on client's actions.
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs_qmi_put_get/service/src/fs_qmi_service.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-03-23   dks   Add remote processor access check for paths.
2011-10-04   dks   Create.

===========================================================================*/

#ifndef __FS_QMI_SERVICE_H__
#define __FS_QMI_SERVICE_H__

#include "fs_config_i.h"

#ifdef FEATURE_EFS_QMI_PUT_GET_SERVICE

#include "qmi_csi.h"

/* Signal used by service to register the service task with the framework to
   be informed of client requests. When the client does connect, disconnect,
   or process request the framework sets this signal on the service task. */
#define  FS_QMI_EFS_SERVICE_HANDLE_EVENT 0x0001

/**************************************************************************//**
 * @par Name:
 *   fs_qmi_service_init - Initialize the efs_qmi_put_get service module.
 *
 * @par Synopsis:
 *   @code
 *     #include "fs_qmi_service.h"
 *     int fs_qmi_service_init ();
 *   @endcode
 *
 * @par Description:
 *   Initializes static objects and registers callbacks with the QCSI.
 *   Initialized by the service task at startup.
 *
 * @return
 *   Returns 0 on success and -1 on error.
 *
******************************************************************************/
int fs_qmi_service_init (void);


/**************************************************************************//**
 * @par Name:
 *  fs_qmi_service_handle_event - Handler called when service task receives the
 *   handle event signal registered with the QCSI framework at initialization.
 *
 * @par Synopsis:
 *   @code
 *     #include "fs_qmi_service.h"
 *     int fs_qmi_service_handle_event ();
 *   @endcode
 *
 * @par Description:
 *   Calls the handle_event function on the framework to get appropriate
 *   callback invoked based on client action.
 *
 * @return
 *   Returns 0 on success and -1 on error.
 *
******************************************************************************/
int fs_qmi_service_handle_event (void);


/**************************************************************************//**
 * @par Name:
 *   fs_qmi_service_close - Closes connection with the QCSI framwork.
 *
 * @par Synopsis:
 *   @code
 *     #include "fs_qmi_service.h"
 *     int fs_qmi_service_close ();
 *   @endcode
 *
 * @par Description:
 *   Releases any handle provided by the framework and unregisters all signals
 *   and callbacks registered at initialization. No more events from the
 *   clients will invoke the service.
 *
 * @return
 *   Returns 0 on success and -1 on error.
******************************************************************************/
int fs_qmi_service_close (void);


/**************************************************************************//**
 * @par Name:
 *   fs_qmi_service_access_check_init - Initialize remote path access
 *    check module.
 *
 * @par Synopsis:
 *   @code
 *     #include "fs_qmi_service.h"
 *     fs_qmi_service_access_check_init ();
 *   @endcode
 *
 * @par Description:
 *   Initializes the access check module used to check if a remote processor
 *   has access to the requested path.
 *
 *****************************************************************************/
void fs_qmi_service_access_check_init (void);


/**************************************************************************//**
 * @par Name:
 *   fs_qmi_service_access_check - Check if remote processor is allowed
 *      access to the specified modem filesystem path.
 *
 * @par Synopsis:
 *   @code
 *     #include "fs_qmi_service.h"
 *     char *path = "/nv/num/1017";
 *     int result = fs_qmi_service_access_check (path);
 *   @endcode
 *
 * @par Description:
 *   Checks the given path against a list of momde filesystem paths which are
 *   allowed to be accessed from the remote processor.
 *
 * @param path
 *   Path to which access was requested.
 *
 * @return
 *   Returns 1 if ok to access and 0 otherwise.
******************************************************************************/
int fs_qmi_service_access_check (const char *path);


/**************************************************************************//**
 * @par Name:
 *   fs_qmi_service_access_get_list - Returns pointer to the whitelist table.
 *
 * @par Synopsis:
 *   @code
 *     #include "fs_qmi_service.h"
 *     struct fs_qmi_service_access_check_table_type* whitelist_tbl;
 *     whitelist_tbl = fs_qmi_service_access_get_list ();
 *   @endcode
 *
 * @par Description:
 *   Return pointer to the whitelist table to be able to update the
 *    list from tests.
 *
 * @return
 *   Pointer to the whitelist table.
******************************************************************************/
struct fs_qmi_service_access_check_table_type*
fs_qmi_service_access_get_list (void);

/* One single entry in the access list containing the allowed path and it's
   length. */
struct fs_qmi_service_access_check_entry
{
  uint32 path_len;  /* whitelisted path */
  char *path;       /* length of this whitelisted entry */
};

/* Access list table */
struct fs_qmi_service_access_check_table_type
{
  struct fs_qmi_service_access_check_entry *whitelist; /* whitelist */
  uint32 whitelist_size;  /* number of whitelisted entries in the table. */
};

#endif /* FEATURE_EFS_QMI_PUT_GET_SERVICE */

#endif /* not __FS_QMI_SERVICE_H__ */

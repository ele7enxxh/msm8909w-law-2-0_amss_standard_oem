/***********************************************************************
 * fs_mfse.h
 *
 * Header file for the External Modem Filesystem Service.
 * Copyright (C) 2013 Qualcomm Technologies, Inc.
 *
 * Header file for service module that processes efs API requests from off-chip
 * clients. This service currently supports messages to force a sync from
 * Modem filesystem and to query the status of a sync request.
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fs_mfse/src/fs_mfse.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-04-03   dks   Create.

===========================================================================*/

#ifndef __FS_MFSE_H__
#define __FS_MFSE_H__

#include "fs_config_i.h"

#ifdef FEATURE_EFS_MFSE

#include "qmi_csi.h"

/* Signal used by service to register the service task with the framework to
   be informed of client requests. When the client does connect, disconnect,
   or process request the framework sets this signal on the service task. */
#define  FS_MFSE_HANDLE_EVENT 0x0001

/**************************************************************************//**
 * @par Name:
 *   fs_mfse_lock_init - Initialize the critical section for service module.
 *
 * @par Synopsis:
 *   @code
 *     #include "fs_mfse.h"
 *     fs_mfse_lock_init ();
 *   @endcode
 *
 *****************************************************************************/
void fs_mfse_lock_init (void);

/**************************************************************************//**
 * @par Name:
 *   fs_mfse_lock - Enter critical section.
 *
 * @par Synopsis:
 *   @code
 *     #include "fs_mfse.h"
 *     fs_mfse_lock ();
 *   @endcode
 *
 *****************************************************************************/
void fs_mfse_lock (void);

/**************************************************************************//**
 * @par Name:
 *   fs_mfse_unlock - Leave critical section.
 *
 * @par Synopsis:
 *   @code
 *     #include "fs_mfse.h"
 *     fs_mfse_unlock ();
 *   @endcode
 *
 *****************************************************************************/
void fs_mfse_unlock (void);


/**************************************************************************//**
 * @par Name:
 *   fs_mfse_init - Initialize the service module.
 *
 * @par Synopsis:
 *   @code
 *     #include "fs_mfse.h"
 *     fs_mfse_init ();
 *   @endcode
 *
 * @par Description:
 *   Initializes static objects and register callbacks with QCSI and QMUX.
 *   Initialized by the service task at startup.
 *
 *****************************************************************************/
void fs_mfse_init (void);


/**************************************************************************//**
 * @par Name:
 *  fs_mfse_handle_event - Handler called when service task receives the
 *   handle event signal registered with the QCSI framework at initialization.
 *
 * @par Synopsis:
 *   @code
 *     #include "fs_mfse.h"
 *     fs_mfse_handle_event ();
 *   @endcode
 *
 * @par Description:
 *   Calls the qmi_csi_handle_event function on the QCSI framework to get
 *   appropriate callback invoked based on client action.
 *
 *****************************************************************************/
void fs_mfse_handle_event (void);

#endif /* FEATURE_EFS_MFSE */

#endif /* not __FS_MFSE_H__ */

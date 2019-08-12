/***********************************************************************
 * fs_async_put_i.h
 *
 * Internal header for efs_async_put module.
 *
 * Copyright (C) 2011,2013 QUALCOMM Technologies, Inc.
 *
 * APIs called internally between files in efs_async_put and from EFS2
 * modules for processing requests.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs_async_put/inc/fs_async_put_i.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-06-27   wek   Initialize global tcb variable to signal into async init.
2011-11-21   dks   Create

===========================================================================*/
#ifndef __FS_ASYNC_PUT_I_H__
#define __FS_ASYNC_PUT_I_H__

/* Signal used to indicate the task needs to process the command queue. */
#define FS_ASYNC_PUT_CMD_Q_SIG 0x0001

/* Initialize the efs_async_put module to manage requests. */
void fs_async_put_init (void);

/* Function to process all outstanding requests in the command queue. */
void fs_async_put_process_commands (void);

/* Function to do cleanup before shutting down the File System.
 * This should be called only when shutdown or reset is going to occur.
 * It will process all outstanding requests for efs_async_put so all data gets
 * committed before shutdown. Returns 0 on success or -1 on error.
 */
int fs_async_put_prepare_shutdown (void);

#endif /* not __FS_ASYNC_PUT_I_H__ */

/***********************************************************************
 * fs_async_put.c
 *
 * Asynchronous write of EFS item files.
 * Copyright (C) 2011-2014 QUALCOMM Technologies, Inc.
 *
 * Implementation of services to handle queuing and processing of requests to
 * write to EFS2 Item files asynchronously without blocking caller until write
 * is done.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs_async_put/src/fs_async_put.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-04-26   vm    Use correct FS_MSG macros to save RAM.
2013-10-09   nr    Replace banned memory functions with safer versions.
2013-06-27   wek   Initialize global tcb variable to signal into async init.
2012-04-12   wek   Remove check to see if the feature efs2 is defined.
2012-01-17   wek   Remove unused header files to compile for boot.
2012-01-04   dks   Incorporate code review comments : unlock before signal.
2011-11-21   dks   Create

===========================================================================*/

#include "fs_async_put.h"
#include "fs_async_put_i.h"
#include "fs_async_put_config.h"
#include "fs_sys_types.h"
#include "fs_config.h"
#include "fs_public.h"
#include "fs_errno.h"
#include "fs_util.h"
#include "fs_err.h"
#include "fs_lib.h"
#include "fs_osal.h"
#include "queue.h"
#include "msg.h"
#include "stringl/stringl.h"
#include <string.h>

/* FS_PATH_MAX is the maximum length of a full pathname that EFS APIs support
 * not including a trailing '\\0' character. Have FS_ASYNC_PUT_MAX_PATH as
 * one char bigger to hold the terminating NULL.
 */
#define FS_ASYNC_PUT_MAX_PATH (FS_PATH_MAX + 1)


/* Command node structure to store the incoming put requests from clients. */
struct fs_async_put_cmd_type
{
  /* The link object is used as a link field with variables which allows them
     to be moved on and off queues.The specified link need to be initialized
     for use with the Queue Services like q_put and q_get. */
  q_link_type link;
  /* The asynchronous put is provided for clients who do not want to block
     until the write is complete. In this case, we cannot rely on the path and
     data buffer provided by the client to be in scope when the processing task
     gets aato run. Hence we need to copy the path and data into our local
     buffers. */
  char path [FS_ASYNC_PUT_MAX_PATH];
  uint8 data [FS_ASYNC_PUT_MAX_DATA_BYTES];
  fs_size_t length;                        /* Length of data being written. */
  int oflag;
  int mode;
};

/* Main structure for efs_async_put module containing all the objects needed
   for processing of write requests. */
struct fs_async_put_object_type
{
  /* Queues used to store the free and filled command nodes. Clients of the
     API get a free command by popping the cmd_free_queue, fill in the request
     and push the node to the cmd_queue. When the processing task gets to run,
     it will pop the cmd_queue, process the command and return the command node
     back to the cmd_free_queue. At any given time their combined sizes should
     equal FS_ASYNC_PUT_CMD_QUEUE_SIZE. */
  q_type cmd_queue;
  q_type cmd_free_queue;

  /* Array of command nodes distributed between cmd_queue & cmd_free_queue. */
  struct fs_async_put_cmd_type
         fs_async_put_cmd_nodes [FS_ASYNC_PUT_CMD_QUEUE_SIZE];

  /* Critical section to protect the queues and command nodes from simultaneous
     access. */
  fs_os_crit_sect_type fs_async_put_crit_sect;
};
static struct fs_async_put_object_type fs_async_put_obj;

static fs_os_tcb_type *fs_async_put_task_tcb;

/* Allow client tasks to signal the processing task about new incoming
   requests. */
static void
fs_async_put_signal_new_request (void)
{
  fs_os_set_sigs (fs_async_put_task_tcb, FS_ASYNC_PUT_CMD_Q_SIG);
}

/* APIs to control access to the critical section */
static void
fs_async_put_lock_init (void)
{
  memset (&fs_async_put_obj, 0x0, sizeof (struct fs_async_put_object_type));
  fs_os_init_crit_sect (&fs_async_put_obj.fs_async_put_crit_sect);
}

static void
fs_async_put_lock (void)
{
  fs_os_enter_crit_sect (&fs_async_put_obj.fs_async_put_crit_sect);
}

static void
fs_async_put_unlock (void)
{
  fs_os_leave_crit_sect (&fs_async_put_obj.fs_async_put_crit_sect);
}

/* Initialize the module. */
void
fs_async_put_init (void)
{
  int i;

  fs_async_put_lock_init ();
  fs_async_put_lock ();

  fs_async_put_task_tcb = fs_os_self ();

  /* Initialize both queues. */
 (void) q_init (&fs_async_put_obj.cmd_queue);
 (void) q_init (&fs_async_put_obj.cmd_free_queue);

  /* Link all command nodes to free_queue to begin with. */
  for (i = 0; i < FS_ASYNC_PUT_CMD_QUEUE_SIZE; i++)
  {
    q_put (&fs_async_put_obj.cmd_free_queue,
           q_link (&fs_async_put_obj.fs_async_put_cmd_nodes[i],
           &fs_async_put_obj.fs_async_put_cmd_nodes[i].link));
  }

  fs_async_put_unlock ();
}

/* API to queue the write request. */
int
efs_async_put (const char *path, void *data, fs_size_t length, int oflag,
               int mode)
{
  int path_len;
  struct fs_async_put_cmd_type *cmd;
  int result = -1;  /* Set default to failure. */

  /* Validate parameters. */
  if (!path || !data || strlen (path) < 1)
  {
    efs_errno = EINVAL;
    goto cleanup;
  }

  path_len = strlen (path) + 1;
  if (path_len > FS_ASYNC_PUT_MAX_PATH)
  {
    efs_errno = ENAMETOOLONG;
    goto cleanup;
  }

  if (length > FS_ASYNC_PUT_MAX_DATA_BYTES)
  {
    efs_errno = FS_ERANGE;
    goto cleanup;
  }

  /* Grab cmd lock. */
  fs_async_put_lock ();

  /* Pop free Q. */
  cmd = (struct fs_async_put_cmd_type *)
         q_get (&fs_async_put_obj.cmd_free_queue);
  if (cmd == NULL)
  {
    efs_errno = ENOSPC;
    fs_async_put_unlock ();
    goto cleanup;
  }

  /* Populate cmd. */
  memset (cmd, 0x0, sizeof (struct fs_async_put_cmd_type));
  strlcpy (cmd->path, path, path_len);
  fs_memscpy (cmd->data, sizeof (cmd->data), data, length);
  cmd->length = length;
  cmd->oflag = oflag;
  cmd->mode = mode;

  /* Add cmd node to request Q. */
  q_put (&fs_async_put_obj.cmd_queue, q_link (cmd, &cmd->link));

  /* Release cmd lock. */
  fs_async_put_unlock ();

  /* Signal to the processing task about new request. */
  fs_async_put_signal_new_request ();
  result = 0;

cleanup:
  return result;
}

/* Function to process all outstanding requests in the command queue. */
void
fs_async_put_process_commands (void)
{
  struct fs_async_put_cmd_type *cmd;
  int result, error;

  /* Grab cmd lock. */
  fs_async_put_lock ();

  /* Pop the command queue. */
  cmd = (struct fs_async_put_cmd_type *) q_get (&fs_async_put_obj.cmd_queue);

  while (cmd != NULL)
  {
    /* Unlock cmd Q. New requests don't have to be held up while processing. */
    fs_async_put_unlock ();

    /* Process the cmd. */
    result = efs_put (cmd->path, cmd->data, cmd->length,
                      cmd->oflag, cmd->mode);
    if (result != 0)
    {
      /* Since caller will not have a way to check back on result, it is easier
         to debug. */
      error = efs_errno;
      FS_MSG_HIGH_1 ("[%d] async_put failed ", error);
    }

    /* Grab cmd lock. */
    fs_async_put_lock ();

    /* Add cmd to free cmd Q. */
    q_put (&fs_async_put_obj.cmd_free_queue, q_link (cmd, &cmd->link));

    /* Pop the command queue for the next request. */
    cmd = (struct fs_async_put_cmd_type *) q_get (&fs_async_put_obj.cmd_queue);
  }

  fs_async_put_unlock ();
}


/* API called from tmc's / rcinit's context when efs_prepare_shutdown is called
   and used to finish writing all outstanding write requests. */
int
fs_async_put_prepare_shutdown (void)
{
  fs_async_put_process_commands ();

  return 0;
}

/***********************************************************************
 * fs_qmi_service_access_check.c
 *
 * Access control for remote processor requests.
 * Copyright (C) 2013-2014 QUALCOMM Technologies, Inc.
 *
 * Maintains list of filesystem paths that are allowed read and write access
 * from remote processors. fs_qmi_service will check if the remote processor is
 * allowed access to the path specified before the request is sent to the modem
 * filesystem. This makes sure that even if remote processor data is stored on
 * the modem filesystem, the modem filesystem data is  protected from outside
 * access.
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/efs_qmi_put_get/service/src/fs_qmi_service_access_check.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-08-06   dks   Add IPA conf file to whitelist.
2014-07-11   dks   Add IPA path to whitelist.
2014-02-10   dks   Add DebugAgent config-file path to whitelist.
2013-04-30   dks   Add sound driver config-file path to whitelist.
2013-01-17   rp    Add thermal-engines config-file path to whitelist.
2012-10-17   dks   Add QBI path to whitelist.
2012-05-04   dks   Add additional hsusb conf files to the whitelist.
2012-03-23   dks   Create

===========================================================================*/

#include "fs_qmi_service.h"
#include "fs_stdlib.h"
#include "assert.h"
#include "fs_qmi_std_interface.h"
#include <string.h>

/* List of filesystem paths allowed remote access. The path len used for
   stirng comparision which will be filled during initialization. */
static struct fs_qmi_service_access_check_entry
fs_qmi_service_access_check_list [] =
{
  /* USB files */
  {0, "/nv/item_files/hsusb/"},
  {0, "/nv/item_files/conf/hsusb0.conf"},
  {0, "/nv/item_files/conf/hsusb1.conf"},
  {0, "/nv/item_files/conf/hsusb2.conf"},
  {0, "/nv/item_files/conf/hsusb3.conf"},
  {0, "/nv/item_files/conf/hsusb4.conf"},
  {0, "/nv/item_files/conf/hsusb5.conf"},
  {0, "/nv/item_files/conf/hsusb6.conf"},
  {0, "/nv/item_files/conf/hsusb7.conf"},
  {0, "/nv/item_files/conf/hsusb8.conf"},
  {0, "/nv/item_files/conf/hsusb9.conf"},
  {0, "/nv/item_files/conf/hsusb10.conf"},
  /* Timer files */
  {0, "/nvm/alpha/item_file/time/"},
  /* RDM files */
  {0, "/rdm_config.csv"},
  /* Debugtools err files */
  {0, "/nv/item_files/dnt/"},
  {0, "/nv/item_files/f3_trace/"},
  {0, "/nv/item_files/conf/dnt_err0.conf"},
  {0, "/nv/item_files/conf/dnt_err1.conf"},
  {0, "/nv/item_files/conf/dnt_err2.conf"},
  {0, "/nv/item_files/conf/f3_trace0.conf"},
  {0, "/nv/item_files/conf/f3_trace1.conf"},
  {0, "/nv/item_files/conf/f3_trace2.conf"},
  /* Power files */
  {0, "/nv/item_files/apps_dcvs/"},
  /* Sleep files */
  {0, "/nv/item_files/sleep/"},
  /* QBI files */
  {0, "/nv/item_files/qbi/"},
  /* Thermal-Engine config file */
  {0, "/nv/item_files/thermal-engine/thermal-engine.conf"},
  /* Sound-driver config file */
  {0, "/nv/item_files/csd/csd.conf"},
  /* QDSS DebugAgent config files */
  {0, "/debugagent/agent_cfg_def.txt"},
  {0, "/debugagent/agent_cfg1.txt"},
  /* IPA */
  {0, "/nv/item_files/apps/data/ipa/"},
  {0, "/nv/item_files/conf/ipa_apps.conf"}
};

static struct fs_qmi_service_access_check_table_type
              fs_qmi_service_access_check_tbl;

/* Absolute path to hold normalized input path. */
static char abs_path [FS_QMI_MAX_FILE_PATH + 1];

/* Initialize the pointer to the whitelist and it's size. */
void
fs_qmi_service_access_check_init (void)
{
  uint32 i;
  struct fs_qmi_service_access_check_entry *entry;

  memset (&fs_qmi_service_access_check_tbl, 0x0,
          sizeof (struct fs_qmi_service_access_check_table_type));

  fs_qmi_service_access_check_tbl.whitelist =
                                   fs_qmi_service_access_check_list;

  fs_qmi_service_access_check_tbl.whitelist_size =
        sizeof (fs_qmi_service_access_check_list) /
        sizeof (struct fs_qmi_service_access_check_entry);

  for (i = 0; i < fs_qmi_service_access_check_tbl.whitelist_size; i++)
  {
    entry = &fs_qmi_service_access_check_tbl.whitelist[i];
    entry->path_len = strlen (entry->path);
  }
}

/* Return pointr to whitelist table */
struct fs_qmi_service_access_check_table_type*
fs_qmi_service_access_get_list (void)
{
  return &fs_qmi_service_access_check_tbl;
}

/* Check if access is allowed to this path. Returns 1 if path has remote access
   and 0 otherwise. */
int
fs_qmi_service_access_check (const char *path)
{
  uint32 i, len;

  /* Only fs_qmi_service should call for access check and it should not pass
     invalid strings. fs_qmi_client should have already sent EINVAL. */
  ASSERT (path != NULL);
  len = strlen (path);
  ASSERT (len != 0);
  ASSERT  (len < FS_QMI_MAX_FILE_PATH);

  /* Normalize the path to get absolute path. */
  if (efs_normalize_path_string (path, abs_path, sizeof (abs_path)) == NULL)
  {
    return -1;
  }

  /* Get length after normalization. */
  len = strlen (abs_path);
  if  (len > FS_QMI_MAX_FILE_PATH)
  {
    return -1;
  }

  /* Search if path is in the whitelist. */
  for (i = 0; i < fs_qmi_service_access_check_tbl.whitelist_size; i++)
  {
    struct fs_qmi_service_access_check_entry *entry =
          &fs_qmi_service_access_check_tbl.whitelist[i];
    char *tmp_path = entry->path;
    uint32 compare_len = entry->path_len;

   /* efs_normalize_path_string does not add / in the beginning if there isn't
      one since it might cross over the length of the supplied buffer. Fix
      the comapre buffer to skip the first / in the entry path in that case. */
    if (abs_path[0] != '/')
    {
      tmp_path++;
      compare_len = entry->path_len - 1;
    }

    /* When entry path is a directory, the supplied path must only be compared
       until entry path's length. */
    if (entry->path[entry->path_len - 1] == '/')
    {
      if (strncmp (tmp_path, abs_path, compare_len) == 0)
      {
        return 1;
      }
    }
    else /* If file then path lengths should be same and paths must match */
    {
      if ((len == compare_len) &&
         (strncmp (tmp_path, abs_path, compare_len) == 0))
      {
        return 1;
      }
    }
  }

  return 0;
}


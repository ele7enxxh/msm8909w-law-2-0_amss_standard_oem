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
  qmi_nv_is_indexed
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/ds_qmi_nv_util.c#1 $
$DateTime: 2016/12/13 08:00:03 $
$Author: mplcsds1 $
$Change: 11985188 $

when     who  what, where, why
-------- ---  ---------------------------------------------------------------- 
12/09/10 mj   Ported module to QMI

==============================================================================*/


/*==============================================================================

                      INCLUDE FILES FOR MODULE

==============================================================================*/

#include "ds_Utils_DebugMsg.h"

#include "nv_items.h"
#include "ds_qmi_svc.h"
#include "ds_qmi_nv_util.h"
#include "ds_qmi_svc_ext.h"
#include "ps_system_heap.h"


#include <stringl/stringl.h>
#include "fs_errno.h"
#include "fs_public.h"
#include "fs_sys_types.h"
#include "ps_utils.h"

/*==============================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Private Constants and Macros
==============================================================================*/

/* Maximum length of an EFS path for an NV item, including null terminator.
 * This MUST guarantee enough room for the directory + item + index + muxing */
#define QMI_NV_MAX_EFS_PATH_LEN  (72)

/* Use -rwxr--r-- as default permissions */
#define QMI_NV_EFS_PERMS         (0644)

/* Directories where NV items will be stored */
#define QMI_NV_OLD_DIR           "/nvm/num/"
#define QMI_NV_NEW_DIR           "/nv/item_files/modem/data/"
#define QMI_NV_FILE_DIR          "/nv/reg_files/modem/data/"

/* Converts a new-style QMI NV item (qmi_nv_item_e_type) into the index
 * in the qmi_nv_item_paths array that gives its EFS filename. */
#define QMI_NV_ITEM_FNAME_INDEX(item) ((item - QMI_NVI_ITEM_START) - 1)

/* Converts a new-style QMI NV item (qmi_nv_file_e_type) into the index
 * in the qmi_nv_file_paths array that gives its EFS filename. */
#define QMI_NV_FILE_FNAME_INDEX(file) ((file - QMI_NVI_FILE_START) - 1)

/* Total number of new-style NV items */
#define QMI_NV_NUM_ITEMS QMI_NV_ITEM_FNAME_INDEX(QMI_NVI_ITEM_END)

/* Total number of NV EFS files */
#define QMI_NV_NUM_FILES QMI_NV_FILE_FNAME_INDEX(QMI_NVI_FILE_END)

/* Maximum length of the filename (not including directory info) of new-style 
 * QMI NV items.  Includes NULL term.  Also change QMI_NV_MAX_EFS_PATH_LEN
 * if modifying this value. */
#define QMI_NV_FILENAME_MAX_LEN  (32)

/*==============================================================================
  Private Typedefs
==============================================================================*/

typedef struct qmi_nv_indexed_item_info_s {
  qmi_nv_item_e_type item;
  uint8              max_index; 
} qmi_nv_indexed_item_info_s_type;

/*==============================================================================
  Private Variables
==============================================================================*/

/* Filename for new-style QMI NV items. Must have same order as the enum
 * qmi_nv_item_e_type. EFS item files are created in the QMI NV directory
 * with this filename, e.g. "/nv/item_files/modem/data/example" */
static const char qmi_nv_item_paths[QMI_NV_NUM_ITEMS][QMI_NV_FILENAME_MAX_LEN] =
{
  /* NOTE: see the definition of qmi_nv_item_e_type in ds_qmi_nv_util.h for
   * instructions on adding new NV items */
/*"-------------------------------" the filename must fit within these quotes */
  "default_nai",                    /* QMI_NV_ITEM_DEFAULT_NAI */
  "last_otasp_state",               /* QMI_NV_ITEM_LAST_OTASP_STATE */
  "use_otasp_act_state",            /* QMI_NV_ITEM_USE_OTASP_ACT_STATE */
  "allow_invalid_spc",              /* QMI_NV_ITEM_ALLOW_INVALID_SPC */
  "use_max_defined_data_rates",     /* QMI_NV_ITEM_USE_MAX_DEFINED_DATA_RATES */
  
  "restrict_imsi_access",           /* QMI_NV_ITEM_RESTRICT_IMSI_ACCESS */
  "manual_dns_ipv4_pri",            /* QMI_NV_ITEM_MANUAL_DNS_IPV4_PRI */
  "manual_dns_ipv4_sec",            /* QMI_NV_ITEM_MANUAL_DNS_IPV4_SEC */
  "autoconnect_pause_setting",      /* QMI_NV_ITEM_AUTOCONNECT_PAUSE_SETTING */
  "autoconnect_roam_setting",       /* QMI_NV_ITEM_AUTOCONNECT_ROAM_SETTING */
  
  "last_rmnet_mip_error",           /* QMI_NV_ITEM_LAST_RMNET_MIP_ERR_CODE */
  "restrict_autoconnect",           /* QMI_NV_ITEM_RESTRICT_AUTOCONNECT */
  "sync_reset_modem_state",         /* QMI_NV_ITEM_SYNC_RESET_MODEM_STATE */
  "manual_dns_ipv6_pri",            /* QMI_NV_ITEM_MANUAL_DNS_IPV6_PRI */
  "manual_dns_ipv6_sec",            /* QMI_NV_ITEM_MANUAL_DNS_IPV6_SEC */

  "rmnet_global_config",            /* QMI_NV_ITEM_RMNET_GLOBAL_CONFIG */
  "qmap_cmd_resp_time",             /* QMI_NV_ITEM_QMAP_CMD_RESP_TIME */
  "rmnet_autoconnect",              /* QMI_NV_ITEM_RMNET_AUTOCONNECT */
/*"-------------------------------" the filename must fit within these quotes */
};

/* Filename for new-style QMI NV files. Must have same order as the enum
 * qmi_nv_file_e_type. EFS item files are created in the QMI NV directory
 * with this filename, e.g. "/nv/reg_files/modem/data/example" */
static const char qmi_nv_file_paths[QMI_NV_NUM_FILES][QMI_NV_FILENAME_MAX_LEN] =
{
  /* NOTE: see the definition of qmi_nv_file_e_type in ds_qmi_nv_util.h for
   * instructions on adding new NV files */
/*"-------------------------------" the filename must fit within these quotes */
  "user_data_file",                 /* QMI_NV_FILE_USER_DATA */
  "eri_file",                       /* QMI_NV_FILE_ERI */
  "roam_ind_list"                   /* QMI_NV_FILE_ROAM_IND_LIST */
/*"-------------------------------" the filename must fit within these quotes */
};

/* Table listing which new-style QMI NV items are indexed, and the maximum
 * index value allowed for the item */
static const qmi_nv_indexed_item_info_s_type qmi_nv_indexed_items[] =
{
  /* item number, maximum index value (i.e. array size - 1) */
  { QMI_NV_ITEM_MANUAL_DNS_IPV4_PRI,       QMI_INSTANCE_MAX },
  { QMI_NV_ITEM_MANUAL_DNS_IPV4_SEC,       QMI_INSTANCE_MAX },
  { QMI_NV_ITEM_AUTOCONNECT_PAUSE_SETTING, QMI_INSTANCE_MAX },
  { QMI_NV_ITEM_AUTOCONNECT_ROAM_SETTING,  QMI_INSTANCE_MAX },
  { QMI_NV_ITEM_MANUAL_DNS_IPV6_PRI,       QMI_INSTANCE_MAX },
  { QMI_NV_ITEM_MANUAL_DNS_IPV6_SEC,       QMI_INSTANCE_MAX }
};

/*==============================================================================
  Private Function Prototypes
==============================================================================*/

static uint8 qmi_nvi_get_max_index(
  qmi_nv_item_e_type item
  );

static qmi_nv_status_e_type qmi_nvi_item_build_path(
  qmi_nv_item_e_type item,
  uint32             ep_id,
  uint8              mux_id,
  boolean *          new_indexed,
  char *             path,
  uint32             path_len
  );

static qmi_nv_status_e_type qmi_nvi_file_build_path(
  qmi_nv_file_e_type file,
  char *             path,
  uint32             path_len
  );

static qmi_nv_status_e_type qmi_nvi_mkdir_p(
  const char * path_in
  );

static qmi_nv_status_e_type qmi_nvi_file_get_contents(
  const char * path,
  char *       file_buf,
  fs_size_t    buf_size,
  fs_size_t *  file_size,
  uint32       ps_subs_id
  );

static qmi_nv_status_e_type qmi_nvi_file_put_contents(
  const char * path,
  char *       file_buf,
  fs_size_t    buf_size,
  uint32       ps_subs_id
  );


/*==============================================================================

                    FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Functions for internal module use only
==============================================================================*/

/*==========================================================================

  FUNCTION qmi_nvi_get_max_index

  DESCRIPTION
    Given a new-style QMI NV item, returns the maximum allowed index
    value.  If the item is not indexed, this value is 0, otherwise the
    value from qmi_nv_indexed_items[] is used.

  DEPENDENCIES
    path_len must be >= QMI_NV_MAX_EFS_PATH_LEN

  PARAMETERS
    item     [in] NV item: can be nv_items_enum_type or qmi_nv_item_e_type
    
  RETURN VALUE
    Maximum allowed value for index parameter for the given item

  SIDE EFFECTS
    None.

===========================================================================*/
static uint8 qmi_nvi_get_max_index(
  qmi_nv_item_e_type item
  )
{
  uint32 i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Linear search is used currently because the indexed items table is 
   * very small.  This table will be kept ordered, so if the lookup speed
   * becomes a concern, this can easily change to binary search. */
  for (i = 0; i < ARR_SIZE(qmi_nv_indexed_items); i++)
  {
    if (item == qmi_nv_indexed_items[i].item)
    {
      return qmi_nv_indexed_items[i].max_index;
    }
  }
  
  return 0;
} /* qmi_nvi_get_max_index() */

/*==========================================================================

  FUNCTION qmi_nvi_item_build_path

  DESCRIPTION
    This function turns an NV item and an index into a path to a file in 
    EFS.  Performs validation of the item and index.

  DEPENDENCIES
    path_len must be >= QMI_NV_MAX_EFS_PATH_LEN

  PARAMETERS
    item         [in] NV item: can be nv_items_enum_type or
                      qmi_nv_item_e_type
    index        [in] index of the NV item, or 0 if no index
    new_indexed [out] will be set to TRUE if the item is new-style and
                      indexed. can be NULL
    path        [out] EFS path to NV item file
    path_len     [in] size of buffer at data_ptr
    
    
  RETURN VALUE
    QMI_NV_STATUS_OK on success
    QMI_NV_STATUS_BAD_ITEM if item is not in old NV or new QMI NV range,
        or index is outside defined limit (new-style items only)

  SIDE EFFECTS
    None.

===========================================================================*/
static qmi_nv_status_e_type qmi_nvi_item_build_path(
  qmi_nv_item_e_type item,
  uint32             ep_id,
  uint8              mux_id,
  boolean *          new_indexed,
  char *             path,
  uint32             path_len
  )
{
  uint32 path_offset;
  qmi_nv_status_e_type status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  path_offset = 0;
  status = QMI_NV_STATUS_OK;

  /* New-style items */
  if (item > QMI_NVI_ITEM_START && item < QMI_NVI_ITEM_END)
  {
      /* Copy in new-style directory, UQCN muxing (if any), and filename */
      path_offset = strlcpy(path, QMI_NV_NEW_DIR, path_len);
      if (path_offset < path_len)
      {
        path_offset += strlcpy(&path[path_offset],
                               qmi_nv_item_paths[QMI_NV_ITEM_FNAME_INDEX(item)],
                               path_len - path_offset);
      }
    if (new_indexed != NULL)
    {
      *new_indexed = (ep_id != 0);
    }
  }
  else /* item is NOT new-style */
  {
    /* Note that we purposefully do not check the index range for old-style
     * items since that data is maintained in the NV layer, which is not
     * accessible from boot code - and boot is the only place where qmi_nv 
     * should be used with old-style items */
    if (item >= (qmi_nv_item_e_type)NV_MAX_I)
    {
      status = QMI_NV_STATUS_BAD_ITEM;
    }
    else /* item is old-style */
    {
      /* Copy in old-style directory + item number as filename */
      path_offset = strlcpy(path, QMI_NV_OLD_DIR, path_len);
      if (path_offset < path_len)
      {
        path_offset += snprintf(&path[path_offset],path_len - path_offset, 
                                      "%d", item);
      }
    }

    if (new_indexed != NULL)
    {
      *new_indexed = FALSE;
    }
  }
  /* Append item index if non-zero */
  if (status == QMI_NV_STATUS_OK && ep_id != 0 && path_offset < path_len)
  {
    path_offset += snprintf(&path[path_offset], path_len - path_offset,
                                  "_%u", (unsigned int)ep_id);
    path_offset += snprintf(&path[path_offset], path_len - path_offset,
                                  "_%d", mux_id);

  }
  /* If the path offset exceededs the size of the buffer, then it means we 
   * could not copy the entire path. If it matches, then one character was
   * truncated from the input to make room for the null termination. */
  if (path_offset >= path_len)
  {
    status = QMI_NV_STATUS_PATH_TOO_LONG;
  }

  LOG_MSG_INFO3_1("returning status=%d", status);

  return status;
} /* qmi_nvi_item_build_path() */

/*==========================================================================

  FUNCTION qmi_nvi_file_build_path

  DESCRIPTION
    This function turns an NV item and an index into a path to a file in 
    EFS.  Performs validation of the item and index.

  DEPENDENCIES
    path_len must be >= QMI_NV_MAX_EFS_PATH_LEN

  PARAMETERS
    item         [in] NV file: qmi_nv_file_e_type
    path         [out] EFS path to NV EFS file
    path_len     [in] size of buffer at data_ptr
    
  RETURN VALUE
    QMI_NV_STATUS_OK on success
    QMI_NV_STATUS_BAD_ITEM if item is not in old NV or new QMI NV range,
        or index is outside defined limit (new-style items only)

  SIDE EFFECTS
    None.

===========================================================================*/
static qmi_nv_status_e_type qmi_nvi_file_build_path(
  qmi_nv_file_e_type file,
  char *             path,
  uint32             path_len
  )
{
  uint32 path_offset;
  qmi_nv_status_e_type status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  path_offset = 0;
  status = QMI_NV_STATUS_OK;

  /* New-style items */
  if (file > QMI_NVI_FILE_START && file < QMI_NVI_FILE_END)
  {
    /* Copy in new EFS file directory and filename */
    path_offset = strlcpy(path, QMI_NV_FILE_DIR, path_len);
    if (path_offset < path_len)
    {
      path_offset += strlcpy(&path[path_offset],
                           qmi_nv_file_paths[QMI_NV_FILE_FNAME_INDEX(file)],
                           path_len - path_offset);
    }

    /* If the path offset exceededs the size of the buffer, then it means we 
     * could not copy the entire path. If it matches, then one character was
     * truncated from the input to make room for the null termination. */
    if (path_offset >= path_len)
    {
      status = QMI_NV_STATUS_PATH_TOO_LONG;
    }
  } 
  else 
  {
    status = QMI_NV_STATUS_BAD_ITEM;
  }

  LOG_MSG_INFO3_1("returning status=%d", status);

  return status;
} /* qmi_nvi_file_build_path() */

/*==========================================================================

  FUNCTION qmi_nvi_mkdir_p

  DESCRIPTION
    Makes a directory and any needed parent directories.

  DEPENDENCIES
    None.

  PARAMETERS
    path [in] directory to create, with or without trailing slash.
              length should be < QMI_NV_MAX_EFS_PATH_LEN
    
  RETURN VALUE
    QMI_NV_STATUS_OK on success
    QMI_NV_STATUS_EFS_ERR if an error occurs from EFS

  SIDE EFFECTS
    None.

===========================================================================*/
static qmi_nv_status_e_type qmi_nvi_mkdir_p(
  const char * path_in
  )
{
  uint32 offset;
  uint32 len;
  int    efs_ret;
  char   path[QMI_NV_MAX_EFS_PATH_LEN];

  qmi_nv_status_e_type status = QMI_NV_STATUS_OK;
  mcfg_fs_e_type        file_type = MCFG_FS_TYPE_EFS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  len = strlen(path_in);
  len = MIN(len, (sizeof(path) - 1));
  path[0] = '/';

  /* Copy path_in into path, one byte at a time, creating each directory 
   * along the way */
  for (offset = 1; offset < len; offset++)
  {
    if (path_in[offset] == '/')
    {
      /* Temporarily end the string to create the subdirectory */
      path[offset] = '\0';
      efs_ret = mcfg_mkdir(path, MCFG_FS_ALLPERMS, file_type);
      
      /* If efs_mkdir failed for any reason other than the directory already 
       * exists, then it is a problem. */
      if (efs_ret != 0 && mcfg_fs_errno( file_type ) != EEXIST)
      {
        status = QMI_NV_STATUS_EFS_ERR;
        break;
      }
      
      /* Update the path with the next directory separator before continuing */
      path[offset] = '/';
    }
    else
    {
      path[offset] = path_in[offset];
    }
  }

  /* Create bottom-level directory if it wasn't already done in the loop 
   * (i.e. if path_in does not have a trailing slash) */
  if (status == QMI_NV_STATUS_OK && path_in[len - 1] != '/')
  {
    efs_ret = mcfg_mkdir(path_in, MCFG_FS_ALLPERMS, file_type);
    if (efs_ret != 0 && mcfg_fs_errno( file_type ) != EEXIST)
    {
      status = QMI_NV_STATUS_EFS_ERR;
    }
  }

  LOG_MSG_INFO3_1("returning status=%d", status);

  return status;
} /* qmi_nvi_mkdir_p() */

/*==========================================================================

  FUNCTION qmi_nvi_file_get_contents

  DESCRIPTION
    Reads the full contents of a regular file into the buffer. Length is
    assumed to be fixed to buf_size, any other value results in an error.

  DEPENDENCIES
    None.

  PARAMETERS
    path      [in] absolute path to the file in EFS
    file_buf [out] buffer to hold the data
    buf_size  [in] amount of data to read, also size of file_buf
    file_size [out] size of the file
    
  RETURN VALUE
    QMI_NV_STATUS_OK on success
    QMI_NV_STATUS_NOT_ACTIVE if file doesn't exist
    QMI_NV_STATUS_EFS_ERR on EFS error

  SIDE EFFECTS
    None.

===========================================================================*/
static qmi_nv_status_e_type qmi_nvi_file_get_contents(
  const char * path,
  char *       file_buf,
  fs_size_t    buf_size,
  fs_size_t *  file_size,
  uint32       ps_subs_id
  )
{
  int                  fd;
  fs_ssize_t           bytes_read;
  fs_ssize_t           efs_ret;
  struct fs_stat       stat_buf;
  qmi_nv_status_e_type status;
  mcfg_fs_e_type        file_type = MCFG_FS_TYPE_EFS;
  mcfg_fs_sub_id_e_type mcfg_subs_id = MCFG_FS_SUBID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  status = QMI_NV_STATUS_OK;

  if (path == NULL || file_buf == NULL || file_size == NULL)
  {
    status = QMI_NV_STATUS_NULL_PTR;
  }
  else
  {
    mcfg_subs_id = ps_utils_convert_subs_ps_to_mcfg(ps_subs_id);
    fd = mcfg_fopen(path, MCFG_FS_O_RDONLY, MCFG_FS_ALLPERMS,
                    file_type, mcfg_subs_id);
    if (fd < 0) 
    {
      if (mcfg_fs_errno( file_type ) == ENOENT)
      {
        status = QMI_NV_STATUS_NOT_ACTIVE;
      }
      else
      {
        status = QMI_NV_STATUS_EFS_ERR;
      }
    }
    else
    {
      efs_ret = mcfg_fs_stat(path, &stat_buf, file_type, mcfg_subs_id);
      if (efs_ret != MCFG_FS_STATUS_OK)
      {
        status = QMI_NV_STATUS_EFS_ERR;
      }
      else
      {
        *file_size = stat_buf.st_size;
        bytes_read = 0;
        do
        {
          efs_ret = mcfg_fread(fd, &file_buf[bytes_read], (buf_size - bytes_read), file_type);
        } while (efs_ret > 0 && (uint32)(bytes_read += efs_ret) < buf_size);
  
        if (efs_ret < 0)
        {
          status = QMI_NV_STATUS_EFS_ERR;
        }
      }
  
      (void) mcfg_fclose(fd, file_type);
    }
  }

  LOG_MSG_INFO3_1("returning status=%d", status);

  return status;
} /* qmi_nvi_file_get_contents() */

/*==========================================================================

  FUNCTION qmi_nvi_file_put_contents

  DESCRIPTION
    Writes the contents of a buffer to a file. The file will be created
    if it doesn't exist, and truncated before writing if it does.

  DEPENDENCIES
    The directory containing the file must already be created.

  PARAMETERS
    path      [in] absolute path to the file in EFS
    file_buf [out] buffer containing the data
    buf_size  [in] amount of data to write
    
  RETURN VALUE
    QMI_NV_STATUS_OK on success
    QMI_NV_STATUS_EFS_ERR on EFS error

  SIDE EFFECTS
    None.

===========================================================================*/
static qmi_nv_status_e_type qmi_nvi_file_put_contents(
  const char * path,
  char *       file_buf,
  fs_size_t    buf_size,
  uint32       ps_subs_id
  )
{
  int                  fd;
  fs_ssize_t           bytes_written;
  fs_ssize_t           efs_ret;
  qmi_nv_status_e_type status;
  mcfg_fs_e_type        file_type = MCFG_FS_TYPE_EFS;
  mcfg_fs_sub_id_e_type mcfg_subs_id = MCFG_FS_SUBID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  status = QMI_NV_STATUS_OK;

  if (path == NULL || file_buf == NULL)
  {
    status = QMI_NV_STATUS_NULL_PTR;
  }
  else
  {
    mcfg_subs_id = ps_utils_convert_subs_ps_to_mcfg(ps_subs_id);

    fd = mcfg_fopen(path, (MCFG_FS_O_CREAT | MCFG_FS_O_TRUNC | MCFG_FS_O_WRONLY), MCFG_FS_ALLPERMS,
                    file_type, mcfg_subs_id);
    if (fd < 0) 
    {
      status = QMI_NV_STATUS_EFS_ERR;
    }
    else
    {
      bytes_written = 0;
      do
      {
        efs_ret = mcfg_fwrite(fd, &file_buf[bytes_written], 
                            (buf_size - bytes_written), file_type);
      } while (efs_ret > 0 && (uint32)(bytes_written += efs_ret) < buf_size);
  
      if (efs_ret < 0 || bytes_written != buf_size)
      {
        status = QMI_NV_STATUS_EFS_ERR;
      }
      (void) mcfg_fclose(fd, file_type);
    }
  }

  LOG_MSG_INFO3_1("returning status=%d", status);

  return status;
} /* qmi_nvi_file_put_contents() */


/*==============================================================================
  Functions exported in public header files
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
  )
{
  uint32 ps_subs_id = PS_SYS_DEFAULT_SUBS;
  return qmi_nv_read_ex(item,ep_id,mux_id,data_ptr,data_size,ps_subs_id);
} /* qmi_nv_read() */

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
    ps_subs_id [in] subscription on which the NV operation is to be performed
    
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
  )
{
  char path[QMI_NV_MAX_EFS_PATH_LEN];
  int  efs_ret;

  boolean new_indexed;
  qmi_nv_status_e_type status;
  mcfg_fs_e_type        file_type = MCFG_FS_TYPE_EFS;
  mcfg_fs_sub_id_e_type mcfg_subs_id = MCFG_FS_SUBID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (data_ptr == NULL)
  {
    status = QMI_NV_STATUS_NULL_PTR;
  }
  else
  {
    mcfg_subs_id = ps_utils_convert_subs_ps_to_mcfg(ps_subs_id);
    status = qmi_nvi_item_build_path(item, ep_id, mux_id, &new_indexed, path, 
                                     sizeof(path));

    if (status == QMI_NV_STATUS_OK)
    {
      /* New-style indexed items are required to have a uint8 index field 
       * as the first byte of their struct.  This index is not stored in
       * EFS, so manually set it here, then adjust the pointer and size.
       * This is also true of using old-style indexed items through the
       * legacy NV layer, and has been kept for consistency. */
      if (new_indexed)
      {
        uint8 *data_index = (uint8 *) data_ptr;
        memscpy(data_index, sizeof(uint32), (uint8 *)&ep_id, sizeof(uint32));
        data_index += sizeof(uint32);
        *data_index++ = mux_id; 
        data_ptr = (void *) data_index;
        data_size -= (sizeof(ep_id) + sizeof(mux_id));
      }

      efs_ret = mcfg_fs_read(path, data_ptr, data_size, file_type, mcfg_subs_id);
      if (efs_ret != MCFG_FS_STATUS_OK)
      {
        switch (mcfg_fs_errno( file_type ))
        {
          case ENOENT:
            status = QMI_NV_STATUS_NOT_ACTIVE;
            break;

          case FS_ERANGE:
            status = QMI_NV_STATUS_BAD_SIZE;
            break;

          default:
            status = QMI_NV_STATUS_EFS_ERR;
        }
      }
    }
    else
    {
      LOG_MSG_INFO2_3("build path failed for item=%d ep_id=%d and mux_id=%d",item,ep_id,mux_id);
    }
  }

  if (status != QMI_NV_STATUS_OK)
  {
    LOG_MSG_INFO2_1("returning status=%d", status);
  }

  return status;
} /* qmi_nv_read_ex() */

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
  )
{
  uint32 ps_subs_id = PS_SYS_DEFAULT_SUBS;
  return qmi_nv_write_ex(item,ep_id,mux_id,data_ptr,data_size,ps_subs_id);
} /* qmi_nv_write() */


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
    ps_subs_id [in] subs id on which the EFS operation is to be performed
    
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
  )
{
  struct fs_stat stat_buf;
  char path[QMI_NV_MAX_EFS_PATH_LEN];
  int  efs_ret;
  mcfg_fs_e_type        file_type = MCFG_FS_TYPE_EFS;
  mcfg_fs_sub_id_e_type mcfg_subs_id = MCFG_FS_SUBID_NONE;
  boolean new_indexed;
  qmi_nv_status_e_type status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (data_ptr == NULL)
  {
    status = QMI_NV_STATUS_NULL_PTR;
  }
  else if (data_size > QMI_NV_ITEM_SIZE_MAX)
  {
    status = QMI_NV_STATUS_BAD_SIZE;
  }
  else
  {
    mcfg_subs_id = ps_utils_convert_subs_ps_to_mcfg(ps_subs_id);
    status = qmi_nvi_item_build_path(item, ep_id, mux_id, &new_indexed, path, 
                                     sizeof(path));

    if (status == QMI_NV_STATUS_OK)
    {
      /* Delete the file if it exists and has a different size (mcfg_fs_write does
       * not support O_TRUNC) */
      efs_ret = mcfg_fs_stat(path, &stat_buf, file_type, mcfg_subs_id);
      if (efs_ret == MCFG_FS_STATUS_OK && stat_buf.st_size != data_size)
      {
        (void) mcfg_fs_delete(path, file_type, mcfg_subs_id);
      }
  
      /* New-style indexed items are required to have a uint8 index field 
       * as the first byte of their struct.  This index is not stored in
       * EFS, so adjust the pointer and size to skip it.  The legacy NV API
       * required this scheme to determine the index, but we have the index
       * as a discrete parameter, so it is not required but kept for
       * consistency. */
      if (new_indexed)
      {
        data_ptr = (void *) ((uint8*) data_ptr + sizeof(uint8) + sizeof(uint32));
        data_size -= (sizeof(uint32) + sizeof(uint8));
      }

      efs_ret = mcfg_fs_write(path, data_ptr, data_size, (MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR), 
                              MCFG_FS_ALLPERMS, file_type, mcfg_subs_id);
      if (efs_ret != MCFG_FS_STATUS_OK)
      {
        status = QMI_NV_STATUS_EFS_ERR;
      }
    }
    else
    {
      LOG_MSG_INFO2_3("build path failed for item=%d ep_id=%d mux_id=%d",item,ep_id,mux_id);
    }
  }

  LOG_MSG_INFO2_1("returning status=%d", status);

  return status;
} /* qmi_nv_write_ex() */


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
  )
{
  return (qmi_nvi_get_max_index(item) != 0);
} /* qmi_nv_is_indexed() */

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
    file      [in] NV file to read: qmi_nv_file_e_type
    data_ptr [out] location to store file contents
    data_size [in] size of buffer at data_ptr
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
  qmi_nv_file_e_type file,
  void *             data_ptr,
  fs_size_t          data_size,
  fs_size_t *        file_size
  )
{
  uint32 ps_subs_id = PS_SYS_DEFAULT_SUBS;
  return qmi_nv_file_read_ex(file,data_ptr,data_size,file_size,ps_subs_id);
} /* qmi_nv_file_read() */

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
    file      [in] NV file to read: qmi_nv_file_e_type
    data_ptr [out] location to store file contents
    data_size [in] size of buffer at data_ptr
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
qmi_nv_status_e_type qmi_nv_file_read_ex(
  qmi_nv_file_e_type file,
  void *             data_ptr,
  fs_size_t          data_size,
  fs_size_t *        file_size,
  uint32             ps_subs_id
  )
{
  char path[QMI_NV_MAX_EFS_PATH_LEN];
  qmi_nv_status_e_type status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (data_ptr == NULL)
  {
    status = QMI_NV_STATUS_NULL_PTR;
  }
  else
  {
    status = qmi_nvi_file_build_path(file, path, sizeof(path));

    if (status == QMI_NV_STATUS_OK)
    {
      status = qmi_nvi_file_get_contents(path, (char *) data_ptr, data_size,
                                         file_size, ps_subs_id);
    }
  }

  LOG_MSG_INFO2_1("returning status=%d", status);

  return status;
} /* qmi_nv_file_read_ex() */

/*==========================================================================

  FUNCTION qmi_nv_file_write

  DESCRIPTION
    This function writes an NV file to EFS.  If the NV file exists and has
    a different file size than data_size, it will be deleted and recreated
    with length equal to data_size.

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
  )
{
  uint32 ps_subs_id = PS_SYS_DEFAULT_SUBS;
  return qmi_nv_file_write_ex(file,data_ptr,data_size, ps_subs_id);
} /* qmi_nv_file_write() */

/*==========================================================================

  FUNCTION qmi_nv_file_write_ex

  DESCRIPTION
    This function writes an NV file to EFS.  If the NV file exists and has
    a different file size than data_size, it will be deleted and recreated
    with length equal to data_size.

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
qmi_nv_status_e_type qmi_nv_file_write_ex(
  qmi_nv_file_e_type file,
  void *             data_ptr,
  fs_size_t          data_size,
  uint32             ps_subs_id
  )
{
  struct fs_stat stat_buf;
  char path[QMI_NV_MAX_EFS_PATH_LEN];
  mcfg_fs_sub_id_e_type mcfg_subs_id = MCFG_FS_SUBID_NONE;
  mcfg_fs_e_type file_type = MCFG_FS_TYPE_EFS;

  qmi_nv_status_e_type status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (data_ptr == NULL)
  {
    status = QMI_NV_STATUS_NULL_PTR;
  }
  else
  {
    mcfg_subs_id = ps_utils_convert_subs_ps_to_mcfg(ps_subs_id);
    status = qmi_nvi_file_build_path(file, path, sizeof(path));

    if (status == QMI_NV_STATUS_OK)
    {
      /* check that the destination directory exists (can be efs_stat'ed)*/
      if (mcfg_fs_stat(QMI_NV_FILE_DIR, &stat_buf, file_type, mcfg_subs_id) != MCFG_FS_STATUS_OK)
      {
        /* if not, make the directory (recursively makes directories in path) */
        status = qmi_nvi_mkdir_p(QMI_NV_FILE_DIR);
      }
    }

    if (status == QMI_NV_STATUS_OK)
    {
      status = qmi_nvi_file_put_contents(path, (char *) data_ptr, data_size, ps_subs_id);
    }
  }

  LOG_MSG_INFO2_1("returning status=%d", status);

  return status;
} /* qmi_nv_file_write_ex() */


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
#define QMI_NV_MAX_EFS_PATH_SIZE (512)
qmi_nv_status_e_type qmi_nv_init_efs_conf_file
(
  void
)
{
  char    qmi_conf_path[] = "/nv/item_files/conf/qmi_data.conf";
  char  * path = NULL;
  int     fd;
  uint32  path_offset = 0;
  int     n, i;
  qmi_nv_item_e_type item[] = {QMI_NV_ITEM_RMNET_GLOBAL_CONFIG,
                               QMI_NV_ITEM_QMAP_CMD_RESP_TIME,
                               QMI_NV_ITEM_RMNET_AUTOCONNECT};
  mcfg_fs_e_type        file_type = MCFG_FS_TYPE_EFS;
  mcfg_fs_sub_id_e_type mcfg_subs_id = MCFG_FS_SUBID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_SYSTEM_HEAP_MEM_ALLOC(path, QMI_NV_MAX_EFS_PATH_SIZE, char*);
  if (path == NULL)
  {
    return QMI_NV_STATUS_EFS_ERR;
  }

  fd = mcfg_fopen(qmi_conf_path, MCFG_FS_O_CREAT | MCFG_FS_O_RDWR | MCFG_FS_O_TRUNC,
                  MCFG_FS_ALLPERMS, file_type, mcfg_subs_id);
  if (fd < 0)
  {
    LOG_MSG_ERROR_1("EFS qmi conf file open failed: %s", qmi_conf_path);
    PS_SYSTEM_HEAP_MEM_FREE(path);
    return QMI_NV_STATUS_EFS_ERR;
  }

  for (i = 0; i < ARR_SIZE(item); i++)
  {
    if ( (item[i] <= QMI_NVI_ITEM_START) ||
         (item[i] >= QMI_NVI_ITEM_END) )
    {
      LOG_MSG_ERROR_1("Unknown NV item %d", item[i]);
      continue;
    }

    n = snprintf(&path[path_offset],
                 QMI_NV_MAX_EFS_PATH_SIZE - path_offset,
                 "%s%s\n",
                 QMI_NV_NEW_DIR,
                 qmi_nv_item_paths[QMI_NV_ITEM_FNAME_INDEX(item[i])]); 

    if ((uint32)n >= QMI_NV_MAX_EFS_PATH_SIZE - path_offset)
    {
      LOG_MSG_ERROR_0("Insuffient buffer");
      break;
    }

    path_offset += n;
  }

  if (mcfg_fwrite(fd, path, path_offset, file_type) < 0)
  {
    LOG_MSG_ERROR_1("EFS qmi conf file write failed: %s", qmi_conf_path);
    mcfg_fclose(fd, file_type);
    PS_SYSTEM_HEAP_MEM_FREE(path);
    return QMI_NV_STATUS_EFS_ERR;
  }

  mcfg_fclose(fd, file_type);
  PS_SYSTEM_HEAP_MEM_FREE(path);

  return QMI_NV_STATUS_OK;

} /* qmi_nv_init_efs_conf_file() */


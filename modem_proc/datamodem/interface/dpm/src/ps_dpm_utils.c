/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    P S _ D P M _ UTILS . C

GENERAL DESCRIPTION
  This is the implementation of  legacy path Data Path Manager module

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dpm/src/ps_dpm_utils.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/10/13    bvd    created file.
===========================================================================*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#include "amssassert.h"

#include <stringl/stringl.h>
#include "fs_lib.h"
#include "fs_sys_types.h"
#include "fs_errno.h"
#include "ps_dpm_utils.h"
#include "dsutil.h"
#include "nv.h"
#include "ps_system_heap.h"
#include "ps_utils.h"
#include "ds_qmi_svc.h"


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  Macro defining the max integer value of a valid configuration item id + 1.
  Used for validation of item
---------------------------------------------------------------------------*/
#define PS_DPM_CFG_MAX ((int)PS_DPM_CFG_ILLEGAL)

/*---------------------------------------------------------------------------
  Data type used to hold static info about configuration item
---------------------------------------------------------------------------*/
typedef struct {
    ps_dpm_cfg_item_e  ident; /* Item identifier */
    const char        * desc;  /* Human readable description of item */
    const char        * path;  /* EFS path where the item resides */
    size_t              size;  /* size of item in bytes */
} ps_dpmi_cfg_item_info_t;

/*---------------------------------------------------------------------------
  Accessor macros to access individual fields from config item info
---------------------------------------------------------------------------*/
#define PS_DPMI_CFG_ITEM_IDENT(item_info_p) ((item_info_p)->ident)
#define PS_DPMI_CFG_ITEM_DESC(item_info_p)  ((item_info_p)->desc)
#define PS_DPMI_CFG_ITEM_PATH(item_info_p)  ((item_info_p)->path)
#define PS_DPMI_CFG_ITEM_SIZE(item_info_p)  ((item_info_p)->size)

/*---------------------------------------------------------------------------
  Prefix applied to each item path to derive the full path name of config
  items in EFS
---------------------------------------------------------------------------*/
static const char * ps_dpmi_cfg_path_prefix = "/nv/item_files/";

/*---------------------------------------------------------------------------
  Static array of configuration item info. Each configuration item must be
  added here.
---------------------------------------------------------------------------*/
static const ps_dpmi_cfg_item_info_t ps_dpmi_cfg_item_info[] = {
    { 
        PS_DPM_CFG_DISABLE_HW_BRIDGING, 
        "Disable ipa bridging",
        "modem/data/3gpp/ps/3gpp_data_disable_a2_bridge",
        sizeof(boolean)
    }  /* NV-item number: 67343 */
};

/*---------------------------------------------------------------------------
  Accessor macros to access config item from config item array
---------------------------------------------------------------------------*/
#define PS_DPMI_CFG_ITEM_INFO(item) (&ps_dpmi_cfg_item_info[(int)item])

/*---------------------------------------------------------------------------
  Constant defining the max allowed path name size for config items. The 
  max path defined above including prefix should never exceed this
---------------------------------------------------------------------------*/
#define PS_DPMI_CFG_MAX_PATH_SIZE   128

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================
                   CONFIGURATION UTILITIES
===========================================================================*/

/*---------------------------------------------------------------------------
  Macro to validate configuration item identifier
---------------------------------------------------------------------------*/
#define PS_DPMI_VERIFY_CFG_ITEM(item) \
          (((int)item < PS_DPM_CFG_MAX) ? TRUE : FALSE )

/*===========================================================================
FUNCTION PS_DPMI_CFG_GET_ITEM_INFO

DESCRIPTION
  Returns pointer to configuration item info struct.
  
PARAMETERS  
  item - configuration item identifier
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  Pointer to configuration item info, if it exists, NULL otherwise
   
SIDE EFFECTS 
  None.
===========================================================================*/
static const ps_dpmi_cfg_item_info_t * 
ps_dpmi_cfg_get_item_info(ps_dpm_cfg_item_e item)
{
    const ps_dpmi_cfg_item_info_t * info_p = NULL;

    if (PS_DPMI_VERIFY_CFG_ITEM(item)) {
        info_p = PS_DPMI_CFG_ITEM_INFO(item);
    }

    return info_p;
} /* ps_dpmi_cfg_get_item_info */

/*===========================================================================
FUNCTION PS_DPMI_CFG_GET_ITEM_PATH

DESCRIPTION
  Returns the full path in EFS where the configuration item is stored.
  
PARAMETERS  
  info_p - pointer to configuration item info struct
  path   - pointer to buffer to return the path name
  len    - size of the passed buffer in bytes
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean - TRUE if item was set successfully, FALSE otherwise.
   
SIDE EFFECTS 
  None.
===========================================================================*/
static boolean ps_dpmi_cfg_get_item_path
(
    const ps_dpmi_cfg_item_info_t * info_p, 
    char                           * path, 
    size_t                           len
)
{
    int result = -1;
    ASSERT(info_p);
    ASSERT(path);
    /* truncation is taken care in strlcat
       If strlcpy fails return False */
    if( strlcpy(path, ps_dpmi_cfg_path_prefix, len) <= 0 ) 
    {
      return FALSE;
    }
    result  = strlcat(path, PS_DPMI_CFG_ITEM_PATH(info_p), len);
    if( (result >= (int)len) || (result <= 0) )
    {
      return FALSE;
    }

    return TRUE;
} /* ps_dpmi_cfg_get_item_path */

/*===========================================================================
FUNCTION PS_DPMI_CFG_VALIDATE

DESCRIPTION
  Checks if configuration item is valid and parameters specified are correct.
  
PARAMETERS  
  item - configuration item to set/get
  val  - location of bytes to read from or write to
  len  - size of the configuration item in bytes
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  ptr to cfg item info, if item and parameters are valid, NULL otherwise.
   
SIDE EFFECTS 
  None.
===========================================================================*/
static const ps_dpmi_cfg_item_info_t * ps_dpmi_cfg_validate
(
    ps_dpm_cfg_item_e  item, 
    unsigned char     * val, 
    size_t              len
)
{ 
    const ps_dpmi_cfg_item_info_t * info_p = NULL;

    if (!val) 
    {
        LOG_MSG_ERROR_0("val ptr is NULL");
        goto err;
    }

    info_p = ps_dpmi_cfg_get_item_info(item);
    if (!info_p)
    {
        LOG_MSG_ERROR_1("Failed to get item info, item %d", (int)item);
    }

err:
    return info_p;
} /* ps_dpmi_cfg_validate */

boolean ps_dpm_cfg_set
(
    ps_dpm_cfg_item_e  item, 
    unsigned char     * val, 
    size_t              len
)
{
    int rval = -1;
    const ps_dpmi_cfg_item_info_t * info_p;
    char path[PS_DPMI_CFG_MAX_PATH_SIZE];
    mcfg_fs_sub_id_e_type  mcfg_subs_id = MCFG_FS_SUBID_NONE;
    mcfg_fs_e_type file_type = MCFG_FS_TYPE_EFS;

    info_p = ps_dpmi_cfg_validate(item, val, len);
    if (!info_p)
    {
        goto err;
    }

    if(len > PS_DPMI_CFG_ITEM_SIZE(info_p))
    {
       goto err;
    }

    if(!ps_dpmi_cfg_get_item_path(info_p, path, sizeof(path)))
    {
      LOG_MSG_INFO3_0("error in fun ps_dpm_cfg_set");
      goto err;
    }

    rval = mcfg_fs_write(path, val, len, 
                         MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR, 
                         MCFG_FS_DEFFILEMODE, file_type, mcfg_subs_id);

err:
    return ((rval != MCFG_FS_STATUS_OK) ? FALSE : TRUE);
} /* ps_dpm_cfg_set */

int ps_dpm_cfg_get
(
    ps_dpm_cfg_item_e  item, 
    unsigned char     * val, 
    size_t              len
)
{
    int rval = -1;
    const ps_dpmi_cfg_item_info_t * info_p;
    char path[PS_DPMI_CFG_MAX_PATH_SIZE];
    mcfg_fs_sub_id_e_type  mcfg_subs_id = MCFG_FS_SUBID_NONE;
    mcfg_fs_e_type file_type = MCFG_FS_TYPE_EFS;


    info_p = ps_dpmi_cfg_validate(item, val, len);
    if (!info_p)
    {
        goto err;
    }

    if(len < PS_DPMI_CFG_ITEM_SIZE(info_p))
    {   
       goto err;
    }               

    if(!ps_dpmi_cfg_get_item_path(info_p, path, sizeof(path)))
    {
      LOG_MSG_INFO2_0("err in ps_dpm_cfg_get");
      goto err;
    }

    if (MCFG_FS_STATUS_OK != mcfg_fs_read(path, val, len, file_type, mcfg_subs_id))
    {
      rval = -1;
    }
    else
    {
      rval = 0;
    }

err:
    
    return rval;
} /* ps_dpm_cfg_get */


uint32 ps_dpm_get_data_conf_buf_size 
(
  void
)
{
  return PS_DPMI_CFG_MAX_PATH_SIZE * PS_DPM_CFG_MAX;
} /* ps_dpm_get_data_conf_buf_size */


int32 ps_dpm_update_data_config_info
(
  char  *file_paths_buf,
  uint32 file_paths_buf_len
)
{
  int32 i;
  const ps_dpmi_cfg_item_info_t * info_p;
  char nv_item_file_path[PS_DPMI_CFG_MAX_PATH_SIZE];
 
  uint32 position = 0;
/*-------------------------------------------------------------------------*/

  if( file_paths_buf == NULL )
  {
    LOG_MSG_ERROR_0("Failed to allocate memory");
    return -1;
  }

  /* Initialize buffers */
  memset(nv_item_file_path, 0, PS_DPMI_CFG_MAX_PATH_SIZE);
  file_paths_buf[0] = '\0';
  /*-------------------------------------------------------------------------
    Update data_config_info.conf with the list of nv item files.
  -------------------------------------------------------------------------*/
  /* Browse through ps_dpmi_cfg_item_info and copy all items into a
     buffer passed by dsutil.  */
  for (i = 0; i < PS_DPM_CFG_MAX; i ++)
  {
    info_p = ps_dpmi_cfg_get_item_info((ps_dpm_cfg_item_e)i);
    if (!info_p)
    {
      LOG_MSG_ERROR_1(
        "ps_dpmi_utils_init: Failed to get item info, item %d", i);
      continue;
    }
        
    if(ps_dpmi_cfg_get_item_path(info_p, nv_item_file_path, 
                                       PS_DPMI_CFG_MAX_PATH_SIZE))
    {
      /* Concatenate current path in buffer using snprintf and append \n */
      position += snprintf( file_paths_buf + position,
                            file_paths_buf_len - position, 
                            "%s\n", nv_item_file_path );

      /* Check for truncation */
      if( position >= file_paths_buf_len)
      {
        LOG_MSG_ERROR_1("Fail to copy all files path in buffer file_paths_buf_len %d",
                         file_paths_buf_len);
        return -1;
      }
    }/* Only skip items which are failed don't fail init */
    else
    {
      LOG_MSG_ERROR_1("ps_dpmi_cfg_get_item_path failed for item %d skipping",
                      i);
    }
  }

  return position;
} /* ps_dpm_update_data_config_info */

/*===========================================================================
FUNCTION PS_DPM_UTILS_PATH_IS_DIRECTORY

DESCRIPTION
  To check if the EFS directory exists

DEPENDENCIES
  None

PARAMETERS
  dirname - Directory path

RETURN VALUE
   0         - success
   efs_errno - EFS error
   -1        - Other error

SIDE EFFECTS
  None
===========================================================================*/
static int32 ps_dpm_utils_path_is_directory
(
  const char *dirname
)
{
  int                           rsp;
  struct fs_stat                stat_info;
  mcfg_fs_sub_id_e_type  mcfg_subs_id = MCFG_FS_SUBID_NONE;
  mcfg_fs_e_type file_type = MCFG_FS_TYPE_EFS;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rsp = mcfg_fs_stat( dirname, &stat_info, file_type, mcfg_subs_id);
  if( MCFG_FS_STATUS_OK != rsp )
  {
    rsp = mcfg_fs_errno( file_type );
    if( ENOENT != rsp )
    {
      LOG_MSG_ERROR_1("efs_lstat error %d", rsp);
      return rsp;
    }
  }
  else if( S_ISDIR (stat_info.st_mode))
  {
    return 0;
  }

  return -1;
} /* ps_dpm_utils_path_is_directory */

/*===========================================================================
FUNCTION PS_DPM_UTILS_CREATE_EFS_CONFIG_FILE

DESCRIPTION
  Create a config file in EFS which stores the path of EFS item files.

DEPENDENCIES
  None

PARAMETERS
  conf_file_path - File path of config file

RETURN VALUE
  0         - Success
 -1         - Non-EFS related Failures
  efs_errno - EFS related failures. Meaning of this value can be
              found in fs_errno.h

SIDE EFFECTS
  None
===========================================================================*/
static int32 ps_dpm_utils_create_efs_config_file
(
  const char *conf_file_path
)
{
  int32        config_fd, result;
  mcfg_fs_sub_id_e_type  mcfg_subs_id = MCFG_FS_SUBID_NONE;
  mcfg_fs_e_type file_type = MCFG_FS_TYPE_EFS;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL == conf_file_path )
  {
    LOG_MSG_ERROR_0("conf_file_path is NULL");
    return -1;
  }
  
  /*-------------------------------------------------------------------------
    Create common directories if needed.
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO1_0("EFS: Creating conf file if necessary");
  result = ps_dpm_utils_path_is_directory("/nv");
  if( 0 != result )
  {
    /* Directory doesn't exist yet */
    LOG_MSG_ERROR_0("Create /nv dir in EFS");
    result = mcfg_mkdir( "/nv", MCFG_FS_ALLPERMS, file_type);
    if ( -1 == result )
    {
      LOG_MSG_ERROR_1("Create EFS Dir Failed: "
                      "error %d", mcfg_fs_errno( file_type ));
      return mcfg_fs_errno( file_type );
    } 
  }

  result = ps_dpm_utils_path_is_directory("/nv/item_files");
  if( 0 != result )
  {
    /* Directory doesn't exist yet */
    LOG_MSG_INFO2_0("Create /nv/item_files dir in EFS");
    result = mcfg_mkdir( "/nv/item_files",  MCFG_FS_ALLPERMS, file_type);
    if ( -1 == result )
    {
      LOG_MSG_ERROR_1("Create EFS Dir Failed: "
                      "error %d", mcfg_fs_errno( file_type ));
      return mcfg_fs_errno( file_type );
    }
  }

  result = ps_dpm_utils_path_is_directory("/nv/item_files/conf");
  if( 0 != result )
  {
    /* Directory doesn't exist yet */
    LOG_MSG_INFO2_0("Create /nv/item_file/conf dir in EFS");
    result = mcfg_mkdir( "/nv/item_files/conf",  MCFG_FS_ALLPERMS, file_type);
    if ( -1 == result )
    {
      LOG_MSG_ERROR_1("Create EFS Dir Failed: "
                      "error %d", mcfg_fs_errno( file_type ));
      return mcfg_fs_errno( file_type );
    }
  }

  /*-------------------------------------------------------------------------
    Open conf file. Create conf file if does not exist.
    Resulting file is truncated to zero bytes.
  -------------------------------------------------------------------------*/  
  config_fd = mcfg_fopen (conf_file_path, MCFG_FS_O_WRONLY|MCFG_FS_O_CREAT|MCFG_FS_O_TRUNC, 
                          MCFG_FS_ALLPERMS, file_type, mcfg_subs_id);
  if ( 0 > config_fd )
  {
    LOG_MSG_ERROR_1("Error creating config file, "
                    "error %d", mcfg_fs_errno( file_type ));
    return mcfg_fs_errno( file_type );
  }

  result = mcfg_fclose (config_fd, file_type);
  if ( 0 != result )
  {
    LOG_MSG_ERROR_1("Error closing config file, "
                    "error %d", mcfg_fs_errno( file_type ));
    return mcfg_fs_errno( file_type );
  }
  return 0;

} /* ps_dpm_utils_create_efs_config_file() */

/*===========================================================================
FUNCTION      PS_DPM_INIT_EFS_CONFIG_FILE

DESCRIPTION   Write all required item files to the config file.
              Any new item file must be added directly within this init 
              function.
              This function optimizes multiple calls for item file write from
              different modules into a single efs write call.

DEPENDENCIES  None.

RETURN VALUE  0 for success.
              -1 for failure.

SIDE EFFECTS  None.
===========================================================================*/
int32 ps_dpm_init_efs_config_file(void)
{
  char   data_conf_info_path[] = "/nv/item_files/conf/dpm_config_info.conf";
  int  config_fd, result;
  char *file_paths_buf_ptr = NULL;
  char *efs_read_buf_ptr = NULL;
  boolean conf_file_valid = TRUE;
  uint32 efs_conf_file_size;
  int bytes_read = 0, efs_ret = 0;
  struct fs_stat stat_buf;
  int ret_val = 0;
  mcfg_fs_sub_id_e_type  mcfg_subs_id = MCFG_FS_SUBID_NONE;
  mcfg_fs_e_type file_type = MCFG_FS_TYPE_EFS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Allocate memory to store the item file paths
  -------------------------------------------------------------------------*/
  efs_conf_file_size = PS_DPM_DATA_CONF_BUF_SIZE;
  PS_SYSTEM_HEAP_MEM_ALLOC(file_paths_buf_ptr, efs_conf_file_size, char*);
  if(NULL == file_paths_buf_ptr)
  {
    return -1;
  }

  memset(file_paths_buf_ptr, 0, efs_conf_file_size);

  /*-------------------------------------------------------------------------
    ps_dpm_update_data_config_info function, will always NULL terminate
    file_paths_buf_ptr. In case of success return value is the size of string
    copied to buf_ptr.
  -------------------------------------------------------------------------*/
  result = ps_dpm_update_data_config_info(file_paths_buf_ptr, efs_conf_file_size);
  if ( -1 == result || result > (int)PS_DPM_DATA_CONF_BUF_SIZE )
  {
    LOG_MSG_ERROR_0("Error storing efs item file paths in buffer");
    PS_SYSTEM_HEAP_MEM_FREE(file_paths_buf_ptr);
    return -1;
  }

  LOG_MSG_INFO1_1("file_paths_buf_str length %d", result);

  /*-------------------------------------------------------------------------
    Open conf file. If it doesn't exist create it and reopen it.
  -------------------------------------------------------------------------*/
  config_fd = mcfg_fopen (data_conf_info_path, 
                          MCFG_FS_O_RDWR| MCFG_FS_O_APPEND, 
                          MCFG_FS_ALLPERMS,
                          file_type,
                          mcfg_subs_id);
  if ( 0 > config_fd )
  {
    if ( ENOENT == mcfg_fs_errno( file_type ) ) /* Conf file does not exist */
    {
      LOG_MSG_INFO2_0("EFS: Config file not present");
      LOG_MSG_INFO2_0("EFS: Creating config file");
      conf_file_valid = FALSE;

      result = ps_dpm_utils_create_efs_config_file(data_conf_info_path);
      if ( 0 != result )
      {
        LOG_MSG_ERROR_1("EFS: Error creating config file, "
                        "error %d", mcfg_fs_errno( file_type ));
        PS_SYSTEM_HEAP_MEM_FREE(file_paths_buf_ptr);
        return mcfg_fs_errno( file_type );
      }
  
      config_fd = mcfg_fopen (data_conf_info_path, 
                              MCFG_FS_O_RDWR, 
                              MCFG_FS_ALLPERMS,
                              file_type,
                              mcfg_subs_id);
      if ( 0 > config_fd )
      {
        LOG_MSG_ERROR_1("EFS: Error opening config file, "
                        "error %d", mcfg_fs_errno( file_type ));
        PS_SYSTEM_HEAP_MEM_FREE(file_paths_buf_ptr);
        return mcfg_fs_errno( file_type );
      }
    }
    else /* Could not open conf file for some other reason */
    {
      LOG_MSG_INFO2_1("Error opening config file, "
                      "error %d", mcfg_fs_errno( file_type ));
      PS_SYSTEM_HEAP_MEM_FREE(file_paths_buf_ptr);
      return mcfg_fs_errno( file_type );
    }
  }

  /*-------------------------------------------------------------------------
    Check the file size to decide whether writing the file is required or not
  -------------------------------------------------------------------------*/
  if(conf_file_valid)
  {
    if ((MCFG_FS_STATUS_OK != mcfg_fs_stat(data_conf_info_path, &stat_buf, file_type, mcfg_subs_id)) 
        || (strlen(file_paths_buf_ptr) != stat_buf.st_size))
    {
      LOG_MSG_ERROR_2("Conf file invalid size %d, expected size %d", 
                      stat_buf.st_size, strlen(file_paths_buf_ptr));
      conf_file_valid = FALSE;
    }
  }

  /*-------------------------------------------------------------------------
    Read the efs config info file if it is valid & compare the contents read
    with the one stored in the dynamic buffer. If they don't match write the
    buffer info into the efs .conf file
  -------------------------------------------------------------------------*/
  if(conf_file_valid)
  {
    PS_SYSTEM_HEAP_MEM_ALLOC(efs_read_buf_ptr, efs_conf_file_size, char*);
    if(NULL == efs_read_buf_ptr)
    {
      (void)mcfg_fclose (config_fd, file_type);
      PS_SYSTEM_HEAP_MEM_FREE(file_paths_buf_ptr);
      return -1;
    }

    do
    {
      efs_ret = mcfg_fread(config_fd, &efs_read_buf_ptr[bytes_read],
                           (strlen(file_paths_buf_ptr) - bytes_read),
                           file_type);
    } while (efs_ret > 0 && (bytes_read += efs_ret) 
             < (int)strlen(file_paths_buf_ptr));

    if((!(bytes_read == strlen(file_paths_buf_ptr)))
       || (0 != memcmp(file_paths_buf_ptr, efs_read_buf_ptr, strlen(file_paths_buf_ptr))))
    {
      LOG_MSG_ERROR_0("memcmp failed, write into conf file again");
      conf_file_valid = FALSE;
    }
  }

  /*-------------------------------------------------------------------------
    Write the efs item file paths into .conf file if the contents are not
    valid.
  -------------------------------------------------------------------------*/
  if(FALSE == conf_file_valid)
  {
    mcfg_fs_truncate(data_conf_info_path, 0, file_type, mcfg_subs_id); //TODO:Bharath
    mcfg_lseek(config_fd, 0, SEEK_SET, file_type);

    LOG_MSG_INFO1_0("mcfg_fwrite() called");
    if(mcfg_fwrite(config_fd, file_paths_buf_ptr, 
                   (fs_size_t)strlen(file_paths_buf_ptr), 
                   file_type)
       != (fs_size_t)strlen(file_paths_buf_ptr))
    {
      LOG_MSG_ERROR_0("Failed to write efs file paths into conf file");
      ret_val = -1;
    }
   
  }

  PS_SYSTEM_HEAP_MEM_FREE(file_paths_buf_ptr);
  PS_SYSTEM_HEAP_MEM_FREE(efs_read_buf_ptr);

  /* EFS Close */
  result = mcfg_fclose (config_fd, file_type);
  if ( 0 != result )
  {
    LOG_MSG_ERROR_1("Error closing config file, "
                    "error %d", mcfg_fs_errno( file_type ));
    return mcfg_fs_errno( file_type );
  }
  
  return ret_val;
}/* ps_dpm_init_efs_config_file() */

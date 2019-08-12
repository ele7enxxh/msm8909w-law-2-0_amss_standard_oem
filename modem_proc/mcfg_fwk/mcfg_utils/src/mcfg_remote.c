/*==============================================================================

                M O D E M   C O N F I G   E F S
 
GENERAL DESCRIPTION
  EFS library for MCFG storage

Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_utils/src/mcfg_remote.c#1 $ 
$DateTime: 2016/12/13 08:00:27 $ 
$Author: mplcsds1 $ 
$Change: 11985219 $ 

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
05/08/12 cy   Initial creation

==============================================================================*/


#include "comdef.h"

#include "mcfg_osal.h"


#include "mcfg_common.h"
#include "mcfg_utils.h"
#include "mcfg_utils_i.h"
#include "mcfg_auth_i.h"

#include "mcfg_map_i.h"
#include "mcfg_efs_i.h"
#include "mcfg_remote_i.h"

#include "fs_estdio.h"
#ifdef FEATURE_MCFG_REMOTE_STORAGE
#include "rfs_api.h"
#include "mcfg_trl.h"
#include "mcfg_proc.h"

/*==============================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/
//#define DLFS_RFSA_FMT "MPSS:/%s"

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/
  
/*==============================================================================

                     INTERNAL FUNCTION DECLARATIONS

==============================================================================*/
static int mcfg_remote_rfsa_open(const char* filename, unsigned int  fileMode) 
{
   return rfs_open(filename, fileMode, 0);
}

static boolean mcfg_remote_rfsa_read(int fd, uint32 offset, uint32 size, uint8* data)
{
  int rfs_ret;
  fs_size_t total_bytes_read = 0;

  if (data != NULL)
  {
    rfs_ret = rfs_seek(fd, offset, RFS_SEEK_SET);
    MCFG_MSG_LOW_1("rfs_seek ret %d", rfs_ret);
    if (rfs_ret == offset)
    {
      do
      {
        rfs_ret = rfs_read(fd, (char *)(data + total_bytes_read), (size - total_bytes_read));
        MCFG_MSG_LOW_1("rfs_read ret %d", rfs_ret);
      } while (rfs_ret > 0 && (total_bytes_read += rfs_ret) < size);
    }
  }
  
  if(total_bytes_read != size)
  {
    MCFG_MSG_ERROR_2("mcfg_remote_rfsa_read %lu but end up read %lu", size, total_bytes_read);
    return FALSE;
  }

  return TRUE;
} /* mcfg_remote_rfsa_read() */

/*==============================================================================
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

  FUNCTION mcfg_remote_add_config

  DESCRIPTION
    This function is used to add a new remote config file on the device.  

  DEPENDENCIES
    None

  PARAMETERS
    type   [in]  type of the new config
    id     [in]  ID of the new config
    config [in/out] config structure including config size and buffer pointer

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    Config file is stored into device

===========================================================================*/
mcfg_error_e_type mcfg_remote_add_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_config_s_type     *config
)
{
  char path[MCFG_UTILS_RECORD_FILEPATH_MAX];
  char path_info[MCFG_UTILS_RECORD_FILEPATH_MAX+4];
  mcfg_error_e_type error = MCFG_ERR_NONE;
  mcfg_map_record_s_type record;
  mcfg_config_s_type remote_config;
  mcfg_config_info_s_type info;
  mcfg_trl_config_version_s_type version;
  struct rfs_stat_buf buf;
  int fd = -1 ;

  MCFG_CHECK_NULL_PTR_RET_ERROR(id, MCFG_ERR_NULL_POINTER);
  MCFG_CHECK_NULL_PTR_RET_ERROR(config, MCFG_ERR_NULL_POINTER);
  MCFG_ASSERT_COND_RET_ERROR(type < MCFG_NUM_OF_TYPES, MCFG_ERR_NOT_SUPPORTED);

  memset(&info, 0, sizeof(info));
  memset(&remote_config, 0, sizeof(remote_config));
  memset(&version, 0, sizeof(version));

  if(rfs_stat(config->config_addr, &buf) != 0)
  {
    MCFG_MSG_ERROR("mcfg_remote_add_config: Failed to stat");
    return MCFG_ERR_ACCESS_DENIED;
  }

  fd = mcfg_remote_rfsa_open(config->config_addr, RFS_O_RDONLY);
  if (fd < 0)
  {
    MCFG_MSG_ERROR("mcfg_remote_add_config: Failed to open");
    return MCFG_ERR_ACCESS_DENIED;
  }
  
  remote_config.config_len = buf.st_size;
  remote_config.config_addr = NULL;

  do
  {
    if (remote_config.config_len < 0 || 
        remote_config.config_len > MCFG_CONFIG_SIZE_MAX)
    {
      MCFG_MSG_ERROR_1("mcfg_remote_add_config: invalid flen %d", remote_config.config_len);
      error = MCFG_ERR_ACCESS_DENIED;
      break;
    }

    if(!mcfg_utils_alloc_config_buffer(type, &remote_config))
    {
      MCFG_MSG_ERROR_1("mcfg_remote_add_config: failed to malloc size %d", 
                       remote_config.config_len);
      error = MCFG_ERR_INTERNAL;
      break;
    }

    if (!mcfg_remote_rfsa_read(fd, 0, remote_config.config_len, remote_config.config_addr))
    {
      MCFG_MSG_ERROR_1("mcfg_remote_add_config: can't read len %d", remote_config.config_len);
      error = MCFG_ERR_ACCESS_DENIED;
      break;
    }

    /* Authentication */
    error = mcfg_auth_check_config(&remote_config);
    if (error != MCFG_ERR_NONE)
    {
      break;
    }

    /* Save info as file ***.ci */
    if (!mcfg_utils_parse_config_info(&remote_config, &info))
    {
      MCFG_MSG_ERROR("mcfg_remote_add_config: Failed to get config info");
      error = MCFG_ERR_INCOMPATIBLE;
      break;
    }

    if(!mcfg_trl_decode_tlv(&info, MCFG_TRL_TLV_ID_CONFIG_VERSION,
                             &version, sizeof(version)))
    {
      MCFG_MSG_ERROR("trl_decode_tlv:config version not found");
      error = MCFG_ERR_INCOMPATIBLE;
      break;
    }

    if(mcfg_compat_check(type, (uint32 *)&version) == MCFG_COMPAT_FAIL)
    {
      MCFG_MSG_ERROR("config version not compatible");
      error = MCFG_ERR_INCOMPATIBLE;
      break;
    }
    /* Save config as file */
    if (!mcfg_efs_get_config_path(type, id, path))
    {
      MCFG_MSG_ERROR("mcfg_remote_add_config: Failed to get Path");
      error = MCFG_ERR_INTERNAL;
      break;
    }

    if (!mcfg_efs_store_buf_in_path(config->config_addr, config->config_len, path))
    {
      MCFG_MSG_ERROR("mcfg_remote_add_config: Failed to store config");
      error = MCFG_ERR_INTERNAL;
      break;
    }

    info.storage = MCFG_STORAGE_REMOTE;
    if (config->config_len < MCFG_CONFIG_PATH_SIZE_MAX)
    {
      memscpy(info.path, MCFG_CONFIG_PATH_SIZE_MAX-1, 
              config->config_addr, config->config_len);
    }
    else
    {
      memscpy(info.path, MCFG_CONFIG_PATH_SIZE_MAX-1, 
              (uint8 *)config->config_addr+config->config_len-MCFG_CONFIG_PATH_SIZE_MAX+1,
              MCFG_CONFIG_PATH_SIZE_MAX-1);
    }

    if (snprintf(path_info, MCFG_EFS_RECORD_FILEPATH_MAX+3, "%s.ci", path) < 0)
    {
      MCFG_MSG_ERROR("mcfg_remote_add_config: Failed to get config info path");
      error = MCFG_ERR_INTERNAL;
      break;
    }

    if (!mcfg_efs_store_buf_in_path(&info, sizeof(info), path_info))
    {
      MCFG_MSG_ERROR("mcfg_remote_add_config: Failed to store config info");
      error = MCFG_ERR_INTERNAL;
      break;
    }

    /*add record to map */
    if(!mcfg_map_init_record_by_id(&record, id))
    {
      MCFG_MSG_ERROR("mcfg_remote_add_config: Failed to init record");
      error = MCFG_ERR_INTERNAL;
      break;
    }

    record.storage = MCFG_STORAGE_REMOTE;
    record.size = remote_config.config_len;
    if (!mcfg_map_add_record(type, &record))
    {
      MCFG_MSG_ERROR("mcfg_remote_add_config: Failed to add record to map");
      error = MCFG_ERR_INTERNAL;
      break;
    }

    if(!mcfg_map_store_config_map(type))
    {
      MCFG_MSG_ERROR("mcfg_remote_add_config: Failed to store map");
      return MCFG_ERR_INTERNAL;
    }

    MCFG_MSG_LOW("mcfg_remote_add_config success");

  }while(0);

  if (fd >= 0)
  {
    rfs_close(fd);
  }

  if (remote_config.config_addr)
  {
    mcfg_utils_free_config_buffer(&remote_config);
  }

  return error;
} /* mcfg_remote_add_config() */

/*===========================================================================

  FUNCTION mcfg_remote_get_config

  DESCRIPTION
    This function is used to load a config file from remote into
    the config buffer in memory.

  DEPENDENCIES
    None

  PARAMETERS
    type   [in]  type of the new config
    id     [in]  ID of the new config
    config [in/out] config structure including config size and buffer pointer

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    Config file is loaded into memory

===========================================================================*/
boolean mcfg_remote_get_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_config_s_type     *config
)
{
  int fd = -1;
  mcfg_config_s_type path_config;
  mcfg_error_e_type error = MCFG_ERR_NONE;

  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_CHECK_NULL_PTR_RET_FALSE(config);
  MCFG_CHECK_NULL_PTR_RET_FALSE(config->config_addr);
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);

  memset(&path_config, 0, sizeof(path_config));

  do
  {
    if (!mcfg_efs_get_config_size(type, id, &path_config.config_len))
    {
      MCFG_MSG_ERROR("mcfg_remote_get_config: Failed to get size");
      error = MCFG_ERR_INTERNAL;
      break;
    }

    if (path_config.config_len < 0 || 
        path_config.config_len > MCFG_CONFIG_SIZE_MAX)
    {
      MCFG_MSG_ERROR_1("mcfg_remote_get_config: invalid config_len %d", path_config.config_len);
      error = MCFG_ERR_INTERNAL;
      break;
    }

    path_config.config_len++; 
    if(!mcfg_utils_alloc_config_buffer(type, &path_config))
    {
      MCFG_MSG_ERROR_1("mcfg_remote_get_config: failed to malloc size %d", 
                       path_config.config_len);
      error = MCFG_ERR_INTERNAL;
      break;
    }

    path_config.config_len--; 
    if (!mcfg_efs_get_config(type, id, &path_config))
    {
      MCFG_MSG_ERROR("mcfg_remote_get_config: Failed to get path");
      error = MCFG_ERR_INTERNAL;
      break;
    }

    fd = mcfg_remote_rfsa_open(path_config.config_addr, RFS_O_RDONLY);
    if (fd < 0)
    {
      MCFG_MSG_ERROR("mcfg_remote_get_config: Failed to open");
      MCFG_MSG_SPRINTF_1("remote file %s", path_config.config_addr);
      error = MCFG_ERR_INTERNAL;
      break;
    }

    if (!mcfg_remote_rfsa_read(fd, 0, config->config_len, config->config_addr))
    {
      MCFG_MSG_ERROR_1("mcfg_remote_get_config: read failed %d", config->config_len);
      error = MCFG_ERR_ACCESS_DENIED;
      break;
    }

    MCFG_MSG_LOW("mcfg_remote_get_config success");
  }while(0);

  if (fd >= 0)
  {
    rfs_close(fd);
  }

  if (path_config.config_addr)
  {
    mcfg_utils_free_config_buffer(&path_config);
  }

  return (error == MCFG_ERR_NONE);
} /* mcfg_remote_get_config() */

#endif /* FEATURE_MCFG_REMOTE_STORAGE */


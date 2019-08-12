/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             3G  DATA  SERVICES  CONFIGURATION  MANAGER

GENERAL DESCRIPTION
  This software unit manages the configuration for DS. It provides APIs for
  nv/efs configuration related properties.

Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3gcfgmgr.c#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/11/14   vm      Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "mcfg_fs.h"

#include <stringl/stringl.h>

#include "data_msg.h"
#include "ds3gcfgmgr.h"
#include "dstaski.h"
#include "ds_system_heap.h"
#include "dsutil.h"
#include "ds3gutil.h"
#include "ds_3gpp_bearer_context.h"
#include "ds_3gppi_utils.h"
#include "ds707_pkt_mgr.h"
#include "ds_wlan_util.h"
#include "ds_appsrv_utils.h"

/*===========================================================================

                        LOCAL DATA STRUCTURES

===========================================================================*/
#define DS3GCFGMGR_DATA_CONFIG_PATH "/nv/item_files/conf/data_config_info.conf"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Data services task NV command item.
---------------------------------------------------------------------------*/
static nv_cmd_type  ds3gcfgmgr_nv_command;

/*---------------------------------------------------------------------------
  Extended NV command struct for dual sim
---------------------------------------------------------------------------*/
static nv_cmd_ext_type ds3gcfgmgr_nv_command_ext;


/*===========================================================================

                    INTERNAL FUNCTIONS

===========================================================================*/


/*===========================================================================
FUNCTION      DS_CREATE_EFS_CONFIG_FILE

DESCRIPTION   Create a config file in EFS which stores the path of
              EFS item files.

DEPENDENCIES  None

RETURN VALUE  0          - Success
              -1         - Non-EFS related Failures
              efs_errno  - EFS related failures. Meaning of this value can be
                           found in fs_errno.h

SIDE EFFECTS  None
===========================================================================*/
int32 ds3gcfgmgr_create_efs_config_file
(
  const char *conf_file_path
)
{
  int32  config_fd, result, mcfg_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL == conf_file_path )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "conf_file_path is NULL");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Create common directories if needed.
  -------------------------------------------------------------------------*/
  DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "EFS: Creating conf file if necessary");
  result = ds_path_is_directory("/nv");
  if( 0 != result )
  {
    /* Directory doesn't exist yet */
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Create /nv dir in EFS");
    result = mcfg_mkdir( "/nv", S_IREAD|S_IWRITE|S_IEXEC, MCFG_FS_TYPE_EFS);
    if ( -1 == result )
    {
      mcfg_errno = mcfg_fs_errno(MCFG_FS_TYPE_EFS);
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "Create EFS Dir Failed: error %d", mcfg_errno);
      return mcfg_errno;
    }
  }

  result = ds_path_is_directory("/nv/item_files");
  if( 0 != result )
  {
    /* Directory doesn't exist yet */
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Create /nv/item_files dir in EFS");
    result = mcfg_mkdir( "/nv/item_files",
                         S_IREAD|S_IWRITE|S_IEXEC,
                         MCFG_FS_TYPE_EFS);
    if ( -1 == result )
    {
      mcfg_errno = mcfg_fs_errno(MCFG_FS_TYPE_EFS);
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "Create EFS Dir Failed: error %d", mcfg_errno);
      return mcfg_errno;
    }
  }

  result = ds_path_is_directory("/nv/item_files/conf");
  if( 0 != result )
  {
    /* Directory doesn't exist yet */
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Create /nv/item_file/conf dir in EFS");
    result = mcfg_mkdir( "/nv/item_files/conf",
                         S_IREAD|S_IWRITE|S_IEXEC,
                         MCFG_FS_TYPE_EFS);
    if ( -1 == result )
    {
      mcfg_errno = mcfg_fs_errno(MCFG_FS_TYPE_EFS);
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "Create EFS Dir Failed: error %d", mcfg_errno);
      return mcfg_errno;
    }
  }

  /*-------------------------------------------------------------------------
    Open conf file. Create conf file if does not exist.
    Resulting file is truncated to zero bytes.
  -------------------------------------------------------------------------*/
  config_fd = mcfg_fopen( conf_file_path,
                          MCFG_FS_O_WRONLY | MCFG_FS_O_CREAT | MCFG_FS_O_TRUNC,
                          MCFG_FS_ALLPERMS,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE );
  if ( 0 > config_fd )
  {
    mcfg_errno = mcfg_fs_errno(MCFG_FS_TYPE_EFS);
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Error creating config file, error %d", mcfg_errno);
    return mcfg_errno;
  }

  result = mcfg_fclose(config_fd, MCFG_FS_TYPE_EFS);
  if ( 0 != result )
  {
    mcfg_errno = mcfg_fs_errno(MCFG_FS_TYPE_EFS);
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Error closing config file, error %d", mcfg_errno);
    return mcfg_errno;
  }
  return 0;
} /* ds3gcfgmgr_create_efs_config_file() */

/*===========================================================================
FUNCTION      DS3GCFGMGR_INIT_EFS_DATA_CONFIG_FILE

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
int32 ds3gcfgmgr_init_efs_data_config_file( void )
{
  char            data_conf_info_path[] = DS3GCFGMGR_DATA_CONFIG_PATH;
  int             config_fd, result, mcfg_errno;
  char           *file_paths_buf_ptr = NULL;
  char           *efs_read_buf_ptr = NULL;
  boolean         conf_file_valid = TRUE;
  uint32          efs_conf_file_size;
  int             bytes_read = 0, efs_ret = 0;
  struct fs_stat  stat_buf;
  int             ret_val = 0;
  uint32          next_file_paths_buf_write_pos = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Allocate memory to store the item file paths from 3gpp, 3gpp2, MPPM and
    DS3G
  -------------------------------------------------------------------------*/
  efs_conf_file_size =
#ifdef FEATURE_DATA_WLAN_MAPCON
    DS_WLAN_DATA_CONF_BUF_SIZE                     +
#endif  /* FEATURE_DATA_WLAN_MAPCON */
#ifdef FEATURE_DATA_IS707
    ds707_pkt_mgr_3gpp2_efs_item_file_path_size()  +
#endif  /* FEATURE_DATA_IS707 */
    DS_3GPP_DATA_CONF_BUF_SIZE                     +
    ds3g_get_efs_file_path_size()                  +
    ds_appsrv_efs_item_file_path_size();

  file_paths_buf_ptr = (char*)ds_system_heap_mem_alloc(efs_conf_file_size);
  if(NULL == file_paths_buf_ptr)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Failed to allocate memory");
    return -1;
  }
  memset(file_paths_buf_ptr, 0, efs_conf_file_size);

  /*-------------------------------------------------------------------------
    3GPP specific items
    ds_3gpp_update_data_config_info function, will always NULL terminate
    file_paths_buf_ptr. In case of success return value is the size of string
    copied to buf_ptr.
  -------------------------------------------------------------------------*/
  result = ds_3gpp_update_data_config_info(file_paths_buf_ptr,
                                           efs_conf_file_size);

  if ( (-1 == result) || ((uint32)result > DS_3GPP_DATA_CONF_BUF_SIZE) )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Error storing efs item file paths in buffer");
    ds_system_heap_mem_free(file_paths_buf_ptr);
    return -1;
  }
  next_file_paths_buf_write_pos += result;

  /*-------------------------------------------------------------------------
    DS3G common items
    ds3g_update_data_config_info function, will always NULL terminate
    file_paths_buf_ptr. In case of success return value is the size of string
    copied to buf_ptr.
  -------------------------------------------------------------------------*/
  result = ds3g_update_data_config_info(file_paths_buf_ptr +
                                          next_file_paths_buf_write_pos,
                                        efs_conf_file_size);

  if ( (-1 == result) ||
       ( (uint32)result >
         (ds3g_get_efs_file_path_size()+ DS_3GPP_DATA_CONF_BUF_SIZE)
       )
     )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Error storing efs item file paths in buffer");
    ds_system_heap_mem_free(file_paths_buf_ptr);
    return -1;
  }
  next_file_paths_buf_write_pos += result;

  /*-------------------------------------------------------------------------
    Any additional items that need to be added to the buffer need to
    increment next_file_paths_buf_write_pos in order to keep track of a
    running total of the string length that has been written to the buffer
  -------------------------------------------------------------------------*/

#ifdef FEATURE_DATA_WLAN_MAPCON
  /*-------------------------------------------------------------------------
    WLAN specific items
    ds_wlan_update_data_config_info function, will always NULL terminate
    file_paths_buf_ptr. In case of success return value is the size of string
    copied to buf_ptr.
  -------------------------------------------------------------------------*/
  result = ds_wlan_update_data_config_info(file_paths_buf_ptr +
                                             next_file_paths_buf_write_pos);

  if ( -1 == result || (uint32)result > DS_WLAN_DATA_CONF_BUF_SIZE )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Error storing WLAN efs item file paths in buffer");
    ds_system_heap_mem_free(file_paths_buf_ptr);
    return -1;
  }
  next_file_paths_buf_write_pos += result;
#endif  /* FEATURE_DATA_WLAN_MAPCON */

  /*-------------------------------------------------------------------------
    APPSRV specific items
    ds_appsrv_update_data_config_info function, will always NULL terminate
    file_paths_buf_ptr. In case of success return value is the size of string
    copied to buf_ptr.
  -------------------------------------------------------------------------*/
  result = ds_appsrv_update_data_config_info(file_paths_buf_ptr +
                                             next_file_paths_buf_write_pos);

  if ( (result == -1) ||
       ((uint32)result > ds_appsrv_efs_item_file_path_size()) )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Error storing APPSRV efs item file paths in buffer");
    ds_system_heap_mem_free(file_paths_buf_ptr);
    return -1;
  }
  next_file_paths_buf_write_pos += result;

#ifdef FEATURE_DATA_IS707
  /*-------------------------------------------------------------------------
    3GPP2 specific items
    ds_3gpp2_update_data_config_info needs to take in an offset (result)
    that is the current length of the string written into the buffer and
    added it to file_paths_buf_ptr so it writes to the correct position
  -------------------------------------------------------------------------*/
  result = ds_3gpp2_update_data_config_info(file_paths_buf_ptr +
                                              next_file_paths_buf_write_pos);
  if ( 0 != result )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "Error storing efs item file paths in buffer");
    ds_system_heap_mem_free(file_paths_buf_ptr);
    return -1;
  }
  next_file_paths_buf_write_pos += result;
#endif  /* FEATURE_DATA_IS707 */

  /*-------------------------------------------------------------------------
    Open conf file. If it doesn't exist create it and reopen it.
  -------------------------------------------------------------------------*/
  config_fd = mcfg_fopen (data_conf_info_path,
                          MCFG_FS_O_RDWR,
                          MCFG_FS_ALLPERMS,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE);
  if ( 0 > config_fd )
  {
    mcfg_errno = mcfg_fs_errno(MCFG_FS_TYPE_EFS);
    if ( ENOENT == mcfg_errno ) /* Conf file does not exist */
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "EFS: Config file not present");
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "EFS: Creating config file");
      conf_file_valid = FALSE;

      result = ds3gcfgmgr_create_efs_config_file( data_conf_info_path );
      if ( 0 != result )
      {
        mcfg_errno = mcfg_fs_errno(MCFG_FS_TYPE_EFS);
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "EFS: Error creating config file, error %d",
                        mcfg_errno);
        ds_system_heap_mem_free(file_paths_buf_ptr);
        return mcfg_errno;
      }

      config_fd = mcfg_fopen(data_conf_info_path,
                             MCFG_FS_O_RDWR | MCFG_FS_O_APPEND,
                             MCFG_FS_ALLPERMS,
                             MCFG_FS_TYPE_EFS,
                             MCFG_FS_SUBID_NONE);
      if ( 0 > config_fd )
      {
        mcfg_errno = mcfg_fs_errno(MCFG_FS_TYPE_EFS);
        DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                        "EFS: Error opening config file, error %d",
                        mcfg_errno);
        ds_system_heap_mem_free(file_paths_buf_ptr);
        return mcfg_errno;
      }
    }
    else /* Could not open conf file for some other reason */
    {
      mcfg_errno = mcfg_fs_errno(MCFG_FS_TYPE_EFS);
      DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                      "Error opening config file, error %d", mcfg_errno);
      ds_system_heap_mem_free(file_paths_buf_ptr);
      return mcfg_errno;
    }
  }

  /*-------------------------------------------------------------------------
    Check the file size to decide whether writing the file is required or not
  -------------------------------------------------------------------------*/
  if(conf_file_valid)
  {
    if( (mcfg_fstat(config_fd, &stat_buf, MCFG_FS_TYPE_EFS)
           != MCFG_FS_STATUS_OK )
        ||
        (stat_buf.st_size != strlen(file_paths_buf_ptr)) )
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                      "Conf file invalid size %d, expected size %d",
                      stat_buf.st_size, strlen(file_paths_buf_ptr));
      conf_file_valid = FALSE;
    }
  }

  /*-------------------------------------------------------------------------
    Read the efs config info file if it is valid & compare the contents read
    with the one stored in the dynamic buffer. If they don't match write the
    buffer info into the efs conf file
  -------------------------------------------------------------------------*/
  if(conf_file_valid)
  {
    efs_read_buf_ptr = (char*)ds_system_heap_mem_alloc(efs_conf_file_size);
    if(NULL == efs_read_buf_ptr)
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Failed to allocate memory");
      (void)mcfg_fclose(config_fd, MCFG_FS_TYPE_EFS);
      ds_system_heap_mem_free(file_paths_buf_ptr);
      return -1;
    }

    do
    {
      efs_ret = mcfg_fread(config_fd,
                         &efs_read_buf_ptr[bytes_read],
                           (strlen(file_paths_buf_ptr) - bytes_read),
                           MCFG_FS_TYPE_EFS);
    } while (efs_ret > 0 &&
             ((bytes_read += efs_ret) < (int)strlen(file_paths_buf_ptr))
            );

    if( (!(bytes_read == strlen(file_paths_buf_ptr))) ||
        (0 != memcmp(file_paths_buf_ptr,
                     efs_read_buf_ptr,
                     strlen(file_paths_buf_ptr))
        )
      )
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "memcmp failed, write into conf file again");
      conf_file_valid = FALSE;
    }
  }

  /*-------------------------------------------------------------------------
    Write the efs item file paths into conf file if the contents are not
    valid.
  -------------------------------------------------------------------------*/
  if(FALSE == conf_file_valid)
  {
    mcfg_ftruncate(config_fd, 0, MCFG_FS_TYPE_EFS);
    mcfg_lseek(config_fd,0,SEEK_SET, MCFG_FS_TYPE_EFS);

    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "efs_write() called");

    if( (int)strlen(file_paths_buf_ptr) !=
          mcfg_fwrite(config_fd,
                    file_paths_buf_ptr,
                      (uint32)strlen(file_paths_buf_ptr),
                      MCFG_FS_TYPE_EFS)
      )
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                      "Failed to write efs file paths into conf file");
      ret_val = -1;
    }
  }

  ds_system_heap_mem_free(file_paths_buf_ptr);
  ds_system_heap_mem_free(efs_read_buf_ptr);

  /* EFS Close */
  result = mcfg_fclose (config_fd, MCFG_FS_TYPE_EFS);
  if ( 0 != result )
  {
    mcfg_errno = mcfg_fs_errno(MCFG_FS_TYPE_EFS);
    DATA_3GMGR_MSG1(MSG_LEGACY_LOW,
                    "Error closing config file, error %d", mcfg_errno);
    return mcfg_errno;
  }

  return ret_val;
}/* ds3gcfgmgr_init_efs_data_config_file() */

/*===========================================================================
FUNCTION      DS3GCFGMGR_EXTRACT_FILE_EXT

DESCRIPTION   This function extracts the file extension and also returns the
              position of '.' in a file name

DEPENDENCIES  None.

RETURN VALUE  uint32: Postion of '.' in a file name

SIDE EFFECTS  None.
===========================================================================*/
int32 ds3gcfgmgr_extract_file_ext
(
  char                             *file_name,
  char                             *file_ext
)
{
  char                             *tmp_file_name = file_name;
  int32                             dot_position = -1;
  uint32                            ext_index = 0;
  uint32                            file_index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( file_name == NULL || file_ext == NULL )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "file name or file type is NULL");
    return dot_position;
  }

  /*-------------------------------------------------------------------------
    Extract the Dot position by traversing the string back-wards
  -------------------------------------------------------------------------*/
  tmp_file_name += (strlen((const char*)file_name)-1);

  while(tmp_file_name != file_name)
  {
    if((*tmp_file_name) == '.')
    {
      dot_position = (tmp_file_name - file_name);
      break;
    }
    tmp_file_name--;
  }

  if((dot_position != -1) &&
     ((strlen((const char *)file_name) - dot_position) > (9)))
  {
    DATA_3GMGR_MSG_SPRINTF_1( MSG_LEGACY_LOW,
                              "File Extension: %s exceeding bounds", file_name);
    dot_position = -1;
    return dot_position;
  }
  /*-------------------------------------------------------------------------
    Store the file extension along with a dot '.' so that we can directly
    append
  -------------------------------------------------------------------------*/
  if(tmp_file_name == file_name)
  {
    DATA_3GMGR_MSG_SPRINTF_1( MSG_LEGACY_LOW,
                              "no extension for the file :%s ", file_name);
  }
  else
  {
    if(file_ext != NULL)
    {
      file_ext[ext_index++] = '.';
      for(file_index = (dot_position+1);
          file_index < strlen((const char*)file_name); file_index ++)
      {
        file_ext[ext_index++] = file_name[file_index];
      }
      file_ext[ext_index] = '\0';
    }
  }

  return dot_position;
} /* ds3gcfgmgr_extract_file_ext() */

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GCFGMGR_INIT

DESCRIPTION   This function initializes the DS3G Cfg Mgr module. This would
              be invoked from ds3gi_cm_init() which is the initialization
              routine for DSMgr.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds3gcfgmgr_init( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    initialize EFS data config file
  -------------------------------------------------------------------------*/
  ds3gcfgmgr_init_efs_data_config_file();

  return;
} /* ds3gcfgmgr_init() */

/*===========================================================================
FUNCTION      DS3GCFGMGR_READ_LEGACY_NV

DESCRIPTION   This function reads legacy NV item for Subscription ID 1.
              This API can be used to read device specific legacy NV.

DEPENDENCIES  NV task is expected to be available.  Must run in the
              DS task context.

RETURN VALUE  NV return code.

SIDE EFFECTS  None.
===========================================================================*/
nv_stat_enum_type ds3gcfgmgr_read_legacy_nv
(
  nv_items_enum_type   item,
  nv_item_type        *data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds3gcfgmgr_read_legacy_nv_ex(item, data_ptr, SYS_MODEM_AS_ID_1);
} /* ds3gcfgmgr_read_legacy_nv() */

/*===========================================================================
FUNCTION      DS3GCFGMGR_READ_LEGACY_NV_EX

DESCRIPTION   This function reads legacy NV item per subscription.

DEPENDENCIES  NV task is expected to be available.  Must run in the
              DS task context.

RETURN VALUE  NV return code.

SIDE EFFECTS  None.
===========================================================================*/
nv_stat_enum_type ds3gcfgmgr_read_legacy_nv_ex
(
  nv_items_enum_type      item,
  nv_item_type           *data_ptr,
  sys_modem_as_id_e_type  subs_id
)
{
  nv_stat_enum_type  ret_val = NV_FAIL_S;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ds3gsubsmgr_is_subs_id_valid(subs_id) == FALSE )
  {
    return NV_FAIL_S;
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                  "ds3gcfgmgr_read_legacy_nv_ex item:%d subs id:%d",
                  item, subs_id);

#ifdef TEST_FRAMEWORK
  #error code not present
#else
  ASSERT(IS_IN_DS_TASK());
  ds3gcfgmgr_nv_command.tcb_ptr = rex_self();
  ds3gcfgmgr_nv_command.sigs = DS_NV_CMD_SIG;
#endif
  ds3gcfgmgr_nv_command.done_q_ptr = NULL;
  ds3gcfgmgr_nv_command.item = item;
  ds3gcfgmgr_nv_command.cmd = NV_READ_F;

  /* Set up NV so that it will read the data into the correct location */
  ds3gcfgmgr_nv_command.data_ptr    = data_ptr;
  ds3gcfgmgr_nv_command_ext.nvcmd   = &ds3gcfgmgr_nv_command;
  ds3gcfgmgr_nv_command_ext.context = (uint16) subs_id;

  /* Clear the return signal, call NV, and wait for a response */
#ifndef TEST_FRAMEWORK
  (void) rex_clr_sigs( rex_self(), DS_NV_CMD_SIG );
#endif
  nv_cmd_ext( &ds3gcfgmgr_nv_command_ext );
#ifndef TEST_FRAMEWORK
  (void) dsi_wait( DS_NV_CMD_SIG );
#endif

  /*-------------------------------------------------------
    Report error on return code.
  ---------------------------------------------------------*/
  if(( ds3gcfgmgr_nv_command.status != NV_DONE_S ) &&
     ( ds3gcfgmgr_nv_command.status != NV_NOTACTIVE_S))
  {
    DATA_MSG2(MSG_LEGACY_ERROR,"NV Read Failed Item %d Code %d",
              ds3gcfgmgr_nv_command.item, ds3gcfgmgr_nv_command.status);
  }

  ret_val = ds3gcfgmgr_nv_command.status;

  return ret_val;
} /* ds3gcfgmgr_read_legacy_nv_ex() */

/*===========================================================================
FUNCTION      DS3GCFGMGR_WRITE_LEGACY_NV

DESCRIPTION   This function writes legacy NV item for Subscription ID 1.
              This API can be used to write device specific legacy NV.

DEPENDENCIES  NV task is expected to be available.  Must run in the
              DS task context.

RETURN VALUE  NV return code.

SIDE EFFECTS  None.
===========================================================================*/
nv_stat_enum_type ds3gcfgmgr_write_legacy_nv
(
  nv_items_enum_type   item,
  nv_item_type        *data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds3gcfgmgr_write_legacy_nv_ex(item, data_ptr, SYS_MODEM_AS_ID_1);
} /* ds3gcfgmgr_write_legacy_nv() */

/*===========================================================================
FUNCTION      DS3GCFGMGR_WRITE_LEGACY_NV_EX

DESCRIPTION   This function writes legacy NV item per subscription.

DEPENDENCIES  NV task is expected to be available.  Must run in the
              DS task context.

RETURN VALUE  NV return code.

SIDE EFFECTS  None.
===========================================================================*/
nv_stat_enum_type ds3gcfgmgr_write_legacy_nv_ex
(
  nv_items_enum_type      item,
  nv_item_type           *data_ptr,
  sys_modem_as_id_e_type  subs_id
)
{
  nv_stat_enum_type  ret_val = NV_FAIL_S;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ds3gsubsmgr_is_subs_id_valid(subs_id) == FALSE)
  {
    return NV_FAIL_S;
  }

  DATA_3GMGR_MSG2(MSG_LEGACY_LOW,
                  "ds3gcfgmgr_write_legacy_nv_ex item:%d subs id:%d",
                  item, subs_id);

#ifdef TEST_FRAMEWORK
  #error code not present
#else
  ASSERT(IS_IN_DS_TASK());
  ds3gcfgmgr_nv_command.tcb_ptr = rex_self();
  ds3gcfgmgr_nv_command.sigs = DS_NV_CMD_SIG;
#endif
  ds3gcfgmgr_nv_command.done_q_ptr = NULL;

  ds3gcfgmgr_nv_command.item = item;
  ds3gcfgmgr_nv_command.cmd = NV_WRITE_F;

  /* Set up NV so that it will write the data into the correct location */
  ds3gcfgmgr_nv_command.data_ptr    = data_ptr;
  ds3gcfgmgr_nv_command_ext.nvcmd   = &ds3gcfgmgr_nv_command;
  ds3gcfgmgr_nv_command_ext.context = (uint16) subs_id;

  /* Clear the return signal, call NV, and wait for a response */
#ifndef TEST_FRAMEWORK
  (void) rex_clr_sigs( rex_self(), DS_NV_CMD_SIG );
#endif
  nv_cmd_ext( &ds3gcfgmgr_nv_command_ext );
#ifndef TEST_FRAMEWORK
  (void) dsi_wait( DS_NV_CMD_SIG );
#endif

  /*-------------------------------------------------------
    Report error on return code.
  ---------------------------------------------------------*/
  if( ds3gcfgmgr_nv_command.status != NV_DONE_S )
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_ERROR, "NV Write Failed Item %d Code %d",
                    ds3gcfgmgr_nv_command.item, ds3gcfgmgr_nv_command.status);
  }

  ret_val = ds3gcfgmgr_nv_command.status;

  return ret_val;
} /* ds3gcfgmgr_write_legacy_nv_ex() */

/*===========================================================================
FUNCTION      DS3GCFGMGR_READ_EFS_NV

DESCRIPTION   This function reads EFS NV item for Subscription ID 1.
              This API can be used to read device specific EFS NV.
              data_ptr should be correctly by initailized by clients.

DEPENDENCIES  None.

RETURN VALUE  NV return code.

SIDE EFFECTS  None.
===========================================================================*/
nv_stat_enum_type ds3gcfgmgr_read_efs_nv
(
  const char  *item_path,
  void        *data_ptr,
  uint32       item_size
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds3gcfgmgr_read_efs_nv_ex(item_path,
                                   data_ptr,
                                   item_size,
                                   SYS_MODEM_AS_ID_1);
} /* ds3gcfgmgr_read_efs_nv() */

/*===========================================================================
FUNCTION      DS3GCFGMGR_READ_EFS_NV_EX

DESCRIPTION   This function reads EFS NV item per subscription.
              data_ptr should be correctly by initailized by clients.

DEPENDENCIES  None.

RETURN VALUE  NV return code.

SIDE EFFECTS  None.
===========================================================================*/
nv_stat_enum_type ds3gcfgmgr_read_efs_nv_ex
(
  const char             *item_path,
  void                   *data_ptr,
  uint32                  item_size,
  sys_modem_as_id_e_type  subs_id
)
{
  mcfg_fs_status_e_type  read_status;
  nv_stat_enum_type      ret_val = NV_DONE_S;
  struct fs_stat         temp_buf;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "ds3gcfgmgr_read_efs_nv_ex subs id:%d",
                  subs_id);
  DATA_3GMGR_MSG_SPRINTF_2(MSG_LEGACY_LOW, "item_path:%s item_size %d",
                           item_path, item_size);

  if ( (item_path == NULL) || (data_ptr == NULL) )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "item_path or data_ptr is NULL");
    return NV_FAIL_S;
  }

  /* Check if the file is present or not. */
  if( mcfg_fs_stat(item_path,
                   &temp_buf,
                   MCFG_FS_TYPE_EFS,
                   (mcfg_fs_sub_id_e_type)subs_id)
       != MCFG_FS_STATUS_OK )
  {
    DATA_3GMGR_MSG_SPRINTF_1(MSG_LEGACY_HIGH, "Error opening file: %s",
                             item_path);
    return NV_NOTACTIVE_S;
  }

  if((temp_buf.st_size) != (item_size))
  {
    DATA_3GMGR_MSG_SPRINTF_3(MSG_LEGACY_ERROR,
                    "ds3gcfgmgr_read_efs_nv: Print item_path:%s "
                    "item_size:%d temp_buf.st_size:%d . Performing efs_unlink",
                    item_path, item_size, temp_buf.st_size);
    (void)mcfg_fs_delete(item_path,MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)subs_id);
    return NV_NOTACTIVE_S;
  }

  /* Obtain the contents from EFS */
  read_status = mcfg_fs_read(item_path,
                                            data_ptr,
                             item_size,
                             MCFG_FS_TYPE_EFS,
                             (mcfg_fs_sub_id_e_type)subs_id);

  if (read_status != MCFG_FS_STATUS_OK)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "mcfg_fs_read error:%d",
                    read_status);
    ret_val = NV_FAIL_S;
  }
  else
  {
    ret_val = NV_DONE_S;
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "ds3gcfgmgr_read_efs_nv_ex return NV_DONE_S");
  }

  return ret_val;
} /* ds3gcfgmgr_read_efs_nv_ex() */

/*===========================================================================
FUNCTION      DS3GCFGMGR_WRITE_EFS_NV

DESCRIPTION   This function writes the EFS NV item for Subscription ID 1.
              This API can be used to write device specific EFS NV.

DEPENDENCIES  None

RETURN VALUE  NV return code.

SIDE EFFECTS  None
===========================================================================*/
nv_stat_enum_type ds3gcfgmgr_write_efs_nv
(
  const char  *item_path,
  void        *data_ptr,
  uint32       item_size
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds3gcfgmgr_write_efs_nv_ex(item_path,
                                    data_ptr,
                                    item_size,
                                    SYS_MODEM_AS_ID_1);
} /* ds3gcfgmgr_write_efs_nv() */

/*===========================================================================
FUNCTION      DS3GCFGMGR_WRITE_EFS_NV_EX

DESCRIPTION   This function writes the EFS NV item per subscription.

DEPENDENCIES  None

RETURN VALUE  NV return code.

SIDE EFFECTS  None
===========================================================================*/
nv_stat_enum_type ds3gcfgmgr_write_efs_nv_ex
(
  const char             *item_path,
  void                   *data_ptr,
  uint32                  item_size,
  sys_modem_as_id_e_type  subs_id
)
{
  nv_stat_enum_type  ret_val = NV_FAIL_S;
  mcfg_fs_status_e_type  write_status = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_3GMGR_MSG1(MSG_LEGACY_LOW, "ds3gcfgmgr_write_efs_nv_ex subs id:%d",
                  subs_id);

  if ( NULL == item_path || NULL == data_ptr )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "item_path or data_ptr is NULL");
    return NV_FAIL_S;
  }

  /*-------------------------------------------------------------------------
    Write the item into the item file. If file or file path does not exist,
    create the file path and file
  -------------------------------------------------------------------------*/
  write_status = mcfg_fs_write(item_path,
                                             data_ptr,
                         item_size,
                         MCFG_FS_O_CREAT|MCFG_FS_O_AUTODIR,
                         MCFG_FS_ALLPERMS,
                         MCFG_FS_TYPE_EFS,
                         (mcfg_fs_sub_id_e_type)subs_id);
  if( write_status != MCFG_FS_STATUS_OK )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Unable to write EFS item, error:%d ", write_status);
    ret_val = NV_FAIL_S;
  }
  else
  {
    ret_val = NV_DONE_S;
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW,
                    "ds3gcfgmgr_write_efs_nv_ex return NV_DONE_S");
  }

  return ret_val;
}/* ds3gcfgmgr_write_efs_nv_ex() */

/*===========================================================================
FUNCTION      DS3GCFGMGR_EFS_FILE_INIT

DESCRIPTION   The function intializes the state machine and
              also opens the file.

DEPENDENCIES  None.

RETURN VALUE  0  : SUCCESS: The file is good, readable,
                            State Machine Initialized.
              -1 : FAILURE: The file cannot be opened/ readable.

SIDE EFFECTS  None.
===========================================================================*/
int ds3gcfgmgr_efs_file_init
(
  const char                 *file_path,
  ds3gcfgmgr_efs_token_type  *sm
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ds3gcfgmgr_efs_file_init_ex(file_path, SYS_MODEM_AS_ID_1, sm);
} /* ds3gcfgmgr_efs_file_init() */

/*===========================================================================
FUNCTION      DS3GCFGMGR_EFS_FILE_INIT_EX

DESCRIPTION   The function intializes the state machine and
              also opens the file.

DEPENDENCIES  None.

RETURN VALUE  0  : SUCCESS: The file is good, readable,
                            State Machine Initialized.
              -1 : FAILURE: The file cannot be opened/ readable.

SIDE EFFECTS  None.
===========================================================================*/
int ds3gcfgmgr_efs_file_init_ex
(
  const char                 *file_path,
  sys_modem_as_id_e_type      subs_id,
  ds3gcfgmgr_efs_token_type  *sm
)
{
  int             ret_val = -1;
  struct fs_stat  temp_buf;
  size_t          efs_file_path_size;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize the structure variables and open the file in read mode.
    Care must be taken to not initialize this structure variable before
    calling this function
  -------------------------------------------------------------------------*/
  if(sm == NULL)
  {
    DATA_3GMGR_MSG0( MSG_LEGACY_LOW, "SM ptr is NULL");
    return ret_val;
  }

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ret_val;
  }

  memset(sm, 0, sizeof(ds3gcfgmgr_efs_token_type));

  sm->seperator   = ';';
  sm->curr        = sm->buffer;
  sm->end_pos     = sm->buffer;
  sm->eof         = FALSE;
  sm->eol         = FALSE;
  sm->skip_line   = FALSE;
  sm->bol         = TRUE;
  sm->buffer                = NULL;
  sm->efs_file_path         = NULL;
  sm->efs_get_performed     = FALSE;
  sm->subs_id            = subs_id;

  if( mcfg_fs_stat(file_path,
                   &temp_buf,
                   MCFG_FS_TYPE_EFS,
                   (mcfg_fs_sub_id_e_type)subs_id)
       != MCFG_FS_STATUS_OK )
  {
    DATA_3GMGR_MSG_SPRINTF_1(MSG_LEGACY_LOW, "Error opening file: %s",
                             file_path);
    return ret_val;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Allocate efs file path, If it succeeds try allocating buffer.
      If buffer allocation fails, free the file_path allocation.
      Also check for ret_val = -1 to print an appropriate message.
    -----------------------------------------------------------------------*/
    sm->read_buffer_size = temp_buf.st_size;

    if(sm->read_buffer_size <= 0)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                      "efs stat buffer size error,buffer_size:%d",
                      sm->read_buffer_size);
      return ret_val;
    }

    sm->buffer
      = (char*)ds_system_heap_mem_alloc(sizeof(char) * sm->read_buffer_size);
    if( sm->buffer != NULL )
    {
      efs_file_path_size = sizeof(char) * (strlen(file_path)+1);
      sm->efs_file_path = (char*)ds_system_heap_mem_alloc(efs_file_path_size);

      if( sm->efs_file_path != NULL )
      {
        (void)strlcpy(sm->efs_file_path,file_path,efs_file_path_size);
        ret_val = 0;
      }
      else
      {
        ds_system_heap_mem_free(sm->buffer);
      }
    }

    /*-----------------------------------------------------------------------
      The below check is to display message that memory alloc failed
    -----------------------------------------------------------------------*/
    if( ret_val == -1 )
    {
      DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "Memory allocation failed");
    }
   }

  return ret_val;
} /* ds3gcfgmgr_efs_file_init_ex() */

/*===========================================================================
FUNCTION      DS3GCFGMGR_EFS_FILE_CLOSE

DESCRIPTION   The function closes file and releases the state machine

DEPENDENCIES  The file should have opened already.

RETURN VALUE  NONE

SIDE EFFECTS  None.
===========================================================================*/
void ds3gcfgmgr_efs_file_close
(
  ds3gcfgmgr_efs_token_type  *sm
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(sm == NULL)
  {
    DATA_3GMGR_MSG0( MSG_LEGACY_LOW, "SM ptr is NULL,Close unsuccessful");
    return;
  }

  /*-------------------------------------------------------------------------
    For an efs_get we will not have to close, However free all allocated
    dynamic memory
  -------------------------------------------------------------------------*/
  ds_system_heap_mem_free(sm->efs_file_path);
  sm->efs_file_path = NULL;
  ds_system_heap_mem_free(sm->buffer);
  sm->buffer = NULL;

  return;
} /* ds3gcfgmgr_efs_file_close() */

/*===========================================================================
FUNCTION      DS3GCFGMGR_EFS_TOKENIZER

DESCRIPTION   The is the function that reads data from the opened file.
              The data read is looked for tokens
              1. Each token is seperated by ';'
              2. Successive ';' means empty token
              3. Line begining with '#' is comment
              4. '\n' is the end of token and record
              5. Empty line is ignored
              6. Insufficient tokens is a record is considered bad record

DEPENDENCIES  File should have already been opened.

RETURN VALUE
              SUCCESS : Success => Found Token.
                        *begin points to the begining of token.
                        *end points to the end of token.
              EOL     : End of line is reached => record end
                        => no token extracted
              END     : End of file is reached => file end => feof
                        => no token extracted
              FAILURE : Failed

SIDE EFFECTS  None.
===========================================================================*/
ds3gcfgmgr_efs_token_parse_status_type ds3gcfgmgr_efs_tokenizer
(
  ds3gcfgmgr_efs_token_type   *sm,
  char                       **begin,
  char                       **end
)
{
  int                                     bytes_read = 0;
  char                                   *dummy;
  ds3gcfgmgr_efs_token_parse_status_type  retval;
  mcfg_fs_status_e_type                   mcfg_retval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *begin = 0;
  *end   = 0;

  /*-------------------------------------------------------------------------
     Traversed to end of file => return
  -------------------------------------------------------------------------*/
  if( sm->eof )
  {
    return DS3GCFGMGR_EFS_TOKEN_PARSE_EOF;
  }

  /*-------------------------------------------------------------------------
     Have some bytes to read from the buffer
  -------------------------------------------------------------------------*/
  while( sm->curr < sm->end_pos )
  {
    /*-----------------------------------------------------------------------
      Skip over all carriage return characters (\r) added if file was
      editted using a windows machine
    -----------------------------------------------------------------------*/
    if (*sm->curr == '\r')
    {
      sm->skip_line = FALSE;
      sm->curr++;
      continue;
    }

    /*-----------------------------------------------------------------------
       Lines begining the record with # are comments. Continue to read
       until we reach the end of file.
    -----------------------------------------------------------------------*/
    if( sm->bol && *sm->curr ==  '#' )
    {
      sm->skip_line = TRUE;
      sm->bol = FALSE;
      sm->curr++;
      continue;
    }

    if( sm->skip_line )           /* reading a comment */
    {
      if( *sm->curr == '\n' )     /* End of comment */
      {
        sm->skip_line = FALSE;
        sm->eol = TRUE;
        sm->bol = TRUE;
      }
      sm->curr++;
      continue;                   /*Continue to read until the end of line */
    }

    /*-----------------------------------------------------------------------
      Look for the token. If ';' found at the begining then it is
      an empty token.
      There could be a  case where we hit '\n' while we are looking for a
      token so skip over all the new lines.
    -----------------------------------------------------------------------*/
    if( *begin == 0 )                   /* Find the beginning of token */
    {
      if( *sm->curr == sm->seperator )  /* an empty token */
      {
        if( sm->bol == TRUE )
        {
          sm->bol = FALSE;
        }

        *begin = sm->curr;
        *end   = sm->curr;
        sm->curr++;
        return DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS;
      }

      if( *sm->curr == '\n' )           /* Possibly an empty token */
      {
        if( sm->eol )                   /* Skip over any successive new lines */
        {
          sm->curr++;
          continue;
        }

        *begin  = sm->curr;
        *end    = sm->curr;
        sm->eol = TRUE;
        sm->bol = TRUE;
        sm->curr++;
        return DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS;
      }

      /*---------------------------------------------------------------------
       Before beginning a new token, return an end of record for
       previous record.
      ---------------------------------------------------------------------*/
      if( sm->eol )
      {
        sm->eol = FALSE;
        return DS3GCFGMGR_EFS_TOKEN_PARSE_EOL;
      }

      *begin = sm->curr;                /* Initialize to beginning of token */
    }
    else if(*sm->curr == sm->seperator || *sm->curr == '\n' ||
            ((sm->seperator == '*') && ((*sm->curr == ':')|| (*sm->curr == ';'))))
    {
      /*----------------------------------------------------------------------
        Only in cases where the seperator is "Wildcard",
        overwrite the seperator with current value (: or ;)
      ----------------------------------------------------------------------*/
      if(sm->seperator == '*')
      {
        sm->seperator = *sm->curr;
      }
   
      *end = sm->curr++;                /* Found end of token */

      /*---------------------------------------------------------------------
         This is a end of line. Save the state and send
         end of line event when a next token is requested .
      ---------------------------------------------------------------------*/
      if( **end == '\n' )
      {
        sm->eol = TRUE;
        sm->bol = TRUE;
      }
      return DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS;
    }

    sm->curr++;
  }/* while */

  /*-------------------------------------------------------------------------
    In the middle of token and we ran out characters in the buffer
  -------------------------------------------------------------------------*/
  if( *begin )
  {
    if( *begin != sm->buffer )
    {
      /*---------------------------------------------------------------------
        Move the partial token over to beginning of buffer
      ---------------------------------------------------------------------*/
      /*lint -e732 */
      memscpy( sm->buffer,DS_EFS_READ_BUFFER_SZ, *begin, (sm->curr - *begin) );
      /*lint +e732 */
      sm->curr = sm->buffer + (sm->curr - *begin);
      *begin = sm->buffer;
    }
    else
    {
      DATA_3GMGR_MSG0( MSG_LEGACY_LOW,
                       "Token is larger than DS_EFS_READ_BUFFER_SZ" );
      return DS3GCFGMGR_EFS_TOKEN_PARSE_FAILURE;
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      No token or data exists in the buffer
    -----------------------------------------------------------------------*/
    sm->curr = sm->buffer;
  }

  /*-------------------------------------------------------------------------
    efs_get has to be performed only once, If it has already been
    performed return EOF
  -------------------------------------------------------------------------*/
  if(!sm->efs_get_performed)
  {
    DATA_3GMGR_MSG0( MSG_LEGACY_LOW, "IN EFS GET");

    /*-----------------------------------------------------------------------
      Read data from the efs file.
    -----------------------------------------------------------------------*/
    mcfg_retval = mcfg_fs_read((const char*)sm->efs_file_path,
                         (void *)sm->curr,
                               sm->read_buffer_size,
                               MCFG_FS_TYPE_EFS,
                               (mcfg_fs_sub_id_e_type)sm->subs_id);

    if(mcfg_retval != MCFG_FS_STATUS_OK)
    {
      DATA_3GMGR_MSG0( MSG_LEGACY_LOW, "File read failed");
      return DS3GCFGMGR_EFS_TOKEN_PARSE_FAILURE;
    }

   bytes_read = sm->read_buffer_size;

    sm->efs_get_performed = TRUE;
  }
  else
  {
    sm->eof = TRUE;
    DATA_3GMGR_MSG0( MSG_LEGACY_LOW, "END of EFS GET");
    return DS3GCFGMGR_EFS_TOKEN_PARSE_EOL;
  }

  if( bytes_read > 0 )
  {
    sm->end_pos = sm->curr + bytes_read;
    sm->eof    = FALSE;

    if(*begin != 0)
    {
      retval= ds3gcfgmgr_efs_tokenizer( sm, &dummy, end ); /* Call the function
                                            again because you could be in the
                                            middle of reading a token */
    }
    else
    {
      retval = ds3gcfgmgr_efs_tokenizer( sm, begin, end);
    }

    return retval;
  }
  else
  {
    /* No bytes read => reached the end of file. */
    if(*begin == 0)
    {
      sm->eof = 1;
      return DS3GCFGMGR_EFS_TOKEN_PARSE_EOL;
    }
    else
    {
      /*---------------------------------------------------------------------
        If a token was found return the token and
        when next token is requested send EOF
      ---------------------------------------------------------------------*/
      *end = sm->curr;
      if(bytes_read == 0)
      {
        /*-------------------------------------------------------------------
         If the EOF character is missing in the file and the bytes read
         are zero all the time then we are trying to bail out of this.

         NOTE: We might have to revisit this later again if different
         modules
        -------------------------------------------------------------------*/
        sm->eof = 1;
        return DS3GCFGMGR_EFS_TOKEN_PARSE_EOL;
      }
      return DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS;
    }
  }/* End of bytes read*/
} /* ds3gcfgmgr_efs_tokenizer() */

/*===========================================================================
FUNCTION      DS3GCFGMGR_EFSIF_READ_A_RECORD

DESCRIPTION   Read a record including number of fields

DEPENDENCIES  None.

RETURN VALUE  Status to indicate if failed or Success or others

SIDE EFFECTS  None.
===========================================================================*/
ds3gcfgmgr_efs_token_parse_status_type ds3gcfgmgr_efsif_read_a_record
(
  ds3gcfgmgr_efs_token_type  *sm,
  char                        rec[][MAX_FIELD_LEN],
  uint16                      num_fields,
  uint16                     *valid_fields
)
{
  ds3gcfgmgr_efs_token_parse_status_type  ret_val;
  char                                   *from, *to;
  uint16                                  current_field = 0;
  uint32                                  current_field_len;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  while( DS3GCFGMGR_EFS_TOKEN_PARSE_EOF !=
         (ret_val = ds3gcfgmgr_efs_tokenizer(sm, &from, &to)) &&
         DS3GCFGMGR_EFS_TOKEN_PARSE_EOL != ret_val )
  {
    if( (current_field < num_fields) && (from != NULL) )
    {
      current_field_len = (uint32) MIN((to-from), (MAX_FIELD_LEN-1));
      memscpy(rec[current_field],(MAX_FIELD_LEN-1), from, current_field_len);
      rec[current_field][current_field_len] = '\0';
      current_field++;
    }
    else
    {
      break;
    }
  }

  *valid_fields = current_field;
  return ret_val;
} /* ds3gcfgmgr_efsif_read_a_record() */

/*===========================================================================
FUNCTION      DS3GCFGMGR_READ_FAST_DORMANCY_INFO_FROM_EFS

DESCRIPTION   This function will read the efs file all the necessary
              data & fill the timer values in FD config structure.

              EFS File Format - Param_Name:Param_Val;
              For example     - TIMER_1:0;

DEPENDENCIES  None

RETURN VALUE
TRUE- Success
FALSE- Failure

SIDE EFFECTS  None
===========================================================================*/
boolean ds3gcfgmgr_read_fast_dormancy_info_from_efs (void)
{
  ds3gcfgmgr_efs_token_type  efs_db; /* structure used for tokenizing the file*/
  char *from, *to; /* ptrs to start and end of the token */
  ds3gcfgmgr_efs_token_parse_status_type  ret_val
                                           = DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS;
  boolean param_name = TRUE; /* flag to keep track of the parser state
                                TRUE - parser expects Token Name in the EFS
                                FALSE - parser expects Token Val in the EFS*/
  boolean token_check = FALSE;
  int    atoi_result;
  char file_name[]
                = "/nv/item_files/modem/data/3gpp/fast_dormancy_config.txt";
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds3gcfgmgr_efs_file_init(file_name, &efs_db) == -1)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_LOW, "Failure reading Fast Dormancy cnfg");
    return FALSE;
  }

  /* Set the seperator as : */
  efs_db.seperator = ':';
/*lint -save -esym(644,token_id) param_name boolean flag takes care of this */
  while (DS3GCFGMGR_EFS_TOKEN_PARSE_EOF
          != (ret_val = ds3gcfgmgr_efs_tokenizer(&efs_db, &from, &to )))
  {
    /*------------------------------------------------------------------------
      Token being read. 'from' points to the beginning of the token and
      'to' point to the end of the token.

      The tokenizer automatically skips blank lines and comments (lines
      beginning with #, so no need to check for them here).
    -------------------------------------------------------------------*/
    if((from == to) || (DS3GCFGMGR_EFS_TOKEN_PARSE_EOL == ret_val))
    {
      /*---------------------------------------------------------------
         Skip empty tokens.
      ------------------------------------------------------------------*/
      continue;
    }
    else if(DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS == ret_val)
    {
      /*--------------------------------------------------------------
        check if we are looking for param name or param value
      -----------------------------------------------------------------*/
      if (param_name == TRUE)
      {
        /*-------------------------------------------------------------
          set param_name as FALSE This means the next token is a
          param value
        ----------------------------------------------------------------*/
        param_name = FALSE;
        /* set the seperator as ; */
        efs_db.seperator = ';';
      }
      /*---------------------------------------------------------------
         This means that the token is a param value
        ----------------------------------------------------------------*/
      else
      {
        /*--------------------------------------------------------------
          Read the parama Value from efs file and store in FD structure
         --------------------------------------------------------------*/
        DSUTIL_ATOI(from,to,atoi_result);

        /* update the corresponding timer values based on token check */
        if(token_check == FALSE)
        {
          ds_3gpp_bearer_cntxt_set_modem_fd_timer_1_value((uint32)atoi_result);
          token_check = TRUE;
        }
        else
        {
          ds_3gpp_bearer_cntxt_set_modem_fd_timer_2_value((uint32)atoi_result);
        }

        /*---------------------------------------------------------------
          set param_name as TRUE This means that next token is a param name
          ----------------------------------------------------------------*/
        param_name = TRUE;
       /* Set the seperator as : */
        efs_db.seperator = ':';
      }
    }
  } /* end of while loop */

/*lint -restore (644,token_id)*/
  ds3gcfgmgr_efs_file_close(&efs_db);
  return TRUE;
} /* ds3gcfgmgr_read_fast_dormancy_info_from_efs */

/*===========================================================================
FUNCTION      DS3GCFGMGR_READ_FAST_DORMANCY_ENABLED_FLAG_FROM_EFS

DESCRIPTION   This function will read the efs file and gets if Fast
              Dormancy is enabled or not. If file doesnt exist, by
              deafult cosiders as FD is disabled

              EFS File Format - Param_Val
              For example     - 1/0

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gcfgmgr_read_fast_dormancy_enabled_flag_from_efs( void )
{
  char   data_conf_info_path[] =
                         "/ds/qmi/fast_dormancy.txt";
  int   fd =0, efs_ret = 0;
  int result;
  boolean enabled_flag = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*Read FD flag from EFS and restrict FD filter feature based on EFS flag*/
  fd = mcfg_fopen(data_conf_info_path,
                  MCFG_FS_O_RDONLY|MCFG_FS_O_TRUNC,
                  MCFG_FS_DEFFILEMODE,
                  MCFG_FS_TYPE_EFS,
                  MCFG_FS_SUBID_NONE);
 
  if (fd < 0)
  {
    ds_3gpp_bearer_cntxt_set_modem_fd_enabled_flag(TRUE);
    DATA_3GMGR_MSG0(
       MSG_LEGACY_ERROR,
       "EFS item not configured for fastdormancy, Initializing defaults.");
    return;
  }
  else
  {
    /* reading the EFS Content into global variable */
    efs_ret = mcfg_fread(fd,
                         &(enabled_flag),
                         sizeof(enabled_flag),
                         MCFG_FS_TYPE_EFS);

    if (efs_ret > 0)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "FD Enabled flag = %d", enabled_flag);
    }
    ds_3gpp_bearer_cntxt_set_modem_fd_enabled_flag(enabled_flag);
  }
   /* EFS Close */
  result = mcfg_fclose(fd, MCFG_FS_TYPE_EFS);
  if ( 0 != result )
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Error closing EFS file error:%d",
                    mcfg_fs_errno(MCFG_FS_TYPE_EFS));
  }
  return;
}/*ds3gcfgmgr_read_fast_dormancy_enabled_flag_from_efs*/


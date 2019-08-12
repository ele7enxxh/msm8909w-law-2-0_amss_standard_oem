/*===========================================================================
  FILE: ds_Profile_NVMgr.cpp

  OVERVIEW: This file provides implementation of the NVMgr class.

  DEPENDENCIES: None

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2014-02-14 bh Added implementation

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_profile_db.h"
#include "ds_profile_db_internal.h"
#include "ds_Profile_DBMgr.h"
#include "ds_Profile_Profile.h"
#include "ds_Profile_TechMgr.h"
#include "ds_Profile_File.h"
#include "ds_Profile_FileEFS.h"
#include "ds_Profile_TechMgr3GPP.h"
#include "ds_Profile_TechMgr3GPP2.h"
#include "ds_Profile_Platform.h"
#include "ds_Profile_Log.h"
#include "ps_system_heap.h"
#include "ds_Profile_NVMgr.h"

 /* Filename for DS PROFILE DB NV items. Must have same order as the enum
         * ds_profile_db_nv_item_e_type. EFS item files are created in the DS PROFILE DB 
         * NV directory with this filename, e.g. "/nv/item_files/modem/data/dsprofiledb/example" */
static const char ds_profile_db_nv_item_paths[DS_PROFILE_DB_NV_NUM_ITEMS][DS_PROFILE_DB_NV_FILENAME_MAX_LEN] =
{
  "dataprofile_enable_sfs"                    /*DS_PROFILE_DB_ENABLE_SFS */
};

/*===========================================================================

FUNCTION  GetEPCSupportVal

DESCRIPTION
  This function is to return the EFS value of weather EPC is supported or not
  File stored in EFS @ /Data_Profiles/epc_support. This value is read by
  ds_profile_db_read_supported_efs_file() function

DEPENDENCIES
   NONE
 
RETURN VALUE
  EPC_SUPPORT variable that is read during EFS read

SIDE EFFECTS
  None

===========================================================================*/
uint8 NVMgr::GetEPCSupportVal()
{
  return ds_profile_db_epc_support;
}

/*===========================================================================

FUNCTION  ReadEFSFile

DESCRIPTION
  This function initializes the global state after reading required EFS files.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void NVMgr::ReadEFSFile
(
  ds_profile_db_efs_file_e_type          file_type
)
{
  static const int  MAX_PATH_LEN = 128;
  static const char PROFILE_DIR[] = "/Data_Profiles/";
  static const char EPC_FILE[] = "epc_support.txt";

  int          fd = 0;
  struct       fs_stat  temp_buf;
  char         file_path[MAX_PATH_LEN];

  uint32                            temp_data        = 0;
  void                              *data            = NULL;
  uint8                             size_to_read     = 0;
  char                              epc_temp_buf[2]  = {'\0'};

  ds_profile_db_efs_read_state_e_type  efs_state     = DB_FILE_READ_STATE_TYPE_NONE;

  memset(&file_path[0], 0 , sizeof(file_path));
  memset(&ds_profile_db_epc_support, 0 , sizeof(ds_profile_db_epc_support));

  temp_data = strlcpy(file_path,  PROFILE_DIR, sizeof(file_path));

  if (temp_data >= sizeof(file_path))
  {
    efs_state = DB_FILE_READ_STATE_TYPE_1;
    goto bail;
  }

  switch (file_type) 
  {
    case DB_EFS_FILE_TYPE_EPC_SUPPORT:

       /*Move the buffer by temp_data and append the file_path*/
      if (strlcpy(&file_path[temp_data], EPC_FILE, sizeof(file_path)) >= sizeof(file_path))
      {
        efs_state = DB_FILE_READ_STATE_TYPE_2;
        goto bail;
      }
      data         = &epc_temp_buf;
      size_to_read = sizeof(char);
      break;

    default:
      efs_state = DB_FILE_READ_STATE_TYPE_3;
      goto bail;
  }

  /* EFS SUCCESS == 0 */
  if((mcfg_fs_stat (file_path,
                    &temp_buf,
                    MCFG_FS_TYPE_EFS,
                    MCFG_FS_SUBID_NONE) != MCFG_FS_STATUS_OK))
  {
    efs_state = DB_FILE_READ_STATE_TYPE_4;
    goto bail;
  }
  else
  {
    fd = mcfg_fopen(file_path,
                    MCFG_FS_O_RDONLY,
                    MCFG_FS_ALLPERMS,
                    MCFG_FS_TYPE_EFS,
                    MCFG_FS_SUBID_NONE);
    if (fd < 0)
    {
      efs_state = DB_FILE_READ_STATE_TYPE_5;
      goto bail;
    }
    else
    {
      if(-1 == mcfg_fread(fd, data, size_to_read, MCFG_FS_TYPE_EFS))
      {
        efs_state = DB_FILE_READ_STATE_TYPE_6;
        goto bail;
      }
      PROFILE_DB_LOG_HIGH_2("ds_profile_db_read_supported_efs_file size_read = %d, %d", 
                            size_to_read, ds_profile_db_epc_support);

      mcfg_fclose(fd, MCFG_FS_TYPE_EFS);
    }
  }
  /*Convert to int*/
  if (file_type == DB_EFS_FILE_TYPE_EPC_SUPPORT) 
  {
     ds_profile_db_epc_support = atoi(epc_temp_buf);
  }
bail:
   PROFILE_DB_LOG_HIGH_1("State = %d", efs_state);
   return;
}

ds_profile_db_result_type NVMgr::InitEFSConfFile
(
  void
)
{
  char    ds_profile_db_conf_path[] = "/nv/item_files/conf/data_profile.conf";
  char    *path = NULL;
  int     fd;
  uint32  path_offset = 0;
  int     n, i;
  ds_profile_db_nv_item_e_type item[] = { DS_PROFILE_DB_ENABLE_SFS};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_SYSTEM_HEAP_MEM_ALLOC(path, DS_PROFILE_DB_NV_MAX_EFS_PATH_SIZE, char*);
  if (NULL == path)
  {
    PROFILE_DB_LOG_ERROR("Failed to allocate EFS path");
    return DB_RESULT_ERR_OUT_OF_MEMORY;
  }

  fd = mcfg_fopen(ds_profile_db_conf_path, 
                  MCFG_FS_O_CREAT | MCFG_FS_O_RDWR | MCFG_FS_O_TRUNC,
                  MCFG_FS_ALLPERMS,
                  MCFG_FS_TYPE_EFS,
                  MCFG_FS_SUBID_NONE);
  if (fd != MCFG_FS_STATUS_OK)
  {
    PROFILE_DB_LOG_ERROR_1("EFS DS Profile conf file open failed: %s", ds_profile_db_conf_path);
    PS_SYSTEM_HEAP_MEM_FREE(path);
    return DB_RESULT_ERR_FILE_ACCESS;
  }

  for (i = 0; i < ARR_SIZE(item); i++)
  {
    if ( (item[i] <= DS_PROFILE_DB_NVI_ITEM_START) ||
         (item[i] >= DS_PROFILE_DB_NVI_ITEM_END) )
    {
      PROFILE_DB_LOG_ERROR_1("Unknown NV item %d", item[i]);
      continue;
    }

    n = snprintf(&path[path_offset],
                 DS_PROFILE_DB_NV_MAX_EFS_PATH_SIZE - path_offset,
                 "%s%s\n",
                 DS_PROFILE_DB_NV_DIR,
                 ds_profile_db_nv_item_paths[DS_PROFILE_DB_NV_ITEM_FNAME_INDEX(item[i])]); 

    if ((uint32)n >= DS_PROFILE_DB_NV_MAX_EFS_PATH_SIZE - path_offset)
    {
      PROFILE_DB_LOG_ERROR("Insuffient buffer");
      break;
    }

    path_offset += n;
  }

  if (mcfg_fwrite(fd, path, path_offset, MCFG_FS_TYPE_EFS) < 0)
  {
    PROFILE_DB_LOG_ERROR_1("EFS DS Profile conf file open failed: %s", ds_profile_db_conf_path);
    mcfg_fclose(fd, MCFG_FS_TYPE_EFS);
    PS_SYSTEM_HEAP_MEM_FREE(path);
    return DB_RESULT_ERR_FILE_ACCESS;
  }

  mcfg_fclose(fd, MCFG_FS_TYPE_EFS);
  PS_SYSTEM_HEAP_MEM_FREE(path);

  return DB_RESULT_SUCCESS;

} /* ds_profile_db_nv_init_efs_conf_file*/

ds_profile_db_result_type NVMgr::ReadFromNV(ds_profile_db_nv_item_e_type nv_file,
                                            void *                       data_ptr,
                                            fs_size_t                    data_size,
                                            ds_profile_db_subs_type      subs)
{
  char path[DB_MAX_NV_FILE_PATH_LEN] = {};
  ds_profile_db_result_type status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (data_ptr == NULL)
  {
    status = DB_RESULT_ERR_INVAL;
  } 
  else
  {
    status = BuildPathFromNV(nv_file, path, sizeof(path));
    if (status == DB_RESULT_SUCCESS)
    {
      status = GetNVContent(path, (char *)data_ptr, data_size, subs);
    }
  }

  PROFILE_DB_LOG_MED_1("returning status=%d", status);

  return status;
}

ds_profile_db_result_type NVMgr::BuildPathFromNV( ds_profile_db_nv_item_e_type item,
                                                  char *                       path,
                                                  uint32                       path_len
                                                 )
{
  uint32 path_offset = 0;
  ds_profile_db_result_type status = DB_RESULT_FAIL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* New-style items */
  if (item > DS_PROFILE_DB_NVI_ITEM_START && item < DS_PROFILE_DB_NVI_ITEM_END)
  {
      /* Copy in the directory and filename */
      path_offset = strlcpy(path, DS_PROFILE_DB_NV_DIR, path_len);
      if (path_offset < path_len)
      {
        path_offset += strlcpy(&path[path_offset],
                               ds_profile_db_nv_item_paths[DS_PROFILE_DB_NV_ITEM_FNAME_INDEX(item)],
                               path_len - path_offset);
        status = DB_RESULT_SUCCESS;
      }
  }
  /* If the path offset exceededs the size of the buffer, then it means we 
   * could not copy the entire path. If it matches, then one character was
   * truncated from the input to make room for the null termination. */
  if (path_offset >= path_len)
  {
    status = DB_RESULT_ERR_FILE_ACCESS;
  }

  PROFILE_DB_LOG_MED_1("returning status=%d", status);

  return status;
} /* BuildPathFromNV() */


/*==========================================================================

  FUNCTION DS_PROFILE_DB_NVI_FILE_GET_CONTENTS

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
    DB_RESULT_SUCCESS            : On success
    DB_RESULT_FAIL               : On failure
    DB_RESULT_ERR_FILE_ACCESS    : On File Access Failure
  SIDE EFFECTS
    None.

===========================================================================*/
ds_profile_db_result_type NVMgr::GetNVContent(const char * path,
                                              void *             data_ptr,
                                              fs_size_t          data_size,
                                              ds_profile_db_subs_type subs)
{
  ds_profile_db_result_type status = DB_RESULT_SUCCESS;
  mcfg_fs_sub_id_e_type mcfg_subs_id = MCFG_FS_SUBID_NONE;
  mcfg_subs_id = ds_profile_db_convert_ds_to_mcfg_subs(subs);
  int efs_ret = mcfg_fs_read(path, data_ptr, data_size, MCFG_FS_TYPE_EFS, mcfg_subs_id);
  if (efs_ret != MCFG_FS_STATUS_OK)
  {
    status = DB_RESULT_ERR_FILE_ACCESS;
  }
  return status;
}

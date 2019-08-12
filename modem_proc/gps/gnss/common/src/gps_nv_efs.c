/*==============================================================================

                      G P S NV Item  R E G I S T R Y

  DESCRIPTION
    The GPS NV item Registry is a central location for configuration item storage.
    It serves as an interface to potentially various data sources. Currently its
    main data source is nonvolatile memory, via the EFS item interface.

Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/src/gps_nv_efs.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
02/26/15      tf      Added MCFG API support for SIM specific NVs
05/03/10      gk      initial checkin
==============================================================================*/

#include "gps_variation.h"
#include "customer.h"
#include "comdef.h"
#include "aries_os_api.h"

#ifdef FEATURE_CGPS

  #include <math.h>
  #include <float.h>

  #include "fs_errno.h"
  #include "gps_nv_efs.h"
  #include "msg.h"
  #ifdef FEATURE_CGPS_LEGACY_NV_API
  #include "fs_public.h"
  #else
  #include "mcfg_fs.h"
  #endif /* ! FEATURE_CGPS_LEGACY_NV_API */

/*===========================================================================
=============================================================================

   Private data structures and declarations

=============================================================================
===========================================================================*/
#ifdef FEATURE_CGPS_LEGACY_NV_API
#define CGPS_NV_EFS_PERMS ACCESSPERMS
#else
/* Define GPS file and folder permissions */
#define CGPS_NV_EFS_PERMS MCFG_FS_ALLPERMS
#endif /* ! FEATURE_CGPS_LEGACY_NV_API */

typedef struct
{
  cgps_nv_efs_reg_item_type item_start;
  cgps_nv_efs_reg_item_type item_end;
  char                      sz_pathname_prefix[10];
  const cgps_nv_efs_reg_item_struct* table_ptr;
} gps_nv_efs_subsys_data_type;

static gps_nv_efs_subsys_data_type gps_nv_efs_subsys_table[CGPS_NV_EFS_SUBSYS_MAX];

static const char* cgps_nv_efs_file_name[] =
{
  "cgps_sm.conf",
  "cgps_me.conf",
  "cgps_pe.conf",
  "cgps_pdcomms.conf",
  "cgps_tle_tlm.conf",
  "cgps_tle_xtm.conf",
  "cgps_sdp.conf",
  "cgps_common.conf",
  "cgps_wle_wlm.conf",
  "cgps_samlite.conf",
  "cgps_loc.conf",
  /* add new names above this line */
  "cgps_invalid.conf"
};


/*IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

                            Private Functions

IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII*/
/*==============================================================================
  FUNCTION
    gps_nv_efs_find_subsys_entry

  DESCRIPTION
    Helper function to find the subsystem table entry for the given NV item.

  RETURNS
    index into the table

==============================================================================*/
static const gps_nv_efs_subsys_data_type* gps_nv_efs_find_subsys_entry(
  cgps_nv_efs_reg_item_type item_id
)
{
  const gps_nv_efs_subsys_data_type* entry_ptr = NULL;
  uint8 table_idx;
  for (table_idx = 0;
       (table_idx < ARR_SIZE(gps_nv_efs_subsys_table)) && (entry_ptr == NULL);
       table_idx++)
  {
    if ((item_id >= gps_nv_efs_subsys_table[table_idx].item_start) &&
        (item_id <= gps_nv_efs_subsys_table[table_idx].item_end) &&
        (NULL != gps_nv_efs_subsys_table[table_idx].table_ptr))
    {
      /* found it */
      entry_ptr = &gps_nv_efs_subsys_table[table_idx];
      break;
    }
  }
  return entry_ptr;
}

/*==============================================================================
  FUNCTION
    cgps_nv_efs_reg_get_item_fname

  DESCRIPTION
    This is a local function
    Constructs the full pathname of item file name and returns the length of
    the filename char array and file_name_size. file_name_size value will be 0 if error.
    No bound checks for the array are done here. Make sure the checks are done before
    passing it to this function. function is intended to be a used within this file.

  RETURNS

==============================================================================*/
static uint8 cgps_nv_efs_reg_get_item_fname(
  char* file_name,
  uint8 file_name_buf_size,
  boolean newline,
  const cgps_nv_efs_reg_item_struct *reg_items_ptr,
  const char* psz_pathname_prefix
)
{
  uint8 file_name_size = 0;
  /* item is already range checked in the calling function */
  if (reg_items_ptr->data_src == CGPS_NV_EFS_REG_ST_ITEM_EXTERNAL)
  {
    file_name_size = (uint8)GNSS_SNPRINTF( file_name,
                                            file_name_buf_size,
                                            "/nv/item_files/gps/%s/%s",
                                            psz_pathname_prefix,
                                            reg_items_ptr->file_name );
  }
  else
  {
    file_name_size = (uint8)GNSS_SNPRINTF( file_name,
                                            file_name_buf_size,
                                            "/cgps/nv/item_files/%s",
                                            reg_items_ptr->file_name );
  }
  if (newline)
  {
    /* needed only between entries of config file */
    file_name_size = (uint8)GNSS_STRLCAT(file_name, "\n", file_name_buf_size);
    if (file_name_size >= file_name_buf_size)
    {
      MSG_MED("CGPS_NV_EFS_REG: could not add new line to file name file_size %d",
              file_name_size, 0, 0);
      file_name_size = 0;
    }
  }
  return file_name_size;
}

/*==============================================================================
  FUNCTION
    cgps_nv_efs_init_internal_subsid

  DESCRIPTION
    Helper function for initialization required for NV items in the EFS.
    A config file is created and will hold the EFS file names of all NV items
    in the registry for the given subsystem.

    This function is Subscription ID and File System aware

  RETURN VALUE
    TRUE  - Success
    FALSE - Error during initialization
==============================================================================*/
static boolean cgps_nv_efs_init_internal_subsid(
  cgps_nv_efs_subsys_type subsys,
  cgps_nv_efs_reg_item_type table_start,
  cgps_nv_efs_reg_item_type table_end,
  const cgps_nv_efs_reg_item_struct* cgps_nv_efs_reg_items,
  const char* psz_pathname_prefix,
  cgps_nv_efs_fs_type fs,
  cgps_nv_efs_sub_id_type subid
)
{
  int table_idx;
  int bytes_written = 0;
  int bytes2write = 0;
  uint8 file_size = 0;
  int file_handle = -1;
  boolean error_detected = FALSE;
  char conf_dir_name1[]="/nv/";
  char conf_dir_name2[]="/nv/item_files/";
  char conf_dir_name3[]="/nv/item_files/conf/";
  const char* conf_file_name = NULL;
  char file_name[CGPS_MAX_NV_EFS_FILEPATH_SIZE];
  int mkdir_result = -1;
  struct fs_stat  temp_stat = {0};

  MSG_MED("CGPS_NV_EFS: Registry Initialization", 0, 0, 0);

  /* Validate Parameters */
  if (cgps_nv_efs_reg_items == NULL)
  {
    MSG_ERROR("CGPS_NV_EFS_REG: Null table pointer.", 0, 0, 0);
    return FALSE;
  } /* if null table*/

  if (psz_pathname_prefix == NULL)
  {
    MSG_ERROR("CGPS_NV_EFS_REG: Null pathname", 0, 0, 0);
    return FALSE;
  }

  if (table_start > table_end ||
      table_start < CGPS_NV_EFS_MIN_NUM_ITEMS ||
      table_start >= CGPS_NV_EFS_MAX_NUM_ITEMS ||
      table_end   < CGPS_NV_EFS_MIN_NUM_ITEMS ||
      table_end   >= CGPS_NV_EFS_MAX_NUM_ITEMS )
  {
    MSG_ERROR("CGPS_NV_EFS_REG: invalid range: %d, %d", table_start, table_end, 0);
    return FALSE;
  }

  switch (subsys)
  {
    case CGPS_NV_EFS_SM:
    case CGPS_NV_EFS_ME:
    case CGPS_NV_EFS_PE:
    case CGPS_NV_EFS_PDCOMM:
    case CGPS_NV_EFS_TLE_TLM:
    case CGPS_NV_EFS_TLE_XTM:
    case CGPS_NV_EFS_SDP:
    case CGPS_NV_EFS_COMMON:
    case CGPS_NV_EFS_WLE_WLM:
    case CGPS_NV_EFS_SAML:
	case CGPS_NV_EFS_LOC:
      conf_file_name = cgps_nv_efs_file_name[subsys];
      break;
    default:
      MSG_ERROR("CGPS_NV_EFS: No config file available for this type %d", subsys, 0, 0);
      return FALSE;
  }

  if( conf_file_name == NULL )
  {
    MSG_ERROR("CGPS_NV_EFS: Config file name is still NULL", 0, 0, 0);
    return FALSE;
  }

  if( fs <= CGPS_NV_FS_TYPE_MIN ||
      fs >= CGPS_NV_FS_TYPE_MAX)
  {
    MSG_ERROR("CGPS_NV_EFS: Invalid File system specified %d", fs, 0, 0);
    return FALSE;
  }

  if( subid <= CGPS_NV_SUBID_MIN ||
      subid > CGPS_NV_SUBID_MAX )
  {
    MSG_ERROR("CGPS_NV_EFS: Invalid Subscription ID specified %d", subid, 0, 0);
    return FALSE;
  }

#ifdef FEATURE_CGPS_LEGACY_NV_API
  /* create conf directory if it doesn't exist */
  mkdir_result = efs_mkdir(conf_dir_name1, CGPS_NV_EFS_PERMS);
  if (mkdir_result != 0)
  {
    MSG_MED("CGPS_NV_EFS: mcfg_mkdir /nv returned %d", efs_errno, 0, 0);
  }
  mkdir_result = efs_mkdir(conf_dir_name2, CGPS_NV_EFS_PERMS);
  if (mkdir_result != 0)
  {
    MSG_MED("CGPS_NV_EFS: mcfg_mkdir /nv/item_files returned %d", efs_errno, 0, 0);
  }
  mkdir_result = efs_mkdir(conf_dir_name3, CGPS_NV_EFS_PERMS);
  if (mkdir_result != 0)
  {
    MSG_MED("CGPS_NV_EFS: mcfg_mkdir /nv/item_files/conf returned %d", efs_errno, 0, 0);
  }

  (void)GNSS_SNPRINTF(file_name, sizeof(file_name), "%s%s",
                       conf_dir_name3, conf_file_name);
  MSG_SPRINTF_1(MSG_SSID_DFLT, MSG_LEGACY_MED, "CGPS_NV_EFS: file name %s", file_name);

   /* Check if conf file already exists and it has got data in it.
      If conf file does not exist or if it exists and has 0 bytes
      then open the conf file and write the absolute path of NV items in it.
      Conf file is required to access NV items via QXDM/QPST interface.
      If just EFS item file is required conf file need not be updated
      with absolute path. */
  if ( efs_stat(file_name, &temp_stat) != ENOERR ||
       temp_stat.st_size == 0 )
  {
    /* create new conf file. */
    file_handle = efs_open(file_name,
                            O_CREAT|O_WRONLY|O_TRUNC,
                            CGPS_NV_EFS_PERMS);

    if (file_handle == -1)
    {
      MSG_ERROR("CGPS_NV_EFS: Error %d creating config file", efs_errno, 0, 0);
      return FALSE;
    }

    /* for convenience, traverse the table backward */
    table_idx = table_end - table_start;
    while ((table_idx >= 0) && (error_detected == FALSE))
    {
      /* Generate the absolute path for the file entries. */
      file_size = cgps_nv_efs_reg_get_item_fname(
                    file_name, sizeof(file_name), TRUE,
                    &cgps_nv_efs_reg_items[table_idx],
                    psz_pathname_prefix);
      bytes2write = file_size;

      /* Since "It is permitted for mcfg_fwrite to write fewer bytes than
         were requested, even if space is available" , the following while
         loop is designed to expect data not to be written all in one go. */
      while ((bytes2write > 0) && (error_detected == FALSE))
      {
        if( (sizeof(file_name) <= file_size-bytes2write) ||
            ((file_size-bytes2write) < 0))
        {
           MSG_ERROR("CGPS_NV_EFS:Incorrect File Name length. Limit is :%d, Current size:%d",
                      sizeof(file_name), file_size-bytes2write, 0);
           error_detected = TRUE;
        }
        else
        {
          bytes_written = efs_write(file_handle,
                                     &file_name[file_size-bytes2write],
                                     bytes2write);
        }
        if (bytes_written < 0)
        {
          MSG_ERROR("CGPS_NV_EFS: Error %d writing to config file", efs_errno, 0, 0);
          error_detected = TRUE;
        }
        else
        {
          bytes2write -= bytes_written;
        }
      }/* while not done writing */
      table_idx--; /* next entry up */
    } /* while table index is valid */

    if (efs_close(file_handle) == -1)
    {
      /* Error while closing */
      MSG_ERROR("CGPS_NV_EFS: Error %d closing config file", efs_errno, 0, 0);
      error_detected = TRUE;
    } /* mcfg_fclose check */
#else
  /* create conf directory if it doesn't exist */
  mkdir_result = mcfg_mkdir(conf_dir_name1, CGPS_NV_EFS_PERMS, (mcfg_fs_e_type)fs);
  if (mkdir_result != 0 &&
      mcfg_fs_errno((mcfg_fs_e_type)fs) != EEXIST)
  {
    MSG_MED("CGPS_NV_EFS: mcfg_mkdir /nv returned %d", mcfg_fs_errno((mcfg_fs_e_type)fs), 0, 0);
    return FALSE;
  }

  mkdir_result = mcfg_mkdir(conf_dir_name2, CGPS_NV_EFS_PERMS, (mcfg_fs_e_type)fs);
  if (mkdir_result != 0 &&
      mcfg_fs_errno((mcfg_fs_e_type)fs) != EEXIST)
  {
    MSG_MED("CGPS_NV_EFS: mcfg_mkdir /nv/item_files returned %d", mcfg_fs_errno((mcfg_fs_e_type)fs), 0, 0);
    return FALSE;
  }

  mkdir_result = mcfg_mkdir(conf_dir_name3, CGPS_NV_EFS_PERMS, (mcfg_fs_e_type)fs);
  if (mkdir_result != 0 &&
      mcfg_fs_errno((mcfg_fs_e_type)fs) != EEXIST)
  {
    MSG_MED("CGPS_NV_EFS: mcfg_mkdir /nv/item_files/conf returned %d", mcfg_fs_errno((mcfg_fs_e_type)fs), 0, 0);
    return FALSE;
  }

  (void)GNSS_SNPRINTF(file_name, sizeof(file_name), "%s%s",
                       conf_dir_name3, conf_file_name);
  MSG_SPRINTF_1(MSG_SSID_DFLT, MSG_LEGACY_MED, "CGPS_NV_EFS: file name %s", file_name);

   /* Check if conf file already exists and it has got data in it.
      If conf file does not exist or if it exists and has 0 bytes
      then open the conf file and write the absolute path of NV items in it.
      Conf file is required to access NV items via QXDM/QPST interface.
      If just EFS item file is required conf file need not be updated
      with absolute path. */
  if ( mcfg_fs_stat(file_name, &temp_stat, (mcfg_fs_e_type)fs, (mcfg_fs_sub_id_e_type)subid) != MCFG_FS_STATUS_OK ||
       temp_stat.st_size == 0 )
  {
    /* create new conf file. */
    file_handle = mcfg_fopen(file_name,
                             MCFG_FS_O_CREAT|MCFG_FS_O_WRONLY|MCFG_FS_O_TRUNC,
                             CGPS_NV_EFS_PERMS,
                             (mcfg_fs_e_type)fs,
                             (mcfg_fs_sub_id_e_type)subid);

    if (file_handle == -1)
    {
      MSG_ERROR("CGPS_NV_EFS: Error %d creating config file", mcfg_fs_errno((mcfg_fs_e_type)fs), 0, 0);
      return FALSE;
    }

    /* for convenience, traverse the table backward */
    table_idx = table_end - table_start;
    while ((table_idx >= 0) && (error_detected == FALSE))
    {
      /* Generate the absolute path for the file entries. */
      file_size = cgps_nv_efs_reg_get_item_fname(
                    file_name, sizeof(file_name), TRUE,
                    &cgps_nv_efs_reg_items[table_idx],
                    psz_pathname_prefix);
      bytes2write = file_size;

      /* Since "It is permitted for mcfg_fwrite to write fewer bytes than
         were requested, even if space is available" , the following while
         loop is designed to expect data not to be written all in one go. */
      while ((bytes2write > 0) && (error_detected == FALSE))
      {
        if( (sizeof(file_name) <= file_size-bytes2write) ||
            ((file_size-bytes2write) < 0))
        {
           MSG_ERROR("CGPS_NV_EFS:Incorrect File Name length. Limit is :%d, Current size:%d",
                      sizeof(file_name), file_size-bytes2write, 0);
           error_detected = TRUE;
        }
        else
        {
          bytes_written = mcfg_fwrite(file_handle,
                                      &file_name[file_size-bytes2write],
                                      bytes2write,
                                      (mcfg_fs_e_type)fs);
        }
        if (bytes_written < 0)
        {
          MSG_ERROR("CGPS_NV_EFS: Error %d writing to config file", mcfg_fs_errno((mcfg_fs_e_type)fs), 0, 0);
          error_detected = TRUE;
        }
        else
        {
          bytes2write -= bytes_written;
        }
      }/* while not done writing */
      table_idx--; /* next entry up */
    } /* while table index is valid */

    if (mcfg_fclose(file_handle, (mcfg_fs_e_type)fs) == -1)
    {
      /* Error while closing */
      MSG_ERROR("CGPS_NV_EFS: Error %d closing config file", mcfg_fs_errno((mcfg_fs_e_type)fs), 0, 0);
      error_detected = TRUE;
    } /* mcfg_fclose check */
#endif /* FEATURE_CGPS_LEGACY_NV_API */

    if (error_detected == TRUE)
    {
      return FALSE;
    }
  }

  /* bookkeeping */
  gps_nv_efs_subsys_table[subsys].item_start = table_start;
  gps_nv_efs_subsys_table[subsys].item_end = table_end;
  gps_nv_efs_subsys_table[subsys].table_ptr = cgps_nv_efs_reg_items;
  GNSS_STRLCPY(gps_nv_efs_subsys_table[subsys].sz_pathname_prefix,
              psz_pathname_prefix,
              sizeof(gps_nv_efs_subsys_table[subsys].sz_pathname_prefix));

  return TRUE;
}

/*==============================================================================
  FUNCTION
    cgps_nv_efs_init_internal

  DESCRIPTION
    Helper function for initialization required for NV items in the EFS.
    A config file is created and will hold the EFS file names of all NV items
    in the registry for the given subsystem.

  RETURN VALUE
    TRUE  - Success
    FALSE - Error during initialization
==============================================================================*/
static boolean cgps_nv_efs_init_internal(
  cgps_nv_efs_subsys_type type,
  cgps_nv_efs_reg_item_type table_start,
  cgps_nv_efs_reg_item_type table_end,
  const cgps_nv_efs_reg_item_struct* cgps_nv_efs_reg_items,
  const char* psz_pathname_prefix
)
{
  /* Validate Parameters */
  if (cgps_nv_efs_reg_items == NULL)
  {
    MSG_ERROR("CGPS_NV_EFS_REG: Null table pointer.", 0, 0, 0);
    return FALSE;
  } /* if null table*/

  if (psz_pathname_prefix == NULL)
  {
    MSG_ERROR("CGPS_NV_EFS_REG: Null pathname", 0, 0, 0);
    return FALSE;
  }

  if (table_start > table_end ||
      table_start < CGPS_NV_EFS_MIN_NUM_ITEMS ||
      table_start >= CGPS_NV_EFS_MAX_NUM_ITEMS ||
      table_end   < CGPS_NV_EFS_MIN_NUM_ITEMS ||
      table_end   >= CGPS_NV_EFS_MAX_NUM_ITEMS )
  {
    MSG_ERROR("CGPS_NV_EFS_REG: invalid range: %d, %d", table_start, table_end, 0);
    return FALSE;
  }

  if( type <= CGPS_NV_EFS_SUBSYS_MIN ||
      type >= CGPS_NV_EFS_SUBSYS_MAX )
  {
    MSG_ERROR("CGPS_NV_EFS_REG: invalid type: %d", type, 0, 0);
    return FALSE;
  }

  return (cgps_nv_efs_init_internal_subsid(type,
                                           table_start,
                                           table_end,
                                           cgps_nv_efs_reg_items,
                                           psz_pathname_prefix,
                                           CGPS_NV_FS_TYPE_EFS,
                                           CGPS_NV_SUBID_NO_PREF));
}

/*==============================================================================
  FUNCTION
    cgps_nv_efs_item_read_internal_subsid

  DESCRIPTION
    Read the specified item from EFS2 special items file. If the
    item is not found, a default value is returned. The function logs the read
    write activity.

    This function is Subscription ID and File System aware

  RETURN VALUE
    CGPS_NV_EFS_REG_RW_STAT_OK         - Success
    CGPS_NV_EFS_REG_RW_STAT_RD_DEFAULT - Returning default value
    CGPS_NV_EFS_REG_RW_STAT_RD_ERR     - Error during read operation
    CGPS_NV_EFS_REG_RW_STAT_INVALID    - Invalid paramters
==============================================================================*/
static cgps_nv_efs_reg_rw_stat_enum_type cgps_nv_efs_item_read_internal_subsid(
  cgps_nv_efs_reg_item_type item,
  uint32 data_size,
  void *data_ptr,
  cgps_nv_efs_fs_type fs,
  cgps_nv_efs_sub_id_type subid
)
{
  char file_name[CGPS_MAX_NV_EFS_FILEPATH_SIZE]; /* file name with absolute address */
  #ifdef FEATURE_CGPS_LEGACY_NV_API
  int status = 0;
  #else
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_INVALID_PARAM;
  #endif /* ! FEATURE_CGPS_LEGACY_NV_API */
  const gps_nv_efs_subsys_data_type* entry_ptr = NULL;
  const cgps_nv_efs_reg_item_struct* item_reg_ptr = NULL;

  /* Validate Inputs */
  if( item < CGPS_NV_EFS_MIN_NUM_ITEMS ||
      item >= CGPS_NV_EFS_MAX_NUM_ITEMS)
  {
    MSG_ERROR("CGPS_NV_EFS_READ: Invalid NV Item specified %d", item, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if ((entry_ptr = gps_nv_efs_find_subsys_entry(item)) == NULL)
  {
    MSG_ERROR("CGPS_NV_EFS_READ: subsystem not initialized for item:%d", item, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( data_ptr == NULL )
  {
    MSG_ERROR("CGPS_NV_EFS_READ: data pointer is null", 0, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( fs <= CGPS_NV_FS_TYPE_MIN ||
      fs >= CGPS_NV_FS_TYPE_MAX)
  {
    MSG_ERROR("CGPS_NV_EFS_READ: Invalid File system specified %d", fs, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( subid <= CGPS_NV_SUBID_MIN ||
      subid > CGPS_NV_SUBID_MAX)
  {
    MSG_ERROR("CGPS_NV_EFS_READ: Invalid Subscription ID specified %d", subid, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  item_reg_ptr = &entry_ptr->table_ptr[(int)(item - entry_ptr->item_start)];
  if (data_size != item_reg_ptr->size)
  {
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  /* Generate the absolute path for the file */
  cgps_nv_efs_reg_get_item_fname(file_name, sizeof(file_name), FALSE,
                                 item_reg_ptr, entry_ptr->sz_pathname_prefix);

  /* Attempt to read item.  Function will return proper error message if file not present */
  #ifdef FEATURE_CGPS_LEGACY_NV_API
  status = efs_get(file_name,
                   data_ptr,
                   data_size) ;

  /* If error, efs_get returns -1.  Otherwise, it returns the number of bytes read */
  if (status == -1)
  {
    MSG_ERROR("CGPS_NV_EFS_READ: EFS read error %d, %d", status, efs_errno, 0);
    return CGPS_NV_EFS_REG_RW_STAT_RD_ERR;
  }
  #else
  status = mcfg_fs_read(file_name,
                        data_ptr,
                        data_size,
                        (mcfg_fs_e_type)fs,
                        (mcfg_fs_sub_id_e_type)subid) ;

  if (status != MCFG_FS_STATUS_OK)
  {
    return CGPS_NV_EFS_REG_RW_STAT_RD_ERR;
  }
  #endif /* FEATURE_CGPS_LEGACY_NV_API */

  return CGPS_NV_EFS_REG_RW_STAT_OK;
}

/*==============================================================================
  FUNCTION
    cgps_nv_efs_item_read_internal

  DESCRIPTION
    Read the specified item from EFS2 special items file. If the
    item is not found, a default value is returned. The function logs the read
    write activity.

  RETURN VALUE
    CGPS_NV_EFS_REG_RW_STAT_OK         - Success
    CGPS_NV_EFS_REG_RW_STAT_RD_DEFAULT - Returning default value
    CGPS_NV_EFS_REG_RW_STAT_RD_ERR     - Error during read operation
    CGPS_NV_EFS_REG_RW_STAT_INVALID    - Invalid paramters
==============================================================================*/
static cgps_nv_efs_reg_rw_stat_enum_type cgps_nv_efs_item_read_internal(
  cgps_nv_efs_reg_item_type item,
  uint32 data_size,
  void *data_ptr
)
{
  /* Validate Inputs */
  /* data_size can be any uint32, so no input validation needed. */

  if( item < CGPS_NV_EFS_MIN_NUM_ITEMS ||
      item >= CGPS_NV_EFS_MAX_NUM_ITEMS)
  {
    MSG_ERROR("CGPS_NV_EFS_READ: Invalid NV Item specified %d", item, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( data_ptr == NULL )
  {
    MSG_ERROR("CGPS_NV_EFS_READ: data pointer is null", 0, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  return (cgps_nv_efs_item_read_internal_subsid(item,
                                                data_size,
                                                data_ptr,
                                                CGPS_NV_FS_TYPE_EFS,
                                                CGPS_NV_SUBID_NO_PREF));
}

/*==============================================================================
  FUNCTION
    cgps_nv_efs_reg_item_write_internal_subsid

  DESCRIPTION
    Write the specified item value to EFS2 special items file. The function
    logs the write activity.

    This function is Subscription ID and File System aware

  RETURN VALUE
    CGPS_NV_EFS_REG_RW_STAT_OK     - Success
    CGPS_NV_EFS_REG_RW_STAT_WR_ERR - Error during write operation
    CGPS_NV_EFS_REG_RW_STAT_INVALID - Invalid paramters
==============================================================================*/
static cgps_nv_efs_reg_rw_stat_enum_type cgps_nv_efs_reg_item_write_internal_subsid(
  cgps_nv_efs_reg_item_type item,
  uint32 data_size,
  const void *data_ptr,
  cgps_nv_efs_fs_type fs,
  cgps_nv_efs_sub_id_type subid
)
{
  char file_name[CGPS_MAX_NV_EFS_FILEPATH_SIZE]; /* file name with absolute address */
  #ifdef FEATURE_CGPS_LEGACY_NV_API
  int status = 0;
  #else
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_INVALID_PARAM;
  #endif /* FEATURE_CGPS_LEGACY_NV_API */
  const gps_nv_efs_subsys_data_type* entry_ptr = NULL;
  const cgps_nv_efs_reg_item_struct* item_reg_ptr = NULL;

  /* Validate Inputs */
  if( item < CGPS_NV_EFS_MIN_NUM_ITEMS ||
      item >= CGPS_NV_EFS_MAX_NUM_ITEMS)
  {
    MSG_ERROR("CGPS_NV_EFS_WRITE: Invalid NV Item specified %d", item, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if ((entry_ptr = gps_nv_efs_find_subsys_entry(item)) == NULL)
  {
    MSG_ERROR("CGPS_NV_EFS_WRITE: subsystem not initialized for item:%d", item, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( data_ptr == NULL )
  {
    MSG_ERROR("CGPS_NV_EFS_WRITE: data pointer is null", 0, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( fs <= CGPS_NV_FS_TYPE_MIN ||
      fs >= CGPS_NV_FS_TYPE_MAX)
  {
    MSG_ERROR("CGPS_NV_EFS_WRITE: Invalid File system specified %d", fs, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( subid <= CGPS_NV_SUBID_MIN ||
      subid > CGPS_NV_SUBID_MAX)
  {
    MSG_ERROR("CGPS_NV_EFS_WRITE: Invalid Subscription ID specified %d", subid, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  item_reg_ptr = &entry_ptr->table_ptr[(int)(item - entry_ptr->item_start)];
  if (data_size != item_reg_ptr->size)
  {
    MSG_ERROR("CGPS_NV_EFS_WRITE: Destination buffer size incorrect", 0, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  /* Generate the absolute path for the file */
  cgps_nv_efs_reg_get_item_fname(file_name, sizeof(file_name), FALSE,
                                     item_reg_ptr, entry_ptr->sz_pathname_prefix);

  /* Attempt to write item.  Function will return proper error message if file already present */
  #ifdef FEATURE_CGPS_LEGACY_NV_API
  status = efs_put(file_name,
                   (void*) data_ptr,
                   data_size,
                   O_CREAT|O_WRONLY|O_TRUNC,
                   CGPS_NV_EFS_PERMS);
  if (status != ENOERR)
  {
    MSG_ERROR("CGPS_NV_EFS_WRITE: EFS write error %d, %d", status, efs_errno, 0);
    return CGPS_NV_EFS_REG_RW_STAT_WR_ERR;
  }
  #else
  status = mcfg_fs_write(file_name,
                         (void*) data_ptr,
                         data_size,
                         MCFG_FS_O_CREAT|MCFG_FS_O_AUTODIR|MCFG_FS_O_TRUNC,
                         CGPS_NV_EFS_PERMS,
                         (mcfg_fs_e_type)fs,
                         (mcfg_fs_sub_id_e_type)subid);
  if (status != MCFG_FS_STATUS_OK)
  {
    MSG_ERROR("CGPS_NV_EFS_WRITE: EFS write error %d, %d", status, mcfg_fs_errno((mcfg_fs_e_type)fs), 0);
    return CGPS_NV_EFS_REG_RW_STAT_WR_ERR;
  }
  #endif /* FEATURE_CGPS_LEGACY_NV_API */

  return CGPS_NV_EFS_REG_RW_STAT_OK;
}

/*==============================================================================
  FUNCTION
    cgps_nv_efs_reg_item_write_internal

  DESCRIPTION
    Write the specified item value to EFS2 special items file. The function
    logs the write activity.

  RETURN VALUE
    CGPS_NV_EFS_REG_RW_STAT_OK     - Success
    CGPS_NV_EFS_REG_RW_STAT_WR_ERR - Error during write operation
    CGPS_NV_EFS_REG_RW_STAT_INVALID - Invalid paramters
==============================================================================*/
static cgps_nv_efs_reg_rw_stat_enum_type cgps_nv_efs_reg_item_write_internal(
  cgps_nv_efs_reg_item_type item,
  uint32 data_size,
  const void *data_ptr
)
{
  /* Validate Inputs */
  /* data_size can be any uint32, so no input validation needed. */

  if( item < CGPS_NV_EFS_MIN_NUM_ITEMS ||
      item >= CGPS_NV_EFS_MAX_NUM_ITEMS)
  {
    MSG_ERROR("CGPS_NV_EFS_READ: Invalid NV Item specified %d", item, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( data_ptr == NULL )
  {
    MSG_ERROR("CGPS_NV_EFS_READ: data pointer is null", 0, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  return( cgps_nv_efs_reg_item_write_internal_subsid( item,
                                                      data_size,
                                                      data_ptr,
                                                      CGPS_NV_FS_TYPE_EFS,
                                                      CGPS_NV_SUBID_NO_PREF ) );
}

/*XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

                            Public Functions

XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/

/*==============================================================================
  FUNCTION
    cgps_nv_efs_init

  DESCRIPTION
    Performs initialization required for CGPS NV items in the EFS.
    Currently this means generating the config file for diag tools

    Requires a table of the NV items that need to be input to the config file

  RETURN VALUE
    TRUE  - Success
    FALSE - Error during initialization
==============================================================================*/
boolean cgps_nv_efs_init(cgps_nv_efs_subsys_type type,
                         cgps_nv_efs_reg_item_type table_start,
                         cgps_nv_efs_reg_item_type table_end,
                         const cgps_nv_efs_reg_item_struct* cgps_nv_efs_reg_items)
{
  MSG_MED("CGPS_NV_EFS: Registry Initialization", 0, 0, 0);

  /* Validate Inputs */
  if( type <= CGPS_NV_EFS_SUBSYS_MIN ||
      type >= CGPS_NV_EFS_SUBSYS_MAX )
  {
    MSG_ERROR("CGPS_NV_EFS_INIT: Invalid type: %d", type, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( table_start <  CGPS_NV_EFS_MIN_NUM_ITEMS ||
      table_end   >= CGPS_NV_EFS_MAX_NUM_ITEMS ||
      table_start >  table_end
    )
  {
    MSG_ERROR("CGPS_NV_EFS_INIT: Invalid table indexes start:%d, end:%d",
              (int) table_start, (int) table_end, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( cgps_nv_efs_reg_items == NULL )
  {
    MSG_ERROR("CGPS_NV_EFS_READ: NULL Registry pointer", 0, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  return (cgps_nv_efs_init_internal(type, table_start, table_end,
                                     cgps_nv_efs_reg_items, "cgps"));

}  /* end of function cgps_nv_efs_init*/

/*==============================================================================
  FUNCTION
    cgps_nv_efs_init_subsid

  DESCRIPTION
    Performs initialization required for CGPS NV items in the EFS.
    Currently this means generating the config file for diag tools.

    Requires a table of the NV items that need to be input to the config file.

	This function is subscription ID aware and assumes EFS file system.

  RETURN VALUE
    TRUE  - Success
    FALSE - Error during initialization
==============================================================================*/
boolean cgps_nv_efs_init_subsid (cgps_nv_efs_subsys_type type,
                                 cgps_nv_efs_reg_item_type table_start,
                                 cgps_nv_efs_reg_item_type table_end,
                                 const cgps_nv_efs_reg_item_struct  *cgps_nv_efs_reg_items,
                                 cgps_nv_efs_sub_id_type subid
                                )
{
  MSG_MED("CGPS_NV_EFS: Registry Initialization", 0, 0, 0);

  /* Validate Inputs */
  if( type <= CGPS_NV_EFS_SUBSYS_MIN ||
      type >= CGPS_NV_EFS_SUBSYS_MAX )
  {
    MSG_ERROR("CGPS_NV_EFS_INIT: Invalid type: %d", type, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( table_start <  CGPS_NV_EFS_MIN_NUM_ITEMS ||
      table_end   >= CGPS_NV_EFS_MAX_NUM_ITEMS ||
      table_start >  table_end
    )
  {
    MSG_ERROR("CGPS_NV_EFS_INIT: Invalid table indexes. start:%d, end:%d",
              (int) table_start, (int) table_end, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( cgps_nv_efs_reg_items == NULL )
  {
    MSG_ERROR("CGPS_NV_EFS_READ: NULL Registry pointer", 0, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( subid <= CGPS_NV_SUBID_MIN ||
      subid >  CGPS_NV_SUBID_MAX )
  {
    MSG_ERROR("CGPS_NV_EFS_READ: Invalid Sub ID %d", subid, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  return cgps_nv_efs_init_internal_subsid(type,
                                          table_start,
                                          table_end,
                                          cgps_nv_efs_reg_items,
                                          "cgps",
                                          CGPS_NV_FS_TYPE_EFS,
                                          subid);
}  /* end of function cgps_nv_efs_init*/

/*==============================================================================
  FUNCTION
    cgps_nv_efs_reg_item_read

  DESCRIPTION
    Read the specified item from EFS2 special items file. If the
    item is not found, a default value is returned. The function logs the read
    write activity.

  RETURN VALUE
    CGPS_NV_EFS_REG_RW_STAT_OK         - Success
    CGPS_NV_EFS_REG_RW_STAT_RD_DEFAULT - Returning default value
    CGPS_NV_EFS_REG_RW_STAT_RD_ERR     - Error during read operation
    CGPS_NV_EFS_REG_RW_STAT_INVALID    - Invalid paramters
==============================================================================*/
cgps_nv_efs_reg_rw_stat_enum_type cgps_nv_efs_reg_item_read(cgps_nv_efs_reg_item_type const item,
                                                            uint32 data_size,
                                                            void *data_ptr,
                                                            cgps_nv_efs_reg_item_type table_start,
                                                            cgps_nv_efs_reg_item_type table_end,
                                                            const cgps_nv_efs_reg_item_struct  *cgps_nv_efs_reg_items
                                                           )
{
  /* Validate Inputs */
  /* data_size can be any uint32, so no input validation needed. */

  if( table_start <  CGPS_NV_EFS_MIN_NUM_ITEMS ||
      table_end   >= CGPS_NV_EFS_MAX_NUM_ITEMS ||
      table_start >  table_end ||
      item < table_start ||
      item > table_end
    )
  {
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( data_ptr == NULL )
  {
    MSG_ERROR("CGPS_NV_EFS_READ: data pointer is null", 0, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( cgps_nv_efs_reg_items == NULL )
  {
    MSG_ERROR("CGPS_NV_EFS_READ: NULL Registry pointer for item:%d", item, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  return (cgps_nv_efs_item_read_internal(item, data_size, data_ptr));
}

/*==============================================================================
  FUNCTION
    cgps_nv_efs_reg_item_read_subsid

  DESCRIPTION
    Read the specified item from EFS2 special items file.
    The function logs the read write activity. This is a generic read function.
    If this function returns anything other than CGPS_NV_EFS_REG_RW_STAT_OK,
    the caller must set this item to default value if needed.

    This function is subscription ID aware and assumes EFS file system.

  RETURN VALUE
    CGPS_NV_EFS_REG_RW_STAT_OK         - Success
    CGPS_NV_EFS_REG_RW_STAT_RD_ERR     - Error during read operation. if file
                                         does not exist, returns an error
    CGPS_NV_EFS_REG_RW_STAT_INVALID    - Invalid paramters
==============================================================================*/
cgps_nv_efs_reg_rw_stat_enum_type cgps_nv_efs_reg_item_read_subsid(cgps_nv_efs_reg_item_type const item,
                                                                   uint32 data_size,
                                                                   void *data_ptr,
                                                                   cgps_nv_efs_reg_item_type table_start,
                                                                   cgps_nv_efs_reg_item_type table_end,
                                                                   const cgps_nv_efs_reg_item_struct  *cgps_nv_efs_reg_items,
                                                                   cgps_nv_efs_sub_id_type subid
                                                                  )
{
  /* Validate Inputs */
  /* data_size can be any uint32, so no input validation needed. */

  if( table_start <  CGPS_NV_EFS_MIN_NUM_ITEMS ||
      table_end   >= CGPS_NV_EFS_MAX_NUM_ITEMS ||
      table_start >  table_end ||
      item < table_start ||
      item > table_end
    )
  {
    MSG_ERROR("CGPS_NV_EFS_READ: Invalid item index, start:%d, end:%d, item:%d",
              (int) table_start, (int) table_end, (int) item);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( data_ptr == NULL )
  {
    MSG_ERROR("CGPS_NV_EFS_READ: data pointer is null", 0, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( cgps_nv_efs_reg_items == NULL )
  {
    MSG_ERROR("CGPS_NV_EFS_READ: NULL Registry pointer for item:%d", item, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( subid <= CGPS_NV_SUBID_MIN ||
      subid > CGPS_NV_SUBID_MAX )
  {
    MSG_ERROR("CGPS_NV_EFS_READ: Invalid Sub ID %d", subid, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  return (cgps_nv_efs_item_read_internal_subsid(item,
                                                data_size,
                                                data_ptr,
                                                CGPS_NV_FS_TYPE_EFS,
                                                subid));
}

/*==============================================================================
  FUNCTION
    cgps_nv_efs_reg_item_write

  DESCRIPTION
    Write the specified item value to EFS2 special items file. The function
    logs the write activity.

  RETURN VALUE
    CGPS_NV_EFS_REG_RW_STAT_OK     - Success
    CGPS_NV_EFS_REG_RW_STAT_WR_ERR - Error during write operation
    CGPS_NV_EFS_REG_RW_STAT_INVALID - Invalid paramters
==============================================================================*/
cgps_nv_efs_reg_rw_stat_enum_type cgps_nv_efs_reg_item_write(cgps_nv_efs_reg_item_type const item,
                                                             uint32 data_size,
                                                             const void *data_ptr,
                                                             cgps_nv_efs_reg_item_type table_start,
                                                             cgps_nv_efs_reg_item_type table_end,
                                                             const cgps_nv_efs_reg_item_struct  *cgps_nv_efs_reg_items)
{
  MSG_MED("CGPS_NV_EFS_REG: Registry Item Write Started for item:%d", item, 0, 0);

  /* Validate Inputs */
  /* data_size can be any uint32, so no input validation needed. */

  if( table_start <  CGPS_NV_EFS_MIN_NUM_ITEMS ||
      table_end   >= CGPS_NV_EFS_MAX_NUM_ITEMS ||
      table_start >  table_end ||
      item < table_start ||
      item > table_end
    )
  {
    MSG_ERROR("CGPS_NV_EFS_WRITE: Invalid item index, start:%d, end:%d, item:%d",
              (int) table_start, (int) table_end, (int) item);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( data_ptr == NULL )
  {
    MSG_ERROR("CGPS_NV_EFS_WRITE: data pointer is null", 0, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( cgps_nv_efs_reg_items == NULL )
  {
    MSG_ERROR("CGPS_NV_EFS_WRITE: NULL Registry pointer for item:%d", item, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  return (cgps_nv_efs_reg_item_write_internal(item, data_size, data_ptr));
} /* end of write function */

/*==============================================================================
  FUNCTION
    cgps_nv_efs_reg_item_write_subsid

  DESCRIPTION
    Write the specified item value to EFS2 special items file. The function
    logs the write activity. This is a generic write function.

    This function is subscription ID aware and assumes EFS file system.

  RETURN VALUE
    CGPS_NV_EFS_REG_RW_STAT_OK     - Success
    CGPS_NV_EFS_REG_RW_STAT_WR_ERR - Error during write operation
    CGPS_NV_EFS_REG_RW_STAT_INVALID - Invalid paramters
==============================================================================*/
cgps_nv_efs_reg_rw_stat_enum_type cgps_nv_efs_reg_item_write_subsid(cgps_nv_efs_reg_item_type const item,
                                                                    uint32 data_size,
                                                                    const void *data_ptr,
                                                                    cgps_nv_efs_reg_item_type table_start,
                                                                    cgps_nv_efs_reg_item_type table_end,
                                                                    const cgps_nv_efs_reg_item_struct  *cgps_nv_efs_reg_items,
                                                                    cgps_nv_efs_sub_id_type subid
                                                                   )
{
  MSG_MED("CGPS_NV_EFS_WRITE: Registry Item Write Started for item:%d", item, 0, 0);

  /* Validate Inputs */
  /* data_size can be any uint32, so no input validation needed. */

  if( table_start <  CGPS_NV_EFS_MIN_NUM_ITEMS ||
      table_end   >= CGPS_NV_EFS_MAX_NUM_ITEMS ||
      table_start >  table_end ||
      item < table_start ||
      item > table_end
    )
  {
    MSG_ERROR("CGPS_NV_EFS_WRITE: Invalid item index, start:%d, end:%d, item:%d",
              (int) table_start, (int) table_end, (int) item);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( data_ptr == NULL )
  {
    MSG_ERROR("CGPS_NV_EFS_WRITE: data pointer is null", 0, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( cgps_nv_efs_reg_items == NULL )
  {
    MSG_ERROR("CGPS_NV_EFS_WRITE: NULL Registry pointer for item:%d", item, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  if( subid <= CGPS_NV_SUBID_MIN ||
      subid > CGPS_NV_SUBID_MAX )
  {
    MSG_ERROR("CGPS_NV_EFS_WRITE: Invalid Sub ID %d", subid, 0, 0);
    return CGPS_NV_EFS_REG_RW_STAT_INVALID;
  }

  return (cgps_nv_efs_reg_item_write_internal_subsid(item,
                                                     data_size,
                                                     data_ptr,
                                                     CGPS_NV_FS_TYPE_EFS,
                                                     subid));
} /* end of write function */

/*==============================================================================
  FUNCTION
    gps_nv_efs_init

  DESCRIPTION
    Performs initialization required for NV items in the EFS.
    Currently this means generating the config file for diag tools.
    Requires a table of the NV items that need to be input to the config file.

    Unlike cgps_nv_efs_init() which uses the hardcoded name "cgps" in the NV
    item's pathnames, the input parameter psz_pathname_prefix in this function
    allows caller to specify the name to be used as the pathname prefix.

  RETURN VALUE
    TRUE  - Success
    FALSE - Error during initialization
==============================================================================*/
boolean gps_nv_efs_init (cgps_nv_efs_subsys_type subsys,
                         cgps_nv_efs_reg_item_type table_start,
                         cgps_nv_efs_reg_item_type table_end,
                         const cgps_nv_efs_reg_item_struct* cgps_nv_efs_reg_items,
                         const char* psz_pathname_prefix)
{
  MSG_MED("GPS_NV_EFS: Registry Init Started for subsystem:%d", subsys, 0, 0);
  return (cgps_nv_efs_init_internal(subsys, table_start, table_end,
                                    cgps_nv_efs_reg_items,
                                    psz_pathname_prefix));
}

/*==============================================================================
  FUNCTION
    gps_nv_efs_reg_item_read

  DESCRIPTION
    Reads the specified item from EFS2 special items file.

    The subsystem that the given item belongs to must have already been
    initialized, or this function will return CGPS_NV_EFS_REG_RW_STAT_INVALID.

    If the item EFS file is not found, the default value for the item is returned.

  RETURN VALUE
    CGPS_NV_EFS_REG_RW_STAT_OK         - Success
    CGPS_NV_EFS_REG_RW_STAT_RD_ERR     - Error during read operation. if file
                                         does not exist, returns an error
    CGPS_NV_EFS_REG_RW_STAT_INVALID    - Invalid paramters
==============================================================================*/
cgps_nv_efs_reg_rw_stat_enum_type gps_nv_efs_reg_item_read(
  cgps_nv_efs_reg_item_type const item,
  uint32 data_size,
  void *data_ptr
)
{
  return (cgps_nv_efs_item_read_internal(item, data_size, data_ptr));
}

/*==============================================================================
  FUNCTION
    gps_nv_efs_reg_item_write

  DESCRIPTION
    Write the specified item value to EFS2 special items file.

    The subsystem that the given item belongs to must have already been
    initialized, or this function will return CGPS_NV_EFS_REG_RW_STAT_INVALID.

  RETURN VALUE
    CGPS_NV_EFS_REG_RW_STAT_OK     - Success
    CGPS_NV_EFS_REG_RW_STAT_WR_ERR - Error during write operation
    CGPS_NV_EFS_REG_RW_STAT_INVALID - Invalid paramters
==============================================================================*/
cgps_nv_efs_reg_rw_stat_enum_type gps_nv_efs_reg_item_write(
  cgps_nv_efs_reg_item_type const item,
  uint32 data_size,
  const void *data_ptr
)
{
  MSG_MED("GPS_NV_EFS: Registry Item Read Started for item:%d", item, 0, 0);
  return (cgps_nv_efs_reg_item_write_internal(item, data_size, data_ptr));
}

#endif /* FEATURE_CGPS */

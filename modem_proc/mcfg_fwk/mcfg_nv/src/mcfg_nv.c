/*==============================================================================

          MODEM_CFG  -   N O N - V O L A T I L E   S T O R A G E

GENERAL DESCRIPTION
  Simple APIs to access ModemCfg-specific NV settings.  Supports both new-style 
  MCFG NV items and old-style NV items, though it is recommended that 
  both exisiting and new code accessing old-style items use the legacy NV API
  when possible.  

EXTERNALIZED FUNCTIONS
  mcfg_nv_read
  mcfg_nv_write

INITIALIZATION AND SEQUENCING REQUIREMENTS
  EFS must be initialized

Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_nv/src/mcfg_nv.c#1 $
$DateTime: 2016/12/13 08:00:27 $
$Author: mplcsds1 $
$Change: 11985219 $

when     who  what, where, why
-------- ---  ---------------------------------------------------------------- 
01/09/14 sk   Fix for CR596963
02/14/12 sbt  Initial creation

==============================================================================*/

/*==============================================================================

                      INCLUDE FILES FOR MODULE

==============================================================================*/

#include <stringl/stringl.h>
#include "fs_errno.h"
#include "fs_public.h"
#include "nv_items.h"
#include "mcfg_feature_config.h"
#include "mcfg_common.h"
#include "mcfg_nv.h"
#include "mcfg_osal.h"
#include "mcfg_int.h"

/*==============================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Private Constants and Macros
==============================================================================*/


/* Directories where NV items will be stored */
#define MCFG_NV_OLD_DIR           "/nvm/num/"
#define MCFG_NV_OLD_DIR_CONTEXT   "/nvm/context"
#define MCFG_NV_NEW_DIR           "/nv/item_files/mcfg/"
#define MCFG_NV_NEW_DIR_CONTEXT   "/nv/item_files/mcfg/context"

/* Path where mcfg.conf file will be stored */
#define MCFG_NV_CONF_PATH         "/nv/item_files/conf/mcfg.conf"

/* Converts a new-style MCFG NV item (mcfg_nv_item_e_type) into the index
 * in the mcfg_nv_filenames array that gives its EFS filename. */
#define MCFG_NV_FNAME_INDEX(item) (item - MCFG_NVI_ITEM_FILES_START - 1)

/* Total number of new-style NV items */
#define MCFG_NV_NUM_NEW_ITEMS MCFG_NV_FNAME_INDEX(MCFG_NVI_ITEM_FILES_END)

/* Maximum length of the filename (not including directory info) of new-style 
 * MCFG NV items.  Includes NULL term.  Also change MCFG_NV_MAX_EFS_PATH_LEN
 * if modifying this value. */
#define MCFG_NV_FILENAME_MAX_LEN  (32)

/*==============================================================================
  Private Typedefs
==============================================================================*/

typedef struct mcfg_nv_indexed_item_info_s {
  uint32  item;
  uint8   max_index; 
} mcfg_nv_indexed_item_info_s_type;

/*==============================================================================
  Private Variables
==============================================================================*/

/* Filename for new-style MCFG NV items. Must have same order as the enum
 * mcfg_nv_item_e_type. EFS item files are created in the MCFG NV directory
 * with this filename, e.g. "/nv/item_files/image_pref" */ 
static const char mcfg_nv_filenames[MCFG_NV_NUM_NEW_ITEMS][MCFG_NV_FILENAME_MAX_LEN] =
{
  /* NOTE: see the definition of mcfg_nv_item_e_type in mcfg_nv.h for 
   * instructions on adding new NV items */
  "mcfg_sw_version",                /* MCFG_NV_ITEM_SW_VERSION_I */
  "mcfg_hw_version",                /* MCFG_NV_ITEM_HW_VERSION_I */
  "muxd_consl_profile_dir_index",   /* MCFG_NV_ITEM_MUXD_CONSL_PROFILE_DIR_INDEX */
  "mcfg_sw_muxd_version",           /* MCFG_NV_ITEM_MCFG_SW_MUXD_VERSION_I */
  "mcfg_sw_pref",                   /* MCFG_NV_ITEM_MCFG_SW_PREF */
  "mcfg_hw_pref",                   /* MCFG_NV_ITEM_MCFG_HW_PREF */
  "mcfg_test_indexed_item",         /* MCFG_NV_ITEM_TEST_MCFG_INDEXED_PROF */
  "mcfg_autoselect_by_uim",         /* MCFG_NV_ITEM_AUTOSELECT_BY_UIM */
  "mcfg_last_autoselect_iccid",     /* MCFG_NV_ITEM_LAST_AUTOSELECT_ICCID */
  "mcfg_def_config_sw_version",     /* MCFG_NV_ITEM_DEF_CONFIG_SW_VERSION_I */
  "mcfg_def_config_hw_version",     /* MCFG_NV_ITEM_DEF_CONFIG_HW_VERSION_I */
  "mcfg_def_config_enable",         /* MCFG_NV_ITEM_DEF_CONFIG_ENABLE_I */
  "mcfg_def_config_force_load",     /* MCFG_NV_ITEM_DEF_CONFIG_FORCE_LOAD_I */
  "mcfg_config_force_load",         /* MCFG_NV_ITEM_CONFIG_FORCE_LOAD_I */
  "mcfg_segload_config",            /* MCFG_NV_ITEM_SEGLOAD_CONFIG_I */
  "mcfg_last_autoselect_plmn",      /* MCFG_NV_ITEM_LAST_AUTOSELECT_PLMN */
  "mcfg_uim_slot_mapping",          /* MCFG_NV_ITEM_UIM_SLOT_MAPPING */
  "mcfg_refresh_timer",             /* MCFG_NV_ITEM_REFRESH_TIMER */
  "mcfg_uim_slot_mapping_timer",    /* MCFG_NV_ITEM_UIM_SLOT_MAPPING_TIMER */
  "mcfg_multi_mbn_config_version",  /* MCFG_NV_ITEM_MULTI_MBN_CONFIG_VERSION_I*/
  "mcfg_setting",                   /* MCFG_NV_ITEM_SETTING */
  "mcfg_power_down_reason",         /* MCFG_NV_ITEM_MCFG_POWER_DOWN_REASON*/
/*"-------------------------------" the filename must fit within these quotes */
};

/* Table listing which new-style MCFG NV items are indexed, and the maximum 
 * index value allowed for the item */
static const mcfg_nv_indexed_item_info_s_type mcfg_nv_indexed_items[] =
{
  /* item number, maximum index value (i.e. array size - 1) */
  { MCFG_NV_ITEM_TEST_MCFG_INDEXED_PROF,   7 },
  { MCFG_NV_ITEM_MCFG_SW_MUXD_VERSION_I,   255 },
};

/*==============================================================================
  Private Function Prototypes
==============================================================================*/
#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT
static uint32 mcfg_nv_add_muxing(
  char * buf,
  uint32 size
  );
#endif /* FEATURE_MODEM_CFG_MUX_SUPPORT */

static uint8 mcfg_nv_get_max_index(
  uint32  item
  );

/*==============================================================================

                    FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Functions for internal module use only
==============================================================================*/

#ifdef FEATURE_MODEM_CFG_MUX_SUPPORT
/*==========================================================================

  FUNCTION mcfg_nv_add_muxing

  DESCRIPTION
    Adds multiplexing characters to EFS path if the item should be
    multiplexed according to the MCFG_SW_CFG.
 
  DEPENDENCIES
    MCFG_SW_CFG has already been processed

  PARAMETERS
    item [in] item number used to check if it should be multiplexed
    buf [out] location where multiplexing should be added
    size [in] size remaining in buffer
    
  RETURN VALUE
    Number of bytes added to the string buffer - will be 0 if not
    multiplexed

  SIDE EFFECTS
    None.

===========================================================================*/
static uint32 mcfg_nv_add_muxing(
  char * buf,
  uint32 size
  )
{
  uint32 bytes_added = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If this is a multiplexed new-style NV Item, update the path with the 
   * multiplexed carrier index */

  uint8 carrier_index = mcfg_get_muxd_index();
  bytes_added = snprintf(buf, size, "_S%d", carrier_index);

  return bytes_added;
} /* mcfg_nv_add_muxing() */
#endif /* FEATURE_MODEM_CFG_MUX_SUPPORT */

/*==========================================================================

  FUNCTION mcfg_nv_get_max_index

  DESCRIPTION
    Given a new-style MCFG NV item, returns the maximum allowed index
    value.  If the item is not indexed, this value is 0, otherwise the
    value from mcfg_nv_indexed_items[] is used.

  DEPENDENCIES
    path_len must be >= MCFG_NV_MAX_EFS_PATH_LEN

  PARAMETERS
    item     [in] NV item: can be nv_items_enum_type or mcfg_nv_item_e_type
    index    [in] index of the NV item, or 0 if no index
    path    [out] EFS path to NV item file
    path_len [in] size of buffer at data_ptr
    
  RETURN VALUE
    Maximum allowed value for index parameter for the given item

  SIDE EFFECTS
    None.

===========================================================================*/
static uint8 mcfg_nv_get_max_index(
  uint32  item
  )
{
  uint32 i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Linear search is used currently because the indexed items table is 
   * very small.  This table will be kept ordered, so if the lookup speed
   * becomes a concern, this can easily change to binary search. */
  for (i = 0; i < ARR_SIZE(mcfg_nv_indexed_items); i++)
  {
    if (item == mcfg_nv_indexed_items[i].item)
    {
      return mcfg_nv_indexed_items[i].max_index;
    }
  }
  return 0;
} /* mcfg_nv_get_max_index() */

/*==========================================================================

  FUNCTION mcfg_nv_build_path

  DESCRIPTION
    This function turns an NV item and an index into a path to a file in 
    EFS.  Performs validation of the item and index.

  DEPENDENCIES
    path_len must be >= MCFG_NV_MAX_EFS_PATH_LEN

  PARAMETERS
    item         [in] NV item: can be nv_items_enum_type or
                      mcfg_nv_item_e_type
    index        [in] index of the NV item, or 0 if no index
    new_indexed [out] will be set to TRUE if the item is new-style and
                      indexed. can be NULL
    path        [out] EFS path to NV item file
    path_len     [in] size of buffer at data_ptr
    
    
  RETURN VALUE
    MCFG_NV_STATUS_OK on success
    MCFG_NV_STATUS_BAD_ITEM if item is not in old NV or new MCFG NV range,
        or index is outside defined limit (new-style items only)

  SIDE EFFECTS
    None.

===========================================================================*/
mcfg_nv_status_e_type mcfg_nv_build_path(
  uint32    item,
  uint8     index,
  uint8     subs_id,
  boolean * new_indexed,
  char *    path,
  uint32    path_len
  )
{
  uint32 path_offset;
  mcfg_nv_status_e_type status;

  uint8 max_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  path_offset = 0;
  status = MCFG_NV_STATUS_OK;

  if(subs_id >= MCFG_NUM_OF_SUB_ID)
  {
    return MCFG_NV_STATUS_BAD_ITEM; 
  }

  /* New-style items */
  if (item > MCFG_NVI_ITEM_FILES_START && item < MCFG_NVI_ITEM_FILES_END)
  {
    max_index = mcfg_nv_get_max_index(item);
    if (index > max_index)
    {
      status = MCFG_NV_STATUS_BAD_ITEM;
    }
    else
    {
      if (subs_id !=0 )
      {
        path_offset  = strlcpy(path, MCFG_NV_NEW_DIR, path_len);
        path_offset += strlcpy(&path[path_offset],
                                mcfg_nv_filenames[MCFG_NV_FNAME_INDEX(item)],
                                path_len - path_offset);
        path_offset += snprintf(&path[path_offset], path_len - path_offset,
                                "_Subscription%02d", (int)subs_id);
      }
      else
      {
        /* Copy in new-style directory, UQCN muxing (if any), and filename */
        path_offset  = strlcpy(path, MCFG_NV_NEW_DIR, path_len);
        path_offset += strlcpy(&path[path_offset],
                                mcfg_nv_filenames[MCFG_NV_FNAME_INDEX(item)],
                                path_len - path_offset);
      }
    }

    if (new_indexed != NULL)
    {
      if(item == MCFG_NV_ITEM_MCFG_SW_MUXD_VERSION_I)
      {
        *new_indexed = FALSE;
      }
      else
      {
      *new_indexed = (max_index != 0);
    }
  }
  }
  else /* item is NOT new-style */
  {
    max_index = 0;
    if (item >= NV_MAX_I)
    {
      status = MCFG_NV_STATUS_BAD_ITEM;
    }
    else /* item is old-style */
    {
      /* Copy in old-style directory + item number as filename */
      if (subs_id !=0 )
      {
        path_offset  = strlcpy(path, MCFG_NV_OLD_DIR_CONTEXT, path_len);
        path_offset += snprintf(&path[path_offset], path_len - path_offset,
                                "%d/", (int)subs_id);
        path_offset += snprintf(&path[path_offset], path_len - path_offset,
                                "%d", (int)item);

      }
      else
      {
        path_offset  = strlcpy(path, MCFG_NV_OLD_DIR, path_len);
        path_offset += snprintf(&path[path_offset], path_len - path_offset,
                                "%d", (int)item);
      }
    }

    if (new_indexed != NULL)
    {
      *new_indexed = FALSE;
    }
  }
  
  /* Append item index if non-zero */
  if (status == MCFG_NV_STATUS_OK && index != 0)
  {
    /* Note that we purposefully do not check the index range for old-style
     * items since that data is maintained internally in the NV layer, to avoid CBSP dependency! */
    path_offset += snprintf(&path[path_offset], path_len - path_offset,
                              "_%d",index);
  }

  return status;
} /* mcfg_nv_build_path() */


/*==============================================================================
  Functions exported in public header files
==============================================================================*/

/*==========================================================================

  FUNCTION mcfg_nv_delete

  DESCRIPTION
    This function deletes an NV item from the EFS.

    Blocks the calling task until the EFS operation is complete.  This 
    involves acquiring the FS global lock, which may be held by another 
    task.  Therefore this function must not be called from a task-locked 
    context, otherwise a deadlock could result.

  DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    item      [in] NV item to delete: can be nv_items_enum_type or 
                   mcfg_nv_item_e_type
    index     [in] index of the NV item to delete, or 0 if no index
    subs_id   [in] Subscription ID for the Multi-SIM NV item to delete
                   or 0 if not a multi-SIM NV (if unsure, use 0)
 
  RETURN VALUE
    MCFG_NV_STATUS_OK on success or if file does not exist
    MCFG_NV_STATUS_BAD_ITEM if item is not in old NV or new MCFG NV range,
        or index is outside defined limit (new-style items only)
    MCFG_NV_STATUS_EFS_ERR on EFS error

  SIDE EFFECTS
    Waits on fs_global_lock() - see description & dependencies

===========================================================================*/
mcfg_nv_status_e_type mcfg_nv_delete(
  uint32  item,
  uint8   index,
  uint8   subs_id
  )
{
  char path[MCFG_NV_MAX_EFS_PATH_LEN];
  int  efs_ret;
  mcfg_nv_status_e_type status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  status = mcfg_nv_build_path(item, index, subs_id, NULL, path, sizeof(path));

  if (status == MCFG_NV_STATUS_OK)
  {
    efs_ret = efs_unlink(path);

    /* Only error if deleting failed - if the file didn't exist in the first 
     * place then we are fine */
    if (efs_ret == -1 && efs_errno != ENOENT)
    {
      status = MCFG_NV_STATUS_EFS_ERR;
    }
  }

  return status;
} /* mcfg_nv_delete() */

/*==========================================================================

  FUNCTION mcfg_nv_is_indexed

  DESCRIPTION
    Returns whether the new-style MCFG NV item is indexed (i.e. is an array
    of NV items).

  DEPENDENCIES
    None.

  PARAMETERS
    item  [in] NV item to check: can be nv_items_enum_type or 
               mcfg_nv_item_e_type
    
  RETURN VALUE
    TRUE if item is new-style AND it is indexed, otherwise FALSE

  SIDE EFFECTS
    None.

===========================================================================*/
boolean mcfg_nv_is_new_indexed(
  uint32 item
  )
{
  return (mcfg_nv_get_max_index(item) != 0);
} /* mcfg_nv_is_indexed() */

/*==========================================================================

  FUNCTION mcfg_nv_read

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
                   mcfg_nv_item_e_type
    index     [in] index of the NV item to read, or 0 if no index
                   (if unsure, use 0)
    subs_id   [in] Subscription ID for the Multi-SIM NV item to read,
                or 0 if not a multi-SIM NV (if unsure, use 0)
    data_ptr [out] location to store NV item contents
    data_size [in] size of buffer at data_ptr
    
  RETURN VALUE
    MCFG_NV_STATUS_OK on success
    MCFG_NV_STATUS_BAD_ITEM if item is not in old NV or new MCFG NV range,
        or index is outside defined limit (new-style items only)
    MCFG_NV_STATUS_NOT_ACTIVE if item doesn't exist
    MCFG_NV_STATUS_BAD_SIZE if item size doesn't match data_size
    MCFG_NV_STATUS_EFS_ERR on EFS error
    MCFG_NV_STATUS_NULL_PTR if data_ptr is null

  SIDE EFFECTS
    Waits on fs_global_lock() - see description & dependencies

===========================================================================*/
mcfg_nv_status_e_type mcfg_nv_read(
  uint32    item,
  uint8     index,
  uint8     subs_id,
  void *    data_ptr,
  fs_size_t data_size
  )
{
  char path[MCFG_NV_MAX_EFS_PATH_LEN];
  int  efs_ret;

  boolean new_indexed;
  mcfg_nv_status_e_type status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (data_ptr == NULL)
  {
    status = MCFG_NV_NULL_PTR;
  }
  else
  {
    status = mcfg_nv_build_path(item, index, subs_id, &new_indexed, path, sizeof(path));

    MCFG_MSG_LOW_1("mcfg_nv_build_path status %d", status);
    if (status == MCFG_NV_STATUS_OK)
    {
      /* New-style indexed items are required to have a uint8 index field 
       * as the first byte of their struct.  This index is not stored in
       * EFS, so manually set it here, then adjust the pointer and size.
       * This is also true of using old-style indexed items through the
       * legacy NV layer, and has been kept for consistency. */
      if (new_indexed)
      {
        uint8 *data_index = (uint8 *) data_ptr;
        *data_index++ = index;
        data_ptr = (void *) data_index;
        data_size--;
      }

      efs_ret = efs_get(path, data_ptr, data_size);
      if (efs_ret == -1)
      {
        switch (efs_errno)
        {
          case ENOENT:
            status = MCFG_NV_STATUS_NOT_ACTIVE;
            break;

          case FS_ERANGE:
            status = MCFG_NV_STATUS_BAD_SIZE;
            break;

          default:
            status = MCFG_NV_STATUS_EFS_ERR;
        }
      }
      else if (efs_ret != data_size)
      {
        status = MCFG_NV_STATUS_BAD_SIZE;
      }
    }
  }

  MCFG_MSG_LOW_1("mcfg_nv_read status %d", status);
  return status;
} /* mcfg_nv_read() */

/*==========================================================================

  FUNCTION mcfg_nv_write

  DESCRIPTION
    This function writes an NV item to EFS.  Enforces a maximum data_size
    of MCFG_NV_ITEM_SIZE_MAX.  Applications wishing to save larger files 
    should use EFS regular file APIs directly.
    (Exceptions,  NV Items that do not apply - PRL)

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
                   mcfg_nv_item_e_type
    index     [in] index of the NV item to write, or 0 if no index
    subs_id   [in] Subscription ID for the Multi-SIM NV item to write,
              or 0 if not a multi-SIM NV (if unsure, use 0) 
    data_ptr  [in] location to store NV item contents
    data_size [in] number of bytes to write, 
                   must be <= MCFG_NV_ITEM_SIZE_MAX
    
  RETURN VALUE
    MCFG_NV_STATUS_OK on success
    MCFG_NV_STATUS_BAD_ITEM if item is not in old NV or new MCFG NV range,
        or index is outside defined limit (new-style items only)
    MCFG_NV_STATUS_BAD_SIZE if data_size > MCFG_NV_ITEM_SIZE_MAX
    MCFG_NV_STATUS_EFS_ERR on EFS error
    MCFG_NV_STATUS_NULL_PTR if data_ptr is null

  SIDE EFFECTS
    Waits on fs_global_lock() - see description & dependencies

===========================================================================*/
mcfg_nv_status_e_type mcfg_nv_write(
  uint32    item,
  uint8     index,                           
  uint8     subs_id,
  void *    data_ptr,
  fs_size_t data_size
  )
{
  struct fs_stat stat_buf;
  char path[MCFG_NV_MAX_EFS_PATH_LEN];
  int  efs_ret;

  boolean new_indexed;
  mcfg_nv_status_e_type status;
#ifdef MCFG_DEBUG
  uint8* data_verify_ptr;
#endif /* MCFG_DEBUG */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (data_ptr == NULL)
  {
    status = MCFG_NV_NULL_PTR;
  }
  else if (data_size > MCFG_NV_ITEM_SIZE_MAX)
  {
    status = MCFG_NV_STATUS_BAD_SIZE;
  }
  else
  {
    status = mcfg_nv_build_path(item, index, subs_id, &new_indexed, path, sizeof(path));

    if (status == MCFG_NV_STATUS_OK)
    {
      /* Delete the file if it exists and has a different size (efs_put does
       * not support O_TRUNC) */
      efs_ret = efs_stat(path, &stat_buf);
      if (efs_ret == 0 && stat_buf.st_size != data_size)
      {
        (void) efs_unlink(path);
      }
  
      /* New-style indexed items are required to have a uint8 index field 
       * as the first byte of their struct.  This index is not stored in
       * EFS, so adjust the pointer and size to skip it.  The legacy NV API
       * required this scheme to determine the index, but we have the index
       * as a discrete parameter, so it is not required but kept for
       * consistency. */
      if (new_indexed)
      {
        data_ptr = (void *) ((int) data_ptr + sizeof(uint8));
        data_size--;
      }

      efs_ret = efs_put(path, data_ptr, data_size, (O_CREAT | O_AUTODIR), 0777);
      if (efs_ret == -1)
      {
        status = MCFG_NV_STATUS_EFS_ERR;
        return status;
      }
      
#ifdef MCFG_DEBUG
      // Verify checks for debugging       
      data_verify_ptr = mcfg_malloc(data_size); 
      efs_ret = efs_get(path, data_verify_ptr, data_size);
      if(efs_ret == -1)
      {
        MCFG_MSG_HIGH_1("mcfg_nv_write for item %d FAIL!! ", item);
      }
      else
      {
        MCFG_MSG_HIGH_1("mcfg_nv_write for item %d is successful", item);
      }
      mcfg_free(data_verify_ptr);
#endif /* MCFG_DEBUG */

    } /* status == MCFG_NV_STATUS_OK */
  }

  return status;
} /* mcfg_nv_write() */

/*===========================================================================

FUNCTION mcfg_nv_update_symlink

DESCRIPTION
  Creates/updates the symlink for the Muxd NV Items.
  
DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    item      [in] MuxdNV item to update
    index     [in] index of the MuxdNV item 
    subs_id   [in] Subscription ID for the Multi-SIM NV item to write,
              or 0 if not a multi-SIM NV (if unsure, use 0)
    size      [in] size of the item 
                  

RETURN VALUE
  mcfg_nv_status_e_type

SIDE EFFECTS

===========================================================================*/
mcfg_nv_status_e_type mcfg_nv_update_symlink(
   uint32 item, 
   uint8 index, 
   uint8 subs_id,  
   uint32 size)  
{
  mcfg_nv_status_e_type status;
  char symlink_path[MCFG_NV_MAX_EFS_PATH_LEN];

  status = mcfg_nv_build_path(item, index, subs_id, NULL, symlink_path, sizeof(symlink_path));
  if (status == MCFG_NV_STATUS_OK)
  {
    status = mcfg_nv_efs_update_symlink(symlink_path, size);
  }
  MCFG_MSG_LOW_2("mcfg_nv_update_symlink eitem %d status %d", item, status);
  return status;
}

/*===========================================================================

FUNCTION mcfg_nv_efs_update_symlink

DESCRIPTION
  Creates a symlink for the Muxd NV Items or EFS Files
  
DEPENDENCIES
  None.

RETURN VALUE
  mcfg_nv_status_e_type

SIDE EFFECTS

===========================================================================*/
mcfg_nv_status_e_type mcfg_nv_efs_update_symlink(char* org_path, uint32 item_size)  
{
  mcfg_nv_status_e_type status = MCFG_NV_STATUS_OK;
  struct fs_stat temp_buf;
  uint8* temp_data_ptr;
  int return_size, efs_ret=0;

  /* Build the abs_path, from the org_path, note org_path=symlink path */
  uint32 path_len, path_offset;
  char abs_path[MCFG_NV_MAX_EFS_PATH_LEN];     /* Muxd or shadowed path*/
  path_len = sizeof(abs_path);
  if ( strlen(org_path) > (MCFG_NV_MAX_EFS_PATH_LEN-5) )
  {
    return MCFG_NV_STATUS_BAD_SIZE;
  }

  path_offset = strlcpy(abs_path, org_path, path_len);
  path_offset += mcfg_nv_add_muxing(&abs_path[path_offset], 
                                     path_len - path_offset);

  if( (efs_stat(abs_path, &temp_buf) == -1) && (efs_errno == ENOENT) )
  {
    /* Create file of size=item_size at abs path for muxd sw items */
    temp_data_ptr = mcfg_malloc(item_size);
    if( temp_data_ptr ) 
    {
      memset(temp_data_ptr,0,item_size);
      return_size = efs_put(abs_path, temp_data_ptr, item_size, 
                            O_CREAT|O_AUTODIR, 0777);
      MCFG_MSG_MED_3("mcfg_nv_efs_update_symlink- Creating dummy abs_path: %s of size=%d efs_ret=%d", 
                       abs_path, (int)item_size, return_size );
      mcfg_free(temp_data_ptr);
    }
    else
    {
      MCFG_MSG_ERROR_2("mcfg_nv_efs_update_symlink - Memory allocation failure, EFS error = %d, item_size=%lu", efs_errno, item_size);
    }
  }

  if( (efs_lstat(org_path, &temp_buf) == -1) && (efs_errno == ENOENT) )
  {
    /* File does not exist */
    MCFG_MSG_SPRINTF_1("mcfg_nv_efs_update_symlink- Link %s does not exist, creating symlink", org_path);
  }
  else
  {
    efs_unlink(org_path);
  }

  /* Find the type of file */
  MCFG_MSG_HIGH_2("mcfg_nv_efs_update_symlink- now create link %s to point to %s", org_path, abs_path);
  efs_ret = efs_symlink (abs_path, org_path);
             
  MCFG_MSG_LOW_1("mcfg_nv_efs_update_symlink efs_ret %d", efs_ret);
  if(efs_ret == -1)
  {
    status = MCFG_NV_STATUS_EFS_ERR;
  }
  else
  {
    status = MCFG_NV_STATUS_OK;
  }

  MCFG_MSG_LOW_1("mcfg_nv_efs_update_symlink status %d", status);
  return status;
}

/*==========================================================================

  FUNCTION mcfg_nv_efslogging_enabled

  DESCRIPTION
    Checks whether EFS logging is enalbed or not.

  DEPENDENCIES
    None.

  PARAMETERS
    None.
 
  RETURN VALUE
    TRUE if EFS logging is enabled, otherwise FALSE

  SIDE EFFECTS
    None.

===========================================================================*/
boolean mcfg_nv_efslogging_enabled()
{
  struct fs_stat stat;
  if((efs_lstat("/enable_mcfg_efslogging", &stat) == -1) && (efs_errno == ENOENT))
  {
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================

FUNCTION mcfg_nv_rm_symlink

DESCRIPTION
  Removes symlink for the Muxd NV Items or EFS Files
  
DEPENDENCIES
    Must NOT be called from a task locked context
    EFS must be initialized

  PARAMETERS
    item      [in] MuxdNV item to update
    index     [in] index of the MuxdNV item 
    subs_id   [in] Subscription ID for the Multi-SIM NV item to write,
              or 0 if not a multi-SIM NV (if unsure, use 0)

RETURN VALUE
  mcfg_nv_status_e_type

SIDE EFFECTS

===========================================================================*/
mcfg_nv_status_e_type mcfg_nv_rm_symlink(
   uint32 item, 
   uint8 index,
   uint8 subs_id) 
{
  mcfg_nv_status_e_type status;
  char symlink_path[MCFG_NV_MAX_EFS_PATH_LEN];
  struct fs_stat temp_buf;
  fs_mode_t fmode;

  status = mcfg_nv_build_path(item, index, subs_id, NULL, symlink_path, sizeof(symlink_path));
  if (status == MCFG_NV_STATUS_OK)
  {
    if( (efs_lstat(symlink_path, &temp_buf) == -1) && (efs_errno == ENOENT) )
    {
      /* File does not exist */
      MCFG_MSG_LOW_1("mcfg_nv_rm_symlink- file %s does not exist, skipping", symlink_path);
    }
    else
    {
      /* Find the type of file */
      fmode = temp_buf.st_mode;

      /* if (regular or item file) */
      if( S_ISLNK(fmode) )
      {
        /* remove the link */
        MCFG_MSG_MED_1("mcfg_nv_rm_symlink- remove link %s", symlink_path);
        if ( efs_unlink(symlink_path) == -1 )
        {
          MCFG_MSG_ERROR_1("mcfg_nv_rm_symlink - error in efs_unlink,EFS error = %d", efs_errno);
          status = MCFG_NV_STATUS_EFS_ERR;
        }
      }
      else
      {
        MCFG_MSG_LOW_1("mcfg_nv_rm_symlink- file %s is not a link, skipping", symlink_path);
      }

    }
  }

  return status;
}

/*===========================================================================

FUNCTION mcfg_nv_init

DESCRIPTION
  Generate mcfg.conf for QXDM and QPST to access and restore
  
DEPENDENCIES
  None.

RETURN VALUE
  booelan

SIDE EFFECTS

===========================================================================*/
boolean mcfg_nv_init(void) 
{
  struct fs_stat stat_buf;
  fs_ssize_t efs_ret;
  int fd;
  char *mcfg_items = "/nv/item_files/mcfg/mcfg_autoselect_by_uim\n\
/nv/item_files/mcfg/mcfg_autoselect_by_uim_Subscription01\n\
/nv/item_files/mcfg/mcfg_autoselect_by_uim_Subscription02\n\
/nv/item_files/mcfg/mcfg_setting\n\
/nv/item_files/mcfg/mcfg_setting_Subscription01\n\
/nv/item_files/mcfg/mcfg_setting_Subscription02\n\
/nv/item_files/mcfg/mcfg_config_force_load\n\
/nv/item_files/mcfg/mcfg_def_config_enable\n\
/nv/item_files/mcfg/mcfg_def_config_force_load\n\
/nv/item_files/mcfg/mcfg_segload_config\n\
";

  efs_ret = efs_stat(MCFG_NV_CONF_PATH, &stat_buf);
  
  if((efs_ret != 0) || (strlen(mcfg_items) != stat_buf.st_size))
  {
	  fd = efs_creat(MCFG_NV_CONF_PATH, ALLPERMS);
	  if (fd >= 0)
	  {
		  (void)efs_write(fd, mcfg_items, strlen(mcfg_items));
		  efs_close(fd); 
	  }
  }

  return TRUE;
} /* mcfg_nv_init() */


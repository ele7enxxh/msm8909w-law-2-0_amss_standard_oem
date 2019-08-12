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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_utils/src/mcfg_map.c#1 $ 
$DateTime: 2016/12/13 08:00:27 $ 
$Author: mplcsds1 $ 
$Change: 11985219 $ 

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
05/08/12 cy   Initial creation

==============================================================================*/

#include "comdef.h"

#include "fs_errno.h"
#include "fs_public.h"

#include "mcfg_osal.h"

#include "mcfg_common.h"
#include "mcfg_nv.h"
#include "mcfg_proc.h"
#include "mcfg_utils.h"
#include "mcfg_utils_i.h"

#include "mcfg_map_i.h"
#include "mcfg_efs_i.h"

/*==============================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/
/* most significant bit flag: active 1 / pending 0 */
#define MCFG_MAP_EXTRACT_FLAG(value)          ((value) >> 31 & 0x1) 

#define MCFG_MAP_ID_LEN_MASK                  0x0000FFFF 

#define MCFG_MAP_EXTRACT_SUB_ID(value)        ((value) >> 28 & 0x7)

#define MCFG_MAP_COMBINE_SUB_ID_LEN(sub, flag, len) \
        ((((sub) | ((flag) << 3)) << 28) | (len))

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/
  
static mcfg_map_list_s_type mcfg_map_record_map[MCFG_NUM_OF_TYPES];
  
/*==============================================================================

                     INTERNAL FUNCTION DECLARATIONS

==============================================================================*/
static int mcfg_map_config_map_comp_func
( 
  void* item_ptr, 
  void* compare_val
);

/*==============================================================================
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

  FUNCTION mcfg_map_is_config_id_unique

  DESCRIPTION
    This function returns whether the specified unique ID exists on the device.

  DEPENDENCIES
    None

  PARAMETERS
    type    [in]  type of config
    id      [in]  unique ID struct to fill with selected config

  RETURN VALUE
    TRUE if it's unique and doesn't exist, FALSE if it exists

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_is_config_id_unique
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id
)
{
  mcfg_map_record_map_node_list_type *map_list;
  mcfg_map_record_map_node_type      *map_node;
  boolean unique = TRUE;

  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);
  MCFG_ASSERT_COND_RET_FALSE(id->id_len > 0);

  map_list = &(mcfg_map_record_map[type].map);
  if (map_list)
  {
    map_node = (mcfg_map_record_map_node_type *)
      list_linear_search(map_list,
                         mcfg_map_config_map_comp_func, 
                         id);
    if (map_node)
    {
      MCFG_MSG_LOW("mcfg_map_is_config_id_unique:ID found");
      unique = FALSE;
    }
  }

  return unique;
} /* mcfg_map_is_config_id_unique() */

/*===========================================================================

  FUNCTION mcfg_map_list_configs

  DESCRIPTION
    This function returns a list of unique ID's of the specified type that are
    available.

  DEPENDENCIES
    None

  PARAMETERS
    type          [in]  type of config
    max_list_size [in]  size of the id_list array (number of elements)
    id_list[]     [out] list of unique ID struct to fill with selected config

  RETURN VALUE
    number of configs

  SIDE EFFECTS
    None

===========================================================================*/
uint32 mcfg_map_list_configs
(
  mcfg_config_type_e_type type,
  int                     max_list_size,
  mcfg_config_id_s_type  *id_list
)
{
  mcfg_map_record_map_node_list_type *map_list;
  mcfg_map_record_map_node_type      *map_node;
  uint32 total = 0;

  MCFG_CHECK_NULL_PTR_RET_ZERO(id_list);
  MCFG_ASSERT_COND_RET_ZERO(type < MCFG_NUM_OF_TYPES);

  map_list = &(mcfg_map_record_map[type].map);

  map_node = list_peek_front(map_list);
  while (map_node != NULL && total < max_list_size)
  {
    MCFG_MSG_LOW_2("mcfg_map_list_configs: add type %d index %d", 
                   type, total);

    memscpy(id_list+total, sizeof(mcfg_config_id_s_type),
            &(map_node->record.id), sizeof(mcfg_config_id_s_type));

    map_node = list_peek_next(map_list, &(map_node->link));
    total++;
  }

  if (total == 0)
  {
    MCFG_MSG_LOW("mcfg_map_list_configs: list is empty");
  }

  return total;
} /* mcfg_map_list_configs() */

static boolean mcfg_map_read_fd(int fd, uint32 offset, uint32 size, uint8* data)
{
  int efs_ret;
  fs_size_t total_bytes_read = 0;

  if (data != NULL)
  {
    efs_ret = efs_lseek(fd, offset, SEEK_SET);
    if (efs_ret == offset)
    {
      do
      {
        efs_ret = efs_read(fd, (data + total_bytes_read), (size - total_bytes_read));
      } while (efs_ret > 0 && (total_bytes_read += efs_ret) < size);
    }
  }
  return (total_bytes_read == size);
} /* mcfg_map_read_all() */

/*===========================================================================

  FUNCTION mcfg_map_get_active_config

  DESCRIPTION
    This function returns unique ID of the currently activated config file.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of config
    id        [out] unique ID struct to fill with active config
    sub       [in]  subscription index

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_get_active_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_sub_id_type_e_type sub
)
{
  uint16 nv;
  mcfg_nv_status_e_type status;
  uint32 nv_mcfg_version;

  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);
  MCFG_ASSERT_COND_RET_FALSE(sub < MCFG_NUM_OF_SUB_ID);

  if ((mcfg_map_record_map[type].active_id[sub].id_len == 0) || 
      (mcfg_map_record_map[type].active_id[sub].id_len > MCFG_CONFIG_ID_SIZE_MAX))
  {
    return FALSE;
  }
  
  memscpy(id, sizeof(mcfg_config_id_s_type),
         &mcfg_map_record_map[type].active_id[sub], sizeof(mcfg_config_id_s_type));

  /* check mcfg current version and make sure version info is not present */
  if (id->id_len == 0)
  {
    nv = (type == MCFG_TYPE_SW) ? MCFG_SW_VERSION_I : MCFG_HW_VERSION_I;
    status = mcfg_nv_read( nv, 0, sub, (void *)&nv_mcfg_version, sizeof(nv_mcfg_version));
    if (status != MCFG_NV_STATUS_NOT_ACTIVE)
    {
      status = mcfg_nv_delete(nv, 0, sub);
      MCFG_MSG_HIGH_3("get_active_config cleanup mcfg version type %d sub %d status %d", 
                       type, sub, status);
    }
  }

  return TRUE;
} /* mcfg_map_get_active_config() */

/*===========================================================================

  FUNCTION mcfg_map_set_active_config

  DESCRIPTION
    This function sets the currently active config file to the specified
    unique ID.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of config
    id        [in]  unique ID struct to fill with active config
    sub       [in]  subscription index

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_set_active_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_sub_id_type_e_type sub
)
{
  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);
  MCFG_ASSERT_COND_RET_FALSE(id->id_len > 0);
  MCFG_ASSERT_COND_RET_FALSE(sub < MCFG_NUM_OF_SUB_ID);

  if (mcfg_map_is_config_id_unique(type, id))
  {
    MCFG_MSG_ERROR("mcfg_map_set_active_config:ID doesn't exist");
    return FALSE;
  }

  /* save as active id in map list */
  memscpy(&(mcfg_map_record_map[type].active_id[sub]), 
          sizeof(mcfg_config_id_s_type),
          id,
          sizeof(mcfg_config_id_s_type));

  /* save as pending id in map list */
  memscpy(&(mcfg_map_record_map[type].pending_id[sub]), 
          sizeof(mcfg_config_id_s_type),
          id,
          sizeof(mcfg_config_id_s_type));

  /* store the map list into EFS */
  if (!mcfg_map_store_config_map(type))
  {
    MCFG_MSG_ERROR("mcfg_map_set_active_config: Failed to update map file");
    return FALSE;
  }

  return TRUE;
} /* mcfg_map_set_active_config() */

/*===========================================================================

  FUNCTION mcfg_map_rmv_active_config

  DESCRIPTION
    This function sets the currently active config file to the specified
    unique ID.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of config
    sub       [in]  subscription index

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_rmv_active_config
(
  mcfg_config_type_e_type type,
  mcfg_sub_id_type_e_type sub
)
{
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);
  MCFG_ASSERT_COND_RET_FALSE(sub < MCFG_NUM_OF_SUB_ID);

  /* Clear Pending config as well if it matches to active config */
  if (mcfg_utils_config_id_matches(&mcfg_map_record_map[type].pending_id[sub],
                                   &mcfg_map_record_map[type].active_id[sub]))
  {
    memset(&(mcfg_map_record_map[type].pending_id[sub]), 
           0,
           sizeof(mcfg_config_id_s_type));
  }

  /* Clear active id in map list */
  memset(&(mcfg_map_record_map[type].active_id[sub]), 
         0,
         sizeof(mcfg_config_id_s_type));

  /* store the map list into EFS */
  if (!mcfg_map_store_config_map(type))
  {
    MCFG_MSG_ERROR("mcfg_map_rmv_active_config: Failed to update map file");
    return FALSE;
  }

  MCFG_MSG_LOW("mcfg_map_rmv_active_config:removed active config from map file");

  return TRUE;
} /* mcfg_map_rmv_active_config() */

/*===========================================================================

  FUNCTION mcfg_map_get_selected_config

  DESCRIPTION
    This function returns unique ID of the currently selected config file.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of config
    id        [out] unique ID struct to fill with selected config
    sub       [in]  subscription index

  RETURN VALUE
    TRUE if it exists, FALSE if it doesn't

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_get_selected_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_sub_id_type_e_type sub
)
{
  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);
  MCFG_ASSERT_COND_RET_FALSE(sub < MCFG_NUM_OF_SUB_ID);

  if ((mcfg_map_record_map[type].pending_id[sub].id_len == 0) || 
      (mcfg_map_record_map[type].pending_id[sub].id_len > MCFG_CONFIG_ID_SIZE_MAX))
  {
    return FALSE;
  }
  
  memscpy(id, 
          sizeof(*id),
          &(mcfg_map_record_map[type].pending_id[sub]), 
          sizeof(mcfg_map_record_map[type].pending_id[sub]));

  return TRUE;
} /* mcfg_map_get_selected_config() */

/*===========================================================================

  FUNCTION mcfg_map_set_selected_config

  DESCRIPTION
    This function sets the currently selected config file to the specified
    unique ID.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of config
    id        [in]  unique ID struct to fill with selected config
    sub       [in]  subscription index

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_set_selected_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_sub_id_type_e_type sub
)
{
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);
  MCFG_ASSERT_COND_RET_FALSE(sub < MCFG_NUM_OF_SUB_ID);

  if (!id)
  {
    /* remove pending record from map only */
    memset(&(mcfg_map_record_map[type].pending_id[sub]), 0, sizeof(mcfg_config_id_s_type));
  }
  else
  {
    MCFG_ASSERT_COND_RET_FALSE(id->id_len > 0);
    if (mcfg_map_is_config_id_unique(type, id))
    {
      MCFG_MSG_ERROR("mcfg_map_set_selected_config:ID doesn't exist");
      return FALSE;
    }

    memscpy(&(mcfg_map_record_map[type].pending_id[sub]), 
            sizeof(mcfg_config_id_s_type),
            id,
            sizeof(mcfg_config_id_s_type));

  }

  if (!mcfg_map_store_config_map(type))
  {
    MCFG_MSG_ERROR("mcfg_map_set_active_config: Failed to update map file");
    return FALSE;
  }

  return TRUE;
} /* mcfg_map_set_selected_config() */


/*===========================================================================

  FUNCTION mcfg_map_get_config_limits

  DESCRIPTION
    This function is used to read information about memory limits for a given
    type of config file.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of config memory to query
    max_size  [out] maximum size of memory allowed for this type of config
    curr_size [out] current size of memory of all configs of this type

  RETURN VALUE
    TRUE if it exists, FALSE if it doesn't

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_get_config_limits
(
  mcfg_config_type_e_type type,
  uint64                 *max_size,
  uint64                 *curr_size
)
{
  mcfg_map_record_map_node_list_type *map_list;
  mcfg_map_record_map_node_type      *map_node;
  struct  fs_statvfs stat_vfs;
  int     size = 0;
  int     num = 0;

  MCFG_CHECK_NULL_PTR_RET_FALSE(curr_size);
  MCFG_CHECK_NULL_PTR_RET_FALSE(max_size);
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);

  *curr_size = 0;
  map_list = &(mcfg_map_record_map[type].map);
  num = list_size(map_list);
  MCFG_ASSERT_COND_RET_FALSE(num <= MCFG_CONFIG_LIST_SIZE_MAX);

  map_node = list_peek_front(map_list);
  while (map_node != NULL)
  {
    if(!mcfg_efs_get_config_size(type, &(map_node->record.id), &size))
    {
      MCFG_MSG_ERROR("mcfg_efs_get_config_limits: size error");
      return FALSE;
    }

    *curr_size += size;
    map_node = list_peek_next(map_list, &(map_node->link));
  }
  
  /* Set the current possible max size */
  *max_size = *curr_size + (MCFG_CONFIG_LIST_SIZE_MAX - num) * MCFG_CONFIG_SIZE_MAX;

  /*Check the current available free space on EFS */
  if (!mcfg_efs_statvfs(&stat_vfs))
  {
    MCFG_MSG_ERROR("mcfg_efs_statvfs failed");
    return FALSE;
  }

  size = *curr_size + stat_vfs.f_bavail * stat_vfs.f_bsize;
  if (*max_size > size)
  {
    *max_size = size;
  }

  if (*max_size > MCFG_CONFIG_SIZE_MAX)
  {
    *max_size = *max_size - MCFG_CONFIG_SIZE_MAX;
  }
  else
  {
    *max_size = 0;
  }

  return TRUE;
} /* mcfg_map_get_config_limits() */

/*===========================================================================

  FUNCTION mcfg_map_empty_map

  DESCRIPTION
    This function cleans up the map list

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  config type
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_empty_map
(
  mcfg_config_type_e_type type
)
{
  mcfg_map_record_map_node_list_type *map_list;
  mcfg_map_record_map_node_type      *map_node;

  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);
  map_list = &(mcfg_map_record_map[type].map);

  while (list_size(map_list) > 0)
  {
    map_node = (mcfg_map_record_map_node_type *)list_pop_front(map_list);

    mcfg_free(map_node);
  }

  /*Free the list head node here*/
  map_list->back_ptr = NULL;
  map_list->front_ptr = NULL;
  map_list->size = 0;

  memset(&mcfg_map_record_map[type], 0, sizeof(mcfg_map_list_s_type));
  return TRUE;
} /* mcfg_map_empty_map() */

/*===========================================================================

  FUNCTION mcfg_map_print_map

  DESCRIPTION
    This function prints out the map list

  DEPENDENCIES
    None

  PARAMETERS
    None
 
  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_map_print_map
(
  void
)
{
  mcfg_config_type_e_type type;
  mcfg_sub_id_type_e_type sub;
  mcfg_map_record_map_node_list_type *map_list;
  mcfg_map_record_map_node_type      *map_node;

  for(type = MCFG_TYPE_FIRST; type < MCFG_NUM_OF_TYPES; type++)
  {
    for(sub = MCFG_SUB_ID_FIRST; sub < MCFG_NUM_OF_SUB_ID; sub++)
    {
      MCFG_MSG_LOW_2("mcfg_map_print_map: type %d sub %d", type, sub);
      MCFG_MSG_LOW_2("mcfg_map_print_map: active len %d id %02X -", 
                      mcfg_map_record_map[type].active_id[sub].id_len,
                      mcfg_map_record_map[type].active_id[sub].id[0]);
      MCFG_MSG_LOW_2("mcfg_map_print_map: pendng len %d id %02X -", 
                      mcfg_map_record_map[type].pending_id[sub].id_len,
                      mcfg_map_record_map[type].pending_id[sub].id[0]);
    }
    map_list = &(mcfg_map_record_map[type].map);

    MCFG_MSG_LOW("mcfg_map_print_map: loaded config list");
    map_node = list_peek_front(map_list);
    while (map_node != NULL)
    {
      if(map_node->record.id.id_len)
      {
        MCFG_MSG_LOW_3("mcfg_map_print_map: id len %d id %02X - %02X", 
                       map_node->record.id.id_len,
                       map_node->record.id.id[0],
                       map_node->record.id.id[map_node->record.id.id_len-1]);
      }
      else
      {
        MCFG_MSG_LOW_1("mcfg_map_print_map: id len %d", 
                       map_node->record.id.id_len);
      }

      map_node = list_peek_next(map_list, &(map_node->link));
    }
  }
} /* mcfg_map_print_map() */

/*===========================================================================

  FUNCTION mcfg_map_store_config_id

  DESCRIPTION
    This function writes the id structure into efs file

  DEPENDENCIES
    None

  PARAMETERS
    fd      [in]  file descriptor
    id      [in]  pointer of ID structure
    storage [in]  storage mode
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
static boolean mcfg_map_store_config_id
(
  int                      fd,
  mcfg_config_id_s_type   *id,
  mcfg_storage_mode_e_type storage
)
{
  uint32 sig; 
  fs_ssize_t bytes_written, efs_ret;
  uint8 *pbuf;
   
  MCFG_ASSERT_COND_RET_FALSE(fd >= 0);
  MCFG_CHECK_NULL_PTR_RET_FALSE(id);

  sig = (storage == MCFG_STORAGE_LOCAL) ? MCFG_MAP_SIGNATURE : MCFG_MAP_SIGNATURE_REMOTE;

  /* store signature first */
  bytes_written = 0;
  pbuf = (uint8 *)&sig;
  do
  {
    efs_ret = efs_write(fd, pbuf + bytes_written, sizeof(sig) - bytes_written);
  } while (efs_ret > 0 && (bytes_written += efs_ret) < sizeof(sig));

  if(bytes_written < sizeof(sig))
  {  
    return FALSE;
  }

  /* store unique config ID */
  bytes_written = 0;
  pbuf = (uint8 *)id;
  do
  {
    efs_ret = efs_write(fd, pbuf + bytes_written, 
                         sizeof(mcfg_config_id_s_type) - bytes_written);
  } while (efs_ret > 0 && (bytes_written += efs_ret) < sizeof(mcfg_config_id_s_type));

  if(bytes_written < sizeof(mcfg_config_id_s_type))
  {  
    return FALSE;
  }
  
  return TRUE;
} /* mcfg_map_store_config_id() */

/*===========================================================================

  FUNCTION mcfg_map_read_factory_config_map

  DESCRIPTION
    This function reads the default config file and update map list

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to read
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_read_factory_config_map
(
  mcfg_config_type_e_type   type
)
{
  char filename[MCFG_MAP_RECORD_FILENAME_MAX];
  char path[MCFG_MAP_RECORD_FILEPATH_MAX];
  struct fs_stat stat;
  mcfg_config_id_s_type id;

  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);

  memset(&id, 0, sizeof(mcfg_config_id_s_type));
  memset(filename, 0, MCFG_MAP_RECORD_FILENAME_MAX);

  switch(type)
  {
    case MCFG_TYPE_HW:
      memscpy(filename, MCFG_MAP_RECORD_FILEPATH_MAX,
              MCFG_MAP_DEFAULT_FILE_HW, strlen(MCFG_MAP_DEFAULT_FILE_HW));
      id.id_len = strlen(MCFG_MAP_DEFAULT_ID_HW);
      memscpy(id.id, MCFG_CONFIG_ID_SIZE_MAX, 
              MCFG_MAP_DEFAULT_ID_HW, id.id_len);

      break;

    case MCFG_TYPE_SW:
      memscpy(filename, MCFG_MAP_RECORD_FILEPATH_MAX,
              MCFG_MAP_DEFAULT_FILE_SW, strlen(MCFG_MAP_DEFAULT_FILE_SW));
      id.id_len = strlen(MCFG_MAP_DEFAULT_ID_SW);
      memscpy(id.id, MCFG_CONFIG_ID_SIZE_MAX, 
              MCFG_MAP_DEFAULT_ID_SW, id.id_len);
      break;
    
    default:
      return FALSE;
  }

  /* link the factory config with hashed filename and update map list */
  if (snprintf(path, MCFG_MAP_RECORD_FILEPATH_MAX, "%s/cfg_%02d/%s", 
               MCFG_MAP_PATH_META, type, filename) <= 0)
  {
    MCFG_MSG_HIGH("mcfg_map_read_factory_config_map: path error");
    return FALSE;
  }

  if (efs_lstat(path, &stat) !=0)
  {
    MCFG_MSG_HIGH("mcfg_map_read_factory_config_map: factory config doesn't exist");
    return FALSE;
  }

  if (!mcfg_map_add_record_in_map(&id, &(mcfg_map_record_map[type].map), MCFG_STORAGE_LOCAL))
  {
    MCFG_MSG_ERROR("mcfg_map_read_factory_config_map: Failed to add id to map file");
    return FALSE;
  }

  memscpy(&(mcfg_map_record_map[type].pending_id[MCFG_SUB_ID_FIRST]), 
          sizeof(mcfg_config_id_s_type),
          &id, sizeof(mcfg_config_id_s_type));

  if (!mcfg_map_store_config_map(type))
  {
    MCFG_MSG_ERROR("mcfg_map_read_factory_config_map: Failed to update map file");
    return FALSE;
  }
  MCFG_MSG_HIGH_1("mcfg_map_read_factory_config_map: %s added to map", path);

  return TRUE;
} /* mcfg_map_read_factory_config_map() */

/*===========================================================================

  FUNCTION mcfg_map_read_config_map

  DESCRIPTION
    This function reads the ID and file name mapping for a specified config
    type into a link list

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of config
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_read_config_map
(
  mcfg_config_type_e_type   type
)
{
  struct sig_id_s
  {
      uint32 sig;
      mcfg_config_id_s_type id;
  }sig_id;
  char path[MCFG_MAP_RECORD_FILEPATH_MAX];
  int offset;
  int fd, efs_ret, index, i, sub;
  struct fs_stat stat;
  boolean terminated = FALSE;
  EFSDIR *dirp;

  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);

  offset = snprintf(path,MCFG_MAP_RECORD_FILEPATH_MAX, "%s/cfg_%02d/map", MCFG_MAP_PATH_META, type);
  MCFG_ASSERT_COND_RET_FALSE(offset > 0);

  fd = efs_open(path, O_RDONLY); 
  if (fd < 0)
  {
    MCFG_MSG_HIGH_1("mcfg_map_read_config_map: Unable to open MCFG Image file (%d)",efs_errno);

    offset = snprintf(path, MCFG_MAP_RECORD_FILEPATH_MAX, "%s/cfg_%02d", MCFG_MAP_PATH_META, type);
    MCFG_ASSERT_COND_RET_FALSE(offset > 0);

    dirp = efs_opendir(MCFG_MAP_PATH_META);
    if (!dirp)
    {
      MCFG_MSG_HIGH_1("mcfg_map_read_config_map: %s not exist", MCFG_MAP_PATH_META);
      efs_mkdir(MCFG_MAP_PATH_META, 0755);
      efs_mkdir(path, 0755);
      return FALSE;
    }
    efs_closedir(dirp);

    dirp = efs_opendir(path);
    if (!dirp)
    {
      MCFG_MSG_HIGH_1("mcfg_map_read_config_map: %s not exist", path);
      efs_mkdir(path, 0755);
      return FALSE;
    }
    else
    {
      efs_closedir(dirp);
      return mcfg_map_read_factory_config_map(type);
    }
  }
  
  efs_ret = efs_fstat(fd, &stat);

  if (efs_ret < 0)
  {
    efs_close(fd); 
    return FALSE;
  }

  /* Read until EOF or termninated */
  index = 0;
  while(mcfg_map_read_fd(fd, index*sizeof(sig_id), sizeof(sig_id), 
        (uint8 *)&sig_id) && !terminated)
  {
    MCFG_MSG_LOW_2("mcfg_map_read_config_map: index %u sig %8X", 
                     index, sig_id.sig);
    MCFG_MSG_LOW_2("mcfg_map_read_config_map: index %u id_len %u", 
                     index, sig_id.id.id_len);
    if (sig_id.sig != MCFG_MAP_SIGNATURE && sig_id.sig != MCFG_MAP_SIGNATURE_REMOTE)
    {
      MCFG_MSG_ERROR_1("mcfg_map_read_config_map: sig mismatch (%lx)", 
                       sig_id.sig);
      terminated = FALSE;
      break;
    }
    else if (index == 0)
    {
      MCFG_MSG_LOW_1("mcfg_map_read_config_map: set active id with len %d", 
                       sig_id.id.id_len);
      memscpy(&(mcfg_map_record_map[type].active_id[MCFG_SUB_ID_FIRST]), 
              sizeof(mcfg_config_id_s_type),
              &(sig_id.id), 
              sizeof(mcfg_config_id_s_type));
    }
    else if (index == 1)
    {
      MCFG_MSG_LOW_1("mcfg_map_read_config_map: set pending id with len %d", 
                       sig_id.id.id_len);
      memscpy(&(mcfg_map_record_map[type].pending_id[MCFG_SUB_ID_FIRST]), 
              sizeof(mcfg_config_id_s_type),
              &(sig_id.id), 
              sizeof(mcfg_config_id_s_type));
    }
    else if ((sig_id.id.id_len > 0) && (sig_id.id.id_len <= MCFG_CONFIG_ID_SIZE_MAX))
    {
      MCFG_MSG_LOW_1("mcfg_map_read_config_map: add id with len %d", 
                       sig_id.id.id_len);
      mcfg_map_add_record_in_map(
        &(sig_id.id), &(mcfg_map_record_map[type].map),
        (sig_id.sig == MCFG_MAP_SIGNATURE) ? MCFG_STORAGE_LOCAL : MCFG_STORAGE_REMOTE);
    }
    else if (sig_id.id.id_len > MCFG_CONFIG_ID_SIZE_MAX)
    {
      sub = MCFG_MAP_EXTRACT_SUB_ID(sig_id.id.id_len);
      if (sub > MCFG_SUB_ID_FIRST && sub < MCFG_MAX_RESERVED_SUB_ID)
      {
        if(MCFG_MAP_EXTRACT_FLAG(sig_id.id.id_len))
        {
          sig_id.id.id_len = sig_id.id.id_len & MCFG_MAP_ID_LEN_MASK;
          MCFG_MSG_LOW_2("mcfg_map_read_config_map: set active id with len %d sub %d", 
                           sig_id.id.id_len, sub);
          memscpy(&(mcfg_map_record_map[type].active_id[sub]), 
                  sizeof(mcfg_config_id_s_type),
                  &(sig_id.id), 
                  sizeof(mcfg_config_id_s_type));
        }
        else
        {
          sig_id.id.id_len = sig_id.id.id_len & MCFG_MAP_ID_LEN_MASK;
          MCFG_MSG_LOW_2("mcfg_map_read_config_map: set pending id with len %d sub %d", 
                           sig_id.id.id_len, sub);
          memscpy(&(mcfg_map_record_map[type].pending_id[sub]), 
                  sizeof(mcfg_config_id_s_type),
                  &(sig_id.id), 
                  sizeof(mcfg_config_id_s_type));
        }
      }
      else
      {
        terminated = FALSE;
        break;
      }
    }
    else
    {
      MCFG_MSG_LOW_1("mcfg_map_read_config_map: check termination with len %d", 
                       sig_id.id.id_len);
      terminated = TRUE;
      for (i=0; i<MCFG_CONFIG_ID_SIZE_MAX; i++)
      {
        if (sig_id.id.id[i] != 0)
        {
          terminated = FALSE;
          break;
        }
      }
      break;
    }

    index ++;
  };
  
  
  if (!terminated)
  {
    MCFG_MSG_ERROR_1("mcfg_map_read_config_map: error map file type %d", type);
    /* something wrong with the map file, empty map list */
    mcfg_map_empty_map(type);
  }

  efs_close(fd); 
  return TRUE;
} /* mcfg_map_read_config_map() */


/*===========================================================================

  FUNCTION mcfg_map_store_config_map

  DESCRIPTION
    This function stores config record map list into EFS
    for a specified config

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of config
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_store_config_map
(
  mcfg_config_type_e_type   type
)
{
  mcfg_map_record_map_node_type *map_node;
  mcfg_config_id_s_type id;
  char path[MCFG_MAP_RECORD_FILEPATH_MAX];
  int offset;
  int fd;
  mcfg_sub_id_type_e_type sub;
  
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);

  offset = snprintf(path,MCFG_MAP_RECORD_FILEPATH_MAX, "%s/cfg_%02d/map", MCFG_MAP_PATH_META, type);
  MCFG_ASSERT_COND_RET_FALSE(offset > 0);

  fd = efs_open(path, O_CREAT | O_WRONLY); 
  if (fd < 0)
  {
    MCFG_MSG_ERROR_1("Unable to create/open MCFG Image file (%d)",efs_errno);
    return FALSE;
  }

  /* save active ID */
  mcfg_map_store_config_id(
    fd, &(mcfg_map_record_map[type].active_id[MCFG_SUB_ID_FIRST]), MCFG_STORAGE_LOCAL);

  /* save pending-selected ID */
  mcfg_map_store_config_id(
    fd, &(mcfg_map_record_map[type].pending_id[MCFG_SUB_ID_FIRST]), MCFG_STORAGE_LOCAL);

  /* save list of ID one by one */
  map_node = list_peek_front(&(mcfg_map_record_map[type].map));
  while (map_node != NULL)
  {
    MCFG_MSG_LOW_1("mcfg_map_store_config_map: store type %d", type);

    mcfg_map_store_config_id(fd, &(map_node->record.id), map_node->record.storage);
    map_node = list_peek_next(&(mcfg_map_record_map[type].map), 
                              &(map_node->link));
  }

  if (MCFG_NUM_OF_SUB_ID > 1)
  {
    /* save active IDs: 2nd to last */
    for (sub = (MCFG_SUB_ID_FIRST+1); sub < MCFG_MAX_RESERVED_SUB_ID; sub ++)
    {
      memscpy(&id, sizeof(mcfg_config_id_s_type), 
              &mcfg_map_record_map[type].active_id[sub], sizeof(mcfg_config_id_s_type));
      id.id_len = MCFG_MAP_COMBINE_SUB_ID_LEN(sub, 1, id.id_len);
      mcfg_map_store_config_id(fd, &id, MCFG_STORAGE_LOCAL);
    }

    /* save pending-selected IDs: 2nd to last */
    for (sub = (MCFG_SUB_ID_FIRST+1); sub < MCFG_MAX_RESERVED_SUB_ID; sub ++)
    {
      memscpy(&id, sizeof(mcfg_config_id_s_type), 
              &mcfg_map_record_map[type].pending_id[sub], sizeof(mcfg_config_id_s_type));
      id.id_len = MCFG_MAP_COMBINE_SUB_ID_LEN(sub, 0, id.id_len);
      mcfg_map_store_config_id(fd, &id, MCFG_STORAGE_LOCAL);
    }
  }

  /* save an empty ID for termination */
  memset(&id, 0, sizeof(mcfg_config_id_s_type));
  mcfg_map_store_config_id(fd, &id, MCFG_STORAGE_LOCAL);

  efs_close(fd); 
  return TRUE;
} /* mcfg_map_store_config_map() */


/*===========================================================================

  FUNCTION mcfg_map_add_record

  DESCRIPTION
    This function reads the ID and file name mapping for a specified config
    type into a link list

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]    config ID to add
    record    [in]    config record to be added
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_add_record
(
  mcfg_config_type_e_type type,
  mcfg_map_record_s_type *record
)
{
/*-------------------------------------------------------------------------*/
  return mcfg_map_add_record_in_map(
    &(record->id), &(mcfg_map_record_map[type].map), record->storage);
} /* mcfg_map_add_record() */

/*===========================================================================

  FUNCTION mcfg_map_delete_record

  DESCRIPTION
    This function reads the ID and file name mapping for a specified config
    type into a link list

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  config ID to delete
    record    [in] config record to be deleted
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_delete_record
(
  mcfg_config_type_e_type type,
  mcfg_map_record_s_type *record
)
{
  mcfg_config_id_s_type pending_id;
  mcfg_sub_id_type_e_type sub;
/*-------------------------------------------------------------------------*/
  /* Pending config has to be cleared from map list */
  for (sub = MCFG_SUB_ID_FIRST; sub < MCFG_NUM_OF_SUB_ID; sub++)
  {
    if (mcfg_map_get_selected_config(type, &pending_id, sub))
    {
      if (mcfg_utils_config_id_matches(&record->id, &pending_id))
      {
        MCFG_MSG_HIGH_1("De-selected current pending config first before deleting for"\
                       " subscription %d", sub);

        memset(&mcfg_map_record_map[type].pending_id, 0, sizeof(mcfg_config_id_s_type));
      }
    }
  }

  return mcfg_map_del_record_in_map(&record->id, &(mcfg_map_record_map[type].map));
} /* mcfg_map_delete_record() */

/*===========================================================================

  FUNCTION mcfg_map_read_record

  DESCRIPTION
    This function reads the ID and file name mapping for a specified config
    type into a link list

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  config ID to add
    record    [inout]  config record to be read based on id
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_read_record
(
  mcfg_config_type_e_type type,
  mcfg_map_record_s_type *record
)
{
  mcfg_map_record_map_node_type *map_node;
/*-------------------------------------------------------------------------*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(record);

  map_node = (mcfg_map_record_map_node_type *)
    list_linear_search(&(mcfg_map_record_map[type].map),
                       mcfg_map_config_map_comp_func, 
                       &record->id);
  if (!map_node)
  {
    MCFG_MSG_ERROR("mcfg_map_del_record_in_map:unable to find id to delete"); 
    return FALSE;
  }

  record->storage = map_node->record.storage;
  record->size = map_node->record.size;

  return TRUE;
} /* mcfg_map_read_record() */


/*===========================================================================

  FUNCTION mcfg_map_add_record_in_map

  DESCRIPTION
    This function reads the ID and file name mapping for a specified config
    type into a link list

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  config ID to add
    map       [inout] config record map list to be updated
    storage   [in]  storage mode
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_add_record_in_map
(
  mcfg_config_id_s_type              *id,
  mcfg_map_record_map_node_list_type *map,
  mcfg_storage_mode_e_type            storage
)
{
  mcfg_map_record_map_node_type      *map_node;
/*-------------------------------------------------------------------------*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_CHECK_NULL_PTR_RET_FALSE(map);
  MCFG_ASSERT_COND_RET_FALSE(list_size(map) < MCFG_CONFIG_LIST_SIZE_MAX);
  MCFG_ASSERT_COND_RET_FALSE(id->id_len <= MCFG_CONFIG_ID_SIZE_MAX);

  /*create a Node Element*/
  map_node = (mcfg_map_record_map_node_type *)
    mcfg_malloc(sizeof(mcfg_map_record_map_node_type));

  MCFG_CHECK_NULL_PTR_RET_FALSE(map_node);

  memset(map_node, 0, sizeof(mcfg_map_record_map_node_type));
  memscpy(&(map_node->record.id), sizeof(mcfg_config_id_s_type), 
          id, sizeof(mcfg_config_id_s_type)); 
  map_node->record.storage = storage;

  /*If we reach here, a map node has been successfully created,
    Insert the node into the list Now*/
  list_push_back(map, &(map_node->link));

  MCFG_MSG_LOW("mcfg_map_add_record_in_map");

  return TRUE;
} /* mcfg_map_add_record_in_map() */

/*===========================================================================

  FUNCTION mcfg_map_del_record_in_map

  DESCRIPTION
    This function removes a specified id record from map list

  DEPENDENCIES
    None

  PARAMETERS
    id        [in]  config ID to del
    map       [inout] config record map list to be updated
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_del_record_in_map
(
  mcfg_config_id_s_type              *id,
  mcfg_map_record_map_node_list_type *map
)
{
  mcfg_map_record_map_node_type *map_node;
/*-------------------------------------------------------------------------*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_CHECK_NULL_PTR_RET_FALSE(map);
  MCFG_ASSERT_COND_RET_FALSE(id->id_len <= MCFG_CONFIG_ID_SIZE_MAX);

  map_node = (mcfg_map_record_map_node_type *)
    list_linear_search(map,
                       mcfg_map_config_map_comp_func, 
                       id);
  if (!map_node)
  {
    MCFG_MSG_ERROR("mcfg_map_del_record_in_map:unable to find id to delete"); 
    return FALSE;
  }
  list_pop_item(map, &(map_node->link));
  mcfg_free(map_node);
  return TRUE;
} /* mcfg_map_del_record_in_map() */

/*===========================================================================
  FUNCTION mcfg_map_config_map_comp_func

  DESCRIPTION
    Comparison function for record map list search

  PARAMETERS
    item_ptr    : pointer to the filter node
    compare_val : pointer to the rule handle

  RETURN VALUE
    1 if matches, 0 otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static int mcfg_map_config_map_comp_func
( 
  void* item_ptr, 
  void* compare_val
)
{
  mcfg_map_record_map_node_type *map_node;
  mcfg_config_id_s_type *id;
/*-------------------------------------------------------------------------*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(item_ptr);
  MCFG_CHECK_NULL_PTR_RET_FALSE(compare_val);

  id = (mcfg_config_id_s_type *)compare_val;
  MCFG_ASSERT_COND_RET_FALSE(id->id_len <= MCFG_CONFIG_ID_SIZE_MAX);

  map_node = (mcfg_map_record_map_node_type *) item_ptr;
  if (id->id_len != map_node->record.id.id_len)
  {
    return FALSE;
  }

  return (memcmp(map_node->record.id.id, id->id, id->id_len)==0);
} /* mcfg_map_config_map_comp_func() */

/*===========================================================================

  FUNCTION mcfg_map_init_record_by_id

  DESCRIPTION
    This function reads the ID and file name mapping for a specified config
    type into a link list

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  config ID to add
    map       [inout] config record map list to be updated
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_map_init_record_by_id
(
  mcfg_map_record_s_type *record,
  mcfg_config_id_s_type *id
)
{
/*-------------------------------------------------------------------------*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(record);
  MCFG_CHECK_NULL_PTR_RET_FALSE(id);

  memset(record, 0, sizeof(mcfg_map_record_s_type));
  memscpy(&record->id, sizeof(mcfg_config_id_s_type),
          id, sizeof(mcfg_config_id_s_type));
  return TRUE;
} /* mcfg_map_init_record() */


/*===========================================================================

  FUNCTION mcfg_map_init

  DESCRIPTION
    This function initializes the ram map and reads from saved map file

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_map_init
(
  void
)
{
  uint8 i;

  for(i=MCFG_TYPE_FIRST; i<MCFG_NUM_OF_TYPES; i++)
  {
    memset(&mcfg_map_record_map[i], 0, sizeof(mcfg_map_list_s_type));

    mcfg_map_read_config_map(i);
  }

  mcfg_map_print_map();

  return;
} /* mcfg_map_init() */

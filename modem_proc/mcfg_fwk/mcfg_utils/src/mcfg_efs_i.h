#ifndef MCFG_EFS_I_H
#define MCFG_EFS_I_H
/*==============================================================================

                M O D E M   C O N F I G   E F S
 
GENERAL DESCRIPTION
  Internal header for MCFG EFS Storage

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_utils/src/mcfg_efs_i.h#1 $ 
$DateTime: 2016/12/13 08:00:27 $ 
$Author: mplcsds1 $ 
$Change: 11985219 $ 

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
05/08/12 cy   Initial creation

==============================================================================*/
#include "list.h"

#include "mcfg_common.h"
#include "mcfg_utils.h"

/*==============================================================================

                PRIVATE DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/
/* Paths to store config files */
#define MCFG_EFS_PATH_META         "/nvm/mcfg"
#define MCFG_EFS_MAP_SIGNATURE     0x4D634667  /* Signature "McFg" */
#define MCFG_EFS_DEFAULT_FILE_HW   "mcfg_hw.mbn"
#define MCFG_EFS_DEFAULT_FILE_SW   "mcfg_sw.mbn"
#define MCFG_EFS_DEFAULT_ID_HW     MCFG_EFS_DEFAULT_FILE_HW
#define MCFG_EFS_DEFAULT_ID_SW     MCFG_EFS_DEFAULT_FILE_SW
#define MCFG_EFS_FILE_HW_PREFIX    "mcfg_hw"
#define MCFG_EFS_FILE_SW_PREFIX    "mcfg_sw"

/* Maximum filename length for config files */
#define MCFG_EFS_RECORD_FILENAME_MAX MCFG_UTILS_RECORD_FILENAME_MAX
#define MCFG_EFS_RECORD_FILEPATH_MAX (MCFG_EFS_RECORD_FILENAME_MAX + 20)

/*==============================================================================
  Typedefs
==============================================================================*/
/* Struct for config file record */
typedef struct 
{
  /* Unique ID for this config */
  mcfg_config_id_s_type id;
  
  /* Filename for this config */
  uint8 file[MCFG_EFS_RECORD_FILENAME_MAX];
} mcfg_efs_config_record_s_type;

typedef list_type mcfg_efs_record_map_node_list_type;

typedef struct
{
  list_link_type                      link;
  mcfg_efs_config_record_s_type       record;
}mcfg_efs_record_map_node_type;

typedef struct
{
  mcfg_config_id_s_type               active_id;
  mcfg_config_id_s_type               pending_id;
  mcfg_efs_record_map_node_list_type  map;
}mcfg_efs_record_map_type;

/*==============================================================================
  Variables
==============================================================================*/


/*============================================================================== 
 
                    PRIVATE FUNCTION DECLARATIONS FOR MODULE
 
==============================================================================*/

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/


/*==============================================================================
  Typedefs
==============================================================================*/


/*==============================================================================
  Variables
==============================================================================*/


/*============================================================================== 
 
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
 
==============================================================================*/

/*===========================================================================

  FUNCTION mcfg_efs_add_config

  DESCRIPTION
    This function is used to store a new config file on the device.  

  DEPENDENCIES
    None

  PARAMETERS
    type   [in]  type of the new config
    id     [in]  ID of the new config
    config [in/out] config structure including config size and buffer pointer

  RETURN VALUE
    mcfg_error

  SIDE EFFECTS
    Config file is stored into device

===========================================================================*/
mcfg_error_e_type mcfg_efs_add_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_config_s_type     *config
);

/*===========================================================================

  FUNCTION mcfg_efs_delete_config

  DESCRIPTION
    This function deletes the config file with the specified unique ID from the
    device.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of the config to delete
    id        [in]  unique ID struct to fill with selected config (if NULL then
                    delete all config files of the given type)
    storage   [in]  storage mode

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_efs_delete_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_storage_mode_e_type storage
);

/*===========================================================================

  FUNCTION mcfg_efs_get_config

  DESCRIPTION
    This function is used to load a new config file on the device into
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
boolean mcfg_efs_get_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_config_s_type     *config
);

/*===========================================================================

  FUNCTION mcfg_efs_get_config_info

  DESCRIPTION
    This function is used to read extended information for a given config file
    on the device.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    id        [in]  ID of the config to query
    info      [out] info for the specified config

  RETURN VALUE
    TRUE if it exists, FALSE if it doesn't

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_efs_get_config_info
(
  mcfg_config_type_e_type  type,
  mcfg_config_id_s_type   *id,
  mcfg_config_info_s_type *info
);

/*===========================================================================

  FUNCTION mcfg_efs_get_config_size

  DESCRIPTION
    This function is used to read config size for a given
    type and ID of config file.

  DEPENDENCIES
    None

  PARAMETERS
    type [in]  type of config memory to query
    id   [in]  ID of the config to query
    size [out] size of config

  RETURN VALUE
    TRUE if it exists, FALSE if it doesn't

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_efs_get_config_size
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  int                    *size
);

/*===========================================================================

  FUNCTION mcfg_efs_get_config_path

  DESCRIPTION
    This function translates the config type and ID into the efs path which
    stores the config file

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    id        [in]  unique ID struct
    path      [out] file path
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_efs_get_config_path
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  char                   *path
);

/*===========================================================================

  FUNCTION mcfg_efs_get_config_filename

  DESCRIPTION
    This function translates the config ID into the efs file name which
    stores the config file

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID 
    id        [in]  unique ID struct
    filename  [out] filename
    size      [in]  max size of filename buf
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_efs_get_config_filename
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  char                   *filename,
  size_t                  size
);


/*===========================================================================

  FUNCTION mcfg_efs_get_saved_config_info

  DESCRIPTION
    This function is used to read extended information saved in local efs 
    as *.ci for remote storage config file.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    id        [in]  ID of the config to query
    info      [out] info for the specified config

  RETURN VALUE
    TRUE if it exists, FALSE if it doesn't

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_efs_get_saved_config_info
(
  mcfg_config_type_e_type  type,
  mcfg_config_id_s_type   *id,
  mcfg_config_info_s_type *info
);

/*===========================================================================

  FUNCTION mcfg_efs_store_buf_in_path

  DESCRIPTION
    This function stores data buf into EFS with specified path

  DEPENDENCIES
    None

  PARAMETERS
    buf       [in]  data buffer
    len       [in]  length of data buffer
    path      [in]  config path
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_efs_store_buf_in_path
(
  void *buf,
  int   len,
  char *path
);

/*===========================================================================

  FUNCTION mcfg_efs_delete_config_in_path

  DESCRIPTION
    This function delete config on EFS with specified path

  DEPENDENCIES
    None

  PARAMETERS
    path      [in]  config path
 
  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_efs_delete_config_in_path
(
  char  *path
);

boolean mcfg_efs_statvfs
(
  void *stat_vfs
);
#endif /* MCFG_EFS_I_H */
